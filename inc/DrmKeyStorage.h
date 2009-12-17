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



#ifndef DRMKEYSTORAGE_H
#define DRMKEYSTORAGE_H

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  CDrmKeyStorage: Contains key storage for OMA DRM 2.0
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

    virtual ~MDrmKeyStorage() = 0;
    
    virtual TInt ModulusSize() = 0;

    virtual void SelectTrustedRootL(
        const TDesC8& aRootKeyHash) = 0;
        
    virtual TBool SelectedRootIsCmla() = 0;
        
    virtual void SelectDefaultRootL() = 0;
        
    virtual void GetTrustedRootsL(
        RPointerArray<HBufC8>& aRootList) = 0;
        
    virtual void GetCertificateChainL(
        RPointerArray<HBufC8>& aCertChain) = 0;
        
    virtual HBufC8* RsaSignL(
        const TDesC8& aInput) = 0;
    
    virtual HBufC8* RsaDecryptL(
        const TDesC8& aInput) = 0;
        
    virtual void ImportDataL(
        const TDesC8& aPrivateKey,
        const RArray<TPtrC8>& aCertificateChain) = 0;
        
    virtual void GetDeviceSpecificKeyL(
        TBuf8<KDeviceSpecificKeyLength>& aKey) = 0;
        
    virtual void GetRdbSerialNumberL(
    	TBuf8<KRdbSerialNumberLength>& aSerialNumber) = 0;
    	
   	virtual void GenerateNewRdbSerialNumberL() = 0;
   	
   	virtual HBufC8* UdtEncryptL(
   	    const TDesC8& aInput) = 0;
   	    
   	virtual void GetRootCertificatesL(
        RPointerArray<HBufC8>& aRootCerts) = 0; 

    virtual void RandomDataGetL( 
        TDes8& aData, 
        const TInt aLength ) = 0; 

    };
    
IMPORT_C MDrmKeyStorage* DrmKeyStorageNewL();

#endif      // DRMKEYSTORAGE_H
            
// End of File
