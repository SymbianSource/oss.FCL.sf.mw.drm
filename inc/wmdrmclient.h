/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Client definitions
*
*/


#ifndef __WMDRMCLIENT_H
#define __WMDRMCLIENT_H

#include <e32std.h>
#include <f32file.h>

// Name of the WMDRM server executable
_LIT(KWmDrmServerName,"!wmdrmserver.exe");

// Name of the WMDRM server
_LIT(KWmDrmServerImg,"wmdrmserver");

const TInt KWmDrmIdSize = 16;
const TInt KMaxWmDrmNamespaceNameSize = 64;
const TInt KMaxWmDrmStoreNameSize = 64;

enum TWmDrmStoreState
	{
    EStoreSpaceOK,
    EStoreSpaceLow,
    EStoreSpaceFull
    };

/**
* wmdrmclient.dll
*
* All methods may not be supported and will always
* return KErrNotSupported
*/
class RWmDrmClient: public RSessionBase
	{
public:
    /**
    * Connect()
    * Connect to the server
    *
    * @return KErrNone if successful    
    *         Otherwise Symbian OS error codes
    */
	IMPORT_C TInt Connect();

    /**
    * InitStore
    * Initialize a hds store
    *
    * @param aStoreName         Name of the store
    * @param aCreateIfMissing   Boolean value to create 
    *                           if the store is missing
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
    
    IMPORT_C TInt InitStore(
        const TDesC8& aStoreName,
        TBool aCreateIfMissing );

    /**
    * RemoveStore
    * Removes the store
    *
    * @param aStoreName         Name of the store
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
            
    IMPORT_C TInt RemoveStore(
        const TDesC8& aStoreName );

    /**
    * InitNamespace
    * Initializes a namespace
    *
    * @param aStoreName         Name of the store
    * @param aNamespace         Name of the namespace
    * @param aCreateIfMissing   Boolean value to create 
    *                           if the namespace is missing
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
    
    IMPORT_C TInt InitNamespace(
        const TDesC8& aStoreName,
        const TDesC8& aNamespace,
        TBool aCreateIfMissing );
 
     /**
    * RemoveNamespace
    * Removes a namespace
    *
    * @param aStoreName         Name of the store
    * @param aNamespace         Name of the namespace
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
       
    IMPORT_C TInt RemoveNamespace(
        const TDesC8& aStoreName,
        const TDesC8& aNamespace );

    /**
    * SlotOpen
    * Open a slot from the namespace
    *
    * @param aStoreName         Name of the store
    * @param aNamespace         Name of the namespace
    * @param aHashKey           Hash key part of the identifier         
    * @param aUniqueKey         Unique key part of the identifier
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
    
    IMPORT_C TInt SlotOpen(
        const TDesC8& aStoreName,
        const TDesC8& aNamespace,
        const TDesC8& aHashKey,
        const TDesC8& aUniqueKey,
        TInt& aSize );

    /**
    * SlotCreate
    * Create a slot to the namespace
    *
    * @param aStoreName         Name of the store
    * @param aNamespace         Name of the namespace
    * @param aHashKey           Hash key part of the identifier         
    * @param aUniqueKey         Unique key part of the identifier
    * @param aSize              Size of the slot to be created
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
            
    IMPORT_C TInt SlotCreate(
        const TDesC8& aStoreName,
        const TDesC8& aNamespace,
        const TDesC8& aHashKey,
        const TDesC8& aUniqueKey,
        TInt& aSize );

    /**
    * SlotSeek
    * Seek within a slot
    *
    * @param aPos               Requested position
    * @param aOrigin            Seekmode
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
            
    IMPORT_C TInt SlotSeek(
        TInt& aPos,
        TSeek aOrigin );

    /**
    * SlotRead
    * Read data from a slot
    *
    * @param aData              Slot data
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
            
    IMPORT_C TInt SlotRead(
        TDes8& aData );

    /**
    * SlotWrite
    * Write data to the slot
    *
    * @param aData              Slot data
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
            
    IMPORT_C TInt SlotWrite(
        const TDesC8& aData );

    /**
    * SlotResize
    * Resize the slot
    *
    * @param aSize              New size of the slot
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
            
    IMPORT_C TInt SlotResize(
        TInt aSize );

    /**
    * SlotDelete
    * Delete the slot identified by the parameters
    *
    * @param aStoreName         Name of the store
    * @param aNamespace         Name of the namespace
    * @param aHashKey           Hash key part of the identifier         
    * @param aUniqueKey         Unique key part of the identifier
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
            
    IMPORT_C TInt SlotDelete(
        const TDesC8& aStoreName,
        const TDesC8& aNamespace,
        const TDesC8& aHashKey,
        const TDesC8& aUniqueKey );

    /**
    * SlotClose
    * Close the current slot in use
    *
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
                
    IMPORT_C TInt SlotClose();

    /**
    * EnumerateStart
    * Initializes an enumeration
    *
    * @param aStoreName         Name of the store
    * @param aNamespace         Name of the namespace
    * @param aHashKey           Haskey to be used
    *
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
        
    IMPORT_C TInt EnumerateStart(
        const TDesC8& aStoreName,
        const TDesC8& aNamespace,
        const TDesC8& aHashKey );

    /**
    * EnumerateReload
    * Reloads an enumeration
    *
    * @param aStoreName         Name of the store
    * @param aNamespace         Name of the namespace
    * @param aHashKey           Hash key part of the identifier         
    * @param aUniqueKey         Unique key part of the identifier
    *
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
            
    IMPORT_C TInt EnumerateReload(
        TDes8& aStoreName,
        TDes8& aNamespace,
        TDes8& aHashKey,
        TDes8& aUniqueKey );

    /**
    * EnumerateNext
    * Moves to the next enumeration part
    *
    * @param aStoreName         Name of the store
    * @param aNamespace         Name of the namespace
    * @param aHashKey           Hash key part of the identifier         
    * @param aUniqueKey         Unique key part of the identifier
    *
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
            
    IMPORT_C TInt EnumerateNext(
        TDes8& aStoreName,
        TDes8& aNamespace,
        TDes8& aHashKey,
        TDes8& aUniqueKey );

    /**
    * EnumerateDeleteCurrent
    * Deletes the current enumerated slot
    *
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
            
    IMPORT_C TInt EnumerateDeleteCurrent();

    /**
    * EnumerateEnd
    * Finishes the enumeration session
    *
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
        
    IMPORT_C TInt EnumerateEnd();

    /**
    * LogStats
    * Logs the stats of the server
    *
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
    
    IMPORT_C TInt LogStats();

    /**
    * EmptyCache
    * Empties the cache used by the wmdrmserver
    *
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
    
    IMPORT_C TInt EmptyCache();

    /**
    * TimeValid
    * returns the validity of the time
    *
    * @return ETrue if the time is Valid
    *         EFalse if the time is Invalid
    */
        
    IMPORT_C TBool TimeValid();

    /**
    * SetTimeAsValid
    * Sets the time as valid
    *
    * @param aValid             Validity of the time
    *                           ETrue if Valid
    *                           EFalse if Invalid
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
    
    IMPORT_C void SetTimeAsValid( TBool aValid );

    /**
    * DeleteRights
    * Deletes allowed things from the rights database
    * Requires Capabilities or SecureID:s    
    *
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */    
    // Requires Capabilities or SecureID:s
    IMPORT_C TInt DeleteRights();

    /**
    * StoreState
    * Get the store state
    *
    * @param aState        State of the store
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
        
    IMPORT_C TInt StoreState( TWmDrmStoreState& aState );

    /**
    * GetTime
    * Returns the time kept in the WMDRM server
    *
    * @param aTime              Time in UTC
    * @param aValid             ETrue if the time is secure
    *                           EFalse if the time is not secure
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
        
    IMPORT_C TInt GetTime( TTime& aTime, TBool& aValid );
    
    /**
    * CommitData
    * Commits any cached data to storage
    *
    * @return KErrNone if successful
    *         Otherwise Symbian OS error code
    */
       
    IMPORT_C TInt CommitData();    
    
    
	};

#endif
