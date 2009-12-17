/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Camese filters main file
*
*/


// INCLUDES
#include <e32std.h>
#include <ecom/implementationproxy.h>
#include "httpfiltercamese.hrh"
#include "httpfiltercamesedrmheader.h"

// CONSTANTS
const TImplementationProxy KImplementationTable[] =
    {
        {{KCameseFilterDrmHeaderImplementationUid}, (TProxyNewLPtr) CHttpFilterCameseDrmHeader::NewL}
    };

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// Returns the filters implemented in this DLL
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(KImplementationTable) / sizeof(TImplementationProxy);
    return KImplementationTable;
    }

//  End of File
