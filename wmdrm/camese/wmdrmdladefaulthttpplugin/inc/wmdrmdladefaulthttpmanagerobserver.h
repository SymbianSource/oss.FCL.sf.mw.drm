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
* Description:  Defines the HTTP observer interface
*
*/


#ifndef WMDRMDLADEFAULTHTTPMANAGEROBSERVER_H
#define WMDRMDLADEFAULTHTTPMANAGEROBSERVER_H

// INCLUDES
#include <e32std.h>
#include <http/mhttpauthenticationcallback.h>
#include <http/mhttptransactioncallback.h>

/**
* MWmDrmDlaDefaltHttpManagerObserver
* Notification interface used to report HTTP progress.
*/
class MWmDrmDlaDefaltHttpManagerObserver
    {
    public:
        /**
         * Called if authentication is required.
         * @param aUsername - The username to be used for WWW authentication
         *                    client. The pointer should be NULL and the 
         *                    function will allocate new memory to hold the
         *                    username and return it
         * @param aPassword - The password to be used for WWW authentication
         *                    client. the pointer should be NULL and the
         *                    function will allocate new memory to hold the 
         *                    username and return it
         * @return true if the client has provided username and password, 
         *         false otherwise
         */
        virtual TBool OnGetUsernamePasswordL( HBufC8*& aUsername, 
                                              HBufC8*& aPassword ) = 0;

        /**
         * Called when server returns response headers.
         * @param aResponse - HTTP transaction response
         * @param aHeaders - response headers
         * @param aStringPool - reference to the string pool belonging
         *                      to this transaction
         * @param aHttpStatus - status code of the resonse header
         */
        virtual void OnResponseHeadersL( const RHTTPResponse& aResponse, 
                                         const RHTTPHeaders& aHeaders,
                                         const RStringPool& aStringPool, 
                                         TInt aHttpStatus ) = 0;

        /**
         * Called for each chunk of data returned from server.
         * @param aDataChunk - data chunk
         */
        virtual void OnResponseBodyDataL( const TPtrC8 aDataChunk ) = 0;

        /**
         * Called upon transaction has been completed.
         * @param aError - An error returned from the HTTP library
         */
        virtual void OnTransactionComplete( TInt aError ) = 0;
    };

#endif  // WMDRMDLADEFAULTHTTPMANAGEROBSERVER_H

// End of File
