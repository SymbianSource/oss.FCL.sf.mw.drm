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


#ifndef DRMCONTEXTDB_H
#define DRMCONTEXTDB_H

// INCLUDES

#include <e32base.h>
#include <hash.h>


// CONSTANTS

// MACROS

// DATA TYPES

// FORWARD DECLARATIONS
class CDRMRIContext;
class CDRMDomainContext;
class CPermanentFileStore;
class RWriteStream;
class TStreamId;

// TYPE DEFINITIONS

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
*  CDRMContextDB implements the rights database required by DRM Engine
*
*  @lib RightsServer.dll
*  @since 3.0
*/

NONSHARABLE_CLASS( CDRMContextDB ) : public CBase 
    {
public: // Constructors and destructor

    /**
    * NewLC
    *
    * Creates an instance of the CDRMContextDB class and returns a pointer 
    * to it. The function leaves the object into the cleanup stack
    *
    * @since    3.0
    * @param    aRightsIssuerContextFile : Full pathname of the ri context file
    * @param    aDomainContextFile : Full pathname of the domain context file
    * @param    aFileServer : File server instance to use
    * @return   Functional CDRMContextDB object, Function leaves if an error 
    *           occurs.
    */
    static CDRMContextDB* NewLC( const TFileName& aRightsIssuerContextFile, 
                                 const TFileName& aDomainContextFile,
                                 RFs& aFileServer );
    
    /**
    * NewL
    *
    * Creates an instance of the CDRMContextDB class and returns a pointer 
    * to it.
    *
    * @since    3.0
    * @param    aRightsIssuerContextFile : Full pathname of the ri context file
    * @param    aDomainContextFile : Full pathname of the domain context file
    * @param    aFileServer : File server instance to use   
    * @return   Functional CDRMContextDB object, Function leaves if an error 
    *           occurs.
    */
    static CDRMContextDB* NewL( const TFileName& aRightsIssuerContextFile, 
                                const TFileName& aDomainContextFile,
                                RFs& aFileServer );
    /**
    * OpenLC
    *
    * Opens the store and creates an instance of the CDRMContextDB class and returns
    * a pointer to it. The function leaves the object into the cleanup stack
    *
    * @since    3.0
    * @param    aRightsIssuerContextFile : Full pathname of the ri context file
    * @param    aDomainContextFile : Full pathname of the domain context file
    * @param    aFileServer : File server instance to use     
    * @return   Functional CDRMContextDB object, Function leaves if an error 
    *           occurs.
    */
    static CDRMContextDB* OpenLC( const TFileName& aRightsIssuerContextFile, 
                                  const TFileName& aDomainContextFile,
                                  RFs& aFileServer );
    /**
    * OpenL
    *
    * Opens the store and creates an instance of the CDRMContextDB class and returns
    * a pointer to it.
    *
    * @since    3.0
    * @param    aRightsIssuerContextFile : Full pathname of the ri context file
    * @param    aDomainContextFile : Full pathname of the domain context file
    * @param    aFileServer : File server instance to use      
    * @return   Functional CDRMContextDB object, Function leaves if an error 
    *           occurs.
    */
    static CDRMContextDB* OpenL( const TFileName& aRightsIssuerContextFile, 
                                 const TFileName& aDomainContextFile,
                                 RFs& aFileServer );

    /*
    * Destructor
    */ 
    virtual ~CDRMContextDB(); 


public: // New functions
     
    
    /**
    * StoreDomainContextL
    * 
    * Writes the data of the domain context to the stream store
    *
    * @since    3.0
    * @param    aContext : CDRMDomainContext class to externalize
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void StoreDomainContextL( const CDRMDomainContext& aContext );    
    
    /**
    * FetchDomainContextL
    * 
    * Reads the data of the object from the stream store
    *
    * @since    3.0
    * @param    aContext : a CDRMDomainContext class to internalize to
    *                      with the domain id set to the requested id
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void FetchDomainContextL( CDRMDomainContext& aContext ) const;
    
    /**
    * FetchDomainContextL
    * 
    * Reads the data of the object from the stream
    *
    * @since    3.0
    * @param    aContext : a CDRMDomainContext class to internalize to
    *                      with the domain id set to the requested id
    * @param    aStream : stream to read from
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void FetchDomainContextL( CDRMDomainContext& aContext, 
                              const TInt32& aStream ) const;    

    /**
    * DeleteDomainContextL
    * 
    * Deletes the data of the object from the stream store
    *
    * @since    3.0
    * @param    aDomainID : the domain to delete
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void DeleteDomainContextL( const TDesC8& aDomainID );


    /**
    * StoreRightsIssuerContextL
    * 
    * Writes the data of the rights issuer context to the stream store
    *
    * @since    3.0
    * @param    aPermission : CDRMPermission class to externalize
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void StoreRightsIssuerContextL( const CDRMRIContext& aContext );    
    
    /**
    * FetchRightsIssuerContextL
    * 
    * Reads the data of the object from the stream store
    *
    * @since    3.0
    * @param    aContext : a CDRMRightsIssuerContext class to internalize to
    *                      with the domain id set to the requested id
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void FetchRightsIssuerContextL( CDRMRIContext& aContext ) const;

    /**
    * FetchRightsIssuerContextL
    * 
    * Reads the data of the object from the stream
    *
    * @since    3.0
    * @param    aContext : a CDRMRightsIssuerContext class to internalize to
    *                      with the domain id set to the requested id
    * @param    aStream : stream to read from
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void FetchRightsIssuerContextL( CDRMRIContext& aContext, 
                                    const TInt32& aStream ) const;
    /**
    * DeleteRightsIssuerContextL
    * 
    * Deletes the data of the object from the stream store
    *
    * @since    3.0
    * @param    aRightsIssuerID : the rights issuer to delete
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void DeleteRightsIssuerContextL( const TDesC8& aRightsIssuerID );

                           
    /**
    * WhiteListURLExistsL
    * 
    * Goes through all the Rights Issuer contexts and checks if the given URL
    * is in one of them.
    *
    * @since    3.0
    * @param    aPointerArray : return value pointer array of the permissions
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    TBool WhiteListURLExistsL( const TDesC8& aRightsIssuerURL );

    
    /**
    * DeleteExpiredDomainContextsL
    * 
    * Deletes the expired domain contexts
    *
    * @since    3.0
    * @param    aTime : the reference time, all contexts before this time will be
    *                   deleted
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void DeleteExpiredDomainContextsL( const TTime& aTime );
 
 
    /**
    * DeleteExpiredRightsIssuerContextsL
    * 
    * Deletes the expired domain contexts
    *
    * @since    3.0
    * @param    aTime : the reference time, all contexts before this time will be
    *                   deleted
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void DeleteExpiredRightsIssuerContextsL( const TTime& aTime );
    
    /**
    *
    * DeleteAllL
    *
    * Deletes all stored RI and Domain contexts
    *
    * @since    3.0
    * @param    aRightsIssuerContextFile: Full pathname of the ri context file
    * @param    aDomainContextFile: Full pathname of the domain context file
    *
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    *
    */
    void DeleteAllL( const TFileName& aRightsIssuerContextFile, 
                     const TFileName& aDomainContextFile );

    /**
    *
    * FetchAllRightsIssuerContextsL
    *
    * Retrieves all stored RI contexts in a pointer array
    *
    * @since    3.2
    * @param    aContexts: RPointerArray for storage of the RI contexts
    *
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    *
    */
    void FetchAllRightsIssuerContextsL( RPointerArray<CDRMRIContext>& aContexts) const;

    /**
    *
    * UpdateRightsIssuerContextL
    *
    * Updates the metering status of the RI context
    *
    * @since    3.2
    * @param    aContext: The RI context which has the updated data
    *
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    *
    */
    void UpdateRightsIssuerContextL( const CDRMRIContext& aContext );  

private:

    /**
    * CreateAndInitializeRIStoreL
    * 
    * Writes the data of index into the store root
    *
    * @since    3.0
    * @param    aRightsStore : Full path filename of the ri context store    
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void CreateAndInitializeRIStoreL( const TFileName& aRIStore );

    /**
    * CreateAndInitializeDomainStoreL
    * 
    * Writes the data of index into the store root
    *
    * @since    3.0
    * @param    aRightsStore : Full path filename of the domain context store    
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void CreateAndInitializeDomainStoreL( const TFileName& aDomainStore );

    /**
    * StoreDomainIndexL
    * 
    * Writes the data of index into the store root
    *
    * @since    3.0
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void StoreDomainIndexL( RWriteStream& aStream ) const;
    
    /**
    * FetchDomainIndexL
    * 
    * Reads the data of the index from the store root
    *
    * @since    3.0
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void FetchDomainIndexL();

    /**
    * StoreRightsIssuerIndexL
    * 
    * Writes the data of index into the store root
    *
    * @since    3.0
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void StoreRightsIssuerIndexL( RWriteStream& aStream ) const;
    
    /**
    * FetchRightsIssuerIndexL
    * 
    * Reads the data of the index from the store root
    *
    * @since    3.0
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void FetchRightsIssuerIndexL();
    
    /*
    * Default Constructor
    */ 
    CDRMContextDB( RFs& aFileServer );

    /*
    * 2nd phase Constructor
    *
    * @param    aRightsStore : Full path filename of the rights store
    */ 
    void ConstructL( const TFileName& aRightsIssuerContextFile, 
                     const TFileName& aDomainContextFile );

private:

    NONSHARABLE_CLASS( TContextIndex )
        {
    public:
        
        TBuf8<SHA1_HASH> iContextID;
        TInt32 iStreamID;
        
        static TInt Compare( const TContextIndex& aFirst,
                             const TContextIndex& aSecond )
            {
            return aFirst.iContextID.Compare( aSecond.iContextID );
            };
            
        static TBool Match( const TContextIndex& aFirst,
                            const TContextIndex& aSecond )
            {
            if( aFirst.iContextID.Compare( aSecond.iContextID ) )
                {
                return EFalse;
                }
            return ETrue;    
            };    
            
        };

    RFs& iFileServer;

    // File stores
    CPermanentFileStore* iDomainStore;
    CPermanentFileStore* iRightsIssuerStore;
    
    // Index arrays
    RArray<TContextIndex> iDomainArray;
    RArray<TContextIndex> iRightsIssuerArray;
   
    // Some index data
    };

#endif      // DRMCONTEXTDB_H
            
// End of File
