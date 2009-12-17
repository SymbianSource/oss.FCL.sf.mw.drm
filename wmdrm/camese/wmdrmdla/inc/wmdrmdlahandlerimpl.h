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
* Description:  Implementation class for WMDRM DLA Handler
*
*/


#ifndef C_WMDRMDLAHANDLERIMPL_H
#define C_WMDRMDLAHANDLERIMPL_H

#include <e32base.h>
#include <f32file.h>
#include <wmdrmdlatypes.h>

class CWmDrmDlaHttpLicAcqPluginFw;
class CWmDrmDlaHttpMeteringPluginFw;
class CWmDrmDlaRequest;

/**
 *  Implementation class for WMDRM DLA Handler
 *
 *  @since S60 v9.1
 */
NONSHARABLE_CLASS( CWmDrmDlaHandlerImpl ) : public CActive
    {

public:

    static CWmDrmDlaHandlerImpl* NewL();
    static CWmDrmDlaHandlerImpl* NewLC();

    /**
    * Destructor.
    */
    virtual ~CWmDrmDlaHandlerImpl();

    /**
     * Checks if license acquisition for file is silent
     *
     * @param   aFile wmdrm file
     * @return  ETrue if license acquisition is silent
     * @leave   KErrNotFound if there is no handler for the file
     */
    TBool SilentL( const RFile& aFile );

    /**
     * Acquire license based on the file handle
     *
     * @param   aFile wmdrm file
     * @param   aContentUrl contains content URL on return. Can be NULL.
     * If it's not NULL, it must be freed by caller.  
     * @param   aHtmlData contains HTML data returned by license server
     *                    on return. Can be NULL.
     * If it's not NULL, it must be freed by caller.  
     */
    void AcquireLicenseL( const RFile& aFile, HBufC*& aContentUrl, 
                          HBufC*& aHtmlData );

    /**
     * Acquire license based on the file handle, asynchronous version
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
     * Acquire license based on the DRM Header
     *
     * @param   aDrmHeader DRM Header
     * @param   aErrorUrl URL where error code is sent. Can be NULL.
     * @param   aContentUrl contains content URL on return. Can be NULL.
     * If it's not NULL, it must be freed by caller.  
     * @param   aHtmlData contains HTML data returned by license server
     *                    on return. Can be NULL.
     * If it's not NULL, it must be freed by caller.  
     */
    void AcquireLicenseFromDrmHeaderL( const TDesC8& aDrmHeader, 
                                       HBufC* aErrorUrl,
                                       HBufC*& aContentUrl, 
                                       HBufC*& aHtmlData );

    /**
     * Acquire license based on the DRM Header, asynchronous version
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
     * Cancels current and all queued license acquisition requests
     */
    void CancelLicenseAcquisition();
    
    /**
     * Handles metering certificate OTA
     *
     * @param   aCertificate metering certificate
     */
    void ProcessMeteringCertificateL( const TDesC8& aCertificate );

    /**
     * Handles metering certificate OTA, asynchronous version
     *
     * @param   aCertificate metering certificate
     * @param   aStatus Request status. On completion contains the error code.
     */
    void ProcessMeteringCertificate( const TDesC8& aCertificate, 
                                     TRequestStatus& aStatus );

    /*
     * Cancels current and all queued metering requests
     */
    void CancelMetering();
    
    /**
     * Sets the IAP to be used in the HTTP transactions
     *
     * @param   aIapId  Internet Access Point id
     */
    void SetIapId( TInt aIapId );
    
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

    CWmDrmDlaHandlerImpl();
    void ConstructL();

    void CompleteSelf();
    void CompleteClientRequest( TInt aError );
    void CompleteSyncRequest( TInt aError );
    
    /*
     * Takes next request to handling.
     * If synchronous request is available, it will be handled first.
     * Otherwise next async request is taken from the queue and handled.
     * If there aren't any requests, state is set to idle
     */
    void NextRequest();
    
    //State handlers
    
    /*
     * Completes current request and retrieves next one, 
     * if available, to handling
     */
    void Completed( TInt aError );
	
    /*
     * Makes asynchronous call to CWmDrmDlaHttpPluginFw to 
     * acquire license from drm header
     */
    void AcquiringLicenseFromDrmHeader();
    
    /*
     * Makes asynchronous call to CWmDrmDlaHttpPluginFw to
     * acquire license from file
     */
    void AcquiringLicense();
    
    /*
     * Makes asynchronous call to CWmDrmDlaHttpPluginFw to
     * process metering certificate
     */
    void ProcessingMeteringCertificate();
    
private: // data

    enum TDlaHandlerState
        {
        EIdle,
        EAcquiringLicense,
        EAcquiringLicenseFromDrmHeader,
        EProcessingMeteringCertificate,
        ECompleted
        };

    TDlaHandlerState iState;
    CWmDrmDlaHttpLicAcqPluginFw* iLicenseAcquisitionFw;
    CWmDrmDlaHttpMeteringPluginFw* iMeteringFw;
    RPointerArray<CWmDrmDlaRequest> iRequests;
    CWmDrmDlaRequest* iCurrentRequest;
    CWmDrmDlaRequest* iSyncRequest;
    TInt iSyncReturnValue;
    CActiveSchedulerWait* iWait;
    };

#endif // C_WMDRMDLAHANDLERIMPL_H
