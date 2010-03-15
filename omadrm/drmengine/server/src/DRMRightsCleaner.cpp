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
* Description:  Implementation of the DRM Rights Database
*
*/



// INCLUDE FILES

#include <e32std.h>     // RPointerArray
#include <e32def.h>     // Type definitions

#include <caf/caf.h>
#include "DRMCommon.h"  // DRM Error messages
#include "DRMRightsCleaner.h"
#include "drmrightsdb.h"
#include "drmlog.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

const TInt KMaxDirs = 16;

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================
// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CDRMRightsCleaner::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMRightsCleaner* CDRMRightsCleaner::NewL( RFs& aFs,
                                            CDRMRightsDB* aDatabase,
                                            TRequestStatus& aStatus,
                                            const TDesC& aDatabasePath,
                                            const TTime& aTime )
    {
    CDRMRightsCleaner* self = new( ELeave ) CDRMRightsCleaner( aFs,
                                                        aDatabase, aStatus,
                                                        aDatabasePath, aTime );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CDRMRightsCleaner::~CDRMRightsCleaner()
    {
    // just in case
    Deque();

    if( iCurrentDirectory )
        {
        delete iCurrentDirectory;
        iCurrentDirectory = NULL;
        }
    };

// -----------------------------------------------------------------------------
// CDRMRightsCleaner::ExecuteCleanupLD
// -----------------------------------------------------------------------------
//
void CDRMRightsCleaner::ExecuteCleanupLD()
    {
    TRequestStatus* status = 0;

    if( !IsAdded() )
        {
        CActiveScheduler::Add(this);
        }
    if ( !IsActive() )
        {
        SetActive();
        }

    iOperationStatus = KRequestPending;
    status = &iStatus;
    User::RequestComplete(status,KErrNone);
    };

// -----------------------------------------------------------------------------
// Default Constructor - First phase.
// -----------------------------------------------------------------------------
//
CDRMRightsCleaner::CDRMRightsCleaner( RFs& aFs,
                                      CDRMRightsDB* aDatabase,
                                      TRequestStatus& aStatus,
                                      const TDesC& aDatabasePath,
                                      const TTime& aTime ) :
    CActive( EPriorityLow ),
    iFileServer( aFs ),
    iRightsDb( aDatabase ),
    iOperationStatus( aStatus ),
    iDatabasePath( aDatabasePath ),
    iExpirationTime( aTime ),
    iCurrentDirectory( NULL ),
    iDirIndex( 0 ),
    iCurrentFile( 0 )
    {

    };


// -----------------------------------------------------------------------------
// CDRMRightsCleaner::ConstructL
// -----------------------------------------------------------------------------
//
void CDRMRightsCleaner::ConstructL()
    {
    };

// -----------------------------------------------------------------------------
// CDRMRightsCleaner::RunError
// More or less just ignore all errors and call RunL again
// -----------------------------------------------------------------------------
//
TInt CDRMRightsCleaner::RunError(TInt aError)
    {
    TRequestStatus* status = 0;

    if( aError == KErrCancel )
        {
        // we are complete:
        status = &iOperationStatus;
        User::RequestComplete( status, KErrNone );
        delete this;
        return KErrNone;
        }

    SetActive();
    status = &iStatus;
    User::RequestComplete(status,KErrNone);

    return KErrNone;
    };

// -----------------------------------------------------------------------------
// CDRMRightsCleaner::RunL
// -----------------------------------------------------------------------------
//
void CDRMRightsCleaner::RunL()
    {
    TRequestStatus* status = 0;
    TInt error = KErrNone;
    TFileName path;
    TInt modIndex = 0;
    TInt removeFile = EFalse;

    // If the status of the cleaning is other than KErrNone
    User::LeaveIfError( iStatus.Int() );

    if( !iCurrentDirectory ||
        iCurrentFile >= iCurrentDirectory->Count() )
        {

        if( iDirIndex >= KMaxDirs )
            {
            // we are complete:
            status = &iOperationStatus;
            User::RequestComplete( status, KErrNone );
            delete this;
            return;
            }

        // if it exists, delete it
        if( iCurrentDirectory )
            {
            delete iCurrentDirectory;
            iCurrentDirectory = 0;
            }

        TFileName path = iDatabasePath;


        path.Append(iDirIndex < 10 ? iDirIndex + '0' : iDirIndex + 'a' - 10);
        path.Append('\\');


        error =  iFileServer.GetDir(path, KEntryAttDir, ESortNone, iCurrentDirectory);

        DRMLOG(_L("Entering directory:"));
        DRMLOG( path );

        // increase the dir counter
        iDirIndex++;
        iCurrentFile = 0;
        }

    if( !error && iCurrentDirectory->Count() )
        {
        modIndex = iDirIndex-1;

        path = iDatabasePath;
        path.Append(modIndex < 10 ? modIndex + '0' : modIndex + 'a' - 10);
        path.Append('\\');

        path.Append((*iCurrentDirectory)[iCurrentFile].iName);

        DRMLOG(_L("Checking file:"));
        DRMLOG( path );

        // increase the file counter
        iCurrentFile++;

        TRAP( error, removeFile = iRightsDb->DeleteExpiredL( path, iExpirationTime ) );
        if( error != KErrNone )
            {
            DRMLOG2( _L( "CDRMRightsCleaner: error %d cleaning:" ), error );
            DRMLOG( path );
            }
        else
            {
            if ( removeFile )
                {
                DRMLOG(_L("Deleting file:"));
                DRMLOG( path );
                iFileServer.Delete( path );
                }
            }
        }

    SetActive();
    status = &iStatus;
    User::RequestComplete(status, KErrNone);
    };

// -----------------------------------------------------------------------------
// CDRMRightsCleaner::DoCancel
// -----------------------------------------------------------------------------
//
void CDRMRightsCleaner::DoCancel()
    {
    };


// -----------------------------------------------------------------------------
// CDRMRightsCleaner::DoCleanup
// -----------------------------------------------------------------------------
//
void CDRMRightsCleaner::DoCleanup()
    {
    TRequestStatus* status = 0;
    if( iCancel <= 0 )
        {
        Cancel();
        iCancel = 1;
        SetActive();
        status = &iStatus;
        User::RequestComplete(status, KErrCancel);
        }
    };









