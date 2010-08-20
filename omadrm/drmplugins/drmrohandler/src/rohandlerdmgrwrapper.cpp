/*
* Copyright (c) 2008 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <cmconnectionmethod.h>
#include <cmdestination.h>
#include <cmconnectionmethoddef.h>
#include <cmmanager.h>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include <data_caging_path_literals.hrh>

#include <es_enum.h> // tconnectioninfo
#include <es_sock.h> // rconnection rsocket
#include <RoapEng.h>
#include <RoapDef.h>
#include <RoapObserver.h>
#include "RoapSyncWrapper.h"

// Download manager apis
#include <qobject.h>
#include <downloadmanager.h>
#include <download.h>

#include "rohandlerdmgrwrapper.h"
#include "cleanupresetanddestroy.h"
#include "buffercontainers.h"
#include "qrohandlerdmgreventhandler.h"

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
    _LIT( KMethDoDownloadAndHandleRoapTriggerL,
        "DoDownloadAndHandleRoapTriggerL" );
    _LIT( KFormatDoDlHdlRoapTrigL, "DoDownloadAndHandleRoapTriggerL: %S" );
    _LIT( KStrDlCreated, "download created" );
    _LIT( KStrDlFinished, "download finished" );

    _LIT( KMethSetDefaultAccessPointL, "SetDefaultAccessPointL" );

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

    _LIT( KMiState, "iState" );

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

// CONSTANTS
#ifndef RD_MULTIPLE_DRIVE
_LIT( KHelperTriggerFilePath, "d:\\" );
#endif

using namespace WRT;

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
    try
		{
		QString roHandlerDmgrWrapperUid(QString::number(KRoHandlerDMgrWrapperUid));
		iDlMgr = q_check_ptr(new DownloadManager(roHandlerDmgrWrapperUid));
		}
    catch(const std::exception& exception)
    	{
		qt_symbian_exception2LeaveL(exception);
    	}
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

    delete iDlMgr;
    delete iRoHandlerDMgrEventHandler;
    
    iFs.Close();
    
    }

// ---------------------------------------------------------------------------
// CRoHandlerDMgrWrapper::HandleRoapTriggerL
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
// CRoHandlerDMgrWrapper::DoDownloadRoapTriggerL
// ---------------------------------------------------------------------------
//
void CRoHandlerDMgrWrapper::DoDownloadRoapTriggerL( TMeterState aNextState )
    {
    RFile roapTrigger;
    DRM::CFileNameContainer* triggerFileName(NULL);

    DRMDEBUGMETHOD( RoHdlrDMgrWrDebugLiterals::KMethDoDownloadAndHandleRoapTriggerL() );
    // If no Trigger URL then nothing to download. So finish transaction
    if ( !iTriggerUrl || iTriggerUrl->Length() <= 0 )
        {
        Continue( EComplete, KErrNone );
        return;
        }

    triggerFileName=DRM::CFileNameContainer::NewLC();
#ifndef RD_MULTIPLE_DRIVE

    User::LeaveIfError( roapTrigger.Temp(
            iFs, KHelperTriggerFilePath, triggerFileName->iBuffer, EFileWrite ) );

#else //RD_MULTIPLE_DRIVE
    _LIT( KDrive, "%c:\\" );
    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRam, driveNumber );
    iFs.DriveToChar( driveNumber, driveLetter );

    DRM::CFileNameContainer*
        helperTriggerFilePath( DRM::CFileNameContainer::NewLC() );

    helperTriggerFilePath->iBuffer.Format( KDrive, ( TUint )driveLetter );

    User::LeaveIfError( roapTrigger.Temp( iFs, helperTriggerFilePath->iBuffer,
                triggerFileName->iBuffer, EFileWrite ) );
    CleanupStack::PopAndDestroy( helperTriggerFilePath );
    helperTriggerFilePath=NULL;

#endif
    UpdateBufferL< HBufC, TFileName >( iFileName, triggerFileName->iBuffer );
    CleanupStack::PopAndDestroy( triggerFileName );
    triggerFileName=NULL;

    try
		{
		// create and start download
        QString downloadUrl((QChar*)iTriggerUrl->Des().Ptr(),iTriggerUrl->Length());
        iDownload = iDlMgr->createDownload( downloadUrl );
		iRoHandlerDMgrEventHandler = q_check_ptr(new QRoHandlerDMgrEventHandler(*this, *iDownload));
		}
    catch(const std::exception& exception)
		{
		qt_symbian_exception2LeaveL(exception);
		}
    		
    CleanupClosePushL( roapTrigger );

    
	DRMDEBUG2(
		RoHdlrDMgrWrDebugLiterals::KFormatDoDlHdlRoapTrigL(),
		&RoHdlrDMgrWrDebugLiterals::KStrDlCreated() );
	iDownloadSuccess = EFalse;
	iConnectionError = EFalse;

	SetDefaultAccessPointL();
        
	try
		{
		RBuf fileName;
		User::LeaveIfError(fileName.Create(KMaxFileName));
		CleanupClosePushL(fileName);
		roapTrigger.Name(fileName);
		const QVariant& roapTriggerValue( QString((QChar*) fileName.Ptr(), fileName.Length()) );
		CleanupStack::PopAndDestroy(&fileName);
		iDownload->setAttribute(FileName,roapTriggerValue);
		const QVariant& val(ETrue);
		iDownload->setAttribute(ContentType, val);
		iDownload->start();
		}
	catch(const std::exception& exception)
		{
		qt_symbian_exception2LeaveL(exception);
		}
	
	// wait until download is finished
	iState = aNextState;
	TRequestStatus* status( &iStatus );
	*status = KRequestPending;
	SetActive();
        
    CleanupStack::PopAndDestroy( &roapTrigger );
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

    try
		{
		// Fetch download created in DoDownloadRoapTriggerL
		QString downloadUrl((QChar*)iTriggerUrl->Des().Ptr(),iTriggerUrl->Length());
		//uncomment
		iDownload = NULL; //iDlMgr->findDownload( downloadUrl );
		}
    catch(const std::exception& exception)
		{
		qt_symbian_exception2LeaveL(exception);
		} 
    
    // Delete trigger URL so that it is possible to check
    // whether or not meteringResponse has PrUrl.
    delete iTriggerUrl;
    iTriggerUrl = NULL;
    
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
    CleanupResetAndDestroyPushL( contentIds );

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
    DRMDEBUGMETHOD( RoHdlrDMgrWrDebugLiterals::KMethSetDefaultAccessPointL() );

    if ( iIapId )
        {
        QVariant iapId((unsigned long long)iIapId);
        iDlMgr->setAttribute(DefaultDestinationPath, iapId);
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
void CRoHandlerDMgrWrapper::HandleDownloadEventL( WRT::DownloadEvent* aEvent )
    {
    QString KDRMHelperMimeTypeROAPTrigger("application/vnd.oma.drm.roap-trigger+xml" );

    DRMDEBUGMETHOD( RoHdlrDMgrWrDebugLiterals::KMethHandleDMgrEventL() );
    DRMDEBUG3( RoHdlrDMgrWrDebugLiterals::KFormatMembValInt(),
            &RoHdlrDMgrWrDebugLiterals::KMiState(), aEvent->type() );
    
    if ( aEvent->type() == DownloadEvent::HeadersReceived )
        {
        DRMDEBUG2( RoHdlrDMgrWrDebugLiterals::KFormatMethHandleDMgrEventL(),
                &RoHdlrDMgrWrDebugLiterals::KStrEHttpContentTypeReceived() );
        // check received mimetype
        QString contentType = iDownload->attribute( ContentType).toString();
        if ( contentType.contains( KDRMHelperMimeTypeROAPTrigger )  == EFalse )
            {
            // ROAP trigger found, continue download
            iDownload->start();
            }
        else
            {
            // wrong MIME type?, stop download
            iDownloadSuccess = EFalse;
            iDownload->cancel();
            }
        CleanupStack::PopAndDestroy( &contentType );
        }

    if ( aEvent->type() == DownloadEvent::Created )
        {
        DRMDEBUG2( RoHdlrDMgrWrDebugLiterals::KFormatMethHandleDMgrEventL(),
                &RoHdlrDMgrWrDebugLiterals::KStrEHttpDlCreated() );
        }
    else
        if ( aEvent->type() == DownloadEvent::NetworkLoss )
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
		if ( aEvent->type() == DownloadEvent::InProgress )
			{
			DRMDEBUG2( RoHdlrDMgrWrDebugLiterals::KFormatMethHandleDMgrEventL(),
					&RoHdlrDMgrWrDebugLiterals::KStrEHttpDlInprogress() );
			}
	else
		if ( aEvent->type() == DownloadEvent::Completed )
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
		if ( aEvent->type() == DownloadEvent::Failed )
			{
			TInt32 err;

			DRMDEBUG2( RoHdlrDMgrWrDebugLiterals::KFormatMethHandleDMgrEventL(),
					&RoHdlrDMgrWrDebugLiterals::KStrEHttpDlFailed() );
			// store failure
			iDownloadSuccess = EFalse;
			err = iDownload->attribute(LastError ).toInt();
			DRMDEBUG2( RoHdlrDMgrWrDebugLiterals::KFormatEDlAttrErrorId(), err );

			if ( err == ConnectionFailed || err == TransactionFailed )
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

