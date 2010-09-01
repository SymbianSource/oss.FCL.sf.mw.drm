/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <commdbconnpref.h>
#include <cdblen.h>
#include <es_enum.h>
#ifdef __SERIES60_NATIVE_BROWSER
#include <browseruisdkcrkeys.h>
#endif
#include "RoapConnection.h"
#include "RoapDef.h"
#include "RoapLog.h"



#ifndef __SERIES60_NATIVE_BROWSER
    const TUid KCRUidBrowser   = {0x10008D39};
    const TUint32 KBrowserDefaultAccessPoint =  0x0000000E;
    const TUint32 KBrowserAccessPointSelectionMode = 0x0000001E;
    const TUint32 KBrowserNGDefaultSnapId = 0x00000053;
#endif


// ================= LOCAL FUNCTIONS =========================================
// ---------------------------------------------------------------------------
// IapIdOfDefaultSnapL
// for trapping purposes only
// ---------------------------------------------------------------------------
//
LOCAL_C TUint32 IapIdOfDefaultSnapL(
    RCmManager& aCmManager,
    const TUint32 aDefaultSnap )
    {
    RCmDestination dest( aCmManager.DestinationL( aDefaultSnap ) );
    CleanupClosePushL( dest );
    TUint32 iapIdOfDest( 0 );

    if ( dest.ConnectionMethodCount() <= 0 )
        {
        User::Leave( KErrNotFound );
        }

    RCmConnectionMethod cMeth( dest.ConnectionMethodL( 0 ) );
    CleanupClosePushL( cMeth );

    iapIdOfDest = cMeth.GetIntAttributeL( CMManager::ECmIapId );
    CleanupStack::PopAndDestroy( &cMeth );
    CleanupStack::PopAndDestroy( &dest );
    return iapIdOfDest;
    }

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
    const TInt KAlwaysAskSelectionMode( 1 );
    const TInt KDestinationSelectionMode( 2 );

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
        TInt ap = 0;
        TInt alwaysAsk = 0;
        TUint count;
        User::LeaveIfError( iConnection.EnumerateConnections( count ) );
        TUint i;
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
#ifdef __WINS__
            // WINS connection creation does not work if preferences are given.
            // Defaults are to be used always.
            iConnection.Start( iStatus );
#else
            // Note: the TCommDbConnPref must NOT be stack variable.
            // It must persist until completion of RConnection::Start().
            iConnPref.SetDirection( ECommDbConnectionDirectionOutgoing );
            //iConnPref.SetDialogPreference( ECommDbDialogPrefWarn )
            iConnPref.SetBearerSet( ECommDbBearerCSD | ECommDbBearerWcdma );
            // New connection is always created with user-selected AP
            // so 0 is used instead of aIap.

            TInt defaultSnap( 0 );
            CRepository* repository( NULL );
            repository = CRepository::NewL( KCRUidBrowser );
            CleanupStack::PushL( repository );

            repository->Get( KBrowserDefaultAccessPoint, ap);
            repository->Get( KBrowserAccessPointSelectionMode, alwaysAsk );
            repository->Get( KBrowserNGDefaultSnapId, defaultSnap );
            CleanupStack::PopAndDestroy( repository );
            repository = NULL;

            TUint32 iapd32 = 0;
            TInt err = KErrNone;

            if ( ap <= KErrNotFound && defaultSnap <= KErrNotFound )
                {
                alwaysAsk = KAlwaysAskSelectionMode;
                }
            else
                {
                RCmManager cmManager;
                cmManager.OpenLC();
                if ( !alwaysAsk )
                    {
                    TRAP( err, iapd32 = cmManager.GetConnectionMethodInfoIntL(
                            ap, CMManager::ECmIapId ) );
                    }
                else if ( alwaysAsk == KDestinationSelectionMode )
                    {
                    TRAP( err, iapd32 = IapIdOfDefaultSnapL(
                            cmManager, defaultSnap ) );
                    }
                CleanupStack::PopAndDestroy( &cmManager );
                }

            if ( err || alwaysAsk == KAlwaysAskSelectionMode )
                {
                // Always ask
                LOGLIT( "SetDialogPreference( ECommDbDialogPrefPrompt )" )
                iConnPref.SetDialogPreference( ECommDbDialogPrefPrompt );
                iapd32 = 0;
                }
            else
                {
                // User defined
                LOGLIT( "SetDialogPreference( ECommDbDialogPrefDoNotPrompt )" )
                iConnPref.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
                }

            iConnPref.SetIapId( iapd32 );
            iConnection.Start( iConnPref, iStatus );
#endif
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

