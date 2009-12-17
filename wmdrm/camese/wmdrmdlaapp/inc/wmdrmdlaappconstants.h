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
* Description:  Interface class for WMDRM DLA App Constants
*
*/


#ifndef WMDRMDLAAPPCONSTANTS_H
#define WMDRMDLAAPPCONSTANTS_H

// INCLUDE FILES
#include <e32std.h>
#include <apaserverapp.h>

//Application UID
const TUid KWmDrmDlaAppUid = { 0x2001E5E1 };

//Application Server Service ID
const TUid KWmDrmDlaAppServiceId = { 0x2001E5E3 };

//Browser View ID
const TUid KWmDrmDlaAppBrowserViewId = { 0x1 };

//Service Commands
enum TWmDrmDlaServiceCmd
    {
    ESetIAP = RApaAppServiceBase::KServiceCmdBase,
    EPost,
    EGetLicenseResponseSize,
    EGetLicenseResponse
    };

#endif // WMDRMDLAAPPCONSTANTS_H