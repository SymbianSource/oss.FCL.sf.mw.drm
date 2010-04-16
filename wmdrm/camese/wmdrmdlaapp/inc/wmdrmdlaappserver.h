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
* Description:  Interface class for WMDRM DLA App Server
*
*/


#ifndef C_WMDRMDLAAPPSERVER_H
#define C_WMDRMDLAAPPSERVER_H

// INCLUDES
#include <AknServerApp.h>

class CWmDrmDlaAppServer : public CAknAppServer
    {

    public: // from CAknAppServer

        CApaAppServiceBase* CreateServiceL( TUid aServiceType ) const;

    };

#endif // C_WMDRMDLAAPPSERVER_H
