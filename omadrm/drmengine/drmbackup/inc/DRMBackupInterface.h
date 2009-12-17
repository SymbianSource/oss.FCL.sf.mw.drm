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


#ifndef DRMBACKUPINTERFACE_H
#define DRMBACKUPINTERFACE_H

// INCLUDES

// CONSTANTS
const TInt KDRMNormalBackup    = 0;
const TInt KDRMUDTBackup       = 1;

// Backup Directory
_LIT(KBackupDirectory, "C:\\private\\101F51F2\\backup\\");

// Filename of the rights database
_LIT( KRightsDbBackupFile, "rightsdb.bak" );

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class RFile;
class TDesC8;
// CLASS DECLARATION

/**
*  MDRMBackupInterface is the interface required to be
*  implemented by components in DRM wanting to use
*  active backup
*
*  @lib rightsserver.exe
*  @since 3.0
*/

class MDRMBackupInterface
    {
    public:

    /**
    * BackupContentToFileL
    *
    * Backups content to file in encrypted format
    *
    * @since    3.0
    * @param    aBackupFile : File handle to the backup file
    * @param    aEncryptionKey : the key to use to encrypt the backup file
    * @leave    Symbian OS error code if an error occurs 
    */
    virtual void BackupContentToFileL( RFile& aBackupFile,
                                       const TDesC8& aEncryptionKey, 
                                       const TInt aMode ) = 0;

    /**
    * RestoreContentFromFileL
    *
    * Restores the content file from encrypted format
    *
    * @since    3.0
    * @param    aBackupFile : File handle to backup file
    * @param    aEncryptionKey : the key to use to encrypt the backup file
    * @leave    Symbian OS error code if an error occurs 
    */                           
    virtual void RestoreContentFromFileL( RFile& aBackupFile,
                                          const TDesC8& aEncryptionKey,
                                          const TInt aMode ) = 0;
        
    };
#endif      // DRMBACKUPINTERFACE_H   
            
// End of File
