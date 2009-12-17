/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#ifndef DRM_UDT_MODULE_H
#define DRM_UDT_MODULE_H

// INCLUDES

#include <e32base.h>
#include <http.h>
#include <stringpool.h>
#include <http/mhttpauthenticationcallback.h>
#include "DrmUdtObserver.h"


// FORWARD DECLARATIONS
class CDrmUdtConn;
class CPeriodic;
class MDrmUdtObserver;

// CLASS DECLARATION

class CDrmUdtHandler: public CActive,
                      public MHTTPTransactionCallback,
                      public MHTTPDataSupplier
    {
    public:     // Constructors and destructor
    
        enum TUdtErrorCode
            {
            EUdtOk = KErrNone,
            EUdtClientError = -100,
            EUdtServerError = -101,
            EUdtInvalidServerAddress = -102,
            EUdtKeyRestoreFailed = -103
            };
  
        /**
        * Two-phased constructor
        */
        IMPORT_C static CDrmUdtHandler* NewL();

        /**
        * Destructor
        */
        IMPORT_C virtual ~CDrmUdtHandler();

    public:     // new methods

        /**
        * DoUserDataTransferL
        *
        * 
        * @since  3.0
        * @param  aOneTimePassword:
        * @param  aRdbFileName:
        * @param  aServiceUrl:
        * @param  aStatus:
        *
        *
        * @leave  System wide error code
        */  
        EXPORT_C void DoUserDataTransferL( const TDesC8& aOneTimePassword,
                                           const TDesC8& aServiceUrl,
                                           MDrmUdtObserver* aObserver,
                                           TRequestStatus& aStatus );
                                                           
        /**
        * SetPreferredIap
        * 
        * 
        * @since  3.0
        * @param  aPreferredIap:
        *                        
        *
        * @leave  System wide error code
        */  
        IMPORT_C void SetPreferredIap( TUint32 aPreferredIap );


    protected:  // from CActive

        virtual void DoCancel();

        virtual void RunL();

        virtual TInt RunError( TInt aError );

    protected:  // from MHTTPTransactionCallback

        virtual void MHFRunL( RHTTPTransaction aTransaction,
                              const THTTPEvent& aEvent );

        virtual TInt MHFRunError ( TInt aError,
                                   RHTTPTransaction aTransaction,
                                   const THTTPEvent& aEvent );

    protected:  // from MHTTPDataSupplier

        virtual TBool GetNextDataPart( TPtrC8& aDataPart );

        virtual void ReleaseData();

        virtual TInt OverallDataSize();

        virtual TInt Reset();
        
    private:    // functions
    
    void ConnectL();
    
    void CreateSessionL();

    void SendUdtMessageL();
    
    void ResponseReceivedL();
   
    void CreateUdtRequestL();
    
    void ReadUdtDataL( TDes8& aTargetSerialNumber,
                       TInt& aUdtKeyVersion,
                       TDes8& aEncryptedRdbData );
                       
    HBufC* SerialNumberL();
 
    void CreateStatusNotificationL();
    
    void InstallHttpFiltersL();
    
    void SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField, const TDesC8& aHdrValue);

    void HandleResponseHeadersL( RHTTPResponse aHttpResponse );
           
    TBool CheckHttpCode( TInt aHttpStatus );

    TInt AppendResponseData( const TDesC8& aDataChunk );
    
    void TimeOut();
   
    static TInt StaticTimeOut( TAny* aPointer );
    
    void SelfComplete( TInt aResult );
    
    void Complete();

    private:    // types

        enum TState
            {
            EInit,
            EStart,
            EConnect,
            ESendMessage,
            EResponseReceived,
            EComplete
            };
            
        enum TRequestType
            {
            EUdtRequest,
            EStatusNotification
            };

    private:    // Constructors and destructor
     
        CDrmUdtHandler();
    
        void ConstructL();
        
    private:    // Data
    
        RHTTPSession iSession;
        RHTTPTransaction iTransaction;
        CDrmUdtConn* iConnection;
        CPeriodic* iTimeout;
        TState iState;
        TRequestType iRequestType;
        TRequestStatus* iParentStatus;
        TInt iError;
        TUint32 iPreferredIap;
        TInt iUdtError;

        HBufC8* iUri;
        HBufC8* iOneTimePassword;
        HBufC8* iUdtRequest;
        HBufC8* iUdtResponse;
        MDrmUdtObserver* iObserver;
        TUdtStateInfo iStateInfo;
        
    };
    
#endif  // DRM_UDT_HANDLER_H
