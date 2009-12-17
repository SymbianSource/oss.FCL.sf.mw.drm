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



// INCLUDE FILES
#include <e32std.h>
#include <asymmetric.h>
#include <symmetric.h>
#include <hash.h>
#include "OmaCrypto.h"
#include "CmlaCrypto.h"
#include "BitStream.h"

// LOCAL CONSTANTS AND MACROS

_LIT8(KOma, "http://www.rsasecurity.com/rsalabs/pkcs/schemas/pkcs-1#rsaes-kem-kdf2-kw-aes128");
_LIT8(KCmlaIp1, "http://www.cm-la.com/tech/cmlaip/cmlaip#cmlaip-1");
_LIT8(KCmlaIp2, "http://www.cm-la.com/tech/cmlaip/cmlaip#cmlaip-2");
_LIT8(KCmlaIp3, "http://www.cm-la.com/tech/cmlaip/cmlaip#cmlaip-3");
_LIT8(KCmlaIp4, "http://www.cm-la.com/tech/cmlaip/cmlaip#cmlaip-4");
_LIT8(KCmlaIp5, "http://www.cm-la.com/tech/cmlaip/cmlaip#cmlaip-5");
_LIT8(KCmlaIp6, "http://www.cm-la.com/tech/cmlaip/cmlaip#cmlaip-6");
_LIT8(KCmlaIp7, "http://www.cm-la.com/tech/cmlaip/cmlaip#cmlaip-7");

// ==================== LOCAL FUNCTIONS ====================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CmlaCrypto::
// 
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// CmlaCrypto::
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CmlaCrypto::SupportedAlgorithmsL(
    RArray<TPtrC8>& /*aAlgorithmList*/)
    {
    return KErrNotSupported;
    }

EXPORT_C HBufC8* CmlaCrypto::DdtPermL(
    const TDesC8& /*aInput*/)
    {
    return NULL;
    }

EXPORT_C HBufC8* CmlaCrypto::DdtPermInvL(
    const TDesC8& /*aInput*/)
    {
    return NULL;
    }

EXPORT_C HBufC8* CmlaCrypto::DdtExpL(
    const TDesC8& /*aInput*/)
    {
    return NULL;
    }

EXPORT_C HBufC8* CmlaCrypto::DdtExpInvL(
    const TDesC8& /*aInput*/)
    {
    return NULL;
    }

EXPORT_C HBufC8* CmlaCrypto::WrapL(
    const TDesC8& /*aKek*/,
    const TDesC8& /*aKey*/)
    {
    return NULL;
    }

EXPORT_C HBufC8* CmlaCrypto::UnwrapL(
    const TDesC8& /*aKek*/,
    const TDesC8& /*aWrap*/)
    {
    return NULL;
    }

EXPORT_C HBufC8* CmlaCrypto::KdfL(
    const TDesC8& /*aInput*/)
    {
    return NULL;
    }

EXPORT_C HBufC8* CmlaCrypto::RsaDecryptCmlaL(
    MDrmKeyStorage* /*aKeyStorage*/,
    const TDesC8& /*aInput*/)
    {
    return NULL;
    }

EXPORT_C HBufC8* CmlaCrypto::RsaEncryptCmlaL(
    CRSAPublicKey* /*aKey*/,
    const TDesC8& /*aInput*/)
    {
    return NULL;
    }

EXPORT_C void CmlaCrypto::CmlaIpDecryptL(
    TKeyTransportScheme /*aTransportScheme*/,
    MDrmKeyStorage* /*aKeyStorage*/,
    const TDesC8& /*aInput*/,
    TDes8& /*aRek*/,
    TDes8& /*aMac*/)
    {
    User::Leave(KErrNotSupported);
    }

EXPORT_C HBufC8* CmlaCrypto::CmlaIpEncryptL(
    TKeyTransportScheme /*aTransportScheme*/,
    CRSAPublicKey* /*aKey*/,
    const TDesC8& /*aRek*/,
    const TDesC8& /*aMac*/)
    {
    return NULL;
    }

EXPORT_C TKeyTransportScheme CmlaCrypto::AlgorithmIdToTransportScheme(
    const TDesC8& aAlgorithmId)
    {
    if (aAlgorithmId.CompareF(KOma) == 0)
        {
        return EOma;
        }
    if (aAlgorithmId.CompareF(KCmlaIp1) == 0)
        {
        return ECmlaIp1;
        }
    if (aAlgorithmId.CompareF(KCmlaIp2) == 0)
        {
        return ECmlaIp2;
        }
    if (aAlgorithmId.CompareF(KCmlaIp3) == 0)
        {
        return ECmlaIp3;
        }
    if (aAlgorithmId.CompareF(KCmlaIp4) == 0)
        {
        return ECmlaIp4;
        }
    if (aAlgorithmId.CompareF(KCmlaIp5) == 0)
        {
        return ECmlaIp5;
        }
    if (aAlgorithmId.CompareF(KCmlaIp6) == 0)
        {
        return ECmlaIp6;
        }
    if (aAlgorithmId.CompareF(KCmlaIp7) == 0)
        {
        return ECmlaIp7;
        }
    return EOma;
    }
    
//  End of File  
