/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares document for application.
*
*/



#ifndef DRMRIGHTSMGRDOCUMENT_H
#define DRMRIGHTSMGRDOCUMENT_H

// INCLUDES
#include <AknDoc.h>

#include <GenericParamConsumer.h>

#include "DRMRightsManager.hrh"


// FORWARD DECLARATIONS
class CEikAppUi;

// CLASS DECLARATION

/**
*  CDRMRightsMgrDocument application class.
*/
class CDRMRightsMgrDocument : public CAiwGenericParamConsumer
    {
    private:
        /**
        * Symbian default constructor.
        * @param aApp The application instance with which the new document is
        * associated.
        */
        CDRMRightsMgrDocument( CEikApplication& aApp );

        /**
        * Symbian default constructor.
        */
        void ConstructL();


    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aApp   The application instance with which the new document is
        *               associated.
        * @return       The document
        */
        static CDRMRightsMgrDocument* NewL( CEikApplication& aApp );

        /**
        * Destructor.
        */
        virtual ~CDRMRightsMgrDocument();


    public:     // Functions from base classes
        /**
        * This function is called when UI has to be opened embedded
        * @param aDoOpen    Normally ETrue
        * @param aParams    Not used
        * @param aFs        Not used
        * @return           Always NULL
        */
        CFileStore* OpenFileL( TBool aDoOpen, const TDesC& aParams, RFs& aFs );


    private:
        /**
        * From CEikDocument, create CDRMRightsMgrAppUi "App UI" object.
        * @return The AppUi object
        */
        CEikAppUi* CreateAppUiL();
    };

#endif

// End of File
