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
* Description:  Declares UI class for application.
*
*/


#ifndef DRMEncryptorAPPUI_H
#define DRMEncryptorAPPUI_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknappui.h>

// FORWARD DECLARATIONS
class CDRMEncryptorContainer;

// CONSTANTS

// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
*
*/
class CDRMEncryptorAppUi : public CAknAppUi
    {
    public: // Constructors and destructor

        /**
        * EPOC default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CDRMEncryptorAppUi();

    public: // New functions

    public: // Functions from base classes

        TBool ProcessCommandParametersL(
            TApaCommand aCommand,
            TFileName& aDocumentName,
            const TDesC8& aTail);


    private:
        /**
        * From CEikAppUi, takes care of command handling.
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );

    private: // Data
        CDRMEncryptorContainer* iAppContainer;
    };

#endif

// End of File
