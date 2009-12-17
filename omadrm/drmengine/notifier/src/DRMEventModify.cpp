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
#include    "DRMEventModify.h"

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
// CDRMEventModify::NewLC
// Two phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMEventModify* CDRMEventModify::NewLC()
    {
    CDRMEventModify* self = new (ELeave) CDRMEventModify( 0 );
    CleanupStack::PushL( self );
    return self;     
    };

// -----------------------------------------------------------------------------
// CDRMEventModify::NewL
// Two phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMEventModify* CDRMEventModify::NewL()
    {
    CDRMEventModify* self = NewLC();
    CleanupStack::Pop();
    return self;    
    };

// -----------------------------------------------------------------------------
// CDRMEventModify::CDRMEventModify
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMEventModify::CDRMEventModify( TUint32 aUniqueID ) : 
    MDRMEvent( KEventModify ), 
    iContentID( NULL ),
    iUniqueID( aUniqueID )
    {
    };

// Destructor
EXPORT_C CDRMEventModify::~CDRMEventModify()
    {
    if( iContentID )
        {
        delete iContentID;
        }
    };

// -----------------------------------------------------------------------------
// CDRMEventModify::SetContentIDL
// Reserves memory for the content id, if one has been defined it is freed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMEventModify::SetContentIDL( const TDesC8& aContentID )
    {
    if ( iContentID )
        {
        delete iContentID;
        iContentID = NULL;
        }
    iContentID = aContentID.AllocL();
    };

// -----------------------------------------------------------------------------
// CDRMEventModify::SetUniqueID
// sets the unique id variable
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMEventModify::SetUniqueID( const TUint32 aUniqueID )
    {
    iUniqueID = aUniqueID;
    };

// -----------------------------------------------------------------------------
// CDRMEventModify::Status
// returns the unique id variable
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CDRMEventModify::UniqueID() const
    {
    return iUniqueID;
    };

// -----------------------------------------------------------------------------
// CDRMEventModify::GetContentIDL
// returns the content id as parameter
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CDRMEventModify::GetContentIDL() const 
    {
    return iContentID->AllocL();
    };            
    
// -----------------------------------------------------------------------------
// CDRMEventModify::ExternalizeL
// Writes the data of the object into the given stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMEventModify::ExternalizeL(
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
    aOutput.WriteUint32L(iUniqueID);
    };


// --------------------------------------------------------------------------- --
// CDRMEventModify::Internalize
// Reads the data of the object from the given stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMEventModify::InternalizeL(
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

    // Read the HBufC8*
    TPtr8 inRead(iContentID->Des());
    aInput.ReadL(inRead,inputLength); 

    // this implementation may need to be changed if the size of the enum changes
    iUniqueID = aInput.ReadUint32L();
    };

// End of File
