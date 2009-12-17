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
* Description:  Implementation for HTTP plugins ECOM structure
*
*/


#include <ecom/ecom.h>
#include <wmdrmdlahttpplugin.h>

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpPluginIf::NewL(
// ---------------------------------------------------------------------------
//
inline CWmDrmDlaHttpPluginIf* CWmDrmDlaHttpPluginIf::NewL( TUid aUid )
    {
    return static_cast<CWmDrmDlaHttpPluginIf*>( 
        REComSession::CreateImplementationL( aUid, 
                                             _FOFF( CWmDrmDlaHttpPluginIf, 
                                                    iDlaHttpPluginUid ) ) );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHttpPluginIf::~CWmDrmDlaHttpPluginIf
// ---------------------------------------------------------------------------
//
inline CWmDrmDlaHttpPluginIf::~CWmDrmDlaHttpPluginIf()
    {
    REComSession::DestroyedImplementation( iDlaHttpPluginUid );
    }

