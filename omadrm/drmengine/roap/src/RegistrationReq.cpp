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
#include "RegistrationReq.h"

using namespace Roap;

// LOCAL CONSTANTS AND MACROS
_LIT8(KReqHeader, "<roap:registrationRequest xmlns:roap=\"urn:oma:bac:dldrm:roap-1.0\"\
 sessionId=\"");
_LIT8(KReqSessionIdEnd, "\"");
_LIT8(KReqNonceTrigger, " triggerNonce=\"");
_LIT8(KReqNonceTriggerEnd, "\"");
_LIT8(KReqHeaderEnd, ">");
_LIT8(KReqNonce, "<nonce>");
_LIT8(KReqNonceEnd, "</nonce>");
_LIT8(KReqTime, "<time>");
_LIT8(KReqTimeEnd, "</time>");
_LIT8(KReqCertChain, "<certificateChain>");
_LIT8(KReqCert, "<certificate>");
_LIT8(KReqCertEnd, "</certificate>");
_LIT8(KReqCertChainEnd, "</certificateChain>");
_LIT8(KReqTrustedAuthorities,"<trustedAuthorities>");
_LIT8(KReqTrustedRoot, "<keyIdentifier xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
 algorithm=\"http://www.w3.org/2000/09/xmldsig#sha1\" xsi:type=\"roap:X509SPKIHash\"><hash>");
_LIT8(KReqTrustedRootEnd, "</hash></keyIdentifier>");
_LIT8(KReqTrustedAuthoritiesEnd, "</trustedAuthorities>");
_LIT8(KReqServerInfo, "<serverInfo>");
_LIT8(KReqServerInfoEnd, "</serverInfo>");
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
_LIT8(KReqDevDetails, "<extension xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
 xsi:type=\"roap:DeviceDetails\"><manufacturer>");
_LIT8(KReqDevDetailsModel, "</manufacturer><model>");
_LIT8(KReqDevDetailsVersion, "</model><version>");
_LIT8(KReqDevDetailsEnd, "</version></extension>");
_LIT8(KReqSig, "<signature>");
_LIT8(KReqSigEnd, "</signature></roap:registrationRequest>");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRegistrationReq::CRegistrationReq
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRegistrationReq::CRegistrationReq():
    iSession(NULL),
    iNonce(NULL),
    iServerInfo(NULL),
    iSignature(NULL),
    iDeviceDetailsManufacturer(NULL),
    iDeviceDetailsModel(NULL),
    iDeviceDetailsVersion(NULL),
    iOcspResponderKeyId(NULL),
    iTriggerNonce(NULL)
    {
    }

// -----------------------------------------------------------------------------
// CRegistrationReq::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRegistrationReq::ConstructL()
    {
    iTime.UniversalTime();
    iPeerKeyIdentifier.SetLength(0);
    iOcspInfoStored = EFalse;
    }

// -----------------------------------------------------------------------------
// CRegistrationReq::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRegistrationReq* CRegistrationReq::NewL()
    {
    CRegistrationReq* self = new( ELeave ) CRegistrationReq;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CRegistrationReq::~CRegistrationReq()
    {
    iCertificateChain.ResetAndDestroy();
    delete iSession;
    delete iSignature;
    delete iDeviceDetailsManufacturer;
    delete iDeviceDetailsModel;
    delete iDeviceDetailsVersion;
    delete iTriggerNonce;
    delete iServerInfo;
    delete iOcspResponderKeyId;
    iTrustedAuthorities.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CRegistrationReq::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CRegistrationReq::MessageAsXmlL(void)
    {
    HBufC8* r = NULL;
    CBufFlat* b = NULL;
    TInt i;

    b = CBufFlat::NewL(128);
    CleanupStack::PushL(b);
    BufAppendL(b, KReqHeader);
    BufAppendL(b, *iSession);
    BufAppendL(b, KReqSessionIdEnd);
    if (iTriggerNonce)
        {
        BufAppendL(b, KReqNonceTrigger);
        BufAppendL(b, *iTriggerNonce);
        BufAppendL(b, KReqNonceTriggerEnd);
        }
    BufAppendL(b, KReqHeaderEnd);

    BufAppendL(b, KReqNonce);
    BufAppendBase64L(b, iNonce);
    BufAppendL(b, KReqNonceEnd);

    BufAppendL(b, KReqTime);
    BufAppendTimeL(b, iTime);
    BufAppendL(b, KReqTimeEnd);

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

    if (iTrustedAuthorities.Count() > 0)
        {
        BufAppendL(b, KReqTrustedAuthorities);
        for (i = 0; i < iTrustedAuthorities.Count(); i++)
            {
            BufAppendL(b, KReqTrustedRoot);
            BufAppendBase64L(b, *iTrustedAuthorities[i]);
            BufAppendL(b, KReqTrustedRootEnd);
            }
        BufAppendL(b, KReqTrustedAuthoritiesEnd);
        }

    if (iServerInfo)
        {
        BufAppendL(b, KReqServerInfo);
        BufAppendL(b, *iServerInfo);
        BufAppendL(b, KReqServerInfoEnd);
        }

    if ((iDeviceDetailsManufacturer &&
         iDeviceDetailsModel &&
         iDeviceDetailsVersion) ||
         iPeerKeyIdentifier.Length() ||
         iOcspResponderKeyId ||
         iOcspInfoStored )
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

    if (iDeviceDetailsManufacturer != NULL &&
        iDeviceDetailsModel != NULL &&
        iDeviceDetailsVersion != NULL)
        {
        BufAppendL(b, KReqDevDetails);
        BufAppendL(b, *iDeviceDetailsManufacturer);
        BufAppendL(b, KReqDevDetailsModel);
        BufAppendL(b, *iDeviceDetailsModel);
        BufAppendL(b, KReqDevDetailsVersion);
        BufAppendL(b, *iDeviceDetailsVersion);
        BufAppendL(b, KReqDevDetailsEnd);
        }

    if ((iDeviceDetailsManufacturer &&
         iDeviceDetailsModel &&
         iDeviceDetailsVersion) ||
         iPeerKeyIdentifier.Length() ||
         iOcspResponderKeyId ||
         iOcspInfoStored )
        {
        BufAppendL(b, KReqExtensionsEnd);
        }

    BufAppendL(b, KReqSig);
    BufAppendL(b, KReqSigEnd);

    r = b->Ptr(0).AllocL();
    CleanupStack::PopAndDestroy(b);
    return r;
    }

//  End of File
