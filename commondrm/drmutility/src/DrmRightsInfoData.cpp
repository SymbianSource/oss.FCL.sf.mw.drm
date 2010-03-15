/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DrmRightsInfoImpl async command storage class
*
*/


// INCLUDE FILES
#include "DrmRightsInfoData.h"

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDrmRightsInfoData::NewLC
// Two-Phase constructor
// -----------------------------------------------------------------------------
//
DRM::CDrmRightsInfoData* DRM::CDrmRightsInfoData::NewL()
    {
    DRM::CDrmRightsInfoData* self( NewLC() );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CDrmRightsInfoData::NewLC
// Two-Phase constructor
// -----------------------------------------------------------------------------
//
DRM::CDrmRightsInfoData* DRM::CDrmRightsInfoData::NewLC()
    {
    DRM::CDrmRightsInfoData* self( new (ELeave) CDrmRightsInfoData );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
DRM::CDrmRightsInfoData::CDrmRightsInfoData()
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
DRM::CDrmRightsInfoData::~CDrmRightsInfoData()
    {
    delete iUniqueId;
    delete iUniqueId8;
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfoData::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
void DRM::CDrmRightsInfoData::ConstructL()
    {
    }


//  End of File
