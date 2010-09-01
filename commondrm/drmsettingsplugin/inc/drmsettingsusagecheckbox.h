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


#ifndef C_DRMSETTINGSUSAGECHECKBOX_H
#define C_DRMSETTINGSUSAGECHECKBOX_H

// INCLUDES
#include <akncheckboxsettingpage.h>
#include <aknsettingpage.h>

// FORWARD DECLARATIONS
class CDRMSettingUsageList;
class CDRMSettingsModel;
class CAknInfoPopupNoteController;
class CDRMSettingsPlugin;

/**
 * CDrmSettingUsageCheckBox class
 */
NONSHARABLE_CLASS( CDrmSettingUsageCheckBox ) : public CAknCheckBoxSettingPage
    {

    public: // New functions

        /**
		* C++ default constructor.
		*/
		CDrmSettingUsageCheckBox( TInt aResourceId,
		                          CDRMSettingUsageList* aList,
		                          CDRMSettingsModel* aModel,
		                          CDRMSettingsPlugin* aPlugin );
		                          
		/**
		* Destructor.
        */
        virtual ~CDrmSettingUsageCheckBox();	

	private: // From CAknSettingPage
	
	    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
	                                 TEventCode aType );
	
	    void DynamicInitL();
	
	    TBool OkToExitL(TBool aAccept);
	    
	    void AcceptSettingL();    
	
	private: // New functions
	
	    void ShowInfoPopupL();

    private: // Data
		
		// Not owned
		CDRMSettingUsageList* iList;
		
		// Not owned
        CDRMSettingsModel* iModel;
        
        // Owned
        CAknInfoPopupNoteController* iPopupController;
        
        // Pointer to CDrmSettingsPlugin instance
        CDRMSettingsPlugin* iDrmSettingsPlugin;

    };

#endif // C_DRMSETTINGSUSAGECHECKBOX_H
