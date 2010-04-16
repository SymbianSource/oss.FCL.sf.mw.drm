/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of the Oma2AgentImportFile class
*
*/


#ifndef __OMA2AGENTIMPORTFILE_H__
#define __OMA2AGENTIMPORTFILE_H__

#include <s32file.h>
#include <caf/agentinterface.h>
#include <Oma2Agent.h>

using namespace ContentAccess;

class CDRMMessageParser;
class COma1DcfCreator;
class CDRMRights;

class COma2AgentImportFile : public CAgentImportFile
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

    static COma2AgentImportFile* NewL(
        const TDesC8& aMimeType,
        const CMetaDataArray& aMetaDataArray,
        const TDesC& aOutputDirectory,
        const TDesC& aSuggestedFileName);

    static COma2AgentImportFile* NewL(
        const TDesC8& aMimeType,
        const CMetaDataArray& aMetaDataArray);

    virtual ~COma2AgentImportFile();

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

    COma2AgentImportFile(
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
    HBufC *iOutputDirectory;
    HBufC *iSuggestedFileName;
    HBufC *iOutputFileName;
    HBufC8 *iLastWriteData;
    RFs iFs;
    RFileWriteStream iFile;
    RPointerArray<CSupplierOutputFile> iOutputFiles;
    TImportStatus iImportStatus;
    TBool iAgentCreatesOutputFiles;
    TBool iFileOpen;
    CDRMMessageParser* iMessageParser;
    COma1DcfCreator* iDcfCreator;
    CDRMRights* iDcfRights;
    HBufC8* iDcfMimeType;
    };

#endif // __OMA2AGENTIMPORTFILE_H__
