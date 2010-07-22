/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DRM Message storage handles all the messages and stores them
*                when needed
*
*/



#ifndef CDRMMESSAGESTORAGE_H
#define CDRMMESSAGESTORAGE_H

//  INCLUDES
#include <e32base.h>
#include <DRMEvent.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CDRMNotifierSession;
class CLogFile;

// CLASS DECLARATION

/**
*  This class implements the DRM3 Message Storage
*
*  @lib RightsServer.exe
*  @since S60Rel2.6
*/
NONSHARABLE_CLASS( CDRMMessageStorage ) : public CBase
    {
    public: // User defined types
        struct TMessageData
            {
            TInt iRefCount;
            TDRMEventType iEventType;
            HBufC8* iData;
            TUint8* iMessageData;
            };

    public:  // Constructors and destructor
        
        /**
         * Two-phased constructor.
         * @since S60Rel2.6
         * @return Pointer to newly created server instance.
         */
        static CDRMMessageStorage* NewL();
        
        /**
         * Destructor.
         * @since S60Rel2.6
         */
        ~CDRMMessageStorage();
        
    public: // New functions
        /**
        *  AddSession
        *
        *  Adds a session to the session list
        *
        *  @since S60Rel2.6
        *  @param : aSession - The notifier session to recieve callbacks
        *  @param : aMessage - Message related to the reques
        *  @return None
        */
        TInt AddSession( CDRMNotifierSession* aSession );

        /**
        *  UpdateMessage
        *
        *  Updates a message in the message list, if the reference
        *  counter is reduced to 0 it removes it from the list
        *
        *  @since S60Rel2.6
        *  @param : aMessage - message to be updated
        *  @return None
        */
        void UpdateMessage( struct TMessageData* aMessage );

        /**
        *  GetEventObjectLC
        *  
        *  Creates an event object, adds it to the cleanup stack and
        *  returns a pointer to it
        *
        *  @since S60Rel2.6
        *  @param aEventType the type of event object needed
        *  @return pointer to the proper event object
        */
        MDRMEvent* GetEventObjectLC( TDRMEventType aEventType );

        /**
        *  NotifyL
        *
        *  Calls the SendNotificationL function from all registered sessions 
        *
        *  @since S60Rel2.6
        *  @param : aEventType - The event type of the message
        *  @param : aMessage - Message related to the request
        *  @return None, Leaves with symbian OS error code if an error occurs
        */
        void NotifyL( TDRMEventType& aEventType, const RMessage2& aMessage );  

        /**
        *  CancelL
        *
        *  Removes the object(s) related to the given session from the lists 
        *
        *  @since S60Rel2.6
        *  @param : aSession - Session connected to a client
        *  @return None, Leaves with symbian OS error code if an error occurs
        */
        void CancelL( CDRMNotifierSession* aSession );

    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:
        /**
         * C++ default constructor.
         * @since S60Rel2.6
         */
        CDRMMessageStorage();
        
        /**
         * By default Symbian 2nd phase constructor is private.
         * @since S60Rel2.6
         */
        void ConstructL();
        
    private:    // Data
        RPointerArray<CDRMNotifierSession> iSessions;
        RPointerArray<TMessageData> iMessages;
        
        
    public:     // Friend classes
        
    protected:  // Friend classes
        
    private:    // Friend classes
        
};

#endif      // CDRMMESSAGESTORAGE_H   

// End of File
