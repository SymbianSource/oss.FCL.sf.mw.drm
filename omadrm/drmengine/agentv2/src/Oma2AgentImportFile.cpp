/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <s32mem.h>
#include <f32file.h>
#include <caf/caf.h>
#include <caf/cafplatform.h>
#include <caf/caferr.h>
#include <caf/supplieroutputfile.h>
#include "Oma2Agent.h"
#include "Oma2AgentImportFile.h"
#include "DcfCommon.h"
#include "DRMRights.h"
#include "DRMMessageParser.h"
#include "Oma1DcfCreator.h"
#include "DrmRightsParser.h"
#include "DRMProtectedRoParser.h"

using namespace ContentAccess;

// LOCAL FUNCTION PROTOTYPES
template<class S>
LOCAL_C void PointerArrayResetDestroyAndClose(TAny* aPtr);

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
template<class S>
void PointerArrayResetDestroyAndClose(TAny* aPtr)
    {
    (reinterpret_cast<RPointerArray<S>*>(aPtr))->ResetAndDestroy();
    (reinterpret_cast<RPointerArray<S>*>(aPtr))->Close();
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// COma2AgentImportFile::
//
// -----------------------------------------------------------------------------
//
COma2AgentImportFile* COma2AgentImportFile::NewL(
    const TDesC8& aMimeType,
    const CMetaDataArray& aMetaDataArray,
    const TDesC& aOutputDirectory,
    const TDesC& aSuggestedFileName)
    {
    COma2AgentImportFile* self=new(ELeave) COma2AgentImportFile(EFalse);
    CleanupStack::PushL(self);
    self->ConstructL(aMimeType, aMetaDataArray, aOutputDirectory,
        aSuggestedFileName);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// COma2AgentImportFile::
//
// -----------------------------------------------------------------------------
//
COma2AgentImportFile* COma2AgentImportFile::NewL(
    const TDesC8& aMimeType,
    const CMetaDataArray& aMetaDataArray)
    {
    COma2AgentImportFile* self=new(ELeave) COma2AgentImportFile(EFalse);
    CleanupStack::PushL(self);
    self->ConstructL(aMimeType, aMetaDataArray, KNullDesC(), KNullDesC());
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// COma2AgentImportFile::
//
// -----------------------------------------------------------------------------
//
COma2AgentImportFile::COma2AgentImportFile(
    TBool aAgentCreatesOutputFiles):
    iOutputDirectory(NULL),
    iSuggestedFileName(NULL),
    iOutputFileName(NULL),
    iLastWriteData(NULL),
    iAgentCreatesOutputFiles(aAgentCreatesOutputFiles),
    iMessageParser(NULL),
    iDcfCreator(NULL),
    iDcfRights(NULL),
    iDcfMimeType(NULL)
    {
    iImportStatus = EInProgress;
    }

// -----------------------------------------------------------------------------
// COma2AgentImportFile::
//
// -----------------------------------------------------------------------------
//
COma2AgentImportFile::~COma2AgentImportFile()
    {
    if (iFileOpen)
        {
        iFile.Close();
        iFileOpen = EFalse;
        }
    iFs.Close();
    delete iOutputDirectory;
    delete iSuggestedFileName;
    delete iOutputFileName;
    delete iLastWriteData;
    iOutputFiles.ResetAndDestroy();
    iOutputFiles.Close();
    delete iMessageParser;
    delete iDcfCreator;
    delete iDcfRights;
    delete iDcfMimeType;
    }

// -----------------------------------------------------------------------------
// COma2AgentImportFile::
//
// -----------------------------------------------------------------------------
//
void COma2AgentImportFile::ConstructL(
    const TDesC8& aMimeType,
    const CMetaDataArray& aMetaDataArray,
    const TDesC& aOutputDirectory,
    const TDesC& aSuggestedFileName)
    {
    RMemReadStream rights;

    if (aMimeType.CompareF(KOma1DrmMessageContentType) == 0 ||
        aMimeType.CompareF(KOmaImportContentType) == 0)
        {
        if (aMimeType.CompareF(KOma1DrmMessageContentType) == 0)
            {
            iMessageParser = CDRMMessageParser::NewL();
            iImportType = EOma1DrmMessage;
            }
        else
            {
            iDcfCreator = COma1DcfCreator::NewL();
            iImportType = EPlainContent;
            const TDesC8& mimeDes = aMetaDataArray.SearchL(
                KOmaImportMimeTypeField);
            if (mimeDes.Length() == 0)
                {
                User::Leave(KErrArgument);
                }
            iDcfMimeType = mimeDes.AllocL();
            const TDesC8& rightsDes = aMetaDataArray.SearchL(
                KOmaImportRightsField);
            if (rightsDes.Length() > 0)
                {
                rights.Open(rightsDes.Ptr(), rightsDes.Length());
                CleanupClosePushL(rights);
                iDcfRights = CDRMRights::NewL();
                iDcfRights->InternalizeL(rights);
                CleanupStack::PopAndDestroy(); // rights
                }
            }
        iOutputDirectory = aOutputDirectory.AllocL();
        iSuggestedFileName = aSuggestedFileName.AllocL();
        iOutputFileName = HBufC::NewL(iOutputDirectory->Des().Length() +
            iSuggestedFileName->Des().Length());
        iAgentCreatesOutputFiles = ETrue;
        }
    else if (aMimeType.CompareF(KOma1XmlRoContentType) == 0)
        {
        iImportType = EOma1XmlRo;
        }
    else if (aMimeType.CompareF(KOma1WbxmlRoContentType) == 0)
        {
        iImportType = EOma1WbxmlRo;
        }
    else if (aMimeType.CompareF(KOma2RoContentType) == 0)
        {
        iImportType = EOma2Ro;
        }
    else
        {
        User::Leave(KErrCANotSupported);
        }
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFs.ShareAuto());
    }

// -----------------------------------------------------------------------------
// COma2AgentImportFile::
//
// -----------------------------------------------------------------------------
//
TInt COma2AgentImportFile::OpenOutputFile()
    {
    TInt r = KErrNone;
    TPtr fileNamePtr = iOutputFileName->Des();

    fileNamePtr.Copy(*iOutputDirectory);
    fileNamePtr.Append(*iSuggestedFileName);

    r = iFile.Create(iFs, fileNamePtr, EFileShareAny  | EFileStream |
        EFileWrite);
    if (r == KErrNone)
        {
        iFileOpen = ETrue;
        if (iImportType == EOma1DrmMessage)
            {
            TRAP(r, iMessageParser->InitializeMessageParserL(iFile));
            }
        else
            {
            TRAP(r, iDcfCreator->EncryptInitializeL(iFile, *iDcfMimeType,
                iDcfRights));
            }
        }
    else
        {
        r = KErrCANewFileHandleRequired;
        }
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentImportFile::
//
// -----------------------------------------------------------------------------
//
void COma2AgentImportFile::WriteDataL(const TDesC8& aData)
    {
    CDrmRightsParser* oma1Parser = NULL;
    CDrmProtectedRoParser* oma2Parser = NULL;
    RPointerArray<CDRMRights> rights;
    TCleanupItem listCleanup(PointerArrayResetDestroyAndClose<CDRMRights>,
        &rights);
    TInt r;

    switch (iImportType)
        {
        case EOma1DrmMessage:
            if (!iFileOpen)
                {
                r = OpenOutputFile();
                if (r == KErrNone)
                    {
                    iMessageParser->ProcessMessageDataL(aData);
                    }
                else
                    {
                    iLastWriteData = aData.AllocL();
                    User::Leave(r);
                    }
                }
            else
                {
                iMessageParser->ProcessMessageDataL(aData);
                }
            break;
        case EPlainContent:
            if (!iFileOpen)
                {
                r = OpenOutputFile();
                if (r == KErrNone)
                    {
                    iDcfCreator->EncryptUpdateL(aData);
                    }
                else
                    {
                    iLastWriteData = aData.AllocL();
                    User::Leave(r);
                    }
                }
            else
                {
                iDcfCreator->EncryptUpdateL(aData);
                }
            break;
        case EOma1XmlRo:
        case EOma1WbxmlRo:
            oma1Parser = CDrmRightsParser::NewL();
            CleanupStack::PushL(oma1Parser);
            CleanupStack::PushL(listCleanup);
            oma1Parser->ParseAndStoreL(aData, rights);
            CleanupStack::PopAndDestroy(2); // listCleanup, oma1Parser
            break;
        case EOma2Ro:
            __UHEAP_MARK;
            oma2Parser = CDrmProtectedRoParser::NewL();
            CleanupStack::PushL(oma2Parser);
            CleanupStack::PushL(listCleanup);
            oma2Parser->ParseAndStoreL(aData, rights);
            CleanupStack::PopAndDestroy(2); // listCleanup, oma2Parser
            __UHEAP_MARKEND;
            break;
        }
    }

// -----------------------------------------------------------------------------
// COma2AgentImportFile::
//
// -----------------------------------------------------------------------------
//
TInt COma2AgentImportFile::WriteData(const TDesC8& aData)
    {
    TInt r = KErrNone;
    TRAP(r, WriteDataL(aData));
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentImportFile::
//
// -----------------------------------------------------------------------------
//
TInt COma2AgentImportFile::WriteDataComplete()
    {
    TInt r = KErrNone;
    CSupplierOutputFile *temp = NULL;
    TPtrC8 type(NULL, 0);

    if (iFileOpen)
        {
        if (iImportType == EOma1DrmMessage)
            {
            TRAP(r, iMessageParser->FinalizeMessageParserL());
            }
        else
            {
            TRAP(r, iDcfCreator->EncryptFinalizeL());
            }
        iFile.Close();
        iFileOpen = EFalse;
        }
    if( r )
        {
          return r;
      }
    if (iOutputFileName)
        {
        switch (iImportType)
            {
            case EPlainContent:
            case EOma1DrmMessage:
                type.Set(KOma1DcfContentType());
                break;
            default:
                return KErrGeneral;;
            }
        TRAP(r, temp = CSupplierOutputFile::NewL(iOutputFileName->Des(),
            EContent, type));
        if (r == KErrNone)
            {
            r = iOutputFiles.Append(temp);
            if (r != KErrNone)
                {
                delete temp;
                }
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentImportFile::
//
// -----------------------------------------------------------------------------
//
void COma2AgentImportFile::WriteData(
    const TDesC8& aData,
    TRequestStatus &aStatus)
    {
    TRequestStatus *ptr = &aStatus;
    TInt r = WriteData(aData);
    User::RequestComplete(ptr,r);
    }

// -----------------------------------------------------------------------------
// COma2AgentImportFile::
//
// -----------------------------------------------------------------------------
//
void COma2AgentImportFile::WriteDataComplete(
    TRequestStatus &aStatus)
    {
    TRequestStatus *ptr = &aStatus;
    TInt r = WriteDataComplete();
    User::RequestComplete(ptr,r);
    }

// -----------------------------------------------------------------------------
// COma2AgentImportFile::
//
// -----------------------------------------------------------------------------
//
TInt COma2AgentImportFile::OutputFileCountL() const
    {
    return iOutputFiles.Count();
    }

// -----------------------------------------------------------------------------
// COma2AgentImportFile::
//
// -----------------------------------------------------------------------------
//
CSupplierOutputFile& COma2AgentImportFile::OutputFileL(
    TInt aIndex)
    {
    return *iOutputFiles[aIndex];
    }

// -----------------------------------------------------------------------------
// COma2AgentImportFile::
//
// -----------------------------------------------------------------------------
//
TImportStatus COma2AgentImportFile::GetImportStatus() const
    {
    return iImportStatus;
    }

// -----------------------------------------------------------------------------
// COma2AgentImportFile::
//
// -----------------------------------------------------------------------------
//
TInt COma2AgentImportFile::GetSuggestedOutputFileExtension(
    TDes& aFileExtension)
    {
    aFileExtension.Copy(_L(".dcf"));
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// COma2AgentImportFile::
//
// -----------------------------------------------------------------------------
//
TInt COma2AgentImportFile::GetSuggestedOutputFileName(
    TDes& aFileName)
    {
    aFileName.Append(_L(".dcf"));
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// COma2AgentImportFile::
//
// -----------------------------------------------------------------------------
//
TInt COma2AgentImportFile::ContinueWithNewOutputFile(
    RFile& aFile,
    const TDesC& aFileName)
    {
    RFile file;
    TInt r = KErrNone;

    if (iOutputFileName != NULL)
        {
        delete iOutputFileName;
        iOutputFileName = NULL;
        }
    TRAP(r, iOutputFileName = aFileName.AllocL());
    if (r == KErrNone)
        {
        file.Duplicate(aFile);
        iFile.Attach(file);
        iFileOpen = ETrue;
        if (iImportType == EOma1DrmMessage)
            {
            TRAP(r, iMessageParser->InitializeMessageParserL(iFile));
            }
        else
            {
            TRAP(r, iDcfCreator->EncryptInitializeL(iFile, *iDcfMimeType,
                iDcfRights));
            }
        if (r == KErrNone)
            {
            r = WriteData(*iLastWriteData);
            }
        delete iLastWriteData;
        iLastWriteData = NULL;
        }
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentImportFile::
//
// -----------------------------------------------------------------------------
//
void COma2AgentImportFile::ContinueWithNewOutputFile(
    RFile& aFile,
    const TDesC& aFileName,
    TRequestStatus& aStatus)
    {
    TRequestStatus *ptr = &aStatus;
    TInt r = ContinueWithNewOutputFile(aFile, aFileName);
    User::RequestComplete(ptr,r);
    }

// -----------------------------------------------------------------------------
// COma2AgentImportFile::
//
// -----------------------------------------------------------------------------
//
void COma2AgentImportFile::NewMimePartL(
    const TDesC8& /*aMimeType*/,
    const CMetaDataArray& /*aImportMetaData*/)
    {
    User::Leave(KErrCANotSupported);
    }

// -----------------------------------------------------------------------------
// COma2AgentImportFile::
//
// -----------------------------------------------------------------------------
//
void COma2AgentImportFile::EndMimePartL()
    {
    User::Leave(KErrCANotSupported);
    }

// End of file
