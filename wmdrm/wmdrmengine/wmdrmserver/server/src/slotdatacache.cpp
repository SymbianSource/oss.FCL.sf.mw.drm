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


#include "wmdrmserver.h"
#include "slotdata.h"
#include "slotdatacache.h"
#include "wmdrmkeystorage.h"
#include "wmdrmdb.h"

#define _LOGGING_FILE L"wmdrmserver.txt"

#include "flogger.h"
#include "logfn.h"


CSlotDataCache* CSlotDataCache::NewL( CWmDrmServer* aServer )
    {
    CSlotDataCache* self = new (ELeave) CSlotDataCache( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
    
void CSlotDataCache::ConstructL()
    {
    LOGFN( "CSlotDataCache::ConstructL" );
#ifndef __WINSCW__    
	iKeyStorage = CWmDrmKeyStorage::NewL();
    if ( !iKeyStorage )
        {
        User::Leave( KErrNotSupported );
        }
#endif // __WINSCW__
    iMaxCachedSlots = KDefaultMaxCachedSlots;
    }
    
CSlotDataCache::CSlotDataCache(
    CWmDrmServer* aServer):
    iServer( aServer )
    {
    }

CSlotDataCache::~CSlotDataCache()
    {
    iSlotCache.ResetAndDestroy();
    iSlotCache.Close();
    delete iKeyStorage;
    }

CSlotData* CSlotDataCache::GetSlotDataL( 
    const TDesC8& aStore,
    const TDesC8& aNamespace,
    const TDesC8& aHashKey,
    const TDesC8& aUniqueKey )
    {
    CSlotData* r = NULL;
    TInt i = 0;
    
    LOGFN( "CSlotDataCache::GetSlotDataL" );
    LOG( aNamespace );
    LOG( aHashKey );
    LOG( aUniqueKey );
    for ( i = 0; r == NULL && i < iSlotCache.Count(); i++ )
        {
        CSlotData* data = iSlotCache[i];
        if ( /*iSlotCache[i]->iStore == aStore &&*/
             iSlotCache[i]->iNamespace == aNamespace &&
             iSlotCache[i]->iHashKey == aHashKey && 
             iSlotCache[i]->iUniqueKey == aUniqueKey )
            {
            r = iSlotCache[i];
            Claim( r );
            }
        }
    if ( r == NULL )
        {
        Cleanup();
        r = CSlotData::NewL( iServer, 
                             aStore, 
                             aNamespace, 
                             aHashKey, 
                             aUniqueKey );
        CleanupStack::PushL( r );
        iSlotCache.AppendL( r );
        Claim( r );
        CleanupStack::Pop( r );
        }
    return r;
    }

void CSlotDataCache::Claim( CSlotData* aSlot )
    {
    LOGFN( "CSlotDataCache::ClaimL" );
    aSlot->iReferences++;
    }

TInt CSlotDataCache::Delete( CSlotData* aSlot )
    {
    TInt r = KErrNone;
    LOGFNR( "CSlotDataCache::Delete", r );
    aSlot->CloseFile();
    TRAP( r, iServer->Db()->DeleteRecordL( aSlot->iStore,
                                           aSlot->iNamespace,
                                           aSlot->iHashKey,
                                           aSlot->iUniqueKey  ) );
    return r;
	}
    
void CSlotDataCache::Release( CSlotData* aSlot )
    {
    TInt i = 0;
    
    LOGFN( "CSlotDataCache::ReleaseL" );
    while ( i < iSlotCache.Count() && iSlotCache[i] != aSlot )
        {
        i++;
        }
    if ( i < iSlotCache.Count() )
        {
        iSlotCache[i]->iReferences--;
        }
    }

void CSlotDataCache::Cleanup()
    {
    TInt i = 0;
    TInt minReferencesCount = 0;
    TInt minReferencesIndex = 0;
    
    LOGFN( "CSlotDataCache::CleanupL" );
    LOG3( "Used: %d, max: %d", iSlotCache.Count(), iMaxCachedSlots );
    
    if ( iSlotCache.Count() >= iMaxCachedSlots )
        {
        minReferencesCount = iSlotCache[minReferencesIndex]->iReferences;
        
        while ( iSlotCache.Count() >= iMaxCachedSlots && i < iMaxCachedSlots ) 
            {
            if ( iSlotCache[i]->iReferences == 0 )
                {
                LOG2( "Releasing %d", i );
                delete iSlotCache[i];
                iSlotCache.Remove( i );
                }
            else if ( iSlotCache[i]->iReferences < minReferencesCount )
                {
                minReferencesCount = iSlotCache[i]->iReferences;
                minReferencesIndex = i;
                }
            i++;
            }
    
        if ( iSlotCache.Count() >= iMaxCachedSlots ) 
            {
            LOG2( "Force Releasing %d", minReferencesIndex );
            delete iSlotCache[minReferencesIndex];
            iSlotCache.Remove( minReferencesIndex );
            }
        }
    }
    
void CSlotDataCache::FlushL()
    {
    TInt i;
    
    LOGFN( "CSlotDataCache::FlushL" );
    for ( i = 0; i < iSlotCache.Count(); i++ )
        {
        iSlotCache[i]->FlushL();
        }
    }
