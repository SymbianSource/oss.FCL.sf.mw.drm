/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Dynamically loadable wrapper for Drm Service Api
*
*/


#include <drmserviceapi.h>
#include "drmserviceapiwrapper.h"

// CONSTANTS

// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
DRM::CDrmServiceApiWrapper::CDrmServiceApiWrapper()
    {
    }

// ---------------------------------------------------------------------------
// CDrmServiceApiWrapper::ConstructL
// ---------------------------------------------------------------------------
//
void DRM::CDrmServiceApiWrapper::ConstructL()
    {
    iServiceApi = DRM::CDrmServiceApi::NewL();
    }

// ---------------------------------------------------------------------------
// CDrmServiceApiWrapper::NewL
// ---------------------------------------------------------------------------
//
DRM::CDrmServiceApiWrapper* DRM::CDrmServiceApiWrapper::NewL()
    {
    CDrmServiceApiWrapper* self = CDrmServiceApiWrapper::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CDrmServiceApiWrapper::NewLC
// ---------------------------------------------------------------------------
//
DRM::CDrmServiceApiWrapper* DRM::CDrmServiceApiWrapper::NewLC()
    {
	CDrmServiceApiWrapper* self = new( ELeave ) CDrmServiceApiWrapper();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
DRM::CDrmServiceApiWrapper::~CDrmServiceApiWrapper()
    {
    delete iServiceApi;
    }


// ---------------------------------------------------------------------------
// CDrmServiceApiWrapper::GetSecureTime
// ---------------------------------------------------------------------------
//
TInt DRM::CDrmServiceApiWrapper::GetSecureTime( 
    TTime& aTime, 
    TInt& aTimeZone,
    DRMClock::ESecurityLevel& aSecurityLevel ) const
    {
    return iServiceApi->GetSecureTime( aTime, aTimeZone, aSecurityLevel );
    }

// ---------------------------------------------------------------------------
// CDrmServiceApiWrapper::UpdateSecureTime
// ---------------------------------------------------------------------------
//
TInt DRM::CDrmServiceApiWrapper::UpdateSecureTime( const TTime& aTime, 
                                              const TInt& aTimeZone )
    {
    return iServiceApi->UpdateSecureTime( aTime, aTimeZone );
    }

// ---------------------------------------------------------------------------
// CDrmServiceApiWrapper::GetDevicePublicKeyDerL
// ---------------------------------------------------------------------------
//
void DRM::CDrmServiceApiWrapper::GetDevicePublicKeyDerL( HBufC8*& aPublicKey )
    {
    iServiceApi->GetDevicePublicKeyDerL( aPublicKey );
    }

// ---------------------------------------------------------------------------
// CDrmServiceApiWrapper::SignL
// ---------------------------------------------------------------------------
//
void DRM::CDrmServiceApiWrapper::SignL( const TDesC8& aHash, HBufC8*& aSignature )
    {
    iServiceApi->SignL( aHash, aSignature );
    }


// ======== GLOBAL FUNCTIONS ========

//------------------------------------------------------------------------------
// GateFunctionDrmServiceApiWrapper
// DRM gate function
//------------------------------------------------------------------------------
EXPORT_C TAny* GateFunctionDrmServiceApiWrapper()
	{
	DRM::CDrmServiceApiWrapper* launcher = NULL;
	TRAPD( err, launcher = DRM::CDrmServiceApiWrapper::NewL() );
	if( err != KErrNone )
	    {
	    return NULL;
	    }

	return launcher;
	}

