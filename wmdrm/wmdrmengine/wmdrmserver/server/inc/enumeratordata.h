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


#ifndef __ENUMERATORDATA_H
#define __ENUMERATORDATA_H

#include <e32base.h>
#include <f32file.h>
#include "wmdrmserver.h"

class CEnumeratorData: public CBase
    {
public:

    static CEnumeratorData* NewL( 
        CWmDrmServer* aServer, 
        const TDesC8& aStore,
        const TDesC8& aNamespace,
        const TDesC8& aHashKey );

    void ConstructL( 
        CWmDrmServer* aServer, 
        const TDesC8& aStore,
        const TDesC8& aNamespace,
        const TDesC8& aHashKey );

    CEnumeratorData();
    ~CEnumeratorData();

    void Close();
    
	CWmDrmServer* iServer;
	TInt iReferences;
	TBuf8<KMaxWmDrmStoreNameSize> iStore;
    TBuf8<KMaxWmDrmNamespaceNameSize> iNamespace;
    TBuf8<KWmDrmIdSize> iHashKey;
    RPointerArray<HBufC8> iEntries;
    };
    
#endif
