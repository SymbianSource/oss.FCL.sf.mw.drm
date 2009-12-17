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
* Description:  Server definitions
*
*/


#ifndef __WMDRMSERVER_H
#define __WMDRMSERVER_H

#include <e32base.h>
#include <f32file.h>
#include "wmdrmclient.h"
#include "wmdrmclientserver.h"

class CSlotDataCache;
class CSlotEnumeratorCache;
class CClock;
class CWmDrmDataStore;
class CWmDrmDb;

// Max length of the slot name
const TInt KMaxSlotNameSize = KMaxWmDrmStoreNameSize + KMaxWmDrmNamespaceNameSize + 2 * KWmDrmIdSize + 4 * sizeof( TInt );

// Length of the AES key (128 bits)
const TInt KAESKeyLength = 16;

// Max number of cached enumerators
const TInt KDefaultMaxCachedEnumerators = 16;

// Max number of cached slots
const TInt KDefaultMaxCachedSlots = 16;

// Max size of the data in a slot
const TInt KMaxSlotSize = 64 * 1024;

// Private directory of the WMDRM server
_LIT( KPrivateDir, "%c:\\private\\10282F1B\\" );

// Filename for the last-known-good time
_LIT( KTimeSaverFile, "%c:\\private\\10282F1B\\clock.dat" );

// Filename for the initial free space
_LIT( KInitialFreeSpaceFile, "%c:\\private\\10282F1B\\freespace.dat" );

// Filename for the dummy db file
_LIT( KDummyDbFile, "%c:\\private\\10282F1B\\dummydb.dat" );

// Default store, sometimes the porting kit does not pass a store name,
// this is used instead.
_LIT( KDefaultStore, "hds" );

/* WMDRM Server class, implements slot and namespace access functionality
 */
class CWmDrmServer : public CServer2
	{
public:
    /* Create a server object and pushes it onto the cleanup stack.
     *
     * @return Server object
     */
	static CServer2* NewLC();

    /* Destructor.
     */
	~CWmDrmServer();


    /* Returns the slot cache object.
     *
     * @return Slot cache
     */
    CSlotDataCache* Cache();

    /* Returns the enumerator cache object.
     *
     * @return Enumerator cache
     */
    CSlotEnumeratorCache* EnumeratorCache();

    /* Returns the anti-rollback clock object.
     *
     * @return Clock object
     */
    CClock* Clock();

    /* Returns a shared file server reference.
     *
     * @return Shared file server
     */
    RFs& Fs();
    
    /* Returns the data store object.
     *
     * @return Data store object
     */
    CWmDrmDataStore* DataStore();
    
    /* Returns the database object.
     *
     * @return Database object
     */
    CWmDrmDb* Db();
    
    /* Flushes and empties the slot and enumerator caches.
     */
    void ResetCacheL();

    /* Returns the amount of free space either on the system 
     * drive or the configured drive for WM DRM rights storage
     * @return Amount of free space on system drive
     */
    TInt64 FreeSpaceL( TBool aConfiguredDrive );

private:

    /* Private constructor.
     */
	CWmDrmServer();

    /* Second phase constructor.
     */
	void ConstructL();

    /* Creates a new session.
     *
     * @param aVersion  Client version
     * @param aMessage  IPC message
     * @return New session
     */
	CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;

    // Own: Slot cache
	CSlotDataCache* iCache;
    
    // Own: Enumerator cache
	CSlotEnumeratorCache* iEnumeratorCache;
    
    // Own: Anti-rollback clock
	CClock* iClock;
    
    // Own: File server
	RFs iFs;
	
	// Own: Data store
	CWmDrmDataStore* iDataStore;
	
	// Own: Database
	CWmDrmDb* iDb;
	
	// Drive number for the drive to be used as a storage of WMDRM rights
	TInt iDriveNumber;
	
	// Default system drive number
	TInt iSystemDriveNumber;
	
	// Whether WM DRM rights are configured to be (partly) stored to an internal
	// drive
	TBool iWmDrmRightsConfigFound;
	
	};

#endif
