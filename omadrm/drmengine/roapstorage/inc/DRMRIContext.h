/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Datatype for the Common Rights Database Data
*
*/


#ifndef DRMRICONTEXT_H
#define DRMRICONTEXT_H

// INCLUDES

#include <e32base.h>
#include <hash.h>
#include "RoapMessage.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FORWARD DECLARATIONS
class RReadStream;
class RWriteStream;

// TYPE DEFINITIONS

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
*  CDRMRIContext implements a datatype for the common data for a single
*  content identifier
*
*  @lib RightsServer.dll
*  @since 3.0
*/

class CDRMRIContext : public CBase 
    {
    
    public: // Constructors and destructor
        
        /**
        * NewLC
        *
        * Creates an instance of the CDRMRIContext class and returns a pointer 
        * to it. The function leaves the object into the cleanup stack
        *
        * @since    3.0
        * @param    aRIID : Rights issue identifier SHA1_HASH
        * @param    aRiAlias: RI Alias, extracted from RoapTrigger        
        * @param    aVersion : Selected version KMaxRoapVersionLength
        * @param    aAlgorithms : Algorithm strings
        * @param    aRIURLs : Rights issuer urls,
        * @param    aRightsIssuerURL: An URL that can be used for contactig the RI
        *                             (e,g, in the case where RI Context is expired)
        * @param    aExpiryTime : Expiration time
        * @param    aChain: The RI ceritificate chain
        * @param    aResponses: The OCSP responses for validating certificates
        *                       in the RI certificate chain
        * @param    aSelectedDeviceRoot: The selected certificate chain that is
        *           used for wrapping the REKs and Domain Keys. Presented as a
        *           public key hash of the root certificate
        * @param    aIsMeteringAllowed: Wheter metering is allowed for this RI
        *  
        * @return   Functional CDRMRIContext object, Function leaves if an error 
        *           occurs.
        */
        IMPORT_C static CDRMRIContext* NewLC( const TDesC8& aRIID,
                                              const TDesC8& aRiAlias,
                                              const TDesC8& aVersion,
                                              const RPointerArray< HBufC8 >& aAlgorithms,
                                              const RPointerArray< HBufC8 >& aRIURLs,
                                              const TDesC8& aRightsIssuerURL,
                                              const TTime& aExpiryTime,
                                              const RPointerArray< HBufC8 >& aChain,
                                              const RPointerArray< HBufC8 >& aResponses,
                                              const TUint8 aDeviceCertCached,
                                              const TDesC8& aSelectedDeviceRoot,
                                              const TUint8 aIsMeteringAllowed = ETrue );
        
        /**
        * NewL
        *
        * Creates an instance of the CDRMRIContext class and returns a pointer 
        * to it.
        *
        * @since    3.0
        * @param    aRIID : Rights issue identifier SHA1_HASH
        * @param    aRiAlias: RI Alias, extracted from RoapTrigger            
        * @param    aVersion : Selected version KMaxRoapVersionLength
        * @param    aAlgorithms : Algorithm strings
        * @param    aRIURLs : Rights issuer urls
        * @param    aExpiryTime : Expiration time
        * @param    aChain: The RI ceritificate chain
        * @param    aResponses: The OCSP responses for validating certificates
        *                       in the RI certificate chain        
        * @param    aSelectedDeviceRoot: The selected certificate chain that is
        *           used for wrapping the REKs and Domain Keys. Presented as a
        *           public key hash of the root certificate
        * @param    aIsMeteringAllowed: Wheter metering is allowed for this RI        
        *  
        * @return   Functional CDRMRIContext object, Function leaves if an error 
        *           occurs.
        */
        IMPORT_C static CDRMRIContext* NewL( const TDesC8& aRIID,
                                             const TDesC8& aRiAlias,        
                                             const TDesC8& aVersion,
                                             const RPointerArray< HBufC8 >& aAlgorithms,
                                             const RPointerArray< HBufC8 >& aRIURLs,
                                             const TDesC8& aRightsIssuerURL,
                                             const TTime& aExpiryTime,
                                             const RPointerArray< HBufC8 >& aChain,
                                             const RPointerArray< HBufC8 >& aResponses,
                                             const TUint8 aDeviceCertCached,
                                             const TDesC8& aSelectedDeviceRoot,
                                             const TUint8 aIsMeteringAllowed = ETrue );

        /**
        * NewLC
        *
        * Creates an instance of the CDRMRIContext class and returns a pointer 
        * to it. The function leaves the object into the cleanup stack
        *
        * @since    3.0
        * @return   Functional CDRMRIContext object, Function leaves if an error 
        *           occurs.
        */
        IMPORT_C static CDRMRIContext* NewLC();
        
        /**
        * NewL
        *
        * Creates an instance of the CDRMRIContext class and returns a pointer 
        * to it.
        *
        * @since    3.0
        * @return   Functional CDRMRIContext object, Function leaves if an error 
        *           occurs.
        */
        IMPORT_C static CDRMRIContext* NewL();

        /**
        * Destructor
        */
        IMPORT_C virtual ~CDRMRIContext();
        
  public: // Get and set functions
        
        /**
        * RIID
        * 
        * Returns a constant TDesC8 reference to the rights issuer id
        *
        * @since    3.0
        * @return   a const reference to the rights issuer id
        */        
        IMPORT_C const TDesC8& RIID() const;
        
        /**
        * RIAlias
        * 
        * Returns a constant TDesC8 reference to the rights issuer alias
        *
        * @since    3.2
        * @return   a const reference to the rights issuer alias
        */
        IMPORT_C const TDesC8& RIAlias() const;
        
        
        /**
        * Version
        * 
        * Returns a constant TDesC8 reference to the version
        *
        * @since    3.0
        * @return   a const reference to the rights issuer id
        */        
        IMPORT_C const TDesC8& Version() const;
        

        /**
        * Algorithms
        * 
        * Returns a constant reference to the algorithm pointer array
        *
        * @since    3.0
        * @return   a const reference to a pointer array with the algorithms
        */        
        IMPORT_C const RPointerArray< HBufC8 >& Algorithms() const;  


        /**
        * RIURLs
        * 
        * Returns a constant reference to the url pointer array
        *
        * @since    3.0
        * @return   a const reference to a pointer array with the right issuer
        *           urls
        */        
        IMPORT_C const RPointerArray< HBufC8 >& RIURLs() const;
        
        
        /**
        * RIURLs
        * 
        * Returns a constant reference to the rights issuer url
        *
        * @since    3.0
        * @return   a const reference to a the rights issuer url
        */
        IMPORT_C const TDesC8& RightsIssuerURL() const;


        /**
        * ExpiryTime
        *
        * Returns a constant reference to the expiration time
        *
        * @since    3.0
        * @return   a cont reference to the expiration time
        */
        IMPORT_C const TTime ExpiryTime() const;

        
        /**
        * CertificateChain
        * 
        * Returns a constant reference to the algorithm pointer array
        *
        * @since    3.0
        * @return   a const reference to a pointer array with the algorithms
        */       
        IMPORT_C const RPointerArray< HBufC8 >& CertificateChain() const;


        /**
        * OCSPResponse
        * 
        * Returns a constant reference to the algorithm pointer array
        *
        * @since    3.0
        * @return   a const reference to a pointer array with the algorithms
        */        
        IMPORT_C const RPointerArray< HBufC8 >& OCSPResponse() const;
        

        /**
        * DeviceCertCached
        * 
        * Returns an indication if the RI has stored the Device certificate or not
        *
        * @since    3.0
        * @return   EFalse or ETrue
        */         
        IMPORT_C TUint8 DeviceCertCached() const;
        
        /**
        * SelectedDeviceRoot
        * 
        * Returns a constant TDesC8 reference to the selected Device root
        *
        * @since    3.0
        * @return   a const reference to the selected Device root
        */      
        IMPORT_C const TDesC8& SelectedDeviceRoot() const;

        /**
        * IsMeteringAllowed
        * 
        * Checks wheter metering is allowed for a this RightsIssuer
        *
        * @since    3.2
        */        
        IMPORT_C TUint8 IsMeteringAllowed() const; 

        /**
        * SetRIIDL
        * 
        * Sets the rights issuer identifier, overwrites the previous one 
        * if one exits
        *
        * @since    3.0
        * @param    aRIID : Rights Issuer Identifier        
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */        
        IMPORT_C void SetRIIDL( const TDesC8& aRIID );


        /**
        * SetRightsIssuerURLL
        * 
        * Sets the RI URL, deletes the old one
        *
        * @since    3.0
        * @param    aRightsIssuerURL : a reference to the new rights issuer url
        */ 
        IMPORT_C void SetRightsIssuerURLL( const TDesC8& aRightsIssuerURL );        
        
         /**
        * SetVersionL
        * 
        * Sets the version, overwrites the previous one if one exits
        *
        * @since    3.0
        * @param    aVersion : version string       
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */        
        IMPORT_C void SetVersionL( const TDesC8& aRIID );    
           

        /**
        * SetAlgorithmsL
        * 
        * Sets the algorithms, deletes the old ones
        *
        * @since    3.0
        * @param    aAlgorithms : a pointer array with the used algorithms
        * @return   a pointer to the content id or null
        */        
        IMPORT_C void SetAlgorithmsL( const RPointerArray< HBufC8 >& aAlgorithms ); 
        
        
        /**
        * SetRIURLsL
        * 
        * Sets the algorithms, deletes the old ones
        *
        * @since    3.0
        * @param    aRIURLs : a pointer array with the rights issuer urls       
        * @return   a pointer to the content id or null
        */        
        IMPORT_C void SetRIURLsL( const RPointerArray< HBufC8 >& aRIURLs );        
             
        /**
        * SetExpiryTimeL
        * 
        * Sets the expiration time to the given value
        *
        * @since    3.0
        * @param    aExpiryTime : the expiration time        
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */        
        IMPORT_C void SetExpiryTimeL( const TTime& aExpiryTime );

         /**
        * SetCertificateChainL
        * 
        * Sets the certificate chain
        *
        * @since    3.0
        * @param    aChain : the certificate chain        
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */          
        IMPORT_C void SetCertificateChainL( const RPointerArray< HBufC8 >& aChain );
        
         /**
        * SetOCSPResponseL
        * 
        * Sets the OCSP responses
        *
        * @since    3.0
        * @param    aResponses : the OCSP responses      
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */         
        IMPORT_C void SetOCSPResponseL( const RPointerArray< HBufC8 >& aResponses );
        
         /**
        * SetDeviceCertCached
        * 
        * Sets the indication whether the RI has stored the Device public key or not
        *
        * @since    3.0
        * @param    aIsCached : The certificate caching indication (ETrue/EFalse)     
        */         
        IMPORT_C void SetDeviceCertCached( TUint8& aIsCached );

        /**
        * WhiteListURLExistsL
        * 
        * Checks the white list to see if the url is in the list
        *
        * @since    3.0
        * @param    aURL : the URL to look for       
        * @return   ETrue if the url is in the white list
        *           EFalse if it is not
        */    
        IMPORT_C TBool WhiteListURLExistsL( const TDesC8& aURL ) const;
        
        
        /**
        * SetSelectedDeviceRoot
        * 
        * Sets the selected Device root, overwrites the previous one 
        * if one exits
        *
        * @since    3.0
        * @param    aSelectedDeviceRoot : Hash of the root certificate public key
        */        
        IMPORT_C void SetSelectedDeviceRoot( const TDesC8& aSelectedDeviceRoot );

        /**
        * SetMeteringStatus
        * 
        * Sets the value for metering to be enabled or disabled
        *
        * @since    3.2
        * @param    aIsAllowed : ETrue if set to allowed, EFalse if not allowed
        */        
        IMPORT_C void SetMeteringStatus( const TBool aIsAllowed );               
                     
    public: // Storing functions
        /**
        * ExternalizeL
        * 
        * Writes the data of the object into the stream
        *
        * @since    3.0
        * @param    aStream : the output stream to write to
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */
        IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
        
        /**
        * InternalizeL
        * 
        * Reads the data of the object from the stream
        *
        * @since    3.0
        * @param    aStream : the output stream to write to
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */
        IMPORT_C void InternalizeL( RReadStream& aStream );   


        /**
        * ImportL
        * 
        * Reads the data of the object from the descriptor and initialized the 
        * object accordingly
        *
        * @since    3.0
        * @param    aBuffer : the buffer containing the data of the imported object
        * @return   None, The function leaves with Symbian OS error code if an
        *           error occurs
        */
        IMPORT_C void ImportL( const TDesC8& aBuffer );
    
        /**
        * ExportL
        * 
        * Writes the data of the object into an HBufC8 and returns a pointer
        * to it. The caller is responsible for deleting the object.
        *
        * @since    3.0
        * @return   Returns an HBufC8* containing the data of the object,
        *           The function leaves with Symbian OS error code if an
        *           error occurs
        */
        IMPORT_C HBufC8* ExportL() const; 
       
        
        /**
        * Size
        *
        * Returns the size of the externalized object
        *
        * @since    3.0
        * @return   the amount of bytes the externalize object occupies
        */           
        IMPORT_C TInt Size() const;
        
    private:
        /**
        * Default constructor
        */
        CDRMRIContext();    
    
        /**
        * ConstructL
        *
        * Second phase constructor
        *
        * @since    3.0
        * @param    aRIID : Rights issuer identifier SHA1_HASH
        * @param    aRiAlias : Rights issuer alias from RoapTrigger        
        * @param    aVersion : Selected version KMaxRoapVersionLength
        * @param    aAlgorithms : Algorithm strings
        * @param    aRIURLs : Rights issuer urls,
        * @param    aRightsIssuerURL: An URL that can be used for contactig the RI
        *                             (e,g, in the case where RI Context is expired)
        * @param    aExpiryTime : Expiration time
        * @param    aChain: The RI ceritificate chain
        * @param    aResponses: The OCSP responses for validating certificates
        *                       in the RI certificate chain
        * @param    aSelectedDeviceRoot: The selected certificate chain that is
        *           used for wrapping the REKs and Domain Keys. Presented as a
        *           public key hash of the root certificate
        * @param    aIsMeteringAllowed: Wheter metering is allowed for this RI
        *        
        * @return   Functional CDRMRIContext object, Function leaves if an error 
        *           occurs.
        */  
        void ConstructL( const TDesC8& aRIID,
                         const TDesC8& aRiAlias,
                         const TDesC8& aVersion,
                         const RPointerArray< HBufC8 >& aAlgorithms,
                         const RPointerArray< HBufC8 >& aRIURLs,
                         const TDesC8& aRightsIssuerURL,
                         const TTime& aExpiryTime,
                         const RPointerArray< HBufC8 >& aChain,
                         const RPointerArray< HBufC8 >& aResponses,
                         const TUint8 aDeviceCertCached,
                         const TDesC8& aSelectedDeviceRoot,
                         const TUint8 aIsMeteringAllowed );

        /**
        * Assignment operator - Prevented
        */
        CDRMRIContext& operator =( const CDRMRIContext& );    
    
        /**
        * Copy constructor - Prevented
        */
        CDRMRIContext( const CDRMRIContext& );                

        /**
        * WriteInt64L
        * 
        * Writes the 64 bit integer to the stream
        *
        * @since    3.0
        * @param    aWrite : the 64 bit integer to write
        * @param    aStream : the output stream to write to
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */
        void WriteInt64L( const TInt64& aWrite, RWriteStream& aStream ) const;

        /**
        * ReadInt64L
        * 
        * Reads the 64 bit integer from the stream
        *
        * @since    3.0
        * @param    aRead : the 64 bit integer read
        * @param    aStream : the output stream to write to
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */
        void ReadInt64L( TInt64& aRead, RReadStream& aStream ); 


    private: // Internal Data
    
        // Rights Issuer identifier
        TBuf8<SHA1_HASH> iRIID;
        
        // Rights Issuer Alias
        HBufC8* iRiAlias;
        
        // Selected Version
        TBuf8<Roap::KMaxRoapVersionLength> iVersion;
        
        // Selected Algorithms
        RPointerArray< HBufC8 > iAlgorithms;
        
        // Domain name white list
        RPointerArray< HBufC8 > iRIURLs;
        
        // Rights issuer URL
        HBufC8* iRightsIssuerURL;
        
        // Rights issuer context expiry time
        TTime iExpiryTime;

        // Rights issuer certificate chain        
        RPointerArray<HBufC8> iCertificateChain;

        // OCSP responses for validating certificates       
        RPointerArray<HBufC8> iOcspResponse;

        // Certificate caching indication (has an RI stored the Device certificate)        
        TUint8 iDeviceCertCached;
        
        // The selected Device certificate chain
        TBuf8<SHA1_HASH> iSelectedDeviceRoot;
        
        // The status of metering allowance
        TUint8 iIsMeteringAllowed;
    };

#endif      // DRMRICONTEXT_H   
            
// End of File
