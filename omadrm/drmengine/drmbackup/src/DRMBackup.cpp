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
* Description:  Implementation of the Active Backup in DRM
*
*/


// INCLUDE FILES
#include <s32mem.h>

#include <sysutil.h>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include "DRMBackup.h"
#include "drmlog.h"
#include "RoapStorageClient.h"
#include "drmrightsdb.h"
#include "DRMBackupInterface.h"
/*
#include "flogger.h"


_LIT( KLogDir, "drm");
_LIT( KLogName, "backup.log");
*/

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KDoBackup        =   0x00000001;
const TInt KRightsFileOpen  =   0x00000002;
const TInt KContextFileOpen =   0x00000004;
const TInt KRightsSizeRead  =   0x00000008;
const TInt KContextSizeRead =   0x00000010;

// MACROS

// LOCAL CONSTANTS AND MACROS
#ifdef RD_MULTIPLE_DRIVE
// Backup Directory
_LIT( KBackupDir, "%c:\\private\\101F51F2\\backup\\" );
#endif

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES
LOCAL_C void CleanupData( TAny* aPtr );

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CleanupData
// Used to catch errors and delete the file if it's needed
// -----------------------------------------------------------------------------
//
LOCAL_C void CleanupData( TAny* aPtr )
    {
    CDRMBackup* backup = reinterpret_cast<CDRMBackup*>( aPtr );

    backup->PerformCleanup();
    }


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CDRMBackup::CDRMBackup
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMBackup::CDRMBackup( CDRMRightsDB* aRightsDatabase,
                        RFs& aFileServer ) :
    iFileServer( aFileServer ),
    iDRMRightsDB( aRightsDatabase ),
    iRightsFileSize( 0 ),
    iContextFileSize( 0 ),
    iPosition( 0 ),
    iReadData( 0 ),
    iStatus( 0 )
    {
    };

// -----------------------------------------------------------------------------
// CDRMBackup::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDRMBackup::ConstructL()
    {
    TInt error = KErrNone;

#ifndef RD_MULTIPLE_DRIVE

    // Create the backup directory if needed
    error = iFileServer.MkDirAll( KBackupDirectory );

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
    iFileServer.DriveToChar( driveNumber, driveLetter );

    TFileName backupDir;
    backupDir.Format( KBackupDir, (TUint)driveLetter );

    // Create the backup directory if needed
    error = iFileServer.MkDirAll( backupDir );

#endif

    if( error != KErrAlreadyExists )
        {
        User::LeaveIfError( error );
        }

#ifndef RD_MULTIPLE_DRIVE

    iRightsFileName.Copy( KBackupDirectory );

#else //RD_MULTIPLE_DRIVE

    iRightsFileName.Copy( backupDir );

#endif

    iRightsFileName.Append( KRightsDbBackupFile );
    iSizeBuffer.SetLength( 4 );
    };

// -----------------------------------------------------------------------------
// CDRMBackup::NewLC
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CDRMBackup* CDRMBackup::NewLC( CDRMRightsDB* aRightsDatabase,
                               RFs& aFileServer )
    {
    CDRMBackup* self = new(ELeave) CDRMBackup( aRightsDatabase,
                                               aFileServer );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    };

// -----------------------------------------------------------------------------
// CDRMBackup::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CDRMBackup* CDRMBackup::NewL( CDRMRightsDB* aRightsDatabase,
                              RFs& aFileServer )
    {
    CDRMBackup* self = NewLC( aRightsDatabase,
                              aFileServer );
    CleanupStack::Pop();

    return self;
    };

// ---------------------------------------------------------
// CDRMBackup::~CDRMBackup
// Destructor
// ---------------------------------------------------------
//
CDRMBackup::~CDRMBackup()
    {
    };


// ---------------------------------------------------------
// CDRMBackup::AllSnapshotsSuppliedL
// ---------------------------------------------------------
//
void CDRMBackup::AllSnapshotsSuppliedL()
    {
    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("AllSnapshotsSuppliedL\n\r"));
    return;
    };

// ---------------------------------------------------------
// CDRMBackup::ReceiveSnapshotDataL
// ---------------------------------------------------------
//
void CDRMBackup::ReceiveSnapshotDataL(TDriveNumber /* aDrive */,
                                      TDesC8& /* aBuffer */,
                                      TBool /* aLastSection */)
    {
    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("RecieveSnapshotdataL\n\r"));
    User::Leave( KErrNotSupported );
    };

// ---------------------------------------------------------
// CDRMBackup::GetExpectedDataSize
// ---------------------------------------------------------
//
TUint CDRMBackup::GetExpectedDataSize(TDriveNumber /* aDrive */)
    {
    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("GetExpectedDataSize\n\r"));
    return 0;
    };

// ---------------------------------------------------------
// CDRMBackup::GetSnapshotDataL
// ---------------------------------------------------------
//
void CDRMBackup::GetSnapshotDataL(TDriveNumber /* aDrive */,
                                  TPtr8& /* aBuffer */,
                                  TBool& /* aFinished */)
    {
    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("GetSnapShopDataL\n\r"));
    User::Leave( KErrNotSupported );
    };

// ---------------------------------------------------------
// CDRMBackup::InitialiseGetBackupDataL
// ---------------------------------------------------------
//
void CDRMBackup::InitialiseGetBackupDataL(TDriveNumber aDrive)
    {
    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("Init backup data\n\r"));
    // Clean up existing data:
    PerformCleanup();

#ifndef RD_MULTIPLE_DRIVE

    // Only do a backup when backing up drive C
    if( aDrive == EDriveC )

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );

    // Only do a backup when backing up system drive
    if( aDrive == driveNumber )

#endif
        {
        iStatus |= KDoBackup;
        }
    else
        {
        iStatus &= ~KDoBackup;
        return;
        }

    // Add the cleanup item to the cleanup stack
    TCleanupItem resetAndDestroy( CleanupData, reinterpret_cast<TAny*>(this) );
    CleanupStack::PushL( resetAndDestroy );

    // This function needs to write the two files required to be backed up.
    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("Init: create rights file\n\r"));
    // create the rights database backup file
    CreateRightsBackupFileL();

    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("Init: create context file\n\r"));
    // create the context database backup file
    CreateContextBackupFileL();

    // If everything went properly, pop
    CleanupStack::Pop();
    };

// ---------------------------------------------------------
// CDRMBackup::GetBackupDataSectionL
// ---------------------------------------------------------
//
void CDRMBackup::GetBackupDataSectionL(TPtr8& aBuffer, TBool& aFinished)
    {
    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("GetBackupDataSectionL\n\r"));
    TInt seekAmount = 0;

    // if we are not supposed to do anything, just return
    if( !(iStatus & KDoBackup ) )
        {
        // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("GetBackupDataSectionL: no Do Backup\n\r"));
        aFinished = ETrue;
        return;
        }

    // Make sure that the buffer is empty and starts from the beginning
    aBuffer.SetLength(0);

     // Add the cleanup item to the cleanup stack
    TCleanupItem resetAndDestroy( CleanupData, reinterpret_cast<TAny*>(this) );
    CleanupStack::PushL( resetAndDestroy );

    if( iPosition < iRightsFileSize )
        {
        // Find the right position
        iRightsBackupFile.Seek( ESeekStart,iPosition );

        // Read data
        User::LeaveIfError( iRightsBackupFile.Read( aBuffer ) );

        iPosition += aBuffer.Length();

        if( iPosition == iRightsFileSize )
            {
            aFinished = ETrue;
            }
        else
            {
            aFinished = EFalse;
            }

        }
    else if( iPosition < iRightsFileSize + iContextFileSize )
        {
        // Find the right position
        seekAmount = iPosition-iRightsFileSize;
        iContextBackupFile.Seek( ESeekStart, seekAmount );

        // Read data
        User::LeaveIfError( iContextBackupFile.Read( aBuffer ) );

        iPosition += aBuffer.Length();

        if( iPosition-iRightsFileSize == iContextFileSize )
            {
            aFinished = ETrue;
            }
        else
            {
            aFinished = EFalse;
            }
        }
    else
        {
        // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("GetBackupDataSectionL - Error\n\r"));
        User::Leave( KErrGeneral );
        }

    // if we are finished, we can clean up the stuff otherwise not
    if( aFinished )
        {
        // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("GetBackupDataSectionL - Finished\n\r"));
        CleanupStack::PopAndDestroy();
        }
    else
        {
        // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("GetBackupDataSectionL - Continues\n\r"));
        CleanupStack::Pop();
        }
    };

// ---------------------------------------------------------
// CDRMBackup::InitialiseRestoreBaseDataL
// ---------------------------------------------------------
//
void CDRMBackup::InitialiseRestoreBaseDataL(TDriveNumber aDrive)
    {
    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("InitializeRestoreBaseDataL\n\r"));

    // Clean up existing data:
    PerformCleanup();

#ifndef RD_MULTIPLE_DRIVE

    // Only do a restore when restoring drive C
    if( aDrive == EDriveC )

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );

    // Only do a restore when restoring drive C
    if( aDrive == driveNumber )

#endif
        {
        iStatus |= KDoBackup;
        }
    else
        {
        iStatus &= ~KDoBackup;
        }
    };

// ---------------------------------------------------------
// CDRMBackup::RestoreBaseDataSectionL
// Create the files where the restore is performed from
// ---------------------------------------------------------
//
void CDRMBackup::RestoreBaseDataSectionL(TDesC8& aBuffer, TBool aFinished)
    {
    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("RestoreBaseDataSectionL\n\r"));

    TInt seekAmount = 0;
    Roap::RRoapStorageClient client;
    TFileName tempFileName;
    TPtrC8 buffer( aBuffer );

    if( !(iStatus & KDoBackup ) )
        {
        return;
        }

     // Add the cleanup item to the cleanup stack
    TCleanupItem resetAndDestroy( CleanupData, reinterpret_cast<TAny*>(this) );
    CleanupStack::PushL( resetAndDestroy );

    // Read the size of the first file, even if it comes in one byte chunks
    if( iPosition == 0 && iReadData < 4 && !( iStatus & KRightsSizeRead ) )
        {
        if( iReadData + buffer.Length() >= 4 )
            {
            Mem::Copy( const_cast<TUint8*>( iSizeBuffer.Ptr() ) + iReadData,
                       buffer.Ptr(), 4-iReadData );
            RMemReadStream stream( iSizeBuffer.Ptr(), 4 );
            CleanupClosePushL( stream );
            iRightsFileSize = stream.ReadInt32L();
            CleanupStack::PopAndDestroy();

            // Remove the 'used' data from the beginning of the data block
            buffer.Set( buffer.Mid( 4-iReadData ) );

            iStatus |= KRightsSizeRead;

            iReadData = 0;
            }
        else
            {
            if( aFinished )
                {
                CleanupStack::PopAndDestroy();
                return;
                }
            // copy the data to the temp buffer
            Mem::Copy( const_cast<TUint8*>( iSizeBuffer.Ptr() ) + iReadData,
                       buffer.Ptr(), buffer.Length() );
            iReadData += buffer.Length();
            CleanupStack::Pop();
            return;
            }
        }

    // Read the size of the 2nd file when it's needed:
    if( iPosition == iRightsFileSize && iReadData < 4 && !(iStatus & KContextSizeRead) )
        {
        if( iReadData + buffer.Length() >= 4 )
            {
            Mem::Copy( const_cast<TUint8*>( iSizeBuffer.Ptr() ) + iReadData,
                       buffer.Ptr(), 4-iReadData );
            RMemReadStream stream( iSizeBuffer.Ptr(), 4 );
            CleanupClosePushL( stream );
            iContextFileSize = stream.ReadInt32L();
            CleanupStack::PopAndDestroy();

            // Remove the 'used' data from the beginning of the data block
            buffer.Set( buffer.Mid( 4-iReadData ) );

            iStatus |= KContextSizeRead;
            iReadData = 0;

            }
        else
            {
            if( aFinished )
                {
                // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("RestoreBaseDataSectionL : buggy file(1)\n\r"));
                CleanupStack::PopAndDestroy();
                return;
                }
            // copy the data to the temp buffer
            Mem::Copy( const_cast<TUint8*>( iSizeBuffer.Ptr()) + iReadData,
                       buffer.Ptr(), buffer.Length() );
            iReadData += buffer.Length();
            CleanupStack::Pop();
            return;
            }
        }

    // Restore the two files:
    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("RestoreBaseDataSectionL : before restore\n\r"));

    if( iPosition < iRightsFileSize )
        {
        // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("RestoreBaseDataSectionL : restore starts\n\r"));
        if( !( iStatus & KRightsFileOpen ) )
            {
            // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("RestoreBaseDataSectionL : replace the file\n\r"));
            User::LeaveIfError( iRightsBackupFile.Replace( iFileServer,
                                                           iRightsFileName,
                                                           EFileRead|EFileWrite ) );
            iStatus |= KRightsFileOpen;
            // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("RestoreBaseDataSectionL : write startn\r"));
            iRightsBackupFile.Write( iSizeBuffer, 4 );
            // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("RestoreBaseDataSectionL : write end\n\r"));
            iPosition += 4;
            }
    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("RestoreBaseDataSectionL : before seek\n\r"));
        // Find the right position
        User::LeaveIfError( iRightsBackupFile.Seek( ESeekStart,iPosition ) );


    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("RestoreBaseDataSectionL : after seek\n\r"));
        // Read data
        User::LeaveIfError( iRightsBackupFile.Write( buffer ) );

    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("RestoreBaseDataSectionL : after write\n\r"));
        iPosition += buffer.Length();
        }
    else if( iPosition < iRightsFileSize + iContextFileSize )
        {

        // Open the file if it's not open
        if( !( iStatus & KContextFileOpen ) )
            {
            User::LeaveIfError( iContextBackupFile.Replace( iFileServer,
                                                            iContextFileName,
                                                            EFileRead|EFileWrite ) );
            iStatus |= KContextFileOpen;
            iContextBackupFile.Write( iSizeBuffer, 4 );
            iPosition += 4;
            }

        // Find the right position
        seekAmount = iPosition-iRightsFileSize;
        iContextBackupFile.Seek( ESeekStart, seekAmount );

        // Read data
        User::LeaveIfError( iContextBackupFile.Write( buffer ) );

        iPosition += buffer.Length();
        }
    else
        {
        // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("RestoreBaseDataSectionL : error\n\r"));
        User::Leave( KErrGeneral );
        }

    CleanupStack::Pop();
    };

// ---------------------------------------------------------
// CDRMBackup::InitialiseRestoreIncrementDataL
// ---------------------------------------------------------
//
void CDRMBackup::InitialiseRestoreIncrementDataL(TDriveNumber /* aDrive */)
    {
    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("init incremental restore\n\r"));
    User::Leave( KErrNotSupported );
    };

// ---------------------------------------------------------
// CDRMBackup::RestoreIncrementDataSectionL
// ---------------------------------------------------------
//
void CDRMBackup::RestoreIncrementDataSectionL(TDesC8& /* aBuffer */, TBool /* aFinished */)
    {
    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("restore incremenetal\n\r"));
    User::Leave( KErrNotSupported );
    };

// ---------------------------------------------------------
// CDRMBackup::RestoreComplete
// This function performs the actual restoring
// ---------------------------------------------------------
//
void CDRMBackup::RestoreComplete(TDriveNumber aDrive)
    {
    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("RestoreComplete\n\r"));
    TInt error = KErrNone;
    TBool doUdt = EFalse;

#ifndef RD_MULTIPLE_DRIVE

    // Only do a backup when backing up drive C
    if( aDrive != EDriveC )

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );

    // Only do a backup when backing up drive C
    if( aDrive != driveNumber )

#endif
        {
        // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("RestoreComplete : not Drive C\n\r"));
        return;
        }
    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("PerformRightsMergeL\n\r"));



    // Merge the rights
    TRAP( error, PerformRightsMergeL() );
    if( error )
        {
        if( error == KErrPermissionDenied )
            {
            doUdt = ETrue;
            }
        // Log some error
        error = KErrNone;
        }

    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("PerformContextMergeL\n\r"));

    // Merge the Contexts
    TRAP( error, PerformContextMergeL() );
    if( error )
        {
        // Log some error
        error = KErrNone;
        }

    PerformCleanup( doUdt );
    };

// ---------------------------------------------------------
// CDRMBackup::TerminateMultiStageOperation
// ---------------------------------------------------------
//
void CDRMBackup::TerminateMultiStageOperation()
    {
    PerformCleanup();
    };

// ---------------------------------------------------------
// CDRMBackup::GetDataChecksum
// ---------------------------------------------------------
//
TUint CDRMBackup::GetDataChecksum(TDriveNumber /* aDrive */)
    {
    return 0;
    };

// ---------------------------------------------------------
// CDRMBackup::CreateRightsBackupFileL
// ---------------------------------------------------------
//
void CDRMBackup::CreateRightsBackupFileL()
    {
    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("Create backup file\n\r"));

    // Open the file and get the filename
    User::LeaveIfError( iRightsBackupFile.Replace( iFileServer,
                                                   iRightsFileName,
                                                   EFileRead|EFileWrite ) );
    iStatus |= KRightsFileOpen;

    iDRMRightsDB->BackupContentToFileL( iRightsBackupFile, KNullDesC8 );

    User::LeaveIfError( iRightsBackupFile.Size( iRightsFileSize ) );
    };

// ---------------------------------------------------------
// CDRMBackup::CreateContextBackupFileL
// ---------------------------------------------------------
//
void CDRMBackup::CreateContextBackupFileL()
    {
    //Roap::RRoapStorageClient client;

    // Open the file and get the filename
/*    User::LeaveIfError( iContextBackupFile.Replace( iFileServer,
                                                    iContextFileName,
                                                    EFileRead|EFileWrite ) );

    iStatus |= KContextFileOpen;
*/
    // Connect to the roap storage server
    //User::LeaveIfError( client.Connect() );
    //CleanupClosePushL( client );

    //client.BackupContentToFileL( iContextBackupFile, KNullDesC8 );
    };

// ---------------------------------------------------------
// CDRMBackup::PerformRightsMergeL
// ---------------------------------------------------------
//
void CDRMBackup::PerformRightsMergeL()
    {
    if( iStatus & KRightsFileOpen )
        {
        iDRMRightsDB->RestoreContentFromFileL( iRightsBackupFile, KNullDesC8,
                                               KDRMNormalBackup );
        }
    else
        {
        User::Leave(KErrNotReady);
        }
    };

// ---------------------------------------------------------
// CDRMBackup::PerformContextMergeL
// ---------------------------------------------------------
//
void CDRMBackup::PerformContextMergeL()
    {
    //Roap::RRoapStorageClient client;

    // Connect to the roap storage server
    //User::LeaveIfError( client.Connect() );
    //CleanupClosePushL( client );

    //client.RestoreContentFromFileL( iContextBackupFile, KNullDesC8 );

    //CleanupStack::PopAndDestroy();// client
    };

// ---------------------------------------------------------
// CDRMBackup::PerformCleanup
// ---------------------------------------------------------
//
void CDRMBackup::PerformCleanup( TBool aUdt )
    {
    iRightsFileSize = 0;
    iContextFileSize = 0;

    if( iStatus & KRightsFileOpen )
        {
        if( !aUdt )
            {
            iRightsBackupFile.SetSize(0);
            }

        iRightsBackupFile.Close();

        if( !aUdt )
            {
            iFileServer.Delete( iRightsFileName );
            }
        }

    if( iStatus & KContextFileOpen )
        {
        if( !aUdt )
            {
            iContextBackupFile.SetSize(0);
            }

        iContextBackupFile.Close();

        if( !aUdt )
            {
            iFileServer.Delete( iContextFileName );
            }
        }

    iPosition = 0;
    Mem::FillZ(const_cast<TUint8*>(iSizeBuffer.Ptr()), 4);
    iReadData = 0;
    iStatus = 0;
    return;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File
