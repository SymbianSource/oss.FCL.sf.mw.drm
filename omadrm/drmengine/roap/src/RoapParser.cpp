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
* Description:  ROAP response parser
*
*/



// INCLUDE FILES
#include <e32base.h>

#include "DeviceHello.h"
#include "JoinDomainReq.h"
#include "JoinDomainResp.h"
#include "LeaveDomainReq.h"
#include "LeaveDomainResp.h"
#ifdef RD_DRM_METERING
#include "MeteringReportReq.h"
#include "MeteringReportResp.h"
#include "MeteringReportRespParser.h"
#endif
#include "RIHello.h"
#include "RegistrationReq.h"
#include "RegistrationResp.h"
#include "RightsReq.h"
#include "RightsResp.h"
#include "RoapTrigger.h"
#include "RoapMessage.h"

#include "RoapParser.h"
#include "RespParser.h"
#include "JoinDomainRespParser.h"
#include "LeaveDomainRespParser.h"
#include "RIHelloParser.h"
#include "RegistrationRespParser.h"
#include "RightsRespParser.h"
#include "RoapTriggerParser.h"
#include "RoapLog.h"

using namespace Roap;

// LOCAL CONSTANTS AND MACROS
#define ELEMENT_COUNT(x) static_cast<TInt>((sizeof(x) / sizeof (x[0])))

const TInt KParserChunkSize = 512;
const TInt KRoapElementCount = ELast;
const TInt KMaxElementNesting = 24;

struct TRoapElements
    {
    const TText8* iString;
    TInt iNumber;
    };

struct TStackState
    {
    TParserStackState iState;
    TRoapElementEnum iStack[KMaxElementNesting];
    };

static const TRoapElements KStatusValues[] =
    {
    {_S8("Success"), ESuccess},
    {_S8("UnknownError"), EUnknownError},
    {_S8("Abort"), EAbort},
    {_S8("NotSupported"), ENotSupported},
    {_S8("AccessDenied"), Roap::EAccessDenied},
    {_S8("NotFound"), ENotFound},
    {_S8("MalformedRequest"), EMalformedRequest},
    {_S8("UnknownRequest"), EUnknownRequest},
    {_S8("UnknownCriticalExtension"), EUnknownCriticalExtension},
    {_S8("UnsupportedVersion"), EUnsupportedVersion},
    {_S8("UnsupportedAlgorithm"), EUnsupportedAlgorithm},
    {_S8("NoCertificateChain"), ENoCertificateChain},
    {_S8("InvalidCertificateChain"), EInvalidCertificateChain},
    {_S8("TrustedRootCertificateNotPresent"), ETrustedRootCertificateNotPresent},
    {_S8("SignatureError"), ESignatureError},
    {_S8("DeviceTimeError"), EDeviceTimeError},
    {_S8("NotRegistered"), ENotRegistered},
    {_S8("InvalidDCFHash"), EInvalidDCFHash},
    {_S8("InvalidDomain"), EInvalidDomain},
    {_S8("DomainFull"), EDomainFull},
    {_S8("MalformedMeteringReport"), EMalformedMeteringReport},
    {_S8("UnableToDecryptMeteringReport"), EUnableToDecryptMeteringReport},
    {_S8("UnableToValidateMeteringReportMAC"), EUnableToValidateMeteringReportMAC},
    };

static const TRoapElements KRoapElements[ELast] =
    {
    {_S8("certificate"), ECertificate},
    {_S8("certificateChain"), ECertificateChain},
    {_S8("CipherData"), ECipherData},
    {_S8("CipherValue"), ECipherValue},
    {_S8("contentID"), Roap::EContentID},
    {_S8("dcfHash"), EDcfHash},
    {_S8("deviceDetails"), EDeviceDetails},
    {_S8("deviceID"), EDeviceID},
    {_S8("DigestValue"), EDigestValue},
    {_S8("dn"), EDomainName},
    {_S8("domainAlias"), EDomainAlias},
    {_S8("domainID"), EDomainID},
    {_S8("domainInfo"), EDomainInfo},
    {_S8("domainKey"), EDomainKey},
    {_S8("encKey"), EEncKey},
    {_S8("EncryptionMethod"), EEncryptionMethod},
    {_S8("extension"), EExtension},
    {_S8("extensions"), EExtensions},
    {_S8("hash"), EHash},
    {_S8("id"), EId},
    {_S8("identifier"), EIdentifier},
    {_S8("joinDomain"), EJoinDomain},
    {_S8("joinDomainResponse"), EJoinDomainResponse},
    {_S8("keyIdentifier"), EKeyIdentifier},
    {_S8("leaveDomain"), ELeaveDomain},
    {_S8("leaveDomainResponse"), ELeaveDomainResponse},
    {_S8("mac"), EMac},
    {_S8("manufacturer"), EManufacturer},
    {_S8("model"), EModel},
    {_S8("nonce"), ENonce},
    {_S8("notAfter"), ENotAfter},
    {_S8("ocspResponse"), EOcspResponse},
    {_S8("protectedRO"), EProtectedRO},
    {_S8("registrationRequest"), ERegistrationRequest},
    {_S8("registrationResponse"), ERegistrationResponse},
    {_S8("riAlias"), ERiAlias},
    {_S8("rights"), Roap::ERights},
    {_S8("riHello"), ERiHello},
    {_S8("riID"), ERiID},
    {_S8("riNonce"), ERiNonce},
    {_S8("riURL"), ERiURL},
    {_S8("ro"), ERo},
    {_S8("roAcquisition"), ERoAcquisition},
    {_S8("roapTrigger"), ERoapTrigger},
    {_S8("roapURL"), ERoapURL},
    {_S8("roID"), ERoID},
    {_S8("roInfo"), ERoInfo},
    {_S8("roResponse"), ERoResponse},
    {_S8("selectedAlgorithm"), ESelectedAlgorithm},
    {_S8("selectedVersion"), ESelectedVersion},
    {_S8("serverInfo"), EServerInfo},
    {_S8("signature"), ESignature},
    {_S8("SignatureValue"), ESignatureValue},
    {_S8("supportedAlgorithm"), ESupportedAlgorithm},
    {_S8("time"), ETime},
    {_S8("timeStamp"), ETimeStamp},
    {_S8("trustedAuthorities"), ETrustedAuthorities},
    {_S8("version"), EVersion},
    {_S8("X509SPKIHash"), EX509SPKIHash},
#ifdef RD_DRM_METERING
    {_S8("extendedTrigger"), EExtendedTriggerElement},
    {_S8("meteringReport"), EMeteringReportRequest},
    {_S8("meteringReportResponse"), EMeteringReportResponse},
    {_S8("prURL"), EPrUrl}
#endif
    };

// The stack states have to be sorted with descending length. The
// MatchStackState function searches for the first matching state.
static const TStackState KParserStackStates[] =
    {
    // 4 elements --------------------------------------------------------------
    {ERiIdRoResponseState, {EHash, EKeyIdentifier, ERiID, ERoResponse, ELast}},
    {ERiIdJoinDomainResponseState, {EHash, EKeyIdentifier, ERiID, EJoinDomainResponse, ELast}},
    // 3 elements --------------------------------------------------------------
#ifdef RD_DRM_METERING
    {EPostResponseUrlState, {EPrUrl, EExtension, EExtensions, ELast}},
#endif
    {EDeviceIdState, {EHash, EKeyIdentifier, EDeviceID, ELast}},
    {ETrustedAuthoritiesState, {EHash, EKeyIdentifier, ETrustedAuthorities, ELast}},
    {EPeerKeyIdentifierRIHelloState, {EIdentifier, EExtension, EExtensions, ELast}},
    {ETransactionIdState, {EId, EExtension, EExtensions, ELast}},
    {ETransactionIdContentIdState, {Roap::EContentID, EExtension, EExtensions, ELast}},
    {EWhiteListState, {EDomainName, EExtension, EExtensions, ELast}},
    {EDeviceDetailsState, {EDeviceDetails, EExtensions, ERiHello, ELast}},
    {ERiIdState, {EHash, EKeyIdentifier, ERiID, ELast}},
    {EEncKeyState, {ECipherValue, ECipherData, EEncKey, ELast}},
    {EDomainKeyEncryptionMethodState, {EEncryptionMethod, EEncKey, EDomainKey, ELast}},
    {EMacJoinDomainResponseState, {EMac, EDomainKey, EDomainInfo, ELast}},
    // 2 elements --------------------------------------------------------------
    {EDomainKeyEncKeyState, {EEncKey, EDomainKey, ELast}},
    {ECertificateState, {ECertificate, ECertificateChain, ELast}},
    {ESignatureRoResponseState, {ESignature, ERoResponse, ELast}},
    {EExtensionState, {EExtension, EExtensions, ELast}},
    // 1 element ---------------------------------------------------------------
    {ERiAliasState, {ERiAlias, ELast}},
    {EDomainAliasState, {EDomainAlias, ELast}},
    {ERiHelloState, {ERiHello, ELast}},
    {ESelectedVersionState, {ESelectedVersion, ELast}},
    {ERiNonceState, {ERiNonce, ELast}},
    {EOcspResponseState, {EOcspResponse, ELast}},
    {ERiUrlState, {ERiURL, ELast}},
    {ESignatureState, {ESignature, ELast}},
    {ERegistrationResponseState, {ERegistrationResponse, ELast}},
    {ERoResponseState, {ERoResponse, ELast}},
    {EJoinDomainResponseState, {EJoinDomainResponse, ELast}},
    {ELeaveDomainResponseState, {ELeaveDomainResponse, ELast}},
    {EDomainIdState, {EDomainID, ELast}},
    {ENonceState, {ENonce, ELast}},
    {ENotAfterState, {ENotAfter, ELast}},
    {EDomainKeyState, {EDomainKey, ELast}},
    {ERoapTriggerState, {ERoapTrigger, ELast}},
    {ERoapUrlState, {ERoapURL, ELast}},
    {ERegistrationRequestState, {ERegistrationRequest, ELast}},
    {ERoAcquisitionState, {ERoAcquisition, ELast}},
    {EJoinDomainState, {EJoinDomain, ELast}},
    {ELeaveDomainState, {ELeaveDomain, ELast}},
    {ERoIdState, {ERoID, ELast}},
    {EContentIdState, {Roap::EContentID, ELast}},
    {ESignatureValueState, {ESignatureValue, ELast}},
    {EDigestValueState, {EDigestValue, ELast}},
    {EServerInfoState, {EServerInfo, ELast}},
    {ESupportedAlgorithmState, {ESupportedAlgorithm, ELast}},
    {ESelectedAlgorithmState, {ESelectedAlgorithm, ELast}},
#ifdef RD_DRM_METERING
    {EExtendedTriggerElementState, {EExtendedTriggerElement, ELast}},
    {EMeteringReportRequestState, {EMeteringReportRequest, ELast}},
    {EMeteringReportResponseState, {EMeteringReportResponse, ELast}}
#endif
    };

_LIT8(KParserMimeType, "text/xml");
_LIT8(KDomainKey, "domainKey");
_LIT8(KMac, "mac>");
_LIT8(KMacEnd, "</mac>");

// ============================ auto_handde helper class =======================
//Auto handle for easening handle release on exceptional exit situations
template<class T> class auto_handle
    {
    public:

    auto_handle() {}
    auto_handle(T aHandle) : iHandle( aHandle ) {}
    auto_handle( auto_handle<T>& aHandle) : iHandle( aHandle.release() ) {}
    ~auto_handle() { iHandle.Close(); }
    const T& operator()() const { return iHandle; }
    T& operator()() { return iHandle; }
    T get() const { return iHandle; }
    T release() { T temp = iHandle; iHandle = 0; return temp; }

    private:
    T iHandle;
    };


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRoapParser::CRoapParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRoapParser::CRoapParser():
    iParser(NULL),
    iResponseParser(NULL)
    {
    }

// -----------------------------------------------------------------------------
// CRoapParser::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRoapParser::ConstructL()
    {
    TInt i;

    iParser = CParser::NewL(KParserMimeType, *this);
    for (i = 0; i < KRoapElementCount; i++)
        {
        TPtrC8 ptr(KRoapElements[i].iString, User::StringLength(
            KRoapElements[i].iString));
        iRoapElements[KRoapElements[i].iNumber] =
            iParser->StringPool().OpenStringL(ptr);
        }
    }

// -----------------------------------------------------------------------------
// CRoapParser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRoapParser* CRoapParser::NewL()
    {
    CRoapParser* self = new( ELeave ) CRoapParser;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CRoapParser::~CRoapParser()
    {
    TInt i;
    for (i = 0; i < ELEMENT_COUNT(KRoapElements); i++)
        {
        iRoapElements[KRoapElements[i].iNumber].Close();
        }
    delete iParser;
    delete iContent;
    }

// -----------------------------------------------------------------------------
// CRoapParser::ParseMeteringRespL
// -----------------------------------------------------------------------------
//
#ifndef RD_DRM_METERING
CMeteringResp* CRoapParser::ParseMeteringRespL( const TDesC8& /*aMessage*/ )
    {
    return NULL;
    }
#else
CMeteringResp* CRoapParser::ParseMeteringRespL( const TDesC8& aMessage )
    {
    LOG( _L("CRoapParser::ParseMeteringRespL") );
    CMeteringResp* r = CMeteringResp::NewL();
    CleanupStack::PushL(r);
    TMeteringRespParser p(r);
    iResponseParser = &p;
    ParseL(aMessage);
    CleanupStack::Pop(r);
    return r;
    }
#endif
// -----------------------------------------------------------------------------
// CRoapParser::ParseJoinDomainRespL
// -----------------------------------------------------------------------------
//
CJoinDomainResp* CRoapParser::ParseJoinDomainRespL(
    const TDesC8& aMessage, RPointerArray<HBufC8>& aDomainKeyElements)
    {
    LOG( _L("CRoapParser::ParseJoinDomainRespL") );

    CJoinDomainResp* r = CJoinDomainResp::NewL();
    CleanupStack::PushL(r);
    TJoinDomainRespParser p(r);
    iResponseParser = &p;
    ParseL(aMessage);
    if ( r->iStatus == ESuccess )
        {
        User::LeaveIfError(
            ExtractDomainKeyElements(aMessage, aDomainKeyElements));
        }
    CleanupStack::Pop(r);
    return r;
    }

// -----------------------------------------------------------------------------
// CRoapParser::ParseLeaveDomainRespL
// -----------------------------------------------------------------------------
//
CLeaveDomainResp* CRoapParser::ParseLeaveDomainRespL(
    const TDesC8& aMessage)
    {
    LOG( _L("CRoapParser::ParseLeaveDomainRespL") );

    CLeaveDomainResp* r = CLeaveDomainResp::NewL();
    CleanupStack::PushL(r);
    TLeaveDomainRespParser p(r);
    iResponseParser = &p;
    ParseL(aMessage);
    CleanupStack::Pop(r);
    return r;
    }

// -----------------------------------------------------------------------------
// CRoapParser::ParseRegistrationResp
// -----------------------------------------------------------------------------
//
CRegistrationResp* CRoapParser::ParseRegistrationRespL(
    const TDesC8& aMessage)
    {
    LOG( _L("CRoapParser::ParseRegistrationRespL") );

    CRegistrationResp* r = CRegistrationResp::NewL();
    CleanupStack::PushL(r);
    TRegistrationRespParser p(r);
    iResponseParser = &p;
    ParseL(aMessage);
    CleanupStack::Pop(r);
    return r;
    }

// -----------------------------------------------------------------------------
// CRoapParser::ParseRightsRespL
// -----------------------------------------------------------------------------
//
CRightsResp* CRoapParser::ParseRightsRespL(
    const TDesC8& aMessage)
    {
    LOG( _L("CRoapParser::ParseRightsRespL") );

    CRightsResp* r = CRightsResp::NewL();
    CleanupStack::PushL(r);
    TRightsRespParser p(r);
    iResponseParser = &p;
    ParseL(aMessage);
    CleanupStack::Pop(r);
    return r;
    }

// -----------------------------------------------------------------------------
// CRoapParser::ParseRIHelloL
// -----------------------------------------------------------------------------
//
CRIHello* CRoapParser::ParseRIHelloL(
    const TDesC8& aMessage)
    {
    LOG( _L("CRoapParser::ParseRIHelloL") );

    CRIHello* r = CRIHello::NewL();
    CleanupStack::PushL(r);
    TRIHelloParser p(r);
    iResponseParser = &p;
    ParseL(aMessage);
    CleanupStack::Pop(r);
    return r;
    }

// -----------------------------------------------------------------------------
// CRoapParser::ParseRoapTriggerL
// -----------------------------------------------------------------------------
//
CRoapTrigger* CRoapParser::ParseRoapTriggerL(
    const TDesC8& aMessage)
    {
    LOG( _L("CRoapParser::ParseRoapTriggerL") );

    CRoapTrigger* r = CRoapTrigger::NewL();
    CleanupStack::PushL(r);
    TRoapTriggerParser p(r);
    iResponseParser = &p;
    ParseL(aMessage);
    CleanupStack::Pop(r);
    return r;
    }

// -----------------------------------------------------------------------------
// CRoapParser::ParseL
// -----------------------------------------------------------------------------
//
void CRoapParser::ParseL(
    const TDesC8& aMessage)
    {
    TInt i;
    TInt n;

    iElementStackDepth = 0;
    delete iContent;
    iContent = NULL;
    iParser->ParseBeginL();
    i = 0;
    while (i < aMessage.Length())
        {
        n = Min(aMessage.Length() - i, KParserChunkSize);
        iParser->ParseL(aMessage.Mid(i, n));
        i += n;
        }
    iParser->ParseEndL();
    }

// -----------------------------------------------------------------------------
// CRoapParser::OnStartDocumentL
// -----------------------------------------------------------------------------
//
void CRoapParser::OnStartDocumentL(
    const RDocumentParameters& /*aDocParam*/,
    TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// CRoapParser::OnEndDocumentL
// -----------------------------------------------------------------------------
//
void CRoapParser::OnEndDocumentL(
    TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// CRoapParser::OnStartElementL
// -----------------------------------------------------------------------------
//
void CRoapParser::OnStartElementL(
    const RTagInfo& aElement,
    const RAttributeArray& aAttributes,
    TInt /*aErrorCode*/)
    {
    TInt i;
    TInt state;

    if (iContent)
        {
        delete iContent;
        iContent = NULL;
        iContent = HBufC8::NewL(0);
        }

    for (i = 0; i < KRoapElementCount; i++)
        {
        if (aElement.LocalName() == iRoapElements[i])
            {
            iElementStack[iElementStackDepth] =
                static_cast<TRoapElementEnum>(i);
            iElementStackDepth++;
            state = MatchStackState();
            iResponseParser->OnStartElementL(*this, state, aElement,
                aAttributes);
            if (iElementStackDepth == KMaxElementNesting)
                {
                User::Leave(EXmlUnexpectedState);
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CRoapParser::OnEndElementL
// -----------------------------------------------------------------------------
//
void CRoapParser::OnEndElementL(
    const RTagInfo& aElement,
    TInt /*aErrorCode*/)
    {
    TInt i;
    TInt state;

    for (i = 0; i < KRoapElementCount; i++)
        {
        if (aElement.LocalName() == iRoapElements[i])
            {
            state = MatchStackState();
            iResponseParser->OnEndElementL(*this, state, aElement);
            iElementStackDepth--;
            if (iElementStackDepth < 0)
                {
                User::Leave(EXmlUnexpectedState);
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CRoapParser::OnContentL
// -----------------------------------------------------------------------------
//
void CRoapParser::OnContentL(
    const TDesC8& aBytes,
    TInt /*aErrorCode*/)
    {
    if ( !iContent )
        {
        iContent = HBufC8::NewL(aBytes.Size());
        *iContent = aBytes;
        }
    else
        {
        iContent = iContent->ReAllocL(iContent->Size() + aBytes.Size());
        TPtr8 c(iContent->Des());
        c.Append(aBytes);
        }
    }

// -----------------------------------------------------------------------------
// CRoapParser::OnStartPrefixMappingL
// -----------------------------------------------------------------------------
//
void CRoapParser::OnStartPrefixMappingL(
    const RString& /*aPrefix*/,
    const RString& /*aUri*/,
    TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// CRoapParser::OnEndPrefixMappingL
// -----------------------------------------------------------------------------
//
void CRoapParser::OnEndPrefixMappingL(
    const RString& /*aPrefix*/,
    TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// CRoapParser::OnIgnorableWhiteSpaceL
// -----------------------------------------------------------------------------
//
void CRoapParser::OnIgnorableWhiteSpaceL(
    const TDesC8& /*aBytes*/,
    TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// CRoapParser::OnSkippedEntityL
// -----------------------------------------------------------------------------
//
void CRoapParser::OnSkippedEntityL(
    const RString& /*aName*/,
    TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// CRoapParser::OnProcessingInstructionL
// -----------------------------------------------------------------------------
//
void CRoapParser::OnProcessingInstructionL(
    const TDesC8& /*aTarget*/,
    const TDesC8& /*aData*/,
    TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// CRoapParser::OnOutOfData
// -----------------------------------------------------------------------------
//
void CRoapParser::OnOutOfData()
    {
    }

// -----------------------------------------------------------------------------
// CRoapParser::OnError
// -----------------------------------------------------------------------------
//
void CRoapParser::OnError(
    TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// CRoapParser::GetExtendedInterface
// -----------------------------------------------------------------------------
//
TAny* CRoapParser::GetExtendedInterface(
    const TInt32 /*aUid*/)
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CRoapParser::MatchStackState
// -----------------------------------------------------------------------------
//
TParserStackState CRoapParser::MatchStackState(void)
    {
    TInt i;
    TInt j;
    TInt k;
    TParserStackState r = EUnknownState;

    for (i = 0;
        r == EUnknownState &&
        i < ELEMENT_COUNT(KParserStackStates); i++)
        {
        for (j = iElementStackDepth - 1, k = 0;
            j > 0 && KParserStackStates[i].iStack[k] != ELast;
            j--, k++)
            {
            if (iElementStack[j] != KParserStackStates[i].iStack[k])
                {
                break;
                }
            }
        if ((j == 0 && iElementStack[j] == KParserStackStates[i].iStack[k]) ||
            KParserStackStates[i].iStack[k] == ELast)
            {
            r = KParserStackStates[i].iState;
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// CRoapParser::GetAttributeValueL
// -----------------------------------------------------------------------------
//
HBufC8* CRoapParser::GetAttributeValueL(
    const RAttributeArray& aAttrList,
    const TDesC8& aAttrName)
    {
    HBufC8* r = NULL;
    RAttribute a;
    TInt i;

    for (i = 0; !r && i < aAttrList.Count(); i++)
        {
        a = aAttrList[i];
        if (a.Attribute().LocalName().DesC().Compare(aAttrName) == 0)
            {
            r = a.Value().DesC().AllocL();
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// CRoapParser::ConvertRoapStatus
// -----------------------------------------------------------------------------
//
TRoapStatus CRoapParser::ConvertRoapStatus(
    const TDesC8& aStatus)
    {
    TInt i;
    TRoapStatus r = EUnknownStatus;

    for (i = 0; r == EUnknownStatus && i < ELEMENT_COUNT(KStatusValues); i++)
        {
        if (aStatus.Compare(TPtrC8(KStatusValues[i].iString,
            aStatus.Size())) == 0)
            {
            r = static_cast<TRoapStatus>(KStatusValues[i].iNumber);
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// CRoapParser::ExtractElement
// Note: uses auto_handle helper class
// -----------------------------------------------------------------------------
//
TPtrC8 CRoapParser::ExtractElement(
    const TDesC8& aMessage, const TDesC8& aElement, TInt& aOffset ) const
    {
    LOG( _L("CRoapParser::ExtractElement") );

    TPtrC8 temp( KNullDesC8 );
    TInt pos = ( 0 );
    TInt startPos = ( 0 );
    TInt endPos = ( 0 );
    TInt ret( 0 );
    TInt startLength ( 0 );
    const TInt KSpaceForDelimiters( 3 ); // max "</" aElement ">"

    auto_handle< RBuf8 > tagToBeFound;
    // Must be nonleaving since this function is nonleaving
    ret = tagToBeFound().Create( aElement.Length() + KSpaceForDelimiters );
    if ( ret != KErrNone )
        {
        aOffset = -1;
        return KNullDesC8();
        }

    //First we try to find start tag (as localname)
    tagToBeFound().SetLength( 0 );
    tagToBeFound().AppendFormat( _L8( "<%S" ), &aElement );

    temp.Set( aMessage.Mid( aOffset ) );


    startPos = temp.Find( tagToBeFound() );

    if ( startPos < 0 )
        {//Not found, let's try with namespace start
        tagToBeFound().SetLength( 0 );
        tagToBeFound().AppendFormat( _L8( ":%S" ), &aElement );
        pos = temp.Find( tagToBeFound() );
        if ( pos < 0 )
            {//Not possible to find valid tag with given name
            aOffset = -1;
            return KNullDesC8();
            }
        temp.Set( aMessage.Left( pos ) );
        startPos = temp.LocateReverse('<');
        if ( startPos < 0 )
            {
            aOffset = -1;
            return KNullDesC8();
            }
        //Calculate length for the new string to be stored
        ret = tagToBeFound().ReAlloc(
            ( pos - startPos ) + aElement.Length() + KSpaceForDelimiters );
        if ( ret != KErrNone )
            {
            aOffset = -1;
            return KNullDesC8();
            }
        tagToBeFound().SetLength( 0 );
        tagToBeFound().Append( temp.Mid( startPos ) );
        tagToBeFound().AppendFormat( _L8( ":%S" ), &aElement );
        }

    startLength = tagToBeFound().Length(); //needed in later calculations
    startPos += aOffset;
    temp.Set( aMessage.Mid( startPos + startLength ) );

    //Now find the end element. First prepare element to be found
    tagToBeFound().Insert( 1, _L8( "/" ) ); // '/' after '<'
    tagToBeFound().Append( _L8( ">" ) ); // '>' as last


    endPos = temp.Find(tagToBeFound());
    if ( endPos < 0)
        {
        aOffset = -1;
        return KNullDesC8();
        }
    endPos += startPos + startLength + tagToBeFound().Length();

    temp.Set( aMessage.Mid(startPos, endPos - startPos) );

    aOffset = endPos;
    LOG( _L( "Extracted element" ) );
    LOGHEX( temp.Ptr(), temp.Length() );

    //auto_handle closes and frees allocated resources
    return temp;
    }

// -----------------------------------------------------------------------------
// CRoapParser::ExtractDomainKeyElements
// -----------------------------------------------------------------------------
//
TInt CRoapParser::ExtractDomainKeyElements(
    const TDesC8& aMessage, RPointerArray<HBufC8>& aDomainKeyElements ) const
    {
    LOG( _L("CRoapParser::ExtractDomainKeyElements") );

    TPtrC8 temp(KNullDesC8);
    TPtrC8 elementPtr(KNullDesC8);
    TInt pos = 0;
    HBufC8* element = NULL;
    TInt err = KErrNone;
    TPtr8 ptr(0, 0);
    TInt macStart = 0;
    TInt macEnd = 0;

    // extract DomainKey elements to aDomainKeyElements array
    while (true)
        {
        elementPtr.Set( ExtractElement( aMessage, KDomainKey(), pos ) );

        if( pos < 0 )
            {
            // no more DomainKeys present
            if(aDomainKeyElements.Count())
                {
                return KErrNone;
                }
            else
                {
                aDomainKeyElements.ResetAndDestroy();
                return KErrRoapServerFatal;
                }
            }

        element = elementPtr.Alloc();
        if(!element)
            {
            return KErrNoMemory;
            }

        // remove mac element from found DomainKeyElement
        macStart = element->Find( KMac );
        temp.Set(element->Left(macStart));
        macStart = temp.LocateReverse('<');
        macEnd = element->Find( KMacEnd ) + KMacEnd().Length();
        if ( macStart < 0 || macEnd < 0 )
            {
            aDomainKeyElements.ResetAndDestroy();
            return KErrRoapServerFatal;
            }
        ptr.Set( element->Des() );
        ptr.Replace( macStart, macEnd - macStart, KNullDesC8 );

        err = aDomainKeyElements.Append(element);
        if(err)
            {
            delete element;
            element = NULL;
            aDomainKeyElements.ResetAndDestroy();
            return err;
            }
        }
    }

//  End of File
