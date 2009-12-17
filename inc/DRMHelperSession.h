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
* Description:  the server session implementation of DRM Helper
*
*/


#ifndef DRMHELPERSESSION_H
#define DRMHELPERSESSION_H

// INCLUDES
#include <e32base.h>
// DATA TYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION




/**
*  Server session of DRM Helper
*  This class is the server side instance of C/S communications of a certain
*  session.
*
*  @lib DRMHelperServer.exe
*  @since S60Rel2.6
*/
class CDRMHelperSession : public CSession2
{
    public:  // Constructors and destructor
   
        /**
        * Two-phased constructor.
        * @since S60Rel2.6
        * @param aClient Associated client side thread.
        * @return New session instance.
        */
        static CDRMHelperSession* NewL();
   
        /**
        * Destructor.
        * @since S60Rel2.6
        */
        virtual ~CDRMHelperSession();
   
    public: // New functions
   
    public: // Functions from base classes
   
        /**
        * From CSession: Handles the service request event.
        * @since S60Rel2.6
        * @param aMessage The message related to the event.
        * @exception Method leaves with appropriate exception value
        *            if any errors occured.
        */
        void ServiceL( const RMessage2& aMessage );
   
    protected:  // New functions
   
    protected:  // Functions from base classes
   
    private:
   
        /**
        * C++ constructor.
        * @since S60Rel2.6
        * @param aClient Client thread.
        * 
        */
        CDRMHelperSession();
   
  
        /** 
        * ServiceL() runs DispatchL() under TRAP harness, so
        * all errors can be catched properly. DispatchL() then calls
        * appropriate private method depending on the request.
        * @since S60Rel2.6
        * @param aMessage The message associated to the event.
        */
        void DispatchL( const RMessage2& aMessage );
   
        /** 
        * RegisterL register the automated content into server 
        * @since S60Rel2.6
        * @param aMessage The message associated to the event.
        */
        void RegisterL( const RMessage2& aMessage );


        /** 
        * IsRegisteredL check if the automated content is registered into server 
        * @since S60Rel2.6
        * @param aMessage The message associated to the event.
        */
        void IsRegisteredL( const RMessage2& aMessage );


        /** 
        * RemoveL remove the automated content from server 
        * @since S60Rel2.6
        * @param aMessage The message associated to the event.
        */
        void RemoveL( const RMessage2& aMessage );


        /** 
        * IndicateIdleL indicate the ilde time comes
        * @since S60Rel2.6
        * @param aMessage The message associated to the event.
        */
        void IndicateIdleL( const RMessage2& aMessage );

        // Prohibit copy constructor.
        CDRMHelperSession( const CDRMHelperSession& );
        // Prohibit assigment operator.
        CDRMHelperSession& operator=( const CDRMHelperSession& );
   
    public:     // Data
    protected:  // Data
    private:    // Data
    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes
   
};



#endif      // DRMHELPERSESSION_H   
            
// End of File
