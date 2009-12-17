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
* Description:  WMDRM DLA Default Http License Acquisition
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
#include "asf.h"
#include "wmdrmdladefaultlicacq.h"
#include "wmdrmdladefaulthttpplugin.h"
#include "wmdrmdlabrowserviewhandler.h"
#include "wmdrmdlabrowserobserver.h"
#include "wmdrmdladefaulthttpmanager.h"

#define _LOGGING_FILE L"wmdrmdladefaulthttpplugin.txt"
#include "logfn.h"

// CONSTANTS
_LIT8( KDefaultIdentifier8, "camese" );
_LIT8( KDefaultIdentifier, "c\0a\0m\0e\0s\0e\0" );
_LIT8( KNonSilentIdentifier, "n\0o\0n\0s\0i\0l\0e\0n\0t" );

_LIT8( KAcceptHeader8, "*/*" );
_LIT8( KAcceptLanguageHeader8, "en-us" );
_LIT8( KPostContentTypeHeader8, "application/x-www-form-urlencoded" );

_LIT8( KDataTypeLicenseResponse, "application/vnd.ms-wmdrm.lic-resp" );
_LIT8( KDataTypeHtml, "text/html" );

_LIT8( KGetLicenseChallengeParam8, "challenge" );
_LIT8( KGetNonSilentParam8, "nonsilent" );
_LIT8( KLicenseStatusParam8, "License1Status" );

_LIT8( KTIDHeader8, "TID=" );
_LIT8( KQueryTag8, "?" );
_LIT8( KNonSilentValue, "1" );

_LIT8( KAmpersand, "&" );
_LIT8( KChallenge, "nonsilent=1&challenge=" );

_LIT8( KHRESULTFormat,"0x%08x" );
const TInt KHRESULTFormatLength = 10; // '0x' + %08x
const TUint KHRESULTBase = 0x80000000; 
const TInt KHRESULTErrorMask = 0x01 << 31 | 0x04 << 16;

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultLicAcq::ConstructL()
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::ConstructL" );
    iHttpManager = CWmDrmDlaDefaultHttpManager::NewL( *this );
    iHttpManager->SetKeepAlive( ETrue );
    iIdle =  CIdle::NewL( CActive::EPriorityIdle );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::CWmDrmDlaDefaultLicAcq
// ---------------------------------------------------------------------------
//
CWmDrmDlaDefaultLicAcq::CWmDrmDlaDefaultLicAcq(
    CWmDrmDlaDefaultHttpPlugin* aPlugin ) 
    : iState( CWmDrmDlaDefaultLicAcq::ENotActive ), iPlugin( aPlugin )
      
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::CWmDrmDlaDefaultLicAcq" );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::NewL
// ---------------------------------------------------------------------------
//
CWmDrmDlaDefaultLicAcq* CWmDrmDlaDefaultLicAcq::NewL(
    CWmDrmDlaDefaultHttpPlugin* aPlugin )
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::NewL" );
    CWmDrmDlaDefaultLicAcq* self 
        = new( ELeave ) CWmDrmDlaDefaultLicAcq( aPlugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::~CWmDrmDlaDefaultLicAcq
// ---------------------------------------------------------------------------
//
CWmDrmDlaDefaultLicAcq::~CWmDrmDlaDefaultLicAcq()
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::~CWmDrmDlaDefaultLicAcq" );
    delete iIdle;
    delete iBrowserViewHandler;
    delete iHttpManager;
    delete iFormEncoder;
    delete iPostData;
    delete iLicenseChallenge;
    delete iLicenseResponse;
    delete iLicenseServerUrl16;
    delete iLicenseServerUrl;
    delete iHtmlData;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::SupportedFileL
// ---------------------------------------------------------------------------
//
TBool CWmDrmDlaDefaultLicAcq::SupportedFileL( 
    const RFile& aFile,
    CWmDrmDlaUiNotifier*& aUiNotifier)
    {
    TInt err( KErrNone );
    TBool supported( EFalse );
        
    LOGFNR( "CWmDrmDlaDefaultLicAcq::SupportedFileL", err );
    
    //Check that the file is valid ASF-file and that the extented content
    //encryption object of the file includes MS test site identifier
    CAsf* asf( NULL );
    TRAP( err, asf = CAsf::NewL( aFile ) );
    if ( !err )
        {
        supported = iPlugin->Supported( *asf->iExtendedContentEncryptionObject,
                                        KDefaultIdentifier );
        delete asf;
        }
    
    //If file is recognized, then initialize UiNotifier and transfer its
    //ownership to the FW
    if ( supported )
        {
        iPlugin->InitializeUiNotifierL();
        aUiNotifier = iPlugin->UiNotifier();
        }
    LOG2( "supported: %d", supported );     
    return supported;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::SupportedDrmHeaderL
// ---------------------------------------------------------------------------
//
TBool CWmDrmDlaDefaultLicAcq::SupportedDrmHeaderL( 
    const TDesC8& aHeader,
    CWmDrmDlaUiNotifier*& aUiNotifier )
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::SupportedDrmHeaderL" );
    
    //Check that the drm header includes MS test site identifier
    TBool supported( iPlugin->Supported( aHeader, KDefaultIdentifier8 ) );
    //If header is recognized, then initialize UiNotifier and transfer its
    //ownership to the FW
    if ( supported )
        {
        iPlugin->InitializeUiNotifierL();
        aUiNotifier = iPlugin->UiNotifier();
        }
    LOG2( "supported: %d", supported );    
    return supported;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::SilentL
// ---------------------------------------------------------------------------
//
TBool CWmDrmDlaDefaultLicAcq::SilentL( 
    const RFile& aFile )
    {
    TInt err( KErrNone );
    
    LOGFNR( "CWmDrmDlaDefaultLicAcq::SilentL", err );
    
    //Check is the content silent or not by checking that the extented content
    //encryption object of the file includes MS test site non-silent identifier
    iNonSilent = EFalse;
    CAsf* asf( NULL );
    TRAP( err, asf = CAsf::NewL( aFile ) );
    if ( !err )
        {
        iNonSilent = 
            iPlugin->Supported( *asf->iExtendedContentEncryptionObject,
                                KNonSilentIdentifier );
        delete asf;
        }
    iPlugin->UiNotifier()->SetSilent( !iNonSilent );
    LOG2( "!iNonSilent: %d", !iNonSilent );
    return !iNonSilent;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::ProcessDrmHeaderL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultLicAcq::ProcessDrmHeaderL( 
    const TDesC8& /*aDrmHeader*/ )
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::ProcessDrmHeaderL" );
    iPlugin->UiNotifier()->SetProgress( EProcessingDRMHeader );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::AcquireLicense
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultLicAcq::AcquireLicense( 
    const TDesC8& aLicenseChallenge,
    HBufC* aLicenseServerUrl,
    TRequestStatus& aStatus )
    {
    TInt err( KErrNone );
    
    LOGFNR( "CWmDrmDlaDefaultLicAcq::AcquireLicense", err );
    
    iPlugin->UiNotifier()->SetProgress( ERetrievingLicense );
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    
    delete iLicenseServerUrl;
    iLicenseServerUrl = NULL;
    TRAP( err, 
            iLicenseServerUrl = 
                EscapeUtils::ConvertFromUnicodeToUtf8L( *aLicenseServerUrl ) );
    if ( !iLicenseServerUrl )
        {
        CompleteClientRequest( err );
        return;
        }
    delete iLicenseChallenge;
    iLicenseChallenge = NULL;
    iLicenseChallenge = aLicenseChallenge.Alloc();
    if ( !iLicenseChallenge )
        {
        CompleteClientRequest( KErrNoMemory );
        return;
        }
    TRAP( err, AcquireLicenseL() );
    if ( err )
        {
        CompleteClientRequest( err );
        return;
        }
    iState = CWmDrmDlaDefaultLicAcq::ESendingLicenseChallenge;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::GetLicenseResponseL
// ---------------------------------------------------------------------------
//
HBufC8* CWmDrmDlaDefaultLicAcq::GetLicenseResponseL()
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::GetLicenseResponseL" );
    iPlugin->UiNotifier()->SetProgress( EAddingLicense );
    if ( iLicenseResponse )
        {
        return iLicenseResponse->AllocL();
        }
    else
        {
        return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::SendLicenseStorageStatus
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultLicAcq::SendLicenseStorageStatus( 
    HBufC8* aTid,
    const TDesC& /*aUrl*/,
    TInt aError, 
    TRequestStatus& aStatus )
    {
    TInt err( KErrNone );
    
    LOGFNR( "CWmDrmDlaDefaultLicAcq::SendLicenseStorageStatus", err );
    LOG2( "aError: %d", aError );
    
    iPlugin->UiNotifier()->SetProgress( ESendingLicenseStorageStatus );
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    
    if ( !aTid )
        {
        CompleteClientRequest( KErrCorrupt );
        return;
        }
    
    TRAP( err, SendLicenseStorageStatusL( *aTid, aError ) );
    if ( err )
        {
        CompleteClientRequest( err );
        return;
        }
    iState = CWmDrmDlaDefaultLicAcq::ESendingStorageStatus;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::SendDrmHeaderError
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultLicAcq::SendDrmHeaderError( 
    const TDesC& /*aErrorUrl*/, 
    TInt /*aError*/, 
    TRequestStatus& aStatus )
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::SendDrmHeaderError" );
    iPlugin->UiNotifier()->SetProgress( ESendingDrmHeaderStatus );
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    CompleteClientRequest( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::GetContentUrlL
// ---------------------------------------------------------------------------
//
HBufC* CWmDrmDlaDefaultLicAcq::GetContentUrlL()
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::GetContentUrlL" );
    return NULL;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::GetHtmlDataL
// ---------------------------------------------------------------------------
//
HBufC* CWmDrmDlaDefaultLicAcq::GetHtmlDataL()
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::GetHtmlDataL" );
    iPlugin->UiNotifier()->SetProgress( EIdle );
    if ( iHtmlData )
        {
        return iHtmlData->AllocL();
        }
    else
        {
        return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::CancelLicenseAcquisition
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultLicAcq::CancelLicenseAcquisition()
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::CancelLicenseAcquisition" );
    iHttpManager->Stop();
    CompleteClientRequest( KErrCancel );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::SetIapId
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultLicAcq::SetIapId( TInt aIapId )
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::SetIapId" );
    LOG2( "aIapId: %d", aIapId );
    iHttpManager->SetIapId( aIapId );
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::OnGetUsernamePasswordL
// From MHttpManagerObserver
// This function is called when using HTTPS authentication. Authentication is 
// not currently used in the Camese project
// ----------------------------------------------------------------------------
//
TBool CWmDrmDlaDefaultLicAcq::OnGetUsernamePasswordL(
    HBufC8*& /*aUsername*/,
    HBufC8*& /*aPassword*/)
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::OnGetUsernamePasswordL" );
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::OnResponseHeadersL
// From MHttpManagerObserver
// ----------------------------------------------------------------------------
//
void CWmDrmDlaDefaultLicAcq::OnResponseHeadersL(
    const RHTTPResponse& /*aResponse*/,
    const RHTTPHeaders& aHeaders,
    const RStringPool& aStringPool,
    TInt aHttpStatus )
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::OnResponseHeadersL" );
    LOG2( "iState: %d", iState );
    LOG2( "aHttpStatus: %d", aHttpStatus );
    //Redirection in license acquisition
    if ( aHttpStatus >= HTTPStatus::EMultipleChoices && 
         aHttpStatus < HTTPStatus::EBadRequest &&
         iState == CWmDrmDlaDefaultLicAcq::ESendingLicenseChallenge )
        {
        HandleRedirectionHeadersL( aHeaders, aStringPool );
        }
    //License acquisition
    else if ( aHttpStatus == HTTPStatus::EOk &&
              iState != CWmDrmDlaDefaultLicAcq::ESendingStorageStatus )
        {
        HandleLicenseAcquistionHeaders( aHeaders, aStringPool );
        }
    //Sending license storage status or error
    else
        {
        iHttpStatus = aHttpStatus;
        }
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::OnResponseBodyDataL
// From MHttpManagerObserver
// ----------------------------------------------------------------------------
//
void CWmDrmDlaDefaultLicAcq::OnResponseBodyDataL( const TPtrC8 aDataChunk )
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::OnResponseBodyDataL" );
    LOG2( "iState: %d", iState );
    LOG2( "iHttpStatus: %d", iHttpStatus );
    //If we are acquiring license and response headers are OK,
    //then save the response.
    if ( iHttpStatus == HTTPStatus::EOk && 
         iState == CWmDrmDlaDefaultLicAcq::ESendingLicenseChallenge )
        {
        if ( !iLicenseResponse )
            {
            iLicenseResponse = HBufC8::NewL( aDataChunk.Length() );
            }
        else
            {
            iLicenseResponse = 
                iLicenseResponse->ReAllocL( iLicenseResponse->Length() + 
                                            aDataChunk.Length() );
            }
        iLicenseResponse->Des().Append( aDataChunk );
        }
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::OnTransactionComplete
// From MHttpManagerObserver
// ----------------------------------------------------------------------------
//
void CWmDrmDlaDefaultLicAcq::OnTransactionComplete( TInt aSystemError )
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::OnTransactionComplete" );
    LOG2( "iState: %d", iState );
    LOG2( "aSystemError: %d", aSystemError );
    LOG2( "iHttpStatus: %d", iHttpStatus );
    TInt err( KErrNone );
    //Transaction succeeded
    if ( iHttpStatus == HTTPStatus::EOk && !aSystemError )
        {
        //If redirection happened, then start a new license acquisition
        //asynchronously to a new location.
        if ( iState == CWmDrmDlaDefaultLicAcq::ERedirection )
            {
            TCallBack callBack( CWmDrmDlaDefaultLicAcq::Redirection, 
                                this );
            iIdle->Start( callBack );
            iState = CWmDrmDlaDefaultLicAcq::ESendingLicenseChallenge;
            }
        //If license acquisition is non-silent, then launch a browser view.
        else if ( iState == 
                        CWmDrmDlaDefaultLicAcq::ENonSilentLicenseAcquisition )
            {
            TRAP( err, LaunchBrowserViewL() );
            if ( err )
                {
                CompleteClientRequest( err );
                return;
                }
            iPlugin->UiNotifier()->SetProgress( EIdle );
            iState = CWmDrmDlaDefaultLicAcq::EBrowserView;
            }
        //License received silently or license storage status sent succesfully.
        else 
            {
            CompleteClientRequest( aSystemError );
            }
        }
    //Error in transaction
    else if ( iHttpStatus != HTTPStatus::EOk && 
              iHttpStatus != KErrNone )
        {
        CompleteClientRequest( KErrCommsBreak );
        }
    //System error
    else
        {
        CompleteClientRequest( aSystemError );
        }
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::CompleteClientRequest
// ----------------------------------------------------------------------------
//
void CWmDrmDlaDefaultLicAcq::CompleteClientRequest( TInt aError )
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::CompleteClientRequest" );
    LOG2( "aError: %d", aError );
    if ( iClientStatus )
        {
        User::RequestComplete( iClientStatus, aError );
        iClientStatus = NULL;
        }
    iState = CWmDrmDlaDefaultLicAcq::ENotActive;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::AcquireLicenseL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultLicAcq::AcquireLicenseL()
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::AcquireLicenseL" );
    // Set headers
    RArray<CWmDrmDlaDefaultHttpManager::THeader> headers;
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

    //If license acquisition is non-silent, then add non-silent field to
    //request
    if ( iNonSilent )
        {
        iFormEncoder->AddFieldL( KGetNonSilentParam8, KNonSilentValue );
        }
    
    iFormEncoder->AddFieldL( KGetLicenseChallengeParam8, *iLicenseChallenge );

    //Post a request, make sure that iLicenseResponse is NULL
    delete iLicenseResponse;
    iLicenseResponse = NULL;
    iHttpManager->Post( *iLicenseServerUrl, headers, iFormEncoder );

    CleanupStack::PopAndDestroy( &headers );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::SendLicenseStorageStatusL
// ---------------------------------------------------------------------------
//    
void CWmDrmDlaDefaultLicAcq::SendLicenseStorageStatusL( 
    const TDesC8& aTid, 
    TInt aError )
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::SendLicenseStorageStatusL" );
    LOG2( "aError: %d", aError );
    TInt licenseHRESULT( MakeHRESULT( aError ) );
    
    HBufC8* statusUrl = NULL;
    TInt pos( iLicenseServerUrl->Locate( '?' ) );
    // If no query ('?') pos is rightmost character
    pos = (pos != KErrNotFound) ? pos : iLicenseServerUrl->Length();
    TPtrC8 ptrUrl( iLicenseServerUrl->Left( pos ) );
    TInt len( ptrUrl.Length() + KQueryTag8().Length() + 
              KTIDHeader8().Length() + aTid.Length() );

    statusUrl = HBufC8::NewLC( len );
    TPtr8 url( statusUrl->Des() );
    url.Append( ptrUrl );
    url.Append( KQueryTag8 );
    url.Append( KTIDHeader8 );
    url.Append( aTid );

    SendHRESULTL( *statusUrl, KLicenseStatusParam8(), licenseHRESULT );

    CleanupStack::PopAndDestroy( statusUrl );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::MakeHRESULT
// ---------------------------------------------------------------------------
//
TInt CWmDrmDlaDefaultLicAcq::MakeHRESULT( 
    TInt aError )
    {
    TInt result( 0 );
	
    LOGFNR( "CWmDrmDlaDefaultLicAcq::MakeHRESULT", result );
        
    // If there is no error then we need to send 0x00000000
    if ( !aError )
        {
        result = 0;
        }
    // Convert Symbian error convert to an HRESULT
    else if ( static_cast<TUint>(aError) < KHRESULTBase )
        {
        result = KHRESULTErrorMask | aError; 
        }
    else
        {
        // This is already in HRESULT format so use it directly
        result = aError;
        }
    return result;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::SendHRESULTL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultLicAcq::SendHRESULTL( 
    const TDesC8& aUrl, 
    const TDesC8& aHeaderField, 
    TInt aHRESULT )
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::SendHRESULTL" );
    
    //Set headers
    RArray<CWmDrmDlaDefaultHttpManager::THeader> headers;
    CleanupClosePushL( headers );

    headers.AppendL( CWmDrmDlaDefaultHttpManager::THeader( HTTP::EAccept,
                                                           KAcceptHeader8 ) );
    headers.AppendL( 
            CWmDrmDlaDefaultHttpManager::THeader( HTTP::EAcceptLanguage,
                                                  KAcceptLanguageHeader8 ) );
    headers.AppendL( 
            CWmDrmDlaDefaultHttpManager::THeader( HTTP::EContentType, 
                                                  KPostContentTypeHeader8 ) );

    HBufC8* resultStatus = HBufC8::NewLC( KHRESULTFormatLength );
    resultStatus->Des().Format( KHRESULTFormat, aHRESULT );

    //Set POST contents
    delete iFormEncoder;
    iFormEncoder = NULL;
    iFormEncoder = CHTTPFormEncoder::NewL();

    iFormEncoder->AddFieldL( aHeaderField, *resultStatus );

    CleanupStack::PopAndDestroy( resultStatus );

    //Post a request
    iHttpManager->Post( aUrl, headers, iFormEncoder );

    CleanupStack::PopAndDestroy( &headers );
    } 

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::HandleRedirectionHeadersL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultLicAcq::HandleRedirectionHeadersL( 
    const RHTTPHeaders& aHeaders,
    const RStringPool& aStringPool )
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::HandleRedirectionHeadersL" );
    
    //Get the redirected location
    RStringF locationStr = aStringPool.StringF( HTTP::ELocation, 
                                                RHTTPSession::GetTable() );
    CleanupClosePushL( locationStr );
    THTTPHdrVal locationValue;
    if ( !aHeaders.GetField( locationStr, 0, locationValue ) )
        {
        //Set new redirected url and change state to redirection
        delete iLicenseServerUrl;
        iLicenseServerUrl = NULL;
        iLicenseServerUrl = locationValue.StrF().DesC().AllocL();
        iState = CWmDrmDlaDefaultLicAcq::ERedirection;
        iHttpStatus = HTTPStatus::EOk;
        }
    else
        {
        iHttpStatus = KErrHttpRedirectNoLocationField;
        }
    CleanupStack::PopAndDestroy( &locationStr );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::HandleLicenseAcquistionHeaders
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultLicAcq::HandleLicenseAcquistionHeaders( 
    const RHTTPHeaders& aHeaders,
    const RStringPool& aStringPool )
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::HandleLicenseAcquistionHeaders" );
    
    RStringF contentTypeNameStr = 
        aStringPool.StringF( HTTP::EContentType, RHTTPSession::GetTable() );

    // read the first part of content-type field
    THTTPHdrVal contentTypeVal;
    if( !aHeaders.GetField( contentTypeNameStr, 0, contentTypeVal ) )
        {
        //Silent license acquisition
        if ( iState == CWmDrmDlaDefaultLicAcq::ESendingLicenseChallenge &&
             contentTypeVal.StrF().DesC().MatchF( KDataTypeLicenseResponse ) !=
                                                  KErrNotFound )
            {
            iHttpStatus = HTTPStatus::EOk;
            }
        //Non silent license acquisition
        else if ( iState == CWmDrmDlaDefaultLicAcq::ESendingLicenseChallenge &&
                  contentTypeVal.StrF().DesC().MatchF( KDataTypeHtml ) != 
                                                       KErrNotFound )
            {
            iHttpStatus = HTTPStatus::EOk;
            iNonSilent = ETrue;
            iState = CWmDrmDlaDefaultLicAcq::ENonSilentLicenseAcquisition;
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

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::LaunchBrowserViewL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultLicAcq::LaunchBrowserViewL()
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::LaunchBrowserViewL" );
    
    if ( !iBrowserViewHandler )
        {
        iBrowserViewHandler = CWmDrmDlaBrowserViewHandler::NewL();
        }
    iBrowserViewHandler->LaunchBrowserViewL( this );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::Redirection
// ---------------------------------------------------------------------------
//
TInt CWmDrmDlaDefaultLicAcq::Redirection( TAny* aPtr )
    {
    TInt err( KErrNone );
    
    LOGFNR( "CWmDrmDlaDefaultLicAcq::Redirection", err );
    
    CWmDrmDlaDefaultLicAcq* defaultLicAcq = 
        static_cast<CWmDrmDlaDefaultLicAcq*>( aPtr );

    TRAP( err, defaultLicAcq->AcquireLicenseL() );
    if ( err )
        {
        defaultLicAcq->CompleteClientRequest( err );
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::PostUrlL
// ---------------------------------------------------------------------------
//
const TDesC& CWmDrmDlaDefaultLicAcq::PostUrlL()
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::PostUrlL" );
    delete iLicenseServerUrl16;
    iLicenseServerUrl16 = NULL;
    iLicenseServerUrl16 
        = EscapeUtils::ConvertToUnicodeFromUtf8L( *iLicenseServerUrl );
    return *iLicenseServerUrl16;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::PostContentTypeL
// ---------------------------------------------------------------------------
//
const TDesC8& CWmDrmDlaDefaultLicAcq::PostContentTypeL()
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::PostContentTypeL" );
    return KPostContentTypeHeader8;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::PostDataL
// ---------------------------------------------------------------------------
//
const TDesC8& CWmDrmDlaDefaultLicAcq::PostDataL()
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::PostDataL" );
    delete iPostData;
    iPostData = NULL;
    iPostData = HBufC8::NewL( KChallenge().Length() + 
                              iLicenseChallenge->Length() );
    TPtr8 ptr( iPostData->Des() );
    ptr.Append( KChallenge() );
    ptr.Append( *iLicenseChallenge );
    return *iPostData;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::PostContentDataBoundaryL
// ---------------------------------------------------------------------------
//
const TDesC8& CWmDrmDlaDefaultLicAcq::PostContentDataBoundaryL()
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::PostContentDataBoundaryL" );
    return KAmpersand;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::IapId
// ---------------------------------------------------------------------------
//
TInt CWmDrmDlaDefaultLicAcq::IapId()
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::IapId" );
    return iHttpManager->IapId();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::LicenseResponseReceived
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultLicAcq::LicenseResponseReceived( 
    const TDesC8& aLicenseResponse )
    {
    TInt err( KErrNone );
    
    LOGFNR( "CWmDrmDlaDefaultLicAcq::LicenseResponseReceived", err );
    
    //Non-silent license response received, allocate the response and complete
    //client request
    delete iLicenseResponse;
    iLicenseResponse = NULL;
    iLicenseResponse = aLicenseResponse.Alloc();
    if ( !iLicenseResponse )
        {
        err = KErrNoMemory;
        }
    CompleteClientRequest( err );
    iPlugin->UiNotifier()->SetProgress( EProcessingDRMHeader );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultLicAcq::BrowserExit
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultLicAcq::BrowserExit( TInt aError )
    {
    LOGFN( "CWmDrmDlaDefaultLicAcq::BrowserExit" );
    LOG2( "aError: %d", aError );
    //Browser view closed in non-silent license acquisition, complete
    //client request with the error code.
    CompleteClientRequest( aError );
    }
