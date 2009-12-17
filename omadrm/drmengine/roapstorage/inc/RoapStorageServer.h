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
* Description:  DRM3 Notifier manages all notifications out of the system.
*
*/



#ifndef CRoapStorageServer_H
#define CRoapStorageServer_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "RoapStorageSession.h"

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
TInt StartupRoapStorage( TAny* );
 
// FORWARD DECLARATIONS
// CLASS DECLARATION
class CDRMContextDB;

/**
*  This class implements the Roap Storage Server functionality.
*
*  @lib RightsServer.exe
*  @since 3.0
*/
NONSHARABLE_CLASS( CRoapStorageServer ) : public CServer2
    {
    public:  // Constructors and destructor
        
        /**
         * Two-phased constructor.
         * @since 3.0
         * @return Pointer to newly created server instance.
         */
        static CRoapStorageServer* NewL();
        
        /**
         * Destructor.
         * @since 3.0
         */
        ~CRoapStorageServer();
        
         /**
         * Returns CDRMContextDB handle 
         * @since 3.0
         */        
        CDRMContextDB* ContextDB();
        
    public: // Functions from base classes

    protected:  // New functions
    
        /**
         * Import PKI keys into the private directory
         * @since 3.0
         */
        void ImportKeysL();
        
    protected:  // Functions from base classes
        //class CServer
        /**
         * RunError.
         * @since 3.0
         * @param aError Error code from RunL function.
         * @return An error is returned if RunError() cannot process the
         * error.
         */
        TInt RunError( TInt aError );
        
    private:
        /**
         * C++ default constructor.
         * @since 3.0
         */
        CRoapStorageServer();
        
        /**
         * By default Symbian 2nd phase constructor is private.
         * @since 3.0
         */
        void ConstructL();
        
        
    private: // Functions from base classes
        // Class CServer
        /** 
         * NewSessionL
         * @since 3.0
         */
        CSession2* NewSessionL( const TVersion& aVersion,
                                const RMessage2& aMessage) const;
        
    public:     // Data
        
    protected:  // Data
        
    private:    // Data
    
        RFs iRFs;
        CDRMContextDB* iRoapStorage;
        
    public:     // Friend classes
        
    protected:  // Friend classes
        
    private:    // Friend classes
        
};

#endif      // CDRMRIGHTSSERVER_H   

// End of File
