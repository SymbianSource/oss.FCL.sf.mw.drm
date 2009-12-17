/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares main application class
*
*/


#ifndef DRMEncryptorAPP_H
#define DRMEncryptorAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application
const TUid KUidDRMEncryptor = { 0x01105901 };

// CLASS DECLARATION

/**
* CDRMEncryptorApp application class.
* Provides factory to create concrete document object.
*
*/
class CDRMEncryptorApp : public CAknApplication
    {

    public: // Functions from base classes
    private:

        /**
        * From CApaApplication, creates CDRMEncryptorDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();

        /**
        * From CApaApplication, returns application's UID (KUidDRMEncryptor).
        * @return The value of KUidDRMEncryptor.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File

