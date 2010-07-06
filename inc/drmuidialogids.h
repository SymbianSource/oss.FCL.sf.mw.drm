/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Defines an enumeration for the DRM dialog IDs
*/


#ifndef C_DRMUIDIALOGIDS_H
#define C_DRMUIDIALOGIDS_H

//  Enumeration for the dialog IDs to be launched.
enum TDrmUIDialogId
    {
    EQueryNoRightsObj,
    EQueryFileLocked,              // %U is filename. dialog to be implemented
    EQueryFileLockedForSim,
    EQueryRightsObjExpired,
    EQueryFileWithNoRightsObj,     // %U is filename. dialog to be implemented
    EQueryNoUsage, 
    EQueryNoCount, 
    EQueryDeleteDRMFile, 
    EQueryMoveObjects, 
    EQueryCopyObjects, 
    EQuerySendUnprotectedObjs, 
    EQueryAccountUpdate,
    EQueryRetryForLicence, 
    EQueryGetNewLicence,
    EQueryConnectToActivate,       // dialog to be implemented
    EQueryOpenErrorUrl,            // dialog to be implemented
    EQueryLicenceValidUntil,       // %U is expiration date. dialog to be implemented yet
    EQueryValidForLimitedTime,     // dialog to be implemented
    EQueryUnlockRingToneForSim,    // %U is filename. dialog to be implemented
    EQueryGetRingToneLicence,      // %U is filename. dialog to be implemented
    EQueryUnlockMsgToneForSim,     // %U is filename. dialgo to be implemented
    EQueryGetMsgToneLicence,       // %U is filename. dialog to be implemented
    EQueryUnlockEmailToneForSim,   // %U is filename. dialog to be implemented
    EQueryGetEmailToneLicence,     // %U is filename. dialog to be implemented
    EQueryUseDefaultEmailTone,     // %U is filename. dialog to be implemented
    EQueryActivateIMToneLicence,   // %U is filename. dialog to be implemented
    EQueryUnlockCalToneForSim,     // %U is filename. dialog to be implemented
    EQueryGetCalToneLicence,       // %U is filename. dialog to be implemented
    
    EConfObjLocked, 
    EConfUnsupportedObj, 
    EConfWaitingForLicence,
    EConfDeleted, 
    EConfUnableToMove, 
    EConfUnableToCopy, 
    EConfLicenceExpired,
    EConfFileDeleted,              // %U is filename. dialog to be implemented
    EConfForwardProtected, 
    EConfUnableToSend, 
    EConfViewRestriction, 
    EConfUnableToUse,              // %U is start date of RO. dialog to be implemented
    EConfUnableToDeregister,
    EConfAccountNotRecognized, 
    EConfPhoneDeregistered, 
    EConfRegUpdateFailure, 
    EConfRegistrationUpdated, 
    EConfPhoneRegistered, 
    EConfRegistrationFailure, 
    EConfUnableToOpen,             // dialog to be implemented
    EConfLicenceNotReceived,       // dialog to be implemented
    EConfFileLockedForSim,
    EConfNoAccessPoint,            // dialog to be implemented
    EConfConnectionFailed,         // dialog to be implemented
    EConfUnableToUnlock,           // dialog to be implemented    
    EConfUseDefaultRingTone,       // %U is filename. dialog to be implemented
    EConfRingToneLicenceExpired,   // %U is filename. dialog to be implemented
    EConfUseDefaultMsgTone,        // %U is filename. dialog to be implemented
    EConfMsgToneLicenceExpired,    // %U is filename. dialog to be implemented
    EConfEmailToneLicenceExpired,  // %U is filename. dialog to be implemented
    EConfIMToneLicenceExpired,     // %U is filename. dialog to be implemented
    EConfUseDefaultCalTone,        // %U is filename. dialog to be implemented
    EConfCalToneExpired,           // %U is filename. dialog to be implemented

    EProgDeregisteringPhone,       // progress bar to be implemented
    EProgAcquiringLicence,         // progress bar to be implemented
    EProgRegisteringPhone,         // progress bar to be implemented
    EProgUpdatingRegistration,     // progress bar to be implemented

    ENoteRightsObjReceived,        // popup
    EUnusedId // The last ID
    };
    
enum TDrmUiResultCode
    {
    ECancelled,
    EOk
    };

#endif  // C_DRMUIDIALOGIDS_H

