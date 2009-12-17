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


#ifndef DRMACTIVEOPERATION_H
#define DRMACTIVEOPERATION_H

// INCLUDES

#include <e32base.h>
#include <s32file.h>

// CONSTANTS

const TInt KOperationDeleteExpired  = 1;
const TInt KOperationExportObsolete = 2;

// MACROS

// DATA TYPES

// FORWARD DECLARATIONS
class CDRMRightsDB;
class CDRMDbSession;
class RFs;
                                                     

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
*  CDRMActiveOperation implements active objects for
*  for drm rights server
*
*  @lib RightsServer.exe
*  @since 3.0
*/
NONSHARABLE_CLASS( CDRMActiveOperation ) : public CActive
    {
    public: // Typedefs
    typedef enum 
        {
        EOperationNone = 0,
        EOperationDeleteExpired  = 1,
        EOperationExportObsolete = 2    
        } TOperationType;
    
    public: // Constructors and destructor
        
        /**
        * NewLC
        *
        * Creates an instance of CDRMActiveOperation class and returns a pointer to it
        * also leaves the object to the cleanup stack
        *
        * @since    3.0
        * @param    aMessage: the asynchronous message
        * @param    aSession : Session this operation is connected to
        * @param    aOperation : The operation to perform
        * @return   Functional CDRMActiveOperation object, Function leaves if an error
        *           occurs.
        */
        static CDRMActiveOperation* NewLC( const RMessagePtr2& aMessage,
                                          CDRMDbSession& aSession,
                                          TOperationType aOperation );
          
        /**
        * Destructor
        */
        virtual ~CDRMActiveOperation();

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
        void ActivateL( CDRMRightsDB& aDb,
                        const TTime& aSecureTime );


        /**
        * ActivateL
        * 
        * Activates the object by adding it to scheduler etc.
        *
        * @since    3.0
        * @return   None
        *
        */      
        void ActivateL( CDRMRightsDB& aDb,
                        RFs& aFileServer,
                        const TDesC& aTempPath, 
                        const TBool aPerformScan = EFalse );
         
         
        void Remove(); 
    protected:
    
        /**
        * Default Constructor - First phase.
        */
        CDRMActiveOperation( const RMessagePtr2& aMessage,
                            CDRMDbSession& aSession,
                            TOperationType aOperation ); 
        
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
        CDRMActiveOperation(); 
    
        /**
        * Assignment operator - Prevented
        */
        CDRMActiveOperation& operator =( const CDRMActiveOperation& );    
    
        /**
        * Copy constructor - Prevented
        */
        CDRMActiveOperation( const CDRMActiveOperation& );                
     
    private:
        // The message.
        const RMessagePtr2& iMessage;
        CDRMDbSession& iSession;
        
        // The operation to perform
        TOperationType iOperation;
        
        // The instance doing the operation.
        CActive* iActiveOperation;
        
        // File name of the operation
        TFileName* iFileName;
        RFileWriteStream iFileStream;
        RFs* iFileServer;
    };

#endif      // DRMACTIVEOPERATION_H   
            
// End of File
