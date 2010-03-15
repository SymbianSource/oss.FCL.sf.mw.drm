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
* Description:  This class handles all client requests.
*
*/


// INCLUDE FILES
#include <s32file.h>
#include <f32file.h>
#include "DRMCommon.h"
#include "DRMClockSession.h"
#include "drmclockclientserver.h"
#include "DRMClockServer.h"
#include <DRMTypes.h>

#include <e32test.h>

#include "drmlog.h"

// NAMESPACES
using namespace DRMClock;

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS


// MACROS

#define IPCREAD0L( a ) aMessage.ReadL( 0, a )
#define IPCREAD1L( a ) aMessage.ReadL( 1, a )
#define IPCREAD2L( a ) aMessage.ReadL( 2, a )
#define IPCWRITE0L( a ) aMessage.WriteL( 0, a )
#define IPCWRITE1L( a ) aMessage.WriteL( 1, a )
#define IPCWRITE2L( a ) aMessage.WriteL( 2, a )


// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// DATA TYPES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CDRMClockSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMClockSession* CDRMClockSession::NewL( CDRMClock* aClock)
    {
    CDRMClockSession* self = new( ELeave ) CDRMClockSession( aClock );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CDRMClockSession::~CDRMClockSession
// Destructor.
// -----------------------------------------------------------------------------
//
CDRMClockSession::~CDRMClockSession()
    {
    DRMLOG(  _L( "CDRMClockSession::Notifier session closed." ) );
    }


// -----------------------------------------------------------------------------
// CDRMClockSession::ServiceL
// This method runs DispatchL() under TRAP harness, since every error case
// can be handled ==> no need to let this function leave.
// -----------------------------------------------------------------------------
//
void CDRMClockSession::ServiceL( const RMessage2& aMessage )
    {
    DRMLOG(  _L( "CDRMClockSession::ServiceL called" ) );
    // Trap possible errors...

    TRAPD( error, DispatchL( aMessage ) );

    if ( error )
        {
        DRMLOG2(  _L( "CDRMClockSession::DispatcL threw an exception %d" ), error );
        // ...and complete the request in case of an error.
        aMessage.Complete( error );
        return;
        }

    // The message has already completed successfully.
    DRMLOG(  _L( "CDRMClockSession::DispatchL completed successfully" ) );
    }

// -----------------------------------------------------------------------------
// CDRMClockSession::CDRMClockSession
// Default constructor.
// -----------------------------------------------------------------------------
//
CDRMClockSession::CDRMClockSession( CDRMClock* aClock ) :
    // Base class' constructor is called first.
    iDRMClock( aClock )
    {
    // Nothing.
    }

// -----------------------------------------------------------------------------
// CDRMClockSession::ConstructL
// Second phase constructor. Initializes the log tool in DRM internal testing.
// -----------------------------------------------------------------------------
//
void CDRMClockSession::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CDRMClockSession::DispatchL
// Checks which command the user requested, and forwards the request to
// appropriate private method. This helps to keep the code more readable.
// -----------------------------------------------------------------------------
//
void CDRMClockSession::DispatchL( const RMessage2& aMessage )
    {
    switch ( aMessage.Function() )
        {
        case EGetDRMTime:
            GetDRMTimeL( aMessage );
            break;
        case EUpdateDRMTime:
            UpdateDRMTimeL( aMessage );
            break;
        default:
            DRMLOG(  _L( "CDRMClockSession::DispatchL: Invalid command" ) );
            User::Leave( KErrNotSupported );
        }
    }

// -----------------------------------------------------------------------------
// CDRMClockSession::GetDRMTimeL
// -----------------------------------------------------------------------------
//
void CDRMClockSession::GetDRMTimeL( const RMessage2& aMessage )
    {
    DRMLOG(  _L( "CDRMClockSession::GetDRMTimeL" ) );
    TTime drmTime;
    TInt timeZone;
    DRMClock::ESecurityLevel level;

    iDRMClock->GetSecureTime( drmTime, timeZone, level );

    TPckg<TTime> package(drmTime);
    TPckg<TInt> package2(timeZone);
    TPckg<DRMClock::ESecurityLevel> package3(level);

    IPCWRITE0L( package );
    IPCWRITE1L( package2 );
    IPCWRITE2L( package3 );

    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CDRMClockSession::UpdateDRMTimeL
// -----------------------------------------------------------------------------
//
void CDRMClockSession::UpdateDRMTimeL( const RMessage2& aMessage )
    {
    DRMLOG(  _L( "CDRMClockSession::UpdateDRMTimeL" ) );
    TInt error = KErrNone;
    TTime drmTime;
    TInt timeZone;
    TPckg<TTime> package( drmTime );
    TPckg<TInt> package2( timeZone );
    TInt r = KErrAccessDenied;

    // Add a check for the vendor id
    _LIT_SECURITY_POLICY_V0(vidCheck, VID_DEFAULT); // Check Default VID
    _LIT_SECURITY_POLICY_C1(drmCheck, ECapabilityDRM);
    RThread client;

    aMessage.ClientL( client );

    if( vidCheck.CheckPolicy( client ) || drmCheck().CheckPolicy(client) )
        {
        r = KErrNone;
        }

    client.Close();
    User::LeaveIfError( r );

    IPCREAD0L( package );
    IPCREAD1L( package2 );

    TRAP( error, iDRMClock->ResetSecureTimeL( drmTime, timeZone ) );

    // All done.
    aMessage.Complete( error );
    }



// ========================== OTHER EXPORTED FUNCTIONS =========================
//  End of File

