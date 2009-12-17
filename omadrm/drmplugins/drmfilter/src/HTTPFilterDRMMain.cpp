/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <ecom/implementationproxy.h>
#include "HTTPFilterDRM.h"


const TImplementationProxy KImplementationTable[] = 
	{
	#ifdef __EABI__ 
		IMPLEMENTATION_PROXY_ENTRY(0x101F9711,CHTTPFilterDRM::InstallFilterL) 
 	#else
		{{0x101F9711}, CHTTPFilterDRM::InstallFilterL}
 	#endif

	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(KImplementationTable) / sizeof(TImplementationProxy);
	return KImplementationTable;
	}
