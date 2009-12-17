/*
* Copyright (c) 2006 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ECOM proxy table for DRMSettinsPlugin
*
*/


// INCLUDES
#include <e32std.h>
#include <ecom/implementationproxy.h>

#include "drmsettingsplugin.h"


// CONSTANTS
const TImplementationProxy KDRMSettingsPluginImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( 0x102750CD, CDRMSettingsPlugin::NewL )
    };


// ---------------------------------------------------------------------------
// ImplementationGroupProxy
//
// Gate/factory function
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
                                                  TInt& aTableCount )
    {
    aTableCount = sizeof( KDRMSettingsPluginImplementationTable )
        / sizeof( TImplementationProxy );
    return KDRMSettingsPluginImplementationTable;
    }



// End of File
