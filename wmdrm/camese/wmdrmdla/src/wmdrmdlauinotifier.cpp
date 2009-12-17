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
* Description:  Implementation of WMDRM DLA UI Notifier
*
*/


#include <wmdrmdlauinotifier.h>
#include <wmdrmdlacancelobserver.h>
#include "wmdrmdlauinotifierimpl.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifier::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaUiNotifier::ConstructL()
    {
    iImpl = CWmDrmDlaUiNotifierImpl::NewL();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifier::CWmDrmDlaUiNotifier
// ---------------------------------------------------------------------------
//
CWmDrmDlaUiNotifier::CWmDrmDlaUiNotifier()
    {
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifier::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CWmDrmDlaUiNotifier* CWmDrmDlaUiNotifier::NewL()
    {
    CWmDrmDlaUiNotifier* self = CWmDrmDlaUiNotifier::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifier::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CWmDrmDlaUiNotifier* CWmDrmDlaUiNotifier::NewLC()
    {
    CWmDrmDlaUiNotifier* self = new( ELeave ) CWmDrmDlaUiNotifier;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifier::~CWmDrmDlaUiNotifier
// ---------------------------------------------------------------------------
//
CWmDrmDlaUiNotifier::~CWmDrmDlaUiNotifier()
    {
    delete iImpl;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifier::InitializeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWmDrmDlaUiNotifier::InitializeL( 
    TUid aUid )
    {
    iImpl->InitializeL( aUid );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifier::InitializeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWmDrmDlaUiNotifier::InitializeL( 
    TDesC8& aOpaqueData )
    {
    iImpl->InitializeL( aOpaqueData );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifier::SetCancelObserver
// ---------------------------------------------------------------------------
//
EXPORT_C void CWmDrmDlaUiNotifier::SetCancelObserver( 
    MWmDrmDlaCancelObserver* aObserver )
    {
    iImpl->SetCancelObserver( aObserver );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifier::SetSilent
// ---------------------------------------------------------------------------
//
EXPORT_C void CWmDrmDlaUiNotifier::SetSilent( 
    TBool aSilent )
    {
    iImpl->SetSilent( aSilent );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifier::SetLicenseAcquisitionProgress
// ---------------------------------------------------------------------------
//
EXPORT_C void CWmDrmDlaUiNotifier::SetProgress( 
    TInt aState )
    {
    iImpl->SetProgress( aState );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifier::HandleErrorL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWmDrmDlaUiNotifier::HandleErrorL( 
    TInt aError )
    {
    iImpl->HandleErrorL( aError );
    }
