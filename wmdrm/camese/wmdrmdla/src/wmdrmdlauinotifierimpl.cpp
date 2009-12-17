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
* Description:  Implementation of WMDRM DLA UI Notifier Impl.
*
*/


#include <ecom/ecom.h>
#include <wmdrmdlacancelobserver.h>
#include "wmdrmdlauinotifierimpl.h"
#include "wmdrmdlauiplugin.h"

#define _LOGGING_FILE L"wmdrmdla.txt"
#include "logfn.h"

//CONSTANTS
const TUid KWmDrmDlaDefaultUiPluginUid = { 0x20019575 };

// ======== LOCAL FUNCTIONS ========

// -----------------------------------------------------------------------------
// DoResetDestroyAndClose
// Does ResetAndDestroy() and Close to given RImplInfoPtrArray
// -----------------------------------------------------------------------------
LOCAL_C void DoResetDestroyAndClose( TAny* aPtr )
    {
    ( reinterpret_cast<RImplInfoPtrArray*>( aPtr ) )->ResetAndDestroy();
    ( reinterpret_cast<RImplInfoPtrArray*>( aPtr ) )->Close();
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifierImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaUiNotifierImpl::ConstructL()
    {
    LOGFN( "CWmDrmDlaUiNotifierImpl::ConstructL" );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifierImpl::CWmDrmDlaUiNotifierImpl
// ---------------------------------------------------------------------------
//
CWmDrmDlaUiNotifierImpl::CWmDrmDlaUiNotifierImpl()
    {
    LOGFN( "CWmDrmDlaUiNotifierImpl::CWmDrmDlaUiNotifierImpl" );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifierImpl::NewL
// ---------------------------------------------------------------------------
//
CWmDrmDlaUiNotifierImpl* CWmDrmDlaUiNotifierImpl::NewL()
    {
    LOGFN( "CWmDrmDlaUiNotifierImpl::NewL" );
    CWmDrmDlaUiNotifierImpl* self = CWmDrmDlaUiNotifierImpl::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifierImpl::NewLC
// ---------------------------------------------------------------------------
//
CWmDrmDlaUiNotifierImpl* CWmDrmDlaUiNotifierImpl::NewLC()
    {
    LOGFN( "CWmDrmDlaUiNotifierImpl::NewLC" );
    CWmDrmDlaUiNotifierImpl* self = new( ELeave ) CWmDrmDlaUiNotifierImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifierImpl::~CWmDrmDlaUiNotifierImpl
// ---------------------------------------------------------------------------
//
CWmDrmDlaUiNotifierImpl::~CWmDrmDlaUiNotifierImpl()
    {
    LOGFN( "CWmDrmDlaUiNotifierImpl::~CWmDrmDlaUiNotifierImpl" );
    delete iUiPlugin;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifierImpl::InitializeL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaUiNotifierImpl::InitializeL( 
    TUid aUid )
    {
    LOGFN( "CWmDrmDlaUiNotifierImpl::InitializeL" );
    LOG2( "aUid: %d", aUid );
    delete iUiPlugin;
    iUiPlugin = NULL;
    iUiPlugin = CWmDrmDlaUiPluginIf::NewL( aUid );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifierImpl::InitializeL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaUiNotifierImpl::InitializeL( 
    TDesC8& aOpaqueData )
    {
    TInt err( KErrNotFound );
    
    LOGFNR( "CWmDrmDlaUiNotifierImpl::InitializeL", err );
    LOGHEX( aOpaqueData.Ptr(), aOpaqueData.Length() );
        
    delete iUiPlugin;
    iUiPlugin = NULL;
    RImplInfoPtrArray implArray;
    CleanupStack::PushL( TCleanupItem( DoResetDestroyAndClose, &implArray ) );
    REComSession::ListImplementationsL( KWmDrmDlaUiPluginIfUid, implArray );
    
    for ( TInt i( 0 ); i < implArray.Count() && err == KErrNotFound; ++i )
        {
        if ( aOpaqueData.CompareF( implArray[i]->OpaqueData() ) == 0 &&
             implArray[i]->VendorId() == VID_DEFAULT  )
            {
            err = KErrNone;
            iUiPlugin = 
                CWmDrmDlaUiPluginIf::NewL( implArray[i]->ImplementationUid() );
            }
        }
    if ( err == KErrNotFound )
        {
        err = KErrNone;
        iUiPlugin = CWmDrmDlaUiPluginIf::NewL( KWmDrmDlaDefaultUiPluginUid );
        }
    CleanupStack::PopAndDestroy( &implArray );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifierImpl::SetCancelObserver
// ---------------------------------------------------------------------------
//
void CWmDrmDlaUiNotifierImpl::SetCancelObserver( 
    MWmDrmDlaCancelObserver* aObserver )
    {
    LOGFN( "CWmDrmDlaUiNotifierImpl::SetCancelObserver" );
    if ( iUiPlugin )
        {
        iUiPlugin->SetCancelObserver( aObserver );
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifierImpl::SetSilent
// ---------------------------------------------------------------------------
//
void CWmDrmDlaUiNotifierImpl::SetSilent( 
    TBool aSilent )
    {
    LOGFN( "CWmDrmDlaUiNotifierImpl::SetSilent" );
    LOG2( "aSilent: %d", aSilent );
    if ( iUiPlugin )
        {
        iUiPlugin->SetSilent( aSilent );
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifierImpl::SetProgress
// ---------------------------------------------------------------------------
//
void CWmDrmDlaUiNotifierImpl::SetProgress( 
    TInt aState )
    {
    LOGFN( "CWmDrmDlaUiNotifierImpl::SetProgress" );
    LOG2( "aState: %d", aState );
    if ( iUiPlugin )
        {
        iUiPlugin->SetProgress( aState );
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUiNotifierImpl::HandleErrorL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaUiNotifierImpl::HandleErrorL( 
    TInt aError )
    {
    LOGFN( "CWmDrmDlaUiNotifierImpl::HandleErrorL" );
    LOG2( "aError: %d", aError );
    if ( iUiPlugin )
        {
        iUiPlugin->HandleErrorL( aError );
        }
    }
    
