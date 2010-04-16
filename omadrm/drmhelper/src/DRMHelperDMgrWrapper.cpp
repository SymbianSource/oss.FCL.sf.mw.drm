/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Dynamically loadable wrapper for Download manager
*
*/

#include <centralrepository.h>
#include <cdblen.h>

#include <cmconnectionmethod.h>
#include <cmdestination.h>
#include <cmconnectionmethoddef.h>
#include <cmmanager.h>

#ifdef __SERIES60_NATIVE_BROWSER
#include <BrowserUiSDKCRKeys.h>
#endif

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include <StringLoader.h>
#include <data_caging_path_literals.hrh>

#include <ConeResLoader.h>
#include <apparc.h>

#include <drmhelperdmgrwrapper.rsg>

#include "RoapEng.h"
#include "RoapSyncWrapper.h"
#include "RoapDef.h"
#include "DRMHelperDMgrWrapper.h"
#include "DRMHelperDMgrWrapperLogger.h"

// DEBUG macros
#ifdef _DEBUG
#define DRMDEBUGLIT( a, b ) \
_LIT( a , b )
#define DRMDEBUG( a ) \
RDebug::Print( a )
#define DRMDEBUG2( a, b ) \
RDebug::Print( a, b )
#else
#define DRMDEBUGLIT( a, b )
#define DRMDEBUG( a )
#define DRMDEBUG2( a, b )
#endif

// CONSTANTS
#ifndef __SERIES60_NATIVE_BROWSER
const TUid KCRUidBrowser = {0x10008D39};
const TUint32 KBrowserDefaultAccessPoint = 0x0000000E;
const TUint32 KBrowserAccessPointSelectionMode = 0x0000001E;
#endif

// CONSTANTS
#ifndef RD_MULTIPLE_DRIVE
_LIT( KDriveZ, "z:" );
_LIT( KHelperTriggerFilePath, "d:\\" );
#endif

_LIT( KCDRMHelperDMgrWrapperResFileName,"DRMHelperDMgrWrapper.rsc" );
const TInt KProgressInfoFinalValue( 200 );
const TInt KProgressInfoIncrementSmall( 5 );
const TInt KProgressInfoIncrementMedium( 10 );
const TInt KProgressInfoIncrementLarge( 30 );

// ======== LOCAL FUNCTIONS ========
LOCAL_C void DeleteHttpDowload( TAny* aDownload )
    {
    reinterpret_cast< RHttpDownload* >( aDownload )->Delete();
    }

// ---------------------------------------------------------------------------
// IapIdOfDefaultSnapL
// for trapping purposes only
// ---------------------------------------------------------------------------
//
LOCAL_C TUint32 IapIdOfDefaultSnapL(
    RCmManager& aCmManager,
    const TUint32 aDefaultSnap )
    {
    RCmDestination dest( aCmManager.DestinationL( aDefaultSnap ) );
    CleanupClosePushL( dest );
    TUint32 iapIdOfDest( 0 );

    if ( dest.ConnectionMethodCount() <= 0 )
        {
        User::Leave( KErrNotFound );
        }

    RCmConnectionMethod cMeth( dest.ConnectionMethodL( 0 ) );
    CleanupClosePushL( cMeth );

    iapIdOfDest = cMeth.GetIntAttributeL( CMManager::ECmIapId );
    CleanupStack::PopAndDestroy( &cMeth );
    CleanupStack::PopAndDestroy( &dest );
    return iapIdOfDest;
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDRMHelperDMgrWrapper::CDRMHelperDMgrWrapper
// ---------------------------------------------------------------------------
//
CDRMHelperDMgrWrapper::CDRMHelperDMgrWrapper() :
    iUseCoeEnv( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// CDRMHelperDMgrWrapper::ConstructL
// ---------------------------------------------------------------------------
//
void CDRMHelperDMgrWrapper::ConstructL()
    {
    CLOG_WRITE( "DMgrWrapper::ConstructL" );
    const TInt KDrmHelperDMgrWrapperUid = 0x102823D9;
    iDlMgr.ConnectL( TUid::Uid(KDrmHelperDMgrWrapperUid), *this, EFalse );
    iProgressInfo = NULL;
    iProgressNoteDialog = NULL;
    iDialogDismissed = ETrue;
    }



// ---------------------------------------------------------------------------
// CDRMHelperDMgrWrapper::NewL
// ---------------------------------------------------------------------------
//
CDRMHelperDMgrWrapper* CDRMHelperDMgrWrapper::NewL()
    {
    CLOG_WRITE( "DMgrWrapper::NewL" );
    CDRMHelperDMgrWrapper* self( CDRMHelperDMgrWrapper::NewLC() );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CDRMHelperDMgrWrapper::NewLC
// ---------------------------------------------------------------------------
//
CDRMHelperDMgrWrapper* CDRMHelperDMgrWrapper::NewLC()
    {
    CLOG_WRITE( "DMgrWrapper::NewLC" );
    CDRMHelperDMgrWrapper* self( new( ELeave ) CDRMHelperDMgrWrapper() );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CDRMHelperDMgrWrapper::~CDRMHelperDMgrWrapper
// ---------------------------------------------------------------------------
//
CDRMHelperDMgrWrapper::~CDRMHelperDMgrWrapper()
    {
    CLOG_WRITE( "DMgrWrapper destructor" );
    if ( iProgressNoteDialog )
       {
       // deletes the dialog
       TRAPD( err, iProgressNoteDialog->ProcessFinishedL() );
       if ( err )
           {
           delete iProgressNoteDialog;
           }
       iProgressNoteDialog = NULL;
       }
    delete iErrorUrl;
    delete iPostResponseUrl;

#ifdef _DEBUG

    if ( iDlMgr.Handle() )
        {
        iDlMgr.Close();
        }

#else

    iDlMgr.Close();

#endif

    }


// ---------------------------------------------------------------------------
// CDRMHelperDMgrWrapper::DownloadAndHandleRoapTriggerL
// ---------------------------------------------------------------------------
//
void CDRMHelperDMgrWrapper::DownloadAndHandleRoapTriggerL( const HBufC8* aUrl )
    {
    CLOG_WRITE( "DMgrWrapper::DownloadAndHandleRoapTriggerL" );
    iUseCoeEnv = EFalse;
    DoDownloadAndHandleRoapTriggerL( aUrl );
    HandlePostResponseUrlL();
    }


// ---------------------------------------------------------------------------
// CDRMHelperDMgrWrapper::DownloadAndHandleRoapTriggerL
// ---------------------------------------------------------------------------
//
void CDRMHelperDMgrWrapper::DownloadAndHandleRoapTriggerL(
    const HBufC8* aUrl, CCoeEnv& aCoeEnv )
    {
    CLOG_WRITE( "DMgrWrapper::DownloadAndHandleRoapTriggerL" );
    iCoeEnv = &aCoeEnv;
    iUseCoeEnv = ETrue;
    DoDownloadAndHandleRoapTriggerL( aUrl );
    HandlePostResponseUrlL();
    }


// ---------------------------------------------------------------------------
// CDRMHelperDMgrWrapper::HandlePostResponseUrlL
// ---------------------------------------------------------------------------
//
void CDRMHelperDMgrWrapper::HandlePostResponseUrlL()
    {
    if ( iPostResponseUrl )
        {
        DoDownloadAndHandleRoapTriggerL( iPostResponseUrl );
        // prevent infinite post response fetches.
        delete iPostResponseUrl;
        iPostResponseUrl = NULL;

        // Ensure progress note gets deleted.
        // It remains open if prUrl initialted ROAP operation has PrUrl
        // (unsupported chained metering report)
        RemoveProgressNoteL();
        }
    }


// ---------------------------------------------------------------------------
// CDRMHelperDMgrWrapper::DoDownloadAndHandleRoapTriggerL
// ---------------------------------------------------------------------------
//
void CDRMHelperDMgrWrapper::DoDownloadAndHandleRoapTriggerL(
    const HBufC8* aUrl )
    {
    RFs fs;
    RFile roapTrigger;
    HBufC8* triggerBuf( NULL );
    TBool result( EFalse );
    TFileName triggerFileName;

    CLOG_WRITE( "DMgrWrapper::DoDownloadAndHandleRoapTriggerL" );
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    User::LeaveIfError( fs.ShareProtected() );


#ifndef RD_MULTIPLE_DRIVE

    User::LeaveIfError( roapTrigger.Temp(
            fs, KHelperTriggerFilePath, triggerFileName, EFileWrite ) );

#else //RD_MULTIPLE_DRIVE

    _LIT( KDrive, "%c:\\");
    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRam, driveNumber );
    fs.DriveToChar( driveNumber, driveLetter );

    TFileName helperTriggerFilePath;

    helperTriggerFilePath.Format( KDrive, (TUint)driveLetter );

    User::LeaveIfError( roapTrigger.Temp(
            fs, helperTriggerFilePath, triggerFileName, EFileWrite ) );

#endif

    TPtrC8 KNullPtr8( NULL, 0 );
    RHttpDownload* downloadPtr( iDlMgr.FindDownload( *aUrl, KNullPtr8 ) );
    if ( downloadPtr )
        {
        // Stale download found.
        // Remove it, and re-create a new download.
        downloadPtr->Delete();
        downloadPtr = NULL;
        }

    // create and start download
    RHttpDownload& download = iDlMgr.CreateDownloadL( *aUrl, result );
    // Put download for proper cleanup.
    TCleanupItem item( DeleteHttpDowload, &download );
    CleanupStack::PushL( item );

    CleanupClosePushL( roapTrigger );

    if ( !iPostResponseUrl )
        {
        // No post response retieval. Note must be created.
        ShowProgressNoteL();
        }

    if ( result )
        {
        const TInt KReadBufSize( 512 );
        TInt triggerFileSize( 0 );

        CLOG_WRITE(
            "DMgrWrapper::DoDownloadAndHandleRoapTriggerL: download created" );
        iDownloadSuccess = EFalse;
        iConnectionError = EFalse;

        SetDefaultAccessPointL();

        User::LeaveIfError( download.SetFileHandleAttribute( roapTrigger ) );
        User::LeaveIfError(
            download.SetBoolAttribute( EDlAttrNoContentTypeCheck, ETrue ) );
        User::LeaveIfError( download.Start() );

        // wait until download is finished
        iWait.Start();

        // Check success of download
        CLOG_WRITE(
            "DMgrWrapper::DoDownloadAndHandleRoapTriggerL: download finished" );

        CleanupStack::Pop( &roapTrigger );
        roapTrigger.Close();
        if ( !iDownloadSuccess )
            {
            RemoveProgressNoteL();
            if ( iConnectionError )
                {
                User::Leave( KErrCouldNotConnect );
                }
            else
                {
                User::Leave( KErrGeneral );
                }
            }
        User::LeaveIfError( roapTrigger.Open( fs,
                                              triggerFileName,
                                              EFileShareReadersOrWriters ) );
        CleanupClosePushL( roapTrigger );

        // Get filehandle of ROAP trigger
        // Read file to buffer
        User::LeaveIfError( roapTrigger.Size( triggerFileSize ) );
        triggerBuf = HBufC8::NewLC( triggerFileSize );

        RBuf8 readBuf;
        readBuf.CleanupClosePushL();
        readBuf.CreateL( KReadBufSize );

        User::LeaveIfError( roapTrigger.Read( readBuf, KReadBufSize ) );
        triggerBuf->Des().Copy( readBuf );
        while ( readBuf.Length() == KReadBufSize )
            {
            User::LeaveIfError( roapTrigger.Read( readBuf, KReadBufSize ) );
            triggerBuf->Des().Append( readBuf );
            }

        CleanupStack::PopAndDestroy( &readBuf );

        if ( iUseCoeEnv && iProgressInfo )
            {
            iProgressInfo->IncrementAndDraw( KProgressInfoIncrementMedium );
            }

        // And let ROAP handle it...
        CRoapSyncWrapper* roapWrapper( CRoapSyncWrapper::NewL() );
        CleanupStack::PushL( roapWrapper );
        TRAPD( err, roapWrapper->HandleTriggerL( *triggerBuf ) );
        if ( err )
            {
            TInt errorType( 0 );
            TRAPD( err2, iErrorUrl =
                roapWrapper->GetErrorUrlL( err, errorType ) );
            if ( err2 )
                {
                RemoveProgressNoteL();
                delete iErrorUrl;
                iErrorUrl = NULL;
                User::Leave( err2 );
                }
            else if ( errorType != KErrRoapTemporary )
                {
                RemoveProgressNoteL();
                delete iErrorUrl;
                iErrorUrl = NULL;
                User::Leave( err );
                }
            else
                {
                RemoveProgressNoteL();
                User::Leave( err );
                }
            }
        if ( iPostResponseUrl )
            {
            delete iPostResponseUrl;
            iPostResponseUrl = NULL;
            }
        iPostResponseUrl = roapWrapper->GetPostResponseUrlL();
        CleanupStack::PopAndDestroy( 2, triggerBuf );

        if ( iUseCoeEnv && iProgressInfo && !iPostResponseUrl )
            {
            // No PrUrl found. Progess is complete.
            iProgressInfo->SetAndDraw( KProgressInfoFinalValue );
            }
        }

    // Trick to keep note open long enough during prUrl retrieval
    if ( !iPostResponseUrl )
        {
        RemoveProgressNoteL();
        }
    else
        {
        if ( iUseCoeEnv && iProgressInfo )
            {
            iProgressInfo->IncrementAndDraw( KProgressInfoIncrementMedium );
            }
        }

    CleanupStack::PopAndDestroy( &roapTrigger );
    CleanupStack::PopAndDestroy( &download );

    fs.Delete(triggerFileName);
    CleanupStack::PopAndDestroy(&fs);
    }


// ---------------------------------------------------------------------------
// CDRMHelperDMgrWrapper::SetDefaultAccessPointL
// ---------------------------------------------------------------------------
//
void CDRMHelperDMgrWrapper::SetDefaultAccessPointL()
    {
    const TInt KDestinationSelectionMode( 2 );
    CRepository* repository( NULL );
    TInt ap( 0 );
    TInt alwaysAsk( 0 );
    TUint32 iapd32( 0 );
    TInt defaultSnap( 0 );
    TInt err( KErrNone );

    CLOG_WRITE( "DMgrWrapper::SetDefaultAccessPointL" );
    repository = CRepository::NewL( KCRUidBrowser );
    CleanupStack::PushL( repository );
    repository->Get( KBrowserDefaultAccessPoint, ap );
    repository->Get( KBrowserAccessPointSelectionMode, alwaysAsk );
    repository->Get( KBrowserNGDefaultSnapId, defaultSnap );

    if ( ap <= KErrNotFound && defaultSnap <= KErrNotFound )
        {
        alwaysAsk = ETrue;
        }
    else
        {
        RCmManager cmManager;
        cmManager.OpenLC();
        if ( !alwaysAsk )
            {
            TRAP( err, iapd32 = cmManager.GetConnectionMethodInfoIntL(
                    ap, CMManager::ECmIapId ) );
            }
        else if ( alwaysAsk == KDestinationSelectionMode )
            {
            TRAP( err, iapd32 =
                IapIdOfDefaultSnapL( cmManager, defaultSnap ) );
            }
        CleanupStack::PopAndDestroy( &cmManager );
        }
    if ( !err && ( !alwaysAsk || alwaysAsk == KDestinationSelectionMode ) )
        {
        err = iDlMgr.SetIntAttribute( EDlMgrIap, iapd32 );
        }
    CleanupStack::PopAndDestroy( repository );
    }


// ---------------------------------------------------------------------------
// CDRMHelperDMgrWrapper::GetErrorUrlL
// ---------------------------------------------------------------------------
//
HBufC8* CDRMHelperDMgrWrapper::GetErrorUrlL()
    {
    if( iErrorUrl )
        {
        return iErrorUrl->AllocL();
        }
    return NULL;
    }


// ---------------------------------------------------------------------------
// From class MHttpDownloadMgrObserver
// ---------------------------------------------------------------------------
//
void CDRMHelperDMgrWrapper::HandleDMgrEventL(
    RHttpDownload& aDownload,
    THttpDownloadEvent aEvent )
    {
    _LIT8( KDRMHelperMimeTypeROAPTrigger,
        "application/vnd.oma.drm.roap-trigger+xml" );

    CLOG_WRITE( "DMgrWrapper::HandleDMgrEventL" );
    CLOG_WRITE_FORMAT( "iDownLoadState = %d", aEvent.iDownloadState );
    CLOG_WRITE_FORMAT( "iProgressState = %d", aEvent.iProgressState );

    if ( aEvent.iProgressState == EHttpContentTypeReceived )
        {
        // check received mimetype
        RBuf8 contentType;
        contentType.CleanupClosePushL();
        contentType.CreateL( KMaxContentTypeLength );
        User::LeaveIfError(
            aDownload.GetStringAttribute( EDlAttrContentType, contentType ) );
        if ( !contentType.FindF( KDRMHelperMimeTypeROAPTrigger ) )
            {
            // ROAP trigger found, continue download
            User::LeaveIfError( aDownload.Start() );
            }
        else
            {
            // wrong MIME type, so stop download
            iDownloadSuccess = EFalse;
            User::LeaveIfError( aDownload.Delete() );
            }
        CleanupStack::PopAndDestroy( &contentType );
        }

    if ( aEvent.iDownloadState == EHttpDlCreated )
        {
        CLOG_WRITE( "DMgrWrapper::HandleDMgrEventL: EHttpDlCreated" );
        if ( iUseCoeEnv )
            {
        iProgressInfo->IncrementAndDraw( KProgressInfoIncrementMedium );
            }
        }
    else if ( aEvent.iProgressState == EHttpProgDisconnected )
        {
        CLOG_WRITE( "DMgrWrapper::HandleDMgrEventL: EHttpProgDisconnected" );
        // store failure
        iDownloadSuccess = EFalse;
        iConnectionError = ETrue;
        // finished
        iWait.AsyncStop();
        }
    else if ( aEvent.iDownloadState == EHttpDlInprogress )
        {
        CLOG_WRITE( "DMgrWrapper::HandleDMgrEventL: EHttpDlInprogress" );
        if ( iUseCoeEnv )
            {
        iProgressInfo->IncrementAndDraw( KProgressInfoIncrementSmall );
            }
        }
    else if ( aEvent.iDownloadState == EHttpDlCompleted )
        {
        // store success
        CLOG_WRITE( "DMgrWrapper::HandleDMgrEventL: EHttpDlCompleted" );
        iDownloadSuccess = ETrue;
        if ( iUseCoeEnv )
            {
        iProgressInfo->IncrementAndDraw( KProgressInfoIncrementLarge );
            }
        // finished
        iWait.AsyncStop();
        }
    else if ( aEvent.iDownloadState == EHttpDlFailed )
        {
        TInt32 err( KErrNone );

        CLOG_WRITE( "DMgrWrapper::HandleDMgrEventL: EHttpDlFailed" );
        // store failure
        iDownloadSuccess = EFalse;
        User::LeaveIfError( aDownload.GetIntAttribute( EDlAttrErrorId, err ) );
        CLOG_WRITE_FORMAT( "EDlAttrErrorId = %d", err );

        if ( err == EConnectionFailed ||
             err == ETransactionFailed)
            {
            CLOG_WRITE( "DMgrWrapper::HandleDMgrEventL: EConnectionFailed" );
            iConnectionError = ETrue;
            }

        // finished
        iWait.AsyncStop();
        }
    }


// ---------------------------------------------------------------------------
// CDRMHelperDMgrWrapper::ShowProgressNoteL
// ---------------------------------------------------------------------------
//
void CDRMHelperDMgrWrapper::ShowProgressNoteL( )
    {
    TFileName resFileName;

    CLOG_WRITE( "DMgrWrapper::ShowProgressNoteL" );
    if ( iUseCoeEnv )
        {
        // Initialize the progress note dialog, it's values,
        // and execute it

#ifndef RD_MULTIPLE_DRIVE

        resFileName.Copy( KDriveZ );

#else //RD_MULTIPLE_DRIVE

        _LIT( KDrive, "%c:");
        TInt driveNumber( -1 );
        TChar driveLetter;
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, driveNumber );

        iCoeEnv->FsSession().DriveToChar( driveNumber, driveLetter );

        resFileName.Format( KDrive, (TUint)driveLetter );

#endif

        resFileName.Append( KDC_RESOURCE_FILES_DIR );
        resFileName.Append( KCDRMHelperDMgrWrapperResFileName );
        RConeResourceLoader loader( *iCoeEnv );
        loader.OpenL( resFileName );

        iProgressNoteDialog = new (ELeave) CAknProgressDialog(
            reinterpret_cast< CEikDialog** >( &iProgressNoteDialog ) );
        iProgressNoteDialog->PrepareLC( R_SILENT_PROGRESS_NOTE );
        iProgressNoteDialog->SetCallback( this );
        iProgressInfo = iProgressNoteDialog->GetProgressInfoL();
        iProgressInfo->SetFinalValue( KProgressInfoFinalValue );
        iDialogDismissed = EFalse;
        iProgressNoteDialog->RunLD();

        loader.Close();
        }

    }


// ---------------------------------------------------------------------------
// CDRMHelperDMgrWrapper::RemoveProgressNoteL
// ---------------------------------------------------------------------------
//
void CDRMHelperDMgrWrapper::RemoveProgressNoteL( )
    {

    if ( iUseCoeEnv )
        {
        if (iProgressNoteDialog && !iDialogDismissed)
            {
            // deletes the dialog
            TRAPD(err, iProgressNoteDialog->ProcessFinishedL());
            if (err != KErrNone)
                {
                delete iProgressNoteDialog;
                }
            iProgressNoteDialog = NULL;
            }
        }

    }


// ---------------------------------------------------------------------------
// From class MAknProgressDialogCallback
// ---------------------------------------------------------------------------
//
void CDRMHelperDMgrWrapper::DialogDismissedL( TInt /*aButtonId*/ )
    {
    iDialogDismissed = ETrue;

    // Already freed, just set to NULL
    iProgressNoteDialog = NULL;
    iProgressInfo = NULL;

    if( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }

    }


// ======== GLOBAL FUNCTIONS ========

//------------------------------------------------------------------------------
// GateFunctionDRM
// DRM gate function
//------------------------------------------------------------------------------
EXPORT_C TAny* GateFunctionDMgr()
    {
    CDRMHelperDMgrWrapper* launcher = NULL;
    TRAPD( err, launcher = CDRMHelperDMgrWrapper::NewL() );
    if( err != KErrNone )
        {
        return NULL;
        }

    return launcher;
    }

