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
* Description:  DRMSettingsPlugin internal Central Repository keys.
*
*/


#ifndef DRMSETTINGSPLUGININTERNALCRKEYS_H
#define DRMSETTINGSPLUGININTERNALCRKEYS_H

// INCLUDES
#include <e32def.h>

// CONSTANTS


/**
* Security settings CR UID
*/
const TUid KCRUidDRMSettings = {0x102823B4};

/**
* Transaction Tracking status
* Possible values are:
* 0 = disabled
* 1 = enabled
*/
const TUint32 KDRMSettingsTransactionTracking = 0x00000001;

/**
* Silent Rights Acquisition status
* Possible values are:
* 0 = not allowed
* 1 = allowed
*/
const TUint32 KDRMSettingsSilentRightsAcquisition = 0x00000002;

#endif      // DRMSETTINGSINTERNALCRKEYS_H

// End of file