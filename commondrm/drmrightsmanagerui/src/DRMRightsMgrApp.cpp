/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CDRMRightsMgrApp class
*
*/


// INCLUDE FILES
#include <eikstart.h>

#include "DRMRightsMgrApp.h"
#include "DRMRightsMgrDocument.h"


// ================= MEMBER FUNCTIONS =======================
//
// ---------------------------------------------------------
// CDRMRightsMgrApp::AppDllUid
// ---------------------------------------------------------
//
TUid CDRMRightsMgrApp::AppDllUid() const
    {
    return KUidDRMRightsManager;
    }


// ---------------------------------------------------------
// CDRMRightsMgrApp::CreateDocumentL
// ---------------------------------------------------------
//
CApaDocument* CDRMRightsMgrApp::CreateDocumentL()
    {
    return CDRMRightsMgrDocument::NewL( *this );
    }


// ================= OTHER FUNCTIONS =======================
//

// ---------------------------------------------------------
// NewApplication
// ---------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    return new CDRMRightsMgrApp;
    }


// ---------------------------------------------------------
// E32Main
// ---------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }


// ---------------------------------------------------------
// Panic
// ---------------------------------------------------------
//
void Panic( TInt aReason )
    {
    _LIT( KAppName, "DRM RightsMgrUI" );
    User::Panic( KAppName, aReason );
    }


// End of File
