/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  WMDRM Server implementation
*
*/

#include <bacntf.h>
#include <driveinfo.h>
#include "wmdrmsession.h"
#include "wmdrmserver.h"
#include "clock.h"
#include "slotdatacache.h"
#include "slotenumeratorcache.h"
#include "wmdrmdatastore.h"
#include "wmdrmdb.h"
#include "drmrightsstoringlocation.h"

#define _LOGGING_FILE L"wmdrmserver.txt"

#include "flogger.h"
#include "logfn.h"

CWmDrmServer::CWmDrmServer():
    CServer2( CActive::EPriorityStandard,  ESharableSessions ),
    iCache( NULL ), iDriveNumber( -1 ), iWmDrmRightsConfigFound( EFalse )
    {
    }

CServer2* CWmDrmServer::NewLC()
    {
    LOGFN( "CWmDrmServer::NewLC" );
    CWmDrmServer* self = new( ELeave ) CWmDrmServer;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CWmDrmServer::~CWmDrmServer()
    {
    LOGFN( "CWmDrmServer::~CWmDrmServer" );
    delete iCache;
    delete iEnumeratorCache;
    delete iClock;
    delete iDb;
    delete iDataStore;
    iFs.Close();
    }

void CWmDrmServer::ConstructL()
    {
    LOGFN( "CWmDrmServer::ConstructL" );
    TDrmScheme drmScheme( EDrmSchemeWmDrm );
    TChar driveLetter;

    StartL( KWmDrmServerName );
    User::LeaveIfError( iFs.Connect() );
    iCache = NULL;
    iEnumeratorCache = NULL;
    ResetCacheL();
    iClock = CClock::NewL( this );
    iClock->Start();

    // Check which drive is configured in the Central Repository Key
    // for the desired storing location of license and sync stores.
    // Convert the drive letter to drive number and store it.
    iWmDrmRightsConfigFound =
        DrmRightsStoringLocation::CheckDrmRightsStorageDriveL( iFs,
            drmScheme, driveLetter );

    // Check also the system drive
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, iSystemDriveNumber );

    iFs.CharToDrive( driveLetter, iDriveNumber );

    iDb = CWmDrmDb::NewL( this );
    iDataStore = CWmDrmDataStore::NewL( this );
    }

void CWmDrmServer::ResetCacheL()
    {
    LOGFN( "CWmDrmServer::ResetCacheL" );
    if ( iCache )
        {
        delete iCache;
        iCache = NULL;
        }
    iCache = CSlotDataCache::NewL( this );
    if ( iEnumeratorCache )
        {
        delete iEnumeratorCache;
        iEnumeratorCache = NULL;
        }
    iEnumeratorCache = CSlotEnumeratorCache::NewL( this );
    }

CSession2* CWmDrmServer::NewSessionL(
    const TVersion& /*aVersion*/,
    const RMessage2& /*aMessage*/ ) const
    {
    LOGFN( "CWmDrmServer::NewSessionL" );
    return new( ELeave ) CWmDrmSession();
    }

CSlotDataCache* CWmDrmServer::Cache()
    {
    return iCache;
    }

CSlotEnumeratorCache* CWmDrmServer::EnumeratorCache()
    {
    return iEnumeratorCache;
    }

CClock* CWmDrmServer::Clock()
    {
    return iClock;
    }

RFs& CWmDrmServer::Fs()
    {
    return iFs;
    }

CWmDrmDataStore* CWmDrmServer::DataStore()
    {
    return iDataStore;
    }

CWmDrmDb* CWmDrmServer::Db()
    {
    return iDb;
    }

TInt64 CWmDrmServer::FreeSpaceL( TBool aConfiguredDrive )
    {
    TVolumeInfo info;
    if ( aConfiguredDrive )
        {
        // Check the disk space from the configured drive
        User::LeaveIfError( iFs.Volume( info, iDriveNumber ) );
        }
    else
        {
        // Check the disk space from the system drive
        User::LeaveIfError( iFs.Volume( info, iSystemDriveNumber ) );
        }
    return info.iFree;
    }

static void RunServerL()
    {
    LOGFN( "RunServerL" );

    User::LeaveIfError( RThread::RenameMe( KWmDrmServerName ) );
    CActiveScheduler* s = new( ELeave ) CActiveScheduler;
    CleanupStack::PushL( s );
    CActiveScheduler::Install( s );
    CWmDrmServer::NewLC();
    RProcess::Rendezvous( KErrNone );
    CActiveScheduler::Start();
    CleanupStack::PopAndDestroy( 2 ); // server, s
    }

TInt E32Main()
    {
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TInt r = KErrNoMemory;
    if ( cleanup )
        {
        TRAP( r, RunServerL() );
        delete cleanup;
        }
    __UHEAP_MARKEND;
    return r;
    }
