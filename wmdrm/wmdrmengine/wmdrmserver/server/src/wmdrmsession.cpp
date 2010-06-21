/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include <bacntf.h>
#include "wmdrmsession.h"
#include "wmdrmserver.h"
#include "clock.h"
#include "slotdatacache.h"
#include "slotenumeratorcache.h"
#include "slot.h"
#include "slotdata.h"
#include "enumerator.h"
#include "enumeratordata.h"
#include "wmdrmdatastore.h"
#include "wmdrmdb.h"
#include "drmrightsstoringlocation.h"
#include "drmutilityinternaltypes.h"

#define _LOGGING_FILE L"wmdrmserver.txt"

#include "flogger.h"
#include "logfn.h"

//---------------------------------------------------------------------------
// CWmDrmSession::CWmDrmSession
// Constructor
//---------------------------------------------------------------------------
//
CWmDrmSession::CWmDrmSession()
	{
	}

//---------------------------------------------------------------------------
// CWmDrmSession::~CWmDrmSession
// Destructor
//---------------------------------------------------------------------------
//	
CWmDrmSession::~CWmDrmSession()
	{
	LOGFN( "CWmDrmSession::~CWmDrmSession" );
    //Server().Cache()->FlushL();
	delete iEnumerator;
	iEnumerator = NULL;
    delete iSlot;
	iSlot = NULL;
	}

//---------------------------------------------------------------------------
// CWmDrmSession::InitStoreL
//---------------------------------------------------------------------------
//
TInt CWmDrmSession::InitStoreL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    TBuf8<KMaxWmDrmStoreNameSize> store;
    
    LOGFNR( "CWmDrmSession::InitStoreL", r );
    if ( aMessage.GetDesLength( 0 ) > KMaxWmDrmStoreNameSize || aMessage.GetDesLength( 0 ) <= 0 )
        {
        User::Leave( KErrArgument );
        }
    aMessage.ReadL( 0, store );
    TRAP( r, Server().Db()->InitStoreL( store, aMessage.Int1() ) );
    if ( r == KErrAlreadyExists )
        {
        r = KErrNone;
        }
    return r;
    }

//---------------------------------------------------------------------------
// CWmDrmSession::RemoveStoreL
//---------------------------------------------------------------------------
//    
TInt CWmDrmSession::RemoveStoreL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    TBuf8<KMaxWmDrmStoreNameSize> store;
    
    LOGFNR( "CWmDrmSession::RemoveStoreL", r );
    if ( aMessage.GetDesLength( 0 ) > KMaxWmDrmStoreNameSize || aMessage.GetDesLength( 0 ) <= 0  )
        {
        User::Leave( KErrArgument );
        }
    Server().ResetCacheL();
    aMessage.ReadL( 0, store );
    Server().Db()->RemoveStoreL( store );
    return r;
    }

//---------------------------------------------------------------------------
// CWmDrmSession::InitNamespaceL
//---------------------------------------------------------------------------
//    
TInt CWmDrmSession::InitNamespaceL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    TBuf8<KMaxWmDrmStoreNameSize> store;
    TBuf8<KMaxWmDrmNamespaceNameSize> nameSpace;
    
    LOGFNR( "CWmDrmSession::InitNamespaceL", r );
    // :KLUDGE: Note: store length of zero is allowed
    if ( aMessage.GetDesLengthL( 0 ) > KMaxWmDrmStoreNameSize ||
        aMessage.GetDesLengthL( 1 ) > KMaxWmDrmNamespaceNameSize ||
        aMessage.GetDesLength( 1 ) <= 0 )
        {
        User::Leave( KErrArgument );
        }

    if ( aMessage.GetDesLengthL( 0 ) > 0 )
        {
        aMessage.ReadL( 0, store );
        }
    else
        {
        // Use hds as the default store if none given
        store.Copy( KDefaultStore );
        }
    aMessage.ReadL( 1, nameSpace );
    Server().Db()->InitNameSpaceL( store, nameSpace, aMessage.Int1() );
    return r;
    }

//---------------------------------------------------------------------------
// CWmDrmSession::RemoveNamespaceL
//---------------------------------------------------------------------------
//    
TInt CWmDrmSession::RemoveNamespaceL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    TBuf8<KMaxWmDrmStoreNameSize> store;
    TBuf8<KMaxWmDrmNamespaceNameSize> nameSpace;
    
    LOGFNR( "CWmDrmSession::RemoveNamespaceL", r );
    // :KLUDGE: Note: store length of zero is allowed
    if ( aMessage.GetDesLengthL( 0 ) > KMaxWmDrmStoreNameSize ||
        aMessage.GetDesLengthL( 1 ) > KMaxWmDrmNamespaceNameSize ||
        aMessage.GetDesLength( 1 ) <= 0 )
        {
        User::Leave( KErrArgument );
        }

    if ( aMessage.GetDesLengthL( 0 ) > 0 )
        {
        aMessage.ReadL( 0, store );
        }
    else
        {
        // :KLUDGE: Use hds as the default store if none given
        store.Copy( KDefaultStore );
        }
    aMessage.ReadL( 1, nameSpace );
    Server().Db()->RemoveNameSpaceL( store, nameSpace );
    return r;
    }

//---------------------------------------------------------------------------
// CWmDrmSession::SlotOpenL
//---------------------------------------------------------------------------
//    
TInt CWmDrmSession::SlotOpenL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    RBuf8 messageBuffer;
	TInt size;
    
    LOGFNR( "CWmDrmSession::SlotOpenL", r );
    if ( aMessage.GetDesLength( 0 ) > KMaxSlotNameSize || aMessage.GetDesLength( 0 ) <= 0 )
        {
        User::Leave( KErrArgument );
        }
    messageBuffer.CreateL( aMessage.GetDesLengthL( 0 ) );
    messageBuffer.CleanupClosePushL();
    aMessage.ReadL( 0, messageBuffer );
    if ( iSlot != NULL )
        {
        delete iSlot;
        iSlot = NULL;
        }
    iSlot = CSlot::NewL( &Server(), messageBuffer );
    r = iSlot->OpenL( size );
    aMessage.WriteL( 1, TPckg<TInt>( size ) );
    CleanupStack::PopAndDestroy( &messageBuffer );
    return r;
    }

//---------------------------------------------------------------------------
// CWmDrmSession::SlotCreateL
//---------------------------------------------------------------------------
//    
TInt CWmDrmSession::SlotCreateL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    RBuf8 messageBuffer;
	TInt size;
    
    LOGFNR( "CWmDrmSession::SlotCreateL", r );
    if ( aMessage.GetDesLength( 0 ) > KMaxSlotNameSize || aMessage.GetDesLength( 0 ) <= 0  )
        {
        User::Leave( KErrArgument );
        }
    messageBuffer.CreateL( aMessage.GetDesLengthL( 0 ) );
    messageBuffer.CleanupClosePushL();
    aMessage.ReadL( 0, messageBuffer );
    size = aMessage.Int1();
    if ( iSlot != NULL )
        {
        delete iSlot;
        iSlot = NULL;
        }
    
    iSlot = CSlot::NewL( &Server(), messageBuffer );
    iSlot->CreateL( size );
    CleanupStack::PopAndDestroy( &messageBuffer );
    return r;
    }

//---------------------------------------------------------------------------
// CWmDrmSession::SlotSeekL
//---------------------------------------------------------------------------
//    
TInt CWmDrmSession::SlotSeekL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    TSeek whence = ESeekStart;
    TInt pos = aMessage.Int0();
    TInt size;
    
    LOGFNR( "CWmDrmSession::SlotSeekL", r );
    if ( iSlot == NULL )
        {
        User::Leave( KErrNotReady );
        }
    switch ( aMessage.Int1() )
        {
        case ESeekStart:
            whence = ESeekStart;
            break;
        case ESeekCurrent:
            whence = ESeekCurrent;
            break;
        case ESeekEnd:
            whence = ESeekEnd;
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }
    r = iSlot->SeekL( pos, whence );
    if ( r == KErrNone && whence == ESeekStart )
        {
        size = iSlot->Size();
        if ( pos >= size )
            {
            pos = 0;
            r = iSlot->SeekL( pos, ESeekEnd );
            }
        }
    aMessage.WriteL( 2, TPckg<TInt>( pos ) );
    return r;
    }

//---------------------------------------------------------------------------
// CWmDrmSession::SlotReadL
//---------------------------------------------------------------------------
//    
TInt CWmDrmSession::SlotReadL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    RBuf8 buf;
    TInt amountRead = 0;
    
    LOGFNR( "CWmDrmSession::SlotReadL", r );
    if ( iSlot == NULL )
        {
        User::Leave( KErrNotReady );
        }
    buf.CreateL( aMessage.GetDesMaxLengthL( 0 ) );
    buf.CleanupClosePushL();
    r = iSlot->Read( buf );
    if ( r == KErrNone )
        {
        aMessage.WriteL( 0, buf );
        amountRead = buf.Size();
        }
    aMessage.WriteL( 1, TPckg<TInt>( amountRead ) );
    CleanupStack::PopAndDestroy( &buf );
    return r;
    }

//---------------------------------------------------------------------------
// CWmDrmSession::SlotWriteL
//---------------------------------------------------------------------------
//    
TInt CWmDrmSession::SlotWriteL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    RBuf8 buf;
    
    LOGFNR( "CWmDrmSession::SlotWriteL", r );
    if ( iSlot == NULL )
        {
        User::Leave( KErrNotReady );
        }
    buf.CreateL( aMessage.GetDesLengthL( 0 ) );
    buf.CleanupClosePushL();
    aMessage.ReadL( 0, buf );
    r = iSlot->WriteL( buf );
    CleanupStack::PopAndDestroy( &buf );
    return r;
    }
    
TInt CWmDrmSession::SlotResizeL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    
    LOGFNR( "CWmDrmSession::SlotResizeL", r );
    if ( iSlot == NULL )
        {
        User::Leave( KErrNotReady );
        }
    r = iSlot->ResizeL( aMessage.Int0() );
    return r;
    }

//---------------------------------------------------------------------------
// CWmDrmSession::SlotDeleteL
//---------------------------------------------------------------------------
//    
TInt CWmDrmSession::SlotDeleteL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    RBuf8 messageBuffer;
    
    LOGFNR( "CWmDrmSession::SlotDeleteL", r );
    if ( aMessage.GetDesLength( 0 ) > KMaxSlotNameSize )
        {
        User::Leave( KErrArgument );
        }
    messageBuffer.CreateL( aMessage.GetDesLengthL( 0 ) );
    messageBuffer.CleanupClosePushL();
    aMessage.ReadL( 0, messageBuffer );
    if ( iSlot != NULL )
        {
        delete iSlot;
        iSlot = NULL;
        }
    iSlot = CSlot::NewL( &Server(), messageBuffer );
    r = iSlot->DeleteL();
    delete iSlot;
    iSlot = NULL;
    CleanupStack::PopAndDestroy( &messageBuffer );
    return r;
    }

//---------------------------------------------------------------------------
// CWmDrmSession::SlotCloseL
//---------------------------------------------------------------------------
//    
TInt CWmDrmSession::SlotCloseL( const RMessage2& /*aMessage*/ )
    {
    TInt r = KErrNone;
    
    LOGFNR( "CWmDrmSession::SlotCloseL", r );
    if ( iSlot == NULL )
        {
        User::Leave( KErrNotReady );
        }
    delete iSlot;
    iSlot = NULL;
    return r;
    }

//---------------------------------------------------------------------------
// CWmDrmSession::EnumerateStartL
//---------------------------------------------------------------------------
//    
TInt CWmDrmSession::EnumerateStartL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    RBuf8 messageBuffer;
    
    LOGFNR( "CWmDrmSession::EnumerateStartL", r );
    if ( aMessage.GetDesLength( 0 ) > KMaxSlotNameSize || aMessage.GetDesLength( 0 ) <= 0  )
        {
        User::Leave( KErrArgument );
        }
    messageBuffer.CreateL( aMessage.GetDesLengthL( 0 ) );
    messageBuffer.CleanupClosePushL();
    aMessage.ReadL( 0, messageBuffer );
    if ( iSlot != NULL )
        {
        delete iSlot;
        iSlot = NULL;
        }
    if ( iEnumerator != NULL )
        {
        delete iEnumerator;
        iEnumerator = NULL;
        }
    iEnumerator = CEnumerator::NewL( &Server(), messageBuffer );
    r = iEnumerator->OpenL();
    CleanupStack::PopAndDestroy( &messageBuffer );
    return r;
    }

//---------------------------------------------------------------------------
// CWmDrmSession::EnumerateReloadL
//---------------------------------------------------------------------------
//    
TInt CWmDrmSession::EnumerateReloadL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    TBuf8<KWmDrmIdSize> hash;
    TBuf8<KWmDrmIdSize> id;
    
    LOGFNR( "CWmDrmSession::EnumerateReloadL", r );
    if ( iEnumerator == NULL )
        {
        User::Leave( KErrNotReady );
        }
    iEnumerator->GetHashKey( hash );
    LOGHEX( hash.Ptr(), hash.Size() );
    iEnumerator->GetUniqueKey( id );
    LOGHEX( id.Ptr(), id.Size() );
    aMessage.WriteL( 0, iEnumerator->iStore);
    aMessage.WriteL( 1, iEnumerator->iNamespace );
    aMessage.WriteL( 2, hash );
    aMessage.WriteL( 3, id );
    return r;
    }

//---------------------------------------------------------------------------
// CWmDrmSession::EnumerateNextL
//---------------------------------------------------------------------------
//    
TInt CWmDrmSession::EnumerateNextL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    TBuf8<KWmDrmIdSize> hash;
    TBuf8<KWmDrmIdSize> id;
    
    LOGFNR( "CWmDrmSession::EnumerateNextL", r );
    if ( iEnumerator == NULL )
        {
        User::Leave( KErrNotReady );
        }
    r = iEnumerator->NextL();
    if ( r == KErrNone )
        {
        iEnumerator->GetHashKey( hash );
        LOGHEX( hash.Ptr(), hash.Size() );
        iEnumerator->GetUniqueKey( id );
        LOGHEX( id.Ptr(), id.Size() );
        aMessage.WriteL( 0, iEnumerator->iStore);
        aMessage.WriteL( 1, iEnumerator->iNamespace );
        aMessage.WriteL( 2, hash );
        aMessage.WriteL( 3, id );
        }
    return r;
    }

//---------------------------------------------------------------------------
// CWmDrmSession::EnumerateDeleteL
//---------------------------------------------------------------------------
//    
TInt CWmDrmSession::EnumerateDeleteL ( const RMessage2& /*aMessage*/ )
    {
    TInt r = KErrNone;
    
    LOGFNR( "CWmDrmSession::EnumerateDeleteL", r );
    if ( iEnumerator == NULL )
        {
        User::Leave( KErrNotReady );
        }
    iEnumerator->DeleteCurrentL();
    return r;
    }

//---------------------------------------------------------------------------
// CWmDrmSession::EnumerateEndL
//---------------------------------------------------------------------------
//
TInt CWmDrmSession::EnumerateEndL ( const RMessage2& /*aMessage*/ )
    {
    TInt r = KErrNone;
    
    LOGFNR( "CWmDrmSession::EnumerateEndL", r );
    if ( iEnumerator == NULL )
        {
        User::Leave( KErrNotReady );
        }
    iEnumerator->Close();
    delete iEnumerator;
    iEnumerator = NULL;
    return r;
    }

//---------------------------------------------------------------------------
// CWmDrmSession::EmptyCacheL
//---------------------------------------------------------------------------
//
TInt CWmDrmSession::EmptyCacheL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    
    LOGFNR( "CWmDrmSession::EmptyCacheL", r );
    TRAP_IGNORE( EnumerateEndL( aMessage ) );
    TRAP_IGNORE( SlotCloseL( aMessage ) );
    Server().ResetCacheL();
    return r;
    }

//---------------------------------------------------------------------------
// CWmDrmSession::LogStats
//---------------------------------------------------------------------------
//    
TInt CWmDrmSession::LogStats( const RMessage2& /*aMessage*/ )
    {
    TInt r = KErrNone;

#ifdef _LOGGING    
    TInt cells;
    TInt amount;
    TInt i;
    TInt j;
    _LIT( KStatsDir, "wmdrm" );
    _LIT( KStatsFile, "wmdrmserver.txt" );
    
    LOGFNR( "CWmDrmSession::LogStatsL", r );
    cells = User::AllocSize( amount );
    RFileLogger::Write( KStatsDir, KStatsFile, EFileLoggingModeAppend, _L( "Statistics:" ) );
    RFileLogger::WriteFormat( KStatsDir, KStatsFile, EFileLoggingModeAppend,
        _L( "Memory: %d cells, %d bytes allocated" ), cells, amount );

    if (Server().Clock()->TimeIsGood() )
        {
        RFileLogger::Write( KStatsDir, KStatsFile, EFileLoggingModeAppend, _L( "Time is valid" ) );
        }
    else
        {
        RFileLogger::Write( KStatsDir, KStatsFile, EFileLoggingModeAppend, _L( "Time is invalid" ) );
        }

    RFileLogger::WriteFormat( KStatsDir, KStatsFile, EFileLoggingModeAppend,
        _L( "Cache: %d out of %d slots used" ),
        Server().Cache()->iSlotCache.Count(),
        Server().Cache()->iMaxCachedSlots );
    for ( i = 0; i < Server().Cache()->iSlotCache.Count(); i++ )
        {
        RFileLogger::Write( KStatsDir, KStatsFile, EFileLoggingModeAppend,
            Server().Cache()->iSlotCache[i]->iHashKey );
        RFileLogger::Write( KStatsDir, KStatsFile, EFileLoggingModeAppend,
            Server().Cache()->iSlotCache[i]->iUniqueKey );
        }

    RFileLogger::WriteFormat( KStatsDir, KStatsFile, EFileLoggingModeAppend,
        _L( "Enumerators: %d out of %d slots used" ),
        Server().EnumeratorCache()->iEnumeratorCache.Count(),
        Server().EnumeratorCache()->iMaxCachedEnumerators );
    for ( i = 0; i < Server().EnumeratorCache()->iEnumeratorCache.Count(); i++ )
        {
        RFileLogger::Write( KStatsDir, KStatsFile, EFileLoggingModeAppend,
            Server().EnumeratorCache()->iEnumeratorCache[i]->iHashKey );
        for ( j = 0; j < Server().EnumeratorCache()->iEnumeratorCache[i]->iEntries.Count(); j++ )
            {
            RFileLogger::Write( KStatsDir, KStatsFile, EFileLoggingModeAppend,
                *Server().EnumeratorCache()->iEnumeratorCache[i]->iEntries[j] );
            }
        }
#endif
    
    return r;
    }

//---------------------------------------------------------------------------
// CWmDrmSession::TimeValid
//---------------------------------------------------------------------------
//
TInt CWmDrmSession::TimeValid( const RMessage2& /*aMessage*/ )
    {
    TInt r = KErrNone;
    
    LOGFNR( "CWmDrmSession::TimeValid", r );
    r = Server().Clock()->TimeIsGood();
    return r;
    }

//---------------------------------------------------------------------------
// CWmDrmSession::SetTimeAsValidL
//---------------------------------------------------------------------------
//    
void CWmDrmSession::SetTimeAsValidL( const RMessage2& aMessage )
    {
    LOGFN( "CWmDrmSession::TimeValid" );
    Server().Clock()->SetTimeAsGoodL( aMessage.Int0() == 0 ? EFalse : ETrue );
    }
    

//---------------------------------------------------------------------------
// CWmDrmSession::DeleteRightsL
//---------------------------------------------------------------------------
//    
TInt CWmDrmSession::DeleteRightsL( const RMessage2& /*aMessage*/ )
    {
    TInt r = KErrNone;
    TBool wmDrmRightsConfigFound( EFalse );
    TChar driveLetter;
    TDrmScheme drmScheme( EDrmSchemeWmDrm );
    
    LOGFNR( "CWmDrmSession::DeleteRightsL", r );
    // Close EVERYTHING down, clear cache etc, so that we have nothing going on
    // When we start to delete stuff
    Server().ResetCacheL();
    
    wmDrmRightsConfigFound = DrmRightsStoringLocation::CheckDrmRightsStorageDriveL(
	  		Server().Fs(), drmScheme, driveLetter );
    
    // If WM DRM rights storage configuration is found, update both databases
    if ( wmDrmRightsConfigFound )
        {
    	Server().DataStore()->UpdateDummyDbFileL( 
    	    Server().DataStore()->DummyDBSizeL( ETrue ), ETrue );
    	}
    Server().DataStore()->UpdateDummyDbFileL( 
        Server().DataStore()->DummyDBSizeL( EFalse ), EFalse );
    
    // Delete license store
    Server().Db()->DeleteLicenseStoreL();
    
    // If WM DRM rights storage configuration is found, update both databases 
    if ( wmDrmRightsConfigFound )
    	{
    	Server().DataStore()->UpdateDummyDbFileL( 0, ETrue );
    	}	
    Server().DataStore()->UpdateDummyDbFileL( 0, EFalse );
    
    return r;
    }

//---------------------------------------------------------------------------
// CWmDrmSession::StoreStateL
//---------------------------------------------------------------------------
//
void CWmDrmSession::StoreStateL( const RMessage2& aMessage )
    {
    TWmDrmStoreState state;
    
    LOGFN( "CWmDrmSession::StoreStateL" );
    state = Server().DataStore()->DataStoreStateL();
    aMessage.WriteL( 0, TPckg<TWmDrmStoreState>( state ) );
    }

//---------------------------------------------------------------------------
// CWmDrmSession::GetTimeL
//---------------------------------------------------------------------------
//
void CWmDrmSession::GetTimeL( const RMessage2& aMessage )
    {
    TTime time = Time::NullTTime();
    TBool validity = EFalse;
    TPckg<TTime> timePckg(time);
    TPckg<TBool> validityPckg(validity);
    
    LOGFN( "CWmDrmSession::GetTimeL" );
    
    // Get the time from the clock
    Server().Clock()->GetTimeL( time, validity );
    
    aMessage.WriteL(0, timePckg);
    aMessage.WriteL(1, validityPckg);
    }


//---------------------------------------------------------------------------
// CWmDrmSession::CommitDataL
//---------------------------------------------------------------------------
//
void CWmDrmSession::CommitDataL( const RMessage2& aMessage )
    {
    
    LOGFN( "CWmDrmSession::CommitDataL" );
    
    // Force a commit on the database
    Server().Db()->CheckDatabaseCommitL( ETrue );
    }


 
//---------------------------------------------------------------------------
// CWmDrmSession::ServiceL
//---------------------------------------------------------------------------
//    
void CWmDrmSession::ServiceL( const RMessage2& aMessage )
	{
	TInt r = KErrNone;
	TInt trap = KErrNone;
	_LIT_SECURITY_POLICY_C1(drmCheck, ECapabilityDRM);
    RThread client;
	
	LOGFNR( "CWmDrmSession::ServiceL", r );
#ifdef _LOGGING
    TInt c;
    TInt s;
    c = User::AllocSize(s);
    LOG3( "Memory: %d cells, %d bytes allocated", c, s );
#endif

    aMessage.ClientL( client );

    if ( !drmCheck().CheckPolicy( client ) )
        {
        r = KErrAccessDenied;
        }
    else
        {
        switch ( aMessage.Function() )
            {
            case EInitStore:
                TRAP( trap, r = InitStoreL( aMessage ) );
                break;
            case ERemoveStore:
                TRAP( trap, r = RemoveStoreL( aMessage ) );
                break;
            case EInitNamespace:
                TRAP( trap, r = InitNamespaceL( aMessage ) );
                break;
            case ERemoveNamespace:
                TRAP( trap, r = RemoveNamespaceL( aMessage ) );
                break;
            case ESlotOpen:
                TRAP( trap, r = SlotOpenL( aMessage ) );
                break;
            case ESlotCreate:
                TRAP( trap, r = SlotCreateL( aMessage ) );
                break;
            case ESlotSeek:
                TRAP( trap, r = SlotSeekL( aMessage ) );
                break;
            case ESlotRead:
                TRAP( trap, r = SlotReadL( aMessage ) );
                break;
            case ESlotWrite:
                TRAP( trap, r = SlotWriteL( aMessage ) );
                break;
            case ESlotDelete:
                TRAP( trap, r = SlotDeleteL( aMessage ) );
                break;
            case ESlotResize:
                TRAP( trap, r = SlotResizeL( aMessage ) );
                break;
            case ESlotClose:
                TRAP( trap, r = SlotCloseL( aMessage ) );
                break;
            case EEnumerateStart:
                TRAP( trap, r = EnumerateStartL( aMessage ) );
                break;
            case EEnumerateReload:
                TRAP( trap, r = EnumerateReloadL( aMessage ) );
                break;
            case EEnumerateNext:
                TRAP( trap, r = EnumerateNextL( aMessage ) );
                break;
            case EEnumerateDelete:
                TRAP( trap, r = EnumerateDeleteL( aMessage ) );
                break;
            case EEnumerateEnd:
                TRAP( trap, r = EnumerateEndL( aMessage ) );
                break;
            case ELogStats:
                r = LogStats( aMessage );
                break;
            case EEmptyCache:
                TRAP( trap, r = EmptyCacheL( aMessage ) );
                break;
            case ETimeValid:
                r = TimeValid( aMessage );
                break;
            case ESetTimeAsValid:
                TRAP( trap, SetTimeAsValidL( aMessage ) );
                break;
            case EDeleteRights:
                TRAP( trap, DeleteRightsL( aMessage ) );
                break;
            case EStoreState:
                TRAP( trap, StoreStateL( aMessage ) );
                break;
            case EGetTime:
                TRAP( trap, GetTimeL( aMessage ) );
                break;      
            case ECommitData:
                TRAP( trap, CommitDataL( aMessage ) );
                break;            
            default:
                r = KErrArgument;
                break;
                }
        if ( trap != KErrNone )
            {
            r = trap;
            }
        if ( r == KErrDiskFull || r == KErrNoMemory )
            {
            TRAP_IGNORE( EmptyCacheL( aMessage ) );
            }
        }
    client.Close();
    LOG2( "** ServiceL error: %d", r );
    aMessage.Complete( r );
	}

//---------------------------------------------------------------------------
// CWmDrmSession::ServiceError
//---------------------------------------------------------------------------
//
void CWmDrmSession::ServiceError( const RMessage2& aMessage, TInt aError )
	{
	LOGFN( "CWmDrmSession::ServiceError" );
	LOG2( "** Error: %d", aError );
	CSession2::ServiceError( aMessage, aError );
	}

//---------------------------------------------------------------------------
// CWmDrmSession::Server
//---------------------------------------------------------------------------
//
CWmDrmServer& CWmDrmSession::Server()
	{
	return *static_cast<CWmDrmServer*>( const_cast<CServer2*>( CSession2::Server() ) );
	}
