/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Client side class implementation
*
*/


// INCLUDE FILES
#include "DRMClockClient.h"
#include "drmclockclientserver.h"
#include "DRMTypes.h"

#include "drmlog.h"


// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
// MACROS

// LOCAL CONSTANTS AND MACROS

// Maximum number of message slots that we use
LOCAL_C const TInt KMaxMessageSlots = 3;

#ifndef __DRM_CLOCK
LOCAL_C const TInt KMinuteInMicroseconds = 60000000;
LOCAL_C const TInt KTimeZoneIncrement = 15;
#endif
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RDRMClockClient::RDRMClockClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RDRMClockClient::RDRMClockClient()
    {
    }

// -----------------------------------------------------------------------------
// RDRMClockClient::~RDRMClockClient
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C RDRMClockClient::~RDRMClockClient()
    {
    }

// -----------------------------------------------------------------------------
// RDRMClockClient::Connect
// Opens connection to the server.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMClockClient::Connect()
    {
    TInt ret = KErrNone;
#ifdef __DRM_CLOCK
    DRMLOG( _L( "RDRMClockClient::Connect" ) );

    const TVersion requiredVersion(
        DRMClock::KServerMajorVersion,
        DRMClock::KServerMinorVersion,
        DRMClock::KServerBuildVersion );

    DRMLOG( _L("RDRMClockClient: Create a new session" ) );
    ret = CreateSession( DRMClock::KServerName,
                         requiredVersion,
                         KMaxMessageSlots );
    DRMLOG2( _L( "Result: %d") , ret );
#else
    DRMLOG( _L( "RDRMClockClient: No Session Created, DRMClock is off") );
#endif // __DRM_CLOCK

    return ret;
    }

// -----------------------------------------------------------------------------
// RDRMClockClient::Close
// Closes the connection to the server.
// -----------------------------------------------------------------------------
//
EXPORT_C void RDRMClockClient::Close()
    {
    DRMLOG( _L( "RDRMClockClient::Close" ) );

    RHandleBase::Close();
    }

// -----------------------------------------------------------------------------
// RDRMClockClient::GetSecureTime
// Gets the secure time from the DRMClockServer
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMClockClient::GetSecureTime( TTime& aTime, TInt& aTimeZone,
                                     DRMClock::ESecurityLevel& aSecurityLevel ) const
    {
#ifdef __DRM_CLOCK
    TPckg<TTime> package(aTime);
    TPckg<TInt> package2(aTimeZone);
    TPckg<DRMClock::ESecurityLevel> package3(aSecurityLevel);

    DRMLOG( _L( "RDRMClockClient::GetSecureTime" ) );
    TInt error = KErrNone;

    // Send the message.
    error = SendReceive( DRMClock::EGetDRMTime, TIpcArgs( &package, &package2, &package3 ) );

    DRMLOG2( _L( "RDRMClockClient::GetSecureTime: %d" ), error );
    return error;
#else
    DRMLOG( _L( "RDRMClockClient::GetSecureTime, UI Time is returned, DRMClock is off" ) );
      TTime currentLocal;
    TInt64 result = 0;

      // aTime:
      aTime.UniversalTime();
      currentLocal.HomeTime();

    result = currentLocal.Int64() - aTime.Int64();
    result /= KMinuteInMicroseconds;
    result /= KTimeZoneIncrement;

    // aTimeZone:
    aTimeZone = I64INT(result);

      // aSecurityLevel:
      aSecurityLevel = DRMClock::KSecure;
      return KErrNone;
#endif // __DRM_CLOCK
    }


// -----------------------------------------------------------------------------
// RDRMClockClient::UpdateSecureTime
// Updates the secure time on the DRMClockServer
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMClockClient::UpdateSecureTime( const TTime& aTime, const TInt& aTimeZone )
    {
    TInt error = KErrNone;
#ifdef __DRM_CLOCK
    TPckg<TTime> package(aTime);
    TPckg<TInt> package2(aTimeZone);

    DRMLOG( _L( "RDRMClockClient::UpdateSecureTime" ) );

    error = SendReceive( DRMClock::EUpdateDRMTime, TIpcArgs( &package, &package2 ) );

    DRMLOG2( _L( "RDRMClockClient::UpdateSecureTime: " ), error );
#else
    DRMLOG( _L("RDRMClockClient::UpdateSecureTime, Did nothing DRMClock is off") );
#endif
    // All done.
    return error;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
