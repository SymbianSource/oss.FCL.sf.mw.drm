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
* Description:  Implementation class for DrmServiceAPI.dll
*
*/



#include <drmserviceapi.h>

#include "RoapStorageClient.h"
#include "DRMClockClient.h"


// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// two-phase costructor
// ---------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmServiceApi* DRM::CDrmServiceApi::NewL()
    {
    CDrmServiceApi* self = CDrmServiceApi::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// two-phase costructor
// ---------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmServiceApi* DRM::CDrmServiceApi::NewLC()
    {
    CDrmServiceApi* self = new( ELeave ) CDrmServiceApi;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
DRM::CDrmServiceApi::~CDrmServiceApi()
    {
    // Clock client
    if( iClockClient )
        {
        iClockClient->Close();
        delete iClockClient;
        iClockClient = NULL;
        }

    // Roap storage client
    if( iRoapStorageClient )
        {
        iRoapStorageClient->Close();
        delete iRoapStorageClient;
        iRoapStorageClient = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CDrmServiceApi::GetSecureTime
// ---------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmServiceApi::GetSecureTime(
    TTime& aTime,
    TInt& aTimeZone,
    DRMClock::ESecurityLevel& aSecurityLevel ) const
    {
    return iClockClient->GetSecureTime( aTime, aTimeZone, aSecurityLevel);
    };

// ---------------------------------------------------------------------------
// CDrmServiceApi::UpdateSecureTime
// ---------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmServiceApi::UpdateSecureTime( const TTime& aTime, const TInt& aTimeZone )
    {
    return iClockClient->UpdateSecureTime( aTime, aTimeZone );
    };


// ---------------------------------------------------------------------------
// CDrmServiceApi::GetDevicePublicKeyDerL
// ---------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmServiceApi::GetDevicePublicKeyDerL( HBufC8*& aPublicKey )
    {
    iRoapStorageClient->GetDevicePublicKeyDerL( aPublicKey );
    };

// ---------------------------------------------------------------------------
// CDrmServiceApi::SignL
// ---------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmServiceApi::SignL( const TDesC8& aHash, HBufC8*& aSignature )
    {
    iRoapStorageClient->SignL( aHash, aSignature );
    };


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
DRM::CDrmServiceApi::CDrmServiceApi()
    {
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void DRM::CDrmServiceApi::ConstructL()
    {
    // Create an instance of the clock client
    iClockClient = new (ELeave) RDRMClockClient;

    // Connect to the server
    User::LeaveIfError( iClockClient->Connect() );

    // Create and instance of the roap storage client
    iRoapStorageClient = new (ELeave) Roap::RRoapStorageClient;

    // Connect to the server
    User::LeaveIfError( iRoapStorageClient->Connect() );
    }
