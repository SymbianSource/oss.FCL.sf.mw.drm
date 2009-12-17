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
* Description:  Interface class for WMDRM DLA App
*
*/


#ifndef C_WMDRMDLAAPP_H
#define C_WMDRMDLAAPP_H

// INCLUDES
#include <aknapp.h>

class CWmDrmDlaApp : public CAknApplication
    {
    
    public:    // from CAknApplication (CApaApplication)

        /**
        * Create document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * Return application's UID.
        * @return Application UID.
        */
        TUid AppDllUid() const;
        
        /**
         * Sets new application server.
         * @param aAppServer The server pointer to be set.
         */
        void NewAppServerL( CApaAppServer*& aAppServer );
        
    };

#endif // C_WMDRMDLAAPP_H
