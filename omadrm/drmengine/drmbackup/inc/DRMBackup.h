/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for the DRMBackup service
*
*/


#ifndef DRMBACKUP_H
#define DRMBACKUP_H

// INCLUDES

#include <e32base.h>	// CBase
#include <connect/abclient.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CDRMContextDB;
class CDRMRightsDB;

// CLASS DECLARATION

/**
*   CDRMBackup implements the Active Backup required by
*   - DRM Rights Database
*   - ROAP  
*
*
*
*  @lib rightsserver.exe
*  @since 3.0
*/

class CDRMBackup : public CBase, 
                   public conn::MActiveBackupDataClient
    {
    public:

        /**
        * NewLC
        *
        * Creates an instance of the CDRMBackup class and returns a pointer to it
        * The function leaves the object into the cleanup stack
        *
        * @since    3.0
        * @param    aRightsDatabase : CDRMRightsDB object for backing it up
        * @param    aContextDatabase : CDRMContextDB object for backing it up
        * @param    aFileServer : file server instance
        * @return   Functional CDRMClock object, Function leaves if an error occurs.
        */
        static CDRMBackup* NewLC( CDRMRightsDB* aRightsDatabase,
                                  RFs& aFileServer );

        /**
        * NewL
        *
        * Creates an instance of the CDRMBackup class and returns a pointer to it
        *
        * @since    3.0
        * @param    aRightsDatabase : CDRMRightsDB object for backing it up
        * @param    aContextDatabase : CDRMContextDB object for backing it up
        * @param    aFileServer : file server instance        
        * @return   Functional CDRMClock object, Function leaves if an error occurs.
        */
        static CDRMBackup* NewL( CDRMRightsDB* aDRMRightsDB,
                                 RFs& aFileServer );
  
        /**
        * Destructor
        */
        virtual ~CDRMBackup();



    public: // Inherited from MActiveBackupDataClient
    


        /**
        This method informs the active backup data client that all snapshots have
        been supplied. If the client has not received a snapshot then it should
        perform a base backup.
        */
        virtual void AllSnapshotsSuppliedL();
        
        /**
        This method receives all or part of a snapshot of data to allow calculation of an
        incremental backup.  The snapshot is one that was previously supplied by the data
        owner.  The snapshot data should be read from the location supplied.
        The snapshot data may be larger than the location supplied in which case the routine will
        be called repeatedly until all data has been supplied.

        Snapshot data will also be supplied as part of a restore operation.
        
        @param aDrive the drive being backed up
        @param aBuffer a pointer to the base of the location from whence data can be copied.
        @param aLastSection ETrue if this is the last section of snapshot data, else EFalse.
        @leave KErrNotSupported if the data owner does not support incremental backups.
        */
        virtual void ReceiveSnapshotDataL(TDriveNumber aDrive, TDesC8& aBuffer, TBool aLastSection);

        /**
        This method returns the expected size of backup data that will be supplied.   If an
        incremental backup is underway then this method will not be called until after
        ReceiveSnapshotDataL().  The size data will be used for the purpose of tracking progess
        during a backup.  If it is inaccurate then the user may see irregular progress but the
        actual backup data will not be affected so it is acceptable to return an estimated
        value.

        @param aDrive the drive being backed up.
        @return the size of the data that will be returned
        */
        virtual TUint GetExpectedDataSize(TDriveNumber aDrive);

        /**
        This method returns a snapshot of data to accompany a backup.  The snapshot is expected
        to contain details on files / data being backed up.  The format of the snapshot is only
        meaningful to the data owner.  The snapshot will be supplied if the data owner is asked
        for an incremental backup and for a restore operation.  The snapshot data should be
        copied to the location supplied.
        The snapshot data may be larger than the location supplied in which case the routine will
        be called repeatedly until all data has been retrieved.
        
        @param aDrive the drive being backed up
        @param aBuffer a pointer to the base of the location where data can be copied.
        @param aFinished on return ETrue if all data has been returned for this drive, else EFalse.
        @leave KErrNotSupported if the data owner does not support  incremental backups.
        */
        virtual void GetSnapshotDataL(TDriveNumber aDrive, TPtr8& aBuffer, TBool& aFinished);

        /**
        This method prepares the implementor to return backup data.  It will be followed by a
        sequence of calls to request the actual data.
        
        @param aDrive the drive being backed up.
        */
        virtual void InitialiseGetBackupDataL(TDriveNumber aDrive);

        /**
        This method requests a section of backup data.  InitialiseGetBackupDataL() will have been
        called prevously to specify the drive concerned.  The data returned may be base or
        incremental depending on the type of backup and the capability of the data owner.

        @param aBuffer a pointer to the base of the location where data can be copied.
        @param aFinished on return ETrue if all data has been returned for this drive, else EFalse.
        */
        virtual void GetBackupDataSectionL(TPtr8& aBuffer, TBool& aFinished);
        
        ///// Restore Methods /////
        
        /**
        This method prepares the implementor to receive base restore data for a drive.
        It will be followed by a sequence of calls to supply the actual data.
        
        @param aDrive the drive being restored.
        */
        virtual void InitialiseRestoreBaseDataL(TDriveNumber aDrive);

        /**
        This method receives a section of base restore data.
        InitialiseRestoreBaseDataL() will have been called prevously to specify the drive concerned.
        
        @param aBuffer a pointer to the base of the location whence data can be read.
        @param aFinished ETrue if all data has been returned for this drive, else EFalse.
        */
        virtual void RestoreBaseDataSectionL(TDesC8& aBuffer, TBool aFinished);

        /**
        This method prepares the implementor to receive incremental restore data for a drive.
        It will be followed by a sequence of calls to supply the actual data.  If multiple
        increments are supplied then this methid will be called before each increment.
        
        @param aDrive the drive being restored.
        */
        virtual void InitialiseRestoreIncrementDataL(TDriveNumber aDrive);

        /**
        This method receives a section of increment restore data.
        InitialiseRestoreIncrementDataL() will have been called prevously to specify the 
        drive concerned.
        
        @param aBuffer a pointer to the base of the location whence data can be read.
        @param aFinished ETrue if all data has been returned for this increment, else EFalse.
        */
        virtual void RestoreIncrementDataSectionL(TDesC8& aBuffer, TBool aFinished);

        /**
        This method is called when all data to be restored has been supplied.
        
        @param aDrive the drive being restored.
        */
        virtual void RestoreComplete(TDriveNumber aDrive);
        
        ///// General Methods /////
        
        /**
        This method is called if copying of data is terminated prematurely to allow the 
        implementor to tidy up.  The same method applies to all types of data and to backup
        and restore
        */
        virtual void TerminateMultiStageOperation();
        
        ///// Test Methods /////
        /**
        Gets a 32-bit checksum for its private data.
        This routine is for test purposes.  It must be implemented but an invariant checksum 
        value can be provided.  Some tests may cause checksum values to be compared.
        
        @param aDrive the drive containing data being checksummed
        @return the 32-bit checksum
        */
        virtual TUint GetDataChecksum(TDriveNumber aDrive);
    
        /**
        * PerformCleanup
        *
        * Delete the backup files and free other relevant resources
        *
        * @since  3.0
        * @return none
        */	 
        void PerformCleanup( TBool aUdt = EFalse );
    
    
    private:
        /**
        * Default Constructor - First phase
        */
        CDRMBackup( CDRMRightsDB* aRightsDatabase,
                    RFs& aFileServer );

        /**
        * ConstructL
        *
        * Second phase constructor
        *
        * @since  3.0
        * @return Leaves if an error occurs
        */	   
        void ConstructL();

        /**
        * CreateRightsBackupFile
        *
        * Create the encrypted rights db backup file
        *
        * @since  3.0
        * @return return a Symbian OS error code if an error occurs
        */	 
        void CreateRightsBackupFileL();
        
        /**
        * CreateContextBackupFile
        *
        * Create the encrypted context db backup file
        *
        * @since  3.0
        * @return returns a Symbian OS error code if an error occurs
        */	 
        void CreateContextBackupFileL();

        /**
        * PerformRightsMerge
        *
        * Merge in the encrypted rights db backup file
        *
        * @since  3.0
        * @return return a Symbian OS error code if an error occurs
        */	 
        void PerformRightsMergeL();
        
        /**
        * PerformContextMerge();
        *
        * Merge in the encrypted context db backup file
        *
        * @since  3.0
        * @return returns a Symbian OS error code if an error occurs
        */	 
        void PerformContextMergeL();

    private: // Variables
        
        // File server session
        RFs& iFileServer;
        
        // Rights database instance
        CDRMRightsDB* iDRMRightsDB;     
        
        // file handle of the file used for rights db backup
        RFile iRightsBackupFile;
        
        // name of the file used for rights db backup
        TFileName iRightsFileName;
        
        // size of the file
        TInt iRightsFileSize;
        
        // file handle of the file used for context db backup
        RFile iContextBackupFile;

        // name of the file used for context db backup
        TFileName iContextFileName;
                
        // size of the file
        TInt iContextFileSize;
        
        // the position in the file
        TInt iPosition;
        
        // buffer to read the size of data
        TBuf8<4> iSizeBuffer;
        
        // how much data has been read to the iSizeBuffer
        TInt8 iReadData;
        
        // internal status of the object
        TInt iStatus;
        
    };
#endif      // DRMBACKUP_H   
            
// End of File
