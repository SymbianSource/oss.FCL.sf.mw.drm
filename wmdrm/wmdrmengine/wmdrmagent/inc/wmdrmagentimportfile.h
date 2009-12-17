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
* Description:  Declaration of the WmDrmAgentImportFile class
*
*/


#ifndef WMDRMAGENTIMPORTFILE_H
#define WMDRMAGENTIMPORTFILE_H

#include <s32file.h>
#include <WmDrmAgent.h>

#include "agentinterface.h"

using namespace ContentAccess;


class CWmDrmAgentImportFile : public CAgentImportFile
    {
public:

    enum TImportType
        {
        EOma1DrmMessage,
        EOma1XmlRo,
        EOma1WbxmlRo,
        EOma2Ro,
        EPlainContent
        };

    static CWmDrmAgentImportFile* NewL(
        const TDesC8& aMimeType,
        const CMetaDataArray& aMetaDataArray,
        const TDesC& aOutputDirectory,
        const TDesC& aSuggestedFileName);
        
    static CWmDrmAgentImportFile* NewL(
        const TDesC8& aMimeType,
        const CMetaDataArray& aMetaDataArray);
        
    virtual ~CWmDrmAgentImportFile();

public:

    // From CAgentImportFile
    virtual TInt WriteData(
        const TDesC8& aData);
        
    virtual void WriteData(
        const TDesC8& aData,
        TRequestStatus& aStatus);
        
    virtual TInt WriteDataComplete();
    
    virtual void WriteDataComplete(
        TRequestStatus& aStatus);
        
    virtual TInt OutputFileCountL() const;
    
    virtual CSupplierOutputFile& OutputFileL(
        TInt aIndex);
        
    virtual TImportStatus GetImportStatus() const;
    
    virtual TInt GetSuggestedOutputFileExtension(
        TDes& aFileExtension);
        
    virtual TInt GetSuggestedOutputFileName(
        TDes& aFileName);
        
    virtual TInt ContinueWithNewOutputFile(
        RFile& aFile, 
        const TDesC& aFileName);
        
    virtual void ContinueWithNewOutputFile(
        RFile& aFile, 
        const TDesC& aFileName, 
        TRequestStatus& aStatus);
        
    virtual void NewMimePartL(
        const TDesC8& aMimeType, 
        const CMetaDataArray& aImportMetaData);
        
    virtual void EndMimePartL();

private:

    CWmDrmAgentImportFile(
        TBool aAgentCreatesOutputFiles);
        
    void ConstructL(
        const TDesC8& aMimeType, 
        const CMetaDataArray& aMetaDataArray, 
        const TDesC& aOutputDirectory, 
        const TDesC& aSuggestedFileName);
        
    TInt OpenOutputFile();
    
    void WriteDataL(const TDesC8& aData);
    
private:

    TImportType iImportType;
    HBufC8 *iMimeType;
    HBufC *iOutputDirectory;
    HBufC *iSuggestedFileName;
    HBufC *iOutputFileName;
    HBufC8 *iLastWriteData;
    RFs iFs;
    RFile iFile;
    RPointerArray<CSupplierOutputFile> iOutputFiles;
    TImportStatus iImportStatus;
    TBool iAgentCreatesOutputFiles;
    TBool iFileOpen;
    };
    
#endif // WMDRMAGENTIMPORTFILE_H
