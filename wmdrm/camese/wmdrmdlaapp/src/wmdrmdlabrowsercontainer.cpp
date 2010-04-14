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
* Description:  Implementation class for WMDRM DLA Browser Container
*
*/


// INCLUDE FILES
#include <AknDef.h>
#include <aknview.h>
#include <brctlinterface.h>
#include "wmdrmdlabrowserview.h"
#include "wmdrmdlabrowsercontainer.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserContainer::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserContainer::ConstructL(
    MBrCtlSpecialLoadObserver* aObserver )
    {
    CreateWindowL();
    SetRect( iView->ClientRect() );
    
    iBrCtlInterface = CreateBrowserControlL(
            this,                                      // parent control
            iView->ClientRect(),                       // client rect
            TBrCtlDefs::ECapabilityLoadHttpFw |
            TBrCtlDefs::ECapabilityDisplayScrollBar |
            TBrCtlDefs::ECapabilityUseDlMgr |
            TBrCtlDefs::ECapabilityCursorNavigation,   // Capabilities
            TBrCtlDefs::ECommandIdBase,                // command base
            NULL,                                      // Softkeys observer
            NULL,                                      // LinkResolver
            aObserver,                                 // Special load observer
            NULL,                                      // Layout Observer
            NULL,                                      // Dialog provider
            NULL,                                      // window observer
            NULL                                       // Download observer
            );

    iBrCtlInterface->SetBrowserSettingL( TBrCtlDefs::ESettingsAutoLoadImages,
                                         ETrue );
    iBrCtlInterface->SetBrowserSettingL( TBrCtlDefs::ESettingsCookiesEnabled,
                                         ETrue );
    iBrCtlInterface->SetBrowserSettingL( TBrCtlDefs::ESettingsEmbedded,
                                         ETrue );
    ActivateL();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserContainer::CWmDrmDlaBrowserContainer
// ---------------------------------------------------------------------------
//
CWmDrmDlaBrowserContainer::CWmDrmDlaBrowserContainer(
    CAknView* aView ) : iView( aView )
    {
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserContainer::NewL
// ---------------------------------------------------------------------------
//
CWmDrmDlaBrowserContainer* CWmDrmDlaBrowserContainer::NewL(
    CAknView* aView,
    MBrCtlSpecialLoadObserver* aObserver )
    {
    CWmDrmDlaBrowserContainer* self
        = CWmDrmDlaBrowserContainer::NewLC( aView, aObserver );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserContainer::NewLC
// ---------------------------------------------------------------------------
//
CWmDrmDlaBrowserContainer* CWmDrmDlaBrowserContainer::NewLC(
    CAknView* aView,
    MBrCtlSpecialLoadObserver* aObserver )
    {
    CWmDrmDlaBrowserContainer* self
        = new( ELeave ) CWmDrmDlaBrowserContainer( aView );
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserContainer::~CWmDrmDlaBrowserContainer
// ---------------------------------------------------------------------------
//
CWmDrmDlaBrowserContainer::~CWmDrmDlaBrowserContainer()
    {
    delete iBrCtlInterface;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserContainer::BrCtlInterface
// ---------------------------------------------------------------------------
//
CBrCtlInterface* CWmDrmDlaBrowserContainer::BrCtlInterface()
    {
    return iBrCtlInterface;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserContainer::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CWmDrmDlaBrowserContainer::CountComponentControls() const
    {
    if ( iBrCtlInterface )
        {
        return 1;
        }
    return 0;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserContainer::SizeChanged
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserContainer::SizeChanged()
    {
    if ( iBrCtlInterface )
        {
        iBrCtlInterface->SetRect( Rect() );
        }
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
        case 0:
            return iBrCtlInterface;

        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserContainer::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CWmDrmDlaBrowserContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    if ( iBrCtlInterface )
        {
        return iBrCtlInterface->OfferKeyEventL( aKeyEvent, aType );
        }
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserContainer::FocusChanged
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserContainer::FocusChanged(
    TDrawNow aDrawNow )
    {
    iBrCtlInterface->SetFocus( IsFocused() );
    CCoeControl::FocusChanged( aDrawNow );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserContainer::HandleResourceChange
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserContainer::HandleResourceChange(
    TInt aType )
    {
    if ( iBrCtlInterface )
        {
        iBrCtlInterface->HandleResourceChange( aType );
        }
    CCoeControl::HandleResourceChange( aType );
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SetRect( iView->ClientRect() );
        DrawDeferred();
        }
    }
