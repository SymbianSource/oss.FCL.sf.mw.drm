/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*  wmdrmaccessecominterface.inl
*
*/
#include <ecom/ecom.h> // for interface

// ---------------------------------------------------------------------------
// CWmDrmAccessEcomInterface::NewL
// ---------------------------------------------------------------------------
//
CWmDrmAccessEcomInterface* CWmDrmAccessEcomInterface::NewL()
    {
    const TUid KWmdrmAccessEcomInterfaceUid =
        {
        0x20026FDB
        };
    TAny* theImplementation( NULL );

    TEComResolverParams resolveParams;
    resolveParams.SetWildcardMatch( ETrue );

    theImplementation = REComSession::CreateImplementationL(
        KWmdrmAccessEcomInterfaceUid,
        _FOFF( CWmDrmAccessEcomInterface, iDtor_ID_Key ),
        resolveParams );

    return reinterpret_cast<CWmDrmAccessEcomInterface*>( theImplementation );
    }


// ---------------------------------------------------------------------------
// CWmDrmAccessEcomInterface::NewLC
// ---------------------------------------------------------------------------
//
CWmDrmAccessEcomInterface* CWmDrmAccessEcomInterface::NewLC()
    {
    CWmDrmAccessEcomInterface* self( CWmDrmAccessEcomInterface::NewL() );

    CleanupStack::PushL( self );
    return self;
    }



// ---------------------------------------------------------------------------
// CWmDrmAccessEcomInterface::~CWmDrmAccessEcomInterface
// ---------------------------------------------------------------------------
//
CWmDrmAccessEcomInterface::~CWmDrmAccessEcomInterface()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

