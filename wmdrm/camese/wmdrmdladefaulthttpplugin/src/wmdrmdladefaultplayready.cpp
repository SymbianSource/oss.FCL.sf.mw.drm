/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  WMDRM DLA Default Http PlayReady Implementation
*
*/


#include <chttpformencoder.h>
#include <httpstringconstants.h>
#include <httperr.h>
#include <http/rhttpheaders.h>
#include <http/thttphdrfielditer.h>
#include <stringpool.h>
#include <escapeutils.h>
#include <wmdrmdlatypes.h>
#include <wmdrmdlauinotifier.h>
#include "wmdrmdladefaultplayready.h"
#include "wmdrmdladefaulthttpplugin.h"
#include "wmdrmdladefaulthttpmanager.h"

#define _LOGGING_FILE L"wmdrmdladefaulthttpplugin.txt"
#include "logfn.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultPlayReady::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultPlayReady::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultPlayReady::CWmDrmDlaDefaultPlayReady
// ---------------------------------------------------------------------------
//
CWmDrmDlaDefaultPlayReady::CWmDrmDlaDefaultPlayReady(
    CWmDrmDlaDefaultHttpPlugin* aPlugin ) 
    : iState( CWmDrmDlaDefaultPlayReady::ENotActive ), iPlugin( aPlugin)
    {
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultPlayReady::NewL
// ---------------------------------------------------------------------------
//
CWmDrmDlaDefaultPlayReady* CWmDrmDlaDefaultPlayReady::NewL(
    CWmDrmDlaDefaultHttpPlugin* aPlugin )
    {
    CWmDrmDlaDefaultPlayReady* self 
        = new( ELeave ) CWmDrmDlaDefaultPlayReady( aPlugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultPlayReady::~CWmDrmDlaDefaultPlayReady
// ---------------------------------------------------------------------------
//
CWmDrmDlaDefaultPlayReady::~CWmDrmDlaDefaultPlayReady()
    {
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultPlayReady::SupportedInitiatorL
// ---------------------------------------------------------------------------
//
TBool CWmDrmDlaDefaultPlayReady::SupportedInitiatorL( 
    const TDesC8& /*aInitiator*/, 
    CWmDrmDlaUiNotifier*& /*aUiNotifier*/ )
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultPlayReady::ProcessInitiatorL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultPlayReady::ProcessInitiatorL( 
    const TDesC8& /*aInitiator*/ )
    {
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultPlayReady::ProcessJoinDomainChallenge
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultPlayReady::ProcessJoinDomainChallenge( 
    const TDesC8& /*aJoinDomainChallenge*/, 
    const TDesC& /*aUrl*/, 
    TRequestStatus& aStatus )
    {
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    CompleteClientRequest( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultPlayReady::ProcessLeaveDomainChallenge
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultPlayReady::ProcessLeaveDomainChallenge( 
    const TDesC8& /*aLeaveDomainChallenge*/, 
    const TDesC& /*aUrl*/, 
    TRequestStatus& aStatus )
    {
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    CompleteClientRequest( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::SetIapId
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultPlayReady::SetIapId( TInt /*aIapId*/ )
    {
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultPlayReady::OnGetUsernamePasswordL
// From MHttpManagerObserver
// This function is called when using HTTPS authentication. Authentication is 
// not currently used in the Camese project
// ----------------------------------------------------------------------------
//
TBool CWmDrmDlaDefaultPlayReady::OnGetUsernamePasswordL(
    HBufC8*& /*aUsername*/,
    HBufC8*& /*aPassword*/)
    {
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultPlayReady::OnResponseHeadersL
// From MHttpManagerObserver
// ----------------------------------------------------------------------------
//
void CWmDrmDlaDefaultPlayReady::OnResponseHeadersL(
    const RHTTPResponse& /*aResponse*/,
    const RHTTPHeaders& /*aHeaders*/,
    const RStringPool& /*aStringPool*/,
    TInt /*aHttpStatus*/ )
    {
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultPlayReady::OnResponseBodyDataL
// From MHttpManagerObserver
// ----------------------------------------------------------------------------
//
void CWmDrmDlaDefaultPlayReady::OnResponseBodyDataL( 
    const TPtrC8 /*aDataChunk*/ )
    {
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultPlayReady::OnTransactionComplete
// From MHttpManagerObserver
// ----------------------------------------------------------------------------
//
void CWmDrmDlaDefaultPlayReady::OnTransactionComplete( TInt /*aSystemError*/ )
    {
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaDefaultPlayReady::CompleteClientRequest
// ----------------------------------------------------------------------------
//
void CWmDrmDlaDefaultPlayReady::CompleteClientRequest( TInt aError )
    {
    if ( iClientStatus )
        {
        User::RequestComplete( iClientStatus, aError );
        iClientStatus = NULL;
        }
    }
