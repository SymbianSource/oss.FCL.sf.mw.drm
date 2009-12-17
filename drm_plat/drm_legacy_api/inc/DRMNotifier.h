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
* Description:  This class handles registering and unregistering of
*                DRM event observers to DRM notifier, and sending
*                DRM notifications to other registered observers.
*
*/



#ifndef CDRMNOTIFIER_H
#define CDRMNOTIFIER_H

//  INCLUDES
#include <e32base.h>
#include <DRMEventObserver.h>
#include <DRMEvent.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CDRMEventHandler;

// CLASS DECLARATION

/**
 *  With this class an application can register to receive DRM related 
 *  notifications.
 *
 *  @lib DRMCommon.dll
 *  @since S60Rel2.6
 */
class CDRMNotifier : public CBase
{
public:  // Constructors and destructor   
    /**
    * Two-phased constructor.
    */
    IMPORT_C static CDRMNotifier* NewL( void );
   
    /**
    * Two-phased constructor.
    */
    IMPORT_C static CDRMNotifier* NewLC( void );
   
    /**
    * Destructor.
    */
    IMPORT_C virtual ~CDRMNotifier();
   
public: // New functions   
    /**
    * RegisterEventObserverL
    *
    * Registers a new event observer to DRM Notifier. Listens to all events
    * of the given type
    *
    * @since S60Rel2.6
    * @param aObserver Observer.
    * @param aEvent the type of event to listen to
    * @return none, leaves with Symbian OS error code if an error occurs
    */
    IMPORT_C void RegisterEventObserverL( MDRMEventObserver& aObserver , 
                                          const TDRMEventType& aEvent);   
    /**
    * UnRegisterEventObserverL
    *
    * Unregisters existing event observer from DRM Notifier.
    *
    * @since S60Rel2.6
    * @param aObserver Observer.
    * @param aEvent the type of event to unregister
    * @return none, leaves with Symbian OS error code if an error occurs
    */
    IMPORT_C void UnRegisterEventObserverL( MDRMEventObserver& aObserver, 
                                            const TDRMEventType& aEvent);
    /**
    * RegisterEventObserverL
    *
    * Registers a new event observer to DRM Notifier. Only recieves
    * notification when the rights accociated with the given content id
    * are handled
    *
    * @since S60Rel2.6
    * @param aObserver Observer
    * @param aEvent the type of event to listen to
    * @param aContentID the specific content id to listen to
    * @return none, leaves with Symbian OS error code if an error occurs
    */
    IMPORT_C void RegisterEventObserverL( MDRMEventObserver& aObserver , 
                                          const TDRMEventType& aEvent,
                                          const TDesC8& aContentID);
    /**
    * UnRegisterEventObserverL
    *
    * Unregisters existing event observer from DRM Notifier.
    *
    * @since S60Rel2.6
    * @param aObserver Observer.
    * @param aEvent the type of event to listen to
    * @param aContentID the specific content id to listen to
    * @return none, leaves with Symbian OS error code if an error occurs
    */
    IMPORT_C void UnRegisterEventObserverL( MDRMEventObserver& aObserver, 
                                            const TDRMEventType& aEvent,
                                            const TDesC8& aContentID);

    /**
    * SendEventL
    *
    * Sends a notification to registered observers
    *
    * @since S60Rel2.6
    * @param aEvent  the event to be sent
    * @param aStatus  the request status for the event, will be updated when
    *                 the event has been sent
    * @return none
    */
    IMPORT_C void SendEventL( MDRMEvent& aEvent, TRequestStatus& aStatus ); 
   
private:
   
    /**
    * C++ default constructor.
    */
    CDRMNotifier( void );

    /**
    * First Phase constructor
    */
    void ConstructL();
   
    // Prohibit copy constructor if not deriving from CBase.
    CDRMNotifier( const CDRMNotifier& );

    // Prohibit assigment operator if not deriving from CBase.
    CDRMNotifier& operator=( const CDRMNotifier& );

  private:    // Data
   // Event handler.
   CDRMEventHandler* iEventHandler;
};

#endif      // CDRMNOTIFIER_H   

// End of File

