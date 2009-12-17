/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DRMClockServer manages the all operation to the DRMClock
*
*/



// INCLUDE FILES
#include <e32std.h>
#include "DRMClockServer.h"
#include "drmlog.h"
#include "drmclockclientserver.h"
#include "drmcommonclientserver.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
// MACROS


// LOCAL CONSTANTS AND MACROS
const TUint8 KMaxStartTries = 3;
const TInt KWaitingTime = 100000; // 1 secs
_LIT( KClockThread, "DRMClockServer" );

// MODULE DATA STRUCTURES
using DRMClock::KServerMajorVersion;
using DRMClock::KServerMinorVersion;
using DRMClock::KServerBuildVersion;

// LOCAL FUNCTION PROTOTYPES
LOCAL_C TInt StartClockServer( RSemaphore& aClientSem );

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// Function StartClockServer().
// This function starts the actual server under TRAP harness and starts
// waiting for connections. This function returns only if there has been
// errors during server startup or the server is stopped for some reason.
// 
// Returns: TInt: Symbian OS error code.
// -----------------------------------------------------------------------------
LOCAL_C TInt StartClockServer( RSemaphore& aClientSem ) 

    {
    TInt error = KErrNone;
    CDRMClockServer* server = NULL;

    TUint8 count = 0;

    do 
    {
       DRMLOG2( _L( "unipertar.exe: StartClockServer: %d" ), error );
       
       ++count;
       
       TRAP( error, ( server = CDRMClockServer::NewL() ) );
       
       if ( error ) 
       {
          User::After( TTimeIntervalMicroSeconds32(KWaitingTime) );
       }
       
    } while( error && ( count <= KMaxStartTries ) );
    
    if( error ) 
    {
        // Failed
        return error;
        }
       
    // Release the semaphore...
    aClientSem.Signal();
    
    // and close it. Otherwise there will be an unused handle to the semaphore
    // until the phone is switched off.
    aClientSem.Close();
    
    DRMLOG( _L( "unipertar.exe: StartClockServer: starting..." ) );

    // Start waiting for connections
    CActiveScheduler::Start();
    
    // Dying.
    
    DRMLOG( _L( "unipertar.exe: StartClockServer: dying" ) );

    delete server;
    
    return KErrNone;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMClockServer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMClockServer* CDRMClockServer::NewL()
    {
    CDRMClockServer* self = new( ELeave ) CDRMClockServer();
    
    CleanupStack::PushL( self );
    
    self->ConstructL();
    
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CDRMClockServer::~CDRMClockServer() 
    {
    if( iDRMClock )
        {
        delete iDRMClock;
        iDRMClock = 0;
        }   
    
    DRMLOG( _L( "CDRMClockServer::~" ) );
    }

// -----------------------------------------------------------------------------
// CDRMClockServer::RunErrorL
// From CActive. Complete the request and restart the scheduler.
// -----------------------------------------------------------------------------
//
TInt CDRMClockServer::RunError( TInt aError ) 
    {
    DRMLOG2( _L( "CDRMClockServer::RunError: %d" ), aError );

    // Inform the client.
    Message().Complete( aError );
    
    // Restart the scheduler.
    ReStart();
    
    // Error handled.
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMClockServer::NewSessionL
// Called when a client requires a new instance.
// -----------------------------------------------------------------------------
CSession2* CDRMClockServer::NewSessionL( 
    const TVersion& aVersion,
    const RMessage2& /*aMessage*/ ) const 
    {
    DRMLOG( _L( "CDRMClockServer::NewSessionL" ) );
    
    // Check that the versions are compatible.
    if ( ! User::QueryVersionSupported( TVersion( KServerMajorVersion,
                                                  KServerMinorVersion,
                                                  KServerBuildVersion ), 
                                        aVersion ) ) 
        {
        // Sorry, no can do.
        User::Leave( KErrNotSupported );
        }
    
    DRMLOG( _L( "CDRMClockServer::NewSessionL: Creating a new session" ) );
    
    return CDRMClockSession::NewL( iDRMClock );    
    }


// -----------------------------------------------------------------------------
// CDRMClockServer::CDRMClockServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMClockServer::CDRMClockServer() : 
    CServer2( EPriorityStandard )
    {
    // Nothing
    }
    
// -----------------------------------------------------------------------------
// CDRMClockServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDRMClockServer::ConstructL() 
    {
    DRMLOG( _L( "CDRMClockServer::ConstructL" ) );

    // Ignore errors
    User::LeaveIfError( User::RenameThread( KClockThread ) );

    iDRMClock = CDRMClock::NewL();
    
    DRMLOG( _L( "CDRMClockServer::clock Server started." ) );
    
    // Add the server to the scheduler.
    StartL( DRMClock::KServerName );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// -----------------------------------------------------------------------------
// Function StartupClock().
// This function starts the actual DRM Notifier
// the cleanup stack and active scheduler.
// Returns: TInt: Symbian OS error code.
// -----------------------------------------------------------------------------
//

TInt StartupClock( TAny* ) 
    {
    DRMLOG( _L( "unipertar.exe: StartupClock" ) );
    TInt error = KErrNone;
    CTrapCleanup* trap = CTrapCleanup::New();
    
    // Check that memory allocation was successful.
    __ASSERT_ALWAYS( trap, User::Invariant() );
    
    DRMLOG( _L( "unipertar.exe: StartupClock: active scheduler" ) );

    CActiveScheduler* scheduler = new CActiveScheduler();
    
    __ASSERT_ALWAYS( scheduler, User::Invariant() );
    
    CActiveScheduler::Install( scheduler );
    
    // Reusing semaphore
    RSemaphore clientSem;
    __ASSERT_ALWAYS( !( clientSem.OpenGlobal( KDRMEngCommonSemaphore ) ), 
        User::Invariant() );
    
    error = StartClockServer( clientSem );
    
    if ( error ) {
        // Server creation failed. Release the semaphore.
        // In case of successful startup, CDRMClockServer
        // releases the semaphore.
        clientSem.Signal();
        clientSem.Close();
        }
    
    delete scheduler; 
    scheduler = NULL;
    
    delete trap;
    trap = NULL;
    
    DRMLOG2( _L( "unipertar.exe: StartupClock exits with %d" ), error );
    
    return error;
    }

//  End of File
