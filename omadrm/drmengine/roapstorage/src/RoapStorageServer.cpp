/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



// INCLUDE FILES
#include <e32std.h>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include "RoapStorageServer.h"
#include "DRMContextDB.h"
#include "RoapLog.h"
#include "drmroapclientserver.h"
#include "drmcommonclientserver.h"
#include "DrmKeyStorage.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS


// LOCAL CONSTANTS AND MACROS
const TUint8 KMaxStartTries = 5;
const TInt KWaitingTime = 1000000; // 10 secs

_LIT( KRoapStorageThread, "RoapStorageServer" );

#ifdef RD_MULTIPLE_DRIVE
_LIT( KRIContextFileName, "%c:\\private\\101F51F2\\ricontexts.dat" );
_LIT( KDomainContextFileName, "%c:\\private\\101F51F2\\domaincontexts.dat" );
// File names and paths for importing keys
_LIT( KImportDir, "%c:\\private\\101F51F2\\import\\" );
_LIT( KInputFilePattern, "%c:\\private\\101F51F2\\import\\SigningCert*" );
_LIT( KDeviceKeyFileName, "%c:\\private\\101F51F2\\import\\DevicePrivateKey.der" );
_LIT( KDeviceCertFileName, "%c:\\private\\101F51F2\\import\\DeviceCert.der" );
#else
// File names and paths for importing keys
_LIT( KImportDir, "c:\\private\\101F51F2\\import\\" );
_LIT( KInputFilePattern, "c:\\private\\101F51F2\\import\\SigningCert*" );
_LIT( KDeviceKeyFileName, "c:\\private\\101F51F2\\import\\DevicePrivateKey.der" );
_LIT( KDeviceCertFileName, "c:\\private\\101F51F2\\import\\DeviceCert.der" );
#endif

// MODULE DATA STRUCTURES
using Roap::KServerMajorVersion;
using Roap::KServerMinorVersion;
using Roap::KServerBuildVersion;

// LOCAL FUNCTION PROTOTYPES
LOCAL_C TInt StartRoapServer( RSemaphore& aClientSem );

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// Function StartRoapServer().
// This function starts the actual server under TRAP harness and starts
// waiting for connections. This function returns only if there has been
// errors during server startup or the server is stopped for some reason.
//
// Returns: TInt: Symbian OS error code.
// -----------------------------------------------------------------------------
LOCAL_C TInt StartRoapServer( RSemaphore& aClientSem )

    {
    TInt error = KErrNone;
    CRoapStorageServer* server = NULL;
    TUint8 count = 0;

    do
        {
        ++count;
        TRAP( error, ( server = CRoapStorageServer::NewL() ) );
        if ( error )
           {
           User::After( TTimeIntervalMicroSeconds32(KWaitingTime) );
           }

        } while( error && ( count <= KMaxStartTries ) );

    if( error )
        {
        return error;
        }

    // Release the semaphore...
    aClientSem.Signal();
    aClientSem.Close();

    // Start waiting for connections
    CActiveScheduler::Start();

    // Delete CRoapStorageServer
    delete server;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Function ReadFileL().
// Read a file into a buffer
// -----------------------------------------------------------------------------
LOCAL_C void ReadFileL( RFs& aFs, HBufC8*& aContent, const TDesC& aName )
    {
    TInt size = 0;
    RFile file;

    User::LeaveIfError( file.Open( aFs, aName, EFileRead ) );
    CleanupClosePushL( file );
    User::LeaveIfError( file.Size( size ) );
    aContent = HBufC8::NewLC( size );
    TPtr8 ptr( aContent->Des() );
    User::LeaveIfError( file.Read( ptr, size) );
    CleanupStack::Pop(); // aContent
    CleanupStack::PopAndDestroy(); // file
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRoapStorageServer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRoapStorageServer* CRoapStorageServer::NewL()
    {
    CRoapStorageServer* self = new( ELeave ) CRoapStorageServer();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CRoapStorageServer::~CRoapStorageServer()
    {
    iRFs.Close();
    delete iRoapStorage;
    }

// -----------------------------------------------------------------------------
// CRoapStorageServer::RunErrorL
// From CActive. Complete the request and restart the scheduler.
// -----------------------------------------------------------------------------
//
TInt CRoapStorageServer::RunError( TInt aError )
    {
    // Inform the client.
    Message().Complete( aError );
    // Restart the scheduler.
    ReStart();
    // Error handled.
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CRoapStorageServer::NewSessionL
// Called when a client requires a new instance.
// -----------------------------------------------------------------------------
CSession2* CRoapStorageServer::NewSessionL(
    const TVersion& aVersion,
    const RMessage2& /*aMessage*/) const
    {
    RThread client;
    // Check that the versions are compatible.
    if ( ! User::QueryVersionSupported( TVersion( KServerMajorVersion,
                                                  KServerMinorVersion,
                                                  KServerBuildVersion ),
                                        aVersion ) )
        {
        // Sorry, no can do.
        User::Leave( KErrNotSupported );
        }
    return CRoapStorageSession::NewL();
    }

// -----------------------------------------------------------------------------
// CRoapStorageServer::CRoapStorageServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRoapStorageServer::CRoapStorageServer() :
    CServer2( EPriorityStandard ),
    iRoapStorage( NULL )
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CRoapStorageServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRoapStorageServer::ConstructL()
    {
    TInt err = KErrNone;

    User::RenameThread( KRoapStorageThread );
    StartL( Roap::KServerName );
    User::LeaveIfError(iRFs.Connect());

#ifndef RD_MULTIPLE_DRIVE

    iRoapStorage = CDRMContextDB::NewL( KRIContextFile(),
                                        KDomainContextFile(),
                                        iRFs );

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
    iRFs.DriveToChar( driveNumber, driveLetter );

    TFileName riContextFile;
    riContextFile.Format( KRIContextFileName, (TUint)driveLetter );

    TFileName domainContextFile;
    domainContextFile.Format( KDomainContextFileName, (TUint)driveLetter );

    iRoapStorage = CDRMContextDB::NewL( riContextFile,
                                        domainContextFile,
                                        iRFs );

#endif

    TRAP( err, ImportKeysL() );
    }

// -----------------------------------------------------------------------------
// CRoapStorageServer::ContextDB
// Return the internal ROAP storage object
// -----------------------------------------------------------------------------
//
CDRMContextDB* CRoapStorageServer::ContextDB()
    {
    return iRoapStorage;
    }

// -----------------------------------------------------------------------------
// CRoapStorageServer::ImportKeys
// Import keys which are stored in the import directory into the private
// directory. Only one key pair plus assocated certificates can be imported
// at a time. The file names are DevicePrivateKey.der, DeviceCert.der and
// SigningCertXX.der.
// -----------------------------------------------------------------------------
//
void CRoapStorageServer::ImportKeysL()
    {
    MDrmKeyStorage* storage = NULL;
    HBufC8* privateKey = NULL;
    HBufC8* cert = NULL;
    RArray<TPtrC8> certChain;
    RPointerArray<HBufC8> buffers;
    TFileName fileName;
    RFile file;
    TInt i;
    CDir* dir = NULL;
    TInt err = KErrNone;

    __UHEAP_MARK;
    LOG( _L( "CRoapStorageServer::ImportKeysL" ) );
    CleanupClosePushL( buffers );
    CleanupClosePushL( certChain );

#ifndef RD_MULTIPLE_DRIVE

    ReadFileL( iRFs, privateKey, KDeviceKeyFileName );

#else //RD_MULTIPLE_DRIVE

    TFileName tempPath;
    TFileName tempPath2;
    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
    iRFs.DriveToChar( driveNumber, driveLetter );

    tempPath.Format( KDeviceKeyFileName, (TUint)driveLetter );

    ReadFileL( iRFs, privateKey, tempPath );

#endif

    CleanupStack::PushL( privateKey );

#ifndef RD_MULTIPLE_DRIVE

    ReadFileL( iRFs, cert, KDeviceCertFileName );

#else //RD_MULTIPLE_DRIVE

    tempPath2.Format( KDeviceCertFileName, (TUint)driveLetter );

    ReadFileL( iRFs, cert, tempPath2 );

#endif

    CleanupStack::PushL( cert );
    buffers.AppendL( cert );

#ifndef RD_MULTIPLE_DRIVE

    iRFs.Delete( KDeviceKeyFileName );
    iRFs.Delete( KDeviceCertFileName );

#else //RD_MULTIPLE_DRIVE

    iRFs.Delete( tempPath );
    iRFs.Delete( tempPath2 );

#endif

#ifndef RD_MULTIPLE_DRIVE

    User::LeaveIfError( iRFs.GetDir( KInputFilePattern, KEntryAttNormal,
        ESortByName, dir ) );

#else //RD_MULTIPLE_DRIVE

    tempPath.Format( KInputFilePattern, (TUint)driveLetter );

    User::LeaveIfError( iRFs.GetDir( tempPath, KEntryAttNormal,
        ESortByName, dir ) );

#endif

    CleanupStack::PushL( dir );

    for ( i = 0; i < dir->Count(); i++ )
        {

#ifndef RD_MULTIPLE_DRIVE

        fileName.Copy( KImportDir );

#else //RD_MULTIPLE_DRIVE

        tempPath.Format( KImportDir, (TUint)driveLetter );

        fileName.Copy( tempPath );

#endif

        fileName.Append( (*dir)[i].iName );
        ReadFileL( iRFs, cert, fileName );
        CleanupStack::PushL( cert );
        buffers.AppendL( cert );

        iRFs.Delete( fileName );
        }
    for ( i = 0; i < buffers.Count(); i++ )
        {
        certChain.AppendL( *( buffers[i] ) );
        }
    storage = DrmKeyStorageNewL();
    TRAP( err, storage->ImportDataL( *privateKey, certChain ) );
    delete storage;
    CleanupStack::PopAndDestroy( i + 1 ); // certs & dir
    CleanupStack::PopAndDestroy( 3 ); // privateKey, certChain, buffers
    LOG( _L( "CRoapStorageServer::ImportKeysL done" ) );
    __UHEAP_MARKEND;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// -----------------------------------------------------------------------------
// Function StartupRoapStorage().
// This function starts the actual Roap Storage
// the cleanup stack and active scheduler.
// Returns: TInt: Symbian OS error code.
// -----------------------------------------------------------------------------
//

TInt StartupRoapStorage( TAny* )
    {
    TInt error = KErrNone;
    CTrapCleanup* trap = CTrapCleanup::New();
    __ASSERT_ALWAYS( trap, User::Invariant() );

    CActiveScheduler* scheduler = new CActiveScheduler();
    __ASSERT_ALWAYS( scheduler, User::Invariant() );

    CActiveScheduler::Install( scheduler );
    RSemaphore clientSem;
    __ASSERT_ALWAYS( clientSem.OpenGlobal( KDRMEngCommonSemaphore ) == 0, User::Invariant() );

    error = StartRoapServer( clientSem );

    if ( error ) {
        // Server creation failed. Release the semaphore.
        // In case of successful startup, CRoapStorageServer
        // releases the semaphore.
        clientSem.Signal();
        clientSem.Close();
        }

    delete scheduler;
    scheduler = NULL;

    delete trap;
    trap = NULL;

    // __ASSERT_ALWAYS( !error, User::Invariant() );

    return KErrNone;
    }

//  End of File
