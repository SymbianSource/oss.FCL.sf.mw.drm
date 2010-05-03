/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Http interface for RoapHandler
*
*/


// INCLUDE FILES
#include <stringpool.h>
#include <http/thttphdrval.h>
#include <httpfiltercommonstringsext.h>

//#include <CookieFilterInterface.h>
//#include <uaproffilter_interface.h>
//#include <HttpFilterProxyInterface.h>
#include <httpfilteriopinterface.h>

#include "RoapHttpHandler.h"
#include "RoapConnection.h"
#include "RoapResponse.h"
#include "RoapObserver.h"
#include "RoapDef.h"
#include "RoapLog.h"

#include "buffercontainers.h"


using namespace Roap;

// ================= CONSTANTS ======================

// The time out value in HTTP, 30 sec
LOCAL_D const TInt KRoapTimeoutValue = 60000000;

_LIT8( KTestUserName, "" );
_LIT8( KTestPassword, "" );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CRoapHttpHandler::NewL()
// ---------------------------------------------------------
//
CRoapHttpHandler* CRoapHttpHandler::NewL()
    {
    CRoapHttpHandler* handler = new ( ELeave ) CRoapHttpHandler();
    CleanupStack::PushL( handler );
    handler->ConstructL();
    CleanupStack::Pop( handler );
    return handler;
    }

// ---------------------------------------------------------
// CRoapHttpHandler::~CHttpLoader()
// ---------------------------------------------------------
//
CRoapHttpHandler::~CRoapHttpHandler()
    {
    Cancel();
    iSession.Close(); // iSession also closes iTransaction
    delete iRequestBody;
    delete iUri;
    delete iBoundary;
    delete iConnection;
    delete iTimeout;
    }

// ---------------------------------------------------------
// CRoapHttpHandler::ConstructL()
// ---------------------------------------------------------
//
void CRoapHttpHandler::ConstructL()
    {
    LOG( _L("CRoapHttpHandler::ConstructL") );

    iConnection = CRoapConnection::NewL();
    iTimeout = CPeriodic::NewL( CActive::EPriorityUserInput );
    }

// ---------------------------------------------------------
// CRoapHttpHandler::CreateConnectionL()
// ---------------------------------------------------------
//
void CRoapHttpHandler::CreateConnectionL( TRequestStatus* aStatus )
    {
    LOG( _L("CRoapHttpHandler::CreateConnectionL") );

    __ASSERT_ALWAYS( iState == EInit || iState == EReady, \
                                            User::Invariant() );
    __ASSERT_ALWAYS( iConnection, User::Invariant() );

    TUint32 ap;
    if ( iConnection->IsConnected( ap ) )
        {
        // Already connected
        User::RequestComplete( iParentStatus, KErrAlreadyExists );
        iParentStatus = NULL;
        return;
        }

    if ( iObserver && !iObserver->ConnectionConfL() )
        {
        User::Leave( KErrCancel );
        }

    iParentStatus = aStatus;
    *iParentStatus = KRequestPending;
    iState = EStart;
    TRequestStatus* ownStatus = &iStatus;
    *ownStatus = KRequestPending;
    SetActive();
    User::RequestComplete( ownStatus, KErrNone );
    }

// ---------------------------------------------------------
// CRoapHttpHandler::IsConnectedL()
// ---------------------------------------------------------
//
void CRoapHttpHandler::SetObserver( MRoapObserver* aRoapObserver )
    {
    LOG( _L("CRoapHttpHandler::SetObserver") );

    iObserver = aRoapObserver;
    }

// ---------------------------------------------------------
// CRoapHttpHandler::CreateConnectionL()
// ---------------------------------------------------------
//
void CRoapHttpHandler::DoTransactionL(
            CRoapResponse& aResponse,
            TDesC8& aReqBody,
            TRequestStatus* aStatus )
    {
    LOG( _L("CRoapHttpHandler::DoTransactionL1") );

    __ASSERT_ALWAYS( iState == EReady, User::Invariant() );
    __ASSERT_ALWAYS( !iRequestBody, User::Invariant() );

    iRequestBody = aReqBody.AllocL();
    iResponse = &aResponse;

    iParentStatus = aStatus;
    *iParentStatus = KRequestPending;
    iState = ERequest;
    TRequestStatus* ownStatus = &iStatus;
    *ownStatus = KRequestPending;
    SetActive();
    User::RequestComplete( ownStatus, KErrNone );
    }

// ---------------------------------------------------------
// CRoapHttpHandler::SetUrlL()
// ---------------------------------------------------------
//
void CRoapHttpHandler::SetUrlL( TDesC8& aUrl )
    {
    LOG( _L("CRoapHttpHandler::SetUrlL") );

    delete iUri;
    iUri = NULL;
    iUri = aUrl.AllocL();
    }



// ---------------------------------------------------------
// CRoapHttpHandler::DoCancel()
// ---------------------------------------------------------
//
void CRoapHttpHandler::DoCancel()
    {
    LOG( _L("CRoapHttpHandler::DoCancel") );

    switch ( iState )
        {
        case EStart:
        case EConnect:
            {
            iConnection->Cancel();
            break;
            }
        case EComplete:
            {
            iTransaction.Close();
            SelfComplete( iError );
            break;
            }
        default:
            {
            break;
            }
        }
    iError = KErrCancel;
    Complete();
    }

// ---------------------------------------------------------
// CRoapHttpHandler::RunL()
// ---------------------------------------------------------
//
void CRoapHttpHandler::RunL()
    {
    LOG( _L("CRoapHttpHandler::RunL") );

    User::LeaveIfError( iStatus.Int() );

    switch ( iState )
        {
        case EStart:
            {
            ConnectL();
            break;
            }
        case EConnect:
            {
            CreateSessionL();
            break;
            }
        case ERequest:
            {
            DoTransactionL();
            break;
            }
        case EComplete:
            {
            TUint32 ap;
            iState = iConnection->IsConnected( ap ) ? EReady : EInit;
            Complete();
            break;
            }
        case EInit:
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------
// CRoapHttpHandler::RunError()
// ---------------------------------------------------------
//
TInt CRoapHttpHandler::RunError( TInt aError )
    {
    LOG( _L("CRoapHttpHandler::RunError") );

    iError = aError;
    TUint32 ap;
    iState = iConnection->IsConnected( ap ) ? EReady : EInit;
    Complete();
    return KErrNone;
    }

// ---------------------------------------------------------
// CRoapHttpHandler::ConnectL()
// ---------------------------------------------------------
//
void CRoapHttpHandler::ConnectL()
    {
    LOG( _L("CRoapHttpHandler::ConnectL") );

    __ASSERT_ALWAYS( iState == EStart, User::Invariant() );

    iConnection->ConnectL ( iPreferredIap, &iStatus );
    iState = EConnect;
    iError = KErrNone;
    SetActive();
    }

// ---------------------------------------------------------
// CRoapHttpHandler::SetPreferredIap()
// ---------------------------------------------------------
void CRoapHttpHandler::SetPreferredIap( TUint32 aPreferredIap )
    {
    LOG( _L("CRoapHttpHandler::SetPreferredIap") );

    iPreferredIap = aPreferredIap;
    }

// ---------------------------------------------------------
// CRoapHttpHandler::CreateSessionL()
// ---------------------------------------------------------
//
void CRoapHttpHandler::CreateSessionL()
    {
    LOG( _L("CRoapHttpHandler::CreateSessionL") );

    __ASSERT_ALWAYS( iState == EConnect, User::Invariant() );

    TUint32 ap;

    if ( !iConnection->IsConnected( ap ) )
        {
        User::Leave( KErrRoapGeneral );
        }

    iSession.Close();
    iSession.OpenL();

    RStringPool strPool = iSession.StringPool();

    // Remove first session properties just in case.
    RHTTPConnectionInfo connInfo = iSession.ConnectionInfo();

    // Clear RConnection and Socket Server instances
    connInfo.RemoveProperty(strPool.StringF(HTTP::EHttpSocketServ,RHTTPSession::GetTable()));
    connInfo.RemoveProperty(strPool.StringF(HTTP::EHttpSocketConnection,RHTTPSession::GetTable()));

    THTTPHdrVal proxyUsage(strPool.StringF(HTTP::EUseProxy,RHTTPSession::GetTable()));
    connInfo.RemoveProperty(strPool.StringF(HTTP::EProxyUsage,RHTTPSession::GetTable()));
    connInfo.RemoveProperty(strPool.StringF(HTTP::EProxyAddress,RHTTPSession::GetTable()));

    connInfo.SetPropertyL
        (
        strPool.StringF( HTTP::EHttpSocketServ, RHTTPSession::GetTable() ),
        THTTPHdrVal( iConnection->SocketServ().Handle() )
        );

    connInfo.SetPropertyL
        (
        strPool.StringF( HTTP::EHttpSocketConnection, RHTTPSession::GetTable() ),
        THTTPHdrVal( REINTERPRET_CAST( TInt, &iConnection->Conn() ) )
        );

    InstallHttpFiltersL();

    // Complete requests
    iState = EReady;
    SelfComplete( iError );
    // Notify CRoapEngBase
    Complete();
    }


// ---------------------------------------------------------
// CRoapHttpHandler::InstallHttpFilters()
// ---------------------------------------------------------
//
void CRoapHttpHandler::InstallHttpFiltersL()
    {
    LOG( _L("CRoapHttpHandler::InstallHttpFiltersL") );

 // CHttpUAProfFilterInterface::InstallFilterL( iSession );
 // CHttpCookieFilter::InstallFilterL( iSession );
    InstallAuthenticationL( iSession );
 // CHttpFilterProxyInterface::InstallFilterL( iSession );

    // This filter adds port number to HTTP header Host
    // on HTTP(S) CONNECT via proxy.
    // Note: the filter is for interoperability, since some WAP GWs
    // do not work with Host header not having port number.
    CHttpFilterIopInterface::InstallFilterL( iSession, iopOptionHostHeader );
    }


// ---------------------------------------------------------
// CRoapHttpHandler::DoTransactionL()
// ---------------------------------------------------------
//
void CRoapHttpHandler::DoTransactionL()
    {
    LOG( _L("CRoapHttpHandler::DoTransactionL2") );

    __ASSERT_ALWAYS( iState == ERequest, User::Invariant() );

    TUriParser8 uri;
    User::LeaveIfError( uri.Parse( *iUri ) );
    RStringF POST;
    POST = iSession.StringPool().StringF( HTTP::EPOST, RHTTPSession::GetTable() );
    iTransaction = iSession.OpenTransactionL( uri, *this, POST );

    // Set required headers
    RHTTPHeaders hdrs = iTransaction.Request().GetHeaderCollection();

    SetHeaderL(hdrs, HTTP::EAccept, _L8("*/*") ); // for testing
    SetHeaderL(hdrs, HTTP::EAccept, KRoapPDUType );
    SetHeaderL(hdrs, HTTP::EAccept, KMultipartRelatedType );

    SetHeaderL(hdrs, HTTP::EContentType, KRoapPDUType);

    iBytesReceived = 0;
    // Add request body
    MHTTPDataSupplier* ds = this;
    iTransaction.Request().SetBody(*ds);

    iTransaction.SubmitL();

    iState = EComplete;
    iStatus = KRequestPending;
    SetActive();

    iTimeout->Cancel();
    iTimeout->Start( KRoapTimeoutValue,
                     KRoapTimeoutValue,
                     TCallBack( StaticTimeOut,this ) );
    }


// ---------------------------------------------------------
// CRoapHttpHandler::SetHeaderL()
// ---------------------------------------------------------
//
void CRoapHttpHandler::SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField, const TDesC8& aHdrValue)
    {
    LOG( _L("CRoapHttpHandler::SetHeaderL") );

    RStringF valStr = iSession.StringPool().OpenFStringL(aHdrValue);
    THTTPHdrVal val(valStr);
    aHeaders.SetFieldL(iSession.StringPool().StringF(aHdrField,RHTTPSession::GetTable()), val);
    valStr.Close();
    }


// ---------------------------------------------------------
// CRoapHttpHandler::Complete()
// ---------------------------------------------------------
//
void CRoapHttpHandler::Complete()
    {
    LOG( _L("CRoapHttpHandler::Complete") );

    delete iRequestBody;
    iRequestBody = NULL;
    delete iUri;
    iUri = NULL;

    if ( iTimeout )
        {
        iTimeout->Cancel();
        }

    User::RequestComplete( iParentStatus, iError );
    iParentStatus = NULL;
    }


// ---------------------------------------------------------
// CRoapHttpHandler::CRoapHttpHandler()
// ---------------------------------------------------------
//
CRoapHttpHandler::CRoapHttpHandler(): CActive( CActive::EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------
// CRoapHttpHandler::SelfComplete()
// ---------------------------------------------------------
//
void CRoapHttpHandler::SelfComplete( TInt& aResult )
    {
    LOG( _L("CRoapHttpHandler::SelfComplete") );

    if ( iStatus == KRequestPending )
        {
        TRequestStatus* ownStatus = &iStatus;
        User::RequestComplete( ownStatus, aResult );
        }
    else
        {
        if ( aResult != KErrNone )
            {
            iStatus = aResult;
            }
        }
    }


// ---------------------------------------------------------
// CRoapHttpHandler::MHFRunL()
// ---------------------------------------------------------
//
void CRoapHttpHandler::MHFRunL( RHTTPTransaction  /* aTransaction */,
                                const THTTPEvent& aEvent )
    {
    LOG( _L("CRoapHttpHandler::MHFRunL") );

    // Restart timer
    iTimeout->Cancel();
    iTimeout->Start( KRoapTimeoutValue,
                     KRoapTimeoutValue,
                     TCallBack( StaticTimeOut,this ) );

    switch ( aEvent.iStatus )
        {
        case THTTPEvent::EGotResponseHeaders:
            {
            LOG( _L("Got Response Headers!") );

            HandleResponseHeadersL( iTransaction.Response() );
            break;
            }

        case THTTPEvent::EGotResponseBodyData:
            {
            LOG( _L("Got Response Body Data!") );

            TInt ret( KErrNone );
            MHTTPDataSupplier* body = iTransaction.Response().Body();
            TPtrC8 ptr;
            body->GetNextDataPart( ptr );

            iBytesReceived += ptr.Length();
            if ( iResponse->iDataType == TDataType( KMultipartRelatedType ) )
                {
                ret = AppendMultipartData( ptr );
                }
            else
                {
                ret = AppendPduData( ptr );
                }

            if ( iObserver && iReportBytes )
                {
                iObserver->RoapProgressInfoL( KRoapProgressMax + iBytesReceived );
                }

            body->ReleaseData();
            User::LeaveIfError( ret );
            break;
            }

        case THTTPEvent::EFailed:
            {
            LOG( _L("HTTP Event Failed!") );
            if ( iResponse->iDataType == TDataType( KMultipartRelatedType ) )
                {
                }
            if ( iError == KErrNone )
                {
                iError = KErrRoapGeneral;
                }
            iTransaction.Close();
            SelfComplete( iError );
            break;
            }

        case THTTPEvent::ESucceeded:
            {
            LOG( _L("HTTP Event Succeeded!") );

            if ( iResponse->iDataType == TDataType( KMultipartRelatedType ) )
                {
                }
            iTransaction.Close();
            SelfComplete( iError );
            break;
            }

        case THTTPEvent::ERedirectRequiresConfirmation:
            {
            LOG( _L("HTTP event ERedirectRequiresConfirmation received") );
            iTransaction.SubmitL();
            break;
            }

        default:
            {
            LOG( _L("Unknown HTTP event recieved") );
            if ( aEvent.iStatus == KErrHttpRedirectUseProxy )
                {
                }
            else
                {
                if (aEvent.iStatus < 0)
                    {
                     LOG( _L("An Error Occured during HTTP transaction!") );
                    }
                // Handle errors in MHFRunError.
                User::LeaveIfError( aEvent.iStatus );
                }
            break;
            }
        }

    }

// ---------------------------------------------------------
// CRoapHttpHandler::MHFRunError()
// ---------------------------------------------------------
//
TInt CRoapHttpHandler::MHFRunError (
        TInt aError,
        RHTTPTransaction /* aTransaction */,
        const THTTPEvent& /* aEvent */
        )
    {
    LOG( _L("CRoapHttpHandler::MHFRunError") );

    iTransaction.Close();
    iError = aError;
    SelfComplete( iError );
    return KErrNone;
    }

// ---------------------------------------------------------
// CRoapHttpHandler::HandleResponseHeadersL()
// ---------------------------------------------------------
//
void CRoapHttpHandler::HandleResponseHeadersL( RHTTPResponse aHttpResponse )
    {
    LOG( _L("CRoapHttpHandler::HandleResponseHeadersL ->") );

    RHTTPHeaders headers = aHttpResponse.GetHeaderCollection();

    TInt httpCode = aHttpResponse.StatusCode();
    TBool status;

    status = CheckHttpCode( httpCode );

    if ( status )
        {
        RStringF contentTypeStr;
        RStringF boundaryStr;
        THTTPHdrVal contentTypeVal;
        THTTPHdrVal boundaryVal;
        TPtrC8 ptrContentType(KNullDesC8);
        RStringPool srtPool;
        srtPool = iSession.StringPool();

        contentTypeStr = srtPool.StringF( HTTP::EContentType, RHTTPSession::GetTable() );

        User::LeaveIfError( headers.GetField( contentTypeStr, 0, contentTypeVal ) );

        if ( contentTypeVal.Type() == THTTPHdrVal::KStrFVal )
            {
            iResponse->iDataType = TDataType( contentTypeVal.StrF().DesC() );
            }

        if ( iResponse->iDataType == TDataType( KMultipartRelatedType ) )
            {
            DRM::CPathContainer* tempPath( DRM::CPathContainer::NewLC() );
            DRM::CFileNameContainer* fileName( DRM::CFileNameContainer::NewLC() );
            TInt maxSize( 0 );

            if ( iObserver )
                {
                iObserver->ContentDownloadInfoL( tempPath->iBuffer, fileName->iBuffer, maxSize );
                }

            boundaryStr = srtPool.StringF( HttpFilterCommonStringsExt::EBoundary,
                                           HttpFilterCommonStringsExt::GetTable() );

            if ( headers.GetParam( contentTypeStr, boundaryStr, boundaryVal ) == KErrNone )
                {
                // Boundary param found
                delete iBoundary;
                iBoundary = NULL;
                iBoundary = boundaryVal.StrF().DesC().AllocL();
                }
            else
                {
                // a multipart without boundary param
                LOG( _L("Error: multipart boundary missing") );
                   User::Leave( KErrRoapGeneral );
                }
            iResponse->SetDcfPathL( tempPath->iBuffer );
            iResponse->SetContentNameL( fileName->iBuffer );
            CleanupStack::PopAndDestroy( fileName );
            CleanupStack::PopAndDestroy( tempPath );
            iReportBytes = ETrue;

            if ( iObserver )
                {
                iObserver->RoapProgressInfoL( KRoapProgressMax );
                }
            }
        else
            {
            iReportBytes = EFalse;
            }

        LOG( iResponse->iDataType.Des() );

        if ( iResponse->iDataType != TDataType( KRoapPDUType ) &&
             iResponse->iDataType != TDataType( KMultipartRelatedType ) )
            {
            LOG( _L("Response type not supported") );
            User::Leave( KErrRoapUnsupported );
            }

        if ( aHttpResponse.HasBody() &&
             iResponse->iDataType == TDataType( KRoapPDUType ) )
            {
            // Allocate memory for Response PDU
            TInt dataSize = aHttpResponse.Body()->OverallDataSize();
            if ( dataSize >= 0 )
                {
                HBufC8* buf = HBufC8::NewL( dataSize );
                delete iResponse->iPdu;
                iResponse->iPdu = buf;
                }
            }
        }
    LOG( _L("-> CRoapHttpHandler::HandleResponseHeadersL") );
    }


// ---------------------------------------------------------
// CRoapHttpHandler::CheckHttpCode()
// ---------------------------------------------------------
//
TBool CRoapHttpHandler::CheckHttpCode( TInt aHttpStatus )
    {
    RBuf logBuffer;
    logBuffer.CleanupClosePushL();
    logBuffer.CreateL( KMaxPath );
    LOG( _L( "CRoapHttpHandler::CheckHttpCode") );
    logBuffer.Append( _L( "HTTP status code: ") );
    logBuffer.AppendNum( aHttpStatus );
    LOG( logBuffer );
    CleanupStack::PopAndDestroy( &logBuffer );

    if ( HTTPStatus::IsInformational( aHttpStatus ) )
        {
        // 1xx
        // Informational messages.
        iError = KErrNone;
        return EFalse;
        }
    else if ( aHttpStatus == HTTPStatus::EOk ||
              aHttpStatus == HTTPStatus::ENonAuthoritativeInfo )
        {
        // 200 OK
        // 203 Non-Authoritative Information
        iError = KErrNone;
        return ETrue;
        }
    else if ( HTTPStatus::IsSuccessful( aHttpStatus ) )
        {
        // 2xx
        // Success codes without an usable body.
        iError = KErrRoapServer;
        return EFalse;
        }
    // 3xx codes handled by redirect filter.
    else if ( aHttpStatus == HTTPStatus::EUnauthorized ||
              aHttpStatus == HTTPStatus::EProxyAuthenticationRequired )
        {
        // 401 Unauthorized
        // 407 Proxy authentication required
        iError = KErrRoapGeneral;
        return EFalse;
        }
    else if ( aHttpStatus == HTTPStatus::ENotFound ||
              aHttpStatus == HTTPStatus::EGone )
        {
        // 404 Not found
        // 410 Gone
        iError = KErrRoapGeneral;
        return EFalse;
        }
    else if ( HTTPStatus::IsClientError( aHttpStatus ) )
        {
        // 4xx
        iError = KErrRoapDevice;
        return EFalse;
        }
    else if ( aHttpStatus == HTTPStatus::EHTTPVersionNotSupported )
        {
        // 505 HTTP Version Not Supported
        // Retry with lower HTTP version if we can.
        iError = /*VersionRetryL() ? KErrNone :*/ KErrRoapServer;
        return EFalse;
        }
    else if ( HTTPStatus::IsServerError( aHttpStatus ) )
        {
        // 5xx
        // HTTP/1.0 servers may return other 5xx error codes for HTTP/1.1
        // requests. So the same treatment  is given for all 5xx errors
        // (version retry) - it's worth a try.
        iError = /* VersionRetryL() ? KErrNone :*/ KErrRoapServer;
        return EFalse;
        }
    else
        {
        // Everything else.
        iError = KErrRoapGeneral;
        }
    return EFalse;
    }


// ---------------------------------------------------------
// CRoapHttpHandler::AppendData()
// ---------------------------------------------------------
//
TInt CRoapHttpHandler::AppendPduData( const TDesC8& aDataChunk )
    {
    LOG( _L("CRoapHttpHandler::AppendData") );

    TInt needed = iResponse->iPdu->Des().Length() + aDataChunk.Length();
    if ( iResponse->iPdu->Des().MaxLength() < needed )
        {
        HBufC8* buf = iResponse->iPdu->ReAlloc( needed );
        if ( buf )
            {
            iResponse->iPdu = buf;
            }
        else
            {
            return KErrNoMemory;
            }
        }
    iResponse->iPdu->Des().Append( aDataChunk );
    return KErrNone;
    }


// ---------------------------------------------------------
// CRoapHttpHandler::AppendMultipartData()
// ---------------------------------------------------------
//
TInt CRoapHttpHandler::AppendMultipartData( const TDesC8& aDataChunk  )
    {
//    LOG( _L("CRoapHttpHandler::AppendMultipartData") );
    TInt r = KErrNone;
    TRAP( r, iResponse->AppendMultipartDataL( aDataChunk ) );
    return r;
    }


// -----------------------------------------------------------------------------
// CRoapHttpHandler::StaticTimeOut()
// -----------------------------------------------------------------------------
//
TInt CRoapHttpHandler::StaticTimeOut( TAny* aPointer )
    {
    CRoapHttpHandler* itself = STATIC_CAST(CRoapHttpHandler*, aPointer);
    if ( itself )
        {
        itself->TimeOut();
        }
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CRoapHttpHandler::TimeOut()
// -----------------------------------------------------------------------------
//
void CRoapHttpHandler::TimeOut()
    {
    LOG( _L("CRoapHttpHandler::TimeOut") );

    iTransaction.Close();
    iError = KErrTimedOut;
    SelfComplete( iError );
    }


// ---------------------------------------------------------
// CRoapHttpHandler::GetNextDataPart()
// ---------------------------------------------------------
//
TBool CRoapHttpHandler::GetNextDataPart( TPtrC8& aDataPart )
    {
    LOG( _L("CRoapHttpHandler::GetNextDataPart") );

    aDataPart.Set( iRequestBody->Des() );
    return ETrue;
    }

// ---------------------------------------------------------
// CRoapHttpHandler::ReleaseData()
// ---------------------------------------------------------
//
void CRoapHttpHandler::ReleaseData()
    {
    LOG( _L("CRoapHttpHandler::ReleaseData") );
    }

// ---------------------------------------------------------
// CRoapHttpHandler::OverallDataSize()
// ---------------------------------------------------------
//
TInt CRoapHttpHandler::OverallDataSize()
    {
    LOG( _L("CRoapHttpHandler::OverallDataSize") );

    return iRequestBody->Des().Size();
    }

// ---------------------------------------------------------
// CRoapHttpHandler::Reset()
// ---------------------------------------------------------
//
TInt CRoapHttpHandler::Reset()
    {
    LOG( _L("CRoapHttpHandler::Reset") );

    return KErrNone;
    }


// ---------------------------------------------------------
// CRoapHttpHandler::GetCredentialsL()
// ---------------------------------------------------------
//
TBool CRoapHttpHandler::GetCredentialsL
        (
        const TUriC8& /*aURI*/,
        RString /*aRealm*/,
        RStringF aAuthenticationType,
        RString& aUsername,
        RString& aPassword
        )
    {
    LOG( _L( "CRoapHttpHandler::GetCredentialsL") );
    TBuf8<64> b;

    HBufC8* username = NULL;
    HBufC8* password = NULL;

    b.Copy(aAuthenticationType.DesC());
    LOG(b);

    username = KTestUserName().AllocLC();
    aUsername = iSession.StringPool().OpenStringL( *username );
    CleanupStack::PopAndDestroy( username );

    password = KTestPassword().AllocLC();
    aPassword = iSession.StringPool().OpenStringL( *password );
    CleanupStack::PopAndDestroy( password );

    return ETrue;
    }
