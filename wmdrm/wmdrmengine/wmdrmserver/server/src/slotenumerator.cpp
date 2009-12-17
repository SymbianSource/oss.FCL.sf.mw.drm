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


#include <e32std.h>

#include "wmdrmserver.h"
#include "slotenumerator.h"
#include "enumeratordata.h"
#include "slotenumeratorcache.h"
#include "wmdrmdb.h"

#define _LOGGING_FILE L"wmdrmserver.txt"

#include "flogger.h"
#include "logfn.h"

CSlotEnumerator* CSlotEnumerator::NewL(
    CWmDrmServer* aServer,
    const TDesC8& aStore,
    const TDesC8& aNameSpace,
    const TDesC8& aHashKey )
    {
    CSlotEnumerator* self = new (ELeave) CSlotEnumerator();
    CleanupStack::PushL( self );
    self->ConstructL( aServer, aStore, aNameSpace, aHashKey );
    CleanupStack::Pop( self );
    return self;
    }

void CSlotEnumerator::ConstructL(
    CWmDrmServer* aServer,
    const TDesC8& aStore,
    const TDesC8& aNameSpace,
    const TDesC8& aHashKey )
    {
    LOGFN( "CSlotEnumerator::ConstructL" );
    iServer = aServer;
    iStore.Copy( aStore );
	iNamespace.Copy( aNameSpace );
	LOG( iNamespace );
	iHashKey.Copy( aHashKey );
    LOG( iHashKey );
    }

CSlotEnumerator::CSlotEnumerator():
    iPosition( 0 ),
    iEnumeratorData( NULL )
    {
    }

CSlotEnumerator::~CSlotEnumerator()
    {
    if ( iEnumeratorData != NULL )
        {
        iEnumeratorData->Close();
        }
    }

TInt CSlotEnumerator::OpenL()
    {
    TInt r = KErrNone;
    
	LOGFNR( "CSlotEnumerator::Open", r );
	iEnumeratorData = iServer->EnumeratorCache()->GetEnumeratorDataL( iStore, iNamespace, iHashKey );
    LOG2( "Slotenumerator entries %d", iEnumeratorData->iEntries.Count() );
    iPosition = 0;
    return r;
    }
	
TInt CSlotEnumerator::NextL()
    {
    TInt r = KErrNone;
    
	LOGFNR( "CSlotEnumerator::Next", r );
	if ( iEnumeratorData != NULL && iPosition < iEnumeratorData->iEntries.Count() )
	    {
        iHashKey.Copy( iEnumeratorData->iHashKey  );
        iUniqueKey.Copy( *iEnumeratorData->iEntries[iPosition] );
        iPosition++;
        LOG3( "Saved slot info %S, %S", &iHashKey, &iUniqueKey );
        }
    else
        {
        LOG1( "Enumerate finished" );
        r = KErrNotFound;
        Close();
        }
    return r;
    }
	
void CSlotEnumerator::Close()
    {
	LOGFN( "CSlotEnumerator::Close" );
	if ( iEnumeratorData != NULL )
	    {
        iEnumeratorData->Close();
        iEnumeratorData = NULL;
        }
    }
	
