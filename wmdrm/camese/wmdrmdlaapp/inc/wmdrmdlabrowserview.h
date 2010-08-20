/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

class CWmDrmDlaBrowserView : public CAknView
    {

    public:

        static CWmDrmDlaBrowserView* NewL();
        static CWmDrmDlaBrowserView* NewLC();

        /**
         * Destructor.
         */
        virtual ~CWmDrmDlaBrowserView();

    public: // From CAknView

        /**
        * @see CAknView
        */
        TUid Id() const;

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
