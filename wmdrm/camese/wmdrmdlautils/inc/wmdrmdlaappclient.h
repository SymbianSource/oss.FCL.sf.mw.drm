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
* Description:  Interface class for WMDRM DLA App Client
*
*/


#ifndef R_WMDRMDLAAPPCLIENT_H
#define R_WMDRMDLAAPPCLIENT_H

#include <e32base.h>
#include <AknServerApp.h>

/**
 *  WMDRM DLA App Client Interface
 *
 *  @lib wmdrmdlautils.lib
 *  @since S60 v9.1
 */
NONSHARABLE_CLASS( RWmDrmDlaAppClient ) : public RAknAppServiceBase
    {

    public:

        virtual ~RWmDrmDlaAppClient();

        /**
         * Sets IAP to browser view
         *
         * @param aIap IAP ID to be used in browser view
         * @return Error code
         */
        TInt SetIAP( TInt aIap );

        /**
         * Makes a POST request to browser view
         *
         * @param aPostUrl URL, where POST request is done
         * @param aPostContentType Content type of the POST operation
         * @param aPostData POST data
         * @param aPostContentBoundary Boundary of the POST content
         * @return Error code
         */
        TInt Post( const TDesC& aPostUrl,
                   const TDesC8& aPostContentType,
                   const TDesC8& aPostData,
                   const TDesC8& aPostContentBoundary );

        /**
         * Gets the license response from the browser view
         *
         * @param aLicenseResponse Pointer, where license response is saved.
         *                         Must be NULL.
         * @return Error code
         */
        TInt LicenseResponse( HBufC8*& aLicenseResponse );

    private: // from RApaAppServiceBase

        TUid ServiceUid() const;

    private: // data

    };

#endif // R_WMDRMDLAAPPCLIENT_H
