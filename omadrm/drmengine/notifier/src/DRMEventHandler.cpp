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
* Description:  Implementation of class CDRMEventHandler
*
*/



// INCLUDE FILES
#include <s32mem.h> 
#include "DRMEventHandler.h"
#include "drmnotifierclientserver.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMEventHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMEventHandler* CDRMEventHandler::NewL()
    {
    CDRMEventHandler* self = new( ELeave ) CDRMEventHandler;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CDRMEventHandler::~CDRMEventHandler
// Destructor.
// -----------------------------------------------------------------------------
//
CDRMEventHandler::~CDRMEventHandler()
{
    if( iHandler )
        {
        Cancel();
        }

    if( iData )
        {
        delete iData;
        iData = NULL;
        }
    if( iDataBuffer )
        {
        delete iDataBuffer;
        iDataBuffer = NULL;
        }
    if( iEventType )
        {
        delete iEventType;
        iEventType = NULL;
        }

    if( iWaitData )
        {
        delete iWaitData;
        iWaitData = NULL;
        }
    if( iWaitDataBuffer )
        {
        delete iWaitDataBuffer;
        iWaitDataBuffer = NULL;
        }
    if( iEventTypeWait )
        {
        delete iEventTypeWait;
        iEventTypeWait = NULL;
        }

    if ( iAddRemoveObservers ) 
        {
        for( TInt i = 0; i < iAddRemoveObservers->Count();i++)
            {
            delete (*iAddRemoveObservers)[i]->iContentID;
            (*iAddRemoveObservers)[i]->iContentID = NULL;
            }
        iAddRemoveObservers->Reset();
        delete iAddRemoveObservers;
        iAddRemoveObservers = NULL;
        }
    if ( iModifyObservers ) 
        {
        for( TInt i = 0; i < iModifyObservers->Count();i++)
            {
            delete (*iModifyObservers)[i]->iContentID;
            (*iModifyObservers)[i]->iContentID = NULL;
            }
        iModifyObservers->Reset();
        delete iModifyObservers;
        iModifyObservers = NULL;
        }
        
    if ( iTimeChangeObservers ) 
        {
        iTimeChangeObservers->Reset();
        delete iTimeChangeObservers;
        iTimeChangeObservers = NULL;
        }
                
    if ( iDelayedObservers )
        {
        for( TInt i = 0; i < iDelayedObservers->Count();i++)
            {
            delete (*iDelayedObservers)[i]->iContentID;
            (*iDelayedObservers)[i]->iContentID = NULL;
            }
        iDelayedObservers->Reset();
        delete iDelayedObservers;
        iDelayedObservers = NULL;
        }

    if( iHandler )
        {
        iHandler->Close();
        delete iHandler;
        iHandler = NULL;
        }
    }


// -----------------------------------------------------------------------------
// CDRMEventHandler::RegisterEventObserverL
// Adds the new handler to the list of handlers.
// Checks if the handler is already in the list
// Sets the active object active if it is not active yet.
// -----------------------------------------------------------------------------
//
void CDRMEventHandler::RegisterEventObserverL(
    MDRMEventObserver& aObserver,
    const TDRMEventType& aEvent )
    {

    TInt count = 0;
    TBool addToServer = ETrue;
    
    
    if( iIsDelayed )
        {
        TDelayedObserverData* delayData = new (ELeave) TDelayedObserverData;
        CleanupStack::PushL( delayData);
        delayData->iRequest = KRegisterOperation;
        delayData->iObserver = &aObserver;
        delayData->iEventType = aEvent;
        delayData->iContentID = NULL;
        iDelayedObservers->AppendL( delayData );
        CleanupStack::Pop();    // delayData;
        return;
        }

    switch ( aEvent )
        {
        case KEventAddRemove:
            if ( iAddRemoveObservers )
                {
                for ( count = 0; count < iAddRemoveObservers->Count(); ++count )
                    {
                    if ( ( *iAddRemoveObservers )[ count ]->iObserver == &aObserver &&
                         !( *iAddRemoveObservers )[ count ]->iContentID )
                        {
                        return;
                        }
                    else if( addToServer && !( *iAddRemoveObservers)[ count ]->iContentID ) 
                        {
                        addToServer = EFalse;
                        }
                    }
                }
            break;
        case KEventModify:
            if ( iModifyObservers )
                {
                for ( count = 0; count < iModifyObservers->Count(); ++count )
                    {
                    if ( ( *iModifyObservers )[ count ]->iObserver == &aObserver &&
                         !( *iModifyObservers )[ count ]->iContentID )
                        {
                        return;
                        }
                    else if( addToServer && !( *iModifyObservers)[ count ]->iContentID ) 
                        {
                        addToServer = EFalse;
                        }
                    }
                }
            break;
        case KEventTimeChange:
            if ( iTimeChangeObservers )        
                {
                for ( count = 0; count < iTimeChangeObservers->Count(); ++count )
                    {
                    if ( ( *iTimeChangeObservers )[ count ]->iObserver == &aObserver )
                        {
                        return;
                        }
                    }
                }      
            break;
        default:
            User::Leave(KErrArgument);
            break;
        }  

    struct TObserverData* observer = new (ELeave) TObserverData;
    CleanupStack::PushL(observer);

    observer->iObserver = &aObserver;
    observer->iContentID = NULL;

    switch( aEvent )
        {
        case KEventAddRemove:
            if( this->iAddRemoveObservers ) // coverity check
                {
                iAddRemoveObservers->AppendL( observer );
                }
            break;
        case KEventModify:
            if( this->iModifyObservers ) // coverity check
                {
                iModifyObservers->AppendL( observer );
                }
            break;
        case KEventTimeChange:
            if( this->iTimeChangeObservers ) // coverity check
                {
                iTimeChangeObservers->AppendL( observer );
                }
            break;
        default:
            User::Leave(KErrArgument);
            break;
        }
   
    if ( !IsAdded() )
        {
        CActiveScheduler::Add( this );
        }

    if ( !IsActive() )
        {
        if( iStatus != KRequestPending )
            {
            iStatus = KRequestPending;
            iHandler->WaitForCompletion(iStatus);
            }
        SetActive();
        }

    // Register the type to the server
    if( addToServer )
        {
        iHandler->RegisterForType(aEvent);
        }

    CleanupStack::Pop(); // observer
    }

// -----------------------------------------------------------------------------
// CDRMEventHandler::UnRegisterEventObserverL
// Removes the observer from the list of handlers.
// -----------------------------------------------------------------------------
//
void CDRMEventHandler::UnRegisterEventObserverL( 
    MDRMEventObserver& aObserver,
    const TDRMEventType& aEvent)
    {
    TInt count = 0;
    TBool removeFromServer = ETrue;

    if( iIsDelayed )
        {
        TDelayedObserverData* delayData = new (ELeave) TDelayedObserverData;
        CleanupStack::PushL( delayData);
        delayData->iRequest = KUnRegisterOperation;
        delayData->iObserver = &aObserver;
        delayData->iEventType = aEvent;
        delayData->iContentID = NULL;
        iDelayedObservers->AppendL( delayData );
        CleanupStack::Pop();    // delayData;
        return;
        }

    // Check if something else uses the registration for this event type, if something
    // Does set removeFromServer to EFalse so the registration is not removed from the
    // server
    switch ( aEvent )
        {
        case KEventAddRemove:
            if ( iAddRemoveObservers )
                {
                for ( count = 0; count < iAddRemoveObservers->Count(); ++count )
                    {
                    if( removeFromServer && !( *iAddRemoveObservers)[ count ]->iContentID &&
                        ( *iAddRemoveObservers)[ count ]->iObserver != &aObserver ) 
                        {
                        removeFromServer = EFalse;
                        }
                    }
                }
            break;
        case KEventModify:
            if ( iModifyObservers )
                {
                for ( count = 0; count < iModifyObservers->Count(); ++count )
                    {
                    if( removeFromServer && !( *iModifyObservers)[ count ]->iContentID &&
                        ( *iModifyObservers)[ count ]->iObserver != &aObserver ) 
                        {
                        removeFromServer = EFalse;
                        }
                    }
                }
            break;
        case KEventTimeChange:
            if ( iTimeChangeObservers )
                {
                for ( count = 0; count < iTimeChangeObservers->Count(); ++count )
                    {
                    if( ( *iTimeChangeObservers)[ count ]->iObserver != &aObserver ) 
                        {
                        removeFromServer = EFalse;
                        }
                    }
                }
            break;        
        default:
            User::Leave(KErrArgument);
            break;
        }  


    // Remove the registrations from internal lists and possibly from the server
    switch ( aEvent )
        {
        case KEventAddRemove:
            if ( iAddRemoveObservers )
                {
                for ( count = 0; count < iAddRemoveObservers->Count(); ++count )
                    {
                    if ( ( *iAddRemoveObservers )[ count ]->iObserver == &aObserver &&
                         !( *iAddRemoveObservers )[ count ]->iContentID )
                        {
                        if( removeFromServer )
                            {
                            // Unregister the type from the server
                            iHandler->UnRegisterFromType(aEvent);
                            }

                        // Free Memory
                        delete ( *iAddRemoveObservers )[ count ];
                        ( *iAddRemoveObservers )[ count ] = NULL;

                        // Delete the element from the table
                        iAddRemoveObservers->Delete( count );
                        iAddRemoveObservers->Compress();
                        return;
                        }
                    }
                }
            User::Leave(KErrNotFound);
            break;
        case KEventModify:
            if ( iModifyObservers )
                {
                for ( count = 0; count < iModifyObservers->Count(); ++count )
                    {
                    if ( ( *iModifyObservers )[ count ]->iObserver == &aObserver &&
                         !( *iModifyObservers )[ count ]->iContentID )
                        {
                        if( removeFromServer )
                            {
                            // Unregister the type from the server
                            iHandler->UnRegisterFromType(aEvent);
                            }

                        // Free Memory
                        delete ( *iModifyObservers )[ count ];
                        ( *iModifyObservers )[ count ] = NULL;

                        // Delete the element from the table
                        iModifyObservers->Delete( count );
                        iModifyObservers->Compress();
                        return;
                        }
                    }
                }
            User::Leave(KErrNotFound);
            break;
        case KEventTimeChange:
            if ( iTimeChangeObservers )
                {
                for ( count = 0; count < iTimeChangeObservers->Count(); ++count )
                    {
                    if ( ( *iTimeChangeObservers )[ count ]->iObserver == &aObserver )
                        {
                        if( removeFromServer )
                            {
                            // Unregister the type from the server
                            iHandler->UnRegisterFromType(aEvent);
                            }

                        // Free Memory
                        delete ( *iTimeChangeObservers )[ count ];
                        ( *iTimeChangeObservers )[ count ] = NULL;

                        // Delete the element from the table
                        iTimeChangeObservers->Delete( count );
                        iTimeChangeObservers->Compress();
                        return;
                        }
                    }
                }
            User::Leave(KErrNotFound);        
            break;
        default:
            User::Leave(KErrArgument);
            break;
        }  
    // Should never get here
    User::Leave( KErrGeneral );
    }

// -----------------------------------------------------------------------------
// CDRMEventHandler::RegisterEventObserverL
// Adds the new handler to the list of handlers.
// Checks is the handler is already in the list
// Sets the active object active if it is not active yet.
// -----------------------------------------------------------------------------
//
void CDRMEventHandler::RegisterEventObserverL(
    MDRMEventObserver& aObserver,
    const TDRMEventType& aEvent,
    const TDesC8& aContentID )
    {
    TInt count = 0;
    TBool addToServer = ETrue;

    if( iIsDelayed )
        {
        TDelayedObserverData* delayData = new (ELeave) TDelayedObserverData;
        CleanupStack::PushL( delayData);
        delayData->iRequest = KRegisterURIOperation;
        delayData->iObserver = &aObserver;
        delayData->iEventType = aEvent;
        delayData->iContentID = aContentID.AllocLC();
        iDelayedObservers->AppendL( delayData );
        CleanupStack::Pop();    // iContentID;
        CleanupStack::Pop();    // delayData;
        return;
        }

    switch ( aEvent )
        {
        case KEventAddRemove:
            if ( iAddRemoveObservers )
                {
                for ( count = 0; count < iAddRemoveObservers->Count(); ++count )
                    {
                    if ( ( *iAddRemoveObservers )[ count ]->iObserver == &aObserver &&
                         ( *iAddRemoveObservers )[ count ]->iContentID &&
                         !( *iAddRemoveObservers )[ count ]->iContentID->Compare( aContentID ) )
                        {
                        return;
                        }
                    else if( addToServer && 
                        !( *iAddRemoveObservers)[ count ]->iContentID->Compare( aContentID ) ) 
                        {
                        addToServer = EFalse;
                        }
                    }
                }
            break;
        case KEventModify:
            if ( iModifyObservers )
                {
                for ( count = 0; count < iModifyObservers->Count(); ++count )
                    {
                    if ( ( *iModifyObservers )[ count ]->iObserver == &aObserver &&
                         ( *iModifyObservers )[ count ]->iContentID &&
                         !( *iModifyObservers )[ count ]->iContentID->Compare( aContentID ) )
                        {
                        return;
                        }
                    else if( addToServer && 
                        !( *iModifyObservers)[ count ]->iContentID->Compare( aContentID ) ) 
                        {
                        addToServer = EFalse;
                        }
                    }
                }
            break;
        default:
            User::Leave(KErrArgument);
            break;
        }  

    HBufC8* contentID = aContentID.AllocLC();
    struct TObserverData* observer = new (ELeave) TObserverData;
    CleanupStack::PushL(observer);

    observer->iObserver = &aObserver;
    observer->iContentID = contentID;

    switch( aEvent )
        {
        case KEventAddRemove:
            if( this->iAddRemoveObservers ) // coverity check
                {
                iAddRemoveObservers->AppendL( observer );
                }
            break;
        case KEventModify:
            if( this->iModifyObservers ) // coverity check
                {
                iModifyObservers->AppendL( observer );
                }
            break;
        default:
            User::Leave(KErrArgument);
            break;
        }
   
    if ( !IsAdded() )
        {
        CActiveScheduler::Add( this );
        }

    if ( !IsActive() )
        {
        if( iStatus != KRequestPending )
            {
            iStatus = KRequestPending;
            iHandler->WaitForCompletion(iStatus);
            }
        SetActive();
        }

    // Register the type to the server
    if ( addToServer )
        {
        iHandler->RegisterForType(aEvent,contentID);
        }

    CleanupStack::Pop(); // contentID
    CleanupStack::Pop(); // observer;
    }

// -----------------------------------------------------------------------------
// CDRMEventHandler::UnRegisterEventObserverL
// Removes the observer from the list of handlers.
// -----------------------------------------------------------------------------
//
void CDRMEventHandler::UnRegisterEventObserverL( 
    MDRMEventObserver& aObserver,
    const TDRMEventType& aEvent,
    const TDesC8& aContentID )
    {
    TInt count = 0;
    TBool removeFromServer = ETrue;

    if( iIsDelayed )
        {
        TDelayedObserverData* delayData = new (ELeave) TDelayedObserverData;
        CleanupStack::PushL( delayData);
        delayData->iRequest = KUnRegisterURIOperation;
        delayData->iObserver = &aObserver;
        delayData->iEventType = aEvent;
        delayData->iContentID = aContentID.AllocLC();
        iDelayedObservers->AppendL( delayData );
        CleanupStack::Pop();    // iContentID;
        CleanupStack::Pop();    // delayData;
        return;
        }

    // Check if something else uses the registration for this event type, if something
    // Does set removeFromServer to EFalse so the registration is not removed from the
    // server
    switch ( aEvent )
        {
        case KEventAddRemove:
            if ( iAddRemoveObservers )
                {
                for ( count = 0; count < iAddRemoveObservers->Count(); ++count )
                    {
                    if( removeFromServer && 
                        !( *iAddRemoveObservers )[ count ]->iContentID->Compare( aContentID ) &&
                        ( *iAddRemoveObservers )[ count ]->iObserver != &aObserver ) 
                        {
                        removeFromServer = EFalse;
                        }
                    }
                }
            break;
        case KEventModify:
            if ( iModifyObservers )
                {
                for ( count = 0; count < iModifyObservers->Count(); ++count )
                    {
                    if( removeFromServer && 
                        !( *iModifyObservers )[ count ]->iContentID->Compare( aContentID ) &&
                        ( *iModifyObservers )[ count ]->iObserver != &aObserver ) 
                        {
                        removeFromServer = EFalse;
                        }
                    }
                }
            break;
        default:
            User::Leave(KErrArgument);
            break;
        }  


    // Remove the registrations from internal lists and possibly from the server
    switch ( aEvent )
        {
        case KEventAddRemove:
            if ( iAddRemoveObservers )
                {
                for ( count = 0; count < iAddRemoveObservers->Count(); ++count )
                    {
                    if ( ( *iAddRemoveObservers )[ count ]->iObserver == &aObserver &&
                         ( *iAddRemoveObservers )[ count ]->iContentID &&
                         !( *iAddRemoveObservers )[ count ]->iContentID->Compare( aContentID ) )
                        {
                        if ( removeFromServer )
                            {
                            // Unregister the type from the server
                            iHandler->UnRegisterFromType(aEvent,(*iAddRemoveObservers)[ count ]->iContentID);
                            }

                        // Free Memory
                        delete ( *iAddRemoveObservers )[ count ]->iContentID;
                        ( *iAddRemoveObservers )[ count ]->iContentID = NULL;
                        delete ( *iAddRemoveObservers )[ count ];
                        ( *iAddRemoveObservers )[ count ] = NULL;

                        // Delete the element from the table
                        iAddRemoveObservers->Delete( count );
                        iAddRemoveObservers->Compress();
                        return;
                        }
                    }
                }
            User::Leave(KErrNotFound);
            break;
        case KEventModify:
            if ( iModifyObservers )
                {
                for ( count = 0; count < iModifyObservers->Count(); ++count )
                    {
                    if ( ( *iModifyObservers )[ count ]->iObserver == &aObserver &&
                         ( *iModifyObservers )[ count ]->iContentID &&
                         !( *iModifyObservers )[ count ]->iContentID->Compare( aContentID ) )
                        {

                        if( removeFromServer )
                            {
                            // Unregister the type from the server
                            iHandler->UnRegisterFromType(aEvent,(*iModifyObservers)[ count ]->iContentID);
                            }

                        // Free Memory
                        delete ( *iModifyObservers )[ count ]->iContentID;
                        ( *iModifyObservers )[ count ]->iContentID = NULL;
                        delete ( *iModifyObservers )[ count ];
                        ( *iModifyObservers )[ count ] = NULL;

                        // Delete the element from the table
                        iModifyObservers->Delete( count );
                        iModifyObservers->Compress();
                        return;
                        }
                    }
                }
            User::Leave(KErrNotFound);
            break;
        default:
            User::Leave(KErrArgument);
            break;
        }  
    // Should never get here
    User::Leave( KErrGeneral );
    }
// -----------------------------------------------------------------------------
// CDRMEventHandler::SendEventL
// Sends the event to the client that sends it to the server
// Opens connection if it's not already open
// -----------------------------------------------------------------------------
//
void CDRMEventHandler::SendEventL( MDRMEvent& aEvent, TRequestStatus& aStatus )
    {
    RMemWriteStream output(iDataBuffer,DRMNotifier::KDRMSizeOfMessage);
    CleanupClosePushL( output );
    
    Mem::FillZ(iDataBuffer,DRMNotifier::KDRMSizeOfMessage);

    if(!iIsOpen)
        {
		if(!iHandler)
            {
            iHandler = new (ELeave) RDRMNotifierClient(iEventType,iEventTypeWait,iData,iWaitData);
            }
        User::LeaveIfError(iHandler->Connect());
        iIsOpen = ETrue;            
        }

    aEvent.ExternalizeL(output);
    aEvent.GetEventType(*iEventType);
    
    iHandler->SendEvent(aStatus);
    CleanupStack::PopAndDestroy();
    };

// -----------------------------------------------------------------------------
// CDRMEventHandler::CDRMEventHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMEventHandler::CDRMEventHandler() :
   CActive( EPriorityUserInput ), iIsOpen( EFalse )
{
   // Nothing
}

// -----------------------------------------------------------------------------
// CDRMNotificationHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDRMEventHandler::ConstructL()
{
    iDataBuffer = static_cast<TUint8*>(User::AllocL(DRMNotifier::KDRMSizeOfMessage));
    iEventType = static_cast<TDRMEventType*>(User::AllocL(sizeof(TDRMEventType*)));
    iWaitDataBuffer = static_cast<TUint8*>(User::AllocL(DRMNotifier::KDRMSizeOfMessage));
    iEventTypeWait = static_cast<TDRMEventType*>(User::AllocL(sizeof(TDRMEventType*)));

    iData = new( ELeave ) TPtr8(iDataBuffer,DRMNotifier::KDRMSizeOfMessage,DRMNotifier::KDRMSizeOfMessage);
    iWaitData = new( ELeave) TPtr8(iWaitDataBuffer,DRMNotifier::KDRMSizeOfMessage,DRMNotifier::KDRMSizeOfMessage);

    iHandler = new (ELeave) RDRMNotifierClient(iEventType,iEventTypeWait,iData,iWaitData);

    User::LeaveIfError( iHandler->Connect() );
    iIsOpen = ETrue;

    iAddRemoveObservers = new( ELeave ) CArrayPtrSeg< TObserverData >( 4 );
    iModifyObservers = new( ELeave ) CArrayPtrSeg< TObserverData >( 4 );
    iTimeChangeObservers = new( ELeave ) CArrayPtrSeg< TObserverData >( 4 );    
    iDelayedObservers = new( ELeave ) CArrayPtrSeg< TDelayedObserverData >( 4 );

    Mem::FillZ(iDataBuffer,DRMNotifier::KDRMSizeOfMessage);
    Mem::FillZ(iDataBuffer,DRMNotifier::KDRMSizeOfMessage);
    Mem::FillZ(iEventType,sizeof(TDRMEventType));
    Mem::FillZ(iEventTypeWait,sizeof(TDRMEventType));
}

// -----------------------------------------------------------------------------
// CDRMNotificationHandler::RunL
// Inherited from CActive
// -----------------------------------------------------------------------------
//
void CDRMEventHandler::RunL()
{
   __ASSERT_DEBUG( iAddRemoveObservers, User::Invariant() );
   __ASSERT_DEBUG( iModifyObservers, User::Invariant() );

   if ( iStatus == KErrCancel )
       {
       return;
       }

   TRAPD( error, HandleRunL() );
   
   if( error && iIsDelayed )
       {
       iIsDelayed = EFalse;
       }

   // Discard all errors except this one.
   if ( error == KErrServerTerminated )
   {
      User::Leave( KErrServerTerminated );
   }  
   SetActive();   
}

void CDRMEventHandler::DoCancel()
{
   iHandler->CancelRequest();
}

// -----------------------------------------------------------------------------
// CDRMEventHandler::GetEventObjectLC
// -----------------------------------------------------------------------------
//
MDRMEvent* CDRMEventHandler::GetEventObjectLC()
    {
    MDRMEvent* event = NULL;
    switch(*iEventTypeWait)
        {
        case KEventAddRemove:
            event = CDRMEventAddRemove::NewLC( ERightsObjectRecieved );
            break;
        case KEventModify:
            event = CDRMEventModify::NewLC();
            break;
        case KEventTimeChange:
            event = CDRMEventTimeChange::NewLC();
            break;
        }           
    return event;
    }

// -----------------------------------------------------------------------------
// CDRMEventHandler::HandleRunL
// -----------------------------------------------------------------------------
//
void CDRMEventHandler::HandleRunL()
{
    MDRMEvent* object = NULL;
    TDRMEventType event = 0;
    TInt count = 0;
    HBufC8* contentID = NULL;
    TInt error = KErrNone;
    
    RMemReadStream input(iWaitDataBuffer,DRMNotifier::KDRMSizeOfMessage);
    CleanupClosePushL( input );
    
    if(!iIsOpen)
        {
		if(!iHandler)
            {
            iHandler = new (ELeave) RDRMNotifierClient(iEventType,iEventTypeWait,iData,iWaitData);
            }
        User::LeaveIfError(iHandler->Connect());
        iIsOpen = ETrue;            
        }

    object = GetEventObjectLC();
    if(!object)
        {
        User::Leave(KErrGeneral);
        }

    object->InternalizeL(input);

    object->GetEventType(event);

    iIsDelayed = ETrue;

    switch ( event )
        {
        case KEventAddRemove:
            if ( iAddRemoveObservers )
                {
                TRAPD( error_mem, contentID = (reinterpret_cast<CDRMEventAddRemove*>(object))->GetContentIDL());
                if ( error_mem )
                    {
                    iIsDelayed = EFalse;
                    User::Leave( error_mem );
                    }                
                CleanupStack::PushL(contentID);

                for ( count = 0; count < iAddRemoveObservers->Count(); ++count )
                    {
                    if( !( *iAddRemoveObservers )[ count ]->iContentID ||
                        !( *iAddRemoveObservers )[ count ]->iContentID->Compare( contentID->Des() ) )
                        {
                        TRAP( error, ( *iAddRemoveObservers )[ count ]->iObserver->HandleEventL( object ) );
                        }
                    }
                CleanupStack::PopAndDestroy();
                }
            break;
        case KEventModify:
            if ( iModifyObservers )
                {
                TRAPD(error_mem, contentID = (reinterpret_cast<CDRMEventModify*>(object))->GetContentIDL());
                if( error_mem )
                    {
                    iIsDelayed = EFalse;
                    User::Leave( error_mem );
                    }                
                CleanupStack::PushL(contentID);
 
                for ( count = 0; count < iModifyObservers->Count(); ++count )
                    {
                    if( !( *iModifyObservers )[ count ]->iContentID ||
                        !( *iModifyObservers )[ count ]->iContentID->Compare( contentID->Des() ) )
                        {
                        TRAP( error, ( *iModifyObservers )[ count ]->iObserver->HandleEventL( object ) );
                        }
                    }
                CleanupStack::PopAndDestroy();
                }
            break;
        case KEventTimeChange:
             if ( iTimeChangeObservers )
                {
                for ( count = 0; count < iTimeChangeObservers->Count(); ++count )
                    {
                    TRAP( error, ( *iTimeChangeObservers )[ count ]->iObserver->HandleEventL( object ) );
                    }
                }      
            break;
        default:
            iIsDelayed = EFalse;
            User::Leave(KErrArgument);
            break;
        }
        
    // Ignore errors   
    if( error )
        {
        // Do nothing
        }
    if( iStatus != KRequestPending )
        {
        iStatus = KRequestPending;
        iHandler->WaitForCompletion(iStatus);
        }
    CleanupStack::PopAndDestroy(); // object
    CleanupStack::PopAndDestroy(); // input
    iIsDelayed = EFalse;

    // Run delayed the adding and removing events in order
    while ( iDelayedObservers->Count() )
        {
        TInt ignore_error = KErrNone;

        switch( ( *iDelayedObservers )[ 0 ]->iRequest )
            {
            case KRegisterOperation:
                TRAP(ignore_error, 
                    RegisterEventObserverL( *( *iDelayedObservers )[ 0 ]->iObserver,
                                            ( *iDelayedObservers )[ 0 ]->iEventType ) );
                break;
            case KRegisterURIOperation:
                TRAP(ignore_error, 
                    RegisterEventObserverL( *( *iDelayedObservers )[ 0 ]->iObserver,
                                            ( *iDelayedObservers )[ 0 ]->iEventType,
                                            ( *iDelayedObservers )[ 0 ]->iContentID->Des() ) );
                break;
            case KUnRegisterOperation:
                TRAP(ignore_error, 
                    UnRegisterEventObserverL( *( *iDelayedObservers )[ 0 ]->iObserver,
                                              ( *iDelayedObservers )[ 0 ]->iEventType ) );
                break;
            case KUnRegisterURIOperation:
                TRAP(ignore_error, 
                    UnRegisterEventObserverL( *( *iDelayedObservers )[ 0 ]->iObserver,
                                              ( *iDelayedObservers )[ 0 ]->iEventType,
                                              ( *iDelayedObservers )[ 0 ]->iContentID->Des() ) );
                break;
            default:
                break;
            }
        if ( ( *iDelayedObservers )[ 0 ]->iContentID )
            {
            delete ( *iDelayedObservers )[ 0 ]->iContentID;
            ( *iDelayedObservers )[ 0 ]->iContentID = NULL;
            }
        delete ( *iDelayedObservers )[ 0 ];
        ( *iDelayedObservers )[ 0 ] = NULL;
        iDelayedObservers->Delete( 0 );
        iDelayedObservers->Compress();
        }
    }

//  End of File  
