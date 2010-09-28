/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Connection manager for DRM
 *
 */

// INCLUDE FILES
#include <es_sock.h>
#include <cmconnectionmethod.h>
#include <cmdestination.h>
#include <cmconnectionmethoddef.h>
#include <cmmanager.h>
#include <extendedconnpref.h>
#include <es_enum.h>
#include <cdbcols.h> // IAP, COMMDB_ID
#include "drmutilityconnection.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "drmutilityconnectionTraces.h"
#endif

// ================= MEMBER FUNCTIONS =======================


// -----------------------------------------------------------------------------
// HasDefaultConnectionL
// Finds default IAP id
// @return Etrue: valid AP found
//         EFalse: valid AP not found
// @leave system wide error codes
// -----------------------------------------------------------------------------
//
TBool DRM::CDrmUtilityConnection::HasDefaultConnectionL()
    {
    TBool hasDefault(EFalse);
    TCmDefConnValue defConn;
    RCmManager cmManager;
    cmManager.OpenLC();
    cmManager.ReadDefConnL(defConn);
    if (defConn.iType == ECmDefConnConnectionMethod)
        {
        cmManager.GetConnectionMethodInfoIntL(defConn.iId,
                CMManager::ECmIapId);
        hasDefault = ETrue;
        }
    else if (defConn.iType == ECmDefConnDestination)
        {
        RCmDestination dest(cmManager.DestinationL(defConn.iId));
        CleanupClosePushL(dest);

        if (dest.ConnectionMethodCount() <= 0)
            {
            User::Leave(KErrNotFound);
            }

        RCmConnectionMethod cMeth(dest.ConnectionMethodL(0));
        CleanupClosePushL(cMeth);

        cMeth.GetIntAttributeL(CMManager::ECmIapId);
        CleanupStack::PopAndDestroy(&cMeth);
        CleanupStack::PopAndDestroy(&dest);
        hasDefault = ETrue;
        }
    CleanupStack::PopAndDestroy(&cmManager);
    return hasDefault;
    }

// -----------------------------------------------------------------------------
// CDrmUtilityConnection::HasAccessPointsL
// -----------------------------------------------------------------------------
//
TBool DRM::CDrmUtilityConnection::HasAccessPointsL()
    {
    TInt apCount(0);
    RCmManager cmManager;
    CleanupClosePushL(cmManager);
    cmManager.OpenL();
    RArray<TUint32> aps;
    CleanupClosePushL(aps);
    cmManager.ConnectionMethodL(aps, EFalse, EFalse, ETrue);
    apCount = aps.Count();
    CleanupStack::PopAndDestroy(2, &cmManager); //aps, cmManager
    return apCount > 0;
    }

// ---------------------------------------------------------------------------
// DRM::CDrmUtilityConnection::NewLC
// ---------------------------------------------------------------------------
//
DRM::CDrmUtilityConnection* DRM::CDrmUtilityConnection::NewLC(
        const TBool aAllowQueries)
    {
    CDrmUtilityConnection* conn = new (ELeave) CDrmUtilityConnection(
            aAllowQueries);
    CleanupStack::PushL(conn);
    conn->ConstructL();
    return conn;
    }
// ---------------------------------------------------------------------------
// DRM::CDrmUtilityConnection::NewL
// ---------------------------------------------------------------------------
//
DRM::CDrmUtilityConnection* DRM::CDrmUtilityConnection::NewL(
        const TBool aAllowQueries)
    {
    CDrmUtilityConnection* conn = CDrmUtilityConnection::NewLC(aAllowQueries);
    CleanupStack::Pop(conn);
    return conn;
    }

// ---------------------------------------------------------------------------
// DRM::CDrmUtilityConnection::~CDrmUtilityConnection
// ---------------------------------------------------------------------------
//
DRM::CDrmUtilityConnection::~CDrmUtilityConnection()
    {
    Cancel();
    iConnection.Close();
    iSocketServ.Close();
    }

// ---------------------------------------------------------------------------
// DRM::CDrmUtilityConnection::ConnectL
// ---------------------------------------------------------------------------
//
void DRM::CDrmUtilityConnection::ConnectL(TRequestStatus* aStatus)
    {
    TInt error(KErrGeneral);
    iStatus = KRequestPending;

    if (iState == EInit)
        {

        // Make this part atomic by pushing closes on the stack.
        error = iSocketServ.Connect();
        if (!error)
            {
            error = iConnection.Open(iSocketServ);
            }
        }

    if (iState == EConnected)
        {
        error=iConnection.Stop();
        //error=iConnection.Stop(RConnection::EStopAuthoritative); //just in case still does not work
        // if autoriative stop does not work then must do something following follow
        // TNifProgressBuf iJalla;
        // loop iConnection.ProgressNotification(iJalla,iStatus,KConnProgressDefault); SetActive
        // at RunL if (iJalla.iStage < KConnectionDown)
        // reactiveteWaitClose();
        //iConnection.ProgressNotification(iJalla,iStatus,(KConnectionDown); (should loop till state KConnectionDown seen)
        iConnection.Close();
        iState = EInit;
        error = KErrNone;
        error = iConnection.Open(iSocketServ);
        }

    iParentStatus = aStatus;
    *iParentStatus = KRequestPending;

    CompleteSelf(error);
    }

// ---------------------------------------------------------------------------
// DRM::CDrmUtilityConnection::HasMoreConnectionAttempts
// ---------------------------------------------------------------------------
//
TBool DRM::CDrmUtilityConnection::HasMoreConnectionAttempts()
    {
    TBool ret(EFalse);
    switch (iType)
        {
        case EUsingPurposeInternet:
            if (iAllowQueries)
                {
                ret = ETrue;
                }
            else
                {
                ret = EFalse;
                }
            break;
        case EUsingQuery:
        case EFail:
            ret = EFalse;
            break;
        default:
            ret = ETrue;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// DRM::CDrmUtilityConnection::IsConnected
// ---------------------------------------------------------------------------
//
TBool DRM::CDrmUtilityConnection::IsConnected(TUint32& aIap)
    {
    TBool ret(EFalse);
    
    if (iState == EConnected)
        {
        aIap = iIapId;
        ret = ETrue;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// DRM::CDrmUtilityConnection::Close
// ---------------------------------------------------------------------------
//
void DRM::CDrmUtilityConnection::Close()
    {
    //LOGLIT( "CDrmUtilityConnection::Close" )

    Cancel();
    iConnection.Close();
    iSocketServ.Close();
    iState = EInit;
    }

// ---------------------------------------------------------------------------
// DRM::CDrmUtilityConnection:CDrmUtilityConnection:
// ---------------------------------------------------------------------------
//
DRM::CDrmUtilityConnection::CDrmUtilityConnection(const TBool aAllowQueries) :
    CActive(CActive::EPriorityStandard), iState(EInit),
            iType(EAttachExisting), iAllowQueries(aAllowQueries)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// DRM::CDrmUtilityConnection::ConstructL
// ---------------------------------------------------------------------------
//
void DRM::CDrmUtilityConnection::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// DRM::CDrmUtilityConnection::DoCancel
// ---------------------------------------------------------------------------
//
void DRM::CDrmUtilityConnection::DoCancel()
    {
    iConnection.Close();
    iSocketServ.Close();
    CompleteParent(KErrCancel);
    }

// ---------------------------------------------------------------------------
// DRM::CDrmUtilityConnection::RunL
// ---------------------------------------------------------------------------
//
void DRM::CDrmUtilityConnection::RunL()
    {
    TInt error(iStatus.Int());
    if (iState == EInit)
        {
        User::LeaveIfError(error); // Handle errors in RunError().
        if (iType == EAttachExisting)
            {
            AttachExistingConnectionL();
            }
        else
            {
            iState = EConnecting;
            error = KErrNotFound;
            CompleteSelf(error);
            }
        }
    else if (iState == EConnecting)
        {
        if (!error)
            {
            iState = EConnected;
            CompleteSelf(error);
            }
        else if (error != KErrNotFound)
            {
            User::LeaveIfError(error);
            }
        else
            {
            switch (iType)
                {
                case EAttachExisting:
                    iType = EUsingDefault;
                    ConnectUsingDefaultL();
                    break;
                case EUsingDefault:
                    iType = EUsingPurposeOperator;
                    ConnectUsingSnapPurposeL(CMManager::ESnapPurposeOperator);
                    break;
                case EUsingPurposeOperator:
                    iType = EUsingPurposeInternet;
                    ConnectUsingSnapPurposeL(CMManager::ESnapPurposeInternet);
                    break;
                case EUsingPurposeInternet:
                    iType = EUsingQuery;
                    if (iAllowQueries)
                        {
                        ConnectUsingQueryL();
                        break;
                        }
                case EUsingQuery:
                    iType = EFail;
                case EFail:
                default:
                    User::Leave(KErrCouldNotConnect);
                    break;
                }
            }
        }
    else if (iState == EConnected)
        {
        UpdateIapIdL();
        CompleteParent(iStatus.Int());
        }
    }

// ---------------------------------------------------------------------------
// DRM::CDrmUtilityConnection::RunError
// ---------------------------------------------------------------------------
//
TInt DRM::CDrmUtilityConnection::RunError(TInt aError)
    {
    //LOGLIT( "CDrmUtilityConnection::RunError" )

    iConnection.Close();
    iSocketServ.Close();

    CompleteParent(aError);
    iState = EInit;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// DRM::CDrmUtilityConnection::AttachExistingConnectionL
// ---------------------------------------------------------------------------
//
void DRM::CDrmUtilityConnection::AttachExistingConnectionL()
    {
    TConnectionInfoBuf connInfo;
    TUint count(0);
    TInt error(KErrNotFound);
    iStatus = KRequestPending;

    User::LeaveIfError(iConnection.EnumerateConnections(count));
    if (count)
        {
        // Select AP with largest index. (Latest created)
        User::LeaveIfError(iConnection.GetConnectionInfo(count, connInfo));
        User::LeaveIfError(iConnection.Attach(connInfo,
                RConnection::EAttachTypeNormal));
        iState = EConnected;
        error = KErrNone;
        }
    else
        {
        iState = EConnecting;
        }
    CompleteSelf(error);
    }

// ---------------------------------------------------------------------------
// DRM::CDrmUtilityConnection::ConnectUsingDefaultL
// ---------------------------------------------------------------------------
//
void DRM::CDrmUtilityConnection::ConnectUsingDefaultL()
    {
    // Create overrides
    TConnPrefList prefList;
    TExtendedConnPref prefs;

    if (!iAllowQueries)
        {
        prefs.SetNoteBehaviour(TExtendedConnPref::ENoteBehaviourConnSilent);
        }
    else
        {
        prefs.SetNoteBehaviour(TExtendedConnPref::ENoteBehaviourConnDisableQueries);
        }
        
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
    CleanupStack::PopAndDestroy(&cmManager);
    if (hasDefault)
        {
        prefList.AppendL(&prefs);

        // Start an Outgoing Connection with overrides
        iState = EConnecting;
        iConnection.Start(prefList, iStatus);
        SetActive();
        }
    else
        {
        // No default found --> next iteration needed
        CompleteSelf(KErrNotFound);
        }
    }

// ---------------------------------------------------------------------------
// DRM::CDrmUtilityConnection::ConnectUsingSnapOperatorL
// ---------------------------------------------------------------------------
//
void DRM::CDrmUtilityConnection::ConnectUsingSnapPurposeL(
        const TUint32 aPurpose)
    {
    // Create overrides
    TConnPrefList prefList;
    TExtendedConnPref prefs;
    prefs.SetSnapPurpose(static_cast<CMManager::TSnapPurpose> (aPurpose));
    if (aPurpose != CMManager::ESnapPurposeUnknown)
        {
        if (!iAllowQueries)
            {
            prefs.SetNoteBehaviour(
                    TExtendedConnPref::ENoteBehaviourConnSilent);
            }
        else
            {
            prefs.SetNoteBehaviour(
                    TExtendedConnPref::ENoteBehaviourConnDisableQueries);
            }
        }
    else
        {
        prefs.SetConnSelectionDialog(ETrue);
        prefs.SetNoteBehaviour(TExtendedConnPref::ENoteBehaviourDefault);
        prefs.SetForcedRoaming(EFalse);
        prefs.SetBearerSet(TExtendedConnPref::EExtendedConnBearerUnknown);
        }
    prefList.AppendL(&prefs);
    iState = EConnecting;

    // Start an Outgoing Connection with overrides
    iConnection.Start(prefList, iStatus);
    SetActive();
    }

// ---------------------------------------------------------------------------
// DRM::CDrmUtilityConnection::ConnectUsingQueryL
// ---------------------------------------------------------------------------
//
void DRM::CDrmUtilityConnection::ConnectUsingQueryL()
    {
    // Implement according to the new Connection Settings
    // QT API when possible to test the implementation
    CompleteSelf(KErrCancel);
    }
// ---------------------------------------------------------------------------
// DRM::CDrmUtilityConnection::CompleteSelf
// ---------------------------------------------------------------------------
//
void DRM::CDrmUtilityConnection::CompleteSelf(TInt aError)
    {
    TRequestStatus* status(&iStatus);
    User::RequestComplete(status, aError);
    SetActive();
    }

// ---------------------------------------------------------------------------
// DRM::CDrmUtilityConnection::CompleteParent
// ---------------------------------------------------------------------------
//
void DRM::CDrmUtilityConnection::CompleteParent(TInt aError)
    {
    if (iParentStatus)
        {
        User::RequestComplete(iParentStatus, aError);
        iParentStatus = NULL;
        }
    }

// ---------------------------------------------------------------------------
// DRM::CDrmUtilityConnection::UpdateIapId
// ---------------------------------------------------------------------------
//
void DRM::CDrmUtilityConnection::UpdateIapIdL()
    {
    TBuf<16> query;

    query.Format(_L("%s\\%s"), IAP, COMMDB_ID);
    User::LeaveIfError(iConnection.GetIntSetting(query, iIapId));
    
    }
