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
* Description:  WMDRM Client implementation
*
*/


#include "wmdrmclient.h"
#include "wmdrmclientserver.h"

#define _LOGGING_FILE L"wmdrmserver.txt"

#ifdef _DEBUG
#define DEBUGATTR( a ) a
#else
#define DEBUGATTR( a )
#endif

#include "logfn.h"

// Constants

const TInt KServerDefaultMessageSlots = -1;
const TInt KServerRetryCount = 3;

// ======== LOCAL FUNCTIONS ========


//---------------------------------------------------------------------------
// SetupSlotSpecL
//---------------------------------------------------------------------------
//
LOCAL_C void SetupSlotSpecL(
    RBuf8& aSlotSpec,
    const TDesC8& aStoreName,
    const TDesC8& aNamespace,
    const TDesC8& aHashKey,
    const TDesC8& aUniqueKey )
    {
    aSlotSpec.CreateL( aStoreName.Length() + aNamespace.Length() + aHashKey.Length() + aUniqueKey.Length() + 4 );
    aSlotSpec.Append( aStoreName.Length() );
    aSlotSpec.Append( aStoreName );
    aSlotSpec.Append( aNamespace.Length() ) ;
    aSlotSpec.Append( aNamespace );
    aSlotSpec.Append( aHashKey.Length() );
    aSlotSpec.Append( aHashKey );
    aSlotSpec.Append( aUniqueKey.Length() );
    aSlotSpec.Append( aUniqueKey );
    }

//---------------------------------------------------------------------------
// SetupSlotEnumSpecL
//---------------------------------------------------------------------------
//
LOCAL_C void SetupSlotEnumSpecL(
    RBuf8& aSlotSpec,
    const TDesC8& aStoreName,
    const TDesC8& aNamespace,
    const TDesC8& aHashKey )
    {
    aSlotSpec.CreateL( aStoreName.Length() + aNamespace.Length() + aHashKey.Length() + 3 );
    aSlotSpec.Append( aStoreName.Length() );
    aSlotSpec.Append( aStoreName );
    aSlotSpec.Append( aNamespace.Length() ) ;
    aSlotSpec.Append( aNamespace );
    aSlotSpec.Append( aHashKey.Length() );
    aSlotSpec.Append( aHashKey );
    }

//---------------------------------------------------------------------------
// StartServer
//---------------------------------------------------------------------------
//
static TInt StartServer()
    {
    RProcess server;
    TInt r = server.Create( KWmDrmServerImg, KNullDesC );
    if ( r != KErrNone )
        return r;
    TRequestStatus stat;
    server.Rendezvous( stat );
    if ( stat != KRequestPending )
        {
        server.Kill( 0 );
        }
    else
        {
        server.Resume();
        }
    User::WaitForRequest( stat );
    r = ( server.ExitType() == EExitPanic ) ? KErrGeneral : stat.Int();
    server.Close();
    return r;
    }

// ======== MEMBER FUNCTIONS ========

//---------------------------------------------------------------------------
// RWmDrmClient::Connect
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::Connect()
    {
    TInt r = KErrNone;

    LOGFNR( "RWmDrmClient::Connect", r );
    TInt retry = KServerRetryCount;
    for ( ;; )
        {
        r = CreateSession( KWmDrmServerName, TVersion( 0, 0, 0 ), KServerDefaultMessageSlots );
        if ( r != KErrNotFound && r != KErrServerTerminated )
            {
            return r;
            }
        if ( --retry == 0 )
            {
            return r;
            }
        r = StartServer();
        if ( r != KErrNone && r != KErrAlreadyExists )
            {
            return r;
            }
        }
    }

//---------------------------------------------------------------------------
// RWmDrmClient::InitStore
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::InitStore(
    const TDesC8& DEBUGATTR( aStoreName ),
    TBool /* aCreateIfMissing */ )
    {
    TInt r = KErrNone;

    LOGFNR( "RWmDrmClient::InitStore", r );
    LOG( aStoreName );

    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::RemoveStore
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::RemoveStore(
    const TDesC8& aStoreName )
    {
    TInt r = KErrNone;

    LOGFNR( "RWmDrmClient::RemoveStore", r );
    LOG( aStoreName );
    r = SendReceive( ERemoveStore, TIpcArgs( &aStoreName ) );
    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::InitNamespace
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::InitNamespace(
    const TDesC8& DEBUGATTR( aStoreName ),
    const TDesC8& DEBUGATTR( aNamespace ),
    TBool /* aCreateIfMissing */)
    {
    TInt r = KErrNone;

    LOGFNR( "RWmDrmClient::InitNamespace", r );
    LOG( aStoreName ); LOG( aNamespace );

    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::RemoveNamespace
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::RemoveNamespace(
    const TDesC8& aStoreName,
    const TDesC8& aNamespace )
    {
    TInt r = KErrNone;

    LOGFNR( "RWmDrmClient::RemoveNamespace", r );
    r = SendReceive( ERemoveNamespace, TIpcArgs( &aStoreName, &aNamespace ) );
    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::SlotOpen
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::SlotOpen(
    const TDesC8& aStoreName,
    const TDesC8& aNamespace,
    const TDesC8& aHashKey,
    const TDesC8& aUniqueKey,
    TInt& aSize )
    {
    TInt r = KErrNone;
    RBuf8 slotSpec;
    TPckg<TInt> sizePckg( aSize );

    LOGFNR( "RWmDrmClient::SlotOpen", r );
    LOG( aStoreName ); LOG( aNamespace ); LOG( aHashKey ); LOG( aUniqueKey );
    TRAP(r, SetupSlotSpecL( slotSpec, aStoreName, aNamespace, aHashKey, aUniqueKey ));
    if( r )
        {
        slotSpec.Close();
        return r;
        }
    r = SendReceive( ESlotOpen, TIpcArgs( &slotSpec, &sizePckg ) );
    slotSpec.Close();
    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::SlotCreate
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::SlotCreate(
    const TDesC8& aStoreName,
    const TDesC8& aNamespace,
    const TDesC8& aHashKey,
    const TDesC8& aUniqueKey,
    TInt& aSize )
    {
    TInt r = KErrNone;
    RBuf8 slotSpec;

    LOGFNR( "RWmDrmClient::SlotCreate", r );
    LOG( aStoreName ); LOG( aNamespace ); LOG( aHashKey ); LOG( aUniqueKey );
    TRAP( r, SetupSlotSpecL( slotSpec, aStoreName, aNamespace, aHashKey, aUniqueKey ));
    if( r )
        {
        slotSpec.Close();
        return r;
        }    
    r = SendReceive( ESlotCreate, TIpcArgs( &slotSpec, aSize ) );
    slotSpec.Close();
    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::SlotSeek
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::SlotSeek(
    TInt& aPos,
    TSeek aOrigin )
    {
    TInt r = KErrNone;
    TInt newPos;
    TPckg<TInt> posPckg( newPos );

    LOGFNR( "RWmDrmClient::SlotSeek", r );
    r = SendReceive( ESlotSeek, TIpcArgs( aPos, aOrigin, &posPckg ) );
    aPos = newPos;
    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::SlotRead
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::SlotRead(
    TDes8& aData )
    {
    TInt r = KErrNone;
    TInt amountRead;
    TPckg<TInt> amountPckg( amountRead );

    LOGFNR( "RWmDrmClient::SlotRead", r );
    r = SendReceive( ESlotRead, TIpcArgs( &aData, &amountPckg ) );
    if ( r == KErrNone )
        {
        aData.SetLength( amountRead );
        }
    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::SlotWrite
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::SlotWrite(
    const TDesC8& aData )
    {
    TInt r = KErrNone;

    LOGFNR( "RWmDrmClient::SlotWrite", r );
    r = SendReceive( ESlotWrite, TIpcArgs( &aData ) );
    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::SlotDelete
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::SlotDelete(
    const TDesC8& aStoreName,
    const TDesC8& aNamespace,
    const TDesC8& aHashKey,
    const TDesC8& aUniqueKey )
    {
    TInt r = KErrNone;
    RBuf8 slotSpec;

    LOGFNR( "RWmDrmClient::SlotDelete", r );
    TRAP(r, SetupSlotSpecL( slotSpec, aStoreName, aNamespace, aHashKey, aUniqueKey ));
    if( r )
        {
        slotSpec.Close();
        return r;
        }
    r = SendReceive( ESlotDelete, TIpcArgs( &slotSpec ) );
    slotSpec.Close();
    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::SlotResize
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::SlotResize(
    TInt aSize )
    {
    TInt r = KErrNone;

    LOGFNR( "RWmDrmClient::SlotResize", r );
    r = SendReceive( ESlotResize, TIpcArgs( aSize ) );
    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::SlotClose
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::SlotClose()
    {
    TInt r = KErrNone;

    LOGFNR( "RWmDrmClient::SlotClose", r );
    r = SendReceive( ESlotClose );
    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::EnumerateStart
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::EnumerateStart(
    const TDesC8& aStoreName,
    const TDesC8& aNamespace,
    const TDesC8& aHashKey )
    {
    TInt r = KErrNone;
    RBuf8 slotSpec;

    LOGFNR( "RWmDrmClient::EnumerateStart", r );
    TRAP(r, SetupSlotEnumSpecL( slotSpec, aStoreName, aNamespace, aHashKey ));
    if( r )
        {
        slotSpec.Close();
        return r;
        }
    r = SendReceive( EEnumerateStart, TIpcArgs( &slotSpec ) );
    slotSpec.Close();
    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::EnumerateReload
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::EnumerateReload(
    TDes8& aStoreName,
    TDes8& aNamespace,
    TDes8& aHashKey,
    TDes8& aUniqueKey )
    {
    TInt r = KErrNone;

    LOGFNR( "RWmDrmClient::EnumerateReload", r );
    r = SendReceive( EEnumerateReload, TIpcArgs( &aStoreName, &aNamespace, &aHashKey, &aUniqueKey ) );
    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::EnumerateNext
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::EnumerateNext(
    TDes8& aStoreName,
    TDes8& aNamespace,
    TDes8& aHashKey,
    TDes8& aUniqueKey )
    {
    TInt r = KErrNone;

    LOGFNR( "RWmDrmClient::EnumerateNext", r );
    r = SendReceive( EEnumerateNext, TIpcArgs( &aStoreName, &aNamespace, &aHashKey, &aUniqueKey ) );
    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::EnumerateDeleteCurrent
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::EnumerateDeleteCurrent()
    {
    TInt r = KErrNone;

    LOGFNR( "RWmDrmClient::EnumerateDelete", r );
    r = SendReceive( EEnumerateDelete );
    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::EnumerateEnd
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::EnumerateEnd()
    {
    TInt r = KErrNone;

    LOGFNR( "RWmDrmClient::EnumerateEnd", r );
    r = SendReceive( EEnumerateEnd );
    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::LogStats
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::LogStats()
    {
    TInt r = KErrNone;

    LOGFNR( "RWmDrmClient::LogStats", r );
    r = SendReceive( ELogStats );
    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::EmptyCache
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::EmptyCache()
    {
    TInt r = KErrNone;

    LOGFNR( "RWmDrmClient::EmptyCache", r );
    r = SendReceive( EEmptyCache );
    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::TimeValid
//---------------------------------------------------------------------------
//
EXPORT_C TBool RWmDrmClient::TimeValid()
    {
    TBool r = EFalse;

    LOGFN( "RWmDrmClient::TimeValid" );
    if ( SendReceive( ETimeValid ) == KErrNone )
        {
        LOG1( "Time valid" );
        r = ETrue;
        }
    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::SetTimeAsValid
//---------------------------------------------------------------------------
//
EXPORT_C void  RWmDrmClient::SetTimeAsValid( TBool aValid )
    {
    TInt v = 0;

    if ( aValid )
        {
        v = 1;
        }
    LOGFN( "RWmDrmClient::SetTimeAsValid" );
    SendReceive( ESetTimeAsValid, TIpcArgs( v ) );
    }

//---------------------------------------------------------------------------
// RWmDrmClient::DeleteRights
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::DeleteRights()
    {
    TInt r = KErrNone;

    LOGFNR( "RWmDrmClient::DeleteRights", r );
    r = SendReceive( EDeleteRights );
    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::StoreState
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::StoreState( TWmDrmStoreState& aState )
    {
    TInt r = KErrNone;
    TPckg<TWmDrmStoreState> statePckg( aState );

    LOGFNR( "RWmDrmClient::StoreState", r );
    r = SendReceive( EStoreState, TIpcArgs( &statePckg ) );
    return r;
    }

//---------------------------------------------------------------------------
// RWmDrmClient::GetTime
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::GetTime( TTime& aTime, TBool& aValid )
    {
    TInt r = KErrNone;
    TPckg<TTime> timePckg( aTime );
    TPckg<TBool> validPckg( aValid );
    
	LOGFNR( "RWmDrmClient::GetTime", r );    
    r = SendReceive( EGetTime, TIpcArgs( &timePckg, &validPckg ) );
    return r;    
    };
    
//---------------------------------------------------------------------------
// RWmDrmClient::CommitData
//---------------------------------------------------------------------------
//
EXPORT_C TInt RWmDrmClient::CommitData()
    {
    TInt r = KErrNone;
    
	LOGFNR( "RWmDrmClient::CommitData", r );    
    r = SendReceive( ECommitData );
    return r;    
    };

    
