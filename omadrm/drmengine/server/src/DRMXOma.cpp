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
* Description:  Datatype for the X-Oma header data
*
*/



// INCLUDE FILES
#include <s32strm.h>
#include "DRMXOma.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMXOma::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMXOma* CDRMXOma::NewLC( const TDesC8& aContentID,
                           const TTime& aTimeStamp,
                           const TTimeIntervalSeconds& aWaitTime )
    {
    CDRMXOma* self = new( ELeave ) CDRMXOma( aTimeStamp, aWaitTime );
    CleanupStack::PushL( self );
    self->ConstructL( aContentID );
    
    return self;    
    };


// -----------------------------------------------------------------------------
// CDRMXOma::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMXOma* CDRMXOma::NewL( const TDesC8& aContentID,
                          const TTime& aTimeStamp,
                          const TTimeIntervalSeconds& aWaitTime )
    {
    CDRMXOma* self = NewLC( aContentID, 
                            aTimeStamp, aWaitTime );
    CleanupStack::Pop();
    
    return self;
    };        

// -----------------------------------------------------------------------------
// CDRMXOma::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMXOma* CDRMXOma::NewLC()
    {
    CDRMXOma* self = new( ELeave ) CDRMXOma();
    CleanupStack::PushL( self );
    
    return self;    
    };


// -----------------------------------------------------------------------------
// CDRMXOma::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMXOma* CDRMXOma::NewL()
    {
    CDRMXOma* self = NewLC();
    CleanupStack::Pop();
    
    return self;
    };          

// -----------------------------------------------------------------------------
// Default Constructor - First phase.
// -----------------------------------------------------------------------------
//
CDRMXOma::CDRMXOma( const TTime& aTimeStamp, 
                   const TTimeIntervalSeconds& aWaitTime ) :
    iTimeStamp( aTimeStamp ),
    iWaitTime( aWaitTime )
    {
    
    }; 

// -----------------------------------------------------------------------------
// Default Constructor - First phase.
// -----------------------------------------------------------------------------
//
CDRMXOma::CDRMXOma()
    {
    
    }; 

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CDRMXOma::~CDRMXOma()
    {
    if( iContentID )
        {
        delete iContentID;
        iContentID = NULL;
        }        
    };
        
        
// -----------------------------------------------------------------------------
// CDRMXOma::ContentID
// -----------------------------------------------------------------------------
//        
const TDesC8& CDRMXOma::ContentID() const
    {
    return *iContentID;
    };

// -----------------------------------------------------------------------------
// CDRMXOma::TimeStamp
// -----------------------------------------------------------------------------
//    
const TTime& CDRMXOma::TimeStamp() const
    {
    return iTimeStamp;
    };

// -----------------------------------------------------------------------------
// CDRMXOma::WaitTime
// -----------------------------------------------------------------------------
//    
const TTimeIntervalSeconds& CDRMXOma::WaitTime() const
    {
    return iWaitTime;
    };

// -----------------------------------------------------------------------------
// CDRMXOma::SetContentIDL
// -----------------------------------------------------------------------------
//         
void CDRMXOma::SetContentIDL( const TDesC8& aContentID )
    {
    HBufC8* newContentID = aContentID.AllocL();
    
    if( iContentID )
        {
        delete iContentID;
        iContentID = NULL;
        }
    iContentID = newContentID;    
    };
    
// -----------------------------------------------------------------------------
// CDRMXOma::SetTimeStampL
// -----------------------------------------------------------------------------
//         
void CDRMXOma::SetTimeStampL( const TTime& aTimeStamp )
    {
    iTimeStamp = aTimeStamp;    
    };    

// -----------------------------------------------------------------------------
// CDRMXOma::SetWaitTimeL
// -----------------------------------------------------------------------------
//         
void CDRMXOma::SetWaitTimeL( const TTimeIntervalSeconds& aWaitTime )
    {
    iWaitTime = aWaitTime;    
    }; 

// -----------------------------------------------------------------------------
// CDRMXOma::ExternalizeL
// -----------------------------------------------------------------------------
// 
void CDRMXOma::ExternalizeL( RWriteStream& aStream ) const
    {
    TInt dataLength = 0;
    
    // Write the ContentID
    dataLength = 0;
    if( iContentID )
        {
        dataLength = iContentID->Length();
        }
    aStream.WriteInt32L( dataLength );
    
    if( dataLength )
        {
        aStream.WriteL( iContentID->Des() );        
        }
    
    // Write the time stamp
    WriteInt64L( iTimeStamp.Int64(), aStream );
    
    // write the wait time
    aStream.WriteInt32L( iWaitTime.Int() );
    };

// -----------------------------------------------------------------------------
// CDRMXOma::InternalizeL
// -----------------------------------------------------------------------------
// 
void CDRMXOma::InternalizeL( RReadStream& aStream )
    {
    TInt64 timeData = 0;    
    TInt dataLength = 0;
    HBufC8* dataPart = 0;
    TPtr8 dataBuffer(NULL,0,0);
    TPtr16 dataBuffer2(NULL,0,0);
    
    // Read the ContentID
    dataLength = aStream.ReadInt32L();
    
    if( dataLength )
        {
        // Reserve a new buffer:
        dataPart = HBufC8::NewMaxLC( dataLength );
        
        // Set the read buffer:
        dataBuffer.Set(const_cast<TUint8*>(dataPart->Ptr()), 0, dataLength);
        
        // Read the data:
        aStream.ReadL( dataBuffer );
        
        // Pop the buffer 
        CleanupStack::Pop(); // dataPart
                
        // If an old content identifier exists delete it        
        if( iContentID )
            {
            delete iContentID;
            iContentID = NULL;
            }
        
        // assign the new content id
        iContentID = dataPart;    
        }
    else
        {
        // If an old content identifier exists delete it 
        if( iContentID )
            {
            delete iContentID;
            iContentID = NULL;
            }        
        }           
        
    // The Time stamp
    ReadInt64L( timeData, aStream );
    iTimeStamp = timeData;    
                   
    // The time interval                
    iWaitTime =  aStream.ReadInt32L();               
    };   

// -----------------------------------------------------------------------------
// CDRMXOma::Size
// -----------------------------------------------------------------------------
// 
TInt CDRMXOma::Size() const
    {
    TInt size = 0;
    
    // Content identifier of the content
    size += sizeof(TInt32);
    
    if( iContentID )
        {
        size += iContentID->Size();
        }
        
    // Issuer of the rights
    size += sizeof(TTime);
        
    // Content name
    size += sizeof(TTimeIntervalSeconds);
    
    return size;
    };    
 
// -----------------------------------------------------------------------------
// CDRMXOma::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
// 
void CDRMXOma::ConstructL( const TDesC8& aContentID )
    {
    iContentID = aContentID.AllocL();
    };                


// -----------------------------------------------------------------------------
// CDRMXOma::WriteInt64L
// -----------------------------------------------------------------------------
//
void CDRMXOma::WriteInt64L( const TInt64& aWrite, RWriteStream& aStream ) const
    {
    TPtr8 output( reinterpret_cast<TUint8*>(const_cast<TInt64*>(&aWrite)), 
                  sizeof(TInt64), sizeof(TInt64) );
        
    aStream.WriteL( output, sizeof(TInt64) );
    }

// -----------------------------------------------------------------------------
// CCDRMXOma::ReadInt64L
// -----------------------------------------------------------------------------
//
void CDRMXOma::ReadInt64L( TInt64& aRead, RReadStream& aStream )
    {
    TPtr8 input( reinterpret_cast<TUint8*>(&aRead), 0, sizeof(TInt64) );
    
    aStream.ReadL( input, sizeof(TInt64) );    
    };
            
// End of File
