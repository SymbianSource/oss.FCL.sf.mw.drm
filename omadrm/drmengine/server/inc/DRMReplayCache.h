/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class declares the interface of class CDRMDbSession.
*
*/


#ifndef DRMREPLAYCACHE_H
#define DRMREPLAYCACHE_H

//  INCLUDES
#include <e32std.h>
#include <d32dbms.h>

// CONSTANTS
// MACROS
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CPersistentStore;

// DATA TYPES

// CLASS DECLARATION

/**
*  Rights server's replay cache.
*  This class manages storing information about the latest stored permissions.
*
*  @lib RightsServer.exe
*  @since S60Rel3.0
*/
NONSHARABLE_CLASS( RDRMReplayCache )
    {
    public: // Constructors and destructor

    /**
    * Constructor.
    * @since S60Rel3.0
    */
    RDRMReplayCache( RFs& aFs );

    /**
    * Default constructor.
    * @since S60Rel3.0
    */
    RDRMReplayCache();

    public: // New functions

    /**
    * Sets the ifs according to this reference.
    *
    * @since S60Rel3.0
    * @param aFs Reference to open file server session.
    */
    void Set( RFs& aFs );

    /**
    * Closes the cache.
    * @since S60Rel3.0
    */
    void Close();

    /**
    * Initializes the view.
    * @since S60Rel3.0
    * @param aTimedDb File name of the db with timestamped permissions.
    * @param aPlainDb File name of the db with permissions without timestamps.
    */
    void InitL( const TDesC& aTimedDb,
                const TDesC& aPlainDb );

    /**
    * Destructor.
    * @since S60Rel3.0
    */
    ~RDRMReplayCache();

    /**
    * Add an entry to the DB. Overloaded.
    * @since S60Rel3.0
    * @param aID    Content-ID.
    * @param aaTime Timestamp of the permission.
    * @param aaTime Insertion time.
    */
    void AddL( const TDesC8& aID,
               const TTime& aTime,
               const TTime& aInsertionTime  );

    /**
    * Add an entry to the DB. Overloaded.
    * @since S60Rel3.0
    * @param aID    Content-ID.
    * @param aaTime Insertion time.
    */
    void AddL( const TDesC8& aID,
               const TTime& aInsertionTime  );

    /**
    * Checks whether an entry is in cache. Overloaded.
    * @since S60Rel3.0
    * @param aID    Content-ID.
    * @param aaTime Timestamp of the permission.
    * @return Boolean.
    */
    TBool InCacheL( const TDesC8& aCID,
                    const TTime& aTime );

    /**
    * Checks whether an entry is in cache. Overloaded.
    * @since S60Rel3.0
    * @param aID    Content-ID.
    * @return Boolean.
    */
    TBool InCacheL( const TDesC8& aCID );

    private: // Constants
    enum TDatabaseId
        {
        ETimeDb,
        EPlainDb
        };

    private: // Private functions

    /**
    * Initializes the database.
    * @since S60Rel3.0
    * @param aDb Database.
    * @param aFileName File name.
    * @param aId Database ID.
    */
    void InitDbL( RDbNamedDatabase& aDb,
                  const TDesC& aFileName,
                  TDatabaseId aId );

    /**
    * Opens a database.
    * @since S60Rel3.0
    * @param aDb Database.
    * @param aFileName File name.
    */
    void OpenDbL( RDbNamedDatabase& aDb,
                  const TDesC& aFileName );

    /**
    * Replace an existing database.
    * @since S60Rel3.0
    * @param aDb Database.
    * @param aFileName File name.
    * @param aId Database ID.
    */
    void ReplaceDbL( RDbNamedDatabase& aDb,
                     const TDesC& aFileName,
                     TDatabaseId aId );

    /**
    * Initializes the view.
    * @since S60Rel3.0
    * @param aView View to initialize.
    * @param aDb Database ID.
    */
    void InitViewLC( RDbView& aView,
                     TDatabaseId aDb,
                     TBool aUpdate );

    /**
    * Compare the given ID to the ID in the current position of the view.
    * @since S60Rel3.0
    * @param aView View.
    * @param aCID Content-ID to check.
    * @return Boolean.
    */
    TBool CompareCIDL( RDbRowSet& aView, const TDesC8& aCID );

    /**
    * Push a cleanup item to the cleanup stack in order to rollback
    * the database.
    * @since S60Rel3.0
    * @param aDb Database.
    */
    void PushL( RDbDatabase& aDb );

    /**
    * Pop the cleanup item pushed in by PushL
    * @since S60Rel3.0
    */
    void Pop();

    /**
    * Delete items starting from the oldest one.
    * @since S60Rel3.0
    * @param aView View.
    * @param aHowMany How many items to delete.
    */
    void DeleteOldestsL( RDbRowSet& aView, TInt16 aHowMany );

    /**
    * Compacts the view to the upper limit of the view capacity.
    * @since S60Rel3.0
    * @param aView View to compact.
    */
    void CompactViewL( RDbRowSet& aView );

    /**
    * Insert a content-id to the current position in the view.
    * @since S60Rel3.0
    * @param aView View to which the insertion is made.
    * @param aId ID to add.
    */
    void InsertIdL( RDbRowSet& aView,
                    const TDesC8& aId );

    private: // Data
    // File server session handle. Not owned by this object.
    RFs* iFs;

    // Database for timestamped items.
    RDbNamedDatabase iTimeDb;

    // Database for the items without timestamp.
    RDbNamedDatabase iPlainDb;
    // RDbView iView;
    };

#endif //DRMREPLAYCACHE_H