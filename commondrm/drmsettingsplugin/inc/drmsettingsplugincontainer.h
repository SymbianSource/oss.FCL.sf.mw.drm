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
* Description:  Container for DRMSettinsPlugin view.
*
*/


#ifndef DRMSETTINGSPLUGINCONTAINER_H
#define DRMSETTINGSPLUGINCONTAINER_H

// INCLUDES
#include <gsbasecontainer.h>

// FORWARD DECLARATIONS
class CGSListBoxItemTextArray;
class CDRMSettingsModel;

// CLASS DECLARATION

/**
*  CDRMSettingsPluginContainer container class
*  @since Series 60_3.1
*
*  Container class for DRM Settings view
*/
class CDRMSettingsPluginContainer : public CGSBaseContainer
    {
        
    public: // Constructors and destructor

        /**
        * Symbian OS constructor.
        * @param aRect Listbox's rect.
        */
        void ConstructL( const TRect& aRect );

        CDRMSettingsPluginContainer( TBool aWmdrmSupported,
                                     TBool aOmadrmSupported );
        
        /**
        * Destructor.
        */
        ~CDRMSettingsPluginContainer();

    public: // new

        /**
        * Updates listbox's item's value.
        * @param aFeatureId An item which is updated.
        */
        void UpdateListBoxL( TInt aFeatureId );

        /**
        * Retrieves the currently selected listbox feature id
        * @return feature id.
        */
        TInt CurrentFeatureId() const;
        
        /**
        * @return Model for the plugin.
        */
        CDRMSettingsModel* Model();
        
    protected: // from CGSBaseContainer
    
        /**
        * See base class.
        */
        void ConstructListBoxL( TInt aResLbxId );
        
    private: // new

        void CreateListBoxItemsL();
        void MakeTransactionTrackingItemL();
        void MakeAutomaticActivationItemL();
        void MakeUsageReportingItemL();
        void MakeWMDRMLicenseDeletionItemL();

    private:
    
        /**
        * Required for help.
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

    private: // data

        // GS listbox item array
        CGSListBoxItemTextArray* iListboxItemArray;
        
        // Model for DRMSettingsPlugin.
        CDRMSettingsModel* iModel;
        
        TBool iWmdrmSupported;
        
        // Oma drm 2 is configured to be supported.
        TBool iOmadrm2Supported;
        
    };

#endif //DRMSETTINGSPLUGINCONTAINER_H

// End of File
