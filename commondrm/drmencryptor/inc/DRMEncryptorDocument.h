/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#ifndef DRMEncryptorDOCUMENT_H
#define DRMEncryptorDOCUMENT_H

// INCLUDES
#include <AknDoc.h>

// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
*  CDRMEncryptorDocument application class.
*/
class CDRMEncryptorDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CDRMEncryptorDocument* NewL( CEikApplication& aApp );

        /**
        * Destructor.
        */
        virtual ~CDRMEncryptorDocument();

        virtual CFileStore* OpenFileL(TBool aDoOpen,const TDesC& aFilename,RFs& aFs);

    private:

        /**
        * Constructor.
        */
        CDRMEncryptorDocument( CEikApplication& aApp );

        /**
        * From CEikDocument, create CDRMEncryptorAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();
    };

#endif

// End of File

