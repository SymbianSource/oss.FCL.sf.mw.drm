/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class handles registering and unregistering of
*                DRM event observers to DRM notifier, and sending
*                DRM notifications to other registered observers.
*
*/



// INCLUDE FILES
#include    "DRMNotifier.h"
#include    "DRMEventHandler.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMNotifier::CDRMNotifier
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMNotifier::CDRMNotifier( void ) : iEventHandler( NULL )
    {
    };
    
// -----------------------------------------------------------------------------
// CDRMNotifier::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDRMNotifier::ConstructL()
    {
    iEventHandler = CDRMEventHandler::NewL();
    }


// -----------------------------------------------------------------------------
// CDRMNotifier::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMNotifier* CDRMNotifier::NewL()
    {
    CDRMNotifier* self = NewLC();
    CleanupStack::Pop();

    return self;
    };

// -----------------------------------------------------------------------------
// CDRMNotifier::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMNotifier* CDRMNotifier::NewLC()
    {
    CDRMNotifier* self = new( ELeave ) CDRMNotifier;
    
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    };



// Destructor
EXPORT_C CDRMNotifier::~CDRMNotifier()
    {
    if ( iEventHandler )
        {
        delete iEventHandler;
        }
    };

// -----------------------------------------------------------------------------
// CDRMNotifier::RegisterEventObserverL
// Registers the observer to the event handler
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMNotifier::RegisterEventObserverL(
    MDRMEventObserver& aObserver,
    const TDRMEventType& aEvent )
    {
    iEventHandler->RegisterEventObserverL( aObserver,aEvent );
    }

// -----------------------------------------------------------------------------
// CDRMNotifier::UnRegisterEventObserverL
// Unregisters the observer from the event handler 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMNotifier::UnRegisterEventObserverL(
    MDRMEventObserver& aObserver,
    const TDRMEventType& aEvent )
    {
    iEventHandler->UnRegisterEventObserverL( aObserver, aEvent );
    }

// -----------------------------------------------------------------------------
// CDRMNotifier::RegisterEventObserverL
// Registers the observer to the event handler
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMNotifier::RegisterEventObserverL(
    MDRMEventObserver& aObserver,
    const TDRMEventType& aEvent,
    const TDesC8& aContentID )
    {
    iEventHandler->RegisterEventObserverL( aObserver, aEvent, aContentID );
    }

// -----------------------------------------------------------------------------
// CDRMNotifier::UnRegisterEventObserverL
// Unregisters the observer from the event handler 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMNotifier::UnRegisterEventObserverL(
    MDRMEventObserver& aObserver,
    const TDRMEventType& aEvent,
    const TDesC8& aContentID )
    {
    iEventHandler->UnRegisterEventObserverL( aObserver, aEvent, aContentID );
    }

// -----------------------------------------------------------------------------
// CDRMNotifier::UnRegisterEventObserverL
// Unregisters the observer from the event handler 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMNotifier::SendEventL( MDRMEvent& aEvent, TRequestStatus& aStatus )
    {
    iEventHandler->SendEventL( aEvent, aStatus );
    }

//  End of File  
