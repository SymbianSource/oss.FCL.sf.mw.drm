/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for WMDRM DLA Wrapper
*
*/


#ifndef CWMDRMDLAWRAPPER_H
#define CWMDRMDLAWRAPPER_H

#include <e32base.h>
#include <f32file.h>

_LIT( KWmDrmDlaWrapperName, "\\system\\libs\\wmdrmdlawrapper.dll" );
const TInt KWmDrmDlaNewL = 1;

class CWmDrmDlaHandler;

class MWmDrmDlaWrapper
    {

public:

    virtual TBool SilentL( const RFile& aFile ) = 0;
    
    virtual void AcquireLicenseL( const RFile& aFile, 
                                  HBufC*& aContentUrl, 
                                  HBufC*& aHtmlData ) = 0;
    
    virtual void AcquireLicense( const RFile& aFile, 
                                 HBufC*& aContentUrl, 
                                 HBufC*& aHtmlData, 
                                 TRequestStatus& aStatus ) = 0;
    
    virtual void AcquireLicenseFromDrmHeaderL( const TDesC8& aDrmHeader, 
                                               HBufC* aErrorUrl, 
                                               HBufC*& aContentUrl, 
                                               HBufC*& aHtmlData ) = 0;
    
    virtual void AcquireLicenseFromDrmHeader( const TDesC8& aDrmHeader, 
                                              HBufC* aErrorUrl,
                                              HBufC*& aContentUrl,
                                              HBufC*& aHtmlData, 
                                              TRequestStatus& aStatus ) = 0;
    
    virtual void CancelLicenseAcquisition() = 0;
    
    virtual void ProcessMeteringCertificateL( const TDesC8& aCertificate ) = 0;
    
    virtual void ProcessMeteringCertificate( const TDesC8& aCertificate, 
                                             TRequestStatus& aStatus ) = 0;
    
    virtual void CancelMetering() = 0;
    
    virtual void SetIapId( TInt aIapId ) = 0;
    
    };

NONSHARABLE_CLASS( CWmDrmDlaWrapper ): public CBase,
                                       public MWmDrmDlaWrapper
    {

public:

    IMPORT_C static CWmDrmDlaWrapper* NewL();
    virtual ~CWmDrmDlaWrapper();

    TBool SilentL( const RFile& aFile );
    
    void AcquireLicenseL( const RFile& aFile, HBufC*& aContentUrl, 
                          HBufC*& aHtmlData );
    
    void AcquireLicense( const RFile& aFile, HBufC*& aContentUrl, 
                         HBufC*& aHtmlData, TRequestStatus& aStatus );
    
    void AcquireLicenseFromDrmHeaderL( const TDesC8& aDrmHeader, 
                                       HBufC* aErrorUrl,
                                       HBufC*& aContentUrl, 
                                       HBufC*& aHtmlData );
    
    void AcquireLicenseFromDrmHeader( const TDesC8& aDrmHeader, 
                                      HBufC* aErrorUrl,
                                      HBufC*& aContentUrl, 
                                      HBufC*& aHtmlData, 
                                      TRequestStatus& aStatus );
    
    void CancelLicenseAcquisition();
    
    void ProcessMeteringCertificateL( const TDesC8& aCertificate );
    
    void ProcessMeteringCertificate( const TDesC8& aCertificate, 
                                     TRequestStatus& aStatus );
    
    void CancelMetering();
    
    void SetIapId( TInt aIapId );
    
private:

    CWmDrmDlaWrapper();
    void ConstructL();

private: // data

    CWmDrmDlaHandler* iDlaHandler;
    
    };

#endif // CWMDRMDLAWRAPPER_H
