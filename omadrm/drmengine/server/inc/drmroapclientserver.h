/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Client/server interface of ROAP server
*
*/


#ifndef DRMROAPCLIENTSERVER_H
#define DRMROAPCLIENTSERVER_H

//  INCLUDES
#include <e32def.h>



_LIT( KRIContextFile, "c:\\private\\101F51F2\\ricontexts.dat" );
_LIT( KDomainContextFile, "c:\\private\\101F51F2\\domaincontexts.dat" );



namespace Roap
    {
    const TUint8 KServerMajorVersion = 5;
    const TUint8 KServerMinorVersion = 0;
    const TUint16 KServerBuildVersion = 1;

    _LIT( KServerName, "!RoapStorageServer" );

    enum TRoapStorageCommand
        {
        EAddRiContext = 0x01,
        EAddDomainContext = 0x02,
        EGetRiContext = 0x03,
        EGetDomainContext = 0x04,
        EGetData = 0x05,
        EDeleteRiContext = 0x06,
        EDeleteDomainContext = 0x07,
        EDeleteExpiredRIs = 0x08,
        EDeleteExpiredDomains = 0x09,
        EWhiteListCheck = 0x10,
        EGetPublicKey = 0x11,
        EGetCertificates = 0x12,
        ESignMessage = 0x13,
        ESelectRoot = 0x14,
        EActivateRoot = 0x15,
        EGetTrustedRoots = 0x16,
        EGetRootCert = 0x17,
        EDeleteExpired = 0x18,
        EDeleteAll = 0x19,
        ERsaSign = 0x20,
        EGetMeteringData = 0x21,
        EDeleteMeteringData = 0x22,
        ERetrieveAllRIContexts = 0x23,
        EUpdateRIContext = 0x24,
        EUpdateDrmTime = 0x25,
        EVerifyOcspResponses = 0x26,
        EGetOcspResponderId = 0x27
        };

    }

#endif      // DRMROAPCLIENTSERVER_H

// End of File
