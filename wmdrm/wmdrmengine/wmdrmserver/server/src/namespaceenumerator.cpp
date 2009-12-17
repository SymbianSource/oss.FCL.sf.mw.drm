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
* Description:  WMDRM Server implementation
*
*/


#include <e32math.h>
#include <symmetric.h>
#include <bacntf.h>

#include "namespaceenumerator.h"
#include "wmdrmserver.h"
#include "wmdrmdb.h"

#define _LOGGING_FILE L"wmdrmserver.txt"

#include "flogger.h"
#include "logfn.h"

CNameSpaceEnumerator* CNameSpaceEnumerator::NewL(
    CWmDrmServer* aServer,
    const TDesC8& aStore,
    const TDesC8& aNameSpace )
    {
    CNameSpaceEnumerator* self = new (ELeave) CNameSpaceEnumerator();
    CleanupStack::PushL( self );
    self->ConstructL( aServer, aStore, aNameSpace );
    CleanupStack::Pop( self );
    return self;
    }

void CNameSpaceEnumerator::ConstructL(
    CWmDrmServer* aServer,
    const TDesC8& aStore,
    const TDesC8& aNameSpace )
    {
    LOGFN( "CNameSpaceEnumerator::ConstructL" );
    iServer = aServer;
    iStore.Copy( aStore );
	iNamespace.Copy( aNameSpace );
    LOG( iNamespace );
    iServer->Db()->EnumerateNameSpaceStartL( iStore, iNamespace );
    }

TInt CNameSpaceEnumerator::NextL()
    {
    TInt r = KErrNone;
    LOGFN( "CNameSpaceEnumerator::Next");
	iServer->Db()->EnumerateNameSpaceNextL( iHashKey, iUniqueKey );
	LOG3( "Saved slot info %S, %S", &iHashKey, &iUniqueKey );
	return r;
	}
