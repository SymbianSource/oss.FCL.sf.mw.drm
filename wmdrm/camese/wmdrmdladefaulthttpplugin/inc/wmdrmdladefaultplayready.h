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
* Description:  WMDRM DLA Default Http PlayReady Implementation
*
*/


#ifndef C_WMDRMDLADEFAULTPLAYREADY_H
#define C_WMDRMDLADEFAULTPLAYREADY_H

#include "wmdrmdladefaulthttpmanagerobserver.h"

class CWmDrmDlaDefaultHttpPlugin;
class CWmDrmDlaUiNotifier;
class CWmDrmDlaDefaultHttpManager;
class CHTTPFormEncoder;

/**
 * WMDRM DLA Default Http PlayReady Implementation
 *
 * @lib wmdrmdladefaulthttpplugin.lib
 * @since S60 v9.1
 */
NONSHARABLE_CLASS( CWmDrmDlaDefaultPlayReady ) 
    : public CBase, 
      public MWmDrmDlaDefaltHttpManagerObserver
    {

public:

    static CWmDrmDlaDefaultPlayReady* NewL( 
                CWmDrmDlaDefaultHttpPlugin* aPlugin );
    ~CWmDrmDlaDefaultPlayReady();

    /**
     * Check if initiator is supported by this plugin
     * This method is called only for playready content
     * 
     * @param aInitiator Initiator
     * @param aUiNotifer If initiator is supported, contains initialized UI
     *                   Notifier upon completion. Will be deleted by the
     *                   caller.
     * @return ETrue if initiator is supported by this plugin
     * @leave with standard Symbian error code in case of error
     */
    TBool SupportedInitiatorL( const TDesC8& aInitiator, 
                               CWmDrmDlaUiNotifier*& aUiNotifier );

    /**
     * Process the initiator. This method is used only to pass initiator to the 
     * plugin. The framework does not require the plugin to do anything. 
     * This method is called only for playready content
     *
     * @param aInitiator Initiator
     * @leave with standard Symbian error code in case of fatal error (the 
     * framework does not proceed with license acquisition if this method
     * leaves)
     */
    void ProcessInitiatorL( const TDesC8& aInitiator );

    /*
     * Process join domain challenge.
     * This method is called only for playready content
     *
     * @param aJoinDomainChallenge Join domain challenge
     * @param aUrl Url to be used in join domain request
     * @param aStatus Request status, will contain error code upon completion
     * including but not limited to:
     * - KErrCommsBreak if network connection was lost during communication
     *                  with server
     * - KErrGeneral if unspecified error happened
     */
    void ProcessJoinDomainChallenge( 
            const TDesC8& aJoinDomainChallenge, 
            const TDesC& aUrl, 
            TRequestStatus& aStatus );

    /*
     * Process leave domain challenge.
     * This method is called only for playready content
     *
     * @param aLeaveDomainChallenge Leave domain challenge
     * @param aUrl Url to be used in leave domain request
     * @param aStatus Request status, will contain error code upon completion
     * including but not limited to:
     * - KErrCommsBreak if network connection was lost during communication
     *                  with server
     * - KErrGeneral if unspecified error happened
     */
    void ProcessLeaveDomainChallenge( 
            const TDesC8& aLeaveDomainChallenge, 
            const TDesC& aUrl, 
            TRequestStatus& aStatus );

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

    CWmDrmDlaDefaultPlayReady( CWmDrmDlaDefaultHttpPlugin* aPlugin );
    void ConstructL();
    
    void CompleteClientRequest( TInt aError );
        
private: // data
    
    enum TPlayReadyState
        {
        ENotActive,
        };
    
    TPlayReadyState iState;
    CWmDrmDlaDefaultHttpManager* iHttpManager;
    CHTTPFormEncoder* iFormEncoder;
    TInt iHttpStatus;
    
    //Not owned
    CWmDrmDlaDefaultHttpPlugin* iPlugin;
    TRequestStatus* iClientStatus;
    
    };

#endif // C_WMDRMDLADEFAULTPLAYREADY_H
