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
* Description:  Declaration of the slot enumerator class
*
*/


#ifndef __SLOTENUMERATORCACHE_H
#define __SLOTENUMERATORCACHE_H

#include <e32base.h>
#include <f32file.h>

class CEnumeratorData;
class CWmDrmServer;

class CSlotEnumeratorCache: public CBase
    {
public:
    
    static CSlotEnumeratorCache* NewL( CWmDrmServer* aServer );

    void ConstructL();

    CSlotEnumeratorCache( CWmDrmServer* aServer );

    ~CSlotEnumeratorCache();

    CEnumeratorData* GetEnumeratorDataL( 
        const TDesC8& aStore,
        const TDesC8& aNamespace,
        const TDesC8& aHashKey );

    void Claim( CEnumeratorData* aData );

    void Release( CEnumeratorData* aData );

    TInt AddEntryL( 
        const TDesC8& aStore,
        const TDesC8& aNamespace,
        const TDesC8& aHashKey,
        const TDesC8& aUniqueKey );

    TInt DeleteEntryL( 
        const TDesC8& aStore,
        const TDesC8& aNamespace,
        const TDesC8& aHashKey,
        const TDesC8& aUniqueKey );

    void Cleanup();
        
    TInt iMaxCachedEnumerators;
    RPointerArray<CEnumeratorData> iEnumeratorCache;
    CWmDrmServer* iServer;
    };
	
#endif
