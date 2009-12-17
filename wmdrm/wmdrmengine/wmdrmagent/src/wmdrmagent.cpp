/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


// INCLUDE FILES
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include "wmdrmagentfactory.h"

const TImplementationProxy ImplementationTable[] =
	{
#ifdef __EABI__
		IMPLEMENTATION_PROXY_ENTRY(0x10205CB6,
            ContentAccess::CWmDrmAgentFactory::NewL)
#else
		{ {0x10205CB6}, ContentAccess::CWmDrmAgentFactory::NewL}
#endif
	}; 

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ImplementationGroupProxy: Lookup method required by ECom
// Returns the ImplementationTable to the  ECom framework
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}


//  End of File  
