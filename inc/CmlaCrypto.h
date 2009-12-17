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



#ifndef CMLACRYPTO_H
#define CMLACRYPTO_H

#include <bigint.h>
#include "Oma2Agent.h"
#include "OmaCrypto.h"

// FORWARD DECLARATIONS
class MDrmKeyStorage;
class CRSAPublicKey;

// CLASS DECLARATION

/**
*  CmlaCrypto: Contains crypto operations for CMLA
*
*  @lib    -
*  @since  3.0
*/
class CmlaCrypto: public OmaCrypto
    {
public: // Constants

    static const TInt KDdtPermLength = 8;
    static const TInt KDdtExpLength = 3;

public: // New functions

    IMPORT_C static TInt SupportedAlgorithmsL(
        RArray<TPtrC8>& aAlgorithmList);

    IMPORT_C static HBufC8* DdtPermL(
        const TDesC8& aInput);

    IMPORT_C static HBufC8* DdtPermInvL(
        const TDesC8& aInput);

    IMPORT_C static HBufC8* DdtExpL(
        const TDesC8& aInput);

    IMPORT_C static HBufC8* DdtExpInvL(
        const TDesC8& aInput);
        
    IMPORT_C static HBufC8* WrapL(
        const TDesC8& aKek,
        const TDesC8& aKey);

    IMPORT_C static HBufC8* UnwrapL(
        const TDesC8& aKek,
        const TDesC8& aKey);

    IMPORT_C static HBufC8* KdfL(
        const TDesC8& aInput);
        
    IMPORT_C static HBufC8* RsaDecryptCmlaL(
        MDrmKeyStorage* aKeyStorage,
        const TDesC8& aInput);

    IMPORT_C static HBufC8* RsaEncryptCmlaL(
        CRSAPublicKey* aKey,
        const TDesC8& aInput);
        
    IMPORT_C static void CmlaIpDecryptL(
        TKeyTransportScheme aTransportScheme,
        MDrmKeyStorage* aKey,
        const TDesC8& aInput,
        TDes8& aRek,
        TDes8& aMac);
        
    IMPORT_C static HBufC8* CmlaIpEncryptL(
        TKeyTransportScheme aTransportScheme,
        CRSAPublicKey* aKey,
        const TDesC8& aRek,
        const TDesC8& aMac);
        
    IMPORT_C static TKeyTransportScheme AlgorithmIdToTransportScheme(
        const TDesC8& aAlgorithmId);

    
protected: // New functions
    };

#endif      // CMLACRYPTO_H   
            
// End of File
