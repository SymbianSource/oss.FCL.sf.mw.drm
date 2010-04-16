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
* Description:  DRM Automated Usage handling
*
*/



// INCLUDE FILES
#include    <caf/caftypes.h>
#include    <caf/data.h>
#include    <drmautomatedusage.h>

#include    "DrmAutomatedUsageImpl.h"
#include    "drmautomatedusageerrorhandling.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::CDrmAutomatedUsage
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
DRM::CDrmAutomatedUsage::CDrmAutomatedUsage()
    {
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void DRM::CDrmAutomatedUsage::ConstructL( CCoeEnv* aCoeEnv )
    {
    // Create the utility instance
    iUtility = DRM::CDrmUtility::NewL();

    // Create the implementation class instance
    iImplementation = DRM::CDrmAutomatedUsageImpl::NewL( aCoeEnv, iUtility );

    // Create the error handler
    iErrorHandler = DRM::CDrmAutomatedUsageErrorHandling::NewL(
        iImplementation );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmAutomatedUsage* DRM::CDrmAutomatedUsage::NewL(
    CCoeEnv* aCoeEnv )
    {
    DRM::CDrmAutomatedUsage* self( DRM::CDrmAutomatedUsage::NewLC( aCoeEnv ) );

    CleanupStack::Pop( self );

    return self;
    }


// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::NewLC
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmAutomatedUsage* DRM::CDrmAutomatedUsage::NewLC(
    CCoeEnv* aCoeEnv )
    {
    DRM::CDrmAutomatedUsage* self( new( ELeave ) DRM::CDrmAutomatedUsage );

    CleanupStack::PushL( self );
    self->ConstructL( aCoeEnv );

    return self;
    }


// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::~CDrmAutomatedUsage
// Destructor
// -----------------------------------------------------------------------------
//
DRM::CDrmAutomatedUsage::~CDrmAutomatedUsage()
    {
    delete static_cast< DRM::CDrmAutomatedUsageErrorHandling* >(
        iErrorHandler );
    delete iImplementation;
    delete iUtility;
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::GetUtility
// (more comments in header file)
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmUtility& DRM::CDrmAutomatedUsage::GetUtility() const
    {
    return *iUtility;
    }


// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::GetErrorHandler
// (more comments in header file)
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::MDrmErrorHandling& DRM::CDrmAutomatedUsage::GetErrorHandler()
    const
    {
    return *iErrorHandler;
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::CanSetAutomatedL
// Checks whether given content can be set as automated.
// (more comments in header file)
// -----------------------------------------------------------------------------
//
EXPORT_C TBool DRM::CDrmAutomatedUsage::CanSetAutomatedL(
    RFile& aFile,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType )
    {
    iUtility->CheckFileHandleL( aFile );
    return iImplementation->CanSetAutomatedL( aFile, aIntent, aAutomatedType );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::CanSetAutomatedL
// Checks whether given content can be set as automated.
// (more comments in header file)
// -----------------------------------------------------------------------------
//
EXPORT_C TBool DRM::CDrmAutomatedUsage::CanSetAutomatedL(
    const ContentAccess::CData& aData,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType )
    {
    return iImplementation->CanSetAutomatedL( aData, aIntent, aAutomatedType );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::SetAutomatedL
// Sets given content as automated.
// (more comments in header file)
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmAutomatedUsage::SetAutomatedL(
    RFile& aFile,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType,
    const DRM::TDrmAutomatedServiceType aServiceType )
    {
    iUtility->CheckFileHandleL( aFile );
    iImplementation->SetAutomatedL( aFile,
                                    aIntent,
                                    aAutomatedType,
                                    aServiceType );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::SetAutomatedAsyncL
// Sets given content asynchronously as automated.
// (more comments in header file)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmAutomatedUsage::SetAutomatedAsyncL(
    RFile& aFile,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType,
    DRM::MDrmAsyncObserver& aObserver,
    const DRM::TDrmAutomatedServiceType aServiceType )
    {
    iUtility->CheckFileHandleL( aFile );
    return iImplementation->SetAutomatedAsyncL( aFile,
                                                aIntent,
                                                aAutomatedType,
                                                aObserver,
                                                aServiceType );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::SetAutomatedL
// Sets given content as automated.
// (more comments in header file)
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmAutomatedUsage::SetAutomatedL(
    const ContentAccess::CData& aData,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType,
    const DRM::TDrmAutomatedServiceType aServiceType )
    {
    iImplementation->SetAutomatedL( aData,
                                    aIntent,
                                    aAutomatedType,
                                    aServiceType );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::SetAutomatedAsyncL
// Sets given content asynchronously as automated.
// (more comments in header file)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmAutomatedUsage::SetAutomatedAsyncL(
    const ContentAccess::CData& aData,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType,
    DRM::MDrmAsyncObserver& aObserver,
    const DRM::TDrmAutomatedServiceType aServiceType )
    {
    return iImplementation->SetAutomatedAsyncL( aData,
                                                aIntent,
                                                aAutomatedType,
                                                aObserver,
                                                aServiceType );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::RemoveAutomatedL
// Removes content from automated use.
// (more comments in header file)
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmAutomatedUsage::RemoveAutomatedL(
    RFile& aFile,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType,
    const DRM::TDrmAutomatedServiceType aServiceType )
    {
    iUtility->CheckFileHandleL( aFile );
    iImplementation->RemoveAutomatedL( aFile,
                                       aIntent,
                                       aAutomatedType,
                                       aServiceType );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::RemoveAutomatedAsyncL
// Removes content from automated use
// (more comments in header file)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmAutomatedUsage::RemoveAutomatedAsyncL(
    RFile& aFile,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType,
    DRM::MDrmAsyncObserver& aObserver,
    const DRM::TDrmAutomatedServiceType aServiceType )
    {
    iUtility->CheckFileHandleL( aFile );
    return iImplementation->RemoveAutomatedAsyncL( aFile,
                                                   aIntent,
                                                   aAutomatedType,
                                                   aObserver,
                                                   aServiceType );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::RemoveAutomatedL
// Removes content from automated use
// (more comments in header file)
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmAutomatedUsage::RemoveAutomatedL(
    const ContentAccess::CData& aData,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType,
    const DRM::TDrmAutomatedServiceType aServiceType )
    {
    iImplementation->RemoveAutomatedL( aData,
                                       aIntent,
                                       aAutomatedType,
                                       aServiceType );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::RemoveAutomatedAsyncL
// Removes content from automated use
// (more comments in header file)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmAutomatedUsage::RemoveAutomatedAsyncL(
    const ContentAccess::CData& aData,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType,
    DRM::MDrmAsyncObserver& aObserver,
    const DRM::TDrmAutomatedServiceType aServiceType )
    {
    return iImplementation->RemoveAutomatedAsyncL( aData,
                                                   aIntent,
                                                   aAutomatedType,
                                                   aObserver,
                                                   aServiceType );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::RemoveAutomatedL
// Removes content from automated use
// (more comments in header file)
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmAutomatedUsage::RemoveAutomatedL(
    const TDesC& aUniqueId,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType,
    const DRM::TDrmAutomatedServiceType aServiceType )
    {
    iImplementation->RemoveAutomatedL( aUniqueId,
                                       aIntent,
                                       aAutomatedType,
                                       aServiceType );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::RemoveAutomatedAsyncL
// Removes content from automated use
// (more comments in header file)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmAutomatedUsage::RemoveAutomatedAsyncL(
    const TDesC& aUniqueId,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType,
    DRM::MDrmAsyncObserver& aObserver,
    const DRM::TDrmAutomatedServiceType aServiceType )
    {
    return iImplementation->RemoveAutomatedAsyncL( aUniqueId,
                                                   aIntent,
                                                   aAutomatedType,
                                                   aObserver,
                                                   aServiceType );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::RegisterEventObserverL
// Registers the client to listen for automated usage events
// (more comments in header file)
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmAutomatedUsage::RegisterEventObserverL(
    DRM::MDrmAutomatedUsageObserver& aObserver )
    {
    iImplementation->RegisterEventObserverL( aObserver );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::UnregisterEventObserverL
// Unregisters the client not to listen for automated usage events
// (more comments in header file)
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmAutomatedUsage::UnregisterEventObserverL(
    DRM::MDrmAutomatedUsageObserver& aObserver )
    {
    iImplementation->UnregisterEventObserverL( aObserver );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsage::CancelOperation
// Cancels the operation
// (more comments in header file)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmAutomatedUsage::CancelOperation( TInt aOperationId )
    {
    return iImplementation->CancelOperation( aOperationId );
    }

//  End of File
