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
* Description:
*  wmdrmotaaccessecominterface.inl
*
*/

#include <ecom/ecom.h> // for interface

// ---------------------------------------------------------------------------
// CWmDrmOtaAccessEcomInterface::NewL
// ---------------------------------------------------------------------------
//
CWmDrmOtaAccessEcomInterface* CWmDrmOtaAccessEcomInterface::NewL()
    {
    const TUid KWmdrmOtaAccessEcomInterfaceUid =
        {
        0x2002E67D
        };
    TAny* theImplementation( NULL );

    TEComResolverParams resolveParams;
    resolveParams.SetWildcardMatch( ETrue );

    theImplementation = REComSession::CreateImplementationL(
        KWmdrmOtaAccessEcomInterfaceUid,
        _FOFF( CWmDrmOtaAccessEcomInterface, iDtor_ID_Key ),
        resolveParams );

    return reinterpret_cast<CWmDrmOtaAccessEcomInterface*>( theImplementation );
    }


// ---------------------------------------------------------------------------
// CWmDrmOtaAccessEcomInterface::NewLC
// ---------------------------------------------------------------------------
//
CWmDrmOtaAccessEcomInterface* CWmDrmOtaAccessEcomInterface::NewLC()
    {
    CWmDrmOtaAccessEcomInterface* self( CWmDrmOtaAccessEcomInterface::NewL() );

    CleanupStack::PushL( self );
    return self;
    }



// ---------------------------------------------------------------------------
// CWmDrmOtaAccessEcomInterface::~CWmDrmOtaAccessEcomInterface
// ---------------------------------------------------------------------------
//
CWmDrmOtaAccessEcomInterface::~CWmDrmOtaAccessEcomInterface()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

