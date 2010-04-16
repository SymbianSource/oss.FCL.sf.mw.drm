/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  rohandler wrapper for Download manager
 *
*/

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_enum_partner.h>
#endif
#include <centralrepository.h>
#include <cdblen.h>

#ifdef __SERIES60_NATIVE_BROWSER
#include <BrowserUiSDKCRKeys.h>
#endif

#include <cmconnectionmethod.h>
#include <cmdestination.h>
#include <cmconnectionmethoddef.h>
#include <cmmanager.h>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include <data_caging_path_literals.hrh>

#include <DownloadMgrClient.h>

#include <es_enum.h> // tconnectioninfo
#include <es_sock.h> // rconnection rsocket
#include <RoapEng.h>
#include <RoapDef.h>
#include <RoapObserver.h>
#include "RoapSyncWrapper.h"

#include "rohandlerdmgrwrapper.h"

#ifdef _DEBUG
#define DRMDEBUG( a ) RDebug::Print( a )
#define DRMDEBUG2( a, b ) RDebug::Print( a, b )
#define DRMDEBUG3( a, b, c ) RDebug::Print( a, b, c )

#define DRMDEBUGMETHOD( a ) RDebug::Print( \
    RoHdlrDMgrWrDebugLiterals::KMethodFormat(), &( a ) )
// #define DRMDEBUGMETHODSTART( a ) RDebug::Print( \
// RoHdlrDMgrWrDebugLiterals::KMethodStartFormat(), &( a ) )

// #define DRMDEBUGMETHODFINISH( a ) RDebug::Print( \
// RoHdlrDMgrWrDebugLiterals::KMethodFinishFormat(), &( a ) )

// #define LOG( a ) RFileLogger::Write( \
// KRoLogDir(), KRoLogFile(), EFileLoggingModeAppend, a );
// #define LOGHEX( ptr, len ) RFileLogger::HexDump( \
//  KRoLogDir(), KRoLogFile(), EFileLoggingModeAppend, \
//  _S( "" ), _S( "" ), ptr, len );
// #define LOG2( a, b ) RFileLogger::WriteFormat( \
// KRoLogDir(), KRoLogFile(), EFileLoggingModeAppend, a, b );


namespace RoHdlrDMgrWrDebugLiterals
    {
    // Uncomment following literals if using macros LOG, LOG2 or LOGHEX anywhere
    //    _LIT( KRoLogDir, "DRM" );
    //    _LIT( KRoLogFile, "RoHdlDmgrWrapper.log" );

    // method Formatters ( used in macros DRMDEBUGMETHOD )
    _LIT( KMethodFormat, "CRoHandlerDMgrWrapper::%S" );

    // method Formatters ( used in macro DRMDEBUGMETHODSTART )
    //    _LIT( KMethodStartFormat, "CRoHandlerDMgrWrapper::%S -->" );

    // method Formatters ( used in macro DRMDEBUGMETHODFINISH )
    //    _LIT( KMethodFinishFormat, "--> CRoHandlerDMgrWrapper::%S" );


    _LIT( KFormatMembValInt, "%S = %d" );

    //Constructors, destructor
    _LIT( KMethDestructor, "~CRoHandlerDMgrWrapper" );
    //Methods
    _LIT( KMethConstructL, "ConstructL" );
    _LIT( KMethNewL, "NewL" );
    _LIT( KMethNewLC, "NewLC" );
    _LIT( KMethDownloadAndHandleRoapTriggerL, "DownloadAndHandleRoapTriggerL" );
    _LIT( KMethDownloadAndHandleRoapTriggerFromPrUrlL,
        "DownloadAndHandleRoapTriggerFromPrUrlL" );
    _LIT( KMethDoDownloadAndHandleRoapTriggerL,
        "DoDownloadAndHandleRoapTriggerL" );
    _LIT( KFormatDoDlHdlRoapTrigL, "DoDownloadAndHandleRoapTriggerL: %S" );
    _LIT( KStrDlCreated, "download created" );
    _LIT( KStrDlFinished, "download finished" );

    _LIT( KMethSetDefaultAccessPointL, "SetDefaultAccessPointL" );
    _LIT( KMiIapId, "iIapId" );

    _LIT( KMethHandleDMgrEventL, "HandleDMgrEventL" );
    _LIT( KFormatMethHandleDMgrEventL, "HandleDMgrEventL %S" );
    _LIT( KStrEHttpDlCreated, "EHttpDlCreated" );
    _LIT( KStrEHttpContentTypeReceived, "EHttpContentTypeReceived" );
    _LIT( KStrEHttpProgDisconnected, "EHttpProgDisconnected" );
    _LIT( KStrEHttpDlInprogress, "EHttpDlInprogress" );
    _LIT( KStrEHttpDlCompleted, "EHttpDlCompleted" );
    _LIT( KStrEHttpDlFailed, "EHttpDlFailed" );
    _LIT( KStrEConnectionFailed, "EConnectionFailed" );
    _LIT( KFormatEDlAttrErrorId, "EDlAttrErrorId = %d" );

    _LIT( KMiDownLoadState, "iDownLoadState" );
    _LIT( KMiProgressState, "iProgressState" );

    }

#else
#define DRMDEBUG( a )
#define DRMDEBUG2( a, b )
#define DRMDEBUG3( a, b, c )

#define DRMDEBUGMETHOD( a )
//#define DRMDEBUGMETHODSTART( a )
//#define DRMDEBUGMETHODFINISH( a )

//#define LOG( a )
//#define LOGHEX( ptr, len )
//#define LOG2( a, b )
#endif

#ifndef __SERIES60_NATIVE_BROWSER
const TUid KCRUidBrowser =
    {0x10008D39};
const TUint32 KBrowserDefaultAccessPoint = 0x0000000E;
const TUint32 KBrowserAccessPointSelectionMode = 0x0000001E;
const TUint32 KBrowserNGDefaultSnapId = 0x00000053;
#endif

// CONSTANTS
#ifndef RD_MULTIPLE_DRIVE
_LIT( KHelperTriggerFilePath, "d:\\" );
#endif

// ============================== LOCAL FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// DoResetAndDestroy
// Does RPointerArray< typename >->ResetAndDestroy() for the given array aPtr.
// ---------------------------------------------------------------------------
//
template< typename elemType >
LOCAL_C void DoResetAndDestroy( TAny* aPtr )
    {
    ( reinterpret_cast< RPointerArray< elemType >* >( aPtr ) )->
        ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// DeleteHttpDowload
// ---------------------------------------------------------------------------
//
LOCAL_C void DeleteHttpDowload( TAny* aDownload )
    {
    reinterpret_cast< RHttpDownload* >( aDownload )->Delete();
    }

// ---------------------------------------------------------------------------
// UpdateBufferL
// ---------------------------------------------------------------------------
//
template< typename bufType, typename descType >
LOCAL_C void UpdateBufferL( bufType*& aTargetBuf, const descType& aSourceBuf )
    {
    if ( aTargetBuf )
        {
        delete aTargetBuf;
        aTargetBuf = NULL;
        }
    if ( aSourceBuf.Length() )
        {
        aTargetBuf = aSourceBuf.AllocL();
        }
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


// ============================= MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// CRoHandlerDMgrWrapper::CRoHandlerDMgrWrapper
// ---------------------------------------------------------------------------
//
CRoHandlerDMgrWrapper::CRoHandlerDMgrWrapper() :
    CActive( CActive::EPriorityStandard ),
    iIapId( 0 ), iState( EInit )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CRoHandlerDMgrWrapper::ConstructL
// ---------------------------------------------------------------------------
//
void CRoHandlerDMgrWrapper::ConstructL()
    {
    DRMDEBUGMETHOD( RoHdlrDMgrWrDebugLiterals::KMethConstructL() );
    // Get UID from process
    const TInt KRoHandlerDMgrWrapperUid = 0x101F7B92;
    iDlMgr.ConnectL( TUid::Uid( KRoHandlerDMgrWrapperUid ), *this, EFalse );
    User::LeaveIfError( iFs.Connect() );
    User::LeaveIfError( iFs.ShareProtected() );

    }

// ---------------------------------------------------------------------------
// CRoHandlerDMgrWrapper::NewL
// ---------------------------------------------------------------------------
//
CRoHandlerDMgrWrapper* CRoHandlerDMgrWrapper::NewL()
    {
    DRMDEBUGMETHOD( RoHdlrDMgrWrDebugLiterals::KMethNewL() );
    CRoHandlerDMgrWrapper* self( CRoHandlerDMgrWrapper::NewLC() );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CRoHandlerDMgrWrapper::NewLC
// ---------------------------------------------------------------------------
//
CRoHandlerDMgrWrapper* CRoHandlerDMgrWrapper::NewLC()
    {
    DRMDEBUGMETHOD( RoHdlrDMgrWrDebugLiterals::KMethNewLC() );
    CRoHandlerDMgrWrapper* self( new ( ELeave ) CRoHandlerDMgrWrapper() );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CRoHandlerDMgrWrapper::~CRoHandlerDMgrWrapper
// ---------------------------------------------------------------------------
//
CRoHandlerDMgrWrapper::~CRoHandlerDMgrWrapper()
    {
    Cancel();

    DRMDEBUGMETHOD( RoHdlrDMgrWrDebugLiterals::KMethDestructor() );
    delete iTriggerUrl;
    delete iTriggerBuf;
    delete iFileName;
    delete iRoapEng;

#ifdef _DEBUG

    if ( iDlMgr.Handle() )
        {
        iDlMgr.Close();
        }

#else

    iDlMgr.Close();

#endif

    iFs.Close();
    }

// ---------------------------------------------------------------------------
// CRoHandlerDMgrWrapper::DownloadAndHandleRoapTriggerL
// ---------------------------------------------------------------------------
//
void CRoHandlerDMgrWrapper::HandleRoapTriggerL( const TDesC8& aTrigger )
    {
    if ( iState != EInit || iWait.IsStarted() )
        {
        User::Leave( KErrNotReady );
        }

    UpdateBufferL< HBufC8, TDesC8 >( iTriggerBuf, aTrigger );
    Continue( EMeteringReportSubmit, KErrNone );
    iWait.Start();
    }

// ---------------------------------------------------------------------------
// CRoHandlerDMgrWrapper::DownloadAndHandleRoapTriggerL
// ---------------------------------------------------------------------------
//
void CRoHandlerDMgrWrapper::DownloadAndHandleRoapTriggerL( const HBufC8* aUrl )
    {
    DRMDEBUGMETHOD(
        RoHdlrDMgrWrDebugLiterals::KMethDownloadAndHandleRoapTriggerL() );
    if ( iState != EInit || iWait.IsStarted() )
        {
        User::Leave( KErrNotReady );
        }

    UpdateBufferL< HBufC8, TDesC8 >( iTriggerUrl, *aUrl );
    Continue( EGetMeteringTrigger, KErrNone );
    iWait.Start();
    }

// ---------------------------------------------------------------------------
// CRoHandlerDMgrWrapper::DownloadAndHandleRoapTriggerFromPrUrlL
// ---------------------------------------------------------------------------
//
void CRoHandlerDMgrWrapper::DownloadAndHandleRoapTriggerFromPrUrlL(
        const HBufC8* aUrl )
    {
    DRMDEBUGMETHOD( RoHdlrDMgrWrDebugLiterals::KMethDownloadAndHandleRoapTriggerFromPrUrlL() );
    if ( iState != EInit || iWait.IsStarted() )
        {
        User::Leave( KErrNotReady );
        }

    UpdateBufferL< HBufC8, TDesC8 >( iTriggerUrl, *aUrl );
    Continue( EGetPrUrlTrigger, KErrNone );
    iWait.Start();
    }

// ---------------------------------------------------------------------------
// CRoHandlerDMgrWrapper::DoDownloadRoapTriggerL
// ---------------------------------------------------------------------------
//
void CRoHandlerDMgrWrapper::DoDownloadRoapTriggerL( TMeterState aNextState )
    {
    RFile roapTrigger;
    TBool result( EFalse );
    TFileName triggerFileName;

    DRMDEBUGMETHOD( RoHdlrDMgrWrDebugLiterals::KMethDoDownloadAndHandleRoapTriggerL() );
    // If no Trigger URL then nothing to download. So finish transaction
    if ( !iTriggerUrl || iTriggerUrl->Length() <= 0 )
        {
        Continue( EComplete, KErrNone );
        return;
        }

#ifndef RD_MULTIPLE_DRIVE

    User::LeaveIfError( roapTrigger.Temp(
            iFs, KHelperTriggerFilePath, triggerFileName, EFileWrite ) );

#else //RD_MULTIPLE_DRIVE
    _LIT( KDrive, "%c:\\" );
    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRam, driveNumber );
    iFs.DriveToChar( driveNumber, driveLetter );

    TFileName helperTriggerFilePath;

    helperTriggerFilePath.Format( KDrive, ( TUint )driveLetter );

    User::LeaveIfError( roapTrigger.Temp( iFs, helperTriggerFilePath,
            triggerFileName, EFileWrite ) );

#endif
    UpdateBufferL< HBufC, TFileName >( iFileName, triggerFileName );

    // create and start download
    RHttpDownload& download = iDlMgr.CreateDownloadL( *iTriggerUrl, result );
    // Put download for proper cleanup.
    TCleanupItem item( DeleteHttpDowload, &download );
    CleanupStack::PushL( item );

    CleanupClosePushL( roapTrigger );

    if ( result )
        {
        DRMDEBUG2(
            RoHdlrDMgrWrDebugLiterals::KFormatDoDlHdlRoapTrigL(),
            &RoHdlrDMgrWrDebugLiterals::KStrDlCreated() );
        iDownloadSuccess = EFalse;
        iConnectionError = EFalse;

        SetDefaultAccessPointL();
        User::LeaveIfError( download.SetFileHandleAttribute( roapTrigger ) );
        User::LeaveIfError( download.SetBoolAttribute(
                EDlAttrNoContentTypeCheck, ETrue ) );
        User::LeaveIfError( download.Start() );

        // wait until download is finished
        iState = aNextState;
        TRequestStatus* status( &iStatus );
        *status = KRequestPending;
        SetActive();
        }
    CleanupStack::PopAndDestroy( &roapTrigger );
    CleanupStack::Pop( &download ); // Left open for DoSaveRoapTrigger
    }
// ---------------------------------------------------------------------------
// CRoHandlerDMgrWrapper::DoSaveRoapTriggerL
// ---------------------------------------------------------------------------
//
void CRoHandlerDMgrWrapper::DoSaveRoapTriggerL( TMeterState aNextState )
    {
    // Check success of download
    DRMDEBUG2(
        RoHdlrDMgrWrDebugLiterals::KFormatDoDlHdlRoapTrigL(),
        &RoHdlrDMgrWrDebugLiterals::KStrDlFinished() );

    // Fetch download created in DoDownloadRoapTriggerL
    RHttpDownload* download = iDlMgr.FindDownload( *iTriggerUrl, KNullDesC8() );
    // Delete trigger URL so that it is possible to check
    // whether or not meteringResponse has PrUrl.
    delete iTriggerUrl;
    iTriggerUrl = NULL;
    // Put download for proper cleanup.
    TCleanupItem item( DeleteHttpDowload, download );
    CleanupStack::PushL( item );
    RFile roapTrigger;

    if ( !iDownloadSuccess )
        {
            if ( iConnectionError )
                {
                User::Leave( KErrCouldNotConnect );
                }
            else
                {
                User::Leave( KErrGeneral );
                }
        }
    User::LeaveIfError( roapTrigger.Open( iFs, *iFileName, EFileShareReadersOrWriters ) );
    CleanupClosePushL( roapTrigger );
    // Get filehandle of ROAP trigger
    const TInt KReadBufSize = 512;

    RBuf8 readBuf;
    readBuf.CleanupClosePushL();
    readBuf.CreateL( KReadBufSize );

    // Read file to buffer
    TInt triggerFileSize( 0 );
    User::LeaveIfError( roapTrigger.Size( triggerFileSize ) );
    if ( iTriggerBuf )
        {
        delete iTriggerBuf;
        iTriggerBuf = NULL;
        }
    iTriggerBuf = HBufC8::NewL( triggerFileSize );
    User::LeaveIfError( roapTrigger.Read( readBuf, KReadBufSize ) );
    iTriggerBuf->Des().Copy( readBuf );
    while ( readBuf.Length() == KReadBufSize )
        {
        User::LeaveIfError( roapTrigger.Read( readBuf, KReadBufSize ) );
        iTriggerBuf->Des().Append( readBuf );
        }

    // And let ROAP handle it...
    CleanupStack::PopAndDestroy( &readBuf );
    CleanupStack::PopAndDestroy( &roapTrigger );
    CleanupStack::PopAndDestroy( download );

    iFs.Delete( *iFileName );
    delete iFileName;
    iFileName=NULL;
    Continue( aNextState, KErrNone );
    }

// ---------------------------------------------------------------------------
// CRoHandlerDMgrWrapper::DoHandleRoapTriggerL
// ---------------------------------------------------------------------------
//
void CRoHandlerDMgrWrapper::DoHandleRoapTriggerL( TMeterState aNextState )
    {
    Roap::TTriggerType triggerType;
    Roap::TRiContextStatus contextStatus;
    Roap::TDomainOperation domainOperation;

    RPointerArray< HBufC8 > contentIds;

    TCleanupItem cleanup( DoResetAndDestroy< HBufC8 >, &contentIds );
    CleanupStack::PushL( cleanup );

    iRoapEng = Roap::CRoapEng::NewL();

    iRoapEng->SetTriggerL( *iTriggerBuf, NULL, triggerType, contextStatus,
        domainOperation, contentIds );

    CleanupStack::PopAndDestroy( &contentIds );

    // if we have a valid RI context,
    // or if there is no user confirmation needed, do the ROAP
    if ( contextStatus != Roap::EInvalidContext )
        {
        iRoapEng->AcceptL( this, &iStatus );
        iState = aNextState;
        SetActive();
        }
    else
        {
        Continue( EComplete, KErrCancel );
        }
    }

// ---------------------------------------------------------------------------
// CRoHandlerDMgrWrapper::SetDefaultAccessPointL
// ---------------------------------------------------------------------------
//
void CRoHandlerDMgrWrapper::SetDefaultAccessPointL()
    {
    const TInt KDestinationSelectionMode( 2 );
    CRepository* repository( NULL );
    TInt ap( 0 );
    TInt alwaysAsk( 0 );
    TUint32 iapd32( 0 );
    TInt defaultSnap( 0 );
    TInt err( KErrNone );

    DRMDEBUGMETHOD( RoHdlrDMgrWrDebugLiterals::KMethSetDefaultAccessPointL() );

    if ( !iIapId )
        {
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
                TRAP( err, iapd32 = IapIdOfDefaultSnapL(
                        cmManager, defaultSnap ) );
                }
            CleanupStack::PopAndDestroy( &cmManager );
            }
        if ( !err && ( !alwaysAsk || alwaysAsk == KDestinationSelectionMode ) )
            {
            iIapId = iapd32;
            DRMDEBUG3( RoHdlrDMgrWrDebugLiterals::KFormatMembValInt(),
                &RoHdlrDMgrWrDebugLiterals::KMiIapId(), iIapId );
            err = iDlMgr.SetIntAttribute( EDlMgrIap, iapd32 );
            }
        CleanupStack::PopAndDestroy( repository );
        }
    else
        {
        err = iDlMgr.SetIntAttribute( EDlMgrIap, iIapId );
        }
    }

// ---------------------------------------------------------------------------
// CRoHandlerDMgrWrapper::Continue
// ---------------------------------------------------------------------------
//
void CRoHandlerDMgrWrapper::Continue(
    CRoHandlerDMgrWrapper::TMeterState aNextState, TInt aError )
    {
    iState = aNextState;
    TRequestStatus* ownStatus = &iStatus;
    *ownStatus = KRequestPending;
    SetActive();
    User::RequestComplete( ownStatus, aError );
    }


// MHttpDownloadMgrObserver methods

// ---------------------------------------------------------------------------
// CRoHandlerDMgrWrapper::HandleDMgrEventL
// ---------------------------------------------------------------------------
//
void CRoHandlerDMgrWrapper::HandleDMgrEventL( RHttpDownload& aDownload,
        THttpDownloadEvent aEvent )
    {
    _LIT8( KDRMHelperMimeTypeROAPTrigger, "application/vnd.oma.drm.roap-trigger+xml" );

    DRMDEBUGMETHOD( RoHdlrDMgrWrDebugLiterals::KMethHandleDMgrEventL() );
    DRMDEBUG3( RoHdlrDMgrWrDebugLiterals::KFormatMembValInt(),
            &RoHdlrDMgrWrDebugLiterals::KMiDownLoadState(), aEvent.iDownloadState );
    DRMDEBUG3( RoHdlrDMgrWrDebugLiterals::KFormatMembValInt(),
            &RoHdlrDMgrWrDebugLiterals::KMiProgressState(), aEvent.iProgressState );

    if ( aEvent.iProgressState == EHttpContentTypeReceived )
        {
        DRMDEBUG2( RoHdlrDMgrWrDebugLiterals::KFormatMethHandleDMgrEventL(),
                &RoHdlrDMgrWrDebugLiterals::KStrEHttpContentTypeReceived() );
        // check received mimetype
        RBuf8 contentType;
        contentType.CleanupClosePushL();
        contentType.CreateL( KMaxContentTypeLength );
        User::LeaveIfError( aDownload.GetStringAttribute( EDlAttrContentType,
                contentType ) );
        if ( !contentType.FindF( KDRMHelperMimeTypeROAPTrigger ) )
            {
            // ROAP trigger found, continue download
            User::LeaveIfError( aDownload.Start() );
            }
        else
            {
            // wrong MIME type?, stop download
            iDownloadSuccess = EFalse;
            User::LeaveIfError( aDownload.Delete() );
            }
        CleanupStack::PopAndDestroy( &contentType );
        }

    if ( aEvent.iDownloadState == EHttpDlCreated )
        {
        DRMDEBUG2( RoHdlrDMgrWrDebugLiterals::KFormatMethHandleDMgrEventL(),
                &RoHdlrDMgrWrDebugLiterals::KStrEHttpDlCreated() );
        }
    else
        if ( aEvent.iProgressState == EHttpProgDisconnected )
            {
            DRMDEBUG2( RoHdlrDMgrWrDebugLiterals::KFormatMethHandleDMgrEventL(),
                    &RoHdlrDMgrWrDebugLiterals::KStrEHttpProgDisconnected() );
            // store failure
            iDownloadSuccess = EFalse;
            iConnectionError = ETrue;
            // finished
            TRequestStatus* status( &iStatus );
            User::RequestComplete( status, KErrCancel );
            }
        else
            if ( aEvent.iDownloadState == EHttpDlInprogress )
                {
                DRMDEBUG2( RoHdlrDMgrWrDebugLiterals::KFormatMethHandleDMgrEventL(),
                        &RoHdlrDMgrWrDebugLiterals::KStrEHttpDlInprogress() );
                }
            else
                if ( aEvent.iDownloadState == EHttpDlCompleted )
                    {
                    // store success
                    DRMDEBUG2( RoHdlrDMgrWrDebugLiterals::KFormatMethHandleDMgrEventL(),
                            &RoHdlrDMgrWrDebugLiterals::KStrEHttpDlCompleted() );
                    iDownloadSuccess = ETrue;

                    // finished
                    TRequestStatus* status( &iStatus );
                    User::RequestComplete( status, KErrNone );
                    }
                else
                    if ( aEvent.iDownloadState == EHttpDlFailed )
                        {
                        TInt32 err;

                        DRMDEBUG2( RoHdlrDMgrWrDebugLiterals::KFormatMethHandleDMgrEventL(),
                                &RoHdlrDMgrWrDebugLiterals::KStrEHttpDlFailed() );
                        // store failure
                        iDownloadSuccess = EFalse;
                        User::LeaveIfError( aDownload.GetIntAttribute(
                                EDlAttrErrorId, err ) );
                        DRMDEBUG2( RoHdlrDMgrWrDebugLiterals::KFormatEDlAttrErrorId(), err );

                        if ( err == EConnectionFailed || err
                                == ETransactionFailed )
                            {
                            DRMDEBUG2( RoHdlrDMgrWrDebugLiterals::KFormatMethHandleDMgrEventL(),
                                    &RoHdlrDMgrWrDebugLiterals::KStrEConnectionFailed() );
                            iConnectionError = ETrue;
                            }

                        // finished
                        TRequestStatus* status( &iStatus );
                        User::RequestComplete( status, KErrCancel );
                        }
    }


// RoapObserver methods

// ---------------------------------------------------------------------------
// CRoHandlerDMgrWrapper::PostResponseUrlL
// ---------------------------------------------------------------------------
//
void CRoHandlerDMgrWrapper::PostResponseUrlL( const TDesC8& aPostResponseUrl )
    {
    UpdateBufferL< HBufC8, TDesC8 >( iTriggerUrl, aPostResponseUrl );

    if ( !iIapId )
        {
        // Take AP from open conenction
        RSocketServ socketServer;

        TInt err( KErrNone );

        err = socketServer.Connect();

        RConnection myConnection;

        err = myConnection.Open( socketServer );

        TUint connectionCount( 0 );

        err = myConnection.EnumerateConnections( connectionCount );

        if ( err != KErrNone || connectionCount < 1 )
            {
            return;
            }

        TPckgBuf<TConnectionInfoV2> connectionInfo;

        err = myConnection.GetConnectionInfo( connectionCount,
                connectionInfo );

        iIapId = connectionInfo().iIapId;

        myConnection.Close();
        socketServer.Close();
        }
    }

// Trivial RoapObserver methods
TBool CRoHandlerDMgrWrapper::ConnectionConfL()
    {
    return ETrue;
    }

TBool CRoHandlerDMgrWrapper::ContactRiConfL()
    {
    return ETrue;
    }

TBool CRoHandlerDMgrWrapper::TransIdConfL()
    {
    return EFalse;
    }

void CRoHandlerDMgrWrapper::RightsObjectDetailsL(
        const RPointerArray<CDRMRights>& /*aRightsList*/ )
    {
    // do nothing
    }

void CRoHandlerDMgrWrapper::ContentDownloadInfoL( TPath& /*aTempFolder*/,
        TFileName& /*aContentName*/, TInt& aMaxSize )
    {
    aMaxSize = -1;
    }

void CRoHandlerDMgrWrapper::ContentDetailsL( const TDesC& /*aPath*/,
        const TDesC8& /*aType*/, const TUid& /*aAppUid*/ )
    {
    }

void CRoHandlerDMgrWrapper::RoapProgressInfoL( const TInt /*aProgressInfo*/ )
    {
    // do nothing
    }

void CRoHandlerDMgrWrapper::ErrorUrlL( const TDesC8& /*aErrorUrl*/ )
    {
    // do nothing
    }



// CActive methods

// ---------------------------------------------------------------------------
// CRoHandlerDMgrWrapper::DoCancel
// ---------------------------------------------------------------------------
//
void CRoHandlerDMgrWrapper::DoCancel()
    {
    delete iRoapEng;
    iRoapEng = NULL;
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// CRoHandlerDMgrWrapper::RunL
// ---------------------------------------------------------------------------
//
void CRoHandlerDMgrWrapper::RunL()
    {
    User::LeaveIfError( iStatus.Int() );
    switch ( iState )
        {
        //case EInit:
        case EGetMeteringTrigger:
            {
            DoDownloadRoapTriggerL( ESaveMeteringTrigger );
            }
            break;
        case ESaveMeteringTrigger:
            {
            DoSaveRoapTriggerL( EMeteringReportSubmit );
            }
            break;

        case EMeteringReportSubmit:
            {
            DoHandleRoapTriggerL( EGetPrUrlTrigger );
            }
            break;
        case EGetPrUrlTrigger:
            {
            delete iRoapEng;
            iRoapEng = NULL;
            DoDownloadRoapTriggerL( ESavePrUrlTrigger );
            }
            break;
        case ESavePrUrlTrigger:
            {
            DoSaveRoapTriggerL( EPrRoapRequest );
            }
            break;
        case EPrRoapRequest:
            {
            DoHandleRoapTriggerL( EComplete );
            }
            break;
        case EComplete:
            {
            delete iRoapEng;
            iRoapEng = NULL;
            iWait.AsyncStop();
            }
            break;

        default:
            User::Leave( KErrNotSupported );
        }
    }

// ---------------------------------------------------------------------------
// CRoHandlerDMgrWrapper::RunError
// ---------------------------------------------------------------------------
//
TInt CRoHandlerDMgrWrapper::RunError( TInt /* aError */ )
    {
    //_LIT( KCatchedError, "Catched error" );
    //User::Panic( KCatchedError, aError );
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    return KErrNone;
    }
