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
* Description:  WMDRM DLA Default Metering
*
*/


#ifndef C_WMDRMDLADEFAULMETERING_H
#define C_WMDRMDLADEFAULMETERING_H

#include "wmdrmdladefaulthttpmanagerobserver.h"

class CWmDrmDlaDefaultHttpPlugin;
class CWmDrmDlaUiNotifier;
class CWmDrmDlaDefaultHttpManager;
class CHTTPFormEncoder;

/**
 * WMDRM DLA Default Metering
 * Reference implementation to demonstrate how to handle metering
 * with the MS test site.
 * 
 * @lib wmdrmdladefaulthttpplugin.lib
 * @since S60 v9.1
 */
NONSHARABLE_CLASS( CWmDrmDlaDefaultMetering ) 
    : public CBase, 
      public MWmDrmDlaDefaltHttpManagerObserver
    {

public:

    static CWmDrmDlaDefaultMetering* NewL( 
                CWmDrmDlaDefaultHttpPlugin* aPlugin );
    ~CWmDrmDlaDefaultMetering();

     /**
     * Check if metering certificate is supported by this plugin
     *
     * @param aCertificate Metering certificate
     * @param aUiNotifer If certificate is supported, contains initialized UI
     *                   Notifier upon completion. Will be deleted by the
     *                   caller after the plugin has been deleted.
     * @return ETrue if certificate is supported by this plugin
     * @leave with standard Symbian error code in case of error
     */
    TBool SupportedMeteringCertificateL( const TDesC8& aCertificate, 
                                         CWmDrmDlaUiNotifier*& aUiNotifier );
    
    /**
     * Process metering challenge. 
     *
     * @param aMeteringChallenge Metering challenge
     * @param aUrl Url to be used in metering request
     * @param aStatus Request status, will contain error code upon completion
     * including but not limited to:
     * - KErrCommsBreak if network connection was lost during communication
     *                  with server
     * - KErrGeneral if unspecified error happened
     */
    void ProcessMeteringChallenge( const TDesC8& aMeteringChallenge, 
                                   const TDesC& aUrl, 
                                   TRequestStatus& aStatus );

    /**
     * Get metering response. 
     *
     * @param aResponse Contains metering response upon completion. 
     *                  Will be deleted by the caller.
     * @param aStatus Request status, will contain error code upon completion
     */
    void GetMeteringResponse( HBufC8*& aResponse, TRequestStatus& aStatus );

    /**
     * Inform plugin that metering finished. 
     *
     */
    void MeteringFinished();
    
    /*
     * Cancel metering
     */
    void CancelMetering();

    /**
     * Sets the IAP to be used in the HTTP transactions
     *
     * @param   aIapId  Internet Access Point id
     */
    void SetIapId( TInt aIapId );

private: // From MWmDrmDlaDefaltHttpManagerObserver
    
    TBool OnGetUsernamePasswordL( HBufC8*& aUsername, HBufC8*& aPassword );
    void OnResponseHeadersL( const RHTTPResponse& aResponse, 
                             const RHTTPHeaders& aHeaders, 
                             const RStringPool& aStringPool, 
                             TInt aHttpStatus );
    void OnResponseBodyDataL( const TPtrC8 aDataChunk );
    void OnTransactionComplete( TInt aSystemError );

private:

    CWmDrmDlaDefaultMetering( CWmDrmDlaDefaultHttpPlugin* aPlugin );
    void ConstructL();
    
    void CompleteClientRequest( TInt aError );
    
    void ProcessMeteringChallengeL( const TDesC8& aMeteringChallenge );
    
private: // data
    
    enum TMeteringState
        {
        ENotActive,
        ESendingMeteringChallenge
        };
    
    TMeteringState iState;
    CWmDrmDlaDefaultHttpManager* iHttpManager;
    CHTTPFormEncoder* iFormEncoder;
    TInt iHttpStatus;
    HBufC8* iMeteringResponse;
    HBufC* iMeteringServerUrl;
    
    //Not owned
    CWmDrmDlaDefaultHttpPlugin* iPlugin;
    TRequestStatus* iClientStatus;
    
    };

#endif // C_WMDRMDLADEFAULMETERING_H
