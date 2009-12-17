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
* Description:  Implementation class for WMDRM DLA App Document
*
*/


// INCLUDE FILES
#include "wmdrmdlaappdocument.h"
#include "wmdrmdlaappui.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CWmDrmDlaAppDocument::NewL
// ---------------------------------------------------------------------------
//
CWmDrmDlaAppDocument* CWmDrmDlaAppDocument::NewL( 
    CEikApplication& aApp )
    {
    CWmDrmDlaAppDocument* self = new (ELeave) CWmDrmDlaAppDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CWmDrmDlaAppDocument::~CWmDrmDlaAppDocument
// ---------------------------------------------------------------------------
//
CWmDrmDlaAppDocument::~CWmDrmDlaAppDocument()
    {
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaAppDocument::CreateAppUiL
// ---------------------------------------------------------------------------
//
CEikAppUi* CWmDrmDlaAppDocument::CreateAppUiL()
    {
    return new (ELeave) CWmDrmDlaAppUi;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaAppDocument::CWmDrmDlaAppDocument
// ---------------------------------------------------------------------------
//
CWmDrmDlaAppDocument::CWmDrmDlaAppDocument( 
    CEikApplication& aApp ) : CAknDocument( aApp )
    {
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaAppDocument::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaAppDocument::ConstructL()
    {
    }
