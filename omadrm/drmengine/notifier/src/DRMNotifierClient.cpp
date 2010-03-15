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
* Description:  This file contains implementation of RDRMNotifierClient class.
*
*/



// INCLUDE FILES
#include    "DRMNotifierClient.h"
#include "drmnotifierclientserver.h"
#include "DRMRightsClient.h"
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
// RDRMNotifierClient::RDRMNotifierClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
RDRMNotifierClient::RDRMNotifierClient(TDRMEventType* aEventType, TDRMEventType* aEventTypeWait,
                                       TPtr8* aPtr, TPtr8* aWaitPtr) :
    iEventType( aEventType ),
    iEventTypeWait( aEventTypeWait ),
    iData( aPtr ),
    iWaitData( aWaitPtr ),
    numdata( 0 )
    {
   // Nothing
    }

// Destructor
RDRMNotifierClient::~RDRMNotifierClient()
    {
    Close();
    }


// -----------------------------------------------------------------------------
// RDRMNotifierClient::Connect
// Connects to the server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt RDRMNotifierClient::Connect( void )
    {
    const TVersion requiredVersion(
        DRMNotifier::KServerMajorVersion,
        DRMNotifier::KServerMinorVersion,
        DRMNotifier::KServerBuildVersion );

    TInt ret = KErrNotFound;
    TUint8 count( 0 );
    TUint8 cont( 1 );

    do
        {
        ret = CreateSession( DRMNotifier::KServerName,
                             requiredVersion,
                             KMaxMessageSlots );
        if ( !ret )
            {
            cont = 0;
            }
        else
            {
            if ( ( count++ == 0 ) && ( ret == KErrNotFound ) )
                {
                ret = RDRMRightsClient::StartServer();
                }
            }
        }
    while ( cont && ( count < 2 ) );

    return ret;
    }

// -----------------------------------------------------------------------------
// RDRMNotifierClient::Close
// Closes the connection to the server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RDRMNotifierClient::Close()
    {
    RHandleBase::Close();
    }


// -----------------------------------------------------------------------------
// RDRMNotifierClient::SendEvent
// Sends an event to the client, asynchronous
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RDRMNotifierClient::SendEvent(TRequestStatus& aStatus)
    {
    // Send the message.
    SendReceive( DRMNotifier::ENotifyClients, TIpcArgs(iData, *iEventType),
        aStatus );
    };

// -----------------------------------------------------------------------------
// RDRMNotifierClient::WaitForCompletion
// Waits for the notifications from the server, asynchronous
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RDRMNotifierClient::WaitForCompletion(TRequestStatus& aStatus)
    {
    numdata.Set(reinterpret_cast<TUint8*>(iEventTypeWait),sizeof(TDRMEventType),sizeof(TDRMEventType));

    // Send the message.
    SendReceive( DRMNotifier::ERecieveNotification, TIpcArgs(iWaitData, &numdata),
        aStatus );
    };

// -----------------------------------------------------------------------------
// RDRMNotifierClient::RegisterForType
// Registeres a type to the server, synchronous
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RDRMNotifierClient::RegisterForType( TDRMEventType aEventType,
                                          HBufC8* aURI )
    {
    TPckg<TInt> eventType(0);
    eventType.Set(reinterpret_cast<TUint8*>(&aEventType),sizeof(TDRMEventType),sizeof(TDRMEventType));

    if( aURI )
        {
        TUint8* dataptr = const_cast<TUint8*>(aURI->Ptr());
        TPtr8 uriPkg(dataptr,aURI->Size(),aURI->Size());

        // Send the message.
        SendReceive( DRMNotifier::ERegisterURI, TIpcArgs(&uriPkg, &eventType));
        }
    else
        {
        // Send the message.
        SendReceive( DRMNotifier::ERegister, TIpcArgs(&eventType));
        }
    }

// -----------------------------------------------------------------------------
// RDRMNotifierClient::UnRegisterFromType
// Unregisteres a type from the server, synchronous
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RDRMNotifierClient::UnRegisterFromType( TDRMEventType aEventType,
                                             HBufC8* aURI)
    {
    TPckg<TInt> eventType(0);
    eventType.Set(reinterpret_cast<TUint8*>(&aEventType),sizeof(TDRMEventType),sizeof(TDRMEventType));

    if( aURI )
        {
        TUint8* dataptr = const_cast<TUint8*>(aURI->Ptr());
        TPtr8 uriPkg(dataptr,aURI->Size(),aURI->Size());

        // Send the message.
        SendReceive( DRMNotifier::EUnRegisterURI, TIpcArgs(&uriPkg, &eventType));
        }
    else
        {
        // Send the message.
        SendReceive( DRMNotifier::EUnRegister, TIpcArgs(&eventType));
        }
    }


// -----------------------------------------------------------------------------
// RDRMNotifierClient::CancelRequest
// Cancels the request from the server, synchronous
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RDRMNotifierClient::CancelRequest()
    {
    // Send the message.
    SendReceive( DRMNotifier::ECancelNotification);
    };

//  End of File
