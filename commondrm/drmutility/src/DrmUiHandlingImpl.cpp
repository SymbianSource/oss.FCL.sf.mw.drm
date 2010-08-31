/*
 * Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  DRM UI Handling implementation class
 *
 */

// INCLUDE FILES


// publish & subscribe
#include <e32property.h>
#include <PSVariables.h>
#include <centralrepository.h>

// coeenv
#include <coemain.h>
#include <apgtask.h>
#include <avkon.hrh>
#include <aknenv.h>

// caf
#include <caf/data.h>
#include <caf/caftypes.h>

// launching embedded details view
#include <AknLaunchAppService.h>
#include <AiwGenericParam.h>
#include <apgcli.h>

// DCF repository
#include <DcfRep.h>
#include <DcfEntry.h>

// character conversions
#include <utf.h>

// resources
#include <data_caging_path_literals.hrh>
#include <drmutility.rsg>

// drm
#include <Oma2Agent.h>
#include <drmagents.h>
#include <DrmPermission.h>
#include <DrmConstraint.h>
#include <DRMRightsClient.h>
#include <drmutility.h>
#include <drmutilitytypes.h>
#include <drmasyncobserver.h>
#include <drmhandleerrorobserver.h>
#include <drmbrowserlauncher.h>

//connectivity logic
#include <cmconnectionmethod.h> // RCmConnectionMethod
#include <cmdestination.h> // RCmDestination
#include <cmmanager.h> // RCmManager



#ifdef _DEBUG
#include <e32debug.h>
#endif

#include "drmutilitycommon.h"
#include "drmutilityui.h"
#include "DrmUiHandlingImpl.h"
#include "drmuihandlingdata.h"
#include "DrmUtilityDownloadManager.h"
#include "drmutilityinternaltypes.h"
#include "drmuicheckrightsobserver.h"
#include "drmutilitywmdrm.h"
#include "drmutilitywmdrmwrapper.h"

#include "RoapStorageClient.h"
#include "DRMTypes.h"
#include "drmsettingsplugininternalcrkeys.h"
#include "DRMRIContext.h"
#include "DRMDomainContext.h"

#include "DrmUtilityInternalcrkeys.h"      // Cenrep extension for OmaBased
#include "drmuidialogids.h"

// CONSTANTS
const TInt KCommandHandleErrorFile( 1 );
const TInt KCommandHandleErrorData( 2 );
const TInt KCommandCheckRightsAmountFile( 3 );
const TInt KCommandCheckRightsAmountData( 4 );
const TInt KCommandShowDetailsViewFile( 5 );
const TInt KCommandShowDetailsViewData( 6 );
const TInt KCommandHandleUrlFile( 7 );
const TInt KCommandHandleUrlData( 8 );
const TInt KCommandAvailableUrlsFile( 9 );
const TInt KCommandAvailableUrlsData( 10 );

_LIT( KEncryptedRightsIssuerMatchString, "flk*" );

// buffer sizes
const TInt KDRMMaxETABufSize( 11 );
const TInt KUrlMaxLen( 1024 );
const TInt KIntegerMaxLen( 10 );

// constant sizes
const TInt KOmaDomainGenerationLength( 3 );

// masks for constraint existence
const TInt KDRMOmaConstraintCount( 1 );
const TInt KDRMOmaConstraintTime( 2 );
const TInt KDRMOmaConstraintAccumulated( 4 );

const TInt KCenRepDataLength( 50 );

const TInt KresOmaBasedBuf( 512 );
const TInt KNameBuf( 256 );
const TInt KNoValue = -1;

#ifdef _DEBUG
// debug panic
_LIT( KDRMUtilityDebugPanicMessage, "DrmUiHandlingDebugPanic" );
const TInt KDRMUtilityDebugPanicCode( 1 );
#endif

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
// MapToCallError
// -----------------------------------------------------------------------------
//
LOCAL_C inline TInt CheckAndMapToCallError(
    RDRMRightsClient& aOmaClient,
    ContentAccess::TIntent aIntent,
    const TDesC8& aContentUri )
    {
    TUint32 rr( KErrNone );
    TInt err( aOmaClient.CheckRights( aIntent, aContentUri, rr ) );
    return err ? KErrCANoRights : KErrNone;
    }

// -----------------------------------------------------------------------------
// HasDefConn
// -----------------------------------------------------------------------------
//
inline TBool HasDefConn()
    {
    TBool found( EFalse );

    TRAP_IGNORE( found = HasDefaultConnectionL() );

    return found;
    }

// ---------------------------------------------------------
// UpdateDCFRepositoryL()
// Update saved file to DCFRepository
// ---------------------------------------------------------

LOCAL_C void UpdateDCFRepositoryL( TDesC& aFullFileName )
    {
    CDcfEntry* dcfEntry( NULL );
    CDcfRep* dcfRep( NULL );
    
    dcfEntry = CDcfEntry::NewL();
    CleanupStack::PushL( dcfEntry );

    dcfRep = CDcfRep::NewL();
    CleanupStack::PushL( dcfRep );

    dcfEntry->SetLocationL( aFullFileName, 0 );
    dcfRep->UpdateL( dcfEntry );
    CleanupStack::PopAndDestroy( dcfRep );
    CleanupStack::PopAndDestroy( dcfEntry );
    }

// ---------------------------------------------------------
// FindFullFileNameAndUpdateDCFRepositoryL()
// Find full file name for the content and update saved 
// file to DCFRepository
// ---------------------------------------------------------

LOCAL_C void FindFullFileNameAndUpdateDCFRepositoryL(
    const RFile& aFileHandle )
    {
    HBufC* fullPath( NULL );
    TPtr fullPathPtr( NULL, 0 );
        
    fullPath = HBufC::NewLC( KUrlMaxLen );
    fullPathPtr.Set( fullPath->Des() );
    
    aFileHandle.FullName( fullPathPtr );
    UpdateDCFRepositoryL( fullPathPtr );
    CleanupStack::PopAndDestroy( fullPath );
    }

// ---------------------------------------------------------
// FindFullFileNameAndUpdateDCFRepositoryL()
// Find full file name for the content and update saved 
// file to DCFRepository
// ---------------------------------------------------------

LOCAL_C void FindFullFileNameAndUpdateDCFRepositoryL(
    const ContentAccess::CData& aFileData  )
    {
    HBufC* fullPath( NULL );
    TPtr fullPathPtr( NULL, 0 );
    
    fullPath = HBufC::NewLC( KUrlMaxLen );
    fullPathPtr.Set( fullPath->Des() );
                
    aFileData.GetStringAttribute( DRM::EDrmFullName, 
        fullPathPtr );
    
    UpdateDCFRepositoryL( fullPathPtr );
    CleanupStack::PopAndDestroy( fullPath );    
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CDrmUiHandlingImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
DRM::CDrmUiHandlingImpl::CDrmUiHandlingImpl(
    DRM::CDrmUtility* aDrmUtility,
    CCoeEnv* aCoeEnv ) :
    CActive( EPriorityStandard ), iCoeEnv( aCoeEnv ), iDrmUtility(
        aDrmUtility )
    {
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::ConstructL()
    {
    TInt err( KErrNone );

    iDrmUtilityCommon = DRM::CDrmUtilityCommon::NewL();

    iDrmUtilityUi = DRM::CDrmUtilityUI::NewL( iCoeEnv );

    User::LeaveIfError( iOmaClient.Connect() );

    User::LeaveIfError( iSemaphore.CreateLocal( 1 ) );

        TRAP_IGNORE( iWrapperLoader = DRM::CDrmUtilityWMDrm::NewL( iCoeEnv ) );

    if ( iWrapperLoader )
        {
        iWmClient = iWrapperLoader->GetWrapper();
        }
        TRAP(err, GetOmaBasedInfoL() );
    if ( err )
        {
        if ( iOmaBasedAgentName )
            {
            delete iOmaBasedAgentName;
            }
        iOmaBasedAgentName = NULL;
        iOmaBasedUid = 0;
        if ( iOmaBasedMimeType )
            {
            delete iOmaBasedMimeType;
            }
        iOmaBasedMimeType = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::GetOmaBasedInfoL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::GetOmaBasedInfoL()
    {
    TInt err = KErrNone;
    CRepository* repository( NULL );
    RBuf bOmaBasedAgentName;
    RBuf bOmaBasedMimeType;

    CleanupClosePushL( bOmaBasedAgentName );
    CleanupClosePushL( bOmaBasedMimeType );
    bOmaBasedAgentName.CreateL( KCenRepDataLength );
    bOmaBasedMimeType.CreateL( KCenRepDataLength );

        TRAP( err, repository = CRepository::NewL( KCRUidOmaBased ) );
    if ( !err )
        {
        CleanupStack::PushL( repository );

        err = repository->Get( KDrmOmaBasedName, bOmaBasedAgentName );
        if ( !err )
            {
            iOmaBasedAgentName = bOmaBasedAgentName.AllocL();
            }

        err = repository->Get( KDrmOmaBasedUID, iOmaBasedUid );

        err = repository->Get( KOmaBasedMimeType, bOmaBasedMimeType );
        if ( !err )
            {
            iOmaBasedMimeType = CnvUtfConverter::ConvertFromUnicodeToUtf8L(
                bOmaBasedMimeType );
            }
        CleanupStack::PopAndDestroy( repository );
        }

    CleanupStack::PopAndDestroy( 2, &bOmaBasedAgentName );
    User::LeaveIfError( err );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmUiHandlingImpl* DRM::CDrmUiHandlingImpl::NewL(
    DRM::CDrmUtility* aDrmUtility,
    CCoeEnv* aCoeEnv )
    {
    DRM::CDrmUiHandlingImpl* self( NewLC( aDrmUtility, aCoeEnv ) );

    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmUiHandlingImpl* DRM::CDrmUiHandlingImpl::NewLC(
    DRM::CDrmUtility* aDrmUtility,
    CCoeEnv* aCoeEnv )
    {
    DRM::CDrmUiHandlingImpl* self( new ( ELeave ) CDrmUiHandlingImpl(
        aDrmUtility, aCoeEnv ) );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::~CDrmUiHandlingImpl()
// Destructor
// -----------------------------------------------------------------------------
//
DRM::CDrmUiHandlingImpl::~CDrmUiHandlingImpl()
    {
    DRM::CDrmUiHandlingData* data( PopFront() );

    // Empty the queue:
    while ( data )
        {
        // Complete the outstanding requestest with cancel
        data->iObserver->OperationCompleted( data->iOperationId, KErrCancel );
        delete data;
        data = PopFront();
        }

    delete iDrmUtilityCommon;

    delete iDrmUtilityUi;

    delete iWrapperLoader;

    delete iOmaBasedAgentName;

    delete iOmaBasedMimeType;

    iOmaClient.Close();

    // Close the semaphone
    iSemaphore.Close();

    // Remove the object from active scheduler etc.
    if ( IsAdded() )
        {
        Deque();
        }
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CheckRightsAmountAsyncL
// Check how much rights there are left for the content.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiHandlingImpl::CheckRightsAmountAsyncL(
    RFile& aFile,
    ContentAccess::TIntent aIntent,
    DRM::MDrmUiCheckRightsObserver& aObserver )
    {
    TRequestStatus* status( &iStatus );

    // Create data object:
    DRM::CDrmUiHandlingData* data( DRM::CDrmUiHandlingData::NewLC() );
    data->iObserver = &aObserver;
    data->iOperationId = reinterpret_cast<TInt> ( data );
    data->iOperation = KCommandCheckRightsAmountFile;
    data->iFile = &aFile;
    data->iIntent = aIntent;
    data->iCheckRightsObserver = &aObserver;

    // Add it to the queue:
    AppendToQueue( data );

    // Pop it from the cleanup stack
    CleanupStack::Pop( data );

    Activate( status );

    return data->iOperationId;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CheckRightsAmountL
// Check how much rights there are left for the content.
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUiHandlingImpl::CheckRightsAmountL(
    RFile& aFile,
    ContentAccess::TIntent aIntent,
    DRM::MDrmUiCheckRightsObserver* aObserver )
    {
    DRM::CDrmUiHandlingData* data( DRM::CDrmUiHandlingData::NewLC() );

    data->iOperationId = 0; // operation id zero for a syncronous operation
    data->iFile = &aFile;
    data->iIntent = aIntent;
    data->iCheckRightsObserver = aObserver;

    PerformCheckRightsAmountFileL( *data );

    CleanupStack::PopAndDestroy( data );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CheckRightsAmountAsyncL
// Check how much rights there are left for the content.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiHandlingImpl::CheckRightsAmountAsyncL(
    ContentAccess::CData& aFile,
    ContentAccess::TIntent aIntent,
    DRM::MDrmUiCheckRightsObserver& aObserver )
    {
    TRequestStatus* status( &iStatus );

    // create data object:
    DRM::CDrmUiHandlingData* data( DRM::CDrmUiHandlingData::NewLC() );

    data->iObserver = &aObserver;
    data->iOperationId = reinterpret_cast<TInt> ( data );
    data->iOperation = KCommandCheckRightsAmountData;
    data->iFileData = &aFile;
    data->iIntent = aIntent;
    data->iCheckRightsObserver = &aObserver;

    // Add it to the queue:
    AppendToQueue( data );

    // Pop it from the cleanup stack
    CleanupStack::Pop( data );

    Activate( status );

    return data->iOperationId;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CheckRightsAmountL
// Check how much rights there are left for the content.
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUiHandlingImpl::CheckRightsAmountL(
    ContentAccess::CData& aFile,
    ContentAccess::TIntent aIntent,
    DRM::MDrmUiCheckRightsObserver* aObserver )
    {
    DRM::CDrmUiHandlingData* data( DRM::CDrmUiHandlingData::NewLC() );

    data->iOperationId = 0; // operation id zero for a syncronous operation
    data->iFileData = &aFile;
    data->iIntent = aIntent;
    data->iCheckRightsObserver = aObserver;

    PerformCheckRightsAmountDataL( *data );

    CleanupStack::PopAndDestroy( data );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::ShowDetailsViewAsyncL
// Shows DRM specific details
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiHandlingImpl::ShowDetailsViewAsyncL(
    RFile& aFile,
    DRM::MDrmAsyncObserver& aObserver )
    {
    TRequestStatus* status( &iStatus );

    // create data object:
    DRM::CDrmUiHandlingData* data( DRM::CDrmUiHandlingData::NewLC() );

    data->iObserver = &aObserver;
    data->iOperationId = reinterpret_cast<TInt> ( data );
    data->iOperation = KCommandShowDetailsViewFile;
    data->iFile = &aFile;

    // Add it to the queue:
    AppendToQueue( data );

    // Pop it from the cleanup stack
    CleanupStack::Pop( data );

    Activate( status );

    return data->iOperationId;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::ShowDetailsViewL
// Shows DRM specific details
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUiHandlingImpl::ShowDetailsViewL( RFile& aFile )
    {
    DRM::CDrmUiHandlingData* data( DRM::CDrmUiHandlingData::NewLC() );

    data->iOperationId = 0; // operation id zero for a syncronous operation
    data->iFile = &aFile;

    PerformShowDetailsViewFileL( *data );

    CleanupStack::PopAndDestroy( data );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::ShowDetailsViewAsyncL
// Shows DRM specific details
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiHandlingImpl::ShowDetailsViewAsyncL(
    ContentAccess::CData& aFile,
    DRM::MDrmAsyncObserver& aObserver )
    {
    TRequestStatus* status( &iStatus );

    // create data object:
    DRM::CDrmUiHandlingData* data( DRM::CDrmUiHandlingData::NewLC() );

    data->iObserver = &aObserver;
    data->iOperationId = reinterpret_cast<TInt> ( data );
    data->iOperation = KCommandShowDetailsViewData;
    data->iFileData = &aFile;

    // Add it to the queue:
    AppendToQueue( data );

    // Pop it from the cleanup stack
    CleanupStack::Pop( data );

    Activate( status );

    return data->iOperationId;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::ShowDetailsViewL
// Shows DRM specific details.
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUiHandlingImpl::ShowDetailsViewL(
    ContentAccess::CData& aFile )
    {
    DRM::CDrmUiHandlingData* data( DRM::CDrmUiHandlingData::NewLC() );

    data->iOperationId = 0; // operation id zero for a syncronous operation
    data->iFileData = &aFile;

    PerformShowDetailsViewDataL( *data );

    CleanupStack::PopAndDestroy( data );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::HandleUrlAsyncL
// Handles a specific url defined by the file, such as InfoUrl
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiHandlingImpl::HandleUrlAsyncL(
    RFile& aFile,
    DRM::TDrmUiUrlType aType,
    DRM::MDrmAsyncObserver& aObserver )
    {
    TRequestStatus* status( &iStatus );

    // create data object:
    DRM::CDrmUiHandlingData* data( DRM::CDrmUiHandlingData::NewLC() );

    data->iObserver = &aObserver;
    data->iOperationId = reinterpret_cast<TInt> ( data );
    data->iOperation = KCommandHandleUrlFile;
    data->iFile = &aFile;
    data->iTypeParam = aType;

    // Add it to the queue:
    AppendToQueue( data );

    // Pop it from the cleanup stack
    CleanupStack::Pop( data );

    Activate( status );

    return data->iOperationId;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::HandleUrlL
// Handles a specific url defined by the file, such as InfoUrl
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUiHandlingImpl::HandleUrlL(
    RFile& aFile,
    DRM::TDrmUiUrlType aType )
    {
    DRM::CDrmUiHandlingData* data( DRM::CDrmUiHandlingData::NewLC() );

    data->iOperationId = 0; // operation id zero for a syncronous operation
    data->iFile = &aFile;
    data->iTypeParam = aType;

    PerformHandleUrlFileL( *data );

    CleanupStack::PopAndDestroy( data );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::HandleUrlAsyncL
// Handles the specific url defined by the file, such as InfoUrl
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiHandlingImpl::HandleUrlAsyncL(
    ContentAccess::CData& aFile,
    DRM::TDrmUiUrlType aType,
    DRM::MDrmAsyncObserver& aObserver )
    {
    TRequestStatus* status( &iStatus );

    // create data object:
    DRM::CDrmUiHandlingData* data( DRM::CDrmUiHandlingData::NewLC() );

    data->iObserver = &aObserver;
    data->iOperationId = reinterpret_cast<TInt> ( data );
    data->iOperation = KCommandHandleUrlData;
    data->iFileData = &aFile;
    data->iTypeParam = aType;

    // Add it to the queue:
    AppendToQueue( data );

    // Pop it from the cleanup stack
    CleanupStack::Pop( data );

    Activate( status );

    return data->iOperationId;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::HandleUrlL
// Handles the specific url defined by the file, such as InfoUrl
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUiHandlingImpl::HandleUrlL(
    ContentAccess::CData& aFile,
    DRM::TDrmUiUrlType aType )
    {
    DRM::CDrmUiHandlingData* data( DRM::CDrmUiHandlingData::NewLC() );

    data->iOperationId = 0; // operation id zero for a syncronous operation
    data->iFileData = &aFile;
    data->iTypeParam = aType;

    PerformHandleUrlDataL( *data );

    CleanupStack::PopAndDestroy( data );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::AvailableUrlsAsyncL
// Gets information of the available urls
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiHandlingImpl::AvailableUrlsAsyncL(
    RFile& aFile,
    DRM::TDrmUiUrlType& aType,
    DRM::MDrmAsyncObserver& aObserver )
    {
    TRequestStatus* status( &iStatus );

    // create data object:
    DRM::CDrmUiHandlingData* data( DRM::CDrmUiHandlingData::NewLC() );

    data->iObserver = &aObserver;
    data->iOperationId = reinterpret_cast<TInt> ( data );
    data->iOperation = KCommandAvailableUrlsFile;
    data->iFile = &aFile;
    data->iType = &aType;

    // Add it to the queue:
    AppendToQueue( data );

    // Pop it from the cleanup stack
    CleanupStack::Pop( data );

    Activate( status );

    return data->iOperationId;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::AvailableUrlsL
// Gets information of the available urls
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUiHandlingImpl::AvailableUrlsL(
    RFile& aFile,
    DRM::TDrmUiUrlType& aType )
    {
    DRM::CDrmUiHandlingData* data( DRM::CDrmUiHandlingData::NewLC() );

    data->iOperationId = 0; // operation id zero for a syncronous operation
    data->iFile = &aFile;
    data->iType = &aType;

    PerformAvailableUrlsFileL( *data );

    CleanupStack::PopAndDestroy( data );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::AvailableUrlsAsyncL
// Gets information of the available urls
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiHandlingImpl::AvailableUrlsAsyncL(
    ContentAccess::CData& aFile,
    DRM::TDrmUiUrlType& aType,
    DRM::MDrmAsyncObserver& aObserver )
    {
    TRequestStatus* status( &iStatus );

    // create data object:
    DRM::CDrmUiHandlingData* data( DRM::CDrmUiHandlingData::NewLC() );

    data->iObserver = &aObserver;
    data->iOperationId = reinterpret_cast<TInt> ( data );
    data->iOperation = KCommandAvailableUrlsData;
    data->iFileData = &aFile;
    data->iType = &aType;

    // Add it to the queue:
    AppendToQueue( data );

    // Pop it from the cleanup stack
    CleanupStack::Pop( data );

    Activate( status );

    return data->iOperationId;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::AvailableUrlsL
// Gets information of the available urls
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUiHandlingImpl::AvailableUrlsL(
    ContentAccess::CData& aFile,
    DRM::TDrmUiUrlType& aType )
    {
    DRM::CDrmUiHandlingData* data( DRM::CDrmUiHandlingData::NewLC() );

    data->iOperationId = 0; // operation id zero for a syncronous operation
    data->iFileData = &aFile;
    data->iType = &aType;

    PerformAvailableUrlsDataL( *data );

    CleanupStack::PopAndDestroy( data );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::HandleErrorAsyncL
// Handles DRM specific error
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiHandlingImpl::HandleErrorAsyncL(
    RFile& aFile,
    ContentAccess::TIntent aIntent,
    TInt aError,
    DRM::MDrmHandleErrorObserver& aObserver )
    {
    TRequestStatus* status( &iStatus );

    // create data object:
    DRM::CDrmUiHandlingData* data( DRM::CDrmUiHandlingData::NewLC() );

    data->iObserver = &aObserver;
    data->iOperationId = reinterpret_cast<TInt> ( data );
    data->iOperation = KCommandHandleErrorFile;
    data->iFile = &aFile;
    data->iIntent = aIntent;
    data->iError = aError;
    data->iErrorObserver = &aObserver;

    // Add it to the queue:
    AppendToQueue( data );

    // Pop it from the cleanup stack
    CleanupStack::Pop( data );

    Activate( status );

    return data->iOperationId;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::HandleError
// Handles DRM specific error
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUiHandlingImpl::HandleErrorL(
    RFile& aFile,
    ContentAccess::TIntent aIntent,
    TInt aError,
    DRM::MDrmHandleErrorObserver* aObserver )
    {
    DRM::CDrmUiHandlingData* data( DRM::CDrmUiHandlingData::NewLC() );

    data->iOperationId = 0; // operation id zero for a syncronous operation
    data->iFile = &aFile;
    data->iIntent = aIntent;
    data->iError = aError;
    data->iErrorObserver = aObserver;

    PerformHandleErrorFileL( *data );

    CleanupStack::PopAndDestroy( data );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::HandleErrorAsyncL
// Handles DRM specific error
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiHandlingImpl::HandleErrorAsyncL(
    ContentAccess::CData& aFile,
    ContentAccess::TIntent aIntent,
    TInt aError,
    DRM::MDrmHandleErrorObserver& aObserver )
    {
    TRequestStatus* status( &iStatus );

    // create data object:
    DRM::CDrmUiHandlingData* data( DRM::CDrmUiHandlingData::NewLC() );

    data->iObserver = &aObserver;
    data->iOperationId = reinterpret_cast<TInt> ( data );
    data->iOperation = KCommandHandleErrorData;
    data->iFileData = &aFile;
    data->iIntent = aIntent;
    data->iError = aError;
    data->iErrorObserver = &aObserver;

    // Add it to the queue:
    AppendToQueue( data );

    // Pop it from the cleanup stack
    CleanupStack::Pop( data );

    Activate( status );

    return data->iOperationId;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::HandleError
// Handles DRM specific error
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUiHandlingImpl::HandleErrorL(
    ContentAccess::CData& aFile,
    ContentAccess::TIntent aIntent,
    TInt aError,
    DRM::MDrmHandleErrorObserver* aObserver )
    {
    DRM::CDrmUiHandlingData* data( DRM::CDrmUiHandlingData::NewLC() );

    data->iOperationId = 0; // operation id zero for a syncronous operation
    data->iFileData = &aFile;
    data->iIntent = aIntent;
    data->iError = aError;
    data->iErrorObserver = aObserver;

    PerformHandleErrorDataL( *data );

    CleanupStack::PopAndDestroy( data );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CancelOperation
// Cancels the operation
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiHandlingImpl::CancelOperation( TInt aOperationId )
    {
    // Critical area start:
    iSemaphore.Wait();

    TBool found( EFalse );

    DRM::CDrmUiHandlingData* previousData( NULL );
    DRM::CDrmUiHandlingData* currentData( iFirst );

    while ( currentData )
        {
        if ( currentData->iOperationId == aOperationId )
            {
            found = ETrue;

            //Update the previous link
            if ( previousData )
                {
                previousData->iNext = currentData->iNext;
                }
            else
                {
                iFirst = currentData->iNext;
                }

            //If the current is the only one, set iLast NULL
            if ( !iFirst )
                {
                iLast = NULL;
                }
            //If only one left,set it also to be the iLast
            else if ( iFirst && !currentData->iNext )
                {
                iLast = iFirst;
                }

            //Delete the cancelled operation
            currentData->iObserver->OperationCompleted(
                currentData->iOperationId, KErrCancel );
            delete currentData;
            currentData = NULL;
            }
        else
            {
            previousData = currentData;
            currentData = currentData->iNext;
            }
        }

    if ( !found )
        {
        // Critical Area End
        iSemaphore.Signal();
        return KErrNotFound;
        }

    // Critical Area End
    iSemaphore.Signal();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::DoCancel()
// From CActive Cancels async request.
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::RunL()
// From CActive Called when async request completes.
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::RunL()
    {
    DRM::CDrmUiHandlingData* data( PopFront() );
    TRequestStatus* status( &iStatus );

    if ( !data )
        {
        return;
        }

    CleanupStack::PushL( data );

    // Take this into the "current" variable in case an error occurs
    iObserver = data->iObserver;
    iOperationId = data->iOperationId;

    switch ( data->iOperation )
        {
        case KCommandHandleErrorFile:

            // Perform the operation
            PerformHandleErrorFileL( *data );

            break;

        case KCommandHandleErrorData:

            // Perform the operation
            PerformHandleErrorDataL( *data );

            break;

        case KCommandCheckRightsAmountFile:

            // Perform the operation
            PerformCheckRightsAmountFileL( *data );

            break;

        case KCommandCheckRightsAmountData:

            // Perform the operation
            PerformCheckRightsAmountDataL( *data );

            break;

        case KCommandShowDetailsViewFile:

            // Perform the operation
            PerformShowDetailsViewFileL( *data );

            break;

        case KCommandShowDetailsViewData:

            // Perform the operation
            PerformShowDetailsViewDataL( *data );

            break;

        case KCommandHandleUrlFile:

            // Perform the operation
            PerformHandleUrlFileL( *data );

            break;

        case KCommandHandleUrlData:

            // Perform the operation
            PerformHandleUrlDataL( *data );

            break;

        case KCommandAvailableUrlsFile:

            // Perform the operation
            PerformAvailableUrlsFileL( *data );

            break;

        case KCommandAvailableUrlsData:

            // Perform the operation
            PerformAvailableUrlsDataL( *data );

            break;

        default:

            break;
        }

    // Complete the client request
    iObserver->OperationCompleted( iOperationId, KErrNone );

    // destroy the object:
    CleanupStack::PopAndDestroy( data );

    // Get ready for another round:
    SetActive();

    // complete internal request:
    User::RequestComplete( status, KErrNone );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::RunError()
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmUiHandlingImpl::RunError( TInt aError )
    {
    iObserver->OperationCompleted( iOperationId, aError );
    iObserver = NULL;
    iOperationId = NULL;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::Activate
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::Activate( TRequestStatus*& aStatus )
    {
    if ( !IsAdded() )
        {
        CActiveScheduler::Add( this );
        }

    if ( !IsActive() )
        {
        SetActive();

        // complete internal request:
        User::RequestComplete( aStatus, KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::PerformCheckRightsAmountFileL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::PerformCheckRightsAmountFileL(
    DRM::CDrmUiHandlingData& aData )
    {
    TPtrC agent( KNullDesC );
    IsProtectedL( *aData.iFile, agent );

    ContentAccess::CData* content = ContentAccess::CData::NewLC(
        *aData.iFile, ContentAccess::KDefaultContentObject,
        ContentAccess::EPeek );

    if ( !agent.Compare( DRM::KDrmOmaAgentName ) )
        {
        CheckOmaRightsAmountL( *content, aData.iIntent,
            aData.iCheckRightsObserver, aData.iOperationId );
        }

    else if ( iOmaBasedAgentName && ( !agent.Compare( *iOmaBasedAgentName ) ) )
        {
        CheckOmaBasedRightsAmountL( *content, aData.iIntent,
            aData.iCheckRightsObserver, aData.iOperationId );
        }

    else if ( !agent.Compare( DRM::KDrmWMAgentName ) && iWmClient )
        {
        iWmClient->CheckWmRightsAmountL( *content, aData.iIntent,
            aData.iCheckRightsObserver, aData.iOperationId );
        }

    CleanupStack::PopAndDestroy( content );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::PerformCheckRightsAmountDataL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::PerformCheckRightsAmountDataL(
    DRM::CDrmUiHandlingData& aData )
    {
    DRM::TDrmAgentUid agentUid( DRM::EDrmOmaAgent );
    IsProtectedL( aData.iFileData, agentUid );

    switch ( agentUid )
        {
        case DRM::EDrmOmaAgent:

            CheckOmaRightsAmountL( *aData.iFileData, aData.iIntent,
                aData.iCheckRightsObserver, aData.iOperationId );

            break;

        case DRM::EDrmWmAgent:

            if ( iWmClient )
                {
                iWmClient->CheckWmRightsAmountL( *aData.iFileData,
                    aData.iIntent, aData.iCheckRightsObserver,
                    aData.iOperationId );
                }

            break;

        default:

            break;
        }
    if ( agentUid == iOmaBasedUid )
        {
        CheckOmaBasedRightsAmountL( *aData.iFileData, aData.iIntent,
            aData.iCheckRightsObserver, aData.iOperationId );
        }

    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::PerformShowDetailsViewFileL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::PerformShowDetailsViewFileL(
    DRM::CDrmUiHandlingData& aData )
    {
    HBufC* fullPath( NULL );
    TPtr pathPtr( NULL, 0 );
    TPtrC agent( KNullDesC );
    IsProtectedL( *aData.iFile, agent );

    ContentAccess::CData* content = ContentAccess::CData::NewLC(
        *aData.iFile, ContentAccess::KDefaultContentObject,
        ContentAccess::EPeek );

    HBufC8* contentId( NULL );

    fullPath = HBufC::NewLC( KUrlMaxLen );
    pathPtr.Set( fullPath->Des() );
    User::LeaveIfError( aData.iFile->FullName( pathPtr ) );

    if ( !agent.Compare( DRM::KDrmOmaAgentName ) )
        {
        GetContentIdLC( *content, contentId );
        ShowDetailsViewL( contentId, fullPath );
        CleanupStack::PopAndDestroy( contentId );
        }

    else if ( !agent.Compare( DRM::KDrmWMAgentName ) && iWmClient )
        {
        // Show details view for WMDRM file needs full path name
        ShowDetailsViewL( fullPath );
        }

    CleanupStack::PopAndDestroy( fullPath );
    CleanupStack::PopAndDestroy( content ); // content
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::PerformShowDetailsViewDataL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::PerformShowDetailsViewDataL(
    DRM::CDrmUiHandlingData& aData )
    {
    DRM::TDrmAgentUid agentUid( DRM::EDrmOmaAgent );
    IsProtectedL( aData.iFileData, agentUid );
    HBufC* fullPath( NULL );
    HBufC8* contentId( NULL );

    // For OMA and WM DRM, the full path is needed, not just the Content Id
    GetContentLocationLC( *aData.iFileData, fullPath );

    switch ( agentUid )
        {
        case DRM::EDrmOmaAgent:
            GetContentIdLC( *aData.iFileData, contentId );
            ShowDetailsViewL( contentId, fullPath );
            CleanupStack::PopAndDestroy( contentId );
            break;

        case DRM::EDrmWmAgent:

            if ( iWmClient )
                {
                // For WM DRM, the full path is needed, not just the Content Id
                ShowDetailsViewL( fullPath );
                }

            break;

        default:

            break;
        }

    if ( agentUid == iOmaBasedUid )
        {
        User::Leave( KErrNotSupported );
        }

    CleanupStack::PopAndDestroy( fullPath );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::PerformHandleUrlFileL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::PerformHandleUrlFileL(
    DRM::CDrmUiHandlingData& aData )
    {
    TPtrC agent( KNullDesC );
    IsProtectedL( *aData.iFile, agent );

    ContentAccess::CData* content = ContentAccess::CData::NewLC(
        *aData.iFile, ContentAccess::KDefaultContentObject,
        ContentAccess::EPeek );

    if ( !agent.Compare( DRM::KDrmOmaAgentName ) )
        {
        DoHandleOmaUrlL( *content, aData.iTypeParam );
        }

    else if ( !agent.Compare( DRM::KDrmWMAgentName ) && iWmClient )
        {
        //*** WMDRM does not support URL:s like that thus leaving with not supported
        User::Leave( KErrNotSupported );
        }

    CleanupStack::PopAndDestroy( content );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::PerformHandleUrlDataL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::PerformHandleUrlDataL(
    DRM::CDrmUiHandlingData& aData )
    {
    DRM::TDrmAgentUid agentUid( DRM::EDrmOmaAgent );
    IsProtectedL( aData.iFileData, agentUid );

    // *** WMDRM
    switch ( agentUid )
        {
        case DRM::EDrmOmaAgent:

            DoHandleOmaUrlL( *aData.iFileData, aData.iTypeParam );

            break;

        case DRM::EDrmWmAgent:

            if ( iWmClient )
                {
                User::Leave( KErrNotSupported );
                }

            break;

        default:

            break;

        }

    if ( agentUid == iOmaBasedUid )
        {
        User::Leave( KErrNotSupported );
        }
    // *** WMDRM
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::PerformAvailableUrlsFileL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::PerformAvailableUrlsFileL(
    DRM::CDrmUiHandlingData& aData )
    {
    TPtrC agent( KNullDesC );
    IsProtectedL( *aData.iFile, agent );

    ContentAccess::CData* content = ContentAccess::CData::NewLC(
        *aData.iFile, ContentAccess::KDefaultContentObject,
        ContentAccess::EPeek );

    if ( !agent.Compare( DRM::KDrmOmaAgentName ) )
        {
        GetAvailableOmaUrlsL( *content, *aData.iType );
        }

    else if ( !agent.Compare( DRM::KDrmWMAgentName ) && iWmClient )
        {
        //*** WMDRM does not support URL:s like that thus leaving with not supported
        User::Leave( KErrNotSupported );
        }
    CleanupStack::PopAndDestroy( content );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::PerformAvailableUrlsDataL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::PerformAvailableUrlsDataL(
    DRM::CDrmUiHandlingData& aData )
    {
    DRM::TDrmAgentUid agentUid( DRM::EDrmOmaAgent );
    IsProtectedL( aData.iFileData, agentUid );

    // *** WMDRM
    switch ( agentUid )
        {
        case DRM::EDrmOmaAgent:

            GetAvailableOmaUrlsL( *aData.iFileData, *aData.iType );

            break;

        case DRM::EDrmWmAgent:

            if ( iWmClient )
                {
                User::Leave( KErrNotSupported );
                }

            break;

        default:

            break;

        }

    if ( agentUid == iOmaBasedUid )
        {
        User::Leave( KErrNotSupported );
        }
    // *** WMDRM
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::PerformHandleErrorFileL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::PerformHandleErrorFileL(
    DRM::CDrmUiHandlingData& aData )
    {
    TPtrC agent( KNullDesC );
    IsProtectedL( *aData.iFile, agent );
    IsProperErrorL( aData.iError );

    ContentAccess::CData* content = ContentAccess::CData::NewLC(
        *aData.iFile, ContentAccess::KDefaultContentObject,
        ContentAccess::EPeek );

    if ( ( !agent.Compare( DRM::KDrmOmaAgentName ) ) || ( iOmaBasedAgentName
        && !agent.Compare( *iOmaBasedAgentName ) ) )
        {
        // To embed Domain RO in superdistribution case, register the content
        TRAP_IGNORE( FindFullFileNameAndUpdateDCFRepositoryL( *aData.iFile ) );
        
        HandleOmaErrorL( *content, aData.iIntent, aData.iError,
            aData.iErrorObserver, aData.iOperationId );
        }

    else if ( !agent.Compare( DRM::KDrmWMAgentName ) && iWmClient )
        {
        // *** WMDRM
        iWmClient->HandleWmErrorL( *content, aData.iIntent, aData.iError,
            aData.iErrorObserver, aData.iOperationId );
        }

    CleanupStack::PopAndDestroy( content );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::PerformHandleErrorDataL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::PerformHandleErrorDataL(
    DRM::CDrmUiHandlingData& aData )
    {
    
    DRM::TDrmAgentUid agentUid( DRM::EDrmOmaAgent );
    IsProtectedL( aData.iFileData, agentUid );
    IsProperErrorL( aData.iError );

    // *** WMDRM
    switch ( agentUid )
        {
        case DRM::EDrmOmaAgent:
            
            // To embed Domain RO in superdistribution case, register the content
            TRAP_IGNORE( FindFullFileNameAndUpdateDCFRepositoryL( 
                *aData.iFileData ) );
            
            HandleOmaErrorL( *aData.iFileData, aData.iIntent, aData.iError,
                aData.iErrorObserver, aData.iOperationId );

            break;

        case DRM::EDrmWmAgent:

            if ( iWmClient )
                {
                iWmClient->HandleWmErrorL( *aData.iFileData, aData.iIntent,
                    aData.iError, aData.iErrorObserver, aData.iOperationId );
                }

            break;

        default:

            break;

        }
    if ( agentUid == iOmaBasedUid )
        {
        HandleOmaErrorL( *aData.iFileData, aData.iIntent, aData.iError,
            aData.iErrorObserver, aData.iOperationId );
        }
    // *** WMDRM
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::IsProtectedL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::IsProtectedL( RFile& aFile, TPtrC& aAgent )
    {
    DRM::TDrmProtectionStatus drmProtected( DRM::EUUnknown );
    TInt ret( 0 );
    ret = iDrmUtility->GetDrmInfoL( aFile, aAgent, drmProtected );
    if ( !ret || !drmProtected ) // do nothing if non-DRM file
        {
        User::Leave( KErrArgument );
        }
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::IsProtectedL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::IsProtectedL(
    ContentAccess::CData* aContent,
    DRM::TDrmAgentUid& aAgentUid )
    {
    TInt drmProtected( 0 );
    User::LeaveIfError( aContent->GetAttribute( EIsProtected, drmProtected ) );
    if ( !drmProtected ) // do nothing if non-DRM file
        {
        User::Leave( KErrArgument );
        }

    TInt temp( 0 );

    HBufC* nameString = HBufC::NewLC( KNameBuf );

    TPtr namePtr( nameString->Des() );

    User::LeaveIfError( aContent->GetStringAttribute( DRM::EDrmAgentName,
        namePtr ) );

    if ( iOmaBasedAgentName && ( !namePtr.CompareF( *iOmaBasedAgentName ) ) )
        {
        aAgentUid = static_cast<DRM::TDrmAgentUid> ( iOmaBasedUid );
        }
    else
        {
        User::LeaveIfError( aContent->GetAttribute( DRM::EDrmAgentUid, temp ) );
        aAgentUid = static_cast<DRM::TDrmAgentUid> ( temp );
        }
    CleanupStack::PopAndDestroy( nameString );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::IsProperErrorL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::IsProperErrorL( TInt aError )
    {
    if ( aError != ENoPermission && aError != ENoRights && aError
        != EPendingRights )
        {
        User::Leave( aError );
        }
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CheckOmaRightsAmountL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::CheckOmaRightsAmountL(
    ContentAccess::CData& aContent,
    ContentAccess::TIntent aIntent,
    DRM::MDrmUiCheckRightsObserver* aObserver,
    TInt aOperationId )
    {
    HBufC8* contentId( NULL );
    HBufC8* silentUrl( NULL );
    HBufC* riUrl( NULL );
    HBufC* domainRiUrl( NULL );
    DRM::TDrmUiUrlType urlType( DRM::EUHRightsIssuerUrl );
            
    // To embed Domain RO in superdistribution case, register the content
    TRAP_IGNORE( FindFullFileNameAndUpdateDCFRepositoryL( aContent ) );
    
    User::LeaveIfError( GetContentIdLC( aContent, contentId ) );
    GetOmaSilentRightsUrlLC( aContent, silentUrl );
    GetOmaRightsIssuerLC( aContent, riUrl );
    GetOmaDomainRightsIssuerLC( aContent, domainRiUrl );
    SelectOmaRightsUrlL( aContent, domainRiUrl, urlType );

    DoCheckOmaRightsAmountL( aContent, contentId, aIntent, urlType
        == DRM::EUHRightsIssuerUrl ? riUrl : domainRiUrl, urlType, silentUrl,
        aObserver, aOperationId );

    CleanupStack::PopAndDestroy( domainRiUrl );
    CleanupStack::PopAndDestroy( riUrl );
    CleanupStack::PopAndDestroy( silentUrl );
    CleanupStack::PopAndDestroy( contentId );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::DoCheckOmaRightsAmountL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::DoCheckOmaRightsAmountL(
    ContentAccess::CData& aContent,
    HBufC8*& aContentUri,
    ContentAccess::TIntent aIntent,
    const HBufC* aRightsUrl,
    DRM::TDrmUiUrlType aUrlType,
    const HBufC8* aSilentUrl,
    DRM::MDrmUiCheckRightsObserver* aObserver,
    TInt aOperationId )
    {
    TInt omaError( KErrNone );
    CDRMPermission* permission( NULL );
    CDRMConstraint* constraint( NULL );
    CDRMConstraint* toplevel( NULL );
    DRM::TDrmRightsInfo status( DRM::EURightsInfoMissing );
    DRM::TCheckRightsStatus rightsStatus( DRM::EUHCheckRightsNoRights );
    TTime now( Time::NullTTime() ); // current time
    TTime endTime( Time::NullTTime() ); // end time of time-based rights
    TInt mask( 0 ); // tells if object has count or time based rights
    TUint32 reason( 0 );
    TBool unlimited( EFalse );
    TUint32 countsLeft( 0 ); // number of counts left
    TTimeIntervalSeconds secondsLeft( 0 ); // seconds to end of time based rights
    TTimeIntervalSeconds accumulatedSecondsLeft( 0 ); // seconds to end of
    // accumulated time rights

    // Check rights
    omaError = iOmaClient.CheckRights( aIntent, *aContentUri, reason );

    // valid rights
    if ( !omaError )
        {
        permission = iOmaClient.GetActiveRightsL( aIntent, *aContentUri,
            reason );

        if( !permission )
            {
            User::Leave( KErrCANoPermission); //coverity check
            }

        CleanupStack::PushL( permission );

        toplevel = permission->TopLevelConstraint();
        constraint = permission->ConstraintForIntent( aIntent );

        // if top level constraint exists, merge it to constraint for intent
        if ( toplevel )
            {
            constraint->Merge( *toplevel );
            }

        // find out type of constraint
        mask = GetOmaConstraintInfo( constraint, countsLeft, endTime );

        // get the current time
        now.HomeTime();

        // calculate days left in case of time
        // and accumulated time constraints
        if ( mask & KDRMOmaConstraintTime )
            {
            endTime.SecondsFrom( now, secondsLeft );
            }
        if ( mask & KDRMOmaConstraintAccumulated )
            {
            endTime.SecondsFrom( now, accumulatedSecondsLeft );
            }
        // if no count or time based constraint, then unlimited
        if ( !mask )
            {
            unlimited = ETrue;
            }

        CallRightsLeftL( aObserver, aOperationId, unlimited, secondsLeft,
            countsLeft, accumulatedSecondsLeft, aContentUri, aRightsUrl,
            aSilentUrl, aContent, aIntent );

        CleanupStack::PopAndDestroy( permission );
        }

    // Not valid rights
    else
        {
        constraint = CDRMConstraint::NewLC();

        // No rights at all
        if ( omaError == KErrCANoRights )
            {
            rightsStatus = DRM::EUHCheckRightsNoRights;
            }
        else
            {
            // rights expired or future for given content URI
            status = iDrmUtilityCommon->GetOmaRightsStatusL( aContentUri,
                aIntent, constraint );
            if ( status == DRM::EURightsInfoFuture )
                {
                rightsStatus = DRM::EUHCheckRightsFutureRights;
                }
            else if ( status == DRM::EURightsInfoExpired )
                {
                rightsStatus = DRM::EUHCheckRightsExpiredRights;
                }
            }

        CallRightsNotValidL( aObserver, aOperationId, rightsStatus, reason,
            aContent, aContentUri, constraint, aRightsUrl, aUrlType,
            aSilentUrl, aIntent );

        CleanupStack::PopAndDestroy( constraint );
        }
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::HandleOmaErrorL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::HandleOmaErrorL(
    ContentAccess::CData& aContent,
    ContentAccess::TIntent aIntent,
    TInt aError,
    DRM::MDrmHandleErrorObserver* aObserver,
    TInt aOperationId )
    {
    TInt omaError( KErrNone );
    HBufC8* contentId( NULL );
    HBufC* riUrl( NULL );
    HBufC* domainRiUrl( NULL );
    HBufC* previewUrl( NULL );
    HBufC8* silentUrl( NULL );
    CDRMConstraint* constraint( NULL );
    DRM::TDrmRightsInfo rightsStatus( DRM::EURightsInfoValid );
    DRM::TDrmUiUrlType urlType( DRM::EUHRightsIssuerUrl );
    TInt previewType( ContentAccess::ENoPreview );
    TUint32 reason( 0 );
    TBool handled( EFalse );
    
    User::LeaveIfError( GetContentIdLC( aContent, contentId ) );

    // Check that this really is an error situation
    omaError = iOmaClient.CheckRights( aIntent, *contentId, reason );

    // Show that only reason for error was rejected metering.
    TBool onlyMeteringRejection( reason == DRM::EURejectionMetering );
    if ( onlyMeteringRejection )
        {
        // Qt dialog not implemented yet
        iDrmUtilityUi->DisplayNoteL( EConfUnableToOpen );

        CleanupStack::PopAndDestroy( contentId );
        return;
        }

    if ( omaError )
        {

        // Get rights status
        constraint = CDRMConstraint::NewLC();
        rightsStatus = iDrmUtilityCommon->GetOmaRightsStatusL( contentId,
            aIntent, constraint );

        // Try to get all needed urls
        GetOmaPreviewInfoLC( aContent, previewUrl, previewType );
        GetOmaSilentRightsUrlLC( aContent, silentUrl );
        GetOmaRightsIssuerLC( aContent, riUrl );
        GetOmaDomainRightsIssuerLC( aContent, domainRiUrl );
        SelectOmaRightsUrlL( aContent, domainRiUrl, urlType );

        // If content has EmbeddedPreviewUrl, PreviewRightsUrl or
        // SilentRightsUrl, handle it
        if ( previewType == ContentAccess::EInstantPreview )
            {
            handled = ETrue;
            CallEmbeddedPreviewAvailableL( aObserver, aOperationId, aContent,
                previewUrl, rightsStatus, reason, urlType
                    == DRM::EUHRightsIssuerUrl ? riUrl : domainRiUrl,
                contentId, aIntent );
            }

        else if ( previewType == ContentAccess::EPreviewRights )
            {
            handled = ETrue;
            CallPreviewRightsUrlAvailableL( aObserver, aOperationId,
                aContent, previewUrl, rightsStatus, reason, urlType
                    == DRM::EUHRightsIssuerUrl ? riUrl : domainRiUrl,
                contentId, aIntent );
            }

        else if ( silentUrl )
            {
            handled = ETrue;
            CallSilentRightsUrlAvailableL( aObserver, aOperationId,
                silentUrl, rightsStatus, reason, urlType
                    == DRM::EUHRightsIssuerUrl ? riUrl : domainRiUrl,
                contentId, aIntent );
            }

        // Check if rights were retrieved in earlier url handling
        if ( handled )
            {
            omaError = iOmaClient.CheckRights( aIntent, *contentId, reason );
            }

        if ( omaError )
            {
            // No rights available
            // Perform this even if no rights issuer url found
            CallRightsUrlAvailableL( aObserver, aOperationId, rightsStatus,
                reason, constraint, aContent, contentId, urlType
                    == DRM::EUHRightsIssuerUrl ? riUrl : domainRiUrl,
                urlType, aIntent, aError );
            }

        CleanupStack::PopAndDestroy( domainRiUrl );
        CleanupStack::PopAndDestroy( riUrl );
        CleanupStack::PopAndDestroy( silentUrl );
        CleanupStack::PopAndDestroy( previewUrl );
        CleanupStack::PopAndDestroy( constraint );
        }
    else
        {
        User::Leave( KErrArgument );
        }

    CleanupStack::PopAndDestroy( contentId );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::ShowOmaDetailsViewL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::ShowDetailsViewL(
    const HBufC8* aContentId,
    const HBufC* aFullPath )
    {
    __ASSERT_DEBUG( aContentId, User::Panic( KDRMUtilityDebugPanicMessage,
            KDRMUtilityDebugPanicCode ) );

    __ASSERT_DEBUG( aFullPath, User::Panic( KDRMUtilityDebugPanicMessage,
            KDRMUtilityDebugPanicCode ) );

    // For OMA DRM
    HBufC* launchParam( NULL );

    // create parameter string
    CreateLaunchParamL( aContentId, launchParam, aFullPath );
    CleanupStack::PushL( launchParam );

    // show details
    LaunchRightsManagerUiL( *launchParam );

    CleanupStack::PopAndDestroy( launchParam );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CheckOmaBasedRightsAmountL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::CheckOmaBasedRightsAmountL(
    ContentAccess::CData& aContent,
    ContentAccess::TIntent aIntent,
    DRM::MDrmUiCheckRightsObserver* aObserver,
    TInt aOperationId )
    {
    HBufC8* contentId( NULL );
    HBufC* riUrl( NULL );

    DRM::TDrmUiUrlType urlType( DRM::EUHRightsIssuerUrl );

    User::LeaveIfError( GetContentIdLC( aContent, contentId ) );

    DoCheckOmaBasedRightsAmountL( aContent, contentId, aIntent, riUrl,
        urlType, aObserver, aOperationId );

    CleanupStack::PopAndDestroy( contentId );

    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::DoCheckOmaBasedRightsAmountL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::DoCheckOmaBasedRightsAmountL(
    ContentAccess::CData& aContent,
    const HBufC8* aContentUri,
    ContentAccess::TIntent aIntent,
    const HBufC* aRightsUrl,
    DRM::TDrmUiUrlType aUrlType,
    DRM::MDrmUiCheckRightsObserver* aObserver,
    TInt aOperationId )
    {
    TInt canPlay( KErrNone );
    CDRMConstraint* constraint( NULL );
    DRM::TCheckRightsStatus rightsStatus( DRM::EUHCheckRightsNoRights );

    TTime now( Time::NullTTime() ); // current time
    TTime endTime( Time::NullTTime() ); // end time of time-based rights

    TUint32 reason( EConstraintEndTime );
    TBool unlimited( EFalse );
    TUint32 countsLeft( 0 ); // number of counts left
    TTimeIntervalSeconds secondsLeft( 0 ); // seconds to end of time based rights
    TTimeIntervalSeconds accumulatedSecondsLeft( 0 ); // seconds to end of
    // accumulated time rights

    // Check rights

    User::LeaveIfError( aContent.GetAttribute( ECanPlay, canPlay ) );

    GetOmaBasedEndTimeL( aContent, endTime );

    // valid rights
    if ( canPlay ) // check the CAF parameters
        {
        // get the current time
        now.HomeTimeSecure();

        endTime.SecondsFrom( now, secondsLeft );

        CallRightsLeftL( aObserver, aOperationId, unlimited, secondsLeft,
            countsLeft, accumulatedSecondsLeft, aContentUri, aRightsUrl,
            NULL, aContent, aIntent );
        }

    // Not valid rights
    else
        {
        constraint = CDRMConstraint::NewLC();
        rightsStatus = DRM::EUHCheckRightsExpiredRights;
        constraint->iActiveConstraints |= EConstraintEndTime;
        constraint->iEndTime = endTime;

        CallRightsNotValidL( aObserver, aOperationId, rightsStatus, reason,
            aContent, aContentUri, constraint, aRightsUrl, aUrlType, NULL,
            aIntent );

        CleanupStack::PopAndDestroy( constraint );
        }
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::GetOmaBasedEndTimeL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::GetOmaBasedEndTimeL(
    ContentAccess::CData& aData,
    TTime& aEndTime )
    {
    TInt err = KErrNone;
    RFs fileSession;
    RFile file;
    TBuf8<KIntegerMaxLen> lenTxt;
    TInt readStart = 0;
    TInt headerLen = 0;
    TInt tempVal = 0;

    __UHEAP_MARK;

    User::LeaveIfError( fileSession.Connect() );

    CleanupClosePushL( fileSession );
    CleanupClosePushL( file );

    HBufC* filename = HBufC::NewLC( KNameBuf );

    TPtr resOmaBased( const_cast<TUint16*> ( filename->Ptr() ), 0,
        KresOmaBasedBuf );
    err = aData.GetStringAttribute( DRM::EDrmFullName, resOmaBased );

    if ( file.Open( fileSession, resOmaBased, EFileShareReadersOrWriters
        | EFileRead ) == KErrNone )
        {
        /* Read content type and uri lengths */
        err = file.Read( 1, lenTxt, 2 );
        if ( err == KErrNone && lenTxt.Length() == 2 )
            {
            readStart = lenTxt[0] + lenTxt[1];
            }
        if ( readStart > 0 )
            {
            /* Add the length of version and length informations */
            const TInt KReadSkip( 3 );
            readStart += KReadSkip;
            /* Read header length information (exact length not known
             since it is Uintvar, so read plentily) */
            err = file.Read( readStart, lenTxt, lenTxt.MaxLength() );
            if ( err == KErrNone && lenTxt.Length() > 0 )
                {
                TBool cont = ETrue;
                /* Start reading the header length */
                TInt i = 0;
                while ( i < lenTxt.Length() && cont )
                    {
                    tempVal = lenTxt[i];
                    i++;
                    if ( !( tempVal & 0x80 ) )
                        { /* Highest bit not set, it's the last value */
                        cont = EFalse;
                        }
                    else
                        { /* Clear the highest bit */
                        tempVal &= ~( 0x80 );
                        }
                    headerLen = ( ( headerLen << 0x07 ) | tempVal );
                    }
                if ( cont != EFalse )
                    { /* Reading the length failed */
                    headerLen = 0;
                    }
                else
                    {
                    /* Continue to read past the content length */
                    cont = ETrue;
                    while ( i < lenTxt.Length() && cont )
                        {
                        tempVal = lenTxt[i];
                        i++;
                        if ( !( tempVal & 0x80 ) )
                            { /* Highest bit not set, it's the last value */
                            cont = EFalse;
                            }
                        }
                    if ( cont != EFalse )
                        { /* Reading the length failed */
                        headerLen = 0;
                        }
                    else
                        {
                        /* Increase read start index past length
                         informations */
                        readStart += i;
                        }
                    }
                }
            }
        if ( headerLen > 0 )
            {
            HBufC8 *header = HBufC8::NewLC( headerLen );
            /* Read header */
            TPtr8 headerPtr( header->Des() );
            err = file.Read( readStart, headerPtr, headerLen );
            if ( err == KErrNone && headerPtr.Length() == headerLen )
                {
                // seperate function call from here
                OmaBasedHeaderParserL( tempVal, headerPtr, aEndTime );
                }
            /* Pop and destroy header */
            CleanupStack::PopAndDestroy( header );
            }
        }
    CleanupStack::PopAndDestroy( filename );
    CleanupStack::PopAndDestroy( &file );
    CleanupStack::PopAndDestroy( &fileSession );
    __UHEAP_MARKEND;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::OmaBasedHeaderParserL
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmUiHandlingImpl::OmaBasedHeaderParserL(
    TInt aOffset,
    TPtr8 aHeaderPtr,
    TTime& aEndTime )
    {
    TInt err = KErrNone;
    TTime *time = NULL;
    _LIT8( KEndTimeStr, "Time-Not-After/" );
    _LIT( KZero, "0" );

    /* Get the time */
    TInt pos = aHeaderPtr.FindF( KEndTimeStr );
    if ( pos != KErrNotFound )
        {
        TPtrC8 endTimeStr( aHeaderPtr.Mid( pos + KEndTimeStr().Length() ) );
        pos = endTimeStr.Locate( ',' );
        if ( pos != KErrNotFound )
            {
            endTimeStr.Set( endTimeStr.Left( pos ) );
            /* Check if it contains timezone information */
            pos = endTimeStr.Locate( 'Z' );
            if ( pos != KErrNotFound )
                { /* Cut the time zone information "Zxxx" */
                endTimeStr.Set( endTimeStr.Left( pos ) );
                }
            pos = endTimeStr.Locate( '+' );
            if ( pos != KErrNotFound )
                { /* Cut the time zone information "+xx" */
                endTimeStr.Set( endTimeStr.Left( pos ) );
                }
            pos = endTimeStr.Locate( '-' );
            if ( pos != KErrNotFound )
                { /* Cut the time zone information "-xx" */
                endTimeStr.Set( endTimeStr.Left( pos ) );
                }
            /* Allocate new buffer with length enough
             for adding needed ':' character between date and time */
            const TInt KPlaceOfColon( 8 );
            HBufC16* dateTimeBuf = HBufC16::NewLC( endTimeStr.Length() + 1 );
            /* Copy provides sufficient 8bit -> 16bit conversion here */
            dateTimeBuf->Des().Copy( endTimeStr );
            dateTimeBuf->Des().Insert( KPlaceOfColon, _L16 ( ":" ) );

            /* Decrease 1 from month and day, cause
             TTime::FormatL expects them to start from 0 */
            TPtrC16 tempPtr( dateTimeBuf->Des().Mid( 4, 2 ) );
            TLex16 lex( dateTimeBuf->Des().Mid( 4, 2 ) );
            TBuf<2> replace;
            err = lex.Val( aOffset );
            if ( err == KErrNone )
                {
                replace.Num( ( aOffset - 1 ) );
                if ( replace.Length() == 1 )
                    { /* Insert the '0' needed by TTime */
                    replace.Insert( 0, KZero );
                    }
                dateTimeBuf->Des().Replace( 4, 2, replace );

                lex.Assign( dateTimeBuf->Des().Mid( 6, 2 ) );
                TBuf<2> replace;
                err = lex.Val( aOffset );
                if ( err == KErrNone )
                    {
                    replace.Num( ( aOffset - 1 ) );
                    if ( replace.Length() == 1 )
                        { /* Insert the '0' needed by TTime */
                        replace.Insert( 0, KZero );
                        }
                    dateTimeBuf->Des().Replace( 6, 2, replace );

                    time = new ( ELeave ) TTime( dateTimeBuf->Des() );

                    CleanupStack::PushL( time );
                    aEndTime = *time;

                    /* Pop and destroy time */
                    CleanupStack::PopAndDestroy( time );
                    }
                }
            /* Pop and destroy dateTimeBuf */
            CleanupStack::PopAndDestroy( dateTimeBuf );
            }
        }
    else
        {
        return KErrNotFound;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::ShowDetailsViewL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::ShowDetailsViewL( const HBufC* aFullPath )
    {

    __ASSERT_DEBUG( aFullPath, User::Panic( KDRMUtilityDebugPanicMessage,
            KDRMUtilityDebugPanicCode ) );
    // For WM DRM
    HBufC* launchParam( NULL );

    // create parameter string
    CreateLaunchParamL( aFullPath, launchParam );
    CleanupStack::PushL( launchParam );

    // show details
    LaunchRightsManagerUiL( *launchParam );

    CleanupStack::PopAndDestroy( launchParam );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::DoHandleOmaUrlL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::DoHandleOmaUrlL(
    ContentAccess::CData& aContent,
    DRM::TDrmUiUrlType aType )
    {
    HBufC* url( NULL );
    HBufC8* url8( NULL );
    TInt previewType( 0 );

    switch ( aType )
        {
        // handle RightsIssuerUrl
        case DRM::EUHRightsIssuerUrl:

            GetOmaRightsIssuerLC( aContent, url );
            if ( !url )
                {
                User::Leave( KErrNotFound );
                }

            LaunchBrowserL( url );

            CleanupStack::PopAndDestroy( url );

            break;

            // handle PreviewUrl
        case DRM::EUHPreviewRightsUrl:

            GetOmaPreviewInfoLC( aContent, url, previewType );
            if ( !url )
                {
                User::Leave( KErrNotFound );
                }

#ifdef RD_DRM_PREVIEW_RIGHT_FOR_AUDIO

            if ( previewType == ContentAccess::EPreviewRights )
                {
                url8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *url );
                CleanupStack::PushL( url8 );

                GetSilentRightsL( *url8, ETrue );

                CleanupStack::PopAndDestroy( url8 );
                }

#endif // RD_DRM_PREVIEW_RIGHT_FOR_AUDIO
            CleanupStack::PopAndDestroy( url );

            break;

            // handle SilentRightsUrl
        case DRM::EUHSilentRightsUrl:

            GetOmaSilentRightsUrlLC( aContent, url8 );
            if ( !url8 )
                {
                User::Leave( KErrNotFound );
                }

            GetSilentRightsL( *url8, ETrue );

            CleanupStack::PopAndDestroy( url8 );

            break;

            // handle InfoUrl
        case DRM::EUHInfoUrl:

            GetOmaInfoUrlLC( aContent, url );
            if ( !url )
                {
                User::Leave( KErrNotFound );
                }

            LaunchBrowserL( url );

            CleanupStack::PopAndDestroy( url );

            break;

            // handle DomainRightsIssuerUrl
        case DRM::EUHDomainRiUrl:

            GetOmaDomainRightsIssuerLC( aContent, url );
            if ( !url )
                {
                User::Leave( KErrNotFound );
                }

            LaunchBrowserL( url );

            CleanupStack::PopAndDestroy( url );

            break;

        default:

            User::Leave( KErrNotSupported );

            break;

        }
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::GetAvailableOmaUrlsL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::GetAvailableOmaUrlsL(
    ContentAccess::CData& aContent,
    DRM::TDrmUiUrlType& aType )
    {
    HBufC* riUrl( NULL );
    HBufC* previewUrl( NULL );
    HBufC8* silentUrl( NULL );
    HBufC* infoUrl( NULL );
    HBufC* domainRiUrl( NULL );

    TInt dummy( 0 );
    TInt temp( 0 );

    // Try to get all Urls
    GetOmaRightsIssuerLC( aContent, riUrl );
    if ( riUrl )
        {
        temp |= DRM::EUHRightsIssuerUrl;
        }
    CleanupStack::PopAndDestroy( riUrl );
    riUrl = NULL;

    GetOmaPreviewInfoLC( aContent, previewUrl, dummy );
    if ( previewUrl )
        {
        temp |= DRM::EUHPreviewRightsUrl;
        }
    CleanupStack::PopAndDestroy( previewUrl );
    previewUrl = NULL;

    GetOmaSilentRightsUrlLC( aContent, silentUrl );
    if ( silentUrl )
        {
        temp |= DRM::EUHSilentRightsUrl;
        }
    CleanupStack::PopAndDestroy( silentUrl );
    silentUrl = NULL;

    GetOmaInfoUrlLC( aContent, infoUrl );
    if ( infoUrl )
        {
        temp |= DRM::EUHInfoUrl;
        }
    CleanupStack::PopAndDestroy( infoUrl );
    infoUrl = NULL;

    GetOmaDomainRightsIssuerLC( aContent, domainRiUrl );
    if ( domainRiUrl )
        {
        temp |= DRM::EUHDomainRiUrl;
        }
    CleanupStack::PopAndDestroy( domainRiUrl );
    domainRiUrl = NULL;

    // Return a bit mask of available urls
    aType = static_cast<TDrmUiUrlType> ( temp );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::GetContentIdLC
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmUiHandlingImpl::GetContentIdLC(
    const ContentAccess::CData& aContent,
    HBufC8*& aId )
    {
    __ASSERT_DEBUG( !aId, User::Panic( KDRMUtilityDebugPanicMessage,
            KDRMUtilityDebugPanicCode ) );

    TInt error( KErrNone );
    TInt currentLength( KUrlMaxLen );
    HBufC* contentId( NULL );
    TPtr idPtr( NULL, 0 );

    contentId = HBufC::NewLC( currentLength );
    idPtr.Set( contentId->Des() );

    error = aContent.GetStringAttribute( ContentAccess::EContentID, idPtr );

    while ( error == KErrOverflow )
        {
        currentLength += currentLength;
        CleanupStack::PopAndDestroy( contentId );
        contentId = HBufC::NewLC( currentLength );
        idPtr.Set( contentId->Des() );
        error
            = aContent.GetStringAttribute( ContentAccess::EContentID, idPtr );
        }

    if ( !error && idPtr.Length() )
        {
        aId = CnvUtfConverter::ConvertFromUnicodeToUtf8L( idPtr );
        }

    CleanupStack::PopAndDestroy( contentId );
    CleanupStack::PushL( aId );
    return error;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::GetOmaRightsIssuerLC
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmUiHandlingImpl::GetOmaRightsIssuerLC(
    const ContentAccess::CData& aContent,
    HBufC*& aIssuer )
    {
    __ASSERT_DEBUG( !aIssuer, User::Panic( KDRMUtilityDebugPanicMessage,
            KDRMUtilityDebugPanicCode ) );

    TInt error( KErrNone );
    TInt currentLength( KUrlMaxLen );
    HBufC* decodedIssuer( NULL );
    HBufC* rightsIssuer( NULL );
    TPtr riPtr( NULL, 0 );

    rightsIssuer = HBufC::NewLC( currentLength );
    riPtr.Set( rightsIssuer->Des() );

    // Get value of riUrl
    error = aContent.GetStringAttribute( ContentAccess::ERightsIssuerUrl,
        riPtr );

    while ( error == KErrOverflow )
        {
        currentLength += currentLength;
        CleanupStack::PopAndDestroy( rightsIssuer );
        rightsIssuer = HBufC::NewLC( currentLength );
        riPtr.Set( rightsIssuer->Des() );
        error = aContent.GetStringAttribute( ContentAccess::ERightsIssuerUrl,
            riPtr );
        }

    if ( !error && riPtr.Length() )
        {
        riPtr.TrimLeft();
        // check if rights issuer is encrypted
        if ( riPtr.MatchF( KEncryptedRightsIssuerMatchString )
            != KErrNotFound )
            {
            HBufC8* input( NULL );
            HBufC8* output( NULL );

            input = CnvUtfConverter::ConvertFromUnicodeToUtf8L( riPtr );

            CleanupStack::PushL( input );
            // encrypted, decrypt
            User::LeaveIfError( iOmaClient.DecodeRightsIssuerField( *input,
                output ) );
            CleanupStack::PopAndDestroy( input );

            CleanupStack::PushL( output );
            decodedIssuer = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
                *output );
            CleanupStack::PopAndDestroy( output );

            CleanupStack::PushL( decodedIssuer );
            riPtr.Set( decodedIssuer->Des() );
            aIssuer = riPtr.AllocL();
            CleanupStack::PopAndDestroy( decodedIssuer );
            }
        else
            {
            aIssuer = riPtr.AllocL();
            }
        }

    CleanupStack::PopAndDestroy( rightsIssuer );
    CleanupStack::PushL( aIssuer );
    return error;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::GetOmaDomainRightsIssuerLC
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmUiHandlingImpl::GetOmaDomainRightsIssuerLC(
    const ContentAccess::CData& aContent,
    HBufC*& aIssuer )
    {
    __ASSERT_DEBUG( !aIssuer, User::Panic( KDRMUtilityDebugPanicMessage,
            KDRMUtilityDebugPanicCode ) );

    TInt error( KErrNone );
    TInt currentLength( KUrlMaxLen );
    HBufC* rightsIssuer( NULL );
    TPtr riPtr( NULL, 0 );

    rightsIssuer = HBufC::NewLC( currentLength );
    riPtr.Set( rightsIssuer->Des() );

    // Get value of domainRIUrl
    error = aContent.GetStringAttribute(
        ContentAccess::EDomainRightsIssuerUrl, riPtr );

    while ( error == KErrOverflow )
        {
        currentLength += currentLength;
        CleanupStack::PopAndDestroy( rightsIssuer );
        rightsIssuer = HBufC::NewLC( currentLength );
        riPtr.Set( rightsIssuer->Des() );
        error = aContent.GetStringAttribute(
            ContentAccess::EDomainRightsIssuerUrl, riPtr );
        }

    if ( !error && riPtr.Length() )
        {
        aIssuer = riPtr.AllocL();
        }

    CleanupStack::PopAndDestroy( rightsIssuer );
    CleanupStack::PushL( aIssuer );
    return error;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::GetOmaPreviewInfoLC
// -----------------------------------------------------------------------------
TInt DRM::CDrmUiHandlingImpl::GetOmaPreviewInfoLC(
    const ContentAccess::CData& aContent,
    HBufC*& aPreviewUrl,
    TInt& aPreviewType )
    {
    __ASSERT_DEBUG( !aPreviewUrl, User::Panic( KDRMUtilityDebugPanicMessage,
            KDRMUtilityDebugPanicCode ) );

    TInt error( KErrNone );
    TInt currentLength( KUrlMaxLen );
    HBufC* preview( NULL );
    TPtr previewPtr( NULL, 0 );

    aPreviewType = ContentAccess::ENoPreview;

    preview = HBufC::NewLC( currentLength );
    previewPtr.Set( preview->Des() );

    error = aContent.GetAttribute( ContentAccess::EPreviewType, aPreviewType );

    switch ( aPreviewType )
        {

        case ContentAccess::EInstantPreview:
            // get URI of embedded preview
            error = aContent.GetStringAttribute(
                ContentAccess::EInstantPreviewUri, previewPtr );

            while ( error == KErrOverflow )
                {
                currentLength += currentLength;
                CleanupStack::PopAndDestroy( preview );
                preview = HBufC::NewLC( currentLength );
                previewPtr.Set( preview->Des() );
                error = aContent.GetStringAttribute(
                    ContentAccess::EInstantPreviewUri, previewPtr );
                }

            if ( !error && previewPtr.Length() )
                {
                aPreviewUrl = previewPtr.AllocL();
                }
            else
                {
                aPreviewType = ContentAccess::ENoPreview;
                }
            break;

        case ContentAccess::EPreviewRights:
            // get URI of preview rights
            error = aContent.GetStringAttribute(
                ContentAccess::EPreviewRightsUri, previewPtr );

            while ( error == KErrOverflow )
                {
                currentLength += currentLength;
                CleanupStack::PopAndDestroy( preview );
                preview = HBufC::NewLC( currentLength );
                previewPtr.Set( preview->Des() );
                error = aContent.GetStringAttribute(
                    ContentAccess::EPreviewRightsUri, previewPtr );
                }

            if ( !error && previewPtr.Length() )
                {
                aPreviewUrl = previewPtr.AllocL();
                }
            else
                {
                aPreviewType = ContentAccess::ENoPreview;
                }
            break;

        default:
            // no preview
            break;
        }

    CleanupStack::PopAndDestroy( preview );
    CleanupStack::PushL( aPreviewUrl );
    return error;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::GetOmaSilentRightsUrlLC
// -----------------------------------------------------------------------------
TInt DRM::CDrmUiHandlingImpl::GetOmaSilentRightsUrlLC(
    const ContentAccess::CData& aContent,
    HBufC8*& aSilentUrl )
    {
    __ASSERT_DEBUG( !aSilentUrl, User::Panic( KDRMUtilityDebugPanicMessage,
            KDRMUtilityDebugPanicCode ) );

    TInt error( KErrNone );
    TInt currentLength( KUrlMaxLen );
    Roap::RRoapStorageClient storage;
    HBufC8* silent8( NULL );
    HBufC* silent( NULL );
    TPtr silentPtr( NULL, 0 );

    silent = HBufC::NewLC( currentLength );
    silentPtr.Set( silent->Des() );

    // get SilentRightsUrl
    error = aContent.GetStringAttribute( ContentAccess::ESilentRightsUrl,
        silentPtr );

    while ( error == KErrOverflow )
        {
        currentLength += currentLength;
        CleanupStack::PopAndDestroy( silent );
        silent = HBufC::NewLC( currentLength );
        silentPtr.Set( silent->Des() );
        error = aContent.GetStringAttribute( ContentAccess::ESilentRightsUrl,
            silentPtr );
        }

    if ( !error && silentPtr.Length() )
        {
        silentPtr.TrimLeft();
        silent8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( silentPtr );
        TPtr8 silentPtr8( silent8->Des() );
        CleanupStack::PushL( silent8 );

        User::LeaveIfError( storage.Connect() );
        CleanupClosePushL( storage );

        // check that SilentRightsUrl is on the white list
        // URL is not searched from pre-configured white list
        TBool fromPreConfiguredWhiteList( EFalse );
        if ( storage.WhiteListURLExistsL( *silent8,
            fromPreConfiguredWhiteList ) )
            {
            aSilentUrl = silentPtr8.AllocL();
            }

        CleanupStack::PopAndDestroy( 2, silent8 ); //storage, silent8
        }

    CleanupStack::PopAndDestroy( silent );
    CleanupStack::PushL( aSilentUrl );
    return error;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::GetOmaInfoUrlLC
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmUiHandlingImpl::GetOmaInfoUrlLC(
    const ContentAccess::CData& aContent,
    HBufC*& aInfoUrl )
    {
    __ASSERT_DEBUG( !aInfoUrl, User::Panic( KDRMUtilityDebugPanicMessage,
            KDRMUtilityDebugPanicCode ) );

    TInt error( KErrNone );
    TInt currentLength( KUrlMaxLen );
    HBufC* info( NULL );
    TPtr infoPtr( NULL, 0 );

    info = HBufC::NewLC( currentLength );
    infoPtr.Set( info->Des() );

    // get InfoUrl
    error = aContent.GetStringAttribute( ContentAccess::EInfoURL, infoPtr );

    while ( error == KErrOverflow )
        {
        currentLength += currentLength;
        CleanupStack::PopAndDestroy( info );
        info = HBufC::NewLC( currentLength );
        infoPtr.Set( info->Des() );
        error
            = aContent.GetStringAttribute( ContentAccess::EInfoURL, infoPtr );
        }

    if ( !error && infoPtr.Length() )
        {
        aInfoUrl = infoPtr.AllocL();
        }

    CleanupStack::PopAndDestroy( info );
    CleanupStack::PushL( aInfoUrl );
    return error;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::GetOmaPendingRightsInfoLC
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmUiHandlingImpl::GetOmaPendingRightsInfoLC(
    const ContentAccess::CData& aContent,
    HBufC*& aEta )
    {
    __ASSERT_DEBUG( !aEta, User::Panic( KDRMUtilityDebugPanicMessage,
            KDRMUtilityDebugPanicCode ) );

    TInt error( KErrNone );
    HBufC* eta( NULL );
    TPtr etaPtr( NULL, 0 );

    eta = HBufC::NewLC( KDRMMaxETABufSize );
    etaPtr.Set( eta->Des() );

    // get eta info
    error = aContent.GetStringAttribute( EPendingRightsETA, etaPtr );

    if ( !error && etaPtr.Length() )
        {
        aEta = etaPtr.AllocL();
        }

    CleanupStack::PopAndDestroy( eta );
    CleanupStack::PushL( aEta );
    return error;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::GetContentLocationLC
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmUiHandlingImpl::GetContentLocationLC(
    const ContentAccess::CData& aContent,
    HBufC*& aPath )
    {
    __ASSERT_DEBUG( !aPath, User::Panic( KDRMUtilityDebugPanicMessage,
            KDRMUtilityDebugPanicCode ) );

    TInt error( KErrNone );
    TInt currentLength( KUrlMaxLen );
    TPtr pathPtr( NULL, 0 );

    aPath = HBufC::NewLC( currentLength );
    pathPtr.Set( aPath->Des() );

    error = aContent.GetStringAttribute( DRM::EDrmFullName, pathPtr );

    while ( error == KErrOverflow )
        {
        currentLength += currentLength;
        CleanupStack::PopAndDestroy( aPath );
        aPath = HBufC::NewLC( currentLength );
        pathPtr.Set( aPath->Des() );
        error = aContent.GetStringAttribute( DRM::EDrmFullName, pathPtr );
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CheckOmaDomainStatusL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::CheckOmaDomainStatusL(
    const ContentAccess::CData& aContent,
    const HBufC* aDomainRiUrl,
    TInt& aIsRegistered,
    TInt& aIsJoined )
    {
    Roap::RRoapStorageClient storage;
    CDRMRIContext* riContext( NULL );
    CDRMDomainContext* domainContext( NULL );

    HBufC* riId( NULL );
    HBufC8* riId8( NULL );
    HBufC* domainId( NULL );
    HBufC8* domainId8( NULL );

    TInt error( KErrNone );
    TPtr ptr( NULL, 0 );

    aIsJoined = EFalse;
    aIsRegistered = EFalse;

    riId = HBufC::NewLC( KUrlMaxLen );
    domainId = HBufC::NewLC( KUrlMaxLen );

    // create attribute set
    RStringAttributeSet stringAttributeSet;
    CleanupClosePushL( stringAttributeSet );
    // add the attributes we are interested in
    stringAttributeSet.AddL( EDomainId );
    stringAttributeSet.AddL( EDomainRightsIssuerId );

    User::LeaveIfError( aContent.GetStringAttributeSet( stringAttributeSet ) );

    // pass on values of string attributes
    ptr.Set( domainId->Des() );
    error = stringAttributeSet.GetValue( EDomainId, ptr );
    if ( !error )
        {
        ptr.Set( riId->Des() );
        stringAttributeSet.GetValue( EDomainRightsIssuerId, ptr );
        if ( !error )
            {
            riId8 = HBufC8::NewLC( riId->Length() );
            riId8->Des().Copy( riId->Des() );
            domainId8 = HBufC8::NewLC( domainId->Length() );
            domainId8->Des().Copy( domainId->Des() );

            if ( riId8 && riId8->Length() && aDomainRiUrl
                && aDomainRiUrl->Length() )
                {
                // Check if we have domain RO
                User::LeaveIfError( storage.Connect() );
                CleanupClosePushL( storage );
                riContext = storage.GetRIContextL( *riId8 );
                if ( riContext )
                    {
                    aIsRegistered = ETrue;
                    delete riContext;
                    if ( domainId8 && domainId8->Length() )
                        {
                        domainContext
                            = storage.GetDomainContextL( *domainId8 );
                        if ( domainContext )
                            {
                            // last three digits presents
                            // the domain generation
                            TInt generation( 0 );
                            TLex8 lex( domainId8->Right(
                                KOmaDomainGenerationLength ) );
                            lex.Val( generation );

                            if ( domainContext->DomainGeneration()
                                >= generation )
                                {
                                aIsJoined = ETrue;
                                }
                            delete domainContext;
                            }
                        }
                    }
                CleanupStack::PopAndDestroy( &storage );
                }
            CleanupStack::PopAndDestroy( 2, riId8 ); // domainId8, riId8
            }
        }
    CleanupStack::PopAndDestroy( &stringAttributeSet );
    CleanupStack::PopAndDestroy( 2, riId ); // domainId, riId
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CallRightsNotValidL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::CallRightsNotValidL(
    DRM::MDrmUiCheckRightsObserver* aObserver,
    TInt aOperationId,
    DRM::TCheckRightsStatus aRightsStatus,
    TUint32 aReason,
    ContentAccess::CData& aContent,
    const HBufC8* aContentUri,
    CDRMConstraint* aConstraint,
    const HBufC* aRightsUrl,
    DRM::TDrmUiUrlType aUrlType,
    const HBufC8* aSilentUrl,
    ContentAccess::TIntent aIntent )
    {
    DRM::TCheckRightsAction rightsAction( DRM::EUHCheckRightsActionDefault );

    if ( aObserver )
        {
        // call given CheckRightsObserver
        rightsAction = aObserver->RightsNotValid( aOperationId,
            aRightsStatus, RejectReason( aReason ) );
        }

    switch ( rightsAction )
        {
        case DRM::EUHCheckRightsActionDefault:
            {
            if ( RejectReason( aReason ) == DRM::EURejectionMetering )
                {
                // Show that only reason for error was rejected metering.
                // Qt dialog not implemented yet
                iDrmUtilityUi->DisplayNoteL( EConfUnableToOpen );
                }
            else
                {
                if ( !GetSilentAndCallL( aObserver, aOperationId, aIntent,
                    aContentUri, aSilentUrl, ETrue ) )
                    {
                    // Silent URL not present or silent retrieval failed
                    // show appropriate notes
                    if ( ShowNoRightsNoteL( aContent, RightsInfoStatus(
                        aRightsStatus ), aReason, aConstraint, aRightsUrl,
                        aUrlType, ENoRights ) )
                        {
                        GetRightsAndCallL( aObserver, aOperationId, aIntent,
                            aContentUri, aRightsUrl );
                        }
                    }
                }
            }
            break;

        case DRM::EUHCheckRightsActionOpenDetailsView:
            {
            HBufC* fullPath( NULL );
            GetContentLocationLC( aContent, fullPath );
            ShowDetailsViewL( aContentUri, fullPath );
            CleanupStack::PopAndDestroy( fullPath );
            }
            break;

        case DRM::EUHCheckRightsActionAcquireNewRights:
            {
            if ( !GetSilentAndCallL( aObserver, aOperationId, aIntent,
                aContentUri, aSilentUrl, EFalse ) )
                {
                GetRightsAndCallL( aObserver, aOperationId, aIntent,
                    aContentUri, aRightsUrl );
                if ( !aRightsUrl )
                    {
                    User::Leave( KErrArgument );
                    }
                }
            }
            break;

        case DRM::EUHCheckRightsActionIgnore:
            // do nothing
            break;

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CallRightsLeftL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::CallRightsLeftL(
    DRM::MDrmUiCheckRightsObserver* aObserver,
    TInt aOperationId,
    TBool aUnconstrained,
    TTimeIntervalSeconds aTime,
    TInt aCounts,
    TTimeIntervalSeconds aAccumulated,
    const HBufC8* aContentUri,
    const HBufC* aRightsUrl,
    const HBufC8* aSilentUrl,
    ContentAccess::CData& aContent,
    ContentAccess::TIntent aIntent )
    {
    DRM::TCheckRightsAction rightsAction( DRM::EUHCheckRightsActionDefault );
    HBufC* fullPath( NULL );

    if ( aObserver )
        {
        // call given CheckRightsObserver
        rightsAction = aObserver->RightsLeft( aOperationId, aUnconstrained,
            aTime, aCounts, aAccumulated );
        }

    switch ( rightsAction )
        {
        case DRM::EUHCheckRightsActionDefault:
            {
            // show appropriate notes, none at the moment
            }
            break;

        case DRM::EUHCheckRightsActionOpenDetailsView:
            {
            GetContentLocationLC( aContent, fullPath );
            ShowDetailsViewL( aContentUri, fullPath );
            CleanupStack::PopAndDestroy( fullPath );
            }
            break;

        case DRM::EUHCheckRightsActionAcquireNewRights:
            {
            if ( !GetSilentAndCallL( aObserver, aOperationId, aIntent,
                aContentUri, aSilentUrl, EFalse ) )
                {
                if ( !aRightsUrl )
                    {
                    User::Leave( KErrArgument );
                    }
                // launch browser to get new rights
                LaunchBrowserL( aRightsUrl );
                }
            }
            break;

        case DRM::EUHCheckRightsActionIgnore:
            // do nothing
            break;

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CallEmbeddedPreviewAvailableL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::CallEmbeddedPreviewAvailableL(
    DRM::MDrmHandleErrorObserver* aObserver,
    TInt aOperationId,
#ifdef RD_DRM_PREVIEW_RIGHT_FOR_AUDIO
    ContentAccess::CData& aContent,
#else
    ContentAccess::CData& /*aContent*/,
#endif
    const HBufC* aPreviewUrl,
    DRM::TDrmRightsInfo aRightsStatus,
    TUint32 aReason,
    const HBufC* aRightsUrl,
    const HBufC8* aContentUri,
    ContentAccess::TIntent aIntent )
    {
    DRM::TEmbeddedPreviewAction embeddedPreviewAction(
        DRM::EUEmbeddedPreviewActionDefault );

#ifdef RD_DRM_PREVIEW_RIGHT_FOR_AUDIO

    TInt buttonCode( 0 );

#endif // RD_DRM_PREVIEW_RIGHT_FOR_AUDIO
    if ( aObserver )
        {
        // call given HandleErrorObserver
        embeddedPreviewAction
            = aObserver->EmbeddedPreviewAvailable( aOperationId,
                *aPreviewUrl, aRightsStatus, RejectReason( aReason ) );
        }

    switch ( embeddedPreviewAction )
        {
        case DRM::EUEmbeddedPreviewActionDefault:
        case DRM::EUEmbeddedPreviewActionDefaultAudio:
        case DRM::EUEmbeddedPreviewActionDefaultVideo:
            {

#ifdef RD_DRM_PREVIEW_RIGHT_FOR_AUDIO

            buttonCode = iDrmUtilityUi->DisplayPopupWindowsForPreviewL(
                aContent, ContentAccess::EInstantPreview,
                embeddedPreviewAction );

            switch ( buttonCode )
                {
                case 1: // Normal Rights

                    {
                    GetRightsAndCallL( aObserver, aOperationId, aIntent,
                        aContentUri, aRightsUrl );
                    }
                    break;

                case 2: // Play Preview

                    CallPlayEmbeddedPreviewSelected( aObserver, aOperationId,
                        aPreviewUrl );

                    break;

                default:

                    break;
                }

#endif // RD_DRM_PREVIEW_RIGHT_FOR_AUDIO
            }
            break;

        case DRM::EUEmbeddedPreviewActionFetchRights:
            {
            GetRightsAndCallL( aObserver, aOperationId, aIntent, aContentUri,
                aRightsUrl );
            }
            break;

        case DRM::EUEmbeddedPreviewActionPlayPreview:
            // do nothing (observer plays the preview)
        case DRM::EUEmbeddedPreviewActionIgnore:
            // do nothing
            break;

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CallPreviewRightsUrlAvailableL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::CallPreviewRightsUrlAvailableL(
    DRM::MDrmHandleErrorObserver* aObserver,
    TInt aOperationId,
#ifdef RD_DRM_PREVIEW_RIGHT_FOR_AUDIO
    ContentAccess::CData& aContent,
#else
    ContentAccess::CData& /*aContent*/,
#endif
    const HBufC* aPreviewUrl,
    DRM::TDrmRightsInfo aRightsStatus,
    TUint32 aReason,
    const HBufC* aRightsUrl,
    const HBufC8* aContentUri,
    ContentAccess::TIntent aIntent )
    {
    DRM::TPreviewRightsAction previewRightsAction(
        DRM::EUPreviewRightsActionDefault );

#ifdef RD_DRM_PREVIEW_RIGHT_FOR_AUDIO

    TInt buttonCode( 0 );
    HBufC8* url8( NULL );

#endif // RD_DRM_PREVIEW_RIGHT_FOR_AUDIO
    if ( aObserver )
        {
        // call given HandleErrorObserver
        previewRightsAction
            = aObserver->PreviewRightsUrlAvailable( aOperationId,
                *aPreviewUrl, aRightsStatus, RejectReason( aReason ) );
        }

    switch ( previewRightsAction )
        {
        case DRM::EUPreviewRightsActionDefault:
        case DRM::EUPreviewRightsActionDefaultAudio:
        case DRM::EUPreviewRightsActionDefaultVideo:
            {

#ifdef RD_DRM_PREVIEW_RIGHT_FOR_AUDIO

            buttonCode = iDrmUtilityUi->DisplayPopupWindowsForPreviewL(
                aContent, ContentAccess::EPreviewRights, previewRightsAction );

            switch ( buttonCode )
                {
                case 1: // Normal rights

                    {
                    GetRightsAndCallL( aObserver, aOperationId, aIntent,
                        aContentUri, aRightsUrl );

                    }
                    break;

                case 2: // Preview rights
                    {
                    TInt callError( KErrNotFound );
                    if ( aPreviewUrl )
                        {
                        url8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(
                            *aPreviewUrl );
                        CleanupStack::PushL( url8 );

                        GetSilentRightsL( *url8, ETrue );

                        CleanupStack::PopAndDestroy( url8 );

                        // check if rights exist now
                        callError = CheckAndMapToCallError( iOmaClient,
                            aIntent, *aContentUri );
                        }
                    CallPreviewRightsAvailable( aObserver, aOperationId,
                        callError );
                    }
                    break;

                default:

                    break;
                }

#endif // RD_DRM_PREVIEW_RIGHT_FOR_AUDIO
            }
            break;

        case DRM::EUPreviewRightsActionFetchRights:
            {
            GetRightsAndCallL( aObserver, aOperationId, aIntent, aContentUri,
                aRightsUrl );
            }
            break;

        case DRM::EUPreviewRightsActionFetchPreviewRights:
            // do nothing (observer fetches preview rights)
        case DRM::EUPreviewRightsActionIgnore:
            // do nothing
            break;

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CallSilentRightsUrlAvailableL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::CallSilentRightsUrlAvailableL(
    DRM::MDrmHandleErrorObserver* aObserver,
    TInt aOperationId,
    const HBufC8* aSilentUrl,
    DRM::TDrmRightsInfo aRightsStatus,
    TUint32 aReason,
    const HBufC* aRightsUrl,
    const HBufC8* aContentUri,
    ContentAccess::TIntent aIntent )
    {
    DRM::TSilentRightsAction silentRightsAction(
        DRM::EUSilentRightsActionDefault );
    TInt callError( KErrNotFound );
    HBufC * silent16(
        CnvUtfConverter::ConvertToUnicodeFromUtf8L( *aSilentUrl ) );

    if ( aObserver )
        {
        // call given HandleErrorObserver
        silentRightsAction = aObserver->SilentRightsUrlAvailable(
            aOperationId, *silent16, aRightsStatus, RejectReason( aReason ) );
        }

    delete silent16;

    switch ( silentRightsAction )
        {
        case DRM::EUSilentRightsActionDefault:
            {

            GetSilentRightsL( *aSilentUrl, ETrue );

            // check if rights exist now
            callError = CheckAndMapToCallError( iOmaClient, aIntent,
                *aContentUri );
            CallSilentRightsAvailable( aObserver, aOperationId, callError );

            }
            break;

        case DRM::EUSilentRightsActionFetchRights:
            {
            GetRightsAndCallL( aObserver, aOperationId, aIntent, aContentUri,
                aRightsUrl );
            }
            break;

        case DRM::EUSilentRightsActionFetchSilentRights:
            // do nothing (observer fetches silent rights)
        case DRM::EUSilentRightsActionIgnore:
            // do nothing
            break;

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CallRightsUrlAvailableL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::CallRightsUrlAvailableL(
    DRM::MDrmHandleErrorObserver* aObserver,
    TInt aOperationId,
    DRM::TDrmRightsInfo aRightsStatus,
    TUint32 aReason,
    CDRMConstraint* aConstraint,
    ContentAccess::CData& aContent,
    const HBufC8* aContentUri,
    const HBufC* aRightsUrl,
    DRM::TDrmUiUrlType aUrlType,
    ContentAccess::TIntent aIntent,
    TInt aError )
    {
    DRM::TRightsAction rightsAction( DRM::EURightsActionDefault );

    if ( aObserver && aRightsUrl )
        {
        // call given HandleErrorObserver
        rightsAction = aObserver->RightsUrlAvailable( aOperationId,
            *aRightsUrl, aRightsStatus, RejectReason( aReason ), aUrlType );
        }

    switch ( rightsAction )
        {
        case DRM::EURightsActionDefault:
            {
            if ( ShowNoRightsNoteL( aContent, aRightsStatus, aReason,
                aConstraint, aRightsUrl, aUrlType, aError ) )
                {
                GetRightsAndCallL( aObserver, aOperationId, aIntent,
                    aContentUri, aRightsUrl );
                }
            }
            break;

        case DRM::EURightsActionFetchRights:
            {
            GetRightsAndCallL( aObserver, aOperationId, aIntent, aContentUri,
                aRightsUrl );
            }
            break;

        case DRM::EURightsActionIgnore:
            // do nothing
            break;

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CallPreviewRightsAvailable
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::CallPreviewRightsAvailable(
    DRM::MDrmHandleErrorObserver* aObserver,
    TInt aOperationId,
    TInt aError )
    {

    if ( aObserver )
        {
        // call given HandleErrorObserver
        aObserver->PreviewRightsAvailable( aOperationId, aError );
        }
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CallSilentRightsAvailable
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::CallSilentRightsAvailable(
    DRM::MDrmHandleErrorObserver* aObserver,
    TInt aOperationId,
    TInt aError )
    {

    if ( aObserver )
        {
        // call given HandleErrorObserver
        aObserver->SilentRightsAvailable( aOperationId, aError );
        }
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CallRightsAvailable
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::CallRightsAvailable(
    DRM::MDrmHandleErrorObserver* aObserver,
    TInt aOperationId,
    TInt aError )
    {

    if ( aObserver )
        {
        // call given HandleErrorObserver
        aObserver->RightsAvailable( aOperationId, aError );
        }
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CallPlayEmbeddedPreviewSelected
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::CallPlayEmbeddedPreviewSelected(
    DRM::MDrmHandleErrorObserver* aObserver,
    TInt aOperationId,
    const HBufC* aPreviewUrl )
    {
    if ( aObserver )
        {
        // call given HandleErrorObserver
        aObserver->PlayEmbeddedPreviewSelected( aOperationId, *aPreviewUrl );
        }
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::ShowNoRightsNoteL
// -----------------------------------------------------------------------------
//
TBool DRM::CDrmUiHandlingImpl::ShowNoRightsNoteL(
    ContentAccess::CData& aContent,
    DRM::TDrmRightsInfo aRightsStatus,
    TUint32 aReason,
    CDRMConstraint* aConstraint,
    const HBufC* aRightsUrl,
    DRM::TDrmUiUrlType aUrlType,
    TInt aError )
    {
    HBufC* etaBuf( NULL );
    TBool isRegistered( EFalse );
    TBool isJoined( EFalse );
    TInt ret( ECancelled );
    TInt err( KErrNone );
    TInt eta( 0 );
    TFileName fileName;
    TBool rightsRenewalWanted( EFalse );

    if ( aError != EPendingRights && aRightsStatus == DRM::EURightsInfoFuture )
        {
        iDrmUtilityUi->ShowFutureRightsNoteL( aConstraint );
        }
    else
        {
        // check if rights are expected to arrive
        GetOmaPendingRightsInfoLC( aContent, etaBuf );
        if ( etaBuf )
            {
            TLex lexer( *etaBuf );
            err = lexer.Val( eta );
            }
        CleanupStack::PopAndDestroy( etaBuf );

        if ( !err && aError == EPendingRights && eta == -1 )
            {
            // rights should have come already
            if ( aRightsUrl )
                {
                ret = iDrmUtilityUi->DisplayQueryL( EQueryGetNewLicence, KNoValue );
                }
            else
                {
                // no rights issuer
                // Qt dialog not implemented yet
                iDrmUtilityUi->DisplayNoteL( EConfLicenceNotReceived );
                }
            }
        else if ( !eta || err || eta == -1 )
            {
            // rights not expected to arrive
            if ( aUrlType == DRM::EUHDomainRiUrl )
                {
                CheckOmaDomainStatusL( aContent, aRightsUrl, isRegistered,
                    isJoined );
                }

            if ( aRightsUrl && isRegistered && !isJoined ) // Domain ro case
                {
                ret = iDrmUtilityUi->DisplayQueryL( EQueryAccountUpdate, KNoValue );
                }
            else if ( aRightsUrl ) // Device ro case
                {
                if ( aReason & EConstraintIndividual )
                    {
                    ret = iDrmUtilityUi->DisplayQueryL( EQueryFileLockedForSim, KNoValue );
                    }
                else
                    {
                    // get filename
                    User::LeaveIfError( aContent.GetStringAttribute(
                        EFileName, fileName ) );

                    // Qt dialog not implemented yet
                    ret = iDrmUtilityUi->DisplayQueryL( EQueryFileWithNoRightsObj, fileName );
                    }
                }
            else // no rights issuer
                {
                if ( aReason & EConstraintIndividual )
                    {
                    // Qt dialog not implemented yet
                    iDrmUtilityUi->DisplayNoteL( EConfFileLockedForSim );
                    }
                else
                    {
                    iDrmUtilityUi->DisplayNoteL( EConfLicenceExpired );
                    }
                }
            }
        else
            {
            // rights expected to arrive
            if ( eta != KErrCAPendingRights )
                {
                // rights expected to arrive in eta seconds
                iDrmUtilityUi->DisplayNoteL( EConfWaitingForLicence );
                }
            else
                {
                // rights should have come
                if ( aRightsUrl )
                    {
                    ret = iDrmUtilityUi->DisplayQueryL( EQueryGetNewLicence, KNoValue );
                    }
                else
                    {
                    // no rights issuer
                    // Qt dialog not implemented yet
                    iDrmUtilityUi->DisplayNoteL( EConfLicenceNotReceived );
                    }
                }
            }
        }
    if ( ret == EOk )
        {
        rightsRenewalWanted = ETrue;
        }
    return rightsRenewalWanted;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::GetOmaConstraintInfo
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmUiHandlingImpl::GetOmaConstraintInfo(
    CDRMConstraint* aConstraint,
    TUint32& aCountsLeft,
    TTime& aEndTime )
    {
    __ASSERT_DEBUG( aConstraint, User::Panic( KDRMUtilityDebugPanicMessage,
            KDRMUtilityDebugPanicCode ) );

    TInt ret( 0 );
    TTime start( Time::NullTTime() );
    aCountsLeft = 0;
    aEndTime = Time::NullTTime();

    if ( aConstraint->iActiveConstraints & EConstraintCounter )
        {
        ret |= KDRMOmaConstraintCount;
        aCountsLeft = aConstraint->iCounter;
        }
    if ( aConstraint->iActiveConstraints & EConstraintTimedCounter )
        {
        ret |= KDRMOmaConstraintCount;
        if ( aConstraint->iTimedCounter > aCountsLeft )
            {
            aCountsLeft = aConstraint->iTimedCounter;
            }
        }
    if ( aConstraint->iActiveConstraints & EConstraintEndTime )
        {
        ret |= KDRMOmaConstraintTime;
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
        ret |= KDRMOmaConstraintTime;
        }
    if ( aConstraint->iActiveConstraints & EConstraintAccumulated )
        {
        if ( aEndTime != Time::NullTTime() && aEndTime
            + aConstraint->iAccumulatedTime > aEndTime || aEndTime
            == Time::NullTTime() )
            {
            aEndTime.HomeTime();
            aEndTime = aEndTime + aConstraint->iAccumulatedTime;
            ret |= KDRMOmaConstraintAccumulated;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::RejectReason
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmUiHandlingImpl::RejectReason( TUint32 aReason )
    {
    TInt ret( DRM::EURejectionNone );

    if ( aReason & EConstraintStartTime )
        {
        ret |= DRM::EURejectionStartTime;
        }

    if ( aReason & EConstraintEndTime )
        {
        ret |= DRM::EURejectionEndTime;
        }

    if ( aReason & EConstraintInterval )
        {
        ret |= DRM::EURejectionInterval;
        }

    if ( aReason & EConstraintCounter )
        {
        ret |= DRM::EURejectionCounter;
        }

    if ( aReason & EConstraintTopLevel )
        {
        ret |= DRM::EURejectionTopLevel;
        }

    if ( aReason & EConstraintTimedCounter )
        {
        ret |= DRM::EURejectionTimedCounter;
        }

    if ( aReason & EConstraintAccumulated )
        {
        ret |= DRM::EURejectionAccumulated;
        }

    if ( aReason & EConstraintIndividual )
        {
        ret |= DRM::EURejectionIndividual;
        }

    if ( aReason & EConstraintSystem )
        {
        ret |= DRM::EURejectionSystem;
        }

    if ( aReason & EConstraintVendor )
        {
        ret |= DRM::EURejectionVendor;
        }

    if ( aReason & EConstraintSoftware )
        {
        ret |= DRM::EURejectionSoftware;
        }

    if ( aReason & EConstraintMetering )
        {
        ret |= DRM::EURejectionMetering;
        }

    if ( aReason & EConstraintNullDrmTime )
        {
        ret |= DRM::EURejectionDrmTime;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::RightsInfoStatus
// -----------------------------------------------------------------------------
//
DRM::TDrmRightsInfo DRM::CDrmUiHandlingImpl::RightsInfoStatus(
    DRM::TCheckRightsStatus aStatus )
    {
    DRM::TDrmRightsInfo ret( DRM::EURightsInfoValid );

    if ( aStatus == DRM::EUHCheckRightsExpiredRights )
        {
        ret = DRM::EURightsInfoExpired;
        }
    else if ( aStatus == DRM::EUHCheckRightsNoRights )
        {
        ret = DRM::EURightsInfoMissing;
        }
    else if ( aStatus == DRM::EUHCheckRightsFutureRights )
        {
        ret = DRM::EURightsInfoFuture;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::SelectOmaRightsUrlL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::SelectOmaRightsUrlL(
    const ContentAccess::CData& aContent,
    const HBufC* aDomainRiUrl,
    DRM::TDrmUiUrlType& aUrlType )
    {
    Roap::RRoapStorageClient storage;
    CDRMRIContext* riContext( NULL );
    CDRMDomainContext* domainContext( NULL );

    HBufC* riId( NULL );
    HBufC8* riId8( NULL );
    HBufC* domainId( NULL );
    HBufC8* domainId8( NULL );

    TInt error( KErrNone );
    TPtr ptr( NULL, 0 );
    TBool isJoined( EFalse );
    TBool isRegistered( EFalse );

    aUrlType = DRM::EUHRightsIssuerUrl;
    riId = HBufC::NewLC( KUrlMaxLen );
    domainId = HBufC::NewLC( KUrlMaxLen );

    // create attribute set
    RStringAttributeSet stringAttributeSet;
    CleanupClosePushL( stringAttributeSet );
    // add the attributes we are interested in
    stringAttributeSet.AddL( EDomainId );
    stringAttributeSet.AddL( EDomainRightsIssuerId );

    User::LeaveIfError( aContent.GetStringAttributeSet( stringAttributeSet ) );

    // pass on values of string attributes
    ptr.Set( domainId->Des() );
    error = stringAttributeSet.GetValue( EDomainId, ptr );
    if ( !error )
        {
        ptr.Set( riId->Des() );
        stringAttributeSet.GetValue( EDomainRightsIssuerId, ptr );
        if ( !error )
            {
            riId8 = HBufC8::NewLC( riId->Length() );
            riId8->Des().Copy( riId->Des() );
            domainId8 = HBufC8::NewLC( domainId->Length() );
            domainId8->Des().Copy( domainId->Des() );

            if ( riId8 && riId8->Length() && aDomainRiUrl
                && aDomainRiUrl->Length() )
                {
                // Check if we have domain RO
                User::LeaveIfError( storage.Connect() );
                CleanupClosePushL( storage );
                riContext = storage.GetRIContextL( *riId8 );
                if ( riContext )
                    {
                    isRegistered = ETrue;
                    delete riContext;
                    if ( domainId8 && domainId8->Length() )
                        {
                        domainContext
                            = storage.GetDomainContextL( *domainId8 );
                        if ( domainContext )
                            {
                            // last three digits presents
                            // the domain generation
                            TInt generation( 0 );
                            TLex8 lex( domainId8->Right(
                                KOmaDomainGenerationLength ) );
                            lex.Val( generation );

                            if ( domainContext->DomainGeneration()
                                >= generation )
                                {
                                isJoined = ETrue;
                                }
                            delete domainContext;
                            }
                        }
                    }
                CleanupStack::PopAndDestroy( &storage );

                if ( aDomainRiUrl && aDomainRiUrl->Length()
                    && ( !isRegistered | !isJoined ) )
                    {
                    aUrlType = DRM::EUHDomainRiUrl;
                    }

                }
            CleanupStack::PopAndDestroy( 2, riId8 ); // domainId8, riId8
            }
        }
    CleanupStack::PopAndDestroy( &stringAttributeSet );
    CleanupStack::PopAndDestroy( 2, riId ); // domainId, riId
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::LaunchRightsManagerUiL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::LaunchRightsManagerUiL( const TDesC& aParam16 )
    {
    const TUid KUidDRMUI =
        {
        0x101f85c7
        };

    if ( iCoeEnv )
        {
        CAiwGenericParamList* paramList( CAiwGenericParamList::NewLC() );
        TAiwVariant variantObject( aParam16 );
        TAiwGenericParam param( EGenericParamFile, variantObject );
        paramList->AppendL( param );

        CAknLaunchAppService* launchAppService( CAknLaunchAppService::NewL(
            KUidDRMUI, this, paramList ) );
        CleanupStack::PushL( launchAppService );
        iWait.Start();
        CleanupStack::PopAndDestroy( 2, paramList );
        }
    else
        {
        // no UI to embed, launch Rights Manager UI stand-alone
        // check if Rights Manager UI is already running
        RWsSession wsSession;
        User::LeaveIfError( wsSession.Connect() );
        CleanupClosePushL( wsSession );
        TApaTaskList tasklist( wsSession );
        TApaTask task( tasklist.FindApp( KUidDRMUI ) );
        if ( task.Exists() )
            {
            // Rights Manager UI is already running
            HBufC8* param8( HBufC8::NewLC( aParam16.Length() ) );
            TPtr8 parametersPtr( param8->Des() );
            parametersPtr.Copy( aParam16 );
            task.SendMessage( TUid::Uid( KUidApaMessageSwitchOpenFileValue ),
                *param8 );
            CleanupStack::PopAndDestroy( param8 );
            }
        else
            {
            RApaLsSession appArcSession;
            User::LeaveIfError( appArcSession.Connect() );
            CleanupClosePushL( appArcSession );
            TThreadId id;
            appArcSession.StartDocument( aParam16, KUidDRMUI, id );
            CleanupStack::PopAndDestroy( &appArcSession );
            }
        CleanupStack::PopAndDestroy( &wsSession );
        }
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CreateLaunchParamL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::CreateLaunchParamL(
    const HBufC8* aUrl,
    HBufC*& aLaunchParam,
    const HBufC* aFullPath )
    {
    __ASSERT_DEBUG( !aLaunchParam && aUrl && aFullPath,
        User::Panic( KDRMUtilityDebugPanicMessage,
            KDRMUtilityDebugPanicCode ) );
    _LIT( KMarker, "\x00" );

    TPtr ptr( NULL, 0 );
    TInt localId( 0 );

    // MaxInt will fit into 10 characters
    HBufC* localIDBuf( HBufC::NewLC( KIntegerMaxLen ) );
    ptr.Set( localIDBuf->Des() );
    ptr.AppendNum( localId );

    // length of startparam and drm protection scheme are always 1 and
    // 5 markers are needed
    const TInt KMarkersForParams( 5 );
    TInt length( 1 + aUrl->Length() + ptr.Length() + aFullPath->Length()
        + KMarkersForParams + 1 );

    aLaunchParam = HBufC::NewLC( length );
    ptr.Set( aLaunchParam->Des() );

    // Start parameter is 3 or 4 for launching details view embedded or standalone from
    // DRM Utility, respectively, with full path name included as a parameter. Values 1
    // and 2 are for launching details view from DRM Helper.
    if ( iCoeEnv )
        {
        ptr.AppendNum( EDrmLaunchParamEmbeddedUtility );
        }
    else
        {
        ptr.AppendNum( EDrmLaunchParamStandAloneUtility );
        }
    ptr.Append( KMarker );
    ptr.Append( *localIDBuf );
    ptr.Append( KMarker );

    HBufC* contentUrl( CnvUtfConverter::ConvertToUnicodeFromUtf8L( *aUrl ) );

    ptr.Append( *contentUrl );
    ptr.Append( KMarker );

    // OMA DRM protection scheme
    ptr.AppendNum( EDrmSchemeOmaDrm );
    ptr.Append( KMarker );

    ptr.Append( *aFullPath );
    ptr.Append( KMarker );

    delete contentUrl;

    CleanupStack::Pop( aLaunchParam );
    CleanupStack::PopAndDestroy( localIDBuf );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::CreateLaunchParamL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::CreateLaunchParamL(
    const HBufC* aUrl,
    HBufC*& aLaunchParam )
    {
    __ASSERT_DEBUG( !aLaunchParam && aUrl,
        User::Panic( KDRMUtilityDebugPanicMessage,
            KDRMUtilityDebugPanicCode ) );
    _LIT( KZero, "0" );
    _LIT( KMarker, "\x00" );

    TPtr ptr( NULL, 0 );

    // Length of startparam and drm protection scheme and zero local id
    // are always 1 and total of 4 markers are needed
    TInt length( 1 + aUrl->Length() + 1 + 4 + 1 );

    aLaunchParam = HBufC::NewLC( length );
    ptr.Set( aLaunchParam->Des() );

    // Start parameter is 3 or 4 for launching details view embedded or standalone from
    // DRM Utility, respectively, with full path name included as a parameter. Values 1
    // and 2 are for launching details view from DRM Helper.
    if ( iCoeEnv )
        {
        ptr.AppendNum( EDrmLaunchParamEmbeddedUtility );
        }
    else
        {
        ptr.AppendNum( EDrmLaunchParamStandAloneUtility );
        }
    ptr.Append( KMarker );
    // Default value 0 for localId in case of WM DRM file
    ptr.Append( KZero );
    ptr.Append( KMarker );
    ptr.Append( *aUrl );
    ptr.Append( KMarker );
    // WM DRM protection scheme
    ptr.AppendNum( EDrmSchemeWmDrm );
    ptr.Append( KMarker );

    CleanupStack::Pop( aLaunchParam );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::LaunchBrowserL
// Launches browser embedded with given URL
// -----------------------------------------------------------------------------
//
TBool DRM::CDrmUiHandlingImpl::LaunchBrowserL( const HBufC8* aUrl )
    {
    __ASSERT_DEBUG( aUrl, User::Panic( KDRMUtilityDebugPanicMessage,
            KDRMUtilityDebugPanicCode ) );

    // URL needs to be unicode
    HBufC* parameters( HBufC::NewLC( aUrl->Length() ) );
    parameters->Des().Copy( *aUrl );
    TBool embeddedLaunch( LaunchBrowserL( parameters ) );
    CleanupStack::PopAndDestroy( parameters );
    return embeddedLaunch;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::LaunchBrowserL
// Launches browser embedded with given URL
// -----------------------------------------------------------------------------
//
TBool DRM::CDrmUiHandlingImpl::LaunchBrowserL( const HBufC* aUrl )
    {
    __ASSERT_DEBUG( aUrl, User::Panic( KDRMUtilityDebugPanicMessage,
            KDRMUtilityDebugPanicCode ) );

    HBufC* newUrl( NULL );
    TPtrC ptrc( NULL, 0 );

    ptrc.Set( *aUrl );
    TBool embeddedLaunch( EFalse );
    // if no scheme exists, use http as default scheme
    _LIT( KColon, ":" );
    if ( ptrc.Find( KColon ) == KErrNotFound )
        {
        _LIT( KHttp, "http://" );
        const TInt KHttpLen( 7 );
        newUrl = HBufC::NewLC( KHttpLen + aUrl->Length() );
        TPtr ptr( newUrl->Des() );
        ptr.Append( KHttp );
        ptr.Append( *aUrl );
        ptrc.Set( *newUrl );
        }

    CDrmBrowserLauncher* browserLauncher = CDrmBrowserLauncher::NewLC();
    	
    browserLauncher->LaunchUrlL(ptrc);
    
    CleanupStack::PopAndDestroy(); // browserLauncher

    // delete newUrl if needed
    if ( newUrl )
        {
        CleanupStack::PopAndDestroy( newUrl );
        }
    return embeddedLaunch;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::GetSilentRightsL
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmUiHandlingImpl::GetSilentRightsL(
    const TDesC8& aUrl,
    const TBool aShowNotes )
    {
    TInt r( KErrCancel );
    TInt buttonCode( EOk );
    HBufC8* url( NULL );

    if ( !SilentRightsAllowedL() )
        {
        buttonCode = ECancelled;
        if ( aShowNotes )
            {
            // Qt dialog not implemented yet
            buttonCode = iDrmUtilityUi->DisplayQueryL( EQueryConnectToActivate, KNoValue );
            }
        }
    else if ( !(HasDefConn()) )
        {
        buttonCode = ECancelled;
        if ( aShowNotes )
            {
            // Qt dialog not implemented yet
            buttonCode = iDrmUtilityUi->DisplayQueryL( EQueryConnectToActivate, KNoValue );
            }
        }

    if ( buttonCode == EOk )
        {
        TBool APs( HasAccessPointsL() );
        if ( !APs )
            {
            // No AP defined
            if ( aShowNotes )
                {
                // Qt dialog not implemented yet
                iDrmUtilityUi->DisplayNoteL( EConfNoAccessPoint );
                }
            r = KErrCANoRights;
            }
        else
            {
            // ok to start download
            CDrmUtilityDownloadManager* dlMgr(
                CDrmUtilityDownloadManager::NewL() );
            CleanupStack::PushL( dlMgr );
            url = aUrl.AllocLC();

            if ( iCoeEnv && aShowNotes )
                {
                    TRAP( r, dlMgr->DownloadAndHandleRoapTriggerL( url,
                            *iCoeEnv ) );
                }
            else
                {
                    TRAP( r, dlMgr->DownloadAndHandleRoapTriggerL( url ) );
                }
            CleanupStack::PopAndDestroy( url );

            if ( r )
                {
                // rights were tried to get only if
                // the user hasn't cancelled
                if ( r != KErrCancel )
                    {
                    if ( r == KErrCouldNotConnect )
                        {
                        // Connection failed with selected AP
                        if ( aShowNotes )
                            {
                            // Qt dialog not implemented yet
                            iDrmUtilityUi->DisplayNoteL( EConfConnectionFailed );
                            }
                        r = KErrCANoRights;
                        }
                    else
                        {
                        if ( aShowNotes )
                            {
                            // get error url
                            HBufC8* errorUrl( dlMgr->GetErrorUrlL() );
                            CleanupStack::PushL( errorUrl );

                            // error url is got only if temporary roap error

                            if ( errorUrl )
                                {
                                // ask user whether error url should be opened
                                // Qt dialog not implemented yet
                                buttonCode = iDrmUtilityUi->DisplayQueryL( EQueryOpenErrorUrl, KNoValue );

                                if ( buttonCode == EOk )
                                    {
                                    // Launch browser
                                    LaunchBrowserL( errorUrl );
                                    }
                                }
                            else
                                {
                                // Qt dialog not implemented yet
                                iDrmUtilityUi->DisplayNoteL( EConfUnableToUnlock );
                                }
                            CleanupStack::PopAndDestroy( errorUrl );
                            }
                        }
                    }
                }
            CleanupStack::PopAndDestroy( dlMgr );
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::SilentRightsAllowedL
// -----------------------------------------------------------------------------
//
TBool DRM::CDrmUiHandlingImpl::SilentRightsAllowedL()
    {
    TInt value( KErrNone );
    CRepository* repository( CRepository::NewL( KCRUidDRMSettings ) );
    repository->Get( KDRMSettingsSilentRightsAcquisition, value );
    delete repository;
    return !value ? EFalse : ETrue;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::HandleServerAppExit
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::HandleServerAppExit( TInt aReason )
    {
    if ( aReason == EAknCmdExit)
        {
        CAknEnv::RunAppShutter();
        }

    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::AppendToQueue
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::AppendToQueue( DRM::CDrmUiHandlingData* aData )
    {
    __ASSERT_DEBUG( aData, User::Panic( KDRMUtilityDebugPanicMessage,
            KDRMUtilityDebugPanicCode ) );

    // Critical area start:
    iSemaphore.Wait();

    if ( !iLast )
        {
        iFirst = aData;
        iLast = iFirst;
        }
    else
        {
        iLast->iNext = aData;
        iLast = aData;
        }

    // Critical area end
    iSemaphore.Signal();
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::PopFront
// -----------------------------------------------------------------------------
//
DRM::CDrmUiHandlingData* DRM::CDrmUiHandlingImpl::PopFront()
    {
    DRM::CDrmUiHandlingData* data( NULL );

    // If there is nothing in the queue return NULL
    if ( !iFirst )
        {
        return data;
        }

    // Critical area start:
    iSemaphore.Wait();

    data = iFirst;

    // Set the first and the next
    iFirst = iFirst->iNext;

    // reset the pointer on data
    data->iNext = NULL;

    // if there is no next in iFirst, this is the last or if there is no iFirst
    if ( !iFirst || !iFirst->iNext )
        {
        iLast = iFirst;
        }

    // Critical Area End
    iSemaphore.Signal();

    return data;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::GetSilentAndCallL
// -----------------------------------------------------------------------------
//
TBool DRM::CDrmUiHandlingImpl::GetSilentAndCallL(
    MDrmHandleErrorObserver* aObserver,
    TInt aOperationId,
    const ContentAccess::TIntent aIntent,
    const HBufC8* aContentUri,
    const HBufC8* aSilentUrl,
    const TBool aShowNotes )
    {
    TInt callError( KErrNotFound );
    if ( aSilentUrl )
        {
        GetSilentRightsL( *aSilentUrl, aShowNotes );
        callError
            = CheckAndMapToCallError( iOmaClient, aIntent, *aContentUri );
        CallSilentRightsAvailable( aObserver, aOperationId, callError );
        }
    return callError == KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandlingImpl::GetRightsAndCallL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandlingImpl::GetRightsAndCallL(
    MDrmHandleErrorObserver* aObserver,
    TInt aOperationId,
    const ContentAccess::TIntent aIntent,
    const HBufC8* aContentUri,
    const HBufC* aRightsUrl )
    {
    TInt callError( KErrNotFound );
    TBool embeddedLaunch( EFalse );
    if ( aRightsUrl && LaunchBrowserL( aRightsUrl ) )
        {
        embeddedLaunch = ETrue;
        // Browser launched in embedded mode.
        // Check if rights exist now.
        callError
            = CheckAndMapToCallError( iOmaClient, aIntent, *aContentUri );
        }
    // Inform status to observer if possible
    if ( embeddedLaunch || !aRightsUrl )
        {
        CallRightsAvailable( aObserver, aOperationId, callError );
        }

    }
//  End of File
