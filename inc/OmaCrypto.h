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
* Description:  ?Description
*
*/



#ifndef OMACRYPTO_H
#define OMACRYPTO_H

#include <bigint.h>

// FORWARD DECLARATIONS

class CRSAPrivateKey;
class CRSAPublicKey;
class MDrmKeyStorage;

// CLASS DECLARATION

/**
*  OmaCrypto: Contains crypto operations for OMA DRM 2.0
*
*  @lib    -
*  @since  3.0
*/
class OmaCrypto
    {
public: // Constants    

    static const TInt KKeySize = 16;
    static const TInt KMacSize = 16;
    static const TInt KWrapBlockSize = 8;
    static const TInt KWrappedKeySize = KKeySize + KMacSize + KWrapBlockSize;
    static const TInt KPssSaltLength = 20;

public: // New functions

    IMPORT_C static HBufC8* RsaDecryptL(
        MDrmKeyStorage* aKeyStorage,
        const TDesC8& aInput);

    IMPORT_C static HBufC8* RsaEncryptL(
        CRSAPublicKey* aKey,
        const TDesC8& aInput);
    
    IMPORT_C static HBufC8* RsaVerifyL(
        CRSAPublicKey* aKey,
        const TDesC8& aInput);
    
    IMPORT_C static HBufC8* RsaKemKwsEncryptL(
        CRSAPublicKey* aKey,
        const TDesC8& aRek,
        const TDesC8& aMac);

    IMPORT_C static void RsaKemKwsDecryptL(
        MDrmKeyStorage* aKeyStorage,
        const TDesC8& aInput,
        TDes8& aRek,
        TDes8& aMac);

    IMPORT_C static HBufC8* KdfL(
        const TDesC8& aInput,
        const TDesC8& aOtherData,
        TInt aKLen);

    IMPORT_C static HBufC8* AesWrapL(
        const TDesC8& aKey,
        const TDesC8& aInput);

    IMPORT_C static HBufC8* AesUnwrapL(
        const TDesC8& aKey,
        const TDesC8& aInput);
        
    IMPORT_C static HBufC8* I2OSPL(
        RInteger& aInt);

    IMPORT_C static RInteger OS2IPL(
        const TDesC8& aOctetStream);

    IMPORT_C static HBufC8* Mgf1L(
        const TDesC8& aMfgSeed,
        TInt aMaskLen);
        
    IMPORT_C static HBufC8* RsaPssSignHashL(
        MDrmKeyStorage* aKeyStorage,
        const TDesC8& aMHash);

    IMPORT_C static TBool RsaPssVerifyHashL(
        CRSAPublicKey* aKey,
        const TDesC8& aSignature,
        const TDesC8& aMHash);
        
protected: // New functions

    IMPORT_C static void WriteUint32ToBlock(
        TUint32 aInt,
        TDes8& aBlock,
        TInt aOffset);
    };

#endif      // OMACRYPTO_H   
            
// End of File
