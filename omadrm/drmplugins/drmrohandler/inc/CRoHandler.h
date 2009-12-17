/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ECOM plugin for receiving OMA Rights Objects
*
*/


#ifndef CROHANDLER_H
#define CROHANDLER_H

//  INCLUDES
#include <mtclreg.h>                // for CClientMtmRegistry
#include "RoapEngBase.h"
#include "wbxmlroaptriggerparser.h"

// CONSTANTS
const TUid KUidMsgTypeRO = {0x101F6DC2};
const TInt KWait = 5*60*1000000;  // Timeout value to be used for waiting in "disk full" situation

enum TMessageType
    {
    EOma1Ro,
    EOma2RoapPdu,
    EOma2RoapTrigger,
    EOma2RoapTriggerRoAcquisition,
    EOma2RoapTriggerMetering
    };

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CDRMMessageParser;


// CLASS DECLARATION

// DESCRIPTION
//  ECOM listener plugin for DRM server.
//  Plugin is instantiated by the wap listener in the system watcher.
//  It delivers rights object to DRM server.

class CRoHandler
        : public CPushHandlerBase , public MMsvSessionObserver
    {
    public:  // Constructors and destructor

        /**
        * Destructor
        */
        ~CRoHandler();

        /**
        * Default 1st phase factory method.
        * returns created instance of the CRoHandler class
        */
        static CRoHandler* NewL();

        /**
        * Method for handling a received message asynchronously.
        * param aPushMsg object representing the received push message
        * param aStatus Status of the asynchronous call
        */
        void HandleMessageL( CPushMessage* aPushMsg, TRequestStatus& aStatus );

        /**
        * Method for handling a received message synchronously.
        * param aPushMsg object representing the received push message
        */
        void HandleMessageL( CPushMessage* aPushMsg );

        /**
        * Cancels the requested asynchronous message handling.
        */
        void CancelHandleMessage();

    protected:  // Functions from base classes

        /**
        * Performs operations needed for cancelling the message handling.
        * Called by the Active Scheduler after user calling Cancel().
        */
        void DoCancel();

        /**
        * Performs the actual tasks related to message handling.
        * Called by the Active Scheduler.
        */
        void RunL();

        /**
        * Performs error handling tasks in case RunL() left.
        * Called by the Active Scheduler.
        * Currently does nothing.
        * param aError specifies the error code related to RunL()'s leave.
        * return error value after error handling.
        */
        TInt RunError( TInt aError );

    private:    // New functions

        /**
        * Default constructor.
        */
        CRoHandler();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Performs basic sanity checks for the received message.
        * Only the body is checked since header is not of interest to us.
        * @return standard error code
        */
        TInt PerformChecks();

        TMessageType CheckMessageTypeL();

        void HandleRightsMessageL();

        void HandleRoapPduL();

        void HandleRoapTriggerL();

        void HandleMeteringTriggerSilentlyL();


        /**
        * Creates new empty message.
        */
        TMsvId CreateNewMessageL();

        /**
        * Set message entry.
        */
        void SetEntryL( TMsvId aEntryId );

        /**
        * Moves message entry.
        */
        TMsvId MoveMessageEntryL( TMsvId aTarget );

        /**
        * Adds content to the message.
        */
        void AddRoContentL( TDesC& aMessageContent );

    private:    // Functions from base classes

        /**
        * Reserved for future expansion.
        */
        void CPushHandlerBase_Reserved1();

        /**
        * Reserved for future expansion.
        */
        void CPushHandlerBase_Reserved2();

        HBufC* ConvertDetailsL( const TDesC8& aFrom );

        HBufC* GetDetailLC();

        HBufC* GetDescriptionLC();

        void ReadFromResourceLC(
            const TDesC& aFile ,
            const TInt& aIndex ,
            HBufC*& aBuf );

        HBufC* ConvertUriToDisplayFormL( const TDesC& aUri );

        /**
         * Do a synchronous ROAP request.
         *
         * @since S60 3.1
         * @param aTrigger ROAP Trigger
         * @return ETrue if the ROAP could be started silently, EFalse otherwise (e.g. when
         *         no RI context exists)
         */
        TBool DoRoapL( const TDesC8& aTrigger );

    private: // from MMsvSessionObserver

        /**
        * Handles session events.
        */
        void HandleSessionEventL( TMsvSessionEvent aEvent,
                                  TAny* aArg1, TAny* aArg2,
                                  TAny* aArg3 );

        enum TState
            {
            ERunning,
            EFileSystemWait
            };

        // internal state
        TState iState;
        // message type
        TMessageType iMsgType;
        // flag to handle message one time.
        TBool iFirstTime;
        // uid of the ro handler
        enum { RoHandlerUid = 0x101F7B92 };
        // Pushed message
        CPushMessage* iPushMsg;
        // The body of the pushed message as HBufC8 descriptor
        // api to store the OMA 1 RO
        CDRMMessageParser* iMessageParser;
        // pointer to message body
        TPtrC8 iMessageBodyPtr;
        CMsvSession* iSession;          // Client session on the message server
        CBaseMtm* iMtm;                 // Message Type Module (sms)
        CClientMtmRegistry* iMtmReg;    // Mtm client registry for creating new mtms
        TMsvId iMsvId;                  // message server entry id
        RFs iFs;
        Roap::CRoapEngBase* iRoapHandler;

        TBool iMeteringSupported;       // state variable showing whether
                                        // metering is supported or not
                                        // initialised at instantiation
        DRM::CWbxmlRoapTriggerParser* iWbxmlTriggerParser;
        HBufC8* iParsedXmlTrigger;
        TBool iPutRightsToInbox;         // whether received RO is put into Inbox as an
                                        // entry or not. Initialized at instantiation.
                                        
    };

#endif CROHANDLER_H

// End of File
