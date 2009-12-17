/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the User Data Transfer module
*
*/


// INCLUDE FILES
#include "DrmUdtHandler.h"
#include "DrmUdtConn.h"
#include "RoapStorageClient.h"
#include "DrmRightsClient.h"
#include "DrmUdtObserver.h"

#include <hash.h>
#include <stringpool.h>
#include <http/thttphdrval.h>
#include <etelmm.h> 
#include <mmtsy_names.h>

#ifdef _DEBUG
#define LOGGING
#endif

#define LOGGING

#ifdef LOGGING
_LIT(KLogDir, "DRM");
_LIT(KLogName, "UDT.log");
#include "flogger.h"
#define LOG(string) \
	RFileLogger::Write(KLogDir, KLogName, \
		EFileLoggingModeAppend, string);
#define LOGINT(string, val) \
	RFileLogger::WriteFormat(KLogDir, KLogName, \
		EFileLoggingModeAppend, string, val);
#define LOGHEX(buffer) \
	RFileLogger::HexDump(KLogDir, KLogName, \
		EFileLoggingModeAppend, _S(""), _S(""), \
		buffer.Ptr(), buffer.Length());
#else
#define LOG
#define LOGHEX        
#endif

using namespace Roap;

// ================= CONSTANTS ======================

// The time out value in HTTP, 30 sec
LOCAL_D const TInt KUdtTimeoutValue = 30000000;

LOCAL_D const TInt KMaxSerNumLength = 64;
LOCAL_D const TInt KRdbKeyLength = 256;
LOCAL_D const TInt KVersionSize = 1;
LOCAL_D const TInt KMessageIdSize = 1;
LOCAL_D const TInt KLengthSize = 4;
LOCAL_D const TInt KSignatureLength = 128;

LOCAL_D const TInt KVersion = 0;

LOCAL_D const TInt KPadding255 = 1;

_LIT8( KUdtContentType, "application/binary" );

// UDT message identifiers
LOCAL_D const TUint8 KUdtRequestId = 0;
LOCAL_D const TUint8 KUdtResponseId = 1;
LOCAL_D const TUint8 KStatusResponseId = 3;
LOCAL_D const TUint8 KErrorResponseId = 4;
LOCAL_D const TUint8 KServerErrorValue = 0;
LOCAL_D const TUint8 KClientErrorValue = 1;


LOCAL_D const TInt KUdtResponseSize = 129;

NONSHARABLE_STRUCT( TUnloadModule )
    {
    RTelServer* iServer;
    const TDesC* iName;
    };

// ================= LOCAL FUNCTIONS =========================

LOCAL_C void WriteIntToBlock( TInt aValue, TDes8& aBlock, TInt aOffset )
    {
    aBlock.SetLength(4);
    aBlock[aOffset] =     (aValue & 0xff000000) >> 24;
    aBlock[aOffset + 1] = (aValue & 0x00ff0000) >> 16;
    aBlock[aOffset + 2] = (aValue & 0x0000ff00) >> 8;
    aBlock[aOffset + 3] = (aValue & 0x000000ff);
    }

template<class S>
void PointerArrayResetDestroyAndClose(TAny* aPtr)
    {
    (reinterpret_cast<RPointerArray<S>*>(aPtr))->ResetAndDestroy();
    (reinterpret_cast<RPointerArray<S>*>(aPtr))->Close();
    }
    
LOCAL_C void DoUnloadPhoneModule( TAny* aAny )
    {
    __ASSERT_DEBUG( aAny, User::Invariant() );
    TUnloadModule* module = ( TUnloadModule* ) aAny;
    module->iServer->UnloadPhoneModule( *( module->iName ) );
    } 

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDrmUdtHandler::NewL()
// ---------------------------------------------------------
//
EXPORT_C CDrmUdtHandler* CDrmUdtHandler::NewL( )
    {
    LOG( _L("CDrmUdtHandler:NewL:") );
    CDrmUdtHandler* handler = new( ELeave ) CDrmUdtHandler();
    CleanupStack::PushL( handler );
    handler->ConstructL();
    CleanupStack::Pop( handler );
    return handler;
    }

// ---------------------------------------------------------
// CDrmUdtHandler::~CDrmUdtModule()
// ---------------------------------------------------------
//
CDrmUdtHandler::~CDrmUdtHandler()
    {
    LOG( _L("CDrmUdtHandler::~CDrmUdtHandler") );
    Cancel();
	iSession.Close();
	delete iConnection;
	delete iUri;
	delete iTimeout;
    delete iOneTimePassword;
    delete iUdtRequest;
    delete iUdtResponse;
    }
    
// ---------------------------------------------------------
// CDrmUdtHandler::ConstructL()
// ---------------------------------------------------------
//
void CDrmUdtHandler::ConstructL() 
    {
    LOG( _L("CDrmUdtHandler::ConstructL") );
    iConnection = CDrmUdtConn::NewL();
    iTimeout = CPeriodic::NewL( CActive::EPriorityUserInput );
    iRequestType = EUdtRequest;
    iUdtError = EUdtOk;
    iStateInfo.iState = TUdtStateInfo::EUdtNotStarted;
    iStateInfo.iProgress = 0;
    iStateInfo.iError = EUdtOk;
    }
    
// -----------------------------------------------------------------------------
// CDrmUdtHandler::DoUserDataTransferL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CDrmUdtHandler::DoUserDataTransferL( const TDesC8& aOneTimePassword,
                                                   const TDesC8& aServiceUrl,
                                                   MDrmUdtObserver* aObserver,
                                                   TRequestStatus& aStatus )
    {
    LOG( _L("CDrmUdtHandler::DoUserDataTransferL") );
    __ASSERT_ALWAYS( iState == EInit, User::Invariant() );
    
    /*
    1. fetch original RDB data from the rights client (serial number and key)
    2. create UDT package with the original RDB data, the one time password,
       our serial number and our certificate
    3. open a connection to the service URL
    4. do a POST to the service URL, sending our UDT package
    5. receive the anwser with the re-encrypted RDB key
    6. tell the rights client to do a restore, using the re-encrypted RDB key
    7. do a POST to the service URL, sendind a success or error notification
    */

    iOneTimePassword = aOneTimePassword.AllocLC();
    iUri = aServiceUrl.AllocL();
    iObserver = aObserver;
    
    LOG( _L8("Password: ") );
    LOG( aOneTimePassword );
    LOG( _L8("URL: ") );
    LOG( aServiceUrl );
    
    iParentStatus = &aStatus;    
    *iParentStatus = KRequestPending;
    iState = EStart;
    TRequestStatus* ownStatus = &iStatus;    
    *ownStatus = KRequestPending;
    iRequestType = EUdtRequest;
    
    SetActive();
    User::RequestComplete( ownStatus, KErrNone );      
    CleanupStack::Pop(); // iOneTimePassword    
    }
    
// ---------------------------------------------------------
// CDrmUdtHandler::SetPreferredIap()
// ---------------------------------------------------------
EXPORT_C void CDrmUdtHandler::SetPreferredIap( TUint32 aPreferredIap )
    {
    LOG( _L("CDrmUdtHandler::SetPreferredIap") );
    iPreferredIap = aPreferredIap;
    }    

// ---------------------------------------------------------
// CDrmUdtHandler::DoCancel()
// ---------------------------------------------------------
//
void CDrmUdtHandler::DoCancel()
    {
    LOG( _L("CDrmUdtHandler::DoCancel") );
    switch ( iState )
        {
        case EStart:
        case EConnect:
            {
            iConnection->Cancel();
            break;
            }
        case EResponseReceived:
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
// CDrmUdtHandler::RunL()
// ---------------------------------------------------------
//
void CDrmUdtHandler::RunL()
    {
    LOG( _L("CDrmUdtHandler::RunL") );
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
        case ESendMessage:
            {
            SendUdtMessageL();
            break;
            }
        case EResponseReceived:
            {
            ResponseReceivedL();
            break;
            }
        case EComplete:
            {
            iState = EInit;
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
// CDrmUdtHandler::RunError()
// ---------------------------------------------------------
//
TInt CDrmUdtHandler::RunError( TInt aError )
    {
    LOG( _L("CDrmUdtHandler::RunError") );
    iError = aError;
    iState = EInit;
    Complete();
    return KErrNone;
    }

// ---------------------------------------------------------
// CDrmUdtHandler::ConnectL()
// ---------------------------------------------------------
//
void CDrmUdtHandler::ConnectL()
    {
    LOG( _L("CDrmUdtHandler::ConnectL") );
    __ASSERT_ALWAYS( iState == EStart, User::Invariant() );
    
    iConnection->ConnectL( iPreferredIap, iObserver, &iStatus );
    iState = EConnect;
    iError = EUdtOk;
    SetActive();
    }

// ---------------------------------------------------------
// CDrmUdtHandler::CreateSessionL()
// ---------------------------------------------------------
//
void CDrmUdtHandler::CreateSessionL()
    {
    LOG( _L("CDrmUdtHandler::CreateSessionL") );
    __ASSERT_ALWAYS( iState == EConnect, User::Invariant() );
    
    TUint32 ap;

    if( !iConnection->IsConnected( ap ) )
        {
        User::Leave( KErrGeneral );
        }
    
    iSession.Close();
    iSession.OpenL();

    RStringPool strPool = iSession.StringPool();

    // Remove first session properties just in case.
    RHTTPConnectionInfo connInfo = iSession.ConnectionInfo();
    
    // Clear RConnection and Socket Server instances
    connInfo.RemoveProperty(strPool.StringF(HTTP::EHttpSocketServ,RHTTPSession::GetTable()));
    connInfo.RemoveProperty(strPool.StringF(HTTP::EHttpSocketConnection,RHTTPSession::GetTable()));
    
#ifdef __WINS__    
    // Clear the proxy settings
    RStringF proxy;
    proxy = strPool.OpenFStringL(_L8("172.22.168.15"));
    connInfo.SetPropertyL
        ( 
        strPool.StringF( HTTP::EProxyAddress, RHTTPSession::GetTable() ), 
        THTTPHdrVal( proxy ) 
        );
    proxy.Close();
    connInfo.SetPropertyL
        ( 
        strPool.StringF( HTTP::EProxyUsage, RHTTPSession::GetTable() ), 
        THTTPHdrVal( strPool.StringF(HTTP::EUseProxy, RHTTPSession::GetTable() ) )
        );

#else
    THTTPHdrVal proxyUsage(strPool.StringF(HTTP::EUseProxy,RHTTPSession::GetTable()));
    connInfo.RemoveProperty(strPool.StringF(HTTP::EProxyUsage,RHTTPSession::GetTable()));
    connInfo.RemoveProperty(strPool.StringF(HTTP::EProxyAddress,RHTTPSession::GetTable()));
#endif

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
    TRequestStatus* ownStatus = &iStatus;    
    *ownStatus = KRequestPending;
    iState = ESendMessage;
    SetActive();
    User::RequestComplete( ownStatus, KErrNone );
    }


// ---------------------------------------------------------
// CDrmUdtModule::InstallHttpFilters()
// ---------------------------------------------------------
//
void CDrmUdtHandler::InstallHttpFiltersL()
    {
    LOG( _L("CDrmUdtHandler::InstallHttpFiltersL") );
 // CHttpUAProfFilterInterface::InstallFilterL( iSession );
 // CHttpCookieFilter::InstallFilterL( iSession );
 // InstallAuthenticationL( iSession );
 // CHttpFilterProxyInterface::InstallFilterL( iSession );
    }


// ---------------------------------------------------------
// CDrmUdtHandler::SendUdtMessageL()
// ---------------------------------------------------------
//
void CDrmUdtHandler::SendUdtMessageL()
    {
    LOG( _L("CDrmUdtHandler::SendUdtMessageL") );
    __ASSERT_ALWAYS( iState == ESendMessage, User::Invariant() );
    
    TUriParser8 uri;
    
    if ( iRequestType == EUdtRequest )
        {
        CreateUdtRequestL();
        }
    else if( iRequestType == EStatusNotification )
        {
        CreateStatusNotificationL();
        }
    
    User::LeaveIfError( uri.Parse( *iUri ) );
    RStringF POST;
    POST = iSession.StringPool().StringF( HTTP::EPOST, RHTTPSession::GetTable() );
    iTransaction = iSession.OpenTransactionL( uri, *this, POST );

    // Set required headers
    RHTTPHeaders hdrs = iTransaction.Request().GetHeaderCollection();
    
    SetHeaderL(hdrs, HTTP::EAccept, KUdtContentType() );
    
    SetHeaderL(hdrs, HTTP::EContentType, KUdtContentType() );
    
    // Add request body
    MHTTPDataSupplier* ds = this;
    iTransaction.Request().SetBody(*ds);
    
    iTransaction.SubmitL();

    iState = EResponseReceived;
    iStatus = KRequestPending;
    SetActive();

    iTimeout->Cancel();
    iTimeout->Start( KUdtTimeoutValue,
    				 KUdtTimeoutValue,
    				 TCallBack( StaticTimeOut,this ) );
    }
    
    
// ---------------------------------------------------------
// CDrmUdtHandler::CreateUdtRequestL()
// ---------------------------------------------------------
//    
void CDrmUdtHandler::CreateUdtRequestL()
    {
    RRoapStorageClient client;
    RPointerArray< HBufC8 > certChain;
    TCleanupItem listCleanup( PointerArrayResetDestroyAndClose< HBufC8 >,
        &certChain );
    HBufC8* certBlock;
    TInt i;
    TInt n;
    TPtr8 ptr( NULL, 0 );
    TBuf8< sizeof ( TUint32 ) > intBuf;
    TBuf8< KMaxSerNumLength > targetSer;
    TBuf8< KRdbKeyLength > rdb_data;
    TBuf8< KSignatureLength > hash;
    HBufC8* signature = NULL;
    TInt udtVersion;
    CSHA1* hasher = NULL;
    
    LOG( _L("CDrmUdtHandler::CreateUdtRequestL") );
    delete iUdtRequest;
    
    if ( iObserver )
        {
        iStateInfo.iState = TUdtStateInfo::EUdtReguest;
        iStateInfo.iProgress = 0;
        iObserver->UdtProgressInfoL( iStateInfo );
        }
    
    hasher = CSHA1::NewL();
    CleanupStack::PushL( hasher );
    User::LeaveIfError( client.Connect() );
    CleanupClosePushL( client );
    client.SelectTrustedRootL( KNullDesC8 );

    LOG( _L("  Getting cert chain") );
    User::LeaveIfError( client.GetDeviceCertificateChainL( certChain ) );
    CleanupStack::PushL( listCleanup );

    LOG( _L("  Getting UDT data") );
    ReadUdtDataL( targetSer, udtVersion, rdb_data );

    LOG(_L8("RDB data:"));    
    LOGHEX(rdb_data)
    
    n = 0;
    for ( i = 0; i < certChain.Count(); i++ )
        {
        n = n + KLengthSize + certChain[i]->Size();
        }
    certBlock = HBufC8::NewL( n );
    CleanupStack::PushL( certBlock );
    ptr.Set( certBlock->Des() );
    for ( i = 0; i < certChain.Count(); i++ )
        {
        WriteIntToBlock( certChain[i]->Size(), intBuf, 0 );
        ptr.Append( intBuf );
        ptr.Append( *certChain[i] );
        }

    n = KVersionSize + 
        KMessageIdSize + 
        KLengthSize + 
        iOneTimePassword->Size() + 
        KLengthSize +
        certBlock->Size() + 
        KLengthSize +
        targetSer.Size() +
        rdb_data.Size() + 
        KLengthSize + 
        KSignatureLength;
        
    iUdtRequest = HBufC8::NewL( n );
    ptr.Set( iUdtRequest->Des() );
    WriteIntToBlock( n - (KVersionSize + KMessageIdSize + 
                     KLengthSize + KSignatureLength), intBuf, 0 );
    
    ptr.Append( KVersion );             // 1. version
    ptr.Append( KUdtRequestId );        // 2. request id
    ptr.Append( intBuf );               // 3. request length
    ptr.Append( *iOneTimePassword );    // 4. password
    WriteIntToBlock( certBlock->Size(), intBuf, 0 );
    ptr.Append( intBuf );               // 5. ceritificate block length
    ptr.Append( *certBlock );           // 6. ceritificate block
    WriteIntToBlock( targetSer.Size(), intBuf, 0 );
    ptr.Append( intBuf );               // 7. serial number length
    ptr.Append( targetSer );            // 8. original serial number
    ptr.Append( rdb_data );             // 9. RDB data
    WriteIntToBlock( udtVersion, intBuf, 0 );
    ptr.Append( intBuf );               // 10. UDT key version
    
    hasher->Update( ptr );
    hash.Append( 0 );
    hash.Append( KPadding255 );
    for ( i = 2; i < KSignatureLength - SHA1_HASH - 1; i++ )
        {
        hash.Append( 255 );
        }
    hash.Append( 0 );
    hash.Append( hasher->Final() );
    LOG(_L8("Hash:"));
    LOGHEX(hash);
    client.RsaSignL( hash, signature );
    CleanupStack::PushL(signature);
    ptr.Append( *signature );           // 11. signature
    LOG(_L8("Signature:"));
    LOGHEX((*signature));
    
    CleanupStack::PopAndDestroy( 5 );   // certBlock, listCleanup,
                                        // client, hasher, signature
    if ( iObserver )
        {
        iStateInfo.iState = TUdtStateInfo::EUdtReguest;
        iStateInfo.iProgress += 20;
        iObserver->UdtProgressInfoL( iStateInfo );
        }
        
    LOG(_L8("Request:"));
    LOGHEX((*iUdtRequest));
    }
    
    
// ---------------------------------------------------------
// CDrmUdtHandler::CreateStatusNotificationL()
// ---------------------------------------------------------
//    
void CDrmUdtHandler::CreateStatusNotificationL()
    {
    LOG( _L("CDrmUdtHandler::CreateStatusNotificationL") );
    
    if ( iObserver )
        {
        iStateInfo.iState = TUdtStateInfo::EUdtStatusNotification;
        iStateInfo.iProgress += 20;
        iObserver->UdtProgressInfoL( iStateInfo );
        }
    
    delete iUdtRequest;
    iUdtRequest = NULL;
    iUdtRequest = HBufC8::NewL(64);
    TPtr8 ptr = iUdtRequest->Des();
    ptr.Append(0);
    ptr.Append(2);
    ptr.Append(*iOneTimePassword);
    iUdtError == EUdtOk ? ptr.Append(1) : ptr.Append(0);
    }
    
    
// ---------------------------------------------------------
// CDrmUdtHandler::ResponseReceivedL()
// ---------------------------------------------------------
//    
void CDrmUdtHandler::ResponseReceivedL()
    {
    LOG( _L("CDrmUdtHandler::ResponseReceivedL") );
    __ASSERT_ALWAYS( iState == EResponseReceived, User::Invariant() );
    __ASSERT_ALWAYS( iUdtResponse, User::Invariant() );
    
    TPtrC8 udtRespPtr( *iUdtResponse );
    HBufC8* origDBKey = NULL;
    TPtrC8 origDBKeyPtr( KNullDesC8 );
    RDRMRightsClient rightsClient;
    TInt error = EUdtOk;
        
    LOGHEX((*iUdtResponse));
    
    // check response type
    switch ( udtRespPtr[1] )
        {
        case KUdtResponseId:
            {
            if ( iObserver )
                {
                iStateInfo.iState = TUdtStateInfo::EUdtKeyRestore;
                iStateInfo.iProgress += 20;
                iObserver->UdtProgressInfoL( iStateInfo );
                }
            
            if ( udtRespPtr.Length() < KUdtResponseSize )
                {
                User::Leave( KErrCorrupt );
                }
            origDBKeyPtr.Set( udtRespPtr.Mid( 2 ) );
            origDBKey = origDBKeyPtr.AllocLC();
            
            iUdtError = rightsClient.Connect();
            CleanupClosePushL( rightsClient );
            
            if ( !iUdtError )
                {
                iUdtError = rightsClient.InitiateUdt( origDBKeyPtr );
                }
         
            CleanupStack::PopAndDestroy( 2 ); // origDBKey, rightsClient
            
            iRequestType = EStatusNotification;
            iState = ESendMessage;
            iStatus = KRequestPending;
            SetActive();
            SelfComplete( KErrNone );
            break;
            }
        case KStatusResponseId:
            {
            if ( iObserver )
                {
                iStateInfo.iState = TUdtStateInfo::EUdtStatusNotification;
                iStateInfo.iProgress += 20;
                iObserver->UdtProgressInfoL( iStateInfo );
                }
            
            iState = EComplete;
            iStatus = KRequestPending;
            SetActive();
            
            if ( iUdtError )
                {
                error = EUdtKeyRestoreFailed;
                iUdtError = EUdtOk;
                }
            SelfComplete( error );
            break;
            }
        case KErrorResponseId:
            {
            if ( udtRespPtr.Length() >= 3 && udtRespPtr[2] == KClientErrorValue )
                {
                error = EUdtClientError;
                }
            else
                {
                error = EUdtServerError;
                }
            
            iState = EComplete;
            iStatus = KRequestPending;            
            SetActive();
            SelfComplete( error );
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            }
        }  
    }
    
// ---------------------------------------------------------
// CDrmUdtHandler::SetHeaderL()
// ---------------------------------------------------------
//
void CDrmUdtHandler::SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField, const TDesC8& aHdrValue)
	{
    LOG( _L("CDrmUdtHandler::SetHeaderL") );
    RStringF valStr = iSession.StringPool().OpenFStringL(aHdrValue);
    THTTPHdrVal val(valStr);
    aHeaders.SetFieldL(iSession.StringPool().StringF(aHdrField,RHTTPSession::GetTable()), val);
    valStr.Close();
	}
    

// ---------------------------------------------------------
// CDrmUdtHandler::Complete()
// ---------------------------------------------------------
//
void CDrmUdtHandler::Complete()
    {
    LOG( _L("CDrmUdtHandler::Complete") );
    
    delete iUri;
    iUri = NULL;
    delete iUdtResponse;
    iUdtResponse = NULL;
    delete iUdtRequest;
    iUdtRequest = NULL;
    delete iOneTimePassword;
    iOneTimePassword = NULL;
    
    if( iTimeout )
    	{
    	iTimeout->Cancel();
    	}
    	
    if ( iObserver )
        {
        iStateInfo.iState = TUdtStateInfo::EUdtComplete;
        iStateInfo.iProgress = 100;
        iStateInfo.iError = iError;
        TRAPD(ignore, iObserver->UdtProgressInfoL( iStateInfo ));
        }  
    
    User::RequestComplete( iParentStatus, iError );
    iParentStatus = NULL;
    }


// ---------------------------------------------------------
// CDrmUdtHandler::CDrmUdtModule()
// ---------------------------------------------------------
//
CDrmUdtHandler::CDrmUdtHandler(): CActive( CActive::EPriorityStandard )
    {
    LOG( _L("CDrmUdtHandler::CDrmUdtHandler") );
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------
// CDrmUdtHandler::SelfComplete()
// ---------------------------------------------------------
//
void CDrmUdtHandler::SelfComplete( TInt aResult )
    {
    LOG( _L("CDrmUdtHandler::SelfComplete") );
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
// CDrmUdtHandler::MHFRunL()
// ---------------------------------------------------------
//
void CDrmUdtHandler::MHFRunL( RHTTPTransaction  /*aTransaction */, 
								const THTTPEvent& aEvent )
    {
    LOGINT( _L("CDrmUdtHandler::MHFRunL: %d"), aEvent.iStatus );
    iTimeout->Cancel();
    iTimeout->Start( KUdtTimeoutValue,
    				 KUdtTimeoutValue, 
    				 TCallBack( StaticTimeOut,this ) );    				 

    switch ( aEvent.iStatus )
        {
        case THTTPEvent::EGotResponseHeaders:
            {
            HandleResponseHeadersL( iTransaction.Response() );
            break;
            }

        case THTTPEvent::EGotResponseBodyData:
            {
            TInt ret( KErrNone );
            MHTTPDataSupplier* body = iTransaction.Response().Body();
            TPtrC8 ptr;
            body->GetNextDataPart( ptr );
			ret = AppendResponseData( ptr );
            body->ReleaseData();
            User::LeaveIfError( ret );
            break;
            }

        case THTTPEvent::EFailed:
            {
            if ( iError == KErrNone )
                {
                iError = EUdtServerError;
                }
        	iTransaction.Close();
            SelfComplete( iError );
            break;
            }

        case THTTPEvent::ESucceeded:
            {        
            iTransaction.Close();
            SelfComplete( iError );
            break;
            }

        case THTTPEvent::ERedirectRequiresConfirmation:
            {
            iTransaction.SubmitL();
            }

        default:
            {
            if( aEvent.iStatus == KErrHttpRedirectUseProxy )
                {
                }
            else
                {                
                User::LeaveIfError( aEvent.iStatus );
                }
            break;
            }
        }

    }

// ---------------------------------------------------------
// CDrmUdtHandler::MHFRunError()
// ---------------------------------------------------------
//
TInt CDrmUdtHandler::MHFRunError (
        TInt aError,
        RHTTPTransaction /* aTransaction */,
        const THTTPEvent& /* aEvent */
        )
    {
    LOG( _L("CDrmUdtHandler::MHFRunError") );
    iTransaction.Close();
    iError = aError;
    SelfComplete( iError );
    return KErrNone;
    }

// ---------------------------------------------------------
// CDrmUdtHandler::HandleResponseHeadersL()
// ---------------------------------------------------------
//
void CDrmUdtHandler::HandleResponseHeadersL( RHTTPResponse aHttpResponse )
    {
    LOG( _L("CDrmUdtHandler::HandleResponseHeadersL") );
    RHTTPHeaders headers = aHttpResponse.GetHeaderCollection();
    
    TInt httpCode = aHttpResponse.StatusCode();
    TBool status;
    
    status = CheckHttpCode( httpCode );
    
    if ( status )
    	{
        RStringF contentTypeStr;
        THTTPHdrVal contentTypeVal;
        TPtrC8 ptrContentType(KNullDesC8);
		RStringPool srtPool;
		srtPool = iSession.StringPool();
        
        contentTypeStr = srtPool.StringF( HTTP::EContentType, RHTTPSession::GetTable() );										
    	User::LeaveIfError( headers.GetField( contentTypeStr, 0, contentTypeVal ) );
    	
    	if ( contentTypeVal.StrF().DesC().CompareF( KUdtContentType() ) != KErrNone )
    	    {
			User::Leave( KErrNotSupported );
    	    }
       
    	}
    if ( aHttpResponse.HasBody() )
        {
        TInt dataSize = aHttpResponse.Body()->OverallDataSize();
        if ( dataSize >= 0 )
            {
            HBufC8* buf = HBufC8::NewL( dataSize );
            delete iUdtResponse;
            iUdtResponse = buf;
            }
        }
    }
     
// ---------------------------------------------------------
// CDrmUdtHandler::CheckHttpCode()
// ---------------------------------------------------------
//
TBool CDrmUdtHandler::CheckHttpCode( TInt aHttpStatus )
	{
	LOGINT(_L("CDrmUdtHandler::CheckHttpCode: %d"), aHttpStatus);
    if ( HTTPStatus::IsInformational( aHttpStatus ) )
        {
        // 1xx
        // Informational messages.
        iError = EUdtServerError;
        return EFalse;  
        }
    else if ( aHttpStatus == HTTPStatus::EOk ||
              aHttpStatus == HTTPStatus::ENonAuthoritativeInfo )
        {
        // 200 OK
        // 203 Non-Authoritative Information
        iError = EUdtOk;
        return ETrue;        
        }
    else if ( HTTPStatus::IsSuccessful( aHttpStatus ) )
        {
        // 2xx
        // Success codes without an usable body.
        iError = EUdtServerError;
        return EFalse; 
        }
    else if ( aHttpStatus == HTTPStatus::EUnauthorized ||
              aHttpStatus == HTTPStatus::EProxyAuthenticationRequired )
        {
        // 401 Unauthorized
        // 407 Proxy authentication required
        iError = EUdtInvalidServerAddress;
        return EFalse; 
        }
    else if ( aHttpStatus == HTTPStatus::ENotFound ||
              aHttpStatus == HTTPStatus::EGone )
        {
        // 404 Not found
        // 410 Gone
        iError = EUdtInvalidServerAddress;
        return EFalse; 
        }
    else if ( HTTPStatus::IsClientError( aHttpStatus ) )
        {
        // 4xx
        iError = EUdtInvalidServerAddress;
        return EFalse; 
        }
    else if ( aHttpStatus == HTTPStatus::EHTTPVersionNotSupported )
        {
        // 505 HTTP Version Not Supported
        iError = EUdtServerError;
        return EFalse; 
        }
    else if ( HTTPStatus::IsServerError( aHttpStatus ) )
        {
        // 5xx
        iError = EUdtServerError;
        return EFalse; 
        }
    else 
        {
        // Everything else.
        iError = EUdtServerError;
        }
    return EFalse; 
	}


// ---------------------------------------------------------
// CDrmUdtModule::AppendResponseData()
// ---------------------------------------------------------
//
TInt CDrmUdtHandler::AppendResponseData( const TDesC8& aDataChunk )
    {
    LOG( _L("CDrmUdtHandler::AppendResponseData") );
    TInt needed = iUdtResponse->Des().Length() + aDataChunk.Length();
    if ( iUdtResponse->Des().MaxLength() < needed )
        {
        HBufC8* buf = iUdtResponse->ReAlloc( needed );
        if ( buf )
            {
            iUdtResponse = buf;
            }
        else
            {
            return KErrNoMemory;
            }
        }
    iUdtResponse->Des().Append( aDataChunk );
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDrmUdtHandler::StaticTimeOut()
// -----------------------------------------------------------------------------
//
TInt CDrmUdtHandler::StaticTimeOut( TAny* aPointer )
    {  
    LOG( _L("CDrmUdtHandler::StaticTimeOut") );
    CDrmUdtHandler* itself = STATIC_CAST(CDrmUdtHandler*, aPointer);
    if(itself)
        {
        itself->TimeOut();
        }
    return KErrNone;
    }
 
    
// -----------------------------------------------------------------------------
// CDrmUdtHandler::TimeOut()
// -----------------------------------------------------------------------------
//
void CDrmUdtHandler::TimeOut()
    {
    LOG( _L("CDrmUdtHandler::TimeOut") );
    iTransaction.Close();
    iError = KErrTimedOut;
    SelfComplete( iError );
    }


// ---------------------------------------------------------
// CDrmUdtHandler::GetNextDataPart()
// ---------------------------------------------------------
//
TBool CDrmUdtHandler::GetNextDataPart( TPtrC8& aDataPart )
    {
    LOG( _L("CDrmUdtHandler::GetNextDataPart") );
    aDataPart.Set( iUdtRequest->Des() );
    return ETrue;
    }

// ---------------------------------------------------------
// CDrmUdtHandler::ReleaseData()
// ---------------------------------------------------------
//
void CDrmUdtHandler::ReleaseData()
    {
    LOG( _L("CDrmUdtHandler::ReleaseData") );
    }

// ---------------------------------------------------------
// CDrmUdtHandler::OverallDataSize()
// ---------------------------------------------------------
//
TInt CDrmUdtHandler::OverallDataSize()
    {
    LOG( _L("CDrmUdtHandler::OverallDataSize") );
    return iUdtRequest->Des().Size();
    }

// ---------------------------------------------------------
// CDrmUdtHandler::Reset()
// ---------------------------------------------------------
//
TInt CDrmUdtHandler::Reset()
    {
    LOG( _L("CDrmUdtHandler::Reset") );
    return KErrNone;
    }

// ---------------------------------------------------------
// CDrmUdtHandler::ReadUdtDataL
//
// ---------------------------------------------------------
//
void CDrmUdtHandler::ReadUdtDataL(
    TDes8& aTargetSerialNumber,
    TInt& aUdtKeyVersion,
    TDes8& aEncryptedRdbData )
    {
    RDRMRightsClient rightsClient;
    HBufC* serialNum = NULL;

    LOG( _L("CDrmUdtHandler::ReadUdtDataL") );
    User::LeaveIfError( rightsClient.Connect() );
    CleanupClosePushL( rightsClient );
    serialNum = SerialNumberL();
    
    aTargetSerialNumber.Copy( *serialNum );

    aUdtKeyVersion = 0;
    User::LeaveIfError( rightsClient.GetUdtData( aEncryptedRdbData ) );
    if( !aEncryptedRdbData.Length() )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy(); // rightsClient
    }
    
// ---------------------------------------------------------
// CDrmUdtHandler::ReadUdtDataL
//
// ---------------------------------------------------------
//    
HBufC* CDrmUdtHandler::SerialNumberL()
    {
    TInt error( KErrNone );
    TInt count( 0 );
    TInt count2( 0 );
    TUint32 caps( 0 );
    HBufC* imei = NULL;

#ifndef __WINS__
    LOG( _L("CDrmUdtHandler::SerialNumberL") );

    RTelServer etelServer;
    RMobilePhone phone;
    
    for ( TUint8 i = 0; i < 3; ++i )
        {
        error = etelServer.Connect();
        if ( error )
            {
            User::After( TTimeIntervalMicroSeconds32( 100000 ) );
            }
        }
    
    User::LeaveIfError( error );
    CleanupClosePushL( etelServer );

    LOG( _L("  Connected to ETEL") );
    
    User::LeaveIfError( etelServer.LoadPhoneModule( KMmTsyModuleName ) );
    
    LOG( _L("  Phone Module loaded") );

    TUnloadModule unload;
    unload.iServer = &etelServer;
    unload.iName = &KMmTsyModuleName;
    
    TCleanupItem item( DoUnloadPhoneModule, &unload );
    CleanupStack::PushL( item );
    
    User::LeaveIfError( etelServer.EnumeratePhones( count ) );
    
    LOG( _L("  Phones enumerated") );

    for ( count2 = 0; count2 < count; ++count2 )
        {
        RTelServer::TPhoneInfo phoneInfo;
        User::LeaveIfError( etelServer.GetTsyName( count2, phoneInfo.iName ) );
        
        LOG( _L("    Got TSY module") );
        LOG( phoneInfo.iName );
        if ( phoneInfo.iName.CompareF( KMmTsyModuleName ) == 0 )
            {
            User::LeaveIfError( etelServer.GetPhoneInfo( count2, phoneInfo ) );
            LOG( _L("    Got phone info") );
            User::LeaveIfError( phone.Open( etelServer, phoneInfo.iName ) );
            LOG( _L("    Opened phone") );
            CleanupClosePushL( phone );
            break;
            }
        }
        
    if ( count2 == count )
        {
        // Not found.
        LOG( _L("  No phone found") );
        User::Leave( KErrNotFound );
        }
    
    LOG( _L("  Got phone") );

    User::LeaveIfError( phone.GetIdentityCaps( caps ) );

    LOG( _L("  Got Caps") );

    if ( caps & RMobilePhone::KCapsGetSerialNumber )
        {
        RMobilePhone::TMobilePhoneIdentityV1 id;
        TRequestStatus status;
        
        phone.GetPhoneId( status, id );
        User::WaitForRequest( status );
        
        User::LeaveIfError( status.Int() );
        
        imei = id.iSerialNumber.AllocL();
        
        LOG( _L("  Got serial number") );

        CleanupStack::PopAndDestroy( 3 ); // phone, item, etelServer
        
        return imei;
        }
    
    User::Leave( KErrNotFound );
    
    // Never happens...
    return imei;

#else
    _LIT( KDefaultSerialNumber, "123456789123456789" );
    imei = KDefaultSerialNumber().AllocL();
        
    return imei;
#endif
    }
 

