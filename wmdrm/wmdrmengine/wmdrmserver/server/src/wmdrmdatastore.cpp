/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  WMDRM data store implementation
*
*/


#include <ezlib.h>
#include <e32math.h>
#include <symmetric.h>
#include <bacntf.h>
#include <centralrepository.h>

#include "wmdrmdatastore.h"
#include "WMDRMPrivateCRKeys.h"
#include "wmdrmkeystorage.h"
#include "slotdatacache.h"
#include "wmdrmdb.h"
#include "drmrightsstoringlocation.h"
#include "drmutilityinternaltypes.h"

#define _LOGGING_FILE L"wmdrmserver.txt"

#include "flogger.h"
#include "logfn.h"

const TInt KMegaByte( 1024 * 1024 );
const TInt KTwoHundredMegaBytes ( 200 * 1024 * 1024 );
const TInt KMaxSpaceRatio( 85 );
const TInt KMaxSpaceRatio2( 20 );
const TInt KMaxTInt64BufLength( 20 );
const TInt KDummyDbInitialSize( 0 );

#if defined(FF_PLATFORM_SIMULATOR) || defined(__WINSCW__)
_LIT8( KDummyKey, "0123456789012345" );
#endif

CWmDrmDataStore* CWmDrmDataStore::NewL( CWmDrmServer* aServer )
    {
    LOGFN( "CWmDrmDataStore::NewL" );
    CWmDrmDataStore* self = new ( ELeave ) CWmDrmDataStore( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CWmDrmDataStore::CWmDrmDataStore( CWmDrmServer* aServer )
    : iServer( aServer ), iMinFreeSpace2( 0 )
    {
    LOGFN( "CWmDrmDataStore::CWmDrmDataStore" );
    }

void CWmDrmDataStore::ConstructL()
    {
    TDrmScheme drmScheme( EDrmSchemeWmDrm );
    TFileName tempFile, tempFile2;
    TChar driveLetter;

    LOGFN( "CWmDrmDataStore::ConstructL" );

    // Check which drive is configured in the Central Repository Key
    // for the desired storing location of WM DRM rights (license store).
    iWmDrmRightsConfigFound = DrmRightsStoringLocation::CheckDrmRightsStorageDriveL(
        iServer->Fs(), drmScheme, driveLetter );

    // If the storing location is configured to other drive than the default
    // system drive, prepare utility files for that drive, too.
    if ( iWmDrmRightsConfigFound )
        {
        PrepareInfoFilesL( ETrue, driveLetter, iDummyDb2 );
        }

    // Prepare the utility files for the default system drive
    PrepareInfoFilesL( EFalse, (TUint)iServer->Fs().GetSystemDriveChar(),
        iDummyDb );
    }

CWmDrmDataStore::~CWmDrmDataStore()
    {
    LOGFN( "CWmDrmDataStore::~CWmDrmDataStore" );
    iDummyDb.Close();
    // Close the database on the configured drive
    if ( iWmDrmRightsConfigFound )
        {
        iDummyDb2.Close();
        }
    }

void CWmDrmDataStore::ReadInitialFreeSpaceL( const TDesC& aFileName,
    TBool& aConfiguredDrive )
    {
    RFile file;
    TBuf8<KMaxTInt64BufLength + 2 * KAESKeyLength> encryptedData;
    TBuf8<KMaxTInt64BufLength + 2 * KAESKeyLength> decryptedData;
    TBuf8<KAESKeyLength> key;
    TBuf8<KAESKeyLength> iv;
    CBufferedDecryptor* decryptor = NULL;
    CModeCBCDecryptor* cbcDecryptor = NULL;
    CAESDecryptor* aesDecryptor = NULL;
    CPaddingPKCS7* padding = NULL;
    TInt size = 0;

    LOGFN( "CWmDrmDataStore::ReadInitialFreeSpaceL" );
    User::LeaveIfError( file.Open( iServer->Fs(), aFileName, EFileRead ) );
    CleanupClosePushL( file );

    User::LeaveIfError( file.Size( size ) );

    if( size != ( 2 * KAESKeyLength ) )
        {
        User::Leave(KErrCorrupt);
        }

    User::LeaveIfError( file.Read( iv ) );
    User::LeaveIfError( file.Read( encryptedData ) );
#if defined(FF_PLATFORM_SIMULATOR) || defined(__WINSCW__)
    key.Copy( KDummyKey );
#else
    iServer->Cache()->iKeyStorage->GetDeviceSpecificKeyL( key );
#endif
    aesDecryptor = CAESDecryptor::NewL( key );
    CleanupStack::PushL( aesDecryptor );

    cbcDecryptor = CModeCBCDecryptor::NewL( aesDecryptor, iv );
    CleanupStack::Pop( aesDecryptor );
    CleanupStack::PushL( cbcDecryptor );

    padding = CPaddingPKCS7::NewL( KAESKeyLength );
    CleanupStack::PushL( padding );

    decryptor = CBufferedDecryptor::NewL( cbcDecryptor, padding );
    CleanupStack::Pop( 2, cbcDecryptor ); //padding, cbcDecryptor
    CleanupStack::PushL( decryptor );

    decryptor->ProcessFinalL( encryptedData, decryptedData );
    CleanupStack::PopAndDestroy( 2, &file ); //decryptor, file
    TLex8 lex( decryptedData );

    if ( aConfiguredDrive )
        {
        User::LeaveIfError( lex.Val( iInitialFreeSpace2 ) );
        }
    else
        {
        User::LeaveIfError( lex.Val( iInitialFreeSpace ) );
        }
    }

void CWmDrmDataStore::WriteInitialFreeSpaceL( const TDesC& aFileName,
    TBool& aConfiguredDrive )
    {
    RFile file;
    TBuf8<KMaxTInt64BufLength + 2 * KAESKeyLength> encryptedData;
    TBuf8<KMaxTInt64BufLength + 2 * KAESKeyLength> decryptedData;
    TBuf8<KAESKeyLength> key;
    TBuf8<KAESKeyLength> iv;
    CBufferedEncryptor* encryptor = NULL;
    CModeCBCEncryptor* cbcEncryptor = NULL;
    CAESEncryptor* aesEncryptor = NULL;
    CPaddingPKCS7* padding = NULL;

    LOGFN( "CWmDrmDataStore::WriteInitialFreeSpaceL" );
    User::LeaveIfError( file.Create( iServer->Fs(), aFileName, EFileWrite ) );
    CleanupClosePushL( file );
    if ( aConfiguredDrive )
        {
        iInitialFreeSpace2 = iServer->FreeSpaceL( aConfiguredDrive );
        }
    else
        {
        iInitialFreeSpace = iServer->FreeSpaceL( aConfiguredDrive );
        }
    iv.SetLength( KAESKeyLength );
    TRandom::RandomL( iv );
#if defined(FF_PLATFORM_SIMULATOR) || defined(__WINSCW__)
    key.Copy( KDummyKey );
#else
    iServer->Cache()->iKeyStorage->GetDeviceSpecificKeyL( key );
#endif
    aesEncryptor = CAESEncryptor::NewL( key );
    CleanupStack::PushL( aesEncryptor );

    cbcEncryptor = CModeCBCEncryptor::NewL( aesEncryptor, iv );
    CleanupStack::Pop( aesEncryptor );
    CleanupStack::PushL( cbcEncryptor );

    padding = CPaddingPKCS7::NewL( KAESKeyLength );
    CleanupStack::PushL( padding );

    encryptor = CBufferedEncryptor::NewL( cbcEncryptor, padding );
    CleanupStack::Pop( 2, cbcEncryptor ); //padding, cbcEncryptor
    CleanupStack::PushL( encryptor );

    if ( aConfiguredDrive )
        {
        decryptedData.AppendNum( iInitialFreeSpace2 );
        }
    else
        {
        decryptedData.AppendNum( iInitialFreeSpace );
        }

    encryptor->ProcessFinalL( decryptedData, encryptedData );
    User::LeaveIfError( file.Write( iv ) );
    User::LeaveIfError( file.Write( encryptedData ) );
    CleanupStack::PopAndDestroy( 2, &file ); //encryptor, file
    }

TWmDrmStoreState CWmDrmDataStore::DataStoreStateL()
    {
    TWmDrmStoreState state;
    TInt64 freeSpace( 0 );
    TInt64 freeSpace2( 0 );
    TInt dataStoreSize( 0 );
    TInt dummyDbSize( 0 );
    TInt ratio( 0 );
    TInt ratio2( 0 );
    TBool internalMassDriveNotFull( ETrue );

    LOGFN( "CWmDrmDataStore::DataStoreStateL" );
    freeSpace = iServer->FreeSpaceL( EFalse );

    if ( iWmDrmRightsConfigFound )
        {
        // Check free space from the configured drive, too.
        freeSpace2 = iServer->FreeSpaceL( ETrue );

        if ( freeSpace2 < iMinFreeSpace2 )
            {
            internalMassDriveNotFull = EFalse;
            }

        dummyDbSize = DummyDBSizeL( ETrue );
        dataStoreSize = DataStoreSizeL( ETrue );
        ratio2 = dataStoreSize * 100 / iInitialFreeSpace2;
        freeSpace2 += dummyDbSize;
#ifdef _LOGGING
        TBuf<KMaxTInt64BufLength> free2;
        LOG1( "CWmDrmDataStore::DataStoreStateL: Free space (2): ");
        free2.AppendNumUC( freeSpace2, EDecimal );
        LOG( free2 );
        TBuf<KMaxTInt64BufLength> free2Min;
        LOG1( "CWmDrmDataStore::DataStoreStateL: Minimum free space (2): ");
        free2Min.AppendNumUC( iMinFreeSpace2, EDecimal );
        LOG( free2Min );
#endif
        }

    // Check the system drive storage space next.
    dummyDbSize = DummyDBSizeL( EFalse );
    dataStoreSize = DataStoreSizeL( EFalse );
    ratio = dataStoreSize * 100 / iInitialFreeSpace;
    freeSpace += dummyDbSize;
#ifdef _LOGGING
    TBuf<KMaxTInt64BufLength> free;
    LOG1( "CWmDrmDataStore::DataStoreStateL: Free space: ");
    free.AppendNumUC( freeSpace, EDecimal );
    LOG( free );
    TBuf<KMaxTInt64BufLength> freeMin;
    LOG1( "CWmDrmDataStore::DataStoreStateL: Minimum free space: ");
    freeMin.AppendNumUC( iMinFreeSpace, EDecimal );
    LOG( freeMin );
#endif

    // Select the state of the storage space.
    if ( ( freeSpace > iMinFreeSpace ) && internalMassDriveNotFull )
        {
        LOG1( "CWmDrmDataStore::DataStoreStateL: Store space Ok" );
        state = EStoreSpaceOK;
        }
    else
        {
        // The configured drive is running out of space. The system drive
        // may also be running out of storage space, but calculate
        // the ratio of database size to initial free drive space and the
        // state of the drive storage space from the configured drive because
        // it is likely to fill up faster since the media files may be synced to it.
        if ( !internalMassDriveNotFull )
            {
            LOG2( "Ratio (2): %d", ratio2 );
            if ( ratio2 <= iMaxSpaceRatio2 )
                {
                LOG1( "CWmDrmDataStore::DataStoreStateL: Store space low (2)" );
                state = EStoreSpaceLow;
                }
            else
                {
                LOG1( "CWmDrmDataStore::DataStoreStateL: Store space full (2)" );
                state = EStoreSpaceFull;
                }
            }
        else
            // Only the system drive is running out of storage space.
            {
            LOG2( "Ratio: %d", ratio );
            if ( ratio <= iMaxSpaceRatio )
                {
                LOG1( "CWmDrmDataStore::DataStoreStateL Store space low" );
                state = EStoreSpaceLow;
                }
            else
                {
                LOG1( "CWmDrmDataStore::DataStoreStateL Store space full" );
                state = EStoreSpaceFull;
                }
            }
        }

    LOG2( "DataStoreState: %d", state );
    return state;
    }

void CWmDrmDataStore::InitializeDummyDbFileL( const TDesC& aFileName,
    RFile& aDummyDb, TBool& aConfiguredDrive )
    {
    TInt r( KErrNone );

    LOGFN( "CWmDrmDataStore::InitializeDummyDbFileL" );

    r = aDummyDb.Create( iServer->Fs(), aFileName, EFileWrite );
    if ( r == KErrAlreadyExists )
        {
        User::LeaveIfError(
            aDummyDb.Open( iServer->Fs(), aFileName, EFileWrite ) );
        }
    else if( !r )
        {
        TInt dataStoreSize( DataStoreSizeL( aConfiguredDrive ) );
        if ( aConfiguredDrive )
            {
            if ( dataStoreSize <= iDummyDbInitialSize2 )
                {
                User::LeaveIfError(
                    aDummyDb.SetSize( iDummyDbInitialSize2 - dataStoreSize ) );
                }
            else
                {
                User::LeaveIfError( aDummyDb.SetSize( 0 ) );
                }
            }
        else
            {
            if ( dataStoreSize <= iDummyDbInitialSize )
                {
                User::LeaveIfError(
                    aDummyDb.SetSize( iDummyDbInitialSize - dataStoreSize ) );
                }
            else
                {
                User::LeaveIfError( aDummyDb.SetSize( 0 ) );
                }
            }
        }
    else
        {
        User::Leave( r );
        }
    }

void CWmDrmDataStore::UpdateDummyDbFileL( TInt aSize, TBool aConfiguredDrive )
    {
    LOGFN( "CWmDrmDataStore::UpdateDummyDbFileL" );
    LOG2( "aSize: %d", aSize );
    if ( aSize > 0 )
        {
        TInt dummyDbSize( DummyDBSizeL( aConfiguredDrive ) );
        LOG2( "dummyDbSize: %d", dummyDbSize );
        if ( aSize <= dummyDbSize )
            {
            if ( aConfiguredDrive )
                {
                User::LeaveIfError( iDummyDb2.SetSize( dummyDbSize - aSize ) );
                }
            else
                {
                User::LeaveIfError( iDummyDb.SetSize( dummyDbSize - aSize ) );
                }
            }
        else
            {
            if ( aConfiguredDrive )
                {
                User::LeaveIfError( iDummyDb2.SetSize( 0 ) );
                }
            else
                {
                User::LeaveIfError( iDummyDb.SetSize( 0 ) );
                }
            }
        }
    else
        {
        TInt dataStoreSize( DataStoreSizeL( aConfiguredDrive ) );
        LOG2( "dataStoreSize: %d", dataStoreSize );
        if ( aConfiguredDrive )
            {
            if ( dataStoreSize <= iDummyDbInitialSize2 )
                {
                User::LeaveIfError(
                    iDummyDb2.SetSize( iDummyDbInitialSize2 - dataStoreSize ) );
                }
            else
                {
                User::LeaveIfError( iDummyDb2.SetSize( 0 ) );
                }
            }
        else
            {
            if ( dataStoreSize <= iDummyDbInitialSize )
                {
                User::LeaveIfError(
                    iDummyDb.SetSize( iDummyDbInitialSize - dataStoreSize ) );
                }
            else
                {
                User::LeaveIfError( iDummyDb.SetSize( 0 ) );
                }
            }
        }
    }

TInt CWmDrmDataStore::DataStoreSizeL( TBool aConfiguredDrive )
    {
    TInt dataStoreSize( iServer->Db()->DataBaseSize( aConfiguredDrive ) );
    User::LeaveIfError( dataStoreSize );
    return dataStoreSize;
    }

TInt CWmDrmDataStore::DummyDBSizeL( TBool aConfiguredDrive )
    {
    TInt dummyDbSize( 0 );
    if ( aConfiguredDrive )
        {
        User::LeaveIfError( iDummyDb2.Size( dummyDbSize ) );
        }
    else
        {
        User::LeaveIfError( iDummyDb.Size( dummyDbSize ) );
        }
    return dummyDbSize;
    }

void CWmDrmDataStore::PrepareInfoFilesL( TBool aConfiguredDrive,
    TChar aDriveLetter, RFile& aDummyDb )
    {
    LOGFN( "CWmDrmDataStore::PrepareInfoFilesL" );
    CRepository* repository( NULL );
    TInt r( KErrNone );
    TFileName dummyDbFile;
    TFileName initialFreeSpaceFile;

    initialFreeSpaceFile.Format( KPrivateDir, (TUint)aDriveLetter );
    iServer->Fs().MkDirAll( initialFreeSpaceFile );
    initialFreeSpaceFile.Format( KInitialFreeSpaceFile, (TUint)aDriveLetter );
    dummyDbFile.Format( KDummyDbFile, (TUint)aDriveLetter );

    TRAP( r, WriteInitialFreeSpaceL( initialFreeSpaceFile,
        aConfiguredDrive ) );
    if ( r )
        {
        r = KErrNone;

        // catch the read error
        TRAP( r, ReadInitialFreeSpaceL( initialFreeSpaceFile,
            aConfiguredDrive ) );

        // if an error occurs, this means that we are unable to read the info,
        // thus we need to delete the file and run write again.
        // hopefully this being a temporary error, but if we fail again we fail
        // until next ConstructL
        if( r != KErrNone )
            {
            // delete the file:
            iServer->Fs().Delete( initialFreeSpaceFile );

            // Calc & Write the new info
            WriteInitialFreeSpaceL( initialFreeSpaceFile, aConfiguredDrive );
            }
        }

    if ( aConfiguredDrive )
        {
#ifdef _LOGGING
        TBuf<KMaxTInt64BufLength> freeSpace2;
        LOG1( "CWmDrmDataStore::DataStoreStateL: Initial free space (2): ");
        freeSpace2.AppendNumUC( iInitialFreeSpace2, EDecimal );
        LOG( freeSpace2 );
#endif
        if ( iInitialFreeSpace2 <= 0 )
            {
            User::Leave( KErrNotReady );
            }
        }
    else
        {
#ifdef _LOGGING
        TBuf<KMaxTInt64BufLength> freeSpace;
        LOG1( "CWmDrmDataStore::DataStoreStateL: Initial free space: ");
        freeSpace.AppendNumUC( iInitialFreeSpace, EDecimal );
        LOG( freeSpace );
#endif
        if ( iInitialFreeSpace <= 0 )
            {
            User::Leave( KErrNotReady );
            }
        }

    //LOG1( "CWmDrmDataStore::PrepareInfoFilesL Check Cenrep" );

    TRAP( r, repository = CRepository::NewL( KCRUidWMDRM ) );
    if ( repository )
        {
        TInt rate( 0 );

        // Check the cenrep key parameters either for the default system drive
        // or for the internal mass drive depending whether the WMDRM rights
        // are configured to be partially stored to the internal mass drive or
        // not.
        if ( !aConfiguredDrive )
            {
            r = repository->Get( KWMDRMLicStoreLowMem, rate );
            if ( r )
                {
                iMinFreeSpace = KMegaByte;
                }
            else
                {
                iMinFreeSpace = rate * KMegaByte;
                }
            r = repository->Get( KWMDRMLicStoreSizeRatio, iMaxSpaceRatio );
            if ( r )
                {
                iMaxSpaceRatio = KMaxSpaceRatio;
                }
            r = repository->Get( KWMDRMLicStoreReservedSpace, rate );
            if ( r )
                {
                iDummyDbInitialSize = KDummyDbInitialSize;
                }
            else
                {
                iDummyDbInitialSize = rate * KMegaByte;
                }
            }
        else
            {
            r = repository->Get( KWMDRM2LicStoreLowMem, rate );
            if ( r )
                {
                iMinFreeSpace2 = KTwoHundredMegaBytes;
                }
            else
                {
                iMinFreeSpace2 = rate * KMegaByte;
                }
            r = repository->Get( KWMDRM2LicStoreSizeRatio, iMaxSpaceRatio2 );
            if ( r )
                {
                iMaxSpaceRatio2 = KMaxSpaceRatio2;
                }
            r = repository->Get( KWMDRM2LicStoreReservedSpace, rate );
            if ( r )
                {
                iDummyDbInitialSize2 = KDummyDbInitialSize;
                }
            else
                {
                iDummyDbInitialSize2 = rate * KMegaByte;
                }
            }
        delete repository;
        }
    else
        {
        if ( !aConfiguredDrive )
            {
            iMinFreeSpace = KMegaByte;
            iMaxSpaceRatio = KMaxSpaceRatio;
            iDummyDbInitialSize = KDummyDbInitialSize;
            }
        else
            {
            iMinFreeSpace2 = KTwoHundredMegaBytes;
            iMaxSpaceRatio2 = KMaxSpaceRatio2;
            iDummyDbInitialSize2 = KDummyDbInitialSize;
            }
        }

    InitializeDummyDbFileL( dummyDbFile, aDummyDb, aConfiguredDrive );

    }
