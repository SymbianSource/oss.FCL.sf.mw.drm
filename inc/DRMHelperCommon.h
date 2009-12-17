/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  common file for both client and server
*
*/



#ifndef DRMHELPERCOMMON_H
#define DRMHELPERCOMMON_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS

//server name
_LIT(KDRMHelperServerName,"CDRMHelperServer");
_LIT( KDRMHSServerFileName , "DRMHelperServer" );
_LIT(KDRMHelperServerSemaphoreName, "DRMHelperServerSemaphore");
_LIT(KNullDate,"00000000:000000.000000");
//interval for not endtime based rights
const TInt KTimeIntervalYears = 10;

//default value of informing interval
const TInt KAboutToExpireInterval = 7;

//the server version. A version must be specified when 
//creating a session with the server
const TUint KDRMHSMajorVersionNumber=0;
const TUint KDRMHSMinorVersionNumber=1;
const TUint KDRMHSBuildVersionNumber=1;


// MACROS

// DATA TYPES

//opcodes used in message passing between client and server
enum TDRMHelperServRqst
    {
    ERegister,
    ERemove,
    EIndicateIdle,
    EIsRegistered,
    };


/** Panic Category */
_LIT( KDRMHSServer, "CDRMHelperServer" );

/**  panic codes */
enum TDRMHelperServPanic
    {
    EBadDescriptor,
    ESrvCreateServer,
    ECreateTrapCleanup
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION



#endif      // DRMHELPERCOMMON_H
            
// End of File
