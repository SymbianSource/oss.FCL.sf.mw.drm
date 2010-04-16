/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef CDRMHELPERDMGRWRAPPER_H
#define CDRMHELPERDMGRWRAPPER_H

#include <DownloadMgrClient.h>
#include <AknProgressDialog.h>
#include <eikprogi.h>

/**
* Environment gate function
*
* @since S60 3.1
* @return pointer to DMgr handler
*/
IMPORT_C TAny* GateFunctionDMgr();

class MDRMHelperDMgrWrapper
    {

public:
    virtual void DownloadAndHandleRoapTriggerL( const HBufC8* aUrl ) = 0;

    virtual void DownloadAndHandleRoapTriggerL( const HBufC8* aUrl,
                                                CCoeEnv& aCoeEnv ) = 0;

    virtual HBufC8* GetErrorUrlL() = 0;

    };

/**
*  Class for downloading ROAP triggers
*
*  @lib DRMHelperDMgrWrapper
*  @since S60 v3.1
*/
class CDRMHelperDMgrWrapper : CBase,
                              public MHttpDownloadMgrObserver,
                              public MDRMHelperDMgrWrapper,
                              public MProgressDialogCallback
    {

public:

    static CDRMHelperDMgrWrapper* NewL();

    static CDRMHelperDMgrWrapper* NewLC();

    virtual ~CDRMHelperDMgrWrapper();

    /**
    * Download a ROAP trigger from URL and handle it
    *
    * @since S60 3.1
    * @param aUrl  URL of ROAP trigger
    */
    void DownloadAndHandleRoapTriggerL( const HBufC8* aUrl );

    void DownloadAndHandleRoapTriggerL( const HBufC8* aUrl, CCoeEnv& aCoeEnv );

    HBufC8* GetErrorUrlL();


// from base class MHttpDownloadMgrObserver

    /**
    * From MHttpDownloadMgrObserver.
    * Handle download manager events
    *
    * @since S60 3.1
    * @param aDownload the download
    * @param aEvent the event
    */
    void HandleDMgrEventL( RHttpDownload& aDownload, THttpDownloadEvent aEvent );

public: // Call back methods of MAknProgressDialogCallback

    /**
    * ProgressDialog call back method.
    * Get's called when a dialog is dismissed.
    *
    * @since S60 3.2
    * @param aButtonId ID of the button pressed
    */
    void DialogDismissedL( TInt aButtonId );

protected:

private:

    /**
    * C++ default constructor.
    */
    CDRMHelperDMgrWrapper();

    void ConstructL();

    /**
    * Set the browser default access point to be used
    *
    * @since S60 3.0
    */
    void SetDefaultAccessPointL();

    void DoDownloadAndHandleRoapTriggerL( const HBufC8* aUrl );

    void ShowProgressNoteL( );

    void RemoveProgressNoteL( );

    void HandlePostResponseUrlL();


private: // data

    /**
    * Download manager session
    */
    RHttpDownloadMgr iDlMgr;

    /**
    * Used to make downloads synchronous
    */
    CActiveSchedulerWait iWait;

    /**
    * to store information on download
    */
    TBool iDownloadSuccess;
    TBool iConnectionError;

    TBool iDialogDismissed;

    /**
    * Progess note dialog and progress info
    */
    CAknProgressDialog* iProgressNoteDialog;        // owned
    CEikProgressInfo* iProgressInfo;                // not owned
    TInt iCurrentProgressValue;                     // owned
    TInt iProgressIncrement;                        // owned

    /**
    * Control environment
    */
    CCoeEnv* iCoeEnv;

    /**
    * Is CoeEnv given
    */
    TBool iUseCoeEnv;

    /**
    * Error url for ROAP temporary error
    */
    HBufC8* iErrorUrl;

    /**
    * Post response url for ROAP prUrl
    */
    HBufC8* iPostResponseUrl;

    };

#endif // CDRMHELPERDMGRWRAPPER_H
