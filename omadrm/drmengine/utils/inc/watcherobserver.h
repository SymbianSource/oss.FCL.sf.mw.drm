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
* Description:  Declaration of the Watcher Observer class
*
*/


#ifndef WATCHEROBSERVER_H
#define WATCHEROBSERVER_H

/**
 *  Abstract class for observers of watcher events, e.g. the RDB watcher
 *
 *  @since S60 3.0
 */
class MWatcherObserver
    {
public:
    /**
     * Callback called by the peer when a watched object (e.g. the RDB) has
     * been modified.
     *
     * @since S60 3.0
     * @param aObject Descriptor containing more information
     */
    virtual void WatchedObjectChangedL( const TDesC& aObject ) = 0;

    };

#endif // WATCHEROBSERVER_H
