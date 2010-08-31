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
* Description:  Client side class implementation
*
*/


// INCLUDE FILES
#include <s32file.h>
#include <etelmm.h>
#include "DRMRightsClient.h"
#include "DRMEngineClientServer.h"
#include "DrmPermission.h"
#include "DrmAsset.h"
#include "drmlog.h"

#ifdef __DRM_FULL
#include "rdrmhelper.h"
#endif

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS

// LOCAL CONSTANTS AND MACROS
_LIT( KServerFileName, "rightsserver.exe" );


_LIT( KRightsServerStarterSemaphore, "RightsServerStarterSemaphore" );


// Maximum number of message slots that we use
LOCAL_C const TInt KMaxMessageSlots = 4;

// Try start the server only once. If it doesn't work, it doesn't work.
LOCAL_C const TUint8 KStartupCount = 1;

// MODULE DATA STRUCTURES
struct TDRMFileDeletion
    {
    TDRMFileDeletion() : iName( KNullDesC ) {};
    ~TDRMFileDeletion()
        {
        if ( iName.Length() )
            {
            iFs.Delete( iName );
            }

        iFs.Close();
        }

    RFs iFs;
    TFileName iName;
    };

template< class T > struct RDRMArrayReset
    {
    RDRMArrayReset( T& aItem ) : iItem( aItem ),
                                 iCleanup( ResetAndDestroy, ( TAny* )this ) {};
    ~RDRMArrayReset() { };
    void PushL()
        {
        CleanupStack::PushL( iCleanup );
        };

    static void ResetAndDestroy( TAny* aSelf )
        {
        ( ( RDRMArrayReset< T >* )( aSelf ) )->iItem.ResetAndDestroy();
        ( ( RDRMArrayReset< T >* )( aSelf ) )->iItem.Close();
        };

    private:
        RDRMArrayReset(); // prohibit

    private:
        T& iItem;
        TCleanupItem iCleanup;

    };

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RDRMRightsClient::RDRMRightsClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RDRMRightsClient::RDRMRightsClient() :
    iPtr( NULL )
    {
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::~RDRMRightsClient
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C RDRMRightsClient::~RDRMRightsClient()
    {
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::Connect
// Opens connection to the server.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::Connect()
    {
    TInt ret = KErrNone;
    TUint8 count = 0;

    const TVersion requiredVersion(
        DRMEngine::KServerMajorVersion,
        DRMEngine::KServerMinorVersion,
        DRMEngine::KServerBuildVersion );

    FOREVER
        {
        DRMLOG( _L( "RDRMRightsClient::Connect(): Create a new session" ) );
        ret = CreateSession( DRMEngine::KServerName,
                             requiredVersion,
                             KMaxMessageSlots );

        if ( ret == KErrNotFound && count < KStartupCount )
            {
            ret = StartServer();
            if ( ret )
                {
                break;
                }

            ++count;
            }
        else
            {
            break;
            }
        }

#ifdef __DRM_FULL
    // startup code, if it starts it starts if not it will be tried again.
    RDRMHelper helper;
    TInt ignore = helper.Connect(); // Start HelperServer
    helper.Close();
#endif

    DRMLOG2( _L( "RDRMRightsClient::Connect(): Result: %d" ), ret );

    return ret;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::Close
// Closes the connection to the server.
// -----------------------------------------------------------------------------
//
EXPORT_C void RDRMRightsClient::Close()
    {
    DRMLOG( _L( "RDRMRightsClient::Close()" ) );
    RHandleBase::Close();
    }

EXPORT_C TInt RDRMRightsClient::StartServer()
    {
    DRMLOG( _L( "RDRMRightsClient::StartServer()" ) );

    RSemaphore semaphore;
    RSemaphore semaphore2;
    TFindServer server( DRMEngine::KServerName );
    TFullName name;
    RProcess process;
    TInt error = KErrNone;

    // "local" semaphore
     error = semaphore2.CreateGlobal( KRightsServerStarterSemaphore,   // name
                                    1 ,              // count
                                    EOwnerThread );  // owner

    if ( error == KErrAlreadyExists )
        {
        error = semaphore2.OpenGlobal( KRightsServerStarterSemaphore );
        }


    // Semaphore not created or opened, don't need to close
    if( error )
        {
        return error;
        }

    // Server updated semaphore
    error = semaphore.CreateGlobal( DRMEngine::KDRMSemaphore,   // name
                                    0 ,              // count
                                    EOwnerThread );  // owner

    if ( error == KErrAlreadyExists )
        {
        error = semaphore.OpenGlobal( DRMEngine::KDRMSemaphore );
        }

    // Semaphore not created or opened, don't need to close
    if( error )
        {
        semaphore2.Close();
        return error;
        }

    // Wait until server has done all its things.
    semaphore2.Wait();

    // Check if the server is already running.
    error = server.Next( name );

    if ( !error )
        {
        // Yep, it's already running.
        error = KErrNone;
        }
    else
        {
        error = process.Create( KServerFileName,
                                KNullDesC );

        if ( !error )
            {
            User::After( 1000 );

            process.Resume();
            process.Close();

            // Wait here for the server process startup to complete
            // server will signal the global semaphore
            semaphore.Wait();
            }
        }

    // Close both semaphores and signal the "local" one.
    semaphore.Close();
    semaphore2.Signal();
    semaphore2.Close();

    DRMLOG2( _L( "RDRMRightsClient::StartServer(): %d" ), error );
    return error;

    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::AddRecord
// Add a new entry to the rights database.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::AddRecord( const TDesC8& aCEK, // Content encryption key
                                  // The rights object which is to be added
                                  const CDRMPermission& aRightsObject,
                                  const TDesC8& aCID, // Content-ID
                                  TDRMUniqueID& aID ) // Unique ID, out-parameter
    {
    DRMLOG( _L( "RDRMRightsClient::AddRecord" ) );
    TInt error = KErrArgument;

    // Check the parameters.
    if ( aCEK.Length() )
        {

        HBufC8* rightsData = NULL;
        TRAP( error, rightsData = aRightsObject.ExportL() );
        TInt size = aRightsObject.Size();

        if ( rightsData && size > 0 )
            {
            // For C/S communications.
            TPtr8 rightsObject( NULL, 0 );
            TPtr8 uid( reinterpret_cast< TUint8* >( &aID ),
                       0,
                       sizeof( TDRMUniqueID ) );

            rightsObject.Set( const_cast< TUint8* >( rightsData->Ptr() ),
                              size,
                              size );


            // Send the message.
            error = SendReceive( DRMEngine::EAddRecord,
                             TIpcArgs( &aCID, &rightsObject, &aCEK, &uid ) );

            delete rightsData;
            rightsData = NULL;
            }
        }

    DRMLOG2( _L( "RDRMRightsClient::AddRecord: %d" ), error );
    return error;
    }


// -----------------------------------------------------------------------------
// RDRMRightsClient::AddProtectedRecord
// Add a protected entry to the rights database.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::AddProtectedRecord(
    const TDesC8& aProtectedCek,
    TBool aDomainRecord,
    const CDRMPermission& aRightsObject,
    const TDesC8& aCID,
    TDRMUniqueID& aID ) // Unique ID, out-parameter
    {
    DRMLOG( _L( "RDRMRightsClient::AddProtectedRecord" ) );

    TInt error = KErrNone;
    TInt message = DRMEngine::EAddProtectedRecord;
    HBufC8* rightsData = NULL;
    TInt size = aRightsObject.Size();
    TPtr8 rightsObject( NULL, 0 );
    TPtr8 uid( reinterpret_cast< TUint8* >( &aID ), 0,
        sizeof( TDRMUniqueID ) );
    TPtr8 key( NULL, 0 );

    TRAP( error, rightsData = aRightsObject.ExportL() );
    if ( !error )
        {
        rightsObject.Set( const_cast< TUint8* >( rightsData->Ptr() ), size,
            size );

        if ( aDomainRecord )
            {
            message = DRMEngine::EAddDomainRecord;
            }
        error = SendReceive( message, TIpcArgs( &aCID, &rightsObject, &aProtectedCek, &uid ) );

        delete rightsData;
        rightsData = NULL;
        }
    DRMLOG2( _L( "RDRMRightsClient::AddProtectedRecord: %d" ), error );
    return error;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::GetDBEntriesL
// Get a file name from the server. The file contains the rights objects,
// which are then converted to RPointerArray.
// -----------------------------------------------------------------------------
//
EXPORT_C void RDRMRightsClient::GetDBEntriesL( const TDesC8& aId,
                                      RPointerArray< CDRMPermission >& aRightsList )
    {
    DRMLOG( _L( "RDRMRightsClient::GetDBEntries" ) );
    if( aId.Length() )
        {
        // Temporary file name from the server.
        TDRMFileDeletion item;

        // Make sure that the array is empty.
        aRightsList.ResetAndDestroy();

        // For file operations. Destructor of TDRMFileDeletion
        // deletes the file & closes the session.
        User::LeaveIfError( item.iFs.Connect() );

        User::LeaveIfError( SendReceive( DRMEngine::EGetEntryList,
                                         TIpcArgs( &item.iName, &aId ) ) );

        // Convert the file to a list.
        FileToListL( item.iFs, item.iName, aRightsList );

        DRMLOG( _L( "RDRMRightsClient::GetDBEntries ok" ) );

        return;
        }

    // Invalid parameter.
    User::Leave( KErrArgument );
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::GetDbEntryL
// Get a single RO from the server.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMPermission* RDRMRightsClient::GetDbEntryL( const TDesC8& aContentID,
                                                 const TDRMUniqueID& aUniqueID )
    {
    DRMLOG( _L( "RDRMRightsClient::GetDbEntryL" ) );

    CDRMPermission* permission = NULL;
    TInt size = 0;
    TPckg<TInt> package( size );
    TPtrC8 uid( reinterpret_cast< TUint8* >( const_cast< TDRMUniqueID* >(
        &aUniqueID ) ), sizeof( TDRMUniqueID ) );

    User::LeaveIfError( SendReceive( DRMEngine::EGetDbEntry,
        TIpcArgs( &package, NULL, &uid, &aContentID ) ) );
    HBufC8* rightsData = HBufC8::NewMaxLC( size );
    TPtr8 objectPkg( const_cast< TUint8* >( rightsData->Ptr() ), size,
        size );
    User::LeaveIfError( SendReceive( DRMEngine::EGetPreparedData,
        TIpcArgs( &objectPkg) ) );

    permission = CDRMPermission::NewLC();
    permission->ImportL( rightsData->Des() );
    CleanupStack::Pop(); // permission
    CleanupStack::PopAndDestroy(); // Rights data

    DRMLOG( _L( "RDRMRightsClient::GetDbEntryL ok" ) );
    return permission;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::DeleteDbEntry
// Deletes all rights objects associated with the given UID.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::DeleteDbEntry( const TDesC8& aContentID )
    {
    DRMLOG( _L( "RDRMRightsClient::DeleteDbEntry with CID" ) );

    TInt error = KErrArgument;

    // Check the parameter.
    if ( aContentID.Length() )
        {
        return SendReceive( DRMEngine::EDeleteWithCID,
                            TIpcArgs( NULL, NULL, &aContentID ) );
        }

    return error;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::DeleteDbEntry
// Delete a single rights object identified by given parameters.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::DeleteDbEntry( const TDesC8& aContentID,
                                               const TDRMUniqueID& aUniqueID )
    {
    DRMLOG( _L( "RDRMRightsClient::DeleteDbEntry with CID & UID" ) );

    if ( aContentID.Length() )
        {
        // Something to do.
        // Put aUniqueID inside a descriptor.
        // Works even if its typedef is changed.
        TPtrC8 uid( reinterpret_cast< TUint8* >(
                        const_cast< TDRMUniqueID* >( &aUniqueID ) ),
                    sizeof( TDRMUniqueID ) );

        return SendReceive( DRMEngine::EDeleteRO,
                            TIpcArgs( &uid, NULL, NULL, &aContentID ) );
        }

    return KErrArgument;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::ExportContentIDList
// Overloaded method: requests all content IDs to be put to a file.
// Assumes that the given descriptor represents a buffer large enough to
// contain the file name.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::ExportContentIDList( TDes& aFileName )
    {
    DRMLOG( _L( "RDRMRightsClient::ExportContentIDLis" ) );

    return SendReceive( DRMEngine::EExportCIDs,
                        TIpcArgs( &aFileName ) );

    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::ExportContentIDList
// Overloaded method: requests all content IDs to be put to a file,
// and then converts the file into RPointerArray.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::ExportContentIDList( RPointerArray< HBufC8 >& aCIDList )
    {
    DRMLOG( _L( "RDRMRightsClient::ExportContentIDList to array" ) );
    TFileName name;
    aCIDList.ResetAndDestroy();

    TInt error = ExportContentIDList( name );
    if ( !error )
        {
        RFs fs;
        error = fs.Connect();
        if ( !error )
            {
            TRAP( error, URIFileToArrayL( fs, name, aCIDList ) );
            if ( error )
                {
                aCIDList.ResetAndDestroy();
                }

            fs.Delete( name );
            fs.Close();
            }
        }

    DRMLOG2( _L( "RDRMRightsClient::ExportContentIDList: %d" ), error );

    return error;
    }


// -----------------------------------------------------------------------------
// RDRMRightsClient::GetDecryptionKey
// Fetches the decryption key from the server.
// Uses TR mechanisms. Uses asynchronous C/S call.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::GetDecryptionKey( const TInt aIntent,
                                                  const TDesC8& aContentID,
                                                  TDes8& aKey )
    {
    DRMLOG( _L( "RDRMRightsClient::GetDecryptionKey" ) );
    // Make this internally asynchronous.
    TRequestStatus status;

    SendReceive( DRMEngine::EGetKey,
        TIpcArgs( aIntent, const_cast<TDesC8*>(&aContentID), &aKey ),
        status );

    User::WaitForRequest( status );

    DRMLOG2( _L( "RDRMRightsClient::GetDecryptionKey: %d" ), status.Int() );

    return status.Int();
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::CheckRights
// Checks if appropriate rights exist for a certain content ID.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::CheckRights(
    const TInt aIntent,
    const TDesC8& aContentID,
    TUint32& aRejection )
    {
    TInt error = KErrNone;
    TInt size = 0;

    TPckg<TInt> package( size );
    TPckg<TUint32> package2( aRejection );

    DRMLOG( _L( "RDRMRightsClient::CheckRights" ) );
    error = SendReceive( DRMEngine::ECheckRights, TIpcArgs( aIntent,
        &aContentID, &package, &package2 ) );

    DRMLOG2( _L( "RDRMRightsClient::CheckRights: %d" ), error );

    return error;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::GetActiveRightsL
// Checks if appropriate rights exist for a certain content ID and returns them.
// Returns NULL if no rights are available
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMPermission* RDRMRightsClient::GetActiveRightsL(
    const TInt aIntent,
    const TDesC8& aContentID,
    TUint32& aRejection )
    {
    TInt r;
    TInt size = 0;
    TPckg<TInt> package( size );
    TPckg<TUint32> package2( aRejection );
    CDRMPermission* permission = NULL;
    HBufC8* buffer = NULL;

    DRMLOG( _L( "RDRMRightsClient::GetActiveRightsL" ) );
    r = SendReceive( DRMEngine::ECheckRights, TIpcArgs( aIntent, &aContentID,
        &package, &package2 ) );

    // Get rights. Return NULL if no rights are available, otherwise leave
    // for all other errors.
    if ( r == KErrNone )
        {
        buffer = HBufC8::NewMaxLC( size );
        TPtr8 objectPkg( const_cast< TUint8* >( buffer->Ptr() ), size, size );
        User::LeaveIfError( SendReceive( DRMEngine::EGetPreparedData,
            TIpcArgs( &objectPkg) ) );
        permission = CDRMPermission::NewLC();
        permission->ImportL( buffer->Des() );
        CleanupStack::Pop(); // permission
        CleanupStack::PopAndDestroy(); // buffer
        }
    else if ( r != KErrCANoRights && r != KErrCANoPermission &&
        r != KErrCAPendingRights )
        {
        User::Leave( r );
        }

    DRMLOG( _L( "RDRMRightsClient::GetActiveRightsL: done" ) );
    return permission;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::Count
// Returns the amount of unique content IDs in the database.
// If an error occurs, a negative value is returned (Symbian OS / DRM 3 specific
// error code).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::Count()
    {
    DRMLOG( _L( "RDRMRightsClient::Count" ) );

    TInt count = 0;
    TInt error = KErrNone;

    TPtr8 ptr( reinterpret_cast< TUint8* >( &count ),
               0,
               sizeof( TInt ) );

    error = SendReceive( DRMEngine::ECount,
                         TIpcArgs( &ptr ) );

    if ( !error )
        {
        error = count;
        }

    DRMLOG2( _L( "RDRMRightsClient::Count: %d" ), error );

    return error;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::DeleteAll
// Empties the database.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::DeleteAll()
    {
    DRMLOG( _L( "RDRMRightsClient::DeleteAll" ) );

    TInt error = SendReceive( DRMEngine::EDeleteAll, TIpcArgs( NULL ) );

    DRMLOG2( _L( "RDRMRightsClient::DeleteAll: %d" ), error  );

    return error;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::Consume()
// Consume the right with specific intent and contentID
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::Consume( const TInt aIntent,
                                         const TDesC8& aContentID )
    {
    TInt error = KErrNone;
    DRMLOG( _L( "RDRMRightsClient::Consume" ) );

    error = SendReceive( DRMEngine::EConsume,
                         TIpcArgs( aIntent, &aContentID ) );

    DRMLOG2( _L( "RDRMRightsClient::Consume: %d" ), error  );
    return error;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::CheckConsume()
// Check if Consume is possibke
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::CheckConsume( const TInt aIntent,
                                              const TDesC8& aContentID )
    {
    TInt error = KErrNone;
    DRMLOG( _L( "RDRMRightsClient::CheckConsume" ) );

    error = SendReceive( DRMEngine::ECheckConsume,
                         TIpcArgs( aIntent, &aContentID ) );

    DRMLOG2( _L( "RDRMRightsClient::CheckConsume: %d" ), error  );
    return error;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::CalculatePadding
// Calculate the padding from a data block and a certain content ID.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::CalculatePadding(
    const TDesC8& aLastTwoDataBlocks)
    {
    DRMLOG( _L( "RDRMRightsClient::CalculatePadding" ) );

    return SendReceive( DRMEngine::ECalculatePadding,
                        TIpcArgs ( &aLastTwoDataBlocks) );
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::ForwardLockURI( HBufC8*& aURI )
    {
    DRMLOG( _L( "RDRMRightsClient::ForwardLockURI" ) );
    TBuf8< DRMEngine::KMaxOmaV1CIDLength > buf;
    TInt error = SendReceive( DRMEngine::EGetFLUri,
                              TIpcArgs( &buf ) );

    if ( error )
        {
        aURI = NULL;
        return error;
        }

    aURI = buf.Alloc();

    if ( aURI )
        {
        return KErrNone;
        }

    return KErrNoMemory;
    }


// -----------------------------------------------------------------------------
// RDRMRightsClient::AddDomainRO
// Add domain rights object xml representation to the rdb
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::AddDomainRO( const TDesC8& aRoId,
                                             const TDesC8& aXmlData )
    {
    DRMLOG( _L( "RDRMRightsClient::AddDomainRO" ) );
    TInt error = KErrArgument;

    // Check the parameters.
    if ( aRoId.Length() && aXmlData.Length() )
        {
        // Send the message.
        error = SendReceive( DRMEngine::EAddDomainRO,
                             TIpcArgs( &aRoId, &aXmlData) );
        }

    DRMLOG2( _L( "RDRMRightsClient::AddDomainRO: %d" ), error );
    return error;
    };

// -----------------------------------------------------------------------------
// RDRMRightsClient::GetDomainROL
// Get domain rights object xml representation from the rdb
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* RDRMRightsClient::GetDomainROL( const TDesC8& aRoId )
    {
    DRMLOG( _L( "RDRMRightsClient::GetDomainROL" ) );

    // Check the parameter.
    if ( aRoId.Length() )
        {
        TInt size = 0;
        TPckg<TInt> package( size );

        // Call the server. Throw an exception in case of an error.
        User::LeaveIfError(
            SendReceive( DRMEngine::EGetDomainRO,
                         TIpcArgs( &package, &aRoId ) ) );

        HBufC8* roData = HBufC8::NewMaxLC( size );

        // Package 'object' into TPtr8.
        TPtr8 objectPkg( const_cast< TUint8* >( roData->Ptr() ),
                         size,
                         size );

        User::LeaveIfError(
            SendReceive( DRMEngine::EGetPreparedData,
                         TIpcArgs( &objectPkg) ) );
        CleanupStack::Pop( roData ); // roData
        return roData;
        }

    User::Leave( KErrArgument );

    // Never reached.
    return NULL;
    };

// -----------------------------------------------------------------------------
// RDRMRightsClient::GetDomainROL
// Get domain rights object xml representation from the rdb
// -----------------------------------------------------------------------------
//
EXPORT_C void RDRMRightsClient::GetDomainRosForCidL(
    const TDesC8& aContentId,
    RPointerArray<HBufC8>& aRoList)
    {
    TInt size = 0;
    TPckg<TInt> pkg(size);
    HBufC8* buffer = NULL;
    HBufC8* ro = NULL;
    TInt roSize;
    TInt offset;


    DRMLOG(_L("RDRMRightsClient::GetDomainRoForCidL"));
    User::LeaveIfError(SendReceive(DRMEngine::EGetDomainRoForCid,
        TIpcArgs(&aContentId, &pkg)));
    if (size > 0)
        {
        buffer = HBufC8::NewMaxLC(size);
        TPtr8 ptr( const_cast< TUint8* >( buffer->Ptr() ),
                         size,
                         size );
        User::LeaveIfError(SendReceive(DRMEngine::EGetPreparedData,
            TIpcArgs(&ptr)));
        offset = 0;
        while (offset < size)
            {
            Mem::Copy( &roSize, ptr.Ptr()+offset, sizeof(TInt) );
            offset += sizeof (TInt);
            ro = ptr.Mid(offset, roSize).AllocL();
            CleanupStack::PushL( ro );
            aRoList.AppendL (ro);
            CleanupStack::Pop( ro );
            offset += roSize;
            }
        CleanupStack::PopAndDestroy();
        }
    };

// -----------------------------------------------------------------------------
// RDRMRightsClient::
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::Encrypt( const TDesC8& aIv,
                       TPtr8& aData,
                       TBool aAddPadding )
    {
    return SendReceive( DRMEngine::EEncrypt,
                        TIpcArgs( &aIv, &aData, aAddPadding ) );
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::Decrypt( const TDesC8& aIv,
                       TPtr8& aData,
                       TBool aRemovePadding )
    {
    return SendReceive( DRMEngine::EDecrypt,
                        TIpcArgs( &aIv, &aData, aRemovePadding ) );
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::InitializeKey( const TDesC8& aContentId )
    {
    return SendReceive( DRMEngine::EInitializeKey,
                        TIpcArgs( &aContentId ) );
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::InitializeGroupKey( const TDesC8& aGroupId,
                                                    const TDesC8& aGroupKey,
                                                    TEncryptionMethod aMethod )
    {
    return SendReceive( DRMEngine::EInitializeGroupKey,
                        TIpcArgs( &aGroupId, &aGroupKey, aMethod ) );
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::DeleteDomainRO
// Delete domain rights object xml representation from the rdb
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::DeleteDomainRO( const TDesC8& aRoId )
    {
    DRMLOG( _L( "RDRMRightsClient::DeleteDomainRO" ) );

    // Check the parameter.
    if ( aRoId.Length() )
        {
        return SendReceive( DRMEngine::EDeleteDomainRO,
                            TIpcArgs( &aRoId ) );
        }

    return KErrArgument;
    };


// -----------------------------------------------------------------------------
// RDRMRightsClient::
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::IsInCache( const TDesC8& aID,
                                           const TTime& aTime,
                                           TBool& aInCache )
    {
    DRMLOG( _L( "RDRMRightsClient::IsInCache" ) );
    TPckgC< TTime > timePckg( aTime );
    TPckg< TBool > inCache( aInCache );

    return SendReceive( DRMEngine::EIsInCache,
                        TIpcArgs( &aID, &timePckg, &inCache ) );
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::IsInCache( const TDesC8& aID,
                                           TBool& aInCache )
    {
    DRMLOG( _L( "RDRMRightsClient::IsInCache" ) );

    TPckg< TBool > inCache( aInCache );

    return SendReceive( DRMEngine::EIsInCache,
                        TIpcArgs( &aID, NULL, &inCache ) );
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::AddToCache( const TDesC8& aID,
                                            const TTime& aTime )
    {
    DRMLOG( _L( "RDRMRightsClient::AddToCache" ) );

    TPckgC< TTime > timePckg( aTime );

    return SendReceive( DRMEngine::EAddToCache,
                        TIpcArgs( &aID, &timePckg ) );
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::AddToCache( const TDesC8& aID )
    {
    DRMLOG( _L( "RDRMRightsClient::AddToCache" ) );

    return SendReceive( DRMEngine::EAddToCache,
                        TIpcArgs( &aID, NULL ) );
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::DeleteExpiredPermissions
//
// -----------------------------------------------------------------------------
//
EXPORT_C void RDRMRightsClient::DeleteExpiredPermissions( TRequestStatus& aStatus )
    {
    DRMLOG( _L( "RDRMRightsClient::DeleteExpiredPermissions" ) );

    SendReceive( DRMEngine::EDeleteExpired, aStatus );
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::SetEstimatedArrival
// sets the estimated RO arrival time to the given delta
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::SetEstimatedArrival( const TDesC8& aContentID,
                                            TTimeIntervalSeconds aDeltaSeconds )
    {
    DRMLOG( _L( "RDRMRightsClient::SetEstimatedArrival" ) );
    TInt error = KErrNone;

    TPckg< TTimeIntervalSeconds > delta( aDeltaSeconds );

    error = SendReceive( DRMEngine::ESetEstimatedArrival,
                         TIpcArgs( &aContentID, &delta ) );

    return error;
    };

// -----------------------------------------------------------------------------
// RDRMRightsClient::GetEstimatedArrival
// sets the amount of time in which the RO should arrive in
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::GetEstimatedArrival( const TDesC8& aContentID,
                                            TTimeIntervalSeconds& aDeltaSeconds )
    {
    DRMLOG( _L( "RDRMRightsClient::SetEstimatedArrival" ) );
    TInt error = KErrNone;

    TPckg< TTimeIntervalSeconds > delta( aDeltaSeconds );

    error = SendReceive( DRMEngine::EGetEstimatedArrival,
                         TIpcArgs( &aContentID, &delta ) );

    return error;
    };


// -----------------------------------------------------------------------------
// RDRMRightsClient::SetName
// sets the name.
// -----------------------------------------------------------------------------
//
EXPORT_C  TInt RDRMRightsClient::SetName( const TDesC8& aContentID,
                                          const TDesC& aName )
    {
    DRMLOG( _L( "RDRMRightsClient::SetName" ) );
    return SendReceive( DRMEngine::ESetName,
                        TIpcArgs( &aContentID, &aName ) );
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::GetName
// Gets the name. Two-phase operation.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::GetName( const TDesC8& aContentID,
                                         HBufC*& aName )
    {
    DRMLOG( _L( "RDRMRightsClient::GetName" ) );
    TPckgBuf< TInt > size( 0 );

    TInt error = SendReceive( DRMEngine::EGetName,
                              TIpcArgs( &aContentID, &size ) );
    if ( !error )
        {
        if ( !size() )
            {
            aName = HBufC::New( 1 );
            if ( !aName )
                {
                error = KErrNoMemory;
                }
            }
        else
            {
            aName = HBufC::New( size() );
            if ( aName )
                {
                TPtr data( aName->Des() );

                error = SendReceive( DRMEngine::EGetWideData,
                                     TIpcArgs( &data ) );

                if ( error )
                    {
                    delete aName; aName = NULL;
                    }
                }
            else
                {
                error = KErrNoMemory;
                }
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::Cancel
// -----------------------------------------------------------------------------
//
EXPORT_C void RDRMRightsClient::Cancel()
    {
    DRMLOG( _L(" RDRMRightsClient::Cancel" ) );
    SendReceive( DRMEngine::ECancel );
    }


// -----------------------------------------------------------------------------
// RDRMRightsClient::GetUdtData
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::GetUdtData( TDes8& aUdtData )
    {
    DRMLOG( _L( "RDRMRightsClient::GetUdtData" ) );

    TInt error = SendReceive( DRMEngine::EGetUdtData,
                              TIpcArgs( &aUdtData ) );

    return error;
    };

// -----------------------------------------------------------------------------
// RDRMRightsClient::InitializeUdt
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::InitiateUdt( const TDesC8& aKey )
    {
    DRMLOG( _L( "RDRMRightsClient::InitiateUdt" ) );

    TInt error = SendReceive( DRMEngine::EInitiateUdt,
                              TIpcArgs( &aKey ) );
    return error;
    };

// -----------------------------------------------------------------------------
// RDRMRightsClient::InitOrphanedContentIdList
// -----------------------------------------------------------------------------
//
EXPORT_C void RDRMRightsClient::InitOrphanedContentIdList( TBool aPerformScan,
                                                      TRequestStatus& aStatus )
    {
    DRMLOG( _L( "RDRMRightsClient::ExportOrphanedContentIdList" ) );

    SendReceive( DRMEngine::EInitOrphanedList,
                 TIpcArgs( aPerformScan ), aStatus );

    DRMLOG( _L( "RDRMRightsClient::ExportOrphanedContentIdList done" ) );
    };


// -----------------------------------------------------------------------------
// RDRMRightsClient::ExportOrphanedContentIdList
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::ExportOrphanedContentIdList( TDes& aFileName )
    {
    DRMLOG( _L( "RDRMRightsClient::ExportOrphanedContentIdList" ) );

    return SendReceive( DRMEngine::EGetOrphanedList,
                        TIpcArgs( &aFileName ) );

    };

// -----------------------------------------------------------------------------
// RDRMRightsClient::ExportOrphanedContentIdList
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::ExportOrphanedContentIdList(
                        RPointerArray<HBufC8>& aContentIdList )
    {
    DRMLOG( _L( "RDRMRightsClient::ExportOrphanedContentIdList to array" ) );
    TFileName name;
    aContentIdList.ResetAndDestroy();

    TInt error = ExportOrphanedContentIdList( name );
    if ( !error )
        {
        RFs fs;
        error = fs.Connect();
        if ( !error )
            {
            TRAP( error, URIFileToArrayL( fs, name, aContentIdList ) );
            if ( error )
                {
                aContentIdList.ResetAndDestroy();
                }

            fs.Delete( name );
            fs.Close();
            }
        }

    DRMLOG2( _L( "RDRMRightsClient::ExportOrphanedContentIdList to array: %d" ), error );

    return error;
    };

// -----------------------------------------------------------------------------
// RDRMRightsClient::EncodeRightsIssuerField
// The encoded field is consturcted from
// - F/L prefix
// - phone serial number
// - base64encode(AES IV + AES encoded original field incl. padding)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::EncodeRightsIssuerField(
                                    const TDesC8& aOldValue,
                                    HBufC8*& aNewValue )
    {
    DRMLOG( _L( "RDRMRightsClient::EncodeRightsIssuerField" ) );

    TInt error( KErrNone );

    TInt size( aOldValue.Length() ); // original length
    size += KDCFKeySize - ( size % KDCFKeySize ); // padding
    size += KDCFKeySize; // IV
    size += ( size + 2 ) / 3 * 4; // base64
    size += RMobilePhone::KPhoneSerialNumberSize;
    size += 3; // "flk"

    aNewValue = HBufC8::New( size );
    if ( aNewValue )
        {
        TPtr8 des( aNewValue->Des() );

        des = aOldValue;

        error = SendReceive( DRMEngine::EEncodeRightsIssuerField,
                             TIpcArgs( &des ) );
        if ( error )
            {
            delete aNewValue;
            aNewValue = NULL;
            }
        }
    else
        {
        error = KErrNoMemory;
        }

    DRMLOG2( _L( "RDRMRightsClient::EncodeRightsIssuerField> %d" ), error );

    return error;
    }

EXPORT_C TInt RDRMRightsClient::DecodeRightsIssuerField(
                                    const TDesC8& aEncoded,
                                    HBufC8*& aDecoded )
    {
    DRMLOG( _L( "DRMRightsClient::DecodeRightsIssuerField" ) );

    TInt error( KErrNone );

    aDecoded = HBufC8::New( aEncoded.Length() );
    if ( aDecoded )
        {
        TPtr8 des( aDecoded->Des() );

        error = SendReceive( DRMEngine::EDecodeRightsIssuerField,
                             TIpcArgs( &aEncoded,
                                      &des ) );
        if ( error )
            {
            delete aDecoded;
            aDecoded = NULL;
            }
        else if ( !( aDecoded->Length() ) )
            {
            // The decoding process didn't change it.
            *aDecoded = aEncoded;
            }
        }
    else
        {
        error = KErrNoMemory;
        }

    DRMLOG2( _L( "DRMRightsClient::DecodeRightsIssuerField: error %d" ), error );

    return error;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::SetAuthenticationSeed
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::SetAuthenticationSeed( const TDesC8& aContentID,
                                                       const TDesC8& aSeed )
    {
    DRMLOG( _L( "RDRMRightsClient::SetAuthenticationSeed" ) );
    return SendReceive( DRMEngine::ESetAuthenticationSeed,
                        TIpcArgs( &aContentID, &aSeed ) );
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::GetAuthenticationSeed
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::GetAuthenticationSeed( const TDesC8& aContentID,
                                                       TDes8& aSeed )
    {
    DRMLOG( _L( "RDRMRightsClient::GetAuthenticationSeed" ) );
    return SendReceive( DRMEngine::EGetAuthenticationSeed,
                        TIpcArgs( &aContentID, &aSeed ) );
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::FileToListL
// Converts the given file into an array.
// -----------------------------------------------------------------------------
//
void RDRMRightsClient::FileToListL( RFs& aFs,
                                   const TDesC& aFileName,
                                   RPointerArray< CDRMPermission >& aList )
    {
    DRMLOG( _L( "RDRMRightsClient::FileToListL" ) );
    TInt error = KErrNone;

    // Temporary storage.
    CDRMPermission* tmpObject;

    // To access the file data.
    RFileReadStream fileStream;

    // How many objects there is in the file.
    TInt size = 0;

    // Temporary counter.
    TInt count = 0;


    // Open the file.
    User::LeaveIfError( fileStream.Open( aFs, aFileName, EFileRead | EFileStream ) );
    CleanupClosePushL( fileStream );

    size = fileStream.ReadInt32L();

    while( count < size )
        {
        // Allocate a new RO.
        tmpObject = CDRMPermission::NewL();

        // Read the object.
        TRAP( error, tmpObject->InternalizeL( fileStream ) );

        if ( !error )
            {
            // Add the object into the list.
            error = aList.Append( tmpObject );
            }

        if ( error )
            {
            delete tmpObject;
            User::Leave( error );
            }

        // Now tmpObject is under responsibility of aList.
        ++count;
        }

    // All done.
    CleanupStack::PopAndDestroy(); // fileStream
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::URIFileToArrayL
// Converts the given file into an array.
// -----------------------------------------------------------------------------
//
void RDRMRightsClient::URIFileToArrayL( RFs& aFs,
                                       const TDesC& aFile,
                                       RPointerArray< HBufC8 >& aList )
    {
    DRMLOG( _L( "RDRMRightsClient::URIFileToArrayL" ) );
    RFileReadStream stream;
    TUint16 size = 0;
    TPtr8 data( NULL, 0, 0 );

    User::LeaveIfError( stream.Open( aFs, aFile, EFileRead | EFileStream ) );
    CleanupClosePushL( stream );

    size = stream.ReadUint16L();
    while( size > 0 )
        {
        HBufC8* tmp = HBufC8::NewLC( size );
        data.Set( tmp->Des() );
        stream.ReadL( data, size );
        User::LeaveIfError( aList.Append( tmp ) );
        CleanupStack::Pop(); // tmp
        size = stream.ReadUint16L();
        }

    // All read, return.

    CleanupStack::PopAndDestroy(); // stream
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::VerifyMacL
// Verifies the MAC.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::VerifyMacL(const TDesC8& aSignedInfoElement,
                                  const TDesC8& aMacValue ) const
    {
    DRMLOG( _L( "RDRMRightsClient::VerifyMacL" ) );

    TInt error = SendReceive( DRMEngine::EVerifyMac,
                              TIpcArgs( &aSignedInfoElement, &aMacValue) );
    return error;
    }


// -----------------------------------------------------------------------------
// RDRMRightsClient::GetSupportedIndividualsL
// retrieves the supported individuals list
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::GetSupportedIndividualsL(
                                    RPointerArray<HBufC8>& aIndividuals) const
    {
    DRMLOG( _L( "RDRMRightsClient::GetSupportedIndividualsL" ) );
    TPckgBuf< TInt > size( 0 );
    TInt error = SendReceive( DRMEngine::EGetSupportedIndividuals,
                              TIpcArgs( &size) );
    HBufC8* dataBuf = NULL;
    HBufC8* individual = NULL;
    TInt indivSize;
    TInt offset;

    if ( !error )
        {
        if ( !size() )
            {
            return KErrNone;
            }
        else
            {
            dataBuf = HBufC8::NewLC( size() );
            if ( dataBuf )
                {
                TPtr8 data( dataBuf->Des() );

                error = SendReceive( DRMEngine::EGetPreparedData,
                                     TIpcArgs( &data ) );

                if ( !error )
                    {
                    offset = 0;
                    while (offset < size())
                        {
                        Mem::Copy( &indivSize, data.Ptr()+offset, sizeof(TInt) );
                        offset += sizeof (TInt);
                        individual = data.Mid(offset, indivSize).AllocLC();
                        aIndividuals.AppendL(individual);
                        CleanupStack::Pop(); // individual
                        offset += indivSize;
                        }
                    }
                }
            CleanupStack::PopAndDestroy();
            }
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::StopWatching
// -----------------------------------------------------------------------------
//
EXPORT_C void RDRMRightsClient::StopWatching() const
    {
    SendReceive( DRMEngine::EStopWatching );
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::UnwrapMacAndRek
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::UnwrapMacAndRek( const TDesC8& aMacAndRek,
                                                 TKeyTransportScheme aTransportScheme,
                                                 const TDesC8& aRightsIssuerId,
                                                 const TDesC8& aDomainId ) const
    {
    HBufC8* data = NULL;
    TPtr8 dataPtr( NULL, 0 );
    TInt err = KErrNone;

    data = HBufC8::New( 1 + aMacAndRek.Size() );

    if ( data )
        {
        dataPtr.Set( data->Des() );
        dataPtr.SetLength( 1 );
        dataPtr[0] = aTransportScheme;
        dataPtr.Append( aMacAndRek );

        if( aDomainId.Length() )
            {
            err = SendReceive( DRMEngine::EUnwrapDomainMacAndRek, TIpcArgs( &dataPtr,
                                                                  &aRightsIssuerId,
                                                                  &aDomainId ) );
            }
        else
            {
            err = SendReceive( DRMEngine::EUnwrapDeviceMacAndRek, TIpcArgs( &dataPtr,
                                                                  &aRightsIssuerId,
                                                                  NULL ) );
            }
        delete data;
        data = NULL;
        }
    else
        {
        err = KErrNoMemory;
        }

    return err;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::GetRandomDataL
// -----------------------------------------------------------------------------
//
EXPORT_C void RDRMRightsClient::GetRandomDataL( TDes8& aRandomData ) const
    {
    if( !aRandomData.Length() )
        {
        User::Leave(KErrArgument);
        }
    User::LeaveIfError( SendReceive( DRMEngine::EGetRandomData, TIpcArgs( &aRandomData ) ) );
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::GetMeteringData()
// -----------------------------------------------------------------------------
//
#ifndef RD_DRM_METERING
EXPORT_C HBufC8* RDRMRightsClient::GetMeteringDataL( const TDesC8& /*aRiId*/ )
    {
    return NULL;
    }
#else
EXPORT_C HBufC8* RDRMRightsClient::GetMeteringDataL( const TDesC8& aRiId )
    {
    TInt error = KErrNone;
    HBufC8* meteringData = NULL;

    if ( aRiId.Length() )
        {
        TInt size = 0;
        TPckg<TInt> package( size );

        error = SendReceive( DRMEngine::EGetMeteringData,
                           TIpcArgs( &package, &aRiId ) );

        if ( error == KErrNotFound )
            {
            return NULL;
            }

        User::LeaveIfError( error );

        meteringData = HBufC8::NewMaxLC( size );

        // Package 'object' into TPtr8.
        TPtr8 objectPkg( const_cast< TUint8* >( meteringData->Ptr() ),
                         size,
                         size );

        User::LeaveIfError( SendReceive( DRMEngine::EGetPreparedData,
                                         TIpcArgs( &objectPkg) ) );

        CleanupStack::Pop(); // meteringData
        return meteringData;
        }

    User::Leave( KErrArgument );
    return NULL;
    }
#endif //RD_DRM_METERING

 // -----------------------------------------------------------------------------
// RDRMRightsClient::DeleteMeteringDataL
// -----------------------------------------------------------------------------
//
#ifndef RD_DRM_METERING
EXPORT_C TInt RDRMRightsClient::DeleteMeteringDataL( const TDesC8& /*aRiId*/ )
    {
    return KErrNotSupported;
    }
#else
EXPORT_C TInt RDRMRightsClient::DeleteMeteringDataL( const TDesC8& aRiId )
    {
    if ( aRiId.Length() )
        {
        User::LeaveIfError( SendReceive( DRMEngine::EDeleteMeteringData,
                            TIpcArgs( &aRiId ) ) );
        return KErrNone;
        }
    else
        {
        return KErrArgument;
        }
    }
#endif

// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File
