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
* Description:  Interface class for WMDRM DLA Browser Observer
*
*/


#ifndef M_WMDRMDLABROWSEROBSERVER_H
#define M_WMDRMDLABROWSEROBSERVER_H

#include <e32base.h>

/**
 * Interface for browser observer
 */
class MWmDrmDlaBrowserObserver
    {
    public:
        
        /**
         * Called when POST URL is required.
         * @return POST URL
         */
        virtual const TDesC& PostUrlL() = 0;
        
        /**
         * Called when POST content type is required.
         * @return POST content type
         */
        virtual const TDesC8& PostContentTypeL() = 0;
        
        /**
         * Called when POST data is required.
         * @return POST data
         */
        virtual const TDesC8& PostDataL() = 0;
        
        /**
         * Called when POST content data boundary is required.
         * @return POST content data boundary
         */
        virtual const TDesC8& PostContentDataBoundaryL() = 0;
        
        /**
         * Called when IAP ID is required.
         * @return IAP ID
         */
        virtual TInt IapId() = 0;
        
        /**
         * Called when license is received.
         * @param aLicenseResponse - Received license response
         */
        virtual void LicenseResponseReceived( 
                                        const TDesC8& aLicenseResponse ) = 0;
        
        /**
         * Called when browser view is closed with some error.
         * @param aError - Error or command that closed the browser view
         */
        virtual void BrowserExit( TInt aError ) = 0;

    };


#endif // M_WMDRMDLABROWSEROBSERVER_H
