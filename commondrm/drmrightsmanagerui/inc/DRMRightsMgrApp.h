/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares main application class.
*
*/



#ifndef DRMRIGHTSMGRAPP_H
#define DRMRIGHTSMGRAPP_H

// INCLUDES
#include <aknapp.h>

#include "DRMRightsManager.hrh"


// CONSTANTS
// Uid of the application
const TUid KUidDRMRightsManager = { EUidDRMRightsManager };


// CLASS DECLARATION

/**
* CDRMRightsMgrApp application class.
* Provides factory to create concrete document object.
*/
class CDRMRightsMgrApp : public CAknApplication
    {
    private:
        /**
        * From CApaApplication, creates CDRMRightsMgrDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidDRMRightsManager).
        * @return The value of KUidDRMRightsManager.
        */
        TUid AppDllUid() const;
    };

#endif


// End of File

