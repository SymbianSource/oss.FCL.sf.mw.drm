/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DRM UI Handling
*
*/


// INCLUDE FILES
#include <drmutility.h>
#include <drmuihandling.h>
#include <coemain.h>

#include "drmuierrorhandling.h"
#include "DrmUiHandlingImpl.h"

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDrmUiHandling::CDrmUiHandling
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
DRM::CDrmUiHandling::CDrmUiHandling( )
    {
    }

// -----------------------------------------------------------------------------
// CDrmUiHandling::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void DRM::CDrmUiHandling::ConstructL( CCoeEnv* aCoeEnv )
    {
    // CDrmUtility instance
    iUtility = DRM::CDrmUtility::NewL();

    // Implementation instance
    iImplementation = DRM::CDrmUiHandlingImpl::NewL(
        iUtility, aCoeEnv ? aCoeEnv : CCoeEnv::Static() );

    // Does not take ownership of the iImplementation, just uses it
    iErrorHandler = DRM::CDrmUiErrorHandling::NewL( iImplementation );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandling::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmUiHandling* DRM::CDrmUiHandling::NewL( CCoeEnv* aCoeEnv )
    {
    DRM::CDrmUiHandling* self( NewLC( aCoeEnv ) );

    CleanupStack::Pop( self );

    return self;
    }


// -----------------------------------------------------------------------------
// CDrmUiHandling::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmUiHandling* DRM::CDrmUiHandling::NewLC( CCoeEnv* aCoeEnv )
    {
   DRM:: CDrmUiHandling* self( new( ELeave ) CDrmUiHandling );

    CleanupStack::PushL( self );
    self->ConstructL( aCoeEnv );

    return self;
    }


// Destructor
DRM::CDrmUiHandling::~CDrmUiHandling()
    {
    delete static_cast< DRM::CDrmUiErrorHandling* >( iErrorHandler );
    delete iImplementation;
    delete iUtility;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandling::GetUtility
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmUtility& DRM::CDrmUiHandling::GetUtility() const
    {
    return *iUtility;
    }


// -----------------------------------------------------------------------------
// CDrmUiHandling::GetErrorHandler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::MDrmErrorHandling& DRM::CDrmUiHandling::GetErrorHandler() const
    {
    return *iErrorHandler;
    }



// -----------------------------------------------------------------------------
// CDrmUiHandling::CheckRightsAmountAsyncL
// Check how much rights there are left for the content.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiHandling::CheckRightsAmountAsyncL(
    RFile& aFile,
    ContentAccess::TIntent aIntent,
    DRM::MDrmUiCheckRightsObserver& aObserver )
    {
    TInt operation = iImplementation->CheckRightsAmountAsyncL( aFile,
                                                               aIntent,
                                                               aObserver );
    return operation;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandling::CheckRightsAmount
// Check how much rights there are left for the content.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUiHandling::CheckRightsAmountL(
    RFile& aFile,
    ContentAccess::TIntent aIntent,
    DRM::MDrmUiCheckRightsObserver* aObserver )
    {
    iImplementation->CheckRightsAmountL( aFile, aIntent, aObserver );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandling::CheckRightsAmountAsyncL
// Check how much rights there are left for the content.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiHandling::CheckRightsAmountAsyncL(
    ContentAccess::CData& aFile,
    ContentAccess::TIntent aIntent,
    DRM::MDrmUiCheckRightsObserver& aObserver )
    {
    TInt operation = iImplementation->CheckRightsAmountAsyncL( aFile,
                                                               aIntent,
                                                               aObserver );
    return operation;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandling::CheckRightsAmountL
// Check how much rights there are left for the content.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUiHandling::CheckRightsAmountL(
    ContentAccess::CData& aFile,
    ContentAccess::TIntent aIntent,
    DRM::MDrmUiCheckRightsObserver* aObserver )
    {
    iImplementation->CheckRightsAmountL( aFile, aIntent, aObserver );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandling::ShowDetailsViewAsyncL
// Shows DRM specif details
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiHandling::ShowDetailsViewAsyncL(
    RFile& aFile,
    DRM::MDrmAsyncObserver& aObserver )
    {
    TInt operation = iImplementation->ShowDetailsViewAsyncL( aFile, aObserver );
    return operation;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandling::ShowDetailsViewL
// Shows DRM specif details
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUiHandling::ShowDetailsViewL( RFile& aFile )
    {
    iImplementation->ShowDetailsViewL( aFile );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandling::ShowDetailsViewAsyncL
// Shows DRM specif details
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiHandling::ShowDetailsViewAsyncL(
    ContentAccess::CData& aFile,
    DRM::MDrmAsyncObserver& aObserver )
    {
    TInt operation = iImplementation->ShowDetailsViewAsyncL( aFile, aObserver );
    return operation;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandling::ShowDetailsViewL
// Shows DRM specif details
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUiHandling::ShowDetailsViewL( ContentAccess::CData& aFile )
    {
    iImplementation->ShowDetailsViewL( aFile );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandling::HandleUrlAsyncL
// Handles the specific url defined by the file
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiHandling::HandleUrlAsyncL(
    RFile& aFile,
    DRM::TDrmUiUrlType aType,
    DRM::MDrmAsyncObserver& aObserver )
    {
    TInt operation = iImplementation->HandleUrlAsyncL( aFile, aType, aObserver );
    return operation;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandling::HandleUrlL
// Handles the specific url defined by the file
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUiHandling::HandleUrlL(
    RFile& aFile,
    DRM::TDrmUiUrlType aType )
    {
    iImplementation->HandleUrlL( aFile, aType );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandling::HandleUrlAsyncL
// Handles the specific url defined by the file
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiHandling::HandleUrlAsyncL(
    ContentAccess::CData& aFile,
    DRM::TDrmUiUrlType aType,
    DRM::MDrmAsyncObserver& aObserver  )
    {
    TInt operation = iImplementation->HandleUrlAsyncL( aFile, aType, aObserver );
    return operation;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandling::HandleUrlL
// Handles the specific url defined by the file
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUiHandling::HandleUrlL(
    ContentAccess::CData& aFile,
    DRM::TDrmUiUrlType aType )
    {
    iImplementation->HandleUrlL( aFile, aType );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandling::AvailableUrlsAsyncL
// Gets information of the available urls
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiHandling::AvailableUrlsAsyncL(
    RFile& aFile,
    DRM::TDrmUiUrlType& aType,
    DRM::MDrmAsyncObserver& aObserver )
    {
    TInt operation = iImplementation->AvailableUrlsAsyncL( aFile, aType, aObserver );
    return operation;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandling::AvailableUrlsL
// Gets information of the available urls
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUiHandling::AvailableUrlsL(
    RFile& aFile,
    DRM::TDrmUiUrlType& aType )
    {
    iImplementation->AvailableUrlsL( aFile, aType );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandling::AvailableUrlsAsyncL
// Gets information of the available urls
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiHandling::AvailableUrlsAsyncL(
    ContentAccess::CData& aFile,
    DRM::TDrmUiUrlType& aType,
    DRM::MDrmAsyncObserver& aObserver  )
    {
    TInt operation = iImplementation->AvailableUrlsAsyncL( aFile, aType, aObserver );
    return operation;
    }

// -----------------------------------------------------------------------------
// CDrmUiHandling::AvailableUrlsL
// Gets information of the available urls
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUiHandling::AvailableUrlsL(
    ContentAccess::CData& aFile,
    DRM::TDrmUiUrlType& aType )
    {
    iImplementation->AvailableUrlsL( aFile, aType );
    }

// -----------------------------------------------------------------------------
// CDrmUiHandling::CancelOperation
// Cancels the operation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUiHandling::CancelOperation( TInt aOperationId )
    {
    return iImplementation->CancelOperation( aOperationId );
    }


//  End of File
