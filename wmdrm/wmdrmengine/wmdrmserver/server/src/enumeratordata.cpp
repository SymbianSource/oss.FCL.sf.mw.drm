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


#include <ezlib.h>
#include <e32math.h>
#include <symmetric.h>
#include <bacntf.h>

#include "enumeratordata.h"
#include "slotenumeratorcache.h"
#include "wmdrmserver.h"
#include "wmdrmdb.h"

#define _LOGGING_FILE L"wmdrmserver.txt"

#include "flogger.h"
#include "logfn.h"

CEnumeratorData* CEnumeratorData::NewL(
    CWmDrmServer* aServer,
    const TDesC8& aStore,
    const TDesC8& aNamespace,
    const TDesC8& aHashKey )
    {
    CEnumeratorData* self = new (ELeave) CEnumeratorData();
    CleanupStack::PushL( self );
    self->ConstructL( aServer, aStore, aNamespace, aHashKey );
    CleanupStack::Pop( self );
    return self;
    }

void CEnumeratorData::ConstructL(
    CWmDrmServer* aServer,
    const TDesC8& aStore,
    const TDesC8& aNamespace,
    const TDesC8& aHashKey )
    {
    LOGFN( "CEnumeratorData::ConstructL" );
    iServer = aServer;
    iStore.Copy( aStore );
    iNamespace.Copy( aNamespace );
    iHashKey.Copy( aHashKey );
    iServer->Db()->EnumerateDataL( iStore, iNamespace, iHashKey, iEntries );
    }

CEnumeratorData::CEnumeratorData():
    iReferences( 0 )
    {
    LOGFN( "CEnumeratorData::CEnumeratorData" );
    }

CEnumeratorData::~CEnumeratorData()
    {
    LOGFN( "CEnumeratorData::~CEnumeratorData" );
    iEntries.ResetAndDestroy();
    iEntries.Close();
    }

void CEnumeratorData::Close()
    {
    LOGFN( "CEnumeratorData::Close" );
    iServer->EnumeratorCache()->Release( this );
    }
