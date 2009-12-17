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
* Description:  Server Session implementation
*
*/



#ifndef CDCFREPSRVSES_H
#define CDCFREPSRVSES_H

//  INCLUDES
#include <e32base.h>


// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CPair;

// CLASS DECLARATION


/**
*  Server Session implementation.
*  DCF Repository
*
*  @lib 
*  @since Series 60 3.0 
*/
class CDcfRepSrvSes : public CSession2
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CDcfRepSrvSes* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CDcfRepSrvSes();

    public: // New functions
    public: // Functions from base classes

        /**
        * From CSession2: Handles the service request event.
        * @since S60Rel3.0
        * @param aMessage The message related to the event.
        * @exception Method leaves with appropriate exception value
        *            if any errors occured.
        */
        void ServiceL( const RMessage2& aMessage );
        
    protected:  // New functions
    protected:  // Functions from base classes
    private:

        /**
        * C++ default constructor.
        */
        CDcfRepSrvSes();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /** 
        * AddFileL() will add a file info into database
        * @since S60Rel3.0
        * @param aMessage The message associated to the event.
        * @Leave with KErrNotSupported if file is not protected file
        */        
        void AddFileL( const RMessage2& aMessage );        

        /** 
        * ServiceL() runs DispatchL() under TRAP harness, so
        * all errors can be catched properly. DispatchL() then calls
        * appropriate private method depending on the request.
        * @since S60Rel3.0
        * @param aMessage The message associated to the event.
        */
        void DispatchL( const RMessage2& aMessage );
        
        /** 
        * FullScanL() will triger the file scan for all DCFs
        * @since S60Rel3.0
        * @param aMessage The message associated to the event.
        */        
        void FullScanL( const RMessage2& aMessage );
        
        
        /** 
        * ScanStartL() will initialize scanning dcfs for a list of content ID 
        * @since S60Rel3.0
        * @param aMessage The message associated to the event.
        */        
        void ScanStartL( const RMessage2& aMessage );

        /** 
        * ScanAddL() will add content ID into CID list for scanning
        * @since S60Rel3.0
        * @param aMessage The message associated to the event.
        */        
        void ScanAddL( const RMessage2& aMessage );
        
        /** 
        * ScanEndL() will finnally triger scanning dcfs for a list of content ID 
        * @since S60Rel3.0
        * @param aMessage The message associated to the event.
        */        
        void ScanEndL( const RMessage2& aMessage );
        
        
        /** 
        * SetTtidStartL() will initialize Ttid reset for a list of content ID 
        * @since S60Rel3.0
        * @param aMessage The message associated to the event.
        */        
        void SetTtidStartL( const RMessage2& aMessage );

        /** 
        * SetTtidAddL() will add a pair of content ID and transaction ID (ttid) into list for reset
        * @since S60Rel3.0
        * @param aMessage The message associated to the event.
        */        
        void SetTtidAddL( const RMessage2& aMessage );
        
        /** 
        * SetTtidEndL() will finnally triger scanning dcfs in order to reset them
        * @since S60Rel3.0
        * @param aMessage The message associated to the event.
        */        
        void SetTtidEndL( const RMessage2& aMessage );

        /** 
        * NotifyIdleL() will notify if server becomes idle
        * @since S60Rel3.0
        * @param aMessage The message associated to the event.
        */            
        void NotifyIdleL( const RMessage2& aMessage );
        
        /** 
        * Stop watching the RightsServer
        * @since S60Rel3.1
        * @param aMessage The message associated to the event.
        */            
        void StopWatchingL( const RMessage2& aMessage );
        
        // Prohibit copy constructor if not deriving from CBase.
        CDcfRepSrvSes( const CDcfRepSrvSes& );
        // Prohibit assigment operator if not deriving from CBase.
        CDcfRepSrvSes& operator=( const CDcfRepSrvSes& );

    public:     // Data
    protected:  // Data
    private:    // Data
        TInt iPreviousAction;
        RPointerArray<HBufC8>* iCidList;
        RPointerArray<CPair>* iPairList;
        RMessage2* iMessage;

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes
    };

#endif      // CDCFREPSRVSES_H   
            
// End of File
