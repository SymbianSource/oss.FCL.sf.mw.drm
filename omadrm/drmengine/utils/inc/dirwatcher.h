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
* Description:  Declaration of the CDirWatcher class
*
*/


#ifndef DIRWATCHER_H
#define DIRWATCHER_H

#include "watcherobserver.h"

/**  Prefix used when signaling the observer which directory has changed */
_LIT( KDirIdentifier, "RDB: " );

/**
 *  Watches a single directory
 *
 *  Uses change notifications on the RDB directories, restarts the device if
 *  unauthorized modifications have been detected
 *
 *  @since S60 3.0
 */
NONSHARABLE_CLASS( CDirWatcher ): public CActive
    {
public:

    static CDirWatcher* NewL( MWatcherObserver& aObserver, RFs& aFs, const TDesC& aDir );

    virtual ~CDirWatcher();

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

    CDirWatcher( MWatcherObserver& aObserver, RFs& aFs );

    void ConstructL( const TDesC& aDir );

private: // data

    RFs& iFs;
    TFileName iDir;
    MWatcherObserver& iObserver;
    };

#endif // DIRWATCHER_H
