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

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include "dbwatcher.h"
#include "dirwatcher.h"
#include "drmlog.h"

#ifdef RD_MULTIPLE_DRIVE
_LIT( KRdbBaseDir, "%c:\\private\\101f51f2\\rdb\\");
#else
_LIT( KRdbBaseDir, "c:\\private\\101f51f2\\rdb\\");
#endif

_LIT( KDomainRoDir, "DomainROs" );
static const TChar KDirs[CDbWatcher::KNumDirs] =
    { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

// ======== MEMBER FUNCTIONS ========

void CDbWatcher::ConstructL( MWatcherObserver& aObserver )
    {
    TInt i;
    TFileName file;

    DRMLOG( _L( "CDbWatcher::ConstructL <-" ) );

    User::LeaveIfError( iFs.Connect() );
    for ( i = 0; i < KNumDirs; i++ )
        {

#ifndef RD_MULTIPLE_DRIVE

        // Ignore errors
        file.Copy( KRdbBaseDir );

#else //RD_MULTIPLE_DRIVE

        TInt driveNumber( -1 );
        TChar driveLetter;
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
        iFs.DriveToChar( driveNumber, driveLetter );

        TFileName dbBaseDir;
        dbBaseDir.Format( KRdbBaseDir, (TUint)driveLetter );

        // Ignore errors
        file.Copy( dbBaseDir );

#endif

        if ( i < KNumDirs - 1 )
            {
            file.Append( KDirs[i] );
            }
        else
            {
            file.Append( KDomainRoDir );
            }
        iDirWatcher[i] = CDirWatcher::NewL( aObserver, iFs, file );
        }

    DRMLOG( _L( "CDbWatcher::ConstructL ->" ) );
    }

CDbWatcher* CDbWatcher::NewL( MWatcherObserver& aObserver )
    {
    CDbWatcher* self = new( ELeave) CDbWatcher;
    self->ConstructL( aObserver );
    return self;
    }

CDbWatcher::CDbWatcher()
    {
    DRMLOG( _L( "CDbWatcher::CDbWatcher ->" ) );
    DRMLOG( _L( "CDbWatcher::CDbWatcher <-" ) );
    }

CDbWatcher::~CDbWatcher()
    {
    TInt i;

    DRMLOG( _L( "CDbWatcher::~CDbWatcher ->" ) );

    for ( i = 0; i < KNumDirs; i++ )
        {
        delete iDirWatcher[i];
        }
    iFs.Close();

    DRMLOG( _L( "CDbWatcher::~CDbWatcher <-" ) );
    }

void CDbWatcher::StartWatching()
    {
    TInt i;

    DRMLOG( _L( "CDbWatcher::StartWatching ->" ) );

    for ( i = 0; i < KNumDirs; i++ )
        {
        iDirWatcher[i]->StartWatching();
        }

    DRMLOG( _L( "CDbWatcher::StartWatching <-" ) );
    }
