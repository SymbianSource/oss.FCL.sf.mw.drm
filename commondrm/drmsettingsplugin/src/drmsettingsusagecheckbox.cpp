/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CheckBox class for Usage Reporting settings
*
*/


// INCLUDE FILES
#include <stringloader.h>
#include <aknquerydialog.h>
#include <akninfopopupnotecontroller.h>
#include <drmsettingspluginrsc.rsg>

#include "drmsettingsusagecheckbox.h"
#include "drmsettingsusagelist.h"
#include "drmsettingsmodel.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CDrmSettingUsageCheckBox::CDrmSettingUsageCheckBox
// Default constructor.
// -----------------------------------------------------------------------------
//
CDrmSettingUsageCheckBox::CDrmSettingUsageCheckBox( 
    TInt aResourceId,
    CDRMSettingUsageList* aList,
    CDRMSettingsModel* aModel ) : CAknCheckBoxSettingPage( aResourceId, aList ),
                                  iList( aList ),
                                  iModel( aModel )
    {
	}

// ----------------------------------------------------------------------------
// CDrmSettingUsageCheckBox::~CDrmSettingUsageCheckBox
//
// Destructor
// ----------------------------------------------------------------------------
//
CDrmSettingUsageCheckBox::~CDrmSettingUsageCheckBox()
    {
    delete iPopupController;
    }

// -----------------------------------------------------------------------------
// CDrmSettingUsageCheckBox::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CDrmSettingUsageCheckBox::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    TKeyResponse response( EKeyWasNotConsumed );
    if ( aKeyEvent.iCode != EKeyApplicationF )
        {
        response = this->ListBoxControl()->OfferKeyEventL( aKeyEvent, aType );
        }
    
    if ( aType == EEventKeyUp )
        {
        ShowInfoPopupL();
        }
    
    return response;
    }

// -----------------------------------------------------------------------------
// CDrmSettingUsageCheckBox::DynamicInitL
// -----------------------------------------------------------------------------
//
void CDrmSettingUsageCheckBox::DynamicInitL()
    {
    HBufC* emptyText( StringLoader::LoadLC( R_USAGE_REPORTING_LIST_EMPTY,
                                            iEikonEnv ) );
	this->ListBoxControl()->View()->SetListEmptyTextL( *emptyText );
	CleanupStack::PopAndDestroy( emptyText );
	}

// -----------------------------------------------------------------------------
// CDrmSettingUsageCheckBox::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CDrmSettingUsageCheckBox::OkToExitL( TBool aAccept )
    {
    TBool exit( ETrue );
    
    if ( aAccept )
        {
        iList->UpdateContexts();
    
        if ( !iModel->IsMeteringAllowedForAll() )
            {
            HBufC* query( StringLoader::LoadLC( R_DRM_CONF_QUERY_METERING,
                                                iEikonEnv ) );
        
            CAknQueryDialog* queryDialog( CAknQueryDialog::NewL() );
        
            TBool retVal( queryDialog->ExecuteLD( R_DRM_CONFIRMATION_QUERY_METERING,
                                                  *query ) );
            CleanupStack::PopAndDestroy( query );
            
            if ( !retVal )
                {
                exit = EFalse;
                }
            }
        }
    
    return exit;
    }

// -----------------------------------------------------------------------------
// CDrmSettingUsageCheckBox::AcceptSettingL
// -----------------------------------------------------------------------------
//
void CDrmSettingUsageCheckBox::AcceptSettingL()
    {
    iModel->SaveMeteringChanges();
    }

// -----------------------------------------------------------------------------
// CDrmSettingUsageCheckBox::ShowInfoPopupL
// -----------------------------------------------------------------------------
//
void CDrmSettingUsageCheckBox::ShowInfoPopupL()
    {
    if ( !iPopupController )
        {
        iPopupController = CAknInfoPopupNoteController::NewL();
        }
    TInt index( this->ListBoxControl()->View()->CurrentItemIndex() );
    if ( index != -1 )
        {
        iPopupController->SetTextL( iList->At(index)->ItemText() );
        iPopupController->ShowInfoPopupNote();    
        }
    }

//  End of File
