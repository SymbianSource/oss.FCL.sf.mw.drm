/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  class representing OMA metering report response
*
*/



#ifndef METERINGRESP
#define METERINGRESP

//  INCLUDES
#include <hash.h>
#include "RoapMessage.h"

namespace Roap
{

/**
*  class representing OMA DRM metering report response
*
*  @lib roaphandler.dll
*  @since S60 v3.2
*/
NONSHARABLE_CLASS( CMeteringResp ) : public CRoapMessage
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMeteringResp* NewL();

        /**
        * Destructor.
        */
        virtual ~CMeteringResp();

    private:

        /**
        * C++ default constructor.
        */
        CMeteringResp();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data
        TRoapStatus iStatus;
        TBuf8<SHA1_HASH> iDeviceId;
        TBuf8<SHA1_HASH> iRiId;
        HBufC8* iDeviceNonce;
        HBufC8* iErrorUrl;
        HBufC8* iSignature;
        RPointerArray<HBufC8> iCertificateChain;
        RPointerArray<HBufC8> iOcspResponse;
        HBufC8* iPrUrl;
    };
}

#endif      // METERINGRESP

// End of File
