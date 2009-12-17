/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines the entry point for the CameseUtility library.
*
*/


#ifndef CAMESEUTILITY_H
#define CAMESEUTILITY_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CCameseEngine;
class RConnection;
class RFile;
class MCameseObserver;

/**
* CCameseUtility
*  Interface published by the Camese Utility library.
*
*  @lib CameseUtility.dll
*  @since 5.0
*/
NONSHARABLE_CLASS( CCameseUtility ) : public CBase
    {
    public:
        /**
         * Creates an new instance for license acquisition.
         * @param aObserver Observer interface to be notified on license 
         * acquisition states. If NULL then no notifications will be supplied 
         * (when handling the download result for example)
         * @return address of an instance of this class
         */
        IMPORT_C static CCameseUtility* NewL( MCameseObserver* aObserver = NULL );


        /**
         * Creates an new instance for license acquisition.
         * @param aIapNumber a IAP connection to use for communications.
         * @param aObserver Observer interface to be notified on license 
         * acquisition states. If NULL then no notifications will be supplied 
         * (when handling the download result for example)
         * @return address of an instance of this class
         */
        IMPORT_C static CCameseUtility* NewL( TInt aIapNumber, 
            MCameseObserver* aObserver = NULL );


        /** 
         * Creates an new instance for license acquisition.
         * @param aSockServHandle a socket server handle. If 0 a new socket 
         * server connection is created
         * @param aConnPtr a connection to use for communications. If NULL 
         * a new connection is created
         * @param aObserver Observer interface to be notified on license 
         * acquisition states. If NULL then no notifications will be supplied 
         * (when handling the download result for example)
         * @return address of an instance of this class
         */
        IMPORT_C static CCameseUtility* NewL( TInt aSockServHandle,
            RConnection* aConnPtr, MCameseObserver* aObserver = NULL );


        /**
         * Destructor
         */
        IMPORT_C virtual ~CCameseUtility();


        /**
         * Starts the license acquisition based on a specified DRM header.
         * @param aDrmHeader DRM header to use for license acquisition
         * @param aUrl Heap descriptor to populate with the content URL. 
         * Can be NULL.
         * @param aStatus Request status. On completion contains the error code
         */
        IMPORT_C void AcquireLicense( const TDesC8& aDrmHeader, HBufC8*& aUrl, 
            TRequestStatus& aStatus );


        /**
         * Starts the license acquisition based on a specified file handle.
         * @param aFileHandle a handle to the file for which the license is 
         * acquired.
         * @param aUrl Heap descriptor to populate with the Music Shop URL for 
         * non-silent acquisition or an error page. Can be NULL.
         * @param aStatus Request status. On completion contains the error code.
         */
        IMPORT_C void AcquireLicense( const RFile& aFileHandle, HBufC8*& aUrl, 
            TRequestStatus& aStatus );


        /**
         * Starts the license acquisition based on a specified file path.
         * @param aFileHandle a file handle to use for non-silent license 
         * acquisition
         * @param aLicenseResponse The license response sent by the license 
         * server in a non-silent post-delivery scenerio
         * @param aStatus Request status. On completion contains the error code.
         */
        IMPORT_C void AcquireLicense( const RFile& aFileHandle, 
            const TDesC8& aLicenseResponse, TRequestStatus& aStatus );


        /**
         * Sending the download result.
         * @param aContentUrl The URL to the Content Server where status 
         * should be reported.
         * @param aResult Symbian error code indicating the result of the 
         * content download operation
         * This will be converted to an HRESULT value before being sent to the 
         * Content Server.
         * @param aStatus Request status. On completion contains the error code.
         */
        IMPORT_C void SendDownloadResult(const TDesC8& aContentUrl, TInt aResult, 
            TRequestStatus& aStatus);


        /**
         * Cancels an existing asynchronous call.
         */
        IMPORT_C void Cancel();


        /**
         * Sets the IAP connection to use for communications.
         * @param aIapId The IAP connection that will be used
         */
        IMPORT_C void SetIapId( TInt aIapId );

    private:
        /**
         * Constructor
         */
        CCameseUtility();


        /**
         * Symbian OS 2nd phase constructor.
         */
        void ConstructL( TInt aIapNumber, MCameseObserver* aObserver );

    private:
        CCameseEngine* iEngine;
    };

#endif  // CAMESEUTILITY_H

// End of File
