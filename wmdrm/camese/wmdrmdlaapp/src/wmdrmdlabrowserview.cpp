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
* Description:  Implementation class for WMDRM DLA Browser View
*
*/


// INCLUDE FILES
#include <f32file.h>
#include <bautils.h>
#include <aknViewAppUi.h>
#include <coeaui.h>
#include <coecntrl.h>
#include <wmdrmdlaapp.rsg>
#include "wmdrmdlabrowserview.h"
#include "wmdrmdlabrowsercontainer.h"
#include "wmdrmdlaappconstants.h"

// CONTANTS

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserView::ConstructL()
    {
    BaseConstructL( R_WMDRMDLA_APP_BROWSER_VIEW );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::CWmDrmDlaBrowserView
// ---------------------------------------------------------------------------
//
CWmDrmDlaBrowserView::CWmDrmDlaBrowserView()
    {
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::NewL
// ---------------------------------------------------------------------------
//
CWmDrmDlaBrowserView* CWmDrmDlaBrowserView::NewL()
    {
    CWmDrmDlaBrowserView* self = CWmDrmDlaBrowserView::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::NewLC
// ---------------------------------------------------------------------------
//
CWmDrmDlaBrowserView* CWmDrmDlaBrowserView::NewLC()
    {
    CWmDrmDlaBrowserView* self = new( ELeave ) CWmDrmDlaBrowserView();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::~CWmDrmDlaBrowserView
// ---------------------------------------------------------------------------
//
CWmDrmDlaBrowserView::~CWmDrmDlaBrowserView()
    {
    RemoveContainer();
    delete iLicenseResponse;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::Id
// ---------------------------------------------------------------------------
//
TUid CWmDrmDlaBrowserView::Id() const
    {
    return KWmDrmDlaAppBrowserViewId;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::DoActivateL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserView::DoActivateL(
    const TVwsViewId& /*aPrevViewId*/,
    TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    CreateContainerL();
    AppUi()->AddToStackL( *this, iContainer );
    HandleClientRectChange();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::DoDeactivate
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserView::DoDeactivate()
    {
    RemoveContainer();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::HandleClientRectChange
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::CreateContainerL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserView::CreateContainerL()
    {
    RemoveContainer();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::RemoveContainer
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserView::RemoveContainer()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }
