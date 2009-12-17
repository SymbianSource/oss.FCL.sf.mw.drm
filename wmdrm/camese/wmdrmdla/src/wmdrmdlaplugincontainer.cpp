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
* Description:  Implementation class for WMDRM DLA plugin container
*
*/


#include <wmdrmdlahttpplugin.h>
#include <wmdrmdlauinotifier.h>
#include "wmdrmdlaplugincontainer.h"

#define _LOGGING_FILE L"wmdrmdla.txt"
#include "logfn.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWmDrmDlaPluginContainer::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaPluginContainer::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaPluginContainer::CWmDrmDlaPluginContainer
// ---------------------------------------------------------------------------
//
CWmDrmDlaPluginContainer::CWmDrmDlaPluginContainer()
    {
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaPluginContainer::NewL
// ---------------------------------------------------------------------------
//
CWmDrmDlaPluginContainer* CWmDrmDlaPluginContainer::NewL()
    {
    LOGFN( "CWmDrmDlaPluginContainer::NewL" );
    CWmDrmDlaPluginContainer* self = CWmDrmDlaPluginContainer::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaPluginContainer::NewLC
// ---------------------------------------------------------------------------
//
CWmDrmDlaPluginContainer* CWmDrmDlaPluginContainer::NewLC()
    {
    LOGFN( "CWmDrmDlaPluginContainer::NewLC" );
    CWmDrmDlaPluginContainer* self = new( ELeave ) CWmDrmDlaPluginContainer;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaPluginContainer::~CWmDrmDlaPluginContainer
// ---------------------------------------------------------------------------
//
CWmDrmDlaPluginContainer::~CWmDrmDlaPluginContainer()
    {
    LOGFN( "CWmDrmDlaPluginContainer::~CWmDrmDlaPluginContainer" );
    delete iHttpPlugin;
    delete iUiNotifier;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaPluginContainer::SetHttpPlugin
// ---------------------------------------------------------------------------
//
void CWmDrmDlaPluginContainer::SetHttpPlugin( 
    CWmDrmDlaHttpPluginIf* aHttpPlugin )
    {
    LOGFN( "CWmDrmDlaPluginContainer::SetHttpPlugin" );
    iHttpPlugin = aHttpPlugin;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaPluginContainer::GetHttpPlugin
// ---------------------------------------------------------------------------
//
void CWmDrmDlaPluginContainer::GetHttpPlugin(
    CWmDrmDlaHttpPluginIf*& aHttpPlugin )
    {
    LOGFN( "CWmDrmDlaPluginContainer::GetHttpPlugin" );
    aHttpPlugin = iHttpPlugin;
    iHttpPlugin = NULL;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaPluginContainer::HttpPlugin
// ---------------------------------------------------------------------------
//
CWmDrmDlaHttpPluginIf* CWmDrmDlaPluginContainer::HttpPlugin()
    {
    LOGFN( "CWmDrmDlaPluginContainer::HttpPlugin" );
    return iHttpPlugin;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaPluginContainer::SetUiNotifier
// ---------------------------------------------------------------------------
//
void CWmDrmDlaPluginContainer::SetUiNotifier( 
    CWmDrmDlaUiNotifier* aUiNotifier )
    {
    LOGFN( "CWmDrmDlaPluginContainer::SetUiNotifier" );
    iUiNotifier = aUiNotifier;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaPluginContainer::UiNotifier
// ---------------------------------------------------------------------------
//
void CWmDrmDlaPluginContainer::GetUiNotifier(
    CWmDrmDlaUiNotifier*& aUiNotifier )
    {
    LOGFN( "CWmDrmDlaPluginContainer::GetUiNotifier" );
    aUiNotifier = iUiNotifier;
    iUiNotifier = NULL;
    }
    
// ---------------------------------------------------------------------------
// CWmDrmDlaPluginContainer::SetIndex
// ---------------------------------------------------------------------------
//
void CWmDrmDlaPluginContainer::SetIndex( TInt aIndex )
    {
    LOGFN( "CWmDrmDlaPluginContainer::SetIndex" );
    iIndex = aIndex;
    LOG2( "iIndex: %d", iIndex );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaPluginContainer::Index
// ---------------------------------------------------------------------------
//
TInt CWmDrmDlaPluginContainer::Index()
    {
    LOGFN( "CWmDrmDlaPluginContainer::Index" );
    LOG2( "iIndex: %d", iIndex );
    return iIndex;
    }
