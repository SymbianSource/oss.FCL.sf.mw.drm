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
* Description:  Declaration of class CDRMAsset
*
*/


#ifndef DRMASSET_H
#define DRMASSET_H

#include <e32base.h>
#include <caf/caf.h>
#include "DcfCommon.h"

// FORWARD DECLARATIONS

const TInt KProtectedDCFKeySize = 24;
const TInt KDcfDigestSize = 20;

class RWriteStream;
class RReadStream;

// CLASS DECLARATION

/**
*  CDrmAsset: Encapsulates the content ID, the key and the hash for a RO
*
*  @lib    DrmRights.dll
*  @since  3.0
*/
class CDRMAsset: public CBase
    {
public:

    IMPORT_C static CDRMAsset* NewL(void);
    IMPORT_C static CDRMAsset* NewLC(void);
    IMPORT_C virtual ~CDRMAsset(void);
    
    IMPORT_C void DuplicateL(CDRMAsset& aAsset);
    IMPORT_C void ExternalizeL(RWriteStream& aStream);
    IMPORT_C void InternalizeL(RReadStream& aStream);
    
    HBufC8* iUid;
    HBufC8* iParentRights;
    TBuf8<KDCFKeySize> iKey;
    TBuf8<KProtectedDCFKeySize> iProtectedKey;
    TBuf8<KDcfDigestSize> iDigest;

    TBuf8<KDCFKeySize> iAuthenticationSeed;
    TBuf8<KProtectedDCFKeySize> iProtectedAuthSeed;
    
protected:
    CDRMAsset(void);
    };

#endif      // DRMASSET_H

// End of File
