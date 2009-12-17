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
* Description:  Notification Event object base class
*
*/



// INCLUDE FILES
#include    "DRMEventAddRemove.h"

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
// CDRMEventAddRemove::NewLC
// two phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMEventAddRemove* CDRMEventAddRemove::NewLC( TAddRemoveEventStatus aStatus )
    {
    CDRMEventAddRemove* self = new (ELeave) CDRMEventAddRemove( aStatus );
    CleanupStack::PushL( self );
    return self; 
    };

// -----------------------------------------------------------------------------
// CDRMEventAddRemove::NewL
// two phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMEventAddRemove* CDRMEventAddRemove::NewL( TAddRemoveEventStatus aStatus )
    {
    CDRMEventAddRemove* self = NewLC( aStatus );
    CleanupStack::Pop();
    return self;
    };





// -----------------------------------------------------------------------------
// CDRMEventAddRemove::CDRMEventAddRemove
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMEventAddRemove::CDRMEventAddRemove(
    TAddRemoveEventStatus aStatus ) : MDRMEvent( KEventAddRemove ), 
                                      iContentID( NULL ),
                                      iStatus( aStatus )
    {
    };

// Destructor
EXPORT_C CDRMEventAddRemove::~CDRMEventAddRemove()
    {
    if( iContentID )
        {
        delete iContentID;
        }
    };



// -----------------------------------------------------------------------------
// CDRMEventAddRemove::SetContentIDL
// Reserves memory for a copy of the content id and releases the old memory
// from the object if it already exists
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMEventAddRemove::SetContentIDL( const TDesC8& aContentID )
    {
    if( iContentID )
        {
        delete iContentID;
        iContentID = NULL;
        }
    iContentID = aContentID.AllocL();
    };


// -----------------------------------------------------------------------------
// CDRMEventAddRemove::Status
// Returns the status of the object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TAddRemoveEventStatus CDRMEventAddRemove::Status() const
    {
    return iStatus;
    };

// -----------------------------------------------------------------------------
// CDRMEventAddRemove::GetContentIDL
// reserves memory and returns a pointer to this new object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CDRMEventAddRemove::GetContentIDL() const 
    {
    return iContentID->AllocL();
    };
    
// -----------------------------------------------------------------------------
// CDRMEventAddRemove::ExternalizeL
// Writes the data of the object into the given stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMEventAddRemove::ExternalizeL(
    RWriteStream& aOutput)
    {
    TInt outputLength = 0;

    MDRMEvent::ExternalizeL(aOutput);

    // get the output length of the HBufC8*
    outputLength = iContentID->Size();
    aOutput.WriteInt32L(outputLength);

    // write out the HBufC8*
    aOutput.WriteL(*iContentID);

    // this implementation may need to be changed if the size of the enum changes
    aOutput.WriteInt8L(iStatus);
    };


// -----------------------------------------------------------------------------
// CDRMEventAddRemove::Internalize
// Reads the data of the object from the given stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMEventAddRemove::InternalizeL(
    RReadStream& aInput) 
    {
    TInt inputLength = 0;

    MDRMEvent::InternalizeL(aInput);

    // Read the length of the HBufC8*
    inputLength = aInput.ReadInt32L();

    // Reserve the HBufC8*
    if( !iContentID )
        {
        iContentID = HBufC8::NewL(inputLength);
        }
    else
        {
        iContentID->ReAllocL(inputLength);
        }

    TPtr8 inRead(iContentID->Des());

    // Read the HBufC8*
    aInput.ReadL(inRead, inputLength); 

    // this implementation may need to be changed if the size of the enum changes
    iStatus = static_cast<TAddRemoveEventStatus>(aInput.ReadInt8L());
    };

// End of File
