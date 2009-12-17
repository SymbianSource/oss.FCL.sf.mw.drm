/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of the CDbWatcher class
*
*/


#ifndef DBWATCHER_H
#define DBWATCHER_H

#include "dirwatcher.h"
#include "watcherobserver.h"

/**
 *  Watches the rights database to detect adding or removing of files
 *
 *  Uses change notifications on the RDB directories, restarts the device if
 *  unauthorized modifications have been detected
 *
 *  @since S60 3.0
 */
NONSHARABLE_CLASS( CDbWatcher ): public CBase
    {

public:

    /**  Number of directories to watch */
    static const TInt KNumDirs = 17;

    static CDbWatcher* NewL( MWatcherObserver& aObserver );

    virtual ~CDbWatcher();

    /**
     * Start watching the database
     *
     * @since S60 3.0
     */
    void StartWatching();

protected:

// from base class CActive

    /**
     * From CActive.
     * Cancel outstanding requests
     *
     * @since S60 3.0
     */
    void DoCancel();
    
    /**
     * From CActive.
     * Handle events
     *
     * @since S60 3.0
     */
    void RunL();
    
private:

    CDbWatcher();

    void ConstructL( MWatcherObserver& aObserver );

private: // data

    /**
     * Actual watcher objects
     */
    CDirWatcher* iDirWatcher[KNumDirs]; 
    
    /**
     * Array of request status fields
     */
    TRequestStatus iRequestStatus[KNumDirs];

    /**
     * File server instance
     */
    RFs iFs;
    };

#endif // DBWATCHER_H
