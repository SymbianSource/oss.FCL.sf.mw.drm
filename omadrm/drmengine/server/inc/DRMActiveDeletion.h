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
* Description:  Active object handling "Delete Expired Permission"
*
*/


#ifndef DRMACTIVEDELETION_H
#define DRMACTIVEDELETION_H

// INCLUDES

#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FORWARD DECLARATIONS
class CDRMRightsDB;
class CDRMDbSession;                                                         

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
*  CDRMActiveDeletion implements expired rights cleanup callback
*  for drm rights database
*
*  @lib RightsServer.exe
*  @since 3.0
*/
NONSHARABLE_CLASS( CDRMActiveDeletion ) : public CActive
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
        * @return   Functional CDRMActiveDeletion object, Function leaves if an error
        *           occurs.
        */
        static CDRMActiveDeletion* NewLC( const RMessagePtr2& aMessage,
                                          CDRMDbSession& aSession );
          
        /**
        * Destructor
        */
        virtual ~CDRMActiveDeletion();

    public: // New functions    

        /**
        * ActivateL
        * 
        * Activates the object by adding it to scheduler etc.
        *
        * @since    3.0
        * @return   None
        *
        */      
        void ActivateL( const TTime& aSecureTime,
                        CDRMRightsDB& aDb );
         
    protected:
    
        /**
        * Default Constructor - First phase.
        */
        CDRMActiveDeletion( const RMessagePtr2& aMessage,
                            CDRMDbSession& aSession ); 
        
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
        // void RunError();                        
                        
    private:
        /**
        * Default Constructor - First phase. Prevented.
        */
        CDRMActiveDeletion(); 
    
        /**
        * Assignment operator - Prevented
        */
        CDRMActiveDeletion& operator =( const CDRMActiveDeletion& );    
    
        /**
        * Copy constructor - Prevented
        */
        CDRMActiveDeletion( const CDRMActiveDeletion& );                
     
    private:
        // The message.
        const RMessagePtr2& iMessage;
        CDRMDbSession& iSession;
    
        // The instane doing the deletion.
        CActive* iActiveOperation;
    };

#endif      // DRMACTIVEDELETION_H   
            
// End of File
