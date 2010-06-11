/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*  wmdrmotaaccess.cpp
*
*/

#include <e32base.h>
#include <ecom/ecom.h>

#include <wmdrmotaaccess.h>
#include <wmdrmotaaccessecominterface.h>

// ---------------------------------------------------------------------------
// CWmDrmOtaAccess::NewL
// ---------------------------------------------------------------------------
EXPORT_C CWmDrmOtaAccess* CWmDrmOtaAccess::NewL()
    {
    CWmDrmOtaAccess* self(CWmDrmOtaAccess::NewLC());
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmOtaAccess::NewLC
// ---------------------------------------------------------------------------
EXPORT_C CWmDrmOtaAccess* CWmDrmOtaAccess::NewLC()
    {
    CWmDrmOtaAccess* self(new (ELeave) CWmDrmOtaAccess);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmOtaAccess::ConstructL
// ---------------------------------------------------------------------------
void CWmDrmOtaAccess::ConstructL()
    {
    iWmDrmOtaAccessEcomInterface = CWmDrmOtaAccessEcomInterface::NewL();
    }


// ---------------------------------------------------------------------------
// CWmDrmOtaAccess::CWmDrmOtaAccess
// ---------------------------------------------------------------------------
CWmDrmOtaAccess::CWmDrmOtaAccess()
    {
    }

// ---------------------------------------------------------------------------
// CWmDrmOtaAccess::~CWmDrmOtaAccess
// ---------------------------------------------------------------------------
EXPORT_C CWmDrmOtaAccess::~CWmDrmOtaAccess()
    {
    if (iWmDrmOtaAccessEcomInterface)
        {
        delete iWmDrmOtaAccessEcomInterface;
        }
    REComSession::FinalClose();
    }


// ---------------------------------------------------------------------------
// CWmDrmOtaAccess::GetLicenseChallengeL
// ---------------------------------------------------------------------------
EXPORT_C void CWmDrmOtaAccess::GetLicenseChallengeL( const TDesC8& aDrmHeader, 
                                                     HBufC8*& aChallenge )
    {
    // Ecom interface not available    
    if( !iWmDrmOtaAccessEcomInterface )
        {
        User::Leave( KErrNotReady );    
        }   
    
    iWmDrmOtaAccessEcomInterface->GetLicenseChallengeL( aDrmHeader, 
                                                        aChallenge );                                 
    }

// ---------------------------------------------------------------------------
// CWmDrmOtaAccess::GetLicenseChallengeL
// ---------------------------------------------------------------------------
EXPORT_C void CWmDrmOtaAccess::GetLicenseChallengeL( const TDesC8& aDrmHeader, 
                                                     HBufC8*& aChallenge, 
                                                     TRequestStatus& aStatus )
    {
    // Ecom interface not available    
    if( !iWmDrmOtaAccessEcomInterface )
        {
        User::Leave( KErrNotReady );    
        }   
    
    iWmDrmOtaAccessEcomInterface->GetLicenseChallengeL( aDrmHeader, 
                                                        aChallenge,
                                                        aStatus );                                 
    }                                                     
    
// ---------------------------------------------------------------------------
// CWmDrmOtaAccess::HandleLicenseResponseL
// ---------------------------------------------------------------------------
EXPORT_C void CWmDrmOtaAccess::HandleLicenseResponseL( const TDesC8& aResponse )
    {
    // Ecom interface not available    
    if( !iWmDrmOtaAccessEcomInterface )
        {
        User::Leave( KErrNotReady );    
        }   
    
    iWmDrmOtaAccessEcomInterface->HandleLicenseResponseL( aResponse );                                
    }    
    
// ---------------------------------------------------------------------------
// CWmDrmOtaAccess::HandleLicenseResponseL
// ---------------------------------------------------------------------------
EXPORT_C void CWmDrmOtaAccess::HandleLicenseResponseL( const TDesC8& aResponse, 
                                                       TRequestStatus& aStatus )
    {
    // Ecom interface not available    
    if( !iWmDrmOtaAccessEcomInterface )
        {
        User::Leave( KErrNotReady );    
        }   
    
    iWmDrmOtaAccessEcomInterface->HandleLicenseResponseL( aResponse,
                                                          aStatus );                                 
    }                                                           

// ---------------------------------------------------------------------------
// CWmDrmOtaAccess::CancelAllAsync
// ---------------------------------------------------------------------------
EXPORT_C void CWmDrmOtaAccess::CancelAllAsync()
    {
    // Ecom interface not available    
    if( !iWmDrmOtaAccessEcomInterface )
        {
        return;   
        }   
    
    iWmDrmOtaAccessEcomInterface->CancelAllAsync();                                 
    } 

    
// End of file
