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
* Description:  core ROAP engine
*
*/


// INCLUDE FILES
#include <DRMRights.h>
#include <DRMRightsClient.h>
// #include <GenericParam.h> // from S60
#include "RoapEngBase.h"
#include "RoapHttpHandler.h"
#include "RoapTrigger.h"
#include "RoapDef.h"
#include "RoapMessage.h"
#include "RoapLog.h"
#include "RoapObserver.h"
#include "DRMRIContext.h"
#include "RoapStorageClient.h"

using namespace Roap;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CRoapEngBase::~CRoapEngBase()
// ---------------------------------------------------------
//
EXPORT_C CRoapEngBase::~CRoapEngBase()
    {
    Cancel();
    delete iTrigger;
    delete iRequest;
    delete iResponse;
    delete iRoapResp;
    delete iHttpHandler;
    delete iStoredRiContext;
    iReturnedROs.ResetAndDestroy();
    delete iDomainRightsResp;
    delete iRiAlias;
    }

// ---------------------------------------------------------
// CRoapEngBase::CRoapEngBase()
// ---------------------------------------------------------
//
CRoapEngBase::CRoapEngBase( ): CActive( CActive::EPriorityStandard ),
    iPreferredIap(0)
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CRoapEngBase::ConstructL()
// ---------------------------------------------------------
//
void CRoapEngBase::ConstructL()
    {
    LOGLIT( "CRoapEngBase::ConstructL" )

    iCompleted = EFalse;
    iState = EInit;
    iTransTrackConf = EFalse;
    iAllowedToContactRi = EFalse;
    iUseRiContextUrl = EFalse;
    iImplicitJoinDomain = EFalse;
    iReRegistered = EFalse;
    iRiAlias = NULL;
    }


// ---------------------------------------------------------
// CRoapEngBase::SetTriggerL()
// ---------------------------------------------------------
//
EXPORT_C void CRoapEngBase::SetTriggerL( const TDesC8& aXmlTrigger,
    const CAiwGenericParamList* /*aParamList*/,
    TTriggerType& aType,
    TRiContextStatus& aContextStatus,
    TDomainOperation& aDomainOperation,
    RPointerArray<HBufC8>& aContentIdList )
    {
    LOGLIT( "CRoapEngBase::SetTriggerL" )

#ifndef __DRM_OMA2
    User::Leave(KErrNotSupported);
#endif

    __ASSERT_ALWAYS( iState == EInit || iState == EReady, \
        User::Invariant() );
    TBool registered = EFalse;
    TBool isValidDomainGeneration = ETrue;
    TInt error = KErrNone;
    HBufC8* temp = NULL;
    aDomainOperation = ENotDomainOperation;
    iProgressInBytes = KRoapProgressDelta;

    delete iTrigger;
    iTrigger = NULL;

    iTrigger = ParseTriggerL( aXmlTrigger );

    if ( !iTrigger || !iTrigger->ValidTrigger() )
        {
        LOGLIT( "Trigger parsing failed!" )
        User::Leave( KErrRoapGeneral );
        }

    aType = iTrigger->iTriggerType;

    if ( iTrigger->iTriggerType == ERegistrationTrigger )
        {
        iReqMessage = EDeviceHello;
        if ( iAllowedToContactRi )
            {
            aContextStatus = EInvalidContextNoConf;
            }
        else
            {
            aContextStatus = EInvalidContext;
            }
        iState = EReady;
        return;
        }
    else if ( iTrigger->iTriggerType == ERoAcquisitionTrigger )
        {
        iReqMessage = EROAcquisition;
        }
    else if ( iTrigger->iTriggerType == EJoinDomainTrigger )
        {
        iReqMessage = EJoinDomain;
        }
    else if ( iTrigger->iTriggerType == ELeaveDomainTrigger )
        {
        iReqMessage = ELeaveDomain;
        }
#ifdef RD_DRM_METERING
    else if ( iTrigger->iTriggerType == EMeteringReportTrigger )
        {
        iReqMessage = EMeteringRequest;
        }
#endif
    else
        {
        LOG2(_L ("Unsupported Trigger type: %d"), iTrigger->iTriggerType)
        User::Leave( KErrRoapUnsupported );
        }

    GetRIContextL( registered, iTrigger->iRiId );

    if ( !registered )
        {
        LOGLIT( "No valid RI Context -> Device Hello" )
        iReqMessage = EDeviceHello;
        aContextStatus = EInvalidContext;
        }
    else
        {
        aContextStatus = EValidContext;
        }

    if ( iTrigger->iTriggerType == EJoinDomainTrigger )
        {
        GetDomainContextL( registered, isValidDomainGeneration, *iTrigger->iDomainId );
        registered ? aDomainOperation = EKeyUpgradeOperation : aDomainOperation = EJoinDomainOperation;
        }

    for ( TInt i = 0; i < iTrigger->iContentIdList.Count(); i++ )
        {
        temp = iTrigger->iContentIdList[i]->Alloc();
        if ( temp )
            {
            error = aContentIdList.Append( temp );
            if ( error )
                {
                aContentIdList.ResetAndDestroy();
                delete temp;
                User::Leave( error );
                }
            }
        else
            {
            aContentIdList.ResetAndDestroy();
            User::Leave( KErrNoMemory );
            }
        }

    if ( iTrigger->iDomainId &&  iTrigger->iTriggerType == ERoAcquisitionTrigger )
        {
        GetDomainContextL( registered, isValidDomainGeneration, *iTrigger->iDomainId );
        if ( !registered || !isValidDomainGeneration )
            {
            // We don't have a valid Domain key for the RO -> JoinDomain
            LOGLIT( "Perform implicit Join Domain" )
            iImplicitJoinDomain = ETrue;
            if ( aContextStatus == EValidContext )
                {
                iReqMessage = EJoinDomain;
                }
            }
        }
    iState = EReady;

    if ( aContextStatus == EInvalidContext && iAllowedToContactRi )
        {
        aContextStatus = EInvalidContextNoConf;
        }
    else if ( aContextStatus == EInvalidContext )
        {
        RRoapStorageClient client;
        User::LeaveIfError( client.Connect() );
        CleanupClosePushL( client );
        // check that SilentRightsUrl is on the white list
        // URL is searched from pre-configured white list
        TBool fromPreConfiguredWhiteList( ETrue );
        if ( client.WhiteListURLExistsL( *iTrigger->iRoapUrl, fromPreConfiguredWhiteList ) )
            {
            aContextStatus = EInvalidContextNoConf;
            }
        CleanupStack::PopAndDestroy( &client );
        }

    if ( iTrigger->iRiAlias )
        {
        iRiAlias = iTrigger->iRiAlias->AllocL();
        }

    }


// ---------------------------------------------------------
// CRoapEngBase::AcceptL()
// ---------------------------------------------------------
//
EXPORT_C void CRoapEngBase::AcceptL( MRoapObserver* aObserver, TRequestStatus* aStatus )
    {
    LOGLIT( "CRoapEngBase::Accept" )
    PERFORMANCE_LOGLIT( "ROAP started" )

    if ( iState != EReady )
        {
        User::Leave( KErrNotReady );
        }

    iObserver = aObserver;
    iParentStatus = aStatus;
    *iParentStatus = KRequestPending;
    iResult = KErrNone;

    if ( iObserver )
        {
        iObserver->RoapProgressInfoL( iProgressInBytes );
        }
    iProgressInBytes = Min( iProgressInBytes + KRoapProgressDelta, KRoapProgressMax );

    Continue( ECreateConn );
    }


// ---------------------------------------------------------
// CRoapEngBase::Reject()
// ---------------------------------------------------------
//
EXPORT_C void CRoapEngBase::Reject()
    {
    LOGLIT( "CRoapEngBase::Reject" )

    Cancel();

    // Cleanup first, so that
    DoCleanup();

    delete iTrigger;
    iTrigger = NULL;
    delete iRequest;
    iRequest = NULL;
    delete iResponse;
    iResponse = NULL;
    delete iRoapResp;
    iRoapResp = NULL;
    delete iHttpHandler;
    iHttpHandler = NULL;
    delete iStoredRiContext;
    iStoredRiContext = NULL;
    delete iDomainRightsResp;
    iDomainRightsResp = NULL;

    iImplicitJoinDomain = EFalse;

    iState = EInit;
    }


// ---------------------------------------------------------
// CRoapEngBase::DoCleanup()
// ---------------------------------------------------------
//
EXPORT_C TInt CRoapEngBase::DoCleanup()
    {
    LOGLIT( "CRoapEngBase::DoCleanup" )

    RDRMRightsClient rightsClient;
    HBufC8* contentUri = NULL;
    TInt error = KErrNone;

    if ( iTrigger && iTrigger->iTriggerType == ERoAcquisitionTrigger )
        {
        if ( iCompleted )
            {
            error = rightsClient.Connect();
            if ( !error)
                {
                for ( TInt i = 0; i < iReturnedROs.Count(); i++ )
                    {
                    error = iReturnedROs[i]->GetContentURI( contentUri );
                    if ( !error )
                        {
                        // rightsClient.DeleteDbEntry( *contentUri, iReturnedROs[i]->GetLocalID() )
                        }
                    }
                rightsClient.Close();
                }
            iReturnedROs.ResetAndDestroy();
            }
        }
    if ( iRoapResp )
        {
        if ( iRoapResp->DcfFileName().Length() )
            {
            RFs fs;
            error = fs.Connect();
            if ( error != KErrNone )
                {
                fs.Delete( iRoapResp->DcfFileName() );
                }
            fs.Close();
            }
        }
    return error;
    }


// ---------------------------------------------------------
// CRoapEngBase::HandleRoReponseL()
// ---------------------------------------------------------
//
EXPORT_C void CRoapEngBase::HandleRoReponseL( const TDesC8& aResponse, RPointerArray<CDRMRights>& aParsedRights )
    {
    LOGLIT( "CRoapEngBase::HandleRoReponseL" )

    __ASSERT_ALWAYS( iState == EInit, User::Invariant() );

    aParsedRights.ResetAndDestroy();

    TRAPD( error, HandleRightsResponsePduL( aResponse, ETrue ) );
    if ( error == KErrRoapNotRegistered )
        {
        // Needs to register
        }
    else if ( iReturnedROs.Count() )
        {
        for ( TInt i = 0; i < iReturnedROs.Count(); i++ )
            {
            CDRMRights* temp = CDRMRights::NewL();
            CleanupStack::PushL( temp );
            *temp = *iReturnedROs[i];
            aParsedRights.AppendL( temp );
            CleanupStack::Pop( temp );
            }
        }
    }

// ---------------------------------------------------------
// CRoapEngBase::GetTrigger()
// ---------------------------------------------------------
//
EXPORT_C const CRoapTrigger& CRoapEngBase::Trigger() const
    {
    return *iTrigger;
    }

// ---------------------------------------------------------
// CRoapEngBase::DoCancel()
// ---------------------------------------------------------
//
void CRoapEngBase::DoCancel()
    {
    LOGLIT( "CRoapEngBase::DoCancel" )

    if (iHttpHandler)
        {
        iHttpHandler->Cancel();
        }
    iResult = KErrCancel;
    CompleteRequest();
    }


// ---------------------------------------------------------
// CRoapEngBase::RunL()
// ---------------------------------------------------------
//
void CRoapEngBase::RunL()
    {
    LOGLIT( "CRoapEngBase::RunL" )

    switch ( iState )
        {
        case ECreateConn:
                {
                CreateConnectionL();
                break;
                }
        case EStartTrans:
                {
                StartTransactionL();
                break;
                }
        case ERespReceived:
                {
                HandleResponseL();
                break;
                }
        case EComplete:
                {
                CompleteL();
                break;
                }
        default:
                {
                break;
                }
        }
    }

// ---------------------------------------------------------
// CRoapEngBase::RunError()
// ---------------------------------------------------------
//
TInt CRoapEngBase::RunError( TInt aError )
    {
    LOG2( _L ( "CRoapEngBase::RunError: %d" ), aError )
    PERFORMANCE_LOG2( _L ( "Error occured: %d" ), aError  )

    iResult = aError;

    if ( aError == KErrTimedOut || aError == KErrDisconnected )
        {
        // Retry in time out and disconnected cases
        iResult = KErrRoapServer;
        }

    if ( iReqMessage == ERegistration )
        {
        // in case of retry send DeviceHello (not RegRequest)
        iReqMessage = EDeviceHello;
        }

    if ( iRoapResp )
        {
        if ( iRoapResp->DcfFileName().Length() )
            {
            RFs fs;
            if ( fs.Connect() )
                {
                fs.Delete( iRoapResp->DcfFileName() );
                fs.Close();
                }
            }
        }

    iCompleted = ETrue;
    Continue( EComplete );

    return KErrNone;
    }

// ---------------------------------------------------------
// CRoapEngBase::CreateConnectionL()
// ---------------------------------------------------------
//
void CRoapEngBase::CreateConnectionL()
    {
    LOGLIT( "CRoapEngBase::CreateConnectionL" )

    __ASSERT_ALWAYS( iState == ECreateConn, User::Invariant() );

    if ( iObserver )
        {
        iObserver->RoapProgressInfoL( iProgressInBytes );
        }
    iProgressInBytes = Min( iProgressInBytes + KRoapProgressDelta, KRoapProgressMax );

    iHttpHandler = CRoapHttpHandler::NewL();
    iHttpHandler->SetObserver( iObserver );
    iHttpHandler->SetPreferredIap( iPreferredIap );
    iHttpHandler->CreateConnectionL( &iStatus );

    iState = EStartTrans;
    SetActive();
    }

// ---------------------------------------------------------
// CRoapEngBase::StartTransactionL()
// ---------------------------------------------------------
//
void CRoapEngBase::StartTransactionL()
    {
    LOGLIT( "CRoapEngBase::StartTransactionL" )

    User::LeaveIfError( iStatus.Int() );

    __ASSERT_ALWAYS( iState == EStartTrans, User::Invariant() );
    __ASSERT_ALWAYS( iHttpHandler, User::Invariant() );
    __ASSERT_ALWAYS( iTrigger, User::Invariant() );
    __ASSERT_ALWAYS( !iRequest, User::Invariant() );

    if ( iObserver )
        {
        iObserver->RoapProgressInfoL( iProgressInBytes );
        }
    iProgressInBytes = Min( iProgressInBytes + KRoapProgressDelta, KRoapProgressMax );

    HBufC8* reqBody = NULL;
    HBufC8* signedReqBody = NULL;
    HBufC8* url = NULL;

    CreateReqMessageL(); // creates iRequest

    reqBody = iRequest->MessageAsXmlL();

    CleanupStack::PushL( reqBody );

    if ( iUseRiContextUrl )
        {
        if ( iStoredRiContext )
            {
            url = HBufC8::NewMaxLC( iStoredRiContext->RightsIssuerURL().Length() );
            url->Des().Copy( iStoredRiContext->RightsIssuerURL() );
            }
        else
            {
            User::Leave( KErrRoapNotRegistered );
            }
        }
    else
        {
        url = HBufC8::NewMaxLC( iTrigger->iRoapUrl->Length() );
        url->Des().Copy( *iTrigger->iRoapUrl );
        }

    signedReqBody = SignMessageL( *reqBody );

    CleanupStack::PushL( signedReqBody );

    LOGHEX(signedReqBody->Ptr(), signedReqBody->Length())

    iRoapResp = CRoapResponse::NewL();

    iHttpHandler->SetUrlL( *url );

    if ( iReqMessage == EROAcquisition )
        {
        if ( iObserver )
            {
            iObserver->RoapProgressInfoL( iProgressInBytes );
            }
        iProgressInBytes = Min( iProgressInBytes + KRoapProgressDelta, KRoapProgressMax );
        }

    iHttpHandler->DoTransactionL(*iRoapResp, *signedReqBody, &iStatus );

    CleanupStack::PopAndDestroy( signedReqBody );
    CleanupStack::PopAndDestroy( url );
    CleanupStack::PopAndDestroy( reqBody );

    iState = ERespReceived;
    SetActive();
    }


// ---------------------------------------------------------
// CRoapEngBase::HandleResponseL()
// ---------------------------------------------------------
//
void CRoapEngBase::HandleResponseL()
    {
    LOGLIT( "CRoapEngBase::HandleResponseL" )

    __ASSERT_ALWAYS( iState == ERespReceived, User::Invariant() );
    __ASSERT_ALWAYS( iRoapResp, User::Invariant() );

    User::LeaveIfError( iStatus.Int() );

    LOG(iRoapResp->DataType().Des())
    LOGHEX(iRoapResp->ProtocolUnit().Ptr(), iRoapResp->ProtocolUnit().Length())

    TInt status = KErrNone;

    if ( iRoapResp->DataType() == TDataType( KRoapPDUType  ) )
        {
        LOGLIT( "ROAP PDU datatype received." )

            if ( iObserver )
                {
                iObserver->RoapProgressInfoL( iProgressInBytes );
                }
        iProgressInBytes = Min( iProgressInBytes + KRoapProgressDelta, KRoapProgressMax );

        HandleRoapResponseL( iRoapResp->ProtocolUnit() );
        }
    else if ( iRoapResp->DataType() == TDataType( KMultipartRelatedType ) )
        {
        LOGLIT( "Multipart related datatype received." )
            HandleMultipartL();
        }
    else
        {
        LOGLIT( "Error: Unsupported response datatype." )

            User::Leave( KErrRoapUnsupported );
        }

    status = MapStatusL();

    iUseRiContextUrl = EFalse;

    if ( status == KErrRoapNotRegistered )
        {
        // Not registered or DRM Time is incorrect
        // -> Check user consent and initiate the registration protocol

        if ( iReRegistered )
            {
            LOGLIT( "DRM Time update or re-registering is already performed once!" )
                User::Leave( KErrRoapServer );
            }

        if ( iObserver && !iAllowedToContactRi )
            {
            if ( iObserver->ContactRiConfL() )
                {
                iReqMessage = EDeviceHello;
                iCompleted = EFalse;
                iAllowedToContactRi = ETrue;
                iUseRiContextUrl = EFalse;
                }
            else
                {
                User::Leave( status );
                }
            }
        else
            {
            iReqMessage = EDeviceHello;
            iCompleted = EFalse;
            }
        iReRegistered = ETrue;
        }
    else
        {
        SetProtocolL();
        }

    // delete previous request and response
    delete iRequest;
    iRequest = NULL;
    delete iRoapResp;
    iRoapResp = NULL;

    if ( !iCompleted )
        {
        // Send next request
        Continue( EStartTrans );
        }
    else
        {
        // Complete request
        Continue( EComplete );
        }
    }


// ---------------------------------------------------------
// CRoapEngBase::SetProtocol()
// ---------------------------------------------------------
//
void CRoapEngBase::SetProtocolL()
    {
    LOG2( _L ( "CRoapEngBase::SetProtocol: %d" ), iReqMessage )

        switch ( iReqMessage )
            {
            case EDeviceHello:
                    {
                    iReqMessage = ERegistration;
                    iCompleted = EFalse;
                    break;
                    }
            case ERegistration:
                    {
                    if ( iTrigger->iTriggerType != ERegistrationTrigger )
                        {
                        if ( !iImplicitJoinDomain && iTrigger->iTriggerType == ERoAcquisitionTrigger )
                            {
                            iReqMessage = EROAcquisition;
                            }
                        else if ( iTrigger->iTriggerType == EJoinDomainTrigger )
                            {
                            iReqMessage = EJoinDomain;
                            }
                        else if ( iTrigger->iTriggerType == ELeaveDomainTrigger )
                            {
                            iReqMessage = ELeaveDomain;
                            }
#ifdef RD_DRM_METERING
                        else if ( iTrigger->iTriggerType == EMeteringReportTrigger )
                            {
                            iReqMessage = EMeteringRequest;
                            }
#endif
                        else if ( iImplicitJoinDomain && iTrigger->iTriggerType == ERoAcquisitionTrigger )
                            {
                            iReqMessage = EJoinDomain;
                            }
                        else
                            {
                            User::Leave( KErrRoapUnsupported );
                            }
                        iCompleted = EFalse;
                        }
                    else
                        {
                        iCompleted = ETrue;
                        }
                    break;
                    }
            case EROAcquisition:
                    {
                    if ( iImplicitJoinDomain && iTrigger->iTriggerType == ERoAcquisitionTrigger )
                        {
                        iReqMessage = EJoinDomain;
                        iImplicitJoinDomain = EFalse;
                        iCompleted = EFalse;
                        }
                    else
                        {
                        iCompleted = ETrue;
                        }
                    break;
                    }
            case EJoinDomain:
                    {
                    if ( iImplicitJoinDomain && iTrigger->iTriggerType == ERoAcquisitionTrigger )
                        {
                        iReqMessage = EROAcquisition;
                        iImplicitJoinDomain = EFalse;
                        iCompleted = EFalse;
                        }
                    else
                        {
                        iCompleted = ETrue;
                        }
                    break;
                    }
#ifdef RD_DRM_METERING
            case EMeteringRequest:
                    {
                    if ( iTrigger->iTriggerType != EMeteringReportTrigger )
                        {
                        // PostUrl would be done here?
                        iCompleted = EFalse;
                        }
                    else
                        {
                        iCompleted = ETrue;
                        }
                    break;
                    }
#endif
            default:
                    {
                    iCompleted = ETrue;
                    break;
                    }
            }
    if (iCompleted)
        {
        LOGLIT( "Completed" )
        }
    else
        {
        LOG2(_L ("Next protocol: %d"), iReqMessage)
        }
    }

// ---------------------------------------------------------
// CRoapEngBase::CompleteL()
// ---------------------------------------------------------
//
void CRoapEngBase::CompleteL()
    {
    LOGLIT( "CRoapEngBase::CompleteL" )

        if ( iCompleted )
            {
            CompleteRequest();
            }
    }


// ---------------------------------------------------------
// CRoapEngBase::CompleteRequest()
// ---------------------------------------------------------
//
void CRoapEngBase::CompleteRequest()
    {
    LOGLIT( "CRoapEngBase::CompleteRequest" )
    PERFORMANCE_LOGLIT( "ROAP completed" )

    // Complete request
    iState = EReady;

    // Delete allocated member variables also here since AcceptL
    // might be called in retry case without deleting the CRoapEngBase instance
    delete iRequest;
    iRequest = NULL;
    delete iRoapResp;
    iRoapResp = NULL;
    delete iResponse;
    iResponse = NULL;
    delete iHttpHandler;
    iHttpHandler = NULL;
    iReturnedROs.ResetAndDestroy();
    delete iDomainRightsResp;
    iDomainRightsResp = NULL;
    iImplicitJoinDomain = EFalse;

    // Notify caller
    User::RequestComplete( iParentStatus, iResult );
    iParentStatus = NULL;
    }


// ---------------------------------------------------------
// CRoapEngBase::Continue
// ---------------------------------------------------------
//
void CRoapEngBase::Continue( TRoapState aNextState )
    {
    iState = aNextState;
    TRequestStatus* ownStatus = &iStatus;
    *ownStatus = KRequestPending;
    SetActive();
    User::RequestComplete( ownStatus, KErrNone );
    }
