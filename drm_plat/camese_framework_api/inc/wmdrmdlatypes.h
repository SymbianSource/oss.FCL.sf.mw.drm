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
* Description:  common type definitions needed by multiple classes
*
*/


#ifndef C_WMDRMDLATYPES_H
#define C_WMDRMDLATYPES_H

#include <e32base.h>

/** ECOM interface UIDs */
const TUid KWmDrmDlaHttpPluginIfUid = { 0x20019544 };
const TUid KWmDrmDlaUiPluginIfUid = { 0x20019545  };

/** plugin specific progress states can start after this */ 
const TInt KWmDrmDlaCustomLicenseAcquisitionStateBase = 32;

/**  States for progress information */
enum
    {
    EIdle = 0,
    EProcessingDRMHeader,            
    ERetrievingLicense,
    EProcessingLicense,
    EAddingLicense,
    ESendingLicenseStorageStatus,
    ELicensingComplete,
    EInvisibleRedirection,
    ESendingDrmHeaderStatus,
    EProcessingMeteringChallenge,
    EProcessingJoinDomainChallenge,
    EProcessingLeaveDomainChallenge
    };


#endif // C_WMDRMDLATYPES_H
