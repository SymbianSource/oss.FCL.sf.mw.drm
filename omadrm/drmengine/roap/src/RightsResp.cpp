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
#include "RightsResp.h"

using namespace Roap;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRightsResp::CRightsResp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRightsResp::CRightsResp():
    iSession(NULL),
    iNonce(NULL),
    iRoEncryptionKey(NULL),
    iSignature(NULL)
    {
    }

// -----------------------------------------------------------------------------
// CRightsResp::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRightsResp::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CRightsResp::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRightsResp* CRightsResp::NewL()
    {
    CRightsResp* self = new( ELeave ) CRightsResp;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CRightsResp::~CRightsResp()
    {
    iCertificateChain.ResetAndDestroy();
    iOcspResponse.ResetAndDestroy();
    iTransTrackIDs.ResetAndDestroy();
    iContentIDs.ResetAndDestroy();
    delete iSession;
    delete iSignature;
    delete iNonce;
    delete iRoEncryptionKey;
    delete iErrorUrl;
    }


// -----------------------------------------------------------------------------
// CRightsResp::?member_function
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
