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
* Description:  Synchronous wrapper for ROAP protocol suite
*
*/


#include <RoapEng.h>
#include "RoapSyncWrapper.h"
#include <RoapDef.h>

using namespace Roap;

// ============================  LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// UpdateUrlL
// -----------------------------------------------------------------------------
//
LOCAL_C void UpdateUrlL( HBufC8*& aTargetUrl, const TDesC8& aSourceUrl )
    {
    if ( aTargetUrl )
        {
        delete aTargetUrl;
        aTargetUrl = NULL;
        }
    if ( aSourceUrl.Length() )
        {
        aTargetUrl = aSourceUrl.AllocL();
        }
    }


// ----------------------------------------------------------------------------
// DoResetAndDestroy
// Does RPointerArray< >->ResetAndDestroy() for the given array aPtr.
// ----------------------------------------------------------------------------
//
LOCAL_C void DoResetAndDestroy( TAny* aPtr )
    {
    ( reinterpret_cast< RPointerArray< HBufC8 >* >( aPtr ) )->
        ResetAndDestroy();
    }



// ============================ MEMBER FUNCTIONS ===============================

CRoapSyncWrapper::CRoapSyncWrapper() : CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

void CRoapSyncWrapper::ConstructL()
    {
    }

EXPORT_C CRoapSyncWrapper* CRoapSyncWrapper::NewL()
    {
    CRoapSyncWrapper* self = new ( ELeave ) CRoapSyncWrapper;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

EXPORT_C CRoapSyncWrapper::~CRoapSyncWrapper()
    {
    Cancel();
    delete iErrorUrl;
    delete iPostResponseUrl;
    }

EXPORT_C void CRoapSyncWrapper::HandleTriggerL( const TDesC8& aTrigger )
    {
    TTriggerType triggerType;
    TRiContextStatus contextStatus;
    TDomainOperation domainOperation;
    RPointerArray<HBufC8> contentIds;
    CRoapEng* roapEng = CRoapEng::NewL();

    CleanupStack::PushL( roapEng );
    roapEng->SetTriggerL( aTrigger, NULL, triggerType, contextStatus,
        domainOperation, contentIds );
    contentIds.ResetAndDestroy();
    roapEng->AcceptL( this, &iStatus );
    SetActive();
    iWait.Start();
    CleanupStack::PopAndDestroy( roapEng );
    User::LeaveIfError( iStatus.Int() );
    }

EXPORT_C TBool CRoapSyncWrapper::HandleTriggerSilentlyL( const TDesC8& aTrigger )
    {
    TTriggerType triggerType;
    TRiContextStatus contextStatus;
    TDomainOperation domainOperation;
    RPointerArray<HBufC8> contentIds;
    CRoapEng* roapEng = CRoapEng::NewL();
    TBool r = EFalse;

    TCleanupItem item( DoResetAndDestroy, &contentIds );
    CleanupStack::PushL( item );
    CleanupStack::PushL( roapEng );
    roapEng->SetTriggerL( aTrigger, NULL, triggerType, contextStatus,
        domainOperation, contentIds );

    // if we have a valid RI context, or if there is no user confirmation needed, do the ROAP
    if ( contextStatus != EInvalidContext )
        {
        roapEng->AcceptL( this, &iStatus );
        SetActive();
        iWait.Start();
        User::LeaveIfError( iStatus.Int() );
        r = ETrue;
        }
    CleanupStack::PopAndDestroy( roapEng );
    CleanupStack::PopAndDestroy( &contentIds );
    return r;
    }

EXPORT_C HBufC8* CRoapSyncWrapper::GetErrorUrlL( const TInt aError, TInt& aErrorType )
    {

    // ROAP ERROR CODES
    switch ( aError )
        {
        case KErrRoapUnsupported:
        case KErrRoapServerFatal:
        case KErrRoapDevice:
        case KErrRoapInvalidDomain:
            {
            aErrorType = KErrRoapPermanent;
            break;
            }
        case KErrRoapGeneral:
        case KErrRoapServer:
        case KErrRoapDomainFull:
        case KErrRoapNotRegistered:
            {
            aErrorType = KErrRoapTemporary;
            break;
            }
        }

    if( iErrorUrl )
        {
        return iErrorUrl->AllocL();
        }
    return NULL;
    }

void CRoapSyncWrapper::ErrorUrlL( const TDesC8& aErrorUrl )
    {
    UpdateUrlL( iErrorUrl, aErrorUrl );
    }

void CRoapSyncWrapper::DoCancel()
    {
    if (iWait.IsStarted())
        {
        iWait.AsyncStop();
        }
    }

void CRoapSyncWrapper::RunL()
    {
    iWait.AsyncStop();
    }

TBool CRoapSyncWrapper::ConnectionConfL()
    {
    return ETrue;
    }

TBool CRoapSyncWrapper::ContactRiConfL()
    {
    return ETrue;
    }

TBool CRoapSyncWrapper::TransIdConfL()
    {
    return EFalse;
    }

void CRoapSyncWrapper::RightsObjectDetailsL(
    const RPointerArray<CDRMRights>& /*aRightsList*/ )
    {
    // do nothing
    }

void CRoapSyncWrapper::ContentDownloadInfoL( TPath& /*aTempFolder*/,
    TFileName& /*aContentName*/,
    TInt& aMaxSize )
    {
    aMaxSize = -1;
    }

void CRoapSyncWrapper::ContentDetailsL( const TDesC& /*aPath*/,
    const TDesC8& /*aType*/,
    const TUid& /*aAppUid*/ )
    {
    }


void CRoapSyncWrapper::RoapProgressInfoL( const TInt /*aProgressInfo*/ )
    {
    // do nothing
    }

void CRoapSyncWrapper::PostResponseUrlL( const TDesC8& aPostResponseUrl )
    {
    UpdateUrlL( iPostResponseUrl, aPostResponseUrl );
    }

EXPORT_C HBufC8* CRoapSyncWrapper::GetPostResponseUrlL()
    {
    if ( iPostResponseUrl )
        {
        return iPostResponseUrl->AllocL();
        }
    return NULL;
    }
//  End of File
