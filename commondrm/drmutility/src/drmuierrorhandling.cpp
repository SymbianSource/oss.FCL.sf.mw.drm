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


#include "drmuierrorhandling.h"
#include "DrmUiHandlingImpl.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
DRM::CDrmUiErrorHandling::CDrmUiErrorHandling()
    {
    }

// ---------------------------------------------------------------------------
// CDrmUiErrorHandling::ConstructL
// Second phase constructor
// ---------------------------------------------------------------------------
//
void DRM::CDrmUiErrorHandling::ConstructL( DRM::CDrmUiHandlingImpl* aImplementation )
    {
    iImplementation = aImplementation;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmUiErrorHandling* DRM::CDrmUiErrorHandling::NewL(
    DRM::CDrmUiHandlingImpl* aImplementation )
    {
    DRM::CDrmUiErrorHandling* self( NewLC( aImplementation ) );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmUiErrorHandling* DRM::CDrmUiErrorHandling::NewLC(
    DRM::CDrmUiHandlingImpl* aImplementation )
    {
    DRM::CDrmUiErrorHandling* self( new( ELeave ) CDrmUiErrorHandling );
    CleanupStack::PushL( self );
    self->ConstructL( aImplementation );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
DRM::CDrmUiErrorHandling::~CDrmUiErrorHandling()
    {
    }

// ---------------------------------------------------------------------------
// CDrmUiErrorHandling::HandleErrorAsyncL
// RFile based method
// ---------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiErrorHandling::HandleErrorAsyncL(
    RFile& aFile,
    ContentAccess::TIntent aIntent,
    TInt aError,
    DRM::MDrmHandleErrorObserver& aObserver )
    {
    TInt operation = iImplementation->HandleErrorAsyncL( aFile,
                                                         aIntent,
                                                         aError,
                                                         aObserver);
    return operation;
    }

// ---------------------------------------------------------------------------
// CDrmUiErrorHandling::HandleErrorL
// RFile based method
// ---------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUiErrorHandling::HandleErrorL(
    RFile& aFile,
    ContentAccess::TIntent aIntent,
    TInt aError,
    DRM::MDrmHandleErrorObserver* aObserver )
    {
    iImplementation->HandleErrorL( aFile,
                                   aIntent,
                                   aError,
                                   aObserver);
    }

// ---------------------------------------------------------------------------
// CDrmUiErrorHandling::HandleErrorAsyncL
// ContentAccess::CData based method
// ---------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiErrorHandling::HandleErrorAsyncL(
    ContentAccess::CData& aFile,
    ContentAccess::TIntent aIntent,
    TInt aError,
    DRM::MDrmHandleErrorObserver& aObserver )
    {
    TInt operation = iImplementation->HandleErrorAsyncL( aFile,
                                                         aIntent,
                                                         aError,
                                                         aObserver);
    return operation;
    }

// ---------------------------------------------------------------------------
// CDrmUiErrorHandling::HandleErrorL
// ContentAccess::CData based method
// ---------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUiErrorHandling::HandleErrorL(
    ContentAccess::CData& aFile,
    ContentAccess::TIntent aIntent,
    TInt aError,
    DRM::MDrmHandleErrorObserver* aObserver )
    {
    iImplementation->HandleErrorL( aFile,
                                   aIntent,
                                   aError,
                                   aObserver);
    }



// End of File

