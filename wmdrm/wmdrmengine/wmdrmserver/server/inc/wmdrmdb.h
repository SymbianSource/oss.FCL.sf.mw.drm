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
* Description:  WMDRM database access interface
*
*/


#ifndef C_WMDRMDB_H
#define C_WMDRMDB_H

#include <sqldb.h>
#include "wmdrmserver.h"
/**
 *  Interface class to wmdrm database
 *
 *  @since S60 v3.2
 */
class CWmDrmDb : public CTimer
    {
public:

    /** table type */
    enum TWmDrmTableType
        {
        EWmDrmLicenseTable,
        EWmDrmSecureTable,
        EWmDrmSyncTable,
        EWmDrmMeteringTable
        };
        
    static CWmDrmDb* NewL( CWmDrmServer* aServer );
    static CWmDrmDb* NewLC( CWmDrmServer* aServer );

    virtual ~CWmDrmDb();

    void InitStoreL( 
        const TDesC8& aStore,
        TBool aCreateIfMissing );
    
    void InitNameSpaceL( 
        const TDesC8& aStore,
        const TDesC8& aNamespace,
        TBool aCreateIfMissing );

    void RemoveStoreL( const TDesC8& aStore );

    void RemoveNameSpaceL( 
        const TDesC8& aStore,
        const TDesC8& aNamespace );
    
    void DeleteLicenseStoreL();

    void CreateRecordL(  
        const TDesC8& aStore,
        const TDesC8& aNamespace,
        const TDesC8& aHashKey,
        const TDesC8& aUniqueKey,
        const TInt&   aSize );

    void ReadRecordL( 
        const TDesC8& aStore,
        const TDesC8& aNamespace,
        const TDesC8& aHashKey,
        const TDesC8& aUniqueKey );

    void GetDataSizeL( TInt& aDataSize );

    void DeleteData();

    void DeleteRecordL(  
        const TDesC8& aStore,
        const TDesC8& aNamespace,
        const TDesC8& aHashKey,
        const TDesC8& aUniqueKey );

    void ReadDataL( TDes8& aData );

    void WriteDataL(  
        const TDesC8& aStore,
        const TDesC8& aNamespace,
        const TDesC8& aHashKey,
        const TDesC8& aUniqueKey, 
        TDesC8& aData );
    
    void EnumerateDataL(  
        const TDesC8& aStore,
        const TDesC8& aNamespace,
        const TDesC8& aHashKey,
        RPointerArray<HBufC8>& aUniqueKeyEntries );
    
    void EnumerateNameSpaceStartL(  
        const TDesC8& aStore,
        const TDesC8& aNamespace );
    
    void EnumerateNameSpaceNextL(  
        TBuf8<KWmDrmIdSize>& aHashKey,
        TBuf8<KWmDrmIdSize>& aUniqueKey );

    TInt DataBaseSize( TBool aConfiguredDrive );

    // Checks how many SQL statements are prepared (buffered) currently and if either the maximum 
    // amount of buffered statements is exceeded or an enforced commit is needed, COMMITs the 
    // statements. The method also starts a new BEGIN statement after the commit operation to 
    // start a new buffering round of SQL statements. This method also controls a timer for 
    // triggering enforced commit after a predefined idle period without any prepared SQL statements. 
    void CheckDatabaseCommitL( TBool aEnforcedCommit );

protected: // from base class CActive
    void RunL();
    TInt RunError( TInt aError );

private: // from base class CActive

private:
    CWmDrmDb( CWmDrmServer* aServer );
    void ConstructL();
    
    void CreateDatabaseL( TFileName& aFileNamePath, RSqlDatabase& aDatabase, TBool aConfiguredDrive );
    void CreateTableL( TWmDrmTableType aTableType, RSqlDatabase& aDatabase );
    void DeleteTableL( TWmDrmTableType aTableType );
    void SelectNRecordsWithRowIdL( TWmDrmTableType aTableType, TInt aNumber, RArray<TInt64>& aArray );
    void DeleteRecordsWithRowIdsL( TWmDrmTableType aTableType, RArray<TInt64>& aArray );
    void DropTableL( TWmDrmTableType aTableType );
    TWmDrmTableType TableTypeL( const TDesC8& aNamespace );
    void OpenDatabaseL( TFileName& aFileNamePath, RSqlDatabase& aDatabase, TBool aConfiguredDrive );
    
    void ConvertOldLicenseStoreL();
    

    void Activate();

private: // data

    // Not owned
    CWmDrmServer* iServer;
    
    // Located in the system drive
    RSqlDatabase    iDatabase;
    // Located in the configured drive (internal mass drive)
    RSqlDatabase    iDatabase2;

    HBufC8* iData;
    
    RSqlStatement iEnumerateNamespaceStmt;
    TInt iEnumerateNamespaceHashKeyColumnIndex;
    TInt iEnumerateNamespaceUniqueKeyColumnIndex;
    TBool iEnumerateNamespaceStarted;
    
    // Path in the system drive for the data base
    TFileName iDatabasePath;
    // Path in the internal mass drive (configured drive)
    TFileName iDatabasePath2;
    
    // Whether WM DRM rights storing location is found to be
    // configured to the internal mass drive or not
    TBool iWmDrmRightsConfigFound;
    
    // Amount of cached database import, update or delete operations that will be committed
    // to the database when certain conditions are met.
    TInt iAmountOperationsWithoutCommit;
    
    // When the flag is on, an SQL BEGIN statement has been issued, but not committed 
    // with COMMIT statement. The first one is for the database on the system drive and the 
    // second one is for the database on the configured drive. 
    TBool iSqlTransactionOngoing;
    TBool iSqlTransactionOngoing2;
    
    };

#endif // ? C_CLASSNAME_H
