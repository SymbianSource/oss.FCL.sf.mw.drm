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
* Description:  Implementation of the Metering database
*
*/


// INCLUDE FILES

#include <f32file.h>
#include <s32file.h>
#include <bautils.h>
#include "drmlog.h"
#include "drmmeteringdb.h"
#ifdef RD_DRM_METERING
#include "drmmeteringdbdata.h"
#endif
#include "DRMTypes.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

#ifdef RD_DRM_METERING

_LIT( KCIDColName, "cid" );
_LIT( KRightIssuerColName, "riid" );
_LIT( KCountColName, "count" );
_LIT( KAccumulatedTimeColName, "time" );
_LIT( KMeteringDataTable, "metering" );
_LIT( KParentUIDColName, "parent" );
_LIT( KViewInitQuery, "SELECT * FROM metering ORDER BY riid" );

LOCAL_C const TUint8 KDbViewCIDOrdinal = 1;
LOCAL_C const TUint8 KDbViewRIIDOrdinal = 2;
LOCAL_C const TUint8 KDbViewCountOrdinal = 3;
LOCAL_C const TUint8 KDbViewAccumulatedTimeOrdinal = 4;
LOCAL_C const TUint8 KDbViewParentUIDOrdinal = 5;

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

#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RDrmMeteringDb::RDrmMeteringDb
//
// Default constructor
// -----------------------------------------------------------------------------
//
RDrmMeteringDb::RDrmMeteringDb():
iFs( NULL ),
iDb()
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// RDrmMeteringDb::~RDrmMeteringDb
//
// Destructor
// -----------------------------------------------------------------------------
//
RDrmMeteringDb::~RDrmMeteringDb()
    {
    }

// -----------------------------------------------------------------------------
// RDRMMeteringDb::Close
//
// Closes the databases.
// -----------------------------------------------------------------------------
//
void RDrmMeteringDb::Close()
    {
    // Atomic operations only at the moment, but what about the future.
    iDb.Close();
    }

#ifdef RD_DRM_METERING

// -----------------------------------------------------------------------------
// RDrmMeteringDb::RDrmMeteringDb
//
// Constructor
// -----------------------------------------------------------------------------
//
RDrmMeteringDb::RDrmMeteringDb( RFs& aFs ) :
iFs( &aFs ),
iDb()
    {
    }

// -----------------------------------------------------------------------------
// RDrmMeteringDb::Set
//
// Set iFs to given aFs.
// -----------------------------------------------------------------------------
//
void RDrmMeteringDb::Set( RFs& aFs )
    {
    iFs = &aFs;
    }

// -----------------------------------------------------------------------------
// RDrmMeteringDb::InitL
//
// Initialize the databases.
// -----------------------------------------------------------------------------
//
void RDrmMeteringDb::InitL( const TDesC& aFileName )
    {

    DRMLOG( _L( "RDrmMeteringDb::InitL" ) );
    TInt error = KErrNone;
    TBool exists = BaflUtils::FileExists( *iFs, aFileName );

    if ( exists )
        {
        TRAP( error, OpenDbL( iDb, aFileName ) );
        }
    if ( error || !exists )
        {
        ReplaceDbL( iDb, aFileName );
        }
    DRMLOG( _L( "RDrmMeteringDb::InitL ok" ) );
    }

// -----------------------------------------------------------------------------
// RDrmMeteringDb::AddL
//
// Add an entry to the database. The method checks whether an entry matching
// the given Content Id and Rights Issuer Id already exists or not. A new row
// is added to the database if one does not already exist.
// -----------------------------------------------------------------------------
//
void RDrmMeteringDb::AddL( const CDrmMeteringDbData* aMeteringData )
    {

    DRMLOG( _L( "RDrmMeteringDb::AddL" ) );

    __ASSERT_DEBUG( aMeteringData, User::Invariant() );

    RDbView view;
    TBool res = EFalse;

    PushL( iDb );

    InitViewLC( view );

    User::LeaveIfError( iDb.Begin() );

    for ( view.FirstL(); view.AtRow() && !res ; view.NextL() )
        {
        view.GetL();

        // Check whether an entry already exists or not.
        if ( CompareIDL( view, *( aMeteringData->iContentId ),
             aMeteringData->iRiId ) )
            {

            view.UpdateL(); // Update count and accumulated time of the rowset

            // Get the structure of rowset
            CDbColSet* colset = view.ColSetL();
            CleanupStack::PushL( colset );

            TInt count = 0;
            TTimeIntervalSeconds accumulatedTime =
                aMeteringData->iAccumulatedTime.Int();

            count = aMeteringData->iCount +
                view.ColUint32( KDbViewCountOrdinal );

            view.SetColL( colset->ColNo( KCountColName ), count );

            accumulatedTime = accumulatedTime.Int() +
                view.ColInt32( KDbViewAccumulatedTimeOrdinal );

            view.SetColL( colset->ColNo( KAccumulatedTimeColName ),
                accumulatedTime.Int() );

            if ( aMeteringData->iParentUid )
                {
                view.SetColL( KDbViewParentUIDOrdinal,
                              *( aMeteringData->iParentUid ) );
                }

            view.PutL();
            iDb.Compact();

            res = ETrue;
            CleanupStack::PopAndDestroy( colset );
            }

        }

    // No existing entry was found. Make a new entry to the database.
    if ( !res )
        {
        view.InsertL(); // Add new row to the database

        view.SetColL( KDbViewCIDOrdinal, *( aMeteringData->iContentId ) );
        view.SetColL( KDbViewRIIDOrdinal, aMeteringData->iRiId );
        view.SetColL( KDbViewCountOrdinal, aMeteringData->iCount );
        view.SetColL( KDbViewAccumulatedTimeOrdinal,
                      aMeteringData->iAccumulatedTime.Int() );

        if ( aMeteringData->iParentUid )
            {
            view.SetColL( KDbViewParentUIDOrdinal,
                          *( aMeteringData->iParentUid ) );
            }

        view.PutL();
        }

    CleanupStack::PopAndDestroy(); // view

    User::LeaveIfError( iDb.Commit() );

    Pop(); // iDb
    DRMLOG( _L( "RDrmMeteringDb::AddL ok" ) );

    }


// -----------------------------------------------------------------------------
// RDrmMeteringDb::GetL
//
// Get the metering data list of a Rights Issuer from the database. Return value
// is ETrue if at least one entry was found. return value is EFalse if no entry
// was found. Function will leave if an error happens when accessing the
// database or if the given Rights Issuer Id is either empty or too long.
// -----------------------------------------------------------------------------
//

TBool RDrmMeteringDb::GetL( const TDesC8& aRiId,
                            CDRMPointerArray< CDrmMeteringDbData >&
                            aMeteringDataList )
    {

    DRMLOG( _L( "RDrmMeteringDb::GetL" ) );

    // If Rights Issuer Id not available or is too long
    if ( ( aRiId.Length() == 0 ) || ( aRiId.Length() > KRiIdSize ) )
        {
        User::Leave( KErrArgument );
        }

    TBool found = EFalse;

        RDbView view;

        PushL( iDb );

        InitViewLC( view );

        User::LeaveIfError( iDb.Begin() );

        // Examine the whole database for possible entries matching the Rights
        // Issuer Id
        for ( view.FirstL(); view.AtRow(); view.NextL() )
            {

            view.GetL();

            // Check whether the Rights Issuer Id in the current row matches
            // the given Rights Issuer Id or not.
            if ( CompareIDL( view, aRiId ) )
                {

                found = ETrue;

                CDbColSet* colset = view.ColSetL();
                CleanupStack::PushL( colset );

                // Create a new instance of the Metering information storage
                // class to be included in the given Metering data pointer
                // array
                CDrmMeteringDbData* meteringdata = CDrmMeteringDbData::NewLC();
                TPtrC8 cid = view.ColDes8( colset->ColNo( KCIDColName ) );

                meteringdata->iContentId = cid.AllocL();
                meteringdata->iRiId.Copy( aRiId );
                meteringdata->iCount = view.ColUint32( KDbViewCountOrdinal );

                meteringdata->iAccumulatedTime =
                    static_cast< TTimeIntervalSeconds >\
                    ( view.ColInt32( KDbViewAccumulatedTimeOrdinal ) );

                TPtrC8 parentuid =
                    view.ColDes8( colset->ColNo( KParentUIDColName ) );

                // Alloc has been used instead of AllocL in order not to leave
                // if an error happens in the memory allocation.
                meteringdata->iParentUid = parentuid.Alloc();

                // Insert the instance to the Metering data pointer array
                aMeteringDataList.AppendL( meteringdata );

                CleanupStack::Pop( meteringdata );
                CleanupStack::PopAndDestroy( colset );
                }

            }

        User::LeaveIfError( iDb.Commit() );

        CleanupStack::PopAndDestroy(); // view

        Pop(); // iDb

    DRMLOG( _L( "RDrmMeteringDb::GetL ok" ) );

    return found;

    }

// -----------------------------------------------------------------------------
// RDrmMeteringDb::DeleteL
//
// Delete all the metering data associated to a Rights Issuer from the database.
// Return value is ETrue if at least one entry was found. Return value is EFalse
// if no entry was found. Function will leave if an error happens when accessing
// the database or if the given Rights Issuer Id is either empty or too long.
// -----------------------------------------------------------------------------
//
TBool RDrmMeteringDb::DeleteL( const TDesC8& aRiId )
    {

    DRMLOG( _L( "RDrmMeteringDb::DeleteL" ) );

    // If Rights Issuer Id is empty or is not available
    if ( ( aRiId.Length() == 0 ) || ( aRiId.Length() > KRiIdSize ) )
        {
        User::Leave( KErrArgument );
        }

    TBool found = EFalse;

    RDbView view;

    PushL( iDb );

    InitViewLC( view );

    User::LeaveIfError( iDb.Begin() );

    for ( view.FirstL(); view.AtRow(); view.NextL() )
        {

        view.GetL();

        if ( CompareIDL( view, aRiId ) )
            {
            found = ETrue;
            view.DeleteL();
            }

        }

    if ( found )
        {
        iDb.Compact();
        }

    User::LeaveIfError( iDb.Commit() );

    CleanupStack::PopAndDestroy(); // view

    Pop(); // iDb

    DRMLOG( _L( "RDrmMeteringDb::DeleteL ok" ) );

    return found;

    }

// -----------------------------------------------------------------------------
// RDrmMeteringDb::OpenDbL
//
// Open the database.
// -----------------------------------------------------------------------------
//
void RDrmMeteringDb::OpenDbL( RDbNamedDatabase& aDb,
                               const TDesC& aFileName )
    {

    DRMLOG( _L( "RDrmMeteringDb::OpenDbL" ) );
    CDbTableNames* tables = NULL;

    User::LeaveIfError( aDb.Open( *iFs, aFileName ) );
    CleanupClosePushL( aDb );

    if ( aDb.IsDamaged() )
        {
        User::LeaveIfError( aDb.Recover() );
        }

    // Sanity check
    tables = aDb.TableNamesL();
    CleanupStack::PushL( tables );

    if ( tables->Count() != 1 ||
         ( *tables )[ 0 ].Compare( KMeteringDataTable ) )
        {
        User::Leave( KErrCorrupt );
        }

    CleanupStack::PopAndDestroy( tables );
    CleanupStack::Pop(); // aDb
    DRMLOG( _L( "RDrmMeteringDb::OpenDbL ok" ) );

    }

// -----------------------------------------------------------------------------
// RDrmMeteringDb::ReplaceDbL
//
// Replace the database.
// -----------------------------------------------------------------------------
//
void RDrmMeteringDb::ReplaceDbL( RDbNamedDatabase& aDb,
                                 const TDesC& aFileName )
    {
    DRMLOG( _L( "RDrmMeteringDb::ReplaceDbL" ) );

    CDbColSet*  colSet = NULL;

    // Define column names and their data types
    TDbCol cidCol( KCIDColName, EDbColText8 );
    TDbCol riidCol( KRightIssuerColName, EDbColText8 );
    TDbCol countCol( KCountColName, EDbColUint32  );
    TDbCol accumulatedTimeCol( KAccumulatedTimeColName, EDbColInt32 );
    TDbCol parentUIDCol( KParentUIDColName, EDbColText8 );

    TDoDeleteFile deletefile = { iFs, &aFileName };

    TCleanupItem item( DoDeleteFile, &deletefile );
    CleanupStack::PushL( item );

    User::LeaveIfError( aDb.Replace( *iFs, aFileName ) );
    CleanupClosePushL( aDb );

    // Add columns
    colSet = CDbColSet::NewLC();
    colSet->AddL( cidCol );
    colSet->AddL( riidCol );
    colSet->AddL( countCol );
    colSet->AddL( accumulatedTimeCol );
    colSet->AddL( parentUIDCol);

    // Create indices
    TDbKeyCol cidKeyCol( KCIDColName );
    TDbKeyCol riidKeyCol( KRightIssuerColName );

    CDbKey* key = CDbKey::NewLC();
    key->AddL( cidKeyCol );
    key->AddL( riidKeyCol );
    key->MakeUnique();

    User::LeaveIfError( aDb.Begin() );
    User::LeaveIfError( aDb.CreateTable( KMeteringDataTable, *colSet ) );

    User::LeaveIfError( aDb.CreateIndex( KMeteringDataTable,
                                         KMeteringDataTable,
                                         *key ) );

    User::LeaveIfError( aDb.Commit() );

    CleanupStack::PopAndDestroy( 2, colSet ); // key, colset
    CleanupStack::Pop(); // aDb
    CleanupStack::Pop(); // item

    DRMLOG( _L( "RDrmMeteringDb::ReplaceDbL ok" ) );

    }

// -----------------------------------------------------------------------------
// RDrmMeteringDb::InitViewLC
//
// Initialize the view.
// -----------------------------------------------------------------------------
//
void RDrmMeteringDb::InitViewLC( RDbView& aView )
    {

    DRMLOG( _L( "RDrmMeteringDb::InitViewLC" ) );

    User::LeaveIfError(
        aView.Prepare( iDb,
                       TDbQuery( KViewInitQuery, EDbCompareCollated ),
                       RDbRowSet::EUpdatable ) );

    CleanupClosePushL( aView );

    User::LeaveIfError( aView.EvaluateAll() );

    DRMLOG( _L( "RDrmMeteringDb::InitViewLC ok" ) );

    }

// -----------------------------------------------------------------------------
// RDrmMeteringDb::CompareIDL
//
// Compare the Rights Issuer Id and Content Id to their counterparts in the current
// row of the view. Return value is ETrue only if both the Content Id and the Rights
// Issuer Id match their counterpart Ids in the view.
// -----------------------------------------------------------------------------
//
TBool RDrmMeteringDb::CompareIDL( RDbRowSet& aView,
                                  const TDesC8& aCID,
                                  const TDesC8& aRiId )
    {

    DRMLOG( _L( "RDrmMeteringDb::CompareIDL" ) );

    TBool res = EFalse;

    CDbColSet* colset = aView.ColSetL();
    CleanupStack::PushL( colset );

    TPtrC8 riid = aView.ColDes8( colset->ColNo( KRightIssuerColName ) );
    TBuf8< KRiIdSize > buf = riid;

    TPtrC8 cid = aView.ColDes8( colset->ColNo( KCIDColName ) );
    HBufC8* des = cid.AllocLC();

    if ( ( aRiId.CompareC( buf ) == 0 ) && ( aCID.CompareC( *des ) == 0 ) )
    {
    res = ETrue;
    }

    CleanupStack::PopAndDestroy( des );
    CleanupStack::PopAndDestroy( colset );

    DRMLOG( _L( "RDrmMeteringDb::CompareIDL ok" ) );

    return res;
    }

// -----------------------------------------------------------------------------
// RDrmMeteringDb::CompareIDL
//
// Compare whether the rowset's ID matches the given ID. For comparison of
// Rights Issuer ID. Overloaded.
// -----------------------------------------------------------------------------
//
TBool RDrmMeteringDb::CompareIDL( RDbRowSet& aView,
                                  const TDesC8& aRiId )
    {

    DRMLOG( _L( "RDrmMeteringDb::CompareIDL" ) );

    TBool res = EFalse;

    CDbColSet* colset = aView.ColSetL();
    CleanupStack::PushL( colset );

    TPtrC8 riid = aView.ColDes8( colset->ColNo( KRightIssuerColName ) );
    TBuf8< KRiIdSize > buf = riid;

    if ( aRiId.CompareC( buf ) == 0 )
    {
    res = ETrue;
    }

    CleanupStack::PopAndDestroy( colset );

    DRMLOG( _L( "RDrmMeteringDb::CompareIDL ok" ) );

    return res;
    }


// -----------------------------------------------------------------------------
// RDrmMeteringDb::PushL
//
// Push a cleanup item to cleanup stack.
// -----------------------------------------------------------------------------
//
void RDrmMeteringDb::PushL( RDbDatabase& aDb )
    {
    TCleanupItem item( DoRollBack, &aDb );
    CleanupStack::PushL( item );
    }

// -----------------------------------------------------------------------------
// RDrmMeteringDB::Pop
//
// Pop a cleanup item pushed in by PushL.
// -----------------------------------------------------------------------------
//
void RDrmMeteringDb::Pop()
    {
    CleanupStack::Pop();
    }

#endif

// End of File
