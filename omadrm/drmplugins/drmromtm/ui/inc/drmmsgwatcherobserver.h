/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*     Msv operation for drm view closing
*
*/


#ifndef C_DRMMSGWATCHEROBSERVER_H
#define C_DRMMSGWATCHEROBSERVER_H



#include <msvapi.h>        // CMsvOperation
#include <AknServerApp.h>  // MAknServerAppExitObserver
#include <AknLaunchAppService.h> // app service
#include <DocumentHandler.h> // Document handler

class CDocumentHandler;

// CLASS DECLARATION

/**
* CDrmMsgWatcherObserver
* Simple operation class which watches a service (drm),
* completing when that thread closes.
*
* The passed CMsvSession is not used (merely required to base-construct
* a CMsvOperation).
*/
NONSHARABLE_CLASS( CDrmMsgWatcherObserver ) : public CMsvOperation, public MAknServerAppExitObserver			
    { 
    public:

	    static CDrmMsgWatcherObserver* NewL( 
            CMsvSession& aMsvSession,
            TInt aPriority,
            TRequestStatus& aObserverRequestStatus,
            TUid aMtm,
            CAiwGenericParamList* aParamList );


	    static CDrmMsgWatcherObserver* NewL( 
            CMsvSession& aMsvSession,
            TInt aPriority,
            TRequestStatus& aObserverRequestStatus,
            TUid aMtm,
            const TDesC& aFileName,
            TDataType& aDataType );
	    
        /**
        * C++ constructor
        */
        CDrmMsgWatcherObserver(
            CMsvSession& aMsvSession,
            TInt aPriority,
            TRequestStatus& aObserverRequestStatus,
            TUid aMtm );

        /**
        * Destructor.
        */
        ~CDrmMsgWatcherObserver();

    private: // From MApaServerAppExitObserver
        void HandleServerAppExit( TInt aReason );
		
    protected:
        
        
        /**
        * From CMsvOperation
        */
        virtual const TDesC8& ProgressL();

        /**
        * From CMsvOperation
        */
        virtual const TDesC8& FinalProgress();
                
        /**
        * From CActive
        */
        virtual void RunL();

        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        *
        */
        void Start();

    private:
        void ConstructL( CAiwGenericParamList* aParamList );
        void ConstructL( const TDesC& aFileName, TDataType& aDataType );
        
        /**
        * Completes observer with the completion code
        * @param aCode: Completion code
        */
        void CompleteObserver( TInt aCode );


    protected:// Data
        HBufC8*     iProgress;

    private:
        CAknLaunchAppService* iService;
        CDocumentHandler* iDocHandler;

    };
    
#endif // C_DRMMSGWATCHEROBSERVER_H   

// End of file
