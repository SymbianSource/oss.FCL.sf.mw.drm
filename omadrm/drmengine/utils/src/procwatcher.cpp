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
* Description:  Implementation of the process watcher
*
*/


#include <e32def.h>
#include <e32base.h>
#include <f32file.h>
#include <sysutil.h>
#include "procwatcher.h"
#include "drmlog.h"

// ======== MEMBER FUNCTIONS ========

void CProcWatcher::ConstructL( const TDesC& aProcess, const TDesC& aFile )
    {
    DRMLOG( _L( "CProcWatcher::ConstructL <-" ) );

    iProcessName.Copy( aProcess );
    iFileName.Copy( aFile );
    CActiveScheduler::Add( this );
    
    DRMLOG( _L( "CProcWatcher::ConstructL ->" ) );
    }

CProcWatcher* CProcWatcher::NewL( MWatcherObserver& aObserver, const TDesC& aProcess, const TDesC& aFile )
    {
    CProcWatcher* self = new( ELeave ) CProcWatcher( aObserver);
    self->ConstructL( aProcess, aFile );
    return self;
    }

CProcWatcher::CProcWatcher( MWatcherObserver& aObserver ):
    CActive( EPriorityStandard ),
    iObserver( aObserver )
    {
    DRMLOG( _L( "CProcWatcher::CProcWatcher ->" ) );
    DRMLOG( _L( "CProcWatcher::CProcWatcher <-" ) );
    }

CProcWatcher::~CProcWatcher()
    {
    DRMLOG( _L( "CProcWatcher::~CProcWatcher ->" ) );
    
    Cancel();
    
    DRMLOG( _L( "CProcWatcher::~CProcWatcher <-" ) );
    }

TInt CProcWatcher::StartWatching()
    {
    TFullName name;
    TFindProcess finder( iProcessName );
    TInt r = KErrNone;
    
    DRMLOG( _L( "CProcWatcher::StartWatching ->" ) );
    DRMLOG( iProcessName );
    
    r = finder.Next( name );
    if ( r != KErrNone )
        {
        r = iProcess.Create( iFileName, KNullDesC );
        if ( r == KErrNone )
            {
            iProcess.Resume();
            }
        }
    else
        {
        iProcessName.Copy( name );
        DRMLOG( iProcessName );
        r = iProcess.Open( iProcessName );
        }
        
    if ( r == KErrNone )
        {
        iProcess.Logon( iStatus );
        SetActive();
        DRMLOG( _L( "Active " ) );
        }

    DRMLOG( _L( "CProcWatcher::StartWatching <-" ) );
    return r;
    }
    
void CProcWatcher::DoCancel()
    {
    DRMLOG( _L( "CProcWatcher::DoCancel ->" ) );
    
    DRMLOG( _L( "CProcWatcher::DoCancel <-" ) );
    }
    
void CProcWatcher::RunL()
    {
    TBuf<256> logBuffer;
    TFileName object;
    
    DRMLOG( _L( "CProcWatcher::RunL ->" ) );
    
    DRMLOG( iProcessName );
    logBuffer.AppendNum( iStatus.Int() );
    logBuffer.Append( ' ' );
    logBuffer.AppendNum( iProcess.ExitType() );
    DRMLOG( logBuffer );
    if ( iProcess.ExitType() != EExitPending )
        {
        object.Copy( KProcIdentifier );
        object.Append( iProcessName );
        iObserver.WatchedObjectChangedL( object );
        }
    else
        {
        if ( iProcess.Open( iProcessName ) == KErrNone )
            {
            iProcess.Logon( iStatus );
            }
        }
    SetActive();
    
    DRMLOG( _L( "CProcWatcher::RunL <-" ) );
    }
