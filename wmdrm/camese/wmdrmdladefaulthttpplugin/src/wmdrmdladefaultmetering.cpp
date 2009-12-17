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
* Description:  WMDRM DLA Default Metering
*
*/


#include <chttpformencoder.h>
#include <httpstringconstants.h>
#include <httperr.h>
#include <http/rhttpheaders.h>
#include <http/thttphdrfielditer.h>
#include <stringpool.h>
#include <escapeutils.h>
#include <wmdrmdlatypes.h>
#include <wmdrmdlauinotifier.h>
#include "wmdrmdladefaultmetering.h"
#include "wmdrmdladefaulthttpplugin.h"
#include "wmdrmdladefaulthttpmanager.h"

#define _LOGGING_FILE L"wmdrmdladefaulthttpplugin.txt"
#include "logfn.h"

// CONSTANTS
_LIT8( KDefaultIdentifier, "camese" );

_LIT8( KAcceptHeader8, "*/*" );
_LIT8( KAcceptLanguageHeader8, "en-us" );
_LIT8( KPostContentTypeHeader8, "application/x-www-form-urlencoded" );

_LIT8( KDataTypeMeteringResponse, "application/vnd.ms-wmdrm.meter-resp" );
_LIT8( KGetMeterChallengeParam8, "meterchallenge" );

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultMetering::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultMetering::ConstructL()
    {
    LOGFN( "CWmDrmDlaDefaultMetering::ConstructL" );
    iHttpManager = CWmDrmDlaDefaultHttpManager::NewL( *this );
    iHttpManager->SetKeepAlive( ETrue );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultMetering::CWmDrmDlaDefaultMetering
// ---------------------------------------------------------------------------
//
CWmDrmDlaDefaultMetering::CWmDrmDlaDefaultMetering( 
    CWmDrmDlaDefaultHttpPlugin* aPlugin ) 
    : iState( CWmDrmDlaDefaultMetering::ENotActive ), iPlugin( aPlugin )
    {
    LOGFN( "CWmDrmDlaDefaultMetering::CWmDrmDlaDefaultMetering" );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultMetering::NewL
// ---------------------------------------------------------------------------
//
CWmDrmDlaDefaultMetering* CWmDrmDlaDefaultMetering::NewL(
    CWmDrmDlaDefaultHttpPlugin* aPlugin )
    {
    LOGFN( "CWmDrmDlaDefaultMetering::NewL" );
    CWmDrmDlaDefaultMetering* self 
        = new( ELeave ) CWmDrmDlaDefaultMetering( aPlugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultMetering::~CWmDrmDlaDefaultMetering
// ---------------------------------------------------------------------------
//
CWmDrmDlaDefaultMetering::~CWmDrmDlaDefaultMetering()
    {
    LOGFN( "CWmDrmDlaDefaultMetering::~CWmDrmDlaDefaultMetering" );
    delete iHttpManager;
    delete iFormEncoder;
    delete iMeteringResponse;
    delete iMeteringServerUrl;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultMetering::SupportedMeteringCertificateL
// ---------------------------------------------------------------------------
//
TBool CWmDrmDlaDefaultMetering::SupportedMeteringCertificateL( 
    const TDesC8& aCertificate,
    CWmDrmDlaUiNotifier*& aUiNotifier )
    {
    LOGFN( "CWmDrmDlaDefaultMetering::SupportedMeteringCertificateL" );
    
    TBool supported( iPlugin->Supported( aCertificate, KDefaultIdentifier ) );
    if ( supported )
        {
        iPlugin->InitializeUiNotifierL();
        aUiNotifier = iPlugin->UiNotifier();
        }
    LOG2( "supported: %d", supported );     
    return supported;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultMetering::ProcessMeteringChallenge
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultMetering::ProcessMeteringChallenge( 
    const TDesC8& aMeteringChallenge, 
    const TDesC& aUrl, 
    TRequestStatus& aStatus )
    {
    TInt err( KErrNone );
    
    LOGFNR( "CWmDrmDlaDefaultMetering::ProcessMeteringChallenge", err );
    
    iPlugin->UiNotifier()->SetProgress( EProcessingMeteringChallenge );
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    
    delete iMeteringServerUrl;
    iMeteringServerUrl = NULL;
    iMeteringServerUrl = aUrl.Alloc();
    if ( !iMeteringServerUrl )
        {
        CompleteClientRequest( KErrNoMemory );
        return;
        }
    
    TRAP( err, ProcessMeteringChallengeL( aMeteringChallenge ) );
    if ( err )
        {
        CompleteClientRequest( err );
        return;
        }
    iState = CWmDrmDlaDefaultMetering::ESendingMeteringChallenge;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultMetering::GetMeteringResponse
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultMetering::GetMeteringResponse( 
    HBufC8*& aResponse, 
    TRequestStatus& aStatus )
    {
    TInt err( KErrNone );
    
    LOGFNR( "CWmDrmDlaDefaultMetering::GetMeteringResponse", err );
        
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    
    if ( iMeteringResponse )
        {
        aResponse = iMeteringResponse->Alloc();
        if ( !aResponse )
            {
            err = KErrNoMemory;
            }
        }
    else
        {
        aResponse = NULL;
        }
    CompleteClientRequest( err );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultMetering::MeteringFinished
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultMetering::MeteringFinished()
    {
    LOGFN( "CWmDrmDlaDefaultMetering::MeteringFinished" );
    iPlugin->UiNotifier()->SetProgress( EIdle );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultMetering::CancelMetering
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultMetering::CancelMetering()
    {
    LOGFN( "CWmDrmDlaDefaultMetering::CancelMetering" );
    iHttpManager->Stop();
    CompleteClientRequest( KErrCancel );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::SetIapId
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultMetering::SetIapId( TInt aIapId )
    {
    LOGFN( "CWmDrmDlaDefaultMetering::SetIapId" );
    LOG2( "aIapId: %d", aIapId );
    iHttpManager->SetIapId( aIapId );
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultMetering::OnGetUsernamePasswordL
// From MHttpManagerObserver
// This function is called when using HTTPS authentication. Authentication is 
// not currently used in the Camese project
// ----------------------------------------------------------------------------
//
TBool CWmDrmDlaDefaultMetering::OnGetUsernamePasswordL(
    HBufC8*& /*aUsername*/,
    HBufC8*& /*aPassword*/)
    {
    LOGFN( "CWmDrmDlaDefaultMetering::OnGetUsernamePasswordL" );
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultMetering::OnResponseHeadersL
// From MHttpManagerObserver
// ----------------------------------------------------------------------------
//
void CWmDrmDlaDefaultMetering::OnResponseHeadersL(
    const RHTTPResponse& /*aResponse*/,
    const RHTTPHeaders& aHeaders,
    const RStringPool& aStringPool,
    TInt aHttpStatus )
    {
    LOGFN( "CWmDrmDlaDefaultMetering::OnResponseHeadersL" );
    LOG2( "iState: %d", iState );
    LOG2( "aHttpStatus: %d", aHttpStatus );
    if ( aHttpStatus == HTTPStatus::EOk )
        {
        RStringF contentTypeNameStr 
            = aStringPool.StringF( HTTP::EContentType, 
                                   RHTTPSession::GetTable() );

        // read the first part of content-type field
        THTTPHdrVal contentTypeVal;
        if( !aHeaders.GetField( contentTypeNameStr, 0, contentTypeVal ) )
            {
            //Metering
            if ( iState == 
                        CWmDrmDlaDefaultMetering::ESendingMeteringChallenge &&
                 contentTypeVal.StrF().DesC().MatchF( 
                         KDataTypeMeteringResponse ) != KErrNotFound )
                {
                iHttpStatus = HTTPStatus::EOk;
                }
            else
                {
                iHttpStatus = KErrHttpDecodeContentType;
                }
            }
        else
            {
            iHttpStatus = KErrHttpDecodeContentType;
            }
        contentTypeNameStr.Close();
        }
    else
        {
        iHttpStatus = aHttpStatus;
        }
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultMetering::OnResponseBodyDataL
// From MHttpManagerObserver
// ----------------------------------------------------------------------------
//
void CWmDrmDlaDefaultMetering::OnResponseBodyDataL( const TPtrC8 aDataChunk )
    {
    LOGFN( "CWmDrmDlaDefaultMetering::OnResponseBodyDataL" );
    LOG2( "iState: %d", iState );
    LOG2( "iHttpStatus: %d", iHttpStatus );
    if ( iHttpStatus == HTTPStatus::EOk && 
         iState == CWmDrmDlaDefaultMetering::ESendingMeteringChallenge )
        {
        if ( !iMeteringResponse )
            {
            iMeteringResponse = HBufC8::NewL( aDataChunk.Length() );
            }
        else
            {
            iMeteringResponse 
                = iMeteringResponse->ReAllocL( iMeteringResponse->Length() + 
                                               aDataChunk.Length() );
            }
        iMeteringResponse->Des().Append( aDataChunk );
        }
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultMetering::OnTransactionComplete
// From MHttpManagerObserver
// ----------------------------------------------------------------------------
//
void CWmDrmDlaDefaultMetering::OnTransactionComplete( TInt aSystemError )
    {
    LOGFN( "CWmDrmDlaDefaultMetering::OnTransactionComplete" );
    LOG2( "iState: %d", iState );
    LOG2( "aSystemError: %d", aSystemError );
    LOG2( "iHttpStatus: %d", iHttpStatus );
    if ( iHttpStatus == HTTPStatus::EOk && !aSystemError )
        {
        CompleteClientRequest( aSystemError );
        }
    else if ( iHttpStatus != HTTPStatus::EOk )
        {
        CompleteClientRequest( KErrCommsBreak );
        }
    else
        {
        CompleteClientRequest( aSystemError );
        }
    iState = CWmDrmDlaDefaultMetering::ENotActive;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultMetering::CompleteClientRequest
// ----------------------------------------------------------------------------
//
void CWmDrmDlaDefaultMetering::CompleteClientRequest( TInt aError )
    {
    LOGFN( "CWmDrmDlaDefaultMetering::CompleteClientRequest" );
    LOG2( "aError: %d", aError );
    if ( iClientStatus )
        {
        User::RequestComplete( iClientStatus, aError );
        iClientStatus = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultMetering::ProcessMeteringChallengeL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultMetering::ProcessMeteringChallengeL( 
    const TDesC8& aMeteringChallenge )
    {
    LOGFN( "CWmDrmDlaDefaultMetering::ProcessMeteringChallengeL" );
    // Make an 8-bit URI of the challenge URL.
    HBufC8* meteringServerUrl8( 
            EscapeUtils::ConvertFromUnicodeToUtf8L( *iMeteringServerUrl ) );
    CleanupStack::PushL( meteringServerUrl8 );
    TUriParser8 uri;
    User::LeaveIfError( uri.Parse( *meteringServerUrl8 ) );
    
    // POST the DrmHeader status to the server
    // Set headers
    RArray< CWmDrmDlaDefaultHttpManager::THeader > headers;
    CleanupClosePushL( headers );

    headers.AppendL( CWmDrmDlaDefaultHttpManager::THeader( HTTP::EAccept,
                                                           KAcceptHeader8 ) );
    headers.AppendL( 
            CWmDrmDlaDefaultHttpManager::THeader( HTTP::EAcceptLanguage, 
                                                  KAcceptLanguageHeader8 ) );
    headers.AppendL( 
            CWmDrmDlaDefaultHttpManager::THeader( HTTP::EContentType, 
                                                  KPostContentTypeHeader8 ) );
    
    // Set POST contents
    delete iFormEncoder;
    iFormEncoder = NULL;
    iFormEncoder = CHTTPFormEncoder::NewL();
    iFormEncoder->AddFieldL( KGetMeterChallengeParam8, aMeteringChallenge );

    iHttpManager->Post( *meteringServerUrl8, headers, iFormEncoder );

    CleanupStack::PopAndDestroy( 2, meteringServerUrl8 ); //headers, 
                                                          //meteringServerUrl8
    }
