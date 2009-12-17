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
* Description:  Datatype for Rights Constraint
*
*/



// INCLUDE FILES
#include <s32file.h>
#include <s32std.h>
#include <caf/caf.h>

#include "DRMContextDB.h"
#include "DRMRIContext.h"
#include "DRMDomainContext.h"


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
// CDRMContextDB::NewLC
// -----------------------------------------------------------------------------
//
CDRMContextDB* CDRMContextDB::NewLC( const TFileName& aRightsIssuerContextFile, 
                                     const TFileName& aDomainContextFile,
                                     RFs& aFileServer )
    {
    CDRMContextDB* self = new( ELeave ) CDRMContextDB( aFileServer );
    CleanupStack::PushL( self );
    self->ConstructL( aRightsIssuerContextFile, 
                      aDomainContextFile );

    return self;    
    };
    
// -----------------------------------------------------------------------------
// CDRMContextDB::NewL
// -----------------------------------------------------------------------------
//
CDRMContextDB* CDRMContextDB::NewL( const TFileName& aRightsIssuerContextFile, 
                                    const TFileName& aDomainContextFile,
                                    RFs& aFileServer )
    {
    CDRMContextDB* self = NewLC( aRightsIssuerContextFile, 
                                 aDomainContextFile,  
                                 aFileServer );
    CleanupStack::Pop();	
    
    return self;    
    };
                                 
// -----------------------------------------------------------------------------
// CDRMContextDB::OpenLC
// -----------------------------------------------------------------------------
//
CDRMContextDB* CDRMContextDB::OpenLC( const TFileName& aRightsIssuerContextFile, 
                                      const TFileName& aDomainContextFile,
                                      RFs& aFileServer )
    {
    CDRMContextDB* self = new( ELeave ) CDRMContextDB( aFileServer );
    CleanupStack::PushL( self );    
    self->ConstructL( aRightsIssuerContextFile, 
                      aDomainContextFile );
    
    return self;
    };    
    
// -----------------------------------------------------------------------------
// CDRMContextDB::OpenL
// -----------------------------------------------------------------------------
//
CDRMContextDB* CDRMContextDB::OpenL( const TFileName& aRightsIssuerContextFile, 
                                     const TFileName& aDomainContextFile,
                                     RFs& aFileServer )
    {
    CDRMContextDB* self = OpenLC( aRightsIssuerContextFile, 
                                  aDomainContextFile,  
                                  aFileServer );
    CleanupStack::Pop();
    
    return self;    
    };    
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
// 
CDRMContextDB::~CDRMContextDB()
    {
    // Permanent File Store
    if( iDomainStore )
        {
        delete iDomainStore;
        iDomainStore = NULL;
        }
        
    // Permanent File Store
    if( iRightsIssuerStore )
        {
        delete iRightsIssuerStore;
        iRightsIssuerStore = NULL;
        }
        
    // index array    
    iDomainArray.Close();
    
    // index array
    iRightsIssuerArray.Close();
    }; 
      
    
// -----------------------------------------------------------------------------
// CDRMContextDB::StoreDomainContextL
// -----------------------------------------------------------------------------
//
void CDRMContextDB::StoreDomainContextL( const CDRMDomainContext& aContext )
    {
    TStreamId rootId;
    RStoreWriteStream stream;
    RStoreWriteStream rootStream;
    CDRMContextDB::TContextIndex searchData;
    TInt index;
    
    // Copy the domain id to the 
    searchData.iContextID.Copy( aContext.DomainID() );
    
    // get the id from the array if it doesn't exist, error
    index = iDomainArray.Find( searchData, TContextIndex::Match );    
    
    if( index == KErrNotFound )
        {
        TStreamId sid;
        // Create a new stream to the store:
        sid = stream.CreateLC( *iDomainStore );    
        
        searchData.iStreamID = sid.Value();
        
        // Externalize the domain data
        aContext.ExternalizeL( stream );
        stream.CommitL();
        
        // add the id to the array
        iDomainArray.Append( searchData );
        
        // Get the stream id of the root
        rootId = iDomainStore->Root();  
        
        // Open the root stream
        rootStream.ReplaceLC( *iDomainStore, rootId );
        
        // Store the changed index
        StoreDomainIndexL( rootStream );    
        rootStream.CommitL(); 
        
        // Commit the store
        iDomainStore->CommitL();                      
        
        CleanupStack::PopAndDestroy();  // root stream
        CleanupStack::PopAndDestroy();  // stream       
        }
    else
        {
        // open an existing one
        TStreamId sid( iDomainArray[ index ].iStreamID );
        
        // Replace the existing stream
        stream.ReplaceLC( *iDomainStore, sid );
    
        // Externalize the permission data
        aContext.ExternalizeL( stream );
    
        // Required by the ReplaceLC
        stream.CommitL();

        // Commit the store    
        iDomainStore->CommitL();
    
        CleanupStack::PopAndDestroy(); // stream    
        }
        
    iDomainStore->CompactL();
    iDomainStore->CommitL();
    };    
    
// -----------------------------------------------------------------------------
// CDRMContextDB::FetchDomainContextL
// -----------------------------------------------------------------------------
//
void CDRMContextDB::FetchDomainContextL( CDRMDomainContext& aContext ) const
    {   
    RStoreReadStream stream;
    TUint32 index = 0;

    TContextIndex searchData;
    
    // Copy the domain id to the search data
    searchData.iContextID.Copy( aContext.DomainID() );
    
    // get the id from the array if it doesn't exist, error
    User::LeaveIfError( index = iDomainArray.Find( searchData, 
                                                   TContextIndex::Match ) );

    TStreamId sid( iDomainArray[ index ].iStreamID );
    // Open the root stream
    
    stream.OpenLC( *iDomainStore, sid );

    // Read the common id
    aContext.InternalizeL( stream );
    
    // Close the stream
    CleanupStack::PopAndDestroy();        
    };
    
// -----------------------------------------------------------------------------
// CDRMContextDB::FetchDomainContextL 
// For getting with the current id
// -----------------------------------------------------------------------------
//
void CDRMContextDB::FetchDomainContextL( CDRMDomainContext& aContext, 
                                         const TInt32& aStream ) const
    {
    RStoreReadStream stream;
    TStreamId sid( aStream );
       
    stream.OpenLC( *iDomainStore, sid );

    // Read the common id
    aContext.InternalizeL( stream );
    
    // Close the stream
    CleanupStack::PopAndDestroy();        
    };    
    
// -----------------------------------------------------------------------------
// CDRMContextDB::DeleteDomainContextL
// -----------------------------------------------------------------------------
//
void CDRMContextDB::DeleteDomainContextL( const TDesC8& aDomainID )
    {
    TStreamId rootId;
    TInt index = 0;
    RStoreWriteStream rootStream;

    TContextIndex searchData;
    
    // Copy the domain id to the search data
    // The last three digits presents the Domain generation
    searchData.iContextID.Copy( aDomainID.Mid(0, aDomainID.Length() - 3 ) );
    
    // get the id from the array if it doesn't exist, error
    index = iDomainArray.Find( searchData, TContextIndex::Match ); 
    
    User::LeaveIfError( index = iDomainArray.Find( searchData, 
                                                   TContextIndex::Match ) );

    TStreamId sid( iDomainArray[ index ].iStreamID );

    // Delete the stream from the store
    iDomainStore->DeleteL( sid );
            
    iDomainArray.Remove( index );       
    
    
    // Get the stream id of the root
    rootId = iDomainStore->Root();
    
    // Open the root stream
    rootStream.ReplaceLC( *iDomainStore, rootId );
        
    // Store the changed index
    StoreDomainIndexL( rootStream );    
    rootStream.CommitL();

    // Commit the store
    iDomainStore->CommitL();
    
    CleanupStack::PopAndDestroy();  // root stream
    
    // Compact and commit the changes
    iDomainStore->CompactL();
    iDomainStore->CommitL();        
    };
 
// -----------------------------------------------------------------------------
// CDRMContextDB::StoreRightsIssuerContextL
// -----------------------------------------------------------------------------
//
void CDRMContextDB::StoreRightsIssuerContextL( const CDRMRIContext& aContext )
    {
    TStreamId rootId;
    RStoreWriteStream stream;
    RStoreWriteStream rootStream;
    TContextIndex searchData;
    TInt index;
    
    // Copy the domain id to the 
    searchData.iContextID.Copy( aContext.RIID() );
    
    // get the id from the array if it doesn't exist, error
    index = iRightsIssuerArray.Find( searchData, TContextIndex::Match );    
    
    if( index == KErrNotFound )
        {
        TStreamId sid;
        // Create a new stream to the store:
        sid = stream.CreateLC( *iRightsIssuerStore );    
        
        searchData.iStreamID = sid.Value();
        
        // Externalize the domain data
        aContext.ExternalizeL( stream );
        stream.CommitL();
        
        // add the id to the array
        iRightsIssuerArray.Append( searchData );
        
        // Get the stream id of the root
        rootId = iRightsIssuerStore->Root();  
        
        // Open the root stream
        rootStream.ReplaceLC( *iRightsIssuerStore, rootId );
        
        // Store the changed index
        StoreRightsIssuerIndexL( rootStream );    
        rootStream.CommitL(); 
        
        // Commit the store
        iRightsIssuerStore->CommitL();                      
        
        CleanupStack::PopAndDestroy();  // root stream
        CleanupStack::PopAndDestroy();  // stream       
        }
    else
        {
        // open an existing one
        TStreamId sid( iRightsIssuerArray[ index ].iStreamID );
        
        // Replace the existing stream
        stream.ReplaceLC( *iRightsIssuerStore, sid );
    
        // Externalize the permission data
        aContext.ExternalizeL( stream );
    
        // Required by the ReplaceLC
        stream.CommitL();

        // Commit the store    
        iRightsIssuerStore->CommitL();
    
        CleanupStack::PopAndDestroy(); // stream    
        }
        
    iRightsIssuerStore->CompactL();
    iRightsIssuerStore->CommitL();
    };    
    
// -----------------------------------------------------------------------------
// CDRMContextDB::FetchRightsIssuerContextL
// -----------------------------------------------------------------------------
//
void CDRMContextDB::FetchRightsIssuerContextL( CDRMRIContext& aContext ) const
    {   
    RStoreReadStream stream;
    TUint32 index = 0;

    TContextIndex searchData;
    
    // Copy the domain id to the search data
    searchData.iContextID.Copy( aContext.RIID() );
    
    // get the id from the array if it doesn't exist, error
    User::LeaveIfError( index = iRightsIssuerArray.Find( searchData, 
                                                   TContextIndex::Match ) );

    TStreamId sid( iRightsIssuerArray[ index ].iStreamID );
    // Open the root stream
    
    stream.OpenLC( *iRightsIssuerStore, sid );

    // Read the common id
    aContext.InternalizeL( stream );
    
    // Close the stream
    CleanupStack::PopAndDestroy();        
    };

// -----------------------------------------------------------------------------
// CDRMContextDB::FetchRightsIssuerContextL 
// For getting with the current id
// -----------------------------------------------------------------------------
//
void CDRMContextDB::FetchRightsIssuerContextL( CDRMRIContext& aContext, 
                                               const TInt32& aStream ) const
    {
    RStoreReadStream stream;
    TStreamId sid( aStream );
       
    stream.OpenLC( *iRightsIssuerStore, sid );

    // Read the common id
    aContext.InternalizeL( stream );
    
    // Close the stream
    CleanupStack::PopAndDestroy();        
    };


// -----------------------------------------------------------------------------
// CDRMContextDB::FetchAllRightsIssuerContextsL
// For getting with the current id
// -----------------------------------------------------------------------------
//
void CDRMContextDB::FetchAllRightsIssuerContextsL( RPointerArray<CDRMRIContext>& aContexts) const
    {
    CDRMRIContext* context = NULL;

    // No RI contexts to retrieve
    if ( iRightsIssuerArray.Count() == 0) 
        {
        return;
        }
    
    for( TInt count = 0; count < iRightsIssuerArray.Count(); count++ )
        {
        context = CDRMRIContext::NewLC();
        FetchRightsIssuerContextL( *context, iRightsIssuerArray[ count ].iStreamID );
        aContexts.AppendL( context );
        CleanupStack::Pop( context );    
        }    
    }

// -----------------------------------------------------------------------------
// CDRMContextDB::UpdateRightsIssuerContextL
// -----------------------------------------------------------------------------
//
void CDRMContextDB::UpdateRightsIssuerContextL( const CDRMRIContext& aContext )
    {
    TStreamId rootId;
    TInt index = 0;
    RStoreWriteStream stream;    
    CDRMRIContext* context = NULL;
    
    TContextIndex searchData;
    
    // Copy the domain id to the search data
    searchData.iContextID.Copy( aContext.RIID() );
    
    // get the id from the array if it doesn't exist, error
    User::LeaveIfError( index = iRightsIssuerArray.Find( searchData, 
                                                   TContextIndex::Match ) );

    TStreamId sid( iRightsIssuerArray[ index ].iStreamID );
    
    // Read the old context:    
    context = CDRMRIContext::NewLC();
    FetchRightsIssuerContextL( *context, iRightsIssuerArray[ index ].iStreamID );
    
    // update only the metering status:    
    context->SetMeteringStatus( aContext.IsMeteringAllowed() );
    
    // Replace the existing stream
    stream.ReplaceLC( *iRightsIssuerStore, sid );
    
    // Externalize the permission data
    context->ExternalizeL( stream );
     
    // Required by the ReplaceLC
    stream.CommitL();   
    
    // Commit the store    
    iRightsIssuerStore->CommitL();
    
    CleanupStack::PopAndDestroy(); // stream
    CleanupStack::PopAndDestroy(context);        
    
    
    // Compact and commit the changes
    iRightsIssuerStore->CompactL();
    iRightsIssuerStore->CommitL();        
    };


    
// -----------------------------------------------------------------------------
// CDRMContextDB::DeleteRightsIssuerContextL
// -----------------------------------------------------------------------------
//
void CDRMContextDB::DeleteRightsIssuerContextL( const TDesC8& aStream )
    {
    TStreamId rootId;
    TInt index = 0;
    RStoreWriteStream rootStream;

    TContextIndex searchData;
    
    // Copy the domain id to the search data
    searchData.iContextID.Copy( aStream );
    
    // get the id from the array if it doesn't exist, error
    User::LeaveIfError( index = iRightsIssuerArray.Find( searchData, 
                                                   TContextIndex::Match ) );

    TStreamId sid( iRightsIssuerArray[ index ].iStreamID );

    // Delete the stream from the store
    iRightsIssuerStore->DeleteL( sid );
            
    iRightsIssuerArray.Remove( index );       
    
    
    // Get the stream id of the root
    rootId = iRightsIssuerStore->Root();
    
    // Open the root stream
    rootStream.ReplaceLC( *iRightsIssuerStore, rootId );
        
    // Store the changed index
    StoreDomainIndexL( rootStream );    
    rootStream.CommitL();

    // Commit the store
    iRightsIssuerStore->CommitL();
    
    CleanupStack::PopAndDestroy();  // root stream
    
    // Compact and commit the changes
    iRightsIssuerStore->CompactL();
    iRightsIssuerStore->CommitL();        
    }; 
                           
// -----------------------------------------------------------------------------
// CDRMContextDB::WhiteListURLExistsL
// -----------------------------------------------------------------------------
//
TBool CDRMContextDB::WhiteListURLExistsL( const TDesC8& aRightsIssuerURL )
    {
    CDRMRIContext* context = NULL;
    TBool retVal = EFalse;

    if ( iRightsIssuerArray.Count() == 0) 
        {
        return retVal;
        }
    
    context = CDRMRIContext::NewLC();
    
    for( TInt count = 0; count < iRightsIssuerArray.Count(); count++ )
        {
        FetchRightsIssuerContextL( *context, iRightsIssuerArray[ count ].iStreamID );
        if( context->WhiteListURLExistsL( aRightsIssuerURL ) )
            {
            retVal = ETrue;
            count = iRightsIssuerArray.Count();           
            }
        }
    CleanupStack::PopAndDestroy(); // context       
    return retVal;    
    };    

// -----------------------------------------------------------------------------
// CDRMContextDB::DeleteExpiredDomainContextsL
// -----------------------------------------------------------------------------
//
void CDRMContextDB::DeleteExpiredDomainContextsL( const TTime& aTime )
    {  
    CDRMDomainContext* context = NULL;    
    TStreamId rootId;
    RStoreWriteStream rootStream;

    // Check if there is anything to go through
    if ( iDomainArray.Count() == 0) 
        {
        return;
        }
      
    context = CDRMDomainContext::NewLC();
        
    // Delete all permissions from the store which have expired    
    for( TInt i = 0; i < iDomainArray.Count();)
        {
        FetchDomainContextL( *context, iDomainArray[i].iStreamID );
        
        if( context->ExpiryTime() < aTime && context->ExpiryTime() != Time::NullTTime() )
            {
            iDomainStore->DeleteL( TStreamId( iDomainArray[i].iStreamID ) );
            iDomainArray.Remove(i);
            }
        else 
            {
            i++; // increment the counter for the next run       
            }   
        }
        
    CleanupStack::PopAndDestroy();  // context
    
    // Get the stream id of the root
    rootId = iDomainStore->Root();
    
    // Open the root stream
    rootStream.ReplaceLC( *iDomainStore, rootId );
        
    // Store the changed index
    StoreDomainIndexL( rootStream );    
    rootStream.CommitL();

    // Commit the store
    iDomainStore->CommitL();
    
    CleanupStack::PopAndDestroy();  // root stream
    
    // Compact and Commit the store
    iDomainStore->CompactL();
    iDomainStore->CommitL();          
    };

// -----------------------------------------------------------------------------
// CDRMContextDB::DeleteExpiredRightsIssuerContextsL
// -----------------------------------------------------------------------------
//
void CDRMContextDB::DeleteExpiredRightsIssuerContextsL( const TTime& aTime )
    {
    CDRMRIContext* context = NULL;    
    TStreamId rootId;
    RStoreWriteStream rootStream;

    // Check if there is anything to go through
    if ( iRightsIssuerArray.Count() == 0) 
        {
        return;
        }
        
    context = CDRMRIContext::NewLC(); 
           
    // Delete all permissions from the store which have expired    
    for( TInt i = 0; i < iRightsIssuerArray.Count();)
        {
        FetchRightsIssuerContextL( *context, iRightsIssuerArray[i].iStreamID );
        
        if( context->ExpiryTime() < aTime && context->ExpiryTime() != Time::NullTTime() )
            {
            iRightsIssuerStore->DeleteL( TStreamId( iRightsIssuerArray[i].iStreamID ) );
            iRightsIssuerArray.Remove(i);
            }
        else
            {
            i++; // increment the counter for the next run                 
            }       
        }
        
    CleanupStack::PopAndDestroy();  // context 
    
    // Get the stream id of the root
    rootId = iRightsIssuerStore->Root();
    
    // Open the root stream
    rootStream.ReplaceLC( *iRightsIssuerStore, rootId );
        
    // Store the changed index
    StoreRightsIssuerIndexL( rootStream );    
    rootStream.CommitL();

    // Commit the store
    iRightsIssuerStore->CommitL();
    
    CleanupStack::PopAndDestroy();  // root stream
    
    // Compact and Commit the store
    iRightsIssuerStore->CompactL();
    iRightsIssuerStore->CommitL();           
    };

// -----------------------------------------------------------------------------
// CDRMContextDB::CreateAndInitializeRIStoreL
// -----------------------------------------------------------------------------
//
void CDRMContextDB::CreateAndInitializeRIStoreL( const TFileName& aRightsIssuerStore )
    {
    TStreamId rootId;  
    RStoreWriteStream rootStream;
        
    // Create and initialize the permanent file store
    iRightsIssuerStore = CPermanentFileStore::ReplaceL( iFileServer, 
                                                        aRightsIssuerStore,
                                                        EFileRead|EFileWrite );                                          
    iRightsIssuerStore->SetTypeL( KPermanentFileStoreLayoutUid );
    iRightsIssuerStore->CommitL(); 
    
    // Create the root stream:                                                   
    rootId = rootStream.CreateLC( *iRightsIssuerStore );
         
    // Create the index
    // Commit the previous changes 
    StoreRightsIssuerIndexL( rootStream );
    rootStream.CommitL();
    
    iRightsIssuerStore->SetRootL( rootId );
    iRightsIssuerStore->CommitL();  
    
    // Commit the changes to the store as well
    CleanupStack::PopAndDestroy(); // rootStream
    
    iRightsIssuerStore->CompactL();
    iRightsIssuerStore->CommitL();
    };


// -----------------------------------------------------------------------------
// CDRMContextDB::CreateAndInitializeDomainStoreL
// -----------------------------------------------------------------------------
//
void CDRMContextDB::CreateAndInitializeDomainStoreL( const TFileName& aDomainStore )
    {
    TStreamId rootId;  
    RStoreWriteStream rootStream;
        
    // Create and initialize the permanent file store
    iDomainStore = CPermanentFileStore::ReplaceL( iFileServer, 
                                                        aDomainStore,
                                                        EFileRead|EFileWrite );                                          
    iDomainStore->SetTypeL( KPermanentFileStoreLayoutUid );
    iDomainStore->CommitL(); 
    
    // Create the root stream:                                                   
    rootId = rootStream.CreateLC( *iDomainStore );
                                               
    // Create the index
    // Commit the previous changes 
    StoreDomainIndexL( rootStream );
    rootStream.CommitL();
    
    iDomainStore->SetRootL( rootId );   
    iDomainStore->CommitL();    
        
    // Commit the changes to the store as well
    CleanupStack::PopAndDestroy(); // rootStream 
    
    iDomainStore->CompactL();
    iDomainStore->CommitL();       
    };

// -----------------------------------------------------------------------------
// CDRMContextDB::StoreRightsIssuerIndexL
// -----------------------------------------------------------------------------
//
void CDRMContextDB::StoreRightsIssuerIndexL( RWriteStream& aStream ) const
    {
    // Write the array size and possibly the array to the stream
    aStream.WriteInt32L( iRightsIssuerArray.Count() );
             
    for( TInt count = 0; count < iRightsIssuerArray.Count(); count++ )
        {
        aStream << iRightsIssuerArray[ count ].iContextID;
        aStream.WriteUint32L( iRightsIssuerArray[ count ].iStreamID );
        }
    };

// -----------------------------------------------------------------------------
// CDRMContextDB::StoreDomainIndexL
// -----------------------------------------------------------------------------
//
void CDRMContextDB::StoreDomainIndexL( RWriteStream& aStream ) const
    {
    // Write the array size and possibly the array to the stream
    aStream.WriteInt32L( iDomainArray.Count() );
    
    for( TInt count = 0; count < iDomainArray.Count(); count++ )
        {
        aStream << iDomainArray[ count ].iContextID;
        aStream.WriteUint32L( iDomainArray[ count ].iStreamID );
        }
    };
    
// -----------------------------------------------------------------------------
// CDRMContextDB::FetchRightsIssuerIndexL
// -----------------------------------------------------------------------------
//
void CDRMContextDB::FetchRightsIssuerIndexL()
    {
    TStreamId rootId;
    RStoreReadStream stream;
    TInt count = 0;
    TContextIndex index;
 
    // Get the stream id of the root
    rootId = iRightsIssuerStore->Root();
    
    // Open the root stream
    stream.OpenLC( *iRightsIssuerStore, rootId );
    
    // read the count of the array:
    iRightsIssuerArray.Reset();
    
    count = stream.ReadInt32L();
    
    for( ; count > 0; count-- )
        {
        stream >> index.iContextID;
        index.iStreamID = stream.ReadInt32L();
        iRightsIssuerArray.Append( index );
        }
     
    // Close the stream
    CleanupStack::PopAndDestroy();    
    };
    
// -----------------------------------------------------------------------------
// CDRMContextDB::FetchDomainIndexL
// -----------------------------------------------------------------------------
//
void CDRMContextDB::FetchDomainIndexL()
    {
    TStreamId rootId;
    RStoreReadStream stream;
    TInt count = 0;
    TContextIndex index;
 
    // Get the stream id of the root
    rootId = iDomainStore->Root();
    
    // Open the root stream
    stream.OpenLC( *iDomainStore, rootId );
    
    // read the count of the array:
    iDomainArray.Reset();
    
    count = stream.ReadInt32L();
    
    for( ; count > 0; count-- )
        {
        stream >> index.iContextID;
        index.iStreamID = stream.ReadInt32L();
        iDomainArray.Append( index );
        }
     
    // Close the stream
    CleanupStack::PopAndDestroy();    
    };

// -----------------------------------------------------------------------------
// CDRMContextDB::DeleteAllL
// -----------------------------------------------------------------------------
//
void CDRMContextDB::DeleteAllL( const TFileName& aRightsIssuerContextFile, 
                                const TFileName& aDomainContextFile )
    {
    delete iRightsIssuerStore;
    iRightsIssuerStore = NULL;
    iRightsIssuerArray.Reset();
    CreateAndInitializeRIStoreL( aRightsIssuerContextFile );
 
    delete iDomainStore;
    iDomainStore = NULL;
    iDomainArray.Reset();
    CreateAndInitializeDomainStoreL( aDomainContextFile );
    }

// -----------------------------------------------------------------------------
// Default Constructor - First phase.
// Can be used by itself to generate an empty object
// -----------------------------------------------------------------------------
//
CDRMContextDB::CDRMContextDB( RFs& aFileServer ) :
    iFileServer( aFileServer )
    {
    };

// -----------------------------------------------------------------------------
// 2nd phase Constructor
// -----------------------------------------------------------------------------
//
void CDRMContextDB::ConstructL( const TFileName& aRightsIssuerContextFile, 
                                const TFileName& aDomainContextFile )
    {
    TInt error = KErrNone;
    
    // Open the file store
    TRAP( error, iRightsIssuerStore = CPermanentFileStore::OpenL( iFileServer, 
                                                      aRightsIssuerContextFile, 
                                                      EFileRead|EFileWrite ) );
    if( error == KErrNotFound )
        {
        CreateAndInitializeRIStoreL( aRightsIssuerContextFile );
        }
    else 
        {   
        User::LeaveIfError( error );
        
        // Get the index
        FetchRightsIssuerIndexL();       
        }    
    
    
    // Open the file store
    TRAP( error, iDomainStore = CPermanentFileStore::OpenL( iFileServer, 
                                                      aDomainContextFile, 
                                                      EFileRead|EFileWrite ) );
    if( error == KErrNotFound ) 
        {
        CreateAndInitializeDomainStoreL( aDomainContextFile );
        }
    else 
        {   
        User::LeaveIfError( error );
        
        // Get the index
        FetchDomainIndexL();       
        
        }
    };
            
// End of File    
