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


#ifndef __ENUMERATOR_H
#define __ENUMERATOR_H

#include <e32base.h>
#include <f32file.h>
#include "wmdrmserver.h"

class CEnumerator: public CBase
    {
public:

    static CEnumerator* NewL(
        CWmDrmServer* aServer,
        const TDesC8& aMessageBuffer );
    
    virtual ~CEnumerator();

	virtual TInt OpenL();
	virtual TInt NextL() = 0;
	virtual void Close();

    void GetHashKey( TDes8& aHashKey );

    void GetUniqueKey( TDes8& aHashKey );

    void DeleteCurrentL();

    TBuf8<KMaxWmDrmStoreNameSize> iStore;
    TBuf8<KMaxWmDrmNamespaceNameSize> iNamespace;
    TBuf8<KWmDrmIdSize> iHashKey;
    TBuf8<KWmDrmIdSize> iUniqueKey;
    CWmDrmServer* iServer;
    };
    
#endif
