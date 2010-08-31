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
* Description:  Implementation class for WMDRM DLA Browser Container
*
*/


// INCLUDE FILES
#include <AknDef.h>
#include <aknview.h>
#include "wmdrmdlabrowserview.h"
#include "wmdrmdlabrowsercontainer.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserContainer::CWmDrmDlaBrowserContainer
// ---------------------------------------------------------------------------
//
CWmDrmDlaBrowserContainer::CWmDrmDlaBrowserContainer(
    CAknView* aView ) : iView( aView )
    {
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserContainer::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CWmDrmDlaBrowserContainer::ComponentControl(
    TInt aIndex ) const
    {
    switch ( aIndex )
        {
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserContainer::FocusChanged
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserContainer::FocusChanged(
    TDrawNow aDrawNow )
    {
    CCoeControl::FocusChanged( aDrawNow );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserContainer::HandleResourceChange
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserContainer::HandleResourceChange(
    TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SetRect( iView->ClientRect() );
        DrawDeferred();
        }
    }
