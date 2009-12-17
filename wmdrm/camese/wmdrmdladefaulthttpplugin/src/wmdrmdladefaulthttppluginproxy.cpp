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
* Description:  WMDRM DLA Default HTTP Plugin Proxy
*
*/


#include <ecom/implementationproxy.h>
#include "wmdrmdladefaulthttpplugin.h"

const TImplementationProxy ImplementationTable[] =
	{
#ifdef __EABI__
		IMPLEMENTATION_PROXY_ENTRY( 0x20019574, CWmDrmDlaDefaultHttpPlugin::NewL )
#else
		{ {0x20019574}, CWmDrmDlaDefaultHttpPlugin::NewL }
#endif
	}; 

// -----------------------------------------------------------------------------
// ImplementationGroupProxy: Lookup method required by ECom
// Returns the ImplementationTable to the  ECom framework
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
	{
	aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
	return ImplementationTable;
	}
