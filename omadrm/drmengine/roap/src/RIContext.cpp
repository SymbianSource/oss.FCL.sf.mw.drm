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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include <e32base.h>
#include "RIContext.h"

using namespace Roap;

// LOCAL CONSTANTS AND MACROS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRIContext::CRIContext
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRIContext::CRIContext()
    {
    }

// -----------------------------------------------------------------------------
// CRIContext::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRIContext::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CJoinDomainReq::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRIContext* CRIContext::NewL()
    {
    CRIContext* self = new( ELeave ) CRIContext;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CRIContext::~CRIContext()
    {
    }
