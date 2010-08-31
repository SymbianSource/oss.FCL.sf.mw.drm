/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation class for WMDRM DLA App Service Session
*
*/


// INCLUDE FILES
#include "wmdrmdlaappui.h"
#include "wmdrmdlabrowserview.h"
#include "wmdrmdlaappservicesession.h"
#include "wmdrmdlaappconstants.h"

// CONSTANTS
const TInt KSanityDataLengthLow = 0;
const TInt KSanityDataLengthHigh = KMaxTInt32;

// ============================= LOCAL FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// SanitizeL
// Performs a sanity check on length parameters
// ----------------------------------------------------------------------------
//
LOCAL_C void SanitizeL( TInt aParam )
    {
    if ( aParam <= KSanityDataLengthLow || aParam > KSanityDataLengthHigh )
        {
        User::Leave( KErrArgument );
        }
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CWmDrmDlaAppServiceSession::CWmDrmDlaAppServiceSession
// ---------------------------------------------------------------------------
//
CWmDrmDlaAppServiceSession::CWmDrmDlaAppServiceSession()
    {
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaAppServiceSession::~CWmDrmDlaAppServiceSession
// ---------------------------------------------------------------------------
//
CWmDrmDlaAppServiceSession::~CWmDrmDlaAppServiceSession()
    {
    delete iWait;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaAppServiceSession::LicenseReceived
// ---------------------------------------------------------------------------
//
void CWmDrmDlaAppServiceSession::LicenseReceived()
    {
    //License received, stop waiting and complete post-service
    if ( iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaAppServiceSession::ServiceL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaAppServiceSession::ServiceL( const RMessage2& aMessage )
    {
    switch ( aMessage.Function() )
        {
        case ESetIAP:
            SetIapL( aMessage );
            break;
        
        case EPost:
            PostL( aMessage );
            break;
        
        case EGetLicenseResponseSize:
            LicenseResponseSizeL( aMessage );        
            break;
            
        case EGetLicenseResponse:
            LicenseResponseL( aMessage );        
            break;    
            
        default:
             CAknAppServiceBase::ServiceL( aMessage );
             break;
        }
     }

// ---------------------------------------------------------------------------
// CWmDrmDlaAppServiceSession::SetIapL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaAppServiceSession::SetIapL( const RMessage2& aMessage )
    {
    TInt iap( 0 );
    TPckg<TInt> iapPckg( iap );
    
    SanitizeL( aMessage.GetDesLength( 0 ) );
    TInt size = aMessage.GetDesLength( 0 );
    if ( size != sizeof( TInt ) )
        {
        User::Leave( KErrArgument );
        }
    aMessage.ReadL( 0, iapPckg );
    
    CompleteMessage( aMessage, KErrNone );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaAppServiceSession::PostL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaAppServiceSession::PostL( const RMessage2& aMessage )
    {
    HBufC* postUrl( NULL );
    TPtr ptr16( NULL, 0 );
    HBufC8* postContentType( NULL );
    HBufC8* postData( NULL );
    HBufC8* postContentBoundary( NULL );
    TPtr8 ptr8( NULL, 0 );
    
    SanitizeL( aMessage.GetDesLength( 0 ) );
    postUrl = HBufC::NewLC( aMessage.GetDesLength( 0 ) );
    ptr16.Set( postUrl->Des() );
    aMessage.ReadL( 0, ptr16 );
    
    SanitizeL( aMessage.GetDesLength( 1 ) );
    postContentType = HBufC8::NewLC( aMessage.GetDesLength( 1 ) );
    ptr8.Set( postContentType->Des() );
    aMessage.ReadL( 1, ptr8 );

    SanitizeL( aMessage.GetDesLength( 2 ) );
    postData = HBufC8::NewLC( aMessage.GetDesLength( 2 ) );
    ptr8.Set( postData->Des() );
    aMessage.ReadL( 2, ptr8 );    

    SanitizeL( aMessage.GetDesLength( 3 ) );
    postContentBoundary = HBufC8::NewLC( aMessage.GetDesLength( 3 ) );
    ptr8.Set( postContentBoundary->Des() );
    aMessage.ReadL( 3, ptr8 );
    
    CleanupStack::PopAndDestroy( 4, postUrl ); //postContentBoundary, postData,
                                               //postContentType, postUrl
    
    if ( !iWait )
        {
        iWait = new (ELeave) CActiveSchedulerWait;
        }
    //Wait until license is received
    iWait->Start();
    
    CompleteMessage( aMessage, KErrNone );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaAppServiceSession::LicenseResponseSizeL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaAppServiceSession::LicenseResponseSizeL( 
    const RMessage2& aMessage )
    {
    TInt err( KErrNone );
    
        err = KErrArgument;
    
    CompleteMessage( aMessage, err );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaAppServiceSession::LicenseResponseL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaAppServiceSession::LicenseResponseL( const RMessage2& aMessage )
    {
    TInt err( KErrNone );
    
        err = KErrArgument;
   
    CompleteMessage( aMessage, err );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaAppServiceSession::CompleteMessage
// ---------------------------------------------------------------------------
//
void CWmDrmDlaAppServiceSession::CompleteMessage( 
    const RMessage2& aMessage,
    TInt aError)
    {
    if ( !aMessage.IsNull() )
        {
        aMessage.Complete( aError );
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaAppServiceSession::BrowserView
// ---------------------------------------------------------------------------
//
CWmDrmDlaBrowserView* CWmDrmDlaAppServiceSession::BrowserView()
    {
    return ((CWmDrmDlaAppUi*)CEikonEnv::Static()->EikAppUi())->BrowserView();
    }
