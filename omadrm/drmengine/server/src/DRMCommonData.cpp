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
* Description:  Datatype for the Common Rights Database Data
*
*/



// INCLUDE FILES
#include <s32strm.h>
#include "DRMCommonData.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
_LIT8( KOwnNullDesC8, "" );
_LIT( KOwnNullDesC, "" );

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMCommonData::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMCommonData* CDRMCommonData::NewLC( const TDesC8& aContentID,
                                       const TDesC8& aContentHash,
                                       const TDesC8& aRightsIssuer,
                                       const TDesC& aContentName,
                                       const TDesC8& aAuthenticationSeed )
    {
    CDRMCommonData* self = new( ELeave ) CDRMCommonData();
    CleanupStack::PushL( self );
    self->ConstructL( aContentID, aContentHash, 
                      aRightsIssuer, aContentName,
                      aAuthenticationSeed );
    
    return self;    
    };


// -----------------------------------------------------------------------------
// CDRMCommonData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMCommonData* CDRMCommonData::NewL( const TDesC8& aContentID,
                                      const TDesC8& aContentHash,
                                      const TDesC8& aRightsIssuer,
                                      const TDesC& aContentName,
                                      const TDesC8& aAuthenticationSeed )
    {
    CDRMCommonData* self = NewLC( aContentID, aContentHash, 
                                  aRightsIssuer, aContentName,
                                  aAuthenticationSeed );
    CleanupStack::Pop();
    
    return self;
    };        


// -----------------------------------------------------------------------------
// CDRMCommonData::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMCommonData* CDRMCommonData::NewLC()
    {
    CDRMCommonData* self = new( ELeave ) CDRMCommonData();
    CleanupStack::PushL( self );
    
    return self;    
    };


// -----------------------------------------------------------------------------
// CDRMCommonData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMCommonData* CDRMCommonData::NewL()
    {
    CDRMCommonData* self = NewLC();
    CleanupStack::Pop();
    
    return self;
    };           

// -----------------------------------------------------------------------------
// Default Constructor - First phase.
// Can be used by itself to generate an empty object
// -----------------------------------------------------------------------------
//
CDRMCommonData::CDRMCommonData()
    {
    
    }; 

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CDRMCommonData::~CDRMCommonData()
    {
    if( iContentID )
        {
        delete iContentID;
        iContentID = NULL;
        }
        
    if( iContentHash )
        {
        delete iContentHash;
        iContentHash = NULL;
        }
        
    if( iRightsIssuer )
        {
        delete iRightsIssuer;
        iRightsIssuer = NULL;
        }
        
    if( iContentName )
        {
        delete iContentName;
        iContentName = NULL;
        }

    if( iAuthenticationSeed )
        {
        delete iAuthenticationSeed;
        iAuthenticationSeed = NULL;	
        }   
    };
        
        
// -----------------------------------------------------------------------------
// CDRMCommonData::ContentID
// -----------------------------------------------------------------------------
//        
const TDesC8& CDRMCommonData::ContentID() const
    {
    if ( iContentID )
        {
        return *iContentID;
        }
    return KOwnNullDesC8;
    };

// -----------------------------------------------------------------------------
// CDRMCommonData::ContentHash
// -----------------------------------------------------------------------------
// 
const TDesC8& CDRMCommonData::ContentHash() const
    {
    if ( iContentHash )
        {
        return *iContentHash;
        }
    
    return KOwnNullDesC8;
    };

// -----------------------------------------------------------------------------
// CDRMCommonData::RightsIssuer
// -----------------------------------------------------------------------------
//    
const TDesC8& CDRMCommonData::RightsIssuer() const
    {
    if ( iRightsIssuer )
        {
        return *iRightsIssuer;
        }
    return KOwnNullDesC8;
    };
    
// -----------------------------------------------------------------------------
// CDRMCommonData::ContentName
// -----------------------------------------------------------------------------
//   
const TDesC& CDRMCommonData::ContentName() const
    {
    if ( iContentName )
        {
        return *iContentName;
        }
    
    return KOwnNullDesC;
    };    

// -----------------------------------------------------------------------------
// CDRMCommonData::AuthenticationSeed
// -----------------------------------------------------------------------------
//    
const TDesC8& CDRMCommonData::AuthenticationSeed() const
    {
    if ( iAuthenticationSeed )
        {
        return *iAuthenticationSeed;
        }
    return KOwnNullDesC8;
    };


// -----------------------------------------------------------------------------
// CDRMCommonData::SetContentIDL
// -----------------------------------------------------------------------------
//         
void CDRMCommonData::SetContentIDL( const TDesC8& aContentID )
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
// CDRMCommonData::SetContentHashL
// -----------------------------------------------------------------------------
//        
void CDRMCommonData::SetContentHashL( const TDesC8& aContentHash )
    {
    HBufC8* newContentHash = aContentHash.AllocL();
    
    if( iContentHash )
        {
        delete iContentHash;
        iContentHash = NULL;
        }
    iContentHash = newContentHash;        
    };

// -----------------------------------------------------------------------------
// CDRMCommonData::SetRightsIssuerL
// -----------------------------------------------------------------------------
//      
void CDRMCommonData::SetRightsIssuerL( const TDesC8& aRightsIssuer )
    {
    HBufC8* newRightsIssuer = aRightsIssuer.AllocL();
    
    if( iRightsIssuer )
        {
        delete iRightsIssuer;
        iRightsIssuer = NULL;
        }
    iRightsIssuer = newRightsIssuer;     
    };


// -----------------------------------------------------------------------------
// CDRMCommonData::SetContentNameL
// -----------------------------------------------------------------------------
//   
void CDRMCommonData::SetContentNameL( const TDesC& aContentName )
    {
    HBufC* newContentName = NULL; 
       
    // If the content name you want to give is empty, just delete the existing
    // one and not do anything else;
    if( aContentName.Length() == 0 )
    	{
    	if( iContentName )
        	{
        	delete iContentName;
        	iContentName = NULL;
        	}    	
    	return;	
    	}
    	
    newContentName = aContentName.AllocL();
    
    if( iContentName )
        {
        delete iContentName;
        iContentName = NULL;
        }
    iContentName = newContentName;     
    };


// -----------------------------------------------------------------------------
// CDRMCommonData::SetAuthenticationSeedL
// -----------------------------------------------------------------------------
//      
void CDRMCommonData::SetAuthenticationSeedL( const TDesC8& aAuthenticationSeed )
    {
    HBufC8* newAuthenticationSeed = aAuthenticationSeed.AllocL();
    
    if( iAuthenticationSeed )
        {
        delete iAuthenticationSeed;
        iAuthenticationSeed= NULL;
        }
    iAuthenticationSeed = newAuthenticationSeed;     
    };


// -----------------------------------------------------------------------------
// CDRMCommonData::ExternalizeL
// -----------------------------------------------------------------------------
// 
void CDRMCommonData::ExternalizeL( RWriteStream& aStream ) const
    {
    TInt dataLength = 0;
    
    // Write the ContentID
    if( iContentID )
        {
        dataLength = iContentID->Length();
        }
    aStream.WriteInt32L( dataLength );
    
    if( dataLength )
        {
        aStream.WriteL( iContentID->Des() );        
        }
        
    // Write the ContentHash
    dataLength = 0;
    if( iContentHash )
        {
        dataLength = iContentHash->Length();
        }
    aStream.WriteInt32L( dataLength );
    
    if( dataLength )
        {
        aStream.WriteL( iContentHash->Des() );        
        }
        
    // Write the Rights Issuer
    dataLength = 0;
    if( iRightsIssuer )
        {
        dataLength = iRightsIssuer->Length();
        }
    aStream.WriteInt32L( dataLength );
    
    if( dataLength )
        {
        aStream.WriteL( iRightsIssuer->Des() );        
        }
        
    // Write the ContentName                    
    dataLength = 0;
    if( iContentName )
        {
        dataLength = iContentName->Length();
        }
    aStream.WriteInt32L( dataLength );
            
    if( dataLength )
        {
        aStream.WriteL( iContentName->Des() );
        }

    // Write the Rights Issuer
    dataLength = 0;
    if( iAuthenticationSeed )
        {
        dataLength = iAuthenticationSeed->Length();
        }
    aStream.WriteInt32L( dataLength );
    
    if( dataLength )
        {
        aStream.WriteL( iAuthenticationSeed->Des() );        
        }
    
    };

// -----------------------------------------------------------------------------
// CDRMCommonData::InternalizeL
// -----------------------------------------------------------------------------
// 
void CDRMCommonData::InternalizeL( RReadStream& aStream )
    {
    TInt dataLength = 0;
    HBufC8* dataPart = 0;
    TPtr8 dataBuffer(NULL,0,0);
        
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
        
    // Read the Content Hash
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
        if( iContentHash )
            {
            delete iContentHash;
            iContentHash = NULL;
            }
        
        // assign the new content id
        iContentHash = dataPart;    
        }
    else
        {
        // If an old content identifier exists delete it 
        if( iContentHash )
            {
            delete iContentHash;
            iContentHash = NULL;
            }        
        }           
        
    // Read the rights issuer
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
        if( iRightsIssuer )
            {
            delete iRightsIssuer;
            iRightsIssuer = NULL;
            }
        
        // assign the new content id
        iRightsIssuer = dataPart;    
        }
    else
        {
        // If an old content identifier exists delete it 
        if( iRightsIssuer )
            {
            delete iRightsIssuer;
            iRightsIssuer = NULL;
            }        
        }

    // Read the rights issuer
    dataLength = aStream.ReadInt32L();
    
    if( dataLength )
        {
        HBufC16* dataPart2 = 0;
        TPtr16 dataBuffer2(NULL,0,0);
        
        // Reserve a new buffer:
        dataPart2 = HBufC::NewMaxLC( dataLength );
        
        // Set the read buffer:
        dataBuffer2.Set(const_cast<TUint16*>(dataPart2->Ptr()), 0, dataLength);
        
        // Read the data:
        aStream.ReadL( dataBuffer2 );
        
        // Pop the buffer 
        CleanupStack::Pop(); // dataPart
                
        // If an old content identifier exists delete it        
        if( iContentName )
            {
            delete iContentName;
            iContentName = NULL;
            }
        
        // assign the new content id
        iContentName = dataPart2;    
        }
    else
        {
        // If an old content identifier exists delete it 
        if( iContentName )
            {
            delete iContentName;
            iContentName = NULL;
            }        
        }           

    // Read the iAuthenticationSeed
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
        if( iAuthenticationSeed )
            {
            delete iAuthenticationSeed;
            iAuthenticationSeed = NULL;
            }
        
        // assign the new content id
        iAuthenticationSeed = dataPart;    
        }
    else
        {
        // If an old content identifier exists delete it 
        if( iAuthenticationSeed )
            {
            delete iAuthenticationSeed;
            iAuthenticationSeed = NULL;
            }        
        }
                   
    };   

// -----------------------------------------------------------------------------
// CDRMCommonData::Size
// -----------------------------------------------------------------------------
// 
TInt CDRMCommonData::Size() const
    {
    TInt size = 0;
    
    // Content identifier of the content
    size += sizeof(TInt32);
    
    if( iContentID )
        {
        size += iContentID->Size();
        }
        
    // Hash of the content DCF
    size += sizeof(TInt32);
    
    if( iContentHash )
        {
        size += iContentHash->Size();
        }
        
    // Issuer of the rights
    size += sizeof(TInt32);
    
    if( iRightsIssuer)
        {
        size += iRightsIssuer->Size();
        }
        
    // Content name
    size += sizeof(TInt32);
    
    if( iContentName )
        {
        size += iContentName->Size();
        }
    
    // Authentication seed    
    size += sizeof(TInt32);
    
    if( iAuthenticationSeed )
        {
        size += iAuthenticationSeed->Size();	
        }    
    return size;
    };    
 
// -----------------------------------------------------------------------------
// CDRMCommonData::ConstrutL
// Second phase constructor
// -----------------------------------------------------------------------------
// 
void CDRMCommonData::ConstructL( const TDesC8& aContentID,
                                 const TDesC8& aContentHash,
                                 const TDesC8& aRightsIssuer,
                                 const TDesC& aContentName,
                                 const TDesC8& aAuthenticationSeed )
    {
    iContentID = aContentID.AllocL();
    iContentHash = aContentHash.AllocL();
    iRightsIssuer = aRightsIssuer.AllocL();
    iContentName = aContentName.AllocL();
    iAuthenticationSeed = aAuthenticationSeed.AllocL();
    };                
            
// End of File
