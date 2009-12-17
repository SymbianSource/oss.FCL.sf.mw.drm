/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is the client side handle of DRM Engine
*
*/



#ifndef RDRMRIGHTSCLIENT_H
#define RDRMRIGHTSCLIENT_H

//  INCLUDES
#include <e32std.h>
#include <Oma2Agent.h>
#include <DRMTypes.h>

using namespace ContentAccess;

// CONSTANTS
// MACROS

// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class RFs;
class CDRMPermission;

// CLASS DECLARATION

/**
*  This class is the client side interface for DRM3 Rights Server.
*  This class can be used to access the DRM Rights Database.
*
*  @lib DRM Core
*  @since S60 Release 2.5
*/
class RDRMRightsClient : public RSessionBase
    {
    public:  // Constructor & destructor
        /**
         * C++ default constructor.
         */
        IMPORT_C RDRMRightsClient();
        
        /** 
         * Destructor.
         */
        IMPORT_C virtual ~RDRMRightsClient();
        
        /** 
         * This method opens the connection between the client and the server.
         * @since 2.5
         * @return Error code. KErrNone if the operation is successful.
         */
        IMPORT_C TInt Connect();
        
        /**
         * This function closes the connection between the client and the server.
         * It is safe to call this method even if connection is not established.
         * @since S60Rel2.5
         */ 
        IMPORT_C void Close();
        
    public: // New functions
        
        /**
        * Starts the rights server in case it is not yet started. It is
        * safe to call this method even if the server is running.
        * @since S60Rel3.0
        * @return Symbian OS error code if any.
        */ 
        IMPORT_C static TInt StartServer();
        
        /**
         * Adds the given rights object into rights database.
         * @since S60Rel2.5
         * @param aCEK CEK.
         * @param aRightsObject Rights object to be added.
         * @param aCID Content-ID.
         * @return Error code.
         */ 
        IMPORT_C TInt AddRecord(
            const TDesC8& aCEK,
            const CDRMPermission& aRightsObject,
            const TDesC8& aCID,
            TDRMUniqueID& aID );
        
        /**
         * Adds a protected RO where the CEK wrapped with another key.
         * @since 3.0
         * @param aProtectedCek wrapped CEK
         * @param aDomainPermission Domain or regular permission
         * @param aRightsObject Rights object to be added.
         * @param aCID Content-ID.
         * @return Error code.
         */ 

        IMPORT_C TInt AddProtectedRecord(
            const TDesC8& aProtectedCek,
            TBool aDomainRecord,
            const CDRMPermission& aRightsObject,
            const TDesC8& aCID,
            TDRMUniqueID& aID ); // Unique ID, out-parameter

        /**
         * Gets all database entries related to specified Content-ID.
         * @since S60Rel2.5
         * @param aId Content ID.
         * @param aRightsList List of rights objects.
         */
        IMPORT_C void GetDBEntriesL(
            const TDesC8& aId,
            RPointerArray< CDRMPermission >& aRightsList );
        
        /**
         * Gets a database entry related to given content ID and unique ID.
         * @since S60Rel2.5
         * @param aContentID Content-ID.
         * @param aUniqueID Unique identifier.
         * @return Associated rights object.
         */
        IMPORT_C CDRMPermission* GetDbEntryL(
            const TDesC8& aContentID,
            const TDRMUniqueID& aUniqueID );
        
        /** 
         * Deletes all rights objects with specified Content-ID.
         * @since S60Rel2.5
         * @param aContentID Content-ID of the rights objects to be deleted.
         * @return Error code. KErrNone if successful.
         */ 
        IMPORT_C TInt DeleteDbEntry( const TDesC8& aContentID );
        
        /**
         * Deletes one rights object, identified by Content-ID and unique ID.
         * @since S60Rel2.5
         * @param aContentID Content-ID of the rights object.
         * @param aUniqueID Unique ID.
         * @return Error code. KErrNone if successful operation.
         */
        IMPORT_C TInt DeleteDbEntry(
            const TDesC8& aContentID, 
            const TDRMUniqueID& aUniqueID );
        
        /**
         * Exports all content ID's from the database to a file.
         * @since S60Rel2.5
         * @param aFileName Out-parameter: contains the name of the file
         *                  if the method completes with KErrNone. The 
         *                  descriptor parameter is assumed to be large 
         *                  enough. The caller is responsible of  deleting 
         *                  the file afterwards.       
         * @return Error code. KErrNone if successful operation.
         */
        IMPORT_C TInt ExportContentIDList( TDes& aFileName );
        
        /**
         * This method exports all Content-IDs from the database file
         * in an array. The caller is responsible of destroying the array
         * afterwards.
         * @since S60Rel2.5
         * @param aCIDList An out-parameter: contains the Content-IDs 
         *        after successful return.
         * @return A Symbian OS error code or DRM3 error code. KErrNone in
         *         successful operation.
         */
        IMPORT_C TInt ExportContentIDList( RPointerArray< HBufC8 >& aCIDList );
        
        /** 
         * Returns the decryption key to the content.
         *
         * @since 2.5
         * @param aIntent In-parameter: How the content is going to be consumed.
         * @param aContentID Content-ID of the related content.
         * @param aKey Out-parameter: Contains the key if function returns 
         *             successfully.
         * @return Error code. KErrNone if successful operation.
        */
        IMPORT_C TInt GetDecryptionKey(
            const TInt aIntent,
            const TDesC8& aContentID,
            TDes8& aKey );
        
        /** 
         * This method checks whether the caller has sufficient rights over
         * the content at this very moment.
         * @param aIntent Intended usage.
         * @param aContentID Content-ID.
         * @return KErrNone in successful
         * completition.
         */
        IMPORT_C TInt CheckRights(
            const TInt aIntent,
            const TDesC8& aContentID,
            TUint32& aRejection );
            
        /** 
         * This method checks whether the caller has sufficient rights over
         * the content at this very moment. It also returns the active rights.
         * @param aIntent Intended usage.
         * @param aContentID Content-ID.
         * @return Permission for the intent. NULL if no permissions exist
         * completition.
         */
        IMPORT_C CDRMPermission* GetActiveRightsL(
            const TInt aIntent,
            const TDesC8& aContentID,
            TUint32& aRejection );           
       
        /**
        * Sets the key for subsequent encryption and decryption operations.
        * The key is stored on the server side in the session and will
        * be usable until the session is closed. The key will be fetched
        * from the rights database by looking up the content ID.
        *
        * @param aContentId Content ID to get the key for
        * @return KErrNone if the key was initialized properly
        */                                       
        IMPORT_C TInt InitializeKey( const TDesC8& aContentId );
        
        /**
        * Similar to InitializeKeyL, but sets the key by using a group
        * key as the input.
        *
        * @param aContentId Group ID to get the key for
        * @param aGroupKey Group key
        * @param aEncryptionMethod encryption method
        * @return KErrNone if the key was initialized properly
        */                                       
        IMPORT_C TInt InitializeGroupKey(
            const TDesC8& aGroupId,
            const TDesC8& aGroupKey,
            TEncryptionMethod aMethod );
        
        /**
        * Encrypts data using the session key. The data parameter must
        * have enough space to accomodate for the addition of the paddding
        * if it shall be added (up to 32 bytes more data)
        *
        * @param aIv AES CBC initialization vector
        * @param aData data to be encrypted, encrypted data on return
        * @return KErrNone if the encryption succeeded
        */                                       
        IMPORT_C TInt Encrypt(
            const TDesC8& aIv,
            TPtr8& aData,
            TBool aAddPadding = EFalse );
        
        /** 
         * Decrypts data using the session key. Padding will be removed
         * if requested.
         *
        * @param aIv AES CBC initialization vector
        * @param aData data to be encrypted, encrypted data on return
        * @return KErrNone if the encryption succeeded
         */
        IMPORT_C TInt Decrypt(
            const TDesC8& aIv,
            TPtr8& aData,
            TBool aRemovePadding = EFalse );        
        /**
         * Returns the amount of unique URIs in the database.
         * @since 2.5
         * @return => 0: Amount of ROs,
         *         < 0 : error.
         */
        IMPORT_C TInt Count();

        /**
         * Empties the rights database totally.
         * @since 2.5
         * @return Symbian OS / DRM3 wide error code.
         */
        IMPORT_C TInt DeleteAll();

        /**
         * Consume the right with specific contentID and intent
         * @since 2.5
         * @return Symbian OS / DRM3 wide error code.
         */
		IMPORT_C TInt Consume( 
			const TInt aIntent, 
			const TDesC8& aContentID);

        /**
         * Check if Consume is possible. Does not consume rights.
         * @since 3.0
         * @return KErrNone if Consume is possible
         */
		IMPORT_C TInt CheckConsume( 
			const TInt aIntent, 
			const TDesC8& aContentID);

        /**
         * Calculate the padding amount for a data block
         * @since 3.0
         * @return The padding value
         */
        IMPORT_C TInt CalculatePadding(
            const TDesC8& aLastTwoDataBlocks);

        IMPORT_C TInt ForwardLockURI( HBufC8*& aURI );
        
        /**
        * AddDomainRO
        *
        * Add the xml representation of the domain RO to the database
        *
        * @param    aRoId : the rights object id of the domain ro
        * @param    aXmlData : the xml data of the rights object
        * @return   Symbian OS error code
        */
        IMPORT_C TInt AddDomainRO(
            const TDesC8& aRoId, 
            const TDesC8& aXmlData );        

        /**
        * GetDomainROL
        *
        * Get the xml representation of the domain RO from the database
        *
        * @param    aRoId : the rights object id of the domain ro
        * @return   Functional RO xml representation or Leave with
        *           Symbian OS error code
        */
        IMPORT_C HBufC8* GetDomainROL( const TDesC8& aRoId ); 

        /**
        * GetDomainRoForCidL
        *
        * Get the domain RO for a content ID from the database
        *
        * @param    aContentId the content ID of the domain ro
        * @param    aRoList list of domain rights objects for this ID
        * @return   Symbian OS error code
        */
        IMPORT_C void GetDomainRosForCidL(
            const TDesC8& aContentId,
            RPointerArray<HBufC8>& aRoList );

        /**
        * DeleteDomainRO
        *
        * Delete the xml representation of the domain RO from the database
        *
        * @param    aRoId : the rights object id of the domain ro
        * @return   Symbian OS error code
        */
        IMPORT_C TInt DeleteDomainRO( const TDesC8& aRoId );          
        
        /**
        * IsInReplayCache
        *
        * Checks whether the protected RO identified by aID and aTime 
        * has already been added to Replay Cache.
        *
        * @param aID:       rights ID
        * @param aTime:     RO timestamp
        * @param aInCache:  out-parameter: boolean result
        * @return Symbian OS error code
        */
        IMPORT_C TInt IsInCache( const TDesC8& aID, 
                                 const TTime& aTime, 
                                 TBool& aInCache );

        /**
        * IsInReplayCache
        *
        * Checks whether the protected RO identified by aID
        * has already been added to Replay Cache. 
        *
        * @param aID:       rights ID
        * param aInCache:  out-parameter: boolean result
        * @return Symbian OS error code
        */
        IMPORT_C TInt IsInCache( const TDesC8& aID,
                                 TBool& aInCache );
                                       
        /**
        * AddToReplayCache
        *
        * Adds an entry to Replay Cache. 
        * Note! Does not check whether the entry is already in cache. Use
        * IsInCache() first to determine whether the entry already exists.
        *
        * @param aID:       RO ID
        * @param aTIme:     RO timestamp
        * @return Symbian OS error code.
        */                                       
        IMPORT_C TInt AddToCache( const TDesC8& aID,
                                  const TTime& aTime );
                                  
        /**
        * AddToReplayCache
        *
        * Adds an entry to Replay Cache. 
        * Note! Does not check whether the entry is already in cache. Use
        * IsInCache() first to determine whether the entry already exists.
        *
        * @param aID:       RO ID
        * @return Symbian OS error code.
        */                                       
        IMPORT_C TInt AddToCache( const TDesC8& aID );
        
        /**
        * DeleteExpiredPermissions
        *
        * Deletes expired permissions from the rights database
        * Note this function will possibly take a lot of time to complete
        * and if the drm time is not available it does not touch any time
        * based rights
        *
        * @param aStatus:	asynchronous request status
        * @return 	none        
        */
        IMPORT_C void DeleteExpiredPermissions( TRequestStatus& aStatus );
        
        /**
         * SetEstimatedArrival
         *
         * Sets the time in which the rights object should arrive
         *
         * @since   3.0
         * @param   aContentID : content URI
         * @param   aDeltaSeconds : time in seconds to wait for rights
         * @return  Symbian OS error code if any.
         */
        IMPORT_C TInt SetEstimatedArrival( const TDesC8& aContentID,
                                   TTimeIntervalSeconds aDeltaSeconds );
                                  
        /**
         * GetEstimatedArrival
         *
         * Gets the time in which the rights object should arrive
         *
         * @since   3.0
         * @param   aContentID : content URI
         * @param   aDeltaSeconds : time in seconds to wait for rights
         *                          -1 if the content should have arrived
         * @return  Symbian OS error code if any.
         *          KErrNotFound if the content is not in the list       
         */
        IMPORT_C TInt GetEstimatedArrival( const TDesC8& aContentID,
                                 TTimeIntervalSeconds& aDeltaSeconds );  
                                 
        
        /**
        * SetName
        *
        * Associates a human-readable name to given content-ID.
        *
        * @since    3.0
        * @param    aContentID: Content URI.
        * @param    aName: New name.
        * @return   Symbian OS error code if any
        */
        IMPORT_C TInt SetName( const TDesC8& aContentID,
                               const TDesC& aName );
        
        /**
        * GetName
        * 
        * Retrieves the human-readable name associated to given content-ID
        * 
        * @since    3.0
        * @param    aContentID: Content URI.
        * @param    aName:      Out-parameter: The name associated to content.
        * @return   Symbian OS error code if any. KErrNotFound in case the
        *           content URI is not in the database.
        */
        IMPORT_C TInt GetName( const TDesC8& aContentID,
                               HBufC*& aName );
        
        /**
        * Cancel
        * 
        * Cancel any asynchronous operation.
        *
        * @since    3.0
        */
        IMPORT_C void Cancel();
        
        /**
        * GetUdtData
        * 
        * Retrieves the udt data from the server
        * 
        * @since    3.0
        * @param    aUdtData: the udt data
        * @return   Symbian OS error code if any. KErrNotFound in case the
        *           content URI is not in the database.
        */
        IMPORT_C TInt GetUdtData( TDes8& aUdtData );        

        /**
        * InitializeUdt
        * 
        * Initializes the user data transfer
        * 
        * @since    3.0
        * @param    aKey : Encryption key of the udt file encrypted with the
        *                  device public key
        * @return   Symbian OS error code if any. KErrNotFound in case the
        *           content URI is not in the database.
        */
        IMPORT_C TInt InitiateUdt( const TDesC8& aKey );


        /**
         * Initializes the export all orphaned content ID's.
         * When the request completes, you can ask for the
         * contents with the ExportOrphanedContentIdList functions
         *
         * @since   3.0
         * @param   aPerformScan : ETrue if file system scan needs to be
         *                         performed
         *                         EFalse if not
         * @param   aStatus : Request status for the asynchronous call           
         * @return  None
         */
        IMPORT_C void InitOrphanedContentIdList( TBool aPerformScan,
                                                 TRequestStatus& aStatus );

        
        /**
         * Exports all orphaned content ID's from the database to a file.
         * @since   3.0
         * @param   aFileName Out-parameter: contains the name of the file
         *                  if the method completes with KErrNone. The 
         *                  descriptor parameter is assumed to be large 
         *                  enough. The caller is responsible of  deleting 
         *                  the file afterwards.
         * @param   aPerformScan : ETrue if file system scan needs to be
         *                         performed
         *                         EFalse if not       
         * @return  Error code. KErrNone if successful operation.
         */
        IMPORT_C TInt ExportOrphanedContentIdList( TDes& aFileName );        
                 
        /**
         * Exports all orphaned content ID's from the database
         * in an array. The caller is responsible of destroying the array
         * afterwards.
         * @since   3.0
         * @param   aContentIdList : An out-parameter: contains the Content-IDs 
         *                           after successful return.
         * @param   aPerformScan : ETrue if file system scan needs to be
         *                         performed
         *                         EFalse if not           
         * @return  A Symbian OS error code or DRM error code. KErrNone in
         *          successful operation.
         */
        IMPORT_C TInt ExportOrphanedContentIdList( 
                        RPointerArray<HBufC8>& aContentIdList); 

        IMPORT_C TInt EncodeRightsIssuerField( const TDesC8& aOldValue,
                                               HBufC8*& aNewValue );
                                               
        IMPORT_C TInt DecodeRightsIssuerField( const TDesC8& aEncoded,
                                               HBufC8*& aDecoded );
                                               
        /**
         * Sets the authentication seed data for a content ID
         * @since   3.0
         * @param   aContentID Content ID
         * @param   aSeed Authentication seed
         * @return  Error code. KErrNone if successful operation.
         */
        IMPORT_C TInt SetAuthenticationSeed( const TDesC8& aContentId,
                                             const TDesC8& aSeed );
                                               
        /**
         * GSets the authentication seed data for a content ID
         * @since   3.0
         * @param   aContentID Content ID
         * @param   aSeed Authentication seed
         * @return  Error code. KErrNone if successful operation.
         */
        IMPORT_C TInt GetAuthenticationSeed( const TDesC8& aContentId,
                                             TDes8& aSeed );
        /**
         * Integrity protection for protected ROs. Calculates HMAC value
         * usign aSignedInfoElement and MAC key. MAC key is unwrapped
         * in AddRecord method. So AddRecord must be called successfully
         * before this method can be called.
         *
         * @since   3.0
         * @param   aSignedInfoElement Complete XML ro element <roap:ro>
         * @param   aMacValue  MAC value from protectedRO element
         * @return  Error code. KErrNone if successful operation.
                    KErrRightsServerMacFailed, if MAC validation fails 
         */                               
        IMPORT_C TInt VerifyMacL( const TDesC8& aSignedInfoElement,
                                  const TDesC8& aMacValue ) const;


        /**
         * Retrieve the individuals from the Rights Server so proper
         * comparisons of rights object validity can be performed outside
         * of the rights server
         *
         * @since   3.1
         * @param   aIndividuals : Pointer array of the individuals supported
         * @return  Error code with server side error. 
         *          KErrNone if successful operation.
         */
        IMPORT_C TInt GetSupportedIndividualsL(
                                    RPointerArray<HBufC8>& aIndividuals) const;
                                 
        /**
         * Stop watching the DCF repository server
         *
         * @since   3.1
         */
        IMPORT_C void StopWatching() const;
        
        
        /**
         * Unwraps MAC and REK keys that are protected using device public key
         * or domain key. If the keys are wrapped with device public key aDomainId
         * parameter must be set to zero length.
         *
         * @since   3.1
         * @param   aMacAndRek : Public key protected MAC and REK keys
         * @param   aTransportScheme: Used key wrapping scheme (e.g. OMA or CMLA)
         * @param   aRightsIssuerId: Defines the RI that has wrapped the keys
         * @param   aDomainId: Defines the domain key that used for wrapping the keys.
         *          If a device public key is used, the length of aDomainId must be zero
         * @return  Error code with server side error. 
         *          KErrNone if successful operation.
         */
        IMPORT_C TInt UnwrapMacAndRek( const TDesC8& aMacAndRek,
                                       TKeyTransportScheme aTransportScheme,
                                       const TDesC8& aRightsIssuerId,
                                       const TDesC8& aDomainId ) const;
                                       
        /**
        * Fills the input buffer with random data. The length of the aRandomData must
        * be set to the correct value.
        * 
        * @since   3.1
        * @param   aRandomData : Out-parameter that will be filled with random data
        */                              
        IMPORT_C void GetRandomDataL( TDes8& aRandomData ) const;
        
        /**
        * Returns the metering data for the requested rights-issuer id
        * 
        * @since   3.2
        * @param   aRiId : Rights-Issuer Id for which the metering data is acquired
        * @return  MeteringData which includes the cipher data and all tags inside
        *          meteringReport -element       
        */    
                      
        IMPORT_C HBufC8* GetMeteringDataL( const TDesC8& aRiId );

        /**
        * Deletes the metering data of the requested rights-issuer id
        * 
        * @since   3.2
        * @param   aRiId : Rights-Issuer Id for which the metering data is deleted
        * @return  Error code with server side error. 
        *          KErrNone if successful operation.        
        */    
        
        IMPORT_C TInt DeleteMeteringDataL( const TDesC8& aRiId );
                    

    public: // Functions from base classes
        
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:
        
        /**
         * Convert internally used temporary file into list.
         * @since S60Rel2.5
         * @param aFs       Open file server session.
         * @param aFileName Source file name
         * @param aList     Destination list.
         */ 
        void FileToListL( RFs& aFs,
                          const TDesC& aFileName,
                          RPointerArray< CDRMPermission >& aList );
        /**
         * Converts a file of URIs into an array.
         * @since S60Rel2.5
         * @param aFs      Open file server session.
         * @param aFile    Input file.
         * @param aList    Output list.
         */
        void URIFileToArrayL( RFs& aFs,
                              const TDesC& aFile,
                              RPointerArray< HBufC8 >& aList );
        
        
        // Prohibit copy constructor if not deriving from CBase.
        RDRMRightsClient( const RDRMRightsClient& );
        // Prohibit assigment operator if not deriving from CBase.
        RDRMRightsClient& operator=( const RDRMRightsClient& );
        
    public:     // Data
        
    protected:  // Data
        
    private:    // Data
        TAny* iPtr; // Reserved for future use
        
    public:     // Friend classes
        
    protected:  // Friend classes
        
    private:    // Friend classes
    };
    
#endif      // RDRMRIGHTSCLIENT_H   

// End of File
