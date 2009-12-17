/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  server common implementation
*
*/



// INCLUDE FILES
#include    "Pair.h"
#include    <e32std.h>
#include    <e32base.h>

// EXTERNAL DATA STRUCTURES


// EXTERNAL FUNCTION PROTOTYPES  


// CONSTANTS


// MACROS


// LOCAL CONSTANTS AND MACROS


// MODULE DATA STRUCTURES


// LOCAL FUNCTION PROTOTYPES


// FORWARD DECLARATIONS


// ============================= LOCAL FUNCTIONS ===============================



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPair::CPair
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPair::CPair():iCid(NULL),iTtid(NULL)
    {
    }

// -----------------------------------------------------------------------------
// CPair::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPair::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CPair::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPair* CPair::NewL()
    {
    CPair* self = new( ELeave ) CPair;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

    
// Destructor
CPair::~CPair()
    {
    delete iCid;
    delete iTtid;
    }

//  End of File  
