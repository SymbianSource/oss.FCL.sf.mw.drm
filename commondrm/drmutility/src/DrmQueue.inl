/*
* Copyright (c) 2006 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DRM Queue
*
*/


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDrmQueue
// Constructor
// -----------------------------------------------------------------------------
//
template< class T > inline DRM::CDrmQueue< T >::CDrmQueue() : CBase()
    {
    }

// -----------------------------------------------------------------------------
// ~CDrmQueue
// Destructor
// -----------------------------------------------------------------------------
//
template< class T > inline DRM::CDrmQueue< T >::~CDrmQueue()
    {
    T* payload = PopFront();

    // Empty the queue:
    while( payload )
        {
        delete payload;
        payload = PopFront();
        }
    iSemaphore.Close();
    }

// -----------------------------------------------------------------------------
// CDrmQueue::ConstructL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
template< class T > inline void DRM::CDrmQueue< T >::ConstructL()
    {
    User::LeaveIfError( iSemaphore.CreateLocal(1) );
    }


// -----------------------------------------------------------------------------
// CDrmQueue::NewLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
template< class T > inline DRM::CDrmQueue< T >* DRM::CDrmQueue< T >::NewLC()
    {
    DRM::CDrmQueue< T >* self( new( ELeave ) DRM::CDrmQueue< T >() );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CDrmQueue::NewL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
template< class T > inline DRM::CDrmQueue< T >* DRM::CDrmQueue< T >::NewL()
    {
    DRM::CDrmQueue< T >* self( DRM::CDrmQueue< T >::NewLC() );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfoImpl::AppendToQueue
// -----------------------------------------------------------------------------
//
template< class T > inline void DRM::CDrmQueue< T >::AppendToQueueL( T* aData )
    {
    CleanupStack::PushL( aData );

    QueueData* data( new (ELeave) QueueData );
    // insert the data into the queue object
    data->iData = aData;
    data->iNext = NULL;

    CleanupStack::Pop( aData );

    // Critical area start:
    iSemaphore.Wait();

    if( !iLast )
        {
        iFirst = data;
        iLast = iFirst;
        }
    else
        {
        iLast->iNext = data;
        iLast = data;
        }

    // Critical area end
    iSemaphore.Signal();
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfoImpl::PopFront
// -----------------------------------------------------------------------------
//
template< class T > inline T* DRM::CDrmQueue< T >::PopFront()
    {
    QueueData* data = NULL;
    T* payload = NULL;

    // If there is nothing in the queue return NULL
    if( !iFirst )
        {
        return payload;
        }

    // Critical area start:
    iSemaphore.Wait();

    data = iFirst;

    // Set the first and the next
    iFirst = iFirst->iNext;

    // reset the pointer on data
    data->iNext = NULL;

    // if there is no next in iFirst, this is the last or if there is no iFirst
    if( !iFirst || !iFirst->iNext )
        {
        iLast = iFirst;
        }

    // Critical Area End
    iSemaphore.Signal();

    payload = data->iData;
    delete data;
    data = NULL;

    return payload;
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfoImpl::PopItem
// -----------------------------------------------------------------------------
//
template< class T > inline T* DRM::CDrmQueue< T >::PopItem( TInt aId )
    {
    QueueData* current( iFirst );
    QueueData* previous( NULL );
    T* payload( NULL );

    // If there is nothing in the queue return NULL
    if( !iFirst )
        {
        return payload;
        }
    // Critical area start:
    iSemaphore.Wait();
    // Iterate over all items in the queue
    while ( current )
        {
        if ( aId == current->iData->iOperationId )
            {
            payload = current->iData;
            current->iData = NULL;
            if ( previous )
                {
                // Previous present. So its member iNext must be updated.
                previous->iNext = current->iNext;
                }
            else
                {
                // previous not present, so current was first,
                // and iFirst must be updated.
                iFirst = current->iNext;
                }

            if ( !current->iNext )
                {
                // Current was last item. So iLast must be updated.
                iLast = previous;
                }
            current->iNext = NULL;
            delete current;
            break;
            }
        previous = current;
        current = current->iNext;
        }
    // Critical Area End
    iSemaphore.Signal();
    return payload;
    }

//  End of File
