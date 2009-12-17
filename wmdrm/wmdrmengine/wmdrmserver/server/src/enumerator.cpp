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

#include "enumerator.h"
#include "namespaceenumerator.h"
#include "enumeratordata.h"
#include "slotenumerator.h"
#include "wmdrmserver.h"
#include "wmdrmdb.h"

#define _LOGGING_FILE L"wmdrmserver.txt"

#include "flogger.h"
#include "logfn.h"

_LIT8( KZeroID, "0000000000000000" );

LOCAL_C void ExtractEnumeratorComponentsL(
    const TDesC8& aMessageBuffer,
    TDes8& aStore,
    TDes8& aNamespace,
    TDes8& aHashKey )
    {
    TInt len;
    TInt offset;
    TInt total = aMessageBuffer.Length();

	offset = 0;
	len = aMessageBuffer[offset];
	if ( len <= 0 || len > KMaxWmDrmStoreNameSize || offset + len + 1> total )
	    {
	    User::Leave( KErrArgument );
	    }
    aStore.Copy( aMessageBuffer.Mid( offset + 1, len ) );
    
    offset += len + 1;
    if ( offset >= total )
        {
        User::Leave( KErrArgument );
        }
	len = aMessageBuffer[offset];
	if ( len <= 0 || len > KMaxWmDrmNamespaceNameSize || offset + len + 1 > total )
	    {
	    User::Leave( KErrArgument );
	    }
	aNamespace.Copy( aMessageBuffer.Mid( offset + 1, len ) );
	    
    offset += len + 1;
	len = aMessageBuffer[offset];
    if ( offset >= total )
        {
        User::Leave( KErrArgument );
        }
	if ( ( len > 0 && len != KWmDrmIdSize ) || offset + len + 1 > total )
	    {
	    User::Leave( KErrArgument );
	    }
	else if ( len > 0 )
	    {
	    aHashKey.Copy( aMessageBuffer.Mid( offset + 1, len ) );
	    }
	else
	    {
	    aHashKey.SetLength( 0 );
	    }
    }

CEnumerator* CEnumerator::NewL(
    CWmDrmServer* aServer,
    const TDesC8& aMessageBuffer )	
    {
    TBuf8<KMaxWmDrmNamespaceNameSize> nameSpace;
    TBuf8<KMaxWmDrmStoreNameSize> store;
    TBuf8<KWmDrmIdSize> hashKey;
    CEnumerator* self = NULL;

    LOGFN( "CEnumerator::NewL" );
    ExtractEnumeratorComponentsL( aMessageBuffer, store, nameSpace, hashKey );
    if ( hashKey.Length() > 0 )
        {
        self = CSlotEnumerator::NewL( aServer, store, nameSpace, hashKey );
        }
    else
        {
        self = CNameSpaceEnumerator::NewL( aServer, store, nameSpace );
        }
    return self;
    }

CEnumerator::~CEnumerator()
    {
    LOGFN( "CEnumerator::~CEnumerator" );
    Close();
    }

void CEnumerator::GetHashKey( TDes8& aKey )
    {
    LOGFN( "CEnumerator::GetHashKey" );
    aKey.SetLength( 0 );
    aKey.Copy( iHashKey );
    }

void CEnumerator::GetUniqueKey( TDes8& aKey )
    {
    LOGFN( "CEnumerator::GetUniqueKey" );
    aKey.SetLength( 0 );
    if ( iUniqueKey.CompareC( KZeroID ) == 0 )
        {
        aKey.Fill( '\0', KWmDrmIdSize );
        }
    else
        {
        aKey.Copy( iUniqueKey );
        }
    }

void CEnumerator::DeleteCurrentL()
    {
    LOGFN( "CEnumerator::DeleteCurrentL" );
	iServer->Db()->DeleteRecordL( iStore,
	                              iNamespace,
	                              iHashKey,
	                              iUniqueKey  );
	}
	
TInt CEnumerator::OpenL()
    {
    TInt r = KErrNone;
    LOGFNR( "CEnumerator::Open", r );
	return r;
    }
	
void CEnumerator::Close()
    {
	LOGFN( "CEnumerator::Close" );
    }
