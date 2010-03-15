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

#include "DRMPointerArray.h"
#include "DRMCommonData.h"
#include "DrmPermission.h"
#include "DRMRightsData.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KEncryptionKeySize = 16;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class TPermData
    {
    public: // Constructor
    TPermData( HBufC8* aParentId, const TInt aIndex, TBool aIsValid ) :
        iParentId( aParentId ),
        iIndex( aIndex ),
        iIsValid( aIsValid ) { };

    public: // Member variables
    HBufC8* iParentId;
    TInt iIndex;
    TBool iIsValid;
    };


// ============================= LOCAL FUNCTIONS ===============================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMRightsData::NewLC
// -----------------------------------------------------------------------------
//
CDRMRightsData* CDRMRightsData::NewLC( CDRMCommonData* aData,
                                       const TDesC8& aKey,
                                       const TFileName& aRightsFile,
                                       RFs& aFileServer )
    {
    CDRMRightsData* self = new( ELeave ) CDRMRightsData( aData, aFileServer );
    CleanupStack::PushL( self );
    self->ConstructL( aRightsFile, aKey );

    return self;
    };

// -----------------------------------------------------------------------------
// CDRMRightsData::NewL
// -----------------------------------------------------------------------------
//
CDRMRightsData* CDRMRightsData::NewL( CDRMCommonData* aData,
                                      const TDesC8& aKey,
                                      const TFileName& aRightsFile,
                                      RFs& aFileServer )
    {
    CDRMRightsData* self = NewLC( aData, aKey, aRightsFile, aFileServer );
    CleanupStack::Pop();

    return self;
    };

// -----------------------------------------------------------------------------
// CDRMRightsData::OpenLC
// -----------------------------------------------------------------------------
//
CDRMRightsData* CDRMRightsData::OpenLC( const TFileName& aRightsFile,
                                        RFs& aFileServer )
    {
    CDRMCommonData* common = CDRMCommonData::NewL();

    CDRMRightsData* self = new( ELeave ) CDRMRightsData( common, aFileServer );
    CleanupStack::PushL( self );
    self->ConstructL( aRightsFile );

    return self;
    };

// -----------------------------------------------------------------------------
// CDRMRightsData::OpenL
// -----------------------------------------------------------------------------
//
CDRMRightsData* CDRMRightsData::OpenL( const TFileName& aRightsFile,
                                       RFs& aFileServer )
    {
    CDRMRightsData* self = OpenLC( aRightsFile, aFileServer );
    CleanupStack::Pop();

    return self;
    };

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CDRMRightsData::~CDRMRightsData()
    {
    // Permanent File Store
    if( iStore )
        {;
        delete iStore;
        iStore = NULL;
        }

    // index array
    iArray.Close();

    // Common Data
    if( iData )
        {
        delete iData;
        iData = NULL;
        }
    };


// -----------------------------------------------------------------------------
// CDRMRightsData::GetCommonDataL
// -----------------------------------------------------------------------------
//
const CDRMCommonData* CDRMRightsData::GetCommonDataL() const
    {
    return iData;
    };

// -----------------------------------------------------------------------------
// CDRMRightsData::UpdateCommonDataL
// -----------------------------------------------------------------------------
//
void CDRMRightsData::UpdateCommonDataL( CDRMCommonData* aData )
    {
    RStoreWriteStream stream;

    // If not data is given, that is an error
    if( !aData )
        {
        User::Leave( KErrArgument );
        }

    // Open the stream
    stream.ReplaceLC( *iStore, iCommonId );

    // Delete the old data if it's around
    // if the old data has been modified, write it as such
    if( iData != aData )
        {
        if( iData )
            {
            delete iData;
            iData = NULL;
            }
        iData = aData;
        }

    // Write the data to the stream
    iData->ExternalizeL( stream );

    // commit the stream
    stream.CommitL();

    // commit the store
    iStore->CommitL();

    // pop the stream
    CleanupStack::PopAndDestroy();
    };

// -----------------------------------------------------------------------------
// CDRMRightsData::StoreNewPermissionL
// -----------------------------------------------------------------------------
//
void CDRMRightsData::StoreNewPermissionL( CDRMPermission& aPermission,
                                          TDRMUniqueID& aStream )
    {
    TStreamId sid;
    TStreamId rootId;
    RStoreWriteStream stream;
    RStoreWriteStream rootStream;

    // Create a new stream to the store:
    sid = stream.CreateLC( *iStore );

    aPermission.iUniqueID = sid.Value();
    aStream = sid.Value();

    // Externalize the permission data
    aPermission.ExternalizeL( stream );
    stream.CommitL();

    // add the id to the array
    iArray.InsertInUnsignedKeyOrder( aStream );

    // Get the stream id of the root
    rootId = iStore->Root();

    // Open the root stream
    rootStream.ReplaceLC( *iStore, rootId );

    // Store the changed index
    StoreIndexL( rootStream );
    rootStream.CommitL();

    // Commit the store
    iStore->CommitL();

    CleanupStack::PopAndDestroy();  // root stream
    CleanupStack::PopAndDestroy();  // stream

    iStore->CompactL();
    iStore->CommitL();
    };

// -----------------------------------------------------------------------------
// CDRMRightsData::UpdatePermissionL
// -----------------------------------------------------------------------------
//
void CDRMRightsData::UpdatePermissionL( const CDRMPermission& aPermission )
    {
    TStreamId sid( aPermission.iUniqueID );
    RStoreWriteStream stream;
    TInt index = 0;

    // get the id from the array if it doesn't exist, error
    index = iArray.FindInUnsignedKeyOrder( aPermission.iUniqueID );

    if( index == KErrNotFound )
        {
        User::Leave( KErrCANoPermission );
        }

    // Replace the existing stream
    stream.ReplaceLC( *iStore, sid );

    // Externalize the permission data
    aPermission.ExternalizeL( stream );

    // Required by the ReplaceLC
    stream.CommitL();

    // Commit the store
    iStore->CommitL();

    CleanupStack::PopAndDestroy();
    };

// -----------------------------------------------------------------------------
// CDRMRightsData::FetchPermissionL
// -----------------------------------------------------------------------------
//
void CDRMRightsData::FetchPermissionL( CDRMPermission& aPermission,
                                       const TDRMUniqueID& aStream ) const
    {
    TStreamId sid( aStream );
    RStoreReadStream stream;
    TInt index = 0;

    // get the id from the array if it doesn't exist, error
    index = iArray.FindInUnsignedKeyOrder( aStream );

    if( index == KErrNotFound )
        {
        User::Leave( KErrCANoPermission );
        }

    // Open the root stream
    stream.OpenLC( *iStore, sid );

    // Read the common id
    aPermission.InternalizeL( stream );

    // Close the stream
    CleanupStack::PopAndDestroy();
    };

// -----------------------------------------------------------------------------
// CDRMRightsData::DeletePermissionL
// -----------------------------------------------------------------------------
//
void CDRMRightsData::DeletePermissionL( const TDRMUniqueID& aStream )
    {
    TStreamId sid( aStream );
    TStreamId rootId;
    TInt index = 0;
    RStoreWriteStream rootStream;

    // get the id from the array if it doesn't exist, error
    index = iArray.FindInUnsignedKeyOrder( aStream );

    if( index == KErrNotFound )
        {
        User::Leave( KErrCANoPermission );
        }

    // Delete the stream from the store
    iStore->DeleteL( sid );

    iArray.Remove( index );


    // Get the stream id of the root
    rootId = iStore->Root();

    // Open the root stream
    rootStream.ReplaceLC( *iStore, rootId );

    // Store the changed index
    StoreIndexL( rootStream );
    rootStream.CommitL();

    // Commit the store
    iStore->CommitL();

    CleanupStack::PopAndDestroy();  // root stream

    // Compact and commit the changes
    iStore->CompactL();
    iStore->CommitL();
    };

// -----------------------------------------------------------------------------
// CDRMRightsData::FetchAllPermissionsL
// -----------------------------------------------------------------------------
//
void CDRMRightsData::FetchAllPermissionsL( RPointerArray<CDRMPermission>&
                                           aPointerArray)
    {
    CDRMPermission* perm = NULL;

    if ( iArray.Count() == 0)
        {
        User::Leave( KErrCANoPermission );
        }

    for( TInt count = 0; count < iArray.Count(); count++ )
        {
        perm = CDRMPermission::NewLC();
        FetchPermissionL( *perm, iArray[ count ] );
        aPointerArray.AppendL( perm );
        CleanupStack::Pop();
        }
    };

// -----------------------------------------------------------------------------
// CDRMRightsData::DeleteAllPermissionsL
// -----------------------------------------------------------------------------
//
void CDRMRightsData::DeleteAllPermissionsL()
    {
    TStreamId rootId;
    RStoreWriteStream rootStream;

    // Delete all permissions from the store
    for( TInt i = 0; i < iArray.Count(); i++ )
        {
        TStreamId sid( iArray[i] );
        iStore->DeleteL( sid );
        }

    // Reset the array
    iArray.Reset();

    // Get the stream id of the root
    rootId = iStore->Root();

    // Open the root stream
    rootStream.ReplaceLC( *iStore, rootId );

    // Store the changed index
    StoreIndexL( rootStream );
    rootStream.CommitL();

    // Commit the store
    iStore->CommitL();

    CleanupStack::PopAndDestroy();  // root stream

    // Compact and Commit the store
    iStore->CompactL();
    iStore->CommitL();
    };

// -----------------------------------------------------------------------------
// CDRMRightsData::DeleteExpiredPermissionsL
// Deletes all rights objects which have been expired
// if the given time aTime is Time::NullTTime() then
// deletes only non time based rights
// -----------------------------------------------------------------------------
//
TInt CDRMRightsData::DeleteExpiredPermissionsL( const TTime& aTime, TBool& aParents )
    {
    CDRMPermission* permission = NULL;
    RStoreReadStream stream;
    TStreamId rootId;
    RStoreWriteStream rootStream;
    TBool remove = EFalse;
    TInt k = 0;
    TBool performDelete = ETrue;

    // A list for the permissions:
    CDRMPointerArray<CDRMPermission>* permList = CDRMPointerArray<CDRMPermission>::NewLC();
    permList->SetAutoCleanup( ETrue );
    CDRMPointerArray<TPermData>* permDataList = CDRMPointerArray<TPermData>::NewLC();
    permDataList->SetAutoCleanup( ETrue );

    // Fill the array:
    for( TInt j = 0  ; j < iArray.Count(); j++ )
        {
        TPermData* perm = NULL;
        // Create the sid:
        TStreamId psid( iArray[j] );

        // Open the correct stream
        stream.OpenLC( *iStore, psid );

        permission = CDRMPermission::NewLC();

        // Read the permission
        permission->InternalizeL( stream );

        permList->AppendL( permission );
        CleanupStack::Pop(); // permission

        // if the permission has a parent check if we found now or have found an
        // expired one or a valid one,
        // store info if required
        if( permission->iParentUID )
            {
            if( !aParents )
                {
                aParents = ETrue;
                }

            for( k = 0; k < permDataList->Count(); k++ )
                {
                if( !(*permDataList)[k]->iParentId->Compare( *permission->iParentUID ) )
                    {
                    if( !(*permDataList)[k]->iIsValid && !permission->Expired( aTime ) )
                        {
                        (*permDataList)[k]->iParentId = permission->iParentUID;
                        (*permDataList)[k]->iIndex = j;
                        (*permDataList)[k]->iIsValid = ETrue;
                        }
                    break;
                    }
                }

            // if it's not found, add it:
            if( k == permDataList->Count() )
                {
                if( !permission->Expired( aTime ) )
                    {
                    perm = new(ELeave) TPermData( permission->iParentUID, j, ETrue);
                    }
                else
                    {
                    perm = new(ELeave) TPermData( permission->iParentUID, j, EFalse);
                    }
                CleanupStack::PushL( perm );
                permDataList->AppendL( perm );
                CleanupStack::Pop(); // perm
                }
            }

        // Close the stream
        CleanupStack::PopAndDestroy();  // stream
        }


    // Delete all expired permissions from the store
    for( TInt i = iArray.Count() - 1 ; i >= 0; i-- )
        {
        permission = (*permList)[i];

        // Check if the permission is expired
        // if it is, check if it has a parent and if it can be deleted
        if( permission->Expired( aTime ) )
            {
            // if it has a parent go through the list and see if this can be deleted
            // or not
            if( permission->iParentUID )
                {
                performDelete = EFalse;
                for( k = 0; k < permDataList->Count(); k++ )
                    {
                    TPermData* aData = (*permDataList)[k];

                    // since it's set up like this a pointer comparison is enough:
                    if( !(*permDataList)[k]->iParentId->Compare( *permission->iParentUID  ) )
                        {
                        if( i != (*permDataList)[k]->iIndex )
                            {
                            performDelete = ETrue;
                            }
                        }
                    }
                }

            if( performDelete )
                {
                // Create the sid:
                TStreamId sid( iArray[i] );

                iStore->Delete( sid );

                // remove from the array
                iArray.Remove( i );

                if( !remove )
                    {
                    remove = ETrue;
                    }
                }
            }
        performDelete = ETrue;
        }

    CleanupStack::PopAndDestroy( 2 ); // permDataList, permList

    // Write the data into the the file
    // if some of the rights have been removed
    if( remove )
        {
        // Get the stream id of the root
        rootId = iStore->Root();

        // Open the root stream
        rootStream.ReplaceLC( *iStore, rootId );

        // Store the changed index
        StoreIndexL( rootStream );
        rootStream.CommitL();

        // Commit the store
        iStore->CommitL();

        CleanupStack::PopAndDestroy();  // root stream

        // Compact and Commit the store
        iStore->CompactL();
        iStore->CommitL();
        }

    return iArray.Count();
    }


// -----------------------------------------------------------------------------
// CDRMRightsData::StoreKeyL
// -----------------------------------------------------------------------------
//
void CDRMRightsData::StoreKeyL()
    {
    RStoreWriteStream stream;
    TUint32 size = 0;

    stream.OpenLC( *iStore, iKeyId );

    if( iKeyExists )
        {
        size = KEncryptionKeySize;
        }

    stream.WriteUint32L( size );

    if( size )
        {
        stream.WriteL( iKey, KEncryptionKeySize );
        }

    stream.CommitL();
    iStore->CommitL();

    CleanupStack::PopAndDestroy();
    };

// -----------------------------------------------------------------------------
// CDRMRightsData::GetKeyL
// -----------------------------------------------------------------------------
//
HBufC8* CDRMRightsData::GetKeyL() const
    {
    HBufC8* key = NULL;

    if( iKeyExists )
        {
        key = HBufC8::NewL( KEncryptionKeySize );
        *key = iKey;
        }

    return key;
    }

// -----------------------------------------------------------------------------
// CDRMRightsData::FetchKeyL
// -----------------------------------------------------------------------------
//
void CDRMRightsData::FetchKeyL()
    {
    RStoreReadStream stream;
    TUint32 size = 0;

    // Open the root stream
    stream.OpenLC( *iStore, iKeyId );

    size = stream.ReadUint32L();

    if( !size )
        {
        iKeyExists = EFalse;
        }
    else
        {
        stream.ReadL( iKey, KEncryptionKeySize );
        iKeyExists = ETrue;
        }

    CleanupStack::PopAndDestroy();
    };

// -----------------------------------------------------------------------------
// CDRMRightsData::CreateAndInitializeStoreL
// -----------------------------------------------------------------------------
//
void CDRMRightsData::CreateAndInitializeStoreL( const TFileName& aRightsStore )
    {
    TStreamId rootId;
    RStoreWriteStream stream;
    RStoreWriteStream stream2;
    RStoreWriteStream rootStream;
    TUint32 size = 0;

    // Create and initialize the permanent file store
    iStore = CPermanentFileStore::ReplaceL( iFileServer, aRightsStore,
                                            EFileRead|EFileWrite );
    iStore->SetTypeL( KPermanentFileStoreLayoutUid );
    iStore->CommitL();

    // Create the root stream:
    rootId = rootStream.CreateLC( *iStore );
    rootStream.CommitL();
    iStore->SetRootL( rootId );
    iStore->CommitL();

    // Create the common data
    iCommonId = stream.CreateLC( *iStore );
    iData->ExternalizeL( stream );
    stream.CommitL();
    iStore->CommitL();

    CleanupStack::PopAndDestroy(); // stream

    // Create the key
    iKeyId = stream2.CreateLC( *iStore );

    // if the key exists set the key size accordingly
    if( iKeyExists )
        {
        size = KEncryptionKeySize;
        }
    stream2.WriteUint32L( size );

    if( size )
        {
        stream2.WriteL( iKey, KEncryptionKeySize );
        }
    stream2.CommitL();
    iStore->CommitL();

    CleanupStack::PopAndDestroy(); // stream2
    CleanupStack::PopAndDestroy(); // rootStream

    // Create the index now that we have all the data
    rootId = iStore->Root();

    rootStream.ReplaceLC(*iStore, rootId);
    StoreIndexL( rootStream );
    rootStream.CommitL();
    iStore->CommitL();

    // Commit the changes to the store as well
    CleanupStack::PopAndDestroy(); // rootStream
    };


// -----------------------------------------------------------------------------
// CDRMRightsData::StoreIndexL
// -----------------------------------------------------------------------------
//
void CDRMRightsData::StoreIndexL( RWriteStream& aStream ) const
    {
    // Write the common id to the stream
    iCommonId.ExternalizeL( aStream );

    // Write the key id to the stream
    iKeyId.ExternalizeL( aStream );

    // Write the array size and possibly the array to the stream
    aStream.WriteInt32L( iArray.Count() );

    for( TInt count = 0; count < iArray.Count(); count++ )
        {
        aStream.WriteUint32L( iArray[ count ] );
        }
    };

// -----------------------------------------------------------------------------
// CDRMRightsData::FetchIndexL
// -----------------------------------------------------------------------------
//
void CDRMRightsData::FetchIndexL()
    {
    TStreamId rootId;
    RStoreReadStream stream;
    TInt count = 0;
    TDRMUniqueID id;

    // Get the stream id of the root
    rootId = iStore->Root();

    // Open the root stream
    stream.OpenLC( *iStore, rootId );

    // Do the actual reading, reading also the key id and the common id

    // Read the common id
    iCommonId.InternalizeL( stream );

    // Read the key id
    iKeyId.InternalizeL( stream );

    // read the count of the array:
    iArray.Reset();

    count = stream.ReadInt32L();

    for( ; count > 0; count-- )
        {
        id = stream.ReadUint32L();
        iArray.InsertInUnsignedKeyOrder( id );
        }

    // Close the stream
    CleanupStack::PopAndDestroy();
    };

// -----------------------------------------------------------------------------
// CDRMRightsData::FetchCommonDataL
// -----------------------------------------------------------------------------
//
void CDRMRightsData::FetchCommonDataL()
    {
    RStoreReadStream stream;

    // Open the root stream
    stream.OpenLC( *iStore, iCommonId );

    iData->InternalizeL( stream );

    CleanupStack::PopAndDestroy();
    };


// -----------------------------------------------------------------------------
// Default Constructor - First phase.
// Can be used by itself to generate an empty object
// -----------------------------------------------------------------------------
//
CDRMRightsData::CDRMRightsData( CDRMCommonData* aData, RFs& aFileServer ) :
    iFileServer( aFileServer ), iData( aData )
    {
    };

// -----------------------------------------------------------------------------
// 2nd phase Constructor
// -----------------------------------------------------------------------------
//
void CDRMRightsData::ConstructL( const TFileName& aRightsStore )
    {
    // Open the file store
    iStore = CPermanentFileStore::OpenL( iFileServer, aRightsStore, EFileRead|EFileWrite );

    // Get the index
    FetchIndexL();

    // Get the common data
    FetchCommonDataL();

    // Get the key
    FetchKeyL();
    };

// -----------------------------------------------------------------------------
// 2nd phase Constructor
// -----------------------------------------------------------------------------
//
void CDRMRightsData::ConstructL( const TFileName& aRightsStore,
                                 const TDesC8& aKey )
    {

    // Check if the key is given or a null desc
    if( aKey.Length() )
        {
        iKey = aKey;
        iKeyExists = ETrue;
        }
    else
        {
        iKeyExists = EFalse;
        }

    // Creates the required stores and indexes
    CreateAndInitializeStoreL( aRightsStore );
    };


// End of File
