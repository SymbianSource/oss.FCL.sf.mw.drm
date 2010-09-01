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
#include "DRMEncryptorDocument.h"
#include "DRMEncryptorAppUi.h"
#include "DrmKeyStorage.h"

extern TUint KeyStorageFromZipL(const TDesC&, RFs&);

// ================= MEMBER FUNCTIONS =======================

// constructor
CDRMEncryptorDocument::CDRMEncryptorDocument( CEikApplication& aApp ) : CAknDocument( aApp )
    {
    }

// destructor
CDRMEncryptorDocument::~CDRMEncryptorDocument()
    {
    }

// Two-phased constructor.
CDRMEncryptorDocument* CDRMEncryptorDocument::NewL( CEikApplication& aApp )
    {
    return new( ELeave ) CDRMEncryptorDocument( aApp );
    }
    
CFileStore* CDRMEncryptorDocument::OpenFileL(TBool /*aDoOpen*/, const TDesC& aFileName, RFs& aFs)
    {
    KeyStorageFromZipL(aFileName, aFs);
    return NULL;
    }

// -----------------------------------------------------------------------------
// CDRMEncryptorDocument::CreateAppUiL()
// constructs CDRMEncryptorAppUi
// -----------------------------------------------------------------------------
//
CEikAppUi* CDRMEncryptorDocument::CreateAppUiL()
    {
    return new( ELeave ) CDRMEncryptorAppUi;
    }

// End of File  
