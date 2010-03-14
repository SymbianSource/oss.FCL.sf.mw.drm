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
* Description:  View for DRMSettinsPlugin
*
*/


// INCLUDE FILES
#include <coeaui.h>
#include <hlplch.h>             // For HlpLauncher
#include <bautils.h>
#include <eikfrlbd.h>
#include <featmgr.h>
#include <stringloader.h>
#include <aknviewappui.h>
#include <aknradiobuttonsettingpage.h>
#include <gsfwviewuids.h>
#include <gsprivatepluginproviderids.h>
#include <gscommon.hrh>
#include <drmsettingspluginrsc.rsg>

#include "drmsettingsplugin.h"
#include "drmsettingsplugincontainer.h"
#include "drmsettingsplugin.hrh"
#include "drmsettingsmodel.h"
#include "drmsettingsusagecheckbox.h"
#include "drmsettingsusagelist.h"
#include "wmdrmpkclientwrapper.h"

// CONSTANTS
_LIT( KDRMSettingsPluginResourceFileName, "z:drmsettingspluginrsc.rsc" );


// ============================= LOCAL FUNCTIONS ==============================

// ========================= MEMBER FUNCTIONS ================================

// ----------------------------------------------------------------------------
// CDRMSettingsPlugin::CDRMSettingsPlugin()
//
// Constructor
// ----------------------------------------------------------------------------
//
CDRMSettingsPlugin::CDRMSettingsPlugin()
  : iResourceLoader( *iCoeEnv )
    {
    }


// ---------------------------------------------------------------------------
// CDRMSettingsPlugin::NewL()
//
// Symbian OS default constructor
// ---------------------------------------------------------------------------
CDRMSettingsPlugin* CDRMSettingsPlugin::NewL( TAny* /*aInitParams*/ )
    {
    CDRMSettingsPlugin* self = new( ELeave ) CDRMSettingsPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }



// ---------------------------------------------------------------------------
// CDRMSettingsPlugin::ConstructL()
//
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
void CDRMSettingsPlugin::ConstructL()
    {
    FeatureManager::InitializeLibL();
    // Find the resource file
    TParse parse;
    parse.Set( KDRMSettingsPluginResourceFileName,
               &KDC_RESOURCE_FILES_DIR,
               NULL );
    TFileName fileName( parse.FullName() );

    // Get language of resource file
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );

    // Open resource file
    iResourceLoader.OpenL( fileName );

    BaseConstructL( R_DRM_SETTINGS_VIEW );

    if ( FeatureManager::FeatureSupported( KFeatureIdWindowsMediaDrm ) )
        {
        iWmdrmSupported = ETrue;
        }
    }


// ----------------------------------------------------------------------------
// CDRMSettingsPlugin::~CDRMSettingsPlugin
//
// Destructor
// ----------------------------------------------------------------------------
CDRMSettingsPlugin::~CDRMSettingsPlugin()
    {
    FeatureManager::UnInitializeLib();
    iResourceLoader.Close();
    }


// ---------------------------------------------------------------------------
// TUid CDRMSettingsPlugin::Id()
//
// Returns view's ID.
// ---------------------------------------------------------------------------
TUid CDRMSettingsPlugin::Id() const
    {
    return KDRMSettingsPluginUid;
    }


// ========================= From CGSPluginInterface ==================

// ----------------------------------------------------------------------------
// CDRMSettingsPlugin::GetCaption
//
// Return application/view caption.
// ----------------------------------------------------------------------------
//
void CDRMSettingsPlugin::GetCaptionL( TDes& aCaption ) const
    {
    // the resource file is already opened.
    HBufC* result( StringLoader::LoadL( R_DRM_SETTINGS_VIEW_CAPTION ) );
    aCaption.Copy( *result );
    delete result;
    }


// ----------------------------------------------------------------------------
// CDRMSettingsPlugin::PluginProviderCategory
//
// A means to identify the location of this plug-in in the framework.
// ----------------------------------------------------------------------------
//
TInt CDRMSettingsPlugin::PluginProviderCategory() const
    {
    //To identify internal plug-ins.
    return KGSPluginProviderInternal;
    }


// ----------------------------------------------------------------------------
// CDRMSettingsPlugin::Visible
//
// Provides the visibility status of self to framework.
// ----------------------------------------------------------------------------
//
TBool CDRMSettingsPlugin::Visible() const
    {
    TBool visible( EFalse );

    // The plugin is visible if __DRM_OMA2 or __WINDOWS_MEDIA_DRM are enabled.
    if( FeatureManager::FeatureSupported( KFeatureIdDrmOma2 ) ||
        FeatureManager::FeatureSupported( KFeatureIdWindowsMediaDrm ) )
        {
        visible = ETrue;
        }

    return visible;
    }


// ---------------------------------------------------------------------------
// CDRMSettingsPlugin::HandleCommandL(TInt aCommand)
//
// Handles commands directed to this class.
// ---------------------------------------------------------------------------
void CDRMSettingsPlugin::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EDRMSettingsCmdAppChangeMSK:
            {
            const TInt currentFeatureId( Container()->CurrentFeatureId() );

            switch ( currentFeatureId )
                {
#ifdef __DRM_OMA2
                case EDRMSettingsIdTransactionTracking:

                    UpdateTransactionTrackingSettingL( EFalse );

                    break;

#ifdef RD_DRM_SILENT_RIGHTS_ACQUISITION
                case EDRMSettingsIdAutomaticActivation:

                    UpdateAutomaticActivationSettingL( EFalse );

                    break;
#endif // RD_DRM_SILENT_RIGHTS_ACQUISITION

#ifdef RD_DRM_METERING
                case EDRMSettingsIdUsageReporting:

                    UpdateUsageReportingSettingL();

                    break;
#endif // RD_DRM_METERING
#endif // __DRM_OMA2

                case EDRMSettingsIdWMDRMLicenseDeletion:

                    if ( iWmdrmSupported )
                        {
                        DoWMDRMLicenseDeletionL();
                        }

                    break;

                default:

                    break;
                }

            break;
            }
        case EDRMSettingsCmdAppChange:
            {
            const TInt currentFeatureId( Container()->CurrentFeatureId() );

            switch ( currentFeatureId )
                {
#ifdef __DRM_OMA2
                case EDRMSettingsIdTransactionTracking:

                    UpdateTransactionTrackingSettingL( ETrue );

                    break;

#ifdef RD_DRM_SILENT_RIGHTS_ACQUISITION
                case EDRMSettingsIdAutomaticActivation:

                    UpdateAutomaticActivationSettingL( ETrue );

                    break;
#endif // RD_DRM_SILENT_RIGHTS_ACQUISITION

#ifdef RD_DRM_METERING
                case EDRMSettingsIdUsageReporting:

                    UpdateUsageReportingSettingL();

                    break;
#endif // RD_DRM_METERING
#endif // __DRM_OMA2

                case EDRMSettingsIdWMDRMLicenseDeletion:

                    if ( iWmdrmSupported )
                        {
                        DoWMDRMLicenseDeletionL();
                        }

                    break;

                default:

                    break;
                }
            break;
            }
        case EAknSoftkeyBack:

            iAppUi->ActivateLocalViewL( KGSSecurityPluginUid );

            break;

        case EAknCmdHelp:
            {

            if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                HlpLauncher::LaunchHelpApplicationL(
                                                iEikonEnv->WsSession(),
                                                iAppUi->AppHelpContextL() );
                }

            break;

            }
        default:

            iAppUi->HandleCommandL( aCommand );

            break;
        }
    }


// ---------------------------------------------------------------------------
// CDRMSettingsPlugin::UpdateListBoxL
//
// Updates listbox items.
// ---------------------------------------------------------------------------
//
void CDRMSettingsPlugin::UpdateListBoxL( TInt aItemId )
    {
    Container()->UpdateListBoxL( aItemId );
    }


// ----------------------------------------------------------------------------
// CDRMSettingsPlugin::Container
//
// Return handle to container class.
// ----------------------------------------------------------------------------
//
CDRMSettingsPluginContainer* CDRMSettingsPlugin::Container()
    {
    return static_cast<CDRMSettingsPluginContainer*>( iContainer );
    }


// ---------------------------------------------------------------------------
// CDRMSettingsPlugin::NewContainerL()
//
// Creates new iContainer.
// ---------------------------------------------------------------------------
//
void CDRMSettingsPlugin::NewContainerL()
    {
    iContainer = new( ELeave ) CDRMSettingsPluginContainer( iWmdrmSupported );
    }


// ---------------------------------------------------------------------------
// CDRMSettingsPlugin::HandleListBoxSelectionL()
//
// Handles events raised through a rocker key.
// ---------------------------------------------------------------------------
void CDRMSettingsPlugin::HandleListBoxSelectionL()
    {
    const TInt currentFeatureId( Container()->CurrentFeatureId() );

    switch ( currentFeatureId )
        {
#ifdef __DRM_OMA2
        case EDRMSettingsIdTransactionTracking:

            UpdateTransactionTrackingSettingL( EFalse );

            break;

#ifdef RD_DRM_SILENT_RIGHTS_ACQUISITION
        case EDRMSettingsIdAutomaticActivation:

            UpdateAutomaticActivationSettingL( EFalse );

            break;
#endif // RD_DRM_SILENT_RIGHTS_ACQUISITION

#ifdef RD_DRM_METERING
        case EDRMSettingsIdUsageReporting:

            UpdateUsageReportingSettingL();

            break;
#endif // RD_DRM_METERING
#endif // __DRM_OMA2

        case EDRMSettingsIdWMDRMLicenseDeletion:

            if ( iWmdrmSupported )
                {
                DoWMDRMLicenseDeletionL();
                }

            break;

       default:

            break;
        }
    }


// ---------------------------------------------------------------------------
// CDRMSettingsPlugin::UpdateTransactionTrackingSettingL
//
// Display Transaction tracking setting page.
// ---------------------------------------------------------------------------
//
void CDRMSettingsPlugin::UpdateTransactionTrackingSettingL( TBool aShowSettingPage )
    {
    if( FeatureManager::FeatureSupported( KFeatureIdDrmOma2 ) )
        {
        TInt currentValue( Container()->Model()->TransactionTrackingStateL() );
        TBool isValueUpdated( EFalse );

        if ( aShowSettingPage )
            {
            isValueUpdated = ShowTransactionTrackingSettingPageL( currentValue );
            }
        else
            {
            switch ( currentValue )
                {
                case KDRMTransactionTrackingDisabled:

                    currentValue = KDRMTransactionTrackingEnabled;

                    break;

                case KDRMTransactionTrackingEnabled:

                    currentValue = KDRMTransactionTrackingDisabled;

                    break;

                default:

                    break;
                }
            isValueUpdated = ETrue;
            }
        // If value is updated, store it to model:
        if ( isValueUpdated )
            {
            Container()->Model()->SetTransactionTrackingStateL( currentValue );
            UpdateListBoxL( EDRMSettingsIdTransactionTracking );
            }
        }
    }


// ---------------------------------------------------------------------------
// CDRMSettingsPlugin::ShowTransactionTrackingSettingPageL()
//
// Display transaction tracking setting page. Selected listbox item index
// must be mapped to transaction tracking state because index value is
// different from state value.
// ---------------------------------------------------------------------------
//
TBool CDRMSettingsPlugin::ShowTransactionTrackingSettingPageL(
    TInt& aTTState )
    {
    // in case DRM Phase 2 is not supported, return EFalse.
    TBool isValueUpdated( EFalse );
    TInt selectedTTItemIndex( 0 );
    TInt originalTTState( aTTState );

    // Set selected listbox item to current transaction tracking state:
    switch ( aTTState )
        {
        case KDRMTransactionTrackingDisabled:

            selectedTTItemIndex = KDRMTTItemIndexDisabled;

            break;

        case KDRMTransactionTrackingEnabled:

            selectedTTItemIndex = KDRMTTItemIndexEnabled;

            break;
        }

    CDesCArrayFlat* items(
        iCoeEnv->ReadDesC16ArrayResourceL( R_TTRACKING_SETTING_PAGE_LBX ) );
    CleanupStack::PushL( items );
    CAknRadioButtonSettingPage* dlg(
        new (ELeave) CAknRadioButtonSettingPage( R_TTRACKING_SETTING_PAGE,
                                                 selectedTTItemIndex,
                                                 items ) );

    dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged );
    CleanupStack::PopAndDestroy( items );

    // Map selected listbox item to correct state:
    switch ( selectedTTItemIndex )
        {
        case KDRMTTItemIndexDisabled:

            aTTState = KDRMTransactionTrackingDisabled;

            break;

        case KDRMTTItemIndexEnabled:

            aTTState = KDRMTransactionTrackingEnabled;

            break;
        }

    // Check is value updated:
    if( aTTState != originalTTState )
        {
        isValueUpdated = ETrue;
        }

    return isValueUpdated;
    }


// ---------------------------------------------------------------------------
// CDRMSettingsPlugin::UpdateAutomaticActivationSettingL
//
// Display Automatic activation setting page.
// ---------------------------------------------------------------------------
//
void CDRMSettingsPlugin::UpdateAutomaticActivationSettingL( TBool aShowSettingPage )
    {
    if( FeatureManager::FeatureSupported( KFeatureIdDrmOma2 ) )
        {
        TInt currentValue( Container()->Model()->AutomaticActivationStateL() );
        TBool isValueUpdated( EFalse );

        if ( aShowSettingPage )
            {
            isValueUpdated = ShowAutomaticActivationSettingPageL( currentValue );
            }
        else
            {
            switch ( currentValue )
                {
                case KDRMAutomaticActivationNotAllowed:

                    currentValue = KDRMAutomaticActivationAllowed;

                    break;

                case KDRMAutomaticActivationAllowed:

                    currentValue = KDRMAutomaticActivationNotAllowed;

                    break;

                default:

                    break;
                }
            isValueUpdated = ETrue;
            }
        // If value is updated, store it to model:
        if ( isValueUpdated )
            {
            Container()->Model()->SetAutomaticActivationStateL( currentValue );
            UpdateListBoxL( EDRMSettingsIdAutomaticActivation );
            }
        }
    }


// ---------------------------------------------------------------------------
// CDRMSettingsPlugin::ShowAutomaticActivationSettingPageL()
//
// Display Automatic activation setting page. Selected listbox item index
// must be mapped to automatic activation state because index value is
// different from state value.
// ---------------------------------------------------------------------------
//
TBool CDRMSettingsPlugin::ShowAutomaticActivationSettingPageL(
    TInt& aAAState )
    {
    // in case DRM Phase 2 is not supported, return EFalse.
    TBool isValueUpdated( EFalse );
    TInt selectedAAItemIndex( 0 );
    TInt originalAAState( aAAState );

    // Set selected listbox item to current state:
    switch ( aAAState )
        {
        case KDRMAutomaticActivationNotAllowed:

            selectedAAItemIndex = KDRMAAItemIndexDisabled;

            break;

        case KDRMAutomaticActivationAllowed:

            selectedAAItemIndex = KDRMAAItemIndexEnabled;

            break;
        }

    CDesCArrayFlat* items(
        iCoeEnv->ReadDesC16ArrayResourceL( R_AUTOM_ACTIV_SETTING_PAGE_LBX ) );
    CleanupStack::PushL( items );
    CAknRadioButtonSettingPage* dlg(
        new (ELeave) CAknRadioButtonSettingPage( R_AUTOM_ACTIV_SETTING_PAGE,
                                                 selectedAAItemIndex,
                                                 items ) );

    dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged );
    CleanupStack::PopAndDestroy( items );

    // Map selected listbox item to correct state:
    switch ( selectedAAItemIndex )
        {
        case KDRMAAItemIndexDisabled:

            aAAState = KDRMAutomaticActivationNotAllowed;

            break;

        case KDRMAAItemIndexEnabled:

            aAAState = KDRMAutomaticActivationAllowed;

            break;

        }

    // Check is value updated:
    if( aAAState != originalAAState )
        {
        isValueUpdated = ETrue;
        }

    return isValueUpdated;
    }


// ---------------------------------------------------------------------------
// CDRMSettingsPlugin::UpdateUsageReportingSettingL
//
// Display Usage Reporting setting page.
// ---------------------------------------------------------------------------
//
void CDRMSettingsPlugin::UpdateUsageReportingSettingL()
    {
    TBool isValueUpdated( EFalse );

    if( FeatureManager::FeatureSupported( KFeatureIdDrmOma2 ) )
        {
        CDRMSettingsModel* model( this->Container()->Model() );

        CDRMSettingUsageList* usageList( CDRMSettingUsageList::NewL( model ) );
        CleanupStack::PushL( usageList );

        CDrmSettingUsageCheckBox* usageCheckBox(
            new (ELeave) CDrmSettingUsageCheckBox(
                R_DRM_SETTINGS_METERING_CHECKBOX_PAGE,
                usageList,
                model,
                this ) );
                                            
        isValueUpdated = 
            usageCheckBox->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted );

        CleanupStack::PopAndDestroy( usageList );
        }

    if ( isValueUpdated )
        {
        UpdateListBoxL( EDRMSettingsIdUsageReporting );
        }
    }

// ---------------------------------------------------------------------------
// CDRMSettingsPlugin::DoWMDRMLicenseDeletionL
//
// Display WMDRM license deletion setting page.
// ---------------------------------------------------------------------------
//
void CDRMSettingsPlugin::DoWMDRMLicenseDeletionL()
    {
    if ( iWmdrmSupported )
        {
        TInt r = KErrNone;
        RLibrary library;
        r = library.Load( KWmdrmPkClientWrapperName );
        if( !r )
            {
            CleanupClosePushL( library );
            CWmDrmPkClientWrapper* wrapper =
                (CWmDrmPkClientWrapper*)library.Lookup( KWmdrmPkClientNewL )();
            CleanupStack::PushL( wrapper );
            User::LeaveIfError( wrapper->Connect() );
            wrapper->DeleteRights();
            wrapper->Close();
            CleanupStack::PopAndDestroy( 2, &library );
            }
        }
    }

// ----------------------------------------------------------------------------
// CDRMSettingsPlugin::DynInitMenuPaneL()
//
// Display the dynamic menu
// ----------------------------------------------------------------------------
void CDRMSettingsPlugin::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    // show or hide the 'help' menu item when supported
    if( aResourceId == R_DRM_SETTINGS_MENU_ITEM_EXIT )
        {
        User::LeaveIfNull( aMenuPane );

#ifdef __DRM_OMA2
        if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
            {
            aMenuPane->SetItemDimmed( EAknCmdHelp, EFalse );
            }
        else
            {
            aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
            }
#else

        aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );

#endif // __DRM_OMA2
        }
    }



// End of File
