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
* Description:  Declaration of the CProcWatcher class
*
*/


#ifndef PROCWATCHER_H
#define PROCWATCHER_H

#include "watcherobserver.h"

/**  Prefix used when signaling the observer which process was killed */
_LIT( KProcIdentifier, "PROC: " );

/**
 *  Watches a process
 *
 *  Logs onto a specified process and signals an observer when that process dies.
 *
 *  @since S60 3.0
 */
NONSHARABLE_CLASS( CProcWatcher ): public CActive
    {
public:

    static CProcWatcher* NewL( MWatcherObserver& aObserver, const TDesC& aProcess, const TDesC& aFile );

    virtual ~CProcWatcher();

    /**
     * Start watching the process
     *
     * @since S60 3.0
     * @return KErrNone if watching started correctly
     */
    TInt StartWatching();

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

    CProcWatcher( MWatcherObserver& aObserver );

    void ConstructL( const TDesC& aProcess, const TDesC& aFile );

private: // data

    TFullName iProcessName;
    TFileName iFileName;
    RProcess iProcess;
    MWatcherObserver& iObserver;
    };

#endif // DIRWATCHER_H
