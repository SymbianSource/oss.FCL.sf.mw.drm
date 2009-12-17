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
* Description:  This class declares the interface of class CDRMNotifierSession.
*                handles the queues for notification events
*
*/


#ifndef CDRMNOTIFIERSESSION_H
#define CDRMNOTIFIERSESSION_H

//  INCLUDES
#include <e32base.h>
#include "DRMMessageStorage.h"

// CONSTANTS
// MACROS
// FUNCTION PROTOTYPES
// DATA TYPES

// FORWARD DECLARATIONS
class RMessage2;
class RRPointerArray;

// CLASS DECLARATION

/**
*  Server side instance of DRM Notifier session.
*  This class is the server side instance of C/S communications of a certain
*  session.
*
*  @lib DRMCommon.dll
*  @since S60Rel2.6
*/
NONSHARABLE_CLASS( CDRMNotifierSession ) : public CSession2
{
  public:  // Constructors and destructor
   
   /**
    * Two-phased constructor.
    *
    * @since S60Rel2.6
    * @param aClient Associated client side thread.
    * @param aStorage  a pointer to an instance of CDRMMessageStorage
    * @return New session instance.
    */
   static CDRMNotifierSession* NewL( CDRMMessageStorage* aStorage );
   
   /**
    * Destructor.
    * @since S60Rel2.6
    */
   virtual ~CDRMNotifierSession();
   
  public: // New functions
   
   /**
    * SendNotificationL
    * 
    * Sends a notification to a client or adds it to the queue, depending on if
    * the client is active or not
    *
    * @since S60Rel2.6
    * @param aMessage The message that needs to be sent
    * @exception Method leaves with appropriate exception value
    *            if any errors occured.
    * @return ETrue if the message was put into queue
    *         EFalse if it was run normally
    */
    TBool SendNotificationL( CDRMMessageStorage::TMessageData* aMessage );
  
  public: // Functions from base classes
   
   /**
    * From CSession: Handles the service request event.
    * @since S60Rel2.6
    * @param aMessage The message related to the event.
    * @exception Method leaves with appropriate exception value
    *            if any errors occured.
    */
   void ServiceL( const RMessage2& aMessage );
   
  private:
   
   /**
    * C++ constructor.
    * @since S60Rel2.6
    * @param aClient Client thread.
    * @param aStorage  a pointer to an instance of CDRMMessageStorage
    */
   CDRMNotifierSession( CDRMMessageStorage* aStorage);
   
   /**
    * Second phase constructor.
    * @since S60Rel2.6
    */ 
   void ConstructL();
   
   /** 
    * ServiceL() runs DispatchL() under TRAP harness, so
    * all errors can be catched properly. DispatchL() then calls
    * appropriate private method depending on the request.
    * @since S60Rel2.6
    * @param aMessage The message associated to the event.
    */
   void DispatchL( const RMessage2& aMessage );
   
   /** 
    * Helper function for DispatchL(). 
    * @since S60Rel2.6
    * @param aMessage The message associated to the event.
    */
   void NotifyClientsL( const RMessage2& aMessage );
   

   /** 
    * Helper function for DispatchL(). 
    * @since S60Rel2.6
    * @param aMessage The message associated to the event.
    */
   void RecieveNotificationL( const RMessage2& aMessage );
   
   /** 
    * Helper function for DispatchL(). 
    * @since S60Rel2.6
    * @param aMessage The message associated to the event.
    */  
   void CancelNotificationL( const RMessage2& aMessage );

   /** 
    * Helper function for DispatchL(). 
    * @since S60Rel2.6
    * @param aMessage The message associated to the event.
    */  
   void RegisterL( const RMessage2& aMessage );

   /** 
    * Helper function for DispatchL(). 
    * @since S60Rel2.6
    * @param aMessage The message associated to the event.
    */  
   void UnRegisterL( const RMessage2& aMessage );

   /** 
    * Helper function for DispatchL(). 
    * @since S60Rel2.6
    * @param aMessage The message associated to the event.
    */  
   void RegisterURIL( const RMessage2& aMessage );

   /** 
    * Helper function for DispatchL(). 
    * @since S60Rel2.6
    * @param aMessage The message associated to the event.
    */  
   void UnRegisterURIL( const RMessage2& aMessage );

   /**
    * NotifyL
    *
    * Helper function for sending a notification
    *
    * @since S60Rel2.6
    * @param aMessage - The message to be sent
    * @param aFromQueue - ETrue if the message parameter is from the queue,
                          EFalse if the message parameter is not from the queue
    */
   void NotifyL(CDRMMessageStorage::TMessageData *aMessage, TBool aFromQueue );

   /**
    * CanNotify
    *
    * Helper function for determining if we need to handle the notification
    *
    * @since S60Rel2.6
    * @param aMessage - the message data of the message to be sent
    * @return ETrue if it can be sent, EFalse if it can't be send
    */
   TBool CanNotify( CDRMMessageStorage::TMessageData *aMessage );

   // Prohibit copy constructor.
   CDRMNotifierSession( const CDRMNotifierSession& );
   // Prohibit assigment operator.
   CDRMNotifierSession& operator=( const CDRMNotifierSession& );

  private:    // Data
      NONSHARABLE_STRUCT( TContentData )
          {
          HBufC8* iContentID;
          TDRMEventType iEventType;
          };

      CDRMMessageStorage* iStorage;
      RPointerArray<TContentData> iContentIDList;
      RPointerArray<CDRMMessageStorage::TMessageData> iMessageQueue;
      TBool iIsListening;
      RMessage2 iListener;
      TBool iIsInStorage;
   
};


#endif      // CDRMNOTIFIERSESSION_H   

// End of File
