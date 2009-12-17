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


#ifndef DRM_UDT_CONN_H
#define DRM_UDT_CONN_H

// INCLUDES

#include <e32base.h>
#include <es_sock.h>
#include <CommDbConnPref.h>


// FORWARD DECLARATIONS
class MDrmUdtObserver;

// CLASS DECLARATION

class CDrmUdtConn : public CActive
    {
    public:     // Constructors and destructor.

        static CDrmUdtConn* NewL();

        CDrmUdtConn();

        ~CDrmUdtConn();

    public:     // new methods

        void ConnectL( TUint32 aIap,
                       MDrmUdtObserver* aObserver,
                       TRequestStatus* aStatus );

        void Close();

        TBool IsConnected( TUint32& aIap );

        RSocketServ& SocketServ();

        RConnection& Conn();

    private:

        enum TState
            {
            EInit,
            EConnecting,
            EConnected
            };

    private:    // Constructors and destructor.

        void ConstructL();

    private:  // from CActive

        virtual void DoCancel();

        virtual void RunL();

        virtual TInt RunError( TInt aError );

    private:    // new methods

        void DoClose();

        void Done();

    private:    // data

        RSocketServ iSocketServ;
        RConnection iConnection;
        TState iState;
        TRequestStatus* iParentStatus;
        TCommDbConnPref iConnPref;
        MDrmUdtObserver* iObserver;
    };

#endif /* def CONNECTION_H */
