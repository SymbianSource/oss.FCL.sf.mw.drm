/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements generic HTTP GET and POST functionality
*
*/


// INCLUDE FILES
#include <http.h>
#include <cookiefilterinterface.h>
#include <uaproffilter_interface.h>
#include "wmdrmdladefaulthttpmanager.h"

#define _LOGGING_FILE L"wmdrmdladefaulthttpplugin.txt"
#include "logfn.h"

/**
 *  HTTP_STRING macro
 *  To use HTTP_STRING macro you need a variable 'pool' defined (of
 *  RStringPool type).
 *  @param  aStringId  - an ID for string from HTTP Client's string table
 *  (e.g. 'HTTP::EAccept')
 */
#define HTTP_STRING(aStringId)pool.StringF(aStringId, RHTTPSession::GetTable())

/**
 *  SET_HTTP_PROPERTY macro
 *  Sets value for a property of HTTP session
 *  @param  info    - RHTTPConnectionInfo
 *  @param  pool    - RStringPool
 *  @param  property - property ID, a value from HTTP string table
 *  @param  value   - value to set
 */
#define SET_HTTP_PROPERTY(info, pool, property, value) \
                info.SetPropertyL( HTTP_STRING(property), THTTPHdrVal(value))


// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::NewL
// ----------------------------------------------------------------------------
CWmDrmDlaDefaultHttpManager* CWmDrmDlaDefaultHttpManager::NewL(
    MWmDrmDlaDefaltHttpManagerObserver& aObserver )
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::NewL(1)" );
    CWmDrmDlaDefaultHttpManager* self =
        new(ELeave) CWmDrmDlaDefaultHttpManager( aObserver, 0 );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::NewL
// ----------------------------------------------------------------------------
CWmDrmDlaDefaultHttpManager* CWmDrmDlaDefaultHttpManager::NewL(
    MWmDrmDlaDefaltHttpManagerObserver& aObserver,
    TUint32 aIapNumber )
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::NewL(2)" );
    CWmDrmDlaDefaultHttpManager* self =
        new(ELeave) CWmDrmDlaDefaultHttpManager( aObserver, aIapNumber );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::~CWmDrmDlaDefaultHttpManager
// ----------------------------------------------------------------------------
CWmDrmDlaDefaultHttpManager::~CWmDrmDlaDefaultHttpManager()
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::~CWmDrmDlaDefaultHttpManager" );
    Stop();
    }

// ----------------------------------------------------------------------------
// CpHttpManager::Get
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::Get(
    const TDesC8& aUrl,
    const RArray<CWmDrmDlaDefaultHttpManager::THeader>& aHeaders)
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::Get" );
    // Disallow clients to call this method before it has completed asynch
    // request
    ASSERT( !iTransactionOpen );

    iOperation = EGet;
    iDataSupplier = NULL;

    TRAPD( err, DoStartL( aUrl, aHeaders ) );
    if ( err )
        {
        HandleDownloadComplete( err );
        }
    }

// ----------------------------------------------------------------------------
// CpHttpManager::Post
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::Post(
    const TDesC8& aUrl,
    const RArray<CWmDrmDlaDefaultHttpManager::THeader>& aHeaders,
    MHTTPDataSupplier* aDataSupplier )
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::Post" );
    // Disallow clients to call this method before it has completed asynch
    // request
    ASSERT( !iTransactionOpen );
    ASSERT( aDataSupplier );

    iOperation = EPost;
    iDataSupplier = aDataSupplier;

    TRAPD( err, DoStartL( aUrl, aHeaders ) );
    if ( err )
        {
        HandleDownloadComplete( err );
        }
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::Stop
// This method should be used instead of Cancel.
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::Stop()
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::Stop" );
    // Make sure Stop was not called from a callback
    ASSERT( !iInCallback );

    if ( IsActive() )
        {
        // We are still establishing the connection, cancel the active object
        Cancel();
        }
    else
        {
        // The active object is not active - we are past the setup phase
        if ( iTransactionOpen )
            {
            iHttpTransaction.Cancel();

            // Only generate the callback if there was a transaction going on,
            // otherwise the client will get a callback without previously
            // calling StartL
            HandleDownloadComplete( KErrCancel );
            }
        }
	// Close the HTTP session in use
    iHttpSession.Close();

    CleanupConnection();
    DeleteUsernamePassword();
    iState = EConstructed;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::MoreDataAvailableL
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::MoreDataAvailableL()
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::MoreDataAvailableL" );
    iHttpTransaction.NotifyNewRequestBodyPartL();
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::SetKeepAlive
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::SetKeepAlive( TBool aKeepAlive )
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::SetKeepAlive" );
    iKeepAlive = aKeepAlive;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::KeepAlive
// ----------------------------------------------------------------------------
TBool CWmDrmDlaDefaultHttpManager::KeepAlive() const
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::KeepAlive" );
    return iKeepAlive;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::GetConnectionInfoL
// Note: TConnectionInfo& aConnectionInfo creates a PC-Lint warning to make it
// const but aConnectionInfo is being changed after it has been put into a
// package and GetConnectionInfo is called on the connection
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::GetConnectionInfoL(
    TConnectionInfo& aConnectionInfo )
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::GetConnectionInfoL" );
    if ( (iState != EOpen) && (iState != ESubmit) )
        {
        User::Leave( KErrNotReady );
        }

    TUint count( 0 );
    User::LeaveIfError( iConnection.EnumerateConnections( count ) );

    if ( !count )
        {
        User::Leave( KErrNotReady );
        }

    TPckg<TConnectionInfo> pkg( aConnectionInfo );
    User::LeaveIfError( iConnection.GetConnectionInfo( 1, pkg ) );
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::SetIapId
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::SetIapId( TInt aIapId )
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::SetIapId" );
    iIapNumber = aIapId;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::IapId
// ----------------------------------------------------------------------------
TInt CWmDrmDlaDefaultHttpManager::IapId()
    {
    return iIapNumber;
    }

// ============================== PRIVATE METHODS ==============================

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::MHFRunL
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::MHFRunL(
    RHTTPTransaction aTransaction,
    const THTTPEvent& aEvent )
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::MHFRunL" );
    RHTTPResponse response;
    TPtrC8 dataChunk;

    // Either ESucceeded or EFailed will eventually occur
    switch ( aEvent.iStatus )
        {
        case THTTPEvent::EGotResponseHeaders:
            response = aTransaction.Response();

            iInCallback = ETrue;
            iObserver.OnResponseHeadersL(
                response,
                response.GetHeaderCollection(),
                iHttpSession.StringPool(),
                response.StatusCode() );

            break;

        case THTTPEvent::EGotResponseBodyData:
            // A member variable is used to store the body to avoid two
            // potential problems:
            // - OnResponseBodyDataL leaves
            // - Stop is called from within OnResponseBodyDataL
            iBody = aTransaction.Response().Body();
            User::LeaveIfNull( iBody );

            iBody->GetNextDataPart( dataChunk );

            iInCallback = ETrue;
            iObserver.OnResponseBodyDataL( dataChunk );

            // Verify that iBody wasn't already released
            // for example by calling Stop within ResponseBodyDataL
            if ( iBody )
                {
                iBody->ReleaseData();
                iBody = NULL;
                }

            break;

        case THTTPEvent::ESucceeded:
        case THTTPEvent::EFailed:
            // Deal with both the same as iError will either be negative or
            // KErrNone
            // If the user cancelled the credentials dialog then make sure we
            // return KErrCancel
            HandleDownloadComplete( iCredentialsOk ? iError : KErrCancel );
            break;

        default:
            // This will capture system and HTTP lib errors
            // For positive codes iError will remain to KErrNone
            if ( aEvent.iStatus < KErrNone )
                {
                iError = aEvent.iStatus;
                }
            break;
        }

    iInCallback = EFalse;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::MHFRunError
// ----------------------------------------------------------------------------
TInt CWmDrmDlaDefaultHttpManager::MHFRunError(
    TInt aError,
    RHTTPTransaction /*aTransaction*/,
    const THTTPEvent& /*aEvent*/ )
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::MHFRunError" );
    HandleDownloadComplete( aError );
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::GetCredentialsL
// This function is NOT currently used as authentication is not current used in
// this project at the current time
// ----------------------------------------------------------------------------
TBool CWmDrmDlaDefaultHttpManager::GetCredentialsL(
    const TUriC8& /*aURI*/,
    RString aRealm,
    RStringF aAuthenticationType,
    RString& aUsername,
    RString& aPassword )
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::GetCredentialsL" );
    iCredentialsOk = EFalse;
    RStringPool pool = aRealm.Pool();
    if ( aAuthenticationType == HTTP_STRING( HTTP::EBasic ) )
        {
        DeleteUsernamePassword();

        // Get the username/password
        iInCallback = ETrue;
        iCredentialsOk =
            iObserver.OnGetUsernamePasswordL( iUsername, iPassword );
        iInCallback = EFalse;

        // authentication = iCredentialsOk && iUsername && iPassword
        // no authentication = !iCredentialsOk && !iUsername && !iPassword
        ASSERT( (iCredentialsOk && iUsername && iPassword) ||
            (!iCredentialsOk && !iUsername && !iPassword ) );

        if (iCredentialsOk)
            {
            aUsername = pool.OpenStringL( *iUsername );
            aPassword = pool.OpenStringL( *iPassword );
            }
        }

    // If the authentication type is not Basic, we simply return EFalse.
    return iCredentialsOk;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::RunL
// Overrides CActive::RunL
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::RunL()
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::RunL" );
    TInt error( iStatus.Int() );
    LOG3( "CWmDrmDlaDefaultHttpManager State: %d Status: %d", iState, error );

    if ( iState == EOpen && error == KErrNotFound )
        {
        iState=EOpenFailed;
        }
    else
        {
        User::LeaveIfError( error );
        }

    switch (iState)
        {
        case EStart:
            InitializeL();
            break;
        case EInitialize:
            OpenL();
            break;
        case EOpenFailed: // Called only if open fails
            ReconnectL();
            break;
        case EReconnect:
            iState=EOpen;
            // Note: intentionally no break
            // Successfully completed EReconnect is same as EOpen.
        case EOpen:
            SubmitL();
            break;
        default:
            // Panic if called while in ESubmit
            ASSERT( 0 );
            break;
        }

    // Do not advance the state if the transaction was submitted
    // MHFRunL will be called by the HTTP stack while the transaction
    // progresses
    if ( iState != ESubmit )
        {
        // Go to the next state if not finished
        CompleteSelf();
        }
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::DoCancel
// Called while the state machine is still active. Possible states:
//
//  - EStart and EInitialize: iStatus is already completed and CActive::Cancel
//    will not block
//  - ESubmit: this method won't be called since the AO is not active.
//  - EOpen: RConnection::Close will be called from CleanupTransaction,
//    which will complete the RConnection::Start asynchronous call.
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::DoCancel()
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::DoCancel" );
    ASSERT( iState != ESubmit );

    // Cleanup and generate the callback
    HandleDownloadComplete( KErrCancel );
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::RunError
// Overrides CActive::RunError
// ----------------------------------------------------------------------------
TInt CWmDrmDlaDefaultHttpManager::RunError( TInt aError )
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::RunError" );
    LOG2( "aError: %d", aError );
    // Cleanup and generate the callback
    HandleDownloadComplete( aError );
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CpHttpManager::CWmDrmDlaDefaultHttpManager
// ----------------------------------------------------------------------------
CWmDrmDlaDefaultHttpManager::CWmDrmDlaDefaultHttpManager(
    MWmDrmDlaDefaltHttpManagerObserver& aObserver,
    TUint32 aIapNumber ) :
    CActive( CActive::EPriorityStandard ),
    iObserver( aObserver ),
    iState( EConstructed ),
    iIapNumber( aIapNumber ),
    iKeepAlive( EFalse )
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::CWmDrmDlaDefaultHttpManager" );
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CpHttpManager::ConstructL
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::ConstructL()
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::ConstructL" );
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::InitializeL
// Handler for the EStart state.
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::InitializeL()
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::InitializeL" );
    User::LeaveIfError( iSocketServer.Connect() );
    User::LeaveIfError( iConnection.Open( iSocketServer, KAfInet ) );
    iState = EInitialize;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::OpenL
// Handler for the EInitialize state.
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::OpenL()
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::OpenL" );
    // Override dialog preferences
    // Use if IAP is provided, override the default one
    if ( iIapNumber )
        {
        iExtPrefs.SetNoteBehaviour(
            TExtendedConnPref::ENoteBehaviourConnSilent );
        iExtPrefs.SetIapId( iIapNumber );
        }
    else
        {
        iExtPrefs.SetSnapPurpose( CMManager::ESnapPurposeInternet );
        iExtPrefs.SetNoteBehaviour(
            TExtendedConnPref::ENoteBehaviourDefault );
        }
    iPrefList.AppendL( &iExtPrefs );

    iConnection.Start( iPrefList, iStatus );

    SetActive();
    LOG1( "CWmDrmDlaDefaultHttpManager::OpenL - Socket connection started" );

    iState = EOpen;
    }

// ----------------------------------------------------------------------------
// CHttpManager::ReconnectL
// Handler for the EOpenFailed state.
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::ReconnectL()
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::ReconnectL" );

    // Clear values set by OpenL
    for ( TInt i( 0 ); i<iPrefList.Count(); ++i )
        {
        iPrefList.Remove(i);
        }

    // Set as dialog based connection opening Any connection will do.
    iExtPrefs.SetIapId( 0 );
    iExtPrefs.SetSnapId( 0 );
    iExtPrefs.SetSnapPurpose( CMManager::ESnapPurposeUnknown );
    iExtPrefs.SetConnSelectionDialog( ETrue );
    iExtPrefs.SetNoteBehaviour(
        TExtendedConnPref::ENoteBehaviourDefault );
    iExtPrefs.SetForcedRoaming( EFalse );
    iExtPrefs.SetBearerSet( TExtendedConnPref::EExtendedConnBearerUnknown );


    iPrefList.AppendL( &iExtPrefs );
    iConnection.Start( iPrefList, iStatus );

    SetActive();
    LOG1( "CHttpManager::ReconnectL - Socket connection started" );

    iState = EReconnect;

    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::SubmitL
// Handler for the states EOpen and EReconnect.
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::SubmitL()
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::SubmitL" );

    TConnectionInfo info;
    GetConnectionInfoL( info );
    iIapNumber = info.iIapId;

    // Close old session:
    iHttpSession.Close();    

    // Open session
    iHttpSession.OpenL();
    RStringPool pool = iHttpSession.StringPool();

    // Associate HTTP session with connection
    RHTTPConnectionInfo connInfo = iHttpSession.ConnectionInfo();

    // Specify socket server
    SET_HTTP_PROPERTY( connInfo, pool, HTTP::EHttpSocketServ,
        iSocketServer.Handle() );
    // Specify connectionn to use
    TInt connPtr = reinterpret_cast<TInt>(&iConnection);
    SET_HTTP_PROPERTY( connInfo, pool, HTTP::EHttpSocketConnection, connPtr );

    // Install HTTP authentication filter
    InstallAuthenticationL( iHttpSession );

    CHttpCookieFilter::InstallFilterL( iHttpSession );

    RHTTPFilterCollection filterColl = iHttpSession.FilterCollection();
    filterColl.RemoveFilter(
        iHttpSession.StringPool().StringF(
            HTTP::ERedirect, RHTTPSession::GetTable() ) );

    CHttpUAProfFilterInterface::InstallFilterL( iHttpSession );

    // Parse URI
    TUriParser8 srcAddress;
    User::LeaveIfError( srcAddress.Parse( *iSrcAddress ) );

    // Open HTTP transaction
    iHttpTransaction = iHttpSession.OpenTransactionL( srcAddress, *this,
        HTTP_STRING( (EGet == iOperation) ? HTTP::EGET : HTTP::EPOST ) );
    iTransactionOpen = ETrue;

    // Set the data supplier if a POST operation
    if ( EPost == iOperation )
        {
        iHttpTransaction.Request().SetBody( *iDataSupplier );
        }


    TInt pos = iSrcAddress->Locate( '?' );

    // If no query ('?') pos is rightmost character
    pos = (pos != KErrNotFound) ? pos : iSrcAddress->Length();
    TPtrC8 ptrUrl = iSrcAddress->Left( pos );

    // Only print if there is a ('?') and something to print after it
    if ( pos < iSrcAddress->Length() )
      {
      TPtrC8 ptrQs = iSrcAddress->Mid( pos );
      }
    // Set HTTP headers for the transaction
    RHTTPHeaders hdr = iHttpTransaction.Request().GetHeaderCollection();
    TInt nofheaders = iHdrValues.Count();

    for ( TInt i = 0; i < nofheaders; i++ )
        {
        TInt field = iHdrFields[i];
        const TDesC8& fieldStr = HTTP_STRING( field ).DesC();
        HBufC8* pValue = iHdrValues[i];
        SetHeaderL( hdr, field, *pValue );
        }

    // Submit HTTP transaction
    // This will set in motion the HTTP download
    iHttpTransaction.SubmitL();
    iState = ESubmit;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::DoStartL
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::DoStartL(
    const TDesC8& aUrl,
    const RArray<CWmDrmDlaDefaultHttpManager::THeader>& aHeaders )
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::DoStartL" );
    CleanupTransaction();

    // Store callers URL
    iSrcAddress = aUrl.AllocL();

    // Store headers
    TInt nofheaders = aHeaders.Count();
    for ( TInt i = 0; i < nofheaders; i++ )
        {
        iHdrFields.AppendL( aHeaders[i].iField );
        iHdrValues.AppendL( aHeaders[i].iVal.AllocL() );
        }

    if ( (iState == EOpen) && iKeepAlive )
        {
        TConnectionInfo connectionInfo;
        TRAPD(err, GetConnectionInfoL(connectionInfo) );
        if ( err || ( connectionInfo.iIapId != iIapNumber &&
             iIapNumber != 0 && connectionInfo.iIapId != 0 ) )
            {
            CleanupConnection();
            iState = EStart;
            }
        }
    else
        {
        iState = EConstructed == iState ? EStart : EOpen;
        }

    iError = KErrNone;
    iCredentialsOk = ETrue;

    CompleteSelf();
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::HandleDownloadComplete
// Close HTTP connection and clean up instance variables.
//
// Must be called to complete client's request and cleanup, either on
// successful download, or some error condition
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::HandleDownloadComplete( TInt aError )
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::HandleDownloadComplete" );
    LOG2( "aError: %d", aError );
    CleanupTransaction();

    iInCallback = ETrue;
    iObserver.OnTransactionComplete( aError );
    iInCallback = EFalse;

    if ( iDataSupplier )
        {
        iDataSupplier->ReleaseData();
        iDataSupplier = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::CleanupTransaction
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::CleanupTransaction()
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::CleanupTransaction" );
    if ( iBody )
        {
        iBody->ReleaseData();
        iBody = NULL;
        }

    // Release HTTP transaction & session resources
    delete iSrcAddress;
    iSrcAddress = NULL;

    TInt nofheaders = iHdrValues.Count();
    for ( TInt i = 0; i < nofheaders; i++ )
        {
        delete iHdrValues[i];
        }

    // set to empty
    iHdrValues.Reset();
    iHdrFields.Reset();

    iHttpTransaction.Close();

    if ( !iKeepAlive )
        {
        CleanupConnection();
        iState = ESubmit == iState ? EConstructed : iState;
        }
    else if ( ESubmit == iState )
        {
        // end of a transaction
        iState = EOpen;
        }
    else
        {
        // do nothing. This is to get rid of a PC-Lint warning
        }

    iTransactionOpen = EFalse;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::CleanupConnection
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::CleanupConnection()
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::CleanupConnection" );
    iConnection.Close();
    iSocketServer.Close();
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::CompleteSelf
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::CompleteSelf()
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::CompleteSelf" );
    if ( !IsActive() ) // else already waiting
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::SetHeaderL
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::SetHeaderL(
    RHTTPHeaders& aHeaders,
    TInt aHdrField,
    const TDesC8& aHdrValue ) const
    {
    RStringPool pool = iHttpSession.StringPool();
    RStringF valStr = pool.OpenFStringL( aHdrValue );
    CleanupClosePushL( valStr );
    THTTPHdrVal val( valStr );
    aHeaders.SetFieldL( HTTP_STRING( aHdrField ), val );
    CleanupStack::PopAndDestroy( &valStr ); // Close
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpManager::DeleteUsernamePassword
// This function is NOT currently used as authentication is not current used in
// this project at the current time
// ----------------------------------------------------------------------------
void CWmDrmDlaDefaultHttpManager::DeleteUsernamePassword()
    {
    LOGFN( "CWmDrmDlaDefaultHttpManager::DeleteUsernamePassword" );
    delete iUsername;
    iUsername = NULL;
    delete iPassword;
    iPassword = NULL;
    }

// End of File
