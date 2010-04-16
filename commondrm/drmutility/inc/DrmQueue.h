/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CDRMQUEUE_H
#define CDRMQUEUE_H

//*** system include files go here:
#include <e32base.h>

//*** user include files go here:

//*** forward declarations go here:

//*** constants go here:

namespace DRM
{
 /**
  *  DRM Queue
  *
  *
  *
  *  @lib drmutility.lib
  *  @since S60 v4.0
  */
template< class C > class CDrmQueue : public CBase
    {

public:

    /**
    * Construction function
    */
    static CDrmQueue<C>* NewLC();

    /**
    * Construction function
    */
    static CDrmQueue<C>* NewL();


    /**
    * Destructor.
    */
    ~CDrmQueue();

    /**
    * C++ default constructor.
    */
    CDrmQueue();

    /**
    * Checks the rights and their status for a specific unique id
    * implementation
    *
    * @param[in]   aData : a pointer to the data object
    */
    void AppendToQueueL( C* aData );



    /**
    * Checks the rights and their status for a specific unique id
    * implementation
    *
    * @return pointer to popped data.
    */
    C* PopFront();

    /**
    * Checks
    * implementation
    * @param[in]   aId : ID representing data to be popped from queue
    * @return      Pointer to data object in queue, NULL if not found
    *
    */
    C* PopItem( TInt aId );

private:



    /**
    * 2nd phase constructor.
    */
    void ConstructL();


    struct QueueData
        {
        C* iData;
        QueueData* iNext;
        };



    // Operation Queue:
    QueueData* iFirst;
    QueueData* iLast;

    // To prevent queue handling from being messed up by threads
    RSemaphore iSemaphore;
    };
}

// Since it's a template class include the implementation
#include "DrmQueue.inl"

#endif // CDRMAUTOMATEDUSAGE_H
