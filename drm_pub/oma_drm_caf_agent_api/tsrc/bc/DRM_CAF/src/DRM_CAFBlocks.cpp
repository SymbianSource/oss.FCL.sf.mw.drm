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
* Description:
*
*/









// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "DRM_CAF.h"
#include <caf/caftypes.h>
#include <caf/caf.h>
#include <caf/data.h>
#include <f32file.h>
#include <bautils.h>
#include <TestclassAssert.h>
using namespace ContentAccess;
// LOCAL CONSTANTS AND MACROS

#define NUM_ELEMENTS(x) (sizeof(x) / sizeof(x[0]))

const TInt KAttributes[] =
    {
    EIsProtected,
    EIsForwardable,
    EIsModifyable,
    EIsCopyable,
    ECanPlay,
    ECanPrint,
    ECanExecute,
    ECanView,
    ERightsNone,
    ERightsPending,
    ERightsHaveExpired,
    EPreviewAvailable,
    EContentCDataInUse,
    ECanRewind,
    ECopyPaste,
    ERightsConsumable,
    ERightsStateless,
    ECanMove,
    ECanRename,
    ECanAutomaticConsume,
    EFileType,
    EPreviewType,
    ESilentRightsType
    };

const TInt KStringAttributes[] =
    {
    EEmbeddedRightsObject,
    EDomainRightsIssuerUrl,
    EDescription,
    EMimeType,
    ETitle,
    EAuthor,
    EIconURI,
    EPreviewURI,
    EContentURI,
    EContentID,
    EInfoURL,
    EPendingRightsETA,
    EInstantPreviewUri,
    EPreviewRightsUri,
    ERightsIssuerUrl,
    ESilentRightsUrl,
    ETransactionTrackingId,
    };

// Test logging macros(we only want line numbers)
//#undef test
//#undef __test

//#define __test(x, l) test(x, l)
//#define test(x) __test(x, __LINE__)

//#define FuncCount( a ) ( sizeof( a ) / sizeof( TBatTestFunc ) )



#define RUNCHECKED(function) \
    { \
    TInt c = function; \
    if (c != KErrNone) \
        { \
        STIF_ASSERT_TRUE(c); \
        } \
    else \
        { \
        GLastOk = __LINE__;\
        } \
    }

#define RUNCHECKEDL(function) \
    { \
    TInt c = function; \
    if (c != KErrNone) \
        { \
        STIF_ASSERT_TRUE(c); \
        User::Leave(c); \
        } \
    else \
        { \
        GLastOk = __LINE__;\
          } \
    }


#define RUN_TRAPPED(r, function) \
    { \
    r = KErrNone; TRAP(r, function); \
    STIF_ASSERT_EQUALS( KErrNone, r )\
    }

//RTest test(_L("c:\\OMADRM2BC.log"));
//#if 0
//RTest test(_L("c:\\OMADRM2BC.log"));
//TInt GLastOk;
//TInt GAllocatedCount;
//TInt GAllocatedSize;

//TBool flvariant;
//#endif

_LIT8(KROSimple, "<o-ex:rights xmlns:o-ex = \"http://odrl.net/1.1/ODRL-EX\" \
xmlns:o-dd = \"http://odrl.net/1.1/ODRL-DD\" \
xmlns:ds = \"http://www.w3.org/2000/09/xmldsig#/\">\
<o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context>\
<o-ex:agreement><o-ex:asset><o-ex:context>\
<o-dd:uid>cid:9999</o-dd:uid></o-ex:context></o-ex:asset><o-ex:permission>\
<o-dd:display/><o-dd:play/><o-dd:execute/><o-dd:print/>\
</o-ex:permission></o-ex:agreement></o-ex:rights>");

_LIT8(KDRCSimple,
"\003\016\152\000\305\005\205\006\206\007\207\001\106\107\003\061\056\060\000\001\
\001\111\112\106\110\003\143\151\144\072\062\060\060\063\060\064\062\071\061\062\
\060\067\065\063\055\063\064\064\067\060\071\066\063\066\060\100\167\167\167\056\
\147\157\157\147\154\145\056\143\157\155\000\001\001\113\114\303\020\063\323\173\
\130\367\145\145\367\077\137\105\057\120\076\265\377\001\001\001\115\116\122\123\
\003\065\000\001\001\001\117\122\124\125\003\062\060\060\063\055\060\064\055\062\
\071\124\061\062\072\060\064\072\064\067\000\001\126\003\062\060\060\063\055\060\
\065\055\062\061\124\061\062\072\060\064\072\064\067\000\001\001\001\001\001\001\
\001");
_LIT8(KDMHead, "--boundary-1\r\nContent-Type: text/plain\r\n\
Content-Transfer-Encoding: 8bit\r\n\
\r\n");

_LIT8(KDMTail, "\r\n--boundary-1--");

_LIT8(KDMContent, "0123456789");


_LIT8(KDMSimple, "--boundary-1\r\nContent-Type: text/plain\r\n\
Content-Transfer-Encoding: 8bit\r\n\
\r\n\
012345678911234567892123456789312345678941234567895123456789\
612345678971234567898123456789912345678901234567891123456789\
212345678931234567894123456789512345678961234567897123456789\
812345678991234567890123456789112345678921234567893123456789\
412345678951234567896123456789712345678981234567899123456789\
\r\n--boundary-1--\r\n");

_LIT8(KDMCDCount, "--random78o6bP%[GB6b/8&/45&%*^'?vS\r\n\
Content-Type: application/vnd.oma.drm.rights+xml\r\n\
Content-Transfer-Encoding: binary\r\n\
\r\n\
<o-ex:rights\
   xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\"\
   xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\"\
   xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"\
>\
<o-ex:context><o-dd:version>1.0</o-dd:version>\
</o-ex:context><o-ex:agreement><o-ex:asset>\
<o-ex:context><o-dd:uid>cid:20031217123451-8428794033@www.nokia.com</o-dd:uid>\
</o-ex:context></o-ex:asset>\
<o-ex:permission>\
<o-dd:display>\
<o-ex:constraint>\
<o-dd:count>3</o-dd:count>\
</o-ex:constraint>\
</o-dd:display>\
<o-dd:display>\
</o-ex:permission>\
</o-ex:agreement>\
</o-ex:rights>\r\n\
\r\n\
--random78o6bP%[GB6b/8&/45&%*^'?vS\r\n\
Content-Type: text/plain\r\n\
Content-ID: <20031217123451-8428794033@www.nokia.com>\r\n\
Content-Transfer-Encoding: binary\r\n\
\r\n\
Check:\r\n\
DbCnt: 109ms r:0\r\n\
Fixes:\r\n\
\r\n\
--random78o6bP%[GB6b/8&/45&%*^'?vS--");

_LIT(KOma1Content, "c:\\content.dcf");
_LIT(KOma2Content, "c:\\content.odf");
_LIT(KOma2ContentNoEnc, "c:\\content-noenc.odf");
_LIT(KOma2ContentNoEncTruncated, "c:\\content-noenc-trunc.odf");

_LIT(KEncryptedDir, "c:\\");
_LIT(KTempDcfName, "test.dcf");
_LIT(KTempDcfPathName, "c:\\test.dcf");
_LIT(KFVariantFile,"c:\\DrmCAFVarFile.txt");


// ============================= LOCAL FUNCTIONS ===============================


NONSHARABLE_CLASS( CActiveWaiter ): public CActive
    {
public:
    static CActiveWaiter* NewL();

    void StartWait();

    TRequestStatus& GetStatus();

    ~CActiveWaiter();
protected:     // from CActive
    /**
    * part of active object framework, provide default implementation
    */
    void RunL();

    /**
    * part of active object framework, provide default implementation
    */
    void DoCancel();

    /**
    * part of active object framework, provide default implementation
    */
    TInt RunError(TInt aError);


private:
    CActiveWaiter();

    void ConstructL();

    CActiveSchedulerWait iWait;
    };


CActiveWaiter::CActiveWaiter() : CActive(EPriorityStandard)
    {

    }

CActiveWaiter::~CActiveWaiter()
    {
    Deque();

    if( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }

CActiveWaiter* CActiveWaiter::NewL() {
    CActiveWaiter* self = new (ELeave) CActiveWaiter();
    return self;
}


/**
* part of active object framework, provide default implementation
*/
void CActiveWaiter::RunL() {
    iWait.AsyncStop();
}

/**
* part of active object framework, provide default implementation
*/
void CActiveWaiter::DoCancel(){
}

/**
* part of active object framework, provide default implementation
*/
TInt CActiveWaiter::RunError(TInt /*aError*/){
    return KErrNone;
}

void CActiveWaiter::StartWait() {
    if(!IsAdded())
        {
        CActiveScheduler::Add(this);
        }
    if(!IsActive())
        {
        SetActive();
        }

    iStatus = KRequestPending;

    iWait.Start();
}

TRequestStatus& CActiveWaiter::GetStatus()
    {
    return iStatus;
    }



LOCAL_C CRightsManager* GetOmaDrmRightsManagerL()
    {
    RArray<TAgent> agents;
    CRightsManager* rm = NULL;
    TInt i;
    TInt r;

    CManager* manager = CManager::NewLC();
    //STIF_ASSERT_NOT_NULL( manager );
    manager->ListAgentsL(agents);
    for (i = 0; rm == NULL && i < agents.Count(); i++)
        {
        if (agents[i].Name().Compare(KOmaDrm2AgentName) == 0)
            {
            TRAP(r, rm = manager->CreateRightsManagerL(agents[i]));
            }
        }
    agents.Close();
    CleanupStack::PopAndDestroy();
    return rm;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRM_CAF::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CDRM_CAF::Delete()
    {

    }

// -----------------------------------------------------------------------------
// CDRM_CAF::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CDRM_CAF::RunMethodL(
    CStifItemParser& aItem )
    {

    	TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "CSupplier_Oma1XmlRoL", CDRM_CAF::CSupplier_Oma1XmlRoL ),
        ENTRY( "CSupplier_Oma1WbxmlRoL", CDRM_CAF::CSupplier_Oma1WbxmlRoL ),
        ENTRY( "CData_NewL", CDRM_CAF::CData_NewL ),
        ENTRY( "CData_Read_DataSize_SeekL", CDRM_CAF::CData_Read_DataSize_SeekL ),
        ENTRY( "CData_Read2L", CDRM_CAF::CData_Read2L ),
        ENTRY( "CData_SetPropertyL", CDRM_CAF::CData_SetPropertyL ),
        ENTRY( "CData_EvaluateIntentL", CDRM_CAF::CData_EvaluateIntentL ),
        ENTRY( "CData_ExecuteIntentL", CDRM_CAF::CData_ExecuteIntentL ),
        ENTRY( "CData_GetAttributeL", CDRM_CAF::CData_GetAttributeL ),
        ENTRY( "CData_GetAttributeSetL", CDRM_CAF::CData_GetAttributeSetL ),
        ENTRY( "CData_GetStringAttributeL", CDRM_CAF::CData_GetStringAttributeL ),
        ENTRY( "CData_GetStringAttributeSetL", CDRM_CAF::CData_GetStringAttributeSetL ),
        ENTRY( "CContent_NewL", CDRM_CAF::CContent_NewL ),
        ENTRY( "CContent_OpenCloseContainerL", CDRM_CAF::CContent_OpenCloseContainerL ),
        ENTRY( "CContent_GetEmbeddedObjectsL", CDRM_CAF::CContent_GetEmbeddedObjectsL ),
        ENTRY( "CContent_SearchL", CDRM_CAF::CContent_SearchL ),
        ENTRY( "CContent_GetAttributeL", CDRM_CAF::CContent_GetAttributeL ),
        ENTRY( "CContent_GetAttributeSetL", CDRM_CAF::CContent_GetAttributeSetL ),
        ENTRY( "CContent_GetStringAttributeL", CDRM_CAF::CContent_GetStringAttributeL ),
        ENTRY( "CContent_GetStringAttributeSetL", CDRM_CAF::CContent_GetStringAttributeL ),
        ENTRY( "CContent_AgentSpecificCommandL", CDRM_CAF::CContent_AgentSpecificCommandL ),
        ENTRY( "CContent_NotifyStatusChangeL", CDRM_CAF::CContent_NotifyStatusChangeL ),
        ENTRY( "CContent_CancelNotifyStatusChangeL", CDRM_CAF::CContent_CancelNotifyStatusChangeL ),
        ENTRY( "CContent_RequestRightsL", CDRM_CAF::CContent_RequestRightsL ),
        ENTRY( "CContent_DisplayInfoL", CDRM_CAF::CContent_DisplayInfoL ),
        ENTRY( "CContent_SetPropertyL", CDRM_CAF::CContent_SetPropertyL ),
        ENTRY( "CContent_OpenContentL", CDRM_CAF::CContent_OpenContentL ),
        ENTRY( "CManager_FileOperationsL", CDRM_CAF::CManager_FileOperationsL ),
        ENTRY( "CManager_GetAttributeL", CDRM_CAF::CManager_GetAttributeL ),
        ENTRY( "CManager_GetAttributeSetL", CDRM_CAF::CManager_GetAttributeSetL ),
        ENTRY( "CManager_GetStringAttributeL", CDRM_CAF::CManager_GetStringAttributeL ),
        ENTRY( "CManager_GetStringAttributeSetL", CDRM_CAF::CManager_GetStringAttributeSetL ),
        ENTRY( "CManager_SetPropertyL", CDRM_CAF::CManager_SetPropertyL ),
        ENTRY( "CManager_DisplayInfoL", CDRM_CAF::CManager_DisplayInfoL ),
        ENTRY( "CManager_ListAgentsL", CDRM_CAF::CManager_ListAgentsL ),
        ENTRY( "CManager_AgentSpecificCommandL", CDRM_CAF::CManager_AgentSpecificCommandL ),
        ENTRY( "CManager_CreateRightsManagerL", CDRM_CAF::CManager_CreateRightsManagerL ),
        ENTRY( "CSupplier_IsImportSupportedL", CDRM_CAF::CSupplier_IsImportSupportedL ),
        ENTRY( "CSupplier_PrepareHTTPRequestHeadersL", CDRM_CAF::CSupplier_PrepareHTTPRequestHeadersL ),
        ENTRY( "CSupplier_SetOutputDirectory_ImportFileL", CDRM_CAF::CSupplier_SetOutputDirectory_ImportFileL ),
        ENTRY( "CImportFile_WriteData_WriteDataComplete_OutputFileCountL", CDRM_CAF::CImportFile_WriteData_WriteDataComplete_OutputFileCountL ),
        ENTRY( "CImportFile_OutputFileL_GetImportStatusL", CDRM_CAF::CImportFile_OutputFileL_GetImportStatusL ),
        ENTRY( "CImportFile_GetSuggestedOutputFileExtensionL", CDRM_CAF::CImportFile_GetSuggestedOutputFileExtensionL ),
        ENTRY( "CImportFile_GetSuggestedOutputFileNameL", CDRM_CAF::CImportFile_GetSuggestedOutputFileNameL ),
        ENTRY( "CImportFile_ContinueWithNewOutputFileL", CDRM_CAF::CImportFile_ContinueWithNewOutputFileL ),
        ENTRY( "CImportFile_ContentMimeTypeL", CDRM_CAF::CImportFile_ContentMimeTypeL ),
        ENTRY( "CRightsManager_ListRightsL", CDRM_CAF::CRightsManager_ListRightsL ),
        ENTRY( "CRightsManager_ListAllRightsL", CDRM_CAF::CRightsManager_ListAllRightsL ),
        ENTRY( "CRightsManager_ListContentL", CDRM_CAF::CRightsManager_ListContentL ),
        ENTRY( "CRightsManager_DeleteRightsObjectL", CDRM_CAF::CRightsManager_DeleteRightsObjectL ),
        ENTRY( "CRightsManager_DeleteAllRightsObjectsL", CDRM_CAF::CRightsManager_DeleteAllRightsObjectsL ),
        ENTRY( "CRightsManager_SetPropertyL", CDRM_CAF::CRightsManager_SetPropertyL ),
        ENTRY( "CDRMLicenseChecker_CheckLicenseL", CDRM_CAF::CDRMLicenseChecker_CheckLicenseL ),

        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

TInt CDRM_CAF::CSupplier_Oma1XmlRoL()
    {

    CSupplier* supplier = CSupplier::NewLC();
    CMetaDataArray* metaData = CMetaDataArray::NewLC();
    CImportFile* file = supplier->ImportFileL(KOma1XmlRoContentType,
        *metaData);
    STIF_ASSERT_NOT_NULL( file );
    CleanupStack::PushL(file);
    RUNCHECKEDL(file->WriteData(KROSimple));
    RUNCHECKEDL(file->WriteDataComplete());
    CleanupStack::PopAndDestroy(3);

    return KErrNone;
    }

TInt CDRM_CAF::CSupplier_Oma1WbxmlRoL()
    {

    CSupplier* supplier = CSupplier::NewLC();
    CMetaDataArray* metaData = CMetaDataArray::NewLC();
    CImportFile* file = supplier->ImportFileL(KOma1WbxmlRoContentType,
        *metaData);
    STIF_ASSERT_NOT_NULL( file );
    CleanupStack::PushL(file);
    RUNCHECKEDL(file->WriteData(KDRCSimple));
    RUNCHECKEDL(file->WriteDataComplete());
    CleanupStack::PopAndDestroy(3);

    return KErrNone;
    }


TInt CDRM_CAF::CData_NewL()
    {
    CData* data;
    RFile file;

    TVirtualPathPtr dcf1(KOma1Content, KDefaultContentObject);
    TVirtualPathPtr dcf2(KOma2Content, KDefaultContentObject);
    TVirtualPathPtr dcf3(KOma2ContentNoEnc, KDefaultContentObject);


    data = CData::NewL(dcf1, EPeek, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    delete data;



    data = CData::NewL(dcf2, EPeek, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    delete data;



    data = CData::NewL(dcf3, EPeek, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    delete data;



    TInt err = file.Open(iFs, KOma1Content, EFileRead);
    CleanupClosePushL(file);
    data = CData::NewL(file, KDefaultContentObject, EPeek);
    STIF_ASSERT_NOT_NULL( data );
    delete data;
    CleanupStack::PopAndDestroy();



    err = file.Open(iFs, KOma2Content, EFileRead);
    CleanupClosePushL(file);
    data = CData::NewL(file, KDefaultContentObject, EPeek);
    STIF_ASSERT_NOT_NULL( data );
    delete data;
    CleanupStack::PopAndDestroy();



    err = file.Open(iFs, KOma2ContentNoEnc, EFileRead);
    CleanupClosePushL(file);
    data = CData::NewL(file, KDefaultContentObject, EPeek);
    STIF_ASSERT_NOT_NULL( data );
    delete data;
    CleanupStack::PopAndDestroy();



    err = file.Open(iFs, KOma2ContentNoEncTruncated, EFileRead);
    CleanupClosePushL(file);
    data = CData::NewL(file, KDefaultContentObject, EPeek);
    STIF_ASSERT_NOT_NULL( data );
    delete data;
    CleanupStack::PopAndDestroy();

    return KErrNone;
    }

TInt CDRM_CAF::CData_Read_DataSize_SeekL()
    {
    CData* data = NULL;
    TFileName fileName;
    TBuf8<256> buffer;
    TInt size;
    TInt pos;
    TInt64 size64;
    TInt64 pos64;

    fileName.Copy(KEncryptedDir);
    fileName.Append(KTempDcfName);
    TRequestStatus status;
    CActiveWaiter* waiter = CActiveWaiter::NewL();
    CleanupStack::PushL(waiter);

    TVirtualPathPtr dcf1(fileName, KDefaultContentObject);
    TVirtualPathPtr dcf2(KOma2ContentNoEncTruncated, KDefaultContentObject);
    TVirtualPathPtr dcf3(KOma2ContentNoEnc, KDefaultContentObject);


    iFs.Delete(fileName);
    CSupplier* supplier = CSupplier::NewLC();
    CMetaDataArray* metaData = CMetaDataArray::NewLC();
    supplier->SetOutputDirectoryL(KEncryptedDir);
    CImportFile* file = supplier->ImportFileL(KOma1DrmMessageContentType,
        *metaData, KTempDcfName);
    STIF_ASSERT_NOT_NULL( file );
    CleanupStack::PushL(file);
    RUNCHECKEDL(file->WriteData(KDMSimple));
    RUNCHECKEDL(file->WriteDataComplete());
    STIF_ASSERT_EQUALS( 1, file->OutputFileCountL() );
    CleanupStack::PopAndDestroy(3);



    data = CData::NewLC(dcf1, EView, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    data->DataSizeL(size);
    STIF_ASSERT_EQUALS( 300, size );

    STIF_ASSERT_EQUALS( KErrAccessDenied, data->Read(buffer, 10) ); // Cannot decrypt with SDK
    CleanupStack::PopAndDestroy( data );



    data = CData::NewLC(dcf2, EPlay, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    data->DataSizeL(size);
    CleanupStack::PopAndDestroy( data );



    data = CData::NewLC(dcf3, EPlay, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    data->DataSizeL(size);

    RUNCHECKED(data->ExecuteIntent(EPlay));
    RUNCHECKED(data->Read(buffer, 8));
    STIF_ASSERT_EQUALS( 0, buffer.Compare(_L8("34567890")));
    pos = 100;
    RUNCHECKED(data->Seek(ESeekStart, pos));

    data->Read(buffer, 8, status);
    User::WaitForRequest(status);

    STIF_ASSERT_EQUALS( 0, buffer.Compare(_L8("00000000")));
    data->Read(buffer, status);
    User::WaitForRequest(status);

    CleanupStack::PopAndDestroy( data );

    data = CData::NewLC(dcf3, EPlay, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    data->DataSize64L(size64);

    RUNCHECKED(data->ExecuteIntent(EPlay));
    pos64 = 100;
    RUNCHECKED(data->Seek64(ESeekStart, pos64));

    data->Read(pos64, buffer, 8, waiter->GetStatus());
    waiter->StartWait();

    STIF_ASSERT_EQUALS( 0, buffer.Compare(_L8("00000000")));

    CleanupStack::PopAndDestroy( data );
    CleanupStack::PopAndDestroy( waiter );

    return KErrNone;
    }

TInt CDRM_CAF::CData_Read2L()
    {
    CData* data = NULL;
    TFileName fileName;
    TBuf8<256> buffer;
    TInt size;
    TInt pos;
    TInt r;
    CSupplier* supplier = NULL;
    CMetaDataArray* metaData = NULL;
    CImportFile* file = NULL;

    fileName.Copy(KEncryptedDir);
    fileName.Append(KTempDcfName);

    TVirtualPathPtr dcf1(fileName, KDefaultContentObject);
    TVirtualPathPtr dcf2(KOma2ContentNoEncTruncated, KDefaultContentObject);
    TVirtualPathPtr dcf3(KOma2ContentNoEnc, KDefaultContentObject);


    r = iFs.Delete(fileName);
    STIF_ASSERT_EQUALS( KErrNone, r);
    supplier = CSupplier::NewLC();
    metaData = CMetaDataArray::NewLC();
    supplier->SetOutputDirectoryL(KEncryptedDir);
    file = supplier->ImportFileL(KOma1DrmMessageContentType,
        *metaData, KTempDcfName);
    STIF_ASSERT_NOT_NULL( file );
    CleanupStack::PushL(file);
    RUNCHECKEDL(file->WriteData(KDMHead));
    for (pos = 0; pos < 500; pos++)
        {
        file->WriteData(KDMContent);
        }
    RUNCHECKEDL(file->WriteData(KDMTail));
    RUNCHECKEDL(file->WriteDataComplete());
    STIF_ASSERT_EQUALS( 1, file->OutputFileCountL());
    CleanupStack::PopAndDestroy(3);



    data = CData::NewLC(dcf1, EView, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    data->DataSizeL(size);
    STIF_ASSERT_EQUALS( 5000, size);
    STIF_ASSERT_EQUALS( KErrAccessDenied, data->ExecuteIntent(EView) ); // Cannot decrypt with SDK
    //STIF_ASSERT_EQUALS( KErrNone, data->ExecuteIntent(EView) ); // we can decrypt just fine, it's omadrm1 content nokia VID
    CleanupStack::PopAndDestroy(); // data



    data = CData::NewLC(dcf1, EView, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    data->DataSizeL(size);
    STIF_ASSERT_EQUALS( 5000, size);
    CleanupStack::PopAndDestroy();

    return KErrNone;
    }
TInt CDRM_CAF::CData_SetPropertyL()
    {
    CData* data;
    TInt r;
    TBool r1 = EFalse;

    TVirtualPathPtr dcf2(KOma2Content, KDefaultContentObject);


    data = CData::NewLC(dcf2, EPeek, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    r = data->SetProperty(EAgentPropertyBufferSize, 1024);
    if(r == KErrCANotSupported || r == KErrNone)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);
    r1 = EFalse;
    r = data->SetProperty(EAgentPropertyAgentUI, 1);
    if(r == KErrCANotSupported || r == KErrNone)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);
    r1 = EFalse;
    r = data->SetProperty(EAgentPropertyMultipleSequence, 1);
    if(r == KErrCANotSupported || r == KErrNone)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);
    CleanupStack::PopAndDestroy();

    return KErrNone;
    }

TInt  CDRM_CAF::CData_EvaluateIntentL()
    {
    TFileName fileName;
    TInt r;
    TBool r1 = EFalse;
    TInt i;


    fileName.Copy(KFVariantFile);
    // Clearing file before first testcase
    iFs.Delete(fileName);

    fileName.Copy(KEncryptedDir);
    fileName.Append(KTempDcfName);
    r = iFs.Delete(fileName);
    STIF_ASSERT_EQUALS( KErrNone, r);
    CSupplier* supplier = CSupplier::NewLC();
    CMetaDataArray* metaData = CMetaDataArray::NewLC();
    supplier->SetOutputDirectoryL(KEncryptedDir);
    CImportFile* file = supplier->ImportFileL(KOma1DrmMessageContentType,
        *metaData, KTempDcfName);
    STIF_ASSERT_NOT_NULL( file );
    CleanupStack::PushL(file);
    r = file->WriteData(KDMCDCount);
    if (r == KErrNotSupported)
    {
        // Forward lock variant
        RFileWriteStream outFlV;
        outFlV.Create(iFs, KFVariantFile, EFileWrite );
        outFlV.Close();
        CleanupStack::PopAndDestroy(3);
        return KErrNone;
    }
    RUNCHECKEDL(file->WriteDataComplete());
    STIF_ASSERT_EQUALS( 1, file->OutputFileCountL());
    CleanupStack::PopAndDestroy(3);



    TVirtualPathPtr dcf(fileName, KDefaultContentObject);
    CData* data = CData::NewLC(dcf, EView, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    r = data->EvaluateIntent(EView);
    STIF_ASSERT_EQUALS( KErrNone, r);
    r = data->EvaluateIntent(EPause);
    STIF_ASSERT_EQUALS( KErrNone, r);
    r = data->EvaluateIntent(EContinue);
    STIF_ASSERT_EQUALS( KErrNone, r);
    r = data->EvaluateIntent(EStop);
    STIF_ASSERT_EQUALS( KErrNone, r);
    r = data->EvaluateIntent(EPeek);
    STIF_ASSERT_EQUALS( KErrNone, r);
    r = data->EvaluateIntent(EInstall);
    STIF_ASSERT_EQUALS( KErrNone, r);
    r = data->EvaluateIntent(EPlay);
    STIF_ASSERT_EQUALS( KErrCANoPermission, r);
    r = data->EvaluateIntent(EPrint);
    STIF_ASSERT_EQUALS( KErrCANoPermission, r);
    r = data->EvaluateIntent(EExecute);
    STIF_ASSERT_EQUALS( KErrCANoPermission, r);
    CleanupStack::PopAndDestroy();



    data = CData::NewL(TVirtualPathPtr(KOma2ContentNoEnc, KDefaultContentObject),
        EPeek, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    STIF_ASSERT_EQUALS( KErrNone, data->EvaluateIntent(EPlay));
    STIF_ASSERT_NOT_EQUALS(KErrNone ,data->EvaluateIntent(EView) );
    STIF_ASSERT_NOT_EQUALS(KErrNone ,data->EvaluateIntent(EPrint) );
    STIF_ASSERT_NOT_EQUALS(KErrNone ,data->EvaluateIntent(EExecute) );
    STIF_ASSERT_EQUALS(KErrNone ,data->EvaluateIntent(EPause) );
    STIF_ASSERT_EQUALS(KErrNone ,data->EvaluateIntent(EStop) );
    STIF_ASSERT_EQUALS(KErrNone ,data->EvaluateIntent(EContinue) );
    delete data;



    fileName.Copy(KEncryptedDir);
    fileName.Append(KTempDcfName);
    r = iFs.Delete(fileName);
    STIF_ASSERT_EQUALS(KErrNone ,r);
    supplier = CSupplier::NewLC();
    metaData = CMetaDataArray::NewLC();
    metaData->AddL(KOmaImportMimeTypeField, _L8("image/jpeg"));
    supplier->SetOutputDirectoryL(KEncryptedDir);
    file = supplier->ImportFileL(KOmaImportContentType, *metaData, KTempDcfName);
    STIF_ASSERT_NOT_NULL( file );
    CleanupStack::PushL(file);
    for (i = 0; i < 10; i++)
        {
        RUNCHECKEDL(file->WriteData(KDMContent));
        }
    RUNCHECKEDL(file->WriteDataComplete());
    STIF_ASSERT_EQUALS(1 ,file->OutputFileCountL());
    CleanupStack::PopAndDestroy(3);



    data = CData::NewL(TVirtualPathPtr(fileName, KDefaultContentObject),
        EPeek, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );

    r = data->EvaluateIntent(EPlay);
    iLog->Log(_L("    Evaluate intent returned: %d \r\n"), r);
    STIF_ASSERT_NOT_EQUALS(KErrNone , r);


    r = data->EvaluateIntent(EView);
    iLog->Log(_L("    Evaluate intent returned: %d \r\n"), r);
    if(r == -17452 || r == -17451 || r == 0)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);
    r1 = EFalse;
    r = data->EvaluateIntent(EPrint);
    iLog->Log(_L("    Evaluate intent returned: %d \r\n"), r);
    if(r == -17452 || r == -17451 || r == 0)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);

    r = data->EvaluateIntent(EExecute);
    iLog->Log(_L("    Evaluate intent returned: %d \r\n"), r);
    STIF_ASSERT_NOT_EQUALS(KErrNone , r);


    delete data;



    fileName.Copy(KEncryptedDir );
    fileName.Append(KTempDcfName);
    r = iFs.Delete(fileName);
    STIF_ASSERT_EQUALS(KErrNone , r);
    supplier = CSupplier::NewLC();
    metaData = CMetaDataArray::NewLC();
    metaData->AddL(KOmaImportMimeTypeField, _L8("audio/mpeg"));
    supplier->SetOutputDirectoryL(KEncryptedDir);
    file = supplier->ImportFileL(KOmaImportContentType, *metaData, KTempDcfName);
    STIF_ASSERT_NOT_NULL( file );
    CleanupStack::PushL(file);
    for (i = 0; i < 10; i++)
        {
        RUNCHECKEDL(file->WriteData(KDMContent));
        }
    RUNCHECKEDL(file->WriteDataComplete());
    STIF_ASSERT_EQUALS(1 , file->OutputFileCountL());
    CleanupStack::PopAndDestroy(3);



    data = CData::NewL(TVirtualPathPtr(fileName, KDefaultContentObject),
        EPeek, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    r1 = EFalse;
    r = data->EvaluateIntent(EPlay);
    iLog->Log(_L("    Evaluate intent returned: %d \r\n"), r);
    if(r == -17452 || r == -17451 || r == 0)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);

    STIF_ASSERT_NOT_EQUALS(KErrNone , data->EvaluateIntent(EView));
    STIF_ASSERT_NOT_EQUALS(KErrNone , data->EvaluateIntent(EPrint));
    STIF_ASSERT_NOT_EQUALS(KErrNone , data->EvaluateIntent(EExecute));
    delete data;

    return KErrNone;
    }

TInt  CDRM_CAF::CData_ExecuteIntentL()
    {
    RFile fileFLV;

    TInt err = fileFLV.Open(iFs,KFVariantFile, EFileRead);
    if( err == 0 )
    {
        iLog->Log(_L("    Skipped in FL variant \r\n" ));
        fileFLV.Close();
        return KErrNone;
    }
    fileFLV.Close();

    TFileName fileName;
    TInt r;
    TInt i;
    TBool r1 = EFalse;


    fileName.Copy(KEncryptedDir);
    fileName.Append(KTempDcfName);
    r = iFs.Delete(fileName);
    STIF_ASSERT_EQUALS(KErrNone , r);
    CSupplier* supplier = CSupplier::NewLC();
    CMetaDataArray* metaData = CMetaDataArray::NewLC();
    supplier->SetOutputDirectoryL(KEncryptedDir);
    CImportFile* file = supplier->ImportFileL(KOma1DrmMessageContentType,
        *metaData, KTempDcfName);
    STIF_ASSERT_NOT_NULL( file );
    CleanupStack::PushL(file);
    RUNCHECKEDL(file->WriteData(KDMCDCount));
    RUNCHECKEDL(file->WriteDataComplete());
    STIF_ASSERT_EQUALS(1 , file->OutputFileCountL());
    CleanupStack::PopAndDestroy(3);


    // Start the tests

    TVirtualPathPtr dcf(fileName, KDefaultContentObject);
    CData* data = CData::NewLC(dcf, EView, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );


     // illegal: let's start with EPause
    r = data->ExecuteIntent(EPause);
    iLog->Log(_L("    Execute intent returned: %d \r\n"), r);
    STIF_ASSERT_EQUALS( KErrNotReady, r );

    r = data->ExecuteIntent(EContinue);
    iLog->Log(_L("    Execute intent returned: %d \r\n"), r);
    STIF_ASSERT_EQUALS( KErrNotReady, r );

    r = data->ExecuteIntent(EView);
    iLog->Log(_L("    Execute intent returned: %d \r\n"), r);
    STIF_ASSERT_EQUALS( KErrAccessDenied , r );

    r = data->ExecuteIntent(EPause);
    iLog->Log(_L("    Execute intent returned: %d \r\n"), r);
    STIF_ASSERT_EQUALS( KErrNotReady , r);

    r = data->ExecuteIntent(EContinue);
    iLog->Log(_L("    Execute intent returned: %d \r\n"), r);
    STIF_ASSERT_EQUALS( KErrNotReady, r);

    r = data->ExecuteIntent(EStop);
    iLog->Log(_L("    Execute intent returned: %d \r\n"), r);
    STIF_ASSERT_EQUALS( KErrNone, r);

    r = data->ExecuteIntent(EPeek);
    iLog->Log(_L("    Execute intent returned: %d \r\n"), r);
    STIF_ASSERT_EQUALS( KErrAccessDenied, r);

    r = data->ExecuteIntent(EInstall);
    iLog->Log(_L("    Execute intent returned: %d \r\n"), r);
    STIF_ASSERT_EQUALS( KErrAccessDenied, r);

    r = data->ExecuteIntent(EPlay);
    iLog->Log(_L("    Execute intent returned: %d \r\n"), r);
    STIF_ASSERT_EQUALS( KErrCANoPermission, r);

    r = data->ExecuteIntent(EPrint);
    iLog->Log(_L("    Execute intent returned: %d \r\n"), r);
    STIF_ASSERT_EQUALS( KErrCANoPermission, r);

    r = data->ExecuteIntent(EExecute);
    iLog->Log(_L("    Execute intent returned: %d \r\n"), r);
    STIF_ASSERT_EQUALS( KErrCANoPermission, r);

    r = data->ExecuteIntent(EView);
    iLog->Log(_L("    Execute intent returned: %d \r\n"), r);
    STIF_ASSERT_EQUALS( KErrAccessDenied, r);

    CleanupStack::PopAndDestroy();



    data = CData::NewL(TVirtualPathPtr(KOma2ContentNoEnc, KDefaultContentObject),
        EPeek, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    STIF_ASSERT_EQUALS(KErrNone ,data->ExecuteIntent(EPlay) );
    STIF_ASSERT_NOT_EQUALS(KErrNone ,data->ExecuteIntent(EView) );
    STIF_ASSERT_NOT_EQUALS(KErrNone ,data->ExecuteIntent(EPrint) );
    STIF_ASSERT_NOT_EQUALS(KErrNone ,data->ExecuteIntent(EExecute) );
    STIF_ASSERT_EQUALS(KErrNone ,data->ExecuteIntent(EPause) );
    STIF_ASSERT_EQUALS(KErrNone ,data->ExecuteIntent(EStop) );
    STIF_ASSERT_EQUALS(KErrNone ,data->ExecuteIntent(EContinue) );
    delete data;



    fileName.Copy(KEncryptedDir);
    fileName.Append(KTempDcfName);
    r = iFs.Delete(fileName);
    STIF_ASSERT_EQUALS(KErrNone ,r );
    supplier = CSupplier::NewLC();
    metaData = CMetaDataArray::NewLC();
    metaData->AddL(KOmaImportMimeTypeField, _L8("image/jpeg"));
    supplier->SetOutputDirectoryL(KEncryptedDir);
    file = supplier->ImportFileL(KOmaImportContentType, *metaData, KTempDcfName);
    STIF_ASSERT_NOT_NULL( file );
    CleanupStack::PushL(file);
    for (i = 0; i < 10; i++)
        {
        RUNCHECKEDL(file->WriteData(KDMContent));
        }
    RUNCHECKEDL(file->WriteDataComplete());
    STIF_ASSERT_EQUALS(1 ,file->OutputFileCountL() );
    CleanupStack::PopAndDestroy(3);



    data = CData::NewL(TVirtualPathPtr(fileName, KDefaultContentObject),
        EPeek, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );

    STIF_ASSERT_NOT_EQUALS( KErrNone, data->ExecuteIntent(EPlay));
    r = data->ExecuteIntent(EView);
    iLog->Log(_L("    Execute intent returned: %d \r\n"), r);
    if(r == -17452 || r == -17451 || r == -21 )
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);

    r1 = EFalse;
    r = data->ExecuteIntent(EPrint);
    iLog->Log(_L("    Execute intent returned: %d \r\n"), r);
    if(r == -17452 || r == -17451 || r == -21 )
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);

    STIF_ASSERT_NOT_EQUALS( KErrNone, data->ExecuteIntent(EExecute));
    delete data;



    fileName.Copy(KEncryptedDir);
    fileName.Append(KTempDcfName);
    r = iFs.Delete(fileName);
    STIF_ASSERT_EQUALS( KErrNone, r);
    supplier = CSupplier::NewLC();
    metaData = CMetaDataArray::NewLC();
    metaData->AddL(KOmaImportMimeTypeField, _L8("audio/mpeg"));
    supplier->SetOutputDirectoryL(KEncryptedDir);
    file = supplier->ImportFileL(KOmaImportContentType, *metaData, KTempDcfName);
    STIF_ASSERT_NOT_NULL( file );
    CleanupStack::PushL(file);
    for (i = 0; i < 10; i++)
        {
        RUNCHECKEDL(file->WriteData(KDMContent));
        }
    RUNCHECKEDL(file->WriteDataComplete());
    STIF_ASSERT_EQUALS( 1, file->OutputFileCountL());
    CleanupStack::PopAndDestroy(3);



    data = CData::NewL(TVirtualPathPtr(fileName, KDefaultContentObject),
        EPeek, EContentShareReadOnly);

    STIF_ASSERT_NOT_NULL( data );

    r1 = EFalse;
    r = data->ExecuteIntent(EPlay);
    iLog->Log(_L("    Execute intent returned: %d \r\n"), r);
    if(r == -17452 || r == -17451 || r == -21 )
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);

    STIF_ASSERT_NOT_EQUALS( KErrNone, data->ExecuteIntent(EView));


    STIF_ASSERT_NOT_EQUALS( KErrNone, data->ExecuteIntent(EPrint));


    STIF_ASSERT_NOT_EQUALS( KErrNone, data->ExecuteIntent(EExecute));

    delete data;

    return KErrNone;
    }

TInt  CDRM_CAF::CData_GetAttributeL()
    {
    CData* data = NULL;
    TVirtualPathPtr dcf1(KOma1Content, KDefaultContentObject);
    TVirtualPathPtr dcf2(KOma2Content, KDefaultContentObject);
    TVirtualPathPtr dcf3(KOma2ContentNoEnc, KDefaultContentObject);
    TInt value;
    TInt i;


    data = CData::NewLC(dcf1, EPeek, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    RUNCHECKEDL(data->GetAttribute(EIsProtected, value));
    STIF_ASSERT_EQUALS( 1, value);
    RUNCHECKEDL(data->GetAttribute(EFileType, value));
    STIF_ASSERT_EQUALS( 1, value);
    for (i = 0; i < NUM_ELEMENTS(KAttributes); i++)
        {
        data->GetAttribute(KAttributes[i], value);
        }
    CleanupStack::PopAndDestroy();



    data = CData::NewLC(dcf2, EPeek, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    RUNCHECKEDL(data->GetAttribute(EIsProtected, value));
    STIF_ASSERT_EQUALS( 1, value);
    RUNCHECKEDL(data->GetAttribute(EFileType, value));
    STIF_ASSERT_EQUALS( 2, value);
    for (i = 0; i < NUM_ELEMENTS(KAttributes); i++)
        {
        data->GetAttribute(KAttributes[i], value);
        }
    CleanupStack::PopAndDestroy();



    data = CData::NewLC(dcf3, EPeek, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    RUNCHECKEDL(data->GetAttribute(EIsProtected, value));
    STIF_ASSERT_EQUALS( 0, value);
    RUNCHECKEDL(data->GetAttribute(EFileType, value));
    STIF_ASSERT_EQUALS( 2, value);
    for (i = 0; i < NUM_ELEMENTS(KAttributes); i++)
        {
        data->GetAttribute(KAttributes[i], value);
        }
    CleanupStack::PopAndDestroy();

    return KErrNone;
    }

TInt  CDRM_CAF::CData_GetAttributeSetL()
    {
    CData* data = NULL;
    TVirtualPathPtr dcf1(KOma1Content, KDefaultContentObject);
    TVirtualPathPtr dcf2(KOma2Content, KDefaultContentObject);
    RAttributeSet attributes;
    TInt value;

    attributes.AddL(EIsProtected);
    attributes.AddL(EFileType);

    data = CData::NewLC(dcf1, EPeek, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    RUNCHECKEDL(data->GetAttributeSet(attributes));
    RUNCHECKEDL(attributes.GetValue(EIsProtected, value));
    STIF_ASSERT_EQUALS( 1, value);
    RUNCHECKED(attributes.GetValue(EFileType, value));
    STIF_ASSERT_EQUALS( 1, value);
    CleanupStack::PopAndDestroy();



    data = CData::NewLC(dcf2, EPeek, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    RUNCHECKEDL(data->GetAttributeSet(attributes));
    RUNCHECKEDL(attributes.GetValue(EIsProtected, value));
    STIF_ASSERT_EQUALS( 1, value);
    RUNCHECKED(attributes.GetValue(EFileType, value));
    STIF_ASSERT_EQUALS( 2, value);
    CleanupStack::PopAndDestroy();

    attributes.Close();
    return KErrNone;
    }

TInt  CDRM_CAF::CData_GetStringAttributeL()
    {
    TVirtualPathPtr dcf1(KOma1Content, KDefaultContentObject);
    TVirtualPathPtr dcf2(KOma2Content, KDefaultContentObject);
    TVirtualPathPtr dcf3(KOma2Content, KDefaultContentObject);
    CData* data = NULL;
    HBufC* buffer = HBufC::NewLC(256000);
    TPtr value(NULL, 0);
    TInt i;

    value.Set(buffer->Des());

    data = CData::NewLC(dcf1, EPeek, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    RUNCHECKEDL(data->GetStringAttribute(EMimeType, value));
    STIF_ASSERT_EQUALS( 0, value.Compare(_L("text/plain")));
    for (i = 0; i < NUM_ELEMENTS(KStringAttributes); i++)
        {
        data->GetStringAttribute(KStringAttributes[i], value);
        }
    CleanupStack::PopAndDestroy();



    data = CData::NewLC(dcf2, EPeek, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    RUNCHECKEDL(data->GetStringAttribute(EMimeType, value));
    STIF_ASSERT_EQUALS( 0, value.Compare(_L("audio/wav")));
    for (i = 0; i < NUM_ELEMENTS(KStringAttributes); i++)
        {
        data->GetStringAttribute(KStringAttributes[i], value);
        }
    CleanupStack::PopAndDestroy();



    data = CData::NewLC(dcf3, EPeek, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    RUNCHECKEDL(data->GetStringAttribute(EMimeType, value));
    STIF_ASSERT_EQUALS( 0, value.Compare(_L("audio/wav")));
    for (i = 0; i < NUM_ELEMENTS(KStringAttributes); i++)
        {
        data->GetStringAttribute(KStringAttributes[i], value);
        }
    CleanupStack::PopAndDestroy();

    CleanupStack::PopAndDestroy();
    return KErrNone;
    }

TInt  CDRM_CAF::CData_GetStringAttributeSetL()
    {
    RFile fileFLV;

    TInt err = fileFLV.Open(iFs,KFVariantFile, EFileRead);
    if( err == 0 )
    {
        iLog->Log(_L("    Skipped in FL variant \r\n" ));
        fileFLV.Close();
        return KErrNone;
    }
    fileFLV.Close();

    TVirtualPathPtr dcf1(KOma1Content, KDefaultContentObject);
    TVirtualPathPtr dcf2(KOma2Content, KDefaultContentObject);
    CData* data = NULL;
    RStringAttributeSet attributes;
    HBufC* buffer = HBufC::NewLC(256000);
    TPtr value(NULL, 0);
    TBool r1 = EFalse;
    value.Set(buffer->Des());
    attributes.AddL(EMimeType);
    attributes.AddL(ERightsIssuerUrl);
    data = CData::NewLC(dcf1, EPeek, EContentShareReadOnly);
    //CHECKL(data != NULL);
    STIF_ASSERT_NOT_NULL( data );
    RUNCHECKEDL(data->GetStringAttributeSet(attributes));
    RUNCHECKEDL(attributes.GetValue(EMimeType, value));
    STIF_ASSERT_EQUALS(0 , value.Compare(_L("text/plain")));
    STIF_ASSERT_EQUALS(KErrCANotSupported , attributes.GetValue(ERightsIssuerUrl, value));
    CleanupStack::PopAndDestroy();

    data = CData::NewLC(dcf2, EPeek, EContentShareReadOnly);
    STIF_ASSERT_NOT_NULL( data );
    RUNCHECKEDL(data->GetStringAttributeSet(attributes));
    RUNCHECKEDL(attributes.GetValue(EMimeType, value));
    STIF_ASSERT_EQUALS( 0, value.Compare(_L("audio/wav")));
    TInt rr = attributes.GetValue(ERightsIssuerUrl, value);
    if(rr == KErrCANotSupported || rr == KErrNone)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);
    r1 = EFalse;
    if(value.Length() > 0)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);
    CleanupStack::PopAndDestroy();
    CleanupStack::PopAndDestroy();
    attributes.Close();
    return KErrNone;
    }
TInt  CDRM_CAF::CContent_NewL()
    {
    CContent* content = NULL;
    RFile file;


    content = CContent::NewL(KOma1Content);
    STIF_ASSERT_NOT_NULL( content );
    delete content;



    content = CContent::NewL(KOma2Content);
    STIF_ASSERT_NOT_NULL( content );
    delete content;



    content = CContent::NewL(KOma2ContentNoEnc);
    STIF_ASSERT_NOT_NULL( content );
    delete content;



    TInt err = file.Open(iFs, KOma1Content, EFileRead);
    CleanupClosePushL(file);
    content = CContent::NewL(file);
    STIF_ASSERT_NOT_NULL( content );
    delete content;
    CleanupStack::PopAndDestroy();



    err = file.Open(iFs, KOma2Content, EFileRead);
    CleanupClosePushL(file);
    content = CContent::NewL(file);
    STIF_ASSERT_NOT_NULL( content );
    delete content;
    CleanupStack::PopAndDestroy();



    err = file.Open(iFs, KOma2ContentNoEnc, EFileRead);
    err = err;
    CleanupClosePushL(file);
    content = CContent::NewL(file);
    STIF_ASSERT_NOT_NULL( content );
    delete content;
    CleanupStack::PopAndDestroy();

    return KErrNone;
    }

TInt  CDRM_CAF::CContent_OpenCloseContainerL()
    {
    CContent* content = NULL;
    TInt r;


    content = CContent::NewLC(KOma1Content);
    STIF_ASSERT_NOT_NULL( content );
    RUNCHECKED(content->OpenContainer(KDefaultContentObject));
    RUNCHECKED(content->CloseContainer());

    RUNCHECKED(content->OpenContainer(KDefaultContentObject));
    r = content->OpenContainer(KDefaultContentObject);
    STIF_ASSERT_EQUALS( KErrNotFound, r);
    RUNCHECKED(content->CloseContainer());

    r = content->OpenContainer(_L("xyz"));
    STIF_ASSERT_EQUALS( KErrNotFound, r);

    r = content->CloseContainer();
    STIF_ASSERT_EQUALS( KErrNotFound, r);

    CleanupStack::PopAndDestroy();

    return KErrNone;
    }

TInt  CDRM_CAF::CContent_GetEmbeddedObjectsL()
    {
    CContent* content = NULL;
    RStreamablePtrArray<CEmbeddedObject> objects;


    content = CContent::NewLC(KOma1Content);
    STIF_ASSERT_NOT_NULL( content );
    content->GetEmbeddedObjectsL(objects);
    STIF_ASSERT_EQUALS( 1, objects.Count());
    objects.ResetAndDestroy();

    content->GetEmbeddedObjectsL(objects, EContainerObject);
    STIF_ASSERT_EQUALS( 0, objects.Count());

    content->GetEmbeddedObjectsL(objects, EAgentSpecificObject);
    STIF_ASSERT_EQUALS( 0, objects.Count());

    content->GetEmbeddedObjectsL(objects, EContentObject);
    STIF_ASSERT_EQUALS( 1, objects.Count());
    objects.ResetAndDestroy();
    objects.Close();
    CleanupStack::PopAndDestroy();

    content = CContent::NewLC(KOma2Content);
    STIF_ASSERT_NOT_NULL( content );
    content->GetEmbeddedObjectsL(objects);
    STIF_ASSERT_EQUALS( 1, objects.Count());
    objects.ResetAndDestroy();

    content->GetEmbeddedObjectsL(objects, EContainerObject);
    STIF_ASSERT_EQUALS( 0, objects.Count());

    content->GetEmbeddedObjectsL(objects, EAgentSpecificObject);
    STIF_ASSERT_EQUALS( 0, objects.Count());

    content->GetEmbeddedObjectsL(objects, EContentObject);
    STIF_ASSERT_EQUALS( 1, objects.Count());
    objects.ResetAndDestroy();
    objects.Close();

    RUNCHECKEDL(content->OpenContainer(KDefaultContentObject))
    content->GetEmbeddedObjectsL(objects, EContentObject);
    STIF_ASSERT_EQUALS( 0, objects.Count());
    objects.Close();
    RUNCHECKED(content->CloseContainer())
    CleanupStack::PopAndDestroy();

    return KErrNone;
    }

TInt  CDRM_CAF::CContent_SearchL()
    {
    CContent* content = NULL;
    RStreamablePtrArray<CEmbeddedObject> objects;


    content = CContent::NewLC(KOma1Content);
    STIF_ASSERT_NOT_NULL( content );

    RUNCHECKEDL(content->Search(objects, _L8("text/plain"), EFalse));
    STIF_ASSERT_EQUALS( 1, objects.Count());
    objects.ResetAndDestroy();

    RUNCHECKEDL(content->Search(objects, _L8("audio/wav"), EFalse));
    STIF_ASSERT_EQUALS( 0, objects.Count());
    objects.ResetAndDestroy();

    objects.Close();
    CleanupStack::PopAndDestroy();



    content = CContent::NewLC(KOma2Content);
    STIF_ASSERT_NOT_NULL( content );

    RUNCHECKEDL(content->Search(objects, _L8("text/plain"), EFalse));
    STIF_ASSERT_EQUALS( 0, objects.Count());
    objects.ResetAndDestroy();

    RUNCHECKEDL(content->Search(objects, _L8("audio/wav"), EFalse));
    STIF_ASSERT_EQUALS( 1, objects.Count());
    objects.ResetAndDestroy();
    objects.Close();

    RUNCHECKEDL(content->OpenContainer(KDefaultContentObject))
    RUNCHECKEDL(content->Search(objects, _L8("text/plain"), EFalse));
    STIF_ASSERT_EQUALS( 0, objects.Count());
    objects.Close();
    RUNCHECKED(content->CloseContainer());

    CleanupStack::PopAndDestroy();

    return KErrNone;
    }

TInt  CDRM_CAF::CContent_GetAttributeL()
    {
    CContent* content = NULL;
    TInt value;
    TInt i;
    TInt re;
    TBool r1 = EFalse;

    content = CContent::NewLC(KOma1Content);
    STIF_ASSERT_NOT_NULL( content );
    RUNCHECKEDL(content->GetAttribute(EIsProtected, value));
    STIF_ASSERT_EQUALS( 1, value);
    RUNCHECKEDL(content->GetAttribute(EFileType, value));
    STIF_ASSERT_EQUALS( 1, value);
    re = content->GetAttribute(ECanPlay, value);
    if(re == KErrCANotSupported || re == KErrNone)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);
    r1 = EFalse;
    re = content->GetAttribute(ECanPlay, value);
    if(re == KErrCANotSupported || re == KErrNone)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);
    r1 = EFalse;
    re = content->GetAttribute(ECanView, value);
    if(re == KErrCANotSupported || re == KErrNone)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);
    r1 = EFalse;
    re = content->GetAttribute(ECanExecute, value);
    if(re == KErrCANotSupported || re == KErrNone)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);
    r1 = EFalse;
    re = content->GetAttribute(ECanPrint, value);
    if(re == KErrCANotSupported || re == KErrNone)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);

    for (i = 0; i < NUM_ELEMENTS(KAttributes); i++)
        {
        content->GetAttribute(KAttributes[i], value);
        }
    STIF_ASSERT_NOT_EQUALS( KErrNone, content->GetAttribute(EIsProtected, value, _L("xyz")));
    CleanupStack::PopAndDestroy();



    content = CContent::NewLC(KOma2Content);
    STIF_ASSERT_NOT_NULL( content );
    RUNCHECKEDL(content->GetAttribute(EIsProtected, value));
    STIF_ASSERT_EQUALS( 1, value);
    RUNCHECKEDL(content->GetAttribute(EFileType, value));
    STIF_ASSERT_EQUALS( 2, value);
    for (i = 0; i < NUM_ELEMENTS(KAttributes); i++)
        {
        content->GetAttribute(KAttributes[i], value);
        }
    STIF_ASSERT_NOT_EQUALS( KErrNone, content->GetAttribute(EIsProtected, value, _L("xyz")));
    CleanupStack::PopAndDestroy();

    return KErrNone;
    }

TInt  CDRM_CAF::CContent_GetAttributeSetL()
    {
    CContent* content = NULL;
    RAttributeSet attributes;
    TInt value;

    attributes.AddL(EIsProtected);
    attributes.AddL(EFileType);

    content = CContent::NewLC(KOma1Content);
    STIF_ASSERT_NOT_NULL( content );
    RUNCHECKEDL(content->GetAttributeSet(attributes));
    RUNCHECKEDL(attributes.GetValue(EIsProtected, value));
    STIF_ASSERT_EQUALS( 1, value);
    RUNCHECKED(attributes.GetValue(EFileType, value));
    STIF_ASSERT_EQUALS( 1, value);
    CleanupStack::PopAndDestroy();



    content = CContent::NewLC(KOma2Content);
    STIF_ASSERT_NOT_NULL( content );
    RUNCHECKEDL(content->GetAttributeSet(attributes));
    RUNCHECKEDL(attributes.GetValue(EIsProtected, value));
    STIF_ASSERT_EQUALS( 1, value);
    RUNCHECKED(attributes.GetValue(EFileType, value));
    STIF_ASSERT_EQUALS( 2, value);
    CleanupStack::PopAndDestroy();

    attributes.Close();
    return KErrNone;
    }

TInt  CDRM_CAF::CContent_GetStringAttributeL()
    {
    CContent* content = NULL;
    RStreamablePtrArray<CEmbeddedObject> objects;
    HBufC* buffer = HBufC::NewLC(256000);
    TPtr value(NULL, 0);
    TInt i;

    value.Set(buffer->Des());

    content = CContent::NewLC(KOma1Content);
    STIF_ASSERT_NOT_NULL( content );
    RUNCHECKEDL(content->GetStringAttribute(EMimeType, value));
    STIF_ASSERT_EQUALS( 0, value.Compare(_L("text/plain")));
    for (i = 0; i < NUM_ELEMENTS(KStringAttributes); i++)
        {
        content->GetStringAttribute(KStringAttributes[i], value);
        }
    STIF_ASSERT_NOT_EQUALS(KErrNone, content->GetStringAttribute(EMimeType, value, _L("xyz")));
    CleanupStack::PopAndDestroy();



    content = CContent::NewLC(KOma2Content);
    STIF_ASSERT_NOT_NULL( content );
    RUNCHECKEDL(content->Search(objects, _L8("audio/wav"), EFalse));
    STIF_ASSERT_EQUALS( 1, objects.Count());
    RUNCHECKEDL(content->GetStringAttribute(EMimeType, value,
        objects[0]->UniqueId()));
    STIF_ASSERT_EQUALS( 0, value.Compare(_L("audio/wav")));
    objects.ResetAndDestroy();
    objects.Close();
    for (i = 0; i < NUM_ELEMENTS(KStringAttributes); i++)
        {
        content->GetStringAttribute(KStringAttributes[i], value);
        }
    STIF_ASSERT_NOT_EQUALS( KErrNone,content->GetStringAttribute(EMimeType, value, _L("xyz")));
    CleanupStack::PopAndDestroy();

    CleanupStack::PopAndDestroy();
    return KErrNone;
    }

TInt  CDRM_CAF::CContent_GetStringAttributeSetL()
    {
    RFile fileFLV;

    TInt err = fileFLV.Open(iFs,KFVariantFile, EFileRead);
    if( err == 0 )
    {
        iLog->Log(_L("    Skipped in FL variant \r\n" ));
        fileFLV.Close();
        TFileName fileFlVariant;
        fileFlVariant.Copy(KFVariantFile);
        // Clearing file after use
            iFs.Delete(fileFlVariant);
        return KErrNone;
    }
    fileFLV.Close();

    CContent* content = NULL;
    RStringAttributeSet attributes;
    HBufC* buffer = HBufC::NewLC(256000);
    TPtr value(NULL, 0);
    TInt re;
    TBool r1 = EFalse;
    value.Set(buffer->Des());
    attributes.AddL(EMimeType);
    attributes.AddL(ERightsIssuerUrl);

    content = CContent::NewLC(KOma1Content);
    STIF_ASSERT_NOT_NULL( content );
    RUNCHECKEDL(content->GetStringAttributeSet(attributes));
    RUNCHECKEDL(attributes.GetValue(EMimeType, value));
    STIF_ASSERT_EQUALS( 0,value.Compare(_L("text/plain")));
    re = attributes.GetValue(ERightsIssuerUrl, value);
    if(re == KErrCANotSupported || re == KErrNone)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);
    CleanupStack::PopAndDestroy();



    content = CContent::NewLC(KOma2Content);
    STIF_ASSERT_NOT_NULL( content );
    RUNCHECKEDL(content->GetStringAttributeSet(attributes));
    RUNCHECKEDL(attributes.GetValue(EMimeType, value));
    STIF_ASSERT_EQUALS( 0,value.Compare(_L("audio/wav")));
    RUNCHECKED(attributes.GetValue(ERightsIssuerUrl, value));
    if(value.Length() > 0)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);
    CleanupStack::PopAndDestroy();

    attributes.Close();
    CleanupStack::PopAndDestroy();
    return KErrNone;
    }

TInt  CDRM_CAF::CContent_AgentSpecificCommandL()
    {
    CContent* content = NULL;
    TPtr8 ptr(NULL, 0, 0);
    TRequestStatus status;


    content = CContent::NewLC(KOma2Content);
    STIF_ASSERT_NOT_NULL( content );
    RUNCHECKED(content->AgentSpecificCommand(EEmbedDomainRo, KNullDesC8, ptr));
    content->AgentSpecificCommand(EEmbedDomainRo, KNullDesC8, ptr, status);
    User::WaitForRequest(status);
    CleanupStack::PopAndDestroy();

    content = CContent::NewLC(KOma1Content);
    STIF_ASSERT_NOT_NULL( content );
    STIF_ASSERT_EQUALS( KErrArgument,content->AgentSpecificCommand(EEmbedDomainRo, KNullDesC8, ptr));
    CleanupStack::PopAndDestroy();

    return KErrNone;
    }
TInt  CDRM_CAF::CContent_NotifyStatusChangeL()
    {
    _LIT(KSdkVersion31,"Z:\\System\\install\\Series60v3.1.sis");
    _LIT(KSdkVersion50,"Z:\\System\\install\\Series60v5.0.sis");
    RFs fs;
    TBool found = EFalse;
    User::LeaveIfError(fs.Connect ());
    CleanupClosePushL (fs);
    if(BaflUtils::FileExists (fs, KSdkVersion31))
        {
        found = ETrue;
        }
    else if(BaflUtils::FileExists (fs, KSdkVersion50))
        {
        found = ETrue;
        }
    CContent* content = NULL;
    TRequestStatus status = KRequestPending;
    content = CContent::NewLC(KOma2Content);
    STIF_ASSERT_NOT_NULL(content);
    content->NotifyStatusChange(ERightsAvailable, status);//ERightsAvailable
    if(found)
    	{
    	User::After(10000);
			content->CancelNotifyStatusChange(status);
    	User::WaitForRequest(status);
   		}
   else
   		{
   	  User::WaitForRequest(status);
  		}
    CleanupStack::PopAndDestroy(2);
    return KErrNone;
    }

TInt  CDRM_CAF::CContent_CancelNotifyStatusChangeL()
    {
    CContent* content = NULL;
    TRequestStatus status;
    content = CContent::NewLC(KOma2Content);
    STIF_ASSERT_NOT_NULL(content);
    content->CancelNotifyStatusChange(status);
    User::WaitForRequest(status);
    //r = status;
    //STIF_ASSERT_EQUALS( KErrCANotSupported,r);
    CleanupStack::PopAndDestroy();
    return KErrNone;
    }

TInt  CDRM_CAF::CContent_RequestRightsL()
    {
    CContent* content = NULL;
    TRequestStatus status;
    content = CContent::NewLC(KOma2Content);
    STIF_ASSERT_NOT_NULL(content);
    content->RequestRights(status);
    User::WaitForRequest(status);
    //r = status;
#ifdef __SERIES60_
    //STIF_ASSERT_EQUALS( KErrNone,r);
#else
    //STIF_ASSERT_EQUALS( KErrCANotSupported,r);
#endif
    CleanupStack::PopAndDestroy(content);
    return KErrNone;
    }

TInt  CDRM_CAF::CContent_DisplayInfoL()
    {
    CContent* content = NULL;
    TInt r;
    TBool r1 = EFalse;


    content = CContent::NewLC(KOma2Content);
    STIF_ASSERT_NOT_NULL( content );
    TRAP(r, content->DisplayInfoL(EFileAndRights));
    if(r == KErrCANotSupported || r == KErrNone)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);
    CleanupStack::PopAndDestroy();

    return KErrNone;
    }

TInt  CDRM_CAF::CContent_SetPropertyL()
    {
    CContent* content = NULL;
    TInt r;
    TBool r1 = EFalse;

    content = CContent::NewLC(KOma2Content);
    STIF_ASSERT_NOT_NULL( content );
    r = content->SetProperty(EAgentPropertyBufferSize, 1024);
    if(r == KErrCANotSupported || r == KErrNone)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);
    r = content->SetProperty(EAgentPropertyAgentUI, 1);
    if(r == KErrCANotSupported || r == KErrNone)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);

    r = content->SetProperty(EAgentPropertyMultipleSequence, 1);
    if(r == KErrCANotSupported || r == KErrNone)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);
    CleanupStack::PopAndDestroy();

    return KErrNone;
    }

TInt  CDRM_CAF::CContent_OpenContentL()
    {
    CContent* content = NULL;
    CData* data = NULL;
    RStreamablePtrArray<CEmbeddedObject> objects;


    content = CContent::NewLC(KOma1Content);
    STIF_ASSERT_NOT_NULL( content );
    data = content->OpenContentLC(EPeek);
    STIF_ASSERT_NOT_NULL( data );
    CleanupStack::PopAndDestroy(2);



    content = CContent::NewLC(KOma2Content);
    STIF_ASSERT_NOT_NULL( content );
    RUNCHECKEDL(content->Search(objects, _L8("audio/wav"), EFalse));
    STIF_ASSERT_EQUALS( 1,objects.Count());
    data = content->OpenContentLC(EPeek, objects[0]->UniqueId());
    STIF_ASSERT_NOT_NULL( data );
    objects.ResetAndDestroy();
    CleanupStack::PopAndDestroy(2);

    return KErrNone;
    }

TInt CDRM_CAF::CManager_FileOperationsL()
    {
    _LIT(KFile1, "c:\\private\\OMA DRM Agent\\file1.odf");
    _LIT(KFile2, "c:\\private\\OMA DRM Agent\\file2.odf");
    _LIT(KDir, "c:\\private\\OMA DRM Agent\\");
    _LIT(KDir1, "c:\\private\\OMA DRM Agent\\dir1\\");
    _LIT(KDir12, "c:\\private\\OMA DRM Agent\\dir1\\dir2\\");
    CDir* dir;
    TInt r;


    CManager* manager = CManager::NewL();
    STIF_ASSERT_NOT_NULL( manager );
    r = manager->CopyFile(KOma2Content, KFile1);
    STIF_ASSERT_EQUALS( KErrPermissionDenied,r);
    r = manager->RenameFile(KFile1, KFile2);
    STIF_ASSERT_EQUALS( KErrPermissionDenied,r);
    r = manager->DeleteFile(KFile2);
    STIF_ASSERT_EQUALS( KErrPermissionDenied,r);
    r = manager->MkDirAll(KDir12);
    STIF_ASSERT_EQUALS( KErrPermissionDenied,r);
    r = manager->MkDir(KDir1);
    STIF_ASSERT_EQUALS( KErrPermissionDenied,r);
    r = manager->GetDir(KDir, KEntryAttNormal, ESortByName, dir);
    STIF_ASSERT_EQUALS( KErrPermissionDenied,r);
    r = manager->GetDir(KDir, KNullUid, ESortByName, dir);
    STIF_ASSERT_EQUALS( KErrPermissionDenied,r);
    r = manager->GetDir(KDir, KEntryAttNormal, ESortByName, dir, dir);
    STIF_ASSERT_EQUALS( KErrPermissionDenied,r);
    r = manager->RmDir(KDir12);
    STIF_ASSERT_EQUALS( KErrPermissionDenied,r);
    //CleanupStack::PopAndDestroy();

    return KErrNone;
    }

TInt CDRM_CAF::CManager_GetAttributeL()
    {
    TVirtualPathPtr dcf1(KOma1Content, KNullDesC);
    TVirtualPathPtr dcf2(KOma2Content, KNullDesC);
    TInt value;
    TInt i;
    RFile dcf1file;
    RFile dcf2file;
    TInt error( KErrNone );

    CManager* manager = CManager::NewLC();
    STIF_ASSERT_NOT_NULL( manager );
    RUNCHECKEDL(manager->GetAttribute(EIsProtected, value, dcf1));
    STIF_ASSERT_EQUALS( 0,value);
    RUNCHECKEDL(manager->GetAttribute(EFileType, value, dcf1));
    STIF_ASSERT_EQUALS( 1,value);
    for (i = 0; i < NUM_ELEMENTS(KAttributes); i++)
        {
        manager->GetAttribute(KAttributes[i], value, dcf1);
        }
    CleanupStack::PopAndDestroy( manager );



    manager = CManager::NewLC();
    STIF_ASSERT_NOT_NULL( manager );
    RUNCHECKEDL(manager->GetAttribute(EIsProtected, value, dcf2));
    STIF_ASSERT_EQUALS( 0,value);
    RUNCHECKEDL(manager->GetAttribute(EFileType, value, dcf2));
    STIF_ASSERT_EQUALS( 2,value);
    for (i = 0; i < NUM_ELEMENTS(KAttributes); i++)
        {
        manager->GetAttribute(KAttributes[i], value, dcf2);
        }
    CleanupStack::PopAndDestroy( manager );

    User::LeaveIfError(dcf1file.Open(iFs, KOma1Content, EFileRead));
    CleanupClosePushL(dcf1file);
    User::LeaveIfError(dcf2file.Open(iFs, KOma2Content, EFileRead));
    CleanupClosePushL(dcf2file);

    manager = CManager::NewLC();
    STIF_ASSERT_NOT_NULL( manager );
    error = manager->GetAttribute(EIsProtected, value, dcf1file, KDefaultContentObject);
    if ( !error )
        {
        STIF_ASSERT_EQUALS( 0, value );
        }
    else
        {
        STIF_ASSERT_EQUALS( KErrCANotSupported, error );
        }

    error = manager->GetAttribute(EFileType, value, dcf1file, KDefaultContentObject);
    if ( !error )
        {
        STIF_ASSERT_EQUALS( 1, value );
        }
    else
        {
        STIF_ASSERT_EQUALS( KErrCANotSupported, error );
        }

    for (i = 0; i < NUM_ELEMENTS(KAttributes); i++)
        {
        manager->GetAttribute(KAttributes[i], value, dcf1file, KDefaultContentObject);
        }
    CleanupStack::PopAndDestroy( manager );

    manager = CManager::NewLC();
    STIF_ASSERT_NOT_NULL( manager );
    error = manager->GetAttribute(EIsProtected, value, dcf2file, KDefaultContentObject);
    if ( !error )
        {
        STIF_ASSERT_EQUALS( 0, value );
        }
    else
        {
        STIF_ASSERT_EQUALS( KErrCANotSupported, error );
        }

    error = manager->GetAttribute(EFileType, value, dcf2file, KDefaultContentObject);
    if ( !error )
        {
        STIF_ASSERT_EQUALS( 2, value );
        }
    else
        {
        STIF_ASSERT_EQUALS( KErrCANotSupported, error );
        }

    for (i = 0; i < NUM_ELEMENTS(KAttributes); i++)
        {
        manager->GetAttribute(KAttributes[i], value, dcf2file, KDefaultContentObject);
        }
    CleanupStack::PopAndDestroy( manager );

    // close files
    CleanupStack::PopAndDestroy( 2 );

    return KErrNone;
    }

TInt CDRM_CAF::CManager_GetAttributeSetL()
    {
    RAttributeSet attributes;
    TInt value;
    TVirtualPathPtr dcf2(KOma2Content, KNullDesC);
    TInt error( KErrNone );
    RFile dcf2file;

    attributes.AddL(EIsProtected);
    attributes.AddL(EFileType);
    CleanupClosePushL(attributes);
    CManager* manager = CManager::NewLC();
    STIF_ASSERT_NOT_NULL( manager );
    RUNCHECKEDL(manager->GetAttributeSet(attributes, dcf2));
    RUNCHECKEDL(attributes.GetValue(EIsProtected, value));
    STIF_ASSERT_EQUALS( 0,value);
    RUNCHECKED(attributes.GetValue(EFileType, value));
    STIF_ASSERT_EQUALS( 2,value);

    User::LeaveIfError(dcf2file.Open(iFs, KOma2Content, EFileRead));
    CleanupClosePushL(dcf2file);

    error = manager->GetAttributeSet(attributes, dcf2file, KDefaultContentObject);
    if ( !error )
        {
        RUNCHECKEDL(attributes.GetValue(EIsProtected, value));
        STIF_ASSERT_EQUALS( 0,value);
        RUNCHECKED(attributes.GetValue(EFileType, value));
        STIF_ASSERT_EQUALS( 2,value);
        }
    else
        {
        STIF_ASSERT_EQUALS( KErrCANotSupported, error );
        }

    CleanupStack::PopAndDestroy(3);

    return KErrNone;
    }

TInt CDRM_CAF::CManager_GetStringAttributeL()
    {
    TBuf<256> value;
    TVirtualPathPtr dcf1(KOma1Content, KNullDesC);
    TVirtualPathPtr dcf2(KOma2Content, KNullDesC);
    TInt i;
    TInt error( KErrNone );
    RFile dcf1file;
    RFile dcf2file;

    CManager* manager = CManager::NewLC();
    STIF_ASSERT_NOT_NULL( manager );
    RUNCHECKED(manager->GetStringAttribute(EMimeType, value, dcf1));
    for (i = 0; i < NUM_ELEMENTS(KAttributes); i++)
        {
        manager->GetStringAttribute(KStringAttributes[i], value, dcf1);
        }
    CleanupStack::PopAndDestroy( manager );



    manager = CManager::NewLC();
    STIF_ASSERT_NOT_NULL( manager );
    RUNCHECKED(manager->GetStringAttribute(EMimeType, value, dcf2));
    for (i = 0; i < NUM_ELEMENTS(KAttributes); i++)
        {
        manager->GetStringAttribute(KStringAttributes[i], value, dcf2);
        }
    CleanupStack::PopAndDestroy( manager );

    User::LeaveIfError(dcf1file.Open(iFs, KOma1Content, EFileRead));
    CleanupClosePushL(dcf1file);
    User::LeaveIfError(dcf2file.Open(iFs, KOma2Content, EFileRead));
    CleanupClosePushL(dcf2file);

    manager = CManager::NewLC();
    STIF_ASSERT_NOT_NULL( manager );
    error = manager->GetStringAttribute(EMimeType, value, dcf1file, KDefaultContentObject);
    if ( error == KErrCANotSupported )
        {
        error = KErrNone;
        }
    STIF_ASSERT_EQUALS( KErrNone, error );
    for (i = 0; i < NUM_ELEMENTS(KAttributes); i++)
        {
        manager->GetStringAttribute(KStringAttributes[i], value, dcf1file, KDefaultContentObject);
        }
    CleanupStack::PopAndDestroy( manager );



    manager = CManager::NewLC();
    STIF_ASSERT_NOT_NULL( manager );
    error = manager->GetStringAttribute(EMimeType, value, dcf2file, KDefaultContentObject);
    if ( error == KErrCANotSupported )
        {
        error = KErrNone;
        }
    STIF_ASSERT_EQUALS( KErrNone, error );
    for (i = 0; i < NUM_ELEMENTS(KAttributes); i++)
        {
        manager->GetStringAttribute(KStringAttributes[i], value, dcf2file, KDefaultContentObject);
        }
    CleanupStack::PopAndDestroy( manager );

    // close files
    CleanupStack::PopAndDestroy( 2 );

    return KErrNone;
    }

TInt CDRM_CAF::CManager_GetStringAttributeSetL()
    {
    RStringAttributeSet attributes;
    TBuf<256> value;
    TVirtualPathPtr dcf2(KOma2Content, KNullDesC);
    RFile dcf2file;
    TInt error( KErrNone );

    attributes.AddL(EMimeType);
    attributes.AddL(ERightsIssuerUrl);
    CManager* manager = CManager::NewLC();
    STIF_ASSERT_NOT_NULL( manager );
    RUNCHECKEDL(manager->GetStringAttributeSet(attributes, dcf2));
    RUNCHECKED(attributes.GetValue(EMimeType, value));
    STIF_ASSERT_EQUALS( KErrCANotSupported,attributes.GetValue(ERightsIssuerUrl, value));
    STIF_ASSERT_EQUALS( 0,value.Length());
    CleanupStack::PopAndDestroy( manager );

    User::LeaveIfError(dcf2file.Open(iFs, KOma2Content, EFileRead));
    CleanupClosePushL(dcf2file);

    manager = CManager::NewLC();
    STIF_ASSERT_NOT_NULL( manager );
    error = manager->GetStringAttributeSet(attributes, dcf2file, KDefaultContentObject);
    if ( !error )
        {
        RUNCHECKED(attributes.GetValue(EMimeType, value));
        STIF_ASSERT_EQUALS( KErrCANotSupported,attributes.GetValue(ERightsIssuerUrl, value));
        STIF_ASSERT_EQUALS( 0,value.Length());
        }
    else
        {
        STIF_ASSERT_EQUALS( KErrCANotSupported, error );
        }

    CleanupStack::PopAndDestroy( 2 );

    attributes.Close();
    return KErrNone;
    }

TInt CDRM_CAF::CManager_SetPropertyL()
    {
    TInt r;

    TVirtualPathPtr dcf2(KOma2Content, KDefaultContentObject);
    CManager* manager = CManager::NewLC();
    STIF_ASSERT_NOT_NULL( manager );
    r = manager->SetProperty(EAgentPropertyBufferSize, 1024);
    STIF_ASSERT_EQUALS( KErrCANotSupported,r);
    r = manager->SetProperty(EAgentPropertyAgentUI, 1);
    STIF_ASSERT_EQUALS( KErrCANotSupported,r);
    r = manager->SetProperty(EAgentPropertyMultipleSequence, 1);
    STIF_ASSERT_EQUALS( KErrCANotSupported,r);
    CleanupStack::PopAndDestroy();

    return KErrNone;
    }

TInt CDRM_CAF::CManager_DisplayInfoL()
    {
    TInt r;
    RFile file;

    TVirtualPathPtr dcf2(KOma2Content, KDefaultContentObject);
    CManager* manager = CManager::NewLC();
    STIF_ASSERT_NOT_NULL( manager );
    TRAP(r, manager->DisplayInfoL(EFileAndRights, dcf2));
    STIF_ASSERT_EQUALS( KErrCANotSupported,r);

    User::LeaveIfError(file.Open(iFs, KOma2Content, EFileRead));
    TRAP(r, manager->DisplayInfoL(EFileAndRights, file, KDefaultContentObject));
    STIF_ASSERT_EQUALS( KErrCANotSupported,r);

    file.Close();
    CleanupStack::PopAndDestroy( manager );

    return KErrNone;
    }

TInt CDRM_CAF::CManager_ListAgentsL()
    {
    RArray<TAgent> agents;
    TBool f = EFalse;
    TInt i;


    CManager* manager = CManager::NewLC();
    STIF_ASSERT_NOT_NULL( manager );
    manager->ListAgentsL(agents);
    for (i = 0; i < agents.Count(); i++)
        {
        if (agents[i].Name().Compare(KOmaDrm2AgentName) == 0)
            {
            f = ETrue;
            }
        }
    STIF_ASSERT_TRUE(f);
    agents.Close();
    CleanupStack::PopAndDestroy();

    return KErrNone;
    }

TInt CDRM_CAF::CManager_AgentSpecificCommandL()
    {
    TPtr8 ptr(NULL, 0, 0);
    RArray<TAgent> agents;
    TRequestStatus status;
    TInt i;
    TInt r;
    HBufC8* data;
    HBufC8* result;
    HBufC8* buffer;

    r = iFs.Delete(_L("c:\\test.dcf"));
    STIF_ASSERT_EQUALS( KErrNone,r);

    RFileWriteStream out;
    out.Create(iFs, _L("c:\\test.dcf"), EFileWrite );


    out.Close();


    TInt size = 0;
    RFile file;
    User::LeaveIfError(file.Open(iFs, _L("c:\\test.dcf"), EFileRead));
    CleanupClosePushL(file);
    User::LeaveIfError(file.Size(size));
    data = HBufC8::NewLC(size);
    TPtr8 ptr1(data->Des());
    User::LeaveIfError(file.Read(ptr1, size));
    CleanupStack::Pop(); //data
    CleanupStack::PopAndDestroy(); //file

    buffer = HBufC8::NewL(data->Length() + 1);
    ptr.Set(buffer->Des());
    ptr.Append(EView);
    ptr.Append(*data);
    result = HBufC8::NewL(data->Length());
    ptr.Set(result->Des());


    TVirtualPathPtr dcf2(KOma2Content, KDefaultContentObject);
    CManager* manager = CManager::NewLC();
    STIF_ASSERT_NOT_NULL( manager );
    manager->ListAgentsL(agents);
    for (i = 0; i < agents.Count(); i++)
        {
        if (agents[i].Name().Compare(KOmaDrm2AgentName) == 0)
            {
            r = manager->AgentSpecificCommand(agents[i],
                EEmbedDomainRo, KNullDesC8, ptr);
            STIF_ASSERT_EQUALS( KErrCANotSupported,r);
            manager->AgentSpecificCommand(agents[i],
                EEmbedDomainRo, KNullDesC8, ptr, status);
            User::WaitForRequest(status);
            STIF_ASSERT_EQUALS( KErrCANotSupported,status.Int());

            r = manager->AgentSpecificCommand(agents[i],
                EBufferContainsOma1Dcf, KNullDesC8, ptr);
            STIF_ASSERT_NOT_EQUALS( KErrNone,r);

            r = manager->AgentSpecificCommand(agents[i],
                EBufferContainsOma1Dcf, _L8("012345"), ptr);
            STIF_ASSERT_NOT_EQUALS( KErrNone,r);

            r = manager->AgentSpecificCommand(agents[i],
                EDecryptOma1DcfBuffer, *buffer, ptr);
            STIF_ASSERT_EQUALS( KErrArgument,r);

            }
        }
    agents.Close();
    CleanupStack::PopAndDestroy();


    delete buffer;
    delete result;
    delete data;
    return KErrNone;
    }


TInt CDRM_CAF::CManager_CreateRightsManagerL()
    {
    CRightsManager* rm = NULL;
    rm = GetOmaDrmRightsManagerL();
    STIF_ASSERT_NOT_NULL(rm);
    delete rm;

    return KErrNone;
    }

TInt CDRM_CAF::CSupplier_IsImportSupportedL()
    {

    CSupplier* supplier = CSupplier::NewLC();
    STIF_ASSERT_TRUE(supplier->IsImportSupported(KOma1DrmMessageContentType));
    STIF_ASSERT_TRUE(supplier->IsImportSupported(KOma1XmlRoContentType));
    STIF_ASSERT_TRUE(supplier->IsImportSupported(KOma1WbxmlRoContentType));
    STIF_ASSERT_TRUE(supplier->IsImportSupported(KOma2RoContentType));
    STIF_ASSERT_FALSE(supplier->IsImportSupported(_L8("text/plain")));
    CleanupStack::PopAndDestroy(1);

    return KErrNone;
    }

TInt CDRM_CAF::CSupplier_PrepareHTTPRequestHeadersL()
    {
    return KErrNone;
    }

TInt CDRM_CAF::CSupplier_SetOutputDirectory_ImportFileL()
    {

    CSupplier* supplier = CSupplier::NewLC();
    CMetaDataArray* metaData = CMetaDataArray::NewLC();
    supplier->SetOutputDirectoryL(KEncryptedDir);
    CImportFile* file = supplier->ImportFileL(KOma1DrmMessageContentType,
        *metaData, KTempDcfName);
    STIF_ASSERT_NOT_NULL( file );
    CleanupStack::PushL(file);
    CleanupStack::PopAndDestroy(3);

    return KErrNone;
    }

TInt CDRM_CAF::CImportFile_WriteData_WriteDataComplete_OutputFileCountL()
    {
    TFileName fileName;
    TRequestStatus status;
    TInt i;
    TInt r;

    fileName.Copy(KEncryptedDir);
    fileName.Append(KTempDcfName);
    r = iFs.Delete(fileName);
    STIF_ASSERT_EQUALS(KErrNone , r);

    CSupplier* supplier = CSupplier::NewLC();
    CMetaDataArray* metaData = CMetaDataArray::NewLC();
    supplier->SetOutputDirectoryL(KEncryptedDir);
    CImportFile* file = supplier->ImportFileL(KOma1DrmMessageContentType,
        *metaData, KTempDcfName);
    STIF_ASSERT_NOT_NULL( file );
    CleanupStack::PushL(file);
    RUNCHECKEDL(file->WriteData(KDMSimple));
    RUNCHECKEDL(file->WriteDataComplete());
    STIF_ASSERT_EQUALS(1 , file->OutputFileCountL());
    CleanupStack::PopAndDestroy(3);


    fileName.Copy(KEncryptedDir);
    fileName.Append(KTempDcfName);
    r = iFs.Delete(fileName);
    STIF_ASSERT_EQUALS(KErrNone , r);

    supplier = CSupplier::NewLC();
    metaData = CMetaDataArray::NewLC();
    supplier->SetOutputDirectoryL(KEncryptedDir);
    file = supplier->ImportFileL(KOma1DrmMessageContentType,
        *metaData, KTempDcfName);
    STIF_ASSERT_NOT_NULL( file );
    CleanupStack::PushL(file);
    file->WriteData(KDMSimple, status);
    User::WaitForRequest(status);
    STIF_ASSERT_EQUALS(KErrNone , status.Int());
    file->WriteDataComplete(status);
    User::WaitForRequest(status);
    STIF_ASSERT_EQUALS(KErrNone , status.Int());
    STIF_ASSERT_EQUALS(1 , file->OutputFileCountL());
    CleanupStack::PopAndDestroy(3);


    fileName.Copy(KEncryptedDir);
    fileName.Append(KTempDcfName);
    r = iFs.Delete(fileName);
    STIF_ASSERT_EQUALS(KErrNone , r);

    supplier = CSupplier::NewLC();
    metaData = CMetaDataArray::NewLC();
    metaData->AddL(KOmaImportMimeTypeField, _L8("text/plain"));
    supplier->SetOutputDirectoryL(KEncryptedDir);
    file = supplier->ImportFileL(KOmaImportContentType,
        *metaData, KTempDcfName);
    STIF_ASSERT_NOT_NULL( file );
    CleanupStack::PushL(file);
    for (i = 0; i < 100; i++)
        {
        STIF_ASSERT_EQUALS(KErrNone , file->WriteData(KDMContent) );
        }
    STIF_ASSERT_EQUALS(KErrNone , file->WriteDataComplete());
    STIF_ASSERT_EQUALS(1 , file->OutputFileCountL());
    CleanupStack::PopAndDestroy(3);

    return KErrNone;
    }

TInt CDRM_CAF::CImportFile_OutputFileL_GetImportStatusL()
    {
    TFileName fileName;
    TInt r;


    fileName.Copy(KEncryptedDir);
    fileName.Append(KTempDcfName);
    r = iFs.Delete(fileName);
    STIF_ASSERT_EQUALS(KErrNone , r);

    CSupplier* supplier = CSupplier::NewLC();
    CMetaDataArray* metaData = CMetaDataArray::NewLC();
    supplier->SetOutputDirectoryL(KEncryptedDir);
    CImportFile* file = supplier->ImportFileL(KOma1DrmMessageContentType,
        *metaData, KTempDcfName);
    STIF_ASSERT_NOT_NULL( file );
    CleanupStack::PushL(file);
    RUNCHECKEDL(file->WriteData(KDMSimple));
    RUNCHECKEDL(file->WriteDataComplete());
    RUNCHECKED(file->GetImportStatus() == EComplete);
    STIF_ASSERT_EQUALS(1 , file->OutputFileCountL());
    CSupplierOutputFile& output = file->OutputFileL(0);
    STIF_ASSERT_EQUALS(0 , output.MimeTypeL().CompareF(KOma1DcfContentType));
    CleanupStack::PopAndDestroy(3);

    return KErrNone;
    }

TInt CDRM_CAF::CImportFile_GetSuggestedOutputFileExtensionL()
    {
    TInt r;
    TBuf<256> buffer;


    CSupplier* supplier = CSupplier::NewLC();
    CMetaDataArray* metaData = CMetaDataArray::NewLC();
    supplier->SetOutputDirectoryL(KEncryptedDir);
    CImportFile* file = supplier->ImportFileL(KOma1DrmMessageContentType,
        *metaData);
    STIF_ASSERT_NOT_NULL( file );
    CleanupStack::PushL(file);
    r = file->WriteData(KDMSimple);
    STIF_ASSERT_EQUALS(KErrCANewFileHandleRequired , r);
    RUNCHECKEDL(file->GetSuggestedOutputFileExtension(buffer));
    STIF_ASSERT_EQUALS(0 , buffer.CompareF(_L(".dcf")));
    CleanupStack::PopAndDestroy(3);

    return KErrNone;
    }

TInt CDRM_CAF::CImportFile_GetSuggestedOutputFileNameL()
    {
    TInt r;
    TBuf<256> buffer;


    CSupplier* supplier = CSupplier::NewLC();
    CMetaDataArray* metaData = CMetaDataArray::NewLC();
    supplier->SetOutputDirectoryL(KEncryptedDir);
    CImportFile* file = supplier->ImportFileL(KOma1DrmMessageContentType,
        *metaData);
    STIF_ASSERT_NOT_NULL( file );
    CleanupStack::PushL(file);
    r = file->WriteData(KDMSimple);
    STIF_ASSERT_EQUALS(KErrCANewFileHandleRequired , r);
    buffer.Copy(_L("test"));
    RUNCHECKEDL(file->GetSuggestedOutputFileName(buffer));
    STIF_ASSERT_EQUALS(0 , buffer.CompareF(_L("test.dcf")));
    CleanupStack::PopAndDestroy(3);

    return KErrNone;
    }

TInt CDRM_CAF::CImportFile_ContinueWithNewOutputFileL()
    {
    TFileName fileName;
    RFile f;
    TInt r;


    fileName.Copy(KEncryptedDir);
    fileName.Append(KTempDcfName);
    iFs.SetSessionPath(KEncryptedDir);
    r = iFs.Delete(fileName);
    STIF_ASSERT_EQUALS(KErrNone , r);
    CSupplier* supplier = CSupplier::NewLC();
    CMetaDataArray* metaData = CMetaDataArray::NewLC();
    supplier->SetOutputDirectoryL(KEncryptedDir);
    CImportFile* file = supplier->ImportFileL(KOma1DrmMessageContentType,
        *metaData);
    STIF_ASSERT_NOT_NULL( file );
    CleanupStack::PushL(file);
    r = file->WriteData(KDMSimple);
    STIF_ASSERT_EQUALS(KErrCANewFileHandleRequired , r);
    RUNCHECKEDL(f.Create(iFs, fileName, EFileWrite | EFileStream));
    CleanupClosePushL(f);
    file->ContinueWithNewOutputFile(f, fileName);
    RUNCHECKEDL(file->WriteData(KDMSimple));
    RUNCHECKEDL(file->WriteDataComplete());
    CleanupStack::PopAndDestroy(4);

    return KErrNone;
    }

TInt CDRM_CAF::CImportFile_ContentMimeTypeL()
    {
    TInt err( KErrNone );
    CSupplier* supplier = CSupplier::NewLC();
    CMetaDataArray* metaData = CMetaDataArray::NewLC();
    supplier->SetOutputDirectoryL(KEncryptedDir);
    CImportFile* file = supplier->ImportFileL(KOma1DrmMessageContentType,
                                              *metaData);
    CleanupStack::PushL( file );
    err = file->WriteData(KDMSimple);
    STIF_ASSERT_EQUALS(err, KErrCANewFileHandleRequired);
    TBuf8<KMaxDataTypeLength> contentMime;
    TRAP(err, file->ContentMimeTypeL( contentMime ));
    STIF_ASSERT_EQUALS(KErrCANotSupported, err);
    CleanupStack::PopAndDestroy( 3, supplier ); //file, metaData, supplier
    return KErrNone;
    }

TInt CDRM_CAF::CRightsManager_ListRightsL()
    {
    CRightsManager* rm = NULL;
    RStreamablePtrArray<CRightsInfo> rights;
    RFile dcfFile;

    rm = GetOmaDrmRightsManagerL();
    STIF_ASSERT_NOT_NULL( rm );
    CleanupStack::PushL(rm);
    rm->ListRightsL(rights, KTempDcfPathName);
    rights.ResetAndDestroy();
    rights.Close();
    CleanupStack::PopAndDestroy();

    User::LeaveIfError(dcfFile.Open(iFs, KTempDcfPathName, EFileRead));
    CleanupClosePushL(dcfFile);

    rm = GetOmaDrmRightsManagerL();
    STIF_ASSERT_NOT_NULL( rm );
    CleanupStack::PushL(rm);
    TRAPD( err, rm->ListRightsL(rights, dcfFile, KDefaultContentObject) );
    if ( err != KErrCANotSupported )
        {
        User::LeaveIfError( err );
        }
    rights.ResetAndDestroy();
    rights.Close();
    CleanupStack::PopAndDestroy( 2 );

    return KErrNone;
    }

TInt CDRM_CAF::CRightsManager_ListAllRightsL()
    {
    CRightsManager* rm = NULL;
    RStreamablePtrArray<CRightsInfo> rights;
    rm = GetOmaDrmRightsManagerL();
    STIF_ASSERT_NOT_NULL( rm );
    CleanupStack::PushL(rm);
    rm->ListAllRightsL(rights);
    rights.ResetAndDestroy();
    rights.Close();
    CleanupStack::PopAndDestroy();
    return KErrNone;
    }


TInt CDRM_CAF::CRightsManager_ListContentL()
    {
    TInt r = 0;
    CRightsManager* rm = NULL;
    RStreamablePtrArray<CVirtualPath> files;
    CData* data = NULL;
    TBuf<256> id;
    CRightsInfo* info = NULL;

    TBool r1 = EFalse;
    rm = GetOmaDrmRightsManagerL();
    STIF_ASSERT_NOT_NULL( rm );
    CleanupStack::PushL(rm);

    data = CData::NewLC(TVirtualPathPtr(KOma1Content, KDefaultContentObject),
        EPeek, EContentShareReadOnly);

    RUNCHECKEDL(data->GetStringAttribute(EContentID, id));

    info = CRightsInfo::NewL(KNullDesC, id, ERightsTypeStateless,
        ERightsStatusValid);


    CleanupStack::PushL(info);

    TRAP(r, rm->ListContentL(files, *info));
    if(r == KErrNone || r == KErrNotFound)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);
    files.ResetAndDestroy();
    files.Close();
    CleanupStack::PopAndDestroy(3); // rm, data, info

    return KErrNone;
    }

TInt CDRM_CAF::CRightsManager_DeleteRightsObjectL()
    {
    CRightsManager* rm = NULL;
    CData* data = NULL;
    TBuf<256> id;
    CRightsInfo* info = NULL;
    TInt r;
    TBool r1 = EFalse;
    rm = GetOmaDrmRightsManagerL();

    STIF_ASSERT_NOT_NULL( rm );
    CleanupStack::PushL(rm);
    data = CData::NewLC(TVirtualPathPtr(KOma1Content, KDefaultContentObject),
        EPeek, EContentShareReadOnly);
    RUNCHECKEDL(data->GetStringAttribute(EContentID, id));
    info = CRightsInfo::NewL(KNullDesC, id, ERightsTypeStateless,
        ERightsStatusValid);
    CleanupStack::PushL(info);
    r = rm->DeleteRightsObject(*info);
    if(r == -21 || r == 0)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);
    CleanupStack::PopAndDestroy(3);

    return KErrNone;
    }

TInt CDRM_CAF::CRightsManager_DeleteAllRightsObjectsL()
    {
    CRightsManager* rm = NULL;
    TInt r;
    TBool r1 = EFalse;
    rm = GetOmaDrmRightsManagerL();
    RFile file;

    STIF_ASSERT_NOT_NULL( rm );
    CleanupStack::PushL(rm);
    r = rm->DeleteAllRightsObjects(TVirtualPathPtr(KOma1Content, KNullDesC));
    if(r == -21 || r == -1)
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);

    r1 = EFalse;
    User::LeaveIfError(file.Open(iFs, KOma1Content, EFileRead));
    r = rm->DeleteAllRightsObjects(file, KDefaultContentObject);
    if(r == -21 || r == -1 || r == KErrCANotSupported )
        {
        r1 = ETrue;
        }
    STIF_ASSERT_TRUE(r1);
    file.Close();
    CleanupStack::PopAndDestroy( rm );

    return KErrNone;
    }

TInt CDRM_CAF::CRightsManager_SetPropertyL()
    {
    CRightsManager* rm = NULL;
    TInt r;
    rm = GetOmaDrmRightsManagerL();

    STIF_ASSERT_NOT_NULL( rm );
    CleanupStack::PushL(rm);
    r = rm->SetProperty(EAgentPropertyBufferSize, 1024);
    STIF_ASSERT_EQUALS(KErrCANotSupported , r);
    CleanupStack::PopAndDestroy();

    return KErrNone;
    }

TInt CDRM_CAF::CDRMLicenseChecker_CheckLicenseL()
    {
    CDRMLicenseChecker* lc = NULL;
    TInt r;
    TFileName fileName;
    HBufC8* data = NULL;

    fileName.Copy(KEncryptedDir);
    fileName.Append(KTempDcfName);


    lc = CDRMLicenseChecker::NewL();
    STIF_ASSERT_NOT_NULL( lc );
    CleanupStack::PushL(lc);
    r = lc->CheckLicense( fileName, data );
    STIF_ASSERT_EQUALS(KErrAccessDenied , r); // this does not work in SDK
    //STIF_ASSERT_EQUALS(KErrNone , r); // this works in the new SDK's
    delete data;
    CleanupStack::PopAndDestroy();

    return KErrNone;
    }


//  [End of File] - Do not remove
