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
* Description:  DRM3 Engine manages all DRM related database operations.
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <s32mem.h> 
#include "DRMNotifierSession.h"
#include "DRMEventAddRemove.h"
#include "DRMEventModify.h"
#include "DRMEventTimeChange.h"
#include "drmnotifierclientserver.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
// MACROS

#ifdef _DRM_TESTING
_LIT( KDateTimeFormat, "%F%Y%M%D%H%T%S%C" );
#define _LOGBUFNUM( a, b ) { TBuf8< 64 > __b( a ); __b.AppendNum( b ); file.Write( __b ); }
#define LOGBUFNUM( a, b ) { TBuf8< 64 > __b( a ); __b.AppendNum( b ); Log( __b ); }
#define LOG( a ) Log( a )

#else
#define _LOGBUFNUM( a, b )
#define LOGBUFNUM( a, b )
#define LOG( a )
#endif

// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMMessageStorage::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMMessageStorage* CDRMMessageStorage::NewL()
    {
    CDRMMessageStorage* self = new( ELeave ) CDRMMessageStorage();
    
    CleanupStack::PushL( self );
    
    self->ConstructL();
    
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CDRMMessageStorage::~CDRMMessageStorage() 
    {
    for ( TInt i = 0; i < iMessages.Count();i++ )
        {
        delete iMessages[i]->iData;
        delete iMessages[i]->iMessageData;
        }
    iMessages.ResetAndDestroy();
    iSessions.Reset();
    
    LOG( _L8( "Dying..." ) );
    
#ifdef _DRM_TESTING
    delete iLog;
#endif
    }

// -----------------------------------------------------------------------------
// CDRMMessageStorage::AddSession
// -----------------------------------------------------------------------------
//
TInt CDRMMessageStorage::AddSession( CDRMNotifierSession* aSession ) 
    {
    return iSessions.Append( aSession );
    };

// -----------------------------------------------------------------------------
// CDRMMessageStorage::UpdateMessage
// -----------------------------------------------------------------------------
//
void CDRMMessageStorage::UpdateMessage( TMessageData* aMessage )
    {
    TInt count = 0;

    aMessage->iRefCount = aMessage->iRefCount - 1;

    if(aMessage->iRefCount == 0)
        {
        for( count = 0; count < iMessages.Count(); count++ )
            {
            if( iMessages[count] == aMessage )
                {
                delete iMessages[count]->iMessageData;
                delete iMessages[count]->iData;
                iMessages[count]->iMessageData = 0;
                iMessages[count]->iData = 0;
                }
            iMessages.Remove(count);
            count = iMessages.Count();
            }
        }
    }
// -----------------------------------------------------------------------------
// CDRMMessageStorage::GetEventObjectLC
// -----------------------------------------------------------------------------
//
MDRMEvent* CDRMMessageStorage::GetEventObjectLC( TDRMEventType aEventType )
    {
    MDRMEvent* event = NULL;

    switch( aEventType)
        {
        case KEventAddRemove:
            event = CDRMEventAddRemove::NewLC(ERightsObjectRecieved);
            break;
        case KEventModify:
            event = CDRMEventModify::NewLC();
            break;
        case KEventTimeChange:
            event = CDRMEventTimeChange::NewLC();
            break;
	    default:
	        User::Leave(KErrArgument);
	        break;
        }           
    return event;
    }
        
// -----------------------------------------------------------------------------
// CDRMMessageStorage::NotifyL
// -----------------------------------------------------------------------------
//
void CDRMMessageStorage::NotifyL(TDRMEventType& aEventType, const RMessage2& aMessage) 
    {
    TInt queued = EFalse;
    TMessageData* message = new (ELeave) TMessageData;
    CleanupStack::PushL(message);
    TUint8* eventBuf = static_cast<TUint8*>(User::AllocLC(DRMNotifier::KDRMSizeOfMessage));
    TPtr8 event( eventBuf,0,DRMNotifier::KDRMSizeOfMessage );

    message->iRefCount = 0;
    message->iEventType = aEventType;
    message->iData = 0;
    message->iMessageData = eventBuf;


    // Read the buffer:
    aMessage.ReadL( 0, event);

    RMemReadStream input(eventBuf, DRMNotifier::KDRMSizeOfMessage);
    MDRMEvent* eventobj = GetEventObjectLC(aEventType);
    eventobj->InternalizeL(input);

    // We need to extract the uri from the message
    // for those that it is needed
    switch( aEventType )
        {
        case KEventAddRemove:
            message->iData = static_cast<CDRMEventAddRemove*>(eventobj)->GetContentIDL();
            break;
        case KEventModify:
            message->iData = static_cast<CDRMEventModify*>(eventobj)->GetContentIDL();
            break;
        case KEventTimeChange:
            break;
        default:
            User::Leave(KErrArgument);
            break;
        }
    CleanupStack::PopAndDestroy(); // eventobj
    CleanupStack::PushL(message->iData);

    for( TInt i = 0; i < iSessions.Count();i++ )
        {
        queued = EFalse;

        TRAPD(error, queued = iSessions[i]->SendNotificationL( message ) );
        if( error && error != KErrNotFound )
            {
            User::LeaveIfError(error);
            }
        else if( queued )
            {
            message->iRefCount++;
            }
        }

    if( !message->iRefCount ) 
        {
        CleanupStack::PopAndDestroy(); // message->iData;
        CleanupStack::PopAndDestroy(); // eventBuf;
        CleanupStack::PopAndDestroy(); // message
        }
    else
        {
        CleanupStack::PushL( message );
        iMessages.AppendL ( message );
        CleanupStack::Pop( message );
        CleanupStack::Pop(); // message->iData;
        CleanupStack::Pop(); // eventBuf
        CleanupStack::Pop(); // message
        }
    };
// -----------------------------------------------------------------------------
// CDRMMessageStorage::CancelL
// -----------------------------------------------------------------------------
//
void CDRMMessageStorage::CancelL(CDRMNotifierSession* aSession)
    {
    TInt count = 0;

    for ( count = 0; count < iSessions.Count(); count++ )
        {
        if( iSessions[count] == aSession)
            {
            iSessions.Remove(count);
            return;
            }
        } 
    User::Leave(KErrNotFound);
    }

// -----------------------------------------------------------------------------
// CDRMMessageStorage::CDRMMessageStorage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMMessageStorage::CDRMMessageStorage() : iSessions(4), iMessages(4)
    {
    // Nothing
    }
    
// -----------------------------------------------------------------------------
// CDRMMessageStorage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDRMMessageStorage::ConstructL() 
    {
    }


#ifdef _DRM_TESTING

// -----------------------------------------------------------------------------
// CDRMMessageStorage::Log
// Logging operation.
// -----------------------------------------------------------------------------
//
void CDRMMessageStorage::Log( const TDesC8& aLog ) const
    {
    iLog->Log( aLog );
    }

// -----------------------------------------------------------------------------
// CDRMMessageStorage::Log
// Logging operation.
// -----------------------------------------------------------------------------
//
void CDRMMessageStorage::Log( const TDesC& aLog ) const
    {
    iLog->Log( aLog );
    }
#endif

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
