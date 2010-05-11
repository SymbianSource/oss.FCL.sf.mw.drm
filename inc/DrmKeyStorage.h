/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef DRMKEYSTORAGE_H
#define DRMKEYSTORAGE_H

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  CDrmKeyStorage: Contains key storage for OMA DRM 2.x
*
*  Generally the methods to the class require that a root certificate is selected.
*  A specific root certificate can be selected or the default root can be selected.
*  Operations needing the PKI keys should fail if the key has not been selected.
*  Suggested error code: KErrNotReady
*
*  @lib    -
*  @since  3.0
*/
class MDrmKeyStorage
    {
public:
    static const TInt KDeviceSpecificKeyLength = 16;
    static const TInt KRdbSerialNumberLength = 16;
    
public: // New functions

    /**
    *  Destructor
    */

    virtual ~MDrmKeyStorage() = 0;
    
    /**
    * ModulusSize
    *
    * Return the modulus size of the selected certificates private key.
    *
    * @return   Modulus size of the PKI key 
    */
    
    virtual TInt ModulusSize() = 0;
    
    /**
    * SelectTrustedRootL
    *
    * Selects the trusted root certificate to be used for operation occurring after
    * this call
    *
    * @param    aRootKeyHash      Hash of the root key to be used.
    * @leave    Symbian OS error codes
    */
    
    virtual void SelectTrustedRootL(
        const TDesC8& aRootKeyHash) = 0;

    /**
    * SelectedRootIsCmla
    *
    * Checks if the selected root is a CMLA root
    *
    * @return   ETrue if the selected root is a CMLA root
    *           EFalse if the root does not exist or is not a CMLA root
    */
            
    virtual TBool SelectedRootIsCmla() = 0;

    /**
    * SelectDefaultRootL
    *
    * Selects the default root certificate of the device/keystorage implementation
    *
    * @leave    Symbian OS error code if the root can't be selected for some reason
    */
            
    virtual void SelectDefaultRootL() = 0;

    /**
    * GetTrustedRootsL
    *
    * Get a list of unique identifiers for the trusted roots of the
    * device / keystorage implementation. Generally PKI public key hash values.
    *
    * @param    aRootList     A pointer array of trusted root certificate hash values
    *                         or other unique identifiers for the certificates
    * @leave    Symbian OS error codes
    */
            
    virtual void GetTrustedRootsL(
        RPointerArray<HBufC8>& aRootList) = 0;

    /**
    * GetCertificateChainL
    *
    * Get the certificate chain of the selected certificate
    *
    * @param    aRootList     A pointer array of certificate hash values
    *                         or other unique identifiers for the certificates
    *                         in the certificate chain
    * @leave    Symbian OS error codes
    */
            
    virtual void GetCertificateChainL(
        RPointerArray<HBufC8>& aCertChain) = 0;
    
    /**
    * RsaSignL
    *
    * Create an RSA signature of the buffer given
    *
    * @param    aInput         A descriptor containing the data to be signed
    * @return   Signed buffer. Caller is responsible for freeing it.
    * @leave    Symbian OS error codes
    */
        
    virtual HBufC8* RsaSignL(
        const TDesC8& aInput) = 0;

    /**
    * RsaDecryptL
    *
    * Decrypt the RSA encrypted buffer
    *
    * @param    aInput         A descriptor containing the data to be decrypted
    * @return   Decrypted buffer. Caller is responsible for freeing it.
    * @leave    Symbian OS error codes
    */
        
    virtual HBufC8* RsaDecryptL(
        const TDesC8& aInput) = 0;

    /**
    * ImportDataL
    *
    * Allows the importing of PKI keys into the system
    * Should import: Private Key, Public Certificate and all
    * signing certificates for the certificate chain all the way to the root.
    *
    * Mostly usable for debugging purposes. Should be used with caution.
    *
    * @param    aPrivateKey           Private key of the imported PKI key.
    * @param    aCertificateChain     Public Certificate and signing certificates
    * @leave    Symbian OS error codes
    */
            
    virtual void ImportDataL(
        const TDesC8& aPrivateKey,
        const RArray<TPtrC8>& aCertificateChain) = 0;

    /**
    * GetDeviceSpecificKeyL
    *
    * Get the device specific key/identifier that has a length of
    * KDeviceSpecificKeyLength. Must return the same value every time.
    * This method must not require the root to be selected.
    *
    * Does not require the selection of 
    *
    * @param    aKey      Buffer to fit the bytes of a device specific
    *                     unique key / identifier. Must return the same
    *                     value every time
    * @leave    Symbian OS error codes
    */
            
    virtual void GetDeviceSpecificKeyL(
        TBuf8<KDeviceSpecificKeyLength>& aKey) = 0;
    
    /**
    * GetRdbSerialNumberL
    *
    * This method fetches a unique serial number for the OMA DRM rights database
    * currently in use. Does not have to be unique per device, but should return
    * the same value while the current database is in use.
    * Should create a new serial number if one does not exists.
    *
    * @param    aSerialNumber      Serial number for the OMA DRM
    *                              rights database
    * @leave    Symbian OS error codes
    */
        
    virtual void GetRdbSerialNumberL(
    	TBuf8<KRdbSerialNumberLength>& aSerialNumber) = 0;

    /**
    * GenerateNewRdbSerialNumberL(
    *
    * This method creates a new unique serial number for the OMA DRM rights database. 
    * Length of the serial number is defined by KRdbSerialNumberLength constant.
    * Should be random and must be persisted until a new serial number is generated by
    * a new call to this method.
    *
    * Recommend storing the serial number into the file system or other secure location.
    * For example under the rightsserver.exe private directory.    
    *
    * @leave    Symbian OS error codes
    */
        	
   	virtual void GenerateNewRdbSerialNumberL() = 0;

    /**
    * UdtEncryptL
    *
    * Encrypt the User Data Transfer data. Usable only in the user data transfer use case.
    * Uses it's own User Data Transfer PKI key.
    *
    * @param    aInput      Input buffer to be encrypted
    * @return   Buffer encrypted with the User Data Transfer specific PKI key.
    * @leave    Symbian OS error codes
    */
       	
   	virtual HBufC8* UdtEncryptL(
   	    const TDesC8& aInput) = 0;
   	
    /**
    * GetRootCertificatesL
    *
    * Get a list of all the root certificates of the device.
    * Returns actual binary format (.der) certificates.
    *
    * @param    aRootCerts     Param description
    * @return   Return value descriptions
    * @leave    Symbian OS error codes
    */
       	    
   	virtual void GetRootCertificatesL(
        RPointerArray<HBufC8>& aRootCerts) = 0; 

    /**
    * RandomDataGetL
    *
    * Get random data into the beginning of the aData buffer.
    *
    * @param    aData      A return buffer that has enough space for the aLength amount of data.
    * @param    aLength    Amount of random data requested
    * @leave    Symbian OS error codes
    */
    
    virtual void RandomDataGetL( 
        TDes8& aData, 
        const TInt aLength ) = 0; 

    };
    
/**
* DrmKeyStorageNewL
*
* @return   Pointer to an object which implements the MDrmKeyStorage
*           interface. Underlying implementation tells which
*           keystorage to load if there are emulator/hw specific ones
* @leave    Symbian OS error codes
*/    
    
IMPORT_C MDrmKeyStorage* DrmKeyStorageNewL();

#endif      // DRMKEYSTORAGE_H
            
// End of File
