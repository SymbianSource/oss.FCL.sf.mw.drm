/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


// INCLUDE FILES
#include "DRMEncryptorApp.h"
#include "DRMEncryptorDocument.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDRMEncryptorApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CDRMEncryptorApp::AppDllUid() const
    {
    return KUidDRMEncryptor;
    }

   
// ---------------------------------------------------------
// CDRMEncryptorApp::CreateDocumentL()
// Creates CDRMEncryptorDocument object
// ---------------------------------------------------------
//
CApaDocument* CDRMEncryptorApp::CreateDocumentL()
    {
    return CDRMEncryptorDocument::NewL( *this );
    }

#include <eikstart.h>

LOCAL_C CApaApplication* NewApplication()
    {
    return new CDRMEncryptorApp;
    }

// ---------------------------------------------------------
// E32Main()
//  Main startup entry point
// Returns: KErrNone
// ---------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }


// End of File

