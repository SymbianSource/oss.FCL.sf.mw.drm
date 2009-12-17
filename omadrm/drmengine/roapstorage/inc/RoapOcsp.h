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
* Description:  OCSP response classes
*
*/


#ifndef __ROAPOCSP_H__
#define __ROAPOCSP_H__

#include <e32base.h>
#include <signed.h>
#include <f32file.h>

static const TInt KOCSPNonceBytes( 16 );

class COCSPResponseCertInfo;
class COCSPCertID;
class COCSPResponse;
class COCSPResponseDecoder;

class CASN1EncBase;
class CASN1EncContainer;
class TASN1DecGeneric;

class CX509Certificate;
class CX509CertChain;
class CPKIXCertChain;

// Enums placed in OCSP namespace
namespace OCSP
    {
    // Globally-reserved error codes - range is -7601 to -7649
    enum
        {
        KErrTransportFailure = -7601,
        KErrInvalidURI = -7602,
        KErrNoCertificates = -7603
        };

    // What we think of the response, or why we haven't got one
    enum TStatus
        {
        // Error in communication with server
        ETransportError = 1,

        // Internal problem processing response
        EClientInternalError = 2,

        // No OCSP server specified
        ENoServerSpecified = 3,

        // Invalid sever URI
        EInvalidURI = 4,

        // Problems understanding the response
        EMalformedResponse = 10,
        EUnknownResponseType = 11,
        EUnknownCriticalExtension = 12,

        // Unsuccessful responses from server
        EMalformedRequest = 20,
        EServerInternalError = 21,
        ETryLater = 22,
        ESignatureRequired = 23,
        EClientUnauthorised = 24,

        // Response validation failures
        EMissingCertificates = 30,
        EResponseSignatureValidationFailure = 31,

        // Time problems
        EThisUpdateTooLate = 40,
        EThisUpdateTooEarly = 41,
        ENextUpdateTooEarly = 42,
        ECertificateNotValidAtValidationTime = 43,

        // Nonce error
        ENonceMismatch = 50,

        // Response sufficiently valid to use - clients to decide if
        // missing nonce is sufficiently serious to require rejection
        EMissingNonce = 51,

        EValid = 100,
        };

    // What does the response say about the certificates?
    // IMPORTANT: Do not change the order these are in
    enum TResult
        {
        EGood = 10, EUnknown = 20, ERevoked = 30
        };
    }

/**
* Information about a certificate sent as part of an OCSP request.
*/

NONSHARABLE_CLASS( COCSPRequestCertInfo ) : public CBase
    {
public:

    /**
    * Get the certificate present in the request.
    */

    const CX509Certificate& Subject() const;

    /**
    * Get the issuer of the certificate present in the request.
    */

    const CX509Certificate& Issuer() const;

public:

    // Not exported
    static COCSPRequestCertInfo* NewLC(
        const CX509Certificate& aSubject,
        const CX509Certificate& aIssuer );

    ~COCSPRequestCertInfo();

    const COCSPCertID& CertID() const;

private:

    COCSPRequestCertInfo( const CX509Certificate& aIssuer,
        const CX509Certificate& aSubject );
    void ConstructL();

private:

    const CX509Certificate& iSubject;
    const CX509Certificate& iIssuer;
    COCSPCertID* iCertID;
    };

/**
* Represents an OCSP protocol response.
*/

NONSHARABLE_CLASS( COCSPResponse ) : public CSignedObject
    {
public:

    /**
    * Get the number of certificates statuses present in the response.
    */

    TInt CertCount() const;

    /**
    * Get a COCSPResponseCertInfo object containing details about one of the
    * certificate statuses present in the response.
    */

    COCSPResponseCertInfo* CertInfo( TUint aIndex ) const;

    /**
    * Get the producedAt time for the response.
    */

    TTime ProducedAt() const;

    /**
    * Get the archiveCutoff time for the response, or NULL if it was not present.
    */

    const TTime* ArchiveCutoff() const;

    const TPtrC8* SigningCerts() const;

public:

    // This class is created and initialised by the response decoder
    friend class COCSPResponseDecoder;

    ~COCSPResponse();

    /**
    * Get the index for the given cert, or KErrNotFound.
    */

    TInt Find( const COCSPCertID& aCertID ) const;

    // Enums to use in DataElementEncoding
    enum
        {
        ECertificateChain,
        ENonce,
        EResponderIDName, // Only one of the ResponderIDs won't be NULL
        EResponderIDKeyHash
        };

    // From CSignedObject
    const TPtrC8* DataElementEncoding( const TUint aIndex ) const;

private:

    COCSPResponse();

    // From CSignedObject
    const TPtrC8 SignedDataL() const;
    void InternalizeL( RReadStream& aStream );

private:

    // Time of response, and of archiveCutoff extension (if present)
    TTime iProducedAt;
    TTime* iArchiveCutoff;

    // Value of nonce in response extension (if present)
    TPtrC8 iNonce;

    // Point to the signing certificates in the response
    TPtrC8 iSigningCerts;

    // Point to the signed portion of the data
    TPtrC8 iSignedData;

    // There are two ways the responder ID can be specified - only
    // one of these will be used
    TPtrC8 iResponderIDName;
    TPtrC8 iResponderIDKeyHash;

    RPointerArray<COCSPResponseCertInfo> iCertInfos;
    };

/**
* Information about a certificate status, as given in OCSP response
* singleResponse data item.
*/

NONSHARABLE_CLASS( COCSPResponseCertInfo ) : public CBase
    {
public:

    OCSP::TResult Status() const;
    TTime ThisUpdate() const;
    const TTime* NextUpdate() const; // NULL if nextUpdate not set
    const TTime* RevocationTime() const; // NULL if not revoked

public:

    static COCSPResponseCertInfo* NewLC( CArrayPtr<TASN1DecGeneric>& items );
    ~COCSPResponseCertInfo();
    COCSPCertID* CertID() const;

private:
    void ConstructL( CArrayPtr<TASN1DecGeneric>& items );

private:
    OCSP::TResult iStatus;

    TTime iThisUpdate;
    TTime* iNextUpdate;
    TTime* iRevocationTime;

    COCSPCertID* iCertID;
    };

#endif // __ROAPOCSP_H__
