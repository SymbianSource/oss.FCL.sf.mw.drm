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
* Description:  Data class for async operations in DrmUiHandlingImpl class
*
*/


// INCLUDE FILES
#include "drmuihandlingdata.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDrmUiHandlingData::NewLC
// Two-Phase constructor
// -----------------------------------------------------------------------------
//     
DRM::CDrmUiHandlingData* DRM::CDrmUiHandlingData::NewL()
    {
    DRM::CDrmUiHandlingData* self( NewLC() );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CDrmUiHandlingData::NewLC
// Two-Phase constructor
// -----------------------------------------------------------------------------
//     
DRM::CDrmUiHandlingData* DRM::CDrmUiHandlingData::NewLC()
    {
    DRM::CDrmUiHandlingData* self( new (ELeave) CDrmUiHandlingData );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//  
DRM::CDrmUiHandlingData::CDrmUiHandlingData()
	{
	}
	
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//  
DRM::CDrmUiHandlingData::~CDrmUiHandlingData()
	{
	}
	
// -----------------------------------------------------------------------------
// CDrmUiHandlingData::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//  
void DRM::CDrmUiHandlingData::ConstructL()
    {
    }
    
    
//  End of File
