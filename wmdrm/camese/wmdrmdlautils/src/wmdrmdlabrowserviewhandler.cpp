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
* Description:  Implementation class for WMDRM DLA Browser View Handler
*
*/


// INCLUDE FILES
#include <coemain.h>
#include <apaserverapp.h>
#include "wmdrmdlabrowserviewhandler.h"
#include "wmdrmdlabrowserobserver.h"
#include "wmdrmdlaappclient.h"
#include "wmdrmdlaappconstants.h"

#define _LOGGING_FILE L"wmdrmdlautils.txt"
#include "logfn.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserViewHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserViewHandler::ConstructL()
    {
    LOGFN( "CWmDrmDlaBrowserViewHandler::ConstructL" );
    iClient = new (ELeave) RWmDrmDlaAppClient;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserViewHandler::CWmDrmDlaBrowserViewHandler
// ---------------------------------------------------------------------------
//
CWmDrmDlaBrowserViewHandler::CWmDrmDlaBrowserViewHandler() 
    : CActive( EPriorityStandard ), iState( EIdle )
    {
    LOGFN( "CWmDrmDlaBrowserViewHandler::CWmDrmDlaBrowserViewHandler" );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserViewHandler::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CWmDrmDlaBrowserViewHandler* CWmDrmDlaBrowserViewHandler::NewL()
    {
    LOGFN( "CWmDrmDlaBrowserViewHandler::NewL" );
    CWmDrmDlaBrowserViewHandler* self = CWmDrmDlaBrowserViewHandler::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserViewHandler::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CWmDrmDlaBrowserViewHandler* CWmDrmDlaBrowserViewHandler::NewLC()
    {
    LOGFN( "CWmDrmDlaBrowserViewHandler::NewLC" );
    CWmDrmDlaBrowserViewHandler* self 
        = new( ELeave ) CWmDrmDlaBrowserViewHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserViewHandler::~CWmDrmDlaBrowserViewHandler
// ---------------------------------------------------------------------------
//
CWmDrmDlaBrowserViewHandler::~CWmDrmDlaBrowserViewHandler()
    {
    LOGFN( "CWmDrmDlaBrowserViewHandler::~CWmDrmDlaBrowserViewHandler" );
    Cancel();
    delete iClient;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserViewHandler::LaunchBrowserViewL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWmDrmDlaBrowserViewHandler::LaunchBrowserViewL( 
    MWmDrmDlaBrowserObserver* aObserver )
    {
    LOGFN( "CWmDrmDlaBrowserViewHandler::LaunchBrowserViewL" );
    if ( !CCoeEnv::Static() )
        {
        User::Leave( KErrNotSupported );
        }
    if ( !aObserver )
        {
        User::Leave( KErrArgument );
        }
    if ( iState != EIdle )
        {
        User::Leave( KErrInUse );
        }
    iObserver = aObserver;
    iClient->ConnectChainedAppL( KWmDrmDlaAppUid );
    iState = CWmDrmDlaBrowserViewHandler::ELaunchBrowserView;
    CompleteSelf();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserViewHandler::DoCancel
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserViewHandler::DoCancel()
    {
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserViewHandler::RunL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserViewHandler::RunL()
    {
    LOGFN( "CWmDrmDlaBrowserViewHandler::RunL" );
    HBufC8* licenseResponse( NULL );
    switch ( iState )
        {
        case CWmDrmDlaBrowserViewHandler::ELaunchBrowserView:
            User::LeaveIfError( iClient->SetIAP( iObserver->IapId() ) );
            User::LeaveIfError( 
                    iClient->Post( iObserver->PostUrlL(), 
                                   iObserver->PostContentTypeL(), 
                                   iObserver->PostDataL(), 
                                   iObserver->PostContentDataBoundaryL() ) );
            User::LeaveIfError( iClient->LicenseResponse( licenseResponse ) );
            iObserver->LicenseResponseReceived( *licenseResponse );
            delete licenseResponse;
            iClient->Close();
            iState = CWmDrmDlaBrowserViewHandler::EIdle;
            break;
        
        default:
            ASSERT( EFalse );
            break;
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserViewHandler::RunError
// ---------------------------------------------------------------------------
//
TInt CWmDrmDlaBrowserViewHandler::RunError( TInt aError )
    {
    LOGFN( "CWmDrmDlaBrowserViewHandler::RunError" );
    LOG2( "aError: %d", aError );
    iClient->Close();
    iObserver->BrowserExit( aError );
    iState = CWmDrmDlaBrowserViewHandler::EIdle;
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaBrowserViewHandler::CompleteSelf
// ----------------------------------------------------------------------------
//
void CWmDrmDlaBrowserViewHandler::CompleteSelf()
    {
    if ( !IsActive() )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }
    }
