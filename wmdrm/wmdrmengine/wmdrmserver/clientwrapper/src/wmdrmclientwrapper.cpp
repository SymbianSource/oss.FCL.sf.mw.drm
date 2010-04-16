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
* Description:  Dynamically loadable wrapper for Download manager
*
*/


#include "wmdrmclientwrapper.h"

// CONSTANTS

// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWmDrmClientWrapper::CWmDrmClientWrapper()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWmDrmClientWrapper::ConstructL()
    {
    }



// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWmDrmClientWrapper* CWmDrmClientWrapper::NewL()
    {
    CWmDrmClientWrapper* self = CWmDrmClientWrapper::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWmDrmClientWrapper* CWmDrmClientWrapper::NewLC()
    {
    CWmDrmClientWrapper* self = new( ELeave ) CWmDrmClientWrapper();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWmDrmClientWrapper::~CWmDrmClientWrapper()
    {
    iClient.Close();
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CWmDrmClientWrapper::Connect()
    {
    return iClient.Connect();
    }



// ======== GLOBAL FUNCTIONS ========

//------------------------------------------------------------------------------
// GateFunctionDRM
// DRM gate function
//------------------------------------------------------------------------------
EXPORT_C TAny* GateFunctionWmDrmClientWrapper()
    {
    CWmDrmClientWrapper* launcher = NULL;
    TRAPD( err, launcher = CWmDrmClientWrapper::NewL() );
    if( err != KErrNone )
        {
        return NULL;
        }

    return launcher;
    }

