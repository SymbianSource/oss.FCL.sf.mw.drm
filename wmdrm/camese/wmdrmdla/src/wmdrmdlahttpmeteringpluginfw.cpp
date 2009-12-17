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


#include <wmdrmdlatypes.h>
#include <wmdrmdlahttpplugin.h>
#include <wmdrmdlauinotifier.h>
#include "wmdrmdlahttpmeteringpluginfw.h"
#include "wmdrmdlahttpfwpluginresolver.h"
#include "wmdrmdlaui.h"

#define _LOGGING_FILE L"wmdrmdla.txt"
#include "logfn.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHttpMeteringPluginFw::ConstructL()
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::ConstructL" );
    iIdle =  CIdle::NewL( CActive::EPriorityIdle );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::CWmDrmDlaHttpMeteringPluginFw
// ---------------------------------------------------------------------------
//
CWmDrmDlaHttpMeteringPluginFw::CWmDrmDlaHttpMeteringPluginFw() 
    : CActive( EPriorityStandard ), 
      iState( CWmDrmDlaHttpMeteringPluginFw::EIdle )
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::CWmDrmDlaHttpMeteringPluginFw" );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::NewL
// ---------------------------------------------------------------------------
//
CWmDrmDlaHttpMeteringPluginFw* CWmDrmDlaHttpMeteringPluginFw::NewL()
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::NewL" );
    CWmDrmDlaHttpMeteringPluginFw* self = 
        CWmDrmDlaHttpMeteringPluginFw::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::NewLC
// ---------------------------------------------------------------------------
//
CWmDrmDlaHttpMeteringPluginFw* CWmDrmDlaHttpMeteringPluginFw::NewLC()
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::NewLC" );
    CWmDrmDlaHttpMeteringPluginFw* self = 
        new( ELeave ) CWmDrmDlaHttpMeteringPluginFw;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::~CWmDrmDlaHttpMeteringPluginFw
// ---------------------------------------------------------------------------
//
CWmDrmDlaHttpMeteringPluginFw::~CWmDrmDlaHttpMeteringPluginFw()
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::~CWmDrmDlaHttpMeteringPluginFw" );
    CancelMetering();
    delete iHttpPlugin;
    delete iUiNotifier;
    delete iIdle;
    delete iMeteringChallenge;
    delete iMeteringUrl;
    delete iMeteringResponse;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::ProcessMeteringCertificate
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHttpMeteringPluginFw::ProcessMeteringCertificate( 
    const TDesC8& aCertificate, 
    TRequestStatus& aStatus )
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::ProcessMeteringCertificate" );
    
    TRequestStatus *status = &aStatus;
    aStatus = KRequestPending;
    if ( iState != CWmDrmDlaHttpMeteringPluginFw::EIdle )
        {
        User::RequestComplete( status, KErrInUse );
        return;
        }
    
    iClientStatus = &aStatus;
    iCertificate = &aCertificate;
    iState = CWmDrmDlaHttpMeteringPluginFw::EResolvingHttpPlugin;
    CompleteSelf();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::CancelMetering
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHttpMeteringPluginFw::CancelMetering()
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::CancelMetering" );
    if ( iHttpPlugin )
        {
        iHttpPlugin->CancelMetering();
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
// CWmDrmDlaHttpMeteringPluginFw::SetIapId
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHttpMeteringPluginFw::SetIapId( TInt aIapId )
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::SetIapId" );
    LOG2( "aIapId: %d", aIapId );
    iIapId = aIapId;
    if ( iHttpPlugin )
        {
        iHttpPlugin->SetIapId( iIapId );
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::DoCancel
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHttpMeteringPluginFw::DoCancel()
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::DoCancel" );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::RunL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHttpMeteringPluginFw::RunL()
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::RunL" );
    LOG2( "iState: %d", iState );
    LOG2( "iStatus.Int(): %d", iStatus.Int() );
    if ( iStatus.Int() != KErrNone )
        {
        HandleErrorL( iStatus.Int() );
        }
    
    switch ( iState )
	    {
        case CWmDrmDlaHttpMeteringPluginFw::EResolvingHttpPlugin:
            ResolvingHttpPluginHandlerL();
            break;
	    
        case CWmDrmDlaHttpMeteringPluginFw::ERetrievingMeteringChallenge:
    	    RetrievingMeteringChallenge();
    	    break;
    	
    	case CWmDrmDlaHttpMeteringPluginFw::EProcessingMeteringChallenge:
    	    ProcessingMeteringChallenge();
    	    break;
    	
    	case CWmDrmDlaHttpMeteringPluginFw::ERetrievingMeteringResponse:
    	    RetrievingMeteringResponse();
    	    break;
    	
    	case CWmDrmDlaHttpMeteringPluginFw::EProcessingMeteringResponse:
    	    ProcessingMeteringResponse();
    	    break;
    	    
    	case CWmDrmDlaHttpMeteringPluginFw::EMeteringFinished:
    	    MeteringFinished();
    	    break;
    	
    	default:
            ASSERT( EFalse );
    	    break;
    	}
    }    

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::RunError
// ---------------------------------------------------------------------------
//
TInt CWmDrmDlaHttpMeteringPluginFw::RunError( TInt aError )
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::RunError" );
    LOG2( "aError: %d", aError );
    CompleteClientRequest( aError );
    Reset();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::UserCancellation
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHttpMeteringPluginFw::UserCancellation()
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::UserCancellation");
    TCallBack callBack( CWmDrmDlaHttpMeteringPluginFw::AsyncCancel, 
                        this );
    iIdle->Start( callBack );
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::CompleteSelf
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpMeteringPluginFw::CompleteSelf()
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::CompleteSelf" );
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
void CWmDrmDlaHttpMeteringPluginFw::CompleteClientRequest( TInt aError )
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::CompleteClientRequest" );
    LOG2( "aError: %d", aError );
    if ( iClientStatus )
        {
        User::RequestComplete( iClientStatus, aError );
        iClientStatus = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::::Reset
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpMeteringPluginFw::Reset()
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::Reset" );
    delete iHttpPlugin;
    iHttpPlugin = NULL;
    delete iUiNotifier;
    iUiNotifier = NULL;
    delete iMeteringChallenge;
    iMeteringChallenge = NULL;
    delete iMeteringUrl;
    iMeteringUrl = NULL;
    delete iMeteringResponse;
    iMeteringResponse = NULL;
    iCertificate = NULL;
    iState = CWmDrmDlaHttpMeteringPluginFw::EIdle;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::ResolvingHttpPluginHandlerL
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpMeteringPluginFw::ResolvingHttpPluginHandlerL()
    {
    TInt err( KErrNone );

    LOGFNR( "CWmDrmDlaHttpMeteringPluginFw::ResolvingHttpPluginHandlerL", err );

    TRAP( err, ResolveHttpPluginWithCertificateL( *iCertificate ) );
    if ( err )
        {
        HandleErrorL( err );
        }
    iState = CWmDrmDlaHttpMeteringPluginFw::ERetrievingMeteringChallenge;
    CompleteSelf();
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::RetrievingMeteringChallengeL
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpMeteringPluginFw::RetrievingMeteringChallenge()
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::RetrievingMeteringChallenge" );
    delete iMeteringChallenge;
    iMeteringChallenge = NULL;
    delete iMeteringUrl;
    iMeteringUrl = NULL;
    SetActive();
    iState = CWmDrmDlaHttpMeteringPluginFw::EProcessingMeteringChallenge;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNotSupported );
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::ProcessingMeteringChallengeL
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpMeteringPluginFw::ProcessingMeteringChallenge()
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::ProcessingMeteringChallenge" );
    iHttpPlugin->ProcessMeteringChallenge( *iMeteringChallenge, 
                                           *iMeteringUrl, 
                                           iStatus );
    SetActive();
    iState = CWmDrmDlaHttpMeteringPluginFw::ERetrievingMeteringResponse;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::RetrievingMeteringResponseL
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpMeteringPluginFw::RetrievingMeteringResponse()
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::RetrievingMeteringResponse" );
    delete iMeteringResponse;
    iMeteringResponse = NULL;
    iHttpPlugin->GetMeteringResponse( iMeteringResponse, iStatus );
    SetActive();
    iState = CWmDrmDlaHttpMeteringPluginFw::EProcessingMeteringResponse;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::ProcessingMeteringResponseL
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpMeteringPluginFw::ProcessingMeteringResponse()
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::ProcessingMeteringResponse" );
    SetActive();
    iState = CWmDrmDlaHttpMeteringPluginFw::EMeteringFinished;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNotSupported );
    }
    
// ----------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::MeteringFinishedL
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpMeteringPluginFw::MeteringFinished()
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::MeteringFinished" );
    LOG2( "iPartialMetering: %d", iPartialMetering );
    if ( iPartialMetering )
        {
        iState = CWmDrmDlaHttpMeteringPluginFw::ERetrievingMeteringChallenge;
        CompleteSelf();
        }
    else
        {
        iHttpPlugin->MeteringFinished();
        CompleteClientRequest( KErrNone );
        Reset();
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::::ResolveHttpPluginWithCertificateL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHttpMeteringPluginFw::ResolveHttpPluginWithCertificateL( 
    const TDesC8& aCertificate )
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::ResolveHttpPluginL" );

    delete iHttpPlugin;
    iHttpPlugin = NULL;
    delete iUiNotifier;
    iUiNotifier = NULL;
    
    WmDrmDlaHttpFwPluginResolver::ResolveHttpPluginWithCertificateL( 
            aCertificate, iHttpPlugin, iUiNotifier );
    
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
// CWmDrmDlaHttpMeteringPluginFw::HandleErrorL
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHttpMeteringPluginFw::HandleErrorL( TInt aError )
    {
    TInt err( KErrNone );
    
    LOGFNR( "CWmDrmDlaHttpMeteringPluginFw::HandleErrorL", err );
    LOG2( "iState: %d", iState );
    LOG2( "aError: %d", aError );
        
    if ( iUiNotifier )
        {
        TRAP( err, iUiNotifier->HandleErrorL( aError ) );
        }
    
    switch ( iState )
        {
        case CWmDrmDlaHttpMeteringPluginFw::ERetrievingMeteringChallenge:
            break;

        case CWmDrmDlaHttpMeteringPluginFw::EProcessingMeteringChallenge:
            break;

        case CWmDrmDlaHttpMeteringPluginFw::ERetrievingMeteringResponse:
            break;

        case CWmDrmDlaHttpMeteringPluginFw::EProcessingMeteringResponse:
            break;

        case CWmDrmDlaHttpMeteringPluginFw::EMeteringFinished:
            break;

        default:
            break;
        }
    
    User::Leave( aError );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpMeteringPluginFw::AsyncCancel
// ---------------------------------------------------------------------------
//
TInt CWmDrmDlaHttpMeteringPluginFw::AsyncCancel( TAny* aPtr )
    {
    LOGFN( "CWmDrmDlaHttpMeteringPluginFw::AsyncCancel");
    CWmDrmDlaHttpMeteringPluginFw* meteringFw = 
        static_cast<CWmDrmDlaHttpMeteringPluginFw*>( aPtr );
    meteringFw->CancelMetering();
    return EFalse;
    }
