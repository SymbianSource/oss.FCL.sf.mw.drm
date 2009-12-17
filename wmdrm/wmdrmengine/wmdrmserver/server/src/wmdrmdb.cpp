/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  database implementation for WMDRM
*
*/

#include <e32std.h>

#include <systemwarninglevels.hrh>
#include "wmdrmdb.h"
#include "wmdrmfileserverclient.h"
#include "wmdrmdatastore.h"
#include "drmrightsstoringlocation.h"
#include "drmutilityinternaltypes.h"

#define _LOGGING_FILE L"wmdrmserver.txt"

#include "flogger.h"
#include "logfn.h"

#define FROM_HEX_DIGIT(x) (x >= 'a' ? x - 'a' + 10 : x - '0')
#define FROM_HEX(x,y) (FROM_HEX_DIGIT(x) * 16 + FROM_HEX_DIGIT(y))

// For secure, sync and metering stores in the case of differentiation of stores
// to system drive and internal mass drive or for all the stores in the case of 
// no differentation
_LIT( KDatabaseName, "%c:[10282F1B]hds.db" );

// For license store in the case of differentation of stores. Not used
// if no differentation is applied.                                              
_LIT( KDatabaseName2, "%c:[10282F1B]hds2.db" );
_LIT8( KDatabaseConfig, "cache_size=64; page_size=2048; encoding=UTF-8" );

_LIT( KBeginTransaction, "BEGIN TRANSACTION" );
_LIT( KCommitTransaction, "COMMIT TRANSACTION" );

_LIT( KCreateLicenseTableString, "CREATE TABLE IF NOT EXISTS license_t (hashKey BINARY NOT NULL, uniqueKey BINARY NOT NULL, data BINARY, PRIMARY KEY(hashKey, uniqueKey))" );
_LIT( KCreateSecureTableString, "CREATE TABLE IF NOT EXISTS secure_t (hashKey BINARY NOT NULL, uniqueKey BINARY NOT NULL, data BINARY, PRIMARY KEY(hashKey, uniqueKey))" );
_LIT( KCreateSyncTableString, "CREATE TABLE IF NOT EXISTS sync_t (hashKey BINARY NOT NULL, uniqueKey BINARY NOT NULL, data BINARY, PRIMARY KEY(hashKey, uniqueKey))" );
_LIT( KCreateMeteringTableString, "CREATE TABLE IF NOT EXISTS metering_t (hashKey BINARY NOT NULL, uniqueKey BINARY NOT NULL, data BINARY, PRIMARY KEY(hashKey, uniqueKey))" );

_LIT( KDeleteLicenseTableString, "DROP TABLE IF EXISTS license_t" );
_LIT( KDeleteSecureTableString, "DROP TABLE IF EXISTS secure_t" );
_LIT( KDeleteSyncTableString, "DROP TABLE IF EXISTS sync_t" );
_LIT( KDeleteMeteringTableString, "DROP TABLE IF EXISTS metering_t" );

_LIT( KSelectNRowIDsLicenseTableString, "SELECT RowID FROM license_t LIMIT %d" );
_LIT( KSelectNRowIDsSecureTableString, "SELECT RowID FROM secure_t LIMIT %d" );
_LIT( KSelectNRowIDsSyncTableString, "SELECT RowID FROM sync_t LIMIT %d" );
_LIT( KSelectNRowIDsMeteringTableString, "SELECT RowID FROM metering_t LIMIT %d" );

_LIT( KDeleteWithRowIDLicenseTableString, "DELETE FROM license_t WHERE RowID = :rowID" );
_LIT( KDeleteWithRowIDSecureTableString, "DELETE FROM secure_t WHERE RowID = :rowID" );
_LIT( KDeleteWithRowIDSyncTableString, "DELETE FROM sync_t WHERE RowID = :rowID" );
_LIT( KDeleteWithRowIDMeteringTableString, "DELETE FROM metering_t WHERE RowID = :rowID" );

_LIT( KDeleteLicenseString, "DELETE FROM license_t WHERE hashKey = :HashKey AND uniqueKey=:UniqueKey" );
_LIT( KDeleteSecureString, "DELETE FROM secure_t WHERE hashKey = :HashKey AND uniqueKey=:UniqueKey" );
_LIT( KDeleteSyncString, "DELETE FROM sync_t WHERE hashKey = :HashKey AND uniqueKey=:UniqueKey" );
_LIT( KDeleteMeteringString, "DELETE FROM metering_t WHERE hashKey = :HashKey AND uniqueKey=:UniqueKey" );

_LIT( KInsertLicenseString, "INSERT INTO license_t (hashKey, uniqueKey) VALUES (:HashKey, :UniqueKey)" );
_LIT( KInsertSecureString, "INSERT INTO secure_t (hashKey, uniqueKey) VALUES (:HashKey, :UniqueKey)" );
_LIT( KInsertSyncString, "INSERT INTO sync_t (hashKey, uniqueKey) VALUES (:HashKey, :UniqueKey)" );
_LIT( KInsertMeteringString, "INSERT INTO metering_t (hashKey, uniqueKey) VALUES (:HashKey, :UniqueKey)" );

_LIT( KUpdateLicenseDataString, "UPDATE license_t SET data=:Data WHERE hashKey = :HashKey AND uniqueKey=:UniqueKey" );
_LIT( KUpdateSecureDataString, "UPDATE secure_t SET data=:Data WHERE hashKey = :HashKey AND uniqueKey=:UniqueKey" );
_LIT( KUpdateSyncDataString, "UPDATE sync_t SET data=:Data WHERE hashKey = :HashKey AND uniqueKey=:UniqueKey" );
_LIT( KUpdateMeteringDataString, "UPDATE metering_t SET data=:Data WHERE hashKey = :HashKey AND uniqueKey=:UniqueKey" );

_LIT( KSelectLicenseString, "SELECT data FROM license_t WHERE hashKey = :HashKey AND uniqueKey=:UniqueKey" );
_LIT( KSelectSecureString, "SELECT data FROM secure_t WHERE hashKey = :HashKey AND uniqueKey=:UniqueKey" );
_LIT( KSelectSyncString, "SELECT data FROM sync_t WHERE hashKey = :HashKey AND uniqueKey=:UniqueKey" );
_LIT( KSelectMeteringString, "SELECT data FROM metering_t WHERE hashKey = :HashKey AND uniqueKey=:UniqueKey" );

_LIT( KSelectLicenseEnumeratorString, "SELECT uniqueKey FROM license_t WHERE hashKey = :HashKey" );
_LIT( KSelectSecureEnumeratorString, "SELECT uniqueKey FROM secure_t WHERE hashKey = :HashKey" );
_LIT( KSelectSyncEnumeratorString, "SELECT uniqueKey FROM sync_t WHERE hashKey = :HashKey" );
_LIT( KSelectMeteringEnumeratorString, "SELECT uniqueKey FROM metering_t WHERE hashKey = :HashKey" );

_LIT( KSelectLicenseNameSpaceEnumeratorString, "SELECT hashKey, uniqueKey FROM license_t" );
_LIT( KSelectSecureNameSpaceEnumeratorString, "SELECT hashKey, uniqueKey FROM secure_t" );
_LIT( KSelectSyncNameSpaceEnumeratorString, "SELECT hashKey, uniqueKey FROM sync_t" );
_LIT( KSelectMeteringNameSpaceEnumeratorString, "SELECT hashKey, uniqueKey FROM metering_t" );

_LIT( KHashKeyColumn, "hashkey" );
_LIT( KHashKeyParameter, ":HashKey" );
_LIT( KUniqueKeyColumn, "uniquekey" );
_LIT( KUniqueKeyParameter, ":UniqueKey" );
_LIT( KDataColumn, "data" );
_LIT( KDataParameter, ":Data" );
_LIT( KRowIDColumn, "rowID" );
_LIT( KRowIDParameter, ":rowID" );

_LIT8( KSyncStore, "syncstore" );
_LIT8( KMeteringStore, "meteringstore" );
_LIT8( KSecureStore, "securestore" );
_LIT8( KLicenseStore, "licstore" );
_LIT8( KHds, "hds" );

const TSecureId KWmDrmServerSecureId( 0x10282F1B );
const TInt KMaxTIntBufLength( 10 );
const TInt KLicenseSize( 5 * 1024 );
const TInt KSixtySeconds( 60 );
const TInt KMaxOperationsUntilCommit( 60 );

// ---------------------------------------------------------------------------
// CWmDrmDb::CWmDrmDb
// ---------------------------------------------------------------------------
//
CWmDrmDb::CWmDrmDb( CWmDrmServer* aServer ) : CTimer( EPriorityHigh ), 
    iServer( aServer ),
    iAmountOperationsWithoutCommit( 0 ),
    iSqlTransactionOngoing( EFalse ),
    iSqlTransactionOngoing2( EFalse )   
    {
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// CWmDrmDb::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDb::ConstructL()
    {
    LOGFN( "CWmDrmDb::ConstructL" );
    TDrmScheme drmScheme( EDrmSchemeWmDrm );
    TChar driveLetter;
	TChar systemDriveLetter;
    
    CTimer::ConstructL();
    
    // Check which drive is configured in the Central Repository Key
    // for the desired storing location of WM DRM rights.
	iWmDrmRightsConfigFound = DrmRightsStoringLocation::CheckDrmRightsStorageDriveL( 
        iServer->Fs(), drmScheme, driveLetter );
    
    // Format the default database path on the system drive and open
    // the database. If the storing location is not configured, this database
    // will hold all the four stores, otherwise only the secure and metering
    // stores.
	if ( !iWmDrmRightsConfigFound )
		{
    	iDatabasePath.Format( KDatabaseName, (TUint)driveLetter );
    	OpenDatabaseL( iDatabasePath, iDatabase, EFalse );
    	}
    else 
        {
        // Format the database path also on the internal mass drive and 
        // open both databases. The database on the default system drive
        // includes metering and secure stores, whereas the store on the
        // configured drive includes sync and license stores.
		iDatabasePath.Format( KDatabaseName, 
		    (TUint)iServer->Fs().GetSystemDriveChar() );
        iDatabasePath2.Format( KDatabaseName2, (TUint)driveLetter );
        OpenDatabaseL( iDatabasePath, iDatabase, EFalse );
		OpenDatabaseL( iDatabasePath2, iDatabase2, ETrue );
        }               
    }


// ---------------------------------------------------------------------------
// CWmDrmDb::NewL
// ---------------------------------------------------------------------------
//
CWmDrmDb* CWmDrmDb::NewL( CWmDrmServer* aServer )
    {
    LOGFN( "CWmDrmDb::NewL" );
    CWmDrmDb* self( CWmDrmDb::NewLC( aServer ) );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDb::NewLC
// ---------------------------------------------------------------------------
//
CWmDrmDb* CWmDrmDb::NewLC( CWmDrmServer* aServer )
    {
    LOGFN( "CWmDrmDb::NewLC" );
    CWmDrmDb* self( new( ELeave ) CWmDrmDb( aServer ) );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDb::~CWmDrmDb
// ---------------------------------------------------------------------------
//
CWmDrmDb::~CWmDrmDb()
    {
    LOGFN( "CWmDrmDb::~CWmDrmDb" );
    
    TInt error( KErrNone );
    
    if ( IsActive() )
        {
        Cancel();
        }
    
    // Checks if there are prepared SQL operations that need to be 
    // committed before the object is destroyed.
    TRAP( error, CheckDatabaseCommitL( ETrue ) );
       
    delete iData;
    iEnumerateNamespaceStmt.Close();
    iDatabase.Close();
    if ( iWmDrmRightsConfigFound )
		{
		iDatabase2.Close();
		}
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::RunL
// ---------------------------------------------------------------------------
//
void CWmDrmDb::RunL()
    {
    LOGFN( "CWmDrmDb::RunL()" );
    
    switch ( iStatus.Int() )
        {
        case KErrNone:
            // Normal completition
            
        case KErrUnderflow:
            // Time has already passed.
            
        case KErrAbort:
            // System time changed
            CheckDatabaseCommitL( ETrue );
            break;

        default:
            // Some other (real) error
            // Handled in RunError
            User::Leave( iStatus.Int() );
        }
    }

// ------------------------------------------------------------------------
// CWmDrmDb::RunError
// ------------------------------------------------------------------------
//
#if defined( _DEBUG ) || defined( _LOGGING )
TInt CWmDrmDb::RunError( TInt aError )
#else
TInt CWmDrmDb::RunError( TInt /* aError */ )
#endif
    {
    LOG2( "CWmDrmDb::RunError %d", aError );
    // Continue normally and return KErrNone.
    return KErrNone;    
    }
    
// ------------------------------------------------------------------------
// CDRMConsume:: Activate
// ------------------------------------------------------------------------
void CWmDrmDb::Activate()
    {
    LOGFN( "CWmDrmDb::Activate()" );
    
    // Activate one minute timer
    After( TTimeIntervalMicroSeconds32( KSixtySeconds * 1000000 ) );
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::InitStoreL 
// ---------------------------------------------------------------------------
//
void CWmDrmDb::InitStoreL( 
    const TDesC8& aStore,
    TBool /*aCreateIfMissing*/ )
    {
    LOGFN( "CWmDrmDb::InitStoreL" );
    if ( aStore.CompareC( KHds ) == 0 )
        {
        if ( iWmDrmRightsConfigFound )
            {
            CreateDatabaseL( iDatabasePath, iDatabase, EFalse );
            CreateDatabaseL( iDatabasePath2, iDatabase2, ETrue );
            }
        else
            {
            CreateDatabaseL( iDatabasePath, iDatabase, EFalse );
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::InitNameSpaceL 
// ---------------------------------------------------------------------------
//    
void CWmDrmDb::InitNameSpaceL( 
    const TDesC8& /*aStore*/,
    const TDesC8& aNamespace,
    TBool /*aCreateIfMissing*/ )
    {
    LOGFN( "CWmDrmDb::InitializeL" );
    TWmDrmTableType tableType( TableTypeL( aNamespace ) );
    if ( ( tableType == EWmDrmLicenseTable ) && iWmDrmRightsConfigFound )
    		{  
    		CreateTableL( tableType, iDatabase2 );
    		}
    else
    		{		
  			CreateTableL( tableType, iDatabase );
  			}
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::RemoveStoreL 
// ---------------------------------------------------------------------------
//
void CWmDrmDb::RemoveStoreL( const TDesC8& aStore )
    {
    LOGFN( "CWmDrmDb::RemoveStoreL" );
    if ( aStore.CompareC( KHds ) == 0 )
        {
        // Delete databases
        iDatabase.Close();
        User::LeaveIfError( RSqlDatabase::Delete( iDatabasePath ) );
        
        // If WM DRM rights storing configuration is found -> delete also
        // the database on the internal mass drive.
        if ( iWmDrmRightsConfigFound )
            {
            iDatabase2.Close();
            User::LeaveIfError( RSqlDatabase::Delete( iDatabasePath2 ) );
            CreateDatabaseL( iDatabasePath, iDatabase, EFalse );
            CreateDatabaseL( iDatabasePath2, iDatabase2, ETrue );
            }
        else
            {
            CreateDatabaseL( iDatabasePath, iDatabase, EFalse );
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::RemoveNameSpaceL 
// ---------------------------------------------------------------------------
//
void CWmDrmDb::RemoveNameSpaceL( 
    const TDesC8& /*aStore*/,
    const TDesC8& aNamespace )
    {
    LOGFN( "CWmDrmDb::RemoveNameSpaceL" );
    TWmDrmTableType tableType( TableTypeL( aNamespace ) );
    DeleteTableL( tableType );
    if ( ( tableType == EWmDrmLicenseTable ) && iWmDrmRightsConfigFound )
    		{  
    		CreateTableL( tableType, iDatabase2 );
    		}
    else
    		{			
  			CreateTableL( tableType, iDatabase );
  			}
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::DeleteLicenseStoreL
// ---------------------------------------------------------------------------
//
void CWmDrmDb::DeleteLicenseStoreL()
    {
    LOGFN( "CWmDrmDb::DeleteLicenseStoreL" );
    
   	DeleteTableL( EWmDrmLicenseTable );
    DeleteTableL( EWmDrmSecureTable );
    DeleteTableL( EWmDrmSyncTable );
    
    if ( iWmDrmRightsConfigFound )
    		{
    		CreateTableL( EWmDrmLicenseTable, iDatabase2 );
   			}
    else
    		{
    		CreateTableL( EWmDrmLicenseTable, iDatabase );
    		}
    CreateTableL( EWmDrmSecureTable, iDatabase );
	CreateTableL( EWmDrmSyncTable, iDatabase );		
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::CreateRecordL
// ---------------------------------------------------------------------------
//
void CWmDrmDb::CreateRecordL(   
    const TDesC8& /*aStore*/,
    const TDesC8& aNamespace,
    const TDesC8& aHashKey,
    const TDesC8& aUniqueKey,
    const TInt&   aSize )
    {
    LOGFN( "CWmDrmDb::CreateRecordL" );
    TPtrC statementString( NULL, 0 );
    
    // Check if we should begin transaction, cache operation or commit
    // cached operations 
    CheckDatabaseCommitL( EFalse );
    
    // First, find the table where record should be created
    TWmDrmTableType tableType( TableTypeL( aNamespace ) );
    
    // Update the dummy database file at this point.
    if ( iWmDrmRightsConfigFound && ( tableType == EWmDrmLicenseTable ) )
        {
        iServer->DataStore()->UpdateDummyDbFileL( aSize, ETrue );
        }
    else 
    	{
    	iServer->DataStore()->UpdateDummyDbFileL( aSize, EFalse );
    	} 	    
        
    // bind sql statement to database
    RSqlStatement stmt;
    CleanupClosePushL( stmt );
    
    //Select correct insert string
    switch ( tableType )
        {
        case EWmDrmLicenseTable:
            statementString.Set( KInsertLicenseString().Ptr(), 
                                 KInsertLicenseString().Length() );
            if ( iWmDrmRightsConfigFound )
                {
                stmt.PrepareL( iDatabase2, statementString );
                }
            else 
                {
                stmt.PrepareL( iDatabase, statementString );
                }           
            break;
        case EWmDrmSecureTable:
            statementString.Set( KInsertSecureString().Ptr(), 
                                 KInsertSecureString().Length() );
            stmt.PrepareL( iDatabase, statementString );                     
            break;
        case EWmDrmSyncTable:
            statementString.Set( KInsertSyncString().Ptr(), 
                                 KInsertSyncString().Length() );        
        	stmt.PrepareL( iDatabase, statementString );       
            break;
        case EWmDrmMeteringTable:
            statementString.Set( KInsertMeteringString().Ptr(), 
                                 KInsertMeteringString().Length() );
            stmt.PrepareL( iDatabase, statementString );                           
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }

    // Get index of hash key parameter
    TInt hashKeyIndex( stmt.ParameterIndex( KHashKeyParameter ) );
    User::LeaveIfError( hashKeyIndex );
    
    // Replace hash key parameter
    User::LeaveIfError( stmt.BindBinary( hashKeyIndex, aHashKey ) );
    
    // Get index of unique key parameter
    TInt uniqueKeyIndex( stmt.ParameterIndex( KUniqueKeyParameter ) );
    User::LeaveIfError( uniqueKeyIndex );
    
    // Replace unique key parameter
    User::LeaveIfError( stmt.BindBinary( uniqueKeyIndex, aUniqueKey ) );
    
    TInt err( KErrNone );
    // Execute SQL statement
    err = stmt.Exec();
    if ( err == KSqlErrConstraint )
        {
        User::Leave( KErrAlreadyExists );
        }
    User::LeaveIfError( err );
    // Close SQL statement
    CleanupStack::PopAndDestroy( &stmt );
    
    // Update the dummy database file at this point.
    if ( iWmDrmRightsConfigFound && ( tableType == EWmDrmLicenseTable ) )
        {
        iServer->DataStore()->UpdateDummyDbFileL( 0, ETrue );
        }
    else 
    	{    
    	iServer->DataStore()->UpdateDummyDbFileL( 0, EFalse );
    	}
    		
    iAmountOperationsWithoutCommit++;	
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::ReadRecordL
// ---------------------------------------------------------------------------
//
void CWmDrmDb::ReadRecordL(  
    const TDesC8& /*aStore*/,
    const TDesC8& aNamespace,
    const TDesC8& aHashKey,
    const TDesC8& aUniqueKey )
    {
    LOGFN( "CWmDrmDb::ReadRecordL" );
    TPtrC statementString( NULL, 0 );
    // First, find from which table record should be read
    TWmDrmTableType tableType( TableTypeL( aNamespace ) );
    
    // bind sql statement to database
    RSqlStatement stmt;
    CleanupClosePushL( stmt );
    
    // Select correct insert string
    switch ( tableType )
        {
        case EWmDrmLicenseTable:
            statementString.Set( KSelectLicenseString().Ptr(), 
                                 KSelectLicenseString().Length() );
            if ( iWmDrmRightsConfigFound )
                {
                stmt.PrepareL( iDatabase2, statementString );
                }
            else 
                {
                stmt.PrepareL( iDatabase, statementString );
                }                                           
            break;
        case EWmDrmSecureTable:
            statementString.Set( KSelectSecureString().Ptr(), 
                                 KSelectSecureString().Length() );
            stmt.PrepareL( iDatabase, statementString );
            break;
        case EWmDrmSyncTable:
            statementString.Set( KSelectSyncString().Ptr(), 
                                 KSelectSyncString().Length() );     
            stmt.PrepareL( iDatabase, statementString );                      
            break;
        case EWmDrmMeteringTable:
            statementString.Set( KSelectMeteringString().Ptr(), 
                                 KSelectMeteringString().Length() );
            stmt.PrepareL( iDatabase, statementString );
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }
    
    // Get index of hash key parameter
    TInt hashKeyIndex( stmt.ParameterIndex( KHashKeyParameter ) );
    User::LeaveIfError( hashKeyIndex );

    // Replace key parameter
    User::LeaveIfError( stmt.BindBinary( hashKeyIndex, aHashKey ) );

    TInt uniqueKeyIndex( stmt.ParameterIndex( KUniqueKeyParameter ) );
    User::LeaveIfError( uniqueKeyIndex );
    
    // Replace unique key parameter
    User::LeaveIfError( stmt.BindBinary( uniqueKeyIndex, aUniqueKey ) );
    
    // Read data
    TInt dataColumnIndex( stmt.ColumnIndex( KDataColumn ) );
    User::LeaveIfError( dataColumnIndex );

    if ( stmt.Next() == KSqlAtRow )
        {
        // If there is previously cached data, delete it
        delete iData;
        iData = NULL;
        
        TPtrC8 data( NULL, 0 );
        User::LeaveIfError( stmt.ColumnBinary( dataColumnIndex, data ) );
        iData = data.AllocL();
        }
    else 
        {
        User::Leave( KErrNotFound );
        }

    // Close SQL statement
    CleanupStack::PopAndDestroy( &stmt );
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::GetDataSizeL 
// ---------------------------------------------------------------------------
//
void CWmDrmDb::GetDataSizeL( TInt& aDataSize )
    {
    LOGFN( "CWmDrmDb::GetDataSizeL" );
    User::LeaveIfNull( iData );
    aDataSize = iData->Size();
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::DeleteData 
// ---------------------------------------------------------------------------
//
void CWmDrmDb::DeleteData()
    {
    LOGFN( "CWmDrmDb::DeleteData" );
    delete iData;
    iData = NULL;
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::DeleteRecordL
// ---------------------------------------------------------------------------
//
void CWmDrmDb::DeleteRecordL(   
    const TDesC8& /*aStore*/,
    const TDesC8& aNamespace,
    const TDesC8& aHashKey,
    const TDesC8& aUniqueKey )
    {
    LOGFN( "CWmDrmDb::DeleteRecordL" );
    TPtrC statementString( NULL, 0 );
    
    // Check if we should begin transaction, cache operation or commit
    // cached operations 
    CheckDatabaseCommitL( EFalse );
    
    // First, find the table where record should be created
    TWmDrmTableType tableType( TableTypeL( aNamespace ) );
    
    // bind sql statement to database
    RSqlStatement stmt;
    CleanupClosePushL( stmt );
    
    //Select correct insert string
    switch ( tableType )
        {
        case EWmDrmLicenseTable:
            statementString.Set( KDeleteLicenseString().Ptr(), 
                                 KDeleteLicenseString().Length() );
            if ( iWmDrmRightsConfigFound )
                {
                stmt.PrepareL( iDatabase2, statementString );
                }
            else 
                {
                stmt.PrepareL( iDatabase, statementString );
                }                      
            break;
        case EWmDrmSecureTable:
            statementString.Set( KDeleteSecureString().Ptr(), 
                                 KDeleteSecureString().Length() );
            stmt.PrepareL( iDatabase, statementString );                     
            break;
        case EWmDrmSyncTable:
            statementString.Set( KDeleteSyncString().Ptr(), 
                                 KDeleteSyncString().Length() );
            stmt.PrepareL( iDatabase, statementString );
            break;
        case EWmDrmMeteringTable:
            statementString.Set( KDeleteMeteringString().Ptr(), 
                                 KDeleteMeteringString().Length() );
            stmt.PrepareL( iDatabase, statementString );                     
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }

    // Get index of hash key parameter
    TInt hashKeyIndex( stmt.ParameterIndex( KHashKeyParameter ) );
    User::LeaveIfError( hashKeyIndex );
    
    // Replace hash key parameter
    User::LeaveIfError( stmt.BindBinary( hashKeyIndex, aHashKey ) );
    
    // Get index of unique key parameter
    TInt uniqueKeyIndex( stmt.ParameterIndex( KUniqueKeyParameter ) );
    User::LeaveIfError( uniqueKeyIndex );
    
    // Replace unique key parameter
    User::LeaveIfError( stmt.BindBinary( uniqueKeyIndex, aUniqueKey ) );
    
    // Execute SQL statement
    User::LeaveIfError( stmt.Exec() );
    
    // Update dummy database file at this point.
    if ( iWmDrmRightsConfigFound && ( tableType == EWmDrmLicenseTable ) )
        {
        iServer->DataStore()->UpdateDummyDbFileL( 0, ETrue );
        }
    else 
    	{
    	iServer->DataStore()->UpdateDummyDbFileL( 0, EFalse );
    	} 	  
    
    // Close SQL statement
    CleanupStack::PopAndDestroy( &stmt );
    
    iAmountOperationsWithoutCommit++;
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::ReadDataL
// ---------------------------------------------------------------------------
//
void CWmDrmDb::ReadDataL( TDes8& aData )
    {
    LOGFN( "CWmDrmDb::ReadDataL" );
    User::LeaveIfNull( iData );
    aData.Copy( *iData );
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::WriteDataL
// ---------------------------------------------------------------------------
//
void CWmDrmDb::WriteDataL(   
    const TDesC8& /*aStore*/,
    const TDesC8& aNamespace,
    const TDesC8& aHashKey,
    const TDesC8& aUniqueKey, 
    TDesC8& aData )
    {
    LOGFN( "CWmDrmDb::WriteDataL" );
    TPtrC statementString( NULL, 0 );
    
    // Check if we should begin transaction, cache operation or commit
    // cached operations 
    CheckDatabaseCommitL( EFalse );
    
    // First, find from which table record should be read
    TWmDrmTableType tableType( TableTypeL( aNamespace ) );
    
    // Update the dummy database file at this point.
    if ( iWmDrmRightsConfigFound && ( tableType == EWmDrmLicenseTable ) )
        {
        iServer->DataStore()->UpdateDummyDbFileL( aData.Size(), ETrue );
        }
    else 
    	{
    	iServer->DataStore()->UpdateDummyDbFileL( aData.Size(), EFalse );
    	} 	  
    
    // bind sql statement to database
    RSqlStatement stmt;
    CleanupClosePushL( stmt );
    
    //Select correct insert string
    switch ( tableType )
        {
        case EWmDrmLicenseTable:
            statementString.Set( KUpdateLicenseDataString().Ptr(), 
                                 KUpdateLicenseDataString().Length() );
            if ( iWmDrmRightsConfigFound )
                {
                stmt.PrepareL( iDatabase2, statementString );
                }
            else 
                {
                stmt.PrepareL( iDatabase, statementString );
                }                                           
            break;
        case EWmDrmSecureTable:
            statementString.Set( KUpdateSecureDataString().Ptr(), 
                                 KUpdateSecureDataString().Length() );
            stmt.PrepareL( iDatabase, statementString );                     
            break;
        case EWmDrmSyncTable:
            statementString.Set( KUpdateSyncDataString().Ptr(), 
                                 KUpdateSyncDataString().Length() );
            stmt.PrepareL( iDatabase, statementString );                                          
            break;
        case EWmDrmMeteringTable:
            statementString.Set( KUpdateMeteringDataString().Ptr(), 
                                 KUpdateMeteringDataString().Length() );
            stmt.PrepareL( iDatabase, statementString );
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }
    
    // Get index of hash key parameter
    TInt hashKeyIndex( stmt.ParameterIndex( KHashKeyParameter ) );
    User::LeaveIfError( hashKeyIndex );

    // Replace key parameter
    User::LeaveIfError( stmt.BindBinary( hashKeyIndex, aHashKey ) );

    // Get index of unique key parameter
    TInt uniqueKeyIndex( stmt.ParameterIndex( KUniqueKeyParameter ) );
    User::LeaveIfError( uniqueKeyIndex );

    // Replace key parameter
    User::LeaveIfError( stmt.BindBinary( uniqueKeyIndex, aUniqueKey ) );

    // Get index of value parameter
    TInt dataIndex( stmt.ParameterIndex( KDataParameter ) );
    User::LeaveIfError( dataIndex );

    // Replace value parameter
    User::LeaveIfError( stmt.BindBinary( dataIndex, aData ) );

    // Execute SQL statement
    LOG1( "CWmDrmDb::WriteDataL exec" );
    User::LeaveIfError( stmt.Exec() );
    LOG1( "CWmDrmDb::WriteDataL exec ok" );
    
    // Close SQL statement
    CleanupStack::PopAndDestroy( &stmt );
    
    iAmountOperationsWithoutCommit++;
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::EnumerateDataL
// ---------------------------------------------------------------------------
//
void CWmDrmDb::EnumerateDataL(  
    const TDesC8& /*aStore*/,
    const TDesC8& aNamespace,
    const TDesC8& aHashKey,
    RPointerArray<HBufC8>& aUniqueKeyEntries )
    {
    LOGFN( "CWmDrmDb::EnumerateDataL" );
    TPtrC statementString( NULL, 0 );
    // First, find from which table record should be read
    TWmDrmTableType tableType( TableTypeL( aNamespace ) );
    
    // bind sql statement to database
    RSqlStatement stmt;
    CleanupClosePushL( stmt );
    
    //Select correct insert string
    switch ( tableType )
        {
        case EWmDrmLicenseTable:
            statementString.Set( KSelectLicenseEnumeratorString().Ptr(), 
                                 KSelectLicenseEnumeratorString().Length() );
            if ( iWmDrmRightsConfigFound )
                {
                stmt.PrepareL( iDatabase2, statementString );
                }
            else 
                {
                stmt.PrepareL( iDatabase, statementString );
                }                                                
            break;
        case EWmDrmSecureTable:
            statementString.Set( KSelectSecureEnumeratorString().Ptr(), 
                                 KSelectSecureEnumeratorString().Length() );
            stmt.PrepareL( iDatabase, statementString );                     
            break;
        case EWmDrmSyncTable:
            statementString.Set( KSelectSyncEnumeratorString().Ptr(), 
                                 KSelectSyncEnumeratorString().Length() );
            stmt.PrepareL( iDatabase, statementString );                       
            break;
        case EWmDrmMeteringTable:
            statementString.Set( KSelectMeteringEnumeratorString().Ptr(), 
                                 KSelectMeteringEnumeratorString().Length() );
            stmt.PrepareL( iDatabase, statementString );
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }

    TInt hashKeyIndex( stmt.ParameterIndex( KHashKeyParameter ) );
    User::LeaveIfError( hashKeyIndex );
    
    // Replace hash key parameter
    User::LeaveIfError( stmt.BindBinary( hashKeyIndex, aHashKey ) );

    TInt uniqueKeyColumnIndex( stmt.ColumnIndex( KUniqueKeyColumn ) );
    User::LeaveIfError( uniqueKeyColumnIndex );

    TInt err( KErrNone );
    TInt index( 0 );
    TPtrC8 uniqueKeyPtr( NULL, 0 );
    HBufC8* uniquekey( NULL );
    while( ( err = stmt.Next() ) == KSqlAtRow )
        {
        index++;
        User::LeaveIfError( 
            stmt.ColumnBinary( uniqueKeyColumnIndex, uniqueKeyPtr ) );
        uniquekey = uniqueKeyPtr.AllocLC();
        aUniqueKeyEntries.AppendL( uniquekey );
        CleanupStack::Pop( uniquekey );
        }
    if ( err != KSqlAtEnd )
        {
        User::Leave( err );
        }
    if ( index == 0 )
        {
        User::Leave( KErrNotFound );
        }

    // Close SQL statement
    CleanupStack::PopAndDestroy( &stmt );
    }
    
// ---------------------------------------------------------------------------
// CWmDrmDb::EnumerateNameSpaceStartL
// ---------------------------------------------------------------------------
//
void CWmDrmDb::EnumerateNameSpaceStartL(  
    const TDesC8& /*aStore*/,
    const TDesC8& aNamespace )
    {
    LOGFN( "CWmDrmDb::EnumerateNameSpaceStartL" );
    TPtrC statementString( NULL, 0 );
    // First, find from which table record should be read
    TWmDrmTableType tableType( TableTypeL( aNamespace ) );
    
    //Select correct insert string
    switch ( tableType )
        {
        case EWmDrmLicenseTable:
            statementString.Set( KSelectLicenseNameSpaceEnumeratorString().Ptr(), 
                                 KSelectLicenseNameSpaceEnumeratorString().Length() );
            // bind sql statement to database                     
            iEnumerateNamespaceStmt.Close();
            if ( iWmDrmRightsConfigFound )
                {
                iEnumerateNamespaceStmt.PrepareL( iDatabase2, statementString );
                }
            else 
                {
                iEnumerateNamespaceStmt.PrepareL( iDatabase, statementString );
                }                     
            break;
        case EWmDrmSecureTable:
            statementString.Set( KSelectSecureNameSpaceEnumeratorString().Ptr(), 
                                 KSelectSecureNameSpaceEnumeratorString().Length() );                     
            iEnumerateNamespaceStmt.Close();
            iEnumerateNamespaceStmt.PrepareL( iDatabase, statementString );                     
            break;
        case EWmDrmSyncTable:
            statementString.Set( KSelectSyncNameSpaceEnumeratorString().Ptr(), 
                                 KSelectSyncNameSpaceEnumeratorString().Length() );                     
            iEnumerateNamespaceStmt.Close();                     
            iEnumerateNamespaceStmt.PrepareL( iDatabase, statementString );
            break;
        case EWmDrmMeteringTable:
            statementString.Set( KSelectMeteringNameSpaceEnumeratorString().Ptr(), 
                                 KSelectMeteringNameSpaceEnumeratorString().Length() );                     
            iEnumerateNamespaceStmt.Close();                     
            iEnumerateNamespaceStmt.PrepareL( iDatabase, statementString );                     
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }
    
    iEnumerateNamespaceHashKeyColumnIndex = 
        iEnumerateNamespaceStmt.ColumnIndex( KHashKeyColumn );
    User::LeaveIfError( iEnumerateNamespaceHashKeyColumnIndex );
    
    iEnumerateNamespaceUniqueKeyColumnIndex = 
        iEnumerateNamespaceStmt.ColumnIndex( KUniqueKeyColumn );
    User::LeaveIfError( iEnumerateNamespaceUniqueKeyColumnIndex );
    iEnumerateNamespaceStarted = ETrue;
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::EnumerateNameSpaceNextL
// ---------------------------------------------------------------------------
//    
void CWmDrmDb::EnumerateNameSpaceNextL(  
    TBuf8<KWmDrmIdSize>& aHashKey,
    TBuf8<KWmDrmIdSize>& aUniqueKey )
    {
    LOGFN( "CWmDrmDb::EnumerateNameSpaceNextL" );
    if ( !iEnumerateNamespaceStarted )
        {
        User::Leave( KErrNotReady );
        }
    if ( iEnumerateNamespaceStmt.Next() == KSqlAtRow )
        {
        TPtrC8 hashKey( NULL, 0 );
        TPtrC8 uniqueKey( NULL, 0 );
        User::LeaveIfError( 
            iEnumerateNamespaceStmt.ColumnBinary( 
                iEnumerateNamespaceHashKeyColumnIndex, hashKey ) );
        aHashKey.Copy( hashKey );
        User::LeaveIfError( 
            iEnumerateNamespaceStmt.ColumnBinary( 
                iEnumerateNamespaceUniqueKeyColumnIndex, uniqueKey ) );
        aUniqueKey.Copy( uniqueKey );
        }
    else 
        {
        iEnumerateNamespaceStmt.Close();
        iEnumerateNamespaceStarted = EFalse;
        User::Leave( KErrNotFound );
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::DataBaseSize
// ---------------------------------------------------------------------------
//
TInt CWmDrmDb::DataBaseSize( TBool aConfiguredDrive )
    {
    // Find the database size for the database which is located in the
    // configured drive
    if ( aConfiguredDrive )
        {
        return iDatabase2.Size();
        }
    else
        {
        return iDatabase.Size();
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::CreateDatabaseL
// ---------------------------------------------------------------------------
//
void CWmDrmDb::CreateDatabaseL( TFileName& aFileNamePath, RSqlDatabase&
    aSqlDatabase, TBool aConfiguredDrive )
    {
    LOGFN( "CWmDrmDb::CreateDatabaseL" );
    TSecurityPolicy defaultPolicy( KWmDrmServerSecureId );
    RSqlSecurityPolicy securityPolicy;
    securityPolicy.CreateL( defaultPolicy );
    CleanupClosePushL( securityPolicy );
 
    aSqlDatabase.CreateL( aFileNamePath, securityPolicy, &KDatabaseConfig );
 
    if ( aConfiguredDrive ) 
        {
        // Create the license store to the configured drive
        CreateTableL( EWmDrmLicenseTable, aSqlDatabase );   
        }
    else 
        {
        // Secure, metering and sync stores are located on the system drive in
        // configured and non-configured WM DRM rights storing location case.
        CreateTableL( EWmDrmSecureTable, aSqlDatabase );
        CreateTableL( EWmDrmMeteringTable, aSqlDatabase );
		CreateTableL( EWmDrmSyncTable, aSqlDatabase );
		
        if ( !iWmDrmRightsConfigFound )
            {
            CreateTableL( EWmDrmLicenseTable, aSqlDatabase );
            }
        }
    
    CleanupStack::PopAndDestroy( &securityPolicy );
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::CreateTableL
// ---------------------------------------------------------------------------
//
void CWmDrmDb::CreateTableL( TWmDrmTableType aTableType, RSqlDatabase& aDatabase )
    {
    LOGFN( "CWmDrmDb::CreateTableL" );
    switch ( aTableType )
        {
        case EWmDrmLicenseTable:
            User::LeaveIfError( aDatabase.Exec( KCreateLicenseTableString ) );
            break;
        case EWmDrmSecureTable:
            User::LeaveIfError( aDatabase.Exec( KCreateSecureTableString ) );
            break;
        case EWmDrmSyncTable:
            User::LeaveIfError( aDatabase.Exec( KCreateSyncTableString ) );
            break;
        case EWmDrmMeteringTable:
            User::LeaveIfError( aDatabase.Exec( KCreateMeteringTableString ) );
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::DeleteTableL
// ---------------------------------------------------------------------------
//
void CWmDrmDb::DeleteTableL( TWmDrmTableType aTableType )
    {
    LOGFN( "CWmDrmDb::DeleteTableL" );
    TInt count( 1 );
    TInt64 freeSpace( 0 );
    TInt databaseSize( 0 );
    
    // Commit the cached database operations before deleting the table
    CheckDatabaseCommitL( ETrue );
        
    // Check the free space from the system drive or internal mass drive
    // depending on the WM DRM rights storing location configuration and
    // the table type of the database
    if ( iWmDrmRightsConfigFound && ( aTableType == EWmDrmLicenseTable ) )
        {
        freeSpace = iServer->FreeSpaceL( ETrue );
        databaseSize = DataBaseSize( ETrue );
        User::LeaveIfError( databaseSize );
        
        //Just to make sure we don't have reserved space in usage
        iDatabase2.ReleaseReserveAccess();
        }
    else 
        {
        freeSpace = iServer->FreeSpaceL( EFalse );
        databaseSize = DataBaseSize( EFalse );
        User::LeaveIfError( databaseSize );
        iDatabase.ReleaseReserveAccess();
        }
    
    //If size of the database < freespace or there aren't anymore any 
    //records in the table, then drop whole table.
    //Otherwise loop and delete as many records as possible with
    //current amount of free space.
    while ( databaseSize > freeSpace && count > 0 )
        {
        
        //Get reserved space to usage
        if ( iWmDrmRightsConfigFound && ( aTableType == EWmDrmLicenseTable ) )
            {
            User::LeaveIfError( iDatabase2.GetReserveAccess() );
            }
        else
            {
            User::LeaveIfError( iDatabase.GetReserveAccess() );
            } 
        
        //Estimate how many records can be deleted with current amount of free space
        TInt number( 0 );
        //Very low free space, OOD-notes shown, target to get off as soon as possible
        //without risking deletion success
        if ( freeSpace <= KDRIVECWARNINGTHRESHOLD )
            {
            LOG1( "Disk space under warning level" );
            if ( freeSpace <= KDRIVECCRITICALTHRESHOLD )
                {
                LOG1( "Disk space under critical level" );
                //Use reserved space
                number = 5;
                }
            else
                {
                //Use reserved space and space over critical level
                TInt spaceOverCritical( freeSpace - KDRIVECCRITICALTHRESHOLD );
                LOG2( "spaceOverCritical: %d", spaceOverCritical );
                number = spaceOverCritical / KLicenseSize + 5;
                }
            }
        //Disk is still almost full, but we don't wan't to get any OOD-notes anymore
        else if ( freeSpace <= ( 2 * KDRIVECWARNINGTHRESHOLD ) )
            {
            LOG1( "Disk almost full, delete with caution and try to avoid OOD-notes" );
            TInt spaceOverWarning( freeSpace - KDRIVECWARNINGTHRESHOLD );
            LOG2( "spaceOverWarning: %d", spaceOverWarning );
            //Use 1/2 space over warning level, OOD-note might still be shown
            number = ( spaceOverWarning / 2 ) / KLicenseSize;
            }
        //Now there is enough space to delete larger number of records
        //Make absolutely sure we don't anymore get OOD-notes
        else
            {
            TInt spaceOver2Warnings( freeSpace - 2 * KDRIVECWARNINGTHRESHOLD );
            LOG2( "spaceOver2Warnings: %d", spaceOver2Warnings );
            //Use space over 2 * warning level, no OOD-notes
            number = spaceOver2Warnings / KLicenseSize;
            }
        if ( number <= 0 )
            {
            number = 5;
            }
        
        LOG2( "number: %d", number );
        
        //Array to hold rowIDS of to be deleted records
        RArray<TInt64> array;
        CleanupClosePushL( array );
        
        //Get number of rowIDs to array 
        SelectNRecordsWithRowIdL( aTableType, number, array );
        //Get amount of selected rowIds
        count = array.Count();
        
        //Begin transaction
        if ( iWmDrmRightsConfigFound && ( aTableType == EWmDrmLicenseTable ) )
            {
            User::LeaveIfError( iDatabase2.Exec( KBeginTransaction ) );
            iSqlTransactionOngoing2 = ETrue;
            //Execute delete with rowIds
            DeleteRecordsWithRowIdsL( aTableType, array );
            //Commit transaction
            User::LeaveIfError( iDatabase2.Exec( KCommitTransaction ) );
            iSqlTransactionOngoing2 = EFalse;
            }
        else 
            {
            User::LeaveIfError( iDatabase.Exec( KBeginTransaction ) );
            iSqlTransactionOngoing = ETrue;
            DeleteRecordsWithRowIdsL( aTableType, array );
            User::LeaveIfError( iDatabase.Exec( KCommitTransaction ) );
            iSqlTransactionOngoing = EFalse;
            }
        
        CleanupStack::PopAndDestroy( &array );
        
        //Prepare for the next round
        if ( iWmDrmRightsConfigFound && ( aTableType == EWmDrmLicenseTable ) )
        	{
        	freeSpace = iServer->FreeSpaceL( ETrue );
        	databaseSize = DataBaseSize( ETrue );
        	User::LeaveIfError( databaseSize );
        
        	//Release access to reserved space
        	iDatabase2.ReleaseReserveAccess();
        	}
    	else 
        	{
        	freeSpace = iServer->FreeSpaceL( EFalse );
        	databaseSize = DataBaseSize( EFalse );
        	User::LeaveIfError( databaseSize );
        	iDatabase.ReleaseReserveAccess();
        	}
        }		
    DropTableL( aTableType );
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::SelectNRecordsWithRowIdL
// ---------------------------------------------------------------------------
//
void CWmDrmDb::SelectNRecordsWithRowIdL(
    TWmDrmTableType aTableType,
    TInt aNumber,
    RArray<TInt64>& aArray )
    {
    LOGFN( "CWmDrmDb::SelectNRecordsWithRowIdL" );
    TPtr statementPtr( NULL, 0 );
    HBufC* statement( NULL );
    //Select right statement and append number to be the limit in statement
    
    //Prepare select statement and get rowID column index
    RSqlStatement stmt;
    CleanupClosePushL( stmt );
    
    switch ( aTableType )
        {
        case EWmDrmLicenseTable:
            statement = HBufC::NewLC( KSelectNRowIDsLicenseTableString().Length() + 
                                      KMaxTIntBufLength );
            statementPtr.Set( statement->Des() );
            statementPtr.AppendFormat( KSelectNRowIDsLicenseTableString, aNumber );
             //Prepare select statement
            if ( iWmDrmRightsConfigFound )
                {
                stmt.PrepareL( iDatabase2, statementPtr );
                }
            else
                {
                stmt.PrepareL( iDatabase, statementPtr );
                }
            break;
        case EWmDrmSecureTable:
            statement = HBufC::NewLC( KSelectNRowIDsSecureTableString().Length() + 
                                      KMaxTIntBufLength );
            statementPtr.Set( statement->Des() );
            statementPtr.AppendFormat( KSelectNRowIDsSecureTableString, aNumber );
            stmt.PrepareL( iDatabase, statementPtr );
            break;
        case EWmDrmSyncTable:
            statement = HBufC::NewLC( KSelectNRowIDsSyncTableString().Length() + 
                                      KMaxTIntBufLength );
            statementPtr.Set( statement->Des() );
            statementPtr.AppendFormat( KSelectNRowIDsSyncTableString, aNumber );
            stmt.PrepareL( iDatabase, statementPtr );
            break;
        case EWmDrmMeteringTable:
            statement = HBufC::NewLC( KSelectNRowIDsMeteringTableString().Length() + 
                                      KMaxTIntBufLength );
            statementPtr.Set( statement->Des() );
            statementPtr.AppendFormat( KSelectNRowIDsMeteringTableString, aNumber );
            stmt.PrepareL( iDatabase, statementPtr );
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }
    
    //Get rowID column index
    TInt rowIDColumnIndex( stmt.ColumnIndex( KRowIDColumn ) );
    User::LeaveIfError( rowIDColumnIndex );

    //Append all selected rowIDs to array
    TInt err( KErrNone );
    while ( ( err = stmt.Next() ) == KSqlAtRow )
        {
        aArray.AppendL( stmt.ColumnInt64( rowIDColumnIndex ) );
        }

    LOG2( "err: %d", err );
    if ( err != KSqlAtEnd )
        {
        User::Leave( err );
        }
    CleanupStack::PopAndDestroy( 2, statement ); //stmt, statement
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::DeleteRecordsWithRowIdsL
// ---------------------------------------------------------------------------
//
void CWmDrmDb::DeleteRecordsWithRowIdsL(   
    TWmDrmTableType aTableType,
    RArray<TInt64>& aArray )
    {
    LOGFN( "CWmDrmDb::DeleteRecordsWithRowIdsL" );
    //Select right statement
    TPtrC statementString( NULL, 0 );
    RSqlStatement stmt;
    CleanupClosePushL( stmt );
    
    //Prepare delete statement and get rowID parameter index
    
    switch ( aTableType )
        {
        case EWmDrmLicenseTable:
            statementString.Set( KDeleteWithRowIDLicenseTableString().Ptr(), 
                                 KDeleteWithRowIDLicenseTableString().Length() );
            //Prepare delete statement
            if ( iWmDrmRightsConfigFound )
                {
                stmt.PrepareL( iDatabase2, statementString );
                }
            else
                {
                stmt.PrepareL( iDatabase, statementString );
                }                    
            break;
        case EWmDrmSecureTable:
            statementString.Set( KDeleteWithRowIDSecureTableString().Ptr(), 
                                 KDeleteWithRowIDSecureTableString().Length() );
            stmt.PrepareL( iDatabase, statementString );                     
            break;
        case EWmDrmSyncTable:
            statementString.Set( KDeleteWithRowIDSyncTableString().Ptr(), 
                                 KDeleteWithRowIDSyncTableString().Length() );
            //Prepare delete statement
            stmt.PrepareL( iDatabase, statementString );                      
            break;
        case EWmDrmMeteringTable:
            statementString.Set( KDeleteWithRowIDMeteringTableString().Ptr(), 
                                 KDeleteWithRowIDMeteringTableString().Length() );
            stmt.PrepareL( iDatabase, statementString );                     
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }
    
    // Get rowID parameter index
    TInt rowIDIndex( stmt.ParameterIndex( KRowIDParameter ) );
    User::LeaveIfError( rowIDIndex );
    
    //Execute delete statement with every rowID value from the array
    LOG2( "aArray.Count(): %d", aArray.Count() );
    for ( TInt i( 0 ); i < aArray.Count(); ++i )
        {
        User::LeaveIfError( stmt.BindInt64( rowIDIndex, aArray[i] ) );
        User::LeaveIfError( stmt.Exec() );
        User::LeaveIfError( stmt.Reset() );
        }
    CleanupStack::PopAndDestroy( &stmt );
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::DropTableL
// ---------------------------------------------------------------------------
//
void CWmDrmDb::DropTableL( TWmDrmTableType aTableType )
    {
    LOGFN( "CWmDrmDb::DropTableL" );
    switch ( aTableType )
        {
        case EWmDrmLicenseTable:
            if ( iWmDrmRightsConfigFound )
                {
                User::LeaveIfError( iDatabase2.Exec( KDeleteLicenseTableString ) );
                }
            else 
                {
                User::LeaveIfError( iDatabase.Exec( KDeleteLicenseTableString ) );
                }
            break;
        case EWmDrmSecureTable:
            User::LeaveIfError( iDatabase.Exec( KDeleteSecureTableString ) );
            break;
        case EWmDrmSyncTable:
        	User::LeaveIfError( iDatabase.Exec( KDeleteSyncTableString ) );
            break;
        case EWmDrmMeteringTable:
            User::LeaveIfError( iDatabase.Exec( KDeleteMeteringTableString ) );
            break;
        default:
            LOG1( "CWmDrmDb::DropTableL error" );
            User::Leave( KErrArgument );
            break; 
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::TableTypeL
// ---------------------------------------------------------------------------
//
CWmDrmDb::TWmDrmTableType CWmDrmDb::TableTypeL( const TDesC8& aNamespace )
    {
    LOGFN( "CWmDrmDb::TableTypeL" );
    if ( aNamespace.CompareC( KLicenseStore  ) == 0 )
        {
        return EWmDrmLicenseTable;
        }
    else if ( aNamespace.CompareC( KSecureStore  ) == 0 )
        {
        return EWmDrmSecureTable;
        }
    else if ( aNamespace.CompareC( KSyncStore  ) == 0 )
        {
        return EWmDrmSyncTable;
        }
    else if ( aNamespace.CompareC( KMeteringStore  ) == 0 )
        {
        return EWmDrmMeteringTable;
        }    
    else
        {
        User::Leave( KErrNotFound );
        return EWmDrmLicenseTable;
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::OpenDatabaseL
// ---------------------------------------------------------------------------
//
void CWmDrmDb::OpenDatabaseL( TFileName& aFileNamePath, RSqlDatabase&
    aSqlDatabase, TBool aConfiguredDrive )
    {
    TInt error( aSqlDatabase.Open( aFileNamePath ) );
    LOG2( "CWmDrmDb: Opening database, status: %d", error );
    
    if ( error == KErrNotFound )
        {
        // database does not exist, create one
        CreateDatabaseL( aFileNamePath, aSqlDatabase, aConfiguredDrive );
		}
    else if ( error == KSqlErrCorrupt || error == KSqlErrNotDb )
        {
        RemoveStoreL( KHds );
        }
    else
        {
        // Error while opening database
        User::LeaveIfError( error );
        }
    User::LeaveIfError( aSqlDatabase.ReserveDriveSpace( 10 * KLicenseSize ) );
    }

// ---------------------------------------------------------------------------
// CWmDrmDb::CheckDatabaseCommitL
// ---------------------------------------------------------------------------
//
void CWmDrmDb::CheckDatabaseCommitL( TBool aEnforcedCommit )
    {
    LOGFN( "CWmDrmDb::CheckCommitDatabaseL()" );
    
    // Restart the timer for committing the changes to the database
    if ( !aEnforcedCommit )
        {
        if ( IsActive() )
            {
            Cancel();
            }
        Activate();
        }
        
    // Check if the amount of cached database operations reaches the upper 
    // limit or if we need to have a forced commit.
    if ( ( iAmountOperationsWithoutCommit >= KMaxOperationsUntilCommit ) || 
        aEnforcedCommit )
        {
        if ( iWmDrmRightsConfigFound && iSqlTransactionOngoing2 )
            {
            User::LeaveIfError( iDatabase2.Exec( KCommitTransaction ) );
            iSqlTransactionOngoing2 = EFalse;
            }
        
        if ( iSqlTransactionOngoing )
            {
            User::LeaveIfError( iDatabase.Exec( KCommitTransaction ) );
            iSqlTransactionOngoing = EFalse;
            }
            
        // Reset the counter
        iAmountOperationsWithoutCommit = 0;        
        }
    
    // In case of non-forced commit, execute new BEGIN statement for caching
    // future SQL statements.
    if ( ( iAmountOperationsWithoutCommit == 0 ) && !aEnforcedCommit )
        {
        if ( iWmDrmRightsConfigFound )
            {
            User::LeaveIfError( iDatabase2.Exec( KBeginTransaction ) );
            iSqlTransactionOngoing2 = ETrue;
            }
        User::LeaveIfError( iDatabase.Exec( KBeginTransaction ) );
        iSqlTransactionOngoing = ETrue;
        }
    }

