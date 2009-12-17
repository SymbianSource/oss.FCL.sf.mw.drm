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


#ifndef ROAP_HTTP_HANDLER_H
#define ROAP_HTTP_HANDLER_H

// INCLUDES
#include "RoapConnection.h"
#include "RoapResponse.h"
#include "RoapObserver.h"

#include <e32base.h>
#include <http.h>
#include <stringpool.h>
#include <http/mhttpauthenticationcallback.h>

namespace Roap
{

// CLASS DECLARATION

class CRoapHttpHandler
: public CActive,
  public MHTTPTransactionCallback,
  public MHTTPDataSupplier,
  public MHTTPAuthenticationCallback
    {
    public:     // Constructors and destructor
  
        static CRoapHttpHandler* NewL();

        virtual ~CRoapHttpHandler();

    public:     // new methods

        void CreateConnectionL( TRequestStatus* aStatus );

        void DoTransactionL
            (
            CRoapResponse& aResponse,
            TDesC8& aReqBody,
            TRequestStatus* aStatus 
            );

 
        void SetUrlL( TDesC8& aUrl );

        void SetObserver( MRoapObserver* aRoapObserver );

        void SetPreferredIap( TUint32 aPreferredIap );

    protected:  // from CActive

        virtual void DoCancel();

        virtual void RunL();

        virtual TInt RunError( TInt aError );

    protected:  // from MHTTPTransactionCallback

        virtual void MHFRunL
            ( RHTTPTransaction aTransaction, const THTTPEvent& aEvent );

        virtual TInt MHFRunError
            (
            TInt aError,
            RHTTPTransaction aTransaction,
            const THTTPEvent& aEvent
            );

    protected:  // from MHTTPDataSupplier

        virtual TBool GetNextDataPart( TPtrC8& aDataPart );

        virtual void ReleaseData();

        virtual TInt OverallDataSize();

        virtual TInt Reset();


    protected:  // from MHTTPAuthenticationCallback
    
	    virtual TBool GetCredentialsL
            (
            const TUriC8& aURI,
            RString aRealm, 
            RStringF aAuthenticationType,
            RString& aUsername, 
            RString& aPassword
            );

    private:    // types

        enum TRoapState         ///< State.
            {
            EInit,
            EStart,
            EConnect,
            EReady,
            ERequest,
            EComplete,
            ECloseSession,
            };

    private:    // Constructors and destructor
     
        CRoapHttpHandler();
    
        void ConstructL();

    protected:    // state machine parts


        void ConnectL();


        void CreateSessionL();


        void DoTransactionL();
        
        
        void SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField, const TDesC8& aHdrValue);


        void Complete();


        void InstallHttpFiltersL();


        void SelfComplete( TInt& aResult );


        void HandleResponseHeadersL( RHTTPResponse aHttpResponse );
        
        
        TBool CheckHttpCode( TInt aHttpStatus );


        TInt AppendPduData( const TDesC8& aDataChunk );
        
        
        TInt AppendMultipartData( const TDesC8& aDataChunk );
        
       
        static TInt StaticTimeOut( TAny* aPointer );
        
        
        void TimeOut();

    private:    // data

        HBufC8* iUri;
        HBufC8* iRequestBody;
        RHTTPSession iSession;
        RHTTPTransaction iTransaction;
        CRoapConnection* iConnection;
        MRoapObserver* iObserver;
        CRoapResponse* iResponse;
        HBufC8* iBoundary;
        TRequestStatus* iParentStatus;
        TUint32 iPreferredIap;
        TRoapState iState;
        TInt iError;
        CPeriodic* iTimeout;
        TInt iBytesReceived;
        TBool iReportBytes;
    };
}

#endif  // ROAP_HTTP_HANDLER
