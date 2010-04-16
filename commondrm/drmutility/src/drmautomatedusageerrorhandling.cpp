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
* Description:  Implementation class that implements the MDrmErrorHandling
*                API
*
*/


#include "drmautomatedusageerrorhandling.h"
#include "DrmAutomatedUsageImpl.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
DRM::CDrmAutomatedUsageErrorHandling::CDrmAutomatedUsageErrorHandling()
    {
    }


// ---------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageErrorHandling::ConstructL
// Second phase constructor
// ---------------------------------------------------------------------------
//
void DRM::CDrmAutomatedUsageErrorHandling::ConstructL(
    DRM::CDrmAutomatedUsageImpl* aImplementation )
    {
    iImplementation = aImplementation;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmAutomatedUsageErrorHandling*
DRM::CDrmAutomatedUsageErrorHandling::NewL(
    DRM::CDrmAutomatedUsageImpl* aImplementation )
    {
    DRM::CDrmAutomatedUsageErrorHandling* self(
        DRM::CDrmAutomatedUsageErrorHandling::NewLC( aImplementation ) );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmAutomatedUsageErrorHandling*
DRM::CDrmAutomatedUsageErrorHandling::NewLC(
    DRM::CDrmAutomatedUsageImpl* aImplementation )
    {
    DRM::CDrmAutomatedUsageErrorHandling* self(
        new ( ELeave ) DRM::CDrmAutomatedUsageErrorHandling );
    CleanupStack::PushL( self );
    self->ConstructL( aImplementation );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
DRM::CDrmAutomatedUsageErrorHandling::~CDrmAutomatedUsageErrorHandling()
    {
    }

// ---------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageErrorHandling::HandleErrorAsyncL
// RFile based asynchronous method
// ---------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmAutomatedUsageErrorHandling::HandleErrorAsyncL(
    RFile& aFile,
    ContentAccess::TIntent aIntent,
    TInt aError,
    DRM::MDrmHandleErrorObserver& aObserver )
    {
    return iImplementation->HandleErrorAsyncL( aFile,
                                               aIntent,
                                               aError,
                                               aObserver );
    }

// ---------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageErrorHandling::HandleErrorL
// RFile based method
// ---------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmAutomatedUsageErrorHandling::HandleErrorL(
    RFile& aFile,
    ContentAccess::TIntent aIntent,
    TInt aError,
    DRM::MDrmHandleErrorObserver* aObserver )
    {
    iImplementation->HandleErrorL( aFile, aIntent, aError, aObserver );
    }

// ---------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageErrorHandling::HandleErrorAsyncL
// CData based asynchronous method
// ---------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmAutomatedUsageErrorHandling::HandleErrorAsyncL(
    ContentAccess::CData& aFile,
    ContentAccess::TIntent aIntent,
    TInt aError,
    DRM::MDrmHandleErrorObserver& aObserver )
    {
    return iImplementation->HandleErrorAsyncL( aFile,
                                               aIntent,
                                               aError,
                                               aObserver);
    }

// ---------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageErrorHandling::HandleErrorL
// CData based method
// ---------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmAutomatedUsageErrorHandling::HandleErrorL(
    ContentAccess::CData& aData,
    ContentAccess::TIntent aIntent,
    TInt aError,
    DRM::MDrmHandleErrorObserver* aObserver )
    {
    iImplementation->HandleErrorL( aData, aIntent, aError, aObserver );
    }
// End of File

