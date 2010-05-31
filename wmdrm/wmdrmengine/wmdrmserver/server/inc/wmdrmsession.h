/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of the WMDRM Server Session class
*
*/


#ifndef __WMDRMSESSION_H
#define __WMDRMSESSION_H

#include <e32base.h>
#include <f32file.h>
#include "wmdrmserver.h"

class CSlot;
class CEnumerator;

/* Session class for the WMDRM server, implements slot and namespace
 * access functionality.
 */
class CWmDrmSession : public CSession2
	{
public:
	CWmDrmSession();

    /* Initialize a store via a given store name. If the store creation
     * flag is set, the store is created when missing, otherwise an
     * error is returned.
     *
     * @param aMessage  IPC message (store name, store creation flag)
     * @return  KErrArgument for incorrect input data, KErrNotFound if
     *          the store is missing and the store creation flag is not
     *          set, KErrDiskFull if system drive doesn't have enough 
     *          free space, KErrNone if successful
     */
    TInt InitStoreL( const RMessage2& aMessage );

    /* Removes a named store with all its contents.
     *
     * @param aMessage  IPC message (store name)
     * @return  KErrArgument for incorrect input data, CFileMan error
     *          codes, KErrNone if successful
     */
    TInt RemoveStoreL( const RMessage2& aMessage );

    /* Initialize a namespace via a given namespace name. If the
     * namespace creation flag is set, the namespace is created when missing,
     * otherwise an error is returned.
     *
     * @param aMessage  IPC message (namespace name, namespace creation flag)
     * @return  KErrArgument for incorrect input data, KErrNotFound if
     *          the namespace is missing and the namespace creation flag is not
     *          set, KErrDiskFull if system drive doesn't have enough free
     *          space, KErrNone if successful
     */
    TInt InitNamespaceL( const RMessage2& aMessage );

    /* Removes a namespace will all its contents.
     *
     * @param aMessage  IPC message (store name, namespace name)
     * @return  KErrArgument for incorrect input data, CFileMan error
     *          codes, KErrNone if successful
     */
    TInt RemoveNamespaceL( const RMessage2& aMessage );
        

    /* Opens a slot via its full name (store, namespace, slot hash and key ID). 
     *
     * @param aMessage  IPC message (all slot name components)
     * @return  KErrArgument for incorrect input data, KErrNotFound if the slot
     *          does not exist, KErrNone if successful
     */
    TInt SlotOpenL( const RMessage2& aMessage );

    /* Creates a slot via a full slot name and a given initial size.
     *
     * @param aMessage  IPC message (all slot name components)
     * @return  KErrArgument for incorrect input data, KErrDiskFull if system
     *          drive doesn't have enough free space, KErrNone if successful
     */
    TInt SlotCreateL( const RMessage2& aMessage );

    /* Seeks within an opened slot.
     *
     * @param aMessage  IPC message (seek starting point, seek offset)
     * @return  KErrArgument for incorrect input data, KErrNotReady if the
     *          slot is not open, KErrNone if successful
     */
    TInt SlotSeekL( const RMessage2& aMessage );

    /* Read data from an opened slot.
     *
     * @param aMessage  IPC message (data buffer)
     * @return  KErrNotReady if the slot is not open, KErrNone if successful
     */
    TInt SlotReadL( const RMessage2& aMessage );

    /* Write data to an opened slot.
     *
     * @param aMessage  IPC message (data buffer)
     * @return  KErrNotReady if the slot is not open, KErrDiskFull if system
     *          drive doesn't have enough free space, KErrNone if successful
     */
    TInt SlotWriteL( const RMessage2& aMessage );

    /* Changes the size of a slot, truncating it if necessary
     *
     * @param aMessage  IPC message (new size)
     * @return  KErrNotReady if the slot is not open, KErrNone if successful
     */
    TInt SlotResizeL( const RMessage2& aMessage );

    /* Removes a slot via its full name.
     *
     * @param aMessage  IPC message (all slot name components)
     * @return  KErrArgument for incorrect input data, KErrNone if successful
     */
    TInt SlotDeleteL( const RMessage2& aMessage );

    /* Closes an opened slot.
     *
     * @param aMessage  IPC message (not used)
     * @return  KErrNotReady if the slot is not open, otherwise KErrNone
     */
    TInt SlotCloseL( const RMessage2& aMessage );
        

    /* Initiate enumeration over a namespace or slot.
     *
     * @param aMessage  IPC message (namespace and/or slot information)
     * @return  KErrArgument for incorrect input data, KErrNone if successful
     */
    TInt EnumerateStartL( const RMessage2& aMessage );

    /* Reload the current slot information to the current
     * enumerator.
     *
     * @param aMessage  IPC message (not used)
     * @return  KErrNotReady if the enumeration has not started,
     *          otherwise KErrNone
     */
    TInt EnumerateReloadL( const RMessage2& aMessage );

    /* Moves the enumerator to the next slot and updates the slot
     * information.
     *
     * @param aMessage  IPC message (not used)
     * @return  KErrNotReady if the enumeration has not started, KErrNotFound
     *          if the enumerator has reached the end, otherwise KErrNone
     */
    TInt EnumerateNextL( const RMessage2& aMessage );

    /* Delete the currently enumerated slot.
     *
     * @param aMessage  IPC message (full slot information)
     * @return  KErrNotReady if the enumeration has not started,
     *          otherwise KErrNone
     */
    TInt EnumerateDeleteL( const RMessage2& aMessage );

    /* End the current enumeration.
     *
     * @param aMessage  IPC message (not used)
     * @return  KErrNotReady if the enumeration has not started,
     *          otherwise KErrNone
     */
    TInt EnumerateEndL( const RMessage2& aMessage );
    

    /* Empties the slot and enumerator caches.
     *
     * @param aMessage  IPC message (not used)
     * @return KErrNone
     */
    TInt EmptyCacheL( const RMessage2& aMessage );

    /* Log server statistics.
     *
     * @param aMessage  IPC message (not used)
     * @return KErrNone
     */
    TInt LogStats( const RMessage2& aMessage );

    /* Check whether the current time is anti-rollback secure
     *
     * @param aMessage  IPC message (not used)
     * @return KErrNone if the time is secure.
     */
    TInt TimeValid( const RMessage2& aMessage );

    /* Sets the current device time as secure
     *
     * @param aMessage  IPC message (not used)
     */
    void SetTimeAsValidL( const RMessage2& aMessage );


    /* Deletes rights from the stores
     *
     * @param aMessage  IPC message (not used)
     */
    TInt DeleteRightsL( const RMessage2& aMessage );
    
    /* Gets store state 
     *
     * @param aMessage  IPC message (store state)
     */
    void StoreStateL( const RMessage2& aMessage );
    
    /* Gets the drm time and validity
     *
     * @param aMessage  IPC message (time, validity)
     */
    void GetTimeL( const RMessage2& aMessage );    
    
    
    /* Commits the database
     *
     * @param aMessage  IPC message (not used)
     */
    void CommitDataL( const RMessage2& aMessage );    
    
private:
    /* Desctructor.
     */
	~CWmDrmSession();

    /* Returns a reference to the associated server object
     *
     * @return Reference to the server
     */
	inline CWmDrmServer& Server();

    /* Service dispatch function
     *
     * @param aMessage  IPC message
     */
	void ServiceL(const RMessage2& aMessage);

    /* Service error handler function
     *
     * @param aMessage  IPC message
     * @param aError    Error code
     */
	void ServiceError(const RMessage2& aMessage, TInt aError);
	
	// Own: reference to the currently opened slot
	CSlot* iSlot;
    
    // Own: reference to the currently active enumerator
	CEnumerator* iEnumerator;
	};

#endif
