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
* Description:  Interface class for WMDRM DLA App Service Session
*
*/


#ifndef C_WMDRMDLAAPPSERVICESESSION_H
#define C_WMDRMDLAAPPSERVICESESSION_H

// INCLUDES
#include <AknServerApp.h>
#include "wmdrmdlaappui.h"
#include "wmdrmdlabrowserview.h"

/**
* WMDRM DLA Application.
*/
class CWmDrmDlaAppServiceSession : public CAknAppServiceBase,
                                   public MBrowserViewLicenseReceivedCallback
    {

    public:

        CWmDrmDlaAppServiceSession();
        virtual ~CWmDrmDlaAppServiceSession();

    public: // from MBrowserViewLicenseReceivedCallback

        /**
         * Called when license is received
         */
        void LicenseReceived();

    protected: // from CSession2

        /**
         * Override of CSession2::ServiceL().
         * If further overridden, this function must be base-called.
         * @param aMessage The client message
         */
        void ServiceL( const RMessage2& aMessage );

    private:

        void SetIapL( const RMessage2& aMessage );
        void PostL( const RMessage2& aMessage );
        void LicenseResponseSizeL( const RMessage2& aMessage );
        void LicenseResponseL( const RMessage2& aMessage );
        void CompleteMessage( const RMessage2& aMessage, TInt aError );
        CWmDrmDlaBrowserView* BrowserView();

    private: //data

        CActiveSchedulerWait* iWait;

    };

#endif // C_WMDRMDLAAPPSERVICESESSION_H
