/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class handles communications between a notifier client &
*                DRM server in DRM notifier.
*
*/



#ifndef RDRMNOTIFIERCLIENT_H
#define RDRMNOTIFIERCLIENT_H

//  INCLUDES
#include <e32std.h>
#include <DRMEvent.h>

// CONSTANTS
const TInt KMaxMessageSlots = 2;

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*  This class handles all client/server communications.
*
*  @lib DRMCommon.dll
*  @since S60Rel2.6
*/
class RDRMNotifierClient : public RSessionBase
    {
    public:  // Constructors and destructor
       /**
        * C++ default constructor.
        */
       RDRMNotifierClient(TDRMEventType* aEventType, TDRMEventType* aEventTypeWait,
                          TPtr8* aPtr, TPtr8* aWaitPtr);

       /**
        * Destructor.
        */
       virtual ~RDRMNotifierClient();

    public: // New functions

       /**
       * This method opens a connection to the server.
       * @since S60Rel2.6
       * Return Symbian OS / DRM Engine specific error code.
       */
       TInt Connect( void );

       /**
       * WaitForCompletion
       *
       * Waits for notifications. When the status is completed a notification
       * has been received.
       *
       * @since S60Rel2.6
       * @param aStatus  the status will be updated when the a notification
       *                 has been received
       * @return none
       */
       void WaitForCompletion( TRequestStatus& aStatus );


       /**
       * SendEvent
       *
       * Sends an event to the notifier server, the request status is updated
       * when all the notifications have been forwarded
       *
       * @since S60Rel2.6
       * @param aStatus  the status will be updated when all notifications have
       *                 been forwarded
       * @return none
       */
       void SendEvent( TRequestStatus& aStatus );

       /**
       * RegisterForType
       *
       * Register to the server to listen to specific event types, if a URI
       * is provided notifications of the type that have that URI as affected
       * URI will get notified
       *
       * @since S60Rel2.6
       * @param aEventType  the event type to listen to
       * @param aURI  optional URI to limit the C/S communication
       * @return none
       */
       void RegisterForType( TDRMEventType aEventType, HBufC8* aURI = 0 );


       /**
       * UnRegisterFromType
       *
       * Unregister from the server from listening to specific event types,
       * if a URI is provided notifications of the type that have that URI
       * as affected URI will get unregistered
       *
       * @since S60Rel2.6
       * @param aEventType  the event type to listen to
       * @param aURI  optional URI to limit the C/S communication
       * @return none
       */
       void UnRegisterFromType( TDRMEventType aEventType, HBufC8* aURI = 0 );

       /**
       * CancelRequest()
       *
       * Used when the object is being destroyed. All calls are cancelled and
       * the server side will get a notification to remove all notifications
       * from their lists
       *
       * @since S60Rel2.6
       * @return none
       */
       void CancelRequest();


    public: // Functions from base classes
        void Close();
    protected:  // New functions

    protected:  // Functions from base classes

    private:

        // Prohibit copy constructor
        RDRMNotifierClient( const RDRMNotifierClient& );
        // Prohibit assigment operator
        RDRMNotifierClient& operator=( const RDRMNotifierClient& );

    public:     // Data
    protected:  // Data
    private:    // Data
        TDRMEventType* iEventType;
        TDRMEventType* iEventTypeWait;
        TPtr8* iData;
        TPtr8* iWaitData;
        TPckg<TInt> numdata;

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes
    };

#endif      // RDRMNOTIFIERCLIENT_H

// End of File
