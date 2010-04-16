/*
* Copyright (c) 2005 - 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of a parser for protected OMA DRM rights
 *
*/


// INCLUDE FILES
#include <e32base.h>
#include <e32math.h>
#include <apmstd.h>
#include <utf.h>
#include <stringpool.h>
#include <xml/parserfeature.h>

#ifdef _DEBUG
#include <flogger.h>
#endif // _DEBUG
#include "Base64.h"
#include "DcfCommon.h"
#include "DrmRightsParser.h"
#include "DRMProtectedRoParser.h"
#include "DRMRightsClient.h"
#include "hash.h"
#include "DRMCommon.h"
#include "OmaCrypto.h"
#include "CmlaCrypto.h"
#include "DrmPermission.h"
#include "RoapMessage.h"
#include "DcfRep.h"
#include "DcfEntry.h"
#include "oma2dcf.h"

using namespace Roap;
using namespace Xml;

// MODULE DATA STRUCTURES
NONSHARABLE_CLASS(CDrmProtectedRoParser::CParsedProtectedRo): public CBase
    {
public:
    static CParsedProtectedRo* NewL();
    ~CParsedProtectedRo();

protected:
    CParsedProtectedRo();

public:
    TKeyTransportScheme iTransportScheme;
    HBufC8* iKey;
    TBool iDomainRo;
    HBufC8* iId;
    HBufC8* iRiUrl;
    HBufC8* iDomainId;
    HBufC8* iRightsIssuerId;
    TTime iTimeStamp;
    HBufC8* iMac;
    HBufC8* iMacDigest;
    };

enum TElementEnum
    {
    EProtectedRo = 0,
    ERo,
    EDomainRo,
    EId,
    EStateful,
    EVersion,
    ERiId,
    EKeyIdentifier,
    EHash,
    ETimeStamp,
    EEncKey,
    EEncryptionMethod,
    EKeyInfo,
    EX509SPKIHash,
    ECipherData,
    ECipherValue,
    EMac,
    ESignedInfo,
    ECanonicalizationMethod,
    ESignatureMethod,
    EReference,
    EDigestMethod,
    EDigestValue,
    ESignatureValue,
    ERetreivalMethod,
    ERoapDomainId,
    EDomainIdentifier,
    ERoapDomainIdentifier,
    ERoResponse,
    ELast,
    };

enum TParserStackState
    {
    ERoState,
    ECipherValueState,
    EDomainIdentifierState,
    EEncryptionMethodState,
    ERightsIssuerIdState,
    ETimeStampState,
    EMacValueState,
    EMacDigestValueState,
    EUnknownState = -1,
    };

struct TElements
    {
    const TText8* iString;
    TInt iNumber;
    };

// MACROS
#define ELEMENT_COUNT(x) static_cast<TInt>((sizeof(x) / sizeof (x[0])))

#ifdef _DEBUG
_LIT(KParseLogDir, "DRM");
_LIT(KParseLogFile, "Parse.log");
#define DEBUGLOG( a ) { _LIT(KDebugString, a); RFileLogger::Write( KParseLogDir(), KParseLogFile(), EFileLoggingModeAppend, KDebugString() ); }
#define DEBUGLOGHEX( ptr, len ) RFileLogger::HexDump( KParseLogDir(), KParseLogFile(), EFileLoggingModeAppend, _S(""), _S(""), ptr, len );
#else
#define DEBUGLOG( a )
#define DEBUGLOGHEX( ptr, len )
#endif // _DEBUG

// LOCAL CONSTANTS AND MACROS
const TInt KParserChunkSize = 512;
const TInt KMaxElementNesting = 24;
const TInt KMaxElementLen = 80;

_LIT8(KXmlParserMimeType, "text/xml");
_LIT8(KIdAttr, "id");
_LIT8(KDomainRoAttr, "domainRO");
_LIT8(KRiUrlAttr, "riURL");
_LIT8(KAlgorithmAttr, "Algorithm");
_LIT8(KProtectedRoElement, "protectedRO");
_LIT8(KRoapPrefix, "roap:");
_LIT8(KStartElementStart, "<");
_LIT8(KEndElementStart, "</");
_LIT8(KRoElement, "roap:ro");
_LIT8( KElementEnd, ">" );
_LIT8(KQualifiedProtectedRo, "roap:protectedRO");
_LIT8(KSignedInfoElement, "ds:SignedInfo");
_LIT8(KRoapXmlNs, " xmlns:roap=\"urn:oma:bac:dldrm:roap-1.0\"");
_LIT8(KRoapXmlNsUri, "urn:oma:bac:dldrm:roap-1.0");
_LIT8(KMacStart, "<mac");

static const TElements KElements[] =
    {
        {
        _S8("roResponse"), ERoResponse
        },
        {
        _S8("protectedRo"), EProtectedRo
        },
        {
        _S8("ro"), ERo
        },
        {
        _S8("domainRo"), EDomainRo
        },
        {
        _S8("id"), EId
        },
        {
        _S8("stateful"), EStateful
        },
        {
        _S8("version"), EVersion
        },
        {
        _S8("riID"), ERiId
        },
        {
        _S8("keyIdentifier"), EKeyIdentifier
        },
        {
        _S8("hash"), EHash
        },
        {
        _S8("timeStamp"), ETimeStamp
        },
        {
        _S8("encKey"), EEncKey
        },
        {
        _S8("EncryptionMethod"), EEncryptionMethod
        },
        {
        _S8("KeyInfo"), EKeyInfo
        },
        {
        _S8("roap:X509SPKIHash"), EX509SPKIHash
        },
        {
        _S8("CipherData"), ECipherData
        },
        {
        _S8("CipherValue"), ECipherValue
        },
        {
        _S8("mac"), EMac
        },
        {
        _S8("SignedInfo"), ESignedInfo
        },
        {
        _S8("CanonicalizationMethod"), ECanonicalizationMethod
        },
        {
        _S8("SignatureMethod"), ESignatureMethod
        },
        {
        _S8("Reference"), EReference
        },
        {
        _S8("DigestMethod"), EDigestMethod
        },
        {
        _S8("DigestValue"), EDigestValue
        },
        {
        _S8("SignatureValue"), ESignatureValue
        },
        {
        _S8("RetreivalMethod"), ERetreivalMethod
        },
        {
        _S8("domainID"), ERoapDomainId
        },
        {
        _S8("DomainIdentifier"), EDomainIdentifier
        },
        {
        _S8("roap:DomainIdentifier"), ERoapDomainIdentifier
        }
    };

struct TStackState
    {
    TParserStackState iState;
    TElementEnum iStack[KMaxElementNesting];
    };

static const TStackState KParserStackStates[] =
    {
        {
        ERightsIssuerIdState,
            {
            EHash, EKeyIdentifier, ERiId, ERo, ELast
            }
        },
        {
        EDomainIdentifierState,
            {
            ERoapDomainId, EKeyInfo, EEncKey, ERo, ELast
            }
        },
        {
        EDomainIdentifierState,
            {
            EDomainIdentifier, EKeyInfo, EEncKey, ERo, ELast
            }
        },
        {
        EDomainIdentifierState,
            {
            ERoapDomainIdentifier, EKeyInfo, EEncKey, ERo, ELast
            }
        },
        {
        ECipherValueState,
            {
            ECipherValue, ECipherData, EEncKey, ERo, ELast
            }
        },
        {
        EEncryptionMethodState,
            {
            EEncryptionMethod, EEncKey, ERo, ELast
            }
        },
        {
        ETimeStampState,
            {
            ETimeStamp, ERo, ELast
            }
        },
        {
        EMacValueState,
            {
            ESignatureValue, EMac, ELast
            }
        },
        {
        EMacDigestValueState,
            {
            EDigestValue, EReference, ESignedInfo, EMac, ELast
            }
        },
        {
        ERoState,
            {
            ERo, ELast
            }
        }
    };

// LOCAL FUNCTION PROTOTYPES
TTime Iso8601ToTime(TDesC8& aTimeString);

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Iso8601ToTime
// -----------------------------------------------------------------------------
//
TTime Iso8601ToTime(TDesC8& aTimeString)
    {
    TLex8 lex;
    TInt year = 0;
    TInt month = 0;
    TInt day = 0;
    TInt hour = 0;
    TInt minute = 0;
    TInt second = 0;
    TTime r = Time::NullTTime();
    TLocale l;
    TTimeIntervalSeconds offset(l.UniversalTimeOffset());

    if (aTimeString.Length() > 0)
        {
        lex = aTimeString;
        lex.Val(year);
        lex.Inc();
        lex.Val(month);
        lex.Inc();
        lex.Val(day);
        lex.Inc();
        lex.Val(hour);
        lex.Inc();
        lex.Val(minute);
        lex.Inc();
        lex.Val(second);
        r = TTime(TDateTime(year, static_cast<TMonth>(month - 1), day - 1,
                hour, minute, second, 0));
        if (lex.Get() != 'Z')
            {
            r += offset;
            }
        }
    return r;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDrmProtectedRo::NewL
// Allocate a new protected RO object
// -----------------------------------------------------------------------------
//
CDrmProtectedRoParser::CParsedProtectedRo* CDrmProtectedRoParser::CParsedProtectedRo::NewL()
    {
    CParsedProtectedRo* self = new(ELeave) CParsedProtectedRo;
    return self;
    }

// -----------------------------------------------------------------------------
// CParsedProtectedRo::~CParsedProtectedRo
// Release all data
// -----------------------------------------------------------------------------
//
CDrmProtectedRoParser::CParsedProtectedRo::~CParsedProtectedRo()
    {
    delete iKey;
    delete iId;
    delete iRiUrl;
    delete iDomainId;
    delete iRightsIssuerId;
    delete iMac;
    delete iMacDigest;
    }

// -----------------------------------------------------------------------------
// CParsedProtectedRo::CParsedProtectedRo
// Initialize all member data
// -----------------------------------------------------------------------------
//
CDrmProtectedRoParser::CParsedProtectedRo::CParsedProtectedRo() :
    iTransportScheme(EOma), iKey(NULL), iDomainRo(EFalse), iId(NULL),
            iRiUrl(NULL), iDomainId(NULL), iRightsIssuerId(NULL), iMac(NULL)
    {
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::CDrmProtectedRoParser
// Reset all member variables
// -----------------------------------------------------------------------------
//
CDrmProtectedRoParser::CDrmProtectedRoParser() :
    iParser(NULL), iRights(NULL), iContent(NULL)
    {
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::ConstructL
// Initialize the string pool elements
// -----------------------------------------------------------------------------
//
void CDrmProtectedRoParser::ConstructL()
    {
    iParser = CParser::NewL(KXmlParserMimeType, *this);
    for (TInt i = 0; i < ELEMENT_COUNT(KElements); i++)
        {
        TPtrC8 ptr(KElements[i].iString,
                User::StringLength(KElements[i].iString));
        iElements[KElements[i].iNumber] = iParser->StringPool().OpenStringL(ptr);
        }
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDrmProtectedRoParser* CDrmProtectedRoParser::NewL()
    {
    CDrmProtectedRoParser* self = new(ELeave) CDrmProtectedRoParser;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::~CDrmProtectedRoParser
// Release the allocated strings
// -----------------------------------------------------------------------------
//
EXPORT_C CDrmProtectedRoParser::~CDrmProtectedRoParser()
    {
    for (TInt i = 0; i < ELEMENT_COUNT(KElements); i++)
        {
        iElements[KElements[i].iNumber].Close();
        }
    delete iParser;
    delete iRights;
    delete iContent;
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::ParseL
// Parses one <protectedRo> element
// -----------------------------------------------------------------------------
//
void CDrmProtectedRoParser::ParseL(const TDesC8& aProtectedRo,
        CParsedProtectedRo*& aResultRights)
    {
    TInt i( 0 );

    iElementStackDepth = 0;
    delete iRights;
    delete iContent;
    iContent = NULL;
    iRights = NULL;
    iRights = CParsedProtectedRo::NewL();
    iParser->ParseBeginL();

    i = aProtectedRo.Find( KRoapXmlNsUri );

    if ( i == KErrNotFound )
        {
        // Feed in the ROAP XML namespace declaration
        i = aProtectedRo.Find(KProtectedRoElement);
        if ( i == KErrNotFound )
            {
            User::Leave(KErrCorrupt);
            }
        i += KProtectedRoElement().Length();
        iParser->ParseL( aProtectedRo.Left( i ) );
        iParser->ParseL( KRoapXmlNs() );
        }
    else
        {
        i = 0;
        }

    while ( i < aProtectedRo.Length() )
        {
        TInt n( Min( aProtectedRo.Length() - i, KParserChunkSize ) );
        iParser->ParseL( aProtectedRo.Mid( i, n ) );
        i += n;
        }
    iParser->ParseEndL();
    aResultRights = iRights;
    iRights = NULL;
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::GetRiUrlL
// Parse the RO and return the RI URL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CDrmProtectedRoParser::GetRiUrlL(
        const TDesC8& aProtectedRo)
    {
    TInt i( 0 );
    HBufC8* r( NULL );

    iElementStackDepth = 0;
    delete iRights;
    delete iContent;
    iRights = NULL;
    iContent = NULL;
    iRights = CParsedProtectedRo::NewL();
    iParser->ParseBeginL();
    while (i < aProtectedRo.Length())
        {
        TInt n( Min( aProtectedRo.Length() - i, KParserChunkSize ) );
        iParser->ParseL( aProtectedRo.Mid( i, n ) );
        i += n;
        }
    iParser->ParseEndL();
    if ( iRights->iRiUrl )
        {
        r = iRights->iRiUrl->AllocL();
        }
    delete iRights;
    iRights = NULL;
    return r;
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::GetRiUrlL
// Parse the RO and return the RI ID
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CDrmProtectedRoParser::GetRiIdL(
        const TDesC8& aProtectedRo)
    {
    HBufC8* r = NULL;

    iElementStackDepth = 0;
    delete iRights;
    delete iContent;
    iRights = NULL;
    iContent = NULL;
    iRights = CParsedProtectedRo::NewL();
    iParser->ParseBeginL();
    TInt i( 0 );
    while (i < aProtectedRo.Length())
        {
        TInt n( Min( aProtectedRo.Length() - i, KParserChunkSize ) );
        iParser->ParseL(aProtectedRo.Mid(i, n));
        i += n;
        }
    iParser->ParseEndL();
    if ( iRights->iRightsIssuerId )
        {
        r = iRights->iRightsIssuerId->AllocL();
        }
    delete iRights;
    iRights = NULL;
    return r;
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::GetDomainIdL
// Parse the RO and return the domain ID
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CDrmProtectedRoParser::GetDomainIdL(
        const TDesC8& aProtectedRo)
    {
    HBufC8* r = NULL;

    iElementStackDepth = 0;
    delete iRights;
    iRights = NULL;
    delete iContent;
    iContent = NULL;
    iRights = CParsedProtectedRo::NewL();
    iParser->ParseBeginL();
    TInt i( 0 );
    while (i < aProtectedRo.Length())
        {
        TInt n( Min(aProtectedRo.Length() - i, KParserChunkSize) );
        iParser->ParseL(aProtectedRo.Mid(i, n));
        i += n;
        }
    iParser->ParseEndL();
    if ( iRights->iDomainId )
        {
        r = iRights->iDomainId->AllocL();
        }
    delete iRights;
    iRights = NULL;
    return r;
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::ParseAndStoreL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDrmProtectedRoParser::ParseAndStoreL(
        const TDesC8& aRightsObject,
        RPointerArray<CDRMRights>& aResultRights)
    {
    RDRMRightsClient client;
    CDrmRightsParser* parser = NULL;
    CDRMRights* rights = NULL;
    CParsedProtectedRo* parsedRo = NULL;
    TDRMUniqueID id = 0;
    HBufC8 *cid = NULL;
    TPtr8 ptr(0, 0);
    TBool domain = EFalse;
    TPtrC8 riId(0, 0);
    TInt start = 0;
    TInt end = 0;
    TInt roStart = 0;
    TInt roEnd = 0;
    TInt signedInfoStart = 0;
    TInt signedInfoEnd = 0;
    TInt n( 0 );
    TInt r( 0 );
    TBool inCache = EFalse;
    CSHA1* sha1 = NULL;
    TBool integrityCheck = ETrue;
    HBufC8* roWithNs = NULL;
    TPtr8 roWithNsPtr(NULL, NULL);

    User::LeaveIfError(client.Connect());
    CleanupClosePushL(client);

    // Loop through the complete roResponse and find all embedded protected
    // rights objects
    while (GetElementStartEnd(KProtectedRoElement, aRightsObject, start, end))
        {
        TPtrC8 protectedRo = aRightsObject.Mid(start, end - start);
        if (GetElementStartEnd(KRoElement, protectedRo, roStart, roEnd) == EFalse)
            {
            User::Leave(KErrCorrupt);
            }
        TPtrC8 ro = protectedRo.Mid(roStart, roEnd - roStart);

        ParseL(protectedRo, parsedRo);
        User::LeaveIfNull(parsedRo);
        CleanupStack::PushL(parsedRo);

        parser = CDrmRightsParser::NewL();
        CleanupStack::PushL(parser);
        parser->ParseL(protectedRo, aResultRights);
        CleanupStack::PopAndDestroy( parser );

        //////////////////////////
        // Replay protection
        //////////////////////////

        if (parsedRo->iTimeStamp == Time::NullTTime())
            {
            User::LeaveIfError(client.IsInCache(*parsedRo->iId, inCache));
            if (inCache)
                {
                User::Leave(KErrAlreadyExists);
                }
            }
        else
            {
            User::LeaveIfError(client.IsInCache(*parsedRo->iId, parsedRo->iTimeStamp, inCache));
            if (inCache)
                {
                User::Leave(KErrAlreadyExists);
                }
            }

        ///////////////////////////////////////////////////////
        // Unwrap MAC and REK
        // The plain keys are stored on the server side session
        ///////////////////////////////////////////////////////
        if (parsedRo->iDomainRo)
            {
            User::LeaveIfError( client.UnwrapMacAndRek(*parsedRo->iKey,
                            parsedRo->iTransportScheme,
                            *parsedRo->iRightsIssuerId,
                            *parsedRo->iDomainId) );
            }
        else
            {
            User::LeaveIfError( client.UnwrapMacAndRek(*parsedRo->iKey,
                            parsedRo->iTransportScheme,
                            *parsedRo->iRightsIssuerId,
                            KNullDesC8) );
            }

        //////////////////////////
        // Validate digest and MAC
        //////////////////////////
        roWithNs = AddRoapNamespaceL(ro);
        if (roWithNs)
            {
            CleanupStack::PushL(roWithNs);
            roWithNsPtr.Set(roWithNs->Des());
            }

        // Validate RO digest
        sha1 = CSHA1::NewL();
        if (roWithNs)
            {
            DEBUGLOGHEX( roWithNsPtr.Ptr(), roWithNsPtr.Length() );
            sha1->Update(roWithNsPtr);
            CleanupStack::PopAndDestroy(roWithNs);
            }
        else
            {
            DEBUGLOGHEX( ro.Ptr(), ro.Length() );
            sha1->Update(ro);
            }

        CleanupStack::PushL(sha1);
        HBufC8* calculatedHash( sha1->Final().AllocL() );
        CleanupStack::PopAndDestroy( sha1 );
        sha1 = NULL;
        CleanupStack::PushL( calculatedHash );

        if (calculatedHash->Compare(*parsedRo->iMacDigest) == KErrNone)
            {
            // Digest OK -> now verify the MAC
            signedInfoStart = protectedRo.Find(KMacStart());

            if (signedInfoStart < KErrNone)
                {
                integrityCheck = EFalse;
                r = KErrCorrupt;
                }

            if (integrityCheck && GetElementStartEnd(KSignedInfoElement, protectedRo, signedInfoStart, signedInfoEnd) == EFalse)
                {
                integrityCheck = EFalse;
                r = KErrCorrupt;
                }

            if(integrityCheck)
                {
                TPtrC8 signedInfo = protectedRo.Mid(signedInfoStart, signedInfoEnd - signedInfoStart);
                r = client.VerifyMacL(signedInfo, *parsedRo->iMac);

                if (r == KErrNone)
                    {
                    // MAC validation OK
                    // If we parsed a domain RO, store it in its original form as well
                    if (parsedRo->iDomainRo)
                        {
                        HBufC8* protRoWithNs = NULL;
                        protRoWithNs = AddRoapNamespaceL(protectedRo);
                        if(protRoWithNs)
                            {
                            client.AddDomainRO(*parsedRo->iId, *protRoWithNs);
                            delete protRoWithNs;
                            protRoWithNs = NULL;
                            }
                        else
                            {
                            client.AddDomainRO(*parsedRo->iId, protectedRo);
                            }
                        }

                    // Add RO to replay cache
                    if (parsedRo->iTimeStamp == Time::NullTTime())
                        {
                        User::LeaveIfError(client.AddToCache(*parsedRo->iId));
                        }
                    else
                        {
                        User::LeaveIfError(client.AddToCache(*parsedRo->iId, parsedRo->iTimeStamp));
                        }
                    }
                else
                    {
                    integrityCheck = EFalse;
                    }
                }
            }
        else
            {
            integrityCheck = EFalse;
            r = KErrCorrupt;
            }
        CleanupStack::PopAndDestroy( calculatedHash );
        calculatedHash = NULL;

        if ( integrityCheck && !r )
            {
            // Only add the rigths which were parsed in this loop

            for ( TInt i( n ); i < aResultRights.Count(); i++)
                {
                cid = NULL;
                rights = aResultRights[i];
                rights->GetContentURI(cid);
                CleanupStack::PushL(cid);
                rights->GetPermission().iOriginalInsertTime =
                parsedRo->iTimeStamp;
                rights->GetPermission().iRoID = (*parsedRo->iId).AllocL();
                rights->GetPermission().iRiId.Copy(*parsedRo->iRightsIssuerId);
                if (parsedRo->iDomainRo)
                    {
                    domain = ETrue;
                    rights->GetPermission().iDomainID =
                    parsedRo->iDomainId->AllocL();
                    }
                if (parsedRo->iRightsIssuerId)
                    {
                    riId.Set(*parsedRo->iRightsIssuerId);
                    }
                if (parsedRo->iTransportScheme == EOma)
                    {
                    rights->GetPermission().iRightsObjectVersion.iVersionMain =
                    EOma2Rights;
                    }
                else
                    {
                    rights->GetPermission().iRightsObjectVersion.iVersionMain =
                    ECmlaRights;
                    }
                r = client.AddProtectedRecord( rights->GetAsset().iProtectedKey,
                        domain, rights->GetPermission(),
                        *cid, id );
                if (!r)
                    {
                    if (rights->GetAsset().iProtectedAuthSeed.Length()> 0)
                        {
                        r = client.SetAuthenticationSeed(*cid,
                                rights->GetAsset().iProtectedAuthSeed);
                        }
                    }

                User::LeaveIfError(r);
                rights->SetLocalID(id);
                CleanupStack::PopAndDestroy(cid);
                }
            }
        else
            {
            User::Leave(KErrCorrupt);
            }

        start = end;
        n = aResultRights.Count();
        CleanupStack::PopAndDestroy(parsedRo);
        parsedRo = NULL;
        }
    CleanupStack::PopAndDestroy( &client );
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::OnStartDocumentL
// -----------------------------------------------------------------------------
//
void CDrmProtectedRoParser::OnStartDocumentL(
        const RDocumentParameters& /*aDocParam*/, TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::OnEndDocumentL
// -----------------------------------------------------------------------------
//
void CDrmProtectedRoParser::OnEndDocumentL(TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::OnStartElementL
// -----------------------------------------------------------------------------
//
void CDrmProtectedRoParser::OnStartElementL(const RTagInfo& aElement,
        const RAttributeArray& aAttributes, TInt /*aErrorCode*/)
    {
    HBufC8* b= NULL;

    if (iContent)
        {
        delete iContent;
        iContent = NULL;
        iContent = HBufC8::NewL(0);
        }

    for (TInt i = 0; i < KMaxElementCount; i++)
        {
        if (aElement.LocalName() == iElements[i])
            {
            iElementStack[iElementStackDepth] = static_cast<TElementEnum>(i);
            iElementStackDepth++;
            if (iElementStackDepth == KMaxElementNesting)
                {
                User::Leave(EXmlUnexpectedState);
                }
            TInt state( MatchStackState() );
            if (state == ERoState)
                {
                _LIT8( KTrue, "true" );
                iRights->iId = GetAttributeValueL(aAttributes, KIdAttr);
                iRights->iRiUrl = GetAttributeValueL(aAttributes, KRiUrlAttr);
                b = GetAttributeValueL(aAttributes, KDomainRoAttr);
                if ( b && b->Compare( KTrue() ) == 0 )
                    {
                    iRights->iDomainRo = ETrue;
                    }
                else
                    {
                    iRights->iDomainRo = EFalse;
                    }
                delete b;
                }
            else
                if (state == EEncryptionMethodState)
                    {
                    b = GetAttributeValueL(aAttributes, KAlgorithmAttr);
                    if ( b )
                        {
                        iRights->iTransportScheme
                                = CmlaCrypto::AlgorithmIdToTransportScheme(*b);
                        delete b;
                        }
                    }
            }
        }
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::OnEndElementL
// -----------------------------------------------------------------------------
//
void CDrmProtectedRoParser::OnEndElementL(const RTagInfo& aElement, TInt /*aErrorCode*/)
    {
    for (TInt i = 0; i < KMaxElementCount; i++)
        {
        if (aElement.LocalName() == iElements[i])
            {
            switch (MatchStackState())
                {
                case ECipherValueState:
                    iRights->iKey = Base64DecodeL(iContent->Des());
                    break;
                case EDomainIdentifierState:
                    iRights->iDomainId = iContent->AllocL();
                    break;
                case ERightsIssuerIdState:
                    iRights->iRightsIssuerId = Base64DecodeL(iContent->Des());
                    break;
                case ETimeStampState:
                    iRights->iTimeStamp = Iso8601ToTime(*iContent);
                    break;
                case EMacValueState:
                    iRights->iMac = Base64DecodeL(iContent->Des());
                    break;
                case EMacDigestValueState:
                    iRights->iMacDigest = Base64DecodeL(iContent->Des());
                    break;
                }
            iElementStackDepth--;
            if (iElementStackDepth < 0)
                {
                User::Leave(EXmlUnexpectedState);
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::OnContentL
// -----------------------------------------------------------------------------
//
void CDrmProtectedRoParser::OnContentL(const TDesC8& aBytes, TInt /*aErrorCode*/)
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
// CDrmProtectedRoParser::OnStartPrefixMappingL
// -----------------------------------------------------------------------------
//
void CDrmProtectedRoParser::OnStartPrefixMappingL(const RString& /*aPrefix*/,
        const RString& /*aUri*/, TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::OnEndPrefixMappingL
// -----------------------------------------------------------------------------
//
void CDrmProtectedRoParser::OnEndPrefixMappingL(const RString& /*aPrefix*/,
        TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::OnIgnorableWhiteSpaceL
// -----------------------------------------------------------------------------
//
void CDrmProtectedRoParser::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/,
        TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::OnSkippedEntityL
// -----------------------------------------------------------------------------
//
void CDrmProtectedRoParser::OnSkippedEntityL(const RString& /*aName*/, TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::OnProcessingInstructionL
// -----------------------------------------------------------------------------
//
void CDrmProtectedRoParser::OnProcessingInstructionL(
        const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::OnOutOfData
// -----------------------------------------------------------------------------
//
void CDrmProtectedRoParser::OnOutOfData()
    {
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::OnError
// -----------------------------------------------------------------------------
//
void CDrmProtectedRoParser::OnError(TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::GetExtendedInterface
// -----------------------------------------------------------------------------
//
TAny* CDrmProtectedRoParser::GetExtendedInterface(const TInt32 /*aUid*/)
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::MatchStackState
// -----------------------------------------------------------------------------
//
TInt CDrmProtectedRoParser::MatchStackState()
    {
    TInt i( 0 );
    TInt j( 0 );
    TInt k( 0 );
    TParserStackState r = EUnknownState;

    for (i = 0; r == EUnknownState && i < ELEMENT_COUNT(KParserStackStates); i++)
        {
        for (j = iElementStackDepth - 1, k = 0; j > 0
                && KParserStackStates[i].iStack[k] != ELast; j--, k++)
            {
            if (iElementStack[j] != KParserStackStates[i].iStack[k])
                {
                break;
                }
            }
        if ((j == 0 && iElementStack[j] == KParserStackStates[i].iStack[k])
                || KParserStackStates[i].iStack[k] == ELast)
            {
            r = KParserStackStates[i].iState;
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::GetAttributeValueL
// Allocate the attribute value as a buffer if it is found, otherwise it's NULL
// -----------------------------------------------------------------------------
//
HBufC8* CDrmProtectedRoParser::GetAttributeValueL(
        const RAttributeArray& aAttrList, const TDesC8& aAttrName)
    {
    HBufC8* r( NULL );

    for ( TInt i(0); !r && i < aAttrList.Count(); i++ )
        {
        const RAttribute& a( aAttrList[i] );
        if ( a.Attribute().LocalName().DesC().Compare( aAttrName ) == 0 )
            {
            r = a.Value().DesC().AllocL();
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::GetElementStartEnd
// Search for the element as a string
// -----------------------------------------------------------------------------
//
TBool CDrmProtectedRoParser::GetElementStartEnd(const TDesC8& aElement,
        const TDesC8& aData, TInt& aStart, TInt& aEnd)
    {
    TInt i( 0 );
    TInt j( 0 );
    TBool r( EFalse );
    TBuf8<KMaxElementLen> element;

    DEBUGLOG( "CDrmProtectedRoParser::GetElementStartEnd ->" );

    DEBUGLOG( "aElement" );
    DEBUGLOGHEX( aElement.Ptr(), aElement.Length() );
    DEBUGLOG( "aData" );
    DEBUGLOGHEX( aData.Ptr(), aData.Length() );
    element.Copy(KStartElementStart);
    element.Append(aElement);
    // Find the start tag, which has to end with a "<" or whitespace
    i = aData.Mid(aStart).Find(element);

    if (i == KErrNotFound)
        {
        // try the same thing with ROAP prefix
        element.Copy(KStartElementStart);
        element.Append(KRoapPrefix);
        element.Append(aElement);
        i = aData.Mid(aStart).Find(element);
        }

    while (i != KErrNotFound && !(aData[aStart + i + element.Length()] == '>' || aData[aStart + i + element.Length()] <= ' '))
        {
        aStart += i;
        i = aData.Mid(aStart + element.Length() + 1).Find(element);
        }

    if (i != KErrNotFound)
        {
        aStart += i;
        i = aStart;
        element.Copy(KEndElementStart);
        element.Append(aElement);
        element.Append(KElementEnd);
        j = aData.Mid(i + 1).Find(element);
        if (j == KErrNotFound)
            {
            // try the same thing with ROAP prefix
            element.Copy(KEndElementStart);
            element.Append(KRoapPrefix);
            element.Append(aElement);
            element.Append(KElementEnd);
            j = aData.Mid(i + 1).Find(element);
            }
        if (j != KErrNotFound)
            {
            aEnd = i + 1 + j + element.Length();
            r = ETrue;
            }
        }
#ifdef _DEBUG
    if ( r )
        {
        TPtrC8 extracted( aData.Mid( aStart, aEnd - aStart ) );
        DEBUGLOG( "got element: " );
        DEBUGLOGHEX( extracted.Ptr(), extracted.Length() );
        }
#endif
    DEBUGLOG( "-> CDrmProtectedRoParser::GetElementStartEnd" );
    return r;
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::HandleUnregisteredDomainRoL
// Embed the domain RO into all matching DCF files
// -----------------------------------------------------------------------------
//
void CDrmProtectedRoParser::HandleUnregisteredDomainRoL(
        const TDesC8& aContentId, const TDesC8& aDomainRoData,
        CParsedProtectedRo* /*aDomainRo*/)
    {
    CDcfRep* rep( NULL );
    COma2Dcf* dcf( NULL );
    CDcfEntry* entry( NULL );
    RFile file;
    RFs fs;
    TBool exists( EFalse );
    TInt r( KErrNone );

    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    rep = CDcfRep::NewL();
    CleanupStack::PushL( rep );
    TRAP( r, rep->OrderListL( aContentId ) );
    if ( r == KErrNone )
        {
        entry = rep->NextL();
        }
    while ( r == KErrNone && entry )
        {
        CleanupStack::PushL( entry );

        r = file.Open( fs, entry->FileName(), EFileWrite
                | EFileShareReadersOrWriters );
        if ( r == KErrInUse )
            {
            r = file.Open(fs, entry->FileName(), EFileWrite | EFileShareAny);
            }

        if ( r == KErrNone )
            {
            CleanupClosePushL( file );
            dcf = COma2Dcf::NewL( file );
            CleanupStack::PushL( dcf );
            for ( TInt i = 0; !exists && i < dcf->iRightsObjects.Count(); i++ )
                {
                if ( dcf->iRightsObjects[i]->Compare(aDomainRoData) == 0 )
                    {
                    exists = ETrue;
                    }
                }
            if ( !exists )
                {
                dcf->iRightsObjects.Append( aDomainRoData.AllocL() );
                dcf->SetRightsObjectsL( dcf->iRightsObjects );
                }
            CleanupStack::PopAndDestroy( dcf );
            CleanupStack::PopAndDestroy( &file );
            }
        CleanupStack::PopAndDestroy( entry );
        entry = rep->NextL();
        }
    CleanupStack::PopAndDestroy( rep );
    CleanupStack::PopAndDestroy( &fs );
    }

// -----------------------------------------------------------------------------
// CDrmProtectedRoParser::AddRoapNamespaceL
// Add ROAP namespace declaration if needed
// -----------------------------------------------------------------------------
//
HBufC8* CDrmProtectedRoParser::AddRoapNamespaceL(TDesC8& aProtectedRo) const
    {
    TInt index = 0;
    HBufC8* roWithNs= NULL;
    TPtr8 roWithNsPtr(NULL, NULL);

    if (aProtectedRo.Find(KRoapXmlNsUri) == KErrNotFound)
        {
        roWithNs = HBufC8::NewLC(aProtectedRo.Length() + KRoapXmlNs().Length());
        roWithNsPtr.Set(roWithNs->Des());
        index = aProtectedRo.Find(KQualifiedProtectedRo);
        if (index == KErrNotFound)
            {
            index = aProtectedRo.Find(KRoElement);
            if (index == KErrNotFound)
                {
                User::Leave(KErrCorrupt);
                }
            index += KRoElement().Length();
            }
        else
            {
            index += KQualifiedProtectedRo().Length();
            }
        roWithNsPtr.Copy(aProtectedRo.Left(index));
        roWithNsPtr.Append(KRoapXmlNs());
        roWithNsPtr.Append(aProtectedRo.Right(aProtectedRo.Length() - index));
        CleanupStack::Pop(roWithNs);
        }
    return roWithNs;
    }

//  End of File
