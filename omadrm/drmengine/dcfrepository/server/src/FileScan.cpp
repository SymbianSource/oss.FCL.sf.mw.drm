/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  server implementation
*
*/


// INCLUDE FILES
#include    <e32std.h>
#include    <e32base.h>
#include    <f32file.h>
#include    <DRMCommon.h>

#ifdef RD_MULTIPLE_DRIVE
#include    <driveinfo.h>
#endif

#include    "DcfRepSrv.h"
#include    "SearchLeaf.h"
#include    "FileScan.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
_LIT( KWma, ".wma" );
_LIT( KWmv, ".wmv" );
_LIT( KAsf, ".asf" );

#ifdef RD_MULTIPLE_DRIVE
_LIT( KIgnoreDir1, "%c:\\sys" );
_LIT( KIgnoreDir2, "%c:\\private\\101F51F2" );
#else
_LIT( KIgnoreDir1, "c:\\sys" );
_LIT( KIgnoreDir2, "c:\\private\\101F51F2" );
#endif

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES
#ifndef RD_MULTIPLE_DRIVE
LOCAL_C TBool IgnoreDir( TFileName& aDir );
#else // RD_MULTIPLE_DRIVE
LOCAL_C TBool IgnoreDir( RFs& aFs, TFileName& aDir );
#endif
// FORWARD DECLARATIONS

// ============================ LOCAL FUNCTIONS =================================

// ------------------------------------------------------------------------------
// Returns whether directory should be ignored or not
// ------------------------------------------------------------------------------
 #ifndef RD_MULTIPLE_DRIVE
LOCAL_C TBool IgnoreDir( TFileName& aDir )
    {
    if ( !aDir.CompareF( KIgnoreDir1 ) ||
         !aDir.CompareF( KIgnoreDir2 ) )
        {
        return ETrue;
        }
    return EFalse;
    }
#else // RD_MULTIPLE_DRIVE
LOCAL_C TBool IgnoreDir( RFs& aFs, TFileName& aDir )
    {
    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
    aFs.DriveToChar( driveNumber, driveLetter );

    TFileName ignore1;
    TFileName ignore2;

    ignore1.Format( KIgnoreDir1, (TUint)driveLetter );
    ignore2.Format( KIgnoreDir2, (TUint)driveLetter );

    if ( !aDir.CompareF( ignore1 ) ||
         !aDir.CompareF( ignore2 ) )
        {
        return ETrue;
        }
    return EFalse;
    }
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFileScan::CFileScan
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFileScan::CFileScan( RFs& aFs ) :
CActive( CActive::EPriorityStandard ),iServer(NULL),iFs(&aFs),iCurrentLeaf(NULL)
    {
    CleanInternal();
    }

// -----------------------------------------------------------------------------
// CFileScan::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFileScan::ConstructL()
    {
    TInt err = 0;
    if ( !iFs )
        {
        err = KErrArgument;
        }
    else
        {
        err = KErrNone;
        }
    User::LeaveIfError( err );
    }

// -----------------------------------------------------------------------------
// CFileScan::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFileScan* CFileScan::NewL( RFs& aFs )
    {
    CFileScan* self = new( ELeave ) CFileScan( aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// Destructor
CFileScan::~CFileScan()
    {
    CleanInternal();
    iServer = NULL;
    iFs = NULL;
    }



// -----------------------------------------------------------------------------
// CFileScan::DoCancel
// Function is called when the request is completed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CFileScan::DoCancel()
    {
    CleanInternal();
    }


// -----------------------------------------------------------------------------
// CFileScan::IsProtected
// Function returns whether the specific file is protected or not
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CFileScan::IsProtected( const TDesC& aFileName , TBool& aIsDCF )
    {
    TInt err = KErrNone;
    aIsDCF = EFalse;
    err = iServer->ProcessFile( aFileName , aIsDCF );
    if ( err && err != KErrNoMemory )
        {
        err = KErrNone;
        }
    return err;
    }


// -----------------------------------------------------------------------------
// CFileScan::RunL
// Function is called when the request is completed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CFileScan::RunL()
    {
    TInt err = KErrNone;
    if ( iSearching && iServer->State()!=EStateIdle )
        {
        err = SearchNext();
        if ( err == KErrCancel )
            {
            err = KErrNone;
            }
        if ( !err )
            {
            SetActive();
            TRequestStatus* status = &iStatus;
            User::RequestComplete( status , err );
            }
        }
    else
        {
        CleanInternal();
        iServer->CompleteScanning(err);
        Deque();
        }
    if ( err )
        {
        CleanInternal();
        iServer->CompleteScanning(err);
        Deque();
        }
    }



// -----------------------------------------------------------------------------
// CFileScan::SearchContent
// Function starts the active objects to search protected file through whole file system
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CFileScan::SearchContent( CDcfRepSrv* aServer )
    {
    TInt err = KErrNone;

    iServer = aServer;
    CActiveScheduler::Add( this );

    iSearching = ETrue;
    err = SearchNext();
    if ( err )
        {
        return err;
        }
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status , KErrNone );
    return err;
    }

// -----------------------------------------------------------------------------
// CFileScan::SearchDrive
// This function searches for the DCF files on target device
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CFileScan::SearchDrive()
    {
    _LIT( KDrive, "%c:");
    TDriveList drivelist;
    TChar driveLetter;
    TInt driveNumber = EDriveA - 1;
    TInt err = KErrNone;

#ifdef RD_MULTIPLE_DRIVE

    TInt ramDrive( -1 );
    TInt romDrive( -1 );

    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRam, ramDrive );
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, romDrive );

#endif

    err = iFs->DriveList( drivelist );

    if ( iLastPosition != KNullDesC )
        {
        driveLetter = iLastPosition[0];
        err = iFs->CharToDrive( driveLetter, driveNumber );
        }

    driveNumber++;

    for ( ; driveNumber < KMaxDrives ; driveNumber++ )
        {

#ifndef RD_MULTIPLE_DRIVE

        if ( driveNumber == EDriveD || driveNumber == EDriveZ )
            {
            }

#else // RD_MULTIPLE_DRIVE

        if ( driveNumber == ramDrive || driveNumber == romDrive )
            {
            }

#endif

        else if ( drivelist[driveNumber] )
                {
                err = iFs->DriveToChar( driveNumber, driveLetter );
                iLastPosition.Format( KDrive, (TUint)driveLetter );
                iDeeper = ETrue;
                return err;
                }
        }
    CleanInternal();
    return err;
    }



// -----------------------------------------------------------------------------
// CFileScan::SearchFolder
// This function create Leaves for the current leaf
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CFileScan::SearchFolder( CDir*& aFolderList )
    {
    TInt err = KErrNone;
    TRAP( err , iCurrentLeaf->SetLeafL( aFolderList ) );
    return err;
    }

// -----------------------------------------------------------------------------
// CFileScan::SearchFile
// This function searches for the DCF files under specific folder
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CFileScan::SearchFile( CDir*& aFileList )
    {
    TInt err = KErrNone;
    TInt i = 0;
    TBool isDCF;
    _LIT ( KFullFileName , "%S\\%S");
    TFileName fileName;

    for ( ; i < aFileList->Count() && !err ; i++ )
        {
        if (!( *aFileList )[i].IsDir())
            {
            TPtrC extension = (*aFileList)[i].iName.Right( 4 );
            if( !extension.CompareF( KWma ) ||
                !extension.CompareF( KWmv ) ||
                !extension.CompareF( KAsf ) )
                {

                }
            else
                {
                fileName.Format( KFullFileName , &iLastPosition , &( *aFileList )[i].iName );
                err = IsProtected( fileName , isDCF );
                }
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CFileScan::SearchNext
// This function searches for the DCF files under specific folder or drive
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CFileScan::SearchNext()
    {
    TInt err = KErrNone;

    err = CheckDrive();
    if ( !err && iSearching )
        {
        if ( iDeeper )
            {
            err = CheckFolder();
            }
        if ( !err && iSearching )
            {
            err = ToNextLeaf();
            }
        }
    return err;
    }

TInt CFileScan::CheckDrive()
    {
    // check if current drive is end of searching
    TInt err = KErrNone;

    if( !iCurrentLeaf )
        {
        err = SearchDrive();
        if ( err || !iSearching )
            {
            return err;
            }
        CSearchLeaf* root = NULL;
        TRAP( err , iCurrentLeaf = CSearchLeaf::NewL( root , iLastPosition ) );
        }
    return err;
    }

TInt CFileScan::CheckFolder()
    {
    // check current folder
    _LIT ( KSearchDir , "%S\\*");

    TInt err = KErrNone;
    CDir* fileList = NULL;
    CDir* dirList = NULL;
    TFileName temp;

#ifndef RD_MULTIPLE_DRIVE
    if ( IgnoreDir( iLastPosition ) )
#else // RD_MULTIPLE_DRIVE
    if ( IgnoreDir( *iFs, iLastPosition ) )
#endif
        {
        return err;
        }

    if( iLastPosition.Length() + KSearchDir().Length() < iLastPosition.MaxLength() )
        {
        temp.Format( KSearchDir , &iLastPosition );
        err = iFs->GetDir( temp
            , KEntryAttMaskSupported
            , ESortByName
            , fileList , dirList );

        if ( !err )
            {
            err = SearchFolder( dirList );
            if ( !err )
                {
                err = SearchFile( fileList );
                }
            }
        delete fileList;
        fileList = NULL;
        delete dirList;
        dirList = NULL;
        }
    return err;
    }

TInt CFileScan::ToNextLeaf()
    {
    _LIT ( KChildDir , "%S\\%S");
    TInt err = KErrNone;
    TFileName file;
    CSearchLeaf* temp = NULL;

    file.Format( iLastPosition );

    if ( iCurrentLeaf->LeafList().Count() > 0 )
        {
        iDeeper = ETrue;
        iCurrentLeaf = iCurrentLeaf->LeafList()[0];
        iLastPosition.Format( KChildDir , &file , &iCurrentLeaf->FolderName() );
        }
    else
        {
        iDeeper = EFalse;
        temp = iCurrentLeaf;
        iCurrentLeaf = iCurrentLeaf->Root();
        if ( iCurrentLeaf )
            {
            iCurrentLeaf->RemoveLeaf( temp );
            }
        delete temp;
        temp = NULL;
        err = UpFolder();
        }
    return err;
    }

TInt CFileScan::UpFolder()
    {
    TInt err = KErrNone;
    TParse file;
    if ( iLastPosition.Length()<3 )
        {
        return err;
        }
    err = file.Set( iLastPosition , NULL , NULL );
    iLastPosition.Format( file.DriveAndPath() );
    iLastPosition.SetLength( iLastPosition.Length() - 1 );
    return err;
    }


void CFileScan::CleanInternal()
    {
    CSearchLeaf* root = iCurrentLeaf;
    iLastPosition = KNullDesC;
    iSearching = EFalse;
    root = GetRootLeaf();
    delete root;
    root = NULL;
    iCurrentLeaf = NULL;
    iDeeper = ETrue;
    }

CSearchLeaf* CFileScan::GetRootLeaf()
    {
    CSearchLeaf* root = iCurrentLeaf;
    if ( iCurrentLeaf )
        {
        while ( root->Root() )
            {
            root = root->Root();
            }
        }
    return root;
    }

// End of File

