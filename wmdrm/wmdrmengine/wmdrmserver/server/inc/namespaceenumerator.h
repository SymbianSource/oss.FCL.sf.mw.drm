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


#ifndef __NAMESPACEENUMERATOR_H
#define __NAMESPACEENUMERATOR_H

#include <e32base.h>
#include <f32file.h>
#include "wmdrmserver.h"
#include "enumerator.h"

class CNameSpaceEnumerator: public CEnumerator
    {
public:

    static CNameSpaceEnumerator* NewL(
        CWmDrmServer* aServer,
        const TDesC8& aStore,
        const TDesC8& aNameSpace );

    void ConstructL(
        CWmDrmServer* aServer,
        const TDesC8& aStore,
        const TDesC8& aNameSpace );

    TInt NextL();
    
    };

#endif
