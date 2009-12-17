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


#include "slotenumeratorcache.h"
#include "enumeratordata.h"
#include "wmdrmserver.h"

#define _LOGGING_FILE L"wmdrmserver.txt"

#include "flogger.h"
#include "logfn.h"

CSlotEnumeratorCache* CSlotEnumeratorCache::NewL( CWmDrmServer* aServer )
    {
    CSlotEnumeratorCache* self = new (ELeave) CSlotEnumeratorCache( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

void CSlotEnumeratorCache::ConstructL()
    {
    LOGFN( "CSlotEnumeratorCache::ConstructL" );
    iMaxCachedEnumerators = KDefaultMaxCachedEnumerators;
    }
    
CSlotEnumeratorCache::CSlotEnumeratorCache(
    CWmDrmServer* aServer):
    iServer( aServer )
    {
    LOGFN( "CSlotEnumeratorCache::CSlotEnumeratorCache" );
    }

CSlotEnumeratorCache::~CSlotEnumeratorCache()
    {
    LOGFN( "CSlotEnumeratorCache::~CSlotEnumeratorCache" );
    iEnumeratorCache.ResetAndDestroy();
    iEnumeratorCache.Close();
    }

CEnumeratorData* CSlotEnumeratorCache::GetEnumeratorDataL(
    const TDesC8& aStore,
    const TDesC8& aNamespace,
    const TDesC8& aHashKey )
    {
    CEnumeratorData* r = NULL;
    TInt i = 0;
    
    LOGFN( "CSlotEnumeratorCache::GetEnumeratorDataL" );
    LOG( aNamespace );
    LOG( aHashKey );
    for ( i = 0; r == NULL && i < iEnumeratorCache.Count(); i++ )
        {
        CEnumeratorData* data = iEnumeratorCache[i];
        if ( /*data->iStore == aStore &&*/
             data->iNamespace == aNamespace &&
             data->iHashKey == aHashKey )
            {
            r = iEnumeratorCache[i];
            Claim( r );
            }
        }
    if ( r == NULL )
        {
        Cleanup();
        r = CEnumeratorData::NewL( iServer, aStore, aNamespace, aHashKey );
        CleanupStack::PushL( r );
        iEnumeratorCache.AppendL( r );
        Claim( r );
        CleanupStack::Pop( r );
        }
    return r;
    }

void CSlotEnumeratorCache::Claim( CEnumeratorData* aData )
    {
    LOGFN( "CSlotEnumeratorCache::ClaimL" );
    aData->iReferences++;
    }

void CSlotEnumeratorCache::Release( CEnumeratorData* aData )
    {
    LOGFN( "CSlotEnumeratorCache::ReleaseL" );
    aData->iReferences--;
    }

TInt CSlotEnumeratorCache::AddEntryL(
    const TDesC8& /*aStore*/,
    const TDesC8& aNamespace,
    const TDesC8& aHashKey,
    const TDesC8& aUniqueKey )
    {
    TInt r = KErrNone;
    TInt i;
    CEnumeratorData* entry = NULL;
    HBufC8* fileName;
    
    LOGFNR( "CSlotEnumeratorCache::AddEntryL", r );
    LOG( aNamespace );
    LOG( aHashKey );
    LOG( aUniqueKey );
    for ( i = 0; entry == NULL && i < iEnumeratorCache.Count(); i++ )
        {
        CEnumeratorData* data = iEnumeratorCache[i];
        if ( /*data->iStore == aStore &&*/
             data->iNamespace == aNamespace &&
             data->iHashKey == aHashKey )
            {
            entry = iEnumeratorCache[i];
            }
        }
    if ( entry != NULL )
        {
        i = 0;
        while ( i < entry->iEntries.Count() && *entry->iEntries[i] != aUniqueKey )
            {
            i++;
            }
        if ( i == entry->iEntries.Count() )
            {
            fileName = aUniqueKey.AllocLC();
            entry->iEntries.AppendL( fileName );
            CleanupStack::Pop( fileName );
            }
        }
    return r;
    }

TInt CSlotEnumeratorCache::DeleteEntryL(
    const TDesC8& /*aStore*/,
    const TDesC8& aNamespace,
    const TDesC8& aHashKey,
    const TDesC8& aUniqueKey )
    {
    TInt r = KErrNone;
    TInt i;
    CEnumeratorData* entry = NULL;
    
    LOGFNR( "CSlotEnumeratorCache::DeleteEntryL", r );
    
    for ( i = 0; entry == NULL && i < iEnumeratorCache.Count(); i++ )
        {
        CEnumeratorData* data = iEnumeratorCache[i];
        if ( /*data->iStore == aStore &&*/
             data->iNamespace == aNamespace &&
             data->iHashKey == aHashKey )
            {
            entry = iEnumeratorCache[i];
            }
        }
    if ( entry != NULL )
        {
        i = 0;
        while ( i < entry->iEntries.Count() && *entry->iEntries[i] != aUniqueKey )
            {
            i++;
            }
        if ( i < entry->iEntries.Count() )
            {
            delete entry->iEntries[i];
            entry->iEntries.Remove( i );
            }
        }
    return r;
    }

void CSlotEnumeratorCache::Cleanup()
    {
    TInt i = 0;
    TInt minReferencesCount = 0;
    TInt minReferencesIndex = 0;
    
    LOGFN( "CSlotEnumeratorCache::CleanupL" );
    LOG3( "Used: %d, max: %d", iEnumeratorCache.Count(), iMaxCachedEnumerators );
    
    if ( iEnumeratorCache.Count() >= iMaxCachedEnumerators )
        {
        minReferencesCount = iEnumeratorCache[minReferencesIndex]->iReferences;
        
        while ( iEnumeratorCache.Count() >= iMaxCachedEnumerators && i < iMaxCachedEnumerators ) 
            {
            if ( iEnumeratorCache[i]->iReferences == 0 )
                {
                LOG2( "Releasing %d", i );
                delete iEnumeratorCache[i];
                iEnumeratorCache.Remove( i );
                }
            else if ( iEnumeratorCache[i]->iReferences < minReferencesCount )
                {
                minReferencesCount = iEnumeratorCache[i]->iReferences;
                minReferencesIndex = i;
                }
            i++;
            }
    
        if ( iEnumeratorCache.Count() >= iMaxCachedEnumerators ) 
            {
            LOG2( "Force Releasing %d", minReferencesIndex );
            delete iEnumeratorCache[minReferencesIndex];
            iEnumeratorCache.Remove( minReferencesIndex );
            }
        }
    }
