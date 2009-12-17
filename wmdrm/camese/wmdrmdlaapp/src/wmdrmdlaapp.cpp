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
* Description:  Implementation class for WMDRM DLA App
*
*/


// INCLUDE FILES
#include <eikstart.h>
#include "wmdrmdlaapp.h"
#include "wmdrmdlaappdocument.h"
#include "wmdrmdlaappserver.h"
#include "wmdrmdlaappconstants.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CWmDrmDlaApp::CreateDocumentL
// ---------------------------------------------------------------------------
//
CApaDocument* CWmDrmDlaApp::CreateDocumentL()
    {
    return CWmDrmDlaAppDocument::NewL( *this );
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaApp::AppDllUid
// ---------------------------------------------------------------------------
//
TUid CWmDrmDlaApp::AppDllUid() const
    {
    return KWmDrmDlaAppUid;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaApp::NewAppServerL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaApp::NewAppServerL( CApaAppServer*& aAppServer )
    {
    aAppServer = new (ELeave) CWmDrmDlaAppServer;
    }

// ================= OTHER FUNCTIONS ==============

/**
* Create application object.
* @return The created application object.
*/
LOCAL_C CApaApplication* NewApplication()
    {
    return new CWmDrmDlaApp;
    }

/**
* Executable entry point.
* @return Error code.
*/
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }
