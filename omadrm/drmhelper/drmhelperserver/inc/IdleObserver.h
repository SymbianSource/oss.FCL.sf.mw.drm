/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  observe system idle event
*
*/



#ifndef IDLEOBSERVER_H
#define IDLEOBSERVER_H

//  INCLUDES
#include <e32property.h>
// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CDRMHelperServer;


// CLASS DECLARATION

/**
*  End time based rights storage class
*  
*
*  @lib DRMHelperServer.lib
*  @since Series60 2.6
*/
class CIdleObserver : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CIdleObserver* NewL(CDRMHelperServer& aServer);
        
        /**
        * Destructor.
        */
        virtual ~CIdleObserver();


    public: // New functions
        /**
        * StartL starts the system agent and listen to the event
        * @since Series60 2.6
        * @param
        * @return 
        */
        void StartL();
    public: // Functions from base classes
    protected:  // New functions
        
        /**
        * From CActive
        * @since Series60 2.6
        */
        void RunL();

        /**
        * From CActive
        * @since Series60 2.6
        */
        void DoCancel();


    protected:  // Functions from base classes
    private:

        /**
        * C++ default constructor.
        */
        CIdleObserver( CDRMHelperServer& aServer );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        CIdleObserver( const CIdleObserver& );
        // Prohibit assigment operator if not deriving from CBase.
        CIdleObserver& operator=( const CIdleObserver& );
        

        

    public:     // Data
    protected:  // Data
    private:    // Data
        // helper server reference
        CDRMHelperServer& iServer;
        // For getting notify on idle screen available event.
        RProperty iProperty;
    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes
        

    };

#endif      // IDLEOBSERVER_H   
            
// End of File
