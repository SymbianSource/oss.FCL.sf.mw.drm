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
* Description:  Implementation class for WMDRM DLA App Client
*
*/


// INCLUDE FILES
#include "wmdrmdlaappclient.h"
#include "wmdrmdlaappconstants.h"

#define _LOGGING_FILE L"wmdrmdlautils.txt"
#include "logfn.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// RWmDrmDlaAppClient::~RWmDrmDlaAppClient
// ---------------------------------------------------------------------------
//
RWmDrmDlaAppClient::~RWmDrmDlaAppClient()
    {
    LOGFN( "RWmDrmDlaAppClient::~RWmDrmDlaAppClient" );
    Close();
    }

// ---------------------------------------------------------------------------
// RWmDrmDlaAppClient::SetIAP
// ---------------------------------------------------------------------------
//
TInt RWmDrmDlaAppClient::SetIAP( TInt aIap )
    {
    TInt err( KErrNone );
    
    LOGFNR( "RWmDrmDlaAppClient::SetIAP", err );
    
    TPckg<TInt> iapPckg( aIap );
    err = SendReceive( ESetIAP, TIpcArgs( &iapPckg ) );
    return err;
    }

// ---------------------------------------------------------------------------
// RWmDrmDlaAppClient::Post
// ---------------------------------------------------------------------------
//
TInt RWmDrmDlaAppClient::Post(
    const TDesC& aPostUrl,
    const TDesC8& aPostContentType,
    const TDesC8& aPostData,
    const TDesC8& aPostContentBoundary )
    {
    TInt err( KErrNone );
    
    LOGFNR( "RWmDrmDlaAppClient::Post", err );
    
    err = SendReceive( EPost, TIpcArgs( &aPostUrl, 
                                        &aPostContentType, 
                                        &aPostData, 
                                        &aPostContentBoundary ) );
    return err;
    }

// ---------------------------------------------------------------------------
// RWmDrmDlaAppClient::LicenseResponse
// ---------------------------------------------------------------------------
//
TInt RWmDrmDlaAppClient::LicenseResponse( HBufC8*& aLicenseResponse )
    {
    TInt err( KErrNone );
    TInt size( 0 );
    TPckg<TInt> sizePckg( size );
    
    LOGFNR( "RWmDrmDlaAppClient::LicenseResponse", err );
    
    err = SendReceive( EGetLicenseResponseSize, TIpcArgs( &sizePckg )  );
    if  ( !err && size > 0 )
        {
        TRAP( err, aLicenseResponse = HBufC8::NewL( size ) );
        if ( !err )
            {
            TPtr8 ptr( aLicenseResponse->Des() );
            err = SendReceive( EGetLicenseResponse, TIpcArgs( &ptr ) );
            if ( err )
                {
                delete aLicenseResponse;
                aLicenseResponse = NULL;
                }
            }
        }
    return err;
    }

// ---------------------------------------------------------------------------
// RWmDrmDlaAppClient::ServiceUid
// ---------------------------------------------------------------------------
//
TUid RWmDrmDlaAppClient::ServiceUid() const
    {
    return KWmDrmDlaAppServiceId;
    }
