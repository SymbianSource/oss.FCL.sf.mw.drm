/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <caf/caferr.h>
#include <caf/SupplierOutputFile.h>
#include <WmdrmAgent.h>
#include "WmDrmAgentImportfile.h"
#include "logfn.h"

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
// CWmDrmAgentImportFile::
// 
// -----------------------------------------------------------------------------
//
CWmDrmAgentImportFile* CWmDrmAgentImportFile::NewL(
    const TDesC8& aMimeType,
    const CMetaDataArray& aMetaDataArray,
    const TDesC& aOutputDirectory,
    const TDesC& aSuggestedFileName)
	{
	LOGFN( "CWmDrmAgentImportFile::NewL" );
	CWmDrmAgentImportFile* self=new(ELeave) CWmDrmAgentImportFile(EFalse);
	CleanupStack::PushL(self);
	self->ConstructL(aMimeType, aMetaDataArray, aOutputDirectory,
	    aSuggestedFileName);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CWmDrmAgentImportFile::
// 
// -----------------------------------------------------------------------------
//
CWmDrmAgentImportFile* CWmDrmAgentImportFile::NewL(
    const TDesC8& aMimeType,
    const CMetaDataArray& aMetaDataArray)
	{
	LOGFN( "CWmDrmAgentImportFile::NewL (2)" );
	CWmDrmAgentImportFile* self=new(ELeave) CWmDrmAgentImportFile(EFalse);
	CleanupStack::PushL(self);
	self->ConstructL(aMimeType, aMetaDataArray, KNullDesC(), KNullDesC());
	CleanupStack::Pop(self);
	return self;
	}
	
// -----------------------------------------------------------------------------
// CWmDrmAgentImportFile::
// 
// -----------------------------------------------------------------------------
//
CWmDrmAgentImportFile::CWmDrmAgentImportFile(
    TBool aAgentCreatesOutputFiles):
    iOutputDirectory(NULL),
    iSuggestedFileName(NULL),
    iOutputFileName(NULL),
    iLastWriteData(NULL),
    iAgentCreatesOutputFiles(aAgentCreatesOutputFiles)
	{
	LOGFN( "CWmDrmAgentImportFile::CWmDrmAgentImportFile" );
	iImportStatus = EInProgress;
	}

// -----------------------------------------------------------------------------
// CWmDrmAgentImportFile::
// 
// -----------------------------------------------------------------------------
//
CWmDrmAgentImportFile::~CWmDrmAgentImportFile()
	{
	LOGFN( "CWmDrmAgentImportFile::~CWmDrmAgentImportFile" );
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
	}
  
// -----------------------------------------------------------------------------
// CWmDrmAgentImportFile::
// 
// -----------------------------------------------------------------------------
//
void CWmDrmAgentImportFile::ConstructL(
    const TDesC8& aMimeType,
    const CMetaDataArray& /*aMetaDataArray*/,
    const TDesC& aOutputDirectory,
    const TDesC& aSuggestedFileName)
	{
	LOGFN( "CWmDrmAgentImportFile::ConstructL" );

    iMimeType = aMimeType.AllocL();
	iOutputDirectory = aOutputDirectory.AllocL();
	iSuggestedFileName = aSuggestedFileName.AllocL();
    iOutputFileName = HBufC::NewL(iOutputDirectory->Des().Length() +
                                  iSuggestedFileName->Des().Length());
	iAgentCreatesOutputFiles = ETrue;

	User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFs.ShareAuto());
	}

// -----------------------------------------------------------------------------
// CWmDrmAgentImportFile::
// 
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentImportFile::OpenOutputFile()
	{
	TInt r = KErrNone;
	TPtr fileNamePtr = iOutputFileName->Des();
	
	fileNamePtr.Copy(*iOutputDirectory);
	fileNamePtr.Append(*iSuggestedFileName);

	r = iFile.Create(iFs, fileNamePtr, EFileShareReadersOrWriters  | EFileStream |
	    EFileWrite);
	if (r == KErrNone)
		{
		iFileOpen = ETrue;
		}
    else
        {
        r = KErrCANewFileHandleRequired;
        }
    return r;
	}

// -----------------------------------------------------------------------------
// CWmDrmAgentImportFile::
// 
// -----------------------------------------------------------------------------
//
void CWmDrmAgentImportFile::WriteDataL( const TDesC8& aData )
    {
    LOGFN( "CWmDrmAgentImportFile::WriteDataL" );
    TInt err = KErrNone;
    
    if( !iFileOpen ) 
        {
        err = OpenOutputFile();
        if( err )
            {
            User::Leave(KErrCANewFileHandleRequired);
            }
        }
    User::LeaveIfError( iFile.Write( aData ) );        
    }
    
// -----------------------------------------------------------------------------
// CWmDrmAgentImportFile::
// 
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentImportFile::WriteData(const TDesC8& aData)
	{
	LOGFN( "CWmDrmAgentImportFile::WriteData" );
	TInt r = KErrNone;
	TRAP(r, WriteDataL(aData));
	return r;
	}

// -----------------------------------------------------------------------------
// CWmDrmAgentImportFile::
// 
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentImportFile::WriteDataComplete()
	{
	LOGFN( "CWmDrmAgentImportFile::WriteDataComplete" );
    TInt r = KErrNone;
    CSupplierOutputFile *temp = NULL;
    
    if( iFileOpen )
        {
        iFile.Close();
        iFileOpen = EFalse;
        }

    if( iOutputFileName )
        {
        TRAP(r, temp = CSupplierOutputFile::NewL( iOutputFileName->Des(), EContent, iMimeType->Des()));
        if( r == KErrNone )
            {
            r = iOutputFiles.Append(temp);
            if( r != KErrNone )
                {
                delete temp;
                }
            }
        }
	return r;
	}
		
// -----------------------------------------------------------------------------
// CWmDrmAgentImportFile::
// 
// -----------------------------------------------------------------------------
//
void CWmDrmAgentImportFile::WriteData(
    const TDesC8& aData,
    TRequestStatus &aStatus)
	{
	LOGFN( "CWmDrmAgentImportFile::WriteData" );
	TRequestStatus *ptr = &aStatus;
	TInt r = WriteData(aData);
	User::RequestComplete(ptr,r);
	}

// -----------------------------------------------------------------------------
// CWmDrmAgentImportFile::
// 
// -----------------------------------------------------------------------------
//
void CWmDrmAgentImportFile::WriteDataComplete(
    TRequestStatus &aStatus)
	{
	LOGFN( "CWmDrmAgentImportFile::WriteDataComplete" );
	TRequestStatus *ptr = &aStatus;
	TInt r = WriteDataComplete();
	User::RequestComplete(ptr,r);
	}
	
// -----------------------------------------------------------------------------
// CWmDrmAgentImportFile::
// 
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentImportFile::OutputFileCountL() const
	{
	LOGFN( "CWmDrmAgentImportFile::OutputFileCountL" );
	return iOutputFiles.Count();
	}

// -----------------------------------------------------------------------------
// CWmDrmAgentImportFile::
// 
// -----------------------------------------------------------------------------
//
CSupplierOutputFile& CWmDrmAgentImportFile::OutputFileL(
    TInt aIndex)
	{
	LOGFN( "CWmDrmAgentImportFile::OutputFileL" );
	return *iOutputFiles[aIndex];
	}

// -----------------------------------------------------------------------------
// CWmDrmAgentImportFile::
// 
// -----------------------------------------------------------------------------
//
TImportStatus CWmDrmAgentImportFile::GetImportStatus() const
	{
	LOGFN( "CWmDrmAgentImportFile::GetImportStatus" );
	return iImportStatus;
	}	

// -----------------------------------------------------------------------------
// CWmDrmAgentImportFile::
// 
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentImportFile::GetSuggestedOutputFileExtension(
    TDes& /*aFileExtension*/)
	{
	LOGFN( "CWmDrmAgentImportFile::GetSuggestedOutputFileExtension" );
    return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// CWmDrmAgentImportFile::
// 
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentImportFile::GetSuggestedOutputFileName(
    TDes& /*aFileName*/)
	{
	LOGFN( "CWmDrmAgentImportFile::GetSuggestedOutputFileName" );
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// CWmDrmAgentImportFile::
// 
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentImportFile::ContinueWithNewOutputFile(
    RFile& aFile,
    const TDesC& aFileName)
	{
	RFile file;
	TInt r = KErrNone;
	LOGFN( "CWmDrmAgentImportFile::ContinueWithNewOutputFile" );

	if (iOutputFileName != NULL)
	    {
	    delete iOutputFileName;
	    iOutputFileName = NULL;
	    }
	    
	TRAP(r, iOutputFileName = aFileName.AllocL());
	
	if (r == KErrNone)
	    {
	    iFile.Close();
	    iFile.Duplicate(aFile);
        iFileOpen = ETrue;
	    }
	return r;
	}

// -----------------------------------------------------------------------------
// CWmDrmAgentImportFile::
// 
// -----------------------------------------------------------------------------
//
void CWmDrmAgentImportFile::ContinueWithNewOutputFile(
    RFile& aFile,
    const TDesC& aFileName,
    TRequestStatus& aStatus)
	{
	LOGFN( "CWmDrmAgentImportFile::ContinueWithNewOutputFile (2)" );
	TRequestStatus *ptr = &aStatus;
	TInt r = ContinueWithNewOutputFile(aFile, aFileName);
	User::RequestComplete(ptr,r);
	}

// -----------------------------------------------------------------------------
// CWmDrmAgentImportFile::
// 
// -----------------------------------------------------------------------------
//
void CWmDrmAgentImportFile::NewMimePartL(
    const TDesC8& /*aMimeType*/,
    const CMetaDataArray& /*aImportMetaData*/)
	{
	LOGFN( "CWmDrmAgentImportFile::NewMimePartL" );
	User::Leave(KErrCANotSupported);
	}

// -----------------------------------------------------------------------------
// CWmDrmAgentImportFile::
// 
// -----------------------------------------------------------------------------
//
void CWmDrmAgentImportFile::EndMimePartL()
	{
	LOGFN( "CWmDrmAgentImportFile::EndMimePartL" );
	User::Leave(KErrCANotSupported);
	}

// End of file
