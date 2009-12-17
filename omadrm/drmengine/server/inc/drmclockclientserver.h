/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file represents the commands for client/server 
*                communications and common internal data types.
*
*/


#ifndef DRMCLOCKCLIENTSERVER_H
#define DRMCLOCKCLIENTSERVER_H

//  INCLUDES
#include <e32def.h>

namespace DRMClock 
    {
    // CONSTANTS
    const TUint8 KServerMajorVersion = 3;
    const TUint8 KServerMinorVersion = 0;
    const TUint16 KServerBuildVersion = 0;

    _LIT( KServerName, "!DRMClockServer" );  
        
    // MACROS
    // DATA TYPES

    enum TDRMClockCommand 
        {
        /*
          Command: Notifies the clients of the server
          Params:
          TRequestStatus& aRequestStatus ( given by the user )
        */
        EGetDRMTime = 0x01,
    
        /*
          Command: Waits for notifications from the server
          Params:
          TRequestStatus& aRequestStatus ( given by the client )
        */
        EUpdateDRMTime = 0x02
        };
    }    
    
    

#endif      // DRMCLIENTSERVER_H

// End of File
