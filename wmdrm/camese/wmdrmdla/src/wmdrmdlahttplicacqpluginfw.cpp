/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  HTTP plugin framework implementation
*
*/


#include <utf.h>
#include <wmdrmdlatypes.h>
#include <wmdrmdlahttpplugin.h>
#include <wmdrmdlauinotifier.h>
#include <wmdrmdlautils.rsg>
#include "wmdrmdlahttplicacqpluginfw.h"
#include "wmdrmdlahttpfwpluginresolver.h"
#include "wmdrmdlaparser.h"
#include "wmdrmdlaui.h"

#define _LOGGING_FILE L"wmdrmdla.txt"
#include "logfn.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::ConstructL()
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::ConstructL" );
    iDlaParser = CWmDrmDlaParser::NewL();
    iWait = new (ELeave) CActiveSchedulerWait;
    iIdle =  CIdle::NewL( CActive::EPriorityIdle );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::CWmDrmDlaHttpLicAcqPluginFw
// ---------------------------------------------------------------------------
//
CWmDrmDlaHttpLicAcqPluginFw::CWmDrmDlaHttpLicAcqPluginFw() 
    : CActive( EPriorityStandard ), 
      iState( CWmDrmDlaHttpLicAcqPluginFw::EIdle )
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::CWmDrmDlaHttpLicAcqPluginFw" );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::NewL
// ---------------------------------------------------------------------------
//
CWmDrmDlaHttpLicAcqPluginFw* CWmDrmDlaHttpLicAcqPluginFw::NewL()
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::NewL" );
    CWmDrmDlaHttpLicAcqPluginFw* self 
        = CWmDrmDlaHttpLicAcqPluginFw::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::NewLC
// ---------------------------------------------------------------------------
//
CWmDrmDlaHttpLicAcqPluginFw* CWmDrmDlaHttpLicAcqPluginFw::NewLC()
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::NewLC" );
    CWmDrmDlaHttpLicAcqPluginFw* self 
        = new( ELeave ) CWmDrmDlaHttpLicAcqPluginFw;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::~CWmDrmDlaHttpLicAcqPluginFw
// ---------------------------------------------------------------------------
//
CWmDrmDlaHttpLicAcqPluginFw::~CWmDrmDlaHttpLicAcqPluginFw()
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::~CWmDrmDlaHttpLicAcqPluginFw" );
    CancelLicenseAcquisition();
    delete iDlaParser;
    delete iHttpPlugin;
    delete iUiNotifier;
    delete iWait;
    delete iIdle;
    delete iDrmHeader;
    delete iLicenseServerUrl;
    delete iLicenseChallenge;
    delete iLicenseResponse;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::SilentL
// ---------------------------------------------------------------------------
//
TBool CWmDrmDlaHttpLicAcqPluginFw::SilentL( 
    const RFile& aFile )
    {
    TInt err( KErrNone );
    TBool silent( EFalse );
    
    LOGFNR( "CWmDrmDlaHttpLicAcqPluginFw::SilentL", err );
    if ( iState != CWmDrmDlaHttpLicAcqPluginFw::EIdle )
        {
        User::Leave( KErrInUse );
        }
    //Finds out, if current content should be handled silently, also
    //resolves and initializes plugin that handles this content
    TRAP( err, silent = SilentHandlerL( aFile ) );
    if ( err )
        {
        HandleErrorL( err );
        }
    iSilentInitialised = ETrue;
    return silent;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::AcquireLicense
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::AcquireLicense( 
    const RFile& aFile, 
    HBufC*& aContentUrl, 
    HBufC*& aHtmlData, 
    TRequestStatus& aStatus )
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::AcquireLicense" );
    
    TRequestStatus *status = &aStatus;
    aStatus = KRequestPending;
    if ( iState != CWmDrmDlaHttpLicAcqPluginFw::EIdle )
        {
        User::RequestComplete( status, KErrInUse );
        return;
        }
    
    iClientStatus = &aStatus;
    iFile = &aFile;
    iContentUrl = &aContentUrl;
    iHtmlData = &aHtmlData;
    //If FW is already initialized, then go straight to license acquisition.
    //Otherwise try to resolve plugin that handles this content.
    if ( iSilentInitialised )
        {
        iState = 
            CWmDrmDlaHttpLicAcqPluginFw::ERetrievingDRMHeaderFromFileHandle;
        }
    else
        {
        iState = 
            CWmDrmDlaHttpLicAcqPluginFw::EResolvingHttpPluginFromFileHandle;
        }
    CompleteSelf();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::AcquireLicenseFromDrmHeader
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::AcquireLicenseFromDrmHeader( 
    const TDesC8& aDrmHeader,
    HBufC* aErrorUrl,
    HBufC*& aContentUrl, 
    HBufC*& aHtmlData, 
    TRequestStatus& aStatus )
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::AcquireLicenseFromDrmHeader" );
    
    TRequestStatus *status = &aStatus;
    aStatus = KRequestPending;
    if ( iState != CWmDrmDlaHttpLicAcqPluginFw::EIdle )
        {
        User::RequestComplete( status, KErrInUse );
        return;
        }
    
    iClientStatus = &aStatus;
    delete iDrmHeader;
    iDrmHeader = NULL;
    iDrmHeader = aDrmHeader.Alloc();
    if ( !iDrmHeader )
        {
        CompleteClientRequest( KErrNoMemory );
        }
    else
        {
        iErrorUrl = aErrorUrl;
        iContentUrl = &aContentUrl;
        iHtmlData = &aHtmlData;
        iState = 
            CWmDrmDlaHttpLicAcqPluginFw::EResolvingHttpPluginFromDRMHeader;
        CompleteSelf();
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::SetIapId
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::SetIapId( TInt aIapId )
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::SetIapId" );
    LOG2( "aIapId: %d", aIapId );
    iIapId = aIapId;
    if ( iHttpPlugin )
        {
        iHttpPlugin->SetIapId( iIapId );
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::CancelLicenseAcquisition
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::CancelLicenseAcquisition()
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::CancelLicenseAcquisition" );
    LOG2( "iState: %d", iState );
    //If we are currently sending header error or license storage status 
    //to server, then let it finish before cancelling
    if ( iState == CWmDrmDlaHttpLicAcqPluginFw::ESentDRMHeaderError ||
         iState == CWmDrmDlaHttpLicAcqPluginFw::ESentLicenseStorageStatus )
        {
        iWait->Start();
        }
    if ( iHttpPlugin )
        {
        iHttpPlugin->CancelLicenseAcquisition();
        }
    Cancel();
    if ( iUiNotifier )
        {
        TRAP_IGNORE( iUiNotifier->HandleErrorL( KErrCancel ) );
        }
    CompleteClientRequest( KErrCancel );
    Reset();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::DoCancel
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::DoCancel()
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::DoCancel" );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::RunL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::RunL()
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::RunL" );
    LOG2( "iState: %d", iState );
    LOG2( "iStatus.Int(): %d", iStatus.Int() );
    if ( iStatus.Int() != KErrNone )
        {
        HandleErrorL( iStatus.Int() );
        }
    
    switch ( iState )
	    {
	    case CWmDrmDlaHttpLicAcqPluginFw::EResolvingHttpPluginFromDRMHeader:
	        ResolvingHttpPluginFromDRMHeaderHandlerL();
	        break;
	        
	    case CWmDrmDlaHttpLicAcqPluginFw::EResolvingHttpPluginFromFileHandle:
	        ResolvingHttpPluginFromFileHandleHandlerL();
	        break;
	        
    	case CWmDrmDlaHttpLicAcqPluginFw::ERetrievingDRMHeaderFromFileHandle:
    	    RetrievingDRMHeaderFromFileHandleHandler();
    	    break;
    	
    	case CWmDrmDlaHttpLicAcqPluginFw::EProcessingDRMHeader:
    	    ProcessingDRMHeaderHandler();
    	    break;

    	case CWmDrmDlaHttpLicAcqPluginFw::ESentDRMHeaderError:
    	    SentDRMHeaderErrorHandlerL();
    	    break;
    	    
    	case CWmDrmDlaHttpLicAcqPluginFw::ERetrievingChallenge:
    	    RetrievingChallengeHandler();
    	    break;
    	
    	case CWmDrmDlaHttpLicAcqPluginFw::ERetrievingLicense:
    	    RetrievingLicenseHandler();
    	    break;
    	
    	case CWmDrmDlaHttpLicAcqPluginFw::EAddingLicense:
    	    AddingLicenseHandlerL();
    	    break;
    	
    	case CWmDrmDlaHttpLicAcqPluginFw::ESendingLicenseStorageStatus:
    	    SendingLicenseStorageStatusHandler();
    	    break;
    	
    	case CWmDrmDlaHttpLicAcqPluginFw::ESentLicenseStorageStatus:
    	    SentLicenseStorageStatusHandler();
    	    break;
    	    
    	case CWmDrmDlaHttpLicAcqPluginFw::ELicensingComplete:
    	    LicensingCompleteL();
    	    break;
    	
    	default:
            ASSERT( EFalse );
    	    break;
    	}
    }    

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::RunError
// ---------------------------------------------------------------------------
//
TInt CWmDrmDlaHttpLicAcqPluginFw::RunError( TInt aError )
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::RunError" );
    LOG2( "aError: %d", aError );
    CompleteClientRequest( aError );
    Reset();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::UserCancellation
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::UserCancellation()
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::UserCancellation" );
    //User has cancelled the license acquisition from the UI.
    //Cancel the license acquisition asynchronously.
    TCallBack callBack( CWmDrmDlaHttpLicAcqPluginFw::AsyncCancel, 
                        this );
    iIdle->Start( callBack );
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::CompleteSelf
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::CompleteSelf()
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::CompleteSelf" );
    if ( !IsActive() )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::CompleteClientRequest
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::CompleteClientRequest( TInt aError )
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::CompleteClientRequest" );
    LOG2( "aError: %d", aError );
    if ( iClientStatus )
        {
        User::RequestComplete( iClientStatus, aError );
        iClientStatus = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::::Reset
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::Reset()
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::Reset" );
    iSilentInitialised = EFalse;
    delete iHttpPlugin;
    iHttpPlugin = NULL;
    delete iUiNotifier;
    iUiNotifier = NULL;
    delete iDrmHeader;
    iDrmHeader = NULL;
    delete iLicenseServerUrl;
    iLicenseServerUrl = NULL;
    delete iLicenseChallenge;
    iLicenseChallenge = NULL;
    delete iLicenseResponse;
    iLicenseResponse = NULL;
    iFile = NULL;
    iContentUrl = NULL;
    iHtmlData = NULL;
    iClientStatus = NULL;
    iState = CWmDrmDlaHttpLicAcqPluginFw::EIdle;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::SilentHandlerL
// ----------------------------------------------------------------------------
//
TBool CWmDrmDlaHttpLicAcqPluginFw::SilentHandlerL(
    const RFile& aFile )
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::SilentHandlerL" );
    iSilentInitialised = EFalse;
    ResolveHttpPluginL( aFile );
    return iHttpPlugin->SilentL( aFile );
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::ResolvingHttpPluginFromDRMHeaderHandlerL
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::ResolvingHttpPluginFromDRMHeaderHandlerL()
    {
    TInt err( KErrNone );
       
    LOGFNR( "CWmDrmDlaHttpLicAcqPluginFw::ResolvingHttpPluginFromDRMHeaderHandlerL", err );
    
    iSilentInitialised = EFalse;
    TRAP( err, ResolveHttpPluginL( *iDrmHeader ) );
    if ( err )
        {
        HandleErrorL( err );
        }
    iState = CWmDrmDlaHttpLicAcqPluginFw::EProcessingDRMHeader;
    CompleteSelf();
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::ResolvingHttpPluginFromFileHandleHandlerL
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::ResolvingHttpPluginFromFileHandleHandlerL()
    {
    TInt err( KErrNone );
    
    LOGFNR( "CWmDrmDlaHttpLicAcqPluginFw::ResolvingHttpPluginFromFileHandleHandlerL", err );
    
    TRAP( err, SilentHandlerL( *iFile ) );
    if ( err )
        {
        HandleErrorL( err );
        }
    iState = CWmDrmDlaHttpLicAcqPluginFw::ERetrievingDRMHeaderFromFileHandle;
    CompleteSelf();
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::RetrievingDRMHeaderFromFileHandleHandler
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::RetrievingDRMHeaderFromFileHandleHandler()
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::RetrievingDRMHeaderFromFileHandleHandler" );
    delete iDrmHeader;
    iDrmHeader = NULL;
    SetActive();
    iState = CWmDrmDlaHttpLicAcqPluginFw::EProcessingDRMHeader;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNotSupported );
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::ProcessingDRMHeaderHandler
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::ProcessingDRMHeaderHandler()
    {
    TInt err( KErrNone );
    
    LOGFNR( "CWmDrmDlaHttpLicAcqPluginFw::ProcessingDRMHeaderHandler", err );
    
    TRAP( err, iHttpPlugin->ProcessDrmHeaderL( *iDrmHeader ) );
    if ( err )
        {
        SendingDRMHeaderErrorHandler( err );
        }
    delete iLicenseServerUrl;
    iLicenseServerUrl = NULL;
    TRAP( err, ParseDrmHeaderL( *iDrmHeader, iLicenseServerUrl ) );
    if ( err )
        {
        SendingDRMHeaderErrorHandler( err );
        }
    iState = CWmDrmDlaHttpLicAcqPluginFw::ERetrievingChallenge;
    CompleteSelf();
    }        

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::SendingDRMHeaderErrorHandler
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::SendingDRMHeaderErrorHandler( 
    TInt aError )
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::SendingDRMHeaderErrorHandler" );
    LOG2( "aError: %d", aError );
    iHttpPlugin->SendDrmHeaderError( *iErrorUrl, aError, iStatus );
    SetActive();
    iState = CWmDrmDlaHttpLicAcqPluginFw::ESentDRMHeaderError;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::SentDRMHeaderErrorHandlerL
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::SentDRMHeaderErrorHandlerL()
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::SentDRMHeaderErrorHandlerL" );
    //If license acquisition is cancelled during the send operation,
    //then don't continue license acquisition process
    if ( iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    else
        {
        HandleErrorL( KErrCorrupt );
        }
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::RetrievingChallengeHandler
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::RetrievingChallengeHandler()
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::RetrievingChallengeHandler" );
    delete iLicenseChallenge;
    iLicenseChallenge = NULL;
    SetActive();
    iState = CWmDrmDlaHttpLicAcqPluginFw::ERetrievingLicense;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNotSupported );
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::RetrievingLicenseHandler
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::RetrievingLicenseHandler()
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::RetrievingLicenseHandler" );
    iHttpPlugin->AcquireLicense( *iLicenseChallenge, 
                                 iLicenseServerUrl, 
                                 iStatus );
    SetActive();
    iState = CWmDrmDlaHttpLicAcqPluginFw::EAddingLicense;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::AddingLicenseHandlerL
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::AddingLicenseHandlerL()
    {
    TInt err( KErrNone );
    
    LOGFNR( "CWmDrmDlaHttpLicAcqPluginFw::AddingLicenseHandler", err );
    
    delete iLicenseResponse;
    iLicenseResponse = NULL;
    TRAP( err, iLicenseResponse = iHttpPlugin->GetLicenseResponseL() );
    if ( err )
        {
        HandleErrorL( err );
        }
    SetActive();
    iState = CWmDrmDlaHttpLicAcqPluginFw::ESendingLicenseStorageStatus;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNotSupported );
    }    
    
// ----------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::SendingLicenseStorageStatusHandler
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::SendingLicenseStorageStatusHandler()
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::SendingLicenseStorageStatusHandler" );
    LOG2( "iStatus.Int(): %d", iStatus.Int() );
    HBufC8* tid( NULL );
    TRAP_IGNORE( ParseLicenseResponseL( *iLicenseResponse, 
                                        tid, 
                                        *iContentUrl ) );
    //If license couldn't be added succesfully to license store,
    //then make sure content won't be downloaded
    if ( iStatus.Int() )
        {
        delete *iContentUrl;
        *iContentUrl = NULL;
        }
    iHttpPlugin->SendLicenseStorageStatus( tid, 
                                           *iLicenseServerUrl, 
                                           iStatus.Int(), 
                                           iStatus );
    delete tid;
    SetActive();
    iState = CWmDrmDlaHttpLicAcqPluginFw::ESentLicenseStorageStatus;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::SentLicenseStorageStatusHandler
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::SentLicenseStorageStatusHandler()
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::SentLicenseStorageStatusHandler" );
    //If license acquisition is cancelled during the send operation,
    //then don't continue license acquisition process
    if ( iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    else
        {
        iState = CWmDrmDlaHttpLicAcqPluginFw::ELicensingComplete;
        CompleteSelf();
        }
    }


// ----------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::LicensingCompleteL
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::LicensingCompleteL()
    {
    TInt err( KErrNone );
    
    LOGFNR( "CWmDrmDlaHttpLicAcqPluginFw::LicensingComplete", err );
    
    //If content URL couldn't be parsed from the license response,
    //then try to get it from the http plugin
    if ( !*iContentUrl )
        {
        TRAP( err, *iContentUrl = iHttpPlugin->GetContentUrlL() );
        if ( err )
            {
            HandleErrorL( err );
            }
        }
    TRAP( err, *iHtmlData = iHttpPlugin->GetHtmlDataL() );
    if ( err )
        {
        HandleErrorL( err );
        }
    CompleteClientRequest( KErrNone );
    Reset();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::ResolveHttpPluginL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::ResolveHttpPluginL( 
    const RFile& aFile )
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::ResolveHttpPluginL" );

    delete iHttpPlugin;
    iHttpPlugin = NULL;
    delete iUiNotifier;
    iUiNotifier = NULL;
    
    WmDrmDlaHttpFwPluginResolver::ResolveHttpPluginL( aFile, 
                                                      iHttpPlugin, 
                                                      iUiNotifier );
    
    //Set cancel observer for ui plugin
    if ( iUiNotifier )
        {
        iUiNotifier->SetCancelObserver( this );
        }
    //Set iap for http plugin
    if ( iIapId )
        {
        iHttpPlugin->SetIapId( iIapId );
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::ResolveHttpPluginL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::ResolveHttpPluginL( 
    const TDesC8& aDrmHeader )
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::ResolveHttpPluginL" );
    
    delete iHttpPlugin;
    iHttpPlugin = NULL;
    delete iUiNotifier;
    iUiNotifier = NULL;
    
    WmDrmDlaHttpFwPluginResolver::ResolveHttpPluginL( aDrmHeader, 
                                                      iHttpPlugin, 
                                                      iUiNotifier );
    
    //Set cancel observer for ui plugin
    if ( iUiNotifier )
        {
        iUiNotifier->SetCancelObserver( this );
        }
    //Set iap for http plugin
    if ( iIapId )
        {
        iHttpPlugin->SetIapId( iIapId );
        }
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::ParseDrmHeaderL
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::ParseDrmHeaderL( 
    const TDesC8& aDrmHeader,
    HBufC*& aLicenseServerUrl )
    {
    TInt err( KErrNone );
    
    LOGFNR( "CWmDrmDlaHttpLicAcqPluginFw::ParseDrmHeader", err );
    
    HBufC8* licenseServerUrl( NULL );
    err = iDlaParser->GetLicenseServerURLFromDRMHeader( aDrmHeader, 
                                                        licenseServerUrl );
    if ( licenseServerUrl )
        {
        CleanupStack::PushL( licenseServerUrl );
        aLicenseServerUrl 
            = CnvUtfConverter::ConvertToUnicodeFromUtf8L( *licenseServerUrl );
        CleanupStack::PopAndDestroy( licenseServerUrl );
        }
    User::LeaveIfError( err );
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::ParseLicenseResponseL
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::ParseLicenseResponseL( 
    const TDesC8& aLicenseResponse,
    HBufC8*& aTid,
    HBufC*& aContentUrl )
    {
    TInt err( KErrNone );
    
    LOGFNR( "CWmDrmDlaHttpLicAcqPluginFw::ParseLicenseResponse", err );
        
    HBufC8* contentUrl( NULL );
    err = iDlaParser->ProcessLicenseResponse( aLicenseResponse, 
                                              aTid, 
                                              contentUrl );
    if ( contentUrl )
        {
        CleanupStack::PushL( contentUrl );
        aContentUrl 
            = CnvUtfConverter::ConvertToUnicodeFromUtf8L( *contentUrl );
        CleanupStack::PopAndDestroy( contentUrl );
        }
    User::LeaveIfError( err );
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::HandleErrorL
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpLicAcqPluginFw::HandleErrorL( TInt aError )
    {
    TInt err( KErrNotFound );
    
    LOGFNR( "CWmDrmDlaHttpLicAcqPluginFw::HandleErrorL", err );
    LOG2( "iState: %d", iState );
    LOG2( "aError: %d", aError );
    
    //First try to let ui plugin to handle the error
    if ( iUiNotifier )
        {
        TRAP( err, iUiNotifier->HandleErrorL( aError ) );
        }
    //If ui plugin didn't handle the error and we aren't currently sending
    //license storage status to server, then try to show the generic
    //error note.
    if ( err && 
         iState != CWmDrmDlaHttpLicAcqPluginFw::ESendingLicenseStorageStatus )
        {
        CWmDrmDlaUi* ui( NULL );
        TRAP_IGNORE( ui = CWmDrmDlaUi::NewL() );
        if ( ui )
            {
            CleanupStack::PushL( ui );
            ui->ShowErrorNoteL( R_WMDRMDLA_NOTE_AQUISITION_FAILED );
            CleanupStack::PopAndDestroy( ui );
            }
        }
    //If we aren't currently sending license storage status to server,
    //the leave with the error code.
    if ( iState != CWmDrmDlaHttpLicAcqPluginFw::ESendingLicenseStorageStatus )
        {
        User::Leave( aError );
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpLicAcqPluginFw::AsyncCancel
// ---------------------------------------------------------------------------
//
TInt CWmDrmDlaHttpLicAcqPluginFw::AsyncCancel( TAny* aPtr )
    {
    LOGFN( "CWmDrmDlaHttpLicAcqPluginFw::AsyncCancel" );
    CWmDrmDlaHttpLicAcqPluginFw* licAcqFw = 
        static_cast<CWmDrmDlaHttpLicAcqPluginFw*>( aPtr );
    licAcqFw->CancelLicenseAcquisition();
    return EFalse;
    }
