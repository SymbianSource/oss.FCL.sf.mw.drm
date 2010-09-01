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
* Description:  List class for Usage Reporting checkbox
*
*/


#ifndef C_DRMSETTINGSUSAGELIST_H
#define C_DRMSETTINGSUSAGELIST_H

// INCLUDES
#include <akncheckboxsettingpage.h>

// FORWARD DECLARATIONS
class CDRMSettingsModel;

/**
 * CDRMSettingUsageList class
 */
NONSHARABLE_CLASS( CDRMSettingUsageList ) : public CSelectionItemList
	{

    public: // New functions
	
	    /**
		* Two-phased constructor.
        */
		static CDRMSettingUsageList* NewL( CDRMSettingsModel* aModel );

        /**
		* Destructor.
        */
        virtual ~CDRMSettingUsageList();
        
	    /**
	    * Updates contexts
	    */
	    void UpdateContexts();
	
	private:
	
	    /**
	    * Default constructor.
	    */
	    CDRMSettingUsageList( CDRMSettingsModel* aModel );
	    
	    /**
        * Symbian OS constructor.
        */
	    void ConstructL();
	    
	private: // Data
	
	    // Not owned
	    CDRMSettingsModel* iModel;
	
	};
	
#endif // C_DRMSETTINGSUSAGELIST_H