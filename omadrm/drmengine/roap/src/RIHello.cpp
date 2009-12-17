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
#include "RIHello.h"

using namespace Roap;

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRIHello::CRIHello
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRIHello::CRIHello():
    iStatus(ESuccess),
    iSession(NULL),
    iRiNonce(NULL),
    iServerInfo(NULL)
    {
    }

// -----------------------------------------------------------------------------
// CRIHello::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRIHello::ConstructL()
    {
    iSelectedVersion.SetLength(0);
    iRiId.SetLength(0);
    iPeerKeyId.SetLength(0);
    iNeedDeviceDetails = EFalse;
    iPeerKeyIdentifier = EFalse;
    iCertificateCaching = EFalse;
    }

// -----------------------------------------------------------------------------
// CRIHello::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRIHello* CRIHello::NewL()
    {
    CRIHello* self = new( ELeave ) CRIHello;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CRIHello::~CRIHello()
    {
    delete iSession;
    delete iRiNonce;
    delete iServerInfo;
    delete iErrorUrl;
    iAlgorithms.ResetAndDestroy();
    iTrustedAuthorities.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CRIHello::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ?function_name implements...
// ?implementation_description.
// Returns: ?value_1: ?description
//          ?value_n: ?description
//                    ?description
// -----------------------------------------------------------------------------
//

//  End of File  
