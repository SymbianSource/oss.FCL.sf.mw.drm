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


#ifndef C_WMDRMDLAHTTPMETERINGPLUGINFW_H
#define C_WMDRMDLAHTTPMETERINGPLUGINFW_H

#include <e32base.h>
#include <f32file.h>
#include <wmdrmdlatypes.h>
#include <wmdrmdlacancelobserver.h>

class CWmDrmDlaHttpPluginIf;
class CWmDrmDlaUiNotifier;

/**
 *  Implementation class for WMDRM DLA HTTP Metering FW
 *  @since S60 v9.1
 */
NONSHARABLE_CLASS( CWmDrmDlaHttpMeteringPluginFw ) 
    : public CActive, public MWmDrmDlaCancelObserver
    {

public:

    static CWmDrmDlaHttpMeteringPluginFw* NewL();
    static CWmDrmDlaHttpMeteringPluginFw* NewLC();    

    /**
    * Destructor.
    */
    virtual ~CWmDrmDlaHttpMeteringPluginFw();

    /**
     * Handles metering certificate OTA
     *
     * @param   aCertificate metering certificate
     * @param   aStatus Request status. On completion contains the error code.
     */
    void ProcessMeteringCertificate( const TDesC8& aCertificate, 
                                     TRequestStatus& aStatus );

    /*
     * Cancels metering
     */
    void CancelMetering();
    
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

    CWmDrmDlaHttpMeteringPluginFw();
    void ConstructL();
    
    void CompleteSelf();
    void CompleteClientRequest( TInt aError );
    void Reset();
    
    void ResolvingHttpPluginHandlerL();
    void RetrievingMeteringChallenge();
    void ProcessingMeteringChallenge();
    void RetrievingMeteringResponse();
    void ProcessingMeteringResponse();
    void MeteringFinished();
    
    void ResolveHttpPluginWithCertificateL( const TDesC8& aCertificate );
    void HandleErrorL( TInt aError );

    static TInt AsyncCancel( TAny* aPtr );

private: // data

    enum THttpMeteringPluginFwState
        {
        EIdle = 0,
        EResolvingHttpPlugin,
        ERetrievingMeteringChallenge,
        EProcessingMeteringChallenge,
        ERetrievingMeteringResponse,
        EProcessingMeteringResponse,
        EMeteringFinished
        };

    THttpMeteringPluginFwState iState;
    CWmDrmDlaHttpPluginIf* iHttpPlugin;
    CWmDrmDlaUiNotifier* iUiNotifier;
    CIdle* iIdle;
    TInt iIapId;
    
    HBufC8* iMeteringChallenge;
    HBufC* iMeteringUrl;
    HBufC8* iMeteringResponse;
    TUint32 iPartialMetering;
    
    //Not owned
    const TDesC8* iCertificate;
    TRequestStatus* iClientStatus;
    
    };


#endif // C_WMDRMDLAHTTPMETERINGPLUGINFW_H
