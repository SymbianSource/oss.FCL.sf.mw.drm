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
#include <DriveInfo.h>
#endif

#include "DrmKeyStorage.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// Test logging macros

#ifdef RD_MULTIPLE_DRIVE
_LIT(KFullLogDir, "%c:\\logs\\drm\\");
#else
_LIT(KFullLogDir, "c:\\logs\\drm\\");
#endif

_LIT(KLogDir, "drm");
_LIT(KLogName, "DrmEncryptorBb5KeyStorage.log");

#define TEST_STEP(string) \
    GBb5Log.WriteFormat(_L("Next Test: %S"), &string);

#define CHECK(condition) \
    if (!condition) GBb5Log.WriteFormat(_L("FAIL: line %d"), __LINE__);

// MODULE DATA STRUCTURES

RFs GBb5Fs;
RFileLogger GBb5Log;

// STATIC TEST CONTENT AND RIGHTS OBJECTS

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

HBufC8* I2OSPL(
    RInteger& aInt)
    {
    HBufC8* r = aInt.BufferLC();
    CleanupStack::Pop(r);
    return r;
    }

RInteger OS2IPL(
    const TDesC8& aOctetStream)
    {
    RInteger r;
    TInt i;

    r = RInteger::NewL(0);
    for (i = 0; i < aOctetStream.Length(); i++)
        {
        r *= 256;
        r += aOctetStream[i];
        }
    return r;
    }

HBufC8* RsaEncryptL(
    CRSAPublicKey* aKey,
    const TDesC8& aInput)
    {
    RInteger result;
    RInteger input;
    HBufC8* output;

    input = OS2IPL(aInput);
    CleanupClosePushL(input);
    result = TInteger::ModularExponentiateL(input, aKey->E(), aKey->N());
    CleanupClosePushL(result);
    output = I2OSPL(result);
    CleanupStack::PopAndDestroy(2); // result, input
    return output;
    }

LOCAL_C TUint MDrmKeyStorage_GetCertificateChainL()
    {
    MDrmKeyStorage* storage = NULL;
    RPointerArray<HBufC8> chain;
    TInt i;
    TUint result = NULL;

    GBb5Log.WriteFormat(_L("MDrmKeyStorage_GetCertificateChainL -> DrmKeyStorageNewL"));
    TRAPD(err,storage = DrmKeyStorageNewL());
    if (err != KErrNone)
        {
            result = err;
        }

    GBb5Log.WriteFormat(_L("MDrmKeyStorage_GetCertificateChainL -> SelectDefaultRootL"));
    storage->SelectDefaultRootL();

    GBb5Log.WriteFormat(_L("MDrmKeyStorage_GetCertificateChainL -> GetCertificateChainL"));
    storage->GetCertificateChainL(chain);

    for (i = 0; i < chain.Count(); i++)
        {
        GBb5Log.WriteFormat(_L("Certificate %d:"), i);
        GBb5Log.HexDump(_S(""), _S(""), chain[i]->Ptr(), chain[i]->Length());
        }
    chain.ResetAndDestroy();
    chain.Close();
    delete storage;
    return result;
    }

LOCAL_C TUint MDrmKeyStorage_DecryptL()
    {
    MDrmKeyStorage* storage = NULL;
    RPointerArray<HBufC8> chain;
    CRSAPublicKey* key = NULL;
    CX509Certificate* cert = NULL;
    TX509KeyFactory factory;
    TBuf8<128> data;
    HBufC8* encData;
    HBufC8* decData;
    TUint result = KErrNone;


    GBb5Log.WriteFormat(_L("MDrmKeyStorage_Decrypt"));
    storage = DrmKeyStorageNewL();
    storage->SelectDefaultRootL();
    storage->GetCertificateChainL(chain);
    cert = CX509Certificate::NewL(*chain[0]);
    chain.ResetAndDestroy();
    chain.Close();
    key = factory.RSAPublicKeyL(cert->PublicKey().KeyData());
    data.SetLength(128);
    data.Fill(1);
    GBb5Log.WriteFormat(_L("data:"));
    GBb5Log.HexDump(_S(""), _S(""), &data[0], sizeof(data));

    encData = RsaEncryptL(key, data);
    GBb5Log.WriteFormat(_L("encrypted data:"));
    GBb5Log.HexDump(_S(""), _S(""), encData->Ptr(), encData->Length());

    decData = storage->RsaDecryptL(*encData);
    GBb5Log.WriteFormat(_L("decrypted data :"));
    GBb5Log.HexDump(_S(""), _S(""), decData->Ptr(), decData->Length());

    delete cert;
    delete key;
    delete storage;
    return result;
    }

// ==================== TEST FUNCTIONS =====================



TUint Bb5KeyStorage()
    {
    TUint result = 0;
    TInt catchy = 0;
    result = GBb5Fs.Connect();
    if( result != KErrNone )
        {
        return result;
        }

#ifndef RD_MULTIPLE_DRIVE

    GBb5Fs.MkDirAll(KFullLogDir);

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
    GBb5Fs.DriveToChar( driveNumber, driveLetter );

    TFileName fullLogDir;
    fullLogDir.Format( KFullLogDir, (TUint)driveLetter );

    GBb5Fs.MkDirAll(fullLogDir);

#endif

    result = GBb5Log.Connect();
    if( result != KErrNone )
        {
        GBb5Fs.Close();
        return result;
        }
    GBb5Log.CreateLog(KLogDir, KLogName, EFileLoggingModeOverwrite);
    GBb5Log.Write(_L("Start %D"));

    TRAPD(err,result = MDrmKeyStorage_GetCertificateChainL());
    CHECK(err == KErrNone);
    if (err ==KErrNone)
        {
        TRAP(catchy, result = MDrmKeyStorage_DecryptL());
        if( catchy )
            {
            result = catchy;
            }
        }
    CHECK(err == KErrNone);
    GBb5Log.CloseLog();
    GBb5Fs.Close();
    return result;
    }

