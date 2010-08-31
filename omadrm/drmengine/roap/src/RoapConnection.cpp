/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Connection manager for ROAP
*
*/


// INCLUDE FILES
#include <cmconnectionmethod.h>
#include <cmdestination.h>
#include <cmconnectionmethoddef.h>
#include <cmmanager.h>
#include <centralrepository.h>
#include <extendedconnpref.h>
#include <cdblen.h>
#include <cdbcols.h> // IAP, COMMDB_ID
#include <es_enum.h>
#include "RoapConnection.h"
#include "RoapDef.h"
#include "RoapLog.h"



// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// Roap::CRoapConnection::NewL
// ---------------------------------------------------------------------------
//
Roap::CRoapConnection* Roap::CRoapConnection::NewL()
    {
    CRoapConnection* conn = new (ELeave) CRoapConnection();
    CleanupStack::PushL( conn );
    conn->ConstructL();
    CleanupStack::Pop( conn );
    return conn;
    }

// ---------------------------------------------------------------------------
// Roap::CRoapConnection::~CRoapConnection
// ---------------------------------------------------------------------------
//
Roap::CRoapConnection::~CRoapConnection()
    {
    Cancel();
    iConnection.Close();
    iSocketServ.Close();
    }

// ---------------------------------------------------------------------------
// Roap::CRoapConnection::ConnectL
// ---------------------------------------------------------------------------
//
void Roap::CRoapConnection::ConnectL
( TUint32 aIap, TRequestStatus* aStatus )
    {
    LOGLIT( "CRoapConnection::ConnectL" )

    if ( iState == EInit )
        {
        // Not connected. Attach to existing connection, or create new one if
        // allowed.
        iStatus = KErrRoapGeneral;

        // Make this part atomic by pushing closes on the stack.
        User::LeaveIfError( iSocketServ.Connect() );
        CleanupClosePushL<RSocketServ>( iSocketServ );
        User::LeaveIfError( iConnection.Open( iSocketServ ) );
        CleanupClosePushL<RConnection>( iConnection );

        TConnectionInfoBuf connInfo;
        TUint count(0);
        User::LeaveIfError( iConnection.EnumerateConnections( count ) );
        TUint i(0);
        if ( count )
            {
            // Select from existing connections. Try to make AP match.
            for ( i = count; i; i-- )
                {
                // Note: GetConnectionInfo expects 1-based index.
                User::LeaveIfError( iConnection.GetConnectionInfo( i, connInfo ) );
                if ( aIap == 0 || connInfo().iIapId == aIap )
                    {
                    // "Accept any" or AP match. Attach to this one.
                    break;
                    }
                }
            if ( !i )
                {
                // No AP match, select AP with largest index.
                User::LeaveIfError
                    ( iConnection.GetConnectionInfo( count, connInfo ) );
                }
            User::LeaveIfError
                ( iConnection.Attach( connInfo, RConnection::EAttachTypeNormal ) );
            iState = EConnected;
            iStatus = KErrNone;
            }
        else
            {
            // No existing connections, create new one.
            // Create overrides
            TConnPrefList prefList;
            TExtendedConnPref prefs;

            prefs.SetNoteBehaviour(TExtendedConnPref::ENoteBehaviourConnSilent);

            //Fetch default connection
            TBool hasDefault(ETrue);
            TCmDefConnValue defConn;
            RCmManager cmManager;
            cmManager.OpenLC();
            cmManager.ReadDefConnL(defConn);
            if (defConn.iType == ECmDefConnConnectionMethod)
                {
                prefs.SetIapId(defConn.iId);
                }
            else if (defConn.iType == ECmDefConnDestination)
                {
                prefs.SetSnapId(defConn.iId);
                }
            else
                {
                hasDefault = EFalse;
                }
            // End of fetch default connection
            if (hasDefault)
                {
                prefList.AppendL(&prefs);

                // Start an Outgoing Connection with overrides
                iConnection.Start(prefList, iStatus);
                }
            else
                {
                // No default found --> trying with query
                iConnection.Start(iStatus);
                }
            CleanupStack::PopAndDestroy(&cmManager);
            iState = EConnecting;
            SetActive();    // The only path with a real async request.
            }
        CleanupStack::Pop( 2, &iSocketServ ); // closing iConn and iSockServ
        // End of atomic part.
        }
    else
        {
        // Not expecting this to be called in other states.
        }

    iParentStatus = aStatus;
    *iParentStatus = KRequestPending;

    if ( !IsActive() )
        {
        // Unless we have an outstanding connect request (iConn.Start),
        // we are done.
        User::RequestComplete( iParentStatus, iStatus.Int() );
        iParentStatus = NULL;
        }
    }

// ---------------------------------------------------------------------------
// Roap::CRoapConnection::Close
// ---------------------------------------------------------------------------
//
void Roap::CRoapConnection::Close()
    {
    LOGLIT( "CRoapConnection::Close" )

    Cancel();
    iConnection.Close();
    iSocketServ.Close();
    iState = EInit;
    }

// ---------------------------------------------------------------------------
// Roap::CRoapConnection::IsConnected
// ---------------------------------------------------------------------------
//
TBool Roap::CRoapConnection::IsConnected( TUint32& aIap )
    {
    LOGLIT( "CRoapConnection::IsConnected" )

    TBool connected( EFalse );
    if( iState == EConnected )
        {
        TBuf<KCommsDbSvrMaxColumnNameLength * 2 + 1> iapId;
        _LIT( KFormatIapId, "%S\\%S" );
        TPtrC iap( IAP );
        TPtrC id( COMMDB_ID );
        iapId.Format( KFormatIapId, &iap, &id );
        TInt err = iConnection.GetIntSetting( iapId, aIap );
        connected = err ? EFalse : ETrue;
        }
    return connected;
    }

// ---------------------------------------------------------------------------
// Roap::CRoapConnection::CRoapConnection
// ---------------------------------------------------------------------------
//
Roap::CRoapConnection::CRoapConnection()
: CActive( CActive::EPriorityStandard ),
  iState( EInit )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Roap::CRoapConnection::ConstructL
// ---------------------------------------------------------------------------
//
void Roap::CRoapConnection::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// Roap::CRoapConnection::DoCancel
// ---------------------------------------------------------------------------
//
void Roap::CRoapConnection::DoCancel()
    {
    LOGLIT( "CRoapConnection::DoCancel" )

    iConnection.Close();
    iSocketServ.Close();
    User::RequestComplete( iParentStatus, KErrCancel );
    }

// ---------------------------------------------------------------------------
// Roap::CRoapConnection::RunL
// ---------------------------------------------------------------------------
//
void Roap::CRoapConnection::RunL()
    {
    LOGLIT( "CCRoapConnection::RunL" )

    User::LeaveIfError( iStatus.Int() );    // Handle errors in RunError().

    iState = EConnected;
    User::RequestComplete( iParentStatus, iStatus.Int() );
    iParentStatus = NULL;
    }

// ---------------------------------------------------------------------------
// Roap::CRoapConnection::RunError
// ---------------------------------------------------------------------------
//
TInt Roap::CRoapConnection::RunError( TInt  /* aError */ )
    {
    LOGLIT( "CRoapConnection::RunError" )

    iConnection.Close();
    iSocketServ.Close();
    iState = EInit;
    User::RequestComplete( iParentStatus, iStatus.Int() );
    iParentStatus = NULL;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Roap::CRoapConnection::SocketServ
// ---------------------------------------------------------------------------
//
RSocketServ& Roap::CRoapConnection::SocketServ()
    {
    return iSocketServ;
    }

// ---------------------------------------------------------------------------
// Roap::CRoapConnection::Conn
// ---------------------------------------------------------------------------
//
RConnection& Roap::CRoapConnection::Conn()
    {
    return iConnection;
    }

