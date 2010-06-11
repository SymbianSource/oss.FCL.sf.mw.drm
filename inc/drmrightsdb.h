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
* Description:  Interface for the DRM Rights database
*
*/


#ifndef DRMRIGHTSDB_H
#define DRMRIGHTSDB_H

// INCLUDES

#include <e32base.h>
#include <s32mem.h>
#include "DRMBackupInterface.h"
#include "DRMTypes.h"

// CONSTANTS

// Directory for temp files
_LIT( KDRMDbTempPath, "c:\\system\\temp\\" );

// The primary database temp file
_LIT( KDRMDbTempFileName, "c:\\private\\101F51F2\\RightsServer.tmp" );

// These are internally defined in the DRM Authenticated API
// If they change they should be also changed here
_LIT8( KFLKString, "flk:" );
LOCAL_C const TInt KFLKStringLength = 4;

_LIT8( KCIDString, "cid:" );
LOCAL_C const TInt KCIDStringLength = 4;


// MACROS

// DATA TYPES

// FORWARD DECLARATIONS
class CDRMPermission;
class RFs;
class CMD5;
class RReadStream;
class RWriteStream;
class CLogFile;
class CDRMRightsCleaner;
class CDRMRightsServer;
class CDcfRep;
class CDRMRightsServer;

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
*  CDRMRightsDB implements the rights database required by DRM Engine
*
*  @lib RightsServer.dll
*  @since 2.5
*/

NONSHARABLE_CLASS( CDRMRightsDB ) : public CBase , 
                                    public MDRMBackupInterface
    {
    public: // Constructors and destructor
        
        /**
        * NewLC
        *
        * Creates an instance of the CDRMRightDB class and returns a pointer 
        * to it The function leaves the object into the cleanup stack
        *
        * @since  3.0
        * @param  aFs : Open file server session
        * @param  aDatabasePath : full pathname of the database path
        * @param  aKey : content encryption key 16 bytes in length.
        * @param  aImei: device serial number (used for UDT)
        * @return Functional CDRMRightsDB object, Function leaves if an error 
        *         occurs.
        */
        static CDRMRightsDB* NewLC( RFs& aFs,
                                    const TDesC& aDatabasePath,
                                    const TDesC8& aKey,
                                    const TDesC& aImei,
                                    CDRMRightsServer* aServer );
        
        /**
        * NewL
        *
        * Creates an instance of the CDRMRightDB class and returns a pointer
        * to it
        *
        * @since  3.0
        * @param  aFs : Open file server session
        * @param  aDatabasePath : full pathname of the database path
        * @param  aKey : content encryption key 16 bytes in length.
        * @param  aImei: device serial number (used for UDT)
        * @return Functional CDRMRightsDB object, Function leaves if an error
        *         occurs.
        */
        static CDRMRightsDB* NewL( RFs& aFs,
                                   const TDesC& aDatabasePath,
                                    const TDesC8& aKey,
                                    const TDesC& aImei,
                                    CDRMRightsServer* aServer );
          
        /**
        * Destructor
        */
        virtual ~CDRMRightsDB();

    public: // New functions    
        
        /**
        * GetDBEntryByContentIDL
        *
        * Gets all rights objects connected to aContentID and inserts them into
        * the pointer array aRightsList
        *
        * @since  2.5
        * @param  aContentID : content identifier
        * @param  aRightsList : pointer array of the rights object to be filled
        * @return none , Function leaves if an error occurs
        */
        void GetDBEntryByContentIDL( const TDesC8& aContentID, 
                                     RPointerArray<CDRMPermission>& aRightsList);
        
        /**
        * GetDBEntryByContentIDL
        *
        * Gets the rights object connected to aContentID with the unique
        * identifier aUniqueID and returns a pointer to it, caller must free 
        * the memory of the pointer
        *
        * @since  2.5
        * @param  aContentID : content identifier
        * @param  aUniqueID : unique identifier of the rights object
        * @return CDRMPermission pointer, Function leaves if an error occurs
        */
        CDRMPermission* GetDBEntryByContentIDL( const TDesC8& aContentID, 
                                               const TDRMUniqueID aUniqueID );
                
        /**
        * AddDBEntryL
        *
        * Adds a new entry to the database connected with aContentID.
        *
        * @since  2.5
        * @param  aContentID : content identifier
        * @param  aRightsObject : the rights object to be added, all fields but
        *                         the unique identifier must be set that need to
        *                         be set
        * @param  aEncryptionKey : the 16 byte key used to decrypt content
        * @param  aUniqueID : the unique id of the added rights object, in/out
        *                     parameter The function will try to use aUniqueID 
        *                     as the unique id if it is not possible or 
        *                     aUniqueID is 0 a new random unique id will be 
        *                     generated
        * @return None, Function leaves if an error occurs
        */
        void AddDBEntryL( const TDesC8& aContentID, 
                          CDRMPermission& aRightsObject,
                          const TDesC8& aEncryptionKey,
                          TDRMUniqueID& aUniqueID );
                
        /**
        * DeleteDBEntryL
        *
        * Deletes an entry from the database connected to aContentID with unique
        * identifier aUniqueID 
        *
        * @since  2.5
        * @param  aContentID : content identifier
        * @param  aUniqueID : unique identifier of the rights object
        * @return None, Function leaves if an error occurs
        */
        void DeleteDBEntryL( const TDesC8& aContentID, 
            const TDRMUniqueID aUniqueID );
               
        /**
        * DeleteDBEntryL
        *
        * Deletes all entries from the database connected to aContentID 
        *
        * @since  2.5
        * @param  aContentID : content identifier
        * @return None, Function leaves if an error occurs
        */
        void DeleteDBEntryL( const TDesC8& aContentID );
               
        /**
        * UpdateDBEntryL
        *
        * Updates an entry from the database connected to aContentID with 
        * unique identifier aUniqueID 
        *
        * @since  2.5
        * @param  aContentID : content identifier
        * @param  aRightsObject : the updated rights object
        * @return None, Function leaves if an error occurs
        */
        void UpdateDBEntryL( const TDesC8& aContentID,  
                             const CDRMPermission& aRightsObject);
        
        /**
        * ExportContentIDListL
        *
        * Exports a list of content id:s to a file
        *
        * @since  2.5
        * @param  aTempFile : return param full pathname of the temporary file
        *                     to use
        * @return None, Function leaves if an error occurs
        */
        void ExportContentIDListL( TFileName& aTempFile );
                
        /**
        * BackupDBL
        *
        * Backups the database to aBackupDBFile and creates the warranty
        * transfer file aWTFile and encrypts it with the aKey
        *
        * @since  2.5
        * @param  aWTFile : full pathname of the warranty transfer file
        * @param  aEncryptionKey : 16 byte encryption key
        * @return None, Function leaves if an error occurs
        */
/*        void BackupDBL( const TDesC& aWTFile, 
                        const TDesC8& aEncryptionKey );
*/               
        /**
        * MergeDBL
        *
        * Merges the backup database into the current database
        *
        * @since  2.5
        * @return None, Function leaves if an error occurs
        */
//        void MergeDBL();
        
        /**
        * GetDecryptionKey
        *
        * Returns the decryption key for rights object connected to aContentID
        *
        * @since  2.5
        * @param  aContentID content ID
        * @return HBufC8 pointer or NULL if failed
        */
        HBufC8* GetDecryptionKeyL( const TDesC8& aContentID );
                
        /**
        * DeleteDBL
        *
        * Deletes the rights database file and creates an empty file in it's
        * place
        *
        * @since  2.5
        * @return can leave with a symbian error code
        */
        void DeleteDBL( void );

        /**
        * GetAmountOfRightsObjects
        *
        * Returns the amount of unique content id's from in the rights database
        *
        * @since  2.5
        * @return the amount of rights objects in the database
        */
        TInt32 GetAmountOfRightsObjectsL();
        
        
        /**
        * AddDomainROL
        *
        * Adds a domain rights object just the XML representation
        *
        * @since    3.0
        * @param    aRoId : The Rights object identifier of the RO
        * @param    aXmlData : The Xml data to be saved
        * @return can leave with symbian on error code
        */
        void AddDomainROL( const TDesC8& aRoId, const TDesC8& aXmlData );
        
        /**
        * GetDomainROL
        *
        * Gets a domain rights object just the XML representation
        *
        * @since    3.0
        * @param    aRoId : The Rights object identifier of the RO
        * @return   Domain RO Xml representation or NULL
        *           can leave with symbian on error code
        */
        HBufC8* GetDomainROL( const TDesC8& aRoId );

        /**
        * DeleteDomainROL
        *
        * Deletes a domain rights objects XML representation
        *
        * @since    3.0
        * @param    aRoId : The Rights object identifier of the RO
        * @return can leave with symbian on error code
        */
        void DeleteDomainROL( const TDesC8& aRoId );

        /**
        * DeleteExpiredPermissionsL
        * 
        * Delete expired permissions.
        *
        * @since    3.0
        * @param    aTime : current time
        * @param    aStatus : request status for the asynchronous requrest
        * @return   CDRMRightsCleaner object which is used to control the
        *           expired permissions deletion process
        */      
        CDRMRightsCleaner* DeleteExpiredPermissionsL( const TTime& aTime,
                                                      TRequestStatus& aStatus );

        
        /**
        * NameContentL
        * 
        * Give a name to the content, if the name is empty the content name
        * will be cleared
        *
        * @since    3.0
        * @param    aContentId : content identifier
        * @param    aName : the name for the content        
        * @return   None
        * @leave	Leaves with KErrNotFound if the content doesn't exist
        */      
        void NameContentL( const TDesC8& aContentId,
        				   const TDesC& aName );

        /**
        * ContentNameL
        * 
        * Returns a pointer to the name of the content and leaves it in the
        * cleanup stack
        *
        * @since    3.0
        * @param    aContentId : content identifier       
        * @return   the name of the content in a HBufC*
        * @leave	Leaves with KErrNotFound if the content doesn't exist        
        */          
        HBufC* ContentNameLC( const TDesC8& aContentID );
        
        /**
        * DeleteExpiredL
        * 
        * Delete expired from the current file store
        *
        * @since    3.0
        * @param    aFileName : name of the permission file store
        * @param    aTime : current time.
        * @return   TBool : ETrue if the file can be deleted
        *                   EFalse if the file can't be deleted
        */         
        TBool DeleteExpiredL( const TFileName& aFileName,
                             const TTime& aTime );     

        /**
        * DeleteExpiredL
        * 
        * Delete expired from the current file store
        *
        * @since    3.0
        * @param    aFileName : name of the permission file store
        * @param    aTime : current time.
        * @return   TBool : ETrue if the file can be deleted
        *                   EFalse if the file can't be deleted
        */         
        TBool DeleteExpiredL( const TFileName& aFileName,
                             const TTime& aTime,
                             const TDesC8& aContentId );  
  
        /**
        * GetUdtDataL
        *
        * Gets the udt data from a restore file if it exists
        * if not it leaves with KErrNotFound
        *
        * @since    3.0
        * @param    aStream : the stream to write to
        * @return   HBufC8* with the UDT data encrypted with the udt public key
        * @leave Can leave with a Symbian OS error code
        */
        HBufC8* GetUdtDataLC();  
  
        /**
        * InitiateUdtL
        *
        * Initiates the User Data Transfer
        *
        * @since    3.0
        * @param    aKey : the key used to encrypt the data encrypted with
        *                  the device public key
        * @return   none
        * @leave Can leave with a Symbian OS error code
        */
        void InitiateUdtL( const TDesC8& aKey );    

        /**
        * CleanUdtData
        *
        * Deletes the restore file if it exists
        *
        * @since    3.0
        * @return None
        */ 
        void CleanUdtData(); 
        
        /**
        * GetContentIDListL
        *
        * Get a list of all the content id's in the database
        *
        * @since  2.5
        * @param  aArray : The pointer array has all the content id's
        *                  that have been added to the rights database
        * @return None, Function leaves if an error occurs
        */
        void GetContentIDListL( RPointerArray<HBufC8>& aArray );

        /**
        * SetAuthenticationSeedL
        * 
        * Set the authentication seed for a content ID
        *
        * @since    3.0
        * @param    aContentId : content identifier
        * @param    aSeed : value for the seed
        * @return   None
        * @leave	Leaves with KErrNotFound if the content doesn't exist
        */      
        void SetAuthenticationSeedL( const TDesC8& aContentId,
        				             const TDesC8& aSeed );

        /**
        * GetAuthenticationSeedL
        * 
        * Get the authentication seed for a content ID
        *
        * @since    3.0
        * @param    aContentId : content identifier
        * @return   value for the seed
        * @leave	Leaves with KErrNotFound if the content doesn't exist
        */      
        HBufC8* GetAuthenticationSeedL( const TDesC8& aContentID );
        				               
        // test function
        void CreateDummyUDTFileL();        
           
    public: // Inherited from MDRMBackupInterface
    
        /**
        * BackupContentToFileL
        *
        * Backups content to file in encrypted format
        *
        * @since    3.0
        * @param    aBackupFile : File handle to the backup file
        * @param    aEncryptionKey : the key to use to encrypt the backup file
        * @leave    Symbian OS error code if an error occurs 
        */
        virtual void BackupContentToFileL( RFile& aBackupFile,
                                           const TDesC8& aEncryptionKey, 
                                           const TInt aMode = KDRMNormalBackup );

        /**
        * RestoreContentFromFileL
        *
        * Restores the content file from encrypted format
        *
        * @since    3.0
        * @param    aBackupFile : File handle to backup file
        * @param    aEncryptionKey : the key to use to encrypt the backup file
        * @leave    Symbian OS error code if an error occurs 
        */                           
        virtual void RestoreContentFromFileL( RFile& aBackupFile,
                                              const TDesC8& aEncryptionKey,
                                              const TInt aMode = KDRMNormalBackup );    
        
        
        /**
        * Updating
        *
        * Returns the ETrue if the database is being updated
        *
        * @since    3.0
        * @param    aStream : the stream to write to
        * @return   ETrue if files are currently modified in the DB
        */
        TBool Updating();

        /**
        * MarkAsCorrupted
        *
        * Marks the rights database as corrupted, to be deleted on reboot
        *
        * @since    3.0
        */
        void MarkAsCorrupted();

    private:
        /**
        * Default Constructor - First phase. Prevented.
        */
        CDRMRightsDB(); 
    
        /**
        * Default Constructor - First phase.
        */
        CDRMRightsDB( RFs& aFs, CDRMRightsServer* aServer ); 
        
        /**
        * ConstructL
        *
        * Second phase constructor
        *
        * @since  2.5
        * @param  aDatabasePath : full pathname of the database
        * @param  aKey : database encryption key
        * @param  aImei: device serial number (used for UDT)
        * @return Leaves if an error occurs
        */  
        void ConstructL( const TDesC& aDatabasePath,
                         const TDesC8& aKey,
                         const TDesC& aImei );

        /**
        * Assignment operator - Prevented
        */
        CDRMRightsDB& operator =( const CDRMRightsDB& );    
    
        /**
        * Copy constructor - Prevented
        */
        CDRMRightsDB( const CDRMRightsDB& );                
       

        /**
        * InitializeDatabaseL
        *
        * Initializes the database for usage
        *
        * @since  2.5
        * @return Leaves if an error occurs.
        */  
        void InitializeDatabaseL( void );
    
        /**
        * HashContentID
        *
        * Fills the aRightsObject with the correct data from aData and checks if the
        * rights object can be restored or not.
        *
        * @since    3.0  
        * @param    aHashKey  : hashed key, out parameter
        * @param    aContentID : the content id to be hashed
        * @return   None
        */      
        void HashContentID( TPtrC8& aHashKey, const TDesC8& aContentID );

            
        /**
        * GetRightsFileNameL
        * 
        * Gets the whole path of the permissions file
        *
        * @since    3.0
        * @param    aContentID : the content id to look for
        * @param    aPath : return parameter for the filename
        * @return   None, leaves with SymbianOS error code or DRMCommon error
        *           code if an error occurs
        */      
        void GetRightsFileNameL(
            const TDesC8& aContentID,
            TFileName& aPath);

        /**
        * GetXMLFileNameL
        * 
        * Gets the whole path of the domain rights object XML representation
        *
        * @since    3.0
        * @param    aRoID : the rights object id to look for
        * @param    aPath : return parameter for the filename
        * @return   None, leaves with SymbianOS error code or DRMCommon error
        *           code if an error occurs
        */    
        void GetXMLFileNameL(
            const TDesC8& aRoID,
            TFileName& aPath);
            
        /**
        * ModifyKey
        * 
        * Encrypts or decrypts the key
        *
        * @since    3.0
        * @param    aKey : the key to be encrypted or decrypted
        * @return   None
        */      
        void ModifyKey( TDesC8& aKey );
        
        /**
        * AddUDTDataL
        *
        * Adds user data transfer data to the header part of the backup
        * file
        *
        * @since    3.0
        * @param    aStream : the stream to write to
        * @return None
        * @leave Can leave with a Symbian OS error code
        */
        void AddUDTDataL( RWriteStream& aStream );         
        
        // Helper function to do encryption to the stream
        void WriteEncryptedStreamL( RWriteStream& aStream,
                                    const TDesC8& aMessageData,
                                    TDes8& aIv, 
                                    TDes8& aRemainder,
                                    HBufC8*& aEncryptionBuffer,
                                    TInt& aBytesWritten );       

        // Helper function to do encryption to the stream
        void FinalizeEncryptedStreamL( RWriteStream& aStream,
                                       TDes8& aIv, 
                                       TDes8& aRemainder,
                                       HBufC8*& aEncryptionBuffer,
                                       TInt& aBytesWritten ); 
        
        // Aes encryption                               
        void EncryptL( const TDesC8& aIv, 
                       TPtr8& aData, 
                       TBool aAddPadding );
                      
        void DecryptL( const TDesC8& aIv, 
                       TPtr8& aData, 
                       TBool aRemovePadding,
                       const TDesC8& aEncryptionKey );   
                       
        void CheckPaddingL( const TDesC8& aData );
        
        // Helper function to read and decrypt data and reserve memory
        // for it if needed
        void ReadDataL( RFile& aStream,
                        TDes8& aEncIV,
                        TPtr8& aReadData,
                        HBufC8*& aDataBuffer,
                        TInt& dataLeft,
                        TInt size,
                        TBool aStart,
                        const TDesC8& aEncryptionKey );    
        
        // check if the file can be deleted:                
        void CheckCleanup( const TDesC& aFileName );                                                                                      

    private:
        // fileserver session
        RFs& iFileServer;
    
        // rights database file, fileserver subsession
        HBufC* iDbPath;
    
        // The md5 hash algorithm implementation
        CMD5 *iHasher;
        
        // The Rights database key
        HBufC8* iKey;
        
        // mem stream for restore
        RMemReadStream iMemStream;
        
        // device serial number (used for UDT)
        HBufC* iImei;
        
        // Time stamp of the last update operation
        TTime iLastUpdate;
        
        CDRMRightsServer* iRightsServer;
    };

#endif      // DRMRIGHTSDB_H   
            
// End of File
