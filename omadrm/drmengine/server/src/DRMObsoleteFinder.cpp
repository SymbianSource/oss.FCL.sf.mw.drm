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
#include <DcfRep.h>
#include "DRMCommon.h"  // DRM Error messages
#include "DRMObsoleteFinder.h"
#include "drmrightsdb.h"
#include "drmlog.h"
#include <DcfRep.h>

#include <sysutil.h>    // Disk space checking

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

const TInt KScanFileSystem = 1;
const TInt KScanContents = 2;
const TInt KRemoveUsedParents = 3;
const TInt KWriteTempFile = 4;

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES
LOCAL_C TInt CompareHBufC8( const HBufC8& aFirst, const HBufC8& aSecond );

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

LOCAL_C TInt CompareHBufC8( const HBufC8& aFirst, const HBufC8& aSecond )
    {
    return aFirst.Compare( aSecond );
    };

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CDRMObsoleteFinder::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMObsoleteFinder* CDRMObsoleteFinder::NewL( RFs& aFs,
                                            CDRMRightsDB* aDatabase,
                                            TRequestStatus& aStatus,
                                            RWriteStream& aStream,
                                            TBool aPerformScan )
    {
    CDRMObsoleteFinder* self = new( ELeave ) CDRMObsoleteFinder( aFs,
                                                        aDatabase, aStatus,
                                                        aStream );
    CleanupStack::PushL( self );
    self->ConstructL( aPerformScan );
    CleanupStack::Pop();

    return self;
    }


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CDRMObsoleteFinder::~CDRMObsoleteFinder()
    {
    // just in case
    Cancel();

    if( iDcfClient )
        {
        delete iDcfClient;
        iDcfClient = NULL;
        }

    if( iContents )
        {
        delete iContents;
        iContents = NULL;
        }

    if( iParents )
        {
        delete iParents;
        iParents = NULL;
        }

    if( iNoContents )
        {
        delete iNoContents;
        iNoContents = NULL;
        }
    };

// -----------------------------------------------------------------------------
// CDRMObsoleteFinder::ExecuteCleanupLD
// -----------------------------------------------------------------------------
//
void CDRMObsoleteFinder::ExecuteFinderLD()
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
CDRMObsoleteFinder::CDRMObsoleteFinder( RFs& aFs,
                                        CDRMRightsDB* aDatabase,
                                        TRequestStatus& aStatus,
                                        RWriteStream& aStream ) :
    CActive( EPriorityLow ),
    iContents( NULL ),
    iParents( NULL ),
    iNoContents( NULL ),
    iIndex( -1 ),
    iFileServer( aFs ),
    iStream( aStream ),
    iRightsDb( aDatabase ),
    iOperationStatus( aStatus ),
    iState( KScanFileSystem )
    {

    };


// -----------------------------------------------------------------------------
// CDRMObsoleteFinder::ConstructL
// -----------------------------------------------------------------------------
//
void CDRMObsoleteFinder::ConstructL( const TBool aPerformScan )
    {
    // if the scan needs to be done, the initial state is different:
    if( aPerformScan )
        {
        iState = KScanFileSystem;
        }
    else
        {
        iState = KScanContents;
        }

    // connect to dcf repository
    iDcfClient = CDcfRep::NewL();

    // Create a new list
    iContents = CDRMPointerArray<HBufC8>::NewL();

    // Create a new list
    iParents = CDRMPointerArray<HBufC8>::NewL();

    // Create a new list
    iNoContents = CDRMPointerArray<HBufC8>::NewL();

    };

// -----------------------------------------------------------------------------
// CDRMObsoleteFinder::RunError
// -----------------------------------------------------------------------------
//
TInt CDRMObsoleteFinder::RunError(TInt aError)
    {
    TRequestStatus* status = 0;

    if( aError != KErrNone )
        {
        status = &iOperationStatus;
        User::RequestComplete( status, aError );
        delete this;
        }
    return KErrNone;
    };

// -----------------------------------------------------------------------------
// CDRMObsoleteFinder::RunL
// -----------------------------------------------------------------------------
//
void CDRMObsoleteFinder::RunL()
    {
    TRequestStatus* status = 0;
    TInt error = KErrNone;
    HBufC8* buffer = NULL;

    // If the status of the cleaning is other than KErrNone
    User::LeaveIfError( iStatus.Int() );

    switch( iState )
        {
        case KScanFileSystem:
            iState = KScanContents;
            iDcfClient->RefreshDcf( iStatus );
            SetActive();
            break;
        case KScanContents:
            if( iIndex == -1 )
                {
                // Get the contents
                iRightsDb->GetContentIDListL( *iContents );
                // Reset the index
                iIndex = 0;
                }
            else
                {
                iIndex++;
                }

            if( iIndex >= iContents->Count() )
                {
                iState = KRemoveUsedParents;
                iIndex = -1;
                }
            else
                {
                // Check if there is content
                TRAP( error, iDcfClient->OrderListL( *(*iContents)[iIndex] ));

                // If an error occurs, leave if it's ok, continue
                if( error != KErrNotFound )
                    {
                    User::LeaveIfError( error );

                    // Get all the parents
                    if( !error )
                        {
                        GetParentsL( *(*iContents)[iIndex], *iParents );
                        }
                    }
                // If the error is not found, add to the no content list
                else
                    {
                    buffer = (*iContents)[iIndex]->AllocLC();
                    iNoContents->AppendL( buffer );
                    CleanupStack::Pop();
                    }
                }
            SetActive();
            status = &iStatus;
            User::RequestComplete(status,KErrNone);
            break;
        case KRemoveUsedParents:
            if( iIndex == -1 )
                {
                iIndex = 0;
                }
            else
                {
                iIndex++;
                }

            if( iIndex >= iParents->Count() )
                {
                iState = KWriteTempFile;
                iIndex = -1;
                }
            else
                {
                // Find the parent
                error = iNoContents->FindInOrder( (*iParents)[iIndex],
                                       TLinearOrder<HBufC8>(CompareHBufC8));

                if( error != KErrNotFound )
                    {
                    buffer = (*iNoContents)[error];
                    iNoContents->Remove( error );
                    delete buffer;
                    buffer = 0;
                    }
                }
            SetActive();
            status = &iStatus;
            User::RequestComplete(status,KErrNone);
            break;
        case KWriteTempFile:
            ObsoleteToStreamL();
            // we are complete:
            status = &iOperationStatus;
            User::RequestComplete( status, KErrNone );
            delete this;
            return;
        default:
            // illegal status, return error and delete object
            status = &iOperationStatus;
            User::RequestComplete( status, KErrGeneral );
            delete this;
            return;
        }
    };


// ----------------------------------------------------------------------------
// CDRMObsoleteFinder::GetParentsL
// ----------------------------------------------------------------------------
//
void CDRMObsoleteFinder::GetParentsL( const TDesC8& aContentId,
                                      RPointerArray<HBufC8>& aParents )
    {
    HBufC8* parentId = NULL;
    TInt error = KErrNone;
    CDRMPointerArray<CDRMPermission>* permissions =
      CDRMPointerArray<CDRMPermission>::NewLC();
    permissions->SetAutoCleanup( ETrue );
    CDRMPointerArray<CDRMPermission>& perm = *permissions;

    TRAP( error, iRightsDb->GetDBEntryByContentIDL( aContentId, *permissions ) );

    // If there are no keys it means that there is encryption key and such, but
    // no available permissions
    if( error == KErrCANoRights )
        {
        CleanupStack::PopAndDestroy(); // permissions
        return;
        }
    else
        {
        User::LeaveIfError(error);
        }

    for( TInt i = 0; i < permissions->Count(); i++, error = KErrNone )
        {
        // Check if the permission has a parent
        if( perm[i]->iParentUID )
            {
            // if it does, insert it to the aParents array
            error = aParents.FindInOrder( perm[i]->iParentUID,
                                          TLinearOrder<HBufC8>(CompareHBufC8));
            if( error == KErrNotFound )
                {
                parentId = perm[i]->iParentUID->AllocLC();
                User::LeaveIfError( aParents.InsertInOrder(parentId,
                                    TLinearOrder<HBufC8>(CompareHBufC8)) );
                CleanupStack::Pop();
                }
            }
        }

    CleanupStack::PopAndDestroy(); // permissions
    };


// -----------------------------------------------------------------------------
// CDRMObsoleteFinder::ObsoleteToStreamL
// -----------------------------------------------------------------------------
//
void CDRMObsoleteFinder::ObsoleteToStreamL()
    {
    TInt count( 0 );


    TInt size( 4 ); // size of the count
    for( count = 0; count < iNoContents->Count(); count++ )
        {
        size += (*iNoContents)[ count ]->Size();
        size += sizeof(TUint16);
        }

    // Reset count variable:
    count = 0;

#ifndef RD_MULTIPLE_DRIVE

    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFileServer,
                                                size,
                                                EDriveC ) )

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );

    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFileServer,
                                                size,
                                                driveNumber ) )

#endif

        {
        DRMLOG( _L( "CDRMDbSession::UriListToFileL: KErrDiskFull" ) );
        User::Leave( KErrDiskFull );
        }
    // Write the whole stuff into the file.
    while( count < iNoContents->Count() )
        {
        iStream.WriteUint16L( (*iNoContents)[count]->Length() );
        iStream.WriteL( *(*iNoContents)[count] );
        ++count;
        }
    // Finish with a 0
    iStream.WriteUint16L( 0 );

    iStream.CommitL();
    }

// -----------------------------------------------------------------------------
// CDRMObsoleteFinder::DoCancel
// -----------------------------------------------------------------------------
//
void CDRMObsoleteFinder::DoCancel()
    {
    };

// -----------------------------------------------------------------------------
// CDRMObsoleteFinder::DoCleanup
// -----------------------------------------------------------------------------
//
void CDRMObsoleteFinder::DoCleanup()
    {
    TRequestStatus* status = 0;
    if( iCancel <= 0 )
        {
        iCancel = 1;
        status = &iStatus;
        User::RequestComplete(status, KErrCancel);
        }
    };








