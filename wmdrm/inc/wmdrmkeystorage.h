/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of the WMDRM key storage
*
*/


#ifndef WMDRMKEYSTORAGE_H
#define WMDRMKEYSTORAGE_H

#include <e32std.h>
#include <e32base.h>

const TInt KDeviceSpecificKeyLength = 16;

/**
 *  Hardware key storage access via the Security Driver
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CWmDrmKeyStorage ): public CBase
    {
public:

    /**
    * NewL
    * creates an instance of the CWmDrmKeyStorageClass
    *
    * @param    aKeyIndex   Index of the key to be used
    *           Optional parameter, not currently used   
    * @return   CWmDrmKeyStorage -object
    * @leave    Symbian OS or internal error code
    */
    IMPORT_C static CWmDrmKeyStorage* NewL( TInt aKeyIndex = 0 );
    
    /**
    * GetCertificateL
    * request the device certificate in 8bit buffer 
    * caller responsible for releasing the buffer
    *
    * @return   Device WMDRM certificate in an 8bit buffer
    * @leave    Symbian OS or internal error code
    */
    virtual HBufC8* GetCertificateL() = 0;
    
    /**
    * GetUniqueIdL
    *
    * request a 20byte unique Id such as SHA1 hash of some device 
    * specific value, single Unique Id per device
    *
    * caller is responsible for releasing the buffer 
    *
    * @return   Unique Id in an 8bit buffer
    * @leave    Symbian OS or internal error code
    */    
    virtual HBufC8* GetUniqueIdL() = 0;
    
    /**
    * GetUniqueIdRawL
    *
    * request a 20byte unique Id such as SHA1 hash of some device 
    * specific value, single Unique Id per device base 64 encoded
    * and in a 16bit buffer
    *
    * caller is responsible for releasing the buffer 
    *
    * @return   Base 64 encoded Unique Id in a 16bit buffer
    * @leave    Symbian OS or internal error code
    */     
    virtual HBufC* GetUniqueIdRawL() = 0;
    
    /**
    * GetUniqueIdL
    *
    * request a 20byte unique Id such as SHA1 hash of some device 
    * specific value, single Unique Id per device result is base 64 encoded
    *
    * caller is responsible for releasing the buffer 
    *
    * @return   Unique Id in an 8bit buffer base 64 encoded
    * @leave    Symbian OS or internal error code
    */  
    virtual HBufC8* GetUniqueIdB64L() = 0;

    /**
    * GetPublicKeyL
    *
    * request the WMDRM public key of the device in an 8bit descriptor
    *
    * caller is responsible for releasing the buffer 
    *
    * @return   WMDRM public key in an 8bit buffer
    * @leave    Symbian OS or internal error code
    */    
    virtual HBufC8* GetPublicKeyL() = 0;
    
    /**
    * GetPublicKeyB64L
    *
    * request the WMDRM public key of the device in an 8bit descriptor
    * base 64 encoded
    *
    * caller is responsible for releasing the buffer 
    *
    * @return   WMDRM public key in an 8bit buffer base 64 encoded
    * @leave    Symbian OS or internal error code
    */     
    virtual HBufC8* GetPublicKeyB64L() = 0;

    /**
    * SignEcDsaL
    *
    * Signs the data using the device WMDRM private key
    *
    * caller is responsible for releasing the buffer 
    *
    * @param    aData       Data buffer from which the hash is calculated
    * @return   EcDsa signature in an 8bit descriptor
    * @leave    Symbian OS or internal error code
    */     
    virtual HBufC8* SignEcDsaL( const TDesC8& aData ) = 0;
    
    /**
    * VerifyEcDsaL
    *
    * Verifies using the WMDRM public key that the signature is valid
    *
    * @param    aData       Data buffer from which the hash is calculated
    * @param    aSignature  The signature to be checked against
    * @return   ETrue if the signature check succeeded, 
                EFalse if the signature check failed
    * @leave    Symbian OS or internal error code
    */     
    virtual TBool VerifyEcDsaL( const TDesC8& aData, const TDesC8& aSignature ) = 0;
    
    
    /**
    * VerifyEcDsaL
    *
    * Verifies using the given key that the signature is valid
    *
    * @param    aKey        The key in 8bit buffer to be used for signature
    *                       calculation
    * @param    aData       Data buffer from which the hash is calculated
    * @param    aSignature  The signature to be checked against    
    * @return   ETrue if the signature check succeeded , 
                EFalse if the signature check failed
    * @leave    Symbian OS or internal error code
    */     
    virtual TBool VerifyEcDsaL( const TDesC8& aKey, const TDesC8& aData, const TDesC8& aSignature ) = 0;
    
    
    /**
    * DecryptEcIesL
    *
    * Decrypts the EcIes encrypted data using the device private key
    *
    * caller is responsible for releasing the buffer 
    *
    * @param    aData       Data buffer containing the encrypted data
    * @return   Decrypted data in 8bit descriptor
    * @leave    Symbian OS or internal error code
    */     
    virtual HBufC8* DecryptEcIesL( const TDesC8& aData ) = 0;

    /**
    * EncryptEcIesL
    *
    * Encrypts the data using the device WMDRM private key
    *
    * caller is responsible for releasing the buffer 
    *
    * @param    aData       Data buffer to be encrypted
    * @return   Encrypted data in 8bit descriptor
    * @leave    Symbian OS or internal error code
    */
    virtual HBufC8* EncryptEcIesL( const TDesC8& aData ) = 0;
    
    /**
    * EncryptEcIesL
    *
    * Encrypts the data using the given key
    *
    * caller is responsible for releasing the buffer 
    *
    * @param    aKey        Key to be used for EcIes encryption in an 8bit buffer
    * @param    aData       Data buffer to be encrypted
    * @return   Encrypted data in 8bit descriptor
    * @leave    Symbian OS or internal error code
    */    
    virtual HBufC8* EncryptEcIesL( const TDesC8& aKey, const TDesC8& aData ) = 0;


    /**
    * GetDeviceSpecificKeyL
    *
    * request a device specific 16 byte key in an 8bit descriptor
    *
    * caller is responsible for releasing the buffer 
    *
    * @param    aKey    Device specific 16 byte key in an 8bit buffer
    * @leave    Symbian OS or internal error code
    */  
    virtual void GetDeviceSpecificKeyL(
        TBuf8<KDeviceSpecificKeyLength>& aKey) = 0;

    /**
    * GetSymmetricKeyL
    *
    * request the WMDRM symmetric key from the WMDRM certificate in 
    * an 8bit descriptor
    *
    * caller is responsible for releasing the buffer 
    *
    * @return   WMDRM symmetric key in an 8bit buffer
    * @leave    Symbian OS or internal error code
    */  
    virtual HBufC8* GetSymmetricKeyL() = 0;
    };

#endif // WMDRMKEYSTORAGE_H
