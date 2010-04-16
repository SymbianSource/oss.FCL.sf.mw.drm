/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the DRMHelperServer Client
*                session functionality
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <e32math.h>
#include "DRMHelperCommon.h"
#include "DRMHelperServer.h"
#include "rdrmhelperclient.h"

// LOCAL CONSTANTS AND MACROS

// Number of message slots to reserve for this client server session.
// Since we only communicate synchronously here, we never have any
// outstanding asynchronous requests.

const TUint KDefaultMessageSlots = 0;
const TUid KServerUid3 = {0x101F6DC5};


// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// AutomatedServiceTypeToHs Converts AutomatedUsage service type
//                           to HelperServer.
// -----------------------------------------------------------------------------
//
LOCAL_C TInt AutomatedServiceTypeToHs(
    const DRM::TDrmAutomatedServiceType aServiceType,
    TInt& aHsServiceType )
    {
    if ( aServiceType == DRM::EAUActive )
        {
        aHsServiceType = CDRMHelperServer::EActive;
        return KErrNone;
        }
    if ( aServiceType == DRM::EAUPassive )
        {
        aHsServiceType = CDRMHelperServer::EPassive;
        return KErrNone;
        }
  return KErrArgument;
  }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RDrmHelperClient::RDrmHelperClient
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
RDrmHelperClient::RDrmHelperClient() : RSessionBase()
    {
    }

// -----------------------------------------------------------------------------
// RDrmHelperClient::Connect
//
// Connect to the server session
// -----------------------------------------------------------------------------
//
TInt RDrmHelperClient::Connect()
    {
    TInt error = StartServer();

    if ( !error )
        {
        error = CreateSession( KDRMHelperServerName,
                               Version(),
                               KDefaultMessageSlots );
        }
    return error;
    }

// -----------------------------------------------------------------------------
// RDrmHelperClient::Version
//
// return server version
// -----------------------------------------------------------------------------
//
TVersion RDrmHelperClient::Version() const
    {
    return ( TVersion( KDRMHSMajorVersionNumber,
                       KDRMHSMinorVersionNumber,
                       KDRMHSBuildVersionNumber ) );
    }

// -----------------------------------------------------------------------------
// RDrmHelperClient::SetAutomated
//
// Register one content uri to the helper server
// -----------------------------------------------------------------------------
//
TInt RDrmHelperClient::SetAutomated(
    const TDesC8& aUri,
    const DRM::TDrmAutomatedType aAutomatedType,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedServiceType aServiceType ) const
    {
    TInt ret( KErrNone );
    TInt drmHsServiceType( 0 );

    ret = AutomatedServiceTypeToHs( aServiceType, drmHsServiceType );
    if ( ret )
        {
        return ret;
        }

    // Create descriptor to enable copying data between
    // client and server. Note: This can be local since
    // this is a synchronous call.
    // Note : Using TPtr8 since this is binary information
    TPtrC8 descriptor( aUri );

    // This call waits for the server to complete the request before
    // proceeding.
    return SendReceive( ERegister,
                        TIpcArgs( aIntent,
                                  drmHsServiceType,
                                  aAutomatedType,
                                  &descriptor ) );
    }

// -----------------------------------------------------------------------------
// RDrmHelperClient::RemoveAutomated
//
// Unregister one content uri
// -----------------------------------------------------------------------------
//
TInt RDrmHelperClient::RemoveAutomated(
    const TDesC8& aUri,
    const DRM::TDrmAutomatedType aAutomatedType,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedServiceType aServiceType ) const
    {
    TInt ret( KErrNone );
    TInt drmHsServiceType( 0 );

    ret = AutomatedServiceTypeToHs( aServiceType, drmHsServiceType );
    if( ret )
        {
        return ret;
        }

    // Create descriptor to enable copying data between
    // client and server. Note: This can be local since
    // this is a synchronous call.
    // Note : Using TPtr8 since this is binary information
    TPtrC8 descriptor( aUri );

    // This call waits for the server to complete the request before
    // proceeding.
    ret = SendReceive( ERemove,
                       TIpcArgs( aIntent,
                                 drmHsServiceType,
                                 aAutomatedType,
                                 &descriptor ) );

    if ( ret == KErrNotFound )
        {
        // content was never actually registered
        ret = KErrNone;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// RDrmHelperClient::RemoveAutomatedAll
//
// Unregister one content uri
// -----------------------------------------------------------------------------
//
TInt RDrmHelperClient::RemoveAutomatedAll(
    const TDesC8& aUri,
    const DRM::TDrmAutomatedType aAutomatedType,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedServiceType aServiceType ) const
    {
    TPtrC8 descriptor( aUri );
    TInt ret( KErrNone );
    TInt drmHsServiceType( 0 );

    ret = AutomatedServiceTypeToHs( aServiceType, drmHsServiceType );
    if ( ret )
        {
        return ret;
        }

    TBool automated( EFalse );

    // This call waits for the server to complete the request before
    // proceeding.
    ret = SendReceive( ERemove,
                       TIpcArgs( aIntent,
                                 drmHsServiceType,
                                 aAutomatedType,
                                 &descriptor ) );

    ret = IsAutomated( aUri,
                       aAutomatedType,
                       aIntent,
                       aServiceType,
                       automated );

    while ( automated )
        {
        // unregister all
        ret = SendReceive( ERemove,
                           TIpcArgs( aIntent,
                                     drmHsServiceType,
                                     aAutomatedType,
                                     &descriptor ) );

        IsAutomated( aUri, aAutomatedType, aIntent, aServiceType, automated );
        }

    if ( ret == KErrNotFound )
        {
        ret = KErrNone;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// RDrmHelperClient::IndicateIdle
// -----------------------------------------------------------------------------
//
TInt RDrmHelperClient::IndicateIdle() const
    {
    return SendReceive( EIndicateIdle, TIpcArgs() );
    }

// -----------------------------------------------------------------------------
// RDrmHelperClient::IsAutomated
// -----------------------------------------------------------------------------
//
TInt RDrmHelperClient::IsAutomated(
    const TDesC8& aUri,
    const DRM::TDrmAutomatedType aAutomatedType,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedServiceType aServiceType,
    TBool& aAutomated ) const
    {
    TPtr8 flag( reinterpret_cast< TUint8* >( &aAutomated ), 0, sizeof( TInt ) );
    TInt ret( KErrNone );
    TInt drmHsServiceType( 0 );

    ret = AutomatedServiceTypeToHs( aServiceType, drmHsServiceType );
    if ( ret )
        {
        return ret;
        }

    TInt type( CDRMHelperServer::EActive );

    // Create descriptor to enable copying data between
    // client and server. Note: This can be local since
    // this is a synchronous call.
    // Note : Using TPtr8 since this is binary information
    TPtrC8 descriptor( aUri );

    // This call waits for the server to complete the request before
    // proceeding.
    ret = SendReceive( EIsRegistered,
                       TIpcArgs( aIntent,
                                 type,
                                 aAutomatedType,
                                 &descriptor ) );
    if ( !ret )
        {
        type = CDRMHelperServer::EPassive;

        ret = SendReceive( EIsRegistered,
                           TIpcArgs( aIntent,
                                     type,
                                     aAutomatedType,
                                     &descriptor ) );
        }
    aAutomated = ret ? ETrue : EFalse;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RDrmHelperClient::StartServer
// -----------------------------------------------------------------------------
//
TInt RDrmHelperClient::StartServer()
    {
    TInt result(0);

    TFindServer findHelperServer( KDRMHelperServerName );
    TFullName name;

    result = findHelperServer.Next( name );
    if ( !result )
        {
        // Server already running
        return KErrNone;
        }

    RSemaphore semaphore;
    result = semaphore.CreateGlobal( KDRMHelperServerSemaphoreName, 0 );
    if ( result )
        {
        return result;
        }

    result = CreateServerProcess();
    if ( result )
        {
        return result;
        }

    semaphore.Wait();
    semaphore.Close();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RDrmHelperClient::CreateServerProcess
// -----------------------------------------------------------------------------
//
TInt RDrmHelperClient::CreateServerProcess()
    {
    TInt result( 0 );
    const TUidType serverUid( KNullUid, KNullUid, KServerUid3 );

    RProcess server;
    result = server.Create( KDRMHSServerFileName, _L(""), serverUid );
    if ( result )
        {
        return result;
        }

    server.Resume();
    server.Close();

    return KErrNone;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File
