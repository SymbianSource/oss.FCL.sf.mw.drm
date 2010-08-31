/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  OMA DRM rights parser
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <e32math.h>
#include <apmstd.h>
#include <utf.h>
#include <stringpool.h>
#include <xml/parserfeature.h>
#include "DRMRights.h"
#include "DcfCommon.h"
#include "DrmRightsParser.h"
#include "DRMRightsClient.h"
#include "hash.h"
#include "Base64.h"
#include "DrmPermission.h"
#include "DrmConstraint.h"
#include "DRMClockClient.h"

// MODULE DATA STRUCTURES
enum TElementEnum
    {
    ERights = 0,
    EContext,
    EVersion,
    EUid,
    EAgreement,
    EAsset,
    EInherit,
    EDigest,
    EDigestMethod,
    EDigestValue,
    EKeyInfo,
    EEncryptedKey,
    EEncryptionMethod,
    ECipherData,
    ECipherValue,
    ERetreivalMethod,
    EPermission,
    EPlay,
    EDisplay,
    EPrint,
    EExecute,
    EConstraint,
    ECount,
    ETimedCount,
    EDateTime,
    EStart,
    EEnd,
    EInterval,
    EAccumulated,
    EIndividual,
    EExport,
    ESystem,
    EKeyValue,
    EId,
    EIdRef,
    EProperty,
    ESoftware,
    EContainer,
#ifdef RD_DRM_METERING
    ERequirement,
    ETracked,
#endif
    ELast,
    };

struct TElements
    {
    const TText8* iString;
    TInt iNumber;
    };

NONSHARABLE_CLASS( CDrmRightsParser::CParsedAsset ): public CBase
    {
public:
    static CParsedAsset* NewL();
    ~CParsedAsset();
    void ConstructL();

private:
    CParsedAsset();

public:
    HBufC8* iUid;
    TBuf8< KDCFKeySize > iKey;
    TBuf8< KProtectedDCFKeySize > iProtectedKey;
    TBuf8< KDCFKeySize > iAuthenticationSeed;
    TBuf8< KProtectedDCFKeySize > iProtectedAuthSeed;
    TBuf8< SHA1_HASH > iDigest;

    HBufC8* iId;
    HBufC8* iIdRef;

    HBufC8* iInherit;
    };

// Note the class CDrmRightsParser::CParsedPermission maps to
// OMA DRM2 permission container element <permission>
// CDRMConstraint reperesents indivdual permissions as constraint container
// (except Toplevel constraint really represents
//  constraint container <constraint>)
NONSHARABLE_CLASS( CDrmRightsParser::CParsedPermission ): public CBase
    {
public:
    static CParsedPermission* NewL();
    ~CParsedPermission();
    // Removes invalidated permissions, and marks the total permission invalid,
    // if all constraint containers are invalid, or toplevel container
    // is invalid
    void RemoveInvalidL();
    // Sets current constraint and updates available rights ( iAvailableRights )
    void SetCurrentConstraint ( TRightsType aCurrentConstraint );

    RPointerArray< CParsedAsset > iAssets;
    TUint8 iAvailableRights;

    CDRMConstraint *iTopLevel;
    CDRMConstraint *iPlay;
    CDRMConstraint *iDisplay;
    CDRMConstraint *iPrint;
    CDRMConstraint *iExecute;
    CDRMConstraint *iExport;

    //2.1 addition:
    // URL to send HTTP GET on expiration of permission
    HBufC8* iOnExpiredUrl;

    // if constraint has unknown tags, this is ored with iCurrentConstraint
    TUint8 iInvalidConstraints;

    // maintained during parsing. possible values: TRightsType enumerations
    TUint8 iCurrentConstraint;

    // if top level constraint is invalid or all other constraints ate invalid
    // permission will become invalid
    TBool iInvalid;
    // count of not_owned_assets, (updated in transform...)
    TInt iNotOwned;

private:
    CParsedPermission();
    void ConstructL();
    void ResetConstraintL( CDRMConstraint*& aConstraint );
    };

NONSHARABLE_CLASS( CDrmRightsParser::CParsedRightsObject ): public CBase
    {
public:
    CParsedRightsObject();
    ~CParsedRightsObject();

    // Removes invalidated permission containers
    // (of type CParsedPermission ), and marks the whole rights object
    // invalid, if all permission containers are invalid
    void RemoveInvalid();

    HBufC8* iRightsObjectId;
    RPointerArray< CParsedAsset > iAssets;
    RPointerArray< CParsedPermission > iPermissions;

    CParsedAsset* iCurrentAsset;
    CParsedPermission* iCurrentPermission;
    CDRMConstraint* iCurrentConstraint;
    TBool iKeyIsCek;
    TBool iInvalid;
    };

// CONSTANTS
#define ELEMENT_COUNT( x ) static_cast< TInt >( ( sizeof( x ) / sizeof( x[ 0 ] ) ) )

// LOCAL CONSTANTS AND MACROS
const TInt KParserChunkSize = 512;
const TInt KMaxElementNesting = 24;

// 15 minutes per time zone, 60 seconds per minute
const TInt KSecondsPerTimeZone = 900;
const TInt KMinuteInMicroseconds = 60000000;
const TInt KTimeZoneIncrement = 15;

_LIT8( KXmlParserMimeType, "text/xml" );
_LIT8( KWbxmlParserMimeType, "text/wbxml" );
_LIT8( KSchemaAttribute, "schema" );
_LIT8( KSchemeAttribute, "scheme" );
_LIT8( KSchemaSymbianSid, "symbiansid" );
_LIT8( KSchemaSymbianVid, "symbianvid" );
_LIT8( KTimerAttribute, "timer" );
_LIT8( KIdAttribute, "id" );
_LIT8( KIdAttrUpperCase, "Id" );
_LIT8( KIdRefAttribute, "idref" );
_LIT8( KAuthSeed, "authSeed" );
// OMA DRM 2.1 additions
_LIT8( KOnExpiredUrlAttribute, "onExpiredURL" ); //requested on premission expiry if present
#ifdef RD_DRM_METERING
_LIT8( KTimedAttribute, "timed" );
_LIT8( KContentAccessGrantedAttribute, "contentAccessGranted" );
_LIT8( KContentAccessGrantedValueTrue, "true" );
#endif // RD_DRM_METERING

static const TElements KElements[] =
    {
    { _S8( "rights" ),             ::ERights },
    { _S8( "context" ),            EContext },
    { _S8( "version" ),            EVersion },
    { _S8( "uid" ),                EUid },
    { _S8( "agreement" ),          EAgreement },
    { _S8( "asset" ),              EAsset },
    { _S8( "inherit" ),            EInherit },
    { _S8( "digest" ),             EDigest },
    { _S8( "DigestMethod" ),       EDigestMethod },
    { _S8( "DigestValue" ),        EDigestValue },
    { _S8( "KeyInfo" ),            EKeyInfo },
    { _S8( "EncryptedKey" ),       EEncryptedKey },
    { _S8( "EncryptionMethod" ),   EEncryptionMethod },
    { _S8( "CipherData" ),         ECipherData },
    { _S8( "CipherValue" ),        ECipherValue },
    { _S8( "RetrievalMethod" ),    ERetreivalMethod },
    { _S8( "permission" ),         EPermission },
    { _S8( "play" ),               ::EPlay },
    { _S8( "display" ),            ::EDisplay },
    { _S8( "execute" ),            ::EExecute },
    { _S8( "print" ),              ::EPrint },
    { _S8( "constraint" ),         EConstraint },
    { _S8( "count" ),              ECount },
    { _S8( "timed-count" ),        ETimedCount },
    { _S8( "datetime" ),           EDateTime },
    { _S8( "start" ),              EStart },
    { _S8( "end" ),                EEnd },
    { _S8( "interval" ),           EInterval },
    { _S8( "accumulated" ),        EAccumulated },
    { _S8( "individual" ),         EIndividual },
    { _S8( "export" ),             EExport },
    { _S8( "system" ),             ESystem },
    { _S8( "KeyValue" ),           EKeyValue },
    { _S8( "software" ),           ESoftware },
    { _S8( "property" ),           EProperty },
    { _S8( "container" ),          EContainer },
#ifdef RD_DRM_METERING
    { _S8( "requirement" ),        ERequirement },
    { _S8( "tracked" ),            ETracked },
#endif //RD_DRM_METERING
    };

enum TParserStackState
    {
    EUnknownState = -1,
    ERoUidState = 0,
    ETopLevelConstraintState,
    EDisplayConstraintState,
    EPlayConstraintState,
    EPrintConstraintState,
    EExecuteConstraintState,
    EDateTimeStartState,
    EDateTimeEndState,
    EIntervalState,
    ECountState,
    ETimedCountState,
    EIndividualState,
    EAgreementAssetState,
    EPermissionAssetState,
    EAssetUidState,
    EAssetDigestState,
    EAssetKeyState,
    EEncryptedKeyState,
    EAssetInheritUidState,
    EPermissionState,
    ESoftwareState,
    EAccumulatedState,
    EKeyInfoState,
    ESystemState,
    EExportConstraintState,
#ifdef RD_DRM_METERING
    ETrackRequirementState,
#endif // RD_DRM_METERING
    };

struct TStackState
    {
    TParserStackState iState;
    TElementEnum iStack[ KMaxElementNesting ];
    };

// Keep these sorted by the number of elements in the stack
static const TStackState KParserStackStates[] =
    {
    // 5 elements
    { EEncryptedKeyState, { ECipherValue, ECipherData, EEncryptedKey,
                          EKeyInfo, EAsset, ELast } },
    // 4 elements
    { EAssetInheritUidState, { EUid, EContext, EInherit, EAsset, ELast } },
    { EPermissionAssetState, { EAsset, EPermission, EAgreement, ::ERights,
                             ELast } },
    { ESoftwareState, { EProperty, EContext, ESoftware, EContainer, ELast } },
    // 3 elements
    { EAgreementAssetState, { EAsset, EAgreement, ::ERights, ELast } },
    { EIndividualState, { EUid, EContext, EIndividual, ELast } },
    { ESystemState, { EUid, EContext, ESystem, ELast } },
    { ERoUidState, { EUid, EContext, ::ERights, ELast } },
    { EAssetUidState, { EUid, EContext, EAsset, ELast } },
    { EAssetDigestState, { EDigestValue, EDigest, EAsset, ELast } },
    { EAssetKeyState, { EKeyValue, EKeyInfo, EAsset, ELast } },
    // 2 elements
    { EKeyInfoState, { EKeyInfo, EAsset, ELast } },
    { ETopLevelConstraintState, { EConstraint, EPermission, ELast } },
    { EDisplayConstraintState, { ::EDisplay, EPermission, ELast } },
    { EPlayConstraintState, { ::EPlay, EPermission, ELast } },
    { EPrintConstraintState, { ::EPrint, EPermission, ELast } },
    { EExecuteConstraintState, { ::EExecute, EPermission, ELast } },
    { EExportConstraintState, { EExport, EPermission, ELast } },
#ifdef RD_DRM_METERING
    { ETrackRequirementState, { ETracked, ERequirement, ELast } },
#endif // RD_DRM_METERING
    //1 element
    { EDateTimeStartState, { EStart, ELast } },
    { EDateTimeEndState, { EEnd, ELast } },
    { EIntervalState, { EInterval, ELast } },
    { ECountState, { ECount, ELast } },
    { ETimedCountState, { ETimedCount, ELast } },
    { EAccumulatedState, { EAccumulated, ELast } },
    { EPermissionState, { EPermission, ELast } },
    };

// LOCAL FUNCTION PROTOTYPES

// ============================= LOCAL FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CDrmRightsParser::ValidateDay
// Returns ETrue if the day is valid
//         EFalse if the day is not valid
//
// If the month is January, March, May, July, August, October, December
// - Check that the day is between 1 and 31
// If the month is April, June, September, November
// - Check that the day is between 1 and 30
// If the month is February
// - Check that the day is between 1 and 29,
// - if it is 29 check that the following conditions are met in that order:
//   * The year is dividable by 400 it's valid
//   * The year is dividable by 100 it is not valid
//   * The year is dividable by 4 it is valid
// -----------------------------------------------------------------------------
//

TBool CDrmRightsParser::ValidateDay( TInt aYear, TMonth aMonth, TInt aDay )
    {
    TBool retVal = ETrue;

    switch ( aMonth )
        {
        case EJanuary:
        case EMarch:
        case EMay:
        case EJuly:
        case EAugust:
        case EOctober:
        case EDecember:
            {
            if ( aDay < 0 || aDay > 30 )
                {
                retVal = EFalse;
                }
            }
            break;
        case EApril:
        case EJune:
        case ESeptember:
        case ENovember:
            {
            if ( aDay < 0 || aDay > 29 )
                {
                retVal = EFalse;
                }
            }
            break;
        case EFebruary:
            {
            if ( aDay < 0 || aDay > 28 )
                {
                retVal = EFalse;
                }
            else
                {
                // If we are at day 29, it is possible that it is valid:
                if ( aDay == 28 )
                    {
                    // if the year is dividable by 400 it is valid
                    if ( ( aYear % 400 ) )
                        {
                        // if the year is dividable by 100 it is not valid
                        if ( aYear % 100 )
                            {
                            // if the year is dividable by 4 it is valid
                            if ( aYear % 4 )
                                {
                                retVal = EFalse;
                                }
                            }
                        else
                            {
                            retVal = EFalse;
                            }
                        }
                    }
                }
            }
            break;
        default:
            retVal = EFalse;
            break;
        }
    return retVal;
    }



// -----------------------------------------------------------------------------
// CDrmRightsParser::ValidTimeValues
// Returns ETrue if the time values are valid
//         EFalse if any of them are valid
// Checks done as per symbian documentations
// -----------------------------------------------------------------------------
//
TBool CDrmRightsParser::ValidTimeValues( TInt aYear, TMonth aMonth,
        TInt aDay, TInt aHour,
        TInt aMinute, TInt aSecond,
        TInt aMicrosecond )
    {
    // No check for Year

    // check Month
    if ( aMonth < EJanuary || aMonth > EDecember )
        {
        return EFalse;
        }

    // check Day
    if ( !ValidateDay( aYear, aMonth, aDay ) )
        {
        return EFalse;
        }

    // check Hour
    if ( aHour < 0 || aHour > 23 )
        {
        return EFalse;
        }

    // check Minute
    if ( aMinute < 0 || aMinute > 59 )
        {
        return EFalse;
        }

    // check Second
    if ( aSecond < 0 || aSecond > 59 )
        {
        return EFalse;
        }

    // check Microsecond
    if ( aMicrosecond < 0 || aMicrosecond > 9999999 )
        {
        return EFalse;
        }

    return ETrue;
    };


// -----------------------------------------------------------------------------
// CDrmRightsParser::ParseRelTime
// Parses a ISO8601 relative time string
// -----------------------------------------------------------------------------
//
TTime CDrmRightsParser::ParseRelTimeL( TDesC8& aRelTimeString )
    {
    TLex8 lex;
    TInt year( 0 );
    TInt month( 0 );
    TInt day( 0 );
    TInt hour( 0 );
    TInt minute( 0 );
    TInt second( 0 );
    TTime r;
    TTimeIntervalSeconds offset( 0 );

    lex = aRelTimeString;
    lex.Val( year );
    lex.Inc();
    lex.Val( month );
    lex.Inc();
    lex.Val( day );
    lex.Inc();
    lex.Val( hour );
    lex.Inc();
    lex.Val( minute );
    lex.Inc();
    lex.Val( second );


    // The time needs to be validated before
    if ( !ValidTimeValues( year, static_cast< TMonth >( month - 1 ), day - 1,
                hour, minute, second, 0 ) )
        {
        User::Leave( KErrArgument );
        }

    r = TTime( TDateTime( year, static_cast< TMonth >( month - 1 ), day - 1,
                hour, minute, second, 0 ) );
    if ( lex.Get() != 'Z' )
        {
        offset = iTimeZone * KSecondsPerTimeZone;
        r -= offset;
        }
    return r;
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::ParseRelInterval
// Parses a ISO8601 relative interval string
// -----------------------------------------------------------------------------
//
TTimeIntervalSeconds CDrmRightsParser::ParseRelInterval(
        TDesC8& aRelTimeString )
    {
    TLex8 lex;
    TInt year( 0 );
    TInt month( 0 );
    TInt day( 0 );
    TInt hour( 0 );
    TInt minute( 0 );
    TInt second( 0 );
    TInt n( 0 );
    TTimeIntervalSeconds r( 0 );
    TBool done( EFalse );
    TBool inDate( ETrue );

    lex = aRelTimeString;
    lex.Inc();
    while ( !done )
        {
        if ( lex.Peek() == 'T' )
            {
            lex.Inc();
            inDate = EFalse;
            }

        lex.Val( n );
        switch ( lex.Get() )
            {
            case 'Y':
                year = n;
                break;
            case 'D':
                day = n;
                break;
            case 'M':
                if ( inDate )
                    {
                    month = n;
                    }
                else
                    {
                    minute = n;
                    }
                break;
            case 'H':
                hour = n;
                break;
            case 'S':
                second = n;
                break;
            default:
                done = ETrue;
                break;
            }
        }

    r = ( ( ( ( year * 365 + month * 30 + day ) * 24 + hour )
                * 60 + minute ) * 60 + second );
    return r;
    }

// -----------------------------------------------------------------------------
// DecodeAndDeleteUndecodedL()
// Decodes base64 encoded HbufC8 buffer arcument, deletes undecoded buffer, and
// substitutes argument with Decoded buffer
// -----------------------------------------------------------------------------
//
LOCAL_C void DecodeAndDeleteUndecodedL( HBufC8*& aDecodee )
    {
    HBufC8* b( aDecodee );
    aDecodee = NULL;
    CleanupStack::PushL( b );
    aDecodee = Base64DecodeL( *b );
    CleanupStack::PopAndDestroy( b );
    }

// -----------------------------------------------------------------------------
// CopyOrLeaveL( TDes8& aDest, const TDesC8& aSrc )
// Copies aSrc to aDest.
// Leaves, if length of aSrc exceeds capacity of aDest
// -----------------------------------------------------------------------------
//
LOCAL_C void CopyOrLeaveL( TDes8& aDest, const TDesC8& aSrc )
    {
    if ( aSrc.Length() > aDest.MaxLength() )
        {
        User::Leave( KErrArgument );
        }
    aDest.Copy( aSrc );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDrmRightsParser::CParsedAsset::NewL
// -----------------------------------------------------------------------------
//
CDrmRightsParser::CParsedAsset*
CDrmRightsParser::CParsedAsset::NewL()
    {
    CParsedAsset* self = new ( ELeave ) CParsedAsset;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::CParsedAsset::~CParsedAsset
// -----------------------------------------------------------------------------
//
CDrmRightsParser::CParsedAsset::~CParsedAsset()
    {
    delete iUid;
    delete iId;
    delete iIdRef;
    delete iInherit;
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::CParsedAsset::ConstructL
// Allocate the list of attribute values
// -----------------------------------------------------------------------------
//
void CDrmRightsParser::CParsedAsset::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::CParsedAsset::CParsedAsset
// -----------------------------------------------------------------------------
//
CDrmRightsParser::CParsedAsset::CParsedAsset():
    iUid( NULL ),
    iId( NULL ),
    iIdRef( NULL ),
    iInherit( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::CParsedPermission::NewL
// -----------------------------------------------------------------------------
//
CDrmRightsParser::CParsedPermission*
CDrmRightsParser::CParsedPermission::NewL()
    {
    CParsedPermission* self = new ( ELeave ) CParsedPermission;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::CParsedPermission::~CParsedPermission
// -----------------------------------------------------------------------------
//
CDrmRightsParser::CParsedPermission::~CParsedPermission()
    {
    for ( TInt i( 0 ); i < iNotOwned; ++i )
        {
        iAssets.Remove( 0 );
        }
    iAssets.ResetAndDestroy();
    iAssets.Close();
    delete iTopLevel;
    delete iPlay;
    delete iDisplay;
    delete iPrint;
    delete iExecute;
    delete iExport;
    delete iOnExpiredUrl;
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::CParsedPermission::SetCurrentConstraint
// -----------------------------------------------------------------------------
//
void CDrmRightsParser::CParsedPermission::SetCurrentConstraint(
        TRightsType aCurrentConstraint )
    {
    iAvailableRights |= aCurrentConstraint;
    iCurrentConstraint = aCurrentConstraint;
    }
// -----------------------------------------------------------------------------
// CDrmRightsParser::CParsedPermission::RemoveInvalid
// -----------------------------------------------------------------------------
//
void CDrmRightsParser::CParsedPermission::RemoveInvalidL()
    {
    TUint8 valid = ( iInvalidConstraints ^ ERightsAll ) & ERightsAll;
    iAvailableRights &= valid;
    if ( !valid )
        {
        iInvalid = ETrue;
        }
    if ( iInvalid || ( iInvalidConstraints & ERightsTopLevel ) )
        {
        iInvalid = ETrue;
        ResetConstraintL( iTopLevel );
        }
    if ( iInvalid || ( iInvalidConstraints & ERightsPlay ) )
        {
        ResetConstraintL( iPlay );
        }
    if ( iInvalid || ( iInvalidConstraints & ERightsDisplay ) )
        {
        ResetConstraintL( iDisplay );
        }
    if ( iInvalid || ( iInvalidConstraints & ERightsExecute ) )
        {
        ResetConstraintL( iExecute );
        }
    if ( iInvalid || ( iInvalidConstraints & ERightsPrint ) )
        {
        ResetConstraintL( iPrint );
        }
    if ( iInvalid || ( iInvalidConstraints & ERightsExport ) )
        {
        ResetConstraintL( iExport );
        }
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::CParsedPermission::CParsedPermission
// -----------------------------------------------------------------------------
//
CDrmRightsParser::CParsedPermission::CParsedPermission()
    {
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::CParsedPermission::ConstructL
// -----------------------------------------------------------------------------
//
void CDrmRightsParser::CParsedPermission::ConstructL()
    {
    iTopLevel = CDRMConstraint::NewL();
    iPlay = CDRMConstraint::NewL();
    iDisplay = CDRMConstraint::NewL();
    iPrint = CDRMConstraint::NewL();
    iExecute = CDRMConstraint::NewL();
    iExport = CDRMConstraint::NewL();
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::CParsedPermission::ResetConstraintL
// -----------------------------------------------------------------------------
//
void CDrmRightsParser::CParsedPermission::ResetConstraintL(
    CDRMConstraint*& aConstraint )
    {
    delete aConstraint;
    aConstraint = NULL;
    aConstraint = CDRMConstraint::NewL();
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::CParsedRightsObject::CParsedRightsObject
// -----------------------------------------------------------------------------
//
CDrmRightsParser::CParsedRightsObject::CParsedRightsObject():
    iRightsObjectId( NULL ),
    iCurrentAsset( NULL ),
    iCurrentPermission( NULL ),
    iCurrentConstraint( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::CParsedRightsObject::~CParsedRightsObject
// -----------------------------------------------------------------------------
//
CDrmRightsParser::CParsedRightsObject::~CParsedRightsObject()
    {
    delete iRightsObjectId;
    iAssets.ResetAndDestroy();
    iAssets.Close();
    iPermissions.ResetAndDestroy();
    iPermissions.Close();
    }


// -----------------------------------------------------------------------------
// CDrmRightsParser::CParsedRightsObject::RemoveInvalid
// -----------------------------------------------------------------------------
//
void CDrmRightsParser::CParsedRightsObject::RemoveInvalid()
    {
    if ( iPermissions.Count() == 0 )
        {
        return;
        }
    for ( TInt j( 0 ); j < iPermissions.Count(); ++j )
        {
        CDrmRightsParser::CParsedPermission* permission( NULL );
        permission = iPermissions[ j ];
        if ( permission->iInvalid )
            {
            iPermissions.Remove( j );
            delete permission;
            }
        permission = NULL;
        }
    if ( iPermissions.Count() == 0 )
        {
        iInvalid = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::CDrmRightsParser
// -----------------------------------------------------------------------------
//
CDrmRightsParser::CDrmRightsParser():
    iParser( NULL ),
    iRights( NULL ),
    iContent( NULL ),
    iUnknownTag( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::ConstructL
// -----------------------------------------------------------------------------
//
void CDrmRightsParser::ConstructL(
        TParserType aType )
    {
    TTime currentUniversal;
    TTime currentLocal;
    TInt64 result( 0 );

    currentUniversal.UniversalTime();
    currentLocal.HomeTime();

    result = currentLocal.Int64() - currentUniversal.Int64();
    result /= KMinuteInMicroseconds;
    result /= KTimeZoneIncrement;

    iTimeZone = I64INT( result );

    iParserType = aType;
    if ( aType == EXmlParser )
        {
        iParser = CParser::NewL( KXmlParserMimeType, *this );
        }
    else
        {
        iParser = CParser::NewL( KWbxmlParserMimeType, *this );
        }

    for ( TInt i( 0 ); i < ELEMENT_COUNT( KElements ); i++ )
        {
        TPtrC8 ptr( KElements[ i ].iString,
                User::StringLength( KElements[ i ].iString ) );
        iElements[ KElements[ i ].iNumber ] =
            iParser->StringPool().OpenStringL( ptr );
        }
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDrmRightsParser* CDrmRightsParser::NewL(
        TParserType aType )
    {
    CDrmRightsParser* self( new ( ELeave ) CDrmRightsParser );

    CleanupStack::PushL( self );
    self->ConstructL( aType );
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
EXPORT_C CDrmRightsParser::~CDrmRightsParser()
    {
    for ( TInt i( 0 ); i < ELEMENT_COUNT( KElements ); i++ )
        {
        iElements[ KElements[ i ].iNumber ].Close();
        }
    delete iParser;
    delete iRights;
    delete iContent;
    delete iUnknownTag;
    }


// -----------------------------------------------------------------------------
// CDrmRightsParser::ParseL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDrmRightsParser::ParseL(
        const TDesC8& aRightsObject,
        RPointerArray< CDRMRights >& aResultRights )
    {
    TInt i( 0 );

    iElementStackDepth = 0;
    delete iRights;
    iRights = NULL;
    delete iContent;
    iContent = NULL;
    iRights = new ( ELeave ) CParsedRightsObject;
    iParser->ParseBeginL();
    if ( iParserType == EWbxmlParser )
        {
        iParser->EnableFeature( ERawContent );
        }

    while ( i < aRightsObject.Length() )
        {
        TInt n( Min( aRightsObject.Length() - i, KParserChunkSize ) );
        iParser->ParseL( aRightsObject.Mid( i, n ) );
        i += n;
        }
    iParser->ParseEndL();
    TransformRightsObjectL( aResultRights );
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::ParseAndStoreL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDrmRightsParser::ParseAndStoreL(
        const TDesC8& aRightsObject,
        RPointerArray< CDRMRights >& aResultRights )
    {
    RDRMRightsClient client;

    User::LeaveIfError( client.Connect() );
    CleanupClosePushL( client );
    ParseL( aRightsObject, aResultRights );
    for ( TInt i( 0 ); i < aResultRights.Count(); i++ )
        {
        TDRMUniqueID id;
        HBufC8 *cid( NULL );
        CDRMRights* rights( aResultRights[ i ] );
        rights->GetContentURI( cid );
        if ( rights->GetAsset().iKey.Length() == 0 )
            {
            for ( TInt j( 0 ); j < KDCFKeySize; j++ )
                {
                rights->GetAsset().iKey.Append( Math::Random() );
                }
            }
        client.AddRecord( rights->GetAsset().iKey, rights->GetPermission(),
                *cid, id );
        rights->SetLocalID( id );
        delete cid;
        }

    CleanupStack::PopAndDestroy( &client );
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::OnStartDocumentL
// -----------------------------------------------------------------------------
//
void CDrmRightsParser::OnStartDocumentL(
        const RDocumentParameters& /*aDocParam*/,
        TInt /*aErrorCode*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::OnEndDocumentL
// -----------------------------------------------------------------------------
//
void CDrmRightsParser::OnEndDocumentL(
        TInt /*aErrorCode*/ )
    {
    if ( iRights )
        {
        // Remove invalid parsed permissions,
        // and invalidate parsed rights object,
        // if there are no valid ROs left
        iRights->RemoveInvalid();
        }
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::OnStartElementL
// -----------------------------------------------------------------------------
//
void CDrmRightsParser::OnStartElementL(
        const RTagInfo& aElement,
        const RAttributeArray& aAttributes,
        TInt /*aErrorCode*/ )
    {
    TBool tagRecognized( EFalse );

    if ( iUnknownTag )
        {
        return;
        }
    if ( iContent )
        {
        delete iContent;
        iContent = NULL;
        iContent = HBufC8::NewL( 0 );
        }

    for ( TInt i( 0 ); i < KMaxElementCount; i++ )
        {
        if ( aElement.LocalName() == iElements[ i ] )
            {
            tagRecognized = ETrue;
            iElementStack[ iElementStackDepth ] =
                static_cast< TElementEnum >( i );
            iElementStackDepth++;
            if ( iElementStackDepth == KMaxElementNesting )
                {
                User::Leave( EXmlUnexpectedState );
                }
            break;
            }
        }
    if ( tagRecognized )
        {
        TLex8 lex;
        HBufC8* b( NULL );
        TInt n( 0 );

        switch ( MatchStackState() )
            {
            case ETopLevelConstraintState:
                iRights->iCurrentPermission->SetCurrentConstraint(
                    ERightsTopLevel );
                iRights->iCurrentConstraint =
                    iRights->iCurrentPermission->iTopLevel;
                break;
            case EDisplayConstraintState:
                iRights->iCurrentPermission->SetCurrentConstraint(
                    ERightsDisplay );
                iRights->iCurrentConstraint =
                    iRights->iCurrentPermission->iDisplay;
                break;
            case EPlayConstraintState:
                iRights->iCurrentPermission->SetCurrentConstraint(
                    ERightsPlay );
                iRights->iCurrentConstraint =
                    iRights->iCurrentPermission->iPlay;
                break;
            case EPrintConstraintState:
                iRights->iCurrentPermission->SetCurrentConstraint(
                    ERightsPrint );
                iRights->iCurrentConstraint =
                    iRights->iCurrentPermission->iPrint;
                break;
            case EExecuteConstraintState:
                iRights->iCurrentPermission->SetCurrentConstraint(
                    ERightsExecute );
                iRights->iCurrentConstraint =
                    iRights->iCurrentPermission->iExecute;
                break;
            case EExportConstraintState:
                iRights->iCurrentPermission->SetCurrentConstraint(
                    ERightsExport );
                iRights->iCurrentConstraint =
                    iRights->iCurrentPermission->iExport;
                break;
            case EAgreementAssetState:
                iRights->iCurrentAsset = CParsedAsset::NewL();
                iRights->iCurrentAsset->iId =
                    GetAttributeValueL( aAttributes, KIdAttribute );
                iRights->iCurrentAsset->iIdRef =
                    GetAttributeValueL( aAttributes, KIdRefAttribute );
                iRights->iAssets.AppendL ( iRights->iCurrentAsset );
                break;
            case EPermissionAssetState:
                iRights->iCurrentAsset = CParsedAsset::NewL();
                iRights->iCurrentAsset->iId =
                    GetAttributeValueL( aAttributes, KIdAttribute );
                iRights->iCurrentAsset->iIdRef =
                    GetAttributeValueL( aAttributes, KIdRefAttribute );
                iRights->iCurrentPermission->iAssets.Append(
                        iRights->iCurrentAsset );
                break;
            case EPermissionState:
                iRights->iCurrentPermission = CParsedPermission::NewL();
                iRights->iPermissions.AppendL ( iRights->iCurrentPermission );
                iRights->iCurrentPermission->iOnExpiredUrl =
                    GetAttributeValueL( aAttributes, KOnExpiredUrlAttribute );
                break;
            case ESoftwareState:
                b = GetAttributeValueL( aAttributes, KSchemaAttribute );
                if ( !b )
                    {
                    b = GetAttributeValueL( aAttributes, KSchemeAttribute );
                    }
                if ( !b )
                    {
                    User::Leave( KErrArgument );
                    }
                CleanupStack::PushL( b );
                if ( b->CompareF( KSchemaSymbianSid ) == 0 &&
                        iRights->iCurrentConstraint->iSecureId ==
                        TUid::Null() )
                    {
                    iSoftwareSchemeType = ESymbianSid;
                    }
                else if ( b->CompareF( KSchemaSymbianVid ) == 0 &&
                        iRights->iCurrentConstraint->iVendorId ==
                        TUid::Null() )
                    {
                    iSoftwareSchemeType = ESymbianVid;
                    }
                else
                    {
                    User::Leave( KErrNotSupported );
                    }
                CleanupStack::PopAndDestroy( b );
                break;
            case ETimedCountState:
                b = GetAttributeValueL( aAttributes, KTimerAttribute );
                if ( !b )
                    {
                    User::Leave( KErrArgument );
                    }
                CleanupStack::PushL( b );
                lex = *b;
                lex.Val( n );
                if ( n < 0 )
                    {
                    User::Leave( KErrArgument );
                    }
                iRights->iCurrentConstraint->iTimedInterval = n;
                CleanupStack::PopAndDestroy( b );
                break;
            case EKeyInfoState:
                b = GetAttributeValueL( aAttributes, KIdAttrUpperCase );
                if ( b && b->Right( KAuthSeed().Length() ).Compare(
                            KAuthSeed ) == 0 )
                    {
                    iRights->iKeyIsCek = EFalse;
                    delete b;
                    }
                else
                    {
                    iRights->iKeyIsCek = ETrue;
                    }
                break;
#ifdef RD_DRM_METERING
            case ETrackRequirementState:
                if ( !iRights->iCurrentConstraint ||
                     !iRights->iCurrentPermission ||
                     iRights->iCurrentConstraint ==
                     iRights->iCurrentPermission->iTopLevel )
                    {
                    User::Leave( KErrArgument );
                    }
                if ( !iRights->iCurrentConstraint->iDrmMeteringInfo )
                    {
                    iRights->iCurrentConstraint->iDrmMeteringInfo =
                        new ( ELeave ) CDRMConstraint::TDrmMeteringInfo;
                    }
                b = GetAttributeValueL( aAttributes, KTimedAttribute );
                if ( b )
                    {
                    CleanupStack::PushL( b );
                    lex = *b;
                    if ( lex.Val( n ) != KErrNone )
                        {
                        // given input does not fit to TInt
                        User::Leave( KErrArgument );
                        }
                    iRights->iCurrentConstraint->iDrmMeteringInfo->iGraceTime = n;

                    CleanupStack::PopAndDestroy( b );
                    b = NULL;
                    }
                b = GetAttributeValueL( aAttributes, KContentAccessGrantedAttribute );
                // put value to content access granted without metering
                if ( b && !b->CompareF( KContentAccessGrantedValueTrue ) )
                    {
                    iRights->iCurrentConstraint->iDrmMeteringInfo->iAllowUseWithoutMetering = ETrue;
                    }
                else
                    {
                    iRights->iCurrentConstraint->iDrmMeteringInfo->iAllowUseWithoutMetering = EFalse;
                    }
                delete b;
                break;
#endif // RD_DRM_METERING
            }
        }
    if ( !tagRecognized && iElementStackDepth > 0 &&
            iElementStack[ 0 ] == ::ERights )
        {
        //Found unrecognised tag in <(o-ex:)rights>. Let's ignore its content.
        iUnknownTag = aElement.LocalName().DesC().AllocL();
        if ( iRights && iRights->iCurrentPermission )
            {
            iRights->iCurrentPermission->iInvalidConstraints |=
                iRights->iCurrentPermission->iCurrentConstraint;
            if ( iRights->iCurrentPermission->iCurrentConstraint
                    == ERightsTopLevel )
                {
                iRights->iCurrentPermission->iInvalid = ETrue;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::OnEndElementL
// -----------------------------------------------------------------------------
//
void CDrmRightsParser::OnEndElementL(
        const RTagInfo& aElement,
        TInt /*aErrorCode*/ )
    {
    if ( iUnknownTag )
        {
        if ( !iUnknownTag->Compare( aElement.LocalName().DesC() ) )
            {
            delete iUnknownTag;
            iUnknownTag = NULL;
            }
        return;
        }

    for ( TInt i ( 0 ); i < KMaxElementCount; i++ )
        {
        if ( aElement.LocalName() == iElements[ i ] )
            {
            switch ( MatchStackState() )
                {
                case ERoUidState:
                    iContent->Des().Trim(); // remove ignorable white spaces
                    iRights->iRightsObjectId = iContent->AllocL();
                    break;
                case EDateTimeStartState:
                    if ( iRights->iCurrentConstraint )
                        {
                        iRights->iCurrentConstraint->iActiveConstraints |=
                            EConstraintStartTime;
                        iRights->iCurrentConstraint->iStartTime =
                            ParseRelTimeL( *iContent );
                        }
                    break;
                case EDateTimeEndState:
                    if ( iRights->iCurrentConstraint )
                        {
                        iRights->iCurrentConstraint->iActiveConstraints |=
                            EConstraintEndTime;
                        iRights->iCurrentConstraint->iEndTime =
                            ParseRelTimeL( *iContent );
                        }
                    break;
                case EIntervalState:
                    if ( iRights->iCurrentConstraint )
                        {
                        iRights->iCurrentConstraint->iActiveConstraints |=
                            EConstraintInterval;
                        iRights->iCurrentConstraint->iInterval =
                            ParseRelInterval( *iContent );
                        iRights->iCurrentConstraint->iIntervalStart =
                            Time::NullTTime();
                        if ( iRights->iCurrentConstraint->iInterval.Int()
                                <= 0 )
                            {
                            User::Leave( KErrArgument );
                            }
                        }
                    break;
                case EAccumulatedState:
#ifndef __DRM_OMA2
                    User::Leave( KErrArgument );
#endif
                    if ( iRights->iCurrentConstraint )
                        {
                        iRights->iCurrentConstraint->iActiveConstraints |=
                            EConstraintAccumulated;
                        iRights->iCurrentConstraint->iAccumulatedTime =
                            ParseRelInterval( *iContent ).Int();
                        if ( iRights->iCurrentConstraint->
                                iAccumulatedTime.Int() <= 0 )
                            {
                            User::Leave( KErrArgument );
                            }
                        }
                    break;
                case ECountState:
                    if ( iRights->iCurrentConstraint )
                        {
                        TLex8 lex( *iContent );
                        iRights->iCurrentConstraint->iActiveConstraints |=
                            EConstraintCounter;
                        lex.Val( iRights->iCurrentConstraint->
                                iOriginalCounter );
                        iRights->iCurrentConstraint->iCounter =
                            iRights->iCurrentConstraint->iOriginalCounter;
                        if ( iRights->iCurrentConstraint->iCounter <= 0 ||
                                iRights->iCurrentConstraint->iCounter >
                                KDRMCounterMax )
                            {
                            User::Leave( KErrArgument );
                            }
                        }
                    break;
                case ETimedCountState:
#ifndef __DRM_OMA2
                    User::Leave( KErrArgument );
#endif
                    if ( iRights->iCurrentConstraint )
                        {
                        CDRMConstraint* c( iRights->iCurrentConstraint );
                        TLex8 lex( *iContent );
                        lex.Val( c->iOriginalTimedCounter );
                        if ( c->iOriginalTimedCounter <= 0 ||
                                c->iOriginalTimedCounter > KDRMCounterMax )
                            {
                            User::Leave( KErrArgument );
                            }
                        if ( c->iTimedInterval.Int() > 0 )
                            {
                            c->iActiveConstraints |= EConstraintTimedCounter;
                            c->iTimedCounter = c->iOriginalTimedCounter;
                            }
                        else
                            {
                            c->iActiveConstraints |= EConstraintCounter;
                            c->iOriginalCounter = c->iOriginalTimedCounter;
                            c->iCounter = c->iOriginalCounter;
                            c->iOriginalTimedCounter = 0;
                            }
                        }
                    break;
                case ESystemState:
#ifndef __DRM_OMA2
                    User::Leave( KErrArgument );
#endif
                    if ( iRights->iCurrentConstraint )
                        {
                        HBufC8* b( iContent->AllocLC() );
                        iRights->iCurrentConstraint->iSystem.AppendL( b );
                        CleanupStack::Pop( b );
                        iRights->iCurrentConstraint->iActiveConstraints |=
                            EConstraintSystem;
                        }
                    break;
                case EAssetUidState:
                    iContent->Des().Trim(); // remove ignorable white spaces
                    if ( !iRights->iCurrentAsset )
                        {
                        User::Leave( KErrArgument );
                        }
                    iRights->iCurrentAsset->iUid = iContent->AllocL();
                    break;
                case EAssetDigestState:
                    if ( iParserType == EXmlParser )
                        {
                        DecodeAndDeleteUndecodedL( iContent );
                        }
                    CopyOrLeaveL( iRights->iCurrentAsset->iDigest, *iContent );
                    break;
                case EAssetKeyState:
                    if ( iParserType == EXmlParser )
                        {
                        DecodeAndDeleteUndecodedL( iContent );
                        }
                    CopyOrLeaveL( iRights->iCurrentAsset->iKey, *iContent );
                    break;
                case EEncryptedKeyState:
                    if ( iParserType == EXmlParser )
                        {
                        DecodeAndDeleteUndecodedL( iContent );
                        }
                    if ( iRights->iKeyIsCek )
                        {
                        CopyOrLeaveL(
                            iRights->iCurrentAsset->iProtectedKey, *iContent );
                        }
                    else
                        {
                        CopyOrLeaveL(
                            iRights->iCurrentAsset->iProtectedAuthSeed,
                            *iContent );
                        }
                    break;
                case EAssetInheritUidState:
                    iContent->Des().Trim(); // remove ignorable white spaces
                    iRights->iCurrentAsset->iInherit = iContent->AllocL();
                    break;
                case EIndividualState:
#ifndef __DRM_OMA2
                    User::Leave( KErrArgument );
#endif
                    if ( iRights->iCurrentConstraint )
                        {
                        HBufC8* b( NULL );
                        iRights->iCurrentConstraint->iActiveConstraints |=
                            EConstraintIndividual;
                        iContent->Des().Trim(); // remove ignorable white spaces
                        b = iContent->AllocLC();
                        iRights->iCurrentConstraint->iIndividual.AppendL( b );
                        CleanupStack::Pop( b );
                        }
                    break;
                case ESoftwareState:
#ifndef __DRM_OMA2
                    User::Leave( KErrArgument );
#endif
                    if ( iRights->iCurrentConstraint )
                        {
                        TUint32 n( 0 );
                        TLex8 lex( *iContent );
                        User::LeaveIfError( lex.Val( n, EHex ) );
                        if ( iSoftwareSchemeType == ESymbianSid )
                            {
                            iRights->iCurrentConstraint->iActiveConstraints |=
                                EConstraintSoftware;
                            iRights->iCurrentConstraint->iSecureId =
                                TUid::Uid( n );
                            }
                        else
                            {
                            iRights->iCurrentConstraint->iActiveConstraints |=
                                EConstraintVendor;
                            iRights->iCurrentConstraint->iVendorId =
                                TUid::Uid( n );
                            }
                        }
                    break;
                case EExportConstraintState:
                case ETopLevelConstraintState:
#ifndef __DRM_OMA2
                    User::Leave( KErrArgument );
#endif
                case EDisplayConstraintState:
                case EPlayConstraintState:
                case EPrintConstraintState:
                case EExecuteConstraintState:
                    iRights->iCurrentConstraint = NULL;
                    iRights->iCurrentPermission->iCurrentConstraint = 0;
                    break;
                case EPermissionState:
                    iRights->iCurrentPermission->RemoveInvalidL();
                    iRights->iCurrentPermission = NULL;
                    break;

                }
            iElementStackDepth--;
            if ( iElementStackDepth < 0 )
                {
                User::Leave( EXmlUnexpectedState );
                }
            break;  // no need to iterate after element name matched
            }
        }
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::OnContentL
// -----------------------------------------------------------------------------
//
void CDrmRightsParser::OnContentL(
        const TDesC8& aBytes,
        TInt /*aErrorCode*/ )
    {
    if ( iUnknownTag )
        {
        return;
        }
    if ( !iContent )
        {
        iContent = HBufC8::NewL( aBytes.Size() );
        *iContent = aBytes;
        }
    else
        {
        iContent = iContent->ReAllocL( iContent->Size() + aBytes.Size() );
        TPtr8 c( iContent->Des() );
        c.Append( aBytes );
        }
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::OnStartPrefixMappingL
// -----------------------------------------------------------------------------
//
void CDrmRightsParser::OnStartPrefixMappingL(
        const RString& /*aPrefix*/,
        const RString& /*aUri*/,
        TInt /*aErrorCode*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::OnEndPrefixMappingL
// -----------------------------------------------------------------------------
//
void CDrmRightsParser::OnEndPrefixMappingL(
        const RString& /*aPrefix*/,
        TInt /*aErrorCode*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::OnIgnorableWhiteSpaceL
// -----------------------------------------------------------------------------
//
void CDrmRightsParser::OnIgnorableWhiteSpaceL(
        const TDesC8& /*aBytes*/,
        TInt /*aErrorCode*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::OnSkippedEntityL
// -----------------------------------------------------------------------------
//
void CDrmRightsParser::OnSkippedEntityL(
        const RString& /*aName*/,
        TInt /*aErrorCode*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::OnProcessingInstructionL
// -----------------------------------------------------------------------------
//
void CDrmRightsParser::OnProcessingInstructionL(
        const TDesC8& /*aTarget*/,
        const TDesC8& /*aData*/,
        TInt /*aErrorCode*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::OnOutOfData
// -----------------------------------------------------------------------------
//
void CDrmRightsParser::OnOutOfData()
    {
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::OnError
// -----------------------------------------------------------------------------
//
void CDrmRightsParser::OnError(
        TInt /*aErrorCode*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::GetExtendedInterface
// -----------------------------------------------------------------------------
//
TAny* CDrmRightsParser::GetExtendedInterface(
        const TInt32 /*aUid*/ )
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::MatchStackState
// -----------------------------------------------------------------------------
//
TInt CDrmRightsParser::MatchStackState( void )
    {
    TParserStackState r( EUnknownState );

    for ( TInt i( 0 );
            r == EUnknownState &&
            i < ELEMENT_COUNT( KParserStackStates ); i++ )
        {
        TInt j( 0 );
        TInt k( 0 );
        for ( j = iElementStackDepth - 1, k = 0;
                j > 0 && KParserStackStates[ i ].iStack[ k ] != ELast;
                j--, k++ )
            {
            if ( iElementStack[ j ] != KParserStackStates[ i ].iStack[ k ] )
                {
                break;
                }
            }
        if ( ( j == 0 &&
               iElementStack[ j ] == KParserStackStates[ i ].iStack[ k ] &&
               KParserStackStates[ i ].iStack[ k + 1 ] == ELast ) ||
             KParserStackStates[ i ].iStack[ k ] == ELast )
            {
            r = KParserStackStates[ i ].iState;
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::TransformRightsObjectL
// -----------------------------------------------------------------------------
//
void CDrmRightsParser::TransformRightsObjectL(
        RPointerArray< CDRMRights >& aResult )
    {
    CParsedAsset* asset( NULL );
    CDRMRights* rights( NULL );
    CParsedPermission* permission( NULL );
    CDRMPermission *ro( CDRMPermission::NewLC() );
    CDRMAsset* a( NULL );
    HBufC8* id( NULL );
    HBufC8* idRef( NULL );

    // If no permissions are given, create an empty RO.
    // The procedure is quite different
    // than processing the permissions
    if ( iRights->iInvalid )
        {
        //The RO is invalid. Let's leave
        User::Leave( KErrArgument );
        }
    if ( iRights->iPermissions.Count() == 0 )
        {
        for ( TInt i( 0 ); i < iRights->iAssets.Count(); i++ )
            {
            asset = iRights->iAssets[ i ];
            // No uid for the asset, it's an invalid asset
            if( !asset->iUid )
                {
                continue;
                }

            rights = CDRMRights::NewL();
            CleanupStack::PushL( rights );

            a = CDRMAsset::NewLC();
            a->iDigest.Copy( asset->iDigest );
            a->iKey.Copy( asset->iKey );
            a->iProtectedKey.Copy( asset->iProtectedKey );
            a->iAuthenticationSeed.Copy( asset->iAuthenticationSeed );
            a->iProtectedAuthSeed.Copy( asset->iProtectedAuthSeed );

            // guaranteed not to be NULL UID in this phase
            a->iUid = asset->iUid->AllocL();

            if ( ro->iParentUID )
                {
                delete ro->iParentUID;
                ro->iParentUID = NULL;
                }
            if ( asset->iInherit )
                {
                a->iParentRights = asset->iInherit->AllocL();
                ro->iParentUID = asset->iInherit->AllocL();
                }
            rights->SetAssetL( *a );
            CleanupStack::PopAndDestroy( a );

            // by default, set the RO version as OMA 1.0
            ro->iRightsObjectVersion.iVersionMain = EOma1Rights;
            rights->SetPermissionL( *ro );

            // Warning: assuming RPointerArray
            // does not put its argument to clenaupstack
            aResult.AppendL( rights );
            CleanupStack::Pop( rights );
            }
        }
    else for ( TInt i( 0 ); i < iRights->iPermissions.Count(); i++ )
        {
        permission = iRights->iPermissions[ i ];
        if ( permission->iInvalid )
            {
            // The permission under test is invalid, so let's move forward
            continue;
            }
        if ( permission->iAssets.Count() > 0 )
            {
            // The permission refers to asset elements in the RO,
            // add all referenced assets to the permission
            for ( TInt j( 0 ); j < permission->iAssets.Count(); j++ )
                {
                TInt k( 0 );
                for ( ; k < iRights->iAssets.Count(); k++ )
                    {
                    id = iRights->iAssets[ k ]->iId;
                    idRef = permission->iAssets[ j ]->iIdRef;
                    if ( id && idRef && *id == *idRef )
                        {
                        break;
                        }
                    }
                if ( k < iRights->iAssets.Count() )
                    {
                    asset = permission->iAssets[ j ];
                    permission->iAssets[ j ] = iRights->iAssets[ k ];
                    delete asset;
                    permission->iNotOwned++;
                    }
                }
            }
        else
            {
            // The permission does not refer to an asset explictly,
            // link all assets in the RO to this permission
            for ( TInt j( 0 ); j < iRights->iAssets.Count(); j++ )
                {
                permission->iAssets.Append( iRights->iAssets[ j ] );
                permission->iNotOwned++;
                }
            }

        for ( TInt j( 0 ); j < permission->iAssets.Count(); j++ )
            {
            asset = permission->iAssets[ j ];
            // No uid for the asset, it's an invalid asset
            if( !asset->iUid )
                {
                continue;
                }

            rights = CDRMRights::NewL();
            CleanupStack::PushL( rights );

            a = CDRMAsset::NewLC();
            a->iDigest.Copy( asset->iDigest );
            a->iKey.Copy( asset->iKey );
            a->iProtectedKey.Copy( asset->iProtectedKey );
            a->iAuthenticationSeed.Copy( asset->iAuthenticationSeed );
            a->iProtectedAuthSeed.Copy( asset->iProtectedAuthSeed );

            // guaranteed not to be NULL UID in this phase
            a->iUid = asset->iUid->AllocL();

            if ( ro->iParentUID )
                {
                delete ro->iParentUID;
                ro->iParentUID = NULL;
                }
            if ( asset->iInherit )
                {
                a->iParentRights = asset->iInherit->AllocL();
                ro->iParentUID = asset->iInherit->AllocL();
                }
            rights->SetAssetL( *a );
            CleanupStack::PopAndDestroy( a );


            // ( == iOnExpiredUrl must not be referred elsewhere)
            ro->iOnExpiredUrl = permission->iOnExpiredUrl;
            permission->iOnExpiredUrl = NULL;

            ro->iAvailableRights = permission->iAvailableRights;
            ro->iDisplay->DuplicateL( *permission->iDisplay );
            ro->iPlay->DuplicateL( *permission->iPlay );
            ro->iPrint->DuplicateL( *permission->iPrint );
            ro->iExecute->DuplicateL( *permission->iExecute );
            ro->iExport->DuplicateL( *permission->iExport );
            ro->iTopLevel->DuplicateL( *permission->iTopLevel );
            // by default, set the RO version as OMA 1.0
            ro->iRightsObjectVersion.iVersionMain = EOma1Rights;
            rights->SetPermissionL( *ro );

            aResult.AppendL( rights );
            CleanupStack::Pop( rights );
            }
        }
    CleanupStack::PopAndDestroy( ro );
    }

// -----------------------------------------------------------------------------
// CDrmRightsParser::GetAttributeValueL
// -----------------------------------------------------------------------------
//
HBufC8* CDrmRightsParser::GetAttributeValueL(
        const RAttributeArray& aAttrList,
        const TDesC8& aAttrName )
    {
    HBufC8* r( NULL );
    RAttribute a;

    for ( TInt i( 0 ); !r && i < aAttrList.Count(); i++ )
        {
        a = aAttrList[ i ];
        if ( a.Attribute().LocalName().DesC().Compare( aAttrName ) == 0 )
            {
            r = a.Value().DesC().AllocL();
            }
        }
    return r;
    }

//  End of File
