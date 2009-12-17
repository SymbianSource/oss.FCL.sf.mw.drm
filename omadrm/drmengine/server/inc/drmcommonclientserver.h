/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Internal data types for client/server communications.
*
*/


#ifndef DRMGENERICCLIENTSERVER_H
#define DRMGENERICCLIENTSERVER_H

#include <caf/caf.h> 
#include "DRMPointerArray.h"

const TUint8 KDRMKeyLength = 16;
typedef TBuf8< KDRMKeyLength > TDRMKey;


_LIT( KDRMEngCommonSemaphore, "rightsserver" );


enum KDRMErrors
        {
        EOk = KErrNone, 
        EGeneralError = -30100, 
        EUnknownMIME = -30101, 
        EVersionNotSupported = -30102, 
        ESessionError = -30103, 
        ENoRights = KErrCANoRights, 
        ERightsDBCorrupted = -30105, 
        EUnsupported = KErrCANotSupported, 
        ERightsExpired = KErrCANoPermission, 
        EInvalidRights = -30108
        };
#endif      // DRMGENERICCLIENTSERVER_H

// End of File
