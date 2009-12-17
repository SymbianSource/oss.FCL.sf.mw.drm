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


#ifndef __SLOT_H
#define __SLOT_H

#include <e32base.h>
#include "wmdrmserver.h"

class CWmDrmServer;
class CEnumerator;
class CSlotData;

class CSlot: public CBase
    {
public:

    static CSlot* NewL( CWmDrmServer* aServer, const TDesC8& aMessageBuffer );

    static CSlot* NewL( CWmDrmServer* aServer, CEnumerator* aEnumerator );

    void ConstructL( CWmDrmServer* aServer, const TDesC8& aMessageBuffer );

    void ConstructL( CWmDrmServer* aServer, CEnumerator* aEnumerator );

    ~CSlot();
    
    void CreateL( TInt& aInitialSize );

    TInt OpenL( TInt& aCurrentSize );

    TInt SeekL( TInt& aOffset, TSeek aOrigin );

    TInt Read( TDes8& aBuffer );

    TInt WriteL( const TDesC8& aBuffer );

    TInt ResizeL( TInt aNewSize );

    TInt DeleteL();

    TInt Size();

    void Close();
    
    TBuf8<KMaxWmDrmStoreNameSize> iStore;
    TBuf8<KMaxWmDrmNamespaceNameSize> iNamespace;
    TBuf8<KWmDrmIdSize> iHashKey;
    TBuf8<KWmDrmIdSize> iUniqueKey;
	CSlotData* iData;
	TInt iDataPosition;
	
    };

#endif
