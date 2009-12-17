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
* Description:  Server definitions
*
*/


#ifndef __SLOTDATACACHE_H
#define __SLOTDATACACHE_H

#include <e32base.h>
#include <f32file.h>

class CWmDrmKeyStorage;
class CWmDrmServer;
class CSlotData;

class CSlotDataCache: public CBase
    {
public:
    
    static CSlotDataCache* NewL( CWmDrmServer* aServer );

    void ConstructL();
    
    CSlotDataCache( CWmDrmServer* aServer );
    
    ~CSlotDataCache();
    

    CSlotData* GetSlotDataL( 
        const TDesC8& aStore,
        const TDesC8& aNamespace,
        const TDesC8& aHashKey,
        const TDesC8& aUniqueKey );

    void Claim( CSlotData* aData );

    void Release( CSlotData* aData );

    TInt Delete( CSlotData* aData );

    void Cleanup();

    void FlushL();
    
    TInt iMaxCachedSlots;
	CWmDrmKeyStorage* iKeyStorage;    
    RPointerArray<CSlotData> iSlotCache;
    CWmDrmServer* iServer;
    };

#endif
