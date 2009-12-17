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
* Description:  Implementation class for WMDRM DLA functionality
*
*/


#include "wmdrmdlahandler.h"
#include "wmdrmdlahandlerimpl.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWmDrmDlaHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHandler::ConstructL()
    {
    iImpl = CWmDrmDlaHandlerImpl::NewL();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandler::CWmDrmDlaHandler
// ---------------------------------------------------------------------------
//
CWmDrmDlaHandler::CWmDrmDlaHandler()
    {
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandler::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CWmDrmDlaHandler* CWmDrmDlaHandler::NewL()
    {
    CWmDrmDlaHandler* self = CWmDrmDlaHandler::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaHandler::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CWmDrmDlaHandler* CWmDrmDlaHandler::NewLC()
    {
    CWmDrmDlaHandler* self = new( ELeave ) CWmDrmDlaHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaHandler::~CWmDrmDlaHandler
// ---------------------------------------------------------------------------
//
CWmDrmDlaHandler::~CWmDrmDlaHandler()
    {
    delete iImpl;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandler::SilentL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CWmDrmDlaHandler::SilentL( 
    const RFile& aFile )
    {
    return iImpl->SilentL( aFile );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandler::AcquireLicenseL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWmDrmDlaHandler::AcquireLicenseL( 
    const RFile& aFile, 
    HBufC*& aContentUrl, 
    HBufC*& aHtmlData )
    {
    iImpl->AcquireLicenseL( aFile, aContentUrl, aHtmlData );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandler::AcquireLicense
// ---------------------------------------------------------------------------
//
EXPORT_C void CWmDrmDlaHandler::AcquireLicense( 
    const RFile& aFile, 
    HBufC*& aContentUrl, 
    HBufC*& aHtmlData, 
    TRequestStatus& aStatus )
    {
    iImpl->AcquireLicense( aFile, aContentUrl, aHtmlData, aStatus );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandler::AcquireLicenseFromDrmHeaderL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWmDrmDlaHandler::AcquireLicenseFromDrmHeaderL( 
    const TDesC8& aDrmHeader,
    HBufC* aErrorUrl,
    HBufC*& aContentUrl, 
    HBufC*& aHtmlData )
    {
    iImpl->AcquireLicenseFromDrmHeaderL( aDrmHeader, aErrorUrl, 
                                         aContentUrl, aHtmlData );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandler::AcquireLicenseFromDrmHeader
// ---------------------------------------------------------------------------
//
EXPORT_C void CWmDrmDlaHandler::AcquireLicenseFromDrmHeader( 
    const TDesC8& aDrmHeader,
    HBufC* aErrorUrl,
    HBufC*& aContentUrl, 
    HBufC*& aHtmlData, 
    TRequestStatus& aStatus )
    {
    iImpl->AcquireLicenseFromDrmHeader( aDrmHeader, aErrorUrl, 
                                        aContentUrl, aHtmlData, aStatus );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandler::CancelLicenseAcquisition
// ---------------------------------------------------------------------------
//
EXPORT_C void CWmDrmDlaHandler::CancelLicenseAcquisition()
    {
    iImpl->CancelLicenseAcquisition();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandler::ProcessMeteringCertificateL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWmDrmDlaHandler::ProcessMeteringCertificateL( 
    const TDesC8& aCertificate )
    {
    iImpl->ProcessMeteringCertificateL( aCertificate );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandler::ProcessMeteringCertificate
// ---------------------------------------------------------------------------
//
EXPORT_C void CWmDrmDlaHandler::ProcessMeteringCertificate( 
    const TDesC8& aCertificate, 
    TRequestStatus& aStatus )
    {
    iImpl->ProcessMeteringCertificate( aCertificate, aStatus );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandler::CancelMetering
// ---------------------------------------------------------------------------
//
EXPORT_C void CWmDrmDlaHandler::CancelMetering()
    {
    iImpl->CancelMetering();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandler::SetIapId
// ---------------------------------------------------------------------------
//
EXPORT_C void CWmDrmDlaHandler::SetIapId( 
    TInt aIapId )
    {
    iImpl->SetIapId( aIapId );
    }
