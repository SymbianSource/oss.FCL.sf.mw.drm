/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef DRMRIGHTSDATA_H
#define DRMRIGHTSDATA_H

// INCLUDES

#include <e32base.h>
#include "DRMTypes.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FORWARD DECLARATIONS
class CDRMPermission;
class CDRMCommonData;
class CPermanentFileStore;
class RWriteStream;
class TStreamId;

// TYPE DEFINITIONS

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
*  CDRMRightsData implements the rights database required by DRM Engine
*
*  @lib RightsServer.dll
*  @since 3.0
*/

NONSHARABLE_CLASS( CDRMRightsData ) : public CBase 
    {
public: // Constructors and destructor
    /**
    * NewLC
    *
    * Creates an instance of the CDRMRightsData class and returns a pointer 
    * to it. The function leaves the object into the cleanup stack
    *
    * @since  3.0
    * @param    aData : The common data to store takes possession of the data
    * @param    aKey : Key of the content
    * @param    aRightsFile : Full pathname of the rights store
    * @param    aFileServer : File server instance to use
    * @return   Functional CDRMRightsData object, Function leaves if an error 
    *           occurs.
    */
    static CDRMRightsData* NewLC( CDRMCommonData* aData,
                                  const TDesC8& aKey, 
                                  const TFileName& aRightsFile,
                                  RFs& aFileServer );
    
    /**
    * NewL
    *
    * Creates an instance of the CDRMRightsData class and returns a pointer 
    * to it.
    *
    * @since  3.0
    * @param    aData : The common data to store takes possession of the data
    * @param    aKey : Key of the content
    * @param    aRightsFile : Full pathname of the rights store
    * @param    aFileServer : File server instance to use    
    * @return   Functional CDRMRightsData object, Function leaves if an error 
    *           occurs.
    */
    static CDRMRightsData* NewL( CDRMCommonData* aData, 
                                 const TDesC8& aKey,
                                 const TFileName& aRightsFile,
                                 RFs& aFileServer );
    /**
    * OpenLC
    *
    * Opens the store and creates an instance of the CDRMRightsData class and returns
    * a pointer to it. The function leaves the object into the cleanup stack
    *
    * @since  3.0
    * @param  aRightsFile : Full pathname of the rights store
    * @param    aFileServer : File server instance to use    
    * @return Functional CDRMRightsData object, Function leaves if an error 
    *         occurs.
    */
    static CDRMRightsData* OpenLC( const TFileName& aRightsFile,
                                   RFs& aFileServer );
    /**
    * OpenL
    *
    * Opens the store and creates an instance of the CDRMRightsData class and returns
    * a pointer to it.
    *
    * @since  3.0
    * @param  aRightsFile : Full pathname of the rights store
    * @param    aFileServer : File server instance to use    
    * @return Functional CDRMRightsData object, Function leaves if an error 
    *         occurs.
    */
    static CDRMRightsData* OpenL( const TFileName& aRightsFile,
                                  RFs& aFileServer );

    /*
    * Destructor
    */ 
    virtual ~CDRMRightsData(); 


public: // New functions

    /**
    * GetCommonDataL
    * 
    * returns a pointer a common data object
    *
    * @since    3.0
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    const CDRMCommonData* GetCommonDataL() const;

    /**
    * UpdateCommonDataL
    * 
    * Updates the common data part
    *
    * @since    3.0
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void UpdateCommonDataL( CDRMCommonData *aData );        
    
    /**
    * StoreNewPermissionL
    * 
    * Writes the data of the permission to the store and returns
    * the stream id
    *
    * @since    3.0
    * @param    aPermission : CDRMPermission class to externalize
    * @param    aStream : the output stream to write to
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void StoreNewPermissionL( CDRMPermission& aPermission,
                              TDRMUniqueID& aStream );    
    
    /**
    * UpdatePermissionL
    * 
    * Writes the data of the object into the stream
    *
    * @since    3.0
    * @param    aPermission : CDRMPermission class to externalize
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void UpdatePermissionL( const CDRMPermission& aPermission );
    
    /**
    * FetchPermissionL
    * 
    * Reads the data of the object from the stream
    *
    * @since    3.0
    * @param    aPermission : a CDRMPermission class to internalize to
    * @param    aStream : the stream id to write the permission to
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void FetchPermissionL( CDRMPermission& aPermission,
                           const TDRMUniqueID& aStream ) const;

    /**
    * DeletePermissionL
    * 
    * Deletes the data of the object from the stream
    *
    * @since    3.0
    * @param    aStream : the stream id to write the permission to
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void DeletePermissionL( const TDRMUniqueID& aStream );
                           
    /**
    * FetchPermissionsL
    * 
    * Reads all the permissions and puts the data into the pointer array
    *
    * @since    3.0
    * @param    aPointerArray : return value pointer array of the permissions
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void FetchAllPermissionsL( RPointerArray<CDRMPermission>& 
                               aPointerArray );

    /**
    * DeleteAllPermissionsL
    * 
    * Deletes the data of the object from the stream
    *
    * @since    3.0
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void DeleteAllPermissionsL();


    /**
    * DeleteExpiredPermissionsL
    * 
    * Deletes the data of the object from the stream
    *
    * @since    3.0
    * @param	aTime : Current secure time for checking the expiration
    *				    if the time is Time::NullTTime() then only
    *				    expired non time based rights are deleted
    * @param    aParents : ETrue if any Ro's have parents otherwise EFalse
    * @return   The amount of permissions left in the file
    * @leave    The function leaves with Symbian OS error code if an
    *           error occurs
    */   
	TInt DeleteExpiredPermissionsL( const TTime& aTime, TBool& aParents );   
                           
                           
    /**
    * StoreKeyL
    * 
    * Writes the key to the store
    *
    * @since    3.0
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void StoreKeyL();
    
    /**
    * GetKeyL
    * 
    * Returns the key
    *
    * @since    3.0
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    HBufC8* GetKeyL() const;


public:

private:

    /**
    * CreateAndInitializeStoreL
    * 
    * Writes the data of index into the store root
    *
    * @since    3.0
    * @param    aRightsStore : Full path filename of the rights store    
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void CreateAndInitializeStoreL( const TFileName& aRightsStore );

    /**
    * ExternalizeIndexL
    * 
    * Writes the data of index into the store root
    *
    * @since    3.0
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void StoreIndexL( RWriteStream& aStream ) const;
    
    /**
    * InternalizeIndexL
    * 
    * Reads the data of the index from the store root
    *
    * @since    3.0
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void FetchIndexL();
    
    /**
    * FetchCommonDataL();
    *
    * Reads the common data from the store
    *
    */
    void FetchCommonDataL();
    
    /**
    * FetchKeyL
    * 
    * Reads the key from the store
    *
    * @since    3.0
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void FetchKeyL();
    
    /*
    * Default Constructor
    */ 
    CDRMRightsData( CDRMCommonData* aData, RFs& aFileServer );

    /*
    * 2nd phase Constructor
    *
    * @param    aRightsStore : Full path filename of the rights store
    */ 
    void ConstructL( const TFileName& aRightsStore );

    /*
    * 2nd phase Constructor
    *
    * @param    aRightsStore : Full path filename of the rights store
    */ 
    void ConstructL( const TFileName& aRightsStore,
                     const TDesC8& aKey );

private:
    RFs& iFileServer;

    CPermanentFileStore* iStore;
    
    TBool iKeyExists;    
    TBuf8<16> iKey;
    RArray<TDRMUniqueID> iArray;
    CDRMCommonData* iData;
    TStreamId iCommonId;
    TStreamId iKeyId;
   
    // Some index data
    };

#endif      // DRMRIGHTSDATA_H
            
// End of File
