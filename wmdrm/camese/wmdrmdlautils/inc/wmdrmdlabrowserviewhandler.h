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
* Description:  Interface class for WMDRM DLA Browser View Handler
*
*/


#ifndef C_WMDRMDLABROWSERVIEWHANDLER_H
#define C_WMDRMDLABROWSERVIEWHANDLER_H

#include <e32base.h>

class RWmDrmDlaAppClient;
class MWmDrmDlaBrowserObserver;

/**
 *  WMDRM DLA Browser View Handler
 *
 *  @lib wmdrmdlautils.lib
 *  @since S60 v9.1
 */
NONSHARABLE_CLASS( CWmDrmDlaBrowserViewHandler ) : public CActive
    {

    public:

        IMPORT_C static CWmDrmDlaBrowserViewHandler* NewL();
        IMPORT_C static CWmDrmDlaBrowserViewHandler* NewLC();

        /**
         * Launches browser view
         *
         * @param aObserver Observer used to query needed information for
         *                  launching and to inform caller about browser
         *                  view activities.
         * @leave with KErrArgument, if aObserver is NULL, KErrInUse,
         *        if browser view is already active, KErrNotSupported,
         *        if CCoeEnv isn't available, otherwise
         *        standard Symbian error code in case of error.
         */
        IMPORT_C void LaunchBrowserViewL( 
                                    MWmDrmDlaBrowserObserver* aObserver );
        
        /**
         * Destructor.
         */
        virtual ~CWmDrmDlaBrowserViewHandler();

    protected: // from base class CActive

        /**
         * From CActive Cancels async request.
         */
        void DoCancel();

        /**
         * From CActive Called when async request completes.
         */
        void RunL();

        /**
         * From CActive Called if RunL leaves.
         */
        TInt RunError( TInt aError );    
    
    private:

        CWmDrmDlaBrowserViewHandler();
        void ConstructL();

        void CompleteSelf();

    private: // data

        enum TBrowserViewHandlerState
            {
            EIdle,
            ELaunchBrowserView
            };

        TBrowserViewHandlerState iState;
        RWmDrmDlaAppClient* iClient;
                
        //Not owned
        MWmDrmDlaBrowserObserver* iObserver;
        
    };

#endif // C_WMDRMDLABROWSERVIEWHANDLER_H
