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
* Description:  Client Class of ROAP Storage Server
*
*/



#ifndef ROAPSTORAGECLIENT_H
#define ROAPSTORAGECLIENT_H

#include <Oma2Agent.h>

//  INCLUDES

class CRSAPrivateKey;
class CRSAPublicKey;
class CDRMRIContext;
class CDRMDomainContext;

namespace Roap
    {

    // CLASS DECLARATION

    /**
    *  RRoapStorageClient
    *
    *  @lib drmserverinterfaces.lib
    *  @since Series 60 3.0
    */
    NONSHARABLE_CLASS( RRoapStorageClient ) : public RSessionBase
        {
    public:
        // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C RRoapStorageClient();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~RRoapStorageClient();

    public:
        // New functions

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
        IMPORT_C TInt Connect();

        IMPORT_C TInt GetDevicePublicKeyL( CRSAPublicKey*& aPublicKey );

        IMPORT_C TInt GetDevicePublicKeyDerL( HBufC8*& aPublicKey );

        IMPORT_C TInt GetDevicePublicKeyHashL( TDes8& aHash );

        IMPORT_C TInt GetDeviceCertificateChainL(
            RPointerArray<HBufC8>& aCertChain );

        IMPORT_C TInt SignL( const TDesC8& aHash, HBufC8*& aSignature );

        IMPORT_C TInt RsaSignL( const TDesC8& aHash, HBufC8*& aSignature );

        IMPORT_C TBool VerifyL(
            const TDesC8& aSignature,
            const TDesC8& aHash,
            const RPointerArray<HBufC8>& aCertificateChain );

        IMPORT_C CDRMRIContext* GetRIContextL( const TDesC8& aRiID );

        IMPORT_C CDRMDomainContext* GetDomainContextL(
            const TDesC8& aDomainID );

        IMPORT_C void AddRIContextL( const CDRMRIContext& aRiContext );

        IMPORT_C void AddDomainContextL(
            const CDRMDomainContext& aDomainContext,
            const RPointerArray<HBufC8>& aMacs,
            const RPointerArray<HBufC8>& aDomainKeyElements,
            TKeyTransportScheme& aTransportScheme );

        IMPORT_C void DeleteRiContextL( const TDesC8& aRiID );

        IMPORT_C void DeleteDomainContextL( const TDesC8& aDomainID );

        IMPORT_C void DeleteExpiredRIsL( const TTime& aTime );

        IMPORT_C void DeleteExpiredDomainsL( const TTime& aTime );

        IMPORT_C void DeleteExpiredContextsL( const TTime& aTime );

        IMPORT_C void DeleteAllL();

        /*
        * WhiteListURLExistsL
        * @param aURL, url to be searched in white lists
        * @param (inout) aInPreconfiguredWhiteList
        *     as in parameter: define whether or not to search from
        *         pre-defined whitelist
        *         ETrue: given URL is not searched from whitelistURL
        *         EFalse: given URL is not searched from whitelistURL
        *     as out parameter: define whether or not url has been
        *         searched and found from pre-defined whitelist
        *         ETrue: parameter was initially false,
        *             hostname part of aURL does not match hostname part
        *             in any RI URL of any existing RI conext,
        *             and aURL matched at least one of pre-defined
        *             whitelist URLs
        *         EFalse otherwise
        *         @note EFalse also if hostname part of aURL
        *             matches any hostname part in any RI URLs
        *             of any existing RI context
        *
        * @return
        *     ETrue: aURL is in white list
        *     EFalse: aURL is not in white list
        */
        IMPORT_C TBool WhiteListURLExistsL( const TDesC8& aURL,
            TBool& aFromPreconfiguredWhiteList );

        IMPORT_C TInt SelectTrustedRootL(
            const RPointerArray<HBufC8>& aTrustedRoots,
            TDes8& aSelectedRoot ) const;

        IMPORT_C TInt SelectTrustedRootL( const TDesC8& aTrustedRoot ) const;

        IMPORT_C void
            GetTrustedRootsL( RPointerArray<HBufC8>& aTrustedRoots ) const;

        IMPORT_C HBufC8* GetRootCertificateL( const TDesC& aSubject ) const;

        IMPORT_C HBufC8* GetMeteringDataL(
            TDesC8& aRiId,
            TDes8& aMacKey,
            TDes8& aEncKeyHash,
            HBufC8*& aEncryptedMekAndMak );

        IMPORT_C void DeleteMeteringDataL( const TDesC8& aRiId );

        IMPORT_C void GetAllRIContextsL(
            RPointerArray<CDRMRIContext>& aRiContexts );

        IMPORT_C void UpdateRIContextL( const CDRMRIContext& aContext );

        IMPORT_C TBool UpdateDrmTimeL(
            const RPointerArray<HBufC8>& aCertChain,
            const RPointerArray<HBufC8>& aOCSPResponses,
            const TDesC8& aNonce );

        IMPORT_C TBool VerifyOcspResponsesL(
            const RPointerArray<HBufC8>& aOcspResponses,
            const TDesC8& aRiCaCert,
            const RPointerArray<HBufC8>& aCertSerialNums );

        IMPORT_C HBufC8* GetOcspResponderIdL( const TDesC8& aRiID );


    private:

        // Prohibit copy constructor if not deriving from CBase.
        RRoapStorageClient( const RRoapStorageClient& );
        // Prohibit assignment operator if not deriving from CBase.
        RRoapStorageClient& operator=( const RRoapStorageClient& );

        };
    }

#endif // ROAPSTORAGECLIENT_H
// End of File
