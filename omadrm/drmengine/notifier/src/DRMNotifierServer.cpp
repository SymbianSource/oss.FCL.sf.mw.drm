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
* Description:  DRM3 Engine manages all DRM related database operations.
*
*/



// INCLUDE FILES
#include <e32std.h>
#include "DRMNotifierServer.h"
#include "drmnotifierclientserver.h"
#include "drmcommonclientserver.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
// MACROS

#define _LOGBUFNUM( a, b )
#define LOGBUFNUM( a, b )
#define LOG( a )

// LOCAL CONSTANTS AND MACROS
LOCAL_C const TUint8 KMaxStartTries = 5;
LOCAL_C const TInt KWaitingTime = 1000000; // 10 secs
_LIT( KNotifierThread, "drmnotifier" );

// MODULE DATA STRUCTURES
using DRMNotifier::KServerMajorVersion;
using DRMNotifier::KServerMinorVersion;
using DRMNotifier::KServerBuildVersion;

// LOCAL FUNCTION PROTOTYPES
LOCAL_C TInt StartNotifierServer( RSemaphore& aClientSem );

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// Function StartNotifierServer().
// This function starts the actual server under TRAP harness and starts
// waiting for connections. This function returns only if there has been
// errors during server startup or the server is stopped for some reason.
// 
// Returns: TInt: Symbian OS error code.
// -----------------------------------------------------------------------------
TInt StartNotifierServer( RSemaphore& aClientSem ) 

    {
    TInt error = KErrNone;
    CDRMNotifierServer* server = NULL;

    TUint8 count = 0;

#ifdef _DRM_TESTING
    TTime time;
    _LIT8( KDRMErrorDebugText, "Error: " );
    RFs fs;
    RFile file;

    TFileName filename;

    time.UniversalTime();
    
    TRAPD( error2, time.FormatL( filename, KDateTimeFormat ) );
    filename.Append( _L( "DRMNotifier.txt " ) );
    
    error = fs.Connect();
    if ( error )
        {
        return error;
        }

    file.Replace( fs, filename, EFileStreamText | EFileWrite );
    file.Write( _L8( "Started." ) );
#endif

    do 
    {
       _LOGBUFNUM( KDRMErrorDebugText, error );
       
       ++count;
       
       TRAP( error, ( server = CDRMNotifierServer::NewL() ) );
       
       if ( error ) 
       {
          User::After( TTimeIntervalMicroSeconds32(KWaitingTime) );
       }
       
    } while( error && ( count <= KMaxStartTries ) );
    
    if( error ) 
    {
       
#ifdef _DRM_TESTING
       _LOGBUFNUM( _L8( "Failed: " ), error );
        file.Close();
        fs.Close();
#endif
        // Failed
        return error;
        }

#ifdef _DRM_TESTING
    file.Write( _L8( "OK!" ) );
    file.Close();
    fs.Close();
#endif

#ifdef __WINS__
    
    //UserSvr::ServerStarted();
#endif        
    // Release the semaphore...
    aClientSem.Signal();
    aClientSem.Close();
    
    // Start waiting for connections
    CActiveScheduler::Start();
    
    // Dying.
    // Delete CDRMRigntsServer
    delete server;
    
    return KErrNone;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMNotifierServer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMNotifierServer* CDRMNotifierServer::NewL()
    {
    CDRMNotifierServer* self = new( ELeave ) CDRMNotifierServer();
    
    CleanupStack::PushL( self );
    
    self->ConstructL();
    
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CDRMNotifierServer::~CDRMNotifierServer() 
    {
    if( iStorage )
        {
        delete iStorage;
        iStorage = 0;
        }   
    
    LOG( _L8( "Dying..." ) );
    
#ifdef _DRM_TESTING
    delete iLog;
#endif
    }

// -----------------------------------------------------------------------------
// CDRMNotifierServer::RunErrorL
// From CActive. Complete the request and restart the scheduler.
// -----------------------------------------------------------------------------
//
TInt CDRMNotifierServer::RunError( TInt aError ) 
    {
    // Inform the client.
    Message().Complete( aError );
    
    // Restart the scheduler.
    ReStart();
    
    // Error handled.
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMNotifierServer::NewSessionL
// Called when a client requires a new instance.
// -----------------------------------------------------------------------------
CSession2* CDRMNotifierServer::NewSessionL(
    const TVersion& aVersion,
    const RMessage2& /*aMessage*/) const
    {
    RThread client;
    LOG( _L8( "NewSessionL" ) );
    
    // Check that the versions are compatible.
    if ( ! User::QueryVersionSupported( TVersion( KServerMajorVersion,
                                                  KServerMinorVersion,
                                                  KServerBuildVersion ), 
                                        aVersion ) ) 
        {
        // Sorry, no can do.
        User::Leave( KErrNotSupported );
        }
    
    LOG( _L8( "NewSessionL: Creating a new session" ) );
    
    return CDRMNotifierSession::NewL( iStorage );
    }

// -----------------------------------------------------------------------------
// CDRMNotifierServer::CDRMNotifierServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMNotifierServer::CDRMNotifierServer() : 
    CServer2( EPriorityStandard )
    {
    // Nothing
    }
    
// -----------------------------------------------------------------------------
// CDRMNotifierServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDRMNotifierServer::ConstructL() 
    {
    RFs fs;

    // Ignore errors
    User::RenameThread( KNotifierThread );
    
    User::LeaveIfError( fs.Connect() );
    
    fs.Close();

#ifdef _DRM_TESTING
    _LIT( KLogFile, "notifier.txt" );
    TFileName logFile( KLogFile );
    TTime time;
    time.UniversalTime();
    
    time.FormatL( logFile, KDateTimeFormat );
    logFile.Append( KLogFile );

    iLog = CLogFile::NewL( logFile, ETrue );
    iLog->SetAutoFlush( ETrue );
    iLog->SetAutoNewline( ETrue );
    
    LOG( _L8( "DRM Server starting..." ) );
#endif
    iStorage = CDRMMessageStorage::NewL();
    
    LOG( _L8( "Notification Server started." ) );
    
    // Add the server to the scheduler.
    StartL( DRMNotifier::KServerName );
    }


#ifdef _DRM_TESTING

// -----------------------------------------------------------------------------
// CDRMNotifierServer::Log
// Logging operation.
// -----------------------------------------------------------------------------
//
void CDRMNotifierServer::Log( const TDesC8& aLog ) const
    {
    iLog->Log( aLog );
    }


void CDRMNotifierServer::Log( const TDesC& aLog ) const
    {
    iLog->Log( aLog );
    }
#endif

// ========================== OTHER EXPORTED FUNCTIONS =========================
// -----------------------------------------------------------------------------
// Function StartupNotifier().
// This function starts the actual DRM Notifier
// the cleanup stack and active scheduler.
// Returns: TInt: Symbian OS error code.
// -----------------------------------------------------------------------------
//

TInt StartupNotifier( TAny* ) 
    {
    TInt error = KErrNone;
    CTrapCleanup* trap = CTrapCleanup::New();
    
    // Check that memory allocation was successful.
    __ASSERT_ALWAYS( trap, User::Invariant() );
    
    CActiveScheduler* scheduler = new CActiveScheduler();
    
    __ASSERT_ALWAYS( scheduler, User::Invariant() );
    
    CActiveScheduler::Install( scheduler );
    
    RSemaphore clientSem;
    error = clientSem.OpenGlobal( KDRMEngCommonSemaphore );
    if( error ) 
        {
    	  return error;
        }
    error = StartNotifierServer( clientSem );
    
    if ( error ) {
        // If errors didn't occur, signal has been sent.
        clientSem.Signal();
        clientSem.Close();
        }
    
    delete scheduler; 
    scheduler = NULL;
    
    delete trap;
    trap = NULL;
    
    // __ASSERT_ALWAYS( !error, User::Invariant() );
    
    return KErrNone;
    }

//  End of File
