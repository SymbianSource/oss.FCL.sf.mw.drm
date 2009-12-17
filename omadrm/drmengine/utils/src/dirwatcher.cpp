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
* Description:  Implementation of the RDB watcher
*
*/


#include <e32def.h>
#include <e32base.h>
#include <f32file.h>
#include <sysutil.h>
#include "dirwatcher.h"
#include "dbwatcher.h"
#include "drmlog.h"

static const TNotifyType KWatchTrigger = ENotifyEntry;

// ======== MEMBER FUNCTIONS ========

void CDirWatcher::ConstructL( const TDesC& aDir )
    {
    DRMLOG( _L( "CDirWatcher::ConstructL <-" ) );

    iDir.Copy( aDir );    
    CActiveScheduler::Add( this );
    
    DRMLOG( _L( "CDirWatcher::ConstructL ->" ) );
    }

CDirWatcher* CDirWatcher::NewL( MWatcherObserver& aObserver, RFs& aFs, const TDesC& aDir )
    {
    CDirWatcher* self = new( ELeave ) CDirWatcher( aObserver, aFs );
    self->ConstructL( aDir );
    return self;
    }

CDirWatcher::CDirWatcher( MWatcherObserver& aObserver, RFs& aFs ):
    CActive( EPriorityStandard ),
    iFs( aFs ),
    iObserver( aObserver )
    {
    DRMLOG( _L( "CDirWatcher::CDirWatcher ->" ) );
    DRMLOG( _L( "CDirWatcher::CDirWatcher <-" ) );
    }

CDirWatcher::~CDirWatcher()
    {
    DRMLOG( _L( "CDirWatcher::~CDirWatcher ->" ) );

    Cancel();
    
    DRMLOG( _L( "CDirWatcher::~CDirWatcher <-" ) );
    }

void CDirWatcher::StartWatching()
    {
    DRMLOG( _L( "CDirWatcher::StartWatching ->" ) );
    
    DRMLOG( iDir );
    Cancel();
    iFs.NotifyChangeCancel();
    iFs.NotifyChange( KWatchTrigger, iStatus, iDir );
    SetActive();

    DRMLOG( _L( "CDirWatcher::StartWatching <-" ) );
    }
    
void CDirWatcher::DoCancel()
    {
    DRMLOG( _L( "CDirWatcher::DoCancel ->" ) );
    
    iFs.NotifyChangeCancel();
    
    DRMLOG( _L( "CDirWatcher::DoCancel <-" ) );
    }
    
void CDirWatcher::RunL()
    {
    TBuf<256> logBuffer;
    TFileName object;
    
    DRMLOG( _L( "CDirWatcher::RunL ->" ) );
    
    DRMLOG( iDir );
    logBuffer.AppendNum( iStatus.Int() );
    DRMLOG( logBuffer );
    if ( iStatus == KErrNone )
        {
        object.Copy( KDirIdentifier );
        object.Append( iDir );
        iObserver.WatchedObjectChangedL( object );
        }
    iFs.NotifyChange( KWatchTrigger, iStatus, iDir );
    SetActive();
    
    DRMLOG( _L( "CDirWatcher::RunL <-" ) );
    }
