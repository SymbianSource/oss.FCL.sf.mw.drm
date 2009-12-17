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
* Description:  Implementation class for WMDRM DLA App Ui
*
*/


#include "wmdrmdlaappui.h"
#include "wmdrmdlabrowserview.h"
#include "wmdrmdlaappconstants.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CWmDrmDlaAppUi::CWmDrmDlaAppUi
// ---------------------------------------------------------------------------
//
CWmDrmDlaAppUi::CWmDrmDlaAppUi()
    {
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaAppUi::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin | EAknSingleClickCompatible );
    CWmDrmDlaBrowserView* browserView = CWmDrmDlaBrowserView::NewLC();
    AddViewL( browserView );
    CleanupStack::Pop( browserView );
    iBrowserView = browserView;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaAppUi::~CWmDrmDlaAppUi
// ---------------------------------------------------------------------------
//
CWmDrmDlaAppUi::~CWmDrmDlaAppUi()
    {
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaAppUi::BrowserView
// ---------------------------------------------------------------------------
//
CWmDrmDlaBrowserView* CWmDrmDlaAppUi::BrowserView()
    {
    return iBrowserView;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaAppUi::HandleCommandL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyClose:
        case EAknSoftkeyExit:
        case EEikCmdExit:
            Exit();
            break;
        
        default:
            break;
        }
    }
