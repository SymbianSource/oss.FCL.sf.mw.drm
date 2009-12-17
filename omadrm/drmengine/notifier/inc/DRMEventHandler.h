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



#ifndef CDRMEVENTHANDLER_H
#define CDRMEVENTHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <DRMEvent.h>
#include <DRMEventAddRemove.h>
#include <DRMEventObserver.h>
#include "DRMEventModify.h"
#include "DRMEventTimeChange.h"
#include "DRMNotifierClient.h"

// CONSTANTS
const TInt KOperationNone             = 0;
const TInt KRegisterOperation         = 1;
const TInt KRegisterURIOperation      = 2;
const TInt KUnRegisterOperation       = 3;
const TInt KUnRegisterURIOperation    = 4;

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class MDRMEventObserver;

// CLASS DECLARATION

/**
 *  CDRMEventHandler
 *  Performs the required operations between the client and the server
 *  Handles the handles for the observers
 *
 *  @lib DRMCommon.dll
 *  @since S60Rel2.6
 */
class CDRMEventHandler : public CActive
    {
public:  // Constructors and destructor
        
    /**
    * Two-phased constructor.
    */
    static CDRMEventHandler* NewL();
        
    /**
    * Destructor.
    */
    virtual ~CDRMEventHandler();

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
    void RegisterEventObserverL( MDRMEventObserver& aObserver , 
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
    void UnRegisterEventObserverL( MDRMEventObserver& aObserver, 
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
    void RegisterEventObserverL( MDRMEventObserver& aObserver , 
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
    void UnRegisterEventObserverL( MDRMEventObserver& aObserver, 
                                            const TDRMEventType& aEvent,
                                            const TDesC8& aContentID);
    /**
    * SendEventL
    *
    * Sends a notification to registered observers, asyncronous
    *
    * @since S60Rel2.6
    * @param aEvent the event to send
    * @param aStatus the request status of the event
    */
    void SendEventL( MDRMEvent& aEvent, TRequestStatus& aStatus );
           
public: // Functions from base classes
        
protected:  // New functions
    CDRMEventHandler();

    /**
    * 2nd phase constructor.
    */
    void ConstructL();

    protected:  // Functions from base classes
        
    /**
    * From CActive: RunL catches events from DRM server.
    */
    void RunL();
        
    void DoCancel();

    private:

    /**
    * GetEventObjectLC
    *
    * Reserves memory for an object specified in the internal structure
    * and leaves it in the cleanup stack
    *
    * @return a base class pointer to the requested object
    *         The function leaves if an error occurs with a Symbian OS
    *         error code
    */
    MDRMEvent* GetEventObjectLC();
        
    /**
    * HandleRunL
    * 
    * Helper function for RunL that does the actual callbacks to
    * registered clients
    * 
    * @return none
    */
    void HandleRunL();

    // Prohibit copy constructor if not deriving from CBase.
    CDRMEventHandler( const CDRMEventHandler& );
    // Prohibit assigment operator if not deriving from CBase.
    CDRMEventHandler& operator=( const CDRMEventHandler& );

public:     // Data
    
protected:  // Data

private:    // Data
    struct TObserverData 
        {
        MDRMEventObserver* iObserver;
        HBufC8* iContentID;
        };

    struct TDelayedObserverData
        {
        TInt iRequest;
        MDRMEventObserver* iObserver;
        TDRMEventType iEventType;
        HBufC8* iContentID;
        };

    // Instance of c/s communications.
    RDRMNotifierClient* iHandler;
        
    // List of observers
    CArrayPtrSeg< TObserverData >* iAddRemoveObservers;
    CArrayPtrSeg< TObserverData >* iModifyObservers;
    CArrayPtrSeg< TObserverData >* iTimeChangeObservers;
    CArrayPtrSeg< TDelayedObserverData >* iDelayedObservers;

    // variable to tell if calls are being delayed
    TBool iIsDelayed;

    // Status of Client
    TBool iIsOpen;

    // Client Server Communication
    TUint8* iDataBuffer;
    TDRMEventType* iEventType;
    TPtr8* iData;

    TUint8* iWaitDataBuffer;
    TDRMEventType* iEventTypeWait;
    TPtr8* iWaitData;

public:     // Friend classes

protected:  // Friend classes

private:    // Friend classes
    };

#endif      // DRMEVENTHANDLER_H   
            
// End of File
