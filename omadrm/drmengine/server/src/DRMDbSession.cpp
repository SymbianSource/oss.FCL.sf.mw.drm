/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class handles all client requests.
*
*/


// INCLUDE FILES
#include <s32file.h>
#include <f32file.h>
#include <symmetric.h>
#include <rijndael.h>
#include <caf/caf.h>
#include <x509keys.h>
#include <asn1dec.h>
#include <DcfRep.h>

#include <e32math.h>
#include <utf.h>

#include <sysutil.h>    // Disk space checking

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include "Oma2Agent.h"
#include "DrmPermission.h"
#include "DRMDbSession.h"
#include "DRMEngineClientServer.h"
#include "DRMRightsServer.h"
#include "RoapStorageClient.h"
#include "OmaCrypto.h"
#include "CmlaCrypto.h"
#include "DrmKeyStorage.h"
#include "drmrightsdb.h"
#include "drmparentstorage.h"
#include "DRMDomainContext.h"
#include "DRMRIContext.h"
#include "drmenginetypedefs.h"
#include "DRMXOma.h"

#include "DRMNotifier.h"
#include "DRMEventAddRemove.h"
#include "DRMEventModify.h"
#include "DRMRightsCleaner.h"
#include "DRMActiveOperation.h"
#include "drmconsume.h"
#include "drmlog.h"
#include "DRMPointerArray.h"
#include "Base64.h"

#ifdef RD_DRM_METERING
#include "drmmeteringdbdata.h"
#endif

// NAMESPACES
using namespace DRMEngine;

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS

// MACROS
#define REPLAYCACHE reinterpret_cast< CDRMRightsServer* >\
    (const_cast<CServer2*>(Server()))->ReplayCache()
#define DRMDB ( ( CDRMRightsServer* )( Server() ) )->Database()
#ifdef RD_DRM_METERING
#define METERINGDB reinterpret_cast< CDRMRightsServer* >\
    (const_cast<CServer2*>(Server()))->MeteringDatabase()
#endif
#define RFSSESSION ( ( CDRMRightsServer* )( Server() ) )->FileServerSession()
#define DRMNOTIFIER ( ( CDRMRightsServer* )( Server() ) )->Notifier()
#define XOMAHEADER ( ( CDRMRightsServer* )( Server() ) )->XOmaHeaders()
#define IMEI ( ( CDRMRightsServer* )( Server() ) )->GetIMEIL()
#define IMSI ( ( CDRMRightsServer* )( Server() ) )->GetIMSIL()
#define SERVER reinterpret_cast< CDRMRightsServer* >\
    (const_cast<CServer2*>(Server()))

#define IPCREAD0L( a ) aMessage.ReadL( 0, a )
#define IPCREAD1L( a ) aMessage.ReadL( 1, a )
#define IPCREAD2L( a ) aMessage.ReadL( 2, a )
#define IPCREAD3L( a ) aMessage.ReadL( 3, a )
#define IPCWRITE0L( a ) aMessage.WriteL( 0, a )
#define IPCWRITE1L( a ) aMessage.WriteL( 1, a )
#define IPCWRITE2L( a ) aMessage.WriteL( 2, a )
#define IPCWRITE3L( a ) aMessage.WriteL( 3, a )
#define IPCGETDESLEN0 aMessage.GetDesLength( 0 )
#define IPCGETDESLEN1 aMessage.GetDesLength( 1 )
#define IPCGETDESLEN2 aMessage.GetDesLength( 2 )
#define IPCGETDESLEN3 aMessage.GetDesLength( 3 )
#define IPCGETDESMAXLEN0 aMessage.GetDesMaxLength( 0 )
#define IPCGETDESMAXLEN1 aMessage.GetDesMaxLength( 1 )
#define IPCGETDESMAXLEN2 aMessage.GetDesMaxLength( 2 )
#define IPCGETDESMAXLEN3 aMessage.GetDesMaxLength( 3 )

// LOCAL CONSTANTS AND MACROS

#ifdef RD_MULTIPLE_DRIVE

_LIT( KDbTempPath, "%c:\\system\\temp\\" );
_LIT( KTimedReplayCacheFile, "%c:\\private\\101F51F2\\timererc.dat" );
_LIT( KPlainReplayCacheFile, "%c:\\private\\101F51F2\\plainrc.dat" );
#ifdef RD_DRM_METERING
_LIT( KMeteringDataBaseFile, "%c:\\private\\101F51F2\\meterdb.dat" );
#endif

#else

_LIT( KTimedReplayCacheFile, "c:\\private\\101F51F2\\timererc.dat" );
_LIT( KPlainReplayCacheFile, "c:\\private\\101F51F2\\plainrc.dat" );
#ifdef RD_DRM_METERING
_LIT( KMeteringDataBaseFile, "c:\\private\\101F51F2\\meterdb.dat" );
#endif

#endif


const TInt KMicrosecondsToSecond = 1000000;
_LIT8( KFLPrefix, "flk" );
_LIT8( KFLLongPrefix, "flk:flk" );

_LIT8( KFLSuffix, "@localhost" );

// These need to be updated if the DRM Filter / Oma1DcfCreator URI's change
// IMPORTANT IMPORTANT IMPORTANT
_LIT8( KDCMUri, "ldf:31415926535@localhost");
_LIT8( KLDFUri, "flk:flkS60_3_0_Hutchinson_2005");

_LIT8(KTimeStamp, "timeStamp");
#ifdef RD_DRM_METERING
_LIT8(KMeteringDelimiter, ":");
_LIT8(KCRLF, "\r\n" );
#endif

LOCAL_C const TUint8 KFLPrefixLength = 3;
const TUint32 KTrustedShutdownClient = 0x10205CB5;
const TUint32 KAppInstSrv = 0x101F875A;


const TInt KSanityDataLengthLow = 0;
const TInt KSanityDataLengthHigh = 32768;

// MODULE DATA STRUCTURES
NONSHARABLE_STRUCT( TDeleteFile )
    {
    RFs* iFs;
    TFileName* iFileName;
    };

// ============================ auto_handde helper class =======================
//Auto handle for easening handle release on exceptional exit situations
template<class T> class auto_handle
    {
public:

    auto_handle() {}
    auto_handle(T aHandle) : iHandle( aHandle ) {}
    auto_handle( auto_handle<T>& aHandle) : iHandle( aHandle.release() ) {}
    ~auto_handle() { iHandle.Close(); }
    const T& operator()() const { return iHandle; }
    T& operator()() { return iHandle; }
    T get() const { return iHandle; }
    T release() { T temp = iHandle; iHandle = 0; return temp; }

private:
    T iHandle;
    };

// DATA TYPES
// LOCAL FUNCTION PROTOTYPES
LOCAL_C void DeleteFile( TAny* aHandle );
LOCAL_C void DeleteObject( TAny* aObject );
LOCAL_C TTime EndTime( const TTime& aTime1, const TTime& aTime2 );

// FORWARD DECLARATIONS
LOCAL_C void ModifyRightsObjectByTimeL( CDRMPermission* aRights,
                                        TTimeIntervalMicroSeconds& aChange,
                                        TBool aModifyInsertionTime );
LOCAL_C void ModifyTimesInListL( CDRMPermissionList* aList,
                                 TTimeIntervalMicroSeconds& aChange,
                                 TBool aModifyInsertionTime );

LOCAL_C void ModifyConstraintByTime( CDRMConstraint* aConstraint,
                                     TTimeIntervalMicroSeconds& aChange );

LOCAL_C TPtrC8 ExtractElement( const TDesC8& aRights, const TDesC8& aElement,
                               TInt& aOffset );

LOCAL_C TTime Iso8601ToTime( TDesC8& aTimeString );

// ============================= LOCAL FUNCTIONS ==============================

// -----------------------------------------------------------------------------
// SanitizeL
// Performs a sanity check on length parameters
// -----------------------------------------------------------------------------
//
LOCAL_C void SanitizeL( TInt aParam )
    {
    if( aParam <= KSanityDataLengthLow || aParam > KSanityDataLengthHigh )
        {
        User::Leave(KErrArgument);
        }
    }


LOCAL_C void ModifyTimesInListL( CDRMPermissionList* aList,
    TTimeIntervalMicroSeconds& aChange,
    TBool aModifyInsertionTime )
    {
    TInt i = 0;

    // Go through the whole list and run the modification for all objects
    for( i = 0; i < aList->Count(); i++ )
        {
        // Call modification for each rights object in the list
        ModifyRightsObjectByTimeL( (*aList)[i], aChange, aModifyInsertionTime );
        }
    };

LOCAL_C void ModifyRightsObjectByTimeL( CDRMPermission* aRights,
    TTimeIntervalMicroSeconds& aChange,
    TBool aModifyInsertionTime )
    {

    // If original insertion time exists and we want to change it, change it
    if ( aModifyInsertionTime && aRights->iOriginalInsertTime != Time::NullTTime() )
        {
        aRights->iOriginalInsertTime += aChange;
        }

    if ( aRights->iAvailableRights & ERightsTopLevel )
        {
        ModifyConstraintByTime( aRights->iTopLevel, aChange );
        }

    // If play rights are available, check if they need to be changed
    if ( aRights->iAvailableRights & ERightsPlay )
        {
        ModifyConstraintByTime( aRights->iPlay, aChange );
        }

    // If display rights are available, check if they need to be changed
    if ( aRights->iAvailableRights & ERightsDisplay )
        {
        ModifyConstraintByTime( aRights->iDisplay, aChange );
        }

    // If execute rights are available, check if they need to be changed
    if ( aRights->iAvailableRights & ERightsExecute )
        {
        ModifyConstraintByTime( aRights->iExecute, aChange );
        }

    // If print rights are available, check if they need to be changed
    if ( aRights->iAvailableRights & ERightsPrint )
        {
        ModifyConstraintByTime( aRights->iPrint, aChange );
        }

    };

LOCAL_C void ModifyConstraintByTime(
    CDRMConstraint* aConstraint,
    TTimeIntervalMicroSeconds& aChange )
    {
    // if start time exists, modify it
    if ( aConstraint->iActiveConstraints & EConstraintStartTime )
        {
        aConstraint->iStartTime += aChange;
        }

    // if end time exists, modify it
    if ( aConstraint->iActiveConstraints & EConstraintEndTime )
        {
        aConstraint->iEndTime += aChange;
        }

    // if activated interval exists, modify it
    if ( aConstraint->iActiveConstraints & EConstraintInterval &&
         aConstraint->iIntervalStart != Time::NullTTime() )
        {
        aConstraint->iIntervalStart += aChange;
        }
    };

LOCAL_C TPtrC8 ExtractElement( const TDesC8& aRights,
                               const TDesC8& aElement,
                               TInt& aOffset )
    {
    DRMLOG( _L("CDRMDbSession::ExtractElement") );

    TPtrC8 temp( KNullDesC8 );
    TInt startPos = ( 0 );
    TInt endPos = ( 0 );
    TInt ret( 0 );
    TInt startLength ( 0 );

    auto_handle< RBuf8 > tagToBeFound;
    // Must be nonleaving since this function is nonleaving
    ret = tagToBeFound().Create( aElement.Length() + 3 ); // max "</" aElement ">"
    if ( ret != KErrNone )
        {
        aOffset = -1;
        return KNullDesC8();
        }

    // First we try to find the start tag (as localname)
    tagToBeFound().SetLength( 0 );
    tagToBeFound().AppendFormat( _L8( "<%S" ), &aElement );

    temp.Set( aRights.Mid( aOffset ) );

    startPos = temp.Find( tagToBeFound() );

    startLength = tagToBeFound().Length();
    startPos += aOffset;
    temp.Set( aRights.Mid( startPos + startLength ) );

    // Now find the end of the start tag
    tagToBeFound().SetLength( 0 );
    tagToBeFound().Append( _L8( ">" ) ); // '>' as last

    // Define the starting point of the data after the start tag
    // and skip the '>' mark.
    startPos = startPos + startLength + temp.Find( tagToBeFound() ) + 1;

    temp.Set( aRights.Mid( startPos ) );

    // Finally find the start of the end tag
    tagToBeFound().SetLength( 0 );
    tagToBeFound().AppendFormat( _L8( "</%S" ), &aElement );

    endPos = startPos + temp.Find(tagToBeFound() );

    if ( endPos < startPos )
        {
        aOffset = -1;
        return KNullDesC8();
        }

    temp.Set( aRights.Mid(startPos, endPos - startPos) );

    aOffset = endPos;
    DRMLOG2( _L( "Calculated length %d" ), endPos - startPos);
    DRMLOG( _L( "Extracted element" ) );

    //auto_handle closes and frees allocated resources
    return temp;
    };

LOCAL_C TTime Iso8601ToTime( TDesC8& aTimeString )
    {

    DRMLOG( _L("CDRMDbSession::Iso8601ToTime") );

    TLex8 lex;
    TInt year = 0;
    TInt month = 0;
    TInt day = 0;
    TInt hour = 0;
    TInt minute = 0;
    TInt second = 0;
    TTime r = Time::NullTTime();
    TLocale l;
    TTimeIntervalSeconds offset(l.UniversalTimeOffset());

    if (aTimeString.Length() > 0)
        {
        lex = aTimeString;
        lex.Val(year);
        lex.Inc();
        lex.Val(month);
        lex.Inc();
        lex.Val(day);
        lex.Inc();
        lex.Val(hour);
        lex.Inc();
        lex.Val(minute);
        lex.Inc();
        lex.Val(second);
        r = TTime(TDateTime(year, static_cast<TMonth>(month - 1), day - 1,
                hour, minute, second, 0));
        if (lex.Get() != 'Z')
            {
            r += offset;
            }
        }
    return r;
    }

#ifdef RD_DRM_METERING
LOCAL_C HBufC8* CreateMeteringDataL( CDRMPointerArray<CDrmMeteringDbData>* meteringArray )
    {
    // Calculate buffer size of cipher data
    TInt size( sizeof( KCRLF ) );
    HBufC8* cipherData = NULL;
    TPtr8 ptr( NULL, 0 );

    _LIT8( KElementStart, "<rawMeteringReportData>");
    _LIT8( KElementEnd, "</rawMeteringReportData>");

    size = KElementStart().Size() + KElementEnd().Size();

    for( TUint i(0); i < meteringArray->Count(); i++ )
        {
        size += sizeof( ( *meteringArray)[i]->iContentId );
        if ( (*meteringArray)[i]->iParentUid &&
             (*meteringArray)[i]->iParentUid->Size() )
            {
            size += (*meteringArray)[i]->iParentUid->Size() + 1;
            }
        switch( (*meteringArray)[i]->iPermission )
            {
            case EPlay:
                size += 4;
                break;
            case EView:
            case EExecute:
                size += 7;
                break;
            case EPrint:
                size+= 5;
                break;
            default:
                break; // Not a valid permission
            }
        size += (*meteringArray)[i]->iContentId->Size();
        size += sizeof( (*meteringArray)[i]->iAccumulatedTime.Int() / 60 );
        size += sizeof( (*meteringArray)[i]->iAccumulatedTime.Int() % 60 );
        if( ( ( *meteringArray)[i]->iAccumulatedTime.Int() % 60 ) < 10 )
            {
            size++; //for precending zero for seconds..
            }
        size += sizeof( ( *meteringArray)[i]->iCount );
        size += 4 * sizeof( KMeteringDelimiter ); // ":" -delimiter
        size += sizeof( KCRLF );
        }

    cipherData = HBufC8::NewLC( size );
    ptr.Set( cipherData->Des() );
    ptr.Append( KElementStart );

    for( TUint i(0); i < meteringArray->Count(); i++ )
        {
        ptr.Append( KCRLF );
        if ( (*meteringArray)[i]->iParentUid &&
             (*meteringArray)[i]->iParentUid->Size() )
            {
            ptr.Append( *(*meteringArray)[i]->iParentUid );
            ptr.Append( _L(";") );
            }
        ptr.Append( *(*meteringArray)[i]->iContentId );
        ptr.Append( KMeteringDelimiter );
        switch( (*meteringArray)[i]->iPermission )
            {
            case EPlay:
                ptr.Append( _L("play") );
                break;
            case EView:
                ptr.Append( _L("display") );
                break;
            case EExecute:
                ptr.Append( _L("execute") );
                break;
            case EPrint:
                ptr.Append( _L("print") );
                break;
            default:
                break; // Not a valid permission
            }          // export not supported
        ptr.Append( KMeteringDelimiter );
        ptr.AppendNum( (*meteringArray)[i]->iCount );
        ptr.Append( KMeteringDelimiter );
        ptr.AppendNum( (*meteringArray)[i]->iAccumulatedTime.Int() / 60 );
        ptr.Append( KMeteringDelimiter );
        if( ( ( *meteringArray)[i]->iAccumulatedTime.Int() % 60 ) < 10 )
            {
            ptr.AppendNum( 0 ); //precending zero for seconds..
            }
        ptr.AppendNum( (*meteringArray)[i]->iAccumulatedTime.Int() % 60 );
        }
    ptr.Append( KCRLF );
    ptr.Append( KElementEnd );
    CleanupStack::Pop( cipherData );
    return cipherData;
    }
#endif

// ----------------------------------------------------------------------------
// DeleteFile
// Deletes the file by TFileName presented by aHandle
// ----------------------------------------------------------------------------
//
void DeleteFile( TAny* aHandle )
    {
    __ASSERT_DEBUG( aHandle, User::Panic( _L( "DeleteFile" ), KErrArgument ) );
    TDeleteFile* handle = reinterpret_cast< TDeleteFile* >( aHandle );

    handle->iFs->Delete( *( handle->iFileName ) );
    }

// ----------------------------------------------------------------------------
// DeleteObject
// Deletes the file by TFileName presented by aHandle
// ----------------------------------------------------------------------------
//
void DeleteObject( TAny* aObject )
    {
    __ASSERT_DEBUG( aObject, User::Panic( _L( "DeleteObject" ), KErrArgument ) );
    MDrmKeyStorage* object = reinterpret_cast< MDrmKeyStorage* >( aObject );
    delete object;
    object = NULL;
    }


// ----------------------------------------------------------------------------
// EndTime
// Calculate the true end time: pick the smaller one of aTime1 & aTime2,
// but ignore Time::NullTTime anyhow.
// ----------------------------------------------------------------------------
//
TTime EndTime( const TTime& aTime1, const TTime& aTime2 )
    {
    TTime nullTime = Time::NullTTime();

    if ( aTime1 == nullTime )
        {
        return aTime2;
        }

    if ( aTime2 == nullTime )
        {
        return aTime1;
        }

    return Min( aTime1, aTime2 );
    }

// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// CDRMRightsServer::NewLC
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CDRMDbSession* CDRMDbSession::NewL()
    {
    CDRMDbSession* self = new( ELeave ) CDRMDbSession();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CDRMRightsServer::~CDRMDbSession
// Destructor.
// ----------------------------------------------------------------------------
//
CDRMDbSession::~CDRMDbSession()
    {
    DRMLOG( _L( "CDRMDbSession::~" ) );
    delete iPreparedData;
    iPreparedData = NULL;

    delete iWidePreparedData;
    iWidePreparedData = NULL;

    delete iFileName;
    iFileName = NULL;

    if ( iPendingRequest )
        {
        delete iPendingRequest;
        }

    delete iContentId;
    iContentId = NULL;

    iClient.Close();

    iCek = KNullDesC8;
    iCek.FillZ();

    delete iConsume;
    iRoapClient.Close();
    }

// ----------------------------------------------------------------------------
// CDRMRightsServer::CDRMDbSession
// Default constructor.
// ----------------------------------------------------------------------------
//
CDRMDbSession::CDRMDbSession():
    iCredentialsChecked(ENotChecked),
    iContentId(NULL)
    {
    iRek.SetLength( 0 );
    }


// ----------------------------------------------------------------------------
// CDRMDbSession::RoapClient
// Returns a handle to the Roap client
// ----------------------------------------------------------------------------
Roap::RRoapStorageClient& CDRMDbSession::RoapClient()
    {
    return iRoapClient;
    }

// ----------------------------------------------------------------------------
// CDRMRightsServer::ServiceL
// Forwards requests from clients to helper methods.
// ----------------------------------------------------------------------------
//
void CDRMDbSession::ServiceL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::ServiceL" ) );
    DRMLOG2( _L( "Message Handle: %08x"), aMessage.Handle() );

    // Close the client before opening a new connection over it to avoid
    // leaking memory in the kernel side
    iClient.Close();

    aMessage.ClientL( iClient );

    // This function call is TRAPped by framework, and message is completed
    // with the error in case of leaving operation.
    switch ( aMessage.Function() )
        {
        case EAddRecord:
            AddRecordL( aMessage, ENoProtection );
            break;

        case EAddProtectedRecord:
            AddRecordL( aMessage, EPublicKey );
            break;

        case EAddDomainRecord:
            AddRecordL( aMessage, EDomainKey );
            break;

        case EGetDbEntry:
            GetRecordL( aMessage );
            break;

        case EGetEntryList:
            GetEntryListL( aMessage );
            break;

        case EDeleteWithCID:
            DeleteL( aMessage );
            break;

        case EDeleteRO:
            DeleteRecordL( aMessage );
            break;

        case EExportCIDs:
            ExportCIDsL( aMessage );
            break;

        case EGetKey:
            GetKeyL( aMessage );
            break;

        case ECheckRights:
            CheckRightsL( aMessage );
            break;

        case ECount:
            CountL( aMessage );
            break;

        case EDeleteAll:
            DeleteAllL( aMessage );
            break;

        case EConsume:
            ConsumeL( aMessage );
            break;

        case ECheckConsume:
            CheckConsumeL( aMessage );
            break;

        case ECalculatePadding:
            CalculatePaddingL( aMessage);
            break;

        case ESecureTime:
            SecureTimeL( aMessage );
            break;

        case EGetPreparedData:
            GetPreparedDataL( aMessage );
            break;

        case EAddDomainRO:
            AddDomainROL( aMessage );
            break;

        case EGetDomainRO:
            GetDomainROL( aMessage );
            break;

        case EDeleteDomainRO:
            DeleteDomainROL( aMessage );
            break;

        case EIsInCache:
            IsInCacheL( aMessage );
            break;

        case EAddToCache:
            AddToCacheL( aMessage );
            break;

        case EDecrypt:
            DecryptL( aMessage );
            break;

        case EEncrypt:
            EncryptL( aMessage );
            break;

        case EInitializeKey:
            InitializeKeyL( aMessage );
            break;

        case EInitializeGroupKey:
            InitializeGroupKeyL( aMessage );
            break;

        case EGetDomainRoForCid:
            GetDomainRosForCidL( aMessage );
            break;

        case EDeleteExpired:
            DeleteExpiredPermissionsL( aMessage );
            break;

        case ESetEstimatedArrival:
            SetEstimatedArrivalL( aMessage );
            break;

        case EGetEstimatedArrival:
            GetEstimatedArrivalL( aMessage );
            break;

        case ESetName:
            SetNameL( aMessage );
            break;

        case EGetName:
            GetNameL( aMessage );
            break;

        case EGetWideData:
            GetWideDataL( aMessage );
            break;

        case ECancel:
            Cancel( aMessage );
            break;

        case EGetUdtData:
            GetUdtDataL( aMessage );
            break;

        case EInitiateUdt:
            InitiateUdtL( aMessage );
            break;

        case EInitOrphanedList:
            InitExportOrphanedCIDsL( aMessage );
            break;

        case EGetOrphanedList:
            ExportOrphanedCIDsL( aMessage );
            break;

        case EGetFLUri:
            GetFLUriL( aMessage );
            break;

        case EEncodeRightsIssuerField:
            EncodeRightsIssuerL( aMessage );
            break;

        case EDecodeRightsIssuerField:
            DecodeRightsIssuerL( aMessage );
            break;

        case ESetAuthenticationSeed:
            SetAuthenticationSeedL( aMessage );
            break;

        case EGetAuthenticationSeed:
            GetAuthenticationSeedL( aMessage );
            break;

        case EVerifyMac:
            VerifyMacL( aMessage );
            break;

        case EGetSupportedIndividuals:
            GetSupportedIndividualsL( aMessage );
            break;

        case EStopWatching:
            StopWatchingL( aMessage );
            break;

        case EUnwrapDeviceMacAndRek:
            UnwrapMacAndRekL( aMessage, EFalse );
            break;

        case EUnwrapDomainMacAndRek:
            UnwrapMacAndRekL( aMessage, ETrue );
            break;

        case EGetRandomData:
            GetRandomDataL( aMessage );
            break;

        case EGetMeteringData:
            GetMeteringDataL( aMessage );
            break;

        case EDeleteMeteringData:
            DeleteMeteringDataL( aMessage );
            break;

        default:
            DRMLOG( _L( "CDRMDbSession::DispatchL: Invalid command" ) );
            User::Leave( KErrNotSupported );
        }

    // The message has already completed successfully.
    DRMLOG2( _L( "CDRMDbSession::ServiceL ok (%08x)"), aMessage.Handle() );
    }

// ----------------------------------------------------------------------------
// CDRMRightsServer::ServiceError
// Completes the request with given error code if the request is still pending.
// ----------------------------------------------------------------------------
//
void CDRMDbSession::ServiceError( const RMessage2& aMessage, TInt aError )
    {
    DRMLOG2( _L( "CDRMDbSession::ServiceError: error %d" ), aError );

    if ( !aMessage.IsNull() )
        {
        aMessage.Complete( aError );
        }
    }

// -----------------------------------------------------------------------------
// CDRMRightsServer::ConstructL
// Second phase constructor.
// -----------------------------------------------------------------------------
//
void CDRMDbSession::ConstructL()
    {
    iRoapClientConnected = EFalse;
    }

// -----------------------------------------------------------------------------
// CDRMRightsServer::AddRecord
// Get the information from the client, construct a rights object, and add
// it to the database.
// -----------------------------------------------------------------------------
//
void CDRMDbSession::AddRecordL(
    const RMessage2& aMessage,
    TProtectionType aProtection )
    {
    DRMLOG( _L( "CDRMDbSession::AddRecordL" ) );
    //__UHEAP_MARK;

    CDRMPermission* permission = NULL;
    HBufC8* wrappedcek = NULL;
    HBufC8* cek = NULL;
    HBufC8* CID = NULL;
    HBufC8* rightsData = NULL;
    TInt size = 0;
    TPtr8 data( NULL, 0 );
    TDRMUniqueID UID;


    CDRMEventAddRemove* event = CDRMEventAddRemove::NewLC( ERightsObjectRecieved );
    TRequestStatus status;

    SanitizeL( aMessage.GetDesLength(0) );
    SanitizeL( aMessage.GetDesLength(1) );

    size = User::LeaveIfError( IPCGETDESLEN1 );
    rightsData = HBufC8::NewMaxLC( size );
    data.Set( const_cast< TUint8* >( rightsData->Ptr() ), 0, size );
    IPCREAD1L( data );
    permission = CDRMPermission::NewLC();
    permission->ImportL( data );

    UpdateSecureTime();

    // Determine whether the RO is legal.
    if ( ( ( permission->iAvailableRights & ERightsPlay ) &&
           ( Invalid( *permission->iPlay ) ) ) ||
         ( ( permission->iAvailableRights & ERightsDisplay ) &&
           ( Invalid( *permission->iDisplay ) ) ) ||
         ( ( permission->iAvailableRights & ERightsExecute ) &&
           ( Invalid( *permission->iExecute ) ) ) ||
         ( ( permission->iAvailableRights & ERightsPrint ) &&
           ( Invalid( *permission->iPrint ) ) ) ||
         ( ( permission->iAvailableRights & ERightsTopLevel ) &&
           ( Invalid( *permission->iTopLevel ) ) ) )
        {
        User::Leave( ENoRights );
        }

    size = User::LeaveIfError( IPCGETDESLEN0 );
    CID = HBufC8::NewLC( size );

    size = User::LeaveIfError( IPCGETDESLEN2 );

    // Always reserve enough space for a key, if it's 0 then it is, length will be as well.
    size = size > KDCFKeySize ? size : KDCFKeySize;
    if( size > KSanityDataLengthHigh )
        {
        User::Leave(KErrArgument);
        }
    wrappedcek = HBufC8::NewLC( size );

    data.Set( CID->Des() );
    IPCREAD0L( data );

    data.Set( wrappedcek->Des() );
    IPCREAD2L( data );


    if ( aProtection != ENoProtection )
        {
        if(!iRek.Length())
            {
            User::Leave(KErrNotReady);
            }

        cek = OmaCrypto::AesUnwrapL(iRek, *wrappedcek);
        CleanupStack::PopAndDestroy( wrappedcek );
        CleanupStack::PushL( cek );
        }
    else
        {
        cek = wrappedcek;
        }

    DRMLOG(_L("CEK:"));
    DRMLOGHEX(( *cek ));

    if ( permission->iOriginalInsertTime == Time::NullTTime() )
        {
        permission->iOriginalInsertTime = iTrustedTime;
        }

    DRMDB.AddDBEntryL( *CID, *permission, *cek, UID );

    // Remove a rights object from the xoma list if it is there
    RPointerArray<CDRMXOma>& array = XOMAHEADER;
    TInt i = 0;

    for( ; i < array.Count(); i++ )
        {
        // if the content id is found, remove it from the list
        if( !CID->Compare( array[i]->ContentID() ) )
            {
            delete array[i];
            array.Remove( i );
            break;
            }
        }

    // Write the UID back to the client.
    data.Set( reinterpret_cast< TUint8* >( &UID ), sizeof( UID ),
        sizeof( UID ) );
    IPCWRITE3L( data );

    // Notify clients
    event->SetContentIDL(CID->Des());

    DRMNOTIFIER.SendEventL(*event,status);
    User::WaitForRequest(status);

    CleanupStack::PopAndDestroy( 5 ); // cek, CID, permission, data, event

    aMessage.Complete( KErrNone );

    //__UHEAP_MARKEND;
    DRMLOG( _L( "CDRMDbSession::AddRecordL done" ) );
    }

// -----------------------------------------------------------------------------
// CDRMRightsServer::GetRecordL
// Get record.
// -----------------------------------------------------------------------------
//
void CDRMDbSession::GetRecordL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::GetRecordL" ) );

    CDRMPermission* object = NULL;
    TInt size = 0;
    TPckg< TInt > package( size );
    TDRMUniqueID id;
    HBufC8* CID = NULL;
    TPtr8 data( reinterpret_cast< TUint8* >( &id ), 0, sizeof( id ) );

    // Cleanup.
    if ( iPreparedData )
        {
        delete iPreparedData;
        iPreparedData = NULL;
        }

    IPCREAD2L( data );

    SanitizeL( aMessage.GetDesLength(3) );
    CID = HBufC8::NewLC( IPCGETDESLEN3 );

    data.Set( CID->Des() );
    IPCREAD3L( data );

    object = DRMDB.GetDBEntryByContentIDL( *CID, id );
    CleanupStack::PushL( object );

    // modify the times in the rights object to UI time
    UpdateSecureTime();
    TTime currentTime;
    currentTime.HomeTime();
    TTimeIntervalMicroSeconds change = currentTime.Int64() - iTrustedTime.Int64();
    ModifyRightsObjectByTimeL( object, change, ETrue );

    iPreparedData = object->ExportL();
    size = iPreparedData->Length();

    IPCWRITE0L( package );

    CleanupStack::PopAndDestroy( object );
    CleanupStack::PopAndDestroy( CID );

    aMessage.Complete( KErrNone );

    DRMLOG( _L( "CDRMDbSession::GetRecordL ok" ) );
    }

// -----------------------------------------------------------------------------
// CDRMRightsServer::GetEntryListL
// Create a temporary file from RPointerArray list the database created.
// Return the file name to the client. In case of an error, the temporary file
// is deleted.
// -----------------------------------------------------------------------------
//
void CDRMDbSession::GetEntryListL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::GetEntryListL" ) );
    __UHEAP_MARK;

    TFileName tmpFile( KNullDesC );
    HBufC8* CID = NULL;
    TPtr8 data( NULL, 0 );

    SanitizeL( aMessage.GetDesLength(1));

    CID = HBufC8::NewLC( IPCGETDESLEN1 );


    TDeleteFile dodeletefile =
        {
        &RFSSESSION,
        &tmpFile
        };

    CDRMPermissionList* list = CDRMPermissionList::NewLC();
    list->SetAutoCleanup( ETrue );

    data.Set( CID->Des() );
    IPCREAD1L( data );

    DRMDB.GetDBEntryByContentIDL( *CID, *list );

    // Exclude domain rights where we are not part of the domain
    RemoveInvalidPermissionsL( list );
    if ( list->Count() == 0 )
        {
        User::Leave( KErrCANoRights );
        }

    // modify the times in the rights object to UI time
    UpdateSecureTime();
    TTime currentTime;
    currentTime.HomeTime();
    TTimeIntervalMicroSeconds change = currentTime.Int64() - iTrustedTime.Int64();
    ModifyTimesInListL( list, change, ETrue );

    // Delete the file if something goes wrong.
    TCleanupItem fileCleanup( DeleteFile, &dodeletefile );
    CleanupStack::PushL( fileCleanup );

    // Convert the list to a permanent file store.
    ListToFileL( *list, tmpFile );

    // It is virtually impossible to make WriteL to leave in this case,
    // but anything is still possible...
    IPCWRITE0L( tmpFile );

    CleanupStack::Pop( &dodeletefile );
    CleanupStack::PopAndDestroy( 2 ); // list, CID

    __UHEAP_MARKEND;

    // All done
    aMessage.Complete( KErrNone );

    DRMLOG( _L( "CDRMDbSession::GetEntryListL: ok" ) );
    }

// -----------------------------------------------------------------------------
// CDRMRightsServer::DeleteL
// Delete all rights objects with a certain CID.
// -----------------------------------------------------------------------------
//
void CDRMDbSession::DeleteL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::DeleteL" ) );
    __UHEAP_MARK;
    TPtr8 data( NULL, 0 );

    // Check for VendorId

    _LIT_SECURITY_POLICY_V0(vidCheck, VID_DEFAULT); // Check Default VID
    if ( !vidCheck().CheckPolicy(iClient) )
        {
        User::Leave( KErrAccessDenied );
        }

    CDRMEventAddRemove* event = CDRMEventAddRemove::NewLC( ERightsObjectDeletedAll );
    TRequestStatus status;

    HBufC8* CID = HBufC8::NewLC(
        User::LeaveIfError( IPCGETDESLEN2 ) );

    data.Set( CID->Des() );

    IPCREAD2L( data );


    // Check if deletion is allowed:
    if( DeleteAllowedL( *CID ) )
        {
        DRMDB.DeleteDBEntryL( *CID );

        // Notify
        event->SetContentIDL(CID->Des());

        DRMNOTIFIER.SendEventL(*event,status);
        User::WaitForRequest(status);
        }
    CleanupStack::PopAndDestroy( 2 ); // CID, event
    aMessage.Complete( KErrNone );
    __UHEAP_MARKEND;
    DRMLOG( _L( "CDRMDbSession::DeleteL ok" ) );
    }

// -----------------------------------------------------------------------------
// CDRMRightsServer::DeleteRecordL
// Delete a single RO.
// -----------------------------------------------------------------------------
//
void CDRMDbSession::DeleteRecordL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::DeleteRecordL" ) );
    __UHEAP_MARK;
    TDRMUniqueID id;

    _LIT_SECURITY_POLICY_V0(vidCheck, VID_DEFAULT); // Check Default VID
    if ( !vidCheck().CheckPolicy(iClient) )
        {
        User::Leave( KErrAccessDenied );
        }

    CDRMEventAddRemove* event = CDRMEventAddRemove::NewLC( ERightsObjectDeleted );
    TRequestStatus status;

    TPtr8 data(
        reinterpret_cast< TUint8* >( &id ), 0, sizeof( TDRMUniqueID ) );

    HBufC8* CID = HBufC8::NewLC(
        User::LeaveIfError( IPCGETDESLEN3 ) );

    IPCREAD0L( data );

    data.Set( CID->Des() );
    IPCREAD3L( data );

    // Check if deletion is allowed:
    if( DeleteAllowedL( *CID ) )
        {
        DRMDB.DeleteDBEntryL( *CID, id );

        event->SetContentIDL( CID->Des() );

        DRMNOTIFIER.SendEventL( *event, status );
        User::WaitForRequest( status );
        }

    CleanupStack::PopAndDestroy( 2 ); // CID, event

    aMessage.Complete( KErrNone );
    __UHEAP_MARKEND;
    DRMLOG( _L( "CDRMDbSession::DeleteRecordL ok" ) );
    }

// -----------------------------------------------------------------------------
// CDRMRightsServer::ExportCIDsL
// Create a temporary file for rights database to export the CIDs,
// and write the file name to the client.
// -----------------------------------------------------------------------------
//
void CDRMDbSession::ExportCIDsL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::ExportCIDsL" ) );
    __UHEAP_MARK;

    TFileName fileName( KNullDesC );
    TDeleteFile deletefile =
        {
        &RFSSESSION,
        &fileName
        };

    TCleanupItem item( DeleteFile, &deletefile );
    CleanupStack::PushL( item );

    DRMDB.ExportContentIDListL( fileName );

    IPCWRITE0L( fileName );

    CleanupStack::Pop( &deletefile );

    aMessage.Complete( KErrNone );
    __UHEAP_MARKEND;
    DRMLOG( _L( "CDRMDbSession::ExportCIDsL: ok" ) );
    }

// -----------------------------------------------------------------------------
// CDRMRightsServer::GetKeyL
// Ask for a list of rights object based on the given CID.
// If there is even one rights object which allows the required consuming
// operation, get the key from database updating the RO if necessary.
// -----------------------------------------------------------------------------
//
void CDRMDbSession::GetKeyL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::GetKeyL" ) );
    __UHEAP_MARK;
    // If credentials haven't been checked, don't return the key
    if (iCredentialsChecked == ECheckedAndAllowed)
        {
        if (iCek.Length() > 0)
            {
            aMessage.Write(2, iCek);
            aMessage.Complete(KErrNone);
            }
        else
            {
            aMessage.Complete(KErrCANoRights);
            }
        }
    else
        {
        aMessage.Complete(KErrAccessDenied);
        }
    __UHEAP_MARKEND;
    DRMLOG( _L( "CDRMDbSession::GetKeyL: Ok" ) );
    }

// -----------------------------------------------------------------------------
// CDRMRightsServer::CheckRightsL
// Check if there are sufficient rights at the moment. Nothing is changed from
// the database.
// -----------------------------------------------------------------------------
//
void CDRMDbSession::CheckRightsL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::CheckRightsL" ) );
    TIntent intent;
    TInt length = 0;
    CDRMPermission* child = NULL;
    HBufC8* cid = NULL;
    HBufC8* buffer = NULL;
    TPtr8 tmp( NULL, 0, 0 );
    TInt r = KErrNone;
    TInt size = 0;
    TPckg<TInt> package( size );
    TTime currentTime;
    TTimeIntervalMicroSeconds change;
    TUint32 reason = 0;
    TPckg<TUint32> package2( reason );
    TInt error = KErrNone;
    HBufC8* uri = NULL;

    delete iPreparedData;
    iPreparedData = NULL;
    intent = static_cast<TIntent>( aMessage.Int0() );
    DRMLOG2(_L("Intent: %d"), intent);

    if ( intent == EPlay || intent == EView ||
         intent == EExecute || intent == EPrint || intent == EUnknown )
        {
        length = User::LeaveIfError( aMessage.GetDesLength( 1 ) );
        cid = HBufC8::NewLC( length );
        tmp.Set( cid->Des() );
        aMessage.ReadL( 1, tmp );
        DRMLOG(_L("CID:"));
        DRMLOGHEX(tmp);
        error = FindRightsObject( intent, *cid, child, uri, reason );
        delete uri;
        uri = NULL; // URI not used anywhere

        // if an error occurs we still need to write the information to the client,
        // but then we can leave
        if( error )
            {
            aMessage.WriteL( 3, package2 );
            User::Leave( error );
            }

        CleanupStack::PopAndDestroy( cid );
        CleanupStack::PushL( child );
        }
    else if ( intent == EInstall || intent == EPeek )
        {
        length = User::LeaveIfError( aMessage.GetDesLength( 1 ) );
        cid = HBufC8::NewLC( length );
        tmp.Set( cid->Des() );
        aMessage.ReadL( 1, tmp );
        DRMLOG(_L("CID:"));
        DRMLOGHEX(tmp);

        // If we have the key, install and peek always have full rights
        buffer = DRMDB.GetDecryptionKeyL( *cid );
        CleanupStack::PopAndDestroy( cid );

        if ( buffer )
            {
            delete buffer;
            child = CDRMPermission::NewL();
            CleanupStack::PushL( child );
            child->iAvailableRights = ERightsAll;
            }
        else
            {
            r = KErrCANoRights;
            }
        }
    else if ( ( intent == EPause || intent == EContinue || intent == EStop ) &&
        iConsume )
        {
        // These intents only make sense if we have an ongoing session
        child = CDRMPermission::NewL();
        CleanupStack::PushL( child );
        child->DuplicateL( iConsume->GetChild() );
        }
    else
        {
        r = KErrArgument;
        }

    if ( child )
        {
        UpdateSecureTime();
        currentTime.HomeTime();
        change = currentTime.Int64() - iTrustedTime.Int64();
        ModifyRightsObjectByTimeL( child, change, ETrue );
        iPreparedData = child->ExportL();
        size = iPreparedData->Length();
        aMessage.WriteL( 2, package );
        CleanupStack::PopAndDestroy( child );
        }

    aMessage.Complete( r );

    DRMLOG2( _L( "CDRMDbSession::CheckRightsL: ok (%d)" ), r );
    }

// -----------------------------------------------------------------------------
// CDRMRightsServer::CountL
// Tell the client how many ROs there are.
// -----------------------------------------------------------------------------
//
void CDRMDbSession::CountL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::CountL" ) );
    __UHEAP_MARK;
    TInt count = DRMDB.GetAmountOfRightsObjectsL();

    TPtr8 data( reinterpret_cast< TUint8* >( &count ),
                sizeof( TInt ),
                sizeof( TInt ) );

    IPCWRITE0L( data );

    aMessage.Complete( KErrNone );
    __UHEAP_MARKEND;
    DRMLOG( _L( "CDRMDbSession::CountL: ok" ) );
    }

// -----------------------------------------------------------------------------
// CDRMRightsServer::DeleteAll
// Delete all ROs.
// -----------------------------------------------------------------------------
//
void CDRMDbSession::DeleteAllL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::DeleteAllL" ) );

    User::LeaveIfError( VerifyCredentials(NULL, NULL, EUnknown) );
    if( iCredentialsChecked == ECheckedAndDenied )
        {
        User::Leave( KErrPermissionDenied );
        }

    //__UHEAP_MARK;

    DRMDB.DeleteDBL();

    REPLAYCACHE.Close();

#ifndef RD_MULTIPLE_DRIVE

    #ifdef RD_DRM_METERING
    METERINGDB.Close();
    RFSSESSION.Delete( KMeteringDataBaseFile );
    METERINGDB.InitL( KMeteringDataBaseFile );
    #endif

    RFSSESSION.Delete( KTimedReplayCacheFile );
    RFSSESSION.Delete( KPlainReplayCacheFile );

    REPLAYCACHE.InitL( KTimedReplayCacheFile, KPlainReplayCacheFile );

#else //RD_MULTIPLE_DRIVE

    TFileName tempPath;
    TFileName tempPath2;
    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );

    RFSSESSION.DriveToChar( driveNumber, driveLetter );

    #ifdef RD_DRM_METERING
    tempPath.Format( KMeteringDataBaseFile, (TUint)driveLetter );

    METERINGDB.Close();
    RFSSESSION.Delete( tempPath );
    METERINGDB.InitL( tempPath );
    #endif

    tempPath.Format( KTimedReplayCacheFile, (TUint)driveLetter );
    tempPath2.Format( KPlainReplayCacheFile, (TUint)driveLetter );

    RFSSESSION.Delete( tempPath );
    RFSSESSION.Delete( tempPath2 );

    REPLAYCACHE.InitL( tempPath, tempPath2 );

#endif

    if ( !iRoapClientConnected )
        {
        User::LeaveIfError( iRoapClient.Connect() );
        iRoapClientConnected = ETrue;
        }

    iRoapClient.DeleteAllL();

    aMessage.Complete( KErrNone );
    //__UHEAP_MARKEND;
    DRMLOG( _L( "CDRMDbSession::DeleteAllL ok" ) );
    }

// -----------------------------------------------------------------------------
// CDRMRightsServer::ListToFileL
// Convert the list to a file representation.
// -----------------------------------------------------------------------------
//
void CDRMDbSession::ListToFileL( RPointerArray< CDRMPermission >& aList,
                                 TFileName& aFile )
    {
    __UHEAP_MARK;
    DRMLOG( _L( "CDRMDbSession::ListToFileL" ) );

    TInt count( 0 );


    RFileWriteStream fileStream;

#ifndef RD_MULTIPLE_DRIVE

    User::LeaveIfError(
        fileStream.Temp( RFSSESSION,
                         KDRMDbTempPath,
                         aFile,
                         EFileWrite | EFileStream ) );

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );

    RFSSESSION.DriveToChar( driveNumber, driveLetter );

    TFileName dbTempPath;
    dbTempPath.Format( KDbTempPath, (TUint)driveLetter );

    User::LeaveIfError(
      fileStream.Temp( RFSSESSION,
                       dbTempPath,
                       aFile,
                       EFileWrite | EFileStream ) );

#endif

    CleanupClosePushL( fileStream );

    TInt size( sizeof( TUint32 ) ); // streams store also other info there.

    for( count = 0; count < aList.Count(); count++ )
        {
        size += aList[ count ]->Size();
        }

    // Reset count variable:
    count = 0;

#ifndef RD_MULTIPLE_DRIVE

    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &RFSSESSION,
                                                size,
                                                EDriveC ) )

#else //RD_MULTIPLE_DRIVE

    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &RFSSESSION,
                                                size,
                                                driveNumber ) )

#endif
        {
        DRMLOG( _L( "CDRMDbSession::ListToFileL: KErrDiskFull" ) );
        User::Leave( KErrDiskFull );
        }

    fileStream.WriteInt32L( aList.Count() );

    // Write the whole stuff into the file.
    // URIs are ignored. No use to put them to file (always the same
    // and the client knows it already).
    while( count < aList.Count() )
        {
        aList[ count ]->ExternalizeL( fileStream );
        ++count;
        }

    fileStream.CommitL();

    CleanupStack::PopAndDestroy( &fileStream );

    DRMLOG( _L( "CDRMDbSession::ListToFileL: ok" ) );
    __UHEAP_MARKEND;
    }


// -----------------------------------------------------------------------------
// CDRMRightsServer::FindRightsObjectL
// Try to locate the best RO from the given list which allows consuming of
// content.
// If end time is defined or an interval activated, the smallest endtime
// is always chosen first.
// -----------------------------------------------------------------------------
//
void CDRMDbSession::FindRightsObjectL( TIntent aIntent,
                                       const TDesC8& aURI,
                                       CDRMPermission*& aChild,
                                       HBufC8*& aUsedURI,
                                       TUint32& aReason )
    {
    DRMLOG( _L( "CDRMDbSession::FindRightsObjectL" ) );
    TInt res = KErrNotFound;

    if ( aUsedURI || aChild )
        {
        // Check that return parameters are null initially.
        User::Leave( KErrArgument );
        }

    CDRMPermissionList *perms = CDRMPermissionList::NewLC();
    TTime time;

    perms->SetAutoCleanup( ETrue );
    DRMDB.GetDBEntryByContentIDL( aURI , *perms );

    UpdateSecureTime();

    if ( iSecureTime )
        {
        time = iTrustedTime;
        }
    else
        {
        time = Time::NullTTime();
        }

    if ( aIntent == EPeek || aIntent == EInstall )
        {
        // There is at least one RO available.
        // Use any RO, whether they are valid or not.
        // Pick the first RO in the list.

        aChild = (*perms)[0];
        (*perms).Remove( 0 );

        CleanupStack::PopAndDestroy( perms );
        return;
        }
    else if ( aIntent == EUnknown )
        {
        // Pick one that works, no matter what the intent is
        for ( TInt i = 0; res == KErrNotFound && i < perms->Count(); i++ )
            {
            if ( (*perms)[i]->Valid( time, IMSI, aReason ) )
                {
                res = i;
                }
            }
        if ( res != KErrNotFound )
            {
            aChild = (*perms)[res];
            perms->Remove( res );
            }
        else
            {
            User::Leave( KErrCANoPermission );
            }
        }
    else
        {
        res = FindBestROsL( *perms, aURI, aIntent, aUsedURI, aReason );
        if ( res >= 0 )
            {
            aChild = (*perms)[res];
            perms->Remove( res );
            }
        else
            {
            User::Leave( KErrCANoPermission );
            }
        }
    CleanupStack::PopAndDestroy( perms );
    }


// -----------------------------------------------------------------------------
// CDRMRightsServer::FindRightsObject
// Try to locate the best RO from the given list which allows consuming of
// content.
// If end time is defined or an interval activated, the smallest endtime
// is always chosen first.
// -----------------------------------------------------------------------------
//
TInt CDRMDbSession::FindRightsObject( TIntent aIntent,
                                      const TDesC8& aURI,
                                      CDRMPermission*& aChild,
                                      HBufC8*& aUsedURI,
                                      TUint32& aReason )
    {
    DRMLOG( _L( "CDRMDbSession::FindRightsObject" ) );
    TInt error = KErrNone;

    TRAP( error, FindRightsObjectL( aIntent, aURI, aChild, aUsedURI, aReason ) );
    if ( ( error == KErrCANoRights || error == KErrCANoPermission ) &&
        PendingRights(aURI) )
        {
        error = KErrCAPendingRights;
        }
    return error;
    }

// -----------------------------------------------------------------------------
// CDRMDbSession::CheckConsumeL
// Check if Consume is possible
// -----------------------------------------------------------------------------
//
void CDRMDbSession::CheckConsumeL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::CheckConsumeL" ) );

    __UHEAP_MARK;

    HBufC8* CID = NULL;
    TPtr8 cid( NULL, 0, 0 );
    TInt length = 0;
    TIntent intent;
    TUint32 reason = 0;

    length = User::LeaveIfError( IPCGETDESLEN1 );
    CID = HBufC8::NewLC( length );
    cid.Set( CID->Des() );
    IPCREAD1L( cid );
    intent = static_cast<TIntent>(aMessage.Int0());

    // Check only intents which are actually consuming something. All others
    // always succeed, because CheckConsume is called by default when a file
    // is opened.
    if ( intent == EPlay || intent == EView ||
         intent == EExecute || intent == EPrint || intent == EUnknown )
        {
        // Count constraints are valid for the duration of the
        // session after they have been consumed earlier
        if ( !( iConsume && iConsume->CountConstraintActive() ||
                SERVER->HasActiveCountConstraint( *CID ) ||
                SERVER->IsAccessingUrl( *CID ) != KErrNotFound ) )
            {
            CDRMPermission* child( NULL );
            HBufC8* uri( NULL );
            TInt error( FindRightsObject( intent, *CID, child, uri, reason ) );
            delete child;
            delete uri;
            User::LeaveIfError( error );
            }
        }
/* Enable this code as soon as clients use EUnknown, and it is safe to fail
   when no key is available
    else if ( intent == EPeek || EInstall )
        {
        // We need at least the key for peek and install. This will leave if no
        // key is there.
        HBufC8* key = DRMDB.GetDecryptionKeyL( *CID );
        delete key;
        }
*/

    CleanupStack::PopAndDestroy( CID );
    aMessage.Complete( KErrNone );
    __UHEAP_MARKEND;
    DRMLOG( _L( "CDRMDbSession::CheckConsumeL: ok" ) );
    }

// -----------------------------------------------------------------------------
// CDRMDbSession::ConsumeL
// Consume the right and register the consumption.
// -----------------------------------------------------------------------------
//
void CDRMDbSession::ConsumeL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::ConsumeL" ) );

    const TIntent intent = static_cast<TIntent>(aMessage.Int0());
    HBufC8* CID = NULL;
    TInt length = 0;
    TPtr8 cid( NULL, 0, 0 );

    length = User::LeaveIfError( IPCGETDESLEN1 );

    CID = HBufC8::NewLC( length );

    cid.Set( CID->Des() );
    IPCREAD1L( cid );

    if ( intent != EUnknown )
        {
        if ( intent == EPeek || intent == EInstall )
            {
            User::LeaveIfError( VerifyCredentials( CID, NULL, intent ) );
            }
        else
            {
            if ( intent == EPlay ||
                 intent == EView ||
                 intent == EExecute ||
                 intent == EPrint ||
                 intent == EStop )
                {

                if ( iConsume )
                    {
                    delete iConsume;
                    iConsume = NULL;
                    }
                if ( intent != EStop )
                    {
                    CDRMConsume* consume( NULL );
                    consume = CDRMConsume::NewLC( *this, *CID, NULL );
                    consume->HandleL( intent );
                    CleanupStack::Pop( consume );
                    iConsume = consume;
                    }
                }
            else
                {
                if ( !iConsume )
                    {
                    User::Leave( KErrNotReady );
                    }

                if ( intent == EPause )
                    {
                    iConsume->Pause();
                    }
                else if ( intent == EContinue )
                    {
                    iConsume->ContinueL();
                    }
                else
                    {
                    iConsume->Stop();
                    }
                }
            }
        }
    else
        {
        // None of the other intents are allowed here, fail
        User::Leave( KErrArgument );
        }


    CleanupStack::PopAndDestroy( CID );
    aMessage.Complete( KErrNone );
    DRMLOG( _L( "CDRMDbSession::ConsumeL: ok" ) );
    }

// -----------------------------------------------------------------------------
// CDRMDbSession::CalculatePaddingL
//
// -----------------------------------------------------------------------------
//
void CDRMDbSession::CalculatePaddingL( const RMessage2& aMessage )
    {
    __UHEAP_MARK;
    DRMLOG( _L( "CDRMDbSession::CalculatePaddingL" ) );

    CAESDecryptor* decryptor( NULL );
    TInt i( 0 );

    TBuf8<KDCFKeySize * 2> data;
    TBuf8<KDCFKeySize> iv;
    TBuf8<KDCFKeySize> block;
    TPtr8 tmp( NULL, 0, 0 );

    aMessage.ReadL( 0, data );

    // Check that the data we read is actually of the proper length
    if( data.Length() != KDCFKeySize*2 )
        {
        User::Leave(KErrArgument);
        }

    iv.Copy(data.Left(KDCFKeySize));
    block.Copy(data.Right(KDCFKeySize));

    if( iCek.Length() != KDRMKeyLength )
        {
        User::Leave(KErrNotReady);
        }

    decryptor = CAESDecryptor::NewLC(iCek);
    decryptor->Transform(block);
    CleanupStack::PopAndDestroy( decryptor );

    for (i = 0; i < KDCFKeySize; i++)
        {
        block[i] ^= iv[i];
        }

    if ( block[ KDCFKeySize - 1 ] > 0 && block[ KDCFKeySize - 1 ] <= 16 )
        {
        for ( i = KDCFKeySize - block[ KDCFKeySize - 1 ];
              i < KDCFKeySize - 1;
              ++i )
            {
            if ( block[ i ] != block[ KDCFKeySize - 1 ] )
                {
                User::Leave( KErrCorrupt );
                }
            }

        aMessage.Complete( block[ KDCFKeySize - 1 ] );
        DRMLOG2( _L( "CDRMDbSession::CalculatePaddingL: Padding = %d" ),
                 block[ KDCFKeySize - 1 ] );

        return;
        }

    User::Leave( KErrCorrupt );

__UHEAP_MARKEND;
    }

// -----------------------------------------------------------------------------
// CDRMDbSession::SecureTimeL
// returns the secure time
// -----------------------------------------------------------------------------
//
void CDRMDbSession::SecureTimeL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::SecureTimeL" ) );
    __UHEAP_MARK;
    TPckgBuf< TInt64 > time;
    TPckg< TBool > levelPckg( iSecureTime );

    UpdateSecureTime();

    time() = iTrustedTime.Int64();

    IPCWRITE0L( time );
    IPCWRITE1L( levelPckg );

    aMessage.Complete( KErrNone );
    __UHEAP_MARKEND;
    DRMLOG( _L( "CDRMDbSession::SecureTime: ok" ) );
    }

// -----------------------------------------------------------------------------
// CDRMDbSession::EncryptL
//
// -----------------------------------------------------------------------------
//
void CDRMDbSession::EncryptL(const RMessage2& aMessage)
    {
    TBuf8<KDCFKeySize> iv;
    HBufC8* data = NULL;
    TPtr8 ptr(NULL, 0, 0);
    TBool addPadding;
    __UHEAP_MARK;

    DRMLOG(_L("CDRMDbSession::EncryptL"));

    if( iCek.Length() != KDRMKeyLength )
        {
        User::Leave( KErrNotReady );
        }

    SanitizeL( aMessage.GetDesLength(0) );

    addPadding = aMessage.Int2() != 0 ? ETrue : EFalse;
    aMessage.Read(0, iv);
    data = HBufC8::NewLC(aMessage.GetDesMaxLength(1));
    ptr.Set(data->Des());
    aMessage.Read(1, ptr);

    AesEncryptL( iCek, iv, addPadding, ptr );

    aMessage.Write(1, ptr);
    aMessage.Complete(KErrNone);
    CleanupStack::PopAndDestroy( data );
    __UHEAP_MARKEND;
    DRMLOG(_L("CDRMDbSession::EncryptL: Ok"));
    }

// -----------------------------------------------------------------------------
// CDRMDbSession::DecryptL
// Decrypt data and return it to the caller, using the CEK for this session
// -----------------------------------------------------------------------------
//
void CDRMDbSession::DecryptL(const RMessage2& aMessage)
    {
    DRMLOG(_L("CDRMDbSession::DecryptL"));

    // If credentials haven't been checked, do it now, check for expired rights
    if (iCredentialsChecked == ECheckedAndDenied ||
        iCredentialsChecked == ENotChecked &&
        VerifyCredentials(iContentId, NULL, EUnknown) != KErrNone)
        {
        DRMLOG(_L("Decrypt failed: untrusted client"));
        aMessage.Complete(KErrAccessDenied);
        }
    else if ( iConsume && iConsume->IsExpired() )
        {
        DRMLOG(_L("Decrypt failed: rights expired"));
        aMessage.Complete(KErrCANoPermission);
        }
    else
        {
        if (iCek.Length() > 0)
            {
            TBuf8<KDCFKeySize> iv;
            HBufC8* data = NULL;
            TPtr8 ptr(NULL, 0, 0);
            TBool removePadding;

            removePadding = aMessage.Int2() != 0 ? ETrue : EFalse;
            aMessage.Read(0, iv);

            data = HBufC8::NewMaxLC(aMessage.GetDesMaxLength(1));

            ptr.Set(data->Des());
            aMessage.Read(1, ptr);

            AesDecryptL( iCek,
                         iv,
                         removePadding,
                         ptr );

            aMessage.Write(1, ptr);
            aMessage.Complete(KErrNone);
            CleanupStack::PopAndDestroy( data );
            }
        else
            {
            aMessage.Complete(KErrCANoRights);
            }
        }
    DRMLOG(_L("CDRMDbSession::DecryptL: Ok"));
    }

// -----------------------------------------------------------------------------
// CDRMDbSession::InitializeKeyL
// Initialize the CEK for this session based on the content ID. Also sets the
// content ID for later usage.
// -----------------------------------------------------------------------------
//
void CDRMDbSession::InitializeKeyL( const RMessage2& aMessage )
    {
    TPtr8 tmp(NULL, 0, 0);
    HBufC8* key = NULL;
    TInt error = KErrNone;

    DRMLOG( _L( "CDRMDbSession::InitializeKeyL"));
    if ( iContentId )
        {
        delete iContentId;
        iContentId = NULL;
        }
    iContentId  = HBufC8::NewL( User::LeaveIfError( IPCGETDESLEN0 ) );
    tmp.Set( iContentId->Des() );
    IPCREAD0L( tmp );

    __UHEAP_MARK;
    TRAP( error, key = DRMDB.GetDecryptionKeyL( *iContentId  ) );
    if ( ( error == KErrCANoRights || error == KErrCANoPermission ) &&
        PendingRights( *iContentId , EFalse ) )
        {
        error = KErrCAPendingRights;
        }

    if ( error == KErrNone )
        {
        iCek.Copy( *key );
        delete key;
        aMessage.Complete( KErrNone );
        }
    else
        {
        aMessage.Complete( error );
        }
    __UHEAP_MARKEND;
    DRMLOG2( _L( "CDRMDbSession::InitializeKeyL: %d" ), error );
    }

// -----------------------------------------------------------------------------
// CDRMDbSession::InitializeGroupKeyL
// Initialize the CEK for this session from a given group key
// -----------------------------------------------------------------------------
//
void CDRMDbSession::InitializeGroupKeyL(const RMessage2& aMessage )
    {
    HBufC8* groupId = NULL;
    HBufC8* groupKey = NULL;
    TPtr8 tmp(NULL, 0, 0 );
    HBufC8* key = NULL;
    TRequestStatus status;
    CAESDecryptor* aes = NULL;
    CModeCBCDecryptor* cbc = NULL;

    __UHEAP_MARK;

    SanitizeL( aMessage.GetDesLength(0) );
    SanitizeL( aMessage.GetDesLength(1) );

    DRMLOG(_L("CDRMDbSession::InitializeGroupKeyL"));
    if (aMessage.Int2() == EMethodAES_128_CBC)
        {
        groupId = HBufC8::NewLC(aMessage.GetDesLength(0));
        tmp.Set(groupId->Des());
        aMessage.Read(0, tmp);

        groupKey = HBufC8::NewLC(aMessage.GetDesLength(1));
        tmp.Set(groupKey->Des());
        aMessage.Read(1, tmp);

        key = DRMDB.GetDecryptionKeyL(*groupId);
        CleanupStack::PushL(key);

        if ( key )
            {
            aes = CAESDecryptor::NewLC(*key);
            cbc = CModeCBCDecryptor::NewL(aes, groupKey->Left(KDCFKeySize));
            CleanupStack::Pop( aes );
            iCek.Copy(groupKey->Mid(KDCFKeySize, KDCFKeySize));
            cbc->Transform(iCek);
            delete cbc;
            aMessage.Complete(KErrNone);
            }
        else
            {
            aMessage.Complete(KErrCANoRights);
            }
        CleanupStack::PopAndDestroy(3); // key, groupKey, groupId
        }
    else
        {
        aMessage.Complete(KErrNotSupported);
        }

    __UHEAP_MARKEND;
    DRMLOG(_L("CDRMDbSession::InitializeGroupKeyL: Ok"));
    }

// -----------------------------------------------------------------------------
// CDRMDbSession::GetPreparedDataL
// Returns data which has been allocated before
// -----------------------------------------------------------------------------
//
void CDRMDbSession::GetPreparedDataL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::GetPreparedDataL" ) );

    if( !iPreparedData )
        {
        User::Leave( KErrNotReady );
        }

    if( iPreparedData->Length() > aMessage.GetDesMaxLength(0) )
        {
        User::Leave(KErrArgument);
        }

    IPCWRITE0L( iPreparedData->Des() );

    aMessage.Complete( KErrNone );
    DRMLOG( _L( "CDRMDbSession::GetPreparedDataL: ok" ) );

    // Delete the data
    delete iPreparedData;
    iPreparedData = NULL;
    }

// -----------------------------------------------------------------------------
// CDRMDbSession::IsInCacheL
// Checks whether the given entry is in replay cache or not.
// -----------------------------------------------------------------------------
void CDRMDbSession::IsInCacheL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::IsInCacheL" ) );

    HBufC8* buf = HBufC8::NewLC(
        User::LeaveIfError( IPCGETDESLEN0 ) );

    TPckgBuf< TBool > res;
    TPtr8 data( buf->Des() );

    IPCREAD0L( data );

    if ( aMessage.Ptr1() )
        {
        TPckgBuf< TTime > time;

        IPCREAD1L( time );

        res = REPLAYCACHE.InCacheL( *buf, time() );
        }
    else
        {
        res = REPLAYCACHE.InCacheL( *buf );
        }

    CleanupStack::PopAndDestroy( buf );

    IPCWRITE2L( res );

    aMessage.Complete( KErrNone );

    DRMLOG( _L( "CDRMDbSession::IsInCache ok" ) );
    }

// -----------------------------------------------------------------------------
// CDRMDbSession::AddToCacheL
// Adds an entry to replay cache.
// -----------------------------------------------------------------------------
void CDRMDbSession::AddToCacheL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::AddToCacheL" ) );

    HBufC8* buf = NULL;

    UpdateSecureTime();

    if ( !iSecureTime )
        {
        // DoSomething!
        }

    buf = HBufC8::NewLC(
        User::LeaveIfError( IPCGETDESLEN0 ) );

    TPtr8 data( buf->Des() );

    IPCREAD0L( data );

    if ( aMessage.Ptr1() )
        {
        TPckgBuf< TTime > time;

        IPCREAD1L( time );

        REPLAYCACHE.AddL( *buf, time(), iTrustedTime );
        }

    else
        {
        REPLAYCACHE.AddL( *buf, iTrustedTime );
        }

    CleanupStack::PopAndDestroy( buf );

    aMessage.Complete( KErrNone );

    DRMLOG( _L( "CDRMDbSession::AddToCacheL ok" ) );
    }

// -----------------------------------------------------------------------------
// CDRMDbSession::AddDomainROL
// Add domain ro xml data
// -----------------------------------------------------------------------------
//
void CDRMDbSession::AddDomainROL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::AddDomainROL" ) );
    __UHEAP_MARK;
    HBufC8* RoId = NULL;
    HBufC8* XMLData = NULL;
    TInt size = 0;
    TPtr8 data( NULL, 0 );

    size = User::LeaveIfError( IPCGETDESLEN0 );
    RoId = HBufC8::NewLC( size );

    size = User::LeaveIfError( IPCGETDESLEN1 );
    XMLData = HBufC8::NewLC( size );

    data.Set( RoId->Des() );
    IPCREAD0L( data );

    data.Set( XMLData->Des() );
    IPCREAD1L( data );

    DRMDB.AddDomainROL( *RoId, *XMLData );

    CleanupStack::PopAndDestroy( 2 ); // RoId, XMLData

    aMessage.Complete( KErrNone );
    __UHEAP_MARKEND;
    DRMLOG( _L( "CDRMDbSession::AddDomainROL done" ) );
    };

// -----------------------------------------------------------------------------
// CDRMDbSession::GetDomainROL
// Get domain ro xml data
// -----------------------------------------------------------------------------
//
void CDRMDbSession::GetDomainROL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::GetDomainROL" ) );

    TInt size = 0;
    TPckg< TInt > package( size );
    HBufC8* RoId = NULL;
    TPtr8 data(NULL, 0);

    // Cleanup.
    if ( iPreparedData )
        {
        delete iPreparedData;
        iPreparedData = NULL;
        }

    size = User::LeaveIfError( IPCGETDESLEN1 );

    RoId = HBufC8::NewLC( size );

    data.Set( RoId->Des() );
    IPCREAD1L( data );

    iPreparedData = DRMDB.GetDomainROL( *RoId );

    size = iPreparedData->Length();

    IPCWRITE0L( package );

    CleanupStack::PopAndDestroy( RoId );

    aMessage.Complete( KErrNone );

    DRMLOG( _L( "CDRMDbSession::GetDomainROL ok" ) );
    };

// -----------------------------------------------------------------------------
// CDRMDbSession::DeleteDomainROL
// Delete domain ro xml data
// -----------------------------------------------------------------------------
//
void CDRMDbSession::DeleteDomainROL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::DeleteDomainROL" ) );
    __UHEAP_MARK;
    TPtr8 data( NULL, 0 );
    TInt size = 0;

    size = User::LeaveIfError( IPCGETDESLEN0 );


    HBufC8* RoId = HBufC8::NewLC( size );

    data.Set( RoId->Des() );

    IPCREAD0L( data );

    DRMDB.DeleteDomainROL( *RoId );

    CleanupStack::PopAndDestroy( RoId );

    aMessage.Complete( KErrNone );
    __UHEAP_MARKEND;
    DRMLOG( _L( "CDRMDbSession::DeleteDomainROL ok" ) );
    };


// -----------------------------------------------------------------------------
// CDRMDbSession::GetDomainRosForCidL
// Return the domain ROs for a content ID in a buffer. Each RO is preceded
// by a length field
// -----------------------------------------------------------------------------
//
void CDRMDbSession::GetDomainRosForCidL(
    const RMessage2& aMessage)
    {
    DRMLOG(_L("CDRMDbSession::GetDomainRosForCidL"));
    TInt size(0);
    TInt roSize(0);
    TInt pos = 0;
    TInt indexLatest = -1;
    TTime timeStamp = 0;
    TTime timeLatest = 0;
    TPckg<TInt> pkg(size);
    TPtr8 ptr(NULL, 0);
    TPtrC8 elementPtr(KNullDesC8);
    TPckg<TInt> roSizePkg(roSize);
    HBufC8* element = NULL;
    HBufC8* id(NULL);
    HBufC8* ro(NULL);
    CDRMPointerArray<CDRMPermission>*
        rights( CDRMPointerArray<CDRMPermission>::NewLC() );
    rights->SetAutoCleanup(ETrue);

    delete iPreparedData;
    iPreparedData = NULL;

    SanitizeL( aMessage.GetDesLength(0) );

    id = HBufC8::NewLC(aMessage.GetDesLength(0));
    ptr.Set(id->Des());
    aMessage.Read(0, ptr);

    TRAPD(r, DRMDB.GetDBEntryByContentIDL(*id, *rights));

    if (r == KErrCANoRights || r == KErrCANoPermission )
        {
        r = KErrNone;
        }
    User::LeaveIfError(r);

    for (TInt i(0); i < rights->Count(); i++)
        {
        if ( (*rights)[i]->iDomainID )
            {
            ro = DRMDB.GetDomainROL(*(*rights)[i]->iRoID);
            CleanupStack::PushL(ro);

            elementPtr.Set(ExtractElement(*ro, KTimeStamp, pos));

            /* Check whether the timestamp was found or not and whether it is
               the newest one so far or not */
            if (pos >= 0)
                {
                element = elementPtr.AllocLC();
                timeStamp = Iso8601ToTime(*element);
                CleanupStack::PopAndDestroy(element);

                if (timeStamp > timeLatest)
                    {
                    indexLatest = i;
                    }
                }
            CleanupStack::PopAndDestroy(ro);
            ro = NULL;
            }
        pos = 0;
        }

    /* At least one timestamp was found */
    if (indexLatest >= 0 && indexLatest < rights->Count())
        {
        ro = DRMDB.GetDomainROL(*(*rights)[indexLatest]->iRoID);
        CleanupStack::PushL(ro);

        roSize = ro->Size();
        if ( !iPreparedData )
            {
            iPreparedData = HBufC8::NewL(sizeof (TInt) + roSize);
            }
        else
            {
            iPreparedData = iPreparedData->ReAllocL(
                size + sizeof (TInt) + roSize );
            }
        ptr.Set(iPreparedData->Des());
        ptr.Append(roSizePkg);
        ptr.Append(*ro);
        size += sizeof (TInt) + roSize;
        CleanupStack::PopAndDestroy(ro);
        }

    aMessage.Write(1, pkg);
    CleanupStack::PopAndDestroy( id );
    CleanupStack::PopAndDestroy( rights );

    aMessage.Complete(KErrNone);
    DRMLOG(_L("CDRMDbSession::GetDomainRosForCidL ok"));
    }


//------------------------------------------------------------------------------
// CDRMDbSession::DeleteExpiredPermissionsL
// Delete expired permissions if we have secure time.
//------------------------------------------------------------------------------
void CDRMDbSession::DeleteExpiredPermissionsL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::DeleteExpiredPermissionsL" ) );
    CDRMActiveOperation* active( NULL );
    TTime time;

    User::LeaveIfError( VerifyCredentials(NULL, NULL, EUnknown) );
    if( iCredentialsChecked == ECheckedAndDenied )
        {
        User::Leave( KErrPermissionDenied );
        }

    UpdateSecureTime();

    if ( iSecureTime )
        {
        time = iTrustedTime;
        }
    else
        {
        time = Time::NullTTime();
        }

    if ( iPendingRequest )
        {
        User::Leave( KErrAlreadyExists );
        }

    active = CDRMActiveOperation::NewLC( aMessage, *this,
                                         CDRMActiveOperation::EOperationDeleteExpired );
    active->ActivateL( DRMDB, iTrustedTime );

    iPendingRequest = active;

    CleanupStack::Pop( active );

    DRMLOG( _L( "CDRMDbSession::DeleteExpiredPermissionsL ok" ) );
    }

// -----------------------------------------------------------------------------
// CDRMDbSession::SetEstimatedArrivalL
// set the estimated arrival time for the content uri
// -----------------------------------------------------------------------------
//
void CDRMDbSession::SetEstimatedArrivalL( const RMessage2& aMessage )
    {
    HBufC8* CID = NULL;
    TFileName fileName;
    TInt length = 0;
    TPtr8 inRead( NULL, 0 );
    TTimeIntervalSeconds interval = 0;
    TPckg<TTimeIntervalSeconds> package( interval );
    RPointerArray<CDRMXOma>& array = XOMAHEADER;
    CDRMXOma* omaData = NULL;

    // Read the content id
    length = User::LeaveIfError( IPCGETDESLEN0 );
    CID = HBufC8::NewLC( length );
    inRead.Set( CID->Des() );
    IPCREAD0L( inRead );

    // Read the interval
    IPCREAD1L( package );


    for( TInt i = 0; i < array.Count(); i++ )
        {
        if( !CID->Compare( (array)[i]->ContentID() ) )
            {
            omaData = (array)[i];
            break;
            }
        }

    // Update the secure time
    UpdateSecureTime();

    if( omaData ) // Exists, update
        {
        omaData->SetWaitTimeL( interval );
        omaData->SetTimeStampL( iTrustedTime );
        }
    else // Doesn't exist, create a new one
        {
        omaData = CDRMXOma::NewLC( *CID, iTrustedTime, interval );
        array.AppendL( omaData );
        CleanupStack::Pop( omaData );
        }

    CleanupStack::PopAndDestroy( CID );

    // Complete the command
    aMessage.Complete( KErrNone );
    };

// -----------------------------------------------------------------------------
// CDRMDbSession::GetEstimatedArrivalL
// get the estimated arrival time for the content uri
// -----------------------------------------------------------------------------
//
void CDRMDbSession::GetEstimatedArrivalL( const RMessage2& aMessage )
    {
    HBufC8* CID = NULL;
    TInt length = 0;
    TPtr8 inRead( NULL, 0 );
    RPointerArray<CDRMXOma>& array = XOMAHEADER;
    CDRMXOma* omaData = NULL;
    TInt64 interval = 0;
    TInt i = 0;
    TTimeIntervalSeconds result = 0;

    // Read the content id
    length = User::LeaveIfError( IPCGETDESLEN0 );
    CID = HBufC8::NewLC( length );
    inRead.Set( CID->Des() );
    IPCREAD0L( inRead );

    for( ; i < array.Count(); i++ )
        {
        if( !CID->Compare( (array)[i]->ContentID() ) )
            {
            omaData = (array)[i];
            break;
            }
        }

    // Update the secure time
    UpdateSecureTime();

    if( omaData ) // Exists, update
        {

        // Special case, the rights have expired before, but have not been removed
        // so that we do not lose the notification
        if( omaData->WaitTime().Int() == KErrCAPendingRights )
            {
            interval = -1; // Fixed value for this case:
                           // -1 means the rights should have arrived already

            delete omaData;
            omaData = 0;

            array.Remove( i );
            }
        else
            {
            // Update the secure time
            UpdateSecureTime();

            interval = omaData->WaitTime().Int();
            interval -= ( iTrustedTime.Int64() - omaData->TimeStamp().Int64() ) /
                KMicrosecondsToSecond;

            if( interval <= 0 )
                {
                interval = -1; // Fixed value for this case:
                               // -1 means the rights should have arrived already

                // Clean up the data: if the function fails before this
                // the instance remains like it should
                delete omaData;
                omaData = 0;

                array.Remove( i ); // Remove from the array
                }
            }
        result = I64INT(interval);
        TPckg<TTimeIntervalSeconds> package( result );
        IPCWRITE1L( package );
        }
    else // Doesn't exist, create a new one
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( CID );

    // Complete the command
    aMessage.Complete( KErrNone );
    };



//------------------------------------------------------------------------------
// CDRMDbSession::PendingRights
// Check if rights are pending
//------------------------------------------------------------------------------
TBool CDRMDbSession::PendingRights(const TDesC8& aCid, TBool aRemoval)
    {
    TBool f = EFalse;
    RPointerArray<CDRMXOma>& array = XOMAHEADER;
    CDRMXOma* omaData = NULL;
    TInt64 interval = 0;
    TInt i = 0;
    TTimeIntervalSeconds result = 0;

    for( ; i < array.Count() && !f; i++ )
        {
        if( !aCid.Compare( (array)[i]->ContentID() ) )
            {
            omaData = (array)[i];
            // Exists and is different from KErrCAPendingRights -> update
            if( omaData && omaData->WaitTime().Int() != KErrCAPendingRights )
                {
                // Update the secure time
                UpdateSecureTime();
                interval = omaData->WaitTime().Int();
                interval -= ( iTrustedTime.Int64() - omaData->TimeStamp().Int64() ) /
                    KMicrosecondsToSecond;

                // check if removal from the list is needed
                if( aRemoval && interval <= 0 )
                    {
                    TRAPD( error, omaData->SetWaitTimeL( KErrCAPendingRights ) );
                    error = error; // remove error
                    }
                f = ETrue;
                }
            }
        }
    return f;
    }



//------------------------------------------------------------------------------
// CDRMDbSession::GetUdtDataL
// Gets the user data transfer header data
//------------------------------------------------------------------------------
void CDRMDbSession::GetUdtDataL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::GetUdtDataL" ) );
    __UHEAP_MARK;
    // Usage intent.
    HBufC8* udtData = NULL;

    // Get the key
    udtData = DRMDB.GetUdtDataLC();

    // Write the udt data to the client.
    // This shouldn't fail anyway.
    IPCWRITE0L( *udtData );

    CleanupStack::PopAndDestroy( udtData );

    aMessage.Complete( KErrNone );
    __UHEAP_MARKEND;
    DRMLOG( _L( "CDRMDbSession::GetUdtDataL: Ok" ) );
    };

//------------------------------------------------------------------------------
// CDRMDbSession::InitiateUdtL
// Initiate user data transfer
//------------------------------------------------------------------------------
void CDRMDbSession::InitiateUdtL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::InitiateUdtL" ) );
    __UHEAP_MARK;

    HBufC8* encryptionKey = NULL;
    TInt size = 0;
    TPtr8 data( NULL, 0 );

    SanitizeL( aMessage.GetDesLength(0));

    size = User::LeaveIfError( IPCGETDESLEN0 );
    encryptionKey = HBufC8::NewLC( size );

    data.Set( encryptionKey->Des() );
    IPCREAD0L( data );

    // The encrypted data is encrypted with 2048 byte key, it always needs to be atleast this long
    if( encryptionKey->Length() < 254)
        {
        User::Leave(KErrArgument);
        }

    DRMDB.InitiateUdtL( *encryptionKey );

    CleanupStack::PopAndDestroy( encryptionKey );

    aMessage.Complete( KErrNone );
    __UHEAP_MARKEND;

    DRMLOG( _L( "CDRMDbSession::InitiateUdtL done" ) );
    };

//------------------------------------------------------------------------------
// CDRMDbSession::InitExportOrhanedCIDsL
// Create and export the list of orphaned content id's
//------------------------------------------------------------------------------
void CDRMDbSession::InitExportOrphanedCIDsL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::InitExportOrphanedCIDsL" ) );
    CDRMActiveOperation* active( NULL );
    TBool performScan = EFalse;

    User::LeaveIfError( VerifyCredentials(NULL,NULL,ContentAccess::EUnknown) );

    if ( iPendingRequest )
        {
        User::Leave( KErrAlreadyExists );
        }

    // if the old filename exists, delete it, a new operation starts
    if( iFileName )
        {
        delete iFileName;
        iFileName = NULL;
        }

    // Read the scan data:
    performScan = aMessage.Int0() != 0 ? ETrue : EFalse;


    active = CDRMActiveOperation::NewLC( aMessage, *this,
                                         CDRMActiveOperation::EOperationExportObsolete );

#ifndef RD_MULTIPLE_DRIVE

    active->ActivateL( DRMDB, RFSSESSION, KDRMDbTempPath, performScan );

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );

    RFSSESSION.DriveToChar( driveNumber, driveLetter );

    TFileName dbTempPath;
    dbTempPath.Format( KDbTempPath, (TUint)driveLetter );

    active->ActivateL( DRMDB, RFSSESSION, dbTempPath, performScan );

#endif

    iPendingRequest = active;

    CleanupStack::Pop( active );

    DRMLOG( _L( "CDRMDbSession::InitExportOrphanedCIDsL: ok" ) );
    };


//------------------------------------------------------------------------------
// CDRMDbSession::ExportOrhanedCIDsL
// Create and export the list of orphaned content id's
//------------------------------------------------------------------------------
void CDRMDbSession::ExportOrphanedCIDsL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::ExportOrphanedCIDsL" ) );
    if( !iFileName )
        {
        aMessage.Complete( KErrNotReady );
        return;
        }

    if( iFileName->Length() > aMessage.GetDesMaxLength(0) )
        {
        User::Leave(KErrArgument);
        }

    // It is virtually impossible to make WriteL to leave in this case,
    // but anything is still possible...
    IPCWRITE0L( *iFileName );

    // if everything went well, delete the filename if not leave it
    delete iFileName;
    iFileName = NULL;

    // All done
    aMessage.Complete( KErrNone );

    DRMLOG( _L( "CDRMDbSession::ExportOrphanedCIDsL: ok" ) );
    };

// ----------------------------------------------------------------------------
// CDRMDbSession::UnwrapMacAndRekL
// Unwraps public key encrypted MAC and REK keys
// ----------------------------------------------------------------------------
//
void CDRMDbSession::UnwrapMacAndRekL( const RMessage2& aMessage, TBool aDomainRo )
    {
    DRMLOG( _L( "CDRMDbSession::UnwrapMacAndRekL" ) );

    __UHEAP_MARK;

    MDrmKeyStorage* storage;
    TBuf8<OmaCrypto::KMacSize> mac;
    TBuf8<OmaCrypto::KKeySize> rek;
    HBufC8* data = NULL;
    TPtr8 dataPtr(0, 0);
    TPtrC8 macAndRekPtr(0, 0);
    HBufC8* riId = NULL;
    TPtr8 riIdPtr(0, 0);
    HBufC8* domainId = NULL;
    TPtr8 domainIdPtr(0, 0);
    CDRMRIContext* riContext = NULL;
    TInt size = 0;
    TKeyTransportScheme transport;
    HBufC8* unwrappedMacAndRek = NULL;
    CDRMDomainContext* domainContext;
    HBufC8* domainKey = NULL;

    DRMLOG(_L("CDRMDbSession::UnwrapMacAndRekL"));



    SanitizeL( aMessage.GetDesLength(0) );
    SanitizeL( aMessage.GetDesLength(1) );

    size = User::LeaveIfError( IPCGETDESLEN0 );
    data = HBufC8::NewLC( size );

    size = User::LeaveIfError( IPCGETDESLEN1 );
    riId = HBufC8::NewLC( size );

    if(aDomainRo)
        {
        SanitizeL( aMessage.GetDesLength(2) );
        size = User::LeaveIfError( IPCGETDESLEN2 );
        domainId = HBufC8::NewLC( size );
        }

    dataPtr.Set( data->Des() );
    IPCREAD0L( dataPtr );

    riIdPtr.Set( riId->Des() );
    IPCREAD1L( riIdPtr );

    if(aDomainRo)
        {
        domainIdPtr.Set( domainId->Des() );
        IPCREAD2L( domainIdPtr );
        }

    // The first byte defines the transport scheme
    transport = static_cast<TKeyTransportScheme>(dataPtr[0]);
    macAndRekPtr.Set(dataPtr.Mid(1));

    DRMLOG2(_L("Transport scheme: %d"), transport);
    DRMLOG(_L("MAC + REK:"));
    DRMLOGHEX(macAndRekPtr);

    if ( !iRoapClientConnected )
        {
        User::LeaveIfError( iRoapClient.Connect() );
        iRoapClientConnected = ETrue;
        }

    if (!aDomainRo)
        {
        // get the trusted root from the rights issuer context
        riContext = iRoapClient.GetRIContextL(riIdPtr);
        if ( !riContext )
            {
            DRMLOG(_L("RI not registered"));
            User::Leave(KErrRightsServerRiNotRegistered);
            }
        CleanupStack::PushL(riContext);
        // connect to the storage of our PKI keys
        storage = DrmKeyStorageNewL();
        TCleanupItem storageCleanup( DeleteObject, storage );
        CleanupStack::PushL(storageCleanup);
        storage->SelectTrustedRootL(riContext->SelectedDeviceRoot());

        if (transport == EOma)
            {
            OmaCrypto::RsaKemKwsDecryptL(storage, macAndRekPtr, rek, mac);
            }
        else
            {
            CmlaCrypto::CmlaIpDecryptL(transport, storage, macAndRekPtr, rek, mac);
            }
        CleanupStack::PopAndDestroy(4, data);
        }
    else
        {
        domainContext = iRoapClient.GetDomainContextL(domainIdPtr);
        if ( !domainContext )
            {
            DRMLOG(_L("Domain not registered"));
            User::Leave(KErrRightsServerDomainNotRegistered);
            }
        CleanupStack::PushL(domainContext);

        // last three digits presents the domain generation
        TInt generation = 0;
        TLex8 lex( domainIdPtr.Right(3));
        lex.Val(generation);

        domainKey = domainContext->DomainKeyL(generation);
        CleanupStack::PushL( domainKey );

        // unwrap MAC and REK first with the domain key, the CEK with REK
        unwrappedMacAndRek = OmaCrypto::AesUnwrapL(*domainKey, macAndRekPtr);
        CleanupStack::PushL(unwrappedMacAndRek);

        mac.Copy( unwrappedMacAndRek->Left( OmaCrypto::KKeySize) );
        rek.Copy( unwrappedMacAndRek->Right( OmaCrypto::KKeySize) );
        CleanupStack::PopAndDestroy(6, data);
        }

    DRMLOG(_L("REK:"));
    DRMLOGHEX(rek);
    DRMLOG(_L("MAC:"));
    DRMLOGHEX(mac);

    iMac.Copy(mac);
    iRek.Copy(rek);

    aMessage.Complete( KErrNone );

    __UHEAP_MARKEND;

    DRMLOG( _L( "CDRMDbSession::UnwrapMacAndRekL: ok" ) );
    }

//------------------------------------------------------------------------------
// CDRMDbSession::FindParentsAndRemoveUnusableL()
// Find the best ROs from aList and store references to aBest.
//------------------------------------------------------------------------------
void CDRMDbSession::FindParentsAndRemoveUnusableL( RDRMPermissionList& aList,
                                                   const TDesC8& aURI,
                                                   CDRMPointerArray<HBufC8>& aCids,
                                                   const TIntent aIntent,
                                                   const TTime& aDrmTime,
                                                   TUint32& aReason )
    {

    CDRMPermissionList *permList = NULL;
    TInt error = KErrNone;
    HBufC8* uri = NULL;

    aCids.ResetAndDestroy();


    // to separate parents and children we store the content id, since the content id is needed
    // by drm consume:
    for( TInt count = 0; count < aList.Count(); count++ )
        {
        uri = aURI.AllocLC();
        aCids.AppendL( uri );
        CleanupStack::Pop( uri ); // uri
        }


    // Go though the list and remove the unusable RO's
    for( TInt i = aList.Count() - 1 ; i >= 0; i-- )
        {
        // If the RO has valid parent rights add them to the list
        if( aList[i]->iParentUID )
            {
            permList = CDRMPermissionList::NewLC();
            permList->SetAutoCleanup( ETrue );

            TRAP( error, DRMDB.GetDBEntryByContentIDL( *aList[i]->iParentUID, *permList ));
            if( !error )
                {
                for( TInt j = 0; j < permList->Count(); j++ )
                    {
                    // check if the permission is valid and if the RI ID for child and parent
                    // match
                    if ( aList[i]->iRiId.Compare( (*permList)[j]->iRiId ) == 0 &&
                        IsValidPermissionL( *(*permList)[j], aIntent, aDrmTime, aReason ) )
                        {
                        aList.AppendL( (*permList)[j] );
                        (*permList)[j] = 0;
                        uri = aList[i]->iParentUID->AllocLC();
                        aCids.AppendL( uri );
                        CleanupStack::Pop( uri );
                        }
                    }
                }
            CleanupStack::PopAndDestroy( permList );
            }

        // If the child is not valid remove it from the list
        if ( !IsValidPermissionL( *aList[i], aIntent, aDrmTime, aReason ) )
            {
            CDRMPermission* perm = aList[i];
            delete perm;
            aList.Remove( i );

            uri = aCids[i];
            delete uri;
            aCids.Remove( i );
            perm = NULL;
            uri = NULL;
            }
        }
    // Now the list should contain only valid RO's or parents of the RO's
    // which are valid
    }


//------------------------------------------------------------------------------
// CDRMDbSession::FindBestROsL
// Find the best ROs from aList and store references to aBest.
//------------------------------------------------------------------------------
TInt CDRMDbSession::FindBestROsL(
    RDRMPermissionList& aList,
    const TDesC8& aURI,
    const TIntent aIntent,
    HBufC8*& aUsedURI,
    TUint32& aReason )
    {
    TTime time = Time::NullTTime();
    CDRMPointerArray<HBufC8>* uriList = CDRMPointerArray<HBufC8>::NewLC();
    uriList->SetAutoCleanup(ETrue);

    TInt bestRo = -1;

    if ( iSecureTime )
        {
        time = iTrustedTime;
        }
    else
        {
        time = Time::NullTTime();
        }

    FindParentsAndRemoveUnusableL( aList, aURI, *uriList, aIntent, time, aReason );

    if( aList.Count() == 0 )
        {
        User::Leave(KErrCANoPermission);
        }

    bestRo = GetBestROL( aList, aIntent, aReason );

    // Delete if it already exists
    if( aUsedURI )
        {
        delete aUsedURI;
        }
    aUsedURI = NULL;

    // if it's not the same as the normal URI return a value otherwise NULL
    if( (*uriList)[ bestRo ]->Compare( aURI ) )
        {
        aUsedURI = (*uriList)[ bestRo ]->AllocL();
        }

    CleanupStack::PopAndDestroy( uriList );
    return bestRo;
    }

// -----------------------------------------------------------------------------
// CDRMDbSession::IsValidPermissionL
// ETrue if not expired. If the candidate has expired permission (intent,
// not top level), ETrue is returned if the child knows it is a child.
// -----------------------------------------------------------------------------
TBool CDRMDbSession::IsValidPermissionL(
    CDRMPermission& aPermission,
    const ContentAccess::TIntent aIntent,
    const TTime& aTime,
    TUint32& aReason )
    {
    CDRMConstraint* toplevel = NULL;
    CDRMConstraint* constraint = NULL;
    CDRMDomainContext* domainContext = NULL;
    TBool r = ETrue;
    #ifdef RD_DRM_METERING
    CDRMRIContext* riContext = NULL;
    #endif

    toplevel = aPermission.TopLevelConstraint();
    if ( toplevel && ( !( toplevel->Valid( aTime, IMSI, aReason ) ) ) )
        {
        r = EFalse;
        }
    else
        {
        constraint = aPermission.ConstraintForIntent( aIntent );
        if ( !constraint || constraint &&
            ( !( constraint->Valid( aTime, IMSI, aReason ) ) ) )
            {
            r = EFalse;
            }
        else if ( aPermission.iDomainID )
            {
            if ( !iRoapClientConnected )
                {
                User::LeaveIfError( iRoapClient.Connect() );
                iRoapClientConnected = ETrue;
                }
            domainContext = iRoapClient.GetDomainContextL( *aPermission.iDomainID );
            if ( domainContext )
                {
                delete domainContext;
                }
            else
                {
                r = EFalse;
                }

            }

        // If the constraint is software constrained and the sw secureid does not match
        // remove it from the list handling
        if( r && aPermission.SoftwareConstrained() )
            {
            _LIT_SECURITY_POLICY_S0(swSidCheck, constraint->iSecureId.iUid);
            if (constraint->iActiveConstraints & EConstraintSoftware)
                {
                if (!swSidCheck().CheckPolicy(iClient))
                    {
                    r = EFalse;
                    }
                }
            }
        }

    #ifdef RD_DRM_METERING

    // Check if metering restricts the content usage
    if ( r != EFalse && constraint->iDrmMeteringInfo &&
        !constraint->iDrmMeteringInfo->iAllowUseWithoutMetering )
        {
        if ( !iRoapClientConnected )
            {
            User::LeaveIfError( iRoapClient.Connect() );
            iRoapClientConnected = ETrue;
            }
        riContext = iRoapClient.GetRIContextL( aPermission.iRiId );
        if ( !riContext || !riContext->IsMeteringAllowed() )
            {
            aReason |= EConstraintMetering;
            return EFalse;
            }
        }

    delete riContext;

    #endif

    return r;
    }

//------------------------------------------------------------------------------
// CDRMDbSession::GetBestROL
// Find the best ROs from aList and store references to aBest.
//------------------------------------------------------------------------------
TInt CDRMDbSession::GetBestROL(
    RDRMPermissionList& aList,
    const TIntent aIntent,
    TUint32& aReason )
    {
    TInt count( 0 );
    TTime time;

    CDRMConstraint* normalized = CDRMConstraint::NewLC();
    CDRMConstraint* bestOne = CDRMConstraint::NewLC();

    TInt bestRo( -1 );

    if ( iSecureTime )
        {
        time = iTrustedTime;
        }
    else
        {
        time = Time::NullTTime();
        }

    // 'count' is updated if something useful is found, and aList is
    // updated if the permission cannot be used.
    while  ( count < aList.Count() )
        {
        CDRMPermission* perm = aList[ count ];
        TBool found( EFalse );

        // If there is no constraint for the intent, there is no need to normalize
        // There can be no rights to use in that permission
        if( perm->ConstraintForIntent( aIntent ) )
            {
            Normalize( *perm, *normalized, aIntent );
            if ( normalized->Valid( time, IMSI, aReason ) )
                {
                found = ETrue;
                }
            }

        if ( found )
            {
            // Compare whether "normalized" is more suitable than "bestOne".
            // If this is the first usable child & parent combination,
            // take it.
            if ( ( bestRo < 0 ) || BetterPermission( *normalized, *bestOne ) )
                {
                // "normalized" is the new "bestOne"
                CDRMConstraint* temp = bestOne;
                bestOne = normalized;
                normalized = temp;

                bestRo = count;

                ++count;
                }
            else
                {
                // This isn't any better than the previous ones. Throw it away.
                found = EFalse;
                }
            }

        if ( !found )
            {
            // Unusable child.
            delete perm; perm = NULL;
            aList.Remove( count );
            }
        }

    CleanupStack::PopAndDestroy( 2 ); // bestOne, normalized

    return bestRo;
    }

// -----------------------------------------------------------------------------
// CDRMDbSession::Normalize
// Normalizes a permission.
// -----------------------------------------------------------------------------
void CDRMDbSession::Normalize( CDRMPermission& aPermission,
                               CDRMConstraint& aNormalized,
                               const ContentAccess::TIntent aIntent )
    {
    __ASSERT_DEBUG( aPermission.ConstraintForIntent( aIntent ), User::Invariant() );

    TRAPD( error, aNormalized.DuplicateL( *( aPermission.ConstraintForIntent( aIntent ) ) ) );
    if( !error )
        {
        if ( aPermission.TopLevelConstraint() )
            {
            aNormalized.Merge( *( aPermission.TopLevelConstraint() ) );
            }
        }
    }
// -----------------------------------------------------------------------------
// CDRMDbSession::BetterPermission
// Compares two permissions, and returns ETrue if aNewOne is more suitable
// for the usage. Assumes both are valid, i.e. not expired.
// -----------------------------------------------------------------------------
TBool CDRMDbSession::BetterPermission( const CDRMConstraint& aNewOne,
                                       const CDRMConstraint& aOldOne )
    {
    // Check Order:
    // 1. Full
    // 2. Start End, closest end time first
    // 3. Interval, shortest first
    // 4. Accumulated, shortest first
    // 5. Timed Counter, least counters first, longest time first
    // 6. Counter, least counters first or the first one found

    const TTime nullTime = Time::NullTTime();
    TTime oldTime = nullTime;
    TTime newTime = nullTime;
    TTime oldTimePos = nullTime;
    TTime newTimePos = nullTime;

    // 1. Full
    // If the old or new one is the ultimate one, don't bother to
    // check anything else.
    if ( aOldOne.iActiveConstraints == EConstraintNone )
        {
        return EFalse;
        }

    if ( aNewOne.iActiveConstraints == EConstraintNone )
        {
        return ETrue;
        }

    // 2. Start & End Time
    // Choose the one with the closest end time first
    // All RO's to this check are already checked to be valid
    // ActiveIntervals Also hit this spot

    // First get the start and end times from the intervals if they are active or inactive:
    if ( aOldOne.iActiveConstraints & EConstraintInterval )
        {
        if( aOldOne.iIntervalStart == nullTime )
            {
            oldTimePos = iTrustedTime;
            oldTimePos += TTimeIntervalSeconds( aOldOne.iInterval );
            }
        else
            {
            oldTime = aOldOne.iIntervalStart;
            oldTime += TTimeIntervalSeconds( aOldOne.iInterval );
            }
        }

    if( aNewOne.iActiveConstraints & EConstraintInterval )
        {
        if( aNewOne.iIntervalStart == nullTime )
            {
            newTimePos = iTrustedTime;
            newTimePos += TTimeIntervalSeconds( aNewOne.iInterval );
            }
        else
            {
            newTime = aNewOne.iIntervalStart;
            newTime += TTimeIntervalSeconds( aNewOne.iInterval );
            }
        }

    if ( aOldOne.iActiveConstraints & EConstraintEndTime || oldTime != nullTime )
        {
        oldTime = EndTime( oldTime, aOldOne.iEndTime );

        if ( aNewOne.iActiveConstraints & EConstraintEndTime || newTime != nullTime )
            {
            newTime = EndTime( newTime, aNewOne.iEndTime );

            if( newTime != oldTime )
                {
                return ( newTime < oldTime );
                }
            }
        else
            {
            return EFalse;
            }
        }
    else if ( aNewOne.iActiveConstraints & EConstraintEndTime || newTime != nullTime )
        {
        return ETrue;
        }


    // 3. Inactive Intervals:
    // Choose the one with the interval ending first:
    // Continue here if the no SE's exist or SE's are the same
    if( aOldOne.iActiveConstraints & EConstraintInterval )
        {
        if( aNewOne.iActiveConstraints & EConstraintInterval )
            {
            oldTimePos = EndTime( oldTime, oldTimePos );
            newTimePos = EndTime( newTime, newTimePos );

            if( oldTimePos != newTimePos )
                {
                return ( newTimePos < oldTimePos );
                }
            }
        else
            {
            if( aNewOne.iActiveConstraints & EConstraintAccumulated ||
                aNewOne.iActiveConstraints & EConstraintTimedCounter ||
                aNewOne.iActiveConstraints & EConstraintCounter )
                {
                return EFalse;
                }
            else
                {
                return ETrue;
                }
            }
        }
    else if( aNewOne.iActiveConstraints & EConstraintInterval )
        {
        if( aOldOne.iActiveConstraints & EConstraintAccumulated ||
            aOldOne.iActiveConstraints & EConstraintTimedCounter ||
            aOldOne.iActiveConstraints & EConstraintCounter )
            {
            return ETrue;
            }
        else
            {
            return EFalse;
            }
        }

    // 4. Accumulated:
    // Choose the shortest accumulated first
    // Continue here if SE's or intervals do not exist or they are the same
    if( aOldOne.iActiveConstraints & EConstraintAccumulated )
        {
        if( aNewOne.iActiveConstraints & EConstraintAccumulated )
            {
            if( aNewOne.iAccumulatedTime != aOldOne.iAccumulatedTime )
                {
                return ( aNewOne.iAccumulatedTime < aOldOne.iAccumulatedTime );
                }
            }
        else
            {
            if( aNewOne.iActiveConstraints & EConstraintTimedCounter ||
                aNewOne.iActiveConstraints & EConstraintCounter )
                {
                return EFalse;
                }
            else
                {
                return ETrue;
                }
            }
        }
    else if( aNewOne.iActiveConstraints & EConstraintAccumulated )
        {
        if( aOldOne.iActiveConstraints & EConstraintTimedCounter ||
            aOldOne.iActiveConstraints & EConstraintCounter )
            {
            return ETrue;
            }
        else
            {
            return EFalse;
            }
        }


    // 5. Timed Counter
    // Choose the one with least counters first. If there is an equal number of counters
    // left, use the one with the longest time
    // Continue here if SE's or intervals or accumulateds do not exist or they are the same
    if( aOldOne.iActiveConstraints & EConstraintTimedCounter )
        {
        if( aNewOne.iActiveConstraints & EConstraintTimedCounter )
            {
            if( aNewOne.iTimedCounter == aOldOne.iTimedCounter )
                {
                if( aNewOne.iTimedInterval != aOldOne.iTimedInterval )
                    {
                    return ( aNewOne.iTimedInterval < aOldOne.iTimedInterval );
                    }
                else
                    {
                    if( aNewOne.iActiveConstraints & EConstraintCounter )
                        {

                        }
                    }
                }
            else
                {
                return ( aNewOne.iTimedCounter < aOldOne.iTimedCounter );
                }

            }
        else
            {
            if( aNewOne.iActiveConstraints & EConstraintCounter )
                {
                return EFalse;
                }
            else
                {
                return ETrue;
                }
            }
        }
    else if( aNewOne.iActiveConstraints & EConstraintTimedCounter )
        {
        if( aOldOne.iActiveConstraints & EConstraintCounter )
            {
            return ETrue;
            }
        else
            {
            return EFalse;
            }
        }

    // 6. Counter
    // Choose the one with least counters:
    // if they are the same choose the first one.
    // Continue here if SE's or intervals or accumulateds or timed counters
    // do not exist or they are the same
    if( aOldOne.iActiveConstraints & EConstraintCounter )
        {
        if( aNewOne.iActiveConstraints & EConstraintCounter )
            {
            return ( aNewOne.iCounter < aOldOne.iCounter );
            }
        else
            {
            return ETrue;
            }
        }

    // If all else fails use the old one:
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CDRMDbSession::BetterPermission
// Compares two permissions, and returns ETrue if aNewOne is more suitable
// for the usage. Assumes both are valid, i.e. not expired.
// -----------------------------------------------------------------------------
TBool BetterPermissionOld( const CDRMConstraint& aNewOne,
    const CDRMConstraint& aOldOne ) //const
    {

    const TTime nullTime = Time::NullTTime();
    TTime oldTime = nullTime;
    TTime newTime = nullTime;
    TBool inactiveIntervals = EFalse;

    // If the old or new one is the ultimate one, don't bother to
    // check anything else.
    if ( aOldOne.iActiveConstraints == EConstraintNone )
        {
        return EFalse;
        }

    if ( aNewOne.iActiveConstraints == EConstraintNone )
        {
        return ETrue;
        }

    // If old one has count constraints but the new doesn't, the new
    // is better. If the old one doesn't have counters but the
    // new has, the old one is better.
    if ( aOldOne.iActiveConstraints & EConstraintCounter )
        {
        if ( !( aNewOne.iActiveConstraints & EConstraintCounter ) ||
             ( aNewOne.iCounter < aOldOne.iCounter ) )
            {
            return ETrue;
            }
        }
    else
        {
        if ( aNewOne.iActiveConstraints & EConstraintCounter )
            {
            return EFalse;
            }
        }

    if ( aOldOne.iActiveConstraints & EConstraintTimedCounter )
        {
        if ( !( aNewOne.iActiveConstraints & EConstraintTimedCounter ) ||
             ( aNewOne.iTimedCounter < aOldOne.iTimedCounter ) )
            {
            return ETrue;
            }
        }
    else
        {
        if ( aNewOne.iActiveConstraints & EConstraintTimedCounter )
            {
            return EFalse;
            }
        }

    if ( aOldOne.iActiveConstraints & EConstraintAccumulated )
        {
        if ( !( aNewOne.iActiveConstraints & EConstraintAccumulated ) ||
             ( aNewOne.iAccumulatedTime < aOldOne.iAccumulatedTime ) )
            {
            return ETrue;
            }
        }
    else
        {
        if ( aNewOne.iActiveConstraints & EConstraintAccumulated )
            {
            return EFalse;
            }
        }

    // - No intervals is better than inactive intervals.
    // - No intervals compared to activated interval goes to
    //   end time constraint comparison.
    // - Activated interval is better than inactive interval.
    // - Two inactive intervals go to end time constraint comparison.
    if ( aOldOne.iActiveConstraints & EConstraintInterval )
        {
        if ( aNewOne.iActiveConstraints & EConstraintInterval )
            {
            if ( aOldOne.iIntervalStart == nullTime )
                {
                if ( aNewOne.iIntervalStart != nullTime )
                    {
                    return ETrue;
                    }

                // Both have inactive intervals.
                inactiveIntervals = ETrue;

//                oldTime = iTrustedTime;
                oldTime += TTimeIntervalSeconds( aOldOne.iInterval );

//                newTime = iTrustedTime;
                newTime += TTimeIntervalSeconds( aNewOne.iInterval );
                }
            else
                {
                // Old one has activated interval.
                if ( aNewOne.iIntervalStart == nullTime )
                    {
                    return EFalse;
                    }

                // Both have activated intervals.
                oldTime = aOldOne.iIntervalStart;
                oldTime += TTimeIntervalSeconds( aOldOne.iInterval );

                newTime = aNewOne.iIntervalStart;
                newTime += TTimeIntervalSeconds( aNewOne.iInterval );
                }
            }
        else
            {
            // No intervals in the new one.
            if ( aOldOne.iIntervalStart == nullTime )
                {
                return ETrue;
                }

            oldTime = aOldOne.iIntervalStart;
            oldTime += TTimeIntervalSeconds( aOldOne.iInterval );
            }
        }
    else
        {
        // The old one doesn't have intervals.
        if ( aNewOne.iActiveConstraints & EConstraintInterval )
            {
            if ( aNewOne.iIntervalStart == nullTime )
                {
                return EFalse;
                }

            newTime = aNewOne.iIntervalStart + TTimeIntervalSeconds( aNewOne.iInterval );
            }
        }

    if ( inactiveIntervals )
        {
        // The one with end time goes first.
        if ( aOldOne.iActiveConstraints & EConstraintEndTime )
            {
            if ( aNewOne.iActiveConstraints & EConstraintEndTime )
                {
                oldTime = EndTime( oldTime, aOldOne.iEndTime );
                newTime = EndTime( newTime, aNewOne.iEndTime );

                return ( newTime < oldTime );
                }

            return EFalse;
            }

        if( aNewOne.iActiveConstraints & EConstraintEndTime )
            {
            return ETrue;
            }

        // Both have just inactive intervals.
        return ( newTime < oldTime );
        }

    // Check end times and/or activated intervals.
    if ( aOldOne.iActiveConstraints & EConstraintEndTime || oldTime != nullTime )
        {
        oldTime = EndTime( oldTime, aOldOne.iEndTime );

        if ( aNewOne.iActiveConstraints & EConstraintEndTime || newTime != nullTime )
            {
            newTime = EndTime( newTime, aNewOne.iEndTime );

            return ( newTime < oldTime );
            }

        // The new one doesn't have end time constraint and/or activated intervals.
        return EFalse;
        }

    if ( aNewOne.iActiveConstraints & EConstraintEndTime || newTime != nullTime )
        {
        return ETrue;
        }

    // Start time does not expire, so let's keep the old one.

    return EFalse;
    }


// ----------------------------------------------------------------------------
// UnwrapProtectedCekL
// Unwraps the protected CEK aProtectedCek. Caller owns the returned buffer.
// ----------------------------------------------------------------------------
//
HBufC8* CDRMDbSession::UnwrapProtectedCekL(
    const TDesC8& aProtectedCek )
    {
    MDrmKeyStorage* storage;
    TBuf8<OmaCrypto::KMacSize> mac;
    TBuf8<OmaCrypto::KKeySize> rek;
    HBufC8* cek = NULL;
    TPtrC8 macAndRek(0, 0);
    TPtrC8 wrappedCek(0, 0);
    TKeyTransportScheme transport;
    TBuf8<KRiIdSize> rightsIssuer;
    CDRMRIContext* riContext = NULL;
    TInt i;
    TInt len;

    DRMLOG(_L("CDRMDbSession::UnwrapProtectedCekL"));

    // first element: one byte for the transport scheme
    i = 0;
    transport = static_cast<TKeyTransportScheme>(aProtectedCek[i]);
    i++;

    DRMLOG2(_L("Transport scheme: %d"), transport);

    // second element: the concatenated MAC and REK wrapped with the KEK
    len = aProtectedCek[i];
    macAndRek.Set(aProtectedCek.Mid(i + 1, len));
    i = i + 1 + len;
    DRMLOG(_L("MAC + REK:"));
    DRMLOGHEX(macAndRek);

    // third element: 20 bytes with the rights issuer ID
    len = aProtectedCek[i];
    rightsIssuer.Copy(aProtectedCek.Mid(i + 1, len));
    i = i + 1 + len;
    DRMLOG(_L("RI ID:"));
    DRMLOGHEX(rightsIssuer);

    // get the trusted root from the rights issuer context
    if ( !iRoapClientConnected )
        {
        User::LeaveIfError( iRoapClient.Connect() );
        iRoapClientConnected = ETrue;
        }
    riContext = iRoapClient.GetRIContextL(rightsIssuer);
    if ( !riContext )
        {
        DRMLOG(_L("RI not registered"));
        User::Leave(KErrRightsServerRiNotRegistered);
        }
    CleanupStack::PushL(riContext);

    // connect to the storage of our PKI keys
    storage = DrmKeyStorageNewL();
    TCleanupItem storageCleanup( DeleteObject, storage );
    CleanupStack::PushL(storageCleanup);
    storage->SelectTrustedRootL(riContext->SelectedDeviceRoot());

    if (transport == EOma)
        {
        OmaCrypto::RsaKemKwsDecryptL(storage, macAndRek, rek, mac);
        }
    else
        {
        CmlaCrypto::CmlaIpDecryptL(transport, storage, macAndRek, rek, mac);
        }

    DRMLOG(_L("REK:"));
    DRMLOGHEX(rek);
    DRMLOG(_L("MAC:"));
    DRMLOGHEX(mac);

    iMac.Copy(mac);
    iRek.Copy(rek);

    // fourth element: 24 bytes for the CEK (wrapped with the REK),
    // this can be empty, e.g. for a parent RO (indicated by a zero length)
    len = aProtectedCek[i];
    if (len > 0)
        {
        wrappedCek.Set(aProtectedCek.Mid(i + 1, len));
        DRMLOG(_L("Wrapped CEK:"));
        DRMLOGHEX(wrappedCek);
        cek = OmaCrypto::AesUnwrapL(rek, wrappedCek);
        DRMLOG(_L("CEK:"));
        DRMLOGHEX(( *cek ));
        }
    else
        {
        DRMLOG(_L("No CEK"));
        cek = KNullDesC8().AllocL();
        }

    CleanupStack::PopAndDestroy(2); // riContext, storageCleanup
    return cek;
    }

// ----------------------------------------------------------------------------
// UnwrapDomainCekL
// Unwraps CEK with is wrapped with a domain key. Caller owns the returned
// buffer.
// ----------------------------------------------------------------------------
//
HBufC8* CDRMDbSession::UnwrapDomainCekL(
    const TDesC8& aProtectedCek,
    const TDesC8& aDomainId )
    {
    HBufC8* unwrappedMacAndRek = NULL;
    HBufC8* cek = NULL;
    TPtrC8 macAndRek(0, 0);
    TPtrC8 wrappedCek(0, 0);
    CDRMDomainContext* domainContext;
    HBufC8* domainKey = NULL;
    TInt i;
    TInt len;

    DRMLOG(_L("CDRMDbSession::UnwrapDomainCekL"));

    // first element (one byte for the transport scheme) is ignored
    i = 1;

    // second element: MAC and REK wrapped with the domain key (40 bytes)
    len = aProtectedCek[i];
    macAndRek.Set(aProtectedCek.Mid(i + 1, len));
    i = i + 1 + len;
    DRMLOG(_L("MAC + REK:"));
    DRMLOGHEX(macAndRek);

    // third element: 20 bytes with the rights issuer ID, is ignored
    len = aProtectedCek[i];
    i = i + 1 + len;

    // fourth element: 24 bytes for the CEK (wrapped with the REK)
    len = aProtectedCek[i];
    wrappedCek.Set(aProtectedCek.Mid(i + 1, len));
    DRMLOG(_L("Wrapped CEK:"));
    DRMLOGHEX(wrappedCek);

    // get the domain key from the domain context
    if ( !iRoapClientConnected )
        {
        User::LeaveIfError( iRoapClient.Connect() );
        iRoapClientConnected = ETrue;
        }
    domainContext = iRoapClient.GetDomainContextL(aDomainId);
    if ( !domainContext )
        {
        DRMLOG(_L("Domain not registered"));
        User::Leave(KErrRightsServerDomainNotRegistered);
        }
    CleanupStack::PushL(domainContext);

    // last three digits presents the domain generation
    TInt generation = 0;
    TLex8 lex( aDomainId.Right(3));
    lex.Val(generation);

    domainKey = domainContext->DomainKeyL(generation);
    CleanupStack::PushL( domainKey );

    // unwrap MAC and REK first with the domain key, the CEK with REK
    unwrappedMacAndRek = OmaCrypto::AesUnwrapL(*domainKey, macAndRek);
    CleanupStack::PushL(unwrappedMacAndRek);
    cek = OmaCrypto::AesUnwrapL(unwrappedMacAndRek->Right(
            OmaCrypto::KKeySize), wrappedCek);
    iMac.Copy( unwrappedMacAndRek->Left( OmaCrypto::KKeySize) );
    CleanupStack::PopAndDestroy(3); // unwrappedMacAndRek, domainKey, domainContext

    return cek;
    }

// ----------------------------------------------------------------------------
// CDRMDbSession::VerifyCredentials
// Check if the client has enough credentials to grant access to later
// decryption.
// ----------------------------------------------------------------------------
//
TInt CDRMDbSession::VerifyCredentials(
    HBufC8* aContentId,
    CDRMPermission* aPermission,
    TIntent aIntent)
    {
    CDRMConstraint* constraint = NULL;
    _LIT_SECURITY_POLICY_C1(drmCheck, ECapabilityDRM);
    _LIT_SECURITY_POLICY_V0(vidCheck, VID_DEFAULT); // Check Default VID
    RPointerArray<CDRMPermission> permissions;
    TInt r = KErrAccessDenied;
    TBool hasOma2Permissions = EFalse;
    TBool hasSoftwareConstraints = EFalse;
    TInt i;

    __UHEAP_MARK;
    // Get the applicable permission, and check if there are OMA 2 permissions or
    // software constraints
    if (aPermission)
        {
        constraint = aPermission->ConstraintForIntent(aIntent);
        }
    if ( aContentId )
        {
        TRAP_IGNORE( DRMDB.GetDBEntryByContentIDL(*aContentId, permissions) );
        for (i = 0; i < permissions.Count(); i++)
            {
            if (permissions[i]->iRightsObjectVersion.iVersionMain == EOma2Rights ||
                permissions[i]->iRightsObjectVersion.iVersionMain == ECmlaRights)
                {
                hasOma2Permissions = ETrue;
                }
            if (permissions[i]->SoftwareConstrained())
                {
                hasSoftwareConstraints = ETrue;
                }
            }
        permissions.ResetAndDestroy();
        permissions.Close();
        }

    // First, check for DRM capability, access granted only if there are no software
    // constraints.
    if (drmCheck().CheckPolicy(iClient) && !hasSoftwareConstraints)
        {
        r = KErrNone;
        }

    // If a permission is given, check for license manager cases as well as
    // OMA DRM 1.0 case
    if ( r != KErrNone && aPermission )
        {
        // Fallback: Check if the vendor ID is the default vendor ID,
        // allow access for OMA DRM 1.0 rights
        if ((aPermission->iRightsObjectVersion.iVersionMain == EOma1Rights &&
             vidCheck().CheckPolicy(iClient)))
            {
            r = KErrNone;
            }

        // Check the software constraint, this can override the fallback above!
        if ( constraint )
            {
            _LIT_SECURITY_POLICY_S0(swSidCheck, constraint->iSecureId.iUid);
            _LIT_SECURITY_POLICY_V0(swVidCheck, constraint->iVendorId.iUid);
            if (constraint->iActiveConstraints & EConstraintVendor)
                {
                if (swVidCheck().CheckPolicy(iClient))
                    {
                    r = KErrNone;
                    }
                else
                    {
                    r = KErrAccessDenied;
                    }
                }
            if (constraint->iActiveConstraints & EConstraintSoftware)
                {
                if (swSidCheck().CheckPolicy(iClient))
                    {
                    r = KErrNone;
                    }
                else
                    {
                    r = KErrAccessDenied;
                    }
                }
            }

        }

    // Application installer needs to be able to use the content even if it has SW constraints
    _LIT_SECURITY_POLICY_S0(swSidCheck2, KAppInstSrv);

    if( swSidCheck2().CheckPolicy(iClient) && hasSoftwareConstraints )
        {
        r = KErrNone;
        }


    // Check if access can be granted when only
    // OMA DRM 1.0 permissions are currently available, and the client
    // has the default vendor ID
    if ( r != KErrNone && !hasOma2Permissions &&
        !hasSoftwareConstraints && vidCheck().CheckPolicy( iClient ) )
        {
        r = KErrNone;
        }
    __UHEAP_MARKEND;

    if ( r == KErrNone )
        {
        iCredentialsChecked = ECheckedAndAllowed;
        }
    else
        {
        iCredentialsChecked = ECheckedAndDenied;
        }
    return r;
    }

// ----------------------------------------------------------------------------
// CDRMDbSession::RemoveInvalidPermissionsL
// Remove all permissions where the domain context is not available
// ----------------------------------------------------------------------------
//
void CDRMDbSession::RemoveInvalidPermissionsL(
    CDRMPermissionList* aList )
    {
    CDRMDomainContext* domainContext = NULL;
    TInt i;
    if ( !iRoapClientConnected )
        {
        User::LeaveIfError( iRoapClient.Connect() );
        iRoapClientConnected = ETrue;
        }
    for ( i = aList->Count() - 1; i >= 0; i-- )
        {
        if ( (*aList)[i]->iDomainID )
            {
            domainContext = iRoapClient.GetDomainContextL(
                *(*aList)[i]->iDomainID );
            if ( domainContext )
                {
                delete domainContext;
                }
            else
                {
                aList->Remove( i );
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// CDRMDbSession::
//
// ----------------------------------------------------------------------------
//
void CDRMDbSession::SetNameL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::SetNameL" ) );
    __UHEAP_MARK;

    SanitizeL( aMessage.GetDesLength( 0 ) );
    SanitizeL( aMessage.GetDesLength( 1 ) );

    TInt namelength = User::LeaveIfError( IPCGETDESLEN1 );
    HBufC8* cid = HBufC8::NewLC( IPCGETDESLEN0 );
    HBufC* name = HBufC::NewLC( namelength > 0 ? namelength : 1 );
    TPtr8 tmp( cid->Des() );

    IPCREAD0L( tmp );
    if ( namelength )
        {
        TPtr tmp2( name->Des() );
        IPCREAD1L( tmp2 );
        }

    DRMDB.NameContentL( *cid, *name );

    CleanupStack::PopAndDestroy( 2 );
    __UHEAP_MARKEND;
    aMessage.Complete( KErrNone );
    }

// ----------------------------------------------------------------------------
// CDRMDbSession::
//
// ----------------------------------------------------------------------------
//
void CDRMDbSession::GetNameL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::GetNameL" ) );
    HBufC8* cid = NULL;
    TPckgBuf< TInt > size( 0 );

    // Cleanup.
    if ( iWidePreparedData )
        {
        delete iWidePreparedData;
        iWidePreparedData = NULL;
        }

    SanitizeL( aMessage.GetDesLength( 0 ) );
    cid = HBufC8::NewLC( IPCGETDESLEN0 );
    TPtr8 tmp( cid->Des() );
    IPCREAD0L( tmp );

    iWidePreparedData = DRMDB.ContentNameLC( *cid );

    CleanupStack::Pop( iWidePreparedData );
    CleanupStack::PopAndDestroy( cid );

    size() = iWidePreparedData->Length();
    if ( iWidePreparedData->Length() == 0 )
        {
        // Empty name --> not asked.
        delete iWidePreparedData;
        iWidePreparedData = NULL;
        }

    IPCWRITE1L( size );

    aMessage.Complete( KErrNone );
    }

// ----------------------------------------------------------------------------
// CDRMDbSession::
//
// ----------------------------------------------------------------------------
//
void CDRMDbSession::GetWideDataL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSesion::GetWideDataL" ) );
    if ( iWidePreparedData )
        {
        if( iWidePreparedData->Length() > aMessage.GetDesMaxLength(0) )
            {
            User::Leave(KErrArgument);
            }

        IPCWRITE0L( *iWidePreparedData );

        delete iWidePreparedData;
        iWidePreparedData = NULL;

        aMessage.Complete( KErrNone );
        return;
        }

    aMessage.Complete( KErrNotReady );
    }

// ----------------------------------------------------------------------------
// CDRMDbSession::
//
// ----------------------------------------------------------------------------
//
void CDRMDbSession::Cancel( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::Cancel" ) );
    if ( iPendingRequest )
        {
        static_cast<CDRMActiveOperation*>(iPendingRequest)->Remove();
        }

    aMessage.Complete( KErrNone );
    }

// ----------------------------------------------------------------------------
// CDRMDbSession::
//
// ----------------------------------------------------------------------------
//
void CDRMDbSession::GetFLUriL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::GetFLUriL" ) );

    TBuf8< KMaxOmaV1CIDLength > URI;

    GetFlURI( URI );

    if( URI.Length() > aMessage.GetDesMaxLength(0) )
        {
        User::Leave(KErrArgument);
        }

    IPCWRITE0L( URI );
    aMessage.Complete( KErrNone );
    }

// ----------------------------------------------------------------------------
// CDRMDbSession::
//
// ----------------------------------------------------------------------------
//
void CDRMDbSession::EncodeRightsIssuerL( const RMessage2& aMessage )
    {
    __UHEAP_MARK;

    DRMLOG( _L( "CRMDbSession::EncodeRightsIssuerL" ) );
    TBuf8< KDCFKeySize > iv;
    HBufC8* data( NULL );
    HBufC8* tmp( NULL );
    iv.SetLength( KDCFKeySize );

    TInt size( User::LeaveIfError( IPCGETDESLEN0 ) );
    TPtr8 des( NULL, 0, 0 );

    if ( size < 1 )
        {
        User::Leave( KErrArgument );
        }

    // Calculate the size. It could be retrieved also from GetDesMaxLength,
    // but at least now we can be sure we won't panic in case of
    // descriptor overflow.
    size += KDCFKeySize - ( size % KDCFKeySize ); //padding
    size += KDCFKeySize; //iv
    size = ( size + 2 ) / 3 * 4; //base64
    size += IMEI.Length();
    size += KFLPrefixLength;

    data = HBufC8::NewLC( size < KMaxOmaV1CIDLength ? KMaxOmaV1CIDLength : size );
    des.Set( data->Des() );

    GetFlURI( des );

    tmp = DRMDB.GetDecryptionKeyL( *data );
    CleanupStack::PushL( tmp );

    // Get the data from client.
    IPCREAD0L( des );
    GenerateIVL( iv );

    // Fits ok.
    AesEncryptL( *tmp, iv, ETrue, des );

    CleanupStack::PopAndDestroy( tmp );

    // Fits ok.
    des.Insert( 0, iv );

    tmp = Base64EncodeL( *data );

    __ASSERT_DEBUG( tmp->Length() + KFLPrefixLength + IMEI.Length() <= size,
        User::Invariant() );

    // Overwrite the original data.
    des = KFLPrefix;
    des.Append( IMEI );
    des.Append( *tmp );

    delete tmp;
    tmp = NULL;

    IPCWRITE0L( *data );

    CleanupStack::PopAndDestroy( data );

    aMessage.Complete( KErrNone );

    __UHEAP_MARKEND;
    }


// ----------------------------------------------------------------------------
// CDRMDbSession::
//
// ----------------------------------------------------------------------------
//
void CDRMDbSession::DecodeRightsIssuerL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::DecodeRightsIssuerL" ) );
    __UHEAP_MARK;

    HBufC8* data( NULL );
    TInt size( IPCGETDESLEN0 );
    TPtr8 des( NULL, 0, 0 );

    if ( !size )
        {
        User::Leave( KErrArgument );
        }

    SanitizeL( size );

    data = HBufC8::NewLC( size );
    des.Set( data->Des() );

    IPCREAD0L( des );

    if ( des.Left( KFLPrefixLength ).Compare( KFLPrefix ) == 0 )
        {
        HBufC8* tmp( NULL );
        HBufC8* tmp2( NULL );
        TPtr8 des2( NULL, 0, 0 );

        tmp = CnvUtfConverter::ConvertFromUnicodeToUtf8L( IMEI );
        CleanupStack::PushL( tmp );

        if ( des.Mid( KFLPrefixLength, IMEI.Length() ).Compare( *tmp ) )
            {
            User::Leave( KErrCANoPermission );
            }

        tmp2 = Base64DecodeL( des.Mid( KFLPrefixLength + tmp->Length() ) );

        *data = *tmp2;

        delete tmp2;
        tmp2 = NULL;

        CleanupStack::PopAndDestroy( tmp );

        if ( ( data->Length() % KDCFKeySize ) ||
             data->Length() < ( KDCFKeySize << 1 ) )
            {
            User::Leave( KErrArgument );
            }

        tmp = HBufC8::NewLC( KMaxOmaV1CIDLength );

        des.Set( tmp->Des() );

        GetFlURI( des );

        tmp2 = DRMDB.GetDecryptionKeyL( *tmp );

        CleanupStack::PopAndDestroy( tmp );
        CleanupStack::PushL( tmp2 );

        des2.Set( const_cast< TText8* >( data->Ptr() ),
            KDCFKeySize,
            KDCFKeySize );

        // AesDecrypt overwrites the data.
        des.Set( const_cast< TText8* >( data->Ptr() ) + KDCFKeySize,
            data->Length() - KDCFKeySize,
            data->Length() - KDCFKeySize );

        AesDecryptL( *tmp2, des2, ETrue, des );

        // Erase the key.
        des2.Set( tmp2->Des() );
        des2.FillZ();

        CleanupStack::PopAndDestroy( tmp2 );

        if( des.Length() > aMessage.GetDesMaxLength(1) )
            {
            User::Leave(KErrArgument);
            }
        IPCWRITE1L( des );
        }

    // Something else, can't handle it. Leave it as it is.
    CleanupStack::PopAndDestroy( data );

    aMessage.Complete( KErrNone );

    __UHEAP_MARKEND;

    DRMLOG( _L( "CDRMDbSession::DecodeRightsIssuerL ok" ) );
    }

// ----------------------------------------------------------------------------
// CDRMDbSession::
//
// ----------------------------------------------------------------------------
//
void CDRMDbSession::GetFlURI( TDes8& aURI )
    {
    aURI = KFLLongPrefix;
    aURI.Append( IMEI );
    aURI.Append( KFLSuffix );
    }

// ----------------------------------------------------------------------------
// CDRMDbSession::AesEncryptL
// Encrypt data using a given key
// ----------------------------------------------------------------------------
//
void CDRMDbSession::AesEncryptL( const TDesC8& aKey,
                                 const TDesC8& aIV,
                                 const TBool aAddPadding,
                                 TDes8& aData )
    {
    DRMLOG( _L( "CDRMDbSession::AesEncryptL" ) );

    __UHEAP_MARK;

    CModeCBCEncryptor* cbc( NULL );
    TInt lastBlockStart( 0 );
    TPtr8 data( NULL, 0, 0 );

    if( aIV.Length() % KDCFKeySize ||
        aKey.Length() % KDCFKeySize )
        {
        User::Leave( KErrArgument );
        }

    cbc = CModeCBCEncryptor::NewL( CAESEncryptor::NewLC( aKey ), aIV );
    CleanupStack::Pop(); // CAESEncryptor, owned by cbc.
    CleanupStack::PushL( cbc );

    lastBlockStart = aData.Length() - ( aData.Length() % KDCFKeySize );
    for ( TInt i = 0; i  < lastBlockStart; i+= KDCFKeySize )
        {
        data.Set( aData.MidTPtr( i, KDCFKeySize ) );

        cbc->Transform( data );
        }

    if ( aAddPadding )
        {
        TInt dataLength = aData.Length();
        TUint8 padding( static_cast< TUint8 >
            ( lastBlockStart + KDCFKeySize - dataLength ) );

        __ASSERT_DEBUG( lastBlockStart + KDCFKeySize - dataLength <= KDCFKeySize,
                        User::Invariant() );

        aData.SetLength( lastBlockStart + KDCFKeySize );

        for ( TInt i = dataLength; i < aData.Length(); ++i )
            {
            aData[ i ] = padding;
            }

        data.Set( aData.MidTPtr( lastBlockStart, KDCFKeySize ) );
        cbc->Transform( data );
        }

    CleanupStack::PopAndDestroy( cbc );

    __UHEAP_MARKEND;

    DRMLOG( _L( "CDRMDbSession::AesEncryptL ok" ) );
    }

// ----------------------------------------------------------------------------
// CDRMDbSession::AesDecryptL
// Decrypt data using a given key
// ----------------------------------------------------------------------------
//
void CDRMDbSession::AesDecryptL( const TDesC8& aKey,
                                 const TDesC8& aIV,
                                 const TBool aRemovePadding,
                                 TDes8& aData )
    {
    DRMLOG( _L( "CDRMDbSession::AesDecryptL" ) );

    __UHEAP_MARK;

    CModeCBCDecryptor* cbc( NULL );

    cbc = CModeCBCDecryptor::NewL(
        CAESDecryptor::NewLC( aKey ), aIV );

    CleanupStack::Pop(); // CAESDecryptor

    __ASSERT_DEBUG( ( aData.Length() >= 0 ) &&
                    ( aData.Length() % KDCFKeySize == 0 ),
                    User::Invariant() );

    for ( TInt count = 0; count < aData.Length(); count += KDCFKeySize )
        {
        TPtr8 d( aData.MidTPtr( count, KDCFKeySize ) );

        cbc->Transform( d );
        }

    delete cbc; cbc = NULL;

    if ( aRemovePadding )
        {
        TInt count( aData.Length() );
        TUint8 c( aData[ count - 1 ] );

        if ( c > KDCFKeySize )
            {
            User::Leave( KErrCorrupt );
            }

        aData.SetLength( count - c );
        }

    __UHEAP_MARKEND;

    DRMLOG( _L( "CDRMDbSession::AesDecryptL ok" ) );
    }

// ----------------------------------------------------------------------------
// CDRMDbSession::
//
// ----------------------------------------------------------------------------
//
void CDRMDbSession::GenerateIVL( TDes8& aIV )
    {
    DRMLOG( _L( "CDRMDbSession::GenerateIVL" ) );
    __UHEAP_MARK;

    __ASSERT_DEBUG( aIV.MaxSize() >= KDCFKeySize, User::Invariant() );
    MDrmKeyStorage* storage = DrmKeyStorageNewL();
    TCleanupItem storageCleanup( DeleteObject, storage );
    CleanupStack::PushL(storageCleanup);

    storage->RandomDataGetL(aIV,KDCFKeySize);
    CleanupStack::PopAndDestroy( storage );

    DRMLOG(_L("random aIV:"));
    DRMLOGHEX(aIV);


    __UHEAP_MARKEND;
    DRMLOG( _L( "CDRMDbSession::GenerateIVL ok" ) );
    }

// ----------------------------------------------------------------------------
// CDRMDbSession::
//
// ----------------------------------------------------------------------------
//
void CDRMDbSession::AsyncOperationDone()
    {
    DRMLOG( _L( "CDRMDbSession::AsyncOperationDone" ) );
    // All done.
    delete iPendingRequest;
    iPendingRequest = NULL;
    }

// ----------------------------------------------------------------------------
// CDRMDbSession::
//
// ----------------------------------------------------------------------------
//
void CDRMDbSession::AsyncOperationDone( TFileName* aFileName )
    {
    DRMLOG( _L( "CDRMDbSession::AsyncOperationDone" ) );
    // All done.
    delete iPendingRequest;
    iPendingRequest = NULL;

    iFileName = aFileName;
    }


// ----------------------------------------------------------------------------
// CDRMDbSession::SetAuthenticationSeedL
// Sets the authentication seed for a content ID. Requires that the REK and KEK
// have been set during a previous AddRecord operation
// ----------------------------------------------------------------------------
//
void CDRMDbSession::SetAuthenticationSeedL( const RMessage2& aMessage )
    {
    DRMLOG(_L("CDRMDbSession::SetAuthenticationSeedL"));

    __UHEAP_MARK;
    SanitizeL( aMessage.GetDesLength(0));
    SanitizeL( aMessage.GetDesLength(1));

    HBufC8* cid = HBufC8::NewLC(IPCGETDESLEN0);
    HBufC8* wrappedSeed = HBufC8::NewLC(IPCGETDESLEN1);
    HBufC8* seed = NULL;

    if (iRek.Length() == 0)
        {
        User::Leave(KErrNotReady);
        }

    TPtr8 tmp(cid->Des());
    IPCREAD0L(tmp );
    TPtr8 tmp2(wrappedSeed->Des());
    IPCREAD1L(tmp2);

    seed = OmaCrypto::AesUnwrapL(iRek, *wrappedSeed);
    CleanupStack::PushL(seed);
    DRMDB.SetAuthenticationSeedL(*cid, *seed);
    CleanupStack::PopAndDestroy(3); // seed, wrappedSeed, cid
    __UHEAP_MARKEND;
    aMessage.Complete(KErrNone);
    }

// ----------------------------------------------------------------------------
// CDRMDbSession::GetAuthenticationSeedL
// Returns the authentication seed for a content ID
// ----------------------------------------------------------------------------
//
void CDRMDbSession::GetAuthenticationSeedL( const RMessage2& aMessage )
    {
    DRMLOG(_L("CDRMDbSession::GetAuthenticationSeedL"));

    __UHEAP_MARK;
    HBufC8* cid = NULL;
    HBufC8* seed = NULL;

    SanitizeL( aMessage.GetDesLength(0));
    cid = HBufC8::NewLC(IPCGETDESLEN0);

    TPtr8 tmp(cid->Des());
    IPCREAD0L(tmp);
    User::LeaveIfError(VerifyCredentials(cid, NULL, EUnknown));
    seed = DRMDB.GetAuthenticationSeedL(*cid);
    CleanupStack::PushL(seed);

    if( seed->Length() > aMessage.GetDesMaxLength(1) )
        {
        User::Leave(KErrArgument);
        }

    IPCWRITE1L(*seed);
    CleanupStack::PopAndDestroy(2); // seed, cid
    __UHEAP_MARKEND;
    aMessage.Complete(KErrNone);
    }

// ----------------------------------------------------------------------------
// CDRMDbSession::VerifyMacL
// Integrity protection for protecets ROs
// ----------------------------------------------------------------------------
//
void CDRMDbSession::VerifyMacL( const RMessage2& aMessage )
    {
    DRMLOG(_L("CDRMDbSession::VerifyMacL"));

    if( !iMac.Length() )
        {
        User::Leave(KErrNotReady);
        }

    __UHEAP_MARK;

    CHMAC* hMac = NULL;
    CSHA1* sha = NULL;
    TPtrC8 hmac_value( KNullDesC8 );
    TPtrC8 sha1_value( KNullDesC8 );
    HBufC8* signedInfo = NULL;
    HBufC8* macValue = NULL;
    TInt ret = KErrNone;

    signedInfo = HBufC8::NewLC(IPCGETDESLEN0);
    TPtr8 signedInfoPtr(signedInfo->Des());
    macValue = HBufC8::NewLC(IPCGETDESLEN1);
    TPtr8 macValuePtr(macValue->Des());

    IPCREAD0L(signedInfoPtr);
    IPCREAD1L(macValuePtr);

    DRMLOG(_L("Signed info:"));
    DRMLOGHEX(signedInfoPtr);
    DRMLOG(_L("MAC value:"));
    DRMLOGHEX(macValuePtr);

    sha = CSHA1::NewL();
    CleanupStack::PushL( sha );
    hMac = CHMAC::NewL( iMac, sha );
    CleanupStack::Pop( sha ); // sha is now owned by hMac
    CleanupStack::PushL( hMac );
    hMac->Update( signedInfoPtr );
    hmac_value.Set( hMac->Final() );

    DRMLOG(_L("Calculated MAC value:"));
    DRMLOGHEX(hmac_value);
    if ( hmac_value.Compare( macValuePtr ) != 0 )
        {
        // MAC validation failed
        ret = KErrRightsServerMacFailed;
        }

    CleanupStack::PopAndDestroy(3, signedInfo); // hMac, macValue, signedInfo
    __UHEAP_MARKEND;
    aMessage.Complete(ret);
    }

// ----------------------------------------------------------------------------
// CDRMDbSession::GetSupportedIndividualsL
// ----------------------------------------------------------------------------
//
void CDRMDbSession::GetSupportedIndividualsL( const RMessage2& aMessage )
    {
    DRMLOG( _L( "CDRMDbSession::GetSupportedIndividualsL" ) );
    TPckgBuf< TInt > size( 0 );
    TInt stringSize = 0;
    TInt offset = 0;
    TUint8* ptr = 0;

    // Cleanup.
    if ( iPreparedData )
        {
        delete iPreparedData;
        iPreparedData = NULL;
        }

    for ( TInt i = 0; i < IMSI.Count(); i++ )
        {
        stringSize += sizeof(TInt);
        stringSize += IMSI[i]->Size();
        }


    // If it's empty, just return right away
    if( !stringSize )
        {
        IPCWRITE0L( size );
        aMessage.Complete(KErrNone);
        return;
        }

    // Otherwise create a buffer and fill it:
    iPreparedData = HBufC8::NewMaxL( stringSize );
    ptr = const_cast<TUint8*>( iPreparedData->Ptr() );

    size() = stringSize;

    for(  TInt i = 0; i < IMSI.Count(); i++ )
        {
        // Write the size:
        stringSize = IMSI[i]->Size();
        Mem::Copy(ptr+offset, &stringSize, sizeof(TInt));
        offset += sizeof(TInt);

        // Write the data:
        Mem::Copy(ptr+offset, IMSI[i]->Ptr(), IMSI[i]->Size());
        offset += IMSI[i]->Size();
        }
    IPCWRITE0L( size );

    aMessage.Complete( KErrNone );
    }

// ----------------------------------------------------------------------------
// CDRMDbSession::StopWatchingL
// ----------------------------------------------------------------------------
//
void CDRMDbSession::StopWatchingL( const RMessage2& aMessage )
    {
    _LIT_SECURITY_POLICY_S0( sidCheck, KTrustedShutdownClient );
    if ( sidCheck.CheckPolicy( aMessage ) )
        {
        SERVER->StopWatchingL();
        }
    aMessage.Complete( KErrNone );
    }

// ----------------------------------------------------------------------------
// CDRMDbSession::DeleteAllowedL
// ----------------------------------------------------------------------------
//
TBool CDRMDbSession::DeleteAllowedL( const TDesC8& aContentId )
    {
    TBuf8< KMaxOmaV1CIDLength > URI;

    // Get the FL uri
    GetFlURI( URI );

    if( aContentId.Compare( URI ) &&
        aContentId.Compare( KDCMUri ) &&
        aContentId.Compare( KLDFUri ) )
        {
        return ETrue;
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CDRMDbSession::GetRandomDataL
// ----------------------------------------------------------------------------
//
void CDRMDbSession::GetRandomDataL( const RMessage2& aMessage )
    {
    DRMLOG(_L("CDRMDbSession::GetRandomDataL"));

    HBufC8* data = NULL;
    SanitizeL( aMessage.GetDesMaxLength(0) );
    SanitizeL( aMessage.GetDesLength(0));

    data = HBufC8::NewMaxLC(IPCGETDESLEN0);
    TPtr8 ptr(data->Des());

    MDrmKeyStorage* storage = DrmKeyStorageNewL();
    TCleanupItem storageCleanup( DeleteObject, storage );
    CleanupStack::PushL(storageCleanup);
    storage->RandomDataGetL(ptr,ptr.Size());
    IPCWRITE0L(*data);

    CleanupStack::PopAndDestroy(2, data);
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CDRMDbSession::GetMeteringDataL
// -----------------------------------------------------------------------------
//
void CDRMDbSession::GetMeteringDataL( const RMessage2& aMessage )
    {
#ifndef RD_DRM_METERING
    aMessage.Complete ( KErrNotSupported );
#else
    HBufC8* riId = NULL;
    HBufC8* meteringData = NULL;
    CDRMPointerArray<CDrmMeteringDbData>* meteringArray =
        CDRMPointerArray<CDrmMeteringDbData>::NewLC();
    meteringArray->SetAutoCleanup( ETrue );

    TInt size = 0;
    TPtr8 data( NULL, 0 );
    TPckg<TInt> package( size );

    SanitizeL( aMessage.GetDesLength(0));
    // Empty old data
    delete iPreparedData;
    iPreparedData = NULL;

    riId = HBufC8::NewLC( User::LeaveIfError( IPCGETDESLEN1 ) );
    data.Set( riId->Des() );
    IPCREAD1L( data );

    if ( METERINGDB.GetL( *riId, *meteringArray ) )
        {
        meteringData = CreateMeteringDataL( meteringArray );
        CleanupStack::PushL( meteringData );
        }
    else
        {
        _LIT8( KEmptyMetering, "<rawMeteringReportData>\r\n</rawMeteringReportData>");
        meteringData = KEmptyMetering().AllocLC();
        }
    DRMLOGHEX( *meteringData );
    size = meteringData->Size();
    iPreparedData = meteringData;
    CleanupStack::Pop( meteringData );

    IPCWRITE0L( package );
    CleanupStack::PopAndDestroy( 2 ); // riId, meteringArray
    if ( !iPreparedData )
        {
        aMessage.Complete( KErrNotFound );
        return;
        }
    aMessage.Complete( KErrNone );
#endif
    }

// -----------------------------------------------------------------------------
// CDRMDbSession::DeleteMeteringDataL
// -----------------------------------------------------------------------------
//
void CDRMDbSession::DeleteMeteringDataL( const RMessage2& aMessage )
    {
#ifndef RD_DRM_METERING
    aMessage.Complete ( KErrNotSupported );
#else
// Metering supported
    HBufC8* riId = NULL;
    TPtr8 data( NULL, 0 );
    TBool meteringDataDeleted;

    riId = HBufC8::NewLC( User::LeaveIfError( IPCGETDESLEN0 ) );
    data.Set( riId->Des() );
    IPCREAD0L( data );
    meteringDataDeleted = METERINGDB.DeleteL( *riId );

        if( meteringDataDeleted )
            {
            DRMLOG( _L("CDRMDbSession::DeleteMeteringDataL -> some records were destroyed") );
            }
    // Do we have to do something else?
    CleanupStack::PopAndDestroy( riId );
    aMessage.Complete( KErrNone );
#endif
    }
// -----------------------------------------------------------------------------
// CDRMDbSession::ConnectRoapClient()
// -----------------------------------------------------------------------------
//
TInt CDRMDbSession::ConnectRoapClient()
    {
    if( iRoapClientConnected )
        {
        return KErrNone;
        }
    TInt err( iRoapClient.Connect() );
    if ( !err )
        {
        iRoapClientConnected = ETrue;
        }
    return err;
    }

//  End of File
