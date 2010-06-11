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
#include <hash.h>       // MD5 Algorithm
// #include <SysUtil.h>    // Disk space checking
#include <f32file.h>
#include <s32strm.h>
#include <s32file.h>
#include <caf/caf.h>
#include <caf/cafplatform.h>
#include <symmetric.h>  // AES128CBC
#include <DcfRep.h>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include "DRMCommon.h"  // DRM Error messages
#include "drmrightsdb.h"
#include "DRMRightsData.h"
#include "DRMCommonData.h"
#include "DrmPermission.h"
#include "DrmConstraint.h"
#include "DRMRightsCleaner.h"
#include "DRMRightsServer.h"
#include "DrmKeyStorage.h"
#include "utf.h" // charconv, ConvertFromUnicodeToUtf8L
#include "drmlog.h"
#include "DRMRightsServer.h"

#ifdef RD_DRM_RIGHTS_MANAGER_REMOVAL
#include "DRMClockClient.h"
#endif // RD_DRM_RIGHTS_MANAGER_REMOVAL

/*
#include "flogger.h"


_LIT( KLogDir, "drm");
_LIT( KLogName, "backup.log");
*/

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
#ifdef RD_MULTIPLE_DRIVE
// Backup Directory
_LIT( KBackupDir, "%c:\\private\\101F51F2\\backup\\" );
#endif


_LIT(KJavaExtension, ".jar");
_LIT(KSISExtension, ".sis");
_LIT(KROExtension, ".ro");
_LIT(KXmlExtension, ".xml");


#ifdef RD_MULTIPLE_DRIVE
_LIT(KCIDListTempDir, "%c:\\system\\temp\\");
#else
_LIT(KCIDListTempDir, "c:\\system\\temp\\");
#endif

_LIT(KRODirName, "DomainROs\\");
_LIT(KCorruptionFlagFile, "invalid");

#ifdef __DRM_OMA2
const TInt KMaxUDTDataSize = 256;
const TInt KDeviceDataSize = 256;
const TInt KDeviceDataBlock = 128;
#else
const TInt KMaxUDTDataSize = 0;
const TInt KDeviceDataSize = 0;
const TInt KDeviceDataBlock = 0;
#endif
const TInt KUdtDataPos = 4 + KDeviceDataSize;


const TInt KEncryptionKeySize = 16;
const TInt KMaxEncryptionSize = 8192;

const TInt KMaxUpdateTime = 3000000; // consider updates to be finished within
                                     // three seconds

const TUint32 KMaxTIntVal = 2147483647; // 2^31-1

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES
LOCAL_C void CleanupData( TAny* aPtr );
LOCAL_C void WriteIntToBlock( TInt aValue, TDes8& aBlock, TInt aOffset );
LOCAL_C void DeleteObject( TAny* aObject );

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CleanupData
// Used to catch errors and delete the file if it's needed
// -----------------------------------------------------------------------------
//
LOCAL_C void CleanupData( TAny* aPtr )
    {
    CDRMRightsDB* rdb = reinterpret_cast<CDRMRightsDB*>( aPtr );

    rdb->CleanUdtData();
    }


LOCAL_C void WriteIntToBlock( TInt aValue, TDes8& aBlock, TInt aOffset )
    {
    aBlock[aOffset] =     (aValue & 0xff000000) >> 24;
    aBlock[aOffset + 1] = (aValue & 0x00ff0000) >> 16;
    aBlock[aOffset + 2] = (aValue & 0x0000ff00) >> 8;
    aBlock[aOffset + 3] = (aValue & 0x000000ff);
    }

// ----------------------------------------------------------------------------
// DeleteObject
// Deletes the file by TFileName presented by aHandle
// ----------------------------------------------------------------------------
//
LOCAL_C void DeleteObject( TAny* aObject )
    {
    __ASSERT_DEBUG( aObject, User::Panic( _L( "DeleteObject" ), KErrArgument ) );
    MDrmKeyStorage* object = reinterpret_cast< MDrmKeyStorage* >( aObject );
    delete object;
    object = NULL;
    }


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CDRMRightsDB::CDRMRightsDB
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMRightsDB::CDRMRightsDB( RFs& aFs, CDRMRightsServer* aServer ) :
    iFileServer( aFs ),
    iImei( NULL ),
    iLastUpdate( Time::NullTTime() ),
    iRightsServer( aServer )    
    {
    };


// -----------------------------------------------------------------------------
// CDRMRightsDB::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::ConstructL( const TDesC& aDatabasePath,
                               const TDesC8& aKey,
                               const TDesC& aImei )
    {
    TFileName name;
    TUint attr;

    DRMLOG( _L( "RDB starting..." ) );

    iDbPath = aDatabasePath.AllocL();

    // create an instance of the Hash algorithm class
    iHasher = CMD5::NewL();

    // store the key
    iKey = aKey.AllocL();

    // serial number
    iImei = aImei.AllocL();

    name.Copy( *iDbPath );
    name.Append( KCorruptionFlagFile );

    if ( iFileServer.Att( name, attr ) == KErrNone )
        {
        iFileServer.RmDir( name );
        iFileServer.Delete( name );
        DeleteDBL();
        }

    InitializeDatabaseL();
    };

// -----------------------------------------------------------------------------
// CDRMRightsDB::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMRightsDB* CDRMRightsDB::NewLC( RFs& aFs,
                                   const TDesC& aDatabasePath,
                                   const TDesC8& aKey,
                                   const TDesC& aImei,
                                   CDRMRightsServer* aServer )
    {
    CDRMRightsDB* self = new( ELeave ) CDRMRightsDB( aFs, aServer );
    CleanupStack::PushL( self );
    self->ConstructL( aDatabasePath, aKey, aImei );

    return self;
    };

// -----------------------------------------------------------------------------
// CDRMRightsDB::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMRightsDB* CDRMRightsDB::NewL( RFs& aFs,
                                  const TDesC& aDatabaseFile,
                                  const TDesC8& aKey,
                                  const TDesC& aImei,
                                  CDRMRightsServer* aServer )
    {
    CDRMRightsDB* self = NewLC( aFs, aDatabaseFile, aKey, aImei, aServer );
    CleanupStack::Pop();

    return self;
    };

// Destructor
CDRMRightsDB::~CDRMRightsDB()
    {
    if( iHasher )
        {
        delete iHasher;
        iHasher = NULL;
        }

    if( iDbPath )
        {
        delete iDbPath;
        iDbPath = NULL;
        }

    if( iKey )
        {
        delete iKey;
        iKey = NULL;
        }

    if( iImei )
        {
        delete iImei;
        iImei = NULL;
        }

    iMemStream.Close();

    DRMLOG( _L( "RDB Closing." ) );
    };


// -----------------------------------------------------------------------------
// CDRMRightsDB::GetDBEntryByContentIDL
// returns the rights objects connected to aContentID
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::GetDBEntryByContentIDL(
    const TDesC8& aContentID,
    RPointerArray<CDRMPermission>& aRightsList)
    {
    TFileName path;
    TInt error = KErrNone;
    CDRMRightsData* rights = NULL;

#ifdef RD_DRM_RIGHTS_MANAGER_REMOVAL
    TTime time;
    TBool deleteAllowed = EFalse;
    TInt timeZone = 0;
    DRMClock::ESecurityLevel securityLevel;
#endif // RD_DRM_RIGHTS_MANAGER_REMOVAL

    GetRightsFileNameL( aContentID, path);

#ifdef RD_DRM_RIGHTS_MANAGER_REMOVAL
    // Get the secure time:
    RDRMClockClient client;
    error = client.Connect();
    if( !error )
        {
        client.GetSecureTime( time, timeZone, securityLevel);
        client.Close();
        if( securityLevel == DRMClock::KInsecure )
            {
            time = Time::NullTTime();
            }
        }
    else
        {
        time = Time::NullTTime();
        }

    // Delete expired:
    TRAP_IGNORE( deleteAllowed = DeleteExpiredL( path, time, aContentID ) );

    // Check if it's possible to delete the file as well
    if( deleteAllowed )
        {
        iFileServer.Delete( path );
        }

    error = KErrNone;
#endif // RD_DRM_RIGHTS_MANAGER_REMOVAL

    TRAP( error, rights = CDRMRightsData::OpenL( path, iFileServer ) );
    if( rights )
        {
        CleanupStack::PushL( rights );
        }
    else
        {
        if( error == KErrNotFound )
            {
            User::Leave( KErrCANoRights );
            }
        User::Leave( error );
        }

    rights->FetchAllPermissionsL( aRightsList );

    CleanupStack::PopAndDestroy(); // rights
    };

// -----------------------------------------------------------------------------
// CDRMRightsDB::GetDBEntryByContentIDL
// returns the rights object connected to aContentID and aUniqueID
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CDRMPermission* CDRMRightsDB::GetDBEntryByContentIDL(
    const TDesC8& aContentID,
    const TDRMUniqueID aUniqueID )
    {
    TFileName path;
    TInt error = KErrNone;
    CDRMRightsData* rights = NULL;
    CDRMPermission* ro = NULL;

    if( aUniqueID > KMaxTIntVal )
        {
        User::Leave( KErrArgument );
        }

    GetRightsFileNameL( aContentID, path);

    TRAP( error, rights = CDRMRightsData::OpenL( path, iFileServer ) );
    if( rights )
        {
        CleanupStack::PushL( rights );
        }
    else
        {
        if( error == KErrNotFound )
            {
            User::Leave( KErrCANoRights );
            }
        User::Leave( error );
        }

    ro = CDRMPermission::NewLC();

    rights->FetchPermissionL( *ro, aUniqueID );

    CleanupStack::Pop(); // ro
    CleanupStack::PopAndDestroy(); // rights
    return ro;
    };


// -----------------------------------------------------------------------------
// CDRMRightsDB::AddDBEntryL
// adds a new rights object into the database and uses the given aUniqueID if
// possible, if not it generates a new one
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::AddDBEntryL( const TDesC8& aContentID,
                                CDRMPermission& aRightsObject,
                                const TDesC8& aEncryptionKey,
                                TDRMUniqueID& aUniqueID )
    {
    TFileName path;
    TInt error = KErrNone;
    CDRMRightsData* rights = NULL;
    TBuf8<16> nullDesc;
    TBuf<16> nullDesc2;
    HBufC8* oldKey = NULL;

    DRMLOG( _L( "CDRMRightsDB::AddDBEntryL ->" ) );

    // Indicate that the DB is updated
    iLastUpdate.HomeTime();

    // the key can either be empty or it can be 16 characters:
    if( !( aEncryptionKey.Length() == KEncryptionKeySize || aEncryptionKey.Length() == 0 ) )
        {
        User::Leave(KErrArgument);
        }

    TBuf8<16> keyStore = aEncryptionKey;
    // TBuf8<16> authStore = aAuthenticationSeed

    // Encrypt the key
    ModifyKey( keyStore );

    // Modify also the authentication seed
    // ModifyKey( authStore )

    GetRightsFileNameL( aContentID, path);

    TRAP( error, rights = CDRMRightsData::OpenL( path, iFileServer ) );
    if( rights )
        {
        CleanupStack::PushL( rights );


        // Check that the keys match, if not leave with KErrArgument
        oldKey = rights->GetKeyL();
        if ( oldKey != NULL )
            {
            CleanupStack::PushL( oldKey );
            if( oldKey->Compare(keyStore) )
                {
                User::Leave(KErrArgument);
                }
            CleanupStack::PopAndDestroy( oldKey );
            }
        }

    if( error == KErrNotFound )
        {
        // This would need the other info if it was available:
        CDRMCommonData* data = CDRMCommonData::NewL( aContentID, nullDesc, aRightsObject.iRiId, nullDesc2, nullDesc );

        rights = CDRMRightsData::NewLC( data, keyStore, path, iFileServer );
        }
    else
        {
        User::LeaveIfError( error );
        }

    // Store the RO:
    rights->StoreNewPermissionL( aRightsObject, aUniqueID );

    CleanupStack::PopAndDestroy(); // rights

    DRMLOG( _L( "CDRMRightsDB::AddDBEntryL <-" ) );
    };

// -----------------------------------------------------------------------------
// CDRMRightsDB::DeleteDBEntryL
// deletes the rights object connected to aContentID and aUniqueID
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::DeleteDBEntryL( const TDesC8& aContentID,
                                   const TDRMUniqueID aUniqueID )
    {
    TFileName path;
    TInt error = KErrNone;
    CDRMRightsData* rights = NULL;

    DRMLOG( _L( "CDRMRightsDB::DeleteDBEntryL ->" ) );

    if( aUniqueID > KMaxTIntVal )
        {
        User::Leave( KErrArgument );
        }

    GetRightsFileNameL( aContentID, path);

    TRAP( error, rights = CDRMRightsData::OpenL( path, iFileServer ) );
    if( rights )
        {
        CleanupStack::PushL( rights );
        }
    else
        {
        if( error == KErrNotFound )
            {
            User::Leave( DRMCommon::ENoRights );
            }
        User::Leave( error );
        }

    // Indicate that the DB is updated
    iLastUpdate.HomeTime();
    rights->DeletePermissionL( aUniqueID );

    CleanupStack::PopAndDestroy(); // rights

    // Check if the file can be deleted:
    CheckCleanup( path );

    DRMLOG( _L( "CDRMRightsDB::DeleteDBEntryL <-" ) );
    };

// -----------------------------------------------------------------------------
// CDRMRightsDB::DeleteDBEntryL
// deletes all rights object connected to aContenID
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::DeleteDBEntryL( const TDesC8& aContentID )
    {
    TFileName path;
    TInt error = KErrNone;
    CDRMRightsData* rights = NULL;

    DRMLOG( _L( "CDRMRightsDB::DeleteDBEntryL (2) ->" ) );

    GetRightsFileNameL( aContentID, path);

    TRAP( error, rights = CDRMRightsData::OpenL( path, iFileServer ) );
    if( rights )
        {
        CleanupStack::PushL( rights );
        }
    else
        {
        if( error == KErrNotFound )
            {
            User::Leave( DRMCommon::ENoRights );
            }
        User::Leave( error );
        }

    // Indicate that the DB is updated
    iLastUpdate.HomeTime();
    rights->DeleteAllPermissionsL();

    CleanupStack::PopAndDestroy(); // rights

    // Check if the file can be deleted:
    CheckCleanup( path );

    DRMLOG( _L( "CDRMRightsDB::DeleteDBEntryL (2) <-" ) );
    };


// -----------------------------------------------------------------------------
// CDRMRightsDB::UpdateDBEntryL
// updates the requested rights object to the given rights object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::UpdateDBEntryL( const TDesC8& aContentID,
                                   const CDRMPermission& aRightsObject )
    {
    TFileName path;
    TInt error = KErrNone;
    CDRMRightsData* rights = NULL;

    DRMLOG( _L( "CDRMRightsDB::UpdateDBEntryL ->" ) );

    // Should never occur:
    if( aRightsObject.iUniqueID > KMaxTIntVal )
        {
        User::Leave( KErrArgument );
        }


    GetRightsFileNameL( aContentID, path);

    TRAP( error, rights = CDRMRightsData::OpenL( path, iFileServer ) );
    if( rights )
        {
        CleanupStack::PushL( rights );
        }
    else
        {
        if( error == KErrNotFound )
            {
            User::Leave( DRMCommon::ENoRights );
            }
        User::Leave( error );
        }

    // Indicate that the DB is updated
    iLastUpdate.HomeTime();
    rights->UpdatePermissionL( aRightsObject );

    CleanupStack::PopAndDestroy(); // rights

    DRMLOG( _L( "CDRMRightsDB::UpdateDBEntryL <-" ) );
    };

// -----------------------------------------------------------------------------
// CDRMRightsDB::ExportContentIDListL
// writes each unique content id to the requested file
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::ExportContentIDListL( TFileName& aTempFile )
    {
    __UHEAP_MARK;
    RFileWriteStream stream;


#ifndef RD_MULTIPLE_DRIVE

    User::LeaveIfError( stream.Temp( iFileServer,
                                     KCIDListTempDir,
                                     aTempFile,
                                     EFileWrite ) );

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
    iFileServer.DriveToChar( driveNumber, driveLetter );

    TFileName cidTemp;
    cidTemp.Format( KCIDListTempDir, (TUint)driveLetter );

    User::LeaveIfError( stream.Temp( iFileServer,
                                   cidTemp,
                                   aTempFile,
                                   EFileWrite ) );

#endif

    CleanupClosePushL( stream );

    for (TInt i = 0; i < 16; i++)
        {
        CDir* files = NULL;

        TFileName path = *iDbPath;

        path.Append(i < 10 ? i + '0' : i + 'a' - 10);
        path.Append('\\');

        if ( iFileServer.GetDir(path, KEntryAttDir, ESortNone, files) == KErrNone )
            {
            TInt j;

            CleanupStack::PushL( files );

            for (j = 0; j < files->Count(); j++)
                {
                TFileName tempPath( path );

                tempPath.Append((*files)[j].iName);

                TInt error = KErrNone;
                CDRMRightsData* rights = NULL;

                TRAP( error, rights = CDRMRightsData::OpenL( tempPath, iFileServer ) );
                if( rights )
                    {
                    CleanupStack::PushL( rights );
                    }
                else
                    {
                    if( error != KErrNotFound )
                        {
                        User::Leave( error );
                        }
                    continue;
                    }

                const CDRMCommonData* data = rights->GetCommonDataL();
                stream.WriteUint16L( data->ContentID().Length() );
                stream.WriteL( data->ContentID() );

                CleanupStack::PopAndDestroy(); // rights
                }

            CleanupStack::PopAndDestroy(); // files
            }
        }

    stream.WriteUint16L(0);

    CleanupStack::PopAndDestroy(); // stream

    __UHEAP_MARKEND;
    };

// -----------------------------------------------------------------------------
// CDRMRightsDB::BackupDBL
// creates a backupfile of the current rights database and encrypts it using the
// database encryption key
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
void CDRMRightsDB::BackupDBL( const TDesC& aWTFile,
                              const TDesC8& aEncryptionKey )
    {
    };
*/
// -----------------------------------------------------------------------------
// CDRMRightsDB::MergeDBL
// merges the backup database into the current database and deletes the backup
// file afterwards, any rights objects with counter base rights or non activated
// intervals are not inserted. If an equal combination of content id and unique
// id is found that object is not restored
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
void CDRMRightsDB::MergeDBL()
    {
    };
*/


// -----------------------------------------------------------------------------
// CDRMRightsDB::GetDecryptionKeyL
// returns a pointer to the decryption key of the requested content id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CDRMRightsDB::GetDecryptionKeyL(
    const TDesC8& aContentID)
    {

    TFileName path;
    TInt error = KErrNone;
    HBufC8* key = NULL;
    CDRMRightsData* rights = NULL;

    GetRightsFileNameL( aContentID, path);

    TRAP( error, rights = CDRMRightsData::OpenL( path, iFileServer ) );
    if( rights )
        {
        CleanupStack::PushL( rights );
        }
    else
        {
        if( error == KErrNotFound )
            {
            User::Leave( DRMCommon::ENoRights );
            }
        User::Leave( error );
        }

    key = rights->GetKeyL();

    // Decrypt the key
    ModifyKey( *key );

    CleanupStack::PopAndDestroy(); // rights

    return key;
    };

// -----------------------------------------------------------------------------
// CDRMRightsDB::DeleteDBL
// deletes the rights database and creates a new empty one in it's place
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::DeleteDBL( void )
    {
    DRMLOG( _L( "CDRMRightsDB::DeleteDBL ->" ) );

    // Indicate that the DB is updated
    iLastUpdate.HomeTime();

    CFileMan* fileMan = CFileMan::NewL(iFileServer);
    fileMan->RmDir(*iDbPath);
    delete fileMan;

    InitializeDatabaseL();

    DRMLOG( _L( "CDRMRightsDB::DeleteDBL <-" ) );
    };


// -----------------------------------------------------------------------------
// CDRMRightsDB::GetAmountOfRightsObjectsL
// returns the amount of different content id's in the database
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt32 CDRMRightsDB::GetAmountOfRightsObjectsL()
    {
    TInt32 amount( 0 );

    for (TInt i = 0; i < 16; i++)
        {
        CDir* files = NULL;

        TFileName path = *iDbPath;

        path.Append(i < 10 ? i + '0' : i + 'a' - 10);
        path.Append('\\');

        User::LeaveIfError(
            iFileServer.GetDir(path, KEntryAttDir, ESortNone, files) );

#ifdef _DEBUG
        if ( !files )
            {
            DRMLOG( _L( "CDRMRightsDB::GetAmountOfRightsObject: GetDir returned NULL pointer!" ) );
            User::Leave( KErrGeneral );
            }
#endif

        amount += files->Count();
        delete files;
        }

    return amount;
    };



// -----------------------------------------------------------------------------
// CDRMRightsDB::AddDomainROL
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::AddDomainROL( const TDesC8& aRoId, const TDesC8& aXmlData )
    {
    RFile file;
    TFileName path;
    TPtr8 numWrite(NULL,0);
    TInt num = 0;

    DRMLOG( _L( "CDRMRightsDB::AddDomainROL ->" ) );

    // Indicate that the DB is updated
    iLastUpdate.HomeTime();

    // Get the size and if it is zero, leave
    num = aXmlData.Size();
    if( !num )
        {
        User::Leave(KErrArgument);
        }

    // Get the filename
    GetXMLFileNameL( aRoId, path );

    // Replace whatever is in there
    User::LeaveIfError( file.Replace( iFileServer, path, EFileWrite ) );
    CleanupClosePushL( file );

    // write the size of the data
    numWrite.Set(reinterpret_cast<TUint8*>(&num), sizeof(TInt), sizeof(TInt));

    // needs a check if there is enough diskspace for sizeof(TInt) + num bytes

    User::LeaveIfError( file.Write( numWrite ) );

    // write the data
    User::LeaveIfError( file.Write( aXmlData ) );

    CleanupStack::PopAndDestroy(); // file

    DRMLOG( _L( "CDRMRightsDB::AddDomainROL <-" ) );
    };

// -----------------------------------------------------------------------------
// CDRMRightsDB::GetDomainROL
// -----------------------------------------------------------------------------
//
HBufC8* CDRMRightsDB::GetDomainROL( const TDesC8& aRoId )
    {
    RFile file;
    TFileName path;
    TPtr8 inRead(NULL,0);
    TInt num = 0;
    HBufC8* data = NULL;

    // Get the filename
    GetXMLFileNameL( aRoId, path );

    // Replace whatever is in there
    User::LeaveIfError( file.Open( iFileServer, path, EFileRead ) );
    CleanupClosePushL( file );

    // read the size of the data
    inRead.Set(reinterpret_cast<TUint8*>(&num), 0, sizeof(TInt));
    User::LeaveIfError( file.Read( inRead, sizeof(TInt) ) );

    if( num <= 0 )
        {
        User::Leave(KErrCorrupt);
        }

    data = HBufC8::NewMaxLC( num );

    // read the data
    inRead.Set( const_cast<TUint8*>( data->Ptr() ), 0, num );
    User::LeaveIfError( file.Read( inRead, num ) );

    CleanupStack::Pop(); // data
    CleanupStack::PopAndDestroy(); // Close the file
    return data;
    };

// -----------------------------------------------------------------------------
// CDRMRightsDB::DeleteDomainROL
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::DeleteDomainROL( const TDesC8& aRoId )
    {
    TFileName path;

    DRMLOG( _L( "CDRMRightsDB::DeleteDomainROL ->" ) );

    // Indicate that the DB is updated
    iLastUpdate.HomeTime();

    // Get the filename
    GetXMLFileNameL( aRoId, path );

    User::LeaveIfError( iFileServer.Delete( path ) );

    DRMLOG( _L( "CDRMRightsDB::DeleteDomainROL <-" ) );
    };

// -----------------------------------------------------------------------------
// CDRMRightsDB::DeleteExpiredPermissionsL
// -----------------------------------------------------------------------------
//
CDRMRightsCleaner* CDRMRightsDB::DeleteExpiredPermissionsL( const TTime& aTime,
                                                            TRequestStatus& aStatus )
    {


    CDRMRightsCleaner* cleaner = CDRMRightsCleaner::NewL( iFileServer,
                                                          const_cast<CDRMRightsDB*>(this),
                                                          aStatus,
                                                          *iDbPath,
                                                          aTime);
    return cleaner;
    }

// -----------------------------------------------------------------------------
// CDRMRightsDB::NameContentL
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::NameContentL( const TDesC8& aContentId,
                                 const TDesC& aName )
    {
    TFileName path;
    TInt error = KErrNone;
    CDRMRightsData* rights = NULL;
    CDRMCommonData* data = NULL;

    GetRightsFileNameL( aContentId, path);

    TRAP( error, rights = CDRMRightsData::OpenL( path, iFileServer ) );
    if( rights )
        {
        CleanupStack::PushL( rights );
        }
    else
        {
        User::Leave( error );
        }

    data = const_cast<CDRMCommonData*>(rights->GetCommonDataL());

    data->SetContentNameL( aName );

    rights->UpdateCommonDataL( data );

    CleanupStack::PopAndDestroy(); // rights
    };


// -----------------------------------------------------------------------------
// CDRMRightsDB::ContentNameL
// -----------------------------------------------------------------------------
//
HBufC* CDRMRightsDB::ContentNameLC( const TDesC8& aContentId )
    {
    TFileName path;
    CDRMRightsData* rights( NULL );
    HBufC* name( NULL );

    GetRightsFileNameL( aContentId, path );

    rights = CDRMRightsData::OpenL( path, iFileServer );
    CleanupStack::PushL( rights );

    name = const_cast< CDRMCommonData* >( rights->GetCommonDataL() )
            ->ContentName().AllocL();

    CleanupStack::PopAndDestroy(); // rights
    CleanupStack::PushL( name );

    return name;
    }

// -----------------------------------------------------------------------------
// CDRMRightsDB::DeleteExpiredL
// -----------------------------------------------------------------------------
//
TBool CDRMRightsDB::DeleteExpiredL( const TFileName& aFileName,
                                   const TTime& aTime )
    {
    CDRMRightsData* rights = NULL;
    TInt amountLeft = -1;
    TBool retVal = EFalse;
    TBool parents = EFalse;

    DRMLOG( _L( "CDRMRightsDB::DeleteExpiredL ->" ) );

    // Indicate that the DB is updated
    iLastUpdate.HomeTime();

    // Open the rights file
    DRMLOG( _L("Opening the file"));
    rights = CDRMRightsData::OpenLC( aFileName, iFileServer );

    DRMLOG( _L("Running Delete"));
    amountLeft = rights->DeleteExpiredPermissionsL( aTime, parents );

    DRMLOG2( _L("Checking for left RO:s %d"), amountLeft );

    // See if any permissions are left if not check if the whole file
    // can be proposed to be deleted or not, Java files require uninstallation
    // so those need to be checked
    if( !amountLeft && !parents )
        {
        // get the common data
        const CDRMCommonData* common= rights->GetCommonDataL();

        // If it is a java file, dont allow deletion
        if( !common->ContentName().Right(4).CompareF(KJavaExtension) )
            {
            DRMLOG( _L("Is java file, do not delete"));
            retVal = EFalse;
            }
        else if( !common->ContentName().Right(4).CompareF(KSISExtension) )
            {
            DRMLOG( _L("Is an installation package, do not delete"));
            retVal = EFalse;
            }
        else
            {
            retVal = ETrue;
            }
        }
    CleanupStack::PopAndDestroy(); // rights

    DRMLOG( _L( "CDRMRightsDB::DeleteExpiredL <-" ) );

    return retVal;
    }


// -----------------------------------------------------------------------------
// CDRMRightsDB::DeleteExpiredL
// -----------------------------------------------------------------------------
//
TBool CDRMRightsDB::DeleteExpiredL( const TFileName& aFileName,
                                   const TTime& aTime,
                                   const TDesC8& aContentId )
    {
    CDRMRightsData* rights = NULL;
    TInt amountLeft = -1;
    TBool retVal = EFalse;
    TBool parents = EFalse;

    DRMLOG( _L( "CDRMRightsDB::DeleteExpiredL(2) ->" ) );

    // if there is a consumption session ongoing, we can't delete it at all
    // or the decryption will not function properly after pause etc
    if( iRightsServer->IsAccessingUrl( aContentId ) != KErrNotFound )
        {
        return EFalse;    
        }

    // Indicate that the DB is updated
    iLastUpdate.HomeTime();

    // Open the rights file
    DRMLOG( _L("Opening the file"));
    rights = CDRMRightsData::OpenLC( aFileName, iFileServer );

    DRMLOG( _L("Running Delete"));
    amountLeft = rights->DeleteExpiredPermissionsL( aTime, parents );

    DRMLOG2( _L("Checking for left RO:s %d"), amountLeft );

    // See if any permissions are left if not check if the whole file
    // can be proposed to be deleted or not, Java files require uninstallation
    // so those need to be checked
    if( !amountLeft && !parents )
        {
        // get the common data
        const CDRMCommonData* common= rights->GetCommonDataL();

        // If it is a java file, dont allow deletion
        if( !common->ContentName().Right(4).CompareF(KJavaExtension) )
            {
            DRMLOG( _L("Is java file, do not delete"));
            retVal = EFalse;
            }
        else if( !common->ContentName().Right(4).CompareF(KSISExtension) )
            {
            DRMLOG( _L("Is an installation package, do not delete"));
            retVal = EFalse;
            }
        else
            {
            retVal = ETrue;
            }
        }
    CleanupStack::PopAndDestroy(); // rights

    DRMLOG( _L( "CDRMRightsDB::DeleteExpiredL <-" ) );

    if( iRightsServer->HasActiveCountConstraint( aContentId ))
        {
        retVal = EFalse;    
        }

    return retVal;
    }


// -----------------------------------------------------------------------------
// CDRMRightsDB::GetUdtDataLC
// -----------------------------------------------------------------------------
//
HBufC8* CDRMRightsDB::GetUdtDataLC()
    {
#ifdef __DRM_OMA2
    HBufC8* udtData = HBufC8::NewMaxLC( KMaxUDTDataSize );
    TFileName backupFile;
    RFile input;
    TInt pos = KUdtDataPos;
    TPtr8 inRead( udtData->Des() );

#ifndef RD_MULTIPLE_DRIVE

    backupFile.Copy( KBackupDirectory );

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
    iFileServer.DriveToChar( driveNumber, driveLetter );

    TFileName backupDir;
    backupDir.Format( KBackupDir, (TUint)driveLetter );

    backupFile.Copy( backupDir );

#endif

    backupFile.Append( KRightsDbBackupFile );

    // Open the udt file
    User::LeaveIfError( input.Open( iFileServer, backupFile, EFileRead ) );
    CleanupClosePushL( input );

    // Find the correct spot in the file
    User::LeaveIfError( input.Seek( ESeekStart, pos ) );

    // Read the data from the file
    User::LeaveIfError( input.Read( inRead, KMaxUDTDataSize ) );

    CleanupStack::PopAndDestroy(); // input
    return udtData;
 #else
    User::Leave(KErrNotSupported);
    return NULL;
 #endif
    };

// -----------------------------------------------------------------------------
// CDRMRightsDB::InitiateUdtL
// -----------------------------------------------------------------------------
//
#ifdef __DRM_OMA2
void CDRMRightsDB::InitiateUdtL( const TDesC8& aKey )
#else
void CDRMRightsDB::InitiateUdtL( const TDesC8& )
#endif // __DRM_OMA2
    {
#ifdef __DRM_OMA2
    TFileName backupFile;
    RFile input;
    HBufC8* keyData = NULL;

    MDrmKeyStorage* storage = DrmKeyStorageNewL();

    TCleanupItem storageCleanup( DeleteObject, storage );
    CleanupStack::PushL(storageCleanup);

    keyData = storage->RsaDecryptL( aKey );

    CleanupStack::PopAndDestroy();// storageCleanup
    CleanupStack::PushL( keyData );

#ifndef RD_MULTIPLE_DRIVE

    backupFile.Copy( KBackupDirectory );

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
    iFileServer.DriveToChar( driveNumber, driveLetter );

    TFileName backupDir;
    backupDir.Format( KBackupDir, (TUint)driveLetter );

    backupFile.Copy( backupDir );

#endif

    backupFile.Append( KRightsDbBackupFile );

    // Open the udt file
    User::LeaveIfError( input.Open( iFileServer, backupFile, EFileRead ) );
    CleanupClosePushL( input );

    // Add the cleanup item to the cleanup stack
    TCleanupItem resetAndDestroy( CleanupData, reinterpret_cast<TAny*>(this) );
    CleanupStack::PushL( resetAndDestroy );

    // DEBUG DEBUG DEBUG: ignore the error
    TRAPD( error, RestoreContentFromFileL( input,
                             keyData->Right(KEncryptionKeySize),
                             KDRMUDTBackup ) );
    if( error )
        {
        // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("InitiateUdtL: Restore failed\n\r"));
        User::Leave(error);
        }
    CleanupStack::PopAndDestroy( 3 ); // input, cleanup item, keyData
#else
    User::Leave(KErrNotSupported);
#endif // __DRM_OMA2
    };


// -----------------------------------------------------------------------------
// CDRMRightsDB::GetContentIDListL
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::GetContentIDListL( RPointerArray<HBufC8>& aArray )
    {
    HBufC8* contentId = NULL;

    for (TInt i = 0; i < 16; i++)
        {
        CDir* files = NULL;

        TFileName path = *iDbPath;

        path.Append(i < 10 ? i + '0' : i + 'a' - 10);
        path.Append('\\');

        if ( iFileServer.GetDir(path, KEntryAttDir, ESortNone, files) == KErrNone )
            {
            CleanupStack::PushL( files );

            for (TInt j = 0; j < files->Count(); j++)
                {
                TFileName tempPath( path );

                tempPath.Append((*files)[j].iName);

                TInt error = KErrNone;
                CDRMRightsData* rights = NULL;

                TRAP( error, rights = CDRMRightsData::OpenL( tempPath, iFileServer ) );
                if( rights )
                    {
                    CleanupStack::PushL( rights );
                    }
                else
                    {
                    if( error != KErrNotFound )
                        {
                        User::Leave( error );
                        }
                    continue;
                    }

                const CDRMCommonData* data = rights->GetCommonDataL();

                contentId = data->ContentID().AllocLC();
                aArray.AppendL( contentId );
                CleanupStack::Pop(); // contentId

                CleanupStack::PopAndDestroy(); // rights
                }

            CleanupStack::PopAndDestroy(); // files
            }
        }
    };


// -----------------------------------------------------------------------------
// CDRMRightsDB::WriteEncryptedStreamL
// The file will be opened and closed in CDRMBackup
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::WriteEncryptedStreamL( RWriteStream& aStream,
                                          const TDesC8& aMessageData,
                                          TDes8& aIv,
                                          TDes8& aRemainder,
                                          HBufC8*& aEncryptionBuffer,
                                          TInt& aBytesWritten )
    {
    TInt i = 0;
    TInt n = 0;
    TInt size = 0;
    TPtr8 ptr(aEncryptionBuffer->Des());
    TPtrC8 data;

    data.Set(aMessageData);
    if (aRemainder.Size() > 0 && aRemainder.Size() + data.Size() >= KEncryptionKeySize)
        {
        ptr.Copy(aRemainder);
        n = Min(KEncryptionKeySize - aRemainder.Size(), data.Size());
        ptr.Append(data.Left(n));

        EncryptL(aIv, ptr, EFalse);

        aStream.WriteL(ptr);
        aBytesWritten += ptr.Size();
        aIv.Copy(ptr.Right(KEncryptionKeySize));
        data.Set(data.Right(data.Size() - n));
        aRemainder.SetLength(0);
        }

    size = data.Size();
    for (i = 0; size > KEncryptionKeySize; i += KMaxEncryptionSize)
        {
        n = Min(KMaxEncryptionSize, ((size / KEncryptionKeySize)
                * KEncryptionKeySize));
        ptr.Copy(data.Mid(i, n));

        EncryptL(aIv, ptr, EFalse);

        aStream.WriteL(ptr);
        aBytesWritten += ptr.Size();
        aIv.Copy(ptr.Right(KEncryptionKeySize));
        size -= n;
        }
    aRemainder.Append(data.Right(size));
    }


// -----------------------------------------------------------------------------
// CDRMRightsDB::FinalizeEncryptedStreamL
// finalize the encryption
// -----------------------------------------------------------------------------
void CDRMRightsDB::FinalizeEncryptedStreamL( RWriteStream& aStream,
                                             TDes8& aIv,
                                             TDes8& aRemainder,
                                             HBufC8*& aEncryptionBuffer,
                                             TInt& aBytesWritten )
    {
    TPtr8 ptr(aEncryptionBuffer->Des());

    ptr.Copy(aRemainder);
    EncryptL(aIv, ptr, ETrue);
    aStream.WriteL(ptr);
    aStream.CommitL();
    aBytesWritten += ptr.Size();
    }


// -----------------------------------------------------------------------------
// CDRMRightsDB::BackupContentToFileL
// The file will be opened and closed in CDRMBackup
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::BackupContentToFileL( RFile& aBackupFile,
                                         const TDesC8& /* aEncryptionKey */,
                                         const TInt /* aMode */ )
    {
    //RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("BackupContentToFileL\n\r"));
    // In UDT we need to check the mode
    TInt fileSize = 0;

    TUint permissions = 0;
    TUint8 continueMarker = 1;
    RFile copyHandle;
    TInt error = KErrNone;

    HBufC8* dataBuffer = NULL; // This is the buffer which gets reallocated
                               // if more space for data is required
                               // buf only if more space is needed, otherwise
                               // the size remains the same
                               // for start reserve as much as for the other buffer
                               // This is probably enough for all rights object
                               // But it's best to be sure

    TBuf8<KEncryptionKeySize> encIV;
    TBuf8<KEncryptionKeySize> remainder;
    HBufC8* encryptionBuffer = NULL; // The buffer used for the encryption in the middle
                                     // reserved only once
    TInt bytesWritten = 0;
    TPtr8 writeData(NULL,0,0);
    RMemWriteStream memStream;

    dataBuffer = HBufC8::NewLC( KMaxEncryptionSize );
    encryptionBuffer = HBufC8::NewLC( KMaxEncryptionSize );

    User::LeaveIfError( copyHandle.Duplicate(aBackupFile) );

    RFileWriteStream stream( copyHandle );
    CleanupClosePushL( stream );

    // first write the first 4 bytes empty, in the end the size is written there
    stream.WriteInt32L( fileSize );

#ifdef __DRM_OMA2
    AddUDTDataL( stream );
#endif

    encIV.SetLength(KEncryptionKeySize);
    // fill the iv with rnd data and write it to stream:
    MDrmKeyStorage* storage = DrmKeyStorageNewL();

    TCleanupItem storageCleanup( DeleteObject, storage );
    CleanupStack::PushL(storageCleanup);

    storage->RandomDataGetL(encIV,KEncryptionKeySize);
    CleanupStack::PopAndDestroy();//storageCleanup

    DRMLOG(_L("random encIV:"));
    DRMLOGHEX(encIV);


    stream.WriteL( encIV );

    // loop over all the rights objects and write them to the file

    for (TInt i = 0; i < 16; i++)
        {
        CDir* files = NULL;

        TFileName path = *iDbPath;

        path.Append(i < 10 ? i + '0' : i + 'a' - 10);
        path.Append('\\');

        if ( iFileServer.GetDir(path, KEntryAttDir, ESortNone, files) == KErrNone )
            {
            TInt j = 0;

            CleanupStack::PushL( files );

            for (j = 0; j < files->Count(); j++)
                {
                TFileName tempPath( path );
                CDRMPointerArray<CDRMPermission> *permissionArray = CDRMPointerArray<CDRMPermission>::NewLC();
                permissionArray->SetAutoCleanup( ETrue );

                tempPath.Append((*files)[j].iName);

                CDRMRightsData* rights = NULL;

                rights = CDRMRightsData::OpenLC( tempPath, iFileServer );

                // WRITE THE GENERIC DATA ABOUT THE OBJECT
                //-----------------------------------------------------------
                // First count the size we want to use:


                writeData.Set(&continueMarker,
                                sizeof(continueMarker),
                                sizeof(continueMarker));
                WriteEncryptedStreamL( stream,
                                       writeData,
                                       encIV,
                                       remainder,
                                       encryptionBuffer,
                                       bytesWritten );

                TInt sizeOfBuf = KEncryptionKeySize; // continue marker

                // Get the common data, Externalize it
                CDRMCommonData* data =
                  const_cast<CDRMCommonData*>(rights->GetCommonDataL());
                sizeOfBuf += data->Size();


                // Realloc if needed, probably wont be:
                if( sizeOfBuf > dataBuffer->Des().MaxSize() )
                    {
                    dataBuffer->ReAllocL( sizeOfBuf );
                    }

                // Write the size of the permission, used for decryption
                writeData.Set( reinterpret_cast<TUint8*>(&sizeOfBuf),
                               sizeof(sizeOfBuf),
                               sizeof(sizeOfBuf));

                WriteEncryptedStreamL( stream,
                                       writeData,
                                       encIV,
                                       remainder,
                                       encryptionBuffer,
                                       bytesWritten );


                memStream.Open( (TAny*)( dataBuffer->Ptr() ), sizeOfBuf );
                CleanupClosePushL( memStream );

                // The common data
                data->ExternalizeL( memStream );

                // The key
                // Get the key, Externalize it
                HBufC8* encKey = rights->GetKeyL();
                if( encKey == NULL )
                    {
                    encKey = HBufC8::NewMaxL(KEncryptionKeySize);
                    Mem::FillZ( const_cast<TUint8*>(encKey->Ptr()), KEncryptionKeySize);
                    }
                else
                    {
                    // Decrypt the key, the file is encrypted, it doesn't need to be twice
                    ModifyKey( *encKey );
                    }

                CleanupStack::PushL( encKey );

                memStream.WriteL( *encKey );
                CleanupStack::PopAndDestroy(); // enc key

                writeData.Set( const_cast<TUint8*>(dataBuffer->Ptr()),
                               sizeOfBuf, sizeOfBuf );

                WriteEncryptedStreamL( stream,
                                       writeData,
                                       encIV,
                                       remainder,
                                       encryptionBuffer,
                                       bytesWritten );


                CleanupStack::PopAndDestroy(); // memStream

                // Get the permissions and externalize their amount and them
                // If there are none ignore the error and just save the normal data
                TRAP(error, rights->FetchAllPermissionsL( *permissionArray ) );
                if( error )
                    {
                    if( !( error == KErrCANoRights || error == KErrCANoPermission ) )
                        {
                        User::LeaveIfError( error );
                        }
                    }

                // WRITE THE AMOUNT OF PERMISSIONS
                //-----------------------------------------------------------
                permissions = permissionArray->Count();

                writeData.Set(reinterpret_cast<TUint8*>(&permissions),
                              sizeof(permissions),
                              sizeof(permissions));

                WriteEncryptedStreamL( stream,
                                       writeData,
                                       encIV,
                                       remainder,
                                       encryptionBuffer,
                                       bytesWritten );

                for( TInt count = 0; count < permissions; count++ )
                    {
                    // WRITE EACH PERMISSION
                    //-----------------------------------------------------------
                    sizeOfBuf = (*permissionArray)[count]->Size();

                    // Realloc if needed, probably wont be:
                    if( sizeOfBuf > dataBuffer->Des().MaxSize() )
                        {
                        dataBuffer->ReAllocL( sizeOfBuf );
                        }

                    // Write the size of the permission, used for decryption
                    writeData.Set(reinterpret_cast<TUint8*>(&sizeOfBuf),
                                  sizeof(sizeOfBuf),
                                  sizeof(sizeOfBuf));

                    WriteEncryptedStreamL( stream,
                                           writeData,
                                           encIV,
                                           remainder,
                                           encryptionBuffer,
                                           bytesWritten );

                    // write the actual data
                    memStream.Open( (TAny*)( dataBuffer->Ptr() ), sizeOfBuf );
                    CleanupClosePushL( memStream );

                    (*permissionArray)[count]->ExternalizeL( memStream );

                    writeData.Set( const_cast<TUint8*>(dataBuffer->Ptr()),
                                   sizeOfBuf, sizeOfBuf );
                    WriteEncryptedStreamL( stream,
                                           writeData,
                                           encIV,
                                           remainder,
                                           encryptionBuffer,
                                           bytesWritten );

                    CleanupStack::PopAndDestroy();  // memstream

                    }
                CleanupStack::PopAndDestroy(2); // rights, permissionArray
                }

            CleanupStack::PopAndDestroy(); // files
            }
        }
    continueMarker = 0;
    writeData.Set(&continueMarker,
                  sizeof(continueMarker),
                  sizeof(continueMarker));
    WriteEncryptedStreamL( stream,
                           writeData,
                           encIV,
                           remainder,
                           encryptionBuffer,
                           bytesWritten );

    // Finalize the stream:
    FinalizeEncryptedStreamL( stream,
                              encIV,
                              remainder,
                              encryptionBuffer,
                              bytesWritten );

    CleanupStack::PopAndDestroy();// stream

    // Attach it to the file again, set the stream to the beginning

    User::LeaveIfError( copyHandle.Duplicate( aBackupFile ) );

    stream.Attach( copyHandle );
    CleanupClosePushL( stream );

    aBackupFile.Size( fileSize );

    // write the size of the file including the 4 bytes in the start
    stream.WriteInt32L( fileSize );

    CleanupStack::PopAndDestroy(3); // stream, databuffer, encryptionbuffer
    // DEBUG
    // Perform restore:
    // RestoreContentFromFileL( aBackupFile, *iKey, 0 );
    };

// -----------------------------------------------------------------------------
// CDRMRightsDB::RestoreContentFromFileL
// The file will be opened and closed in CDRMBackup
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::RestoreContentFromFileL( RFile& aBackupFile,
                                            const TDesC8& aEncryptionKey,
                                            const TInt aMode )
    {
    // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("RestoreContentFromFileL\n\r"));
    TInt8 continueMarker = 1;
    TBuf8<16> key;
    TBuf8<16> encryptionKey;
    TInt permissions = 0;
    CDRMPermission* permission = CDRMPermission::NewLC();
    CDRMCommonData *commonData = NULL;
    CDRMPointerArray<CDRMPermission> *permissionArray = CDRMPointerArray<CDRMPermission>::NewLC();
    permissionArray->SetAutoCleanup( ETrue );
    TDRMUniqueID uniqueID = 0;
    RFile fileHandle;
    TInt readPos = 0;
    TInt size = 0;
    TInt dataLeft = 0;
    TPtr8 keyData(NULL,0,0);

    // maintain knowledge about stateful rights not being restored
    TBool stateful = EFalse;


    DRMLOG( _L( "CDRMRightsDB::RestoreContentFromFileL ->" ) );

    // Indicate that the DB is updated
    iLastUpdate.HomeTime();

    key.SetLength( KEncryptionKeySize );
    fileHandle.Duplicate( aBackupFile );
    CleanupClosePushL( fileHandle );

    HBufC8* dataBuffer = NULL; // This is the buffer which gets reallocated
                               // if more space for data is required
                               // buf only if more space is needed, otherwise
                               // the size remains the same
                               // for start reserve as much as for the other buffer
                               // This is probably enough for all rights object
                               // But it's best to be sure

    TBuf8<KEncryptionKeySize> encIV;

    TPtr8 readData(NULL,0,0);

    switch( aMode )
        {
        case KDRMNormalBackup:
            {
            encryptionKey.Copy( *iKey );
            }
            break;
        case KDRMUDTBackup:
            {
            encryptionKey.Copy(aEncryptionKey);
            }
            break;
        default:
            User::Leave( KErrArgument );
        }

    encryptionKey.SetLength(KEncryptionKeySize);


    dataBuffer = HBufC8::NewLC( KMaxEncryptionSize );

    encIV.SetLength( KEncryptionKeySize );

    // Check that the decryption works, if it doesn't then the
    // key is faulty
    User::LeaveIfError( aBackupFile.Size( readPos ) );
    if( readPos < KUdtDataPos+KMaxUDTDataSize+(KEncryptionKeySize*2) )
        {
          // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("RestoreContentFromFileL : corrupt\n\r"));
        User::Leave(KErrCorrupt);
        }
    readPos -= KEncryptionKeySize*2;

    User::LeaveIfError( fileHandle.Seek( ESeekStart, readPos ) );

    // Read the IV
    readData.Set( const_cast<TUint8*>(encIV.Ptr()), 0, KEncryptionKeySize );
    User::LeaveIfError( fileHandle.Read( readData, KEncryptionKeySize ) );

    // Read the data:
    readData.Set( const_cast<TUint8*>( dataBuffer->Ptr()), 0,
                  KEncryptionKeySize );
    User::LeaveIfError( fileHandle.Read( readData, KEncryptionKeySize ) );

    DecryptL( encIV, readData, EFalse, encryptionKey );

    // Check if the padding matches, if not the function will leave
    CheckPaddingL( readData );

    // End checking

    // Now we are ready to go through the file
    //-----------------------------------------------------------------------

    // Duplicate file handle
    readPos = KUdtDataPos+KMaxUDTDataSize;
    User::LeaveIfError( fileHandle.Seek( ESeekStart, readPos ) );
    iMemStream.Close();

    ReadDataL( fileHandle, encIV, readData, dataBuffer,
               dataLeft, size, ETrue, encryptionKey );

    iMemStream.Open( const_cast<TUint8*>( readData.Ptr()), dataLeft);

    // Read data now contains decrypted data which we can go through

    // loop over all the rights objects and merge them to the db
    while( true )
        {
        keyData.Set(const_cast<TUint8*>(key.Ptr()), 0, KEncryptionKeySize);

        if( dataLeft < 1 )
            {
            size = 1;
            iMemStream.Close();
            ReadDataL( fileHandle, encIV, readData, dataBuffer,
                       dataLeft, size, EFalse, encryptionKey );
            iMemStream.Open( const_cast<TUint8*>( readData.Ptr()), dataLeft);
            }

        continueMarker = iMemStream.ReadInt8L();
        dataLeft -= 1;

        if( !continueMarker  )
            {
            // RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("RestoreContentFromFileL : exit from loop\n\r"));
            break;
            }

         if( dataLeft < 4 )
            {
            size = 4;
            iMemStream.Close();
            ReadDataL( fileHandle, encIV, readData, dataBuffer,
                       dataLeft, size, EFalse, encryptionKey );
            iMemStream.Open( const_cast<TUint8*>( readData.Ptr()), dataLeft);
            }

        // Read the size of the data:
        size = iMemStream.ReadInt32L();
        dataLeft -= 4;

        if( size > dataLeft )
            {
            iMemStream.Close();
            ReadDataL( fileHandle, encIV, readData, dataBuffer,
                       dataLeft, size, EFalse, encryptionKey );
            iMemStream.Open( const_cast<TUint8*>( readData.Ptr()),dataLeft);
            }

        // Read the common data:
        commonData = CDRMCommonData::NewLC();
        commonData->InternalizeL(iMemStream);

        // Read the content encryption key
        iMemStream.ReadL( keyData, KEncryptionKeySize );
        dataLeft -= size;

        if( dataLeft < 4 )
            {
            size = 4;
            iMemStream.Close();
            ReadDataL( fileHandle, encIV, readData, dataBuffer,
                       dataLeft, size, EFalse, encryptionKey );
            iMemStream.Open( const_cast<TUint8*>( readData.Ptr()), dataLeft);
            }

        // read the amount of permissions
        permissions = iMemStream.ReadInt32L();
        dataLeft -= 4;


        // Create the entry if needed
        TFileName path;
        TInt error = KErrNone;
        CDRMRightsData* rights = NULL;
        TBuf8<16> nullDesc;
        TBuf<16> nullDesc2;
        HBufC8* oldKey = NULL;
        TInt8 insertPerm = 1;
        TBool doInsert = ETrue;
        TBool keyExists = EFalse;

        for( TInt counter = 0; counter < KEncryptionKeySize; counter++ )
            {
            if( key[counter] != 0x00 )
                {
                keyExists = ETrue;
                counter = KEncryptionKeySize;
                }
            }


        // Encrypt the key
        if( keyExists )
            {
            ModifyKey( key );
            }

        GetRightsFileNameL( commonData->ContentID(), path);

        // Indicate that the DB is updated
        iLastUpdate.HomeTime();

        TRAP( error, rights = CDRMRightsData::OpenL( path, iFileServer ) );

        if( error == KErrNotFound )
            {
            // Indicate that the DB is updated
            iLastUpdate.HomeTime();

            if( keyExists )
                {
                TRAP(error, rights = CDRMRightsData::NewL( commonData, key, path, iFileServer ) );
                }
            else
                {
                TRAP(error, rights = CDRMRightsData::NewL( commonData, KNullDesC8, path, iFileServer ) );
                }

            CleanupStack::Pop(); // Pop Common Data
            CleanupStack::PushL(rights); // Push the rights in:
            insertPerm = -1;
            }
        else
            {
            // Destroy common data if it already exits per OpenL
            CleanupStack::PopAndDestroy();

            // Leave if another error occurred
            User::LeaveIfError( error );

            if( rights )
                {
                CleanupStack::PushL( rights );
                oldKey = rights->GetKeyL();

                // if there is no key and there is one in the new one
                if( !oldKey && keyExists )
                    {
                    insertPerm = 0;
                    }
                else if( oldKey && key.Compare( *oldKey ) ) // If the key is different
                    {
                    insertPerm = 0;
                    }
                if( oldKey )
                    {
                    delete oldKey;
                    }
                }
            }

        if( !rights )
            {
            User::Leave( KErrGeneral );
            }




        if( insertPerm == -1 ) // Add everything no checks needed
            {
            for( TInt count = 0; count < permissions; count++ )
                {
                if( dataLeft < 4 )
                    {
                    size = 4;
                    iMemStream.Close();
                    ReadDataL( fileHandle, encIV, readData, dataBuffer,
                               dataLeft, size, EFalse, encryptionKey );
                    iMemStream.Open( const_cast<TUint8*>( readData.Ptr()), dataLeft);
                    }

                // Read the size of the data:
                size = iMemStream.ReadInt32L();
                dataLeft -= 4;

                if( size > dataLeft )
                    {
                    iMemStream.Close();
                    ReadDataL( fileHandle, encIV, readData, dataBuffer,
                               dataLeft, size, EFalse, encryptionKey );
                    iMemStream.Open( const_cast<TUint8*>( readData.Ptr()), dataLeft);
                    }

                permission->InternalizeL( iMemStream );
                dataLeft -= size;

                if( !permission->Stateful() || aMode == KDRMUDTBackup )
                    {
                    rights->StoreNewPermissionL( *permission, uniqueID );
                    }
                else if( !stateful )
                    {
                    stateful = ETrue;
                    }
                }
            }
        else if( insertPerm == 1) // Add stuff that doesn't match the times
            {
            // Indicate that the DB is updated
            iLastUpdate.HomeTime();

            // If there are no rights that's an ok thing
            // Fix memory handling
            TRAP( error, rights->FetchAllPermissionsL( *permissionArray ) );
            if( error )
                {
                if( !( error == KErrCANoRights ||
                       error == KErrCANoPermission ) )
                    {
                    User::LeaveIfError(error);
                    }

                }


            for( TInt count = 0; count < permissions; count++ )
                {
                if( dataLeft < 4 )
                    {
                    size = 4;
                    iMemStream.Close();
                    ReadDataL( fileHandle, encIV, readData, dataBuffer,
                               dataLeft, size, EFalse, encryptionKey );
                    iMemStream.Open( const_cast<TUint8*>( readData.Ptr()), dataLeft);
                    }

                // Read the size of the data:
                size = iMemStream.ReadInt32L();
                dataLeft -= 4;

                if( size > dataLeft )
                    {
                    iMemStream.Close();
                    ReadDataL( fileHandle, encIV, readData, dataBuffer,
                               dataLeft, size, EFalse, encryptionKey );
                    iMemStream.Open( const_cast<TUint8*>( readData.Ptr()), dataLeft);
                    }

                permission->InternalizeL( iMemStream );
                dataLeft -= size;

                doInsert = ETrue;

                for( TInt perm = 0; perm < permissionArray->Count(); perm++)
                    {
                    if( (*permissionArray)[perm]->iOriginalInsertTime ==
                        permission->iOriginalInsertTime )
                        {
                        doInsert = EFalse;
                        break;
                        }
                    }

                if( doInsert && (!permission->Stateful() || aMode == KDRMUDTBackup ) )
                    {
                    rights->StoreNewPermissionL( *permission, uniqueID );
                    }
                else if( doInsert && !stateful )
                    {
                    stateful = ETrue;
                    }
                }
                permissionArray->ResetAndDestroy();
            }
        else  // Just read it all but dont add anything
            {
            // Indicate that the DB is updated
            iLastUpdate.HomeTime();

            for( TInt count = 0; count < permissions; count++ )
                {
                if( dataLeft < 4 )
                    {
                    size = 4;
                    iMemStream.Close();
                    ReadDataL( fileHandle, encIV, readData, dataBuffer,
                               dataLeft, size, EFalse, encryptionKey );
                    iMemStream.Open( const_cast<TUint8*>( readData.Ptr()), dataLeft);
                    }

                // Read the size of the data:
                size = iMemStream.ReadInt32L();
                dataLeft -= 4;

                if( size > dataLeft )
                    {
                    iMemStream.Close();
                    ReadDataL( fileHandle, encIV, readData, dataBuffer,
                               dataLeft, size, EFalse, encryptionKey );
                    iMemStream.Open( const_cast<TUint8*>( readData.Ptr()), dataLeft);

                    }

                permission->InternalizeL( iMemStream );
                dataLeft -= size;
                }
            }
        CleanupStack::PopAndDestroy(); // rights
        }
    iMemStream.Close();
    CleanupStack::PopAndDestroy( 4 ); // permission, permissionArray, fileHandle, dataBuffer

    DRMLOG( _L( "CDRMRightsDB::RestoreContentFromFileL ->" ) );

    // If there are stateful rights not put to the phone, this is always EFalse for
    // the UDT case leave with the special error case to leave the restored database
    // to enable UDT
    if( stateful )
        {
        User::Leave( KErrPermissionDenied );
        }
    };



// -----------------------------------------------------------------------------
// CDRMRightsDB::HashContentID
// removed the cid: or flk: from the beginning of the content ID before hashing
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::HashContentID( TPtrC8& aHashKey, const TDesC8& aContentID )
    {
    TPtrC8 cid;
    iHasher->Reset();

    if ( !aContentID.Left( KFLKStringLength ).Compare( KFLKString ) )
        {
        cid.Set( aContentID.Right( aContentID.Length()-KCIDStringLength ) );
        }
    else if ( !aContentID.Left( KCIDStringLength ).Compare( KCIDString ) )
        {
        cid.Set( aContentID.Right( aContentID.Length()-KCIDStringLength ) );
        }
    else
        {
        cid.Set( aContentID );
        }

    aHashKey.Set( iHasher->Hash( cid ) );
    };


// -----------------------------------------------------------------------------
// CDRMRightsDB::GetRightsFileNameL
//
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::GetRightsFileNameL(
    const TDesC8& aContentID,
    TFileName& aPath)
    {
    TPtrC8 hash;
    TInt i;
    TInt v;

    aPath.Copy(*iDbPath);
    HashContentID(hash, aContentID);
    v = hash[0] >> 4;
    aPath.Append(v < 10 ? v + '0' : v + 'a' - 10);
    aPath.Append('\\');
    for (i = 0; i < hash.Length(); i++)
        {
        v = hash[i] >> 4;
        aPath.Append(v < 10 ? v + '0' : v + 'a' - 10);
        v = hash[i] & 0x0f;
        aPath.Append(v < 10 ? v + '0' : v + 'a' - 10);
        }
    aPath.Append(KROExtension);
    }

// -----------------------------------------------------------------------------
// CDRMRightsDB::GetXMLFileNameL
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::GetXMLFileNameL(
    const TDesC8& aRoID,
    TFileName& aPath)
    {
    TPtrC8 hash;
    TInt i;
    TInt v;

    aPath.Copy(*iDbPath);
    HashContentID(hash, aRoID);
    aPath.Append(KRODirName);
    for (i = 0; i < hash.Length(); i++)
        {
        v = hash[i] >> 4;
        aPath.Append(v < 10 ? v + '0' : v + 'a' - 10);
        v = hash[i] & 0x0f;
        aPath.Append(v < 10 ? v + '0' : v + 'a' - 10);
        }
    aPath.Append(KXmlExtension);
    }

// -----------------------------------------------------------------------------
// CDRMRightsDB::InitializeDatabaseL
// initializes the database for use, called before every external interface
// function ( not NewL, NewLC )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::InitializeDatabaseL(void)
    {
    TInt i;
    TFileName path;

    DRMLOG( _L( "CDRMRightsDB::InitializeDatabaseL ->" ) );

    iFileServer.MkDirAll(*iDbPath);
    for (i = 0; i < 16; i++)
        {
        // Indicate that the DB is updated
        iLastUpdate.HomeTime();

        path.Copy(*iDbPath);
        path.Append(i < 10 ? i + '0' : i + 'a' - 10);
        path.Append('\\');
        iFileServer.MkDir(path);
        }

    // Indicate that the DB is updated
    iLastUpdate.HomeTime();

    // Domain RO XML dir
    path.Copy(*iDbPath);
    path.Append(KRODirName);
    iFileServer.MkDirAll(path);

    DRMLOG( _L( "CDRMRightsDB::InitializeDatabaseL <-" ) );
    };

// -----------------------------------------------------------------------------
// CDRMRightsDB::GetXMLFileNameL
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::ModifyKey( TDesC8& aKey )
    {
    TInt* ptrOriginal = NULL;
    TInt* ptrCryptKey = NULL;

    // Cast into TInt pointers
    ptrOriginal = reinterpret_cast<TInt*>( const_cast<TUint8*>( aKey.Ptr() ) );
    ptrCryptKey = reinterpret_cast<TInt*>( const_cast<TUint8*>( iKey->Ptr() ) );

    // XOR the key with the DB key
    ptrOriginal[0] ^= ptrCryptKey[0];
    ptrOriginal[1] ^= ptrCryptKey[1];
    ptrOriginal[2] ^= ptrCryptKey[2];
    ptrOriginal[3] ^= ptrCryptKey[3];
    };

// -----------------------------------------------------------------------------
// CDRMRightsDB::AddUDTDataL
// EB = 00 || BT || PS || 00 || D
// -----------------------------------------------------------------------------
//
#ifdef __DRM_OMA2
void CDRMRightsDB::AddUDTDataL( RWriteStream& aStream )
#else
void CDRMRightsDB::AddUDTDataL( RWriteStream& )
#endif // __DRM_OMA2
    {
#ifdef __DRM_OMA2
    TBuf8<MDrmKeyStorage::KRdbSerialNumberLength> serialNumber;
    TBuf8<KMaxUDTDataSize> buffer;
    TUint8* ptr = const_cast<TUint8*>(buffer.Ptr());
    HBufC8* result = NULL;
    TPtr8 udtData( ptr+KDeviceDataBlock, KDeviceDataBlock, KDeviceDataBlock );
    HBufC8* phoneSerialNumber = NULL;
    TInt pos = 0;

    MDrmKeyStorage* storage = DrmKeyStorageNewL();
    TCleanupItem storageCleanup( DeleteObject, storage );
    CleanupStack::PushL(storageCleanup);

    storage->GetRdbSerialNumberL( serialNumber );

    // Fill the descriptor with random data

    TPtr8 random( const_cast<TUint8*>(buffer.Ptr()),
                  KMaxUDTDataSize,
                  KMaxUDTDataSize );

    storage->RandomDataGetL(random,KMaxUDTDataSize);

    DRMLOG(_L("random UDTData:"));
    DRMLOGHEX(buffer);

    // Get the serial number:
    phoneSerialNumber = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *iImei );
    CleanupStack::PushL( phoneSerialNumber );

    // Device public key encrypted Rights Database Serial Number 128 bytes
    // Device public key encrypted Rights Database Encryption Key 128 bytes
    // ----------------------------------------------------------
    // 00 || 02 || padding || 00 || 16bytes || 16 bytes

    // Construct the 128 bit buffer
    ptr = const_cast<TUint8*>(udtData.Ptr());

    // Set the first byte to 0
    // Set the padding type as random padding
    ptr[0] = 0x00;
    ptr[1] = 0x02;

    pos = KDeviceDataBlock;
    pos -= KEncryptionKeySize;

    // insert the key
    udtData.Replace( pos, KEncryptionKeySize, *iKey );

    pos -= MDrmKeyStorage::KRdbSerialNumberLength;

    // insert the db serial number
    udtData.Replace( pos, MDrmKeyStorage::KRdbSerialNumberLength, serialNumber );

    // insert the finish padding block
    pos = pos - 1;
    ptr[pos] = 0x00;

    result = storage->RsaSignL( udtData );
    CleanupStack::PushL( result );

    // Write the data to the stream
    aStream.WriteL( *result, KDeviceDataBlock );

    CleanupStack::PopAndDestroy(); // result

    // Device public key encrypted Device serial number 128 bytes
    // ----------------------------------------------------------
    // 00 || 02 || padding || 00 || Size || Imei

    // Construct the 128 bit buffer
    ptr = const_cast<TUint8*>(buffer.Ptr());
    udtData.Set( ptr, KDeviceDataBlock, KDeviceDataBlock );
    ptr = const_cast<TUint8*>(udtData.Ptr());

    // Set the first byte to 0
    // Set the padding type as random padding
    ptr[0] = 0x00;
    ptr[1] = 0x02;

    pos = KDeviceDataBlock;
    pos -= phoneSerialNumber->Length();

    // insert the phone serial number

    udtData.Replace( pos, phoneSerialNumber->Length(), *phoneSerialNumber );

    pos -= 1;

    // insert the db serial number
    ptr[pos] = phoneSerialNumber->Length();

    // insert the finish padding block
    pos -= 1;
    ptr[pos] = 0x00;

    result = storage->RsaSignL( udtData );
    CleanupStack::PushL( result );

    // Write the data to the stream
    aStream.WriteL( *result, KDeviceDataBlock );

    CleanupStack::PopAndDestroy(); // result

    ptr = const_cast<TUint8*>(buffer.Ptr());

    // UDT public key encrypted Rights Database Serial Number 256 bytes
    // UDT public key encrypted Rights Database Encryption Key 256 bytes
    // ----------------------------------------------------------
    // 00 || 02 || padding || 00 || Size || Imei || 16bytes || 16 bytes

    // Fill the descriptor with random data
    storage->RandomDataGetL(random,KMaxUDTDataSize);

    DRMLOG(_L("random UDTData:"));
    DRMLOGHEX(buffer);

    // Make the beginning of the buffer correct for use:
    ptr = const_cast<TUint8*>(buffer.Ptr());
    udtData.Set( ptr, KMaxUDTDataSize, KMaxUDTDataSize );
    ptr[0] = 0x00;
    ptr[1] = 0x02;

    pos = KMaxUDTDataSize;
    pos -= KEncryptionKeySize;

    // insert the key
    udtData.Replace( pos, KEncryptionKeySize, *iKey );

    pos -= MDrmKeyStorage::KRdbSerialNumberLength;

    // insert the db serial number
    udtData.Replace( pos, MDrmKeyStorage::KRdbSerialNumberLength, serialNumber );

    // insert the phone serial number
    pos -= phoneSerialNumber->Length();
    udtData.Replace( pos, phoneSerialNumber->Length(), *phoneSerialNumber );

    pos -= 4;

    // insert the size of the phoneSerialNumber field
    WriteIntToBlock( phoneSerialNumber->Length(), udtData, pos );

    // insert the finish padding block
    pos -= 1;
    ptr[pos] = 0x00;
    TInt error = KErrNone;

    TPtrC8 createData( udtData.Mid(pos+1));

    TRAP( error, result = storage->UdtEncryptL( createData ));
    // No udt certificate write the block but empty
    if( error )
        {
        result = HBufC8::NewMaxL( 256 );
        Mem::FillZ( const_cast<TUint8*>(result->Ptr()), 256);
        }
    CleanupStack::PushL( result );

    aStream.WriteL( *result, 256 );

    CleanupStack::PopAndDestroy(3); //result,phoneSerialNumber, storageCleanup
#else
    User::Leave(KErrNotSupported);
#endif
    };

// test function
void CDRMRightsDB::CreateDummyUDTFileL()
    {
#ifdef __DRM_OMA2
    TFileName backupFile;
    RFile input;
    TInt fileSize = 4 + 256 + 256;

    backupFile.Copy( _L("c:\\") );
    backupFile.Append( KRightsDbBackupFile );

    // Open the udt file
    User::LeaveIfError( input.Replace( iFileServer, backupFile, EFileRead|EFileWrite ) );
    RFileWriteStream stream( input, 0);

    stream.WriteInt32L( fileSize );
    CleanupClosePushL(stream);

    AddUDTDataL( stream );
    CleanupStack::PopAndDestroy();
#else
    User::Leave(KErrNotSupported);
#endif // __DRM_OMA2
    };


// -----------------------------------------------------------------------------
// CDRMRightsDB::CleanUdtData
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::CleanUdtData()
    {
#ifdef __DRM_OMA2
    TFileName backupFile;

#ifndef RD_MULTIPLE_DRIVE

    backupFile.Copy( KBackupDirectory );

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
    iFileServer.DriveToChar( driveNumber, driveLetter );

    TFileName backupDir;
    backupDir.Format( KBackupDir, (TUint)driveLetter );

    backupFile.Copy( backupDir );

#endif

    backupFile.Append( KRightsDbBackupFile );

    // Open the udt file
    iFileServer.Delete( backupFile );
#endif // __DRM_OMA2
    };

// -----------------------------------------------------------------------------
// CDRMRightsDB::EncryptL
//
// -----------------------------------------------------------------------------
//     // iRdb.Encrypt(aIv, ptr, ETrue);
void CDRMRightsDB::EncryptL( const TDesC8& aIv,
                             TPtr8& aData,
                             TBool aAddPadding )
    {
    CAESEncryptor* aes = NULL;
    CModeCBCEncryptor* cbc = NULL;
    TInt i;
    TInt lastBlockStart;
    TInt dataLength;
    TInt padding;
    TPtr8 d(NULL, 0);

    aes = CAESEncryptor::NewLC(*iKey);
    cbc = CModeCBCEncryptor::NewL(aes, aIv);
    CleanupStack::Pop(); // aes, now owned by cbc
    CleanupStack::PushL(cbc);

    dataLength = aData.Length();
    lastBlockStart = (dataLength / KEncryptionKeySize) * KEncryptionKeySize;
    for (i = 0; i < lastBlockStart; i += KEncryptionKeySize)
        {
        d.Set(aData.MidTPtr (i, KEncryptionKeySize));
        cbc->Transform(d);
        }

    if (aAddPadding)
        {
        padding = KEncryptionKeySize - (dataLength - lastBlockStart);
        aData.SetLength(lastBlockStart + KEncryptionKeySize);
        for (i = dataLength; i < lastBlockStart + KEncryptionKeySize; i++)
            {
            aData[i] = padding;
            }
        d.Set(aData.MidTPtr (lastBlockStart, KEncryptionKeySize));
        cbc->Transform(d);
        }
    CleanupStack::PopAndDestroy(); // cbc
    }

// -----------------------------------------------------------------------------
// CDRMRightsDB::DecryptL
// Decrypt data and return it to the caller, using the CEK for this session
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::DecryptL( const TDesC8& aIv,
                             TPtr8& aData,
                             TBool aRemovePadding,
                             const TDesC8& aEncryptionKey )
    {
    CAESDecryptor* aes = NULL;
    CModeCBCDecryptor* cbc = NULL;
    TInt i;
    TInt n;
    TPtr8 d(NULL, 0);

    if (iKey->Length() > 0)
        {
        aes = CAESDecryptor::NewLC( aEncryptionKey );
        cbc = CModeCBCDecryptor::NewLC(aes, aIv);
        CleanupStack::Pop(); // aes, now owned by cbc

        for (i = 0; i < aData.Length(); i += KEncryptionKeySize )
            {
            d.Set(aData.MidTPtr (i, KEncryptionKeySize));
            cbc->Transform(d);
            }
        if (aRemovePadding)
            {
            n = aData.Length();
            aData.SetLength(n - aData[n - 1]);
            }
        CleanupStack::PopAndDestroy(); // cbc
        }
    }

// -----------------------------------------------------------------------------
// CDRMRightsDB::CheckPaddingL
// Check if the padding matches, aka if they key used for decryption was
// incorrect leave with KErrPermissionDenied
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::CheckPaddingL( const TDesC8& aData )
    {
    TUint8 character = 0;
    TInt limiter = 0;

    for( TInt i = 15; i >= limiter; i-- )
        {
        if( i == 15 )
            {
            character = aData[i];
            if( character < 1 || character > 16 )
                {
                User::Leave( KErrPermissionDenied );
                }
            limiter = 16 - character;
            }
        if( aData[i] != character )
            {
            User::Leave( KErrPermissionDenied );
            }
        }
    };

// -----------------------------------------------------------------------------
// CDRMRightsDB::ReadDataL
// Read Data from the file, if it doesn't have enough leave
// Error handling missing
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::ReadDataL( RFile& aStream,
                              TDes8& aEncIV,
                              TPtr8& aReadData,
                              HBufC8*& aDataBuffer,
                              TInt& aDataLeft,
                              TInt aSize,
                              TBool aStart,
                              const TDesC8& aEncryptionKey )
    {
    TPtr8 readData(NULL, 0, 0);
    HBufC8 *newBuffer = 0;
    TInt bufferSize = 0;
    TInt pos = 0;
    TUint8* currPtr = const_cast<TUint8*>(aDataBuffer->Ptr()) + aDataBuffer->Des().MaxSize();
    currPtr -= aDataLeft;
    currPtr -= KEncryptionKeySize;
    TInt checknum = 0;

    // In the beginning read IV from the stream                             1
    //------------------------------------------------------------------------
    if( aStart )
        {
        readData.Set( const_cast<TUint8*>(aEncIV.Ptr()), 0, KEncryptionKeySize );
        User::LeaveIfError(aStream.Read( readData, KEncryptionKeySize ));
        }
    else
        {
        bufferSize = aDataBuffer->Des().MaxLength()-KEncryptionKeySize;
        // Copy the old IV to the new IV
        Mem::Copy( const_cast<TUint8*>( aEncIV.Ptr() ),
                   const_cast<TUint8*>( aDataBuffer->Ptr() ) +
                   bufferSize,
                   KEncryptionKeySize );
        }


    // If the block is too small realloc:                                   2
    //------------------------------------------------------------------------
    bufferSize = aDataBuffer->Des().MaxLength()-KEncryptionKeySize;

    if( aSize > bufferSize)
        {
        bufferSize = aSize / KEncryptionKeySize;
        bufferSize *= KEncryptionKeySize;
        bufferSize += 2 * KEncryptionKeySize;
        newBuffer = HBufC8::NewLC( bufferSize );
        }

    // calculate a proper amount of data to copy so that
    // we stay in increments of 16                                          3
    //------------------------------------------------------------------------
    pos = aDataLeft;

    if( aDataLeft % KEncryptionKeySize )
        {
        pos = pos - ( pos % KEncryptionKeySize ) + KEncryptionKeySize;
        }


    // Copy the existing data into the buffer                               4
    //------------------------------------------------------------------------
    if( !newBuffer )
        {
        Mem::Copy( const_cast<TUint8*>( aDataBuffer->Ptr() ),
                   const_cast<TUint8*>( aDataBuffer->Ptr() ) +
                   ( aDataBuffer->Des().MaxLength()-KEncryptionKeySize - pos ),
                   pos );
        }
    else
        {
        Mem::Copy( const_cast<TUint8*>( newBuffer->Ptr() ),
                   const_cast<TUint8*>( aDataBuffer->Ptr() ) +
                   ( aDataBuffer->Des().MaxLength()-KEncryptionKeySize - pos ),
                   pos );
        delete aDataBuffer;
        aDataBuffer = newBuffer;
        }

    // Read the new data from the file                                      5
    //------------------------------------------------------------------------
    readData.Set( const_cast<TUint8*>( aDataBuffer->Ptr() ) + pos, 0,
              aDataBuffer->Des().MaxLength()-KEncryptionKeySize - pos  );

/*    readData.Set( aDataBuffer->Des().
             MidTPtr( pos,
                      aDataBuffer->Des().MaxLength() -
                        KEncryptionKeySize -
                        pos ) );
*/
    checknum = aStream.Read( readData );
    User::LeaveIfError( checknum );

    checknum = readData.Length();
    checknum = pos + readData.Length();
    checknum = aDataBuffer->Des().MaxLength();
    checknum = aDataBuffer->Des().MaxLength()-KEncryptionKeySize;

    // Check if we are at the final part                                    6
    //------------------------------------------------------------------------
    if( pos + readData.Length() !=
        aDataBuffer->Des().MaxLength()-KEncryptionKeySize )
        {
        DecryptL( aEncIV, readData, ETrue, aEncryptionKey );
        Mem::FillZ( const_cast<TUint8*>( aEncIV.Ptr() ), KEncryptionKeySize );
        }
    else
        {
        Mem::Copy( const_cast<TUint8*>( aDataBuffer->Ptr() ) +
                   aDataBuffer->Des().MaxLength()-KEncryptionKeySize,
                   const_cast<TUint8*>( aDataBuffer->Ptr() ) +
                   aDataBuffer->Des().MaxLength()-(KEncryptionKeySize*2),
                   KEncryptionKeySize );


        DecryptL( aEncIV, readData, EFalse, aEncryptionKey );

        }

    // Set the buffer you read from to this:
    aReadData.Set( const_cast<TUint8*>(aDataBuffer->Ptr()) + pos-aDataLeft,
                   readData.Length()+aDataLeft,
                   readData.Length()+aDataLeft);

    // Set the data length
    aDataLeft = aReadData.Length();
    };

// -----------------------------------------------------------------------------
// CDRMRightsDB::SetAuthenticationSeedL
// Set the seed in the common data
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::SetAuthenticationSeedL( const TDesC8& aContentId,
                                           const TDesC8& aSeed )
    {
    TFileName path;
    TInt error = KErrNone;
    CDRMRightsData* rights = NULL;
    CDRMCommonData* data = NULL;

    GetRightsFileNameL( aContentId, path);

    TRAP( error, rights = CDRMRightsData::OpenL( path, iFileServer ) );
    if( rights )
        {
        CleanupStack::PushL( rights );
        }
    else
        {
        User::Leave( error );
        }

    data = const_cast<CDRMCommonData*>(rights->GetCommonDataL());

    data->SetAuthenticationSeedL( aSeed );

    rights->UpdateCommonDataL( data );

    CleanupStack::PopAndDestroy(); // rights
    }

// -----------------------------------------------------------------------------
// CDRMRightsDB::GetAuthenticationSeedL
// Get the seed from the common data
// -----------------------------------------------------------------------------
//
HBufC8* CDRMRightsDB::GetAuthenticationSeedL( const TDesC8& aContentId )
    {
    TFileName path;
    CDRMRightsData* rights( NULL );
    HBufC8* seed( NULL );

    GetRightsFileNameL( aContentId, path );

    rights = CDRMRightsData::OpenL( path, iFileServer );
    CleanupStack::PushL( rights );

    seed = const_cast< CDRMCommonData* >( rights->GetCommonDataL() )
            ->AuthenticationSeed().AllocL();

    CleanupStack::PopAndDestroy(); // rights

    return seed;
    }

// -----------------------------------------------------------------------------
// CDRMRightsDB::Updating
// Return the iLastUpdate flag. Updating is considered as something that happens
// in a specific time window, to acknowledge the fact that e.g. events which are
// tiggered in the updating process come a bit later through the active
// scheduled.
// -----------------------------------------------------------------------------
//
TBool CDRMRightsDB::Updating()
    {
    TTime now;
    TBool r = EFalse;
    TTimeIntervalMicroSeconds interval;

    now.HomeTime();
    interval = now.MicroSecondsFrom( iLastUpdate );

#ifdef _LOGGING
    TBuf<256> logBuffer;
    logBuffer.AppendNum( interval.Int64() );
    DRMLOG( _L(" CDRMRightsDB::Updating: Update interval: " ) );
    DRMLOG( logBuffer );
#endif

    if ( interval < KMaxUpdateTime )
        {
        r = ETrue;
        }
    return r;
    }

// -----------------------------------------------------------------------------
// CDRMRightsDB::MarkAsCorrupted
// Mark the DB as corrupted by creating a specific file in the RDB structure. If
// that file is detected, the RDB gets recreated.
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::MarkAsCorrupted()
    {
    TFileName name;
    RFile file;

    name.Copy( *iDbPath );
    name.Append( KCorruptionFlagFile );
    file.Create( iFileServer, name, EFileWrite );
    file.Close();
    }

// -----------------------------------------------------------------------------
// CDRMRightsDB::CheckCleanup
// Delete the db file if it's possible
// -----------------------------------------------------------------------------
//
void CDRMRightsDB::CheckCleanup( const TDesC& aFileName )
    {
    TInt canDelete = 0;
    TInt error = KErrNone;

    TRAP(error, canDelete = DeleteExpiredL( aFileName, Time::NullTTime()));

    if( !error && canDelete )
        {
        DRMLOG(_L("File empty, deletion allowed, deleting it:"));
        DRMLOG( aFileName );
        iFileServer.Delete( aFileName );
        }
    }


//  End of File
