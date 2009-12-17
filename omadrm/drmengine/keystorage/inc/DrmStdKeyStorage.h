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



#ifndef DRMSTDKEYSTORAGE_H
#define DRMSTDKEYSTORAGE_H

// FORWARD DECLARATIONS

class CRSAPrivateKeyStandard;

// CLASS DECLARATION

/**
*  CDrmKeyStorage: Contains key storage for OMA DRM 2.0
*
*  @lib    -
*  @since  3.0
*/
NONSHARABLE_CLASS(CDrmStdKeyStorage): public MDrmKeyStorage, public CBase
    {
public: // New functions

    IMPORT_C static CDrmStdKeyStorage* NewL(RLibrary aLibrary);
    
    virtual ~CDrmStdKeyStorage();
    
public: // From MDrmKeyStorage

    TInt ModulusSize();

    void SelectTrustedRootL(
        const TDesC8& aRootKeyHash);
        
    void SelectDefaultRootL();
        
    TBool SelectedRootIsCmla();
        
    void GetTrustedRootsL(
        RPointerArray<HBufC8>& aRootList);
        
    void GetCertificateChainL(
        RPointerArray<HBufC8>& aCertChain);
        
    HBufC8* RsaSignL(
        const TDesC8& aInput);
    
    HBufC8* RsaDecryptL(
        const TDesC8& aInput);
        
    void ImportDataL(
        const TDesC8& aPrivateKey,
        const RArray<TPtrC8>& aCertificateChain);
        
    void GetDeviceSpecificKeyL(
        TBuf8<KDeviceSpecificKeyLength>& aKey);

    void GetRdbSerialNumberL(
    	TBuf8<KRdbSerialNumberLength>& aSerialNumber);
    	
	void GenerateNewRdbSerialNumberL();
	
	HBufC8* UdtEncryptL(
	    const TDesC8& aInput);
	    
    void GetRootCertificatesL(
        RPointerArray<HBufC8>& aRootCerts);
     
    void RandomDataGetL( 
        TDes8& aData, 
        const TInt aLength ); 
    	
protected: // New functions

    void ConstructL();
    
    CDrmStdKeyStorage(RLibrary aLibrary);
    
    HBufC8* ModularExponentiateWithKeyL(
        const TDesC8& aInput);
        
    void InitializeKeyL();
    
    void CheckRootForCmlaL();
    
    const TDesC& GetImeiL();
    
protected: // Data

    RFs iFs;
    CFileMan* iFileMan;    
    TBool iRootSelected;
    TBool iRootIsCmla;
    CRSAPrivateKeyStandard* iKey;
    TBuf8<KDeviceSpecificKeyLength> iDeviceSpecificKey;
    HBufC* iImei;
    RLibrary iLibrary;

    };

#endif      // DRMSTDKEYSTORAGE_H
            
// End of File
