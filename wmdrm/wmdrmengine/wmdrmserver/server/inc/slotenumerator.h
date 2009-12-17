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


#ifndef __SLOTENUMERATOR_H
#define __SLOTENUMERATOR_H

#include <e32base.h>
#include <f32file.h>
#include "wmdrmserver.h"
#include "enumerator.h"

class CEnumeratorData;

class CSlotEnumerator: public CEnumerator
    {
public:

    static CSlotEnumerator* NewL(
        CWmDrmServer* aServer,
        const TDesC8& aStore,
        const TDesC8& aNameSpace,
        const TDesC8& aHashKey );

    void ConstructL(
        CWmDrmServer* aServer,
        const TDesC8& aStore,
        const TDesC8& aNameSpace,
        const TDesC8& aHashKey );

    CSlotEnumerator();
    ~CSlotEnumerator();

    TInt OpenL();
    TInt NextL();
    void Close();

    TInt iPosition;
    CEnumeratorData* iEnumeratorData;
    };

#endif
