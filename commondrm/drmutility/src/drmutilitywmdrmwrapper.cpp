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
* Description:  WM Drm wrapper implementation
*
*/


// INCLUDE FILES

// launching embedded details view
#include <AknLaunchAppService.h>
#include <AiwGenericParam.h>
#include <apgcli.h>
#include <apgtask.h>
#include <w32std.h>

// character conversions
#include <utf.h>
#include <f32file.h>
#include <avkon.hrh>

// access point utils

#include <cdblen.h>
#include <cmconnectionmethod.h>
#include <cmdestination.h>
#include <cmconnectionmethoddef.h>
#include <cmmanager.h>

#include    <wmdrmagent.h> // for WMDRM file details view
#include    <drmutilitytypes.h>
#include    <drmuicheckrightsobserver.h>

// resources
#include    <drmutility.rsg>

#include    "drmutilitywmdrmwrapper.h"
#include    "drmutilityui.h"

#include    "drmagents.h"
#include    "DRMClockClient.h"

#include    "drmutilityinternaltypes.h"
#include    "drmutilitywmdrmutilities.h"

#include    "wmdrmdlawrapper.h"
#include    "drmuidialogids.h"

// CONSTANTS
const TInt KMaxUrlLength( 1024 );
const TInt KMaxUrlSanityLength( 102400 );

#ifdef _DEBUG
// debug panic
_LIT( KWmDrmWrapperDebugPanicMessage, "WmDrmWrapperDebugPanic" );
const TInt KWmDrmWrapperDebugPanicCode( 1 );
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDrmUtilityWMDrmWrapper::CDrmUtilityWMDrmWrapper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
DRM::CDrmUtilityWMDrmWrapper::CDrmUtilityWMDrmWrapper() :
    iCoeEnv( NULL ),
    iDrmUtilityUi( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CDrmUtilityWMDrmWrapper::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityWMDrmWrapper::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    }

// -----------------------------------------------------------------------------
// CDrmUtilityWMDrmWrapper::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmUtilityWMDrmWrapper* DRM::CDrmUtilityWMDrmWrapper::NewL()
    {
    DRM::CDrmUtilityWMDrmWrapper* self( NewLC() );

    CleanupStack::Pop( self );

    return self;
    }


// -----------------------------------------------------------------------------
// CDrmUtilityWMDrmWrapper::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmUtilityWMDrmWrapper* DRM::CDrmUtilityWMDrmWrapper::NewLC()
    {
    DRM::CDrmUtilityWMDrmWrapper* self( new( ELeave ) CDrmUtilityWMDrmWrapper );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }


// Destructor
DRM::CDrmUtilityWMDrmWrapper::~CDrmUtilityWMDrmWrapper()
    {
    delete iDrmUtilityUi;
    delete iDlaWrapper;
    iDlaWrapperDll.Close();
    iFs.Close();
    }


// -----------------------------------------------------------------------------
// CDrmUtilityWMDrmWrapper::CheckWmRightsAmountL()
// Syncronous method
// -----------------------------------------------------------------------------
//

EXPORT_C void DRM::CDrmUtilityWMDrmWrapper::CheckWmRightsAmountL(
    ContentAccess::CData& aContent,
    ContentAccess::TIntent aIntent,
    DRM::MDrmUiCheckRightsObserver* aObserver,
    TInt aOperationId )
    {
    TInt value( EFalse );
    HBufC* contentId( HBufC::NewLC( KMaxUrlLength ) ); // 1024
    TPtr cid( contentId->Des() );
    TBool unlimited( EFalse );
    TInt32 reason( 0 );
    TInt counts( 0 );
    TTimeIntervalSeconds time( 0 );
    TTimeIntervalSeconds accumulated( 0 );
    TInt error( KErrNone );

    // Check intent:
    switch( aIntent )
        {
        case ContentAccess::EPlay:
        case ContentAccess::EView:

            break;

        default:

            // Intent not supported, leave with argument:
            User::Leave( KErrArgument );

            break;
        }


    // Ask the rights from CAF, same call for both ECanPlay and ECanView
    User::LeaveIfError( aContent.GetAttribute( ContentAccess::ECanPlay, value ) );

    // Loop needed to make sure the full path name fits to the cid.
    // If the size is not enough, increase it in steps of one kB.
    while ( ( error = aContent.GetStringAttribute( DRM::EDrmFullName, cid ) )
        == KErrOverflow )
        {
        if ( contentId->Size() + KMaxUrlLength > KMaxUrlSanityLength )
            {
            // Cid is so big that it can not fit to the 100 kB default max buffer.
            User::Leave( error );
            }
        CleanupStack::Pop( contentId );
        contentId->ReAllocL( contentId->Size() + KMaxUrlLength );
        cid.Set( contentId->Des() );
        CleanupStack::PushL( contentId );
        }

    if ( error )
        {
        User::Leave( error );
        }

    // Object can be played and rights are in order:
    if ( value )
        {
        // Get the rights data
        GetRightsDataL(
            cid,
            unlimited,
            time,
            counts,
            accumulated );

        // call given CheckRightsObserver
        // What kind of rights are available:
        CallRightsLeftL (
            aObserver,
            aOperationId,
            unlimited,
            time,
            counts,
            accumulated,
            aContent,
            cid );
        }
    else // The rights are not valid at this point always claim they have expired
        {
        CallRightsNotValidL(
            aObserver,
            aOperationId,
            EUHCheckRightsExpiredRights,
            reason,
            aContent,
            cid );
        }

    CleanupStack::PopAndDestroy( contentId );
    }


// -----------------------------------------------------------------------------
// CDrmUtilityWMDrmWrapper::HandleWmErrorL()
// Syncronous method
// -----------------------------------------------------------------------------
//

EXPORT_C void DRM::CDrmUtilityWMDrmWrapper::HandleWmErrorL(
    ContentAccess::CData& aContent,
    ContentAccess::TIntent /* aIntent */,
    TInt /* aError */,
    DRM::MDrmHandleErrorObserver* aObserver,
    TInt aOperationId )
    {

    TInt32 reason( 0 );
    TInt value( EFalse );
    HBufC* contentId( HBufC::NewLC( KMaxUrlLength ) ); // 1024
    TPtr cid( contentId->Des() );

    // Check Content Id via CAF
    User::LeaveIfError( aContent.GetStringAttribute( ContentAccess::EContentId, cid ) );

    // Ask the rights from CAF, same call for both ECanPlay and ECanView
    User::LeaveIfError( aContent.GetAttribute( ContentAccess::ECanPlay, value ) );

    if ( !value )
        {
        ShowNoRightsNoteL( aContent, reason, aOperationId, aObserver );
        User::LeaveIfError( aContent.GetAttribute( ContentAccess::ECanPlay, value ) );
        if ( value )
            {
            CallRightsAvailable( aObserver, aOperationId, KErrNone );
            }
        else
            {
            CallRightsAvailable( aObserver, aOperationId, KErrCANoRights );
            }
        }
    else
        {
        User::Leave( KErrArgument );
        }

    CleanupStack::PopAndDestroy( contentId );
    }


// -----------------------------------------------------------------------------
// CDrmUtilityWMDrmWrapper::SetCoeEnv()
// Syncronous method
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUtilityWMDrmWrapper::SetCoeEnv( CCoeEnv* aCoeEnv )
    {
    iCoeEnv = aCoeEnv;
    }

// -----------------------------------------------------------------------------
// CDrmUtilityWMDrmWrapper::GetRightsManagerL
// -----------------------------------------------------------------------------
//
ContentAccess::CRightsManager* DRM::CDrmUtilityWMDrmWrapper::GetRightsManagerL()
    {
    ContentAccess::CManager* manager( NULL );
    ContentAccess::CRightsManager* rightsManager( NULL );
    RArray<ContentAccess::TAgent> agents;
    TInt agent( 0 );

    CleanupClosePushL( agents );
    manager = ContentAccess::CManager::NewLC();

    manager->ListAgentsL( agents );

    for ( agent = 0; agent < agents.Count(); agent++ )
        {
        if (agents[agent].Name().Compare( KWmDrmAgentName ) == 0)
            {
            break;
            }
        }

    // If no WM DRM agent is found, leave
    if( agent >= agents.Count() )
        {
        User::Leave( KErrNotSupported );
        }


    // Create a CRightsManager for the agent found
    rightsManager = manager->CreateRightsManagerL( agents[agent] );

    CleanupStack::PopAndDestroy( manager ); // manager
    CleanupStack::PopAndDestroy( &agents ); // agents
    return rightsManager;
    }


// -----------------------------------------------------------------------------
// CDrmUtilityWMDrmWrapper::GetRightsDataL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityWMDrmWrapper::GetRightsDataL(
    const TDesC& aUniqueId,
    TBool& aUnconstrained,
    TTimeIntervalSeconds& aTime,
    TInt& aCounts,
    TTimeIntervalSeconds& aAccumulated )
    {
    ContentAccess::RStreamablePtrArray<ContentAccess::CRightsInfo> aArray;
    ContentAccess::CRightsManager* manager( NULL );
    CleanupClosePushL(aArray);

    aUnconstrained = EFalse;
    aTime = 0;
    aCounts = 0;
    aAccumulated = 0;

    // Create the manager:
    manager = GetRightsManagerL();
    CleanupStack::PushL( manager );

    // Get the list of rights:
    manager->ListRightsL( aArray, aUniqueId );

    // Evaluate the rights:
    if( aArray.Count() )
        {
        for ( TInt i( 0 ); i < aArray.Count(); i++ )
            {
            switch( aArray[i]->RightsStatus() )
                {
                // All of these the rights don't exist or are expired:
                case ContentAccess::ERightsStatusNone:
                case ContentAccess::ERightsStatusExpired:
                case ContentAccess::ERightsStatusPending:

                    break;

                // the rights are valid:
                // Check the duration, counts and whether the rights
                // are unlimited or not
                case ContentAccess::ERightsStatusValid:
                    DrmUtilityWmDrmUtilities::CheckWmDrmRightsL(
                        aUnconstrained, aTime, aCounts, *aArray[0] );
                    break;
                }
            }
        }
    else
        {
        User::Leave( KErrCANoRights );
        }

    CleanupStack::PopAndDestroy( manager );
    CleanupStack::PopAndDestroy( &aArray ); // aArray
    }


// -----------------------------------------------------------------------------
// CDrmUtilityWMDrmWrapper::CallRightsLeftL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityWMDrmWrapper::CallRightsLeftL(
    DRM::MDrmUiCheckRightsObserver* aObserver,
    TInt aOperationId,
    TBool aUnconstrained,
    TTimeIntervalSeconds aTime,
    TInt aCounts,
    TTimeIntervalSeconds aAccumulated,
    ContentAccess::CData& aContent,
    const TDesC& aContentUri )
    {
    DRM::TCheckRightsAction rightsAction( DRM::EUHCheckRightsActionDefault );

    if ( aObserver )
        {
        rightsAction = aObserver->RightsLeft( aOperationId,
                                              aUnconstrained,
                                              aTime,
                                              aCounts,
                                              aAccumulated );
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
            // Commented out for now, should do details for WM DRM when supported
            // create parameter string
            HBufC* launchParam( NULL );
            CreateLaunchParamL( &aContentUri, launchParam ); // use zero as localid
            CleanupStack::PushL( launchParam );

            // show details
            LaunchRightsManagerUiL( *launchParam );
            CleanupStack::PopAndDestroy( launchParam );
            }
            break;

        case DRM::EUHCheckRightsActionAcquireNewRights:
            TRAPD( err, LoadDlaWrapperL() );
            if ( !err )
                {
                SilentDlaLicenseAcquisitionL( aContent );
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
// CDrmUtilityWMDrmWrapper::CallRightsNotValidL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityWMDrmWrapper::CallRightsNotValidL(
    DRM::MDrmUiCheckRightsObserver* aObserver,
    TInt aOperationId,
    DRM::TCheckRightsStatus aRightsStatus,
    TUint32 aReason,
    ContentAccess::CData& aContent,
    const TDesC& aContentUri )
    {
    DRM::TCheckRightsAction rightsAction( DRM::EUHCheckRightsActionDefault );

    if ( aObserver )
        {
        // call given CheckRightsObserver
        rightsAction = aObserver->RightsNotValid( aOperationId,
                                                  aRightsStatus,
                                                  RejectReason( aReason ) );
        }

    //rightsAction = DRM::EUHCheckRightsActionDefault;

    switch ( rightsAction )
        {
        case DRM::EUHCheckRightsActionDefault:
            {
            ShowNoRightsNoteL( aContent, aReason, aOperationId, aObserver );
            }
            break;

        case DRM::EUHCheckRightsActionOpenDetailsView:
            {
            // Commented out for now, should do details for WM DRM when supported
            // create parameter string
            HBufC* launchParam( NULL );
            CreateLaunchParamL( &aContentUri, launchParam ); // use zero as localid
            CleanupStack::PushL( launchParam );

            // show details
            LaunchRightsManagerUiL( *launchParam );
            CleanupStack::PopAndDestroy( launchParam );
            }
            break;

        case DRM::EUHCheckRightsActionAcquireNewRights:
            TRAPD( err, LoadDlaWrapperL() );
            if ( !err )
                {
                SilentDlaLicenseAcquisitionL( aContent );
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
// CDrmUtilityWMDrmWrapper::CallRightsAvailable
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityWMDrmWrapper::CallRightsAvailable(
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
// CDrmUtilityWMDrmWrapper::ShowNoRightsNoteL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityWMDrmWrapper::ShowNoRightsNoteL(
    ContentAccess::CData& aContent,
    TUint32 /*aReason*/,
    TInt aOperationId,
    DRM::MDrmHandleErrorObserver* aObserver )
    {
    TInt value;
    TRAPD( err, LoadDlaWrapperL() );
    if ( !err )
        {
        TInt ret( EOk );
        RFile file;
        GetRFileFromCDataL( aContent, file );
        CleanupClosePushL( file );
        TBool isSilent( EFalse );
        TRAP( err, isSilent = IsDlaLicenseAcquisitionSilentL( file ) );
        if ( !err && !isSilent )
            {
            if ( !iDrmUtilityUi )
                {
                iDrmUtilityUi = DRM::CDrmUtilityUI::NewL( iCoeEnv );
                }
            TFileName fileName;
            User::LeaveIfError( aContent.GetStringAttribute( DRM::EDrmFileName, fileName ) );
            // Qt dialog not implemented yet
            ret = iDrmUtilityUi->DisplayQueryL( EQueryFileWithNoRightsObj, fileName );
            }

        if ( !err && ret == EOk )
            {
            TRAP_IGNORE( DlaLicenseAcquisitionL( file ) );
            
            // Ask the rights from CAF, same call for both ECanPlay and ECanView
            aContent.GetAttribute( ContentAccess::ECanPlay, value );
            // call given HandleErrorObserver
            if( value > 0 )
                {
                CallRightsAvailable( aObserver, aOperationId, KErrNone );             
                }
            }
        CleanupStack::PopAndDestroy( &file );
        }
    else
        {
        if ( !iDrmUtilityUi )
            {
            iDrmUtilityUi = DRM::CDrmUtilityUI::NewL( iCoeEnv );
            }
        iDrmUtilityUi->DisplayNoteL( EConfLicenceExpired );
        }
    }

// -----------------------------------------------------------------------------
// CDrmUtilityWMDrmWrapper::RejectReason
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmUtilityWMDrmWrapper::RejectReason(
    TUint32 /* aReason */ )
    {
    TInt ret = DRM::EURejectionNone;

    return ret;
    }


// -----------------------------------------------------------------------------
// CDrmUtilityWMDrmWrapper::LaunchRightsManagerUiL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityWMDrmWrapper::LaunchRightsManagerUiL(
    const TDesC& aParam16 )
    {

    const TUid KUidDRMUI = { 0x101f85c7 };

    if ( iCoeEnv )
        {
        CAiwGenericParamList* paramList( CAiwGenericParamList::NewLC() );
        TAiwVariant variantObject( aParam16 );
        TAiwGenericParam param( EGenericParamFile, variantObject );
        paramList->AppendL( param );

        CAknLaunchAppService* launchAppService(
                                    CAknLaunchAppService::NewL( KUidDRMUI,
                                                                this,
                                                                paramList ) );
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
            TThreadId id;
            appArcSession.StartDocument( aParam16, KUidDRMUI, id );
            appArcSession.Close();
            }
        CleanupStack::PopAndDestroy( &wsSession );
        }
    }


// -----------------------------------------------------------------------------
// CDrmUtilityWMDrmWrapper::CreateLaunchParamL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityWMDrmWrapper::CreateLaunchParamL(
    const TDesC* aUrl,
    HBufC*& aLaunchParam )
    {

    __ASSERT_DEBUG( !aLaunchParam && aUrl,
                    User::Panic( KWmDrmWrapperDebugPanicMessage,
                                 KWmDrmWrapperDebugPanicCode ) );

    _LIT( KZero, "0" );
    _LIT( KMarker, "\x00" );

    TPtr ptr( NULL, 0 );

    // length of startparam and drm protection type indicator and zero local id
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
// CDrmUtilityWMDrmWrapper::HandleServerAppExit
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityWMDrmWrapper::HandleServerAppExit( TInt /*aReason*/ )
    {
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }

// -----------------------------------------------------------------------------
// CDrmUtilityWMDrmWrapper::LoadDlaWrapperL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityWMDrmWrapper::LoadDlaWrapperL()
    {
    if ( !iDlaWrapper )
        {
        User::LeaveIfError( iDlaWrapperDll.Load( KWmDrmDlaWrapperName ) );
        iDlaWrapper = (CWmDrmDlaWrapper*)iDlaWrapperDll.Lookup( KWmDrmDlaNewL )();
        }
    }

// -----------------------------------------------------------------------------
// CDrmUtilityWMDrmWrapper::GetRFileFromCDataL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityWMDrmWrapper::GetRFileFromCDataL(
    ContentAccess::CData& aContent,
    RFile& aFile )
    {
    TFileName fileName;
    User::LeaveIfError( aContent.GetStringAttribute( DRM::EDrmFullName, fileName ) );
    User::LeaveIfError( aFile.Open( iFs, fileName, EFileRead | EFileShareReadersOnly ) );
    }

// -----------------------------------------------------------------------------
// CDrmUtilityWMDrmWrapper::IsDlaLicenseAcquisitionSilentL
// -----------------------------------------------------------------------------
//
TBool DRM::CDrmUtilityWMDrmWrapper::IsDlaLicenseAcquisitionSilentL(
    RFile& aFile  )
    {
    LoadDlaWrapperL();
    return iDlaWrapper->SilentL( aFile );
    }

// -----------------------------------------------------------------------------
// CDrmUtilityWMDrmWrapper::DlaLicenseAcquisitionL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityWMDrmWrapper::DlaLicenseAcquisitionL(
    RFile& aFile )
    {
    TInt iapId( 0 );
    HBufC* contentUrl( NULL );
    HBufC* htmlData( NULL );
    LoadDlaWrapperL();
    TRAPD( err, iapId = DefaultAccessPointL() );
    if ( !err )
        {
        iDlaWrapper->SetIapId( iapId );
        }
    iDlaWrapper->AcquireLicenseL( aFile, contentUrl, htmlData  );
    delete contentUrl;
    delete htmlData;
    }

// -----------------------------------------------------------------------------
// CDrmUtilityWMDrmWrapper::SilentDlaLicenseAcquisitionL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityWMDrmWrapper::SilentDlaLicenseAcquisitionL(
    ContentAccess::CData& aContent )
    {
    RFile file;
    GetRFileFromCDataL( aContent, file );
    CleanupClosePushL( file );
    DlaLicenseAcquisitionL( file );
    CleanupStack::PopAndDestroy( &file );
    }

// -----------------------------------------------------------------------------
// CDrmUtilityWMDrmWrapper::DefaultAccessPointL
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmUtilityWMDrmWrapper::DefaultAccessPointL()
    {
    //Fetch default connection
    TUint32 iapd32( 0 );
    TCmDefConnValue defConn;
    RCmManager cmManager;
    cmManager.OpenLC();
    cmManager.ReadDefConnL(defConn);
    if (defConn.iType == ECmDefConnConnectionMethod)
        {
        iapd32=defConn.iId;
        }
    else if (defConn.iType == ECmDefConnDestination)
        {
        RCmDestination dest( cmManager.DestinationL( defConn.iId ) );
        CleanupClosePushL( dest );

        if ( dest.ConnectionMethodCount() <= 0 )
            {
            User::Leave( KErrNotFound );
            }

        RCmConnectionMethod cMeth( dest.ConnectionMethodL( 0 ) );
        CleanupClosePushL( cMeth );

        iapd32 = cMeth.GetIntAttributeL( CMManager::ECmIapId );
        CleanupStack::PopAndDestroy( 2, &dest ); //cMeth, dest
        }
    else
        {
        User::Leave( KErrAccessDenied );
        }
    CleanupStack::PopAndDestroy(&cmManager);
    // End of fetch default connection
    return iapd32;
    }

//  End of File
