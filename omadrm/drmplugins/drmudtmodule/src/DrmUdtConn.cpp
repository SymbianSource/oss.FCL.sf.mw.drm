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


// INCLUDE FILES

#include "DrmUdtConn.h"
#include "DrmUdtObserver.h"

#include <CommDbConnPref.h>
#include <commdb.h>
#include <cdblen.h>
#include <es_enum.h>

// #include <ApSelect.h> // for checking APs

#ifdef _DEBUG
#define LOGGING
#endif

#define LOGGING

#ifdef LOGGING
_LIT(KLogDir, "DRM");
_LIT(KLogName, "UDT.log");
#include "flogger.h"
#define LOG(string) \
	RFileLogger::Write(KLogDir, KLogName, \
		EFileLoggingModeAppend, string);
#define LOGHEX(buffer) \
	RFileLogger::HexDump(KLogDir, KLogName, \
		EFileLoggingModeAppend, _S(""), _S(""), \
		buffer.Ptr(), buffer.Length());
#else
#define LOG
#define LOGHEX        
#endif

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDrmUdtConn::NewL()
// ---------------------------------------------------------
//
CDrmUdtConn* CDrmUdtConn::NewL()
    {
    LOG( _L("CDrmUdtConn::NewL") );
    CDrmUdtConn* conn = new (ELeave) CDrmUdtConn();
    CleanupStack::PushL( conn );
    conn->ConstructL();
    CleanupStack::Pop( conn );
    return conn;
    }

// ---------------------------------------------------------
// CDrmUdtConn::~CDrmUdtConn()
// ---------------------------------------------------------
//
CDrmUdtConn::~CDrmUdtConn()
    {
    LOG( _L("CDrmUdtConn::~CDrmUdtConn") );
    Cancel();
    iConnection.Close();
    iSocketServ.Close();
    }

// ---------------------------------------------------------
// CDrmUdtConn::ConnectL()
// ---------------------------------------------------------
//
void CDrmUdtConn::ConnectL( TUint32 aIap,
                            MDrmUdtObserver* aObserver,
                            TRequestStatus* aStatus )
    { 
    LOG( _L("CDrmUdtConn::ConnectL") );
    
    iObserver = aObserver;
    
    if ( iState == EInit )
        {
        // Not connected. Attach to existing connection, or create new one if
        // allowed.
        iStatus = KErrGeneral;

        // Make this part atomic by pushing closes on the stack.
        User::LeaveIfError( iSocketServ.Connect() );
        CleanupClosePushL<RSocketServ>( iSocketServ );
        User::LeaveIfError( iConnection.Open( iSocketServ ) );
        CleanupClosePushL<RConnection>( iConnection );
                
        TConnectionInfoBuf connInfo;
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
            iConnPref.SetDialogPreference( ECommDbDialogPrefPrompt );
            iConnPref.SetBearerSet( ECommDbBearerCSD | ECommDbBearerWcdma );
            // New connection is always created with user-selected AP;
            // so 0 is used instead of aIap.
            iConnPref.SetIapId( 0 );
            iConnection.Start( iConnPref, iStatus );
#endif
            if ( iObserver )
                {
                iObserver->ConnectionStartedL();
                }

            iState = EConnecting;
            SetActive();    // The only path with a real async request.
            }
        CleanupStack::Pop( 2 ); // closing iConn and iSockServ
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

// ---------------------------------------------------------
// CDrmUdtConn::Close()
// ---------------------------------------------------------
//
void CDrmUdtConn::Close()
    {
    LOG( _L("CDrmUdtConn::Close") );
    Cancel();
    iConnection.Close();
    iSocketServ.Close();
    iState = EInit;
    }

// ---------------------------------------------------------
// CDrmUdtConn::IsConnected()
// ---------------------------------------------------------
//
TBool CDrmUdtConn::IsConnected( TUint32& aIap )
    {
    LOG( _L("CDrmUdtConn::IsConnected") );
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

// ---------------------------------------------------------
// CDrmUdtConn::CConnection()
// ---------------------------------------------------------
//
CDrmUdtConn::CDrmUdtConn()
: CActive( CActive::EPriorityStandard ),
  iState( EInit )
    {
    LOG( _L("CDrmUdtConn::CDrmUdtConn") );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CDrmUdtConn::ConstructL()
// ---------------------------------------------------------
//
void CDrmUdtConn::ConstructL()
    {
    LOG( _L("CDrmUdtConn::ConstructL") );
    /*
    TUint32 APs( 0 );
    CCommsDatabase* commsdb = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL( commsdb );
    CApSelect* apSel = CApSelect::NewLC (
        *commsdb,
        KEApIspTypeAll,
        EApBearerTypeAll,
        KEApSortUidAscending );
    APs = apSel->Count();
    CleanupStack::PopAndDestroy( 2 );
    if ( !APs )
        {
        // No AP defined
        User::Leave( KErrRoapGeneral );
        }
     */
    }

// ---------------------------------------------------------
// CDrmUdtConn::DoCancel()
// ---------------------------------------------------------
//
void CDrmUdtConn::DoCancel()
    {
    LOG( _L("CDrmUdtConn::DoCancel") );
    iConnection.Close();
    iSocketServ.Close();
    User::RequestComplete( iParentStatus, KErrCancel );
    }

// ---------------------------------------------------------
// CDrmUdtConn::RunL()
// ---------------------------------------------------------
//
void CDrmUdtConn::RunL()
    {
    LOG( _L("CDrmUdtConn::RunL") );
    User::LeaveIfError( iStatus.Int() );    // Handle errors in RunError().

    iState = EConnected;
    User::RequestComplete( iParentStatus, iStatus.Int() );
    iParentStatus = NULL;
    }

// ---------------------------------------------------------
// CDrmUdtConn::RunError()
// ---------------------------------------------------------
//
TInt CDrmUdtConn::RunError( TInt  /*aError*/ )
    {
    LOG( _L("CDrmUdtConn::RunError") );
    iConnection.Close();
    iSocketServ.Close();
    iState = EInit;
    User::RequestComplete( iParentStatus, iStatus.Int() );
    iParentStatus = NULL;
    return KErrNone;
    }

// ---------------------------------------------------------
// CRoapConnection::Conn()
// ---------------------------------------------------------
//
RSocketServ& CDrmUdtConn::SocketServ()
    {
    LOG( _L("CDrmUdtConn::SocketServ") );
    return iSocketServ;
    }

// ---------------------------------------------------------
// CRoapConnection::Conn()
// ---------------------------------------------------------
//
RConnection& CDrmUdtConn::Conn()
    {
    LOG( _L("CDrmUdtConn::Conn") );
    return iConnection;
    }

