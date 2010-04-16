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
* Description:  Class representing metering report request
*
*/



// INCLUDE FILES
#include <e32std.h>
#include "Base64.h"
#include "MeteringReportReq.h"
#include "RoapLog.h"

using namespace Roap;

// LOCAL CONSTANTS AND MACROS
// XML tag related literals
// Warning: these literals are very fragile in respect to XML canonicalisation.
_LIT8(KReqHeader, "<roap:meteringReportSubmit xmlns:roap=\"urn:oma:bac:dldrm:roap-1.0\"");
_LIT8(KReqNonceTrigger, " triggerNonce=\"");
_LIT8(KReqNonceTriggerEnd, "\"");
_LIT8(KReqHeaderEnd, ">");
_LIT8(KReqDeviceId, "<deviceID><keyIdentifier xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:type=\"roap:X509SPKIHash\"><hash>");
_LIT8(KReqDeviceIdEnd, "</hash></keyIdentifier></deviceID>");
_LIT8(KReqRiId, "<riID><keyIdentifier xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:type=\"roap:X509SPKIHash\"><hash>");
_LIT8(KReqRiIdEnd, "</hash></keyIdentifier></riID>");
_LIT8(KReqNonce, "<nonce>");
_LIT8(KReqNonceEnd, "</nonce>");
_LIT8(KReqTime, "<time>");
_LIT8(KReqTimeEnd, "</time>");
_LIT8(KReqMeteringReport, "<meteringReport>");

_LIT8(KReqReportHeader,
"<encryptedMeteringReport>\
<xenc:EncryptedData xmlns:xenc=\"http://www.w3.org/2001/04/xmlenc#\"\
 Type=\"http://www.w3.org/2001/04/xmlenc#Content\">\
<xenc:EncryptionMethod\
 Algorithm=\"http://www.w3.org/2001/04/xmlenc#aes128-cbc\">\
</xenc:EncryptionMethod>");

_LIT8(KReqKeyInfo, "<ds:KeyInfo xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#\">\
<ds:RetrievalMethod URI=\"#K_MEK_and_K_MAC\"></ds:RetrievalMethod>\
</ds:KeyInfo>");

_LIT8(KReqCipherDataStart, "<xenc:CipherData><xenc:CipherValue>");
_LIT8(KReqCipherDataEnd, "</xenc:CipherValue></xenc:CipherData></xenc:EncryptedData></encryptedMeteringReport>");

_LIT8(KReqEncKeyAndMethod, "<encKey Id=\"K_MEK_and_K_MAC\"><xenc:EncryptionMethod\
 xmlns:xenc=\"http://www.w3.org/2001/04/xmlenc#\" Algorithm=\"" );

_LIT8(KReqEncKeyAndMethodAfterAlgorithm, "\"></xenc:EncryptionMethod>");

_LIT8(KReqEncKeyInfoStart, "<ds:KeyInfo xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#\">");

// Note the two versions below are because of different namespace
// normalisation of element roap:X509SPKIHash on canonicalisation of
// whole request to be signed and on canonicalisation of element
// <meteringReport> to be MAC calculated.
// when changing one of these another one must be cahnges too.
//
// This version is used when canonicalisation is performed over
// <roap:meteringReportSubmit>
//
_LIT8(KReqRoapX509AndHash, "<roap:X509SPKIHash><hash>");
//
//
// This version is used when canonicalisation is taken over <meteringReport>
// (on MAC calculation)
//
_LIT8(KReqRoapX509AndHashNs, "<roap:X509SPKIHash xmlns:roap=\"urn:oma:bac:dldrm:roap-1.0\"><hash>");


_LIT8(KReqEncKeyInfoEnd, "</hash></roap:X509SPKIHash></ds:KeyInfo>\
<xenc:CipherData xmlns:xenc=\"http://www.w3.org/2001/04/xmlenc#\"><xenc:CipherValue>");

_LIT8(KReqCipherValueEnd, "</xenc:CipherValue></xenc:CipherData></encKey>");
_LIT8(KReqMacStart, "<mac>");
_LIT8(KReqMacEnd, "</mac>");
_LIT8(KReqReportHeaderEnd, "</meteringReport>");
_LIT8(KReqCertChain, "<certificateChain>");
_LIT8(KReqCert, "<certificate>");
_LIT8(KReqCertEnd, "</certificate>");
_LIT8(KReqCertChainEnd, "</certificateChain>");
_LIT8(KReqSig, "<signature>");
_LIT8(KReqSigEnd, "</signature></roap:meteringReportSubmit>");
// literals for key transport scheme
_LIT8( KOmaKdf, "http://www.rsasecurity.com/rsalabs/pkcs/schemas/pkcs-1#rsaes-kem-kdf2-kw-aes128");
_LIT8( KCmlaIp1, "http://www.cm-la.com/tech/cmlaip/cmlaip#cmlaip-1");
// ============================ LOCAL FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// MeteringReportElementAsTextL
//
// write content of to given flat buffer
// Used from MessageAsXmlL and from InsertMacL
// Note content depends on value of
// ----------------------------------------------------------------------------
LOCAL_C inline void MeteringReportElementAsTextL(
    CBufFlat*& aBuf,
    const Roap::CMeteringReportReq& aReq
    )
    {
    Roap::CRoapMessage::BufAppendL( aBuf, KReqMeteringReport );
    Roap::CRoapMessage::BufAppendL( aBuf, KReqReportHeader );

    Roap::CRoapMessage::BufAppendL( aBuf, KReqKeyInfo );

    Roap::CRoapMessage::BufAppendL( aBuf, KReqCipherDataStart );

    // actual encrypted metering data
    if ( aReq.iCipherValue )
        {
        Roap::CRoapMessage::BufAppendBase64L( aBuf, *aReq.iCipherValue );
        }

    Roap::CRoapMessage::BufAppendL( aBuf, KReqCipherDataEnd );

    Roap::CRoapMessage::BufAppendL( aBuf, KReqEncKeyAndMethod );

    // Select KDF algorithm in use
    if ( aReq.iAlgorithmInUse == ECmlaIp1 )
        {
        Roap::CRoapMessage::BufAppendL( aBuf, KCmlaIp1 );
        }
    else if ( aReq.iAlgorithmInUse == EOma )
        {
        Roap::CRoapMessage::BufAppendL( aBuf, KOmaKdf );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    Roap::CRoapMessage::BufAppendL( aBuf, KReqEncKeyAndMethodAfterAlgorithm );
    Roap::CRoapMessage::BufAppendL( aBuf, KReqEncKeyInfoStart );

    if ( aReq.iMac )
        {
        // used as part of roap:Meteringreport submit
        // so namespace ROAP already defined in container element
        Roap::CRoapMessage::BufAppendL( aBuf, KReqRoapX509AndHash );
        }
    else
        {
        // used in mac calculation
        // canonical form requires namespace definition for namespace roap
        Roap::CRoapMessage::BufAppendL( aBuf, KReqRoapX509AndHashNs );
        }

    // Insert 128-bit encrypted encryption key PKI hash
    Roap::CRoapMessage::BufAppendBase64L( aBuf, aReq.iEncKeyHash );


    Roap::CRoapMessage::BufAppendL( aBuf, KReqEncKeyInfoEnd );

    // key wrapping info
    Roap::CRoapMessage::BufAppendBase64L( aBuf, *aReq.iEncryptedMekAndMak );

    Roap::CRoapMessage::BufAppendL( aBuf, KReqCipherValueEnd );

    // Insert 128-bit encrypted MAC value
    if ( aReq.iMac )
        {
        Roap::CRoapMessage::BufAppendL( aBuf, KReqMacStart );
        Roap::CRoapMessage::BufAppendBase64L( aBuf, *aReq.iMac );
        Roap::CRoapMessage::BufAppendL( aBuf, KReqMacEnd );
        }
    Roap::CRoapMessage::BufAppendL( aBuf, KReqReportHeaderEnd );
    }

// -----------------------------------------------------------------------------
// CalculateMacL
// -----------------------------------------------------------------------------
//
LOCAL_C HBufC8* CalculateMacL(const TDesC8& aElement, const TDesC8& aMacKey )
    {

    LOG( _L8( "CalculateMacL" ) );
    LOG( _L8( "aMacKey" ) );
    LOGHEX( aMacKey.Ptr(), aMacKey.Length() );
    LOG( _L8( "aElement" ) );
    LOGHEX( aElement.Ptr(), aElement.Length() );
    if( !aMacKey.Length() || !aElement.Length() )
        {
        User::Leave(KErrArgument);
        }

    CHMAC* hMac = NULL;
    CSHA1* sha = NULL;
    TPtrC8 hmac_value( KNullDesC8 );
    TPtrC8 sha1_value( KNullDesC8 );
    HBufC8* macValue = NULL;

    sha = CSHA1::NewL();
    CleanupStack::PushL( sha );
    hMac = CHMAC::NewL( aMacKey, sha );
    CleanupStack::Pop( sha ); // sha is now owned by hMac
    CleanupStack::PushL( hMac );
    hMac->Update( aElement );
    hmac_value.Set( hMac->Final() );

    macValue = hmac_value.AllocL();
    LOG( _L8( "macValue" ) );
    LOGHEX( macValue->Ptr(), macValue->Length() );

    CleanupStack::PopAndDestroy( hMac );
    return macValue;
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRightsReq::CRightsReq
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMeteringReportReq::CMeteringReportReq():
    iNonce(NULL),
    iReportNonce(NULL),
    iSignature(NULL),
    iCipherValue(NULL),
    iMac(NULL),
    iTriggerNonce(NULL)
    {
    }

// -----------------------------------------------------------------------------
// CRightsReq::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMeteringReportReq::ConstructL()
    {
    iTime.UniversalTime();
    }

// -----------------------------------------------------------------------------
// CRightsReq::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMeteringReportReq* CMeteringReportReq::NewL()
    {
    CMeteringReportReq* self = new( ELeave ) CMeteringReportReq;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CMeteringReportReq::~CMeteringReportReq()
    {
    if ( iCertificateChain.Count() )
        {
        iCertificateChain.ResetAndDestroy();
        }
    delete iSignature;
    delete iTriggerNonce;
    delete iCipherValue;
    delete iEncryptedMekAndMak;
    delete iMac;
    }


// -----------------------------------------------------------------------------
// CRightsReq::MessageAsXmlL
// Gives XML representation of meteringreport request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CMeteringReportReq::MessageAsXmlL(void)
    {
    HBufC8* r( NULL );
    CBufFlat* b( CBufFlat::NewL( 128 ) );
    CleanupStack::PushL( b );
    b->InsertL( 0, KReqHeader );
    if ( iTriggerNonce )
        {
        BufAppendL( b, KReqNonceTrigger );
        BufAppendL( b, *iTriggerNonce );
        BufAppendL( b, KReqNonceTriggerEnd );
        }
    BufAppendL( b, KReqHeaderEnd );

    BufAppendL( b, KReqDeviceId );
    BufAppendBase64L( b, iDeviceId );
    BufAppendL( b, KReqDeviceIdEnd );

    BufAppendL( b, KReqRiId );
    BufAppendBase64L( b, iRiId );
    BufAppendL( b, KReqRiIdEnd );

    BufAppendL( b, KReqNonce );
    BufAppendBase64L( b, iNonce );
    BufAppendL( b, KReqNonceEnd );

    BufAppendL( b, KReqTime );
    BufAppendTimeL( b, iTime );
    BufAppendL( b, KReqTimeEnd );

    MeteringReportElementAsTextL( b, *this );

    if ( iCertificateChain.Count() > 0 )
        {
        BufAppendL( b, KReqCertChain );
        for ( TInt i( 0 ); i < iCertificateChain.Count(); i++ )
            {
            BufAppendL( b, KReqCert );
            BufAppendBase64L( b, *iCertificateChain[ i ] );
            BufAppendL( b, KReqCertEnd );
            }
        BufAppendL( b, KReqCertChainEnd );
        }

    BufAppendL(b, KReqSig);
    BufAppendL(b, KReqSigEnd);

    r = b->Ptr( 0 ).AllocL();
    CleanupStack::PopAndDestroy( b );
    return r;
    }

// -----------------------------------------------------------------------------
// CMeteringReportReq::InsertMacL( const TDesC8& aMac )
// Calculate MAC -value over <encryptedMeteringReport>
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMeteringReportReq::InsertMacL( const TDesC8& aMacKey )
    {
    CBufFlat* b( CBufFlat::NewL( 128 ) );
    CleanupStack::PushL( b );


    MeteringReportElementAsTextL( b, *this );

    // Calculate MAC -value over <encryptedMeteringReport>
    // without the actual <mac> -element

    iMac = CalculateMacL( b->Ptr( 0 ), aMacKey );
    CleanupStack::PopAndDestroy( b );
    return;
    }

//  End of File
