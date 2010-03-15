/*
* Copyright (c) 2004 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the Helper Client session functionality
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <e32math.h>
#include <DRMCommon.h>
#include "DRMHelperCommon.h"
#include "DRMHelperServer.h"
#include "rdrmhelper.h"

// LOCAL CONSTANTS AND MACROS

// Number of message slots to reserve for this client server session.
// Since we only communicate synchronously here, we never have any
// outstanding asynchronous requests.
_LIT( KDRMHelperServerSemaphoreStartingName, "DRMHelperServerSemaphoreStarting" );

const TUint KDefaultMessageSlots = 0;
const TUid KServerUid3 = { 0x101F6DC5 };
_LIT_SECURE_ID( KServerSecureId, 0x101F6DC5 );

#ifdef _DEBUG
    #include <e32svr.h> // RDebug
    #define TRACE( x ) RDebug::Print( _L( x ) )
    #define TRACE2( x, y ) RDebug::Print( _L( x ), y )
    #define TRACE3( x, y, z ) RDebug::Print( _L( x ), y, z )
#else
    #define TRACE( x )
    #define TRACE2( x, y )
    #define TRACE3( x, y, z )
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RDRMHelper::RDRMHelper
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
RDRMHelper::RDRMHelper() : RSessionBase()
    {
    }

// -----------------------------------------------------------------------------
// RDRMHelper::Connect
//
// Connect to the server session
// -----------------------------------------------------------------------------
//
TInt RDRMHelper::Connect()
    {
    TInt error = StartServer();
    TRACE2( "RDRMHelper::Connect() StartServer(): error: %d", error );
    if ( !error )
        {
        error = CreateSession( KDRMHelperServerName, Version(),
            KDefaultMessageSlots );
        TRACE2( "RDRMHelper::Connect() CreateSession(): error: %d", error );
        }
    return error;
    }

// -----------------------------------------------------------------------------
// RDRMHelper::Version
//
// return server version
// -----------------------------------------------------------------------------
//
TVersion RDRMHelper::Version() const
    {
    return( TVersion( KDRMHSMajorVersionNumber, KDRMHSMinorVersionNumber,
        KDRMHSBuildVersionNumber ) );
    }

// -----------------------------------------------------------------------------
// RDRMHelper::SetAutomated
//
// Register one content uri to the helper server
// -----------------------------------------------------------------------------
//
TInt RDRMHelper::SetAutomated(
    const TDesC8& aUri,
    TBool aActive,
    TInt aAutomatedType,
    TInt aIntent ) const
    {
    TInt mode =
        aActive ? CDRMHelperServer::EActive : CDRMHelperServer::EPassive;

    // Create descriptor to enable copying data between
    // client and server. Note: This can be local since
    // this is a synchronous call.
    // Note : Using TPtr8 since this is binary information
    TPtrC8 descriptor( aUri );

    // This call waits for the server to complete the request before
    // proceeding.
    return SendReceive(
        ERegister, TIpcArgs( aIntent, mode, aAutomatedType, &descriptor ) );
    }

// -----------------------------------------------------------------------------
// RDRMHelper::RemoveAutomated
//
// Unregister one content uri
// -----------------------------------------------------------------------------
//
TInt RDRMHelper::RemoveAutomated(
    const TDesC8& aUri,
    TBool aActive,
    TInt aAutomatedType,
    TInt aIntent ) const
    {
    TInt ret( 0 );
    TInt mode(
        aActive ? CDRMHelperServer::EActive : CDRMHelperServer::EPassive );

    // Create descriptor to enable copying data between
    // client and server. Note: This can be local since
    // this is a synchronous call.
    // Note : Using TPtr8 since this is binary information
    TPtrC8 descriptor( aUri );

    // This call waits for the server to complete the request before
    // proceeding.
    ret = SendReceive(
        ERemove, TIpcArgs( aIntent, mode, aAutomatedType, &descriptor ) );

    if ( ret == KErrNotFound )
        {
        // content was never actually registered
        ret = KErrNone;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// RDRMHelper::RemoveAutomatedAll
//
// Unregister one content uri
// -----------------------------------------------------------------------------
//
TInt RDRMHelper::RemoveAutomatedAll(
    const TDesC8& aUri,
    TBool aActive,
    TInt aAutomatedType,
    TInt aIntent ) const
    {
    TPtrC8 descriptor( aUri );
    TInt ret;
    TInt mode(
        aActive ? CDRMHelperServer::EActive : CDRMHelperServer::EPassive );
    TBool automated = EFalse;
    TInt tempMode( 0 );

    // This call waits for the server to complete the request before
    // proceeding.
    ret = SendReceive(
        ERemove, TIpcArgs( aIntent, mode, aAutomatedType, &descriptor ) );
    IsAutomated( aUri, aAutomatedType, aIntent, automated, tempMode );
    while ( automated && tempMode == mode )
        {
        // unregister all
        ret = SendReceive(
            ERemove, TIpcArgs( aIntent, mode, aAutomatedType, &descriptor ) );
        IsAutomated( aUri, aAutomatedType, aIntent, automated, tempMode );
        }

    if ( ret == KErrNotFound )
        {
        ret = KErrNone;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// RDRMHelper::IndicateIdle
// -----------------------------------------------------------------------------
//
TInt RDRMHelper::IndicateIdle() const
    {
    return SendReceive( EIndicateIdle, TIpcArgs() );
    }

// -----------------------------------------------------------------------------
// RDRMHelper::IsAutomated
// -----------------------------------------------------------------------------
//
TInt RDRMHelper::IsAutomated(
    const TDesC8& aUri,
    TInt aAutomatedType,
    TInt aIntent,
    TBool& aAutomated,
    TInt& aType ) const
    {
    TPtr8 typeptr( reinterpret_cast< TUint8* >( &aType ), 0, sizeof( TInt ) );
    TPtr8 flag( reinterpret_cast< TUint8* >( &aAutomated ), 0, sizeof( TInt ) );
    TInt ret( 0 );
    TInt type = CDRMHelperServer::EActive;

    // Create descriptor to enable copying data between
    // client and server. Note: This can be local since
    // this is a synchronous call.
    // Note : Using TPtr8 since this is binary information
    TPtrC8 descriptor( aUri );

    // This call waits for the server to complete the request before
    // proceeding.
    ret = SendReceive(
        EIsRegistered,
        TIpcArgs( aIntent, type, aAutomatedType, &descriptor ) );
    if ( !ret )
        {
        type = CDRMHelperServer::EPassive;
        ret = SendReceive(
            EIsRegistered,
            TIpcArgs( aIntent, type, aAutomatedType, &descriptor ) );
        }
    aAutomated = ret > 0 ? ETrue : EFalse;
    aType = type;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RDRMHelper::StartServer
// -----------------------------------------------------------------------------
//
TInt RDRMHelper::StartServer()
    {
    RThread currentThread;
    const TUint32 secureIdAsTuint32( currentThread.SecureId() );
    TRACE2( "RDRMHelper::StartServer(): currentThread: 0x%08x",
            secureIdAsTuint32 );
    if ( currentThread.SecureId() == KServerSecureId )
        {
        // HelperServer cannot connect to itself.
        return KErrCouldNotConnect;
        }

    TInt result( 0 );

    TFindServer findHelperServer( KDRMHelperServerName );
    TFullName name;

    result = findHelperServer.Next( name );
    TRACE2( "RDRMHelper::StartServer(): result: %d", result );
    if ( result == KErrNone )
        {
        // Server already running
        return KErrNone;
        }

    RSemaphore semaphoreStarting;
    TInt semaphoreExist( semaphoreStarting.CreateGlobal(
            KDRMHelperServerSemaphoreStartingName, 0 ) );
    TRACE2( "RDRMHelper::StartServer(): semaphoreExist: %d", semaphoreExist );

    if( semaphoreExist != KErrAlreadyExists && semaphoreExist != KErrNone )
        {
        return semaphoreExist;
        }

    // Semaphore exists, wait until server is finished with it's tasks
    if ( semaphoreExist == KErrAlreadyExists )
        {
        TInt openErr( semaphoreStarting.OpenGlobal(
                KDRMHelperServerSemaphoreStartingName) );
        TRACE2( "RDRMHelper::StartServer(): openErr: %d", openErr );
        if ( openErr != KErrNone )
            {
            return openErr;
            }

        TRACE( "RDRMHelper::StartServer(): semaphoreStarting.Wait()" );
        semaphoreStarting.Wait();
        TRACE( "RDRMHelper::StartServer(): semaphoreStarting.Wait() - returning" );
        }
    else
        {
        RSemaphore semaphoreStartServer;
        TInt result( semaphoreStartServer.CreateGlobal(
                KDRMHelperServerSemaphoreName, 0 ) );
        TRACE2( "RDRMHelper::StartServer(): 2 result: %d", result );
        if ( result != KErrNone )
            {
            semaphoreStarting.Close();
            return result;
            }

        result = CreateServerProcess();
        TRACE2( "RDRMHelper::StartServer(): 3 result: %d", result );
        if ( result != KErrNone )
            {
            return  result;
            }

        semaphoreStartServer.Wait();
        semaphoreStartServer.Close();

        TRACE( "RDRMHelper::StartServer(): semaphoreStarting.Signal()" );

        semaphoreStarting.Signal();
        semaphoreStarting.Close();
        }

    TRACE( "RDRMHelper::StartServer(): return KErrNone" );
    return  KErrNone;
    }

// -----------------------------------------------------------------------------
// RDRMHelper::CreateServerProcess
// -----------------------------------------------------------------------------
//
TInt RDRMHelper::CreateServerProcess()
    {
    TInt result( 0 );

    const TUidType serverUid( KNullUid, KNullUid, KServerUid3 );


    RProcess server;
    result = server.Create( KDRMHSServerFileName, _L( "" ), serverUid );
    if ( result != KErrNone )
        {
        return  result;
        }

    server.Resume();
    server.Close();

    return  KErrNone;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File
