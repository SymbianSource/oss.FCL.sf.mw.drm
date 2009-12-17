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
* Description:  Implementation of request container class
*
*/


// INCLUDES
#include "wmdrmdlarequest.h"

#define _LOGGING_FILE L"wmdrmdla.txt"
#include "logfn.h"

// ----------------------------------------------------------------------------
// CWmDrmDlaRequest::NewL
// ----------------------------------------------------------------------------
//
CWmDrmDlaRequest* CWmDrmDlaRequest::NewL(
    const TDesC8& aDrmHeader,
    HBufC* aErrorUrl,
    HBufC*& aContentUrl, 
    HBufC*& aHtmlData,
    TBool aSync,
    TRequestStatus& aStatus)
    {
    LOGFN( "CWmDrmDlaRequest::NewL(1)" );
    CWmDrmDlaRequest* self = new (ELeave) CWmDrmDlaRequest();
    CleanupStack::PushL( self );
    self->ConstructL( aDrmHeader, aErrorUrl, aContentUrl, 
                      aHtmlData, aSync, aStatus );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaRequest::NewL
// ----------------------------------------------------------------------------
//
CWmDrmDlaRequest* CWmDrmDlaRequest::NewL(
    const RFile& aFileHandle, 
    HBufC*& aContentUrl, 
    HBufC*& aHtmlData,
    TBool aSync,
    TRequestStatus& aStatus )
    {
    LOGFN( "CWmDrmDlaRequest::NewL(2)" );
    CWmDrmDlaRequest* self = new (ELeave) CWmDrmDlaRequest();
    CleanupStack::PushL( self );
    self->ConstructL( aFileHandle, aContentUrl, aHtmlData, aSync, aStatus );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaRequest::NewL
// ----------------------------------------------------------------------------
//
CWmDrmDlaRequest* CWmDrmDlaRequest::NewL( 
    const TDesC8& aCertificate,
    TBool aSync,
    TRequestStatus& aStatus )
    {
    LOGFN( "CWmDrmDlaRequest::NewL(3)" );
    CWmDrmDlaRequest* self = new (ELeave) CWmDrmDlaRequest();
    CleanupStack::PushL( self );
    self->ConstructL( aCertificate, aSync, aStatus );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaRequest::~CWmDrmDlaRequest
// ----------------------------------------------------------------------------
//
CWmDrmDlaRequest::~CWmDrmDlaRequest()
    {
    LOGFN( "CWmDrmDlaRequest::~CWmDrmDlaRequest" );
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaRequest::CWmDrmDlaRequest
// ----------------------------------------------------------------------------
//
CWmDrmDlaRequest::CWmDrmDlaRequest()
    {
    LOGFN( "CWmDrmDlaRequest::CWmDrmDlaRequest" );
    }

// ----------------------------------------------------------------------------
// CAcquireLicenseFromDrmHeaderRequest::ConstructL
// ----------------------------------------------------------------------------
//
void CWmDrmDlaRequest::ConstructL(
    const TDesC8& aDrmHeader, 
    HBufC* aErrorUrl,
    HBufC*& aContentUrl, 
    HBufC*& aHtmlData,
    TBool aSync,
    TRequestStatus& aStatus )
    {
    LOGFN( "CWmDrmDlaRequest::ConstructL(1)" );
    iType = EAcquireLicenseFromDrmHeader;
    iDrmHeader = &aDrmHeader;
    iErrorUrl = aErrorUrl;
	iContentUrl = &aContentUrl; 
	iHtmlData = &aHtmlData;
	iSync = aSync;
    iStatus = &aStatus;
    }

// ----------------------------------------------------------------------------
// CAcquireLicenseFromDrmHeaderRequest::ConstructL
// ----------------------------------------------------------------------------
//
void CWmDrmDlaRequest::ConstructL(
    const RFile& aFileHandle, 
    HBufC*& aContentUrl, 
    HBufC*& aHtmlData,
    TBool aSync,
    TRequestStatus& aStatus )
    {
    LOGFN( "CWmDrmDlaRequest::ConstructL(2)" );
    iType = EAcquireLicenseFromFileHandle;
    iFileHandle = &aFileHandle;
    iContentUrl = &aContentUrl; 
	iHtmlData = &aHtmlData;
	iSync = aSync;
    iStatus = &aStatus;
    }

// ----------------------------------------------------------------------------
// CAcquireLicenseFromDrmHeaderRequest::ConstructL
// ----------------------------------------------------------------------------
//
void CWmDrmDlaRequest::ConstructL(
    const TDesC8& aCertificate,
    TBool aSync,
    TRequestStatus& aStatus )
    {
    LOGFN( "CWmDrmDlaRequest::ConstructL(3)" );
    iType = EProcessMeteringCertificate;
    iCertificate = &aCertificate;
    iSync = aSync;
    iStatus = &aStatus;
    }

