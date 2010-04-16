/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface class for WMDRM DLA App Document
*
*/


#ifndef C_WMDRMDLAAPPDOCUMENT_H
#define C_WMDRMDLAAPPDOCUMENT_H

// INCLUDES
#include <AknDoc.h>

class CWmDrmDlaAppDocument : public CAknDocument
    {

    public: // Constructors and destructor

        /**
        * Two-phased constructor. Leaves on failure.
        * @param aApp Application.
        * @return The created document.
        */
        static CWmDrmDlaAppDocument* NewL( CEikApplication& aApp );

        /**
        * Destructor.
        */
        virtual ~CWmDrmDlaAppDocument();

    public: // from CEikDocument

        /**
        * Create Application Ui.
        * @return Pointer to App Ui.
        */
        CEikAppUi* CreateAppUiL();

    public: // Constructors

        /**
        * Constructor.
        * @param aApp Application.
        */
        CWmDrmDlaAppDocument( CEikApplication& aApp );

        /**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL();

    private:    // data

    };

#endif // C_WMDRMDLAAPPDOCUMENT_H
