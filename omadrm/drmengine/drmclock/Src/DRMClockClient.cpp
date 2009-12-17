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
#include "DRMClientServer.h"

#include "DRMLog.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
// MACROS

// LOCAL CONSTANTS AND MACROS
// To initialize C/S parameters.
#define PARAMS TAny* params[ KMaxMessageArguments ]; \
for ( TUint8 i = 0; i < KMaxMessageArguments; ++i ) { params[ i ] = NULL; }
#define CLEARPARAM Mem::FillZ( params, \
KMaxMessageArguments * sizeof( TAny* ) );

// Maximum number of message slots that we use
const TInt KMaxMessageSlots = 3;


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
    
    DRMLOG( _L( "RDRMClockClient::Connect" ) );

    const TVersion requiredVersion( 
        DRMClock::KServerMajorVersion,
        DRMClock::KServerMinorVersion,
        DRMClock::KServerBuildVersion );
    
    DRMLOG( _L("RDRMClockClient: Create a new session" ) );
    ret = CreateSession( DRMClock::KDRMServerName,
                         requiredVersion, 
                         KMaxMessageSlots );

    DRMLOG2( _L( "Result: %d") , ret );
    
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
                                     DRMClock::ESecurityLevel& aSecurityLevel )
    {
    TPckg<TTime> package(aTime);
    TPckg<TInt> package2(aTimeZone);
    TPckg<DRMClock::ESecurityLevel> package3(aSecurityLevel);
                    
    DRMLOG( _L( "RDRMClockClient::GetSecureTime" ) );
    TInt error = KErrNone;
    
    // For C/S communications.
    PARAMS;
    
    // Set the parameters.
    params[ 0 ] = reinterpret_cast< TAny* >( &package );
    params[ 1 ] = reinterpret_cast< TAny* >( &package2 );
    params[ 2 ] = reinterpret_cast< TAny* >( &package3 );
    
    // Send the message.
    error = SendReceive( DRMClock::EGetDRMTime, params );
    
    // Reset
    CLEARPARAM;
    
    DRMLOG2( _L( "RDRMClockClient::GetSecureTime: %d" ), error );

    return error;
    }


// -----------------------------------------------------------------------------
// RDRMClockClient::UpdateSecureTime
// Updates the secure time on the DRMClockServer
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMClockClient::UpdateSecureTime( const TTime& aTime, const TInt& aTimeZone )
    {
    TInt error = KErrNone;
    TPckg<TTime> package(aTime);
    TPckg<TInt> package2(aTimeZone);
    
    DRMLOG( _L( "RDRMClockClient::UpdateSecureTime" ) );
    
    // For C/S communications.
    PARAMS;
    
    params[ 0 ] = reinterpret_cast< TAny* >( &package );
    params[ 1 ] = reinterpret_cast< TAny* >( &package2 );
    
    error = SendReceive( DRMClock::EUpdateDRMTime, params );
    
    CLEARPARAM;
    
    DRMLOG2( _L( "RDRMClockClient::UpdateSecureTime: " ), error );

    // All done.
    return error;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
