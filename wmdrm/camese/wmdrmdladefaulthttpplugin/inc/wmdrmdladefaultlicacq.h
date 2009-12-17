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
* Description:  WMDRM DLA Default Http License Acquisition
*
*/


#ifndef C_WMDRMDLADEFAULTLICACQ_H
#define C_WMDRMDLADEFAULTLICACQ_H

#include "wmdrmdlabrowserobserver.h"
#include "wmdrmdladefaulthttpmanagerobserver.h"

class CWmDrmDlaDefaultHttpPlugin;
class CWmDrmDlaUiNotifier;
class CWmDrmDlaDefaultHttpManager;
class CWmDrmDlaBrowserViewHandler;
class CHTTPFormEncoder;

/**
 * WMDRM DLA Default Http License Acquisition
 * Reference implementation to demonstrate how to handle license acquisition
 * with the MS test site.
 * 
 * @lib wmdrmdladefaulthttpplugin.lib
 * @since S60 v9.1
 */
NONSHARABLE_CLASS( CWmDrmDlaDefaultLicAcq ) 
    : public CBase, 
      public MWmDrmDlaBrowserObserver,
      public MWmDrmDlaDefaltHttpManagerObserver
    {

public:

    static CWmDrmDlaDefaultLicAcq* NewL( CWmDrmDlaDefaultHttpPlugin* aPlugin );
    ~CWmDrmDlaDefaultLicAcq();

    /**
     * Check if file is supported by this plugin
     *
     * @param aFile WMDRM file
     * @param aUiNotifer If file is supported, contains initialized UI Notifier
     *                   upon completion. Will be deleted by the caller after 
     *                   the plugin has been deleted.
     * @return ETrue if file is supported by this plugin
     * @leave with standard Symbian error code in case of error
     */
     TBool SupportedFileL( const RFile& aFile, 
                           CWmDrmDlaUiNotifier*& aUiNotifier );

    /**
     * Check if DRM Header is supported by this plugin
     *
     * @param aHeader DRM Header
     * @param aUiNotifer If header is supported, contains initialized UI
     *                   Notifier upon completion. Will be deleted by the
     *                   caller after the plugin has been deleted.
     * @return ETrue if header is supported by this plugin
     * @leave with standard Symbian error code in case of error
     */
    TBool SupportedDrmHeaderL( const TDesC8& aHeader, 
                               CWmDrmDlaUiNotifier*& aUiNotifier );

    /**
     * Check if license acquisition should be silent for this file
     *
     * @param aFile WMDRM file
     * @return ETrue if file is supported by this plugin
     * @leave with standard Symbian error code in case of error
     */
    TBool SilentL( const RFile& aFile );

    /**
     * Process DRM Header. This method is used only to pass DRM Header to the
     * plugin. The framework does not require the plugin to do anything 
     *
     * @param aDrmHeader DRM Header
     * @leave with standard Symbian error code in case of fatal error (the 
     * framework does not proceed with license acquisition if this method
     * leaves)
     */
    void ProcessDrmHeaderL( const TDesC8& aDrmHeader );

    /**
     * Acquire License. 
     *
     * @param aLicenseChallenge License challenge based on which license
     *                          will be acquired.
     * @param aLicenseServerUrl URL where license challenge is sent
     * @param aStatus Request status, will contain error code upon completion
     * including but not limited to:
     * - KErrCancel if user cancelled the license acquisition at any point
     * - KErrCommsBreak if network connection was lost during communication 
     *                  with server
     * - KErrGeneral if unspecified error happened
     */
    void AcquireLicense( const TDesC8& aLicenseChallenge, 
                         HBufC* aLicenseServerUrl,
                         TRequestStatus& aStatus );

    /**
     * Get downloaded License. 
     *
     * @leave standard Symbian error code in case of error.
     * @return Buffer containing license data. Will be deleted by the caller.
     */
    HBufC8* GetLicenseResponseL();

    /**
     * Send license storage status. 
     *
     * @param aTid Transaction ID parsed from the license response, can be NULL
     * @param aUrl URL where license storage status is sent
     * @param aError License storage status.
     * @param aStatus Request status, will contain error code upon completion
     * including but not limited to:
     * - KErrCommsBreak if network connection was lost during communication
     *                  with server
     * - KErrGeneral if unspecified error happened
     */
    void SendLicenseStorageStatus( HBufC8* aTid, const TDesC& aUrl,
                                   TInt aError, TRequestStatus& aStatus );
    
    /**
     * Send error code to server if drm header parsing failed.
     *
     * @param   aErrorUrl   URL where error code is sent
     * @param   aError      Error code
     * @param aStatus Request status, will contain error code upon completion
     * including but not limited to:
     * - KErrCommsBreak if network connection was lost during communication
     *                  with server
     * - KErrGeneral if unspecified error happened
     */
    void SendDrmHeaderError( const TDesC& aErrorUrl, TInt aError, 
                             TRequestStatus& aStatus );

    /**
     * Get content url. Called only if framework fails to parse content URL 
     * from the license response. Needs to be implemented only if the supported
     * service uses non-standard way to pass the Content URL, otherwise this
     * can return NULL.
     *
     * @return Content URL, can be NULL. Will be deleted by the caller.
     */
    HBufC* GetContentUrlL();

    /**
     * Get HTML data returned by license server. 
     *
     * @return HTML data, can be NULL. Will be deleted by the caller.
     */
    HBufC* GetHtmlDataL();

    /*
     * Cancel license acquisition
     */
    void CancelLicenseAcquisition();

    /**
     * Sets the IAP to be used in the HTTP transactions
     *
     * @param   aIapId  Internet Access Point id
     */
    void SetIapId( TInt aIapId );

public: // From MWmDrmDlaBrowserObserver
    
    const TDesC& PostUrlL();
    const TDesC8& PostContentTypeL();
    const TDesC8& PostDataL();
    const TDesC8& PostContentDataBoundaryL();
    TInt IapId();
    void LicenseResponseReceived( const TDesC8& aLicenseResponse );
    void BrowserExit( TInt aError );

private: // From MWmDrmDlaDefaltHttpManagerObserver
    
    TBool OnGetUsernamePasswordL( HBufC8*& aUsername, HBufC8*& aPassword );
    void OnResponseHeadersL( const RHTTPResponse& aResponse, 
                             const RHTTPHeaders& aHeaders,
                             const RStringPool& aStringPool, 
                             TInt aHttpStatus );
    void OnResponseBodyDataL( const TPtrC8 aDataChunk );
    void OnTransactionComplete( TInt aSystemError );
    
private:

    CWmDrmDlaDefaultLicAcq( CWmDrmDlaDefaultHttpPlugin* aPlugin );
    void ConstructL();
    
    void CompleteClientRequest( TInt aError );
    
    void AcquireLicenseL();
    void SendLicenseStorageStatusL( const TDesC8& aTid, TInt aError );
    TInt MakeHRESULT( TInt aError );
    void SendHRESULTL( const TDesC8& aUrl, const TDesC8& aHeaderField, 
                       TInt aHRESULT );
    
    void HandleRedirectionHeadersL( const RHTTPHeaders& aHeaders,
                                    const RStringPool& aStringPool );
    void HandleLicenseAcquistionHeaders( const RHTTPHeaders& aHeaders,
                                         const RStringPool& aStringPool );
    
    void LaunchBrowserViewL();
    
    static TInt Redirection( TAny* aPtr );
    
private: // data
    
    enum TLicAcqState
        {
        ENotActive,
        ESendingLicenseChallenge,
        ENonSilentLicenseAcquisition,
        ERedirection,
        ESendingStorageStatus,
        EBrowserView
        };
    
    TLicAcqState iState;
    CWmDrmDlaDefaultHttpManager* iHttpManager;
    CHTTPFormEncoder* iFormEncoder;
    CIdle* iIdle;
    CWmDrmDlaBrowserViewHandler* iBrowserViewHandler;
    TInt iHttpStatus;
    TBool iNonSilent;
    HBufC8* iLicenseChallenge;
    HBufC8* iPostData;
    HBufC8* iLicenseResponse;
    HBufC* iLicenseServerUrl16;
    HBufC8* iLicenseServerUrl;
    HBufC* iHtmlData;
    
    //Not owned
    CWmDrmDlaDefaultHttpPlugin* iPlugin;
    TRequestStatus* iClientStatus;
    
    };

#endif // C_WMDRMDLADEFAULTLICACQ_H
