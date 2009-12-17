/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  HTTP plugin framework interface for WMDRM DLA
*
*/


#ifndef C_WMDRMDLAHTTPLICACQPLUGINFW_H
#define C_WMDRMDLAHTTPLICACQPLUGINFW_H

#include <e32base.h>
#include <f32file.h>
#include <wmdrmdlatypes.h>
#include <wmdrmdlacancelobserver.h>

class CWmDrmDlaHttpPluginIf;
class CWmDrmDlaUiNotifier;
class CWmDrmDlaParser;

/**
 *  Implementation class for WMDRM DLA HTTP License Acquisition FW
 *  @since S60 v9.1
 */
NONSHARABLE_CLASS( CWmDrmDlaHttpLicAcqPluginFw ) 
    : public CActive, public MWmDrmDlaCancelObserver
    {

public:

    static CWmDrmDlaHttpLicAcqPluginFw* NewL();
    static CWmDrmDlaHttpLicAcqPluginFw* NewLC();    

    /**
    * Destructor.
    */
    virtual ~CWmDrmDlaHttpLicAcqPluginFw();

    /**
     * Checks if license acquisition for file is silent
     *
     * @param   aFile wmdrm file
     * @return  ETrue if license acquisition is silent
     * @leave   KErrNotFound if there is no handler for the file
     */
    TBool SilentL( const RFile& aFile );

    /**
     * Acquire license for given WMDRM file
     *
     * @param   aFile wmdrm file
     * @param   aContentUrl contains content URL on completion. Can be NULL.
     * If it's not NULL, it must be freed by caller.  
     * @param   aHtmlData contains HTML data returned by license server
     *                    on completion. Can be NULL.
     * If it's not NULL, it must be freed by caller.  
     * @param   aStatus Request status. On completion contains the error code.
     */
    void AcquireLicense( const RFile& aFile, HBufC*& aContentUrl, 
                         HBufC*& aHtmlData, TRequestStatus& aStatus );

    /**
     * Acquire license based on DRM Header
     *
     * @param   aDrmHeader DRM Header
     * @param   aErrorUrl URL where error code is sent. Can be NULL.
     * @param   aContentUrl contains content URL on completion. Can be NULL.
     * If it's not NULL, it must be freed by caller.  
     * @param   aHtmlData contains HTML data returned by license server
     *                    on completion. Can be NULL.
     * If it's not NULL, it must be freed by caller.  
     * @param   aStatus Request status. On completion contains the error code.
     */
    void AcquireLicenseFromDrmHeader( const TDesC8& aDrmHeader, 
                                      HBufC* aErrorUrl,
                                      HBufC*& aContentUrl, 
                                      HBufC*& aHtmlData, 
                                      TRequestStatus& aStatus );

    /*
     * Cancels license acquisition
     */
    void CancelLicenseAcquisition();
    
    /**
     * Sets the IAP to be used in the HTTP transactions
     *
     * @param   aIapId  Internet Access Point id
     */
    void SetIapId( TInt aIapId );

protected:

    // from base class CActive

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

public: // from MWmDrmDlaCancelObserver
    
    void UserCancellation();
    
private:

    CWmDrmDlaHttpLicAcqPluginFw();
    void ConstructL();
    
    void CompleteSelf();
    void CompleteClientRequest( TInt aError );
    void Reset();
    
    TBool SilentHandlerL( const RFile& aFile );
    void ResolvingHttpPluginFromDRMHeaderHandlerL();
    void ResolvingHttpPluginFromFileHandleHandlerL();
    void RetrievingDRMHeaderFromFileHandleHandler();
    void ProcessingDRMHeaderHandler();
    void SendingDRMHeaderErrorHandler( TInt aError );
    void SentDRMHeaderErrorHandlerL();
    void RetrievingChallengeHandler();
    void RetrievingLicenseHandler();
    void AddingLicenseHandlerL();
    void ProcessingLicenseHandler();
    void SendingLicenseStorageStatusHandler();
    void SentLicenseStorageStatusHandler();
    void LicensingCompleteL();
    
    void ResolveHttpPluginL( const RFile& aFile );
    void ResolveHttpPluginL( const TDesC8& aDrmHeader );
    
    void ParseDrmHeaderL( const TDesC8& aDrmHeader, 
                          HBufC*& aLicenseServerUrl );
    void ParseLicenseResponseL( const TDesC8& aLicenseResponse, 
                                HBufC8*& aTid, HBufC*& aContentUrl );
    
    void HandleErrorL( TInt aError );
    
    static TInt AsyncCancel( TAny* aPtr );

private: // data

    enum THttpLicAcqPluginFwState
        {
        EIdle = 0,
        EResolvingHttpPluginFromDRMHeader,
        EResolvingHttpPluginFromFileHandle,
        ERetrievingDRMHeaderFromFileHandle,
        EProcessingDRMHeader,
        ESentDRMHeaderError,
        ERetrievingChallenge,
        ERetrievingLicense,
        EAddingLicense,
        EProcessingLicense,
        ESendingLicenseStorageStatus,
        ESentLicenseStorageStatus,
        ELicensingComplete
        };

    THttpLicAcqPluginFwState iState;
    CWmDrmDlaHttpPluginIf* iHttpPlugin;
    CWmDrmDlaUiNotifier* iUiNotifier;
    CWmDrmDlaParser* iDlaParser;
    CActiveSchedulerWait* iWait;
    CIdle* iIdle;
    TInt iIapId;
    TBool iSilentInitialised;
    
    HBufC8* iDrmHeader;
    HBufC* iLicenseServerUrl;
    HBufC8* iLicenseChallenge;
    HBufC8* iLicenseResponse;
    
    //Not owned
    HBufC* iErrorUrl;
    const RFile* iFile;
    HBufC** iContentUrl;
    HBufC** iHtmlData;
    TRequestStatus* iClientStatus;
    
    };


#endif // C_WMDRMDLAHTTPLICACQPLUGINFW_H
