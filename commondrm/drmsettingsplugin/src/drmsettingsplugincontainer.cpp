/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container for DRMSettinsPlugin
*
*/


// INCLUDE FILES
#include <aknlists.h>
#include <aknutils.h>
#include <csxhelp/drm.hlp.hrh>
#include <gslistbox.h>
#include <stringloader.h>
#include <drmsettingspluginrsc.rsg>

#include "drmsettingsplugincontainer.h"
#include "drmsettingsplugin.hrh"
#include "drmsettingsmodel.h"

const TUid KUidRightsManager = { 0x101F85C7 };

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CDRMSettingsPluginContainer::ConstructL()
// 
// Symbian OS two phased constructor
// ---------------------------------------------------------------------------
//
void CDRMSettingsPluginContainer::ConstructL( const TRect& aRect )
    {
    iListBox = new( ELeave ) CAknSettingStyleListBox;
    iModel = CDRMSettingsModel::NewL();
    BaseConstructL( aRect, R_DRM_SETTINGS_VIEW_TITLE, R_DRM_SETTINGS_LBX );
    }

// ---------------------------------------------------------------------------
// CDRMSettingsPluginContainer::CDRMSettingsPluginContainer()
// 
// Constructor 
// ---------------------------------------------------------------------------
//
CDRMSettingsPluginContainer::CDRMSettingsPluginContainer( 
    TBool aWmdrmSupported ) : iWmdrmSupported( aWmdrmSupported )
    {
    }

// ---------------------------------------------------------------------------
// CDRMSettingsPluginContainer::~CDRMSettingsPluginContainer()
// 
// Destructor 
// ---------------------------------------------------------------------------
//
CDRMSettingsPluginContainer::~CDRMSettingsPluginContainer()
    {
    if ( iModel )
        {
        delete iModel;
        }
    // delete listbox item array
    if ( iListboxItemArray )
        {
        delete iListboxItemArray;
        }
    }


// ---------------------------------------------------------------------------
// CDRMSettingsPluginContainer::ConstructListBoxL()
// 
// Construct the listbox from resource array.
// ---------------------------------------------------------------------------
//
void CDRMSettingsPluginContainer::ConstructListBoxL( TInt aResLbxId )
    {
    iListBox->ConstructL( this, EAknListBoxSelectionList /* | 
        EAknListBoxItemSpecificMenuDisabled */ );
    iListboxItemArray = CGSListBoxItemTextArray::NewL( aResLbxId, 
                                                       *iListBox, 
                                                       *iCoeEnv );
    iListBox->Model()->SetItemTextArray( iListboxItemArray );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    CreateListBoxItemsL();
    }


// ---------------------------------------------------------------------------
// CDRMSettingsPluginContainer::CreateListBoxItemsL()
// 
// Create listbox items.
// ---------------------------------------------------------------------------
//
void CDRMSettingsPluginContainer::CreateListBoxItemsL()
    {
#ifdef __DRM_OMA2
    MakeTransactionTrackingItemL();
    
#ifdef RD_DRM_SILENT_RIGHTS_ACQUISITION
    MakeAutomaticActivationItemL();
#endif // RD_DRM_SILENT_RIGHTS_ACQUISITION

#ifdef RD_DRM_METERING
    MakeUsageReportingItemL();
#endif // RD_DRM_METERING
#endif // __DRM_OMA2


    MakeWMDRMLicenseDeletionItemL();
    }


// ---------------------------------------------------------------------------
// CDRMSettingsPluginContainer::UpdateListBoxL()
// 
// Update listbox item.
// ---------------------------------------------------------------------------
//
void CDRMSettingsPluginContainer::UpdateListBoxL( TInt aFeatureId )
    {
    switch( aFeatureId )
        {
#ifdef __DRM_OMA2
        case EDRMSettingsIdTransactionTracking:
            MakeTransactionTrackingItemL();
            break;
            
#ifdef RD_DRM_SILENT_RIGHTS_ACQUISITION
        case EDRMSettingsIdAutomaticActivation:
            MakeAutomaticActivationItemL();
            break;
#endif // RD_DRM_SILENT_RIGHTS_ACQUISITION
            
#ifdef RD_DRM_METERING
        case EDRMSettingsIdUsageReporting:
            MakeUsageReportingItemL();
            break;
#endif // RD_DRM_METERING     
#endif // __DRM_OMA2
       
        case EDRMSettingsIdWMDRMLicenseDeletion:
            
            MakeWMDRMLicenseDeletionItemL();
            
            break;
       
        default:
            break;
        }

    iListBox->HandleItemAdditionL();
    }


// ---------------------------------------------------------------------------
// CDRMSettingsPluginContainer::MakeTransactionTrackingItemL()
// 
// Create Transaction tracking list item 
// ---------------------------------------------------------------------------
//
void CDRMSettingsPluginContainer::MakeTransactionTrackingItemL()
    {
    HBufC* dynamicText( NULL );
    TInt trxTrState( iModel->TransactionTrackingStateL() );
    
    switch ( trxTrState )
        {
        case KDRMTransactionTrackingEnabled:
            dynamicText = StringLoader::LoadLC( R_DRM_SETTINGS_TTRACKING_ON );
            break;
        default:
            dynamicText = StringLoader::LoadLC( R_DRM_SETTINGS_TTRACKING_OFF );
            break;
        }
    
    TPtr ptrBuffer ( dynamicText->Des() );

    
    // Finally, set the dynamic text
    iListboxItemArray->SetDynamicTextL( EDRMSettingsIdTransactionTracking, ptrBuffer );

    CleanupStack::PopAndDestroy( dynamicText );

    // And add to listbox
    iListboxItemArray->SetItemVisibilityL( EDRMSettingsIdTransactionTracking, 
        CGSListBoxItemTextArray::EVisible );
    }


// ---------------------------------------------------------------------------
// CDRMSettingsPluginContainer::MakeAutomaticActivationItemL()
// 
// Create Automatic activation list item 
// ---------------------------------------------------------------------------
//
void CDRMSettingsPluginContainer::MakeAutomaticActivationItemL()
    {
    TInt automActivState( iModel->AutomaticActivationStateL() );
    
    HBufC* dynamicText( NULL );
    
    switch ( automActivState )
        {
        case KDRMTransactionTrackingEnabled:
            dynamicText = StringLoader::LoadLC( R_DRM_SETTINGS_AUTOM_ACTIV_ON );
            break;
        default:
            dynamicText = StringLoader::LoadLC( R_DRM_SETTINGS_AUTOM_ACTIV_OFF );
            break;
        }
    
    TPtr ptrBuffer ( dynamicText->Des() );

    // Finally, set the dynamic text
    iListboxItemArray->SetDynamicTextL( EDRMSettingsIdAutomaticActivation, 
                                        ptrBuffer );

    CleanupStack::PopAndDestroy( dynamicText );

    // And add to listbox
    iListboxItemArray->SetItemVisibilityL( EDRMSettingsIdAutomaticActivation, 
                                           CGSListBoxItemTextArray::EVisible );
    }

// ---------------------------------------------------------------------------
// CDRMSettingsPluginContainer::MakeUsageReportingItemL()
// 
// Create Usage reporting list item 
// ---------------------------------------------------------------------------
//
void CDRMSettingsPluginContainer::MakeUsageReportingItemL()
    {
    TInt count( iModel->UsageReportingCount() );
    
    HBufC* dynamicText( NULL );
    
    switch ( count )
        {
        case 0:
            dynamicText = StringLoader::LoadL( R_DRM_SET_USAGE_REPORT_NONE );
            break;
        case 1:
            dynamicText = iModel->GetFirstAllowedMeteringRIAliasL();
            break;
        default:
            dynamicText = StringLoader::LoadL( R_DRM_SET_SEVERAL_SERVICES );
            break;
        }
    CleanupStack::PushL( dynamicText );
    
    TPtr ptrBuffer ( dynamicText->Des() );

    // Finally, set the dynamic text
    iListboxItemArray->SetDynamicTextL( EDRMSettingsIdUsageReporting, 
                                        ptrBuffer );

    CleanupStack::PopAndDestroy( dynamicText );

    // And add to listbox
    iListboxItemArray->SetItemVisibilityL( EDRMSettingsIdUsageReporting, 
                                           CGSListBoxItemTextArray::EVisible );
    }

// ---------------------------------------------------------------------------
// CDRMSettingsPluginContainer::MakeWMDRMLicenseDeletionItemL()
// 
// Create WMDRM license deletion list item 
// ---------------------------------------------------------------------------
//
void CDRMSettingsPluginContainer::MakeWMDRMLicenseDeletionItemL()
    {
    if ( iWmdrmSupported )
        {
        // Add to listbox
        iListboxItemArray->
            SetItemVisibilityL( EDRMSettingsIdWMDRMLicenseDeletion, 
                                CGSListBoxItemTextArray::EVisible );
        }
    else
        {
        // Add to listbox
        iListboxItemArray->
            SetItemVisibilityL( EDRMSettingsIdWMDRMLicenseDeletion, 
                                CGSListBoxItemTextArray::EInvisible );
        
        }
    }

// ---------------------------------------------------------------------------
// CDRMSettingsPluginContainer::GetHelpContext() const
//  
// Gets Help 
// ---------------------------------------------------------------------------
//
void CDRMSettingsPluginContainer::GetHelpContext( 
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidRightsManager;
    aContext.iContext = KSET_HLP_PROTECTED_CONTENT;
    }


// ---------------------------------------------------------------------------
// CDRMSettingsPluginContainer::CurrentFeatureId()
//
// Return the feature id of selected listitem  
// ---------------------------------------------------------------------------
//
TInt CDRMSettingsPluginContainer::CurrentFeatureId( ) const
    {
    return iListboxItemArray->CurrentFeature();
    }


// -----------------------------------------------------------------------------
// CDRMSettingsPluginContainer::Model()
//
//
// -----------------------------------------------------------------------------
//
CDRMSettingsModel* CDRMSettingsPluginContainer::Model()
    {
    return iModel;
    }
    
// End of File
