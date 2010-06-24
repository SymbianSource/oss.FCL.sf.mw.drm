/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class handles all client requests.
*
*/


// INCLUDE FILES
#include <s32file.h>
#include <f32file.h>
#include "DRMCommon.h"
#include "DRMNotifierSession.h"
#include "DRMNotifierServer.h"
#include "drmnotifierclientserver.h"
#include <e32test.h>

// NAMESPACES
using namespace DRMNotifier;

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS


// MACROS
#ifdef _DRM_TESTING
#define LOG( a ) ( ( CDRMNotifierServer* )( Server() ) )->Log( a )
#else
#define LOG( a )
#endif

// LOCAL CONSTANTS AND MACROS

const TInt KSanityDataLengthLow = 0;
const TInt KSanityDataLengthHigh = 32768;

// MODULE DATA STRUCTURES
// DATA TYPES
// LOCAL FUNCTIONS


// -----------------------------------------------------------------------------
// SanitizeL
// Performs a sanity check on length parameters
// -----------------------------------------------------------------------------
//
LOCAL_C void SanitizeL( TInt aParam )
    {
    if( aParam <= KSanityDataLengthLow || aParam > KSanityDataLengthHigh )
        {
        User::Leave(KErrArgument);
        }
    }

// FORWARD DECLARATIONS
// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CDRMNotifierSession::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMNotifierSession* CDRMNotifierSession::NewL( CDRMMessageStorage* aStorage)
    {
    CDRMNotifierSession* self = new( ELeave ) CDRMNotifierSession( aStorage );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CDRMNotifierSession::~CDRMNotifierSession
// Destructor.
// -----------------------------------------------------------------------------
//
CDRMNotifierSession::~CDRMNotifierSession()
    {
    TInt error = KErrNone;
    TInt i = 0;

    if( iStorage )
        {
        TRAP(error, iStorage->CancelL( this ) );
        }

    if(error)
        {
        LOG( _L8( " Something went wrong with Cancelling the notifications " ) );
        }

    for( i = 0; i < iMessageQueue.Count(); i++ )
        {
        if( this->iStorage ) // coverity check
            {
            iStorage->UpdateMessage(iMessageQueue[i]);
            }
        }

    iMessageQueue.Reset();

    for( i = 0; i < iContentIDList.Count(); i++ )
        {
        if( iContentIDList[i]->iContentID )
            {
            delete iContentIDList[i]->iContentID;
            iContentIDList[i]->iContentID = NULL;
            }
        delete iContentIDList[i];
        iContentIDList[i] = NULL;
        }
    iContentIDList.Reset();

    LOG( _L8( "Notifier session closed." ) );
    }



// -----------------------------------------------------------------------------
// CDRMNotifierSession::SendNotificationL
// This method sends a notification to the client or if it's not active adds it
// to the queue and the next time a registration happens the client will get
// notified.
// -----------------------------------------------------------------------------
//
TBool CDRMNotifierSession::SendNotificationL( CDRMMessageStorage::TMessageData* aMessage )
    {
    if( !CanNotify( aMessage ) )
        {
        User::Leave(KErrNotFound);
        }

    if( !iIsListening || iMessageQueue.Count() )
        {
        iMessageQueue.AppendL ( aMessage );
        return ETrue;
        }
    NotifyL( aMessage, EFalse );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CDRMNotifierSession::ServiceL
// This method runs DispatchL() under TRAP harness, since every error case
// can be handled ==> no need to let this function leave.
// -----------------------------------------------------------------------------
//
void CDRMNotifierSession::ServiceL( const RMessage2& aMessage )
    {
    LOG( _L8( "ServiceL called" ) );
    // Trap possible errors...

    TRAPD( error, DispatchL( aMessage ) );

    if ( error )
        {
        LOG( _L8( "DispatcL threw an exception" ) );
        // ...and complete the request in case of an error.
        aMessage.Complete( error );
        return;
        }

    // The message has already completed successfully.
    LOG( _L8( "DispatchL completed successfully" ) );
    }

// -----------------------------------------------------------------------------
// CDRMNotifierSession::CDRMNotifierSession
// Default constructor.
// -----------------------------------------------------------------------------
//
CDRMNotifierSession::CDRMNotifierSession( CDRMMessageStorage* aStorage ) :
    // Base class' constructor is called first.
    iStorage( aStorage )
    {
    // Nothing.
    }

// -----------------------------------------------------------------------------
// CDRMNotifierSession::ConstructL
// Second phase constructor. Initializes the log tool in DRM internal testing.
// -----------------------------------------------------------------------------
//
void CDRMNotifierSession::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CDRMNotifierSession::DispatchL
// Checks which command the user requested, and forwards the request to
// appropriate private method. This helps to keep the code more readable.
// -----------------------------------------------------------------------------
//
void CDRMNotifierSession::DispatchL( const RMessage2& aMessage )
    {
    #ifdef _DEBUG
    RDebug::Printf(">>> [%s] %d", _S8(__PRETTY_FUNCTION__), aMessage.Function());
    #endif

    switch ( aMessage.Function() )
        {
        case ENotifyClients:
            NotifyClientsL( aMessage );
            break;
        case ERecieveNotification:
            RecieveNotificationL( aMessage );
            break;
        case ECancelNotification:
            CancelNotificationL( aMessage );
            break;
        case ERegister:
            RegisterL( aMessage );
            break;
        case EUnRegister:
            UnRegisterL( aMessage );
            break;
        case ERegisterURI:
            RegisterURIL( aMessage );
            break;
        case EUnRegisterURI:
            UnRegisterURIL( aMessage );
            break;
        default:
            LOG( _L8( "DispatchL: Invalid command" ) );
            User::Leave( KErrNotSupported );
        }
    }

// -----------------------------------------------------------------------------
// CDRMNotifierSession::NotifyClientsL
// -----------------------------------------------------------------------------
//
void CDRMNotifierSession::NotifyClientsL( const RMessage2& aMessage )
    {
    LOG( _L8( "NotifyClientsL" ) );
    TDRMEventType eventType;

    eventType = aMessage.Int1();

    iStorage->NotifyL(eventType,aMessage);

    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CDRMNotifierSession::RecieveNotificationL
// -----------------------------------------------------------------------------
//
void CDRMNotifierSession::RecieveNotificationL( const RMessage2& aMessage )
    {
    LOG( _L8( "RecieveNotificationL" ) );
    TInt error = KErrNone;
    TPckg<TInt> package(0);
    TDRMEventType eventType;

    package.Set(reinterpret_cast<TUint8*>(&eventType),sizeof(TDRMEventType),sizeof(TDRMEventType));
    TRAP(error, aMessage.ReadL(1, package));

    // Set the status of listening
    iIsListening = ETrue;

    // Set this to message as listener
    iListener = aMessage;

    if( !iIsInStorage )
        {

        User::LeaveIfError( iStorage->AddSession( this ) );
        iIsInStorage = ETrue;        
        }

    // Check if there are any notifications in queue if so process first one
    if( iMessageQueue.Count() )
        {
        NotifyL( iMessageQueue[0], ETrue );
        }

    // Message complete will be set elsewhere.
    // All done.
    }

// -----------------------------------------------------------------------------
// CDRMNotifierSession::CancelNotificationL
// -----------------------------------------------------------------------------
//
void CDRMNotifierSession::CancelNotificationL( const RMessage2& aMessage )
    {
    LOG( _L8( "CancelNotificationL" ) );
    TInt error = KErrNone;
    TInt i = 0;

    TRAP(error, iStorage->CancelL( this ) );
    if( error && error != KErrNotFound )
        {
        LOG(  _L8( "CDRMNotifierSession:: Something went wrong with Cancelling the notifications " ) );
        }

    for( i = 0; i < iMessageQueue.Count(); i++ )
        {
        iStorage->UpdateMessage(iMessageQueue[i]);
        }
    iMessageQueue.Reset();

    for( i = 0; i < iContentIDList.Count(); i++ )
        {
        if( iContentIDList[i]->iContentID )
            {
            delete iContentIDList[i]->iContentID;
            iContentIDList[i]->iContentID = NULL;
            }
        delete iContentIDList[i];
        iContentIDList[i] = NULL;
        }
    iContentIDList.Reset();

    iIsInStorage = EFalse;

    iIsListening = EFalse;

    if ( !iListener.IsNull() )
        {
        iListener.Complete( KErrCancel );
        }

    aMessage.Complete( KErrNone );
    }
// -----------------------------------------------------------------------------
// CDRMNotifierSession::RegisterURIL
// Get the information from the client, construct a rights object, and add
// it to the database.
// -----------------------------------------------------------------------------
//
void CDRMNotifierSession::RegisterL( const RMessage2& aMessage )
    {
    LOG( _L8( "RegisterL" ) );
    TPckg<TInt> package(0);
    TDRMEventType eventType = 0;
    TContentData* content = 0;

    package.Set(reinterpret_cast<TUint8*>(&eventType),sizeof(TDRMEventType),sizeof(TDRMEventType));
    aMessage.ReadL(0, package);

    content = new (ELeave) TContentData;

    content->iContentID = NULL;
    content->iEventType = eventType;

    iContentIDList.AppendL ( content );

    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CDRMNotifierSession::UnRegisterURIL
// Get the information from the client, construct a rights object, and add
// it to the database.
// -----------------------------------------------------------------------------
//
void CDRMNotifierSession::UnRegisterL( const RMessage2& aMessage )
    {
    LOG( _L8( "UnRegisterL" ) );
    TPckg<TInt> package(0);
    TDRMEventType eventType = 0;

    package.Set(reinterpret_cast<TUint8*>(&eventType),sizeof(TDRMEventType),sizeof(TDRMEventType));
    aMessage.ReadL(0, package);

    for( TInt i = 0; i < iContentIDList.Count(); i++ )
        {
        if( !iContentIDList[i]->iContentID &&
            iContentIDList[i]->iEventType == eventType )
            {
            delete iContentIDList[i];
            iContentIDList[i] = NULL;

            iContentIDList.Remove(i);
            break;
            }
        }

    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CDRMNotifierSession::RegisterURIL
// -----------------------------------------------------------------------------
//
void CDRMNotifierSession::RegisterURIL( const RMessage2& aMessage )
    {
    LOG( _L8( "RegisterURIL" ) );
    TPckg<TInt> package(0);
    TDRMEventType eventType = 0;
    TContentData* content = 0;
    TInt length = 0;

    HBufC8* contentID = 0;
    TPtr8 packageData(NULL,0);

    length = aMessage.GetDesLength(0);

    SanitizeL( length );

    contentID = HBufC8::NewLC(length);
    packageData.Set(contentID->Des());

    aMessage.ReadL(0, packageData);

    package.Set(reinterpret_cast<TUint8*>(&eventType),sizeof(TDRMEventType),sizeof(TDRMEventType));
    aMessage.ReadL(1, package);

    content = new (ELeave) TContentData;

    content->iContentID = contentID;
    content->iEventType = eventType;

    iContentIDList.AppendL ( content );

    CleanupStack::Pop();
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CDRMNotifierSession::UnRegisterURIL
// -----------------------------------------------------------------------------
//
void CDRMNotifierSession::UnRegisterURIL( const RMessage2& aMessage )
    {
    LOG( _L8( "UnRegisterURIL" ) );
    TPckg<TInt> package(0);
    TDRMEventType eventType = 0;
    TInt length = 0;

    HBufC8* contentID = 0;
    TPtr8 packageData(NULL,0);

    length = aMessage.GetDesLength(0);

    SanitizeL( length );

    contentID = HBufC8::NewLC(length);
    packageData.Set(contentID->Des());

    aMessage.ReadL(0, packageData);

    package.Set(reinterpret_cast<TUint8*>(&eventType),sizeof(TDRMEventType),sizeof(TDRMEventType));
    aMessage.ReadL(1, package);

    for( TInt i = 0; i < iContentIDList.Count(); i++ )
        {
        if( iContentIDList[i]->iContentID &&
            iContentIDList[i]->iEventType == eventType &&
            !iContentIDList[i]->iContentID->Compare( contentID->Des() ) )
            {
            if( iContentIDList[i]->iContentID )
                {
                delete iContentIDList[i]->iContentID;
                iContentIDList[i]->iContentID = NULL;
                }
            delete iContentIDList[i];
            iContentIDList[i] = NULL;

            iContentIDList.Remove(i);
            break;
            }
        }
    CleanupStack::PopAndDestroy();
    aMessage.Complete( KErrNone );
    }


// -----------------------------------------------------------------------------
// CDRMNotifierSession::NotifyL
// -----------------------------------------------------------------------------
//
void CDRMNotifierSession::NotifyL( CDRMMessageStorage::TMessageData *aMessage, TBool aFromQueue )
    {
    LOG( _L8( "NotifyL" ) );
    TPtr8 data( reinterpret_cast<TUint8*>(&aMessage->iEventType),
                sizeof(TDRMEventType),
                sizeof(TDRMEventType));
    TPtr8 event( aMessage->iMessageData, DRMNotifier::KDRMSizeOfMessage,
                 DRMNotifier::KDRMSizeOfMessage );

    iListener.WriteL(0, event);
    iListener.WriteL(1, data);

    iListener.Complete( KErrNone );

    // Remove the message from the queue
    if( aFromQueue )
        {
        // Reduce the counter and release the message if needed
        iStorage->UpdateMessage( aMessage );

        iMessageQueue.Remove(0);
        }
    iIsListening = EFalse;
    }
// -----------------------------------------------------------------------------
// CDRMNotifierSession::CanNotifyL
// -----------------------------------------------------------------------------
//
TBool CDRMNotifierSession::CanNotify( CDRMMessageStorage::TMessageData *aMessage )
    {
    for( TInt i = 0; i < iContentIDList.Count(); i++ )
        {
        if( iContentIDList[i]->iEventType == aMessage->iEventType &&
            (!iContentIDList[i]->iContentID ||
            !iContentIDList[i]->iContentID->Compare( aMessage->iData->Des() ) ) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
//  End of File
