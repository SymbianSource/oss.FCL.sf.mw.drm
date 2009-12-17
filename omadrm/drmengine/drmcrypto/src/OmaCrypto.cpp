/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  OMA DRM 2.x Crypto Algorithms
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <asymmetric.h>
#include <symmetric.h>
#include <hash.h>
#include "OmaCrypto.h"
#include "DrmKeyStorage.h"

#ifdef _DEBUG
#define LOGGING
#endif

#ifdef LOGGING
_LIT(KLogDir, "DRM");
_LIT(KLogName, "Crypto.log");
#include "flogger.h"
#define LOG(string) \
    RFileLogger::Write(KLogDir, KLogName, \
        EFileLoggingModeAppend, string);
#define LOGHEX(buffer) \
    RFileLogger::HexDump(KLogDir, KLogName, \
        EFileLoggingModeAppend, _S(""), _S(""), \
        buffer.Ptr(), buffer.Length());
#define LOGFMT(string, value) \
    RFileLogger::WriteFormat(KLogDir, KLogName, \
        EFileLoggingModeAppend, string, value);
#else
#define LOG
#define LOGHEX
#define LOGFMT
#endif

// LOCAL CONSTANTS AND MACROS

_LIT8(KWrapIv, "\xA6\xA6\xA6\xA6\xA6\xA6\xA6\xA6");

// LOCAL FUNCTION PROTOTYPES
LOCAL_C void DeleteObject( TAny* aObject );

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

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
// OmaCrypto::
//
// -----------------------------------------------------------------------------
//
EXPORT_C void OmaCrypto::WriteUint32ToBlock(
    TUint32 aInt,
    TDes8& aBlock,
    TInt aOffset)
    {
    aBlock[aOffset] =  (aInt & 0xff000000) >> 24;
    aBlock[aOffset + 1] = (aInt & 0x00ff0000) >> 16;
    aBlock[aOffset + 2] = (aInt & 0x0000ff00) >> 8;
    aBlock[aOffset + 3] = (aInt & 0x000000ff);
    }

// -----------------------------------------------------------------------------
// OmaCrypto::
//
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* OmaCrypto::RsaDecryptL(
    MDrmKeyStorage* aKeyStorage,
    const TDesC8& aInput)
    {
    return aKeyStorage->RsaDecryptL(aInput);
    }

// -----------------------------------------------------------------------------
// OmaCrypto::
//
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* OmaCrypto::RsaEncryptL(
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

#ifdef LOGGING
    HBufC8* buffer = NULL;
    LOG(_L8("RsaEncryptL"));
    LOG(_L8("Input"));
    LOGHEX(aInput);
    buffer = aKey->E().BufferLC();
    LOG(_L8("E"));
    LOGHEX((*buffer));
    CleanupStack::PopAndDestroy(buffer);
    buffer = aKey->N().BufferLC();
    LOG(_L8("N"));
    LOGHEX((*buffer));
    CleanupStack::PopAndDestroy(buffer);
    LOG(_L8("Result"));
    LOGHEX((*output));
#endif

    CleanupStack::PopAndDestroy(2); // result, input
    return output;
    }

// -----------------------------------------------------------------------------
// OmaCrypto::
//
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* OmaCrypto::RsaVerifyL(
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

// -----------------------------------------------------------------------------
// OmaCrypto::
//
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* OmaCrypto::RsaKemKwsEncryptL(
    CRSAPublicKey* aKey,
    const TDesC8& aRek,
    const TDesC8& aMac)
    {
    HBufC8* C1 = NULL;
    HBufC8* C2 = NULL;
    HBufC8* C;
    TPtr8 c(0, 0);
    TBuf8<128> Z;
    TBuf8<KKeySize + KMacSize> keyAndMac;
    HBufC8* kek = NULL;
    const TUint8 KFirstBitMask( 0x7f );

    Z.SetLength(128);
    MDrmKeyStorage* storage = DrmKeyStorageNewL();
    TCleanupItem storageCleanup( DeleteObject, storage );
    CleanupStack::PushL(storageCleanup);
    storage->RandomDataGetL(Z,Z.Size());
    CleanupStack::PopAndDestroy( storage );

    Z[0] &= KFirstBitMask;

#ifdef LOGGING
    LOG(_L("RsaKemKwsEncryptL"));
    LOG(_L("random Z"));
    LOGHEX(Z);
#endif
    C1 = RsaEncryptL(aKey, Z);
    CleanupStack::PushL(C1);
    kek = KdfL(Z, KNullDesC8, KKeySize);
    CleanupStack::PushL(kek);
    keyAndMac.Copy(aMac);
    keyAndMac.Append(aRek);
    C2 = AesWrapL(*kek, keyAndMac);
    CleanupStack::PushL(C2);
    C = HBufC8::NewL(C1->Length() + C2->Length());
    c.Set(C->Des());
    c.Copy(*C1);
    c.Append(*C2);

#ifdef LOGGING
    LOG(_L("RsaKemKwsEncryptL"));
    LOG(_L("Z"));
    LOGHEX(Z);
    LOG(_L("C1"));
    LOGHEX((*C1));
    LOG(_L("C2"));
    LOGHEX((*C2));
#endif

    CleanupStack::PopAndDestroy(3); // C2, kek, C1
    return C;
    }

// -----------------------------------------------------------------------------
// OmaCrypto::
//
// -----------------------------------------------------------------------------
//
EXPORT_C void OmaCrypto::RsaKemKwsDecryptL(
    MDrmKeyStorage* aKeyStorage,
    const TDesC8& aInput,
    TDes8& aRek,
    TDes8& aMac)
    {
    HBufC8* C1 = NULL;
    HBufC8* C2 = NULL;
    HBufC8* Z = NULL;
    HBufC8* keyAndMac = NULL;
    HBufC8* kek = NULL;

    C1 = aInput.Left(aInput.Length() - KWrappedKeySize).AllocL();
    CleanupStack::PushL(C1);
    C2 = aInput.Right(KWrappedKeySize).AllocL();
    CleanupStack::PushL(C2);
    Z = aKeyStorage->RsaDecryptL(*C1);
    CleanupStack::PushL(Z);
    kek = KdfL(*Z, KNullDesC8, KKeySize);
    CleanupStack::PushL(kek);
    keyAndMac = AesUnwrapL(*kek, *C2);
    aMac.Copy(keyAndMac->Left(KKeySize));
    aRek.Copy(keyAndMac->Right(KMacSize));

#ifdef LOGGING
    LOG(_L("RsaKemKwsDecryptL"));
    LOG(_L("Z"));
    LOGHEX((*Z));
    LOG(_L("C1"));
    LOGHEX((*C1));
    LOG(_L("C2"));
    LOGHEX((*C2));
    LOG(_L("REK + MAC"));
    LOGHEX((*keyAndMac));
    LOG(_L("KEK"));
    LOGHEX((*kek));
#endif

    delete keyAndMac;
    CleanupStack::PopAndDestroy(4); // kek, Z, C2, C1
    }

// -----------------------------------------------------------------------------
// OmaCrypto::
//
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* OmaCrypto::KdfL(
    const TDesC8& aInput,
    const TDesC8& aOtherData,
    TInt aKLen)
    {
    TUint32 d;
    CSHA1* h = NULL;
    TBuf8<sizeof(d)> c;
    HBufC8* t = NULL;
    HBufC8* r = NULL;
    TPtr8 T(0, 0);

    h = CSHA1::NewL();
    CleanupStack::PushL(h);
    t = HBufC8::NewL(aKLen + SHA1_HASH);
    CleanupStack::PushL(t);
    T.Set(t->Des());
    c.SetLength(sizeof(d));
    d = 1;
    do
        {
        WriteUint32ToBlock(d, c, 0);
        h->Hash(aInput);
        h->Hash(c);
        if (aOtherData.Length() > 0)
            {
            h->Hash(aOtherData);
            }
        T.Append(h->Final());
        d++;
        }
    while (d < SHA1_HASH / aKLen);
    r = t->Left(aKLen).AllocL();
    CleanupStack::PopAndDestroy(2); // Tbuf, h
    return r;
    }

// -----------------------------------------------------------------------------
// OmaCrypto::
//
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* OmaCrypto::I2OSPL(
    RInteger& aInt)
    {
    HBufC8* r = aInt.BufferLC();
    CleanupStack::Pop(r);
    return r;
    }

// -----------------------------------------------------------------------------
// OmaCrypto::
//
// -----------------------------------------------------------------------------
//
EXPORT_C RInteger OmaCrypto::OS2IPL(
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

// -----------------------------------------------------------------------------
// OmaCrypto::
//
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* OmaCrypto::AesWrapL(
    const TDesC8& aKey,
    const TDesC8& aInput)
    {
    HBufC8* ret = NULL;
    TBuf8<KWrapBlockSize> A;
    TBuf8<2 * KWrapBlockSize> B;
    TInt i;
    TInt j;
    TPtr8 R(0, 0);
    TPtr8 Rn(0, 0);
    TInt t;
    TInt n;
    CAESEncryptor* e;

    e = CAESEncryptor::NewL(aKey);
    User::LeaveIfNull(e);
    CleanupStack::PushL(e);
    n = aInput.Length() / KWrapBlockSize;
    ret = HBufC8::NewMax((n + 1) * KWrapBlockSize);
    User::LeaveIfNull(ret);
    R.Set(ret->Des());
    A.Copy(KWrapIv);
    R.Copy(A);
    R.Append(aInput);
    for (j = 0; j <= 5; j++)
        {
        for (i = 1; i <= n; i++)
            {
            t = (n * j) + i;
            Rn.Set(const_cast<TUint8*>(R.Ptr()) + KWrapBlockSize * i,
                KWrapBlockSize, KWrapBlockSize);
            B.Copy(A);
            B.Append(Rn);
            e->Transform(B);
            A.Copy(B.Left(KWrapBlockSize));
            A[KWrapBlockSize - 1] ^= t;
            Rn.Copy(B.Right(KWrapBlockSize));
            }
        }
    Rn.Set(const_cast<TUint8*>(R.Ptr()), KWrapBlockSize, KWrapBlockSize);
    Rn.Copy(A);
    CleanupStack::PopAndDestroy(); // e
    return ret;
    }

// -----------------------------------------------------------------------------
// OmaCrypto::
//
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* OmaCrypto::AesUnwrapL(
    const TDesC8& aKey,
    const TDesC8& aInput)
    {
    HBufC8* ret = NULL;
    TBuf8<KWrapBlockSize> A;
    TBuf8<2 * KWrapBlockSize> B;
    TInt i;
    TInt j;
    TPtr8 R(0, 0);
    TPtr8 Rn(0, 0);
    TInt t;
    TInt n;
    CAESDecryptor* d;

    d = CAESDecryptor::NewL(aKey);
    User::LeaveIfNull(d);
    CleanupStack::PushL(d);
    n = (aInput.Length() / KWrapBlockSize) - 1;
    ret = HBufC8::NewMax((n + 1) * KWrapBlockSize);
    User::LeaveIfNull(ret);
    R.Set(ret->Des());
    A.Copy(aInput.Left(KWrapBlockSize));
    R.Copy(aInput);
    for (j = 5; j >= 0; j--)
        {
        for (i = n; i >= 1; i--)
            {
            t = (n * j) + i;
            Rn.Set(const_cast<TUint8*>(R.Ptr()) + KWrapBlockSize * i,
                KWrapBlockSize, KWrapBlockSize);
            A[KWrapBlockSize - 1] ^= t;
            B.Copy(A);
            B.Append(Rn);
            d->Transform(B);
            A.Copy(B.Left(KWrapBlockSize));
            Rn.Copy(B.Right(KWrapBlockSize));
            }
        }
    R.Delete(0, KWrapBlockSize);
    CleanupStack::PopAndDestroy(); // e

#ifdef LOGGING
    LOG(_L("OmaCrypto::AesUnwrapL"));
    LOG(_L("Key:"));
    LOGHEX(aKey);
    LOG(_L("Input:"));
    LOGHEX(aInput);
    LOG(_L("Ret:"));
    LOGHEX((*ret));
#endif

    return ret;
    }

// -----------------------------------------------------------------------------
// OmaCrypto::
//
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* OmaCrypto::Mgf1L(
    const TDesC8& aMfgSeed,
    TInt aMaskLen)
    {
    TUint32 c;
    CSHA1* h = NULL;
    TBuf8<4> counter;
    HBufC8* t = NULL;
    TPtr8 T(0, 0);
    TInt n;

    n = aMaskLen / SHA1_HASH + (aMaskLen % SHA1_HASH ? 1 : 0);
    h = CSHA1::NewL();
    CleanupStack::PushL(h);
    t = HBufC8::NewL(aMaskLen + 2 * SHA1_HASH + 1);
    CleanupStack::PushL(t);
    T.Set(t->Des());
    counter.SetLength(4);
    c = 0;
    do
        {
        WriteUint32ToBlock(c, counter, 0);
        h->Hash(aMfgSeed);
        h->Hash(counter);
        T.Append(h->Final());
        c++;
        }
    while (c < n);
    T.SetLength(aMaskLen);
    CleanupStack::Pop(); // t
    CleanupStack::PopAndDestroy(); // h
    return t;
    }

// -----------------------------------------------------------------------------
// OmaCrypto::RsaPssSignHashL
// Sign a hash (not a message!) using RSASSA-PSS.
// NOTE: The implementation is only correct for a private key modulus divisible
// by eight!
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* OmaCrypto::RsaPssSignHashL(
    MDrmKeyStorage* aKeyStorage,
    const TDesC8& aMHash)
    {
    CSHA1* hasher = NULL;
    HBufC8* em = NULL;
    HBufC8* S = NULL;
    TPtr8 EM(0, 0);
    TInt modBits;
    TInt emLen;
    TBuf8<KPssSaltLength> salt;
    TBuf8<SHA1_HASH> H;
    _LIT8(KMNullVector, "\0\0\0\0\0\0\0\0");
    HBufC8* db = NULL;
    TPtr8 DB(0, 0);
    HBufC8* dbMask = NULL;
    TInt i;

    modBits = aKeyStorage->ModulusSize();

    if( modBits < 0)
        {
        User::LeaveIfError( modBits );
        }
    emLen = modBits / 8;
    hasher = CSHA1::NewL();
    CleanupStack::PushL(hasher);

    salt.SetLength(KPssSaltLength);

    aKeyStorage->RandomDataGetL(salt,salt.Size());

#ifdef LOGGING
    LOG(_L("RsaKemKwsEncryptL"));
    LOG(_L("salt"));
    LOGHEX(salt);
#endif

    hasher->Hash(KMNullVector);
    hasher->Hash(aMHash);
    hasher->Hash(salt);
    H.Copy(hasher->Final());

    db = HBufC8::NewL(emLen - SHA1_HASH - 1);
    CleanupStack::PushL(db);
    DB.Set(db->Des());
    DB.FillZ(emLen - KPssSaltLength - SHA1_HASH - 2);
    DB.Append(0x01);
    DB.Append(salt);

    dbMask = Mgf1L(H, emLen - SHA1_HASH - 1);
    CleanupStack::PushL(dbMask);

    for (i = 0; i < emLen - SHA1_HASH - 1; i++)
        {
        DB[i] = DB[i] ^ (*dbMask)[i];
        }

    em = HBufC8::NewL(emLen);
    CleanupStack::PushL(em);
    EM.Set(em->Des());
    EM.Copy(DB);
    EM[0] &= 0x7f;
    EM.Append(H);
    EM.Append(0xbc);
    S = aKeyStorage->RsaSignL(EM);

    CleanupStack::PopAndDestroy(4); // em, dbMask, db, hasher
    return S;
    }

// -----------------------------------------------------------------------------
// OmaCrypto::
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool OmaCrypto::RsaPssVerifyHashL(
    CRSAPublicKey* aKey,
    const TDesC8& aSignature,
    const TDesC8& aMHash)
    {
    TBool r = EFalse;
    CSHA1* hasher = NULL;
    HBufC8* EM = NULL;
    TInt emLen;
    TBuf8<KPssSaltLength> salt;
    TBuf8<SHA1_HASH> H;
    _LIT8(KMNullVector, "\0\0\0\0\0\0\0\0");
    HBufC8* db = NULL;
    TPtr8 DB(0, 0);
    HBufC8* dbMask = NULL;
    TInt i;

    EM = RsaVerifyL(aKey, aSignature);
    CleanupStack::PushL(EM);
    emLen = EM->Size();

    hasher = CSHA1::NewL();
    CleanupStack::PushL(hasher);

    db = EM->Left(emLen - SHA1_HASH - 1).AllocL();
    CleanupStack::PushL(db);
    DB.Set(db->Des());
    H.Copy(EM->Mid(emLen - SHA1_HASH - 1, SHA1_HASH));

    dbMask = Mgf1L(H, emLen - SHA1_HASH - 1);
    CleanupStack::PushL(dbMask);

    for (i = 0; i < emLen - SHA1_HASH - 1; i++)
        {
        DB[i] = DB[i] ^ (*dbMask)[i];
        }

    salt.Copy(DB.Right(KPssSaltLength));

    hasher->Hash(KMNullVector);
    hasher->Hash(aMHash);
    hasher->Hash(salt);
    if (hasher->Final().Compare(H) == 0)
        {
        r = ETrue;
        }

    CleanupStack::PopAndDestroy(4); // dbMask, db, hasher, EM
    return r;
    }

//  End of File
