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
* Description:  
*
*/


// INCLUDE FILES

#include "DrmAsset.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


EXPORT_C CDRMAsset* CDRMAsset::NewL(void)
    {
    CDRMAsset* self;

    self = new (ELeave) CDRMAsset;
    return self;
    }

EXPORT_C CDRMAsset* CDRMAsset::NewLC(void)
    {
    CDRMAsset* self;

    self = new (ELeave) CDRMAsset;
    CleanupStack::PushL(self);
    return self;
    }

EXPORT_C CDRMAsset::~CDRMAsset(void)
    {
    delete iUid;
    delete iParentRights;
    }

CDRMAsset::CDRMAsset(void):
    iUid(NULL),
    iParentRights(NULL)
    {
    }

EXPORT_C void CDRMAsset::DuplicateL(CDRMAsset& aAsset)
    {
    delete iUid;
    iUid = NULL;
    if (aAsset.iUid != NULL)
        {
        iUid = aAsset.iUid->AllocL();
        }
        
    delete iParentRights;
    iParentRights = NULL;
    if (aAsset.iParentRights != NULL)
        {
        iParentRights = aAsset.iParentRights->AllocL();
        }
        
    iKey.Copy(aAsset.iKey);
    iProtectedKey.Copy(aAsset.iProtectedKey);
    iDigest.Copy(aAsset.iDigest);
    iAuthenticationSeed.Copy(aAsset.iAuthenticationSeed);
    iProtectedAuthSeed.Copy(aAsset.iProtectedAuthSeed);
    }
    
EXPORT_C void CDRMAsset::ExternalizeL(RWriteStream& aStream)
    {
    if (iUid != NULL)
        {
        aStream.WriteInt32L(iUid->Length());
        aStream.WriteL(*iUid);
        }
    else
        {
        aStream.WriteUint32L(0);
        }
        
    if (iParentRights != NULL)
        {
        aStream.WriteInt32L(iParentRights->Length());
        aStream.WriteL(*iParentRights);
        }
    else
        {
        aStream.WriteUint32L(0);
        }
    
    aStream.WriteInt32L(iKey.Length());
    aStream.WriteL(iKey);
    aStream.WriteInt32L(iProtectedKey.Length());
    aStream.WriteL(iProtectedKey);
    aStream.WriteInt32L(iDigest.Length());
    aStream.WriteL(iDigest);
    aStream.WriteInt32L(iAuthenticationSeed.Length());
    aStream.WriteL(iAuthenticationSeed);
    aStream.WriteInt32L(iProtectedAuthSeed.Length());
    aStream.WriteL(iProtectedAuthSeed);
    }
    
EXPORT_C void CDRMAsset::InternalizeL(RReadStream& aStream)
    {
    TInt32 length;
    
    delete iUid;
    iUid = NULL;
    
    delete iParentRights;
    iParentRights = NULL;
    
    TPtr8 inRead(NULL,NULL);
       
    length = aStream.ReadInt32L();
    if (length != 0)
        {
        iUid = HBufC8::NewMaxL(length);
        inRead.Set(const_cast<TUint8*>(iUid->Ptr()), 0, length);
        aStream.ReadL(inRead);
        }

    length = aStream.ReadInt32L();
    if (length != 0)
        {
        iParentRights = HBufC8::NewMaxL(length);
        inRead.Set(const_cast<TUint8*>(iParentRights->Ptr()), 0, length);
        aStream.ReadL(inRead);       
        }

    length = aStream.ReadInt32L();
    iKey.SetLength(0);
    aStream.ReadL(iKey, length);

    length = aStream.ReadInt32L();
    iProtectedKey.SetLength(0);
    aStream.ReadL(iProtectedKey, length);

    length = aStream.ReadInt32L();
    iDigest.SetLength(0);
    aStream.ReadL(iDigest, length);

    length = aStream.ReadInt32L();
    iAuthenticationSeed.SetLength(0);
    aStream.ReadL(iAuthenticationSeed, length);

    length = aStream.ReadInt32L();
    iProtectedAuthSeed.SetLength(0);
    aStream.ReadL(iProtectedAuthSeed, length);

    }
    
// End of file
