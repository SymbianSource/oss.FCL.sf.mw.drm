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
* Description:  DrmUtilityWMDrm implementation
*
*/


#include "drmutilitywmdrm.h"
#include "drmutilitywmdrmwrapper.h"

// CONSTANTS
_LIT( KDrmUtilityWMDrmWrapperName, "\\system\\libs\\drmutilitywmdrmwrapper.dll" );
const TInt KGateFunction( 3 );

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CDrmUtilityWMDrm::CDrmUtilityWMDrm
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
DRM::CDrmUtilityWMDrm::CDrmUtilityWMDrm()
    {
    }

// -----------------------------------------------------------------------------
// CDrmUtilityWMDrm::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityWMDrm::ConstructL( CCoeEnv* aCoeEnv )
    {
    User::LeaveIfError( iWMDrmDll.Load( KDrmUtilityWMDrmWrapperName ) );
    iWMDrmHandler = 
        (DRM::CDrmUtilityWMDrmWrapper*) iWMDrmDll.Lookup( KGateFunction )();
        
    if ( iWMDrmHandler )
        {
        iWMDrmHandler->SetCoeEnv( aCoeEnv );
        }
        
    }

// -----------------------------------------------------------------------------
// CDrmUtilityWMDrm::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmUtilityWMDrm* DRM::CDrmUtilityWMDrm::NewLC( CCoeEnv* aCoeEnv )
    {
    DRM::CDrmUtilityWMDrm* self( new( ELeave ) CDrmUtilityWMDrm );
    CleanupStack::PushL( self );
    self->ConstructL( aCoeEnv );
    return self;
    }

// -----------------------------------------------------------------------------
// CDrmUtilityWMDrm::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmUtilityWMDrm* DRM::CDrmUtilityWMDrm::NewL( CCoeEnv* aCoeEnv )
    {
    DRM::CDrmUtilityWMDrm* self( NewLC( aCoeEnv ) );
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
DRM::CDrmUtilityWMDrm::~CDrmUtilityWMDrm()
    {
    delete iWMDrmHandler;
    iWMDrmDll.Close();
    }


// ---------------------------------------------------------------------------
// CDrmUtilityWMDrm::GetWrapper()
// ---------------------------------------------------------------------------
//
EXPORT_C DRM::MDrmUtilityWMDrmWrapper* DRM::CDrmUtilityWMDrm::GetWrapper()
    {
    return iWMDrmHandler;
    }
