/*
 * Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Dynamically loadable wrapper for Download manager
 *
 */
#include <centralrepository.h>
#include <cdblen.h>
#include <cmconnectionmethod.h>
#include <cmdestination.h>
#include <cmconnectionmethoddef.h>
#include <cmmanager.h>
#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>

#include <downloadmgrclient.h> //download manager
#include <es_enum.h> // tconnectioninfo
#include <es_enum_partner.h> // TConnectionInfoV2
#include <es_sock.h> // rconnection rsocket
#include <AknProgressDialog.h> // avkon classes
#include <eikprogi.h>

#include <ConeResLoader.h>
#include <apparc.h>
#include <DrmUtilityDmgrWrapper.rsg>

#include <RoapEng.h>
#include <RoapDef.h>
#include <RoapObserver.h>

#include "RoapSyncWrapper.h"
#include "RoapDef.h"
#include "DrmUtilityDmgrWrapper.h"
#include "DrmUtilityDmgrWrapperLogger.h"
#include "drmutilityconnection.h"
#include "buffercontainers.h" //CnameContainer etc.
#include "cleanupresetanddestroy.h"
#include "buffercontainers.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "DrmUtilityDmgrWrapperTraces.h"
#endif

#ifndef RD_MULTIPLE_DRIVE
_LIT( KDriveZ, "z:" );
_LIT( KDrmUtilityTriggerFilePath, "d:\\" );
#else
_LIT( KRomDriveFormatter, "%c:" );
_LIT( KKDrmUtilityTriggerFilePathFormatter, "%c:\\" );
#endif
_LIT( KCDrmUtilityDmgrWrapperResFileName,"DrmUtilityDmgrWrapper.rsc" );
const TInt KProgressInfoFinalValue(200);
const TInt KProgressInfoIncrementSmall(5);
const TInt KProgressInfoIncrementMedium(10);
const TInt KProgressInfoIncrementLarge(30);

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// ClearIfNotRoapTemporaryError
// ---------------------------------------------------------------------------
//
void ClearIfNotRoapTemporaryError(TInt aError, HBufC8*& aBuffer)
    {
    // ROAP ERROR CODES
    switch (aError)
        {
        case KErrRoapGeneral:
        case KErrRoapServer:
        case KErrRoapDomainFull:
        case KErrRoapNotRegistered:
            break;
        default:
            delete aBuffer;
            aBuffer = NULL;
            break;
        }
    }


// ---------------------------------------------------------------------------
// DeleteHttpDowload
// ---------------------------------------------------------------------------
//
LOCAL_C void DeleteHttpDowload(TAny* aDownload)
    {
    reinterpret_cast<RHttpDownload*> (aDownload)->Delete();
    }


// ---------------------------------------------------------------------------
// UpdateBufferL
// ---------------------------------------------------------------------------
//
template<typename bufType, typename descType>
LOCAL_C void UpdateBufferL(bufType*& aTargetBuf, const descType& aSourceBuf)
    {
    if (aTargetBuf)
        {
        delete aTargetBuf;
        aTargetBuf = NULL;
        }
    if (aSourceBuf.Length())
        {
        aTargetBuf = aSourceBuf.AllocL();
        }
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDrmUtilityDmgrWrapper::CDrmUtilityDmgrWrapper
// ---------------------------------------------------------------------------
//
CDrmUtilityDmgrWrapper::CDrmUtilityDmgrWrapper() :
            CActive(CActive::EPriorityStandard),
            iUseCoeEnv(EFalse), iIapId(0), iState(EInit)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// CDrmUtilityDmgrWrapper::ConstructL
// ---------------------------------------------------------------------------
//
void CDrmUtilityDmgrWrapper::ConstructL()
    {
    CLOG_WRITE( "DMgrWrapper::ConstructL" );
    const TInt KDrmUtilityDmgrWrapperUid = 0x102830FE;
    iConnection = DRM::CDrmUtilityConnection::NewL(ETrue);
    iDlMgr.ConnectL(TUid::Uid(KDrmUtilityDmgrWrapperUid), *this, EFalse);
    iProgressInfo = NULL;
    iProgressNoteDialog = NULL;
    iDialogDismissed = ETrue;
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFs.ShareProtected());
    }

// ---------------------------------------------------------------------------
// CDrmUtilityDmgrWrapper::NewL
// ---------------------------------------------------------------------------
//
CDrmUtilityDmgrWrapper* CDrmUtilityDmgrWrapper::NewL()
    {
    CLOG_WRITE( "DMgrWrapper::NewL" );
    CDrmUtilityDmgrWrapper* self(CDrmUtilityDmgrWrapper::NewLC());
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CDrmUtilityDmgrWrapper::NewLC
// ---------------------------------------------------------------------------
//
CDrmUtilityDmgrWrapper* CDrmUtilityDmgrWrapper::NewLC()
    {
    CLOG_WRITE( "DMgrWrapper::NewLC" );
    CDrmUtilityDmgrWrapper* self(new (ELeave) CDrmUtilityDmgrWrapper());
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CDrmUtilityDmgrWrapper::~CDrmUtilityDmgrWrapper
// ---------------------------------------------------------------------------
//
CDrmUtilityDmgrWrapper::~CDrmUtilityDmgrWrapper()
    {
    CLOG_WRITE( "DMgrWrapper destructor" );
    Cancel();
    if (iProgressNoteDialog)
        {
        // deletes the dialog
        TRAPD( err, iProgressNoteDialog->ProcessFinishedL() );
        if (err)
            {
            delete iProgressNoteDialog;
            }
        iProgressNoteDialog = NULL;
        }
    delete iErrorUrl;

    delete iPostResponseUrl;
    delete iConnection;

    delete iTriggerUrl;
    delete iTriggerBuf;
    delete iFileName;
    delete iRoapEng;

#ifdef _DEBUG

    if (iDlMgr.Handle())
        {
        iDlMgr.Close();
        }

#else

    iDlMgr.Close();

#endif

    iFs.Close();

    }


// ---------------------------------------------------------------------------
// CDrmUtilityDmgrWrapper::DownloadAndHandleRoapTriggerL
// ---------------------------------------------------------------------------
//
void CDrmUtilityDmgrWrapper::DownloadAndHandleRoapTriggerL(const HBufC8* aUrl)
    {
    CLOG_WRITE( "DMgrWrapper::DownloadAndHandleRoapTriggerL" );
    iUseCoeEnv = EFalse;
    if (iState != EInit || iWait.IsStarted())
        {
        User::Leave(KErrNotReady);
        }

    UpdateBufferL<HBufC8, TDesC8> (iTriggerUrl, *aUrl);
    CompleteToState(EInit, KErrNone);
    iWait.Start();
    }

// ---------------------------------------------------------------------------
// CDrmUtilityDmgrWrapper::DownloadAndHandleRoapTriggerL
// ---------------------------------------------------------------------------
//
void CDrmUtilityDmgrWrapper::DownloadAndHandleRoapTriggerL(
        const HBufC8* aUrl, CCoeEnv& aCoeEnv)
    {
    CLOG_WRITE( "DMgrWrapper::DownloadAndHandleRoapTriggerL" );
    iCoeEnv = &aCoeEnv;
    iUseCoeEnv = ETrue;
    if (iState != EInit || iWait.IsStarted())
        {
        User::Leave(KErrNotReady);
        }

    UpdateBufferL<HBufC8, TDesC8> (iTriggerUrl, *aUrl);
    CompleteToState(EInit, KErrNone);
    iWait.Start();
    }

// ---------------------------------------------------------------------------
// CDrmUtilityDmgrWrapper::GetErrorUrlL
// ---------------------------------------------------------------------------
//
HBufC8* CDrmUtilityDmgrWrapper::GetErrorUrlL()
    {
    if (iErrorUrl)
        {
        return iErrorUrl->AllocL();
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CDrmUtilityDmgrWrapper::DoConnectL
// ---------------------------------------------------------------------------
//
void CDrmUtilityDmgrWrapper::DoConnectL(TDownloadState aNextState)
    {
    iConnection->ConnectL(&iStatus);
    if (iUseCoeEnv && iProgressInfo)
        {
        iProgressInfo->SetAndDraw(0);
        }
    iState = aNextState;
    SetActive();
    }

/////
// ---------------------------------------------------------------------------
// CDrmUtilityDmgrWrapper::DoDownloadRoapTriggerL
// ---------------------------------------------------------------------------
//
void CDrmUtilityDmgrWrapper::DoDownloadRoapTriggerL(TDownloadState aNextState)
    {
    // Fetch name of opend connection to be used as part of DMGR
    // initialisation
    TUint32 iapId(0);
    if (iConnection->IsConnected(iapId))
        {
        iDlMgr.SetIntAttribute( EDlMgrIap, iapId );
        }
    RFile roapTrigger;
    TBool result(EFalse);
    DRM::CFileNameContainer* triggerFileName(NULL);

    // If no Trigger URL then nothing to download. So finish transaction
    if (!iTriggerUrl || iTriggerUrl->Length() <= 0)
        {
        if (iUseCoeEnv && iProgressInfo)
            {
            // No PrUrl found. Progess is complete.
            iProgressInfo->SetAndDraw(KProgressInfoFinalValue);
            }
        CompleteToState(EComplete, KErrNone);
        return;
        }

    TPtrC8 KNullPtr8(NULL, 0);
    RHttpDownload* downloadPtr(iDlMgr.FindDownload(*iTriggerUrl, KNullPtr8));
    if (downloadPtr)
        {
        // Stale download found.
        // Remove it, and re-create a new download.
        downloadPtr->Delete();
        downloadPtr = NULL;
        if (iFileName)
            {
            iFs.Delete(*iFileName);
            }
        }

    triggerFileName=DRM::CFileNameContainer::NewLC();
#ifndef RD_MULTIPLE_DRIVE

    User::LeaveIfError( roapTrigger.Temp(
                    iFs, KDrmUtilityTriggerFilePath, triggerFileName->iBuffer, EFileWrite ) );

#else //RD_MULTIPLE_DRIVE
    TInt driveNumber(-1);
    TChar driveLetter;
    DriveInfo::GetDefaultDrive(DriveInfo::EDefaultRam, driveNumber);
    iFs.DriveToChar(driveNumber, driveLetter);

    DRM::CFileNameContainer*
        utilityTriggerFilePath(DRM::CFileNameContainer::NewLC());

    utilityTriggerFilePath->iBuffer.Format(
            KKDrmUtilityTriggerFilePathFormatter, (TUint) driveLetter);

    User::LeaveIfError(roapTrigger.Temp(iFs, utilityTriggerFilePath->iBuffer,
            triggerFileName->iBuffer, EFileWrite));
    CleanupStack::PopAndDestroy( utilityTriggerFilePath );
    utilityTriggerFilePath=NULL;

#endif
    UpdateBufferL<HBufC, TFileName> (iFileName, triggerFileName->iBuffer);
    CleanupStack::PopAndDestroy( triggerFileName );
    triggerFileName=NULL;

    // create and start download
    RHttpDownload& download = iDlMgr.CreateDownloadL(*iTriggerUrl, result);
    // Put download for proper cleanup.
    TCleanupItem item(DeleteHttpDowload, &download);
    CleanupStack::PushL(item);

    CleanupClosePushL(roapTrigger);

    if (result)
        {
        iDownloadSuccess = EFalse;
        iConnectionError = EFalse;

        User::LeaveIfError(download.SetFileHandleAttribute(roapTrigger));
        User::LeaveIfError(download.SetBoolAttribute(
                EDlAttrNoContentTypeCheck, ETrue));
        User::LeaveIfError(download.Start());

        // wait until download is finished
        iState = aNextState;
        TRequestStatus* status(&iStatus);
        *status = KRequestPending;
        SetActive();
        }
    CleanupStack::PopAndDestroy(&roapTrigger);
    CleanupStack::Pop(&download); // Left open for DoSaveRoapTrigger
    }
// ---------------------------------------------------------------------------
// CDrmUtilityDmgrWrapper::DoSaveRoapTriggerL
// ---------------------------------------------------------------------------
//
void CDrmUtilityDmgrWrapper::DoSaveRoapTriggerL(TDownloadState aNextState)
    {
    // Check success of download

    // Fetch download created in DoDownloadRoapTriggerL
    RHttpDownload* download = iDlMgr.FindDownload(*iTriggerUrl, KNullDesC8());
    // Delete trigger URL so that it is possible to check
    // whether or not meteringResponse has PrUrl.
    delete iTriggerUrl;
    iTriggerUrl = NULL;
    iStatus = KRequestPending;
    // Put download for proper cleanup.
    TCleanupItem item(DeleteHttpDowload, download);
    CleanupStack::PushL(item);
    RFile roapTrigger;

    if (!iDownloadSuccess)
        {
        RemoveProgressNoteL();
        if (iConnectionError)
            {
            User::Leave(KErrCouldNotConnect);
            }
        else
            {
            User::Leave(KErrGeneral);
            }
        }
    User::LeaveIfError(roapTrigger.Open(iFs, *iFileName,
            EFileShareReadersOrWriters));
    CleanupClosePushL(roapTrigger);
    // Get filehandle of ROAP trigger
    const TInt KReadBufSize = 512;

    RBuf8 readBuf;
    readBuf.CleanupClosePushL();
    readBuf.CreateL(KReadBufSize);

    // Read file to buffer
    TInt triggerFileSize(0);
    User::LeaveIfError(roapTrigger.Size(triggerFileSize));
    if (iTriggerBuf)
        {
        delete iTriggerBuf;
        iTriggerBuf = NULL;
        }
    iTriggerBuf = HBufC8::NewL(triggerFileSize);
    User::LeaveIfError(roapTrigger.Read(readBuf, KReadBufSize));
    iTriggerBuf->Des().Copy(readBuf);
    while (readBuf.Length() == KReadBufSize)
        {
        User::LeaveIfError(roapTrigger.Read(readBuf, KReadBufSize));
        iTriggerBuf->Des().Append(readBuf);
        }

    // And let ROAP handle it...
    CleanupStack::PopAndDestroy(&readBuf);
    CleanupStack::PopAndDestroy(&roapTrigger);
    CleanupStack::PopAndDestroy(download);

    iFs.Delete(*iFileName);
    delete iFileName;
    iFileName = NULL;
    if (iUseCoeEnv && iProgressInfo)
        {
        iProgressInfo->IncrementAndDraw(KProgressInfoIncrementMedium);
        }

    CompleteToState(aNextState, KErrNone);
    }

// ---------------------------------------------------------------------------
// CDrmUtilityDmgrWrapper::DoHandleRoapTriggerL
// ---------------------------------------------------------------------------
//
void CDrmUtilityDmgrWrapper::DoHandleRoapTriggerL(TDownloadState aNextState)
    {
    Roap::TTriggerType triggerType;
    Roap::TRiContextStatus contextStatus;
    Roap::TDomainOperation domainOperation;

    RPointerArray<HBufC8> contentIds;
    CleanupResetAndDestroyPushL( contentIds );

    iRoapEng = Roap::CRoapEng::NewL();

    iRoapEng->SetTriggerL(*iTriggerBuf, NULL, triggerType, contextStatus,
            domainOperation, contentIds);

    CleanupStack::PopAndDestroy(&contentIds);

    iRoapEng->AcceptL(this, &iStatus);
    iState = aNextState;
    SetActive();
    }

// ---------------------------------------------------------------------------
// CDrmUtilityDmgrWrapper::CompleteToState
// ---------------------------------------------------------------------------
//
void CDrmUtilityDmgrWrapper::CompleteToState(
        CDrmUtilityDmgrWrapper::TDownloadState aNextState, TInt aError)
    {
    iState = aNextState;
    TRequestStatus* ownStatus(&iStatus);
    User::RequestComplete(ownStatus, aError);
    SetActive();
    }

// MHttpDownloadMgrObserver methods
// ---------------------------------------------------------------------------
// From class MHttpDownloadMgrObserver
//
// CDrmUtilityDmgrWrapper::HandleDMgrEventL
// ---------------------------------------------------------------------------
//
void CDrmUtilityDmgrWrapper::HandleDMgrEventL(RHttpDownload& aDownload,
        THttpDownloadEvent aEvent)
    {
    _LIT8( KDrmUtilityMimeTypeROAPTrigger,
            "application/vnd.oma.drm.roap-trigger+xml" );



    if (aEvent.iProgressState == EHttpContentTypeReceived)
        {
        // check received mimetype
        RBuf8 contentType;
        contentType.CleanupClosePushL();
        contentType.CreateL(KMaxContentTypeLength);
        User::LeaveIfError(aDownload.GetStringAttribute(EDlAttrContentType,
                contentType));
        if (!contentType.FindF(KDrmUtilityMimeTypeROAPTrigger))
            {
            // ROAP trigger found, continue download
            User::LeaveIfError(aDownload.Start());
            }
        else
            {
            // wrong MIME type, so stop download
            iDownloadSuccess = EFalse;
            User::LeaveIfError(aDownload.Delete());
            }
        CleanupStack::PopAndDestroy(&contentType);
        }

    if (aEvent.iDownloadState == EHttpDlCreated)
        {
        CLOG_WRITE( "DMgrWrapper::HandleDMgrEventL: EHttpDlCreated" );
        if (iUseCoeEnv && iProgressInfo)
            {
            iProgressInfo->IncrementAndDraw(KProgressInfoIncrementMedium);
            }
        }
    else if (aEvent.iProgressState == EHttpProgDisconnected)
        {
        CLOG_WRITE( "DMgrWrapper::HandleDMgrEventL: EHttpProgDisconnected" );
        
        // store failure
        iDownloadSuccess = EFalse;
        iConnectionError = ETrue;
        // finished
        }
    else if (aEvent.iDownloadState == EHttpDlInprogress)
        {
        CLOG_WRITE( "DMgrWrapper::HandleDMgrEventL: EHttpDlInprogress" );
        if (iUseCoeEnv)
            {
            iProgressInfo->IncrementAndDraw(KProgressInfoIncrementSmall);
            }
        }
    else if (aEvent.iDownloadState == EHttpDlCompleted)
        {
        // store success
        CLOG_WRITE( "DMgrWrapper::HandleDMgrEventL: EHttpDlCompleted" );
        iDownloadSuccess = ETrue;
        iConnectionError = EFalse;
        if (iUseCoeEnv)
            {
            iProgressInfo->IncrementAndDraw(KProgressInfoIncrementLarge);
            }
        // finished
        TRequestStatus* status(&iStatus);
        User::RequestComplete(status, KErrNone);
        }
    else if (aEvent.iDownloadState == EHttpDlFailed)
        {
        TInt32 err(KErrNone);

        CLOG_WRITE( "DMgrWrapper::HandleDMgrEventL: EHttpDlFailed" );
        // store failure
        iDownloadSuccess = EFalse;
        User::LeaveIfError(aDownload.GetIntAttribute(EDlAttrErrorId, err));
        CLOG_WRITE_FORMAT( "EDlAttrErrorId = %d", err );

        if (err == EConnectionFailed || err == ETransactionFailed)
            {
            CLOG_WRITE( "DMgrWrapper::HandleDMgrEventL: EConnectionFailed" );
            iConnectionError = ETrue;
            }
        User::LeaveIfError(aDownload.Delete()); // remove useless download
        User::LeaveIfError(iDlMgr.Disconnect()); // disconnects Dmgr instantly.
        // finished
        TRequestStatus* status(&iStatus);
        if ( iConnection->HasMoreConnectionAttempts() )
            {
            iState = EInit; // re-try with another conection
            User::RequestComplete(status, KErrNone);
            }
        else
            {
            User::RequestComplete(status, KErrCancel);        
            }
        }
    }

// ---------------------------------------------------------------------------
// CDrmUtilityDmgrWrapper::ShowProgressNoteL
// ---------------------------------------------------------------------------
//
void CDrmUtilityDmgrWrapper::ShowProgressNoteL()
    {
    DRM::CFileNameContainer* resFileName(NULL);

    CLOG_WRITE( "DMgrWrapper::ShowProgressNoteL" );
    if (iUseCoeEnv)
        {
        // Initialize the progress note dialog, it's values,
        // and execute it
        resFileName=DRM::CFileNameContainer::NewLC();
#ifndef RD_MULTIPLE_DRIVE

        resFileName->iBuffer.Copy( KDriveZ );

#else //RD_MULTIPLE_DRIVE
        TInt driveNumber(-1);
        TChar driveLetter;
        DriveInfo::GetDefaultDrive(DriveInfo::EDefaultRom, driveNumber);

        iCoeEnv->FsSession().DriveToChar(driveNumber, driveLetter);

        resFileName->iBuffer.Format(KRomDriveFormatter, (TUint) driveLetter);

#endif

        resFileName->iBuffer.Append(KDC_RESOURCE_FILES_DIR);
        resFileName->iBuffer.Append(KCDrmUtilityDmgrWrapperResFileName);
        RConeResourceLoader loader(*iCoeEnv);
        loader.OpenL(resFileName->iBuffer);
        CleanupStack::PopAndDestroy( resFileName );
        resFileName=NULL;

        iProgressNoteDialog = new (ELeave) CAknProgressDialog(
                reinterpret_cast<CEikDialog**> (&iProgressNoteDialog));
        iProgressNoteDialog->PrepareLC(R_SILENT_PROGRESS_NOTE);
        iProgressNoteDialog->SetCallback(this);
        iProgressInfo = iProgressNoteDialog->GetProgressInfoL();
        iProgressInfo->SetFinalValue(KProgressInfoFinalValue);
        iDialogDismissed = EFalse;
        iProgressNoteDialog->RunLD();

        loader.Close();
        }

    }

// ---------------------------------------------------------------------------
// CDrmUtilityDmgrWrapper::RemoveProgressNoteL
// ---------------------------------------------------------------------------
//
void CDrmUtilityDmgrWrapper::RemoveProgressNoteL()
    {

    if (iUseCoeEnv)
        {
        if (iProgressNoteDialog && !iDialogDismissed)
            {
            // deletes the dialog
            TRAPD(err, iProgressNoteDialog->ProcessFinishedL());
            if (err != KErrNone)
                {
                iProgressNoteDialog->SetCallback(NULL);
                delete iProgressNoteDialog;
                iDialogDismissed = ETrue;
                }
            iProgressNoteDialog = NULL;
            }
        }
    }

// ---------------------------------------------------------------------------
// From class MAknProgressDialogCallback
//
// CDrmUtilityDmgrWrapper::DialogDismissedL
// ---------------------------------------------------------------------------
//
void CDrmUtilityDmgrWrapper::DialogDismissedL(TInt aButtonId )
    {
    iDialogDismissed = ETrue;

    // Already freed, just set to NULL
    iProgressNoteDialog = NULL;
    iProgressInfo = NULL;

    if (IsActive() && aButtonId == EAknSoftkeyCancel)
        {
        if ((iState == EGetMeteringTrigger || iState == EGetPrUrlTrigger))
            {
            Cancel();
            }
        else
            {
            TRequestStatus* status(&iStatus);
            User::RequestComplete(status, KErrCancel);
            }
        }
    //For avoiding active object deadlock
    iDlMgr.DeleteAll();

    }

// RoapObserver methods

// ---------------------------------------------------------------------------
// CDrmUtilityDmgrWrapper::PostResponseUrlL
// ---------------------------------------------------------------------------
//
void CDrmUtilityDmgrWrapper::PostResponseUrlL(const TDesC8& aPostResponseUrl)
    {
    UpdateBufferL<HBufC8, TDesC8> (iTriggerUrl, aPostResponseUrl);

    if (iUseCoeEnv && iProgressInfo)
        {
        iProgressInfo->IncrementAndDraw(KProgressInfoIncrementMedium);
        }
    }

// Trivial RoapObserver methods
TBool CDrmUtilityDmgrWrapper::ConnectionConfL()
    {
    return ETrue;
    }

TBool CDrmUtilityDmgrWrapper::ContactRiConfL()
    {
    return ETrue;
    }

TBool CDrmUtilityDmgrWrapper::TransIdConfL()
    {
    return EFalse;
    }

void CDrmUtilityDmgrWrapper::RightsObjectDetailsL(const RPointerArray<
        CDRMRights>& /*aRightsList*/)
    {
    // do nothing
    }

void CDrmUtilityDmgrWrapper::ContentDownloadInfoL(TPath& /*aTempFolder*/,
        TFileName& /*aContentName*/, TInt& aMaxSize)
    {
    aMaxSize = -1;
    }

void CDrmUtilityDmgrWrapper::ContentDetailsL(const TDesC& /*aPath*/,
        const TDesC8& /*aType*/, const TUid& /*aAppUid*/)
    {
    }

void CDrmUtilityDmgrWrapper::RoapProgressInfoL(const TInt /*aProgressInfo*/)
    {
    // do nothing
    }

void CDrmUtilityDmgrWrapper::ErrorUrlL(const TDesC8& aErrorUrl)
    {
    UpdateBufferL<HBufC8, TDesC8> (iErrorUrl, aErrorUrl);
    }

// CActive methods

// ---------------------------------------------------------------------------
// CDrmUtilityDmgrWrapper::DoCancel
// ---------------------------------------------------------------------------
//
void CDrmUtilityDmgrWrapper::DoCancel()
    {
    delete iRoapEng;
    iRoapEng = NULL;
    iConnection->Close();
    if (iWait.IsStarted())
        {
        iWait.AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// CDrmUtilityDmgrWrapper::RunL
// ---------------------------------------------------------------------------
//
void CDrmUtilityDmgrWrapper::RunL()
    {
    TInt error(iStatus.Int());
    
    ClearIfNotRoapTemporaryError(error, iErrorUrl);
    User::LeaveIfError(error);
    switch (iState)
        {
        case EInit:
            {
            if (!iProgressNoteDialog)
                {
                ShowProgressNoteL();
                }
            DoConnectL(EGetMeteringTrigger);
            }
            break;
        case EGetMeteringTrigger:
            {
            DoDownloadRoapTriggerL(ESaveMeteringTrigger);
            }
            break;
        case ESaveMeteringTrigger:
            {
            DoSaveRoapTriggerL(EMeteringReportSubmit);
            }
            break;

        case EMeteringReportSubmit:
            {
            DoHandleRoapTriggerL(EGetPrUrlTrigger);
            }
            break;
        case EGetPrUrlTrigger:
            {
            delete iRoapEng;
            iRoapEng = NULL;
            DoDownloadRoapTriggerL(ESavePrUrlTrigger);
            }
            break;
        case ESavePrUrlTrigger:
            {
            DoSaveRoapTriggerL(EPrRoapRequest);
            }
            break;
        case EPrRoapRequest:
            {
            DoHandleRoapTriggerL(EComplete);
            }
            break;
        case EComplete:
            {
            RemoveProgressNoteL();
            delete iRoapEng;
            iRoapEng = NULL;
            iWait.AsyncStop();
            }
            break;

        default:
            User::Leave(KErrNotSupported);
        }
    }

// ---------------------------------------------------------------------------
// CDrmUtilityDmgrWrapper::RunError
// ---------------------------------------------------------------------------
//
TInt CDrmUtilityDmgrWrapper::RunError(TInt /* aError */)
    {
    delete iRoapEng;
    iRoapEng = NULL;
    iConnection->Close();
    if (iWait.IsStarted())
        {
        iWait.AsyncStop();
        }

    if (iUseCoeEnv)
        {
        if (iProgressNoteDialog && !iDialogDismissed)
            {
            iProgressNoteDialog->SetCallback(NULL);
            delete iProgressNoteDialog;
            iDialogDismissed = ETrue;
            }
        iProgressNoteDialog = NULL;
        }
    return KErrNone;
    }
// ======== GLOBAL FUNCTIONS ========

//------------------------------------------------------------------------------
// GateFunctionDRM
// DRM gate function
//------------------------------------------------------------------------------
EXPORT_C TAny* GateFunctionDMgr()
    {
    CDrmUtilityDmgrWrapper* launcher = NULL;
    TRAPD( err, launcher = CDrmUtilityDmgrWrapper::NewL() );
    if (err != KErrNone)
        {
        return NULL;
        }

    return launcher;
    }

