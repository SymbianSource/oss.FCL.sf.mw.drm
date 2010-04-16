/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <f32file.h>
#include <flogger.h>
#include <x509cert.h>
#include <symmetric.h>
#include <asymmetric.h>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include <zipfile.h>
#include <aknnotewrappers.h>

#include "DrmKeyStorage.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// Test logging macros

#ifdef RD_MULTIPLE_DRIVE
_LIT(KPrivateRightsDir, "%c:\\private\\101F51F2\\PKI");
_LIT(KFullLogDir, "%c:\\logs\\drm\\");
#else
_LIT(KPrivateRightsDir, "c:\\private\\101F51F2\\PKI");
_LIT(KFullLogDir, "c:\\logs\\drm\\");
#endif


#ifdef __WINS__
_LIT(KInputDir, "c:\\data\\drm\\keys\\");
_LIT(KInputFilePattern, "c:\\data\\drm\\keys\\SigningCert*");
#else
#ifdef RD_MULTIPLE_DRIVE
_LIT(KInputDir, "%c:\\drm\\keys\\");
_LIT(KInputFilePattern, "%c:\\drm\\keys\\SigningCert*");
#else
_LIT(KInputDir, "e:\\drm\\keys\\");
_LIT(KInputFilePattern, "e:\\drm\\keys\\SigningCert*");
#endif
#endif

_LIT(KLogDir, "drm");
_LIT(KLogName, "MtDrmKeyStorage.log");
_LIT(KDeviceKeyFileName, "DevicePrivateKey.der");
_LIT(KDeviceCertFileName, "DeviceCert.der");

#define TEST_STEP(string) \
    GLog.WriteFormat(_L("Next Test: %S"), &string);

#define CHECK(condition) \
    if (!condition) GLog.WriteFormat(_L("FAIL: line %d"), __LINE__);

// MODULE DATA STRUCTURES

RFs GFs;
RFileLogger GLog;

// STATIC TEST CONTENT AND RIGHTS OBJECTS

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

LOCAL_C void ReadFileL(HBufC8*& aContent, const TDesC& aName)
    {
    TInt size = 0;
    RFile file;

    User::LeaveIfError(file.Open(GFs, aName, EFileRead));
    User::LeaveIfError(file.Size(size));
    aContent = HBufC8::NewLC(size);
    TPtr8 ptr(aContent->Des());
    User::LeaveIfError(file.Read(ptr, size));
    CleanupStack::Pop(); //aContent
    }


// ==================== TEST FUNCTIONS =====================

LOCAL_C TUint MDrmKeyStorage_ImportDataL()
    {
    MDrmKeyStorage* storage = NULL;
    HBufC8* privateKey = NULL;
    HBufC8* cert = NULL;
    RArray<TPtrC8> certChain;
    RPointerArray<HBufC8> buffers;
    TFileName fileName;
    TInt i;
    CDir* dir;
    TEntry entry;
    TUint fileCount = 0;

    storage = DrmKeyStorageNewL();

#ifndef RD_MULTIPLE_DRIVE

    GFs.SetSessionPath(KInputDir);

#else //RD_MULTIPLE_DRIVE

    TFileName tempPath;
    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultMassStorage, driveNumber );
    GFs.DriveToChar( driveNumber, driveLetter );

    tempPath.Format( KInputDir, (TUint)driveLetter );

    GFs.SetSessionPath(tempPath);

#endif

    ReadFileL(privateKey, KDeviceKeyFileName);
    ReadFileL(cert, KDeviceCertFileName);
    buffers.Append(cert);

#ifndef RD_MULTIPLE_DRIVE

    GFs.GetDir(KInputFilePattern, KEntryAttNormal, ESortByName, dir);

#else //RD_MULTIPLE_DRIVE

    tempPath.Format( KInputFilePattern, (TUint)driveLetter );

    GFs.GetDir(tempPath, KEntryAttNormal, ESortByName, dir);

#endif

    for (i = 0; i < dir->Count(); i++)
        {
        ReadFileL(cert, (*dir)[i].iName);
        buffers.Append(cert);
        }
    for (i = 0; i < buffers.Count(); i++)
        {
        certChain.Append(*(buffers[i]));
        }
    storage->ImportDataL(*privateKey, certChain);
    delete storage;
    delete privateKey;
    fileCount = buffers.Count();
    certChain.Close();
    buffers.ResetAndDestroy();
    buffers.Close();
    return fileCount;
    }


TUint KeyStorage()
    {
    TUint result = 0;
    result = GFs.Connect();
    if( result != KErrNone )
        {
        return result;
        }

#ifndef RD_MULTIPLE_DRIVE

    GFs.MkDirAll(KFullLogDir);
    GFs.MkDirAll(KPrivateRightsDir);
    GFs.MkDirAll(KInputDir);

#else //RD_MULTIPLE_DRIVE

    TFileName tempPath;
    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
    GFs.DriveToChar( driveNumber, driveLetter );

    tempPath.Format( KFullLogDir, (TUint)driveLetter );
    GFs.MkDirAll(tempPath);

    tempPath.Format( KPrivateRightsDir, (TUint)driveLetter );
    GFs.MkDirAll(tempPath);

    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultMassStorage, driveNumber );
    GFs.DriveToChar( driveNumber, driveLetter );

    tempPath.Format( KInputDir, (TUint)driveLetter );
    GFs.MkDirAll(tempPath);

#endif


    result = GLog.Connect();
    if( result != KErrNone )
        {
        GFs.Close();
        return result;
        }
    GLog.CreateLog(KLogDir, KLogName, EFileLoggingModeOverwrite);
    GLog.Write(_L("Start %D"));

    TRAPD(err,result = MDrmKeyStorage_ImportDataL());

    GLog.WriteFormat(_L("Result: %d"), err);
    CHECK(err == KErrNone);
    GLog.CloseLog();
    GFs.Close();
    return result;
    }

TUint KeyStorageFromZipL(const TDesC& aFileName, RFs& aFs)
    {
    RFile f;
    CZipFile* file;
    CZipFileMemberIterator* iter;
    CZipFileMember* member;
    MDrmKeyStorage* storage = NULL;
    TInt err = KErrNone;
    HBufC8* privateKey = NULL;
    HBufC8* cert = NULL;
    RArray<TPtrC8> certChain;
    TInt numCerts;
    TInt i;
    RZipFileMemberReaderStream* data;
    TPtr8 ptr(NULL, 0);
    TFileName fileName;
    RPointerArray<HBufC8> buffers;

    storage = DrmKeyStorageNewL();

    err = f.Open( aFs, aFileName, EFileShareReadersOrWriters | EFileRead );
    if ( err == KErrInUse )
        {
        err = f.Open( aFs, aFileName, EFileShareAny | EFileRead );
        if ( err == KErrInUse )
            {
            err = f.Open( aFs, aFileName, EFileShareReadersOnly| EFileRead );
            }
        }
    User::LeaveIfError( err );

    CleanupClosePushL<RFile>( f );
    file = CZipFile::NewL( aFs, f );
    CleanupStack::PushL( file );
    iter = file->GetMembersL();
    CleanupStack::PushL( iter );
    member = iter->NextL();
    numCerts = 0;
    while ( member != NULL )
        {
        if (member->Name()->Left(11).CompareF(_L("SigningCert")) == 0)
            {
            numCerts++;
            }
        delete member;
        member = iter->NextL();
        }

    member = file->CaseInsensitiveMemberL(_L("DeviceCert.der"));
    cert = HBufC8::NewL(member->UncompressedSize());
    buffers.Append(cert);
    file->GetInputStreamL(member, data);
    ptr.Set(cert->Des());
    data->Read(ptr, member->UncompressedSize());
    certChain.Append(ptr);
    delete data;
    delete member;
    for (i = 0; i < numCerts; i++)
        {
        fileName.Copy(_L("SigningCert"));
        if (i < 10)
            {
            fileName.Append(_L("0"));
            }
        fileName.AppendNum(i);
        fileName.Append(_L(".der"));
        member = file->CaseInsensitiveMemberL(fileName);
        cert = HBufC8::NewL(member->UncompressedSize());
        buffers.Append(cert);
        file->GetInputStreamL(member, data);
        ptr.Set(cert->Des());
        data->Read(ptr, member->UncompressedSize());
        certChain.Append(ptr);
        delete data;
        delete member;
        }
    member = file->CaseInsensitiveMemberL(_L("DevicePrivateKey.der"));
    privateKey = HBufC8::NewL(member->UncompressedSize());
    file->GetInputStreamL(member, data);
    ptr.Set(privateKey->Des());
    data->Read(ptr, member->UncompressedSize());
    delete data;
    delete member;

    storage->ImportDataL(*privateKey, certChain);
    delete privateKey;
    certChain.Close();
    buffers.ResetAndDestroy();

    CleanupStack::PopAndDestroy(3);
    delete storage;

    numCerts++;
    fileName.SetLength(0);
    fileName.AppendNum(numCerts);
    fileName.Append(_L(" keys imported"));
    CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
    informationNote->ExecuteLD(fileName);

    return KErrNone;
    }

