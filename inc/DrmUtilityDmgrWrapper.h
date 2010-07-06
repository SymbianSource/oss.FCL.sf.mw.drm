/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CDRMUTILITYDMGRWRAPPER_H
#define CDRMUTILITYDMGRWRAPPER_H

#include <f32file.h> // RFs
#include <RoapObserver.h> // Roap::MRoapObserver
#include <AknProgressDialog.h> // MAknProgressDialogCallback CAknProgressDialog

// download apis
#include <downloadmanager.h>
#include <download.h>
#include <downloadevent.h>

namespace Roap
    {
    class CRoapEng;
    }

namespace DRM
    {
    class CDrmUtilityConnection;
    }
class CDRMRights;
class CEikProgressInfo;
class DrmUtilityEventHandler;
class QDrmUtilityDmgrEventHandler;

/**
* Environment gate function
*
* @since S60 v5.0
* @return pointer to DMgr handler
*/
IMPORT_C TAny* GateFunctionDMgr();

class MDrmUtilityDmgrWrapper
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
*  @lib DrmUtilityDmgrWrapper
*  @since S60 v5.0
*/
class CDrmUtilityDmgrWrapper :
    public CActive,
    public Roap::MRoapObserver,
    public MDrmUtilityDmgrWrapper,
    public MProgressDialogCallback
    {
private:

    enum TDownloadState
        {
        EInit,
        EGetMeteringTrigger,
        ESaveMeteringTrigger,
        EMeteringReportSubmit,
        EGetPrUrlTrigger,
        ESavePrUrlTrigger,
        EPrRoapRequest,
        EComplete
        };
public:

    static CDrmUtilityDmgrWrapper* NewL();

    static CDrmUtilityDmgrWrapper* NewLC();

    virtual ~CDrmUtilityDmgrWrapper();

    /**
    * Download a ROAP trigger from URL and handle it
    *
    * @param aUrl  URL of ROAP trigger
    */
    void DownloadAndHandleRoapTriggerL( const HBufC8* aUrl );

    void DownloadAndHandleRoapTriggerL( const HBufC8* aUrl,
        CCoeEnv& aCoeEnv );

    HBufC8* GetErrorUrlL();

// From Roap::MRoapObserver
    /**
    * ConnectionConfL
    * @return ETrue: the network can be establish
    *         EFalse: the ROAP transaction is canceled
    *
    * @leave  System wide error code */
    TBool ConnectionConfL();

    /**
    * ContactRiConfL
    * @return ETrue: the user consent is achieved
    *         EFalse: the user consent is not achieved
    *
    * @leave  System wide error code */
    TBool ContactRiConfL();

    /**
    * TransIdConfL
    * @return ETrue: the user consent is achieved
    *         EFalse: the user consent is not achieved
    *
    * @leave  System wide error code */

    TBool TransIdConfL();

    /**
    * RightsObjectDetailsL
    *
    * The function is called after successful completion of RO acquisition
    * protocol. The passes information about stored rights objects to the
    * observer.
    *
    * @since  3.0
    * @param aRightsList: A list of pointers to rights objects.
    *                     Contents of aRightsList are owend by ROAP engine
    *
    * @leave  System wide error code */
    void RightsObjectDetailsL( const RPointerArray<CDRMRights>& aRightsList );

    /**
    * ContentDownloadInfoL
    *
    * The function is called when the ROAP engine notices that it is about to
    * receive a multipart content as a ROAP response. The method must return via
    * out-parameter the path to a temp folder where the content is saved during
    * download. The name that is used as filename when saving the content to
    * the appropriate palce and the maximum size of the content (a safety upper limit)
    * must also be provided via out-parameters.
    *
    * @since 3.0
    * @param aTempFolder: (out-param) The path of the temp folder, or KNullDesC
    * @param aContentName: (out-param) The name of the content, or KNullDesC (defaut name is used)
    * @param aMaxSize: (out-param) The maximum size of the content, or -1 if not known
    *
    * @leave System wide error code */

    void ContentDownloadInfoL( TPath& aTempFolder,
            TFileName& aContentName,
            TInt& aMaxSize );

    /**
    * ContentDetailsL
    *
    * The function is called when the ROAP engine has received a DCF content (together
    * with a ROAP response) The ROAP engine saves the DCF to the appropriate location
    * and gives out the information about the saved DCF file by calling this method
    *
    * @since 3.0
    * @param aPath: The path andf the filename of the saved DCF file
    * @param aType: The plain MIME type of the saved DCF file
    * @param aAppUid: The handler app UID of the saved DCF file
    *
    * @leave System wide error code */

    void ContentDetailsL( const TDesC& aPath,
            const TDesC8& aType,
            const TUid& aAppUid );

    /**
    * RoapProgressInfoL
    *
    * The function provides progress information about ROAP processing to the
    * observer
    *
    * @since  3.0
    * @param aProgressInfo: An integer value representing the state of
    *                       ROAP processing, counting in bytes
    *
    * @leave  System wide error code */

    void RoapProgressInfoL( const TInt aProgressInfo );

    /**
    * ErrorUrlL
    *
    * The function is called in ROAP error case and it provides an error URL
    * for the caller. The calling entity should send an HTTP GET request to
    * the URL which should then return an HTML page.
    *
    * @since  3.0
    * @param aErrorUrl: The error URL
    *
    *
    * @leave  System wide error code */

    void ErrorUrlL( const TDesC8& aErrorUrl );

    /**
    * PostResponseUrlL
    *
    * The function is called if handled ROAP metering report response
    * contains PostResponseURL extension. The calling entity should send
    * an HTTP GET request to the URL, which should return ROAP trigger,
    * download descriptor or multipart message containing both
    * download descriptor and ROAP trigger.
    *
    * @since  3.2
    * @param aPostResponseUrl: The URL to be fetched
    *                          after handling ROAP metering response
    *                          with PostResponseURL extension.
    *
    *
    * @leave  System wide error code */
    void PostResponseUrlL( const TDesC8& aPostResponseUrl );

public: // Call back methods of MAknProgressDialogCallback

    /**
    * ProgressDialog call back method.
    * Get's called when a dialog is dismissed.
    *
    * @param aButtonId ID of the button pressed
    */
    void DialogDismissedL( TInt aButtonId );
    
    /**
	* Handle download manager events
	*
	* @param aEvent the event
	*/
            
    void HandleDownloadEventL( WRT::DownloadEvent* aEvent );

protected:
    //from Cactive
    virtual void DoCancel();

    virtual void RunL();

    virtual TInt RunError( TInt aError );
private:

    /**
    * C++ default constructor.
    */
    CDrmUtilityDmgrWrapper();

    void ConstructL();

    /**
    * Set the browser default access point to be used
    */


    void ShowProgressNoteL( );

    void RemoveProgressNoteL( );

    void DoConnectL( TDownloadState aNextState );

    void DoDownloadRoapTriggerL( TDownloadState aNextState );

    void DoSaveRoapTriggerL( TDownloadState aNextState );

    void DoHandleRoapTriggerL( TDownloadState aNextState );

    void CompleteToState( TDownloadState aNextState, TInt aError );
    
    void ProcessDownloadEventL( WRT::Download& aDownload, WRT::DownloadEvent& aEvent );

private: // data

    /**
    * Download manager session
    */
    WRT::DownloadManager* iDlMgr;

    /**
     * Download instance needs to be stored for handling signal slot mechanism
     */
    WRT::Download* iDownload;
    
    /**
    * Used to make downloads synchronous
    */
    CActiveSchedulerWait iWait;

    /**
    * to store information on download
    */
    TBool iDownloadSuccess;
    TBool iConnectionError;

    /**
    * trigger URL
    */
    HBufC8* iTriggerUrl;

    /**
    * trigger buffer
    */
    HBufC8* iTriggerBuf;

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

    /**
    * IAP (from ROAP or from UI)
    */
    TUint32 iIapId;

    TDownloadState iState;

    Roap::CRoapEng* iRoapEng;

    RFs iFs;

    HBufC* iFileName;

    DRM::CDrmUtilityConnection* iConnection;
    
    QDrmUtilityDmgrEventHandler* iDrmUtilityDmgrEventHandler; 
    };

#endif // CDRMUTILITYDMGRWRAPPER_H
