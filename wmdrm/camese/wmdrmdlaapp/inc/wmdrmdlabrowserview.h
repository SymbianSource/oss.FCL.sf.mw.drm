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
* Description:  Interface class for WMDRM DLA Browser View
*
*/


#ifndef C_WMDRMDLABROWSERVIEW_H
#define C_WMDRMDLABROWSERVIEW_H

#include <aknview.h>
#include <BrCtlSpecialLoadObserver.h>

class CWmDrmDlaBrowserContainer;
class CInternetConnectionManager;

/**
 * Interface for license received callback.
 */
class MBrowserViewLicenseReceivedCallback
    {
    public:

        virtual void LicenseReceived() = 0;

    };

class CWmDrmDlaBrowserView : public CAknView,
                             public MBrCtlSpecialLoadObserver
    {

    public:

        static CWmDrmDlaBrowserView* NewL();
        static CWmDrmDlaBrowserView* NewLC();

        /**
         * Destructor.
         */
        virtual ~CWmDrmDlaBrowserView();

        /**
         * Set the IAP that is used in network connection
         * @param aIap - IAP to be used
         */
        void SetIAP( TInt aIap );

        /**
         * Make a POST-request
         * @param aCallback - Callback used to inform when license
         *                    response is received
         * @param aPostUrl - Post URL
         * @param aPostContentType - Post content type
         * @param aPostData - Post data
         * @param aPostContentBoundary - Post content boundary
         */
        void PostL( MBrowserViewLicenseReceivedCallback* aCallback,
                    const TDesC& aPostUrl,
                    const TDesC8& aPostContentType,
                    const TDesC8& aPostData,
                    const TDesC8& aPostContentBoundary );

        /**
         * Get the license response
         * @return License response or NULL
         */
        HBufC8* LicenseResponse();

    public: // From CAknView

        /**
        * @see CAknView
        */
        TUid Id() const;

        /**
        * @see CAknView
        */
        void HandleCommandL( TInt aCommand );

        /**
        * @see CAknView
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );
        /**
        * @see CAknView
        */
        void DoDeactivate();

        /**
        * @see CAknView
        */
        void HandleClientRectChange();

    public: // From MBrCtlSpecialLoadObserver

        void NetworkConnectionNeededL( TInt* aConnectionPtr,
                                       TInt* aSockSvrHandle,
                                       TBool* aNewConn,
                                       TApBearerType* aBearerType );

        TBool HandleRequestL( RArray<TUint>* aTypeArray,
                              CDesCArrayFlat* aDesArray );

        TBool HandleDownloadL( RArray<TUint>* aTypeArray,
                               CDesCArrayFlat* aDesArray );

    private:

        CWmDrmDlaBrowserView();
        void ConstructL();

        void CreateContainerL();
        void RemoveContainer();
        void BrCtlHandleCommandL( TInt aCommand );

    private: // data

        //Not Owned
        MBrowserViewLicenseReceivedCallback* iCallback;

        CWmDrmDlaBrowserContainer* iContainer;
        HBufC8* iLicenseResponse;
        CInternetConnectionManager* iConMgr;
        TInt iIap;
    };

#endif // C_WMDRMDLABROWSERVIEW_H
