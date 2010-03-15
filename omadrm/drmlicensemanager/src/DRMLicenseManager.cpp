/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the license manager functionality used in the
*                install process
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include <apmstd.h>
#include <e32math.h>
#include <f32file.h>
#include <s32buf.h>
#include <s32crypt.h>
#include <utf.h>
#include <DRMRights.h>
#include <DRMCommon.h>
#include <Oma1DcfCreator.h>
#include <sysutil.h>     // Disk space checking

#include "zipfile.h"
#include "zipfilememberiterator.h"
#include "DRMLicenseManager.h"

// LOCAL CONSTANTS AND MACROS
_LIT(KDefinitionFileName, "datafiles.def");
_LIT(KSISSuffix, ".sis");
_LIT(KSISXSuffix, ".sisx");
_LIT8(KDefaultMIMEType, "application/binary");
_LIT8( KIssuer , "Rights-Issuer" );

const TInt KSISBufferSize = 512;
const TInt KMaxTypeLength = 11 + 2; // Length of "application" + CR + LF
const TInt KMaxFileDescriptionLength = KMaxPath + KMaxPath + KMaxDataTypeLength;


// ============================ LOCAL FUNCTIONS ================================
// -----------------------------------------------------------------------------
// DriveOfPathL
// -----------------------------------------------------------------------------
LOCAL_C inline TInt DriveOfPathL(
    RFs& aFs,
    const TDesC aPathName,
    TInt aFallbackDrive = KDefaultDrive )
    {
    // find drive for destination for free space check
    TInt driveNumber( KDefaultDrive );
    TParsePtrC pptr( aPathName );
    TInt err( KErrNone );
    if ( pptr.DrivePresent() )
        {
        err = RFs::CharToDrive( pptr.Drive()[ 0 ], driveNumber );
        User::LeaveIfError( err );
        }
    else if ( aFallbackDrive != KDefaultDrive )
        {
        driveNumber = aFallbackDrive;
        }
    else
        {
        HBufC* sessionPath( HBufC::NewLC( KMaxPath ) );
        TPtr pathPtr( sessionPath->Des() );
        User::LeaveIfError( aFs.SessionPath( pathPtr ) );

        TParsePtrC sPPtr( pathPtr );
        if ( sPPtr.DrivePresent() )
            {
            err = RFs::CharToDrive( sPPtr.Drive()[ 0 ], driveNumber );
            User::LeaveIfError( err );
            }
        else
            {
            driveNumber = RFs::GetSystemDrive();
            }
        CleanupStack::PopAndDestroy( sessionPath );
        }
    return driveNumber;
    }

// -----------------------------------------------------------------------------
// CheckNeededFreeSpaceL
// -----------------------------------------------------------------------------
LOCAL_C void CheckNeededFreeSpaceL(
    RFs& aFs,
    CZipFile*& aZipFile,
    RPointerArray< TDRMDataFile >& aDataFiles,
    const TDesC& aDestination,
    TInt& aError )
    {
    static const TInt KExtraSpaceForDcf( 1024 );
    CArrayFixFlat< TInt >* arrayOfNeededSpaces( NULL );
    // Get target drive for relative files
    TInt driveForRelativePath( DriveOfPathL( aFs, aDestination ) );

    arrayOfNeededSpaces = new ( ELeave ) CArrayFixFlat< TInt >( KMaxDrives );
    CleanupStack::PushL(  arrayOfNeededSpaces );

    arrayOfNeededSpaces->SetReserveL( KMaxDrives );
    for ( TInt j( 0 ); j < KMaxDrives; ++j )
        {
        arrayOfNeededSpaces->AppendL(0);
        }

    for ( TInt i = 0; i < aDataFiles.Count() && aError == KErrNone; i++ )
        {
        TDRMDataFile* dataFile(
            static_cast< TDRMDataFile* >( aDataFiles[ i ] ) );
        CZipFileMember* member(
            aZipFile->CaseInsensitiveMemberL( dataFile->iSourceName ) );
        if ( member )
            {
            CleanupStack::PushL( member );
            TInt driveNumber( DriveOfPathL(
                    aFs, dataFile->iTargetName, driveForRelativePath ) );
            ( *arrayOfNeededSpaces )[ driveNumber ] +=
                member->UncompressedSize() + KExtraSpaceForDcf;
            CleanupStack::PopAndDestroy( member );
            }
        else
            {
            aError = CDRMLicenseManager::EPIPInvalid;
            }
        }

    for ( TInt j( 0 ); j < KMaxDrives; ++j )
        {
        TUint element( ( *arrayOfNeededSpaces )[ j ] );
        if ( element && // no need to check if no space required
            SysUtil::DiskSpaceBelowCriticalLevelL( &aFs, element, j) )
            {
            User::Leave( KErrDiskFull );
            }
        }
    CleanupStack::PopAndDestroy( arrayOfNeededSpaces );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMLicenseManager::CDRMLicenseManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMLicenseManager::CDRMLicenseManager():
    iFs(NULL), iRights(NULL), iRightsIssuer(NULL)
    {
    }

// -----------------------------------------------------------------------------
// CDRMLicenseManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDRMLicenseManager::ConstructL()
    {
    iFs = new RFs();
    User::LeaveIfNull(iFs);
    User::LeaveIfError(iFs->Connect());
    }

// -----------------------------------------------------------------------------
// CDRMLicenseManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMLicenseManager* CDRMLicenseManager::NewL()
    {
    CDRMLicenseManager* self = new( ELeave ) CDRMLicenseManager;

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
EXPORT_C CDRMLicenseManager::~CDRMLicenseManager()
    {
    iFs->Close();
    delete iFs;

    iDataFiles.ResetAndDestroy();

    if (iZipFile)
        {
        iZipFile->Close();
        delete iZipFile;
        }

    if (iRights)
        {
        delete iRights;
        }

    if (iRightsIssuer)
        {
        delete iRightsIssuer;
        }
    }

LOCAL_C TBool GetRightsIssuerL(const TFileName aFileName, HBufC8*& aRightsIssuerURL)
    {
    TInt err(KErrNotFound);

    HBufC8* name = HBufC8::NewLC(16);
    TPtr8 headerName( name->Des() );
    headerName.Copy( KIssuer );

    DRMCommon* c = DRMCommon::NewL();
    CleanupStack::PushL(c);
    err = c->GetFileHeader(aFileName, headerName, aRightsIssuerURL);
    CleanupStack::PopAndDestroy(c);
    CleanupStack::PopAndDestroy(name);

    if(err == DRMCommon::EOk && aRightsIssuerURL)
        {
        return ETrue;
        }

    return EFalse;
    }

LOCAL_C TBool GetRightsIssuerL(RFile aFileName, HBufC8*& aRightsIssuerURL)
    {
    TInt err(KErrNotFound);

    HBufC8* name = HBufC8::NewLC(16);
    TPtr8 headerName( name->Des() );
    headerName.Copy( KIssuer );

    DRMCommon* c = DRMCommon::NewL();
    CleanupStack::PushL(c);
    err = c->GetFileHeader(aFileName, headerName, aRightsIssuerURL);
    CleanupStack::PopAndDestroy(c);
    CleanupStack::PopAndDestroy(name);

    if(err == DRMCommon::EOk && aRightsIssuerURL)
        {
        return ETrue;
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CDRMLicenseManager::ProcessL
// Read the PIP definition file and encrypt each referenced data file
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMLicenseManager::ProcessL(
    const TDesC& aZIPFile,
    const TDesC& aDestination)
    {
    TInt r = KErrNone;
    DRMCommon* drm = NULL;
    RPointerArray<CDRMRights>* rights = NULL;

    // Clean up
    if (iZipFile)
        {
        iZipFile->Close();
        delete iZipFile;
        iZipFile = NULL;
        }

    if (iRights)
        {
        delete iRights;
        iRights = NULL;
        }

    if (iRightsIssuer)
        {
        delete iRightsIssuer;
        iRightsIssuer = NULL;
        }

    GetRightsIssuerL(aZIPFile, iRightsIssuer);

    iDataFiles.ResetAndDestroy();

    // Get rights object for the PIP file
    iZipFile = CZipFile::NewL(*iFs, aZIPFile);
    drm = DRMCommon::NewL();
    CleanupStack::PushL(drm);
    r = drm->GetDetailedFileRights(aZIPFile, rights);

    // Read the definition file
    if (r == KErrNone)
        {
        iRights = (*rights)[0];
        rights->Remove(0);
        rights->ResetAndDestroy();
        delete rights;
        r = ReadDefinitionFileL();
        }

    // Process data files
    if (r == KErrNone)
        {
        r = ProcessDataFilesL(aDestination);
        }

    CleanupStack::PopAndDestroy(drm);

    return r;
    }

// -----------------------------------------------------------------------------
// CDRMLicenseManager::ProcessL
// Read the PIP definition file and encrypt each referenced data file
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMLicenseManager::ProcessL(
    RFile& aZIPFile,
    const TDesC& aDestination)
    {
    TInt r = KErrNone;
    DRMCommon* drm = NULL;
    RPointerArray<CDRMRights>* rights = NULL;

    // Clean up
    if (iZipFile)
        {
        iZipFile->Close();
        delete iZipFile;
        iZipFile = NULL;
        }

    if (iRights)
        {
        delete iRights;
        iRights = NULL;
        }

    if (iRightsIssuer)
        {
        delete iRightsIssuer;
        iRightsIssuer = NULL;
        }

    GetRightsIssuerL(aZIPFile, iRightsIssuer);

    iDataFiles.ResetAndDestroy();

    // Get rights object for the PIP file
    iZipFile = CZipFile::NewL(*iFs, aZIPFile);
    drm = DRMCommon::NewL();
    CleanupStack::PushL(drm);
    r = drm->GetDetailedFileRights(aZIPFile, rights);

    // Read the definition file
    if (r == KErrNone)
        {
        iRights = (*rights)[0];
        rights->Remove(0);
        rights->ResetAndDestroy();
        delete rights;
        r = ReadDefinitionFileL();
        }

    // Process data files
    if (r == KErrNone)
        {
        r = ProcessDataFilesL(aDestination);
        }

    CleanupStack::PopAndDestroy(drm);

    return r;
    }

// -----------------------------------------------------------------------------
// CDRMLicenseManager::ExtractSISFileL
// Find the SIS member of the ZIP file and write it to the target destination.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMLicenseManager::ExtractSISFileL(
    const TDesC& aZIPFile,
    const TDesC& aDestination)
    {
    CZipFileMember* sisFile = NULL;
    TInt r = KErrNone;

    if (iZipFile)
        {
        iZipFile->Close();
        delete iZipFile;
        iZipFile = NULL;
        }

    if (iRightsIssuer)
        {
        delete iRightsIssuer;
        iRightsIssuer = NULL;
        }

    GetRightsIssuerL(aZIPFile, iRightsIssuer);

    iZipFile = CZipFile::NewL(*iFs, aZIPFile);

    __UHEAP_MARK;
    sisFile = GetSISMemberL();

    if (sisFile)
        {
        WriteSISMemberL(sisFile, aDestination);
        delete sisFile;
        }
    else
        {
        r = ESISNotFound;
        }
    __UHEAP_MARKEND;

    return r;
    }

// -----------------------------------------------------------------------------
// CDRMLicenseManager::ExtractSISFileL
// Find the SIS member of the ZIP file and write it to the target destination.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMLicenseManager::ExtractSISFileL(
    RFile& aZIPFile,
    const TDesC& aDestination)
    {
    CZipFileMember* sisFile = NULL;
    TInt r = KErrNone;

    if (iZipFile)
        {
        iZipFile->Close();
        delete iZipFile;
        iZipFile = NULL;
        }

    if (iRightsIssuer)
        {
        delete iRightsIssuer;
        iRightsIssuer = NULL;
        }

    GetRightsIssuerL(aZIPFile, iRightsIssuer);

    iZipFile = CZipFile::NewL(*iFs, aZIPFile);

    __UHEAP_MARK;
    sisFile = GetSISMemberL();

    if (sisFile)
        {
        WriteSISMemberL(sisFile, aDestination);
        delete sisFile;
        }
    else
        {
        r = ESISNotFound;
        }
    __UHEAP_MARKEND;

    return r;
    }

// -----------------------------------------------------------------------------
// CDRMLicenseManager::GetSISMemberL
// Finds the SIS member in the ZIP file by looking for the .sis file ending.
// -----------------------------------------------------------------------------
//
EXPORT_C CZipFileMember* CDRMLicenseManager::GetSISMemberL(void)
    {
    CZipFileMemberIterator* members = NULL;
    CZipFileMember* sisFile = NULL;
    CZipFileMember* member = NULL;

    members = iZipFile->GetMembersL();
    CleanupStack::PushL(members);

    member = members->NextL();

    while (member && !sisFile)
        {
        if (member->Name()->Right(4).CompareF(KSISSuffix) == 0)
            {
            sisFile = member;
            }
        else if (member->Name()->Right(5).CompareF(KSISXSuffix) == 0)
            {
            sisFile = member;
            }
        else
            {
            delete member;
            member = NULL;
            member = members->NextL();
            }
        }

    CleanupStack::PopAndDestroy(members);

    if(!sisFile)
        {
        User::Leave(KErrNotFound);
        }

    return sisFile;
    }

// -----------------------------------------------------------------------------
// CDRMLicenseManager::WriteSISMemberL
// Write a SIS member to disk.
// -----------------------------------------------------------------------------
//
void CDRMLicenseManager::WriteSISMemberL(
    CZipFileMember* aSisMember,
    const TDesC& aDestination)
    {
    RZipFileMemberReaderStream* input = NULL;
    RFileWriteStream output;
    TBuf8<KSISBufferSize> buffer;

    // Check free space and leave if not enough space available
    if ( SysUtil::DiskSpaceBelowCriticalLevelL(
            iFs,
            aSisMember->UncompressedSize(),
            DriveOfPathL( *iFs, aDestination ) ) )
        {
        User::Leave( KErrDiskFull );
        }

    iZipFile->GetInputStreamL(aSisMember, input);
    CleanupStack::PushL(input);

    iFs->SetSessionPath(aDestination);
    User::LeaveIfError(output.Replace(*iFs, *aSisMember->Name(), EFileWrite));
    output.PushL();
    do
        {
        input->Read(buffer, KSISBufferSize);
        if (buffer.Size() > 0)
            {
            output.WriteL(buffer);
            }
        }
    while (buffer.Size() > 0);
    output.Close();

    CleanupStack::PopAndDestroy(&output);
    CleanupStack::PopAndDestroy(input);
    }

// -----------------------------------------------------------------------------
// CDRMLicenseManager::ReadDefinitionFileL
// Read the PIP type and definition lines from the definition file.
// -----------------------------------------------------------------------------
//
TInt CDRMLicenseManager::ReadDefinitionFileL(void)
    {
    CZipFileMember* definitionFile = NULL;
    RZipFileMemberReaderStream* input = NULL;
    TBuf8<KMaxTypeLength> type;
    TInt r = KErrNone;

    TRAP(r, definitionFile =
        iZipFile->CaseInsensitiveMemberL(KDefinitionFileName));

    if (r == KErrNone && definitionFile)
        {
        CleanupStack::PushL(definitionFile);
        iZipFile->GetInputStreamL(definitionFile, input);
        ReadLine(input, type);
        while (r == KErrNone)
            {
            r = ReadFileDescription(input);
            }
        delete input;
        CleanupStack::PopAndDestroy(definitionFile);
        }
    else
        {
        r = EPIPInvalid;
        }

    if (r == KErrEof)
        {
        r = KErrNone;
        }

    return r;
    }

// -----------------------------------------------------------------------------
// CDRMLicenseManager::ReadFileDescription
// Read the name, type and target name of a data file.
// -----------------------------------------------------------------------------
//
TInt CDRMLicenseManager::ReadFileDescription(
    RZipFileMemberReaderStream* aStream)
    {
    TBuf8<KMaxFileDescriptionLength> description;
    TInt r = KErrNone;
    TDRMDataFile* dataFile = NULL;

    ReadLine(aStream, description);
    description.TrimAll();
    if (description.Length() > 0)
        {
        TLex8 lex(description);

        dataFile = new TDRMDataFile();
        if( !dataFile )
            {
            return KErrNoMemory;
            }

        // Get the original file name
        lex.SkipSpaceAndMark();
        lex.SkipCharacters();
        CnvUtfConverter::ConvertToUnicodeFromUtf8(
            dataFile->iSourceName, lex.MarkedToken());

        // Get the MIME type. If not present, "application/binary" is used
        // and the target file name is the source file name
        lex.SkipSpaceAndMark();
        lex.SkipCharacters();
        if (lex.TokenLength() > 0)
            {
            dataFile->iMimeType = lex.MarkedToken();

            // Get the target file name. If not present, the target file name
            // will be the original file name, stored in the install directory.
            lex.SkipSpaceAndMark();
            lex.SkipCharacters();
            if (lex.TokenLength() > 0)
                {
                CnvUtfConverter::ConvertToUnicodeFromUtf8(
                    dataFile->iTargetName, lex.MarkedToken());
                }
            else
                {
                dataFile->iTargetName = dataFile->iSourceName;
                }
            }
        else
            {
            dataFile->iMimeType = KDefaultMIMEType;
            dataFile->iTargetName = dataFile->iSourceName;
            }

        iDataFiles.Append(dataFile);
        }
    else
        {
        r = KErrEof;
        }
    return r;
    }

// -----------------------------------------------------------------------------
// CDRMLicenseManager::ReadLine
// Read one line ending with 0x0d or until the input buffer is full.
// -----------------------------------------------------------------------------
//
void CDRMLicenseManager::ReadLine(
    RZipFileMemberReaderStream* aStream,
    TDes8& aLine)
    {
    TBuf8<1> c;
    TInt n = 0;
    TBool done = EFalse;

    while (!done)
        {
        if (aLine.Length() < aLine.MaxLength())
            {
            aStream->Read(c, 1);
            if (c.Length() > 0 && c[0] != 0x0d)
                {
                aLine.Append(c);
                n++;
                if (n == aLine.MaxLength() + 1)
                    {
                    done = ETrue;
                    }
                }
            else
                {
                done = ETrue;
                }
            }
        else
            {
            done = ETrue;
            }
        }

    if (c.Length() > 0 && c[0] == 0x0d)
        {
        aStream->Read(c, 1);
        }
    }

// -----------------------------------------------------------------------------
// CDRMLicenseManager::ProcessDataFilesL
// Get all data files from the ZIP file and encrypt them.
// -----------------------------------------------------------------------------
//
TInt CDRMLicenseManager::ProcessDataFilesL(
    const TDesC& aDestination)
    {
    TDRMDataFile* dataFile( NULL );
    CZipFileMember* member( NULL );
    TInt r( KErrNone );

    // check if there is enough free space for the process
    CheckNeededFreeSpaceL( *iFs, iZipFile, iDataFiles, aDestination, r );

    for ( TInt i = 0; i < iDataFiles.Count() && r == KErrNone; i++ )
        {
        dataFile = static_cast< TDRMDataFile* >( iDataFiles[ i ] );
        member = iZipFile->CaseInsensitiveMemberL( dataFile->iSourceName );
        if ( member )
            {
            CleanupStack::PushL(member);
            EncryptDataFileL(dataFile, aDestination, member);
            CleanupStack::PopAndDestroy(member);
            }
        else
            {
            r = EPIPInvalid;
            }
        }

    return r;
    }

// -----------------------------------------------------------------------------
// CDRMLicenseManager::EncryptDataFileL
// Encrypt a single data file, reusing the outer PIP's RO. For application
// protection, the file is encrypted again.
// -----------------------------------------------------------------------------
//
void CDRMLicenseManager::EncryptDataFileL(
    TDRMDataFile* aDataFile,
    const TDesC& aDestination,
    CZipFileMember* aZipMember)
    {
    RZipFileMemberReaderStream* input = NULL;
    RFileWriteStream output;
    TBuf8<KSISBufferSize> buffer;
    COma1DcfCreator* drm = NULL;
    TFileName outputFile;
    DRMCommon* common = NULL;

    drm = COma1DcfCreator::NewL();
    CleanupStack::PushL(drm);

    iZipFile->GetInputStreamL(aZipMember, input);
    CleanupStack::PushL(input);

    // Set the path so we can use relative paths
    iFs->SetSessionPath(aDestination);

    output.PushL();
    // Create the output file stream
    User::LeaveIfError(output.Replace(*iFs, aDataFile->iTargetName,
        EFileWrite));

    // Encrypt
    drm->EncryptInitializeL(output, aDataFile->iMimeType, iRights);
    do
        {
        input->Read(buffer, KSISBufferSize);
        if (buffer.Size() > 0)
            {
            drm->EncryptUpdateL(buffer);
            }
        }
    while (buffer.Size() > 0);
    drm->EncryptFinalizeL();
    output.Close();
    CleanupStack::PopAndDestroy( &output );

    // Add the rights issuer uri to the file if it exists
    if( iRightsIssuer )
        {
        common = DRMCommon::NewL();
        CleanupStack::PushL(common);
        outputFile.Append( aDataFile->iTargetName );
        common->SetFileHeader( outputFile, KIssuer,
                               iRightsIssuer->Des());
        CleanupStack::PopAndDestroy(common);
        }
    CleanupStack::PopAndDestroy(input);
    CleanupStack::PopAndDestroy(drm);
    }


