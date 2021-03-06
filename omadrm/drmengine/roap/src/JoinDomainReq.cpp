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
#include "Base64.h"
#include "JoinDomainReq.h"
#include "RoapDef.h"

using namespace Roap;

// LOCAL CONSTANTS AND MACROS
_LIT8(KReqHeader, "<roap:joinDomainRequest xmlns:roap=\"urn:oma:bac:dldrm:roap-1.0\"");
_LIT8(KReqNonceTrigger, " triggerNonce=\"");
_LIT8(KReqNonceTriggerEnd, "\"");
_LIT8(KReqHeaderEnd, ">");
_LIT8(KReqNonce, "<nonce>");
_LIT8(KReqNonceEnd, "</nonce>");
_LIT8(KReqTime, "<time>");
_LIT8(KReqTimeEnd, "</time>");
_LIT8(KReqDeviceId, "<deviceID><keyIdentifier xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
 algorithm=\"http://www.w3.org/2000/09/xmldsig#sha1\" xsi:type=\"roap:X509SPKIHash\"><hash>");
_LIT8(KReqDeviceIdEnd, "</hash></keyIdentifier></deviceID>");
_LIT8(KReqRiId, "<riID><keyIdentifier xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
 algorithm=\"http://www.w3.org/2000/09/xmldsig#sha1\" xsi:type=\"roap:X509SPKIHash\"><hash>");
_LIT8(KReqRiIdEnd, "</hash></keyIdentifier></riID>");
_LIT8(KReqDomainId, "<domainID>");
_LIT8(KReqDomainIdEnd, "</domainID>");
_LIT8(KReqCertChain, "<certificateChain>");
_LIT8(KReqCert, "<certificate>");
_LIT8(KReqCertEnd, "</certificate>");
_LIT8(KReqCertChainEnd, "</certificateChain>");
_LIT8(KReqExtensions, "<extensions>");
_LIT8(KReqExtensionsEnd, "</extensions>");
_LIT8(KReqPeerKey, "<extension xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
 xsi:type=\"roap:PeerKeyIdentifier\">\
<identifier algorithm=\"http://www.w3.org/2000/09/xmldsig#sha1\" xsi:type=\"roap:X509SPKIHash\"><hash>");
_LIT8(KReqPeerKeyEnd, "</hash></identifier></extension>");
_LIT8(KReqNoOcspResponse, "<extension xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
 xsi:type=\"roap:NoOCSPResponse\"></extension>");
_LIT8(KReqOcspResponderKeyId, "<extension xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
 xsi:type=\"roap:OCSPResponderKeyIdentifier\">\
<identifier algorithm=\"http://www.w3.org/2000/09/xmldsig#sha1\" xsi:type=\"roap:X509SPKIHash\"><hash>");
_LIT8(KReqOcspResponderKeyIdEnd, "</hash></identifier></extension>");
_LIT8(KReqHashChainSupport, "<extension xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
 xsi:type=\"roap:HashChainSupport\"></extension>");
_LIT8(KReqSig, "<signature>");
_LIT8(KReqSigEnd, "</signature></roap:joinDomainRequest>");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CJoinDomainReq::CJoinDomainReq
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CJoinDomainReq::CJoinDomainReq():
    iDomainId(NULL),
    iOcspResponderKeyId(NULL),
    iSignature(NULL),
    iTriggerNonce(NULL)
    {
    }

// -----------------------------------------------------------------------------
// CJoinDomainReq::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CJoinDomainReq::ConstructL()
    {
    iTime.UniversalTime();
    iPeerKeyIdentifier.SetLength(0);
    iOcspInfoStored = EFalse;
    iHashChainSupport = ETrue;
    }

// -----------------------------------------------------------------------------
// CJoinDomainReq::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CJoinDomainReq* CJoinDomainReq::NewL()
    {
    CJoinDomainReq* self = new( ELeave ) CJoinDomainReq;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CJoinDomainReq::~CJoinDomainReq()
    {
    iCertificateChain.ResetAndDestroy();
    delete iDomainId;
    delete iSignature;
    delete iTriggerNonce;
    delete iOcspResponderKeyId;
    }


// -----------------------------------------------------------------------------
// CJoinDomainReq::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CJoinDomainReq::MessageAsXmlL(void)
    {
    HBufC8* r = NULL;
    CBufFlat* b = NULL;
    TInt i;

    b = CBufFlat::NewL(128);
    CleanupStack::PushL(b);
    BufAppendL(b, KReqHeader);
    if (iTriggerNonce)
        {
        BufAppendL(b, KReqNonceTrigger);
        BufAppendL(b, *iTriggerNonce);
        BufAppendL(b, KReqNonceTriggerEnd);
        }
    BufAppendL(b, KReqHeaderEnd);

    BufAppendL(b, KReqDeviceId);
    BufAppendBase64L(b, iDeviceId);
    BufAppendL(b, KReqDeviceIdEnd);

    BufAppendL(b, KReqRiId);
    BufAppendBase64L(b, iRiId);
    BufAppendL(b, KReqRiIdEnd);

    BufAppendL(b, KReqNonce);
    BufAppendBase64L(b, iNonce);
    BufAppendL(b, KReqNonceEnd);

    BufAppendL(b, KReqTime);
    BufAppendTimeL(b, iTime);
    BufAppendL(b, KReqTimeEnd);

    BufAppendL(b, KReqDomainId);
    BufAppendL(b, *iDomainId);
    BufAppendL(b, KReqDomainIdEnd);

    if (iCertificateChain.Count() > 0)
        {
        BufAppendL(b, KReqCertChain);
        for (i = 0; i < iCertificateChain.Count(); i++)
            {
            BufAppendL(b, KReqCert);
            BufAppendBase64L(b, *iCertificateChain[i]);
            BufAppendL(b, KReqCertEnd);
            }
        BufAppendL(b, KReqCertChainEnd);
        }

    if (iPeerKeyIdentifier.Length() ||
        iOcspResponderKeyId ||
        iOcspInfoStored ||
        iHashChainSupport)
        {
        BufAppendL(b, KReqExtensions);
        }

    if (iPeerKeyIdentifier.Length())
        {
        BufAppendL(b, KReqPeerKey);
        BufAppendBase64L(b, iPeerKeyIdentifier);
        BufAppendL(b, KReqPeerKeyEnd);
        }

    if (iOcspInfoStored)
        {
        BufAppendL(b, KReqNoOcspResponse);
        }

    if (iOcspResponderKeyId)
        {
        BufAppendL(b, KReqOcspResponderKeyId);
        BufAppendBase64L(b, *iOcspResponderKeyId);
        BufAppendL(b, KReqOcspResponderKeyIdEnd);
        }

    if (iHashChainSupport)
        {
        BufAppendL(b, KReqHashChainSupport);
        }

    if (iPeerKeyIdentifier.Length() ||
        iOcspResponderKeyId ||
        iOcspInfoStored ||
        iHashChainSupport)
        {
        BufAppendL(b, KReqExtensionsEnd);
        }

    BufAppendL(b, KReqSig);
    BufAppendL(b, KReqSigEnd);

    r = b->Ptr(0).AllocL();
    CleanupStack::PopAndDestroy(b);
    return r;
    }
