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
#include "slot.h"
#include "slotdatacache.h"
#include "enumerator.h"
#include "slotdata.h"
#include "wmdrmserver.h"

#define _LOGGING_FILE L"wmdrmserver.txt"

#include "flogger.h"
#include "logfn.h"

#ifdef __WINSCW__
_LIT8( KDummyKey, "0123456789012345" );
#endif

_LIT8( KEmptyID, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" );

LOCAL_C void ExtractSlotComponentsL(
    const TDesC8& aMessageBuffer,
    TDes8& aStore,
    TDes8& aNamespace,
    TDes8& aHashKey,
    TDes8& aUniqueKey )
    {
    TInt len;
    TInt offset;
    TInt total = aMessageBuffer.Length();

	offset = 0;
	len = aMessageBuffer[offset];
	if ( len <= 0 || len > KMaxWmDrmStoreNameSize || offset + len + 1 > total )
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
    if ( offset >= total )
        {
        User::Leave( KErrArgument );
        }
	len = aMessageBuffer[offset];
	if ( len != KWmDrmIdSize || offset + len + 1 > total )
	    {
	    User::Leave( KErrArgument );
	    }
	aHashKey.Copy( aMessageBuffer.Mid( offset + 1, len ) );

    offset += len + 1;
    if ( offset >= total )
        {
        User::Leave( KErrArgument );
        }
	len = aMessageBuffer[offset];
	if ( len != KWmDrmIdSize || offset + len + 1 > total )
	    {
	    User::Leave( KErrArgument );
	    }
	aUniqueKey.Copy( aMessageBuffer.Mid( offset + 1, len ) );
	if ( aUniqueKey.CompareC( KEmptyID ) == 0 )
	    {
	    aUniqueKey.Fill( '0' );
	    }
	}
    
CSlot* CSlot::NewL( CWmDrmServer* aServer, const TDesC8& aMessageBuffer )
    {
    CSlot* self = new (ELeave) CSlot();
    CleanupStack::PushL( self );
    self->ConstructL( aServer, aMessageBuffer );
    CleanupStack::Pop( self );
    return self;
    }
    
CSlot* CSlot::NewL( CWmDrmServer* aServer, CEnumerator* aEnumerator )
    {
    CSlot* self = new (ELeave) CSlot();
    CleanupStack::PushL( self );
    self->ConstructL( aServer, aEnumerator );
    CleanupStack::Pop( self );
    return self;
    }
    
void CSlot::ConstructL(
    CWmDrmServer* aServer,
    const TDesC8& aMessageBuffer )
    {
    LOGFN( "CSlot::ConstructL" );
	ExtractSlotComponentsL( aMessageBuffer, iStore, iNamespace, iHashKey, iUniqueKey );
    iData = aServer->Cache()->GetSlotDataL( iStore, iNamespace, iHashKey, iUniqueKey );
	iDataPosition = 0;
    }

void CSlot::ConstructL(
    CWmDrmServer* aServer,
    CEnumerator* aEnumerator )
    {
    LOGFN( "CSlot::ConstructL (2)" );
    aEnumerator->GetHashKey( iHashKey );
    aEnumerator->GetUniqueKey( iUniqueKey );
    iStore.Copy( aEnumerator->iStore );
    iNamespace.Copy( aEnumerator->iNamespace );
    iData = aServer->Cache()->GetSlotDataL( iStore, iNamespace, iHashKey, iUniqueKey );
	iDataPosition = 0;
    }
    

CSlot::~CSlot()
    {
    LOGFN( "CSlot::~CSlot" );
    if ( iData != NULL )
        {
        iData->Close();
        }
    }

void CSlot::CreateL( TInt& aInitialSize )
    {
	LOGFN( "CSlot::CreateL" );
	iData->CreateL( aInitialSize );
    }
    
TInt CSlot::OpenL( TInt& aCurrentSize )
    {
    TInt r = KErrNone;
    
	LOGFNR( "CSlot::Open", r );
	r = iData->OpenL( aCurrentSize );
	iDataPosition = 0;
	return r;
    }
    
TInt CSlot::SeekL( TInt& aOffset, TSeek aOrigin )
    {
    TInt r = KErrNone;
    TInt size;

	LOGFNR( "CSlot::Seek", r );
	if ( iData == NULL )
	    {
	    User::Leave( KErrNotReady );
	    }
	size = iData->Size();
	switch ( aOrigin )
	    {
	    case ESeekStart:
	        iDataPosition = aOffset;
	        break;
	    case ESeekEnd:
	        iDataPosition = size + aOffset;
	        break;
	    case ESeekCurrent:
	        iDataPosition += aOffset;
	        break;
	    }
    iDataPosition = Max( iDataPosition, 0 );
    iDataPosition = Min( iDataPosition, size );
	LOG5( "Origin: %d, offset: %d, new pos: %d (size: %d)", aOrigin, aOffset, iDataPosition, size );
	aOffset = iDataPosition;
    return r;
    }
    
TInt CSlot::Read( TDes8& aBuffer )
    {
    TInt r = KErrNone;

	LOGFNR( "CSlot::Read", r );
	if ( iData == NULL )
	    {
	    r = KErrNotReady;
	    }
	else
	    {
        r = iData->Read( iDataPosition, aBuffer );
        }
    if ( r == KErrNone )
        {
        iDataPosition += aBuffer.Length();
        }
    return r;
    }
    
TInt CSlot::WriteL( const TDesC8& aBuffer )
    {
    TInt r = KErrNone;

	LOGFNR( "CSlot::Write", r );
	r = iData->WriteL( iDataPosition, aBuffer );
    if ( r == KErrNone )
        {
        iDataPosition += aBuffer.Length();
        }
    return r;
    }
    
TInt CSlot::ResizeL( TInt aNewSize )
    {
    TInt r = KErrNone;
	LOGFNR( "CSlot::Resize", r );

	if ( iData == NULL )
	    {
	    r = KErrNotReady;
	    }
	else
	    {
        iData->ResizeL( aNewSize );
        if ( iDataPosition > aNewSize - 1 )
            {
            iDataPosition = aNewSize - 1;
            }
        }
    return r;
    }
    
TInt CSlot::DeleteL()
    {
    TInt r = KErrNone;
    
	LOGFNR( "CSlot::Delete", r );
	if ( iData == NULL )
	    {
	    r = KErrNotReady;
	    }
	else
	    {
    	r = iData->DeleteL();
    	}
	return r;
    }

TInt CSlot::Size()
    {
    TInt r = KErrNone;
    
	LOGFNR( "CSlot::Size", r );
	if ( iData == NULL )
	    {
	    r = KErrNotReady;
	    }
	else
	    {
    	r = iData->Size();
    	}
	return r;
    }

void CSlot::Close()
    {
	LOGFN( "CSlot::Close" );
	if ( iData != NULL )
        {
        iData->Close();
        iData = NULL;    
        }
    }
