/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  AES encryptor function for drm engine use
*
*/

#ifndef DRMAESCRYPTO_H
#define DRMAESCRYPTO_H

/**
*  DrmAesCrypto - utility for encrypting drm content using AES128-CBC
*  @note for DRM engine internal use only
*
*  @lib drmcrypto.dll
*  @since S60 v5.0
*/
NONSHARABLE_CLASS( DrmAesCrypto )
    {
public:
    /**
    * DrmAesEncryptL
    *
    * @since S60 v5.0
    * @param aKey 16 bytes long encryption key
    * @param aIV initial vector used in enctypting
    *            (becomes 1st block of encrypted data)
    * @param aAddPadding padding addition flag (RFC 2630 padding added if true)
    * @param aData data to be encrypted
    * @return encrypted (and possibly padded) data
    */
    IMPORT_C static HBufC8* DrmAesEncryptL(
        const TDesC8& aKey,
        const TDesC8& aIV,
        const TBool aAddPadding,
        const TDesC8& aData );

    };

#endif // DRMAESCRYPTO_H
