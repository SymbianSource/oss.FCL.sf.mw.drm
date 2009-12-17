/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Notification Event object base class
*
*/



// INCLUDE FILES
#include    <DRMEvent.h>

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
// MDRMEvent::MDRMEvent
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C MDRMEvent::MDRMEvent( 
    const TDRMEventType& aEvent ) : iType( aEvent ) 
    {
    };
    
// Destructor
EXPORT_C MDRMEvent::~MDRMEvent()
    {
    };


// -----------------------------------------------------------------------------
// MDRMEvent::ExternalizeL
// Writes the data of the object into the given stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void MDRMEvent::ExternalizeL(
    RWriteStream& aOutput)
    {
    // this implementation may need to be changed if the type of the internal
    // data is changed
    aOutput.WriteUint32L(iType);
    };


// -----------------------------------------------------------------------------
// MDRMEvent::InternalizeL
// Reads the data of the object from the given stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void MDRMEvent::InternalizeL(
    RReadStream& aInput) 
    {
    // this implementation may need to be changed if the type of the internal
    // data is changed
    iType = aInput.ReadUint32L();
    };

// -----------------------------------------------------------------------------
// MDRMEvent::GetEventType
// Returns the event type of the object a return parameter
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void MDRMEvent::GetEventType(
    TDRMEventType& aEventType) const 
    {
    aEventType = iType;
    };

//  End of File  
