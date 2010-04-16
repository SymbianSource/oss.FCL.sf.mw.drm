/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Session class for handling roap storage client requests
*
*/


// INCLUDE FILES
#include <s32file.h>
#include <f32file.h>
#include <x509cert.h>
#include <x509keys.h>
#include <asn1dec.h>
#include <hash.h>
#include <s32mem.h>
#include <centralrepository.h>
#include <uri8.h>
#ifdef RD_DRM_METERING
#include <DRMRightsClient.h>
#include <random.h>
#endif //RD_DRM_METERING
#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif
#include <e32base.h>  // CleanupResetAndDestroyPushL dependencies
#include "drmaescrypto.h"
#include "RoapStorageSession.h"
#include "DRMContextDB.h"
#include "DRMRIContext.h"
#include "DRMDomainContext.h"
#include "RoapStorageServer.h"
#include "CmlaCrypto.h"
#include "drmaescrypto.h"
#include "DrmKeyStorage.h"
#include "RoapDef.h"
#include "RoapLog.h"
#include "drmroapclientserver.h"
#include "DRMClockClient.h"
#include "DRMTypes.h"
#include "DRMPointerArray.h"
//OCSP classes
#include "RoapOcsp.h"
#include "responsedecoder.h"
#include "certid.h"
//drm clock
#include "DRMClockClient.h"

#include "Base64.h"

#include "cleanupresetanddestroy.h" // CleanupResetAndDestroyPushL
// NAMESPACES
using namespace Roap;

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KMaxElementLength = 327680;
const TInt KMinDomainIdLength = 4;
// DEBUG PRINT MACROS
#ifdef _DEBUG
#include <flogger.h>
_LIT(KMacLogDir, "DRM");
_LIT(KMacLogFile, "MAC.log");
#define MACLOG( a ) RFileLogger::Write( KMacLogDir(), KMacLogFile(), EFileLoggingModeAppend, a );
#define MACLOGHEX( ptr, len ) RFileLogger::HexDump( KMacLogDir(), KMacLogFile(), EFileLoggingModeAppend, _S(""), _S(""), ptr, len );
#define MACLOGLIT( a ) MACLOG( _L ( a ) )
#define _DRM_DETAILED_DEBUG // uncomment if want detailed debugs to log file
#else
#undef _DRM_DETAILED_DEBUG
#define MACLOG( a )
#define MACLOGHEX( ptr, len )
#define MACLOGLIT( a )
#endif
#ifdef _DRM_DETAILED_DEBUG
#define MACLOGDETAIL( a ) MACLOG( a )
#define MACLOGHEXDETAIL( ptr, len ) MACLOGHEX( ptr, len )
#define MACLOGLITDETAIL( a ) MACLOGLIT( a )
#define MACLOGLDETAIL( a ) MACLOGLIT( a )
#define CERTDETAIL( c ) \
{ \
const TPtrC8* n( c->DataElementEncoding( CX509Certificate::ESerialNumber ) ); \
const TPtrC8* a( c->DataElementEncoding( CX509Certificate::EAlgorithmId ) ); \
const TPtrC8* s( c->DataElementEncoding( CX509Certificate::ESubjectName ) ); \
const TPtrC8* i( c->DataElementEncoding( CX509Certificate::EIssuerName ) ); \
const TPtrC8* p( c->DataElementEncoding( CX509Certificate::ESubjectPublicKeyInfo ) ); \
MACLOGLDETAIL( "certificate (subject issuer serial algorithm key)" ) \
MACLOGHEXDETAIL( s->Ptr(), s->Length() ) \
MACLOGHEXDETAIL( i->Ptr(), i->Length() ) \
MACLOGHEXDETAIL( n->Ptr(), n->Length() ) \
MACLOGHEXDETAIL( a->Ptr(), a->Length() ) \
MACLOGHEXDETAIL( p->Ptr(), p->Length() ) \
}
#else
#define MACLOGDETAIL( a )
#define MACLOGHEXDETAIL( ptr, len )
#define MACLOGLITDETAIL( a )
#define MACLOGLDETAIL( a )
#define CERTDETAIL( a )
#endif
// END OF DEBUG PRINT MACROS
// LOCAL CONSTANTS AND MACROS
_LIT8( KRoapDomainKey, "roap:domainKey");
_LIT8( KRoapX509SPKIHash, "roap:X509SPKIHash");
_LIT8( KRoapXmlNs, " xmlns:roap=\"urn:oma:bac:dldrm:roap-1.0\"");
#ifdef RD_DRM_METERING
_LIT8( KCmlaIp1, "http://www.cm-la.com/tech/cmlaip/cmlaip#cmlaip-1");
#endif

static const TUid KCRUidRoapHandler =
    {
    0x10205CBF
    };
static const TInt KRoapHandlerRegistrationWhitelist = 1;
static const TInt KMaxWhiteListLen = 1024;

static const TInt KTzZulu( 0 ); //UTC time zone
#ifdef RD_MULTIPLE_DRIVE
_LIT( KRIContextFileName, "_:\\private\\101F51F2\\ricontexts.dat" );
_LIT( KDomainContextFileName, "_:\\private\\101F51F2\\domaincontexts.dat" );
#endif

#define ROAPDB ( ( CRoapStorageServer* )( Server() ) )->ContextDB()

static const TInt KP3( 3 );
#define IPCREAD0L( a ) aMessage.ReadL( 0, a )
#define IPCREAD1L( a ) aMessage.ReadL( 1, a )
#define IPCREAD2L( a ) aMessage.ReadL( 2, a )
#define IPCREAD3L( a ) aMessage.ReadL( KP3, a )
#define IPCWRITE0L( a ) aMessage.WriteL( 0, a )
#define IPCWRITE1L( a ) aMessage.WriteL( 1, a )
#define IPCWRITE2L( a ) aMessage.WriteL( 2, a )
#define IPCWRITE3L( a ) aMessage.WriteL( KP3, a )
#define IPCGETDESLEN0 aMessage.GetDesLength( 0 )
#define IPCGETDESLEN1 aMessage.GetDesLength( 1 )
#define IPCGETDESLEN2 aMessage.GetDesLength( 2 )
#define IPCGETDESLEN3 aMessage.GetDesLength( KP3 )
#define IPCGETDESMAXLEN0 aMessage.GetDesMaxLength( 0 )
#define IPCGETDESMAXLEN1 aMessage.GetDesMaxLength( 1 )
#define IPCGETDESMAXLEN2 aMessage.GetDesMaxLength( 2 )
#define IPCGETDESMAXLEN3 aMessage.GetDesMaxLength( KP3 )

const TInt KSanityDataLengthLow = 0;
const TInt KSanityDataLengthHigh = 32768;
// DATA TYPES
// LOCAL FUNCTION PROTOTYPES
LOCAL_C RPointerArray<HBufC8> BufferToArrayL( TDesC8& aBuffer );
LOCAL_C HBufC8* ArrayToBufferLC( const RPointerArray<HBufC8>& aArray );

LOCAL_C void UnwrapDomainKeyL( MDrmKeyStorage* aKeyStorage,
    const TDesC8& aProtectedDomainKey, HBufC8*& aDomainKey, HBufC8*& aMacKey,
    TKeyTransportScheme& aTransportScheme );
LOCAL_C TBool VerifyMacL( TDesC8& aDomainElements, TDesC8& aMacs,
    RPointerArray<HBufC8>& aMacKeys );

// ---------------------------------------------------------------------------
// DeleteAndSetNull
// ---------------------------------------------------------------------------
//
template<typename Taa> Taa*& DeleteAndSetNull( Taa*& aArg )
    {
    if ( aArg )
        {
        delete aArg;
        aArg = NULL;
        }
    return aArg;
    }

// ---------------------------------------------------------------------------
// SanitizeL
// Performs a sanity check on length parameters
// ---------------------------------------------------------------------------
//
LOCAL_C void SanitizeL( TInt aParam )
    {
    User::LeaveIfError( aParam );
    if ( aParam <= KSanityDataLengthLow || aParam > KSanityDataLengthHigh )
        {
        User::Leave( KErrArgument );
        }
    }

// ---------------------------------------------------------------------------
// Swap elements
// ---------------------------------------------------------------------------
//
template<typename T>
LOCAL_C void SwapElement( RPointerArray<T>& aArray, TInt aFirst, TInt aSecond )
    {
    if ( aFirst != aSecond )
        {
        T* temp( NULL);
        temp = aArray[aFirst];
        aArray[aFirst] = aArray[aSecond];
        aArray[aSecond] = temp;
        }
    }
// ---------------------------------------------------------------------------
// Sort Ocsp responses for certChain validation
// ---------------------------------------------------------------------------
//
LOCAL_C TBool SortOcspCertsL( RPointerArray<CX509Certificate>& aOcspCerts,
    const CX509Certificate* aRootCert )
    {
    MACLOGLITDETAIL( "::SortOcspCertsL -->" )
    const TInt last( aOcspCerts.Count() - 1 );
    TBool sortable( EFalse );

    // Find first find cert issued by root cert
    // There can be only one cert issued by Root on valid cert chain
    for ( TInt i( 0 ); !sortable && i <= last; i++ )
        {
        if ( aRootCert->SubjectName().ExactMatchL(
            aOcspCerts[i]->IssuerName() ) )
            {
            SwapElement<CX509Certificate> ( aOcspCerts, i, last );
            sortable = ETrue;
            }
        }

    for ( TInt s(last); sortable && s > 0; s-- )
        {
        sortable = EFalse;
        for ( TInt i( s - 1 ); !sortable && i >= 0; i-- )
            {
            // compare cert s subject to issuer i
            if ( aOcspCerts[s]->SubjectName().ExactMatchL(
                aOcspCerts[i]->IssuerName() ) )
                {
                SwapElement<CX509Certificate> ( aOcspCerts, i, s-1 );
                sortable = ETrue;
                }
            }
        }
    MACLOGLITDETAIL( "--> ::SortOcspCertsL" )
    return sortable;
    }

// ---------------------------------------------------------------------------
// VerifyOcspCertChainL()
// ---------------------------------------------------------------------------
//
LOCAL_C TBool VerifyOcspCertChainL( const COCSPResponse& aResponse,
    const CX509Certificate* aRootCert, CX509Certificate*& aOcspCert )
    {
    MACLOGLITDETAIL( "::VerifyOcspCertChainL -->" )



    CX509Certificate* cert = NULL;
    const CX509Certificate* signingCert = NULL;
    TInt pos = 0;
    TBool result = ETrue;

    RPointerArray<CX509Certificate> certArray;
    CleanupResetAndDestroyPushL( certArray );

    const TPtrC8* ocspCerts( aResponse.SigningCerts() );

    if ( !ocspCerts )
        {
        MACLOGLIT( "No OCSP certs present" )
#ifdef _DISABLE_OCSP_CHECK
        CleanupStack::PopAndDestroy( &certArray );
        return ETrue;
#endif
        User::Leave( KErrRoapServerFatal );
        }

    MACLOGLITDETAIL( "getting certs in OCSP response" )
    while ( pos < ocspCerts->Length() )
        {
        cert = CX509Certificate::NewLC( *ocspCerts, pos );
        certArray.AppendL( cert );
        CleanupStack::Pop( cert );
        CERTDETAIL( cert )
        }

    result = SortOcspCertsL( certArray, aRootCert );

    // validate the OCSP cert chain up till root cert (root cert is already validated)
    for ( TInt i = 0; i < certArray.Count() && result; i++ )
        {
        cert = certArray[i];
        if ( certArray.Count() - 1 == i )
            {
            signingCert = aRootCert;
            }
        else
            {
            signingCert = certArray[i + 1];
            }
        MACLOGLDETAIL( "OCSP verify, using certificate" )
        CERTDETAIL( cert )
        result = cert->VerifySignatureL( signingCert->PublicKey().KeyData() );
#ifdef _DEBUG
        if ( result )
            {
            MACLOGLIT( "Verify succeeded." )
            }
        else
            {
            MACLOGLIT( "Verify failed." )
            }
#endif
        }

    // Copy OCSP cert and return it
    aOcspCert = CX509Certificate::NewL( *certArray[0] );

    CleanupStack::PopAndDestroy( &certArray );
    MACLOGLITDETAIL( "--> ::VerifyOcspCertChainL" )
    return result;
    }


#ifdef _DISABLE_DRM_TIME_UPDATE_CHECK
LOCAL_C TBool IsCmlaRootL(
    const RPointerArray<HBufC8>& /*aTrustedRoots*/,
    const CX509Certificate* /* aRootCert */)
    {
    MACLOGLITDETAIL( "IsCmlaRootL -->\n--> IsCmlaRootL" )
    return ETrue;
    }
#else
LOCAL_C TBool IsCmlaRootL( const RPointerArray<HBufC8>& aTrustedRoots,
    const CX509Certificate* aRootCert )
    {
    MACLOGLITDETAIL( "IsCmlaRootL -->" )
    TBool isCmlaRoot( EFalse );
    // Check are we running on a CMLA device
    for ( TInt i = 0; i < aTrustedRoots.Count() && !isCmlaRoot; i++ )
        {
        HBufC8* encodedRoot( Base64EncodeL( *aTrustedRoots[i] ) );
        CleanupStack::PushL( encodedRoot );
        if ( encodedRoot->CompareF( KRoapCmlaRootHash() ) == 0
            || encodedRoot->CompareF( KRoapCmlaRnDRootHash() ) == 0
            || encodedRoot->CompareF( KRoapCmlaRnDRootHashOld() ) == 0 )
            {
            isCmlaRoot = ETrue;
            }
        CleanupStack::PopAndDestroy( encodedRoot );
        encodedRoot = NULL;
        }
    if ( isCmlaRoot )
        {
        __UHEAP_MARK;
        // The root cert found, check that it's the CMLA (production or R&D) root
        CSHA1* hash( CSHA1::NewL() );
        CleanupStack::PushL( hash );
        hash->Hash( *aRootCert->DataElementEncoding(
            CX509Certificate::ESubjectPublicKeyInfo ) );
        MACLOGLDETAIL( "Inputs for SHA1" )
        MACLOGHEXDETAIL(
            aRootCert->DataElementEncoding(
                CX509Certificate::ESubjectPublicKeyInfo )->Ptr(),
            aRootCert->DataElementEncoding(
                CX509Certificate::ESubjectPublicKeyInfo )->Length() )

        HBufC8* encodedRoot( Base64EncodeL( hash->Final() ) );
        CleanupStack::PushL( encodedRoot );

        MACLOGLDETAIL( "Encoded root (base64 encoded SHA1 of public key)" )
        MACLOGDETAIL( *encodedRoot )
        MACLOGLDETAIL( "CMLA root" )
        MACLOGDETAIL( KRoapCmlaRootHash() )
        MACLOGLDETAIL( "CMLA RD root" )
        MACLOGDETAIL( KRoapCmlaRnDRootHash() )
        MACLOGLDETAIL( "old CMLA RD root" )
        MACLOGDETAIL( KRoapCmlaRnDRootHashOld() )

        if ( encodedRoot->CompareF( KRoapCmlaRootHash() ) != 0
            && encodedRoot->CompareF( KRoapCmlaRnDRootHash() ) != 0
            && encodedRoot->CompareF( KRoapCmlaRnDRootHashOld() ) != 0 )
            {
            // The RI CA cert is not signed by CMLA root
            MACLOGLIT( "The RI CA cert is not signed by CMLA (production or R&D) root" )
            isCmlaRoot = EFalse;
            }
        else
            {
            // We are running on a CMLA device and
            // RI certificate is signed by the CMLA (production or R&D) root
            // -> Verify still that the OCSP chain is signed by CMLA
            MACLOGLIT( "RI certificate is signed by the CMLA root" )
            }
        CleanupStack::PopAndDestroy( encodedRoot );
        CleanupStack::PopAndDestroy( hash );
        __UHEAP_MARKEND;
        }
    MACLOGLITDETAIL( "--> IsCmlaRootL" )
    return isCmlaRoot;
    }
#endif


// ---------------------------------------------------------------------------
// ValidateNonceL
// ---------------------------------------------------------------------------
//
LOCAL_C TBool ValidateNonceL( const COCSPResponse& aResponse,
    const TDesC8& aRegReqNonce )
    {
    // check that nonce in OCSP response equals to the nonce sent in Reg Request
    TBool ret( EFalse );
    const TPtrC8 * nonce( aResponse.DataElementEncoding(
        COCSPResponse::ENonce ) );

    if ( !nonce )
        {
        // no nonce found -> don't update DRM Time
        MACLOGLIT( "No nonce in the OCSP response!" )
        ret = EFalse;
        }
    else if ( nonce->CompareF( aRegReqNonce ) != 0 )
        {
        // Maybe the nonce is in ASN.1 format?
        // -> Remove the first two bytes (type and size), and try again
        TASN1DecGeneric asn1Dec( *nonce );
        asn1Dec.InitL();
        TPtrC8 ptr( asn1Dec.GetContentDER() );
        if ( ptr.CompareF( aRegReqNonce ) != 0 )
            {
            MACLOGLIT( "Fatal server error: OCSP nonce != RegRequest nonce!" )
            User::Leave( KErrRoapServerFatal );
            }
        else
            {
            MACLOGLIT( "OCSP nonce == RegRequest nonce." )
            ret = ETrue;
            }
        MACLOGLDETAIL( "values ( OCSP nonce, request nonce  )" )
        MACLOGHEXDETAIL( nonce->Ptr(), nonce->Length() )
        MACLOGHEXDETAIL( aRegReqNonce.Ptr(), aRegReqNonce.Length() )
        }
    else
        {
        ret = ETrue;
        MACLOGLDETAIL( "values ( OCSP nonce, request nonce  )" )
        MACLOGHEXDETAIL( nonce->Ptr(), nonce->Length() )
        MACLOGHEXDETAIL( aRegReqNonce.Ptr(), aRegReqNonce.Length() )
        }
    return ret;
    }
// ---------------------------------------------------------------------------
// IsTimeUpdateAllowedL()
// ---------------------------------------------------------------------------
//
LOCAL_C TBool IsTimeUpdateAllowedL(
    const RPointerArray<HBufC8>& aTrustedRoots,
    const RPointerArray<HBufC8>& aRootCertificates,
    const RPointerArray<HBufC8>& aCertChain,
    const COCSPResponse& aOcspResponse,
    const TDesC8& aRegReqNonce )
    {
    MACLOGLITDETAIL( "::IsTimeUpdateAllowedL -->" )


    // check that nonce in OCSP response equals to the nonce sent in Reg Request
    TBool bIsAllowed( ValidateNonceL( aOcspResponse, aRegReqNonce ) );

    if ( !bIsAllowed )
        {
        MACLOGLIT( "No nonce. Not possible to update DRM time" )
        return bIsAllowed;
        }


    if ( !aCertChain.Count() )
        {
        MACLOGLIT( "Wrong input data - No certificates or OCSP responses" )
        bIsAllowed = EFalse;
        return bIsAllowed;
        }

    CX509Certificate* riCa = NULL;
    CX509Certificate* rootCert = NULL;
    // Get the last cert from the chain.
    // It should be signed by some of our trusted anchor
    riCa = CX509Certificate::NewLC( *( aCertChain[aCertChain.Count() - 1] ) );
    MACLOGLITDETAIL( "Selected as RI CA" ) // (last of given cert chain)
    CERTDETAIL( riCa )

    // iterate over root certificates and check, whether given
    // RI CA issuer matches name of the selected root
    for ( TInt i = 0; i < aRootCertificates.Count() && !rootCert; i++ )
        {
        rootCert = CX509Certificate::NewL( *aRootCertificates[i] );
        if ( !rootCert->SubjectName().ExactMatchL( riCa->IssuerName() ) )
            {
            delete rootCert;
            rootCert = NULL;
            }
        }

    if ( rootCert )
        {
        MACLOGLDETAIL( "Selected root CERT" )
        CERTDETAIL( rootCert )
        CleanupStack::PushL( rootCert );

        if ( IsCmlaRootL( aTrustedRoots, rootCert ) )
            {
            CX509Certificate* ocspCert = NULL;
            __UHEAP_MARK;
            // Try to verify OCSP cert chain with the root cert
            if ( VerifyOcspCertChainL( aOcspResponse, rootCert,
                DeleteAndSetNull<CX509Certificate> ( ocspCert ) ) )
                {
                MACLOGLITDETAIL( "OCSP cert chain verified with root cert" )
                MACLOGLIT( "DRM Time update allowed!" )
                bIsAllowed = ETrue;
                }
            // Try to verify OCSP cert chain with the RI CA cert

            else if ( VerifyOcspCertChainL( aOcspResponse, riCa,
                DeleteAndSetNull<CX509Certificate> ( ocspCert ) ) )
                {
                MACLOGLITDETAIL( "OCSP cert chain verified with RI CA cert" )
                // Check signing of RiCa
                bIsAllowed = riCa->VerifySignatureL(
                    rootCert->PublicKey().KeyData() );
                if ( !bIsAllowed )
                    {
                    MACLOGLITDETAIL( "RI CA cert verify failed" )
                    MACLOGLIT( "DRM time update not allowed!" )
                    }
                else
                    {
                    MACLOGLITDETAIL( "RI CA verified with root cert" )
                    MACLOGLIT( "DRM time update allowed" )
                    }
                }
            else
                {
                MACLOGLITDETAIL( "OCSP cert chain verify failed" )
                MACLOGLIT( "DRM Time update is not allowed!" )
                bIsAllowed = EFalse;
                }

            if ( ocspCert )
                {
                if ( !aOcspResponse.VerifySignatureL(
                    ocspCert->PublicKey().KeyData() ) )
                    {
                    MACLOGLIT( "OCSP response signature verification failed" )
                    bIsAllowed = EFalse;
                    }
                if ( bIsAllowed )
                    {
                    const TTime timeCandidate( aOcspResponse.ProducedAt() );
                    if ( !ocspCert->ValidityPeriod().Valid( timeCandidate ) )
                        {
                        MACLOGLIT( "OCSP responder cert expired in respect to proposed new time" )
                        bIsAllowed = EFalse;
                        }
                    }
                }
            DeleteAndSetNull<CX509Certificate> ( ocspCert );
            __UHEAP_MARKEND;
            }
        CleanupStack::PopAndDestroy( rootCert );
        rootCert = NULL;
        }
    CleanupStack::PopAndDestroy( riCa );
    riCa = NULL;
    MACLOGLITDETAIL( "--> ::IsTimeUpdateAllowedL" )
    return bIsAllowed;
    }

//LOCAL CLASS DECLARATION
// Used for lazy connecting to clock server (only connected if needed)
class LazyClockClient
    {
public:
    inline LazyClockClient();
    inline virtual ~LazyClockClient();
    inline void GetSecureTimeL( TTime& aTime, TInt& aZone,
        DRMClock::ESecurityLevel& aLevel );
    inline void UpdateSecureTimeL( const TTime& aTime, const TInt aZone );
    inline void Close();
private:
    inline void ConnectL();
    TBool iConnected;
    RDRMClockClient iClock;
    };

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
inline LazyClockClient::LazyClockClient() :
    iConnected( EFalse )
    {
    }

// ---------------------------------------------------------------------------
inline LazyClockClient::~LazyClockClient()
    {
    Close();
    }

// ---------------------------------------------------------------------------
inline void LazyClockClient::Close()
    {
    iClock.Close();
    iConnected = EFalse;
    }

// ---------------------------------------------------------------------------
inline void LazyClockClient::ConnectL()
    {
    if ( !iConnected )
        {
        User::LeaveIfError( iClock.Connect() );
        iConnected = ETrue;
        }
    }

// ---------------------------------------------------------------------------
inline void LazyClockClient::GetSecureTimeL( TTime& aTime, TInt& aZone,
    DRMClock::ESecurityLevel& aLevel )
    {
    ConnectL();
    User::LeaveIfError( iClock.GetSecureTime( aTime, aZone, aLevel ) );
    }

// ---------------------------------------------------------------------------
inline void LazyClockClient::UpdateSecureTimeL( const TTime& aTime,
    const TInt aZone )
    {
    ConnectL();
    User::LeaveIfError( iClock.UpdateSecureTime( aTime, aZone ) );
    }


// CRoapStorageSession methods

// ---------------------------------------------------------------------------
// CRoapStorageSession::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CRoapStorageSession* CRoapStorageSession::NewL()
    {
    CRoapStorageSession* self = new ( ELeave ) CRoapStorageSession();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::~CRoapStorageSession
// Destructor.
// ---------------------------------------------------------------------------
//
CRoapStorageSession::~CRoapStorageSession()
    {
    delete iPreparedData;
    delete iKeyStorage;
    delete iLazyClock;
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::ServiceL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::ServiceL( const RMessage2& aMessage )
    {
    DispatchL( aMessage );
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::DispatchL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::DispatchL( const RMessage2& aMessage )
    {
    switch ( aMessage.Function() )
        {
        case EAddRiContext:
            {
            AddRiContextL( aMessage );
            break;
            }
        case EAddDomainContext:
            {
            AddDomainContextL( aMessage );
            break;
            }
        case EGetRiContext:
            {
            GetRiContextL( aMessage );
            break;
            }
        case EGetDomainContext:
            {
            GetDomainContextL( aMessage );
            break;
            }
        case EGetData:
            {
            GetDataL( aMessage );
            break;
            }
        case EDeleteRiContext:
            {
            DeleteRiContextL( aMessage );
            break;
            }
        case EDeleteDomainContext:
            {
            DeleteDomainContextL( aMessage );
            break;
            }
        case EDeleteExpiredRIs:
            {
            DeleteExpiredRIsL( aMessage );
            break;
            }
        case EDeleteExpiredDomains:
            {
            DeleteExpiredDomainsL( aMessage );
            break;
            }
        case EWhiteListCheck:
            {
            WhiteListUrlExistsL( aMessage );
            break;
            }
        case EGetPublicKey:
            {
            GetDevicePublicKeyDerL( aMessage );
            break;
            }
        case EGetCertificates:
            {
            GetDeviceCertificateChainL( aMessage );
            break;
            }
        case ESignMessage:
            {
            SignL( aMessage );
            break;
            }
        case ESelectRoot:
            {
            SelectTrustedRootL( aMessage );
            break;
            }
        case EActivateRoot:
            {
            ActivateTrustedRootL( aMessage );
            break;
            }
        case EGetTrustedRoots:
            {
            GetTrustedRootsL( aMessage );
            break;
            }
        case EGetRootCert:
            {
            GetRootCertificateL( aMessage );
            break;
            }
        case EDeleteExpired:
            {
            DeleteExpiredContextsL( aMessage );
            break;
            }
        case EDeleteAll:
            {
            DeleteAllL( aMessage );
            break;
            }
        case ERsaSign:
            {
            RsaSignL( aMessage );
            break;
            }
#ifdef RD_DRM_METERING
        case EGetMeteringData:
            {
            GetMeteringDataL( aMessage );
            break;
            }
        case EDeleteMeteringData:
            {
            DeleteMeteringDataL( aMessage );
            break;
            }
        case ERetrieveAllRIContexts:
            {
            GetAllRIContextsL( aMessage );
            break;
            }
        case EUpdateRIContext:
            {
            UpdateRIContextL( aMessage );
            break;
            }
#endif // RD_DRM_METERING
        case EUpdateDrmTime:
            {
            UpdateDrmTimeL( aMessage );
            break;
            }
        case EVerifyOcspResponses:
            {
            VerifyOcspResponsesL( aMessage );
            break;
            }
        case EGetOcspResponderId:
            {
            GetOcspResponderIdL( aMessage );
            break;
            }
        default:
            MACLOGLIT( "CRoapStorageSession::ServiceL: Invalid command" )
            PanicClient( aMessage, EPanicBadFunction );
        }
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::PanicClient
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::PanicClient(
    const RMessage2& aMessage,
    TPanic aReason )
    {
    _LIT( KPanicCategory, "RoapStorageSession" );
    if ( !aMessage.IsNull() )
        {
        aMessage.Panic( KPanicCategory, aReason );
        }
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::ServiceError
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::ServiceError(
    const RMessage2& aMessage,
    TInt aError )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::ServiceError -->" )

    if ( aError == KErrBadDescriptor )
        {
        PanicClient( aMessage, EPanicBadDescriptor );
        }
    else
        {
        CSession2::ServiceError( aMessage, aError );
        }
    MACLOGLITDETAIL( "--> CRoapStorageSession::ServiceError" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::CRoapStorageSession
// Default constructor.
// ---------------------------------------------------------------------------
//
CRoapStorageSession::CRoapStorageSession()
    {
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::ConstructL
// Second phase constructor. Initializes the log tool in DRM internal testing.
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::ConstructL()
    {
    iKeyStorage = DrmKeyStorageNewL();
    iKeyStorage->SelectDefaultRootL();
    iDeleteExpired = ETrue;
    iLazyClock = new ( ELeave ) LazyClockClient();
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::AddRiContextL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::AddRiContextL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::AddRiContextL -->" )
    CDRMRIContext* context = NULL;
    HBufC8* contextData = NULL;
    TPtr8 data( NULL, 0 );
    TInt size = 0;

    if ( iDeleteExpired )
        {
        TTime drmTime;
        DRMClock::ESecurityLevel level;
        TInt zone = KTzZulu;
        iLazyClock->GetSecureTimeL( drmTime, zone, level );

        if ( level == DRMClock::KSecure )
            {
            ROAPDB->DeleteExpiredRightsIssuerContextsL( drmTime );
            ROAPDB->DeleteExpiredDomainContextsL( drmTime );
            iDeleteExpired = EFalse;
            }
        }

    size = User::LeaveIfError( IPCGETDESLEN0 );
    contextData = HBufC8::NewMaxLC( size );
    data.Set( const_cast<TUint8*> ( contextData->Ptr() ), 0, size );
    IPCREAD0L( data );
    MACLOGLITDETAIL( "Serialized RI context" )
    MACLOGHEXDETAIL( contextData->Ptr(), contextData->Length() )

    context = CDRMRIContext::NewLC();
    context->ImportL( data );
    ROAPDB->StoreRightsIssuerContextL( *context );

    CleanupStack::PopAndDestroy( context );
    CleanupStack::PopAndDestroy( contextData );
    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( "--> CRoapStorageSession::AddRiContextL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::AddDomainContextL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::AddDomainContextL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::AddDomainContextL -->" )
    CDRMDomainContext* context = NULL;
    HBufC8* contextData = NULL;
    HBufC8* domainElements = NULL;
    HBufC8* macs = NULL;
    TPtr8 data( NULL, 0 );
    TInt size = 0;
    TKeyTransportScheme transScheme;
    TPckg<TKeyTransportScheme> package( transScheme );

    HBufC8* plainDomainKey = NULL;
    HBufC8* plainMacKey = NULL;

    if ( iDeleteExpired )
        {
        TTime drmTime;
        DRMClock::ESecurityLevel level;
        TInt zone = KTzZulu;
        iLazyClock->GetSecureTimeL( drmTime, zone, level );

        if ( level == DRMClock::KSecure )
            {
            ROAPDB->DeleteExpiredRightsIssuerContextsL( drmTime );
            ROAPDB->DeleteExpiredDomainContextsL( drmTime );
            iDeleteExpired = EFalse;
            }
        }

    RPointerArray<HBufC8> plainDomainKeys;
    CleanupResetAndDestroyPushL( plainDomainKeys );

    RPointerArray<HBufC8> plainMacKeys;
    CleanupResetAndDestroyPushL( plainMacKeys );

    size = IPCGETDESLEN0;

    if ( size <= 0 )
        {
        User::Leave( KErrArgument );
        }

    contextData = HBufC8::NewMaxLC( size );
    data.Set( const_cast<TUint8*> ( contextData->Ptr() ), 0, size );
    IPCREAD0L( data );
    MACLOGLITDETAIL( "Serialized domain context" )
    MACLOGHEXDETAIL( contextData->Ptr(), contextData->Length() )

    context = CDRMDomainContext::NewLC();
    context->ImportL( data );

    TPtrC8 domainId = context->DomainID();

    if ( domainId.Length() <= 0 )
        {
        User::Leave( KErrArgument );
        }

    IPCREAD1L( package );

    size = IPCGETDESLEN2;

    if ( size <= 0 )
        {
        User::Leave( KErrArgument );
        }

    macs = HBufC8::NewLC( size );
    data.Set( macs->Des() );
    IPCREAD2L( data );

    size = IPCGETDESLEN3;

    if ( size <= 0 )
        {
        User::Leave( KErrArgument );
        }

    domainElements = HBufC8::NewLC( size );
    data.Set( domainElements->Des() );
    IPCREAD3L( data );

    const RPointerArray<HBufC8>& domainKeys( context->DomainKeys() );

    for ( TInt i = 0; i < domainKeys.Count(); i++ )
        {
        UnwrapDomainKeyL( iKeyStorage, *domainKeys[i], plainDomainKey,
            plainMacKey, transScheme );
        TInt ret( plainDomainKeys.Append( plainDomainKey ) );
        if ( ret )
            {
            // Append did not succeed,
            // delete buffers not in CleanupStack and leave.
            delete plainDomainKey;
            plainDomainKey = NULL;
            delete plainMacKey;
            plainMacKey = NULL;
            User::LeaveIfError( ret );
            }
        CleanupStack::PushL( plainMacKey );
        plainMacKeys.AppendL( plainMacKey );
        CleanupStack::Pop( plainMacKey );
        }

    if ( !VerifyMacL( *domainElements, *macs, plainMacKeys ) )
        {
        User::Leave( KErrRoapServerFatal );
        }

    context->SetDomainKeysL( plainDomainKeys );

    ROAPDB->StoreDomainContextL( *context );

    CleanupStack::PopAndDestroy( domainElements );
    CleanupStack::PopAndDestroy( macs );
    CleanupStack::PopAndDestroy( context );
    CleanupStack::PopAndDestroy( contextData );
    CleanupStack::PopAndDestroy( &plainMacKeys );
    CleanupStack::PopAndDestroy( &plainDomainKeys );
    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( "--> CRoapStorageSession::AddDomainContextL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::GetRiContextL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::GetRiContextL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::GetRiContextL -->" )
    CDRMRIContext* context = CDRMRIContext::NewLC();
    HBufC8* riId = NULL;
    TInt size = 0;
    TPtr8 data( NULL, 0 );

    TPckg<TInt> package( size );
    riId = HBufC8::NewLC( User::LeaveIfError( IPCGETDESLEN1 ) );
    data.Set( riId->Des() );
    IPCREAD1L( data );
    if ( riId->Length() != SHA1_HASH )
        {
        User::Leave( KErrArgument );
        }
    context->SetRIIDL( *riId );
    ROAPDB->FetchRightsIssuerContextL( *context );
    size = context->Size();
    IPCWRITE0L( package );
    delete iPreparedData;
    iPreparedData = NULL;
    iPreparedData = context->ExportL();

    CleanupStack::PopAndDestroy( riId ); // riId, context
    CleanupStack::PopAndDestroy( context ); // riId, context
    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( " --> CRoapStorageSession::GetRiContextL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::GetDomainContextL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::GetDomainContextL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::GetDomainContextL -->" )
    RThread thread;
    aMessage.ClientL( thread );
    CleanupClosePushL( thread );

    CDRMDomainContext* context = CDRMDomainContext::NewLC();
    HBufC8* domainId = NULL;
    TInt size = 0;
    TPtr8 data( NULL, 0 );
    _LIT_SECURITY_POLICY_V0(vidCheck, VID_DEFAULT);
    // Check Default VID

    TPckg<TInt> package( size );
    domainId = HBufC8::NewLC( User::LeaveIfError( IPCGETDESLEN1 ) );
    data.Set( domainId->Des() );
    IPCREAD1L( data );
    if ( domainId->Length() < KMinDomainIdLength || domainId->Length()
        > SHA1_HASH )
        {
        User::Leave( KErrArgument );
        }

    context->SetDomainIDL( *domainId );
    ROAPDB->FetchDomainContextL( *context );
    if ( !vidCheck.CheckPolicy( thread ) )
        {
        // Remove sensitive data since trusted vendor id not present
        RPointerArray<HBufC8> emptyDomainKeys;
        CleanupResetAndDestroyPushL( emptyDomainKeys );
        emptyDomainKeys.Reset();
        context->SetDomainKeysL( emptyDomainKeys );
        CleanupStack::PopAndDestroy( &emptyDomainKeys );
        }
    size = context->Size();
    IPCWRITE0L( package );
    delete iPreparedData;
    iPreparedData = NULL;
    iPreparedData = context->ExportL();

    CleanupStack::PopAndDestroy( domainId );
    CleanupStack::PopAndDestroy( context );
    CleanupStack::PopAndDestroy( &thread );
    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( " --> CRoapStorageSession::GetDomainContextL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::GetDataL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::GetDataL( const RMessage2& aMessage )
    {
    if ( !iPreparedData )
        {
        User::Leave( KErrNotReady );
        }

    IPCWRITE0L( iPreparedData->Des() );

    delete iPreparedData;
    iPreparedData = NULL;

    aMessage.Complete( KErrNone );
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::DeleteRiContextL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::DeleteRiContextL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::DeleteRiContextL -->" )
    HBufC8* riId = NULL;
    TPtr8 data( NULL, 0 );

    SanitizeL( aMessage.GetDesLength( 0 ) );

    riId = HBufC8::NewLC( IPCGETDESLEN0 );
    data.Set( riId->Des() );
    IPCREAD0L( data );

    if ( riId->Length() != SHA1_HASH )
        {
        User::Leave( KErrArgument );
        }

    ROAPDB->DeleteRightsIssuerContextL( *riId );

    CleanupStack::PopAndDestroy( riId );
    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( "--> CRoapStorageSession::DeleteRiContextL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::DeleteDomainContextL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::DeleteDomainContextL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::DeleteDomainContextL -->" )
    HBufC8* domainId = NULL;
    TPtr8 data( NULL, 0 );

    SanitizeL( aMessage.GetDesLength( 0 ) );

    domainId = HBufC8::NewLC( IPCGETDESLEN0 );
    data.Set( domainId->Des() );
    IPCREAD0L( data );

    if ( domainId->Length() < KMinDomainIdLength || domainId->Length()
        > SHA1_HASH )
        {
        User::Leave( KErrArgument );
        }

    ROAPDB->DeleteDomainContextL( *domainId );

    CleanupStack::PopAndDestroy( domainId );
    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( "--> CRoapStorageSession::DeleteDomainContextL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::DeleteExpiredRIsL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::DeleteExpiredRIsL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::DeleteExpiredRIsL -->" )
    TTime time;
    TPckg<TTime> package( time );

    IPCREAD0L( package );
    ROAPDB->DeleteExpiredRightsIssuerContextsL( time );

    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( "--> CRoapStorageSession::DeleteExpiredRIsL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::DeleteExpiredDomainsL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::DeleteExpiredDomainsL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::DeleteExpiredDomainsL -->" )
    TTime time;
    TPckg<TTime> package( time );

    IPCREAD0L( package );
    ROAPDB->DeleteExpiredDomainContextsL( time );

    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( "--> CRoapStorageSession::DeleteExpiredDomainsL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::WhiteListUrlExistsL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::WhiteListUrlExistsL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::WhiteListURLExistsL -->" )
    HBufC8* url = NULL;
    TPtr8 data( NULL, 0 );
    TBool exists = EFalse;

    __UHEAP_MARK;
    SanitizeL( aMessage.GetDesLength( 0 ) );
    User::LeaveIfError( IPCGETDESLEN1 );
    TBool fromPreConfiguredWhiteList( EFalse );
    TPckg<TBool> preConfiguredInOutParam( fromPreConfiguredWhiteList );

    url = HBufC8::NewLC( IPCGETDESLEN0 );
    data.Set( url->Des() );
    IPCREAD0L( data );
    IPCREAD1L( preConfiguredInOutParam );
    exists = ROAPDB->WhiteListURLExistsL( *url );

    if ( exists )
        {
        fromPreConfiguredWhiteList = EFalse;
        }
    else if ( fromPreConfiguredWhiteList )
        {
        TPtrC8 whitelistElement( NULL, 0 );
        HBufC* buffer = HBufC::NewLC( KMaxWhiteListLen );
        HBufC8* whitelist = HBufC8::NewLC( KMaxWhiteListLen );

        TPtr ptr( NULL, 0 );
        ptr.Set( buffer->Des() );
        CRepository* repository = CRepository::NewLC( KCRUidRoapHandler );
        repository->Get( KRoapHandlerRegistrationWhitelist, ptr );
        data.Set( whitelist->Des() );
        data.Copy( ptr );
        TLex8 lex( *whitelist );

        TUriParser8 uri;
        uri.Parse( *url );

        const TDesC8& host = uri.Extract( EUriHost );
        while ( !exists && !lex.Eos() )
            {
            whitelistElement.Set( lex.NextToken() );
            if ( host.Right( whitelistElement.Length() ).CompareF(
                whitelistElement ) == 0 )
                {
                exists = ETrue;
                }
            }
        fromPreConfiguredWhiteList = exists;
        CleanupStack::PopAndDestroy( repository );
        CleanupStack::PopAndDestroy( whitelist );
        CleanupStack::PopAndDestroy( buffer );
        }

    CleanupStack::PopAndDestroy( url );
    IPCWRITE1L( preConfiguredInOutParam );
    exists ? aMessage.Complete( KErrNone ) : aMessage.Complete( KErrNotFound );
    __UHEAP_MARKEND;
    MACLOGLITDETAIL( "--> CRoapStorageSession::WhiteListURLExistsL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::GetDevicePublicKeyDerL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::GetDevicePublicKeyDerL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::GetDevicePublicKeyDerL -->" )
    HBufC8* publicKey = NULL;
    TInt size = 0;
    TPtr8 data( NULL, 0 );
    TPckg<TInt> package( size );
    CX509Certificate* cert = NULL;
    RPointerArray<HBufC8> certChain;

    CleanupResetAndDestroyPushL( certChain );

    iKeyStorage->GetCertificateChainL( certChain );
    if ( certChain.Count() > 0 )
        cert = CX509Certificate::NewL( *certChain[0] );
    else
        User::Leave( KErrNotFound );
    CleanupStack::PopAndDestroy( &certChain );
    CleanupStack::PushL( cert );
    publicKey = cert->DataElementEncoding(
        CX509Certificate::ESubjectPublicKeyInfo )->AllocLC();
    size = publicKey->Size();
    IPCWRITE0L( package );
    delete iPreparedData;
    iPreparedData = NULL;
    iPreparedData = publicKey;
    CleanupStack::Pop( publicKey );
    CleanupStack::PopAndDestroy( cert );
    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( "--> CRoapStorageSession::GetDevicePublicKeyDerL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::GetDeviceCertificateChainL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::GetDeviceCertificateChainL(
    const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::GetCertificateChainL -->" )
    RPointerArray<HBufC8> certChain;
    HBufC8* certChainBuf = NULL;
    TInt bufferSize = 0;
    TPckg<TInt> package( bufferSize );

    CleanupResetAndDestroyPushL( certChain );

    iKeyStorage->GetCertificateChainL( certChain );
    certChainBuf = ArrayToBufferLC( certChain );
    CleanupStack::Pop( certChainBuf );
    CleanupStack::PopAndDestroy( &certChain );
    CleanupStack::PushL( certChainBuf );
    bufferSize = certChainBuf->Size();
    IPCWRITE0L( package );
    delete iPreparedData;
    iPreparedData = certChainBuf;
    CleanupStack::Pop( certChainBuf );
    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( "--> CRoapStorageSession::GetCertificateChainL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::SignL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::SignL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::SignL -->" )
    HBufC8* hash = NULL;
    HBufC8* signature = NULL;
    TInt size = 0;
    TPtr8 data( NULL, 0 );
    TPckg<TInt> package( size );

    if ( IPCGETDESLEN1 <= 0 || IPCGETDESLEN1 >= KMaxTInt / 2 )
        {
        User::Leave( KErrArgument );
        }

    hash = HBufC8::NewLC( IPCGETDESLEN1 );

    data.Set( hash->Des() );
    IPCREAD1L( data );
    signature = OmaCrypto::RsaPssSignHashL( iKeyStorage, *hash );
    CleanupStack::PushL( signature );
    size = signature->Size();
    IPCWRITE0L( package );
    delete iPreparedData;
    iPreparedData = signature;
    CleanupStack::Pop( signature );
    CleanupStack::PopAndDestroy( hash );
    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( "--> CRoapStorageSession::SignL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::RsaSignL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::RsaSignL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::RsaSignL -->" )
    HBufC8* hash = NULL;
    HBufC8* signature = NULL;
    TInt size = 0;
    TPtr8 data( NULL, 0 );
    TPckg<TInt> package( size );

    if ( IPCGETDESLEN1 <= 0 || IPCGETDESLEN1 >= KMaxTInt / 2 )
        {
        User::Leave( KErrArgument );
        }

    hash = HBufC8::NewLC( IPCGETDESLEN1 );

    data.Set( hash->Des() );
    IPCREAD1L( data );
    signature = iKeyStorage->RsaSignL( data );
    CleanupStack::PushL( signature );
    size = signature->Size();
    IPCWRITE0L( package );
    delete iPreparedData;
    iPreparedData = signature;
    CleanupStack::Pop( signature );
    CleanupStack::PopAndDestroy( hash );
    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( "--> CRoapStorageSession::RsaSignL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::SelectTrustedRootL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::SelectTrustedRootL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::SelectTrustedRootL -->" )
    RPointerArray<HBufC8> riRoots;
    RPointerArray<HBufC8> deviceRoots;
    HBufC8* rootsBuf = NULL;
    TPtr8 data( NULL, 0 );
    TBool commonRootFound = EFalse;
    TInt i = 0;
    TInt j = 0;

    CleanupResetAndDestroyPushL( riRoots );
    CleanupResetAndDestroyPushL( deviceRoots );

    SanitizeL( aMessage.GetDesLength( 0 ) );
    rootsBuf = HBufC8::NewLC( IPCGETDESLEN0 );

    data.Set( rootsBuf->Des() );
    IPCREAD0L( data );
    riRoots = BufferToArrayL( *rootsBuf );
    iKeyStorage->GetTrustedRootsL( deviceRoots );

    while ( i < deviceRoots.Count() && !commonRootFound )
        {
        j = 0;
        while ( j < riRoots.Count() && !commonRootFound )
            {
            if ( deviceRoots[i]->CompareF( *riRoots[j] ) == KErrNone )
                {
                commonRootFound = ETrue;
                }
            ++j;
            }
        ++i;
        }
    if ( !commonRootFound )
        {
        User::Leave( KErrNotFound );
        }

    const TPtrC8 selectedRoot( *( deviceRoots[i - 1] ) );
    iKeyStorage->SelectTrustedRootL( selectedRoot );
    IPCWRITE1L( selectedRoot );
    MACLOGLITDETAIL( "Selected trusted root" )
    MACLOGHEXDETAIL( selectedRoot.Ptr(), selectedRoot.Length() )

    CleanupStack::PopAndDestroy( rootsBuf );
    CleanupStack::PopAndDestroy( &deviceRoots );
    CleanupStack::PopAndDestroy( &riRoots );
    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( "--> CRoapStorageSession::SelectTrustedRootL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::ActivateTrustedRootL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::ActivateTrustedRootL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::ActivateTrustedRootL -->" )
    TBuf8<SHA1_HASH> trustedRoot;

    if ( IPCGETDESLEN0 <= 0 )
        {
        // select default root
        iKeyStorage->SelectTrustedRootL( KNullDesC8 );
        }
    else
        {
        // select the root indicated by the client
        IPCREAD0L( trustedRoot );
        if ( trustedRoot.Length() != SHA1_HASH )
            {
            User::Leave( KErrArgument );
            }
        iKeyStorage->SelectTrustedRootL( trustedRoot );
        }
    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( "--> CRoapStorageSession::ActivateTrustedRootL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::GetTrustedRootsL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::GetTrustedRootsL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::GetTrustedRootsL -->" )
    RPointerArray<HBufC8> deviceRoots;
    TInt size = 0;
    HBufC8* rootsData = NULL;

    CleanupResetAndDestroyPushL( deviceRoots );

    TPckg<TInt> package( size );
    iKeyStorage->GetTrustedRootsL( deviceRoots );
    rootsData = ArrayToBufferLC( deviceRoots );
    size = rootsData->Size();
    IPCWRITE0L( package );
    delete iPreparedData;
    iPreparedData = NULL;
    iPreparedData = rootsData;

    CleanupStack::Pop( rootsData );
    CleanupStack::PopAndDestroy( &deviceRoots );
    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( "--> CRoapStorageSession::GetTrustedRootsL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::GetRootCertificateL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::GetRootCertificateL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::GetRootCertificateL -->" )
    HBufC8* rootCertBuf( NULL );
    HBufC* subject( NULL );
    TInt size( 0 );
    TPtr data( NULL, 0 );
    RPointerArray<HBufC8> rootCerts;
    TPckg<TInt> package( size );
    CX509Certificate* rootCert( NULL );
    HBufC* rootName( NULL );
    TInt i = 0;

    SanitizeL( IPCGETDESLEN1 );
    subject = HBufC::NewLC( User::LeaveIfError( IPCGETDESLEN1 ) );
    data.Set( subject->Des() );
    IPCREAD1L( data );

    iKeyStorage->GetRootCertificatesL( rootCerts );

    CleanupResetAndDestroyPushL( rootCerts );

    for ( i = 0; i < rootCerts.Count() && !rootCertBuf; i++ )
        {
        rootCert = CX509Certificate::NewLC( *( rootCerts[i] ) );
        rootName = rootCert->IssuerName().DisplayNameL();
        CleanupStack::PushL( rootName );
        const TInt compareResult( rootName->CompareF( *subject ) );
        CleanupStack::PopAndDestroy( rootName );
        CleanupStack::PopAndDestroy( rootCert );
        if ( compareResult == KErrNone )
            {
            rootCertBuf = rootCerts[i]->AllocLC();
            }
        }

    if ( !rootCertBuf )
        {
        User::Leave( KErrNotFound );
        }

    size = rootCertBuf->Size();
    IPCWRITE0L( package );
    delete iPreparedData;
    iPreparedData = NULL;
    iPreparedData = rootCertBuf;

    CleanupStack::Pop( rootCertBuf );
    CleanupStack::PopAndDestroy( &rootCerts );
    CleanupStack::PopAndDestroy( subject );
    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( "--> CRoapStorageSession::GetRootCertificateL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::DeleteExpiredContextsL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::DeleteExpiredContextsL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::DeleteExpiredContextsL -->" )
    TTime time;
    TPckg<TTime> package( time );

    IPCREAD0L( package );
    ROAPDB->DeleteExpiredRightsIssuerContextsL( time );
    ROAPDB->DeleteExpiredDomainContextsL( time );

    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( "--> CRoapStorageSession::DeleteExpiredContextsL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::DeleteAllL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::DeleteAllL( const RMessage2& aMessage )
    {

    MACLOGLITDETAIL( "CRoapStorageSession::DeleteAllL -->" )
#ifndef RD_MULTIPLE_DRIVE

    ROAPDB->DeleteAllL( KRIContextFile(), KDomainContextFile() );

#else //RD_MULTIPLE_DRIVE
    RFs fs;
    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );

    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    fs.DriveToChar( driveNumber, driveLetter );
    CleanupStack::PopAndDestroy( &fs );

    TFileName riContextFile;
    riContextFile.Copy( KRIContextFileName );
    __ASSERT_ALWAYS( riContextFile.Length()>0, User::Invariant() );
    riContextFile[0] = ( TUint )driveLetter;

    TFileName domainContextFile;
    domainContextFile.Copy( KDomainContextFileName );
    __ASSERT_ALWAYS( domainContextFile.Length()>0, User::Invariant() );
    domainContextFile[0] = ( TUint )driveLetter;

    ROAPDB->DeleteAllL( riContextFile, domainContextFile );

#endif

    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( "--> CRoapStorageSession::DeleteAllL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::GetMeteringDataL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::GetMeteringDataL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::GetMeteringDataL -->" )
#ifndef RD_DRM_METERING
    aMessage.Complete( KErrNotSupported );
#else
    HBufC8* riId = NULL;
    HBufC8* meteringData = NULL;
    HBufC8* encryptedMeteringData = NULL;
    RDRMRightsClient client;
    TInt mekAndMakSize = 0;
    TInt meteringDataSize = 0;
    TPtr8 data( NULL, 0 );
    TPtr8 ptr( NULL, 0 );
    TPckg<TInt> MakMek( mekAndMakSize );
    TPckg<TInt> dataSize( meteringDataSize );
    TBuf8<KDCFKeySize> mac;
    TBuf8<KDCFKeySize> mek;
    TBuf8<KDCFKeySize> initializingVector;
    CRSAPublicKey* riPublicKey = NULL;
    CX509Certificate* cert = NULL;
    CSHA1* hasher = NULL;
    TBuf8<SHA1_HASH> encKeyHash;
    TX509KeyFactory factory;
    TKeyTransportScheme selectedAlgorithm = EOma;
    HBufC8* encryptedMekAndMak = NULL;
    CDRMRIContext* context = NULL;

    User::LeaveIfError( client.Connect() );
    CleanupClosePushL( client );

    SanitizeL( aMessage.GetDesLength( 0 ) );

    riId = HBufC8::NewLC( IPCGETDESLEN0 );
    data.Set( riId->Des() );
    IPCREAD0L( data );

    // generate mek
    mek.SetLength( OmaCrypto::KKeySize );
    TRandom::Random( mek );

    // generate mac
    mac.SetLength( OmaCrypto::KMacSize );
    TRandom::Random( mac );

    initializingVector.SetLength( KDCFKeySize );
    TRandom::Random( initializingVector );

    context = CDRMRIContext::NewLC();
    context->SetRIIDL( *riId );
    ROAPDB->FetchRightsIssuerContextL( *context );

    cert = CX509Certificate::NewLC( *( context->CertificateChain() )[0] );

    hasher = CSHA1::NewL();
    CleanupStack::PushL( hasher );
    // Calculate SHA1_HASH for RI public key
    hasher->Hash( *cert->DataElementEncoding(
        CX509Certificate::ESubjectPublicKeyInfo ) );
    encKeyHash.Append( hasher->Final() );
    CleanupStack::PopAndDestroy( hasher );

    riPublicKey = factory.RSAPublicKeyL( cert->PublicKey().KeyData() );
    CleanupStack::PopAndDestroy( cert );
    cert = NULL;
    CleanupStack::PushL( riPublicKey );

    // check if we are not using OMA algorithms
    for ( TInt i = 0; i < context->Algorithms().Count(); i++ )
        {
        if ( context->Algorithms()[i]->CompareF( KCmlaIp1() ) == KErrNone )
            {
            selectedAlgorithm = ECmlaIp1;
            break;
            }
        }

    // Wrap Mek and Mac with RI's public key
    if ( selectedAlgorithm == EOma )
        {
        encryptedMekAndMak = OmaCrypto::RsaKemKwsEncryptL( riPublicKey, mek,
            mac );
        }
    else
        { // CMLA
        encryptedMekAndMak = CmlaCrypto::CmlaIpEncryptL( selectedAlgorithm,
            riPublicKey, mek, mac );
        }
    CleanupStack::PopAndDestroy( riPublicKey );
    riPublicKey = NULL;
    CleanupStack::PopAndDestroy( context );
    context = NULL;

    CleanupStack::PushL( encryptedMekAndMak );

    mekAndMakSize = encryptedMekAndMak->Size();
    // Get the actual metering data!!!
    meteringData = client.GetMeteringDataL( *riId );

    if ( !meteringData )
        {
        MACLOGLIT( "No metering Data found from DB!:" )
        }
    else
        {
        MACLOGLIT( "Raw meteringData:" )
        MACLOGHEX( meteringData->Ptr(), meteringData->Length() )
        CleanupStack::PushL( meteringData );

        encryptedMeteringData = DrmAesCrypto::DrmAesEncryptL( mek,
            initializingVector, ETrue, meteringData->Des() );

        CleanupStack::PopAndDestroy( meteringData );
        meteringData = NULL;

        CleanupStack::PushL( encryptedMeteringData );

        MACLOGLIT( "Ciphered meteringData with 128-bit AESCBC:" )
        MACLOGHEX( encryptedMeteringData->Ptr(), encryptedMeteringData->Length())
        meteringDataSize = encryptedMeteringData->Size();
        }

    IPCWRITE1L( MakMek );
    IPCWRITE2L( dataSize );
    if ( iPreparedData )
        {
        delete iPreparedData;
        iPreparedData = NULL;
        }
    iPreparedData = HBufC8::NewL( OmaCrypto::KMacSize + SHA1_HASH
        + mekAndMakSize + meteringDataSize );
    ptr.Set( iPreparedData->Des() );
    ptr.Copy( mac );
    ptr.Append( encKeyHash );
    ptr.Append( *encryptedMekAndMak );
    if ( encryptedMeteringData )
        {
        ptr.Append( *encryptedMeteringData );
        CleanupStack::PopAndDestroy( encryptedMeteringData );
        }

    MACLOGLIT( "Final iPreparedData:" )
    MACLOGHEX( iPreparedData->Ptr(), iPreparedData->Length() )

    CleanupStack::PopAndDestroy( encryptedMekAndMak );
    CleanupStack::PopAndDestroy( riId );
    CleanupStack::PopAndDestroy( &client );
    // riId, client
    aMessage.Complete( KErrNone );
#endif //RD_DRM_METERING
    MACLOGLITDETAIL( "--> CRoapStorageSession::GetMeteringDataL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::DeleteMeteringDataL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::DeleteMeteringDataL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::DeleteMeteringDataL -->" )
#ifndef RD_DRM_METERING
    aMessage.Complete( KErrNotSupported );
#else
    HBufC8* riId = NULL;
    TPtr8 data( NULL, 0 );
    RDRMRightsClient client;

    User::LeaveIfError( client.Connect() );
    CleanupClosePushL( client );
    riId = HBufC8::NewLC( User::LeaveIfError( IPCGETDESLEN0 ) );
    data.Set( riId->Des() );
    IPCREAD0L( data );
    client.DeleteMeteringDataL( *riId );

    CleanupStack::PopAndDestroy( riId );
    CleanupStack::PopAndDestroy( &client );

    aMessage.Complete( KErrNone );
#endif // RD_DRM_METERING
    MACLOGLITDETAIL( "--> CRoapStorageSession::DeleteMeteringDataL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::GetAllRIContextsL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::GetAllRIContextsL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::GetAllRIContextsL -->" )
#ifndef RD_DRM_METERING
    aMessage.Complete( KErrNotSupported );
#else
    TInt size = 0;
    TPtr8 data( NULL, 0 );
    CDRMPointerArray<CDRMRIContext>* contexts = CDRMPointerArray<
        CDRMRIContext>::NewLC();
    contexts->SetAutoCleanup( ETrue );

    TPckg<TInt> package( size );

    ROAPDB->FetchAllRightsIssuerContextsL( *contexts );

    // Calculate the size of the output buffer:
    if ( contexts->Count() )
        {
        size = contexts->Count() * sizeof(TInt) + sizeof(TInt);
        for ( TInt i = 0; i < contexts->Count(); i++ )
            {
            size += ( *contexts )[i]->Size();
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    IPCWRITE0L( package );

    // Create a buffer of the proper size and export the data to the buffer:
    if ( iPreparedData )
        {
        delete iPreparedData;
        iPreparedData = NULL;
        }
    iPreparedData = HBufC8::NewMaxL( size );

    if ( !iPreparedData )
        {
        User::Leave( KErrNoMemory );
        }

    RMemWriteStream stream( ( TAny* )( iPreparedData->Ptr() ), size );
    CleanupClosePushL( stream );
    TInt value = 0;

    for ( TInt i = 0; i < contexts->Count(); i++ )
        {
        // Size:
        value = ( *contexts )[i]->Size();
        stream.WriteInt32L( value );

        // Data:
        ( *contexts )[i]->ExternalizeL( stream );
        }
    // Finishing size:
    value = 0;
    stream.WriteInt32L( value );

    CleanupStack::PopAndDestroy( 2, contexts ); // stream, contexts

    aMessage.Complete( KErrNone );
#endif // RD_DRM_METERING
    MACLOGLITDETAIL( "--> CRoapStorageSession::GetAllRIContextsL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::UpdateRIContextL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::UpdateRIContextL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::UpdateRIContextL -->" )
#ifndef RD_DRM_METERING
    aMessage.Complete( KErrNotSupported );
#else
    CDRMRIContext* context = NULL;
    HBufC8* contextData = NULL;
    TPtr8 data( NULL, 0 );
    TInt size = 0;

    size = User::LeaveIfError( IPCGETDESLEN0 );
    contextData = HBufC8::NewMaxLC( size );
    data.Set( const_cast<TUint8*> ( contextData->Ptr() ), 0, size );
    IPCREAD0L( data );
    context = CDRMRIContext::NewLC();
    context->ImportL( data );

    // Update RI context:
    ROAPDB->UpdateRightsIssuerContextL( *context );

    CleanupStack::PopAndDestroy( context );
    CleanupStack::PopAndDestroy( contextData );
    aMessage.Complete( KErrNone );
#endif // RD_DRM_METERING
    MACLOGLITDETAIL( "--> CRoapStorageSession::UpdateRIContextL" )
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::UpdateDrmTimeL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::UpdateDrmTimeL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::UpdateDrmTimeL -->" )
    COCSPResponseDecoder* responseDecoder = NULL;
    COCSPResponse* response = NULL;
    RPointerArray<HBufC8> ocspResponses;

    RPointerArray<HBufC8> riCertChain;

    TBool clockUpdated( EFalse );
    TPckg<TBool> retBuf( clockUpdated );

    // Some sanity checks
    const TInt argLen0( IPCGETDESLEN0 );
    const TInt argLen1( IPCGETDESLEN1 );
    const TInt argLen2( IPCGETDESLEN2 );
    SanitizeL( argLen0 );
    SanitizeL( argLen1 );
    SanitizeL( argLen2 );
    User::LeaveIfError( IPCGETDESLEN3 );

    RBuf8 readBuf;
    CleanupClosePushL( readBuf );
    readBuf.CreateL( Max( argLen0, Max( argLen1, argLen2 ) ) );

    readBuf.Zero();
    IPCREAD0L( readBuf );
    MACLOGLDETAIL( "---- read Ri Cert Chain ----" )
    MACLOGHEXDETAIL( readBuf.Ptr(), readBuf.Length() )
    riCertChain = BufferToArrayL( readBuf );
    CleanupResetAndDestroyPushL( riCertChain );

    readBuf.Zero();
    IPCREAD1L( readBuf );
    MACLOGLDETAIL( "---- read serialized OCSP responses ----" )
    MACLOGHEXDETAIL( readBuf.Ptr(), readBuf.Length() )
    ocspResponses = BufferToArrayL( readBuf );
    CleanupResetAndDestroyPushL( ocspResponses );

    readBuf.Zero();
    IPCREAD2L( readBuf );
    MACLOGLDETAIL( "---- read request nonce ----" )
    MACLOGHEXDETAIL( readBuf.Ptr(), readBuf.Length() )
    const TPtrC8 regReqNonce( readBuf ); // Do not modify readBuf after this.

    // Get trusted roots and root certificates.
    RPointerArray<HBufC8> trustedRoots;
    CleanupResetAndDestroyPushL( trustedRoots );
    iKeyStorage->GetTrustedRootsL( trustedRoots );
    RPointerArray<HBufC8> rootCertificates;
    CleanupResetAndDestroyPushL( rootCertificates );
    iKeyStorage->GetRootCertificatesL( rootCertificates );

    //First, check, if update is allowed
    if ( ocspResponses.Count() )
        {
        responseDecoder = COCSPResponseDecoder::NewL( *ocspResponses[0] );
        CleanupStack::PushL( responseDecoder );
        response = responseDecoder->TakeResponse(); // CRoapEng owns the response now
        User::LeaveIfNull( response );
        CleanupStack::PopAndDestroy( responseDecoder );
        responseDecoder = NULL;
        CleanupStack::PushL( response );
        if ( ::IsTimeUpdateAllowedL( trustedRoots, rootCertificates,
            riCertChain, *response, regReqNonce ) )
            {
            iLazyClock->UpdateSecureTimeL( response->ProducedAt(), KTzZulu );
            clockUpdated = ETrue;
            MACLOGLIT( "DRM time updated" )
            }

        CleanupStack::PopAndDestroy( response );
        }
    else
        {
        MACLOGLIT( "No OCSP responses present!" )
        // So wrong argument got
        }
    CleanupStack::PopAndDestroy( &rootCertificates );
    CleanupStack::PopAndDestroy( &trustedRoots );
    CleanupStack::PopAndDestroy( &ocspResponses );
    CleanupStack::PopAndDestroy( &riCertChain );
    CleanupStack::PopAndDestroy( &readBuf );
    IPCWRITE3L( retBuf );

    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( "--> CRoapStorageSession::UpdateDrmTimeL" )
    }

// ---------------------------------------------------------------------------
// BufferToArrayL
// Created buffer will contain
// <amount of elements:int32, element data1, element data2...>
// ---------------------------------------------------------------------------
//
LOCAL_C RPointerArray<HBufC8> BufferToArrayL( TDesC8& aBuffer )
    {
    TInt32 count = 0;
    HBufC8* element = NULL;
    RPointerArray<HBufC8> array;
    CleanupResetAndDestroyPushL( array );
    TInt size = aBuffer.Size();
    RMemReadStream stream( ( TAny* )( aBuffer.Ptr() ), size );
    CleanupClosePushL( stream );

    // amount of elements
    count = stream.ReadInt32L();

    // for each element in RPointerArray
    for ( TInt i = 0; i < count; i++ )
        {
        // Read the element and append it to array
        element = HBufC8::NewLC( stream, KMaxElementLength );
        array.AppendL( element );
        CleanupStack::Pop( element );
        }

    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::Pop( &array );
    return array;
    }

// ---------------------------------------------------------------------------
// ArrayToBuffer
// Created buffer will contain <amount of elements:int32, data1, data2 ...>
// ---------------------------------------------------------------------------
//
LOCAL_C HBufC8* ArrayToBufferLC( const RPointerArray<HBufC8>& aArray )
    {
    HBufC8* buffer = NULL;
    TInt32 sizeOfElements = 0;

    for ( TInt i = 0; i < aArray.Count(); i++ )
        {
        sizeOfElements += aArray[i]->Length() + sizeof(TInt); // sizeof(TInt) is for
        // element length info
        }
    // length of elements + amount of elements
    sizeOfElements += sizeof(TInt32);

    buffer = HBufC8::NewMaxLC( sizeOfElements );

    RMemWriteStream stream( ( TAny* )( buffer->Ptr() ), sizeOfElements );
    CleanupClosePushL( stream );

    stream.WriteInt32L( aArray.Count() );

    for ( TInt i = 0; i < aArray.Count(); i++ )
        {
        stream << *( aArray[i] );
        }

    CleanupStack::PopAndDestroy( &stream );

    return buffer;
    }

// ---------------------------------------------------------------------------
// UnwrapDomainKeyL
// ---------------------------------------------------------------------------
//
LOCAL_C void UnwrapDomainKeyL(
    MDrmKeyStorage* aKeyStorage,
    const TDesC8& aProtectedDomainKey,
    HBufC8*& aDomainKey,
    HBufC8*& aMacKey,
    TKeyTransportScheme& aTransportScheme )
    {
    MACLOGLITDETAIL( "::UnwrapDomainKeyL -->" )
    TBuf8<OmaCrypto::KMacSize> mac;
    TBuf8<OmaCrypto::KKeySize> dk;
    TPtrC8 macAndRek( 0, 0 );
    TPtrC8 wrappedCek( 0, 0 );

    if ( aTransportScheme == EOma )
        {
        OmaCrypto::RsaKemKwsDecryptL( aKeyStorage, aProtectedDomainKey, dk,
            mac );
        }
    else
        {
        CmlaCrypto::CmlaIpDecryptL( aTransportScheme, aKeyStorage,
            aProtectedDomainKey, dk, mac );
        }

    HBufC8* domainKey( dk.AllocLC() );
    aMacKey = mac.AllocL();
    CleanupStack::Pop( domainKey );
    aDomainKey = domainKey;
    MACLOGLITDETAIL( "--> ::UnwrapDomainKeyL" )
    }

// ---------------------------------------------------------------------------
// VerifyMac
// ---------------------------------------------------------------------------
//
LOCAL_C TBool VerifyMacL(
    TDesC8& aDomainElements,
    TDesC8& aMacs,
    RPointerArray<HBufC8>& aMacKeys )
    {
    MACLOGLITDETAIL( "::VerifyMacL -->" )
    __UHEAP_MARK;
    RPointerArray<HBufC8> domainArray;
    RPointerArray<HBufC8> macArray;
    CMessageDigest* hMac = NULL;
    HBufC8* domainKeyWithNs = NULL;
    TPtr8 domainKeyWithNsPtr( NULL, NULL );
    TPtrC8 hmac_value( KNullDesC8 );
    TInt index = 0;

    CleanupResetAndDestroyPushL( domainArray );
    CleanupResetAndDestroyPushL( macArray );

    domainArray = BufferToArrayL( aDomainElements );
    macArray = BufferToArrayL( aMacs );

    if ( domainArray.Count() != macArray.Count() || macArray.Count()
        != aMacKeys.Count() )
        {
        User::Leave( KErrRoapServerFatal );
        }

    //Verify macs
    for ( TInt i = 0; i < aMacKeys.Count(); i++ )
        {
        // Add ROAP namespaces
        domainKeyWithNs = HBufC8::NewLC( domainArray[i]->Length() + 2
            * KRoapXmlNs().Length() );
        domainKeyWithNsPtr.Set( domainKeyWithNs->Des() );
        index = domainArray[i]->Find( KRoapDomainKey() );
        if ( index == KErrNotFound )
            {
            index = domainArray[i]->Find( KRoapX509SPKIHash() );
            if ( index == KErrNotFound )
                {
                User::Leave( KErrCorrupt );
                }
            index += KRoapX509SPKIHash().Length();
            }
        else
            {
            index += KRoapDomainKey().Length();
            }
        domainKeyWithNsPtr.Copy( domainArray[i]->Left( index ) );
        domainKeyWithNsPtr.Append( KRoapXmlNs() );
        domainKeyWithNsPtr.Append( domainArray[i]->Right(
            domainArray[i]->Length() - index ) );

        hMac = CMessageDigestFactory::NewHMACLC( CMessageDigest::ESHA1,
            *aMacKeys[i] );
        hMac->Update( domainKeyWithNsPtr );

        hmac_value.Set( hMac->Final() );

        MACLOGLIT( "DomainKeyElement:" )
        MACLOGHEX( domainKeyWithNsPtr.Ptr(), domainKeyWithNsPtr.Length() )
        MACLOGLIT( "Calculated HMAC_value:" )
        MACLOGHEX( hmac_value.Ptr(), hmac_value.Length() )
        MACLOGLIT( "HMAC_value:" )
        MACLOGHEX( macArray[i]->Ptr(), macArray[i]->Length() )

        if ( hmac_value.CompareF( *macArray[i] ) != 0 )
            {
            // MAC validation failed
            User::Leave( KErrRightsServerMacFailed );
            }
        CleanupStack::PopAndDestroy( 2, domainKeyWithNs ); // hMac, domainKeyWithNs
        }

    CleanupStack::PopAndDestroy( &macArray );
    CleanupStack::PopAndDestroy( &domainArray );
    __UHEAP_MARKEND;
    MACLOGLITDETAIL( "--> ::VerifyMacL" )
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CRoapStorageSession::VerifyOcspResponsesL()
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::VerifyOcspResponsesL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::VerifyOcspResponsesL -->" )

    const TInt argLen0( IPCGETDESLEN0 );
    const TInt argLen1( IPCGETDESLEN1 );
    const TInt argLen2( IPCGETDESLEN2 );
    SanitizeL( argLen0 );
    SanitizeL( argLen1 );
    SanitizeL( argLen2 );
    User::LeaveIfError( IPCGETDESLEN3 );

    COCSPResponseDecoder* responseDecoder = NULL;
    COCSPResponse* response = NULL;
    COCSPResponseCertInfo* certInfo = NULL;
    COCSPCertID* certID = NULL;
    CX509Certificate* ocspCert = NULL;
    TBool result = EFalse;
    TInt certInfoCount = 0;
    TInt found = KErrNone;
    CX509Certificate* riCaCert( NULL );

    TPckg<TBool> package( result );

    RBuf8 readBuf;
    CleanupClosePushL( readBuf );
    readBuf.CreateL( Max( argLen0, Max( argLen1, argLen2 ) ) );

    // read arguments
    IPCREAD0L( readBuf );
    RPointerArray<HBufC8> ocspResponses( BufferToArrayL( readBuf ) );
    CleanupResetAndDestroyPushL( ocspResponses );
    readBuf.Zero();

    IPCREAD1L( readBuf );
    riCaCert = CX509Certificate::NewLC( readBuf );
    readBuf.Zero();

    IPCREAD2L( readBuf );
    RPointerArray<HBufC8> certSerialNums( BufferToArrayL( readBuf ) );
    CleanupResetAndDestroyPushL( certSerialNums );
    readBuf.Zero();

    // Check OcspResponses. Only the very first response is checked.
    if ( ocspResponses.Count() )
        {
        responseDecoder = COCSPResponseDecoder::NewL( *ocspResponses[0] );
        CleanupStack::PushL( responseDecoder );
        response = responseDecoder->TakeResponse();
        CleanupStack::PopAndDestroy( responseDecoder );
        User::LeaveIfNull( response );
        CleanupStack::PushL( response );

        if ( !response )
            {
            MACLOGLIT( "No OCSP certificate!" )
            User::Leave( KErrArgument );
            }

        result = VerifyOcspCertChainL( *response, riCaCert, ocspCert );

        CleanupStack::PushL( ocspCert );

        // validate OCSP response signature and
        // that all the RI cert in the chain is valid (status == good)
        if ( result )
            {
            result = response->VerifySignatureL(
                ocspCert->PublicKey().KeyData() );
            }

        if ( result )
            {
            certInfoCount = response->CertCount();
            if ( certSerialNums.Count() && result )
                {
                TBool isCertStatusFound = EFalse;
                for ( TInt i = 0; i < certInfoCount && result; i++ )
                    {
                    certInfo = response->CertInfo( i ); // certInfo is owned by response
                    certID = certInfo->CertID(); // certID is owned by certInfo
                    found = certSerialNums[0]->Compare(
                        certID->SerialNumber() );
                    if ( found == KErrNone )
                        {
                        isCertStatusFound = ETrue;
                        if ( certInfo->Status() != OCSP::EGood )
                            {
                            result = EFalse;
                            }
                        }
                    }
                if ( !isCertStatusFound )
                    {
                    result = EFalse;
                    }
                }
            }

        CleanupStack::PopAndDestroy( ocspCert );
        CleanupStack::PopAndDestroy( response );
        }
    CleanupStack::PopAndDestroy( &certSerialNums );
    CleanupStack::PopAndDestroy( riCaCert );
    CleanupStack::PopAndDestroy( &ocspResponses );
    CleanupStack::PopAndDestroy( &readBuf );

#ifdef _ROAP_TESTING
    if ( result )
        {
        MACLOGLIT( "OCSP response verification ok." )
        }
    else
        {
        MACLOGLIT( "OCSP response verification failed." )
        }
#endif

#ifdef _DISABLE_OCSP_CHECK
    result = ETrue;
#endif
    IPCWRITE3L( package );
    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( "--> CRoapStorageSession::VerifyOcspResponsesL" )
    }
// ---------------------------------------------------------------------------
// CRoapStorageSession::GetOcspResponderIdL
// ---------------------------------------------------------------------------
//
void CRoapStorageSession::GetOcspResponderIdL( const RMessage2& aMessage )
    {
    MACLOGLITDETAIL( "CRoapStorageSession::GetOCSPResponderIdL -->" )
    COCSPResponseDecoder* rd( NULL );
    COCSPResponse* response( NULL );

    RPointerArray<CX509Certificate> certArray;
    CX509Certificate* cert( NULL );
    CSHA1* hash( NULL );
    TInt pos( 0 );
    TInt size( 0 );
    TPckg<TInt> sizeArg( size );

    SanitizeL( IPCGETDESLEN0 );
    User::LeaveIfError( IPCGETDESLEN1 );
    CleanupResetAndDestroyPushL( certArray );

    HBufC8* riId( HBufC8::NewLC( IPCGETDESLEN0 ) );
    TPtr8 data( riId->Des() );
    IPCREAD0L( data );
    if ( riId->Length() != SHA1_HASH )
        {
        User::Leave( KErrArgument );
        }
    CDRMRIContext* context( CDRMRIContext::NewLC() );
    context->SetRIIDL( *riId );
    CleanupStack::Pop( context );
    CleanupStack::PopAndDestroy( riId );
    riId = NULL;
    CleanupStack::PushL( context );
    ROAPDB->FetchRightsIssuerContextL( *context );

    const RPointerArray<HBufC8>& ocspResponses( context->OCSPResponse() );
    // Fetch OCSP response from given RiId

    if ( !ocspResponses.Count() )
        {
        User::Leave( KErrRoapDevice );
        }

    rd = COCSPResponseDecoder::NewL( *ocspResponses[0] );
    CleanupStack::PushL( rd );
    response = rd->TakeResponse(); // CRoapEng owns the response now
    CleanupStack::PopAndDestroy( rd );
    rd = NULL;
    CleanupStack::PopAndDestroy( context );
    context = NULL;
    User::LeaveIfNull( response );

    CleanupStack::PushL( response );
    const TPtrC8* ocspCerts( response->SigningCerts() );

    TBool ocspResponseUsable( EFalse );
    // Get SecureTime from ClockServer
    TTime secureTime;
    TInt zone( KTzZulu );
    DRMClock::ESecurityLevel securityLevel( DRMClock::KInsecure );
    iLazyClock->GetSecureTimeL( secureTime, zone, securityLevel );

    if ( securityLevel == DRMClock::KSecure )
        {
        ocspResponseUsable = ETrue;
        }
    // Verify that OCSP responses are not expired
    for ( TInt i( 0 ); ocspResponseUsable && i < response->CertCount(); ++i )
        {
        const TTime* nextUpdate( response->CertInfo( i )->NextUpdate() );
        if ( !nextUpdate || *nextUpdate <= secureTime || response->CertInfo(
            i )->RevocationTime() )
            {
            ocspResponseUsable = EFalse;
            }
        }

    if ( ocspResponseUsable )
        {
        while ( ocspCerts && pos < ocspCerts->Length() )
            {
            cert = CX509Certificate::NewLC( *ocspCerts, pos );
            certArray.AppendL( cert );
            CleanupStack::Pop( cert );
            }
        ocspCerts = NULL;
        }
    CleanupStack::PopAndDestroy( response );
    response = NULL;

    if ( cert )
        {
        // hash the SubjectPublicKeyInfo element
        hash = CSHA1::NewL();
        CleanupStack::PushL( hash );
        hash->Hash( *cert->DataElementEncoding(
            CX509Certificate::ESubjectPublicKeyInfo ) );

        delete iPreparedData;
        iPreparedData = NULL;
        iPreparedData = hash->Final().AllocL();
        CleanupStack::PopAndDestroy( hash );
        size = iPreparedData->Length();
        MACLOGLIT( "Got OCSP responder" )
        MACLOGHEX( iPreparedData->Ptr(), iPreparedData->Length() )
        }
    CleanupStack::PopAndDestroy( &certArray );
    IPCWRITE1L( sizeArg );
    aMessage.Complete( KErrNone );
    MACLOGLITDETAIL( "--> CRoapStorageSession::GetOCSPResponderIdL" )
    }

//  End of File
