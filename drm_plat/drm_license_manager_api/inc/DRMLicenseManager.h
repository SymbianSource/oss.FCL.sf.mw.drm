/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of the license manager functionality used in the
*                install process
*
*/


#ifndef DRMLICENSEMANAGER_H
#define DRMLICENSEMANAGER_H

// INCLUDES

#include <f32file.h>

// DATA TYPES

struct TDRMDataFile
    {
    TFileName iSourceName;
    TFileName iTargetName;
    TBuf8<128> iMimeType;
    };

// FORWARD DECLARATIONS

class CZipFile;
class CZipFileMember;
class RZipFileMemberReaderStream;
class CDRMRights;

// CLASS DECLARATION

/**
*  Processes protected installation packages for use by the license checker.
*
*  @lib DRMLicenseManager.lib
*  @since Series 60 2.5
*/
class CDRMLicenseManager: public CBase
    {
    public:  // Local enumerations and types

        enum
            {
            ENoRights = -100000,
            ERightsExpired = -100001,
            EPIPInvalid = -100002,
            ESISNotFound = -100003
            };

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CDRMLicenseManager* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CDRMLicenseManager();

    public: // New functions
        
        /**
        * This function performs the actual processing of a protected
        * installation package. It identifies the data files to be protected
        * by reading the definition file. Each identified data file in the PIP
        * will be encrypted and stored in the location indicated by the
        * definition file. The definition file can refer to these locations
        * relative to the installation destination, which can be given as a
        * separate parameter.
        *
        * @since Series 60 2.6
        * @param aZIPFile: file name of the PIP file to be installed
        * @param aDestination: target directory where the contained SIS file was
        *        installed
        * @return ENoRights, ERightsExpired if the PIP file could not be opened
        *         because of insufficient rights, EPIPInvalid if the file is
        *         not a PIP file, KErrNone otherwise.
        */
        IMPORT_C TInt ProcessL(
            const TDesC& aZIPFile,
            const TDesC& aDestination);

        /**
        * This is the file handle based equivalent of the ProcessL function
        *
        * @since Series 60 3.0
        * @param aZIPFile: file handle of the PIP file to be installed
        * @param aDestination: target directory where the contained SIS file was
        *        installed
        * @return ENoRights, ERightsExpired if the PIP file could not be opened
        *         because of insufficient rights, EPIPInvalid if the file is
        *         not a PIP file, KErrNone otherwise.
        */
        IMPORT_C TInt ProcessL(
            RFile& aZIPFile,
            const TDesC& aDestination);

        /**
        * Extracts the SIS file from within the PIP file and stores it in a 
        * defined location.
        *
        * @since Series 60 2.6
        * @param aZIPFile: file name of the PIP which contains the SIS file to
        *        be extracted.
        * @param aDestination: target directory where the contained SIS file
        *        should be stored.
        * @return ENoRights, ERightsExpired if the PIP file could not be opened
        *         because of insufficient rights, EPIPInvalid if the file is
        *         not a PIP file, ESISNotFound if the PIP file did not contain
        *         a SIS file, KErrNone otherwise.
        */
        IMPORT_C TInt ExtractSISFileL(
            const TDesC& aZIPFile,
            const TDesC& aDestination);

        /**
        * This is the file handle based equivalent of the ExtractSISFileL
        * function.
        *
        * @since Series 60 3.0
        * @param aZIPFile: file name of the PIP which contains the SIS file to
        *        be extracted.
        * @param aDestination: target directory where the contained SIS file
        *        should be stored.
        * @return ENoRights, ERightsExpired if the PIP file could not be opened
        *         because of insufficient rights, EPIPInvalid if the file is
        *         not a PIP file, ESISNotFound if the PIP file did not contain
        *         a SIS file, KErrNone otherwise.
        */
        IMPORT_C TInt ExtractSISFileL(
            RFile& aZIPFile,
            const TDesC& aDestination);

        /**
        * Returns the SIS member contained in a PIP file.
        * 
        * @since Series 60 2.5
        * @return Pointer to the SIS member, has to be released by the caller
        */
        IMPORT_C CZipFileMember* GetSISMemberL(void);

    protected:  // New functions
        
        /**
        * Writes the SIS member contents to a file.
        *
        * @since Series 60 2.5
        * @param aSisMember: SIS member to be written
        * @param aDestination: target directory for the SIS member
        */
        void WriteSISMemberL(
            CZipFileMember* aSisMember,
            const TDesC& aDestination);

        /**
        * Reads the definition file from a PIP file
        *
        * @since Series 60 2.5
        * @return ENoRights, ERightsExpired if the PIP file could not be opened
        *         because of insufficient rights, EPIPInvalid if the file is
        *         not a PIP file, KErrNone otherwise.
        */
        TInt ReadDefinitionFileL(void);

        /**
        * Reads the description line for one file
        *
        * @since Series 60 2.5
        * @param aStream: Pointer to the definition file member stream
        * @return KErrNone or EPIPInvalid
        */
        TInt ReadFileDescription(
            RZipFileMemberReaderStream* aStream);

        /**
        * Reads one line from the definition file.
        *
        * @since Series 60 2.5
        * @param aStream: Pointer to the definition file member stream
        * @param aLine: Descriptor for the line's content
        */
        void ReadLine(
            RZipFileMemberReaderStream* aStream,
            TDes8& aLine);

        /**
        * Process all data files by encrypting them and writing them to their
        * target locations.
        *
        * @since Series 60 2.5
        * @param aDestination: Directory where the SIS file was installed to.
        * @return KErrNone or EPIPInvalid
        */
        TInt ProcessDataFilesL(
            const TDesC& aDestination);

        /**
        * Encrypts a single data file.
        *
        * @since Series 60 2.5
        * @param aDataFile: Data file parameters
        * @param aDestination: Directory where the SIS file was installed to.
        * @param aZipMember: Pointer to the data member to be encrypted.
        * @return ?description
        */
        void EncryptDataFileL(
            TDRMDataFile* aDataFile,
            const TDesC& aDestination,
            CZipFileMember* aZipMember);

    private:

        /**
        * C++ default constructor.
        */
        CDRMLicenseManager();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    protected:  // Data
        // Pointer to the currently opened PIP file
        CZipFile* iZipFile;

        // File server session
        RFs* iFs;

        // Array of data files as read from the definition file.
        RPointerArray<TDRMDataFile> iDataFiles;

        // Rights object used for the PIP file
        CDRMRights* iRights;
        
        HBufC8* iRightsIssuer;
    };

#endif      // DRMLICENSEMANAGER_H   
            
// End of File
