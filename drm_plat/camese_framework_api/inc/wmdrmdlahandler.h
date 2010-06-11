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
* Description:  Interface class for WMDRM DLA functionality
*
*/


#ifndef C_WMDRMDLAHANDLER_H
#define C_WMDRMDLAHANDLER_H

#include <e32base.h>
#include <f32file.h>

class CWmDrmDlaHandlerImpl;

/**
 *  WMDRM DLA Interface
 *
 *  @lib wmdrmdla.lib
 *  @since S60 v9.1
 */
NONSHARABLE_CLASS( CWmDrmDlaHandler ) : public CBase
    {

public:

    IMPORT_C static CWmDrmDlaHandler* NewL();
    IMPORT_C static CWmDrmDlaHandler* NewLC();

    /**
    * Destructor.
    */
    virtual ~CWmDrmDlaHandler();

    /**
     * Checks if license acquisition for file is silent
     *
     * @param   aFile wmdrm file
     * @return  ETrue if license acquisition is silent
     * @leave   KErrNotFound if there is no handler for the file
     */
    IMPORT_C TBool SilentL( const RFile& aFile );

    /**
     * Acquire license based on the file handle
     *
     * @param   aFile wmdrm file
     * @param   aContentUrl contains content URL on return. Can be NULL.
     * If it's not NULL, it must be freed by caller.  
     * @param   aHtmlData contains HTML data returned by license server on
     *                    return. Can be NULL.
     * If it's not NULL, it must be freed by caller.  
     * @leave   KErrNotFound if there is no handler for the file
     */
    IMPORT_C void AcquireLicenseL( const RFile& aFile, HBufC*& aContentUrl, 
                                   HBufC*& aHtmlData );

    /**
     * Acquire license based on the file handle, asynchronous version
     *
     * @param   aFile wmdrm file
     * @param   aContentUrl contains content URL on completion. Can be NULL.
     * If it's not NULL, it must be freed by caller.  
     * @param   aHtmlData contains HTML data returned by license server on 
     *                    completion. Can be NULL.
     * If it's not NULL, it must be freed by caller.  
     * @param   aStatus Request status. On completion contains the error code.
     */
    IMPORT_C void AcquireLicense( const RFile& aFile, HBufC*& aContentUrl, 
                                  HBufC*& aHtmlData, TRequestStatus& aStatus );

    /**
     * Acquire license based on the DRM Header
     *
     * @param   aDrmHeader DRM Header
     * @param   aErrorUrl   URL where error code is sent. Can be NULL.
     * @param   aContentUrl contains content URL on return. Can be NULL.
     * If it's not NULL, it must be freed by caller.  
     * @param   aHtmlData contains HTML data returned by license server on 
     *                    return. Can be NULL.
     * If it's not NULL, it must be freed by caller.  
     * @leave   KErrNotFound if there is no handler for the file
     */
    IMPORT_C void AcquireLicenseFromDrmHeaderL( const TDesC8& aDrmHeader,
                                                HBufC* aErrorUrl,
                                                HBufC*& aContentUrl, 
                                                HBufC*& aHtmlData );

    /**
     * Acquire license based on the DRM Header, asynchronous version
     *
     * @param   aDrmHeader DRM Header
     * @param   aErrorUrl   URL where error code is sent. Can be NULL.
     * @param   aContentUrl contains content URL on completion. Can be NULL.
     * If it's not NULL, it must be freed by caller.  
     * @param   aHtmlData contains HTML data returned by license server on
     *                    completion. Can be NULL.
     * If it's not NULL, it must be freed by caller.  
     * @param   aStatus Request status. On completion contains the error code.
     */
    IMPORT_C void AcquireLicenseFromDrmHeader( const TDesC8& aDrmHeader, 
                                               HBufC* aErrorUrl,
                                               HBufC*& aContentUrl, 
                                               HBufC*& aHtmlData, 
                                               TRequestStatus& aStatus );

    /**
     * Cancels ongoing license acquisition
     *
     */
    IMPORT_C void CancelLicenseAcquisition();

    /**
     * Handles metering certificate OTA
     *
     * @param   aCertificate metering certificate
     */
    IMPORT_C void ProcessMeteringCertificateL( const TDesC8& aCertificate );

    /**
     * Handles metering certificate OTA, asynchronous version
     *
     * @param   aCertificate metering certificate
     * @param   aStatus Request status. On completion contains the error code.
     */
    IMPORT_C void ProcessMeteringCertificate( const TDesC8& aCertificate, 
                                              TRequestStatus& aStatus );

    /**
     * Cancels ongoing metering
     *
     */
    IMPORT_C void CancelMetering();

    /**
     * Sets the IAP to be used in the HTTP transactions
     *
     * @param   aIapId  Internet Access Point id
     */
    IMPORT_C void SetIapId( TInt aIapId );

protected:

private:

    CWmDrmDlaHandler();

    void ConstructL();

private: // data

    /**
     * Implementation class
     * Own.
     */
    CWmDrmDlaHandlerImpl* iImpl;

    };

#endif // C_WMDRMDLAHANDLER_H
