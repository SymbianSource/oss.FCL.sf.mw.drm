/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef DRMRIGHTSCLEANER_H
#define DRMRIGHTSCLEANER_H

// INCLUDES

#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FORWARD DECLARATIONS
class RFs;
class CDir;
class CDRMRightsDB;
                                                         

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
*  CDRMRightsCleaner implements expired rights cleanup
*  for drm rights database
*
*  @lib RightsServer.exe
*  @since 3.0
*/

NONSHARABLE_CLASS( CDRMRightsCleaner ) : public CActive
    {
    public: // Constructors and destructor
        
        /**
        * NewL
        *
        * Creates an instance of the CDRMRightCleaner class and returns a pointer
        * to it
        *
        * @since    3.0
        * @param    aFs : Open file server session
        * @param    aDatabase : CDRMRightsDB object
        * @param    aStatus : The request status to complete when the operation is 
        *                     fully done
        * @param    aDatabasePath : full pathname of the database path
        * @param    aTime : Time to check expiration against 
        *
        * @return   Functional CDRMRightsCleaner object, Function leaves if an error
        *           occurs.
        */
        static CDRMRightsCleaner* NewL( RFs& aFs,
                                        CDRMRightsDB* aDatabase,
                                        TRequestStatus& aStatus,
                                        const TDesC& aDatabasePath,
                                        const TTime& aTime );
          
        /**
        * Destructor
        */
        virtual ~CDRMRightsCleaner();

    public: // New functions    

        /**
        * ExecuteCleanupLD
        * 
        * Delete expired permissions. The object is deleted when
        * the function completes
        *
        * @since    3.0
        * @return   None
        *
        */      
        void ExecuteCleanupLD();
        
        void DoCleanup();
         
    protected:
        /**
        * From CActive: RunL.
        */
        void RunL();


        /**
        * From CActive: DoCancel performs cancel
        */        
        void DoCancel();
      
        /**
        * From CActive: RunError checks the errors from RunL.
        */
        TInt RunError( TInt aError );                        
                        
    private:
        /**
        * Default Constructor - First phase. Prevented.
        */
        CDRMRightsCleaner(); 
    
        /**
        * Default Constructor - First phase.
        */
        CDRMRightsCleaner( RFs& aFs,
                           CDRMRightsDB* aDatabase,
                           TRequestStatus& aStatus,
                           const TDesC& aDatabasePath,
                           const TTime& aTime ); 
        
        /**
        * ConstructL
        *
        * Second phase constructor
        *
        * @since  3.0
        * @return Leaves if an error occurs
        */  
        void ConstructL();  

        /**
        * Assignment operator - Prevented
        */
        CDRMRightsCleaner& operator =( const CDRMRightsCleaner& );    
    
        /**
        * Copy constructor - Prevented
        */
        CDRMRightsCleaner( const CDRMRightsCleaner& );                
     
    private:
        // fileserver session
        RFs& iFileServer;
    
        // rights database file, fileserver subsession
        CDRMRightsDB* iRightsDb;
    
        // request status to be set complete
        TRequestStatus& iOperationStatus;
        
        // Path of the rights database
        TFileName iDatabasePath;
        
        // Expiration time to check against
        TTime iExpirationTime;
        
        // Looping calculators
        CDir* iCurrentDirectory;
        
        // directory index:
        TInt iDirIndex;
        
        // FileIndex
        TInt iCurrentFile;
        
        // Cancelling call:
        TInt iCancel;
    };

#endif      // DRMRIGHTSCLEANER_H   
            
// End of File
