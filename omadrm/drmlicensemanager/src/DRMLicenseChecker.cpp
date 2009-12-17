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
* Description:  ?Description
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <s32file.h>
#include "DRMCommon.h"
#include "DRMLicenseChecker.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMLicenseChecker::CDRMLicenseChecker
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMLicenseChecker::CDRMLicenseChecker()
    {
    }

// -----------------------------------------------------------------------------
// CDRMLicenseChecker::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDRMLicenseChecker::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CDRMLicenseChecker::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMLicenseChecker* CDRMLicenseChecker::NewL()
    {
    CDRMLicenseChecker* self = new( ELeave ) CDRMLicenseChecker;
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
EXPORT_C CDRMLicenseChecker::~CDRMLicenseChecker()
    {
    }

// -----------------------------------------------------------------------------
// CDRMLicenseChecker::CheckLicenseL
// Open the data file and encrypt it.
// -----------------------------------------------------------------------------
//
void CDRMLicenseChecker::CheckLicenseL(
    const TDesC& aDataFile,
    HBufC8*& aDataBuffer)
    {
    CData* data = NULL;
    TInt size;
    TPtr8 ptr(0, 0);
    TVirtualPathPtr name(aDataFile, KDefaultContentObject);
    
    data = CData::NewLC(name, EView, EContentShareReadOnly);
    data->DataSizeL(size);
    aDataBuffer = HBufC8::NewMax(size);
    User::LeaveIfNull(aDataBuffer);
    ptr.Set(aDataBuffer->Des());
    User::LeaveIfError(data->Read(ptr));
    data->ExecuteIntent(EView);
    CleanupStack::PopAndDestroy(); // data
    }

// -----------------------------------------------------------------------------
// CDRMLicenseChecker::CheckLicense
// Open the data file and encrypt it.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMLicenseChecker::CheckLicense(
    const TDesC& aDataFile,
    HBufC8*& aDataBuffer)
    {
    TRAPD(r, CheckLicenseL(aDataFile, aDataBuffer));
    return r;
    }
