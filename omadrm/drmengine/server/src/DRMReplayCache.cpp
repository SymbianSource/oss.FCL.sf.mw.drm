/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the parent storage for Decision Making Machine
*
*/


// INCLUDE FILES

#include <f32file.h>
#include <s32file.h>
#include <bautils.h>
#include "DRMReplayCache.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

_LIT( KCIDColName, "cid" );
_LIT( KTimeColName, "time" );
_LIT( KInsertionTimeColName, "inserttime" );
_LIT( KReplayCacheTable, "cache" );
_LIT( KViewInitQuery, "SELECT * FROM cache ORDER BY inserttime ASC" );

LOCAL_C const TUint KDbViewInsertionTimeOrdinal = 2;
LOCAL_C const TUint8 KDbViewTimeOrdinal = 3;
LOCAL_C const TUint8 KDbViewIDOrdinal = 1;
LOCAL_C const TUint16 KDbMaxNumOfItems = 100;

// MODULE DATA STRUCTURES
NONSHARABLE_STRUCT( TDoDeleteFile )
    {
    RFs* iFs;
    const TDesC* iFile;
    };

// LOCAL FUNCTION PROTOTYPES
LOCAL_C void DoRollBack( TAny* aAny );
LOCAL_C void DoDeleteFile( TAny* aAny );

// FORWARD DECLARATIONS


// ============================= LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// DoRollBack
//
// Do a rollback operation to the RDbDatabase*
// -----------------------------------------------------------------------------
//
LOCAL_C void DoRollBack( TAny* aAny )
    {
    reinterpret_cast< RDbDatabase* >( aAny )->Rollback();
    }

// -----------------------------------------------------------------------------
// DoDeleteFile
//
// Delete the file presented by TDoDeleteFile*
// -----------------------------------------------------------------------------
//
LOCAL_C void DoDeleteFile( TAny* aAny )
    {
    TDoDeleteFile* s = reinterpret_cast< TDoDeleteFile* >( aAny );

    s->iFs->Delete( *( s->iFile ) );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RDRMReplayCache::RDRMReplayCache
//
// Default constructor
// -----------------------------------------------------------------------------
//
RDRMReplayCache::RDRMReplayCache():
iFs( NULL ),
iTimeDb(),
iPlainDb()
    {
    // Nothing.
    }

// -----------------------------------------------------------------------------
// RDRMReplayCache::RDRMReplayCache
//
// Constructor
// -----------------------------------------------------------------------------
//
RDRMReplayCache::RDRMReplayCache( RFs& aFs ) :
iFs( &aFs ),
iTimeDb(),
iPlainDb()
    {
    // Nothing.
    }


// -----------------------------------------------------------------------------
// RDRMReplayCache::Set
//
// Set iFs to given aFs.
// -----------------------------------------------------------------------------
//
void RDRMReplayCache::Set( RFs& aFs )
    {
    iFs = &aFs;
    }

// -----------------------------------------------------------------------------
// RDRMReplayCache::Close
//
// Closes the databases.
// -----------------------------------------------------------------------------
//
void RDRMReplayCache::Close()
    {
    // iView.Close();
    iTimeDb.Close();
    iPlainDb.Close();
    }

// -----------------------------------------------------------------------------
// RDRMReplayCache::InitL
//
// Initialize the databases.
// -----------------------------------------------------------------------------
//
void RDRMReplayCache::InitL( const TDesC& aTimedDb,
                             const TDesC& aPlainDb )
    {
    InitDbL( iTimeDb, aTimedDb, ETimeDb );
    InitDbL( iPlainDb, aPlainDb, EPlainDb );
    }


// -----------------------------------------------------------------------------
// RDRMReplayCache::InCacheL
//
// Check whether the given entry is in cache. Overloaded.
// -----------------------------------------------------------------------------
//
TBool RDRMReplayCache::InCacheL( const TDesC8& aID,
                                 const TTime& aTime )
    {
    TBool res = EFalse;

    RDbView view;
    InitViewLC( view, ETimeDb, EFalse );

    view.FirstL();

    while ( view.AtRow() && !res )
        {
        view.GetL();
        TTime time = view.ColInt64( KDbViewTimeOrdinal );

        if ( time == aTime )
            {
            res = CompareCIDL( view, aID );
            }
        view.NextL();
        }

    CleanupStack::PopAndDestroy(); // view

    return res;
    }

// -----------------------------------------------------------------------------
// RDRMReplayCache::InCacheL
//
// Check whether the given entry is in cache. Overloaded.
// -----------------------------------------------------------------------------
//
TBool RDRMReplayCache::InCacheL( const TDesC8& aID )
    {
    TBool res = EFalse;

    RDbView view;
    InitViewLC( view, EPlainDb, EFalse );

    view.FirstL();

    while ( view.AtRow() && !res )
        {
        view.GetL();

        res = CompareCIDL( view, aID );

        view.NextL();
        }

    CleanupStack::PopAndDestroy(); // view

    return res;
    }

// -----------------------------------------------------------------------------
// RDRMReplayCache::AddL
//
// Add an entry to the database. Overloaded.
// -----------------------------------------------------------------------------
//
void RDRMReplayCache::AddL( const TDesC8& aID,
                            const TTime& aTime,
                            const TTime& aInsertionTime )
    {
    RDbColWriteStream stream;
    RDbView view;

    PushL( iTimeDb );

    InitViewLC( view, ETimeDb, ETrue );
    CompactViewL( view );

    // Oldest ones are in the list earlier, and as long as clock works
    // correctly, insertiontime(n-1)<insertiontime(n).
    view.EndL();

    User::LeaveIfError( iTimeDb.Begin() );

    view.InsertL();

    InsertIdL( view, aID );
    view.SetColL( KDbViewInsertionTimeOrdinal, aInsertionTime.Int64() );
    view.SetColL( KDbViewTimeOrdinal, aTime.Int64() );

    view.PutL();

    CleanupStack::PopAndDestroy(); // view

    User::LeaveIfError( iTimeDb.Commit() );
    User::LeaveIfError( iTimeDb.Compact() );

    Pop(); // iTimeDb
    }

// -----------------------------------------------------------------------------
// RDRMReplayCache::AddL
//
// Add an entry to the database. Overloaded.
// -----------------------------------------------------------------------------
//
void RDRMReplayCache::AddL( const TDesC8& aID,
                            const TTime& aInsertionTime )
    {

    RDbView view;

    PushL( iPlainDb );

    InitViewLC( view, EPlainDb, ETrue );
    CompactViewL( view );

    view .EndL();

    User::LeaveIfError( iPlainDb.Begin() );

    view.InsertL();
    InsertIdL( view, aID );
    view.SetColL( KDbViewInsertionTimeOrdinal, aInsertionTime.Int64() );

    view.PutL();

    CleanupStack::PopAndDestroy(); // view

    User::LeaveIfError( iPlainDb.Commit() );
    User::LeaveIfError( iPlainDb.Compact() );

    Pop(); // iPlainDb
    }

// -----------------------------------------------------------------------------
// RDRMReplayCache::InitDbL
//
// Initialize the databases.
// -----------------------------------------------------------------------------
//
void RDRMReplayCache::InitDbL( RDbNamedDatabase& aDb,
                               const TDesC& aFileName,
                               TDatabaseId aId )
    {
    TInt error = KErrNone;
    TBool exists = BaflUtils::FileExists( *iFs, aFileName );

    if ( exists )
        {
        TRAP( error, OpenDbL( aDb, aFileName ) );
        }
    if ( error || !exists )
        {
        ReplaceDbL( aDb, aFileName, aId );
        }
    }

// -----------------------------------------------------------------------------
// RDRMReplayCache::~RDRMReplayCache
//
// Destructor.
// -----------------------------------------------------------------------------
//
RDRMReplayCache::~RDRMReplayCache()
    {
    }

// -----------------------------------------------------------------------------
// RDRMReplayCache::OpenDbL
//
// Open the database.
// -----------------------------------------------------------------------------
//
void RDRMReplayCache::OpenDbL( RDbNamedDatabase& aDb,
                               const TDesC& aFileName )
    {
    CDbTableNames* tables = NULL;

    User::LeaveIfError( aDb.Open( *iFs, aFileName ) );
    CleanupClosePushL( aDb );

    if ( aDb.IsDamaged() )
        {
        User::LeaveIfError( aDb.Recover() );
        }

    // Sanity check.
    tables = aDb.TableNamesL();
    CleanupStack::PushL( tables );

    if ( tables->Count() != 1 || ( *tables )[ 0 ].Compare( KReplayCacheTable ) )
        {
        User::Leave( KErrCorrupt );
        }

    CleanupStack::PopAndDestroy(); // tables
    CleanupStack::Pop(); // aDb
    }

// -----------------------------------------------------------------------------
// RDRMReplayCache::ReplaceDbL
//
// Replace the database.
// -----------------------------------------------------------------------------
//
void RDRMReplayCache::ReplaceDbL( RDbNamedDatabase& aDb,
                                  const TDesC& aFileName,
                                  TDatabaseId aId )
    {
    CDbColSet*  colSet = NULL;
//    CDbKey* key = NULL;
    TDbCol cidCol( KCIDColName, EDbColLongText8 );
    TDbCol insertionTimeCol( KInsertionTimeColName, EDbColInt64 );
    TDbCol timeCol( KTimeColName, EDbColInt64 );

    TDoDeleteFile deletefile = { iFs, &aFileName };

    TCleanupItem item( DoDeleteFile, &deletefile );
    CleanupStack::PushL( item );

    User::LeaveIfError( aDb.Replace( *iFs, aFileName ) );
    CleanupClosePushL( aDb );

    // Add columns
    colSet = CDbColSet::NewLC();
    colSet->AddL( cidCol );
    colSet->AddL( insertionTimeCol );

    if ( aId == ETimeDb )
        {
        colSet->AddL( timeCol );
        }

    User::LeaveIfError( aDb.Begin() );
    User::LeaveIfError( aDb.CreateTable( KReplayCacheTable, *colSet ) );
    User::LeaveIfError( aDb.Commit() );

    CleanupStack::PopAndDestroy(); // colSet
    CleanupStack::Pop(); // aDb
    CleanupStack::Pop(); // item
    }

// -----------------------------------------------------------------------------
// RDRMReplayCache::InitViewLC
//
// Initialize the view.
// -----------------------------------------------------------------------------
//
void RDRMReplayCache::InitViewLC( RDbView& aView,
                                  TDatabaseId aId,
                                  TBool aUpdate )
    {
    RDbDatabase* db = ( aId ==  ETimeDb ? &iTimeDb : &iPlainDb );

    User::LeaveIfError(
        aView.Prepare( *db,
                       TDbQuery( KViewInitQuery, EDbCompareCollated ),
                       aUpdate ? RDbRowSet::EUpdatable : RDbRowSet::EReadOnly ) );

    CleanupClosePushL( aView );

    User::LeaveIfError( aView.EvaluateAll() );
    }

// -----------------------------------------------------------------------------
// RDRMReplayCache::CompareCIDL
//
// Compare whether the rowset's CID matches the given CID.
// -----------------------------------------------------------------------------
//
TBool RDRMReplayCache::CompareCIDL( RDbRowSet& aView,
                                    const TDesC8& aCID )
    {
    TBool res = EFalse;

    TInt size = aView.ColLength( KDbViewIDOrdinal );

    RDbColReadStream colData;
    colData.OpenLC( aView, KDbViewIDOrdinal );

    // The data contains also the cardinality of the CID data, but anyway...
    HBufC8* des = HBufC8::NewLC( colData, size );

    if ( aCID.CompareC( *des ) == 0 )
        {
        res = ETrue;
        }

    CleanupStack::PopAndDestroy(); // des
    CleanupStack::PopAndDestroy(); // colData

    return res;
    }

// -----------------------------------------------------------------------------
// RDRMReplayCache::PushL
//
// Push a cleanup item to cleanup stack.
// -----------------------------------------------------------------------------
//
void RDRMReplayCache::PushL( RDbDatabase& aDb )
    {
    TCleanupItem item( DoRollBack, &aDb );
    CleanupStack::PushL( item );
    }

// -----------------------------------------------------------------------------
// RDRMReplayCache::Pop
//
// Pop a cleanup item pushed in by PushL.
// -----------------------------------------------------------------------------
//
void RDRMReplayCache::Pop()
    {
    CleanupStack::Pop();
    }

// -----------------------------------------------------------------------------
// RDRMReplayCache::DeleteOldestL
//
// Delete aHowMany entries from the view.
// -----------------------------------------------------------------------------
//
void RDRMReplayCache::DeleteOldestsL( RDbRowSet& aView, TInt16 aHowMany )
    {
    aView.FirstL();

    while ( aHowMany > 0 )
        {
        aView.DeleteL();
        aView.NextL();
        --aHowMany;
        }
    }

// -----------------------------------------------------------------------------
// RDRMReplayCache::CompactViewL
//
// Compact the view, deleting items if necessary.
// -----------------------------------------------------------------------------
//
void RDRMReplayCache::CompactViewL( RDbRowSet& aView )
    {
    TInt count = aView.CountL();

    if ( count >= KDbMaxNumOfItems )
        {
        // usually only one item is deleted, no need to use Begin/Commit.
        DeleteOldestsL( aView, static_cast< TInt >( KDbMaxNumOfItems ) - count + 1 );
        }
    }

// -----------------------------------------------------------------------------
// RDRMReplayCache::InsertIdL
//
// Insert content-ID to the view.
// -----------------------------------------------------------------------------
//
void RDRMReplayCache::InsertIdL( RDbRowSet& aView,
                                 const TDesC8& aId )
    {

    RDbColWriteStream stream;
    stream.OpenLC( aView, KDbViewIDOrdinal );
    stream << aId;

    stream.CommitL();
    CleanupStack::PopAndDestroy(); // stream
    }

// End of File
