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
#include "LeaveDomainReq.h"

using namespace Roap;

// LOCAL CONSTANTS AND MACROS
_LIT8(KReqHeader, "<roap:leaveDomainRequest xmlns:roap=\"urn:oma:bac:dldrm:roap-1.0\"");
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
_LIT8(KReqNotDomainMember, "<extensions><extension xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
 xsi:type=\"roap:NotDomainMember\"></extension></extensions>");
_LIT8(KReqSig, "<signature>");
_LIT8(KReqSigEnd, "</signature></roap:leaveDomainRequest>");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLeaveDomainReq::CLeaveDomainReq
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLeaveDomainReq::CLeaveDomainReq():
    iDomainId(NULL),
    iNonce(NULL),
    iSignature(NULL),
    iTriggerNonce(NULL)
    {
    }

// -----------------------------------------------------------------------------
// CLeaveDomainReq::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLeaveDomainReq::ConstructL()
    {
    iTime.UniversalTime();
    iNotMember = EFalse;
    }

// -----------------------------------------------------------------------------
// CLeaveDomainReq::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLeaveDomainReq* CLeaveDomainReq::NewL()
    {
    CLeaveDomainReq* self = new( ELeave ) CLeaveDomainReq;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CLeaveDomainReq::~CLeaveDomainReq()
    {
    iCertificateChain.ResetAndDestroy();
    delete iDomainId;
    delete iSignature;
    delete iTriggerNonce;
    }


// -----------------------------------------------------------------------------
// CLeaveDomainReq::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CLeaveDomainReq::MessageAsXmlL(void)
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

    if ( iNotMember )
        {
        BufAppendL(b, KReqNotDomainMember);
        }

    BufAppendL(b, KReqSig);
    BufAppendL(b, KReqSigEnd);

    r = b->Ptr(0).AllocL();
    CleanupStack::PopAndDestroy(b);
    return r;
    }
