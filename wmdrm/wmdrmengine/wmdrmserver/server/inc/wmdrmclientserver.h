/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Common client server definitions
*
*/


#ifndef __WMDRMCLIENTSERVER_H
#define __WMDRMCLIENTSERVER_H

#include <e32std.h>

// Client/server messages
enum TWmDrmMessages
	{
    EInitStore,
    ERemoveStore,
    EInitNamespace,
    ERemoveNamespace,
    ESlotOpen,
    ESlotCreate,
    ESlotSeek,
    ESlotRead,
    ESlotWrite,
    ESlotDelete,
    ESlotClose,
    ESlotResize,
    EEnumerateStart,
    EEnumerateReload,
    EEnumerateNext,
    EEnumerateDelete,
    EEnumerateEnd,
    ELogStats,
    EEmptyCache,
    ETimeValid,
    ESetTimeAsValid,
    EDeleteRights,
    EStoreState,
    EGetTime,
    ECommitData
	};

#endif //  __WMDRMCLIENTSERVER_H
