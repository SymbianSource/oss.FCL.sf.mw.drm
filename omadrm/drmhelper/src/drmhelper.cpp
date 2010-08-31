/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CDRMHelper class
*
*/


// INCLUDE FILES
#include "DRMHelper.h"

#include "DRMHelperGlobalNoteWrapper.h"
#include "DRMHelperInfoNoteWrapper.h"
#include "DRMRightsClient.h"
#include "DcfCommon.h"
#include "DRMHelperDownloadManager.h"

#include <Drmhelper.rsg>
#include <AknQueryDialog.h>
#include <DRMCommon.h>

#include <starterclient.h>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include <StringLoader.h>
#include <eikproc.h> //CEikProcess
#include <eikdoc.h> //CEikDocument
#include <DocumentHandler.h> // KDRMErr*
#include <aknnotewrappers.h> // information note
#include <AknGlobalNote.h> // global info note

#include <DrmConstraint.h>

#include <stringresourcereader.h>
#include <apgcli.h>
#include <data_caging_path_literals.hrh>

#include <AiwGenericParam.h>
#include <DcfRep.h>
#include <DcfEntry.h>

#include <caf/caf.h>
#include <caf/cafplatform.h>

#include <AknLaunchAppService.h> // for launching RMUI embedded

#include <utf.h>

#include "DRMHelperServer.h"
#include "ConsumeData.h"
#include "DRMTypes.h"
#include "DRMClockClient.h"
#include "DRMPointerArray.h"
#include <drmbrowserlauncher.h>

#include <SecondaryDisplay/DRMHelperSecondaryDisplay.h> // for secondary display support
#include <AknMediatorFacade.h>
#include <MediatorEventProvider.h>
#include <MediatorDomainUIDs.h>
#include <featmgr.h>

#include "RoapStorageClient.h"

#include <centralrepository.h>
#include <e32property.h>
#ifdef RD_DRM_SILENT_RIGHTS_ACQUISITION
#include "drmsettingsplugininternalcrkeys.h"
#endif
#include "DRMRIContext.h"
#include "DRMDomainContext.h"

// publish & subrscibe
#include <e32property.h>
#include <PSVariables.h>


//connectivity logic
#include <cmconnectionmethod.h> // RCmConnectionMethod
#include <cmdestination.h> // RCmDestination
#include <cmmanager.h> // RCmManager

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

#ifndef RD_MULTIPLE_DRIVE
_LIT( KDriveZ, "z:" );
#endif

_LIT( KCDRMHelperResFileName,"DRMHelper.rsc" );
_LIT( KEncryptedRightsIssuerMatchString, "flk*" );
_LIT( KDRMHelperClassificationRingingTone, "Ringtone" );
_LIT( KDRMHelperClassificationVideoTone, "Videotone" );


// masks for constaint existence
const TInt KDRMHelperConstraintCount = 1;
const TInt KDRMHelperConstraintTime = 2;
const TInt KDRMHelperConstraintAccumulated = 4;

// Buffer sizes
const TInt KDRMHelperMaxDateFormatLen = 30;
const TInt KDRMHelperMaxDateLen = 30;
const TInt KDRMHelperMaxTimeFormatLen = 30;
const TInt KDRMHelperMaxTimeLen = 30;
const TInt KUrlMaxLen = 1024;
const TInt KDRMHelperMaxETABufSize = 11;

// limit in minutes when start to display 'about to expire' notes for accumulated
// time content
#ifndef RD_DRM_REMOVAL_OF_EXPIRATION_WARNING_PROMPTS
const TInt KDRMHelperAccumulatedTimeLimit = 15;
#endif // RD_DRM_REMOVAL_OF_EXPIRATION_WARNING_PROMPTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// ============================ auto_handde helper class =======================
//Auto handle for easening handle release on exceptional exit situations
template<typename T> class auto_handle
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

// ============================= LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// HasDefaultConnectionL
// Finds default IAP id
// @return Etrue: valid AP found
//         EFalse: valid AP not found
// @leave system wide error codes
// -----------------------------------------------------------------------------
//
LOCAL_C TBool HasDefaultConnectionL()
    {
    TBool hasDefault(EFalse);
    TCmDefConnValue defConn;
    RCmManager cmManager;
    cmManager.OpenLC();
    cmManager.ReadDefConnL(defConn);
    if (defConn.iType == ECmDefConnConnectionMethod)
        {
        cmManager.GetConnectionMethodInfoIntL(defConn.iId,
                CMManager::ECmIapId);
        hasDefault = ETrue;
        }
    else if (defConn.iType == ECmDefConnDestination)
        {
        RCmDestination dest(cmManager.DestinationL(defConn.iId));
        CleanupClosePushL(dest);

        if (dest.ConnectionMethodCount() <= 0)
            {
            User::Leave(KErrNotFound);
            }

        RCmConnectionMethod cMeth(dest.ConnectionMethodL(0));
        CleanupClosePushL(cMeth);

        cMeth.GetIntAttributeL(CMManager::ECmIapId);
        CleanupStack::PopAndDestroy(&cMeth);
        CleanupStack::PopAndDestroy(&dest);
        hasDefault = ETrue;
        }
    CleanupStack::PopAndDestroy(&cmManager);
    return hasDefault;
    }

// -----------------------------------------------------------------------------
// HasAccessPointsL
// -----------------------------------------------------------------------------
//
LOCAL_C TBool HasAccessPointsL()
    {
    TInt apCount(0);
    RCmManager cmManager;
    CleanupClosePushL(cmManager);
    cmManager.OpenL();
    RArray<TUint32> aps;
    CleanupClosePushL(aps);
    cmManager.ConnectionMethodL(aps, EFalse, EFalse, ETrue);
    apCount = aps.Count();
    CleanupStack::PopAndDestroy(2, &cmManager); //aps, cmManager
    return apCount > 0;
    }

// -----------------------------------------------------------------------------
// Creates note for resource text to be viewed,
// Defaults to information note.
// -----------------------------------------------------------------------------
LOCAL_C CAknResourceNoteDialog* CreateNoteForResourceL( TInt aResId )
    {
    CAknResourceNoteDialog* r( NULL );
    switch (aResId)
        {
        case R_DRM_ERR_OPENING_FAIL_PERM:
            r = new ( ELeave ) CAknErrorNote( ETrue );
            break;
        case R_DRM_WARN_NO_CONN_DEFINED:
        case R_DRM_WARN_INVALID_OR_NO_AP:
            r = new  ( ELeave ) CAknWarningNote( ETrue );
            break;
        default:
            r = new ( ELeave ) CAknInformationNote( ETrue );
            break;
        }
    return r;
    }

// -----------------------------------------------------------------------------
// Returns note type for globan note to be viewed.
// Defaults to information note.
// -----------------------------------------------------------------------------
LOCAL_C TAknGlobalNoteType GlobalNoteTypeForResource( TInt aResId )
    {
    TAknGlobalNoteType t( EAknGlobalInformationNote );
    switch (aResId)
        {
        case R_DRM_ERR_OPENING_FAIL_PERM:
            t = EAknGlobalErrorNote;
            break;
        case R_DRM_WARN_NO_CONN_DEFINED:
        case R_DRM_WARN_INVALID_OR_NO_AP:
            t = EAknGlobalWarningNote;
            break;
        default:
            break;
        }
    return t;

    }

// -----------------------------------------------------------------------------
// LeaveIfWrongReturnForHandleErrorOrPreviewL
// Leaves if error code under test (argument aError) does match those documented
// for HandleErrorOrPreviewL
// -----------------------------------------------------------------------------
LOCAL_C void LeaveIfWrongReturnForHandleErrorOrPreviewL( TInt aError )
    {
    switch ( aError )
        {
        case KErrNone:
        case KErrCancel:
        case KErrCANoRights:
        case KErrCANoPermission:
        case KErrCompletion:
            break;
        default:
            User::Leave( aError );
        }
    }

// -----------------------------------------------------------------------------
// Checks whether given media type is video type
// -----------------------------------------------------------------------------
LOCAL_C TBool IsVideo(const CData* aData)
    {
    TPtr mimeDes( NULL, 0);
    TBool ret( EFalse );
    HBufC* mimeType( HBufC::New( KMaxDataTypeLength ) );
    if ( !mimeType )
        {
        return ret;
        }
    mimeDes.Set( mimeType->Des() );

    //Note mime types borrowed from CDRMHelper::Intent
    _LIT( KDRMHelperMimeTypeVideo, "Video" );
    _LIT( KDRMHelperMimeTypeShockwave, "Application/x-shockwave-flash" );
    _LIT( KDRMHelperMimeTypeRealMedia, "Application/x-pn-realmedia" );
    _LIT( KDRMHelperMimeTypeVndRealMedia, "Application/vnd.rn-realmedia" );

    if ( aData->GetStringAttribute( EMimeType, mimeDes ) == KErrNone )
        {
        ret = !mimeDes.FindF( KDRMHelperMimeTypeShockwave ) ||
            !mimeDes.FindF( KDRMHelperMimeTypeRealMedia ) ||
            !mimeDes.FindF( KDRMHelperMimeTypeVndRealMedia ) ||
            !mimeDes.FindF( KDRMHelperMimeTypeVideo );
        }
    delete mimeType;
    return ret;
    }

// ---------------------------------------------------------
// Evaluate media type for preview playback
// ---------------------------------------------------------
#ifdef RD_DRM_PREVIEW_RIGHT_FOR_AUDIO
LOCAL_C TInt EvaluatePreviewMediaType( CData* aContent )
    {
    TInt theType( -1 );
    _LIT( KDRMHelperMimeTypeAudio, "Audio" );
    HBufC* mimeType( HBufC::New( KMaxDataTypeLength ) );
    if ( !mimeType )
        {
        return theType;
        }
    TPtr ptr = mimeType->Des();
    TInt error = aContent->GetStringAttribute( EMimeType, ptr );
    if ( IsVideo( aContent ) )
        {
        theType = EPreviewTypeVideo;
        }
    else if ( !ptr.FindF( KDRMHelperMimeTypeAudio ) )
        {
        theType = EPreviewTypeAudio;
        }
    delete mimeType;
    return theType;
    }

#endif

#pragma mark -
// -----------------------------------------------------------------------------
//
// Functions related to WMDRM protection check
//
// -----------------------------------------------------------------------------

// Some magic constants
static const TInt KMinContentLength( 16 );
_LIT8( KWrmHeader, "W\0R\0M\0H\0E\0A\0D\0E\0R\0" );
_LIT8( KASFHeaderObject, "75B22630668E11CFA6D900AA0062CE6C" );

// -----------------------------------------------------------------------------
// FormatGUID
// -----------------------------------------------------------------------------
LOCAL_C void FormatGUID( TDes8 &aGUID )
    {
    TBuf8<16> copyGUID(aGUID);
    TInt i;
    for (i = 0; i < 4; i++)
        {
        copyGUID[i] = aGUID[3 - i];
        }
    for (i = 4; i < 6; i++)
        {
        copyGUID[i] = aGUID[9 - i];
        }
    for (i = 6; i < 8; i++)
        {
        copyGUID[i] = aGUID[13 - i];
        }
    for (i = 8; i < 16 ; i++)
        {
        copyGUID[i] = aGUID[i];
        }
    aGUID.Delete(0, 32);
    for (i = 0; i <16; i++)
        {
        aGUID.AppendNumFixedWidthUC(copyGUID[i], EHex, 2);
        }
    }

// -----------------------------------------------------------------------------
// ConvertToInt64
// -----------------------------------------------------------------------------
LOCAL_C TInt64 ConvertToInt64( TDesC8& aDes )
    {
    TInt64 num = 0;
    TInt i;
    for (i = 7 ; i >= 0; i--)
        {
        num <<= 8;
        num |= aDes[i];
        }
    return num;
    }


// -----------------------------------------------------------------------------
// IsProtectedWmDrmL
// returns ETrue, if file is protected WMDRM file
//         EFalse if file is not protected WMDRM file
// Leaves with KErrUnderflow if file has too little data to decide
//         whether WmDrm or not
//         may also leave with other system wide error code
// -----------------------------------------------------------------------------
LOCAL_C TBool IsProtectedWmDrmL( RFile& aFileHandle )
    {
    TInt r( KErrNone );
    HBufC8* buffer( NULL );
    TInt pos( 0 );
    RFile file;
    TBuf8< 32 > header;

    TInt64 headerSize( 0 );
    TBool isProtectedWmDrm( EFalse );
    TPtr8 headerPtr( NULL, 0 );

    // Leave if given handle is invalid
    if ( !aFileHandle.SubSessionHandle() )
        {
        User::Leave( KErrBadHandle );
        }

    User::LeaveIfError( file.Duplicate( aFileHandle ) );
    CleanupClosePushL( file );

    User::LeaveIfError( file.Seek( ESeekStart, pos ) );

    // Check if the file is an ASF file

    User::LeaveIfError( file.Read( 0, header, KMinContentLength ) );
    if ( header.Length() < KMinContentLength )
        {
        User::Leave( KErrUnderflow );
        }

    FormatGUID( header );

    if ( header == KASFHeaderObject )
        {
        // It's ASF, check still whether it's WM DRM protected or not
        file.Read( header,8 );
        headerSize = ConvertToInt64( header );
        if ( headerSize <= 30 )
            {
            User::Leave( KErrUnderflow );
            }
        if ( headerSize > ( ( KMaxTInt32 / 2 ) - 1 ) )
            {
            User::Leave( KErrOverflow );
            }
        buffer = HBufC8::NewLC( headerSize );

        headerPtr.Set( buffer->Des() );
        User::LeaveIfError( file.Read( headerPtr, headerSize - 24 ) );

        r = headerPtr.Find( KWrmHeader );
        if ( r != KErrNotFound )
            {
            isProtectedWmDrm = ETrue;
            }
        CleanupStack::PopAndDestroy( buffer );
        }
    CleanupStack::PopAndDestroy( &file );
    return isProtectedWmDrm;
    }

#pragma mark -
// -----------------------------------------------------------------------------
//
// End of Functions related to WMDRM protection check
//
// -----------------------------------------------------------------------------

// ---------------------------------------------------------
// UpdateDCFRepositoryL()
// Update saved file to DCFRepository
// ---------------------------------------------------------

LOCAL_C void UpdateDCFRepositoryL( const RFile& aFileHandle )
    {
    TFileName fullName;
    aFileHandle.FullName( fullName );

    CDcfEntry* dcfEntry = NULL;
    dcfEntry = CDcfEntry::NewL();
    CleanupStack::PushL( dcfEntry );

    CDcfRep* dcfRep = NULL;
    dcfRep = CDcfRep::NewL();
    CleanupStack::PushL( dcfRep );

    dcfEntry->SetLocationL( fullName, 0 );
    dcfRep->UpdateL( dcfEntry );
    CleanupStack::PopAndDestroy( dcfRep );
    CleanupStack::PopAndDestroy( dcfEntry );
    }
#pragma mark -


// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::CDRMHelperRightsConstraints
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMHelperRightsConstraints::CDRMHelperRightsConstraints(
    CDRMRightsConstraints* aConstraints )
    {
    iConstraints = aConstraints;
    }

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDRMHelperRightsConstraints::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMHelperRightsConstraints* CDRMHelperRightsConstraints::NewLC(
    CDRMRightsConstraints* aConstraints )
    {
    CleanupStack::PushL( aConstraints );
    CDRMHelperRightsConstraints* self =
        new (ELeave) CDRMHelperRightsConstraints( aConstraints );
    CleanupStack::Pop( aConstraints );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CDRMHelperRightsConstraints* CDRMHelperRightsConstraints::NewL(
    CDRMRightsConstraints* aConstraints )
    {
    CDRMHelperRightsConstraints* self = NewLC( aConstraints );
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
EXPORT_C CDRMHelperRightsConstraints::~CDRMHelperRightsConstraints()
    {
    delete iConstraints;
    }

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::FullRights
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMHelperRightsConstraints::FullRights()
    {
    return iConstraints->FullRights();
    }

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::IsPreview
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMHelperRightsConstraints::IsPreview()
    {
    return iConstraints->IsPreview();
    }

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::GetCountersL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelperRightsConstraints::GetCountersL(
    TUint32& aCounter,
    TUint32& aOriginalCounter )
    {
    if ( iConstraints->GetCounters( aCounter, aOriginalCounter ) ==
        DRMCommon::ENoRights )
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::GetStartTimeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelperRightsConstraints::GetStartTimeL( TTime& aStartTime )
    {
    if ( iConstraints->GetStartTime( aStartTime ) == DRMCommon::ENoRights )
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::GetEndTimeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelperRightsConstraints::GetEndTimeL( TTime& aEndTime )
    {
    if ( iConstraints->GetEndTime( aEndTime ) == DRMCommon::ENoRights )
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::GetIntervalL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelperRightsConstraints::GetIntervalL(
    TTimeIntervalSeconds& aInterval )
    {
    if ( iConstraints->GetInterval( aInterval ) == DRMCommon::ENoRights )
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::GetIntervalStartL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelperRightsConstraints::GetIntervalStartL(
    TTime& aStartTime )
    {
    if ( iConstraints->GetIntervalStart( aStartTime ) == DRMCommon::ENoRights )
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::GetTimedCountL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelperRightsConstraints::GetTimedCountL( TUint32& aCounter,
    TUint32& aOriginalCounter, TTimeIntervalSeconds& aTimer )
    {
    if ((iConstraints->GetConstraint()).iActiveConstraints & EConstraintTimedCounter)
        {
        aCounter = (iConstraints->GetConstraint()).iTimedCounter;
        aOriginalCounter = aCounter;
        aTimer = ( iConstraints->GetConstraint() ).iTimedInterval;
        }
    else
        {
        User::Leave(KErrNotFound);
        }

    }


// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::GetTimedCountL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelperRightsConstraints::GetAccumulatedTimeL(
    TTimeIntervalSeconds& aAccumulatedTime )
    {
    if ((iConstraints->GetConstraint()).iActiveConstraints & EConstraintAccumulated)
        {
        aAccumulatedTime = (iConstraints->GetConstraint()).iAccumulatedTime;
        }
    else
        {
        User::Leave(KErrNotFound);
        }
    }

#pragma mark -

// -----------------------------------------------------------------------------
// CDRMHelper::CDRMHelper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMHelper::CDRMHelper( CCoeEnv& aCoeEnv ) :
    iCoeEnv( &aCoeEnv ),
    iUseCoeEnv( ETrue ),
    iAutomatedType( EAutomatedTypeOther ),
    iPreviewMediaType(-1)
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelper::CDRMHelper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMHelper::CDRMHelper() :
    iUseCoeEnv( EFalse ),
    iAutomatedType( EAutomatedTypeOther )
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelper::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDRMHelper::ConstructL( RFs* aFs )
    {
    TInt drmMethods;
    DRMCommon::TOMALevel omaLevel;

    iDRMCommon = DRMCommon::NewL();
    User::LeaveIfError( iDRMCommon->Connect() );
    User::LeaveIfError( iHelperServer.Connect() );

    iDRMCommon->SupportedDRMMethods( drmMethods, omaLevel );
    iOma2 = omaLevel == DRMCommon::EOMA_2_0 ? ETrue : EFalse;

    if ( aFs )
        {
        iFs = *aFs;
        iFsOwned = EFalse;
        }
    else
        {
        User::LeaveIfError( iFs.Connect() );
        User::LeaveIfError(iFs.ShareAuto());
        iFsOwned = ETrue;
        }

#ifndef RD_MULTIPLE_DRIVE

    TFileName fileName( KDriveZ );

#else //RD_MULTIPLE_DRIVE

    _LIT( KDrive, "%c:");
    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, driveNumber );
    iFs.DriveToChar( driveNumber, driveLetter );

    TFileName fileName;

    fileName.Format( KDrive, (TUint)driveLetter );

#endif

    fileName.Append( KDC_RESOURCE_FILES_DIR );
    fileName.Append( KCDRMHelperResFileName );

    TRAP_IGNORE( FeatureManager::InitializeLibL() );

    iStringResourceReader = CStringResourceReader::NewL( fileName, iFs );

    if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) )
        {
        iEventProvider = CMediatorEventProvider::NewL();
        }
    }

// -----------------------------------------------------------------------------
// CDRMHelper::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMHelper* CDRMHelper::NewLC( CCoeEnv& aCoeEnv )
    {
    CDRMHelper* self = new (ELeave) CDRMHelper( aCoeEnv );
    CleanupStack::PushL( self );
    self->ConstructL( NULL );
    return self;
    }

EXPORT_C CDRMHelper* CDRMHelper::NewL( CCoeEnv& aCoeEnv )
    {
    CDRMHelper* self = NewLC( aCoeEnv );
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C CDRMHelper* CDRMHelper::NewL()
    {
    CDRMHelper* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C CDRMHelper* CDRMHelper::NewLC()
    {
    CDRMHelper* self = new (ELeave) CDRMHelper();
    CleanupStack::PushL( self );
    self->ConstructL( NULL );
    return self;
    }

EXPORT_C CDRMHelper* CDRMHelper::NewLC( CCoeEnv& aCoeEnv, RFs& aFs )
    {
    CDRMHelper* self = new (ELeave) CDRMHelper( aCoeEnv );
    CleanupStack::PushL( self );
    self->ConstructL( &aFs );
    return self;
    }

EXPORT_C CDRMHelper* CDRMHelper::NewL( CCoeEnv& aCoeEnv, RFs& aFs )
    {
    CDRMHelper* self = NewLC( aCoeEnv, aFs );
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C CDRMHelper* CDRMHelper::NewL( RFs& aFs )
    {
    CDRMHelper* self = NewLC( aFs );
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C CDRMHelper* CDRMHelper::NewLC( RFs& aFs )
    {
    CDRMHelper* self = new (ELeave) CDRMHelper();
    CleanupStack::PushL( self );
    self->ConstructL( &aFs );
    return self;
    }

// Destructor
EXPORT_C CDRMHelper::~CDRMHelper()
    {
    // closing all rdb sessions is handled in the destructor
    // of the CConsumeData objects
    // delete rdb connections array
    iConsumeList.ResetAndDestroy();
    iConsumeList.Close();
    iHelperServer.Close();

    iNoteList.ResetAndDestroy(); // GlobalNoteInfo
    iNoteList.Close();

    if (iDRMCommon)
        {
        iDRMCommon->Disconnect();
        }
    delete iStringResourceReader;
    delete iDRMCommon;

    if ( iFsOwned )
        {
        iFs.Close();
        }

    FeatureManager::UnInitializeLib();

    delete iEventProvider;
    }

#pragma mark -

// -----------------------------------------------------------------------------
// CDRMHelper::HandleErrorL
// -----------------------------------------------------------------------------
//

EXPORT_C TInt CDRMHelper::HandleErrorL( TInt aError,
    const TDesC8& aURI )
    {
    RFile fileHandle;
    TInt ret( KErrNotFound );
    CleanupClosePushL( fileHandle );

    if ( GetFileHandleFromURIL( aURI, fileHandle ) == KErrNone )
        {
        ret = CDRMHelper::HandleErrorL( aError, fileHandle );
        }
    CleanupStack::PopAndDestroy( &fileHandle );
    return ret;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::HandleErrorL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::HandleErrorL( TInt aError, RFile& aFileHandle )
    {
    TInt ret = KErrNone;
    HBufC* rightsIssuer = NULL;
    HBufC* mimeType = NULL;
    HBufC* contentURI = NULL;
    HBufC* domainRoUrl = NULL;
    HBufC* eta = NULL;
    HBufC8* contentURI8 = NULL;
    HBufC* riId = NULL;
    HBufC8* riId8 = NULL;
    HBufC* domainId = NULL;
    HBufC8* domainId8 = NULL;

    TInt error;
    TPtr ptr(NULL, 0);
    TFileName fileName;

    // To embed Domain RO in superdistribution case, register the content
    TRAP_IGNORE( UpdateDCFRepositoryL( aFileHandle ) );

    CData* content = CData::NewLC( aFileHandle, KDefaultContentObject, EPeek );

    // get rights-issuer URL
    TRAP_IGNORE(GetRightsIssuerL( *content, rightsIssuer ) );
    if ( rightsIssuer )
        {
        CleanupStack::PushL( rightsIssuer );
        }

    mimeType = HBufC::NewLC( KMaxDataTypeLength );
    contentURI = HBufC::NewLC( KUrlMaxLen );
    domainRoUrl = HBufC::NewLC( KUrlMaxLen );
    eta = HBufC::NewLC( KDRMHelperMaxETABufSize );
    riId = HBufC::NewLC( KUrlMaxLen );
    domainId = HBufC::NewLC( KUrlMaxLen );

    // create attribute set
    RStringAttributeSet stringAttributeSet;
    CleanupClosePushL( stringAttributeSet );
    // add the attributes we are interested in
    stringAttributeSet.AddL( EContentID );
    stringAttributeSet.AddL( EMimeType );
    stringAttributeSet.AddL( EDomainRightsIssuerUrl );
    stringAttributeSet.AddL( EPendingRightsETA );
    stringAttributeSet.AddL( EDomainRightsIssuerId );
    stringAttributeSet.AddL( EDomainId );

    User::LeaveIfError( content->GetStringAttributeSet( stringAttributeSet ) );

    // pass on values of string attributes
    ptr.Set( contentURI->Des() );
    error = stringAttributeSet.GetValue( EContentID, ptr );
    if ( error == KErrNone )
        {
        ptr.Set( domainRoUrl->Des() );
        stringAttributeSet.GetValue( EDomainRightsIssuerUrl, ptr );

        ptr.Set( mimeType->Des() );
        error = stringAttributeSet.GetValue( EMimeType, ptr );

        ptr.Set( riId->Des() );
        stringAttributeSet.GetValue( EDomainRightsIssuerId, ptr );

        ptr.Set( domainId->Des() );
        stringAttributeSet.GetValue( EDomainId, ptr );

        if ( error == KErrNone )
            {
            ptr.Set( eta->Des() );
            error = stringAttributeSet.GetValue( EPendingRightsETA, ptr );
            HBufC* etaPtr = error == KErrNone ? eta : NULL;
            aFileHandle.Name( fileName );
            contentURI8 = HBufC8::NewLC( contentURI->Length() );
            contentURI8->Des().Copy( contentURI->Des() );
            riId8 = HBufC8::NewLC( riId->Length() );
            riId8->Des().Copy( riId->Des() );
            domainId8 = HBufC8::NewLC( domainId->Length() );
            domainId8->Des().Copy( domainId->Des() );
            ret = DoHandleErrorL( aError,
                contentURI8,
                mimeType,
                rightsIssuer,
                domainRoUrl,
                etaPtr,
                riId8,
                domainId8,
                StripPathAndExtension( fileName ) );
            CleanupStack::PopAndDestroy( 3, contentURI8 ); // contentURI8, riId8
            }
        else
            {
            User::Leave( KErrArgument );
            }
        }
    else
        {
        User::Leave( KErrArgument );
        }

    CleanupStack::PopAndDestroy( 7, mimeType );

    if ( rightsIssuer )
        {
        CleanupStack::PopAndDestroy( rightsIssuer );
        }

    CleanupStack::PopAndDestroy( content );
    return ret;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::HandleErrorL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::HandleErrorL( TInt aError, const TDesC& aFileName )
    {
    RFile file;
    TInt r = KErrNone;

    r = file.Open(iFs, aFileName, EFileRead | EFileShareReadersOrWriters);

    if (r == KErrInUse)
        {
        r = file.Open(iFs, aFileName, EFileRead | EFileShareAny);
        if (r == KErrInUse)
            {
            r = file.Open(iFs, aFileName, EFileRead | EFileShareReadersOnly);
            }
        }
    User::LeaveIfError(r);

    CleanupClosePushL( file );
    r = HandleErrorL( aError, file );
    CleanupStack::PopAndDestroy( &file );
    return r;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::DoHandleErrorL
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::DoHandleErrorL(
    TInt aError,
    HBufC8* aContentUri,
    HBufC* aMimeType,
    HBufC* aRIUrl,
    HBufC* aDomainRoUrl,
    HBufC* aEta,
    HBufC8* aRiId8,
    HBufC8* aDomainId8,
    const TDesC& aFileName )
    {
    TBool automated( EFalse );
    TInt error(0);
    TInt ret(0);
    TInt resId(0);
    CDRMRights* rights = NULL;
    Roap::RRoapStorageClient storage;
    CDRMRIContext* riContext = NULL;
    CDRMDomainContext* domainContext = NULL;
    TBool isRegistered = EFalse;
    TBool isJoined = EFalse;

    iReturnCancel = EFalse;
    switch ( aError )
        {
        case KErrCAPendingRights:
        case DRMCommon::ERightsExpired:
        case DRMCommon::ENoRights:
        case KDRMErrNoRights:
            if ( aError == DRMCommon::ENoRights || aError == KDRMErrNoRights )
                {
                // check if it is expired situation after all
                error = GetExpirationDetails(
                    aContentUri, Intent( aMimeType ), rights );
                if ( error == CDRMRights::EExpiredRights )
                    {
                    aError = DRMCommon::ERightsExpired;
                    }
                if ( rights )
                    {
                    delete rights;
                    rights = NULL;
                    }
                }

            if ( aRiId8 && aRiId8->Length() && aDomainRoUrl && aDomainRoUrl->Length() )
                {
                // Check if we have domain RO
                User::LeaveIfError( storage.Connect() );
                CleanupClosePushL( storage );
                riContext = storage.GetRIContextL( *aRiId8 );
                if ( riContext )
                    {
                    isRegistered = ETrue;
                    delete riContext;
                    riContext = NULL;
                    if ( aDomainId8 && aDomainId8->Length() )
                        {
                        domainContext = storage.GetDomainContextL( *aDomainId8 );
                        if ( domainContext )
                            {
                            // last three digits presents the domain generation
                            TInt generation = 0;
                            TLex8 lex( aDomainId8->Right( 3 ) );
                            lex.Val( generation );

                            if ( domainContext->DomainGeneration() >= generation )
                                {
                                isJoined = ETrue;
                                }
                            delete domainContext;
                            domainContext = NULL;
                            }
                        }
                    }
                CleanupStack::PopAndDestroy( &storage );
                }

            // check if future rights
            if ( aError != KErrCAPendingRights &&
                GetExpirationDetails(
                    aContentUri, Intent( aMimeType ), rights ) ==
                CDRMRights::EFutureRights )
                {
                if ( aContentUri )
                    {
                    TRAPD( err,
                        error = ShowFutureNoteL( aMimeType, rights ) );
                    if ( err )
                        {
                        error = err;
                        }
                    }
                if ( rights )
                    {
                    delete rights;
                    rights = NULL;
                    }
                }
            else
                {
                TInt type(0);

                // check if this is automated content
                if ( aContentUri )
                    {
                    error = iHelperServer.IsAutomated(
                        *aContentUri,
                        iAutomatedType,
                        Intent( aMimeType ),
                        automated,
                        type );
                    if (!error && automated)
                        {
                        aError = DRMCommon::ERightsExpired;
                        }
                    }

                if ( aError == DRMCommon::ERightsExpired )
                    {

                    if ( !error && automated )
                        {
                        // automated content
                        TBool active = (type ==
                            CDRMHelperServer::EActive ? ETrue : EFalse);
                        TUint32 rejectReason = RejectReasonL(
                            Intent( aMimeType ), *aContentUri );
                        if ( aRIUrl )
                            {
                            // superdistributable
                            // get resource id
                            resId = AutomatedResIdL( ETrue, ETrue, rejectReason );
                            ret = DisplayQueryL( resId, aFileName );
                            if ( ret == 0 )
                                {
                                // Query has been closed with cancel or similar,
                                // So handleErrorOrPreviewL should return KErrCancel
                                iReturnCancel = ETrue;
                                // new rights not bought, unregister content
                                iHelperServer.RemoveAutomatedAll(
                                    *aContentUri,
                                    active,
                                    iAutomatedType,
                                    Intent( aMimeType ) );
                                }
                            }
                        else
                            {
                            // non-superdistributable, show note and unregister
                            resId = AutomatedResIdL( ETrue, EFalse, rejectReason );
                            ret = DisplayQueryWithIdValueL(
                                resId,
                                R_DRMHELPER_WAITING_RIGHTS_CONFIRMATION_QUERY,
                                aFileName );

                            if ( ret == 0 )
                                {
                                // Query has been closed with cancel or similar,
                                // So handleErrorOrPreviewL should return KErrCancel
                                iReturnCancel = ETrue;
                                }
                            iHelperServer.RemoveAutomatedAll(
                                *aContentUri,
                                active,
                                iAutomatedType,
                                Intent( aMimeType ) );
                            }
                        }
                    else
                        {
                        if ( aRIUrl )
                            {
                            resId = ErrorResIdL(
                                *aContentUri, Intent( aMimeType ), ETrue, ETrue );
                            ret = DisplayQueryWithIdL(
                                resId,
                                R_DRMHELPER_CONFIRMATION_QUERY );
                            if ( ret == 0 )
                                {
                                // Query has been closed with cancel or similar,
                                // So handleErrorOrPreviewL should return KErrCancel
                                iReturnCancel = ETrue;
                                }
                            }
                        else
                            {
                            resId = ErrorResIdL(
                                *aContentUri, Intent( aMimeType ), EFalse, ETrue );
                            DisplayInfoNoteL( resId );
                            }
                        }
                    }
                else
                    {
                    // check if rights are expected to arrive
                    TInt eta(0);
                    if ( aEta )
                        {
                        TLex lexer( *aEta );
                        error = lexer.Val( eta );
                        }
                    if (!error && aError == KErrCAPendingRights && eta == -1)
                        {
                        // rights should have come
                        if ( aRIUrl )
                            {
                            ret = DisplayQueryWithIdL(
                                R_DRMHELPER_RIGHTS_SHOULD_HAVE_COME,
                                R_DRMHELPER_CONFIRMATION_QUERY );
                            if ( ret == 0 )
                                {
                                // Query has been closed with cancel or similar,
                                // So handleErrorOrPreviewL should return KErrCancel
                                iReturnCancel = ETrue;
                                }
                            }
                        else
                            {
                            // no Rights-Issuer
                            DisplayQueryWithIdL(
                                R_DRMHELPER_RIGHTS_SHOULD_HAVE_COME_NO_RI,
                                R_DRMHELPER_WAITING_RIGHTS_CONFIRMATION_QUERY );
                            }
                        }
                    else if ( !eta || error || eta == -1 )
                        { // rights not expected to arrive
                        if ( aDomainRoUrl && aDomainRoUrl->Length() && isRegistered && !isJoined ) // Domain ro case
                            {
                            ret = DisplayQueryWithIdL( R_DRMHELPER_ACTIVATE_ACCOUNT,
                                R_DRMHELPER_CONFIRMATION_QUERY );
                            if ( ret == 0 )
                                {
                                // Query has been closed with cancel or similar,
                                // So handleErrorOrPreviewL should return KErrCancel
                                iReturnCancel = ETrue;
                                }
                            }
                        else if ( aRIUrl ) // Device ro case
                            {
                            resId = ErrorResIdL(
                                *aContentUri, Intent( aMimeType ), ETrue, EFalse );
                            ret = DisplayQueryL( resId, aFileName );
                            if ( ret == 0 )
                                {
                                // Query has been closed with cancel or similar,
                                // So handleErrorOrPreviewL should return KErrCancel
                                iReturnCancel = ETrue;
                                }
                            }
                        else
                            {
                            resId = ErrorResIdL(
                                *aContentUri, Intent( aMimeType ), EFalse, EFalse );
                            DisplayInfoNoteL( resId );
                            }
                        }
                    else
                        {
                        // rights expected to arrive
                        if ( eta != KErrCAPendingRights )
                            {
                            // rights expected to arrive in eta seconds
                            DisplayQueryWithIdL( R_DRMHELPER_WAITING_FOR_RIGHTS,
                                R_DRMHELPER_WAITING_RIGHTS_CONFIRMATION_QUERY );
                            }
                        else
                            {
                            // rights should have come
                            if ( aRIUrl )
                                {
                                ret = DisplayQueryWithIdL(
                                    R_DRMHELPER_RIGHTS_SHOULD_HAVE_COME,
                                    R_DRMHELPER_CONFIRMATION_QUERY );
                                if ( ret == 0 )
                                    {
                                    // Query has been closed with cancel or similar,
                                    // So handleErrorOrPreviewL should return KErrCancel
                                    iReturnCancel = ETrue;
                                    }
                                }
                            else
                                {
                                // no Rights-Issuer
                                DisplayQueryWithIdL(
                                    R_DRMHELPER_RIGHTS_SHOULD_HAVE_COME_NO_RI,
                                    R_DRMHELPER_WAITING_RIGHTS_CONFIRMATION_QUERY );
                                }
                            }
                        }
                    }

                if ( ret == EAknSoftkeyYes )
                    {
                    if ( aDomainRoUrl && aDomainRoUrl->Length() && !isRegistered )
                        { // Domain RO but not registered
                        LaunchBrowserL( aDomainRoUrl );
                        }
                    else if ( aDomainRoUrl && aDomainRoUrl->Length() && !isJoined )
                        { // Domain RO but not part of the domain
                        LaunchBrowserL( aDomainRoUrl );
                        }
                    else if (aRIUrl && aRIUrl->Length() )
                        {
                        LaunchBrowserL( aRIUrl );
                        }
                    }
                }
            break;

        case DRMCommon::ERightsDBCorrupted:
        case KDRMErrRightsDBCorrupted:
            // flow through

        default:
            User::LeaveIfError( aError );
            break;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::ShowFutureNoteL
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::ShowFutureNoteL( const HBufC* aMimeType, CDRMRights* aRightsObject)
    {
    TInt error( KErrArgument );
    TInt intent = Intent( aMimeType );

    TTime startTime;

    error = GetStartTime( aRightsObject, intent, startTime );
    if ( !error )
        {
        TBuf<KDRMHelperMaxDateFormatLen> dateFormat;
        TBuf<KDRMHelperMaxDateLen + KDRMHelperMaxTimeLen + 1> startDateBuf;
        TBuf<KDRMHelperMaxTimeFormatLen> timeFormat;
        TBuf<KDRMHelperMaxTimeLen> startTimeBuf;

#ifndef RD_MULTIPLE_DRIVE

        TFileName avkonResFile( KDriveZ );

#else //RD_MULTIPLE_DRIVE

        _LIT( KDrive, "%c:");
        TInt driveNumber( -1 );
        TChar driveLetter;
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, driveNumber );
        iFs.DriveToChar( driveNumber, driveLetter );

        TFileName avkonResFile;

        avkonResFile.Format( KDrive, (TUint)driveLetter );

#endif

        _LIT( KCDRMHelperAvkonResFileName, "avkon.rsc" );
        _LIT( KSpace, " " );

        avkonResFile.Append( KDC_RESOURCE_FILES_DIR );
        avkonResFile.Append( KCDRMHelperAvkonResFileName );

        // get date string format
        CStringResourceReader* avkonResReader =
            CStringResourceReader::NewLC( avkonResFile );

        dateFormat = avkonResReader->ReadResourceString(
            R_QTN_DATE_USUAL_WITH_ZERO );

        // get time string format
        timeFormat = avkonResReader->ReadResourceString(
            R_QTN_TIME_USUAL_WITH_ZERO );

        CleanupStack::PopAndDestroy( avkonResReader );

        // format date and time
        startTime.FormatL( startDateBuf, dateFormat );
        startTime.FormatL( startTimeBuf, timeFormat );
        startDateBuf.Append( KSpace );
        startDateBuf.Append( startTimeBuf );

        // display note with start date
        DisplayInfoNoteL( R_DRMHELPER_USAGE_RIGHTS_IN_FUTURE,
            startDateBuf );
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::ErrorResIdL
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::ErrorResIdL(
    const TDesC8& aContentUri,
    TInt aIntent,
    TBool aRightsIssuer,
    TBool aExpired )
    {
    TInt resId(0);

    TUint32 rejectReason = RejectReasonL( aIntent, aContentUri );
    if ( rejectReason & EConstraintIndividual )
        {
        if ( aRightsIssuer )
            {
            resId = R_DRMHELPER_INVALID_SIM;
            }
        else
            {
            resId = R_DRM_INFO_SIM_NOT_ALLOWED;
            }
        }
    else
        {
        if ( aExpired )
            {
            resId = aRightsIssuer ? R_DRMHELPER_USAGE_RIGHTS_EXPIRED_BUY_NEW :
                R_DRMHELPER_USAGE_RIGHTS_EXPIRED;
            }
        else
            {
            // no rights
            resId = aRightsIssuer ? R_DRMHELPER_NO_USAGE_RIGHTS_BUY_NEW :
                R_DRMHELPER_NO_USAGE_RIGHTS;
            }
        }

    return resId;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::RejectReasonL
// -----------------------------------------------------------------------------
//
TUint32 CDRMHelper::RejectReasonL( TInt aIntent, const TDesC8& aContentUri )
    {
    RDRMRightsClient rightsClient;
    TUint32 rejectReason(0);

    User::LeaveIfError( rightsClient.Connect() );
    rightsClient.CheckRights( aIntent, aContentUri, rejectReason );
    rightsClient.Close();

    return rejectReason;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::GetExpirationDetails
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::GetExpirationDetails( HBufC8* aContentUri, TInt aIntent,
    CDRMRights*& aRightsObject )
    {
    RPointerArray<CDRMRights>* uriList = NULL;
    TInt r = KErrNone;
    TInt expired(0);
    TInt future(0);
    TInt valid(0);
    TTime startTime( Time::NullTTime() );
    TInt startIndex( -1 );
    DRMClock::ESecurityLevel secLevel = DRMClock::KInsecure;
    TUint32 expiration;
    CDRMRightsConstraints* constraint = NULL;
    TUint32 rightsType;
    auto_handle<RDRMClockClient> client; // autohandle deletes on scope end
    auto_handle<RDRMRightsClient> rclient; // autohandle closes on scope end
    RPointerArray<HBufC8> individuals;
    TUint32 retval = 0;
    TTime time = Time::NullTTime();

    if ( !aContentUri )
        {
        return KErrArgument;
        }

    r = client().Connect();

    if ( !r )
        {
        TInt timeZone(0);
        client().GetSecureTime(time, timeZone, secLevel);
        }

    // Client to rights client needed to retrieve the individual information:
    r = rclient().Connect();
    if ( r )
        {
        return r;
        }

    r = rclient().CheckRights( aIntent, *aContentUri, expiration );
    if ( r && expiration == EConstraintMetering )
        {
        r = CDRMRights::EExpiredRights;
        return r;
        }

    r = KErrNone;

    iDRMCommon->GetDetailedContentRights(*aContentUri, uriList);

    if ( !uriList || !uriList->Count() )
        {
        delete uriList;
        uriList = NULL;
        return KErrCANoRights;
        }


    TRAP( r, r = rclient().GetSupportedIndividualsL( individuals ) );
    rclient().Close();

    for ( TInt i = 0; i < uriList->Count(); ++i )
        {
        switch ( aIntent )
            {
            case EPlay:
                r = (*uriList)[i]->GetPlayRight(constraint);
                break;
            case EView:
                r = (*uriList)[i]->GetDisplayRight(constraint);
                break;
            case EExecute:
                r = (*uriList)[i]->GetExecuteRight(constraint);
                break;
            case EPrint:
                r = (*uriList)[i]->GetPrintRight(constraint);
                break;
            default:
                // Cleanup required when returning and error:
                // auto_handle closes client
                uriList->ResetAndDestroy(); //uriList
                delete uriList;
                uriList = NULL;
                return KErrGeneral;
            }
        if ( r )
            {
            delete constraint;
            constraint = NULL;
            continue;
            }
        constraint->GetConstraintInfo(expiration, rightsType);

        if ( secLevel != DRMClock::KSecure &&
            (rightsType & CDRMRights::ETimeBased || rightsType & CDRMRights::EInActiveInterval ))
            {
            delete constraint;
            constraint = NULL;
            continue;
            }

        if ( expiration == CDRMRights::EFutureRights )
            {
            // We already know it's not valid, ignore the return value of Valid:
            constraint->GetConstraint().Valid( time, individuals, retval);

            if ( retval & EConstraintIndividual )
                {
                ++expired;
                }
            else
                {
                if ( rightsType & CDRMRights::ETimeBased )
                    {
                    TTime tempTime;
                    if ( constraint->GetStartTime( tempTime ) != DRMCommon::ENoRights )
                        {
                        if ( startTime != Time::NullTTime() && tempTime < startTime )
                            {
                            startTime = tempTime;
                            startIndex = i;
                            }
                        else if ( startTime == Time::NullTTime() )
                            {
                            startTime = tempTime;
                            startIndex = i;
                            }
                        }
                    }
                ++future;
                }
            }
        else if ( expiration == CDRMRights::EExpiredRights )
            {
            ++expired;
            }
        else
            {
            if ( !(constraint->GetConstraint().Valid( time, individuals, retval) ) )
                {
                ++expired;
                }
            else
                {
                ++valid;
                }
            }

        delete constraint;
        constraint = NULL;
        }

    if ( valid )
        {
        r = CDRMRights::EValidRights;
        }
    else if ( future )
        {
        r = CDRMRights::EFutureRights;
        if ( startIndex >= 0 )
            {
            aRightsObject = (*uriList)[startIndex];
            (*uriList)[startIndex] = NULL;
            uriList->Remove(startIndex);
            }
        }
    else
        {
        r = CDRMRights::EExpiredRights;
        }
    // Reset the individuals
    individuals.ResetAndDestroy();
    individuals.Close();

    // autohandle closes client

    // Reset the urilist
    uriList->ResetAndDestroy(); //uriList
    delete uriList;
    uriList = NULL;

    return r;
    }

#pragma mark -

// -----------------------------------------------------------------------------
// CDRMHelper::HandleErrorOrPreviewL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::HandleErrorOrPreviewL(
    TInt aError,
    RFile& aFile,
    HBufC8*& aEmbeddedPreviewUri )
    {
    ContentAccess::CData* content( NULL );
    TPtr ptr( NULL,0 );
    TInt previewType = 0;
    TInt buttonCode = 0;
    HBufC* previewUri16 = NULL;
    HBufC* rightsIssuer = NULL;
    HBufC8* silentUrl( NULL );
    TInt r( aError );


    aEmbeddedPreviewUri = NULL;
    content = ContentAccess::CData::NewLC(
            aFile, ContentAccess::KDefaultContentObject, ContentAccess::EPeek );

    // To embed Domain RO in superdistribution case, register the content
    TRAP_IGNORE( UpdateDCFRepositoryL( aFile ) );

    if (aError != KErrCANoPermission &&
        aError != KErrCANoRights &&
        aError != KDRMErrNoRights )
        {
        // not a preview case
        HandleErrorL( aError, aFile );
        }
    else if ( !IsVideo( content ) &&
        ( silentUrl = HasSilentRightsUrlL( content ) ) != NULL )
        {
        // get rights silently if we have the URL and if we are registered
        CleanupStack::PushL( silentUrl );
        r = GetSilentRightsL( *silentUrl );
        CleanupStack::PopAndDestroy( silentUrl );
        if ( r == KErrNone )
            {
            // check if we can use the file now
            HBufC* mimeBuf( HBufC::NewLC( KMaxDataTypeLength ) );
            TPtr mimePtr( mimeBuf->Des() );
            User::LeaveIfError( content->GetStringAttribute( EMimeType,
                mimePtr ) );
            r = content->EvaluateIntent(
                static_cast<ContentAccess::TIntent>( Intent( mimeBuf ) ) );
            CleanupStack::PopAndDestroy( mimeBuf );
            mimeBuf = NULL;

            LeaveIfWrongReturnForHandleErrorOrPreviewL( r );
            if ( r != KErrNone )
                {
                HandleErrorL( r, aFile );
                }
            }
        else if ( r != KErrCancel )
            {
            HandleErrorL( aError, aFile );
            }
        }
    else
        {
        CheckPreviewUriL(content,previewUri16,previewType);
        CleanupStack::PushL(previewUri16);

        // display according to preview type
        switch (previewType)
            {
            case ContentAccess::ENoPreview:
                // no preview
                HandleErrorL(aError,aFile);
                break;
            case ContentAccess::EInstantPreview:
            case ContentAccess::EPreviewRights:
#ifdef RD_DRM_PREVIEW_RIGHT_FOR_AUDIO
                if ( iPreviewMediaType < 0 )
                    { // if no preview type set by application
                    SetPreviewMediaType( static_cast<TDRMHelperPreviewMediaType>(
                            EvaluatePreviewMediaType( content ) ) );
                    if ( iPreviewMediaType < 0 )
                        { //unable to resolve media type, cancel preview
                        HandleErrorL( aError, aFile );
                        break;
                        }
                    }
                buttonCode = DisplayPopupWindowsForPreviewL(previewType);
                aEmbeddedPreviewUri = HBufC8::NewL( previewUri16->Length() );
                aEmbeddedPreviewUri->Des().Copy( previewUri16->Des() );
#else
                HandleErrorL( aError, aFile );
#endif // RD_DRM_PREVIEW_RIGHT_FOR_AUDIO

                break;
            default:
                break;
            }
        CleanupStack::PopAndDestroy(previewUri16);
        previewUri16 = NULL;

        // behave based on user's action
        switch (buttonCode)
            {
            case 1:
                // activate rights
                delete aEmbeddedPreviewUri;
                aEmbeddedPreviewUri = NULL;
                GetRightsIssuerL( *content, rightsIssuer );
                CleanupStack::PushL( rightsIssuer );
                LaunchBrowserL( rightsIssuer );
                CleanupStack::PopAndDestroy(rightsIssuer);

                // check if we can use the file now
                if ( content->EvaluateIntent( ContentAccess::EUnknown ) == KErrNone )
                    {
                    r = KErrNone;
                    }
                else
                    {
                    r = KErrCompletion;
                    }
                break;
            case 2:
                // get preview or play preview
                if (previewType == ContentAccess::EPreviewRights)
                    {
                    CleanupStack::PushL(aEmbeddedPreviewUri);
                    CDrmHelperDownloadManager* dlMgr =
                        CDrmHelperDownloadManager::NewL();
                    CleanupStack::PushL( dlMgr );

                    if ( iUseCoeEnv )
                        {
                        TRAP(r, dlMgr->DownloadAndHandleRoapTriggerL(
                                aEmbeddedPreviewUri, *iCoeEnv ) );
                        // ignore error
                        }
                    else
                        {
                        TRAP(r, dlMgr->DownloadAndHandleRoapTriggerL( aEmbeddedPreviewUri ) );
                        // ignore error
                        }

                    CleanupStack::PopAndDestroy( dlMgr );
                    CleanupStack::PopAndDestroy( aEmbeddedPreviewUri );
                    aEmbeddedPreviewUri = NULL;

                    // check if we can use the file now
                    r = content->EvaluateIntent( ContentAccess::EUnknown );
                    LeaveIfWrongReturnForHandleErrorOrPreviewL( r );
                    }
                else
                    {
                    r = KErrNone;
                    }
                break;
            case 0:
            default:
                // cancelled or no preview
                delete aEmbeddedPreviewUri;
                aEmbeddedPreviewUri = NULL;
                if ( previewType != ContentAccess::ENoPreview )
                    {
                    r = KErrCancel;
                    }
                break;
            }
        }
    if ( iReturnCancel )
        {
        // Cancel pressed in query intitiated from HandleErrorL
        r = KErrCancel;
        }
    CleanupStack::PopAndDestroy(content);
    return r;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::HandleErrorOrPreviewL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::HandleErrorOrPreviewL(
    TInt aError,
    const TDesC& aFileName,
    HBufC8*& aEmbeddedPreviewUri )
    {
    RFile file;
    TInt r = KErrNone;

    r = file.Open(iFs, aFileName, EFileRead | EFileShareReadersOrWriters);

    if (r == KErrInUse)
        {
        r = file.Open(iFs, aFileName, EFileRead | EFileShareAny);
        if (r == KErrInUse)
            {
            r = file.Open(iFs, aFileName, EFileRead | EFileShareReadersOnly);
            }
        }
    User::LeaveIfError(r);
    CleanupClosePushL( file );
    r = HandleErrorOrPreviewL( aError, file, aEmbeddedPreviewUri );
    CleanupStack::PopAndDestroy( &file );
    return r;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::SetPreviewMediaType
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetPreviewMediaType( TDRMHelperPreviewMediaType aMediaType )
    {
    iPreviewMediaType = aMediaType;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::CheckPreviewUriL
// -----------------------------------------------------------------------------
//
void CDRMHelper::CheckPreviewUriL(
    const CData* aContent,
    HBufC*& aEmbeddedPreviewUri,
    TInt& aPreviewType )
    {
    TPtr ptr(NULL,0);
    TInt error = 0;
    aPreviewType = ContentAccess::ENoPreview;
    aEmbeddedPreviewUri = HBufC::NewLC( KUrlMaxLen );
    ptr.Set(aEmbeddedPreviewUri->Des());
    error = aContent->GetStringAttribute( EInstantPreviewUri , ptr );
    if (!error && ptr.Length()>0)
        {
        // embedded preview
        aPreviewType = ContentAccess::EInstantPreview;
        }
    else
        {
        error = aContent->GetStringAttribute( EPreviewRightsUri , ptr );
        if (!error && ptr.Length()>0)
            {
            // preview rights url
            aPreviewType = ContentAccess::EPreviewRights;
            }
        }
    if (aPreviewType == ContentAccess::ENoPreview)
        {
        CleanupStack::PopAndDestroy(aEmbeddedPreviewUri);
        aEmbeddedPreviewUri = NULL;
        }
    else
        {
        CleanupStack::Pop(aEmbeddedPreviewUri);
        }
    }

// -----------------------------------------------------------------------------
// CDRMHelper::DisplayPopupWindowsForPreviewL
// -----------------------------------------------------------------------------
//
#ifdef RD_DRM_PREVIEW_RIGHT_FOR_AUDIO
TInt CDRMHelper::DisplayPopupWindowsForPreviewL( TInt aPreviewType )
    {
    TInt buttonCode = 0;
    CAknListQueryDialog* dlg;
    TInt index( 0 );
    TInt resourceId = -1;
    TInt answer = 0;

    if (aPreviewType == ContentAccess::EPreviewRights)
        {
        // preview rights
        if (iPreviewMediaType == EPreviewTypeAudio)
            {
            resourceId = R_DRMHELPER_PREV_AUDIO_GET_LIST_QUERY;
            }
        else if (iPreviewMediaType == EPreviewTypeVideo)
            {
            resourceId = R_DRMHELPER_PREV_VIDEO_GET_LIST_QUERY;
            }
        }
    else if (aPreviewType == ContentAccess::EInstantPreview)
        {
        // embedded preview
        if (iPreviewMediaType == EPreviewTypeAudio)
            {
            resourceId = R_DRMHELPER_PREV_AUDIO_PLAY_LIST_QUERY;
            }
        else if (iPreviewMediaType == EPreviewTypeVideo)
            {
            resourceId = R_DRMHELPER_PREV_VIDEO_PLAY_LIST_QUERY;
            }
        }


    if (resourceId != -1)
        {

#ifndef RD_MULTIPLE_DRIVE

        TFileName fileName( KDriveZ );

#else //RD_MULTIPLE_DRIVE

        _LIT( KDrive, "%c:");
        TInt driveNumber( -1 );
        TChar driveLetter;
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, driveNumber );
        iFs.DriveToChar( driveNumber, driveLetter );

        TFileName fileName;

        fileName.Format( KDrive, (TUint)driveLetter );

#endif

        fileName.Append( KDC_RESOURCE_FILES_DIR );
        fileName.Append( KCDRMHelperResFileName );
        RConeResourceLoader loader( *iCoeEnv );
        loader.OpenL( fileName );
        CleanupClosePushL( loader );

        dlg = new( ELeave ) CAknListQueryDialog( &index );
        CleanupStack::PushL( dlg );

        PrepareSecondaryDisplayL( *dlg, resourceId );

        CleanupStack::Pop( dlg );

        answer = dlg->ExecuteLD( resourceId );

        CancelSecondaryDisplayL( resourceId );

        CleanupStack::PopAndDestroy( &loader );
        }

    if ( answer )
        {
        buttonCode = index+1;
        }
    else
        {
        buttonCode = 0;
        }

    iPreviewMediaType = -1; // reset preview type
    return buttonCode;
    }
#else // RD_DRM_PREVIEW_RIGHT_FOR_AUDIO
TInt CDRMHelper::DisplayPopupWindowsForPreviewL( TInt /* aPreviewType */ )
    {
    return 0;
    }
#endif // RD_DRM_PREVIEW_RIGHT_FOR_AUDIO

// -----------------------------------------------------------------------------
// CDRMHelper::HasPreviewL
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMHelper::TDRMHelperPreviewType CDRMHelper::HasPreviewL(
    CData& aContent,
    HBufC8*& aPreviewUri )
    {
    TDRMHelperPreviewType ret( ENoPreview );
    if ( iOma2 )
        {
        TInt error;
        TInt silent(0);
        TInt previewRightsType;
        TBuf<KUrlMaxLen> previewUrl;
        TInt previewType( ENoPreview );
        aContent.GetAttribute( EPreviewType, previewType );
        switch ( previewType )
            {
            case EInstantPreview:
                // get URI of embedded preview
                error = aContent.GetStringAttribute( EInstantPreviewUri,
                    previewUrl );
                if ( !error )
                    {
                    ret = EEmbeddedPreview;
                    aPreviewUri = HBufC8::NewL( previewUrl.Length() );
                    aPreviewUri->Des().Copy( previewUrl );
                    }
                break;
            case EPreviewRights:
                // Check if it is silent preview
                aContent.GetAttribute( ESilentRightsType, silent );
                previewRightsType =
                    silent ? ESilentRightsUrl : EPreviewRightsUri;
                error = aContent.GetStringAttribute( previewRightsType,
                    previewUrl );
                if ( !error )
                    {
                    ret = EPreviewRights;
                    aPreviewUri = HBufC8::NewL( previewUrl.Length() );
                    aPreviewUri->Des().Copy( previewUrl );
                    }
                break;
            default:
                // no preview
                break;
            }
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::HasPreviewL
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMHelper::TDRMHelperPreviewType CDRMHelper::HasPreviewL(
    TDesC& aFileName,
    HBufC8*& aPreviewUri )
    {
    TDRMHelperPreviewType ret( ENoPreview );
    if ( iOma2 )
        {
        TVirtualPathPtr virtualPath( aFileName );

        CData* content = NULL;
        TRAPD( r, content = CData::NewL( virtualPath, EPeek, EContentShareReadWrite ) );
        if ( r == KErrInUse )
            {
            content = CData::NewL( virtualPath, EPeek, EContentShareReadOnly );
            }
        else
            {
            User::LeaveIfError( r );
            }

        CleanupStack::PushL( content );

        ret = HasPreviewL( *content, aPreviewUri );
        CleanupStack::PopAndDestroy( content );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::GetPreviewRightsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::GetPreviewRightsL( CData& aContent )
    {
    if ( iOma2 )
        {
        HBufC8* previewUri = NULL;
        TDRMHelperPreviewType previewType = HasPreviewL( aContent, previewUri );
        CleanupStack::PushL( previewUri );
        if ( previewType == EPreviewRights )
            {
            TInt silent(0);
            // Check if it is silent preview
            aContent.GetAttribute( ESilentRightsType, silent );
            if ( silent )
                {
                CDrmHelperDownloadManager* dlMgr =
                    CDrmHelperDownloadManager::NewL();
                CleanupStack::PushL( dlMgr );

                if ( iUseCoeEnv )
                    {
                    dlMgr->DownloadAndHandleRoapTriggerL( previewUri, *iCoeEnv );
                    }
                else
                    {
                    dlMgr->DownloadAndHandleRoapTriggerL( previewUri );
                    }

                CleanupStack::PopAndDestroy( dlMgr );
                }
            else
                {
                // launch browser
                LaunchBrowserL( previewUri );
                }
            }
        else
            {
            // No preview rights available, this function should not be called
            User::Leave( KErrArgument );
            }
        CleanupStack::PopAndDestroy( previewUri );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }


// -----------------------------------------------------------------------------
// CDRMHelper::GetPreviewRightsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::GetPreviewRightsL(TDesC& aFileName )
    {
    if ( iOma2 )
        {
        TVirtualPathPtr virtualPath( aFileName );

        CData* content( NULL );
        TRAPD( r, content = CData::NewL( virtualPath, EPeek, EContentShareReadWrite ) );
        if ( r == KErrInUse )
            {
            content = CData::NewL( virtualPath, EPeek, EContentShareReadOnly );
            }
        else
            {
            User::LeaveIfError( r );
            }

        CleanupStack::PushL( content );

        GetPreviewRightsL( *content );
        CleanupStack::PopAndDestroy( content );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }


// -----------------------------------------------------------------------------
// CDRMHelper::EmbeddedPreviewCompletedL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMHelper::EmbeddedPreviewCompletedL( CData& aContent )
    {
    TBool proceeded = EFalse;
    if ( iOma2 )
        {
        TInt error = KErrNone;
        TInt canPlay;
        HBufC* rightsIssuer = NULL;
        TInt buyRights(0);

        error = aContent.GetAttribute( ECanPlay, canPlay );
        if ( !error && !canPlay )
            {
            TFileName fileName;

            // Get RI URL
            GetRightsIssuerL( aContent, rightsIssuer );
            CleanupStack::PushL( rightsIssuer );
            // Ask if user wants to buy rights
            error = aContent.GetStringAttribute( EFileName, fileName );
            if ( !error )
                {
                if ( buyRights )
                    {
                    // launch browser
                    LaunchBrowserL( rightsIssuer );
                    proceeded = ETrue;
                    }
                else
                    {
                    proceeded = EFalse;
                    }
                }
            else
                {
                User::Leave( KErrArgument );
                }
            CleanupStack::PopAndDestroy( rightsIssuer );
            }
        else
            {
            User::Leave( KErrArgument );
            }
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    return proceeded;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::EmbeddedPreviewCompletedL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMHelper::EmbeddedPreviewCompletedL( TDesC& aFileName )
    {
    TBool proceeded( EFalse );
    if ( iOma2 )
        {
        TVirtualPathPtr virtualPath( aFileName );

        CData* content( NULL );
        TRAPD( r, content = CData::NewL( virtualPath, EPeek, EContentShareReadWrite ) );
        if ( r == KErrInUse )
            {
            content = CData::NewL( virtualPath, EPeek, EContentShareReadOnly );
            }
        else
            {
            User::LeaveIfError( r );
            }

        CleanupStack::PushL( content );

        proceeded = EmbeddedPreviewCompletedL( *content );
        CleanupStack::PopAndDestroy( content );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    return proceeded;
    }

#ifdef RD_DRM_SILENT_RIGHTS_ACQUISITION
HBufC8* CDRMHelper::HasSilentRightsUrlL( CData* aContent )
    {
    HBufC* url = NULL;
    HBufC8* url8 = NULL;
    HBufC8* r = NULL;
    TPtr urlDes( NULL, 0);
    Roap::RRoapStorageClient storage;

    url = HBufC::NewLC( KMaxPath * 4 );
    urlDes.Set( url->Des() );
    if ( aContent->GetStringAttribute( ESilentRightsUrl, urlDes ) == KErrNone )
        {
        url8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *url );
        CleanupStack::PushL( url8 );
        User::LeaveIfError( storage.Connect() );
        CleanupClosePushL( storage );
        // check that SilentRightsUrl is on the white list
        // URL is not searched from pre-configured white list
        TBool fromPreConfiguredWhiteList( EFalse );
        if ( storage.WhiteListURLExistsL( *url8, fromPreConfiguredWhiteList ) )
            {
            r = url8->AllocL();
            }
        CleanupStack::PopAndDestroy( &storage );
        CleanupStack::PopAndDestroy( url8 );
        }
    CleanupStack::PopAndDestroy( url );

    return r;
    }
#else
HBufC8* CDRMHelper::HasSilentRightsUrlL( CData* /*aContent*/ )
    {
    return NULL;
    }
#endif


TInt CDRMHelper::GetSilentRightsL( const TDesC8& aUrl )
    {
    TInt r = KErrCancel;
#ifdef RD_DRM_SILENT_RIGHTS_ACQUISITION
    TInt buttonCode = EAknSoftkeyYes;
    HBufC8* url = NULL;

    if ( !SilentRightsAllowedL() )
        {
        buttonCode = DisplayQueryWithIdL( R_DRM_QRY_CONNECT_TO_ACTIVATE,
            R_DRMHELPER_CONFIRMATION_QUERY );
        }
    else if ( !BrowserAPDefinedL() )
        {
        buttonCode = DisplayQueryWithIdL( R_DRM_QRY_CONNECT_TO_ACTIVATE,
            R_DRMHELPER_CONFIRMATION_QUERY );
        }

    if ( buttonCode == EAknSoftkeyYes )
        {
        // check if there are any APs defined
        TBool APs( HasAccessPointsL() );
        if ( !APs )
            {
            // No AP defined
            DisplayInfoNoteL( R_DRM_WARN_NO_CONN_DEFINED );
            r = KErrCANoRights;
            }
        else
            {
            // ok to start download
            CDrmHelperDownloadManager* dlMgr = CDrmHelperDownloadManager::NewL();
            CleanupStack::PushL( dlMgr );
            url = aUrl.AllocLC();

            if ( iUseCoeEnv )
                {
                TRAP(r, dlMgr->DownloadAndHandleRoapTriggerL( url, *iCoeEnv ) );
                }
            else
                {
                TRAP(r, dlMgr->DownloadAndHandleRoapTriggerL( url ) );
                }
            CleanupStack::PopAndDestroy( url );

            if ( r != KErrNone )
                {
                // rights were tried to get only if the user hasn't cancelled
                if ( r != KErrCancel )
                    {
                    if ( r == KErrCouldNotConnect )
                        {
                        // Connection failed with selected AP
                        DisplayInfoNoteL( R_DRM_WARN_INVALID_OR_NO_AP );
                        r = KErrCANoRights;
                        }
                    else
                        {
                        // get error url
                        HBufC8* errorUrl = NULL;
                        errorUrl = dlMgr->GetErrorUrlL();
                        CleanupStack::PushL( errorUrl );

                        // error url is got only if temporary roap error
                        if ( errorUrl )
                            {
                            // ask user whether error url should be opened
                            buttonCode = DisplayQueryWithIdL( R_DRM_QUERY_OPEN_ERROR_URL,
                                R_DRMHELPER_CONFIRMATION_QUERY );
                            if ( buttonCode == EAknSoftkeyYes )
                                {
                                // Launch browser
                                LaunchBrowserL( errorUrl );
                                }
                            }
                        else
                            {
                            DisplayInfoNoteL( R_DRM_ERR_OPENING_FAIL_PERM );
                            }

                        CleanupStack::PopAndDestroy( errorUrl );
                        }
                    }
                }
            CleanupStack::PopAndDestroy( dlMgr );
            }
        }
#endif // RD_DRM_SILENT_RIGHTS_ACQUISITION
    return r;
    }


TBool CDRMHelper::SilentRightsAllowedL()
    {
#ifdef RD_DRM_SILENT_RIGHTS_ACQUISITION
    TInt value = KErrNone;
    CRepository* repository = CRepository::NewL( KCRUidDRMSettings );
    repository->Get( KDRMSettingsSilentRightsAcquisition, value );
    delete repository;
    return (value == 0 ? EFalse: ETrue);
#else
    return EFalse;
#endif
    }


// -----------------------------------------------------------------------------
// CDRMHelper::CheckRightsPercentL
// Note: obsolete function name kept only
// to avoid SC break on Helper selection logic
// -----------------------------------------------------------------------------
//
TBool CDRMHelper::BrowserAPDefinedL()
    {
    TBool apFound( EFalse );
    TRAP_IGNORE( apFound = HasDefaultConnectionL() );
    return apFound;
    }


#pragma mark -

// -----------------------------------------------------------------------------
// CDRMHelper::CheckRightsPercentL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::CheckRightsPercentL(
    const TDesC& aFileName,
    TInt aThreshold )
    {
    TInt ret(0);
    HBufC* rightsIssuer = NULL;
    HBufC* mimeType = NULL;
    HBufC* contentURI = NULL;
    HBufC8* contentURI8 = NULL;
    TInt error( KErrNone );

    // check that threshold value is within bounds
    if ( aThreshold < 0 || aThreshold > 100 )
        {
        User::Leave( KErrArgument );
        }

    TVirtualPathPtr virtualPath( aFileName, KDefaultContentObject );

    CData* content = NULL;
    TRAPD( r, content = CData::NewL( virtualPath, EPeek, EContentShareReadWrite ) );
    if ( r == KErrInUse )
        {
        content = CData::NewL( virtualPath, EPeek, EContentShareReadOnly );
        }
    else
        {
        User::LeaveIfError( r );
        }

    CleanupStack::PushL( content );

    // get rights-issuer URL
    TRAPD(err, GetRightsIssuerL( *content, rightsIssuer ) );
    if (!err )
        {
        CleanupStack::PushL( rightsIssuer );
        }

    mimeType = HBufC::NewLC( KMaxDataTypeLength );
    contentURI = HBufC::NewLC( KUrlMaxLen );

    // create attribute set
    RStringAttributeSet stringAttributeSet;
    CleanupClosePushL( stringAttributeSet );
    // add the attributes we are interested in
    stringAttributeSet.AddL( EContentID );
    stringAttributeSet.AddL( EMimeType );

    User::LeaveIfError( content->GetStringAttributeSet( stringAttributeSet ));

    // pass on values of string attributes
    TPtr ptr = contentURI->Des();
    error = stringAttributeSet.GetValue( EContentID, ptr );
    if ( error == KErrNone )
        {
        TPtr ptr2 = mimeType->Des();
        error = stringAttributeSet.GetValue( EMimeType, ptr2 );
        if ( error == KErrNone )
            {
            contentURI8 = HBufC8::NewLC( contentURI->Length() );
            contentURI8->Des().Copy( contentURI->Des() );
            ret = DoCheckRightsPercentL(
                contentURI8,
                mimeType,
                rightsIssuer,
                StripPathAndExtension( aFileName ),
                aThreshold );
            CleanupStack::PopAndDestroy( contentURI8 );
            }
        else
            {
            User::Leave( KErrArgument );
            }
        }
    else
        {
        User::Leave( KErrArgument );
        }

    CleanupStack::PopAndDestroy(3, mimeType); // stringAttributeSet.Close(), contentURI, mimeType

    if ( !err )
        {
        CleanupStack::PopAndDestroy( rightsIssuer );
        }

    CleanupStack::PopAndDestroy( content );
    return ret;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::CheckRightsPercentL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::CheckRightsPercentL(
    RFile& aFileHandle,
    TInt aThreshold )
    {
    TInt ret(0);
    HBufC* rightsIssuer = NULL;
    HBufC* mimeType = NULL;
    HBufC* contentURI = NULL;
    HBufC8* contentURI8 = NULL;
    TFileName fileName;
    TInt error;

    // check that threshold value is within bounds
    if ( aThreshold < 0 || aThreshold > 100 )
        {
        User::Leave( KErrArgument );
        }

    CData* content = CData::NewLC( aFileHandle, KDefaultContentObject, EPeek );

    // get rights-issuer URL
    TRAPD(err, GetRightsIssuerL( *content, rightsIssuer ) );
    if (!err )
        {
        CleanupStack::PushL( rightsIssuer );
        }

    mimeType = HBufC::NewLC( KMaxDataTypeLength );
    contentURI = HBufC::NewLC( KUrlMaxLen );

    // create attribute set
    RStringAttributeSet stringAttributeSet;
    CleanupClosePushL( stringAttributeSet );
    // add the attributes we are interested in
    stringAttributeSet.AddL( EContentID );
    stringAttributeSet.AddL( EMimeType );

    User::LeaveIfError( content->GetStringAttributeSet( stringAttributeSet ));

    // pass on values of string attributes
    TPtr ptr = contentURI->Des();
    error = stringAttributeSet.GetValue( EContentID, ptr );
    if ( error == KErrNone )
        {
        TPtr ptr2 = mimeType->Des();
        error = stringAttributeSet.GetValue( EMimeType, ptr2 );
        if ( error == KErrNone )
            {
            aFileHandle.Name( fileName );
            contentURI8 = HBufC8::NewLC( contentURI->Length() );
            contentURI8->Des().Copy( contentURI->Des() );
            ret = DoCheckRightsPercentL(
                contentURI8,
                mimeType,
                rightsIssuer,
                StripPathAndExtension( fileName ),
                aThreshold );
            CleanupStack::PopAndDestroy( contentURI8 );
            }
        else
            {
            User::Leave( KErrArgument );
            }
        }
    else
        {
        User::Leave( KErrArgument );
        }

    CleanupStack::PopAndDestroy(3, mimeType); // stringAttributeSet.Close(), contentURI, mimeType

    if ( !err )
        {
        CleanupStack::PopAndDestroy( rightsIssuer );
        }

    CleanupStack::PopAndDestroy( content );
    return ret;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::CheckRightsAmountL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::CheckRightsAmountL(
    const TDesC& aFileName,
    TInt aCount,
    TInt aDays )
    {
    TInt ret(0);
    HBufC* rightsIssuer = NULL;
    HBufC* mimeType = NULL;
    HBufC* contentURI = NULL;
    HBufC8* contentURI8 = NULL;
    TInt error( KErrNone );
    TInt drmProtected(0);

    // Leave if limit is negative
    if ( aCount < 0 || aDays < 0 )
        {
        User::Leave( KErrArgument );
        }

    TVirtualPathPtr virtualPath( aFileName, KDefaultContentObject );

    CData* content( NULL );
    TRAPD( r, content = CData::NewL( virtualPath, EPeek, EContentShareReadWrite ) );
    if ( r == KErrInUse )
        {
        content = CData::NewL( virtualPath, EPeek, EContentShareReadOnly );
        }
    else
        {
        User::LeaveIfError( r );
        }

    CleanupStack::PushL( content );

    User::LeaveIfError( content->GetAttribute( EIsProtected, drmProtected ) ) ;
    if ( drmProtected ) // do nothing if non-DRM file
        {
        // get rights-issuer URL
        TRAPD(err, GetRightsIssuerL( *content, rightsIssuer ) );
        if (!err )
            {
            CleanupStack::PushL( rightsIssuer );
            }

        mimeType = HBufC::NewLC( KMaxDataTypeLength );
        contentURI = HBufC::NewLC( KUrlMaxLen );

        // create attribute set
        RStringAttributeSet stringAttributeSet;
        CleanupClosePushL( stringAttributeSet );
        // add the attributes we are interested in
        stringAttributeSet.AddL( EContentID );
        stringAttributeSet.AddL( EMimeType );

        User::LeaveIfError( content->GetStringAttributeSet( stringAttributeSet ));

        // pass on values of string attributes
        TPtr ptr = contentURI->Des();
        error = stringAttributeSet.GetValue( EContentID, ptr );
        if ( error == KErrNone )
            {
            TPtr ptr2 = mimeType->Des();
            error = stringAttributeSet.GetValue( EMimeType, ptr2 );
            if ( error == KErrNone )
                {
                contentURI8 = HBufC8::NewLC( contentURI->Length() );
                contentURI8->Des().Copy( contentURI->Des() );
                ret = DoCheckRightsAmountL(
                    contentURI8,
                    mimeType,
                    rightsIssuer,
                    StripPathAndExtension( aFileName ),
                    aCount,
                    aDays );
                CleanupStack::PopAndDestroy( contentURI8 );
                }
            else
                {
                User::Leave( KErrArgument );
                }
            }
        else
            {
            User::Leave( KErrArgument );
            }

        CleanupStack::PopAndDestroy(3, mimeType); // stringAttributeSet.Close(), contentURI, mimeType

        if ( !err )
            {
            CleanupStack::PopAndDestroy( rightsIssuer );
            }
        }

    CleanupStack::PopAndDestroy( content );
    return ret;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::CheckRightsAmountL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::CheckRightsAmountL(
    RFile& aFileHandle,
    TInt aCount,
    TInt aDays )
    {
    TInt ret(0);
    HBufC* rightsIssuer = NULL;
    HBufC* mimeType = NULL;
    HBufC* contentURI = NULL;
    HBufC8* contentURI8 = NULL;
    TFileName fileName;
    TInt error;
    TInt drmProtected(0);

    // Leave if limit is negative
    if ( aCount < 0 || aDays < 0 )
        {
        User::Leave( KErrArgument );
        }

    CData* content = CData::NewLC( aFileHandle, KDefaultContentObject, EPeek );
    User::LeaveIfError( content->GetAttribute( EIsProtected, drmProtected ) ) ;
    if ( drmProtected ) // do nothing if non-DRM file
        {
        // get rights-issuer URL
        TRAPD(err, GetRightsIssuerL( *content, rightsIssuer ) );
        if (!err )
            {
            CleanupStack::PushL( rightsIssuer );
            }

        mimeType = HBufC::NewLC( KMaxDataTypeLength );
        contentURI = HBufC::NewLC( KUrlMaxLen );

        // create attribute set
        RStringAttributeSet stringAttributeSet;
        CleanupClosePushL( stringAttributeSet );
        // add the attributes we are interested in
        stringAttributeSet.AddL( EContentID );
        stringAttributeSet.AddL( EMimeType );

        User::LeaveIfError( content->GetStringAttributeSet( stringAttributeSet ));

        // pass on values of string attributes
        TPtr ptr = contentURI->Des();
        error = stringAttributeSet.GetValue( EContentID, ptr );
        if ( error == KErrNone )
            {
            TPtr ptr2 = mimeType->Des();
            error = stringAttributeSet.GetValue( EMimeType, ptr2 );
            if ( error == KErrNone )
                {
                aFileHandle.Name( fileName );
                contentURI8 = HBufC8::NewLC( contentURI->Length() );
                contentURI8->Des().Copy( contentURI->Des() );
                ret = DoCheckRightsAmountL(
                    contentURI8,
                    mimeType,
                    rightsIssuer,
                    StripPathAndExtension( fileName ),
                    aCount,
                    aDays );
                CleanupStack::PopAndDestroy( contentURI8 );

                }
            else
                {
                User::Leave( KErrArgument );
                }
            }
        else
            {
            User::Leave( KErrArgument );
            }

        CleanupStack::PopAndDestroy(3, mimeType); // stringAttributeSet.Close(), contentURI, mimeType

        if ( !err )
            {
            CleanupStack::PopAndDestroy( rightsIssuer );
            }

        }
    CleanupStack::PopAndDestroy( content );
    return ret;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::CheckRightsAmountL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::CheckRightsAmountL(
    const TDesC8& aUri,
    TInt aCount,
    TInt aDays )
    {
    TInt buttonCode( 0 );
    RFile fileHandle;

    CleanupClosePushL( fileHandle );
    if ( !GetFileHandleFromURIL( aUri, fileHandle ) )
        {
        buttonCode = CheckRightsAmountL( fileHandle, aCount, aDays );
        }
    CleanupStack::PopAndDestroy( &fileHandle );
    return buttonCode;
    }



// -----------------------------------------------------------------------------
// CDRMHelper::DoCheckRightsPercentL
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::DoCheckRightsPercentL(
    HBufC8* aContentUri,
    HBufC* aMimeType,
    HBufC* aRIUrl,
    const TDesC& aFileName,
    TInt aThreshold )
    {
    TInt buttonCode(0);
    TUint32 originalValue(0);
    TTimeIntervalDays duration(0);
    CDRMRightsConstraints* constraint = NULL;
    TInt mask(0);
    CDRMRights::TRestriction restriction;
    CDRMRights::TExpiration expiration;
    TUint32 constType;

    // get original amount of rights
    if ( aContentUri )
        {
        TInt intent = Intent( aMimeType );
        CDRMRights* rights = NULL; // rights object

        if ( iDRMCommon->GetActiveRights( *aContentUri, intent, rights ) ==
            DRMCommon::ENoRights )
            {
            //no rights for given content URI, handle error
            buttonCode = DoHandleErrorL( DRMCommon::ENoRights, aContentUri,
                aMimeType, aRIUrl, NULL, NULL, NULL, NULL, aFileName );
            }
        else
            {
            User::LeaveIfNull( rights );

            rights->GetRightsInfo( intent, restriction, expiration,
                constType );

            // No need to do anything for full or preview rights
            if ( restriction == CDRMRights::ERestrictedRights )
                {
                if ( expiration == CDRMRights::EExpiredRights ||
                    expiration == CDRMRights::EFutureRights )
                    {
                    // rights expired or not yet valid for given content URI,
                    // handle error
                    buttonCode = DoHandleErrorL( DRMCommon::ERightsExpired,
                        aContentUri, aMimeType, aRIUrl, NULL, NULL, NULL, NULL, aFileName );
                    }
                else
                    {
                    if ( (intent == DRMCommon::EPlay) &&
                        rights->GetPlayRight( constraint ) ==
                        DRMCommon::EOk )
                        {
                        mask = GetCounts( constraint, originalValue,
                            duration );
                        }
                    delete constraint;
                    constraint = NULL;

                    if ( (intent == DRMCommon::EDisplay) &&
                        rights->GetDisplayRight( constraint ) ==
                        DRMCommon::EOk )
                        {
                        mask = GetCounts( constraint, originalValue,
                            duration );
                        }
                    delete constraint;
                    constraint = NULL;

                    if ( (intent == DRMCommon::EPrint) &&
                        rights->GetPrintRight( constraint ) ==
                        DRMCommon::EOk )
                        {
                        mask = GetCounts( constraint, originalValue,
                            duration );
                        }
                    delete constraint;
                    constraint = NULL;

                    if ( (intent == DRMCommon::EExecute) &&
                        rights->GetExecuteRight( constraint ) ==
                        DRMCommon::EOk )
                        {
                        mask = GetCounts( constraint, originalValue,
                            duration );
                        }
                    delete constraint;
                    constraint = NULL;
                    }
                }
            delete rights;
            }

        // calculate new threshold values and call next function
        switch ( mask )
            {
            case KDRMHelperConstraintCount:
                buttonCode = DoCheckRightsAmountL( aContentUri,
                    aMimeType, aRIUrl, aFileName,
                    (aThreshold * originalValue) / 100,
                    KDRMHelperDefaultTimeThreshold );
                break;

            case KDRMHelperConstraintTime:
                buttonCode = DoCheckRightsAmountL( aContentUri,
                    aMimeType, aRIUrl, aFileName,
                    KDRMHelperDefaultCountThreshold,
                    (aThreshold * duration.Int()) / 100 );
                break;

            case (KDRMHelperConstraintCount | KDRMHelperConstraintTime):
                buttonCode = DoCheckRightsAmountL( aContentUri,
                    aMimeType, aRIUrl, aFileName,
                    (aThreshold * originalValue) / 100,
                    (aThreshold * duration.Int()) / 100 );
                break;

            default:
                break;
            }
        }
    return buttonCode;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::DoCheckRightsAmountL
// -----------------------------------------------------------------------------
//
#ifndef RD_DRM_REMOVAL_OF_EXPIRATION_WARNING_PROMPTS
TInt CDRMHelper::DoCheckRightsAmountL(
    HBufC8* aContentUri,
    HBufC* aMimeType,
    HBufC* aRIUrl,
    const TDesC& aFileName,
    TInt aCount,
    TInt aDays )
#else // RD_DRM_REMOVAL_OF_EXPIRATION_WARNING_PROMPTS
TInt CDRMHelper::DoCheckRightsAmountL(
    HBufC8* aContentUri,
    HBufC* aMimeType,
    HBufC* aRIUrl,
    const TDesC& aFileName,
    TInt /* aCount */,
    TInt /* aDays */ )
#endif // RD_DRM_REMOVAL_OF_EXPIRATION_WARNING_PROMPTS
    {
    TInt errorButton(0); // button code returned by HandleErrorL
    TInt buttonCode(0); // button code to be returned
#ifndef RD_DRM_REMOVAL_OF_EXPIRATION_WARNING_PROMPTS
    TUint32 countsLeft(0); // number of counts left
    TTime endTime; // end time of time-based rights
    TTimeIntervalDays daysLeft(0); // days until end of time-based rights
    TTimeIntervalMinutes minutesLeft(0); // minutes until end of time-based rights
    TInt mask(0); // tells if object has count or time based rights
#endif // RD_DRM_REMOVAL_OF_EXPIRATION_WARNING_PROMPTS
    TTime now; // current time
    RDRMRightsClient client;
    CDRMPointerArray<HBufC8>* individual = NULL;
    TUint32 reason = 0;

    now.HomeTime();
    if ( aContentUri )
        {
        TIntent intent = static_cast<TIntent>( Intent( aMimeType ) );
        TRightsType type;
        CDRMPermission* permission = NULL;

        User::LeaveIfError( client.Connect() );
        CleanupClosePushL( client );
        permission = client.GetActiveRightsL( intent, *aContentUri, reason );

        if ( !permission )
            {
            CleanupStack::PopAndDestroy( &client );
            //no rights for given content URI, handle error
            errorButton = DoHandleErrorL( DRMCommon::ENoRights, aContentUri,
                aMimeType, aRIUrl, NULL, NULL, NULL, NULL, aFileName );
            }
        else
            {
            // rights object found
            CleanupStack::PushL( permission );
            individual = CDRMPointerArray<HBufC8>::NewLC();
            individual->SetAutoCleanup(ETrue);
            client.GetSupportedIndividualsL( *individual );
            switch ( intent )
                {
                case EPlay: type = ERightsPlay; break;
                case EView: type = ERightsDisplay; break;
                case EPrint: type = ERightsPrint; break;
                case EExecute: type = ERightsExecute; break;
                default: type = ERightsAll; break;
                }
            if ( !permission->Valid( now, *individual, reason, type ) )
                {
                //rights expired for given content URI, handle error
                errorButton = DoHandleErrorL(
                    DRMCommon::ERightsExpired, aContentUri,
                    aMimeType, aRIUrl, NULL, NULL, NULL, NULL, aFileName );
                }
#ifndef RD_DRM_REMOVAL_OF_EXPIRATION_WARNING_PROMPTS
            else
                {
                CDRMConstraint* constraint;
                constraint = permission->ConstraintForIntent( intent );
                if ( constraint )
                    {
                    _LIT8( KForwardLockCidStartString, "flk:" );
                    TInt matchFound =
                        aContentUri->FindF( KForwardLockCidStartString );
                    // Don't display about to expire note for preview
                    // note SD with 1 count is not a preview
                    // unless in FL and not having RI URL
                    if ( constraint->iOriginalCounter != 1 ||
                        matchFound == KErrNotFound ||
                        matchFound > 0 || // flk not found at beginning
                        ( aRIUrl && aRIUrl->Length() > 0 ) )
                        {
                        mask = GetCounts( constraint, countsLeft, endTime );
                        }
                    }
                }
#endif // RD_DRM_REMOVAL_OF_EXPIRATION_WARNING_PROMPTS
            CleanupStack::PopAndDestroy( individual );
            CleanupStack::PopAndDestroy( permission );
            CleanupStack::PopAndDestroy( &client );
            }

#ifndef RD_DRM_REMOVAL_OF_EXPIRATION_WARNING_PROMPTS
        // calculate days left in case of time based constraint
        if ( mask & ( KDRMHelperConstraintTime | KDRMHelperConstraintAccumulated ) )
            {
            daysLeft = endTime.DaysFrom( now );
            endTime.MinutesFrom( now, minutesLeft );
            }

        // display correct warning note if needed.
        switch ( mask )
            {
            case KDRMHelperConstraintCount:
                if ( countsLeft == 1 )
                    {
                    if ( aRIUrl && aRIUrl->Length() > 0 )
                        {
                        buttonCode = DisplayQueryWithIdL(
                            R_DRMHELPER_QUERY_EXP_1_USE_GET_NEW,
                            R_DRMHELPER_CONFIRMATION_QUERY );
                        }
                    else
                        {
                        DisplayInfoNoteL( R_DRMHELPER_INFO_EXP_1_USE );
                        }
                    }
                else
                    {
                    if ( countsLeft <= (TUint32)aCount )
                        {
                        if ( aRIUrl && aRIUrl->Length() > 0 )
                            {
                            buttonCode = DisplayQueryL(
                                R_DRMHELPER_QUERY_EXP_N_USE_GET_NEW,
                                countsLeft );
                            }
                        else
                            {
                            DisplayInfoNoteL( R_DRMHELPER_INFO_EXP_N_USES , countsLeft );
                            }
                        }

                    }
                break;

            case KDRMHelperConstraintTime:
                if ( daysLeft.Int() == 1 )
                    {
                    if ( aRIUrl && aRIUrl->Length() > 0 )
                        {
                        buttonCode = DisplayQueryWithIdL(
                            R_DRMHELPER_QUERY_EXP_1_DAY_GET_NEW,
                            R_DRMHELPER_CONFIRMATION_QUERY );
                        }
                    else
                        {
                        DisplayInfoNoteL( R_DRMHELPER_INFO_EXP_1_DAY );
                        }
                    }
                else
                    {
                    if ( daysLeft.Int() < aDays )
                        {
                        if ( aRIUrl && aRIUrl->Length() > 0 )
                            {
                            buttonCode = DisplayQueryL(
                                R_DRMHELPER_QUERY_EXP_N_DAY_GET_NEW,
                                daysLeft.Int() );
                            }
                        else
                            {
                            DisplayInfoNoteL( R_DRMHELPER_INFO_EXP_N_DAYS , daysLeft.Int() );
                            }
                        }
                    }
                break;

            case KDRMHelperConstraintAccumulated:
                if ( minutesLeft.Int() == 1 )
                    {
                    buttonCode = DisplayQueryWithIdL(
                        R_DRMHELPER_QUERY_EXP_1_MIN_GET_NEW,
                        R_DRMHELPER_CONFIRMATION_QUERY );
                    }
                else
                    {
                    if ( daysLeft.Int() < KDRMHelperAccumulatedTimeLimit )
                        {
                        buttonCode = DisplayQueryL(
                            R_DRMHELPER_QUERY_EXP_N_MIN_GET_NEW,
                            minutesLeft.Int() );
                        }
                    }
                break;

            case (KDRMHelperConstraintTime | KDRMHelperConstraintAccumulated):
            case (KDRMHelperConstraintCount | KDRMHelperConstraintAccumulated):
            case (KDRMHelperConstraintCount | KDRMHelperConstraintTime):
                if ( countsLeft <= (TUint32)aCount || daysLeft.Int() < aDays )
            {
            if ( aRIUrl && aRIUrl->Length() > 0 )
                {
                buttonCode = DisplayQueryWithIdL(
                    R_DRMHELPER_QUERY_EXP_MANY_CONSTR,
                    R_DRMHELPER_CONFIRMATION_QUERY );

                }
            else
                {
                buttonCode = DisplayQueryWithIdL(
                    R_DRMHELPER_USAGE_RIGHTS_TO_EXPIRE,
                    R_DRMHELPER_CONFIRMATION_QUERY );
                if ( buttonCode )
                    {
                    LaunchDetailsViewEmbeddedL( aContentUri );
                    buttonCode = 0;
                    }
                }

            }
                break;

            default:
                break;
            }
#endif // RD_DRM_REMOVAL_OF_EXPIRATION_WARNING_PROMPTS

        // Launch details view if needed
        if ( buttonCode == EAknSoftkeyYes )
            {
            LaunchBrowserL( aRIUrl );
            }

        if ( errorButton )
            {
            buttonCode = errorButton;
            }
        }
    return buttonCode;
    }

EXPORT_C void CDRMHelper::SetCountLimitL( TUint /*aCounts*/ )
    {
    }

EXPORT_C void CDRMHelper::SetTimeLimitL( TUint /*aDays*/ )
    {
    }

EXPORT_C void CDRMHelper::SetPercentageLimitL( TUint /*aPercentage*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelper::GetCounts
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::GetCounts(
    CDRMRightsConstraints* aConstraint,
    TUint32& aOriginalCount,
    TTimeIntervalDays& aDuration )
    {
    TInt ret(0);
    TTime startTime;
    TTime endTime;
    TUint32 countsLeft;
    TTimeIntervalSeconds interval;
    TUint32 count(0);

    aOriginalCount = KMaxTUint32;
    interval = 0;
    aDuration = 0;
    if ( aConstraint->GetCounters( countsLeft, count ) ==
        DRMCommon::EOk )
        {
        ret |= KDRMHelperConstraintCount;
        aOriginalCount = count;
        }
    if ( aConstraint->GetTimedCounters( countsLeft, count ) ==
        DRMCommon::EOk )
        {
        ret |= KDRMHelperConstraintCount;
        aOriginalCount = Min( count, aOriginalCount );
        }
    if ( aConstraint->GetStartTime( startTime ) == DRMCommon::EOk )
        {
        aConstraint->GetEndTime( endTime );
        aDuration = endTime.DaysFrom( startTime );
        ret |= KDRMHelperConstraintTime;
        }
    if ( aConstraint->GetInterval( interval ) == DRMCommon::EOk )
        {
        // change to days
        aDuration = interval.Int() / (60*60*24);
        ret |= KDRMHelperConstraintTime;
        }
    if ( aConstraint->GetAccumulated( interval ) == DRMCommon::EOk )
        {
        // change to days
        aDuration = Min( aDuration.Int(), interval.Int() / (60*60*24) );
        ret |= KDRMHelperConstraintAccumulated;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::GetCounts
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::GetCounts(
    CDRMRightsConstraints* aConstraint,
    TUint32& aCountsLeft,
    TTime& aEndTime )
    {
    TInt ret(0);
    TUint32 originalCount;
    TTime start;
    TTimeIntervalSeconds interval;
    TUint32 count(0);

    aCountsLeft = KMaxTUint32;
    aEndTime = Time::NullTTime();
    if ( aConstraint->GetCounters( count, originalCount ) ==
        DRMCommon::EOk )
        {
        ret |= KDRMHelperConstraintCount;
        aCountsLeft = count;
        }
    if ( aConstraint->GetTimedCounters( count, originalCount ) ==
        DRMCommon::EOk )
        {
        ret |= KDRMHelperConstraintCount;
        aCountsLeft = Min( count, aCountsLeft );
        }
    if ( aConstraint->GetEndTime( aEndTime ) == DRMCommon::EOk )
        {
        ret |= KDRMHelperConstraintTime;
        }
    if ( aConstraint->GetInterval( interval ) == DRMCommon::EOk )
        {
        if ( aConstraint->GetIntervalStart( start ) == DRMCommon::ENoRights )
            {
            // interval not yet started
            start.HomeTime();
            }
        aEndTime = start + interval;
        ret |= KDRMHelperConstraintTime;
        }
    if ( aConstraint->GetAccumulated( interval ) == DRMCommon::EOk )
        {
        if ( aEndTime != Time::NullTTime() &&
            aEndTime + interval > aEndTime ||
            aEndTime == Time::NullTTime() )
            {
            aEndTime.HomeTime();
            aEndTime = aEndTime + interval;
            ret |= KDRMHelperConstraintAccumulated;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::GetCounts
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::GetCounts(
    CDRMConstraint* aConstraint,
    TUint32& aOriginalCount,
    TTimeIntervalDays& aDuration )
    {
    TInt ret(0);
    TTime endTime;

    aOriginalCount = KMaxTUint32;
    aDuration = 0;
    if ( aConstraint->iActiveConstraints & EConstraintCounter ||
        aConstraint->iActiveConstraints & EConstraintTimedCounter )
        {
        ret |= KDRMHelperConstraintCount;
        aOriginalCount = aConstraint->iOriginalCounter;
        }
    if ( aConstraint->iActiveConstraints & EConstraintStartTime &&
        aConstraint->iActiveConstraints & EConstraintEndTime )
        {
        aDuration = aConstraint->iEndTime.DaysFrom( aConstraint->iStartTime );
        ret |= KDRMHelperConstraintTime;
        }
    if ( aConstraint->iActiveConstraints & EConstraintInterval )
        {
        // change to days
        aDuration = aConstraint->iInterval.Int() / ( 60 * 60 * 24 );
        ret |= KDRMHelperConstraintTime;
        }
    if ( aConstraint->iActiveConstraints & EConstraintAccumulated )
        {
        // change to days
        aDuration = Min( aDuration.Int(),
            aConstraint->iAccumulatedTime.Int() / (60*60*24) );
        ret |= KDRMHelperConstraintAccumulated;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::GetCounts
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::GetCounts(
    CDRMConstraint* aConstraint,
    TUint32& aCountsLeft,
    TTime& aEndTime )
    {
    TInt ret(0);
    TTime start;

    aCountsLeft = KDRMCounterMax;
    aEndTime = Time::NullTTime();
    if ( aConstraint->iActiveConstraints & EConstraintCounter )
        {
        ret |= KDRMHelperConstraintCount;
        aCountsLeft = aConstraint->iCounter;
        }
    if ( aConstraint->iActiveConstraints & EConstraintTimedCounter )
        {
        ret |= KDRMHelperConstraintCount;
        if ( aConstraint->iTimedCounter < aCountsLeft )
            {
            aCountsLeft = aConstraint->iTimedCounter;
            }
        }
    if ( aConstraint->iActiveConstraints & EConstraintEndTime )
        {
        ret |= KDRMHelperConstraintTime;
        aEndTime = aConstraint->iEndTime;
        }
    if ( aConstraint->iActiveConstraints & EConstraintInterval )
        {
        start = aConstraint->iIntervalStart;
        if ( start == Time::NullTTime() )
            {
            start.HomeTime();
            }
        aEndTime = start + aConstraint->iInterval;
        ret |= KDRMHelperConstraintTime;
        }
    if ( aConstraint->iActiveConstraints & EConstraintAccumulated )
        {
        if ( aEndTime != Time::NullTTime() &&
            aEndTime + aConstraint->iAccumulatedTime > aEndTime ||
            aEndTime == Time::NullTTime() )
            {
            aEndTime.HomeTime();
            aEndTime = aEndTime + aConstraint->iAccumulatedTime;
            ret |= KDRMHelperConstraintAccumulated;
            }
        }
    return ret;
    }

#pragma mark -

// -----------------------------------------------------------------------------
// CDRMHelper::LaunchDetailsViewEmbeddedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::LaunchDetailsViewEmbeddedL( const HBufC8* aContentURI )
    {
    CDRMRights* rights = NULL; // rights object

    if ( iDRMCommon->GetActiveRights( *aContentURI, 0, rights ) ==
        DRMCommon::ENoRights )
        {
        //no rights for given content URI, leave
        User::Leave( DRMCommon::ENoRights );
        }
    else
        {
        User::LeaveIfNull( rights );

        // get local ID
        TUint32 localId = rights->GetLocalID();
        HBufC* launchParam;


        // create string needed to launch rights manager ui embedded.
        CreateLaunchParamL( localId, aContentURI, launchParam );
        CleanupStack::PushL( launchParam );
        LaunchRightsManagerUiL( *launchParam );
        CleanupStack::PopAndDestroy( launchParam );

        }

    // delete rights
    delete rights;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::LaunchDetailsViewEmbeddedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::LaunchDetailsViewEmbeddedL( const TDesC& aFileName )
    {
    HBufC* contentURI( NULL );
    HBufC8* contentURI8( NULL );
    TInt error( KErrNone );

    TVirtualPathPtr virtualPath( aFileName, KDefaultContentObject );

    CData* content( NULL );
    TRAPD( r, content = CData::NewL( virtualPath, EPeek, EContentShareReadWrite ) );
    if ( r == KErrInUse )
        {
        content = CData::NewL( virtualPath, EPeek, EContentShareReadOnly );
        }
    else
        {
        User::LeaveIfError( r );
        }

    CleanupStack::PushL( content );

    contentURI = HBufC::NewLC( KUrlMaxLen );
    TPtr ptr = contentURI->Des();
    error = content->GetStringAttribute( EContentID, ptr );
    if ( !error )
        {
        contentURI8 = HBufC8::NewLC( contentURI->Length() );
        contentURI8->Des().Copy( contentURI->Des() );
        LaunchDetailsViewEmbeddedL( contentURI8 );
        CleanupStack::PopAndDestroy( contentURI8 );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    CleanupStack::PopAndDestroy( 2, content ); // contentURI, content
    }


// -----------------------------------------------------------------------------
// CDRMHelper::LaunchDetailsViewEmbeddedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::LaunchDetailsViewEmbeddedL( RFile& aFileHandle )
    {
    HBufC* contentURI = NULL;
    HBufC8* contentURI8 = NULL;
    TInt error;

    CData* content = CData::NewLC( aFileHandle, KDefaultContentObject, EPeek );

    contentURI = HBufC::NewLC( KUrlMaxLen );
    TPtr ptr = contentURI->Des();
    error = content->GetStringAttribute( EContentID, ptr );
    if ( !error )
        {
        contentURI8 = HBufC8::NewLC( contentURI->Length() );
        contentURI8->Des().Copy( contentURI->Des() );
        LaunchDetailsViewEmbeddedL( contentURI8 );
        CleanupStack::PopAndDestroy( contentURI8 );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    CleanupStack::PopAndDestroy( 2, content ); // contentURI, content
    }

EXPORT_C void CDRMHelper::LaunchDetailsViewEmbeddedL(
    const TDesC8& /*aURI*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelper::CreateLaunchParamL
// -----------------------------------------------------------------------------
//
void CDRMHelper::CreateLaunchParamL(
    TUint32 aLocalId,
    const HBufC8* aUrl,
    HBufC*& aLaunchParam )
    {
    TInt index;

    // MaxInt will fit into 11 characters
    HBufC* b( HBufC::NewLC( 11 ) );
    b->Des().NumUC( aLocalId );
    HBufC* localIDBuf( b->ReAllocL( b->Des().Length() ) );
    CleanupStack::Pop( b );
    b = NULL;
    CleanupStack::PushL( localIDBuf );

    // length of startparam is always 1 and some markers are needed
    TInt length = 1 + aUrl->Length() + localIDBuf->Des().Length() + 3;

    aLaunchParam = HBufC::NewMaxL( length );

    TPtr ptr( aLaunchParam->Des() );
    ptr.SetLength( 0 );
    _LIT( KOne, "1" );
    _LIT( KTwo, "2" );
    _LIT( KMarker, "\x00" );

    // start param is 1 for embedded launch and 2 for launching details view
    // standalone
    if ( iUseCoeEnv )
        {
        ptr.Append( KOne() );
        }
    else
        {
        ptr.Append( KTwo() );
        }
    ptr.Append( KMarker() );
    ptr.Append( localIDBuf->Des() );
    ptr.Append( KMarker() );

    index = ptr.Length();
    ptr.SetLength( length );
    for ( TInt i = 0 ; i < aUrl->Length(); i++ )
        {
        ptr[index++] = ( unsigned char ) (*aUrl)[i];
        }

    ptr[index] = ( unsigned char ) KMarker()[0];

    CleanupStack::PopAndDestroy( localIDBuf );
    }


// -----------------------------------------------------------------------------
// CDRMHelper::GetRightsDetailsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::GetRightsDetailsL(
    const TDesC& aFileName,
    TUint32 aIntent,
    TBool& aExpired,
    TBool& aSendingAllowed,
    CDRMHelperRightsConstraints*& aPlay,
    CDRMHelperRightsConstraints*& aDisplay,
    CDRMHelperRightsConstraints*& aExecute,
    CDRMHelperRightsConstraints*& aPrint )
    {
    HBufC* contentURI = NULL;
    HBufC8* contentURI8 = NULL;
    TInt drmProtected(0);
    TInt intent;

    TVirtualPathPtr virtualPath( aFileName, KDefaultContentObject );

    CData* content( NULL );
    TRAPD( r, content = CData::NewL( virtualPath, EPeek, EContentShareReadWrite ) );
    if ( r == KErrInUse )
        {
        content = CData::NewL( virtualPath, EPeek, EContentShareReadOnly );
        }
    else
        {
        User::LeaveIfError( r );
        }

    CleanupStack::PushL( content );

    User::LeaveIfError( content->GetAttribute( EIsProtected, drmProtected ) ) ;

    if ( !drmProtected )
        {
        User::Leave( KErrArgument );
        }

    User::LeaveIfError( content->GetAttribute( EIsForwardable, aSendingAllowed ) ) ;

    contentURI = HBufC::NewLC( KUrlMaxLen );
    TPtr ptr = contentURI->Des();
    User::LeaveIfError( content->GetStringAttribute( EContentID, ptr ) );

    contentURI8 = HBufC8::NewLC( contentURI->Length() );
    contentURI8->Des().Copy( contentURI->Des() );
    if ( aIntent == ContentAccess::EPlay ||
        aIntent == ContentAccess::EView ||
        aIntent == ContentAccess::EExecute ||
        aIntent == ContentAccess::EPrint )
        {
        intent = aIntent;
        }
    else
        {
        // not valid CAF intent, determine intent from mimetype
        HBufC* mimetype = HBufC::NewLC( KUrlMaxLen );
        ptr.Set( mimetype->Des() );
        content->GetStringAttribute( EMimeType, ptr );
        intent = Intent( mimetype );
        CleanupStack::PopAndDestroy( mimetype );
        }
    DoGetRightsDetailsL( contentURI8, intent, aExpired, aPlay, aDisplay,
        aExecute, aPrint );
    CleanupStack::PopAndDestroy( 3, content ); // contentURI8, contentURI, content
    }

// -----------------------------------------------------------------------------
// CDRMHelper::GetRightsDetailsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::GetRightsDetailsL(
    RFile& aFileHandle,
    TUint32 aIntent,
    TBool& aExpired,
    TBool& aSendingAllowed,
    CDRMHelperRightsConstraints*& aPlay,
    CDRMHelperRightsConstraints*& aDisplay,
    CDRMHelperRightsConstraints*& aExecute,
    CDRMHelperRightsConstraints*& aPrint )
    {
    HBufC* contentURI = NULL;
    HBufC8* contentURI8 = NULL;
    TInt drmProtected(0);
    TInt intent;

    CData* content = CData::NewLC( aFileHandle, KDefaultContentObject, EPeek );

    User::LeaveIfError( content->GetAttribute( EIsProtected, drmProtected ) ) ;

    if ( !drmProtected )
        {
        User::Leave( KErrArgument );
        }

    User::LeaveIfError( content->GetAttribute( EIsForwardable, aSendingAllowed ) ) ;

    contentURI = HBufC::NewLC( KUrlMaxLen );
    TPtr ptr = contentURI->Des();
    User::LeaveIfError( content->GetStringAttribute( EContentID, ptr ) );

    contentURI8 = HBufC8::NewLC( contentURI->Length() );
    contentURI8->Des().Copy( contentURI->Des() );
    if ( aIntent == ContentAccess::EPlay ||
        aIntent == ContentAccess::EView ||
        aIntent == ContentAccess::EExecute ||
        aIntent == ContentAccess::EPrint )
        {
        intent = aIntent;
        }
    else
        {
        // not valid CAF intent, determine intent from mimetype
        HBufC* mimetype = HBufC::NewLC( KUrlMaxLen );
        ptr.Set( mimetype->Des() );
        content->GetStringAttribute( EMimeType, ptr );
        intent = Intent( mimetype );
        CleanupStack::PopAndDestroy( mimetype );
        }

    DoGetRightsDetailsL( contentURI8, intent, aExpired, aPlay, aDisplay,
        aExecute, aPrint );

    CleanupStack::PopAndDestroy( 3, content );  // contentURI8, contentURI, content
    }


// -----------------------------------------------------------------------------
// CDRMHelper::DoGetRightsDetailsL
// -----------------------------------------------------------------------------
//
void CDRMHelper::DoGetRightsDetailsL(
    HBufC8* aContentUri,
    TUint32 aIntent,
    TBool& aExpired,
    CDRMHelperRightsConstraints*& aPlay,
    CDRMHelperRightsConstraints*& aDisplay,
    CDRMHelperRightsConstraints*& aExecute,
    CDRMHelperRightsConstraints*& aPrint )
    {
    CDRMRights* activeRights = NULL;

    // GetActiveRights returns negative value if rights are not valid
    if ( iDRMCommon->GetActiveRights( *aContentUri, aIntent, activeRights ) > 0 )
        {
        // rights are valid
        aExpired = EFalse;
        CleanupStack::PushL( activeRights );
        GetConstraintsL( *activeRights,
                aPlay,
                aDisplay,
                aExecute,
                aPrint );
        CleanupStack::PopAndDestroy( activeRights );
        }
    else
        {
        // rights are not valid, check if they are future or expired.
        TInt expiration = GetExpirationDetails(
            aContentUri, aIntent, activeRights );
        CleanupStack::PushL( activeRights );
        switch ( expiration )
            {
            case CDRMRights::EExpiredRights:
                // expired rights
                aExpired = ETrue;
                break;
            case CDRMRights::EFutureRights:
                // future rights, mark it as "expired" but return also
                // constraint objects
                aExpired = ETrue;
                GetConstraintsL( *activeRights,
                        aPlay,
                        aDisplay,
                        aExecute,
                        aPrint );
                break;
            case KErrCANoRights:
                // no rights, leave
                User::Leave( KErrCANoRights );
                break;
            default:
                // something else, should never come here
                User::Leave( KErrGeneral );
                break;
            }
        CleanupStack::PopAndDestroy( activeRights );
        }
    }

// -----------------------------------------------------------------------------
// CDRMHelper::GetConstraintsL
// -----------------------------------------------------------------------------
//
void CDRMHelper::GetConstraintsL(
    CDRMRights& aRights,
    CDRMHelperRightsConstraints*& aPlay,
    CDRMHelperRightsConstraints*& aDisplay,
    CDRMHelperRightsConstraints*& aExecute,
    CDRMHelperRightsConstraints*& aPrint )
    {
    CDRMRightsConstraints* constraint( NULL );
    CDRMHelperRightsConstraints* play( NULL );
    CDRMHelperRightsConstraints* display( NULL );
    CDRMHelperRightsConstraints* execute( NULL );
    CDRMHelperRightsConstraints* print( NULL );
    TInt poppableItems(0);

    aRights.GetPlayRight( constraint );
    if ( constraint )
        {
        // takes ownership of constraint
        play = CDRMHelperRightsConstraints::NewLC( constraint );
        poppableItems++;
        constraint = NULL;
        }
    aRights.GetDisplayRight( constraint );
    if ( constraint )
        {
        // takes ownership of constraint
        display = CDRMHelperRightsConstraints::NewLC( constraint );
        poppableItems++;
        constraint = NULL;
        }
    aRights.GetExecuteRight( constraint );
    if ( constraint )
        {
        // takes ownership of constraint
        execute = CDRMHelperRightsConstraints::NewLC( constraint );
        poppableItems++;
        constraint = NULL;
        }
    aRights.GetPrintRight( constraint );
    if ( constraint )
        {
        // takes ownership of constraint
        print = CDRMHelperRightsConstraints::NewLC( constraint );
        poppableItems++;
        constraint = NULL;
        }
    if ( poppableItems )
        {
        CleanupStack::Pop( poppableItems );
        }
    aPlay = play;
    aDisplay = display;
    aExecute = execute;
    aPrint = print;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::GetRightsDetailsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::GetRightsDetailsL( const TDesC& aFileName,
    TUint32 aIntent, TBool& aExpired, TBool& aSendingAllowed,
    CDRMRightsConstraints*& aPlay,
    CDRMRightsConstraints*& aDisplay,
    CDRMRightsConstraints*& aExecute,
    CDRMRightsConstraints*& aPrint )
    {
    TVirtualPathPtr virtualPath( aFileName, KDefaultContentObject );
    TInt intent;

    CData* content( NULL );
    TRAPD( r, content = CData::NewL( virtualPath, EPeek, EContentShareReadWrite ) );
    if ( r == KErrInUse )
        {
        content = CData::NewL( virtualPath, EPeek, EContentShareReadOnly );
        }
    else
        {
        User::LeaveIfError( r );
        }

    CleanupStack::PushL( content );

    // if aIntent is not any of CAF intents, use default intent
    if ( aIntent == ContentAccess::EPlay ||
        aIntent == ContentAccess::EView ||
        aIntent == ContentAccess::EExecute ||
        aIntent == ContentAccess::EPrint )
        {
        intent = aIntent;
        }
    else
        {
        // not valid CAF intent, determine intent from mimetype
        HBufC* mimetype = HBufC::NewLC( KUrlMaxLen );
        TPtr ptr = mimetype->Des();
        content->GetStringAttribute( EMimeType, ptr );
        intent = Intent( mimetype );
        CleanupStack::PopAndDestroy( mimetype );
        }

    DoGetRightsDetailsL( content, intent, aExpired, aSendingAllowed, aPlay,
        aDisplay, aExecute, aPrint );

    CleanupStack::PopAndDestroy( content ); // content
    }

// -----------------------------------------------------------------------------
// CDRMHelper::GetRightsDetailsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::GetRightsDetailsL( RFile& aFileHandle,
    TUint32 aIntent, TBool& aExpired, TBool& aSendingAllowed,
    CDRMRightsConstraints*& aPlay,
    CDRMRightsConstraints*& aDisplay,
    CDRMRightsConstraints*& aExecute,
    CDRMRightsConstraints*& aPrint )
    {
    TInt intent;
    CData* content = CData::NewLC( aFileHandle, KDefaultContentObject, EPeek );

    // if aIntent is not any of CAF intents, use default intent
    if ( aIntent == ContentAccess::EPlay ||
        aIntent == ContentAccess::EView ||
        aIntent == ContentAccess::EExecute ||
        aIntent == ContentAccess::EPrint )
        {
        intent = aIntent;
        }
    else
        {
        // not valid CAF intent, determine intent from mimetype
        HBufC* mimetype = HBufC::NewLC( KUrlMaxLen );
        TPtr ptr = mimetype->Des();
        content->GetStringAttribute( EMimeType, ptr );
        intent = Intent( mimetype );
        CleanupStack::PopAndDestroy( mimetype );
        }
    DoGetRightsDetailsL( content, intent, aExpired, aSendingAllowed, aPlay,
        aDisplay, aExecute, aPrint );
    CleanupStack::PopAndDestroy( content ); // content
    }

// -----------------------------------------------------------------------------
// CDRMHelper::DoGetRightsDetailsL
// -----------------------------------------------------------------------------
//
void CDRMHelper::DoGetRightsDetailsL(
    CData* aContent,
    TUint32 aIntent,
    TBool& aExpired,
    TBool& aSendingAllowed,
    CDRMRightsConstraints*& aPlay,
    CDRMRightsConstraints*& aDisplay,
    CDRMRightsConstraints*& aExecute,
    CDRMRightsConstraints*& aPrint )
    {
    HBufC* contentURI = NULL;
    HBufC8* contentURI8 = NULL;
    TInt drmProtected(0);
    CDRMRights* activeRights;

    __ASSERT_DEBUG( aContent, User::Invariant() );
    User::LeaveIfNull( aContent );
    User::LeaveIfError( aContent->GetAttribute( EIsProtected, drmProtected ) ) ;
    if ( !drmProtected )
        {
        User::Leave( KErrArgument );
        }

    User::LeaveIfError(
        aContent->GetAttribute( EIsForwardable, aSendingAllowed ) ) ;

    contentURI = HBufC::NewLC( KUrlMaxLen );
    TPtr ptr = contentURI->Des();
    User::LeaveIfError( aContent->GetStringAttribute( EContentID, ptr ) );
    contentURI8 = HBufC8::NewLC( contentURI->Length() );
    contentURI8->Des().Copy( contentURI->Des() );

    // GetActiveRights returns negative value if rights are not valid
    if ( iDRMCommon->GetActiveRights( *contentURI8, aIntent, activeRights ) > 0 )
        {
        // rights are valid
        aExpired = EFalse;
        CleanupStack::PushL( activeRights );
        activeRights->GetPlayRight( aPlay );
        activeRights->GetDisplayRight( aDisplay );
        activeRights->GetExecuteRight( aExecute );
        activeRights->GetPrintRight( aPrint );
        CleanupStack::PopAndDestroy( activeRights );
        }
    else
        {
        // rights are not valid, check if they are future or expired.
        TInt expiration = GetExpirationDetails(
            contentURI8, aIntent, activeRights );
        CleanupStack::PushL( activeRights );
        switch ( expiration )
            {
            case CDRMRights::EExpiredRights:
                // expired rights
                aExpired = ETrue;
                break;
            case CDRMRights::EFutureRights:
                // future rights, mark it as "expired" but return also
                // constraint objects
                aExpired = ETrue;
                activeRights->GetPlayRight( aPlay );
                activeRights->GetDisplayRight( aDisplay );
                activeRights->GetExecuteRight( aExecute );
                activeRights->GetPrintRight( aPrint );
                break;
            case KErrCANoRights:
                // no rights, leave
                User::Leave( KErrCANoRights );
                break;
            default:
                // something else, should never come here
#ifdef _DEBUG
                User::Invariant();
#endif
                User::Leave( KErrGeneral );
                break;
            }
        CleanupStack::PopAndDestroy( activeRights );
        }
    CleanupStack::PopAndDestroy( 2, contentURI ); // contentURI8, contentURI
    }

EXPORT_C TInt CDRMHelper::ShowDRMUINotification2L( TDRMHelperNotificationID /*aTextId*/,
    const TDesC8& /*aURI*/ )
    {
    return KErrNotSupported;
    }

EXPORT_C TInt CDRMHelper::ShowDRMUINotification2L( TDRMHelperNotificationID /*aTextId*/,
    const TDesC& /*aFileName*/ )
    {
    return KErrNotSupported;
    }

#pragma mark -

// -----------------------------------------------------------------------------
// CDRMHelper::CanSetAutomated
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::CanSetAutomated(
    const TDesC& aFilename,
    TBool& aValue )
    {
    CData* content( NULL );
    TInt error( KErrNone );

    RFile fileHandle;
    TBool protectedWmDrm( EFalse );

    error = fileHandle.Open( iFs, aFilename,
        EFileRead | EFileShareReadersOrWriters );
    if ( error == KErrInUse )
        {
        error = fileHandle.Open( iFs, aFilename, EFileRead | EFileShareAny );
        if ( error == KErrInUse )
            {
            error = fileHandle.Open( iFs, aFilename, EFileRead | EFileShareReadersOnly );
            }
        }

    if ( error == KErrNone )
        {
        // check if file is WMDRM protected
        TRAP( error, protectedWmDrm = IsProtectedWmDrmL( fileHandle ) );
        fileHandle.Close();
        if ( protectedWmDrm || error != KErrNone )
            {
            // file is either corrupt or WMDRM protected
            // so it can not be set as automated
            aValue = EFalse;
            return error;
            }
        }
    else
        {
        return error;
        }

    // do other needed checks
    TVirtualPathPtr virtualPath( aFilename, KDefaultContentObject );

    TRAP( error, content = CData::NewL( virtualPath, EPeek, EContentShareReadWrite ) );
    if ( error == KErrInUse )
        {
        TRAP( error, content = CData::NewL( virtualPath, EPeek, EContentShareReadOnly ) );
        }

    if ( error == KErrNone )
        {

        TRAPD( err, error =  PerformCanSetAutomatedL( *content, aValue ) );
        if ( err )
            {
            error = err;
            }

        delete content;
        }

    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::CanSetAutomated
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::CanSetAutomated(
    RFile& aFileHandle,
    TBool& aValue )
    {
    CData* content( NULL );
    TInt error( KErrNone );
    TBool protectedWmDrm( EFalse );

    // check if file is WMDRM protected
    TRAP( error, protectedWmDrm = IsProtectedWmDrmL( aFileHandle ) );
    if ( protectedWmDrm || error != KErrNone )
        {
        // file is either corrupt or WMDRM protected
        // so it can not be set as automated
        aValue = EFalse;
        return error;
        }
    // do other checks for given file
    TRAP(error, content = CData::NewL( aFileHandle, KDefaultContentObject, EPeek ) );
    if ( error == KErrNone )
        {

        TRAPD( err, error =  PerformCanSetAutomatedL( *content, aValue ) );
        if ( err )
            {
            error = err;
            }

        delete content;
        }

    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::CanSetAutomated
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::CanSetAutomated( const TDesC8& aURI, TBool& aValue )
    {
    HBufC* mimeType = NULL;
    TRAPD( error, mimeType = MimeTypeFromURIL( aURI ) );
    if ( !error )
        {
        error = DoCanSetAutomated( aURI, mimeType, aValue );
        }
    delete mimeType;
    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::SetAutomated
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetAutomated( const TDesC& aFilename )
    {
    CData* content( NULL );
    TInt error( KErrNone );

    TVirtualPathPtr virtualPath( aFilename, KDefaultContentObject );

    TRAP( error, content = CData::NewL( virtualPath, EPeek, EContentShareReadWrite ) );
    if ( error == KErrInUse )
        {
        TRAP( error, content = CData::NewL( virtualPath, EPeek, EContentShareReadOnly ) );
        }

    if ( error == KErrNone )
        {

        TRAPD( err, error =  PerformSetAutomatedL( *content, ETrue ) );
        if ( err )
            {
            error = err;
            }

        delete content;
        }

    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::SetAutomated
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetAutomated( RFile& aFileHandle )
    {
    CData* content( NULL );
    TInt error( KErrNone );

    TRAP(error, content = CData::NewL( aFileHandle, KDefaultContentObject, EPeek ) );
    if ( error == KErrNone )
        {

        TRAPD( err, error =  PerformSetAutomatedL( *content, ETrue ) );
        if ( err )
            {
            error = err;
            }

        delete content;
        }

    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::SetAutomated
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetAutomated( const TDesC8& aURI )
    {
    TInt error( KErrArgument );
    HBufC* mimeType = NULL;

    TRAPD( err, mimeType = MimeTypeFromURIL( aURI ) );
    if ( !err && mimeType )
        {
        TRAP( err, error = DoSetAutomatedL( aURI, ETrue, mimeType ) );
        }
    if ( err )
        {
        error = err;
        }
    delete mimeType;
    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::ShowAutomatedNote
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::ShowAutomatedNote( const TDesC& aFilename )
    {
    CData* content( NULL );
    TInt error( KErrNone );

    TVirtualPathPtr virtualPath( aFilename, KDefaultContentObject );
    TRAP( error, content = CData::NewL( virtualPath, EPeek, EContentShareReadWrite ) );
    if ( error == KErrInUse )
        {
        TRAP( error, content = CData::NewL( virtualPath, EPeek, EContentShareReadOnly ) );
        }

    if ( error == KErrNone )
        {

        TRAPD( err, error =  PerformShowAutomatedNoteL( *content ) );
        if ( err )
            {
            error = err;
            }

        delete content;
        }

    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::ShowAutomatedNote
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::ShowAutomatedNote( RFile& aFileHandle )
    {
    CData* content( NULL );
    TInt error( KErrNone );

    TRAP(error, content = CData::NewL( aFileHandle, KDefaultContentObject, EPeek ) );
    if ( error == KErrNone )
        {

        TRAPD( err, error =  PerformShowAutomatedNoteL( *content ) );
        if ( err )
            {
            error = err;
            }

        delete content;
        }

    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::ShowAutomatedNote
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::ShowAutomatedNote( const TDesC8& aURI )
    {
    TInt error ( KErrArgument );
    HBufC* mimeType = NULL;

    TRAPD( err, mimeType = MimeTypeFromURIL( aURI ) );
    if ( !err && mimeType )
        {
        TRAP( err, error = DoShowAutomatedNoteL( aURI, mimeType ) );
        }
    if ( err )
        {
        error = err;
        }
    delete mimeType;
    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::SetAutomatedPassive
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetAutomatedPassive( const TDesC& aFilename )
    {
    CData* content( NULL );
    TInt error( KErrNone );

    TVirtualPathPtr virtualPath( aFilename, KDefaultContentObject );
    TRAP( error, content = CData::NewL( virtualPath, EPeek, EContentShareReadWrite ) );
    if ( error == KErrInUse )
        {
        TRAP( error, content = CData::NewL( virtualPath, EPeek, EContentShareReadOnly ) );
        }

    if ( error == KErrNone )
        {

        TRAPD( err, error =  PerformSetAutomatedL( *content, EFalse ) );
        if ( err )
            {
            error = err;
            }

        delete content;
        }

    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::SetAutomatedPassive
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetAutomatedPassive( RFile& aFileHandle )
    {
    CData* content( NULL );
    TInt error( KErrNone );

    TRAP(error, content = CData::NewL( aFileHandle, KDefaultContentObject, EPeek ) );
    if ( error == KErrNone )
        {

        TRAPD( err, error =  PerformSetAutomatedL( *content, EFalse ) );
        if ( err )
            {
            error = err;
            }

        delete content;
        }

    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::SetAutomatedPassive
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetAutomatedPassive( const TDesC8& aURI )
    {
    TInt error( KErrArgument );
    HBufC* mimeType = NULL;

    TRAPD( err, mimeType = MimeTypeFromURIL( aURI ) );
    if ( !err && mimeType )
        {
        TRAP( err, error = DoSetAutomatedL( aURI, EFalse, mimeType ) );
        }
    if ( err )
        {
        error = err;
        }
    delete mimeType;
    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::SetAutomatedSilent
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetAutomatedSilent( const TDesC& aFilename,
    TBool aActive )
    {
    CData* content( NULL );
    TInt error( KErrNone );

    TVirtualPathPtr virtualPath( aFilename, KDefaultContentObject );
    TRAP( error, content = CData::NewL( virtualPath, EPeek, EContentShareReadWrite ) );
    if ( error == KErrInUse )
        {
        TRAP( error, content = CData::NewL( virtualPath, EPeek, EContentShareReadOnly ) );
        }

    if ( error == KErrNone )
        {

        TRAPD( err, error =  PerformSetAutomatedSilentL( *content, aActive ) );
        if ( err )
            {
            error = err;
            }

        delete content;
        }

    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::SetAutomatedSilent
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetAutomatedSilent( RFile& aFileHandle,
    TBool aActive )
    {
    CData* content( NULL );
    TInt error( KErrNone );

    TRAP(error, content = CData::NewL( aFileHandle, KDefaultContentObject, EPeek ) );
    if ( error == KErrNone )
        {
        TRAPD( err, error =  PerformSetAutomatedSilentL( *content, aActive ) );
        if ( err )
            {
            error = err;
            }
        delete content;
        }

    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::SetAutomatedSilent
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetAutomatedSilent( const TDesC8& aURI,
    TBool aActive )
    {
    HBufC* mimetype = NULL;
    TRAPD( err, mimetype = MimeTypeFromURIL( aURI ) );
    if ( !err )
        {
        err = DoSetAutomatedSilent( aURI, aActive, Intent( mimetype ) );
        }
    delete mimetype;
    return err;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::RemoveAutomated
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::RemoveAutomated( const TDesC& aFilename )
    {
    CData* content( NULL );
    TInt error( KErrNone );

    TVirtualPathPtr virtualPath( aFilename, KDefaultContentObject );
    TRAP( error, content = CData::NewL( virtualPath, EPeek, EContentShareReadWrite ) );
    if ( error == KErrInUse )
        {
        TRAP( error, content = CData::NewL( virtualPath, EPeek, EContentShareReadOnly ) );
        }

    if ( error == KErrNone )
        {

        TRAPD( err, error =  PerformRemoveAutomatedL( *content, ETrue ) );
        if ( err )
            {
            error = err;
            }

        delete content;
        }

    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::RemoveAutomated
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::RemoveAutomated( RFile& aFileHandle )
    {
    CData* content( NULL );
    TInt error( KErrNone );

    TRAP(error, content = CData::NewL( aFileHandle, KDefaultContentObject, EPeek ) );
    if ( error == KErrNone )
        {
        TRAPD( err, error =  PerformRemoveAutomatedL( *content, ETrue ) );
        if ( err )
            {
            error = err;
            }
        delete content;
        }

    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::RemoveAutomated
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::RemoveAutomated( const TDesC8& aURI )
    {
    HBufC* mimetype = NULL;
    TRAPD( error, mimetype = MimeTypeFromURIL( aURI ) );
    if ( !error )
        {
        error = iHelperServer.RemoveAutomated(
            aURI, ETrue, iAutomatedType, Intent( mimetype ) );
        }
    delete mimetype;
    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::RemoveAutomatedPassive
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::RemoveAutomatedPassive( const TDesC& aFilename )
    {
    CData* content( NULL );
    TInt error( KErrNone );

    TVirtualPathPtr virtualPath( aFilename, KDefaultContentObject );
    TRAP( error, content = CData::NewL( virtualPath, EPeek, EContentShareReadWrite ) );
    if ( error == KErrInUse )
        {
        TRAP( error, content = CData::NewL( virtualPath, EPeek, EContentShareReadOnly ) );
        }

    if ( error == KErrNone )
        {
        TRAPD( err, error =  PerformRemoveAutomatedL( *content, EFalse ) );
        if ( err )
            {
            error = err;
            }

        delete content;
        }

    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::RemoveAutomatedPassive
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::RemoveAutomatedPassive( RFile& aFileHandle )
    {
    CData* content( NULL );
    TInt error( KErrNone );

    TRAP(error, content = CData::NewL( aFileHandle, KDefaultContentObject, EPeek ) );
    if ( error == KErrNone )
        {
        TRAPD( err, error =  PerformRemoveAutomatedL( *content, EFalse ) );
        if ( err )
            {
            error = err;
            }
        delete content;
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::RemoveAutomatedPassive
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::RemoveAutomatedPassive( const TDesC8& aURI )
    {
    HBufC* mimetype = NULL;
    TRAPD( error, mimetype = MimeTypeFromURIL( aURI ) );
    if ( !error )
        {
        error = iHelperServer.RemoveAutomated(
            aURI, EFalse, iAutomatedType, Intent( mimetype ) );
        }
    delete mimetype;
    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::SetAutomatedType
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetAutomatedType(
    CDRMHelper::TDRMHelperAutomatedType aAutomatedType )
    {
    iAutomatedType = aAutomatedType;
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::DoSetAutomatedL
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::DoSetAutomatedL(
    const TDesC8& aURI,
    TBool aActive,
    const HBufC* aMimeType )
    {
    TInt error = DoShowAutomatedNoteL( aURI, aMimeType );
    if ( !error )
        {
        // register with server
        error = DoSetAutomatedSilent( aURI, aActive, Intent( aMimeType) );
        }
    return error;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::DoShowAutomatedNoteL
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::DoShowAutomatedNoteL(
    const TDesC8& aURI,
    const HBufC* aMimeType )
    {
    CDRMRights* rights = NULL;
    TInt error( KErrArgument );
    TInt intent = Intent( aMimeType );
    TInt rightsType = iDRMCommon->GetActiveRights( aURI, intent, rights );
    if ( rightsType == CDRMRights::ERestrictedRights )
        {
        CDRMRights::TRestriction restriction;
        CDRMRights::TExpiration expiration;
        TUint32 constType( 0 );

        rights->GetRightsInfo( intent, restriction, expiration, constType );
        if ( !(constType & CDRMRights::ECountBased) &&
            !(constType & CDRMRights::ETimeIsAccumulatedTime) &&
            expiration == CDRMRights::EValidRights )
            {
            TTime endTime;
            TBool inactive;

            error = CalculateEndTime( rights, intent, endTime, inactive );
            if ( !error )
                {
                TBuf<KDRMHelperMaxDateFormatLen> dateFormat;
                TBuf<KDRMHelperMaxDateLen> endDate;
                TInt buttonCode( 0 );

#ifndef RD_MULTIPLE_DRIVE

                TFileName avkonResFile( KDriveZ );

#else //RD_MULTIPLE_DRIVE

                _LIT( KDrive, "%c:");
                TInt driveNumber( -1 );
                TChar driveLetter;
                DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, driveNumber );
                iFs.DriveToChar( driveNumber, driveLetter );

                TFileName avkonResFile;

                avkonResFile.Format( KDrive, (TUint)driveLetter );

#endif

                _LIT( KCDRMHelperAvkonResFileName, "avkon.rsc" );

                avkonResFile.Append( KDC_RESOURCE_FILES_DIR );
                avkonResFile.Append( KCDRMHelperAvkonResFileName );

                if ( inactive )
                    {
                    buttonCode = DisplayQueryWithIdL( R_DRM_QUERY_SET_AUTO_INTERVAL ,
                        R_DRMHELPER_CONFIRMATION_QUERY );
                    }
                else
                    {
                    // get date string format
                    CStringResourceReader* avkonResReader(
                        CStringResourceReader::NewLC( avkonResFile ) );

                    dateFormat = avkonResReader->ReadResourceString(
                        R_QTN_DATE_USUAL_WITH_ZERO );

                    CleanupStack::PopAndDestroy( avkonResReader );

                    // format date
                    endTime.FormatL( endDate, dateFormat );
                    AknTextUtils::LanguageSpecificNumberConversion( endDate );

                    // display query
                    buttonCode = DisplayQueryL( R_DRMHELPER_SET_AUTOMATED,
                        endDate );
                    }
                error = buttonCode ? KErrNone : KErrCancel;
                }
            else if ( error == KErrNotFound )
                {
                // end time not found, this is datetime constraint without end
                // time
                error = KErrNone;
                }
            }
        }
    else
        {
        if ( rightsType == CDRMRights::EFullRights )
            {
            error = KErrNone;
            }
        }
    delete rights;
    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::DoSetAutomatedSilent
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::DoSetAutomatedSilent(
    const TDesC8& aURI,
    TBool aActive,
    TInt aIntent )
    {
    return iHelperServer.SetAutomated( aURI, aActive, iAutomatedType, aIntent );
    }

// -----------------------------------------------------------------------------
// CDRMHelper::DoCheckSkinCanSetAutomated
//
// Checks all rights constraints for certain RO and checks if interval is the same
// If not, return EFalse
// -----------------------------------------------------------------------------
//
TBool CDRMHelper::DoCheckSkinCanSetAutomated(CDRMRights* aRights)
    {
    TBool Rights[3] = {EFalse, EFalse, EFalse}; //Display, Execute, Play
    TTimeIntervalSeconds EndTimes[3] = {0, 0, 0}; //Display, Execute, Play
    TBool interval = EFalse;
    TTimeIntervalSeconds endTime = 0;

    CDRMRightsConstraints *constraint = NULL;

    // Display rights
    if ( aRights->GetDisplayRight( constraint ) == KErrNone)
        {
        Rights[0] = ETrue;
        constraint->GetInterval( endTime );
        EndTimes[0] = endTime;
        }
    delete constraint;
    constraint = NULL;
    endTime = 0;

    // Execute rights
    if ( aRights->GetExecuteRight( constraint ) == KErrNone)
        {
        Rights[1] = ETrue;
        constraint->GetInterval( endTime );
        EndTimes[1] = endTime;
        }
    delete constraint;
    constraint = NULL;
    endTime = 0;

    // Play rights
    if ( aRights->GetPlayRight( constraint )  == KErrNone )
        {
        Rights[2] = ETrue;
        constraint->GetInterval( endTime );
        EndTimes[2] = endTime;
        }
    delete constraint;
    constraint = NULL;
    endTime = 0;

    for (TInt i = 0; i < 3; ++i)
        {
        // we have a interval
        if (EndTimes[i].Int() != 0)
            {
            interval = ETrue;
            // test if we have endtimes
            if (EndTimes[i].Int() != 0)
                {
                // compare to other
                for (TInt u = 0; u < 3 ; u++)
                    {
                    if ( Rights[u] && (EndTimes[u].Int() != EndTimes[i].Int() ))
                        {
                        return EFalse;
                        }
                    }
                }
            }
        else if ( interval )
            {
            // If no interval but constraint found
            if (Rights[i])
                {
                return EFalse;
                }
            }
        }

    // If time intervals were ok.
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::DoCanSetAutomated
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::DoCanSetAutomated(
    const TDesC8& aURI,
    const HBufC* aMimeType,
    TBool& aValue )
    {
    TInt rightsType;
    CDRMRights* rights = NULL;
    TInt error( KErrNone );
    TInt intent = Intent( aMimeType );

    rightsType = iDRMCommon->GetActiveRights( aURI, intent, rights );
    if ( rightsType == CDRMRights::EFullRights )
        {
        aValue = ETrue;
        }
    else
        {
        if ( rights )
            {
            CDRMRights::TRestriction restriction;
            CDRMRights::TExpiration expiration;
            TUint32 constType;

            rights->GetRightsInfo( intent, restriction, expiration, constType );
            if ( expiration == CDRMRights::EValidRights )
                {
                // Check theme interval rights validity
                if ( iAutomatedType == EAutomatedTypeTheme)
                    {
                    if (!DoCheckSkinCanSetAutomated(rights))
                        {
                        delete rights;
                        aValue = EFalse;
                        return DRMCommon::EInvalidRights;
                        }
                    }

                // if rights are valid and not count or accumulated time
                // based, content can be set automated
                aValue = !(constType &
                    ( CDRMRights::ECountBased | CDRMRights::ETimeIsAccumulatedTime ) );
                }
            else
                {
                // expired or future rights
                aValue = EFalse;

                // return expired error code only for content that has
                // time-based rights
                if ( !(constType &
                        ( CDRMRights::ECountBased | CDRMRights::ETimeIsAccumulatedTime ) ) )
                    {
                    error = DRMCommon::ERightsExpired;
                    }
                }
            }
        else
            {
            // no rights
            aValue = EFalse;
            error = DRMCommon::ENoRights;
            }
        }
    delete rights;
    return error;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::PerformCanSetAutomatedL
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::PerformCanSetAutomatedL(
    const CData& aContent,
    TBool& aValue )
    {
    HBufC* mimeType = NULL;
    HBufC* contentURI = NULL;
    HBufC8* contentURI8 = NULL;
    TInt drmProtected(0);
    TInt error;
    TInt dcfType(0);

    error = aContent.GetAttribute( EIsProtected, drmProtected );
    if ( !error )
        {
        if ( !drmProtected )
            {
            aValue = ETrue;
            error = KErrNone;
            }
        else
            {
            mimeType = HBufC::NewLC( KMaxDataTypeLength );
            contentURI = HBufC::NewLC( KUrlMaxLen );

            // create attribute set
            RStringAttributeSet stringAttributeSet;
            CleanupClosePushL( stringAttributeSet );
            // add the attributes we are interested in
            stringAttributeSet.AddL( EContentID );
            stringAttributeSet.AddL( EMimeType );

            error = aContent.GetStringAttributeSet( stringAttributeSet );

            if ( error == KErrNone )
                {
                // pass on values of string attributes
                TPtr ptr = contentURI->Des();
                error = stringAttributeSet.GetValue( EContentID, ptr );
                if ( error == KErrNone )
                    {
                    TPtr ptr2 = mimeType->Des();
                    error = stringAttributeSet.GetValue( EMimeType, ptr2 );

                    //OMA2 content with is not allowed without classification info
                    if ( error == KErrNone )
                        {
                        error = aContent.GetAttribute( EFileType, dcfType );
                        TInt intent = Intent( mimeType );

                        if (dcfType == EOma2Dcf && intent == EPlay )
                            {

                            HBufC* classificationInfo = HBufC::NewLC( KUrlMaxLen );
                            TPtr ptr = classificationInfo->Des();
                            aContent.GetStringAttribute( EClassificationInfo, ptr);

                            if ( !classificationInfo->FindF(
                                    KDRMHelperClassificationRingingTone ) ||
                                !classificationInfo->FindF(
                                    KDRMHelperClassificationVideoTone ) )
                                {
                                aValue = ETrue;
                                CleanupStack::PopAndDestroy(
                                    classificationInfo );
                                }
                            else
                                {
                                aValue = EFalse;
                                CleanupStack::PopAndDestroy( 4, mimeType );
                                // classificationInfo, stringAttributeSet.Close(),
                                // contentURI, mimeType
                                return error;
                                }
                            }
                        }
                    if ( error == KErrNone )
                        {
                        contentURI8 = HBufC8::NewLC( contentURI->Length() );
                        contentURI8->Des().Copy( contentURI->Des() );
                        error = DoCanSetAutomated( *contentURI8, mimeType, aValue );
                        CleanupStack::PopAndDestroy( contentURI8 );
                        }
                    else
                        {
                        error = KErrArgument;
                        }
                    }
                else
                    {
                    error = KErrArgument;
                    }
                }
            else
                {
                error = KErrArgument;
                }
            CleanupStack::PopAndDestroy(3, mimeType); // stringAttributeSet.Close(), contentURI, mimeType
            }
        }
    else
        {
        error = KErrArgument;
        }
    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::PerformSetAutomatedL
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::PerformSetAutomatedL( const CData& aContent, TBool aActive )
    {
    HBufC* mimeType = NULL;
    HBufC* contentURI = NULL;
    HBufC8* contentURI8 = NULL;
    TInt drmProtected(0);
    TInt error;

    error = aContent.GetAttribute( EIsProtected, drmProtected );
    if ( !error )
        {
        if ( drmProtected )
            {
            mimeType = HBufC::NewLC( KMaxDataTypeLength );
            contentURI = HBufC::NewLC( KUrlMaxLen );

            // create attribute set
            RStringAttributeSet stringAttributeSet;
            CleanupClosePushL( stringAttributeSet );
            // add the attributes we are interested in
            stringAttributeSet.AddL( EContentID );
            stringAttributeSet.AddL( EMimeType );

            error = aContent.GetStringAttributeSet( stringAttributeSet );

            if ( error == KErrNone )
                {
                // pass on values of string attributes
                TPtr ptr = contentURI->Des();
                error = stringAttributeSet.GetValue( EContentID, ptr );
                if ( error == KErrNone )
                    {
                    TPtr ptr2 = mimeType->Des();
                    error = stringAttributeSet.GetValue( EMimeType, ptr2 );
                    if ( error == KErrNone )
                        {
                        contentURI8 = HBufC8::NewLC( contentURI->Length() );
                        contentURI8->Des().Copy( contentURI->Des() );

                        TRAPD( err, error = DoSetAutomatedL( *contentURI8, aActive, mimeType ) );
                        if ( err )
                            {
                            error = err;
                            }
                        CleanupStack::PopAndDestroy( contentURI8 );
                        }
                    else
                        {
                        error = KErrArgument;
                        }
                    }
                else
                    {
                    error = KErrArgument;
                    }
                }
            else
                {
                error = KErrArgument;
                }
            CleanupStack::PopAndDestroy(3, mimeType); // stringAttributeSet.Close(), contentURI, mimeType
            }
        }
    else
        {
        error = KErrArgument;
        }
    return error;
    }



// -----------------------------------------------------------------------------
// CDRMHelper::PerformShowAutomatedNoteL
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::PerformShowAutomatedNoteL( const CData& aContent )
    {
    HBufC* mimeType = NULL;
    HBufC* contentURI = NULL;
    HBufC8* contentURI8 = NULL;
    TInt drmProtected(0);
    TInt error;

    error = aContent.GetAttribute( EIsProtected, drmProtected );
    if ( !error )
        {
        if ( drmProtected )
            {
            mimeType = HBufC::NewLC( KMaxDataTypeLength );
            contentURI = HBufC::NewLC( KUrlMaxLen );

            // create attribute set
            RStringAttributeSet stringAttributeSet;
            CleanupClosePushL( stringAttributeSet );
            // add the attributes we are interested in
            stringAttributeSet.AddL( EContentID );
            stringAttributeSet.AddL( EMimeType );

            error = aContent.GetStringAttributeSet( stringAttributeSet );

            if ( error == KErrNone )
                {
                // pass on values of string attributes
                TPtr ptr = contentURI->Des();
                error = stringAttributeSet.GetValue( EContentID, ptr );
                if ( error == KErrNone )
                    {
                    TPtr ptr2 = mimeType->Des();
                    error = stringAttributeSet.GetValue( EMimeType, ptr2 );
                    if ( error == KErrNone )
                        {
                        contentURI8 = HBufC8::NewLC( contentURI->Length() );
                        contentURI8->Des().Copy( contentURI->Des() );

                        TRAPD( err, error = DoShowAutomatedNoteL( *contentURI8, mimeType ) );
                        if ( err )
                            {
                            error = err;
                            }
                        CleanupStack::PopAndDestroy( contentURI8 );
                        }
                    else
                        {
                        error = KErrArgument;
                        }
                    }
                else
                    {
                    error = KErrArgument;
                    }
                }
            else
                {
                error = KErrArgument;
                }
            CleanupStack::PopAndDestroy(3, mimeType); // stringAttributeSet.Close(), contentURI, mimeType
            }
        }
    else
        {
        error = KErrArgument;
        }
    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::PerformSetAutomatedSilentL
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::PerformSetAutomatedSilentL( const CData& aContent, TBool aActive )
    {
    HBufC* contentURI = NULL;
    HBufC8* contentURI8 = NULL;
    TInt drmProtected(0);
    TInt error;

    error = aContent.GetAttribute( EIsProtected, drmProtected );
    if ( !error )
        {
        if ( drmProtected )
            {
            contentURI = HBufC::NewLC( KUrlMaxLen );
            TPtr ptr = contentURI->Des();
            error = aContent.GetStringAttribute( EContentID, ptr );
            if ( !error )
                {
                HBufC* mimeType = HBufC::NewLC( KMaxDataTypeLength );
                TPtr ptr2 = mimeType->Des();
                error = aContent.GetStringAttribute( EMimeType, ptr2 );
                if ( !error )
                    {
                    contentURI8 = HBufC8::NewLC( contentURI->Length() );
                    contentURI8->Des().Copy( contentURI->Des() );
                    error = DoSetAutomatedSilent( *contentURI8, aActive, Intent( mimeType ) );
                    CleanupStack::PopAndDestroy( contentURI8 );
                    }
                CleanupStack::PopAndDestroy( mimeType );
                }
            CleanupStack::PopAndDestroy( contentURI );
            }
        }
    else
        {
        error = KErrArgument;
        }
    return error;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::PerformRemoveAutomatedL
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::PerformRemoveAutomatedL( const CData& aContent, TBool aActive )
    {
    HBufC* contentURI = NULL;
    HBufC8* contentURI8 = NULL;
    TInt drmProtected(0);
    TInt error;

    error = aContent.GetAttribute( EIsProtected, drmProtected );
    if ( !error )
        {
        if ( !drmProtected )
            {
            error = KErrArgument;
            }
        else
            {
            contentURI = HBufC::NewLC( KUrlMaxLen );
            TPtr ptr = contentURI->Des();
            error = aContent.GetStringAttribute( EContentID, ptr );
            if ( !error )
                {
                contentURI8 = HBufC8::NewLC( contentURI->Length() );
                contentURI8->Des().Copy( contentURI->Des() );
                if ( aActive )
                    {
                    error = RemoveAutomated( *contentURI8 );
                    }
                else
                    {
                    error = RemoveAutomatedPassive( *contentURI8 );
                    }
                CleanupStack::PopAndDestroy( contentURI8 );
                }
            CleanupStack::PopAndDestroy( contentURI );
            }
        }
    else
        {
        error = KErrArgument;
        }
    return error;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::AutomatedResIdL
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::AutomatedResIdL( TBool aExpired, TBool aSecondParameter, TUint32 aRejectReason )
    {
    TInt resId(0);
    TBool invalidImsi( EFalse );
    invalidImsi = aRejectReason & EConstraintIndividual;

    switch ( iAutomatedType )
        {
        case EAutomatedTypeRingingTone:
            if ( aExpired )
                {
                if ( aSecondParameter )
                    {
                    resId = invalidImsi ?
                        R_DRMHELPER_RINGING_TONE_INVALID_SIM_BUY_NEW :
                        R_DRMHELPER_RINGING_TONE_EXPIRED_BUY_NEW;
                    }
                else
                    {
                    resId = invalidImsi ?
                        R_DRM_NON_SD_RTONE_WRONG_SIM :
                        R_DRMHELPER_RINGING_TONE_EXPIRED;
                    }
                }
            break;
        case EAutomatedTypeMessageAlert:
            if ( aExpired )
                {
                if ( aSecondParameter )
                    {
                    resId = invalidImsi ?
                        R_DRMHELPER_MESSAGE_TONE_INVALID_SIM_BUY_NEW :
                        R_DRMHELPER_MESSAGE_TONE_EXPIRED_BUY_NEW;
                    }
                else
                    {
                    resId = invalidImsi ?
                        R_DRM_NON_SD_MSG_AL_WRONG_SIM :
                        R_DRMHELPER_MESSAGE_TONE_EXPIRED;
                    }
                }
            break;
        case EAutomatedTypeEmailAlert:
            if ( aExpired )
                {
                if ( aSecondParameter )
                    {
                    resId = invalidImsi ?
                        R_DRMHELPER_EMAIL_TONE_INVALID_SIM_BUY_NEW :
                        R_DRMHELPER_EMAIL_TONE_EXPIRED_BUY_NEW;
                    }
                else
                    {
                    resId = invalidImsi ?
                        R_DRM_NON_SD_EMAIL_WRONG_SIM :
                        R_DRMHELPER_EMAIL_TONE_EXPIRED;
                    }
                }
            break;
        case EAutomatedTypeIMAlert:
            if ( aExpired )
                {
                if ( aSecondParameter )
                    {
                    resId = /* invalidImsi ?
                               R_DRMHELPER_IM_TONE_INVALID_SIM_BUY_NEW : */
                        R_DRMHELPER_IM_TONE_EXPIRED_BUY_NEW;
                    }
                else
                    {
                    resId = R_DRMHELPER_IM_TONE_EXPIRED;
                    }
                }
            break;
        case EAutomatedTypeCalendarAlarm:
            if ( aExpired )
                {
                if ( aSecondParameter )
                    {
                    resId = invalidImsi ?
                        R_DRMHELPER_CAL_ALERT_INVALID_SIM_BUY_NEW :
                        R_DRMHELPER_CAL_ALERT_EXPIRED_BUY_NEW;
                    }
                else
                    {
                    resId = invalidImsi ?
                        R_DRM_NON_SD_CALEND_WRONG_SIM :
                        R_DRMHELPER_CAL_ALERT_EXPIRED;
                    }
                }
            break;
        case EAutomatedTypeScreenSaver:
            if ( aExpired )
                {
                if ( aSecondParameter )
                    {
                    resId = invalidImsi ?
                        R_DRMHELPER_SCREENSAVER_INVALID_SIM_BUY_NEW :
                        R_DRMHELPER_SCREENSAVER_EXPIRED_BUY_NEW;
                    }
                else
                    {
                    resId = invalidImsi ?
                        R_DRM_NON_SD_SCREEN_WRONG_SIM :
                        R_DRMHELPER_SCREENSAVER_EXPIRED;
                    }
                }
            break;
        case EAutomatedTypeWallpaper:
            if ( aExpired )
                {
                if ( aSecondParameter )
                    {
                    resId = invalidImsi ?
                        R_DRMHELPER_WALLPAPER_INVALID_SIM_BUY_NEW :
                        R_DRMHELPER_WALLPAPER_EXPIRED_BUY_NEW;
                    }
                else
                    {
                    resId = invalidImsi ?
                        R_DRM_NON_SD_WALLP_WRONG_SIM :
                        R_DRMHELPER_WALLPAPER_EXPIRED;
                    }
                }
            break;
        case EAutomatedTypeTheme:
            if ( aExpired )
                {
                if ( aSecondParameter )
                    {
                    resId = invalidImsi ?
                        R_DRMHELPER_THEME_INVALID_SIM_BUY_NEW :
                        R_DRMHELPER_THEME_EXPIRED_BUY_NEW;
                    }
                else
                    {
                    resId = invalidImsi ?
                        R_DRM_NON_SD_THEME_WRONG_SIM :
                        R_DRMHELPER_THEME_EXPIRED;
                    }
                }
            break;
        case EAutomatedTypeClockAlarm:
            if ( aExpired )
                {
                if ( aSecondParameter )
                    {
                    resId = invalidImsi ?
                        R_DRMHELPER_CLOCK_ALARM_INVALID_SIM_BUY_NEW :
                        R_DRMHELPER_CLOCK_ALARM_EXPIRED_BUY_NEW;
                    }
                else
                    {
                    resId = invalidImsi ?
                        R_DRM_NON_SD_CLOCK_WRONG_SIM :
                        R_DRMHELPER_CLOCK_ALARM_EXPIRED;
                    }
                }
            break;
        case EAutomatedTypeOther:
            // Flow through to default branch
        default:
            if ( aExpired )
                {
                if ( aSecondParameter )
                    {
                    resId = invalidImsi ?
                        R_DRMHELPER_AUTOMATED_SD_INVALID_SIM :
                        R_DRMHELPER_AUTOMATED_SD_EXPIRED;
                    }
                else
                    {
                    resId = invalidImsi ?
                        R_DRM_NON_SD_GENER_WRONG_SIM :
                        R_DRMHELPER_AUTOMATED_CD_EXPIRED;
                    }
                }
            break;
        }

    return resId;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::IndicateIdle
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::IndicateIdle()
    {
    iHelperServer.IndicateIdle();
    }

#pragma mark -

// -----------------------------------------------------------------------------
// CDRMHelper::LaunchBrowserL
// Launches browser embedded with given URL
// -----------------------------------------------------------------------------
//
void CDRMHelper::LaunchBrowserL( HBufC8* aUrl )
    {
    // URL needs to be unicode
    HBufC* parameters = HBufC::NewLC( aUrl->Length() );
    parameters->Des().Copy( *aUrl );
    LaunchBrowserL( parameters );
    CleanupStack::PopAndDestroy( parameters );
    }

// -----------------------------------------------------------------------------
// CDRMHelper::LaunchBrowserL
// Launches browser embedded with given URL
// -----------------------------------------------------------------------------
//
void CDRMHelper::LaunchBrowserL( HBufC* aUrl )
    {
    if ( aUrl )
        {
        HBufC* newUrl = NULL;
        TInt i(0);
        TPtrC ptr;
        ptr.Set( *aUrl );
        TBool space = ptr[i] == ' ' ? ETrue : EFalse;
        while ( space )
            {
            i++;
            space = ptr[i] == ' ' ? ETrue : EFalse;
            }

        // if no scheme exists, use http as default scheme
        _LIT( KColon, ":" );
        if ( ptr.Find( KColon ) == KErrNotFound )
            {
            _LIT( KHttp, "http://" );
            const TInt KHttpLen(7);
            newUrl = HBufC::NewLC( KHttpLen + aUrl->Length() );
            newUrl->Des().Copy( KHttp );
            newUrl->Des().Append( ptr.Mid( i ) );
            ptr.Set( *newUrl );
            i = 0;
            }

				// convert given URL to QUrl format
				DRM::CDrmBrowserLauncher* browserLauncher = DRM::CDrmBrowserLauncher::NewLC();
    	
		    browserLauncher->LaunchUrlL(ptr);
    
    		CleanupStack::PopAndDestroy(); // browserLauncher

        
        // delete newUrl if needed
        if ( newUrl )
            {
            CleanupStack::PopAndDestroy( newUrl );
            }
        }
    else
        {
        User::Leave( KErrArgument );
        }
    }

// -----------------------------------------------------------------------------
// CDRMHelper::LaunchRightsManagerUiL
// -----------------------------------------------------------------------------
//
void CDRMHelper::LaunchRightsManagerUiL( const TDesC& aParam16 )
    {
    const TUid KUidDRMUI = {0x101f85c7};

    if ( iUseCoeEnv )
        {
        CAiwGenericParamList* paramList = CAiwGenericParamList::NewLC();
        TAiwVariant variantObject( aParam16 );
        TAiwGenericParam param( EGenericParamFile, variantObject );
        paramList->AppendL( param );

        CAknLaunchAppService* launchAppService =
            CAknLaunchAppService::NewL( KUidDRMUI, this, paramList );

        CleanupStack::PushL( launchAppService );
        iWait.Start();

        CleanupStack::PopAndDestroy( 2, paramList ); // paramList, launchAppService
        }
    else
        {
        // no UI to embed, launch Rights Manager UI stand-alone

        // check if Rights Manager UI is already running
        RWsSession wsSession;
        User::LeaveIfError( wsSession.Connect() );
        CleanupClosePushL( wsSession );
        TApaTaskList tasklist( wsSession );
        TApaTask task = tasklist.FindApp( KUidDRMUI );
        if ( task.Exists() )
            {
            // Rights Manager UI is already running
            HBufC8* param8 = HBufC8::NewLC( aParam16.Length() );
            TPtr8 parametersPtr = param8->Des();
            parametersPtr.Copy( aParam16 );
            task.SendMessage( TUid::Uid( KUidApaMessageSwitchOpenFileValue ), *param8 );
            CleanupStack::PopAndDestroy( param8 );
            }
        else
            {
            RApaLsSession appArcSession;
            User::LeaveIfError( appArcSession.Connect() );
            TThreadId id;
            appArcSession.StartDocument( aParam16, KUidDRMUI, id );
            appArcSession.Close();
            }
        CleanupStack::PopAndDestroy( &wsSession );
        }
    return;
    }

#pragma mark -

// -----------------------------------------------------------------------------
// CDRMHelper::GetRightsIssuerL
// -----------------------------------------------------------------------------
//
void CDRMHelper::GetRightsIssuerL( const CData& aContent, HBufC*& aIssuer )
    {
    RDRMRightsClient client;
    TInt error;
    HBufC* decodedIssuer = NULL;
    HBufC* newBuf = NULL;
    TInt urlLength = KUrlMaxLen;

    // connect to the rights server
    User::LeaveIfError( client.Connect() );
    CleanupClosePushL( client );

    HBufC* rightsIssuerBuf = HBufC::NewLC( urlLength );
    TPtr rightsIssuer(const_cast<TUint16*>(rightsIssuerBuf->Ptr()), 0, urlLength);

    // Get value of rights-issuer header field
    error = aContent.GetStringAttribute( ERightsIssuerUrl, rightsIssuer );

    while ( error == KErrOverflow )
        {
        urlLength += KUrlMaxLen;
        newBuf = rightsIssuerBuf->ReAllocL( urlLength );
        CleanupStack::Pop( rightsIssuerBuf );
        rightsIssuerBuf = newBuf;
        CleanupStack::PushL( rightsIssuerBuf );
        rightsIssuer.Set(const_cast<TUint16*>(rightsIssuerBuf->Ptr()), 0, urlLength);
        error = aContent.GetStringAttribute( ERightsIssuerUrl, rightsIssuer );
        }

    // If we got no error, and found some rights issuer
    if ( ( !error  ) && ( rightsIssuer.Length() != 0 ) )
        {
        if ( aIssuer )
            {
            delete aIssuer;
            aIssuer = NULL;
            }

        // check if rights issuer is encrypted
        if ( rightsIssuer.MatchF( KEncryptedRightsIssuerMatchString ) !=
            KErrNotFound )
            {
            HBufC8* input = NULL;
            HBufC8* output = NULL;

            input = CnvUtfConverter::ConvertFromUnicodeToUtf8L(rightsIssuer);
            CleanupStack::PushL( input );

            // encrypted, decrypt
            User::LeaveIfError( client.DecodeRightsIssuerField( *input,
                    output ) );
            CleanupStack::PopAndDestroy( input );
            CleanupStack::PushL( output );

            decodedIssuer= CnvUtfConverter::ConvertToUnicodeFromUtf8L(*output);
            CleanupStack::PopAndDestroy( output );

            aIssuer = decodedIssuer;
            }
        else
            {
            aIssuer = rightsIssuer.AllocL();
            }
        }
    else if ( rightsIssuer.Length() != 0 )
        {
        if ( aIssuer )
            {
            delete aIssuer;
            aIssuer = NULL;
            }
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy( rightsIssuerBuf );
    CleanupStack::PopAndDestroy( &client );
    User::LeaveIfError( error );
    }

// -----------------------------------------------------------------------------
// CDRMHelper::CalculateEndTime
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::CalculateEndTime(
    CDRMRights* aRights,
    TInt aIntent,
    TTime& aEndTime,
    TBool& aInactive )
    {
    CDRMRightsConstraints* constraint = NULL;
    TInt error( KErrNotFound );

    if ( aIntent == DRMCommon::EPrint )
        {
        if ( aRights->GetPrintRight( constraint ) == DRMCommon::EOk )
            {
            error = GetEndTime( constraint, aEndTime, aInactive );
            }
        delete constraint;
        constraint = NULL;
        }

    if ( aIntent == DRMCommon::EExecute )
        {
        if ( aRights->GetExecuteRight( constraint ) == DRMCommon::EOk )
            {
            error = GetEndTime( constraint, aEndTime, aInactive );
            }
        delete constraint;
        constraint = NULL;
        }

    if ( aIntent == DRMCommon::EPlay )
        {
        if ( aRights->GetPlayRight( constraint ) == DRMCommon::EOk )
            {
            error = GetEndTime( constraint, aEndTime, aInactive );
            }
        delete constraint;
        constraint = NULL;
        }

    if ( aIntent == DRMCommon::EDisplay )
        {
        if ( aRights->GetDisplayRight( constraint ) == DRMCommon::EOk )
            {
            error = GetEndTime( constraint, aEndTime, aInactive );
            }
        delete constraint;
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::GetStartTime
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::GetStartTime(
    CDRMRights* aRights,
    TInt aIntent,
    TTime& aStartTime)
    {
    CDRMRightsConstraints* constraint = NULL;
    TInt error( KErrNotFound );

    if ( aIntent == DRMCommon::EPrint )
        {
        if ( aRights->GetPrintRight( constraint ) == DRMCommon::EOk )
            {
            error = constraint->GetStartTime( aStartTime );
            }
        delete constraint;
        constraint = NULL;
        }

    if ( aIntent == DRMCommon::EExecute )
        {
        if ( aRights->GetExecuteRight( constraint ) == DRMCommon::EOk )
            {
            error = constraint->GetStartTime( aStartTime );
            }
        delete constraint;
        constraint = NULL;
        }

    if ( aIntent == DRMCommon::EPlay )
        {
        if ( aRights->GetPlayRight( constraint ) == DRMCommon::EOk )
            {
            error = constraint->GetStartTime( aStartTime );
            }
        delete constraint;
        constraint = NULL;
        }

    if ( aIntent == DRMCommon::EDisplay )
        {
        if ( aRights->GetDisplayRight( constraint ) == DRMCommon::EOk )
            {
            error = constraint->GetStartTime( aStartTime );
            }
        delete constraint;
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::GetEndTime
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::GetEndTime(
    CDRMRightsConstraints* aConstraint,
    TTime& aEndTime,
    TBool& aInactive )
    {
    TTime endTime;
    TInt error ( KErrNotFound );

    if ( aConstraint->GetEndTime( endTime ) == DRMCommon::EOk )
        {
        aEndTime = endTime;
        error = KErrNone;
        aInactive = EFalse;
        }
    else
        {
        TTimeIntervalSeconds interval;

        if ( aConstraint->GetInterval( interval ) == DRMCommon::EOk )
            {
            // interval rights
            TTime start;

            if ( aConstraint->GetIntervalStart( start ) == DRMCommon::EOk )
                {
                // activated interval
                aEndTime = start + interval;
                aInactive = EFalse;
                }
            else
                {
                // inactive interval
                aEndTime.HomeTime();
                aEndTime += interval;
                aInactive = ETrue;
                }
            error = KErrNone;
            }
        }
    return error;
    }


#pragma mark -

EXPORT_C TInt CDRMHelper::Consume2(
    const TDesC8& aContentURI,
    TUint32 aRightsSpec,
    TDRMHelperConsumeAction anAction)
    {
    CConsumeData* newData = NULL;
    TInt ret = KErrNone;
    ContentAccess::TIntent intent = ContentAccess::EUnknown;
    TInt i;
    TBool found = EFalse;

    // convert rights spec + action -> intent
    if ( anAction == EStart )
        {
        switch ( aRightsSpec )
            {
            case CDRMRights::EPlay:
                intent = ContentAccess::EPlay;
                break;
            case CDRMRights::EDisplay:
                intent = ContentAccess::EView;
                break;
            case CDRMRights::EPrint:
                intent = ContentAccess::EPrint;
                break;
            case CDRMRights::EExecute:
                intent = ContentAccess::EExecute;
                break;
            case CDRMRights::EInstall:
                intent = ContentAccess::EInstall;
                break;
            default:
                break;
            }
        }
    else if ( anAction == EFinish )
        {
        intent = ContentAccess::EStop;
        }
    else if ( anAction == EPause )
        {
        intent = ContentAccess::EPause;
        }
    else if ( anAction == EContinue )
        {
        intent = ContentAccess::EContinue;
        }

    // find correct item from list
    for ( i = 0; i < iConsumeList.Count(); i++ )
        {
        if ( iConsumeList[i]->CompareUri( aContentURI ) == 0 )
            {
            found = ETrue;
            break;
            }
        }

    if ( intent == ContentAccess::EStop )
        {
        if ( found )
            {
            // finish consume
            ret = iConsumeList[i]->Consume( intent );

            // remove from list
            delete iConsumeList[i];
            iConsumeList.Remove( i );
            }
        else
            {
            // not found
            ret = KErrNotFound;
            }
        }
    else
        {
        // start, pause or continue
        if ( !found )
            {
            TInt len = aContentURI.Length();
            if ( len )
                {
                // for pause and continue existing connection is required
                if ( intent != ContentAccess::EPause &&
                    intent != ContentAccess::EContinue )
                    {
                    // no previous connection with this content
                    TRAP( ret, newData = CConsumeData::NewL( aContentURI ) );

                    if ( !ret )
                        {
                        iConsumeList.Append( newData );
                        ret = newData->Consume( intent );
                        }
                    }
                else
                    {
                    // pause or continue
                    ret = KErrNotFound;
                    }
                }
            else
                {
                // empty content URI
                ret = KErrArgument;
                }
            }
        else
            {
            // we have consume active for this content, just call
            // consume again
            ret = iConsumeList[i]->Consume( intent );
            }
        }
    return ret;
    }


EXPORT_C TInt CDRMHelper::ConsumeFile2(
    const TDesC& aFileName,
    TUint32 aRightsSpec,
    TDRMHelperConsumeAction anAction)
    {
    CDcfCommon* dcf = NULL;
    RFile file;
    TInt r = KErrNone;

    r = file.Open(iFs, aFileName, EFileRead | EFileShareReadersOrWriters);

    if (r == KErrInUse)
        {
        r = file.Open(iFs, aFileName, EFileRead | EFileShareAny);
        if (r == KErrInUse)
            {
            r = file.Open(iFs, aFileName, EFileRead | EFileShareReadersOnly);
            }
        }
    if (r == KErrNone)
        {
        TRAP(r, dcf = CDcfCommon::NewL(file));
        if ( dcf )
            {
            r = Consume2(*dcf->iContentID, aRightsSpec, anAction);
            delete dcf;
            }
        file.Close();
        }
    return r;
    }


EXPORT_C TInt CDRMHelper::ConsumeFile2(
    RFile& aFileHandle,
    TUint32 aRightsSpec,
    TDRMHelperConsumeAction anAction)
    {
    CDcfCommon* dcf = NULL;
    TInt r = KErrNone;

    TRAP(r, dcf = CDcfCommon::NewL(aFileHandle));
    if ( dcf )
        {
        r = Consume2(*dcf->iContentID, aRightsSpec, anAction);
        delete dcf;
        }
    return r;
    }


#pragma mark -

// -----------------------------------------------------------------------------
// CDRMHelper::ActivateContentL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::ActivateContentL( CData& aContent )
    {
    if ( iOma2 )
        {
        TInt drmProtected(0);
        aContent.GetAttribute( EIsProtected, drmProtected );
        // no need to do anything if content is not protected
        if ( drmProtected )
            {
            TInt expired(0);
            TInt missing(0);

            // Check if rights are expired or missing
            aContent.GetAttribute( ERightsHaveExpired, expired );
            aContent.GetAttribute( ERightsNone, missing );
            if ( expired || missing )
                {
                HBufC* rightsIssuer = NULL;

                // Get RI URL
                TRAPD( err, GetRightsIssuerL( aContent, rightsIssuer ) );
                if ( !err )
                    {
                    TInt buyRights;
                    // superdistributable
                    CleanupStack::PushL( rightsIssuer );

                    if ( expired )
                        {
                        buyRights = DisplayQueryWithIdL(
                            R_DRMHELPER_USAGE_RIGHTS_EXPIRED_BUY_NEW,
                            R_DRMHELPER_CONFIRMATION_QUERY );
                        }
                    else
                        {
                        // rights missing
                        buyRights = DisplayQueryWithIdL(
                            R_DRMHELPER_NO_USAGE_RIGHTS_BUY_NEW,
                            R_DRMHELPER_CONFIRMATION_QUERY );
                        }

                    if ( buyRights )
                        {
                        // Launch browser
                        LaunchBrowserL( rightsIssuer );
                        }
                    else
                        {
                        User::Leave( KErrCancel );
                        }

                    CleanupStack::PopAndDestroy( rightsIssuer );
                    }
                else
                    {
                    // non-superdistributable
                    if ( expired )
                        {
                        // rights expired
                        DisplayInfoNoteL( R_DRMHELPER_USAGE_RIGHTS_EXPIRED );
                        }
                    else
                        {
                        // rights missing
                        DisplayInfoNoteL( R_DRMHELPER_NO_USAGE_RIGHTS );
                        }
                    }
                }
            else
                {
                // If rights are valid or future rights, they should not be
                // updated
                User::Leave( KErrArgument );
                }
            }
        else
            {
            // this function should be called only for protected content
            User::Leave( KErrArgument );
            }
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }


// -----------------------------------------------------------------------------
// CDRMHelper::ActivateContentL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::ActivateContentL( TDesC& aFileName )
    {
    if ( iOma2 )
        {
        TVirtualPathPtr virtualPath( aFileName );

        CData* content = NULL;
        TRAPD( r, content = CData::NewL( virtualPath, EPeek, EContentShareReadWrite ) );
        if ( r == KErrInUse )
            {
            content = CData::NewL( virtualPath, EPeek, EContentShareReadOnly );
            }
        else
            {
            User::LeaveIfError( r );
            }
        CleanupStack::PushL( content );

        ActivateContentL( *content );
        CleanupStack::PopAndDestroy( content );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }


#pragma mark -

// -----------------------------------------------------------------------------
// CDRMHelper::HasInfoUrlL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMHelper::HasInfoUrlL( CData& aContent, HBufC8*& aInfoUrl )
    {
    TBool ret = EFalse;

    if ( iOma2 )
        {
        TBuf<KUrlMaxLen> infoUrl;

        TInt error = aContent.GetStringAttribute( EInfoURL, infoUrl );
        if ( !error )
            {
            infoUrl.TrimLeft();
            if ( infoUrl.Length() > 0 )
                {
                aInfoUrl = HBufC8::NewL( infoUrl.Length() );
                aInfoUrl->Des().Copy( infoUrl );
                ret = ETrue;
                }
            else
                {
                ret = EFalse;
                }
            }
        else
            {
            ret = EFalse;
            }
        }
    else
        {
        User::Leave( KErrNotSupported );
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::HasInfoUrlL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMHelper::HasInfoUrlL(TDesC& aFileName, HBufC8*& aInfoUrl )
    {
    TBool ret = EFalse;

    if ( iOma2 )
        {
        TVirtualPathPtr virtualPath( aFileName );

        CData* content( NULL );
        TRAPD( r, content = CData::NewL( virtualPath, EPeek, EContentShareReadWrite ) );
        if ( r == KErrInUse )
            {
            content = CData::NewL( virtualPath, EPeek, EContentShareReadOnly );
            }
        else
            {
            User::LeaveIfError( r );
            }

        CleanupStack::PushL( content );

        ret = HasInfoUrlL( *content, aInfoUrl );
        CleanupStack::PopAndDestroy( content );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::OpenInfoUrlL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::OpenInfoUrlL( CData& aContent )
    {
    if ( iOma2 )
        {
        HBufC8* infoUrl;
        if ( HasInfoUrlL( aContent, infoUrl ) )
            {
            CleanupStack::PushL( infoUrl );
            LaunchBrowserL( infoUrl );
            CleanupStack::PopAndDestroy( infoUrl );
            }
        else
            {
            User::Leave( KErrArgument );
            }
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }


// -----------------------------------------------------------------------------
// CDRMHelper::OpenInfoUrlL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::OpenInfoUrlL(TDesC& aFileName )
    {
    if ( iOma2 )
        {
        TVirtualPathPtr virtualPath( aFileName );

        CData* content( NULL );
        TRAPD( r, content = CData::NewL( virtualPath, EPeek, EContentShareReadWrite ) );
        if ( r == KErrInUse )
            {
            content = CData::NewL( virtualPath, EPeek, EContentShareReadOnly );
            }
        else
            {
            User::LeaveIfError( r );
            }

        CleanupStack::PushL( content );

        OpenInfoUrlL( *content );
        CleanupStack::PopAndDestroy( content );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }

#pragma mark -

// -----------------------------------------------------------------------------
// CDRMHelper::DisplayQueryL
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::DisplayQueryWithIdL(
    TInt aTextResourceId,
    TInt aQueryResourceId )
    {
    TInt button(0);
    if ( iUseCoeEnv )
        {

#ifndef RD_MULTIPLE_DRIVE

        TFileName fileName( KDriveZ );

#else //RD_MULTIPLE_DRIVE

        _LIT( KDrive, "%c:");
        TInt driveNumber( -1 );
        TChar driveLetter;
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, driveNumber );
        iFs.DriveToChar( driveNumber, driveLetter );

        TFileName fileName;

        fileName.Format( KDrive, (TUint)driveLetter );

#endif

        fileName.Append( KDC_RESOURCE_FILES_DIR );
        fileName.Append( KCDRMHelperResFileName );
        RConeResourceLoader loader( *iCoeEnv );
        loader.OpenL( fileName );
        CleanupClosePushL( loader );

        HBufC* msgText = StringLoader::LoadLC( aTextResourceId, iCoeEnv );
        button = DisplayQueryL( *msgText, aQueryResourceId );
        CleanupStack::PopAndDestroy( msgText );
        CleanupStack::PopAndDestroy( &loader );
        }
    else
        {
        TInt buttonsId =
            aQueryResourceId == R_DRMHELPER_WAITING_RIGHTS_CONFIRMATION_QUERY ?
            R_AVKON_SOFTKEYS_OK_EMPTY__OK : R_AVKON_SOFTKEYS_YES_NO__YES;
        CDRMHelperGlobalNoteWrapper* noteWrapper =
            CDRMHelperGlobalNoteWrapper::NewLC( iStringResourceReader );

        button = noteWrapper->ShowNoteWithButtonsL( aTextResourceId, buttonsId );
        CleanupStack::PopAndDestroy( noteWrapper );
        }
    return button;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::DisplayQueryL
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::DisplayQueryWithIdValueL(
    TInt aTextResourceId,
    TInt aQueryResourceId,
    const TDesC& aString )
    {
    TInt button(0);
    if ( iUseCoeEnv )
        {

#ifndef RD_MULTIPLE_DRIVE

        TFileName fileName( KDriveZ );

#else //RD_MULTIPLE_DRIVE

        _LIT( KDrive, "%c:");
        TInt driveNumber( -1 );
        TChar driveLetter;
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, driveNumber );
        iFs.DriveToChar( driveNumber, driveLetter );

        TFileName fileName;

        fileName.Format( KDrive, (TUint)driveLetter );

#endif

        fileName.Append( KDC_RESOURCE_FILES_DIR );
        fileName.Append( KCDRMHelperResFileName );
        RConeResourceLoader loader( *iCoeEnv );
        loader.OpenL( fileName );
        CleanupClosePushL( loader );

        HBufC* msgText = StringLoader::LoadLC( aTextResourceId, aString, iCoeEnv );
        button = DisplayQueryL( *msgText, aQueryResourceId );
        CleanupStack::PopAndDestroy( msgText );
        CleanupStack::PopAndDestroy( &loader );
        }
    else
        {
        TInt buttonsId =
            aQueryResourceId == R_DRMHELPER_WAITING_RIGHTS_CONFIRMATION_QUERY ?
            R_AVKON_SOFTKEYS_OK_EMPTY__OK : R_AVKON_SOFTKEYS_YES_NO__YES;
        CDRMHelperGlobalNoteWrapper* noteWrapper =
            CDRMHelperGlobalNoteWrapper::NewLC( iStringResourceReader );

        button = noteWrapper->ShowNoteWithButtonsL( aTextResourceId, buttonsId, aString );
        CleanupStack::PopAndDestroy( noteWrapper );
        }
    return button;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::DisplayQueryL
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::DisplayQueryL( TInt aTextResourceId, TInt aValue )
    {
    TInt buttonCode( 0 );
    if ( iUseCoeEnv )
        {

#ifndef RD_MULTIPLE_DRIVE

        TFileName fileName( KDriveZ );

#else //RD_MULTIPLE_DRIVE

        _LIT( KDrive, "%c:");
        TInt driveNumber( -1 );
        TChar driveLetter;
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, driveNumber );
        iFs.DriveToChar( driveNumber, driveLetter );

        TFileName fileName;

        fileName.Format( KDrive, (TUint)driveLetter );

#endif

        fileName.Append( KDC_RESOURCE_FILES_DIR );
        fileName.Append( KCDRMHelperResFileName );
        RConeResourceLoader loader( *iCoeEnv );
        loader.OpenL( fileName );
        CleanupClosePushL( loader );
        TPtr bufPtr(0,0);

        HBufC* stringholder = StringLoader::LoadLC( aTextResourceId, aValue,
            iCoeEnv );
        CAknQueryDialog* dlg = CAknQueryDialog::NewL( CAknQueryDialog::ENoTone );
        CleanupStack::PushL( dlg );

        bufPtr.Set( stringholder->Des() );
        AknTextUtils::LanguageSpecificNumberConversion( bufPtr );
        PrepareSecondaryDisplayL( *dlg, aTextResourceId, KNullDesC, aValue );

        CleanupStack::Pop( dlg );

        buttonCode = dlg->ExecuteLD( R_DRMHELPER_CONFIRMATION_QUERY, *stringholder );

        CancelSecondaryDisplayL( aTextResourceId );
        CleanupStack::PopAndDestroy( stringholder );
        CleanupStack::PopAndDestroy( &loader );
        }
    else
        {
        CDRMHelperGlobalNoteWrapper* noteWrapper =
            CDRMHelperGlobalNoteWrapper::NewLC( iStringResourceReader );
        buttonCode = noteWrapper->ShowNoteL( aTextResourceId, aValue );
        CleanupStack::PopAndDestroy( noteWrapper );
        }
    return buttonCode;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::DisplayQueryL
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::DisplayQueryL( TInt aTextResourceId, const TDesC& aString )
    {
    TInt buttonCode( 0 );
    if ( iUseCoeEnv )
        {

#ifndef RD_MULTIPLE_DRIVE

        TFileName fileName( KDriveZ );

#else //RD_MULTIPLE_DRIVE

        _LIT( KDrive, "%c:");
        TInt driveNumber( -1 );
        TChar driveLetter;
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, driveNumber );
        iFs.DriveToChar( driveNumber, driveLetter );

        TFileName fileName;

        fileName.Format( KDrive, (TUint)driveLetter );

#endif

        fileName.Append( KDC_RESOURCE_FILES_DIR );
        fileName.Append( KCDRMHelperResFileName );
        RConeResourceLoader loader( *iCoeEnv );
        loader.OpenL( fileName );
        CleanupClosePushL( loader );
        TPtr bufPtr(0,0);

        HBufC* stringholder = StringLoader::LoadLC( aTextResourceId, aString,
            iCoeEnv );
        CAknQueryDialog* dlg(
            CAknQueryDialog::NewL( CAknQueryDialog::ENoTone ) );
        CleanupStack::PushL( dlg );

        bufPtr.Set( stringholder->Des() );
        AknTextUtils::LanguageSpecificNumberConversion( bufPtr );
        PrepareSecondaryDisplayL( *dlg, aTextResourceId, aString, -1 );
        CleanupStack::Pop( dlg );

        buttonCode = dlg->ExecuteLD( R_DRMHELPER_CONFIRMATION_QUERY, *stringholder );

        CancelSecondaryDisplayL( aTextResourceId );
        CleanupStack::PopAndDestroy( stringholder );
        CleanupStack::PopAndDestroy( &loader );
        }
    else
        {
        CDRMHelperGlobalNoteWrapper* noteWrapper =
            CDRMHelperGlobalNoteWrapper::NewLC( iStringResourceReader );
        buttonCode = noteWrapper->ShowNoteL( aTextResourceId, aString );
        CleanupStack::PopAndDestroy( noteWrapper );
        }
    return buttonCode;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::DisplayQueryL
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::DisplayQueryL(
    TInt aTextResourceId,
    const TDesC& aString,
    TInt aValue,
    TInt aStringPos,
    TInt aValuePos )
    {
    TInt buttonCode( 0 );
    if ( iUseCoeEnv )
        {

#ifndef RD_MULTIPLE_DRIVE

        TFileName fileName( KDriveZ );

#else //RD_MULTIPLE_DRIVE

        _LIT( KDrive, "%c:");
        TInt driveNumber( -1 );
        TChar driveLetter;
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, driveNumber );
        iFs.DriveToChar( driveNumber, driveLetter );

        TFileName fileName;

        fileName.Format( KDrive, (TUint)driveLetter );

#endif

        fileName.Append( KDC_RESOURCE_FILES_DIR );
        fileName.Append( KCDRMHelperResFileName );
        RConeResourceLoader loader( *iCoeEnv );
        loader.OpenL( fileName );
        CleanupClosePushL( loader );
        TPtr bufPtr( 0,0 );

        TBuf<KDRMHelperNoteMaxSize> textBuffer, textBuffer2;
        StringLoader::Load( textBuffer, aTextResourceId, iCoeEnv );
        StringLoader::Format( textBuffer2, textBuffer, aValuePos, aValue );
        textBuffer = textBuffer2;
        StringLoader::Format( textBuffer2, textBuffer, aStringPos, aString );

        bufPtr.Set( const_cast <TUint16*>( textBuffer2.Ptr() ), textBuffer2.Length(),
            textBuffer2.Length() );
        AknTextUtils::LanguageSpecificNumberConversion( bufPtr );

        CAknQueryDialog* dlg =
            CAknQueryDialog::NewL( CAknQueryDialog::ENoTone );
        PrepareSecondaryDisplayL( *dlg, aTextResourceId, aString, aValue );
        buttonCode =
            dlg->ExecuteLD( R_DRMHELPER_CONFIRMATION_QUERY, textBuffer2 );

        CancelSecondaryDisplayL( aTextResourceId );
        CleanupStack::PopAndDestroy( &loader );
        }
    else
        {
        CDRMHelperGlobalNoteWrapper* noteWrapper =
            CDRMHelperGlobalNoteWrapper::NewLC( iStringResourceReader );
        buttonCode = noteWrapper->ShowNoteL( aTextResourceId, aString,
            aValue, aStringPos, aValuePos );
        CleanupStack::PopAndDestroy( noteWrapper );
        }
    return buttonCode;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::DisplayQueryL
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::DisplayQueryL( TDesC& aPromptText, TInt aQueryResourceId )
    {
    CAknQueryDialog* dlg(
        new (ELeave) CAknQueryDialog( aPromptText, CAknQueryDialog::ENoTone ) );
    CleanupStack::PushL( dlg );

    TPtr bufPtr( 0,0 );
    bufPtr.Set( const_cast <TUint16*>( aPromptText.Ptr() ), aPromptText.Length(),
        aPromptText.Length() );
    AknTextUtils::LanguageSpecificNumberConversion( bufPtr );

    PrepareSecondaryDisplayL( *dlg, aQueryResourceId, aPromptText, -1 );

    CleanupStack::Pop( dlg );

    TInt ret = dlg->ExecuteLD( aQueryResourceId );
    CancelSecondaryDisplayL( aQueryResourceId );
    return ret;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::DisplayInfoNoteL
// -----------------------------------------------------------------------------
//
void CDRMHelper::DisplayInfoNoteL( TInt aTextResourceId )
    {
    if ( iUseCoeEnv )
        {

#ifndef RD_MULTIPLE_DRIVE

        TFileName fileName( KDriveZ );

#else //RD_MULTIPLE_DRIVE

        _LIT( KDrive, "%c:");
        TInt driveNumber( -1 );
        TChar driveLetter;
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, driveNumber );
        iFs.DriveToChar( driveNumber, driveLetter );

        TFileName fileName;

        fileName.Format( KDrive, (TUint)driveLetter );

#endif

        fileName.Append( KDC_RESOURCE_FILES_DIR );
        fileName.Append( KCDRMHelperResFileName );
        RConeResourceLoader loader( *iCoeEnv );
        loader.OpenL( fileName );
        CleanupClosePushL( loader );

        HBufC* msgText = StringLoader::LoadLC( aTextResourceId, iCoeEnv );
        DisplayInfoNoteL( *msgText, aTextResourceId );
        CleanupStack::PopAndDestroy( msgText );
        CleanupStack::PopAndDestroy( &loader );
        }
    else
        {

        TBuf<KDRMHelperNoteMaxSize> buffer =
            iStringResourceReader->ReadResourceString( aTextResourceId );

        DisplayInfoNoteL( buffer, aTextResourceId );
        }
    }

// -----------------------------------------------------------------------------
// CDRMHelper::DisplayInfoNoteL
// -----------------------------------------------------------------------------
//
void CDRMHelper::DisplayInfoNoteL( TInt aTextResourceId, const TDesC& aString )
    {
    if ( iUseCoeEnv )
        {

#ifndef RD_MULTIPLE_DRIVE

        TFileName fileName( KDriveZ );

#else //RD_MULTIPLE_DRIVE

        _LIT( KDrive, "%c:");
        TInt driveNumber( -1 );
        TChar driveLetter;
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, driveNumber );
        iFs.DriveToChar( driveNumber, driveLetter );

        TFileName fileName;

        fileName.Format( KDrive, (TUint)driveLetter );

#endif

        fileName.Append( KDC_RESOURCE_FILES_DIR );
        fileName.Append( KCDRMHelperResFileName );
        RConeResourceLoader loader( *iCoeEnv );
        loader.OpenL( fileName );
        CleanupClosePushL( loader );

        HBufC* msgText = StringLoader::LoadLC( aTextResourceId, aString,
            iCoeEnv );
        DisplayInfoNoteL( *msgText, aTextResourceId );
        CleanupStack::PopAndDestroy( msgText );
        CleanupStack::PopAndDestroy( &loader );
        }
    else
        {
        TBuf<KDRMHelperNoteMaxSize + KMaxFileName> destBuffer;


        TBuf<KDRMHelperNoteMaxSize> buffer =
            iStringResourceReader->ReadResourceString( aTextResourceId );


        StringLoader::Format( destBuffer, buffer, -1, aString );
        DisplayInfoNoteL( destBuffer, aTextResourceId, aString, -1 );
        }
    }

// -----------------------------------------------------------------------------
// CDRMHelper::DisplayInfoNoteL
// -----------------------------------------------------------------------------
//
void CDRMHelper::DisplayInfoNoteL( TInt aTextResourceId, TInt aValue )
    {
    if ( iUseCoeEnv )
        {

#ifndef RD_MULTIPLE_DRIVE

        TFileName fileName( KDriveZ );

#else //RD_MULTIPLE_DRIVE

        _LIT( KDrive, "%c:");
        TInt driveNumber( -1 );
        TChar driveLetter;
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, driveNumber );
        iFs.DriveToChar( driveNumber, driveLetter );

        TFileName fileName;

        fileName.Format( KDrive, (TUint)driveLetter );

#endif

        fileName.Append( KDC_RESOURCE_FILES_DIR );
        fileName.Append( KCDRMHelperResFileName );
        RConeResourceLoader loader( *iCoeEnv );
        loader.OpenL( fileName );
        CleanupClosePushL( loader );

        HBufC* msgText = StringLoader::LoadLC( aTextResourceId, aValue,
            iCoeEnv );
        DisplayInfoNoteL( *msgText, aTextResourceId, KNullDesC, aValue );
        CleanupStack::PopAndDestroy( msgText );
        CleanupStack::PopAndDestroy( &loader );
        }
    else
        {
        TBuf<KDRMHelperNoteMaxSize + KMaxFileName> destBuffer;


        TBuf<KDRMHelperNoteMaxSize> buffer =
            iStringResourceReader->ReadResourceString( aTextResourceId );


        StringLoader::Format( destBuffer, buffer, -1, aValue );
        DisplayInfoNoteL( destBuffer, aTextResourceId, KNullDesC, aValue );
        }
    }

// -----------------------------------------------------------------------------
// CDRMHelper::DisplayInfoNoteL
// -----------------------------------------------------------------------------
//
void CDRMHelper::DisplayInfoNoteL( TDesC& aPromptText, TInt aResourceId,
    const TDesC& aString, TInt aValue )
    {
    TPtr bufPtr(0,0);
    bufPtr.Set(
        const_cast <TUint16*>( aPromptText.Ptr() ),
        aPromptText.Length(),
        aPromptText.Length() );
    AknTextUtils::LanguageSpecificNumberConversion( bufPtr );

    if ( iUseCoeEnv )
        {
        CAknResourceNoteDialog* note( CreateNoteForResourceL( aResourceId ) );

        PrepareSecondaryDisplayL( *note, aResourceId, aString, aValue );
        note->ExecuteLD( aPromptText );
        CancelSecondaryDisplayL( aResourceId );
        }
    else
        {
        CDRMHelperInfoNoteWrapper* note = CDRMHelperInfoNoteWrapper::NewLC();
        User::LeaveIfError(iNoteList.Append(note)); //a list for simultanous notes
        CleanupStack::Pop( note );
        note->ShowNoteL( GlobalNoteTypeForResource( aResourceId ),
            aPromptText,
            aResourceId,
            aString,
            aValue );
        }
    }

#pragma mark -

// -----------------------------------------------------------------------------
// CDRMHelper::StripPathAndExtensionL
// -----------------------------------------------------------------------------
//
TPtrC CDRMHelper::StripPathAndExtension( const TDesC& aFileName )
    {
    // find index of last backslash
    TInt index = aFileName.LocateReverse( '\\' );

    // get filename+extension part
    TPtrC filenameWithExtension = aFileName.Right(
        aFileName.Length() - (index + 1) );

    // get index of dot
    index = filenameWithExtension.Locate( '.' );

    // if dot not found, there's no extension
    if ( index == KErrNotFound )
        {
        index = (filenameWithExtension.Length() - 1);
        }

    // strip extension
    return filenameWithExtension.Left( index );
    }

// -----------------------------------------------------------------------------
// CDRMHelper::GetFileHandleFromURIL
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::GetFileHandleFromURIL( const TDesC8& aURI, RFile& aFileHandle )
    {
    CDcfRep* rep = CDcfRep::NewL();
    CleanupStack::PushL( rep );
    rep->GetFileHandleL( aURI, aFileHandle, iFs );
    CleanupStack::PopAndDestroy( rep );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::Intent
// -----------------------------------------------------------------------------
//
TInt CDRMHelper::Intent( const HBufC* aMimeType )
    {
    TInt intent = ContentAccess::EUnknown;

    _LIT( KDRMHelperMimeTypeAudio, "Audio" );
    _LIT( KDRMHelperMimeTypeVideo, "Video" );
    _LIT( KDRMHelperMimeTypeImage, "Image" );
    _LIT( KDRMHelperMimeTypeApplication, "Application" );
    _LIT( KDRMHelperMimeTypeShockwave, "Application/x-shockwave-flash" );
    _LIT( KDRMHelperMimeTypeRingingTone, "Application/vnd.nokia.ringing-tone" );
    _LIT( KDRMHelperMimeTypeSkin, "Application/x-series60-skn" );
    _LIT( KDRMHelperMimeTypePIP, "Application/x-pip" );
    _LIT( KDRMHelperMimeTypeSis, "Application/vnd.symbian.install" );
    _LIT( KDRMHelperMimeTypeRealMedia, "Application/x-pn-realmedia" );
    _LIT( KDRMHelperMimeTypeVndRealMedia, "Application/vnd.rn-realmedia" );

    if ( iAutomatedType == EAutomatedTypeTheme)
        {
        // if automated theme
        return DRMCommon::EDisplay;
        }

    if ( aMimeType )
        {
        if ( !aMimeType->FindF( KDRMHelperMimeTypeRingingTone ) )
            {
            // nokia ringning tone
            intent = DRMCommon::EPlay;
            }
        else if ( !aMimeType->FindF( KDRMHelperMimeTypeSkin ) )
            {
            // skin package
            intent = DRMCommon::EDisplay;
            }
        else if ( !aMimeType->FindF( KDRMHelperMimeTypePIP ) )
            {
            // PIP package
            intent = DRMCommon::EExecute;
            }
        else if ( !aMimeType->FindF( KDRMHelperMimeTypeSis ) )
            {
            // executable application or skin
            intent = DRMCommon::EExecute;
            }
        else if ( !aMimeType->FindF( KDRMHelperMimeTypeShockwave ) )
            {
            // shockwave flash
            intent = DRMCommon::EExecute;
            }
        else if ( !aMimeType->FindF( KDRMHelperMimeTypeRealMedia ) ||
            !aMimeType->FindF( KDRMHelperMimeTypeVndRealMedia ) )
            {
            // realmedia
            intent = DRMCommon::EPlay;
            }
        else if ( !aMimeType->FindF( KDRMHelperMimeTypeApplication ) )
            {
            // executable application
            intent = DRMCommon::EExecute;
            }
        else if ( !aMimeType->FindF( KDRMHelperMimeTypeImage ) )
            {
            // image
            intent = DRMCommon::EDisplay;
            }
        else if ( !aMimeType->FindF( KDRMHelperMimeTypeVideo ) )
            {
            // video
            intent = DRMCommon::EPlay;
            }
        else if ( !aMimeType->FindF( KDRMHelperMimeTypeAudio ) )
            {
            // audio
            intent = DRMCommon::EPlay;
            }
        }
    return intent;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::MimeTypeFromURIL
// -----------------------------------------------------------------------------
//
HBufC* CDRMHelper::MimeTypeFromURIL( const TDesC8& aURI )
    {
    HBufC* mimeType = NULL;
    RFile fileHandle;
    TInt error ( KErrNone );
    CleanupClosePushL(fileHandle);

    if ( !GetFileHandleFromURIL( aURI, fileHandle ) )
        {
        CData* content = CData::NewLC( fileHandle, KDefaultContentObject, EPeek );

        mimeType = HBufC::NewLC( KMaxDataTypeLength );
        TPtr ptr = mimeType->Des();
        error = content->GetStringAttribute( EMimeType, ptr );
        if ( error != KErrNone )
            {
            User::Leave( KErrArgument );
            }
        CleanupStack::Pop( mimeType );
        CleanupStack::PopAndDestroy( content );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    CleanupStack::PopAndDestroy( &fileHandle );
    return mimeType;
    }

EXPORT_C TInt CDRMHelper::GetContentURIList(
    RPointerArray<HBufC8>*& aURIList)
    {
    return iDRMCommon->GetContentURIList( aURIList );
    }

EXPORT_C TInt CDRMHelper::DataTypesCount(
    TInt& aCount)
    {
    return iDRMCommon->DataTypesCount( aCount );
    }

EXPORT_C TInt CDRMHelper::SupportedDataType(
    const TInt aIndex, TDataType& aDataType)
    {
    return iDRMCommon->SupportedDataType( aIndex, aDataType );
    }

EXPORT_C TInt CDRMHelper::RegisterDataType(
    const TDataType& aDataType)
    {
    return iDRMCommon->RegisterDataType( aDataType );
    }

EXPORT_C TInt CDRMHelper::UnRegisterDataType(
    const TInt aIndex )
    {
    return iDRMCommon->UnRegisterDataType( aIndex );
    }

EXPORT_C TInt CDRMHelper::SupportedDRMMethods2(
    TInt& aDRMMethod, TDRMHelperOMALevel& aOMALevel)
    {
#ifndef __DRM_FULL
    aDRMMethod = CDRMHelper::EForwardLock;
#else
    aDRMMethod =
        CDRMHelper::EForwardLock |
        CDRMHelper::ECombinedDelivery |
        CDRMHelper::ESeparateDelivery |
        CDRMHelper::ESuperDistribution;
#ifdef __DRM_OMA2
    aOMALevel = EOMA_2_0;
#else
    aOmaLevel = EOMA_1_0;
#endif // __DRM_OMA2

#endif // __DRM_FULL
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::HandleServerAppExit
// -----------------------------------------------------------------------------
//
void CDRMHelper::HandleServerAppExit( TInt aReason )
    {
    if ( aReason == EAknCmdExit )
        {
        CAknEnv::RunAppShutter();
        }

    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }

// -----------------------------------------------------------------------------
// CDRMHelper::PrepareSecondaryDisplayL
// -----------------------------------------------------------------------------
//
void CDRMHelper::PrepareSecondaryDisplayL( CEikDialog& aDialog , TInt aResourceId,
    const TDesC& aString, TInt aValue )
    {

    // Check if we have cover display and shall we display something
    if ( !FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) ||
        !CDRMHelperInfoNoteWrapper::EvaluateCoverResourceId( aResourceId ) )
        {
        return;
        }

    RProcess myProcess;
    TUid myProcessUid( KNullUid );

    RThread().Process( myProcess );
    myProcessUid = myProcess.Identity();

    aDialog.PublishDialogL( aResourceId, KUidCoverUiCategoryDRMHelper );

    // fetch akn utility for mediator support
    CAknMediatorFacade* covercl = AknMediatorFacade( &aDialog );

    if ( covercl ) // returns null if cover display is not available
        {
        // Package dialog data <oem/SecondaryDisplay/DRMHelperSecondaryDisplay.h>
        THelperSDData helperData;

        helperData.iHelperUid = KUidCoverUiCategoryDRMHelper;        // First field is DRMHelper's Uid
        helperData.iHandlerProcessId = myProcessUid; // ProcessId which uses DRMHelper
        helperData.iCreatorProcessId = User::CreatorSecureId();

        if ( aString.Compare( KNullDesC ) )
            {
            // If there is filename given, it's always in the PrimaryString
            helperData.iStringParam.Append( aString );
            }
        if ( aValue >= 0 )
            {
            // If there is no other than numeric data, put it as NumericString
            helperData.iNumParam.AppendNum( aValue );
            }
        THelperSDDataPckg pckg( helperData );

        covercl->BufStream() << pckg;   // Write the package data
        covercl->BufStream().CommitL(); // no more data to send so commit buf
        }
    return;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::CancelSecondaryDisplayL
// -----------------------------------------------------------------------------
//
void CDRMHelper::CancelSecondaryDisplayL( TInt aResourceId )
    {

    // Check if we have cover display and shall we display something
    if ( !FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) ||
        !CDRMHelperInfoNoteWrapper::EvaluateCoverResourceId( aResourceId ) )
        {
        return;
        }

    RProcess myProcess;
    TUid myProcessUid( KNullUid );

    RThread().Process( myProcess );
    myProcessUid = myProcess.Identity();

    THelperSDDataCancel cancelData;
    cancelData.iHandlerProcessId = myProcessUid;
    cancelData.iCreatorProcessId = User::CreatorSecureId();
    cancelData.iNoteId = aResourceId;

    TPckgBuf<THelperSDDataCancel> cancel( cancelData );

    HBufC8* buf = HBufC8::NewLC(sizeof(THelperSDDataCancel) + sizeof(TInt));
    TPtr8 ptr = buf->Des();

    RDesWriteStream stream(ptr);
    stream << cancel;
    stream.CommitL();

    iEventProvider->RaiseEvent( KMediatorSecondaryDisplayDomain,
        KUidCoverUiCategoryDRMHelper,
        ECover_dialog_cancel,
        TVersion( 0, 0, 0 ),
        *buf );

    stream.Close();
    CleanupStack::PopAndDestroy( buf );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
