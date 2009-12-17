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
* Description:  DRM Notifier's client/server interface
*
*/


#ifndef DRMNOTIFIERCLIENTSERVER_H
#define DRMNOTIFIERCLIENTSERVER_H

//  INCLUDES
#include <e32def.h>
#include <e32uid.h>

namespace DRMNotifier 
    {
    // CONSTANTS
    const TUint8 KServerMajorVersion = 3;
    const TUint8 KServerMinorVersion = 0;
    const TUint16 KServerBuildVersion = 0;

    const TInt KDRMSizeOfMessage = 350;

    _LIT( KServerName, "!DRMNotifier" );
    
    // MACROS
    // DATA TYPES

    enum TDRMNotifyCommand 
        {
        /*
          Command: Notifies the clients of the server
          Params:
          TRequestStatus& aRequestStatus ( given by the user )
        */
        ENotifyClients = 0x01,
    
        /*
          Command: Waits for notifications from the server
          Params:
          TRequestStatus& aRequestStatus ( given by the client )
        */
        ERecieveNotification = 0x02,


        /*
          Command: Register to the specific session
                    Used to reduce messages between client
                    and server
        */
        ERegister = 0x03,

        /*
          Command: Register to the specific session
                    Used to reduce messages between client
                    and server
        */
        EUnRegister = 0x04,

        /*
          Command: Register the uri to the specific session
                    Used to reduce messages between client
                    and server
        */
        ERegisterURI = 0x05,

        /*
          Command: Register the uri to the specific session
                    Used to reduce messages between client
                    and server
        */
        EUnRegisterURI = 0x06,

        /*
          Command: Waits for notifications from the server
          Params:
        */
        ECancelNotification = 0xFF
        };
    }
    
#endif      // DRMNOTIFIERCLIENTSERVER_H

// End of File
