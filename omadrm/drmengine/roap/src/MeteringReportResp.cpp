/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  class representing metering report response
*
*/



// INCLUDE FILES
#include <e32base.h>
#include "MeteringReportResp.h"

using namespace Roap;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRegistrationResp::CRegistrationResp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMeteringResp::CMeteringResp()
    {
    }

// -----------------------------------------------------------------------------
// CRegistrationResp::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMeteringResp::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CRegistrationResp::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMeteringResp* CMeteringResp::NewL()
    {
    CMeteringResp* self = new( ELeave ) CMeteringResp;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CMeteringResp::~CMeteringResp()
    {
    iCertificateChain.ResetAndDestroy();
    iOcspResponse.ResetAndDestroy();
    delete iDeviceNonce;
    delete iErrorUrl;
    delete iSignature;
    delete iPrUrl;
    }

//  End of File
