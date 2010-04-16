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
* Description:  Interface class for WMDRM DLA App Ui
*
*/


#ifndef C_WMDRMDLAAPPUI_H
#define C_WMDRMDLAAPPUI_H

// INCLUDES
#include <aknViewAppUi.h>
#include <AknServerApp.h>

class CWmDrmDlaBrowserView;

class CWmDrmDlaAppUi : public CAknViewAppUi
    {

    public:     // Constructors and destructor

        /**
        * Constructor.
        */
        CWmDrmDlaAppUi();

        /**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        virtual ~CWmDrmDlaAppUi();

        /**
         * Get browser view
         */
        CWmDrmDlaBrowserView* BrowserView();

    private: // From CEikAppUi

        /**
         * From CEikAppUi, takes care of command handling.
         * @param aCommand command to be handled
         */
        void HandleCommandL( TInt aCommand );

    private:    // data

        //Not owned
        CWmDrmDlaBrowserView* iBrowserView;

    };

#endif // C_WMDRMDLAAPPUI_H
