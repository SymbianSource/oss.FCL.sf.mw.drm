/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DRM Rights Information Handling
*
*/


// INCLUDE FILES
#include <drmutility.h>
#include <drmrightsinfo.h>
#include <drmasyncobserver.h>

#include "DrmRightsInfoImpl.h"

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDrmRightsInfo::CDrmRightsInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
DRM::CDrmRightsInfo::CDrmRightsInfo()
    {
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void DRM::CDrmRightsInfo::ConstructL()
    {
    iImplementation = DRM::CDrmRightsInfoImpl::NewL();
    iUtility = DRM::CDrmUtility::NewL();
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmRightsInfo* DRM::CDrmRightsInfo::NewL()
    {
    DRM::CDrmRightsInfo* self( NewLC() );

    CleanupStack::Pop( self );

    return self;
    }


// -----------------------------------------------------------------------------
// CDrmRightsInfo::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmRightsInfo* DRM::CDrmRightsInfo::NewLC()
    {
    DRM::CDrmRightsInfo* self( new( ELeave ) CDrmRightsInfo );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }


// Destructor
DRM::CDrmRightsInfo::~CDrmRightsInfo()
    {
    delete iImplementation;
    delete iUtility;
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfo::GetUtility
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmUtility& DRM::CDrmRightsInfo::GetUtility() const
    {
    return *iUtility;
    }


// -----------------------------------------------------------------------------
// CDrmRightsInfo::CheckRightsL
// Syncronous method
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmRightsInfo::CheckRightsL(
    const TDesC& aUniqueId,
    ContentAccess::TIntent aIntent,
    DRM::TDrmRightsInfo& aDetails )
    {
    iImplementation->CheckRightsL( aUniqueId, aIntent, aDetails );
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfo::CheckRightsL
// Asyncronous method
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmRightsInfo::CheckRightsAsyncL(
    const TDesC& aUniqueId,
    ContentAccess::TIntent aIntent,
    DRM::TDrmRightsInfo& aDetails,
    DRM::MDrmAsyncObserver& aObserver )
    {
    TInt operation = iImplementation->CheckRightsAsyncL( aUniqueId,
                                                         aIntent,
                                                         aDetails,
                                                         aObserver );
    return operation;
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfo::CancelOperation
// Cancels the operation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmRightsInfo::CancelOperation( TInt aOperationId )
    {
    return iImplementation->CancelOperation( aOperationId );
    }

//  End of File
