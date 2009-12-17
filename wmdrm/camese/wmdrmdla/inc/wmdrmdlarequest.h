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
* Description:  Defines the request container class
*
*/


#ifndef WMDRMDLAREQUEST_H
#define WMDRMDLAREQUEST_H

// INCLUDES
#include <e32base.h>
#include <f32file.h>

/**
* CWmDrmDlaRequest
*
*  @lib wmdrmdla.dll
*  @since 9.1
*/
NONSHARABLE_CLASS( CWmDrmDlaRequest ) : public CBase
    {
    
    public:
        
        enum TRequestType
            {
            EAcquireLicenseFromDrmHeader,
            EAcquireLicenseFromFileHandle,
            EProcessMeteringCertificate
            };

        //License Pre-delivery
        static CWmDrmDlaRequest* NewL( const TDesC8& aDrmHeader,  HBufC* aErrorUrl,
                                       HBufC*& aContentUrl, HBufC*& aHtmlData, 
                                       TBool aSync, TRequestStatus& aStatus );
        
        //License Post-delivery
        static CWmDrmDlaRequest* NewL( const RFile& aFileHandle, HBufC*& aContentUrl, 
                                       HBufC*& aHtmlData, TBool aSync, 
                                       TRequestStatus& aStatus );
        
        //Metering
        static CWmDrmDlaRequest* NewL( const TDesC8& aCertificate, TBool aSync, 
                                       TRequestStatus& aStatus );

        virtual ~CWmDrmDlaRequest();
    
    private:
        
        CWmDrmDlaRequest();

        //License Pre-delivery
        void ConstructL( const TDesC8& aDrmHeader, HBufC* aErrorUrl,
                         HBufC*& aContentUrl, HBufC*& aHtmlData, 
                         TBool aSync, TRequestStatus& aStatus );
        
        //License Post-delivery
        void ConstructL( const RFile& aFileHandle, HBufC*& aContentUrl, 
                         HBufC*& aHtmlData, TBool aSync, 
                         TRequestStatus& aStatus );
        
        //Metering
        void ConstructL( const TDesC8& aCertificate, TBool aSync, 
                         TRequestStatus& aStatus );
        
    public:
		
        TRequestType iType;
        TBool iSync;
        
		//Not owned
        const TDesC8* iDrmHeader;
        const TDesC8* iCertificate;
        const RFile* iFileHandle;
        TRequestStatus* iStatus;
        HBufC* iErrorUrl;
        HBufC** iContentUrl;
		HBufC** iHtmlData;
		
    };

#endif  // WMDRMDLAREQUEST_H

// End of File
