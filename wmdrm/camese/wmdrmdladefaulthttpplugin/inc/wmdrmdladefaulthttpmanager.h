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
* Description:  Defines the CWmDrmDlaDefaultHttpManager class
*
*/


#ifndef WMDRMDLADEFAULTHTTPMANAGER_H
#define WMDRMDLADEFAULTHTTPMANAGER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <es_sock.h>
#include <es_enum.h>
#include <in_sock.h>

#include <http/mhttpauthenticationcallback.h>
#include <http/mhttptransactioncallback.h>
#include <http/mhttpdatasupplier.h>

#include <extendedconnpref.h> // TExtendedConnPref, TConnPrefList, TConnectionInfo

#include "wmdrmdladefaulthttpmanagerobserver.h"

/**
* CWmDrmDlaDefaultHttpManager
* The HTTP communication handling class
*/
NONSHARABLE_CLASS( CWmDrmDlaDefaultHttpManager ) :
    public CActive,
    public MHTTPTransactionCallback,
    public MHTTPAuthenticationCallback
    {
    public:
        /**
         * THeader - encapsulates an HTTP header id and its value.
         */
        struct THeader
            {
        public:
            inline THeader( TInt aField, const TDesC8& aDesC ) :
                iField(aField), iVal(aDesC) {};
        public:
            TInt iField;            // e.g. 'HTTP::EAccept'
            const TDesC8& iVal;     // value: e.g. '*/*'
            };

    public:
        /**
         * Constructor.
         * @param aObserver - reference to observer
         * @return address of an instance of this class
         */
        static CWmDrmDlaDefaultHttpManager* NewL(
                    MWmDrmDlaDefaltHttpManagerObserver& aObserver );

        /**
         * Constructor.
         * @param aObserver - reference to observer
         * @param aIapNumber - Internet Access Point to be used
         * @return address of an instance of this class
         */
        static CWmDrmDlaDefaultHttpManager* NewL(
                    MWmDrmDlaDefaltHttpManagerObserver& aObserver,
                    TUint32 aIapNumber );

        /**
         * Destructor.
         */
        virtual ~CWmDrmDlaDefaultHttpManager();

        /**
         * Called to start the GET transaction using the configured IAP.
         * @param aUrl - URL to be used for the transaction
         * @param aHeaders - a list of headers and their values to be
         *                   included with the HTTP request
         */
        void Get( const TDesC8& aUrl, const RArray<THeader>& aHeaders );

        /**
         * Called to start the POST transaction using the configured IAP.
         * @param aUrl - URL to be used for the transaction
         * @param aHeaders - a list of headers and their values to be
         *                   included with the HTTP request
         * @param aDataSupplier - interface to be used to get the POST data
         */
        void Post( const TDesC8& aUrl, const RArray<THeader>& aHeaders,
                   MHTTPDataSupplier* aDataSupplier );

        /**
         * To be called to stop an ongoing transaction instead of Cancel.
         * Cancel SHOULD NOT BE CALLED by clients.
         */
        void Stop();

        /**
        * Notifies the CWmDrmDlaDefaultHttpManager that another piece of data
        * is available to be sent as the body of a request.
        * If the MHTTPDataSupplier::NextDataPart returns EFalse indicating
        * there is more data available this method needs to be called for the
        * next piece of data to be requested from the MHTTPDataSupplier.
        * @leave any of the system wide error codes.
        */
        void MoreDataAvailableL();

        /**
         * If set to true, the connection is not closed when a
         * transaction ends but when the object is deleted. Default: false.
         * @param aKeepAlive - value of the KeepAlive property
         */
        void SetKeepAlive( TBool aKeepAlive );

        /**
         * Retrieves the value of the KeepAlive property
         */
        TBool KeepAlive() const;

        /**
         * Retrieves the current connection info.
         * This method leaves with KErrNotReady if a connection
         * is not open.
         */
        void GetConnectionInfoL( TConnectionInfo& aConnectionInfo );

        /**
         * Set the IAP connection for the next Get or Post
         * @param aIapId The IAP connection that will be used
         */
        void SetIapId( TInt aIapId );

        /**
         * Get the IAP Connection ID
         */
        TInt IapId();

    private: // From MHTTPTransactionCallback

        virtual void MHFRunL( RHTTPTransaction aTransaction,
                              const THTTPEvent& aEvent );
        virtual TInt MHFRunError( TInt aError, RHTTPTransaction aTransaction,
                                  const THTTPEvent& aEvent );

    private: // From MHTTPAuthenticationCallback

        virtual TBool GetCredentialsL( const TUriC8& aURI, RString aRealm,
                                       RStringF aAuthenticationType,
                                       RString& aUsername,
                                       RString& aPassword );

    private:    // From CActive

        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);

    private:

        /**
         * Contructor
         * @param aObserver An observer to monitor the HTTP communications
         * @param aIap An IAP connection to use for HTTP communication
         */
        CWmDrmDlaDefaultHttpManager(
                MWmDrmDlaDefaltHttpManagerObserver& aObserver,
                TUint32 aIapNumber );

        /**
         * Symbian OS 2nd phase constructor.
         */
        void ConstructL();

    private:

        // State transition handlers
        /**
         * Handler for the EStart state.
         */
        void InitializeL();
        /**
         * Handler for the EInitialize state.
         */
        void OpenL();
        /**
         * Handler for the EOpenFailed state.
         */
        void ReconnectL();
        /**
         * Handler for the states EOpen and EReconnect.
         */
        void SubmitL();

        // Utility methods
        void DoStartL( const TDesC8& aUrl, const RArray<THeader>& aHeaders );
        /**
         * Close HTTP connection and clean up instance variables.
         */
        void HandleDownloadComplete( TInt aError );
        /**
         * Close HTTP transaction and clean up instance variables.
         */
        void CleanupTransaction();
        /**
         * Close connection and clean up instance variables.
         */
        void CleanupConnection();
        /**
         * Complete itself to keep state machine running
         */
        void CompleteSelf();
        /**
         * Set the HTTP header to send
         */
        void SetHeaderL( RHTTPHeaders& aHeaders, TInt aHdrField,
                         const TDesC8& aHdrValue ) const;
        /**
         * Delete the username and password used in authentication
         */
        void DeleteUsernamePassword();

    private:    // Private types

        // Asynchronous connection states
        enum TState
            {
            EConstructed,
            EStart,
            EInitialize,
            EOpen,
            EOpenFailed,
            EReconnect,
            ESubmit
            };

        // HTTP operation: GET or POST
        enum TOperation
            {
            EGet,
            EPost
            };

    private: // Data

        MWmDrmDlaDefaltHttpManagerObserver& iObserver;

        TState iState; // State of the asynch connection
        TInt iError;
        TBool iCredentialsOk;

        RSocketServ  iSocketServer;
        RConnection  iConnection;

        TConnPrefList iPrefList;
        TExtendedConnPref iExtPrefs;

        TUint32 iIapNumber;

        RHTTPSession iHttpSession;
        RHTTPTransaction iHttpTransaction;
        MHTTPDataSupplier* iBody;

        TBool iTransactionOpen;
        TBool iInCallback;

        HBufC8* iSrcAddress;
        RArray<TInt> iHdrFields;
        RArray<HBufC8*> iHdrValues;

        HBufC8* iUsername;
        HBufC8* iPassword;

        TOperation iOperation;
        MHTTPDataSupplier* iDataSupplier;   // not owned

        TBool iKeepAlive;
    };

#endif // WMDRMDLADEFAULTHTTPMANAGER_H

// End of File

