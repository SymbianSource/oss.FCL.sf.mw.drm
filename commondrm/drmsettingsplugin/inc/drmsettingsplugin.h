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
* Description:  View for DRMSettingsPlugin.
*
*/


#ifndef DRMSETTINGSPLUGIN_H
#define DRMSETTINGSPLUGIN_H

// INCLUDES
#include <aknsettingpage.h>
#include <ConeResLoader.h>
#include <gsplugininterface.h>
#include <gsfwviewuids.h>
#include <gsbaseview.h>
#include <eikmenup.h>

#include "drmsettingsplugincontainer.h"

//CONSTANTS
const TUid KDRMSettingsPluginUid = { 0x1020750CC };

// Listbox item indexes of the transcaction tracking setting items
const TInt KDRMTTItemIndexDisabled  = 1;
const TInt KDRMTTItemIndexEnabled   = 0;

// Listbox item indexes of the automatic activation setting items
const TInt KDRMAAItemIndexDisabled  = 1;
const TInt KDRMAAItemIndexEnabled   = 0;

// FORWARD DECLARATIONS
class CAknViewAppUi;
class CDRMSettingsPluginContainer;
class CDRMSettingsModel;

// CLASS DECLARATION

/**
*  CDRMSettingsPlugin view class
*
*  View class for DRM Settings plugin
*/
class CDRMSettingsPlugin : public CGSBaseView
    {

    public: // Constructors and destructor

        /**
        * Symbian OS two-phased constructor
        * @return connection view.
        */
        static CDRMSettingsPlugin* NewL( TAny* aInitParams );

        /**
        * Destructor.
        */
        ~CDRMSettingsPlugin();

    private:
        /**
        * Symbian OS default constructor.
        *
        */
        void ConstructL();

        /**
        * C++ default constructor.
        */
        CDRMSettingsPlugin();

    public: // from CAknView

        /**
        * Returns view id.
        * @return An unsigned integer (view id).
        */
        TUid Id() const;

    public: // from MEikCommandObserver

        /**
        * Handles commands.
        * @param aCommand Command to be handled.
        *
        */
        void HandleCommandL( TInt aCommand );

    public: //new

        /**
        * Updates listbox's item's value.
        * @param aItemId An item which is updated.
        *
        */
        void UpdateListBoxL( TInt aItemId );

    public: // From CGSPluginInterface

        /**
        * @see CGSPluginInterface header file.
        */
        void GetCaptionL( TDes& aCaption ) const;

        /**
        * @see CGSPluginInterface header file.
        */
        TInt PluginProviderCategory() const;

        /**
        * @see CGSPluginInterface header file.
        */
        TBool Visible() const;

    private: // from CGSBaseView
        //
        void NewContainerL();
        //
        void HandleListBoxSelectionL();

    private: // new

        /**
        * Update transaction tracking setting
        */
        void UpdateTransactionTrackingSettingL( TBool aShowSettingPage );

        /**
        * Update automatic activation setting
        */
        void UpdateAutomaticActivationSettingL( TBool aShowSettingPage );

        /**
        * Update usage reporting setting
        */
        void UpdateUsageReportingSettingL();

        /**
        * Delete WMDRM license store
        */
        void DoWMDRMLicenseDeletionL();

        /**
        * Display setting page
        * @param aTTState Current state. This will be updated.
        * @return ETrue if value is updated.
        *         EFalse if value is not updated.
        */
        TBool ShowTransactionTrackingSettingPageL( TInt& aTTState );

        /**
        * Display setting page
        * @param aAAState Current state. This will be updated.
        * @return ETrue if value is updated.
        *         EFalse if value is not updated.
        */
        TBool ShowAutomaticActivationSettingPageL( TInt& aAAState );

        /**
        * Get DRMSettingsPlugin's ccontainer.
        */
        CDRMSettingsPluginContainer* Container();

    protected: // From MEikMenuObserver

        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    private: // data
        //resource loader
        RConeResourceLoader iResourceLoader;

        TBool iWmdrmSupported;
    };

#endif //DRMSETTINGSPLUGIN_H

// End of File

