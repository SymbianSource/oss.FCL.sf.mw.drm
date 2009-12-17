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
* Description:  Dynamically loadable wrapper for WMDRM DLA
*
*/


#include "wmdrmdlahandler.h"
#include "wmdrmdlawrapper.h"

// ---------------------------------------------------------------------------
// CWmDrmDlaWrapper::CWmDrmDlaWrapper
// ---------------------------------------------------------------------------
//
CWmDrmDlaWrapper::CWmDrmDlaWrapper()
    {
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaWrapper::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaWrapper::ConstructL()
    {
    iDlaHandler = CWmDrmDlaHandler::NewL();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaWrapper::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CWmDrmDlaWrapper* CWmDrmDlaWrapper::NewL()
    {
    CWmDrmDlaWrapper* self = new( ELeave ) CWmDrmDlaWrapper;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaWrapper::~CWmDrmDlaWrapper
// ---------------------------------------------------------------------------
//
CWmDrmDlaWrapper::~CWmDrmDlaWrapper()
    {
    delete iDlaHandler;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaWrapper::SilentL
// ---------------------------------------------------------------------------
//
TBool CWmDrmDlaWrapper::SilentL( 
    const RFile& aFile )
    {
    return iDlaHandler->SilentL( aFile );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaWrapper::AcquireLicenseL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaWrapper::AcquireLicenseL( 
    const RFile& aFile, 
    HBufC*& aContentUrl, 
    HBufC*& aHtmlData )
    {
    iDlaHandler->AcquireLicenseL( aFile, aContentUrl, aHtmlData );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaWrapper::AcquireLicense
// ---------------------------------------------------------------------------
//
void CWmDrmDlaWrapper::AcquireLicense( 
    const RFile& aFile, 
    HBufC*& aContentUrl, 
    HBufC*& aHtmlData, 
    TRequestStatus& aStatus )
    {
    iDlaHandler->AcquireLicense( aFile, aContentUrl, aHtmlData, aStatus );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaWrapper::AcquireLicenseFromDrmHeaderL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaWrapper::AcquireLicenseFromDrmHeaderL( 
    const TDesC8& aDrmHeader, 
    HBufC* aErrorUrl, 
    HBufC*& aContentUrl, 
    HBufC*& aHtmlData )
    {
    iDlaHandler->AcquireLicenseFromDrmHeaderL( aDrmHeader, aErrorUrl, 
                                               aContentUrl, aHtmlData );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaWrapper::AcquireLicenseFromDrmHeader
// ---------------------------------------------------------------------------
//
void CWmDrmDlaWrapper::AcquireLicenseFromDrmHeader( 
    const TDesC8& aDrmHeader, 
    HBufC* aErrorUrl,
    HBufC*& aContentUrl, 
    HBufC*& aHtmlData, 
    TRequestStatus& aStatus )
    {
    iDlaHandler->AcquireLicenseFromDrmHeader( aDrmHeader, aErrorUrl, 
                                              aContentUrl, aHtmlData, 
                                              aStatus );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaWrapper::CancelLicenseAcquisition
// ---------------------------------------------------------------------------
//
void CWmDrmDlaWrapper::CancelLicenseAcquisition()
    {
    iDlaHandler->CancelLicenseAcquisition();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaWrapper::ProcessMeteringCertificateL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaWrapper::ProcessMeteringCertificateL( 
    const TDesC8& aCertificate )
    {
    iDlaHandler->ProcessMeteringCertificateL( aCertificate );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaWrapper::ProcessMeteringCertificate
// ---------------------------------------------------------------------------
//
void CWmDrmDlaWrapper::ProcessMeteringCertificate( 
    const TDesC8& aCertificate, 
    TRequestStatus& aStatus )
    {
    iDlaHandler->ProcessMeteringCertificate( aCertificate, aStatus );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaWrapper::CancelMetering
// ---------------------------------------------------------------------------
//
void CWmDrmDlaWrapper::CancelMetering()
    {
    iDlaHandler->CancelMetering();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaWrapper::SetIapId
// ---------------------------------------------------------------------------
//
void CWmDrmDlaWrapper::SetIapId( 
    TInt aIapId )
    {
    iDlaHandler->SetIapId( aIapId );
    }
