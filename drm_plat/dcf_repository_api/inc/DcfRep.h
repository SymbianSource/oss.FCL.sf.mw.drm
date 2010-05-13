/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Client side implementation
*
*/



#ifndef CDCFREP_H
#define CDCFREP_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <d32dbms.h>
#include "DcfRepCli.h"
// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CDcfEntry;
// CLASS DECLARATION

/**
*  CDcfRep
*  Client of DCF Repository
*
*  @lib ?library
*  @since Series 60 3.0
*/
class CDcfRep : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CDcfRep* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CDcfRep();

    public: // New functions
        
        /**
        * UpdateL 
        * the function will add the entry into database. 
        * in 3.0 file info will be added 
        * @since Series 60 3.0
        * @param aEntry protected file entry to database, either filename or content ID field shall be filled,
        *           if filename is filled, the file is added into DCF DB and also rights DB, domain RO is added into the file
                    if content ID is filled, domain RO are added into all files(registered in DCF DB) that are linked with this content ID 
        *           if both are filled, then content ID is ignored.
                    if none is filled, then KErrArgument is returned
        *   server will obtain other info from the file or DCF DB.
        * @Leave with KErrNotSupported if file is not protected file 
        */
        IMPORT_C void UpdateL( const CDcfEntry* aEntry);
        

        /**
        * GetFileHandleL 
        * The function get file info from database and return read-only file handle back
        * @since Series 60 3.0
        * @param aCid a content ID
        * @param aFile a file handle
        * @param aFs file server session that is connected with server.
        * @return function will leave with KErrNotFound if the content ID is not found in database or 
        *   info is out of date, and will leave with KErrAccessDenied if caller doesnt have access to
        *   the file.
        */
        IMPORT_C void GetFileHandleL( const TDesC8& aCid , RFile& aFile , RFs& aFs );        

        /**
        * NextL 
        * After a list is ordered, the function will return entries as iteration.
        * Caller must call OrderListL before calling this function.
        * @since Series 60 3.0
        * @return DCF entry from database, NULL if no entry left.
        */
        IMPORT_C CDcfEntry* NextL();   
        
        /**
        * GetTtidL 
        * The function will go through database to get transaction IDs for every specific CID
        * @since Series 60 3.0
        * @param aList contain content IDs and transaction ID will be filled afterwards
            TPair.iCid must be filled by user. TPair.iTtid must be NULL
        * @return TPair.iTtid will be filled if corresponding info is found in database.
            caller owns TPair.iCid and TPair.iTtid. 
        */
        IMPORT_C void GetTtidL( RArray<TPair>& aList );
        
        
        /**
        * OrderListL 
        * Order all entries from DB, NextL will be used after this function call in order to get entries.
        * @since Series 60 3.0
        * @Leave with KErrNotFound if there is no entries
        */
        IMPORT_C void OrderListL(void);
         
        /**
        * OrderListL 
        * Order all entries with specific CID from DB, 
        * if the CID cannot be found as a CID then DB will search for it as a Group ID 
        * NextL will be used after this function call in order to get entries.
        * @since Series 60 3.0
        * @param aCid a content ID
        * @Leave with KErrNotFound if there is no entries for the CID
        */
        IMPORT_C void OrderListL(const TDesC8& aCid); 
        

        /**
        * RemoveDcfFromDbL 
        *
        * the function will remove all the entries that have the filename in database.
        * @since Series 60 3.0
        * @param aFileName file name of Dcf to be removed
        * @return 
        */
        IMPORT_C void RemoveDcfFromDbL( const TDesC& aFileName );

        /**
        * RefreshDcf 
        * the function will refresh the DCF list for both OMA 1 and OMA 2(.odf) DCFs
        * and save it into DB. 
        * Note. if CDcfRep is destructed, the status will not be completed afterwards even server becomes idle.
        * @since Series 60 3.0
        * @param aStatus a status for this request
        * @return 
        */
        IMPORT_C void RefreshDcf( TRequestStatus& aStatus );


        /**
        * RefreshDcf 
        * the function will refresh the DCF list for both OMA 1 and OMA 2(.odf) DCFs
        * and save it into DB. the function will complete when every single Cid has found at least one 
        * corresponding DCFs in the file system
        * Note. if CDcfRep is destructed, the status will not be completed afterwards even server becomes idle.
        * @since Series 60 3.0
        * @param aList contain content IDs which info need to be refreshed 
        * @param aStatus a status for this request
        * @return 
        */
        IMPORT_C void RefreshDcf( RPointerArray<HBufC8>& aList , TRequestStatus& aStatus );
       
        /**
        * SetTtid 
        * the function will scan whole file system, whenever a .odf with a specifed CID is found, server
        * will reset the TTID for the .odf with the value in TPair.iTtid and also update info in database. 
        * Note. if CDcfRep is destructed, the status will not be completed afterwards even server becomes idle.
        * @since Series 60 3.0
        * @param aList a list of CID and TTID pairs, Caller owns TPair.iCid and TPair.iTtid.
        *        CID and TTID must be filled by caller, no default value is given. 
        * @param aStatus a status for this request
        * @param aRefresh ETrue by default, EFalse is not supported in this version
        * @return 
        */
        IMPORT_C void SetTtid( 
            RArray<TPair>& aList,
            TRequestStatus& aStatus, 
            TBool aRefresh = ETrue);   

        /**
        * NotifyServerIdle 
        * the function will complete the status when DCF Repository server becomes idle
        * if the server is in ilde status right now, then the status will get completed right now.
        * Note. if CDcfRep is destructed, the status will not be completed afterwards even server becomes idle.
        * @since Series 60 3.0
        * @param aStatus a status for this request
        * @return 
        */            
        IMPORT_C void NotifyServerIdle( TRequestStatus& aStatus );       
                
    public: // Functions from base classes
    protected:  // New functions
    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */

    private:

        /**
        * C++ default constructor.
        */
        CDcfRep();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Open database
        */
        void OpenDatabaseL(); 

        // Prohibit copy constructor if not deriving from CBase.
        CDcfRep( const CDcfRep& );
        // Prohibit assigment operator if not deriving from CBase.
        CDcfRep& operator=( const CDcfRep& );
        
        void GetTtidL(const TDesC8& aCid , HBufC8*& aTtid);
        
        TInt SetName(const TDesC8& aCid , const TDesC16& aName);
        
        void GetFilenameL(const TDesC8& aCid , RPointerArray<HBufC>& aList);
        
        void GetCidL(const TDesC16& aName , RPointerArray<HBufC8>& aList);
  
        void DoGetFileHandleL( const TDesC8& aCid , RFile& aFile , RFs& aFs ); 
      
    public:     // Data
        RDcfRepCli iClient;  
        RDbs iDbs;
        RDbNamedDatabase iDb;
        RDbView iView;
    
    protected:  // Data
    private:    // Data
    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes
    };

#endif      // CDCFREP_H   
            
// End of File
