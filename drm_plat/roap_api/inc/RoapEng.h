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


#ifndef ROAP_ENG_H
#define ROAP_ENG_H

// INCLUDES

#include <e32base.h>
#include <hash.h>

#include <RoapMessage.h>
#include <RoapEngBase.h>
#include <Oma2Agent.h>

// FORWARD DECLARATIONS
class CDrmProtectedRoParser;
class RDRMClockClient;
class CDcfRep;
class CX509Certificate;
class COCSPResponse;

namespace Roap
{
class CRoapParser;
class CRoapSigner;
class CRIHello;
class CRegistrationResp;
class CRightsResp;
class CJoinDomainResp;
class CLeaveDomainResp;
class RRoapStorageClient;


// CLASS DECLARATION
class CRoapEng: public CRoapEngBase
    {

    public:     // Constructors and destructor

         /**
        * Two-phased constructor.
        */
        IMPORT_C static CRoapEng* NewL( );
        
        /**
        * Destructor.
        */
        IMPORT_C ~CRoapEng();

    protected:
    
        // Needed in trigger parsing phase

        CRoapTrigger* ParseTriggerL( const TDesC8& aXmlTrigger );

        void GetRIContextL( TBool& aRegistered, const TDesC8& aRiId );
        
        void  GetDomainContextL( TBool& aIsJoined,
                                 TBool& aIsValidGeneration,
                                 const TDesC8& aDomainId );

        
        
        // For creating ROAP requests
        
        void CreateReqMessageL();
        
        CRoapMessage* CreateDeviceHelloL();
        
        CRoapMessage* CreateRegistrationRequestL();
        
        CRoapMessage* CreateRightsRequestL();
        
        CRoapMessage* CreateJoinDomainRequestL();
        
        CRoapMessage* CreateLeaveDomainRequestL();
        
        CRoapMessage* CreateMeteringReportRequestL();
        
        
        // For handling ROAP responses
        
        void HandleRoapResponseL( const TDesC8& aXmlResponse );
        
        void HandleRIHelloPduL( const TDesC8& aRiHello );
        
        void HandleReqResponsePduL( const TDesC8& aRegResp );
        
        void HandleRightsResponsePduL( const TDesC8& aRightsResp, TBool aOnePass );
        
        void HandleJoinDomainResponsePduL( const TDesC8& aJoinResp );
        
        void HandleLeaveDomainResponsePduL( const TDesC8& aLeaveResp );
        
        void HandleMeteringReportResponsePduL( const TDesC8& aMeteringResp );
                 
        void HandleMultipartL();
        
        
        // For signing and certificate checking
        
        HBufC8* SignMessageL( const TDesC8& aMessage ) const;
        
        TBool VerifySignatureL( const TDesC8& aMessage,
                                const TDesC8& aSignature,
                                const RPointerArray< HBufC8 >& aCertificateChain ) const;
                                
        TBool VerifyTriggerSignatureL( const TDesC8& aXmlTrigger,
                                         const CRoapTrigger& aTrigger  ) const;
                                
        TBool VerifyCertificateChainL( const RPointerArray< HBufC8 >& aCertificateChain,
                                       const RPointerArray< HBufC8 >& aOcspResponses ) const;
                                       
        TBool VerifyOcspResponsesL( const RPointerArray< HBufC8 >& aOcspResponses,
                                    const TDesC8& aRiCaCert,
                                    const RPointerArray< HBufC8 >& aCertSerialNums ) const;
                                    
        TBool VerifyOcspCertChainL( COCSPResponse& aResponse,
                                    const TDesC8& aRootCert,
                                    CX509Certificate*& aOcspCert ) const;
                                    
        TBool ValidateRiCertificateL( const CX509Certificate* aCert );


        // For fetching data for ROAP requests
        
		RPointerArray<HBufC8> GetCertificateChainL() const;
		
		RPointerArray<HBufC8> GetSupportedAlgorithmsL() const;
		
		void GetDeviceDetailsL( HBufC8*& aManufacturer,
                                HBufC8*& aModel,
                                HBufC8*& aVersion );
                                         
        HBufC8* GetOCSPResponderKeyHashL() const;
                                
        
        // For transaction tracking
        
        void FetchTransactionIDL( RPointerArray<HBufC8>& aTransIDs,
                                  RPointerArray<HBufC8>& aContentIDs );
        
        void InsertTransactionIDL( RPointerArray<HBufC8>& aTransIDs,
                                   RPointerArray<HBufC8>& aContentIDs );

        
        // For fetching and setting DRM Time
        
        TTime GetDrmTimeL();
        
        void SetDrmTimeSecureL();
        
        void AdjustDrmTimeL(  const RPointerArray< HBufC8 >& aOcspResponses,
                             TDesC8& aRegReqNonce ) const;
                             
                             
        // For storing a Domain RO in implicit Join Domain case
        void StoreDomainRightsL();
        
        // Inserting domain ROs after receiving                     
        void InsertDomainRosL();

        // For mapping ROAP status to error code
        TInt MapStatusL();
        
        // For checking is the root certificate issued by CMLA
        void IsTimeUpdateAllowedL( const RPointerArray<HBufC8>& aCertChain,
                                   const RPointerArray<HBufC8>& aOcspResponses,
                                   TBool& aIsAllowed ) const;
      
          
        TBool ValidateRiIdL( TDesC8& aRiId, TDesC8& aCertBuf );
   
                // Update the status of transaction tracking variable	     
        void UpdateTransactionTrackingStatusL();
        
        // Creates list of device IDs for Multi-PKI
        TInt CreateDeviceIdHashArrayL( RPointerArray<TDesC8>& aIdArray);
          
    private:

        CRoapEng();
        
        void ConstructL();
        
        enum TTransStatus
            {
            ENotAsked,
            EForbidden,
            EAllowed
            };
        
    private:
    
    	CRoapParser* iParser;
    	CRoapSigner* iSigner;
    	RRoapStorageClient* iStorageClient;
    	RDRMClockClient* iClockClient;
    	CDcfRep* iDcfRep;
    	TInt iRoapStatus;
    	CDrmProtectedRoParser* iRoParser;  	
    	TBool iCertNeeded;
    	TBool iRiSupportsCertCaching;
    	HBufC8* iDeviceId;
    	TTransStatus iTransStatus;
    	TKeyTransportScheme iSelectedAlgorithms;
    	TBuf8< SHA1_HASH > iSelectedRoot;
    	TBuf8< KDomainIdLength > iDomainId;
    	TBuf8< SHA1_HASH > iRiId;
        TBuf8< KMaxRoapVersionLength > iRiVersion;
        RPointerArray< HBufC8 > iRiAlgorithms;
        TBool iDeviceTimeError;
        TBuf8< KDeviceNonceLength > iRegReqNonce; // for DRM Time sync
        TBool iSecureTime;
        TInt iZone;
    };
}

#endif /* def ROAP_ENG_H */
