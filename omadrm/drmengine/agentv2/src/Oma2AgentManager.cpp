/*
* Copyright (c) 2005 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the Manager Interface
*
*/


// INCLUDE FILES
#include <caf/caf.h>
#include <caf/cafplatform.h>
#include <http.h>
#include <http/rhttpheaders.h>
#include <stringpool.h>
#include <utf.h>


#include <w32std.h>
#include <apparc.h>
#include <apgtask.h>
#include <apgcli.h>
#include <apaserverapp.h>
#include <sysutil.h>
#include <centralrepository.h>

#include "Oma2AgentManager.h"
#include "Oma2AgentAttributes.h"
#include "Oma1Dcf.h"
#include "oma2dcf.h"
#include "DRMRightsClient.h"

#include "DrmUtilityInternalcrkeys.h"      // Cenrep extension for OmaBased

using namespace ContentAccess;


const TInt KMinimumOma1DcfLength = 16;

const TInt KCenRepDataLength( 50 );

// ============================= LOCAL FUNCTIONS ===============================

LOCAL_C void DecryptL(
    const TDesC8& aContent,
    TDes8& aOutput)
    {
    COma1Dcf* dcf = NULL;
    TBuf8<KDCFKeySize> iv;
    TPtr8 input(NULL, 0);
    RDRMRightsClient client;
    TPtr8 ptr(NULL, 0);

    dcf = COma1Dcf::NewL(aContent.Mid(1));
    CleanupStack::PushL(dcf);
    User::LeaveIfError(client.Connect());
    CleanupClosePushL(client);
    User::LeaveIfError(client.Consume(aContent[0], *dcf->iContentID));
    User::LeaveIfError(client.InitializeKey(*dcf->iContentID));

    iv.Copy(aContent.Mid(dcf->iOffset + 1, KDCFKeySize));
    aOutput.Copy(&aContent[dcf->iOffset + KDCFKeySize + 1], dcf->iDataLength -
        KDCFKeySize);
    ptr.Set(&aOutput[0], aOutput.Length(), aOutput.Length());
    User::LeaveIfError(client.Decrypt(iv, ptr, ETrue));

    dcf->iPlainTextLength = aOutput.Length();
    dcf->iPadding = dcf->iDataLength - dcf->iPlainTextLength;
    client.Consume(EStop, *dcf->iContentID);
    CleanupStack::PopAndDestroy(2); // client, dcf
    }

LOCAL_C TInt SetName( const TDesC8& aContentUri,
    const TDesC& aContentName )
    {
    RDRMRightsClient client;
    TInt r = KErrNone;
    r = client.Connect();
    if( !r )
        {
        r = client.SetName( aContentUri, aContentName );
        }
    client.Close();
    return r;
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// COma2AgentManager::COma2AgentManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
COma2AgentManager::COma2AgentManager(void):
    iNotifier(NULL),
    iWatchedId(NULL)
    {
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void COma2AgentManager::ConstructL()
    {
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFs.ShareAuto());
    iFileManager = CFileMan::NewL(iFs);

    TInt err( KErrNone );

    TRAP(err, FetchOmaBasedInfoL() );
    if( err)
        {
        if( iOmaBasedMimeType )
            {
            delete iOmaBasedMimeType;
            }
        iOmaBasedMimeType = NULL;
        }

    }

// -----------------------------------------------------------------------------
// COma2AgentManager::FetchOmaBasedInfoL
// -----------------------------------------------------------------------------
//
void COma2AgentManager::FetchOmaBasedInfoL()
    {
    TInt err = KErrNone;
    CRepository* repository( NULL );
    RBuf bOmaBasedMimeType;

    CleanupClosePushL(bOmaBasedMimeType);
    bOmaBasedMimeType.CreateL( KCenRepDataLength );

    TRAP( err, repository = CRepository::NewL( KCRUidOmaBased ) );
    if ( !err )
        {
        CleanupStack::PushL( repository );

        err = repository->Get( KOmaBasedMimeType, bOmaBasedMimeType );
        if( !err )
            {
            iOmaBasedMimeType = CnvUtfConverter::ConvertFromUnicodeToUtf8L( bOmaBasedMimeType );
            }
        CleanupStack::PopAndDestroy( repository );
        }

    CleanupStack::PopAndDestroy();

    User::LeaveIfError( err );
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
COma2AgentManager* COma2AgentManager::NewL()
    {
    COma2AgentManager* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

COma2AgentManager* COma2AgentManager::NewLC()
    {
    COma2AgentManager* self=new(ELeave) COma2AgentManager();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

COma2AgentManager::~COma2AgentManager()
    {
    delete iFileManager;
    iFs.Close();
    if (iNotifier != NULL)
        {
        if( iWatchedId )
            {
            TRAP_IGNORE(iNotifier->UnRegisterEventObserverL(*this, KEventAddRemove, *iWatchedId));
            TRAP_IGNORE(iNotifier->UnRegisterEventObserverL(*this, KEventModify, *iWatchedId));
            }
        delete iNotifier;
        }
    delete iWatchedId;

    delete iOmaBasedMimeType;
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::DeleteFile
// -----------------------------------------------------------------------------
//
TInt COma2AgentManager::DeleteFile(const TDesC& aFileName)
    {
    return iFs.Delete(aFileName);
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::CopyFile
// -----------------------------------------------------------------------------
//
TInt COma2AgentManager::CopyFile(
    const TDesC& aSource,
    const TDesC& aDestination)
    {
    TInt driveNumber = 0;
    TChar drive( aDestination[0] );
    RFile file;
    TInt size = 0;
    TInt result = KErrNone;

    TInt err = KErrNone;
    TBool retval = KErrNone;


    // Check the destination drive letter
    result = iFs.CharToDrive(drive,driveNumber);

    if( result )
        {
        return result;
        }

    // open the file to read the size
    result = file.Open(iFs, aSource, EFileShareReadersOrWriters|EFileRead);

    if( result )
        {
        return result;
        }

    // read the size
    result = file.Size( size );

    // close the file
    file.Close();

    if( result )
        {
        return result;
        }

    // check that the drive has enough space for the copy operation

    TRAP( err, retval = SysUtil::DiskSpaceBelowCriticalLevelL( &iFs,
                                                               size,
                                                               driveNumber ) )
    if( retval )
        {
        return KErrDiskFull;
        }


    return iFileManager->Copy( aSource, aDestination);
    }


// -----------------------------------------------------------------------------
// CWmDrmAgentManager::CopyFile
//
// -----------------------------------------------------------------------------
//
TInt COma2AgentManager::CopyFile(RFile& aSource,
    const TDesC& aDestination)
    {
    TInt driveNumber = 0;
    TChar drive( aDestination[0] );
    TInt size = 0;
    TInt result = KErrNone;
    RFile output;
    TFileName fileName;

    TInt err = KErrNone;
    TBool retval = KErrNone;


    // Same file, do not even try to copy
    // And since they are the same don't return an error
    aSource.FullName( fileName );

    if( !aDestination.CompareF( fileName ) )
        {
        return KErrNone;
        }

    // Check the destination drive letter
    result = iFs.CharToDrive(drive,driveNumber);

    if( result )
        {
        return result;
        }

    // read the size
    result = aSource.Size( size );

    if( result )
        {
        return result;
        }

    // check that the drive has enough space for the copy operation

    TRAP( err, retval = SysUtil::DiskSpaceBelowCriticalLevelL( &iFs,
                                                               size,
                                                               driveNumber ) )
    if( retval )

        {
        return KErrDiskFull;
        }

    // Perform the copy:

    // Rewind just in case:
    size = 0;
    result = aSource.Seek(ESeekStart, size);

    if( !result )
        {
        result = iFileManager->Copy(aSource, aDestination);
        }

    return result;
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::RenameFile
// -----------------------------------------------------------------------------
//
TInt COma2AgentManager::RenameFile(
    const TDesC& aSource,
    const TDesC& aDestination)
    {
    TInt driveNumber = 0;
    TChar drive( aDestination[0] );
    RFile file;
    TInt size = 0;

    TInt err = KErrNone;
    TBool retval = KErrNone;


    TInt result( iFileManager->Rename(aSource, aDestination) );
    // If the files are on a different drive, Rename will fail
    // Therefore we simulate the Move by doing a Copy, followed by Delete
    if ( result != KErrNone )
        {
        // Check the destination drive letter
        result = iFs.CharToDrive(drive,driveNumber);

        if( result )
            {
            return result;
            }

        // open the file to read the size
        result = file.Open(iFs, aSource, EFileShareReadersOrWriters|EFileRead);

        if( result )
            {
            return result;
            }

        // read the size
        result = file.Size( size );

        // close the file
        file.Close();

        if( result )
            {
            return result;
            }

        // check that the drive has enough space for the copy operation

        TRAP( err, retval = SysUtil::DiskSpaceBelowCriticalLevelL( &iFs,
                                                                   size,
                                                                   driveNumber ) )
        if( retval )

            {
            return KErrDiskFull;
            }


        result = iFileManager->Copy(aSource,aDestination);
        if (result == KErrNone)
            {
            // If the copy was successful try and delete the original
            result = iFileManager->Delete(aSource);
            if (result != KErrNone)
                {
                // Delete failed so try to cleanup the destination file
                // as we're going to exit with an error
                // We can safely ignore any error from this as the previous error
                // is more important to propagate, since this is just cleanup
                iFileManager->Delete(aDestination);
                }
            }
        }
    return result;
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::MkDir
// -----------------------------------------------------------------------------
//
TInt COma2AgentManager::MkDir(
    const TDesC& aPath)
    {
    return iFs.MkDir( aPath );
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::MkDirAll
// -----------------------------------------------------------------------------
//
TInt COma2AgentManager::MkDirAll(
    const TDesC& aPath)
    {
    return iFs.MkDirAll( aPath );
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::RmDir
// -----------------------------------------------------------------------------
//
TInt COma2AgentManager::RmDir(
    const TDesC& aPath)
    {
    return iFileManager->RmDir( aPath );
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::GetDir
// -----------------------------------------------------------------------------
//
TInt COma2AgentManager::GetDir(
    const TDesC& aName,
    TUint aEntryAttMask,
    TUint aEntrySortKey,
    CDir*& aEntryList) const
    {
    return iFs.GetDir( aName, aEntryAttMask, aEntrySortKey, aEntryList );
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::GetDir
// -----------------------------------------------------------------------------
//
TInt COma2AgentManager::GetDir(
    const TDesC& aName,
    TUint aEntryAttMask,
    TUint aEntrySortKey,
    CDir*& aEntryList,
    CDir*& aDirList) const
    {
    return iFs.GetDir( aName, aEntryAttMask, aEntrySortKey, aEntryList, aDirList );
    }


// -----------------------------------------------------------------------------
// COma2AgentManager::GetDir
// -----------------------------------------------------------------------------
//
TInt COma2AgentManager::GetDir(
    const TDesC& aName,
    const TUidType& aEntryUid,
    TUint aEntrySortKey,
    CDir*& aFileList) const
    {
    return iFs.GetDir( aName, aEntryUid, aEntrySortKey, aFileList );
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::GetAttribute
// -----------------------------------------------------------------------------
//
TInt COma2AgentManager::GetAttribute(
    TInt aAttribute,
    TInt& aValue,
    const TVirtualPathPtr& aVirtualPath)
    {
    TInt r = KErrNone;
    CDcfCommon* dcfFile = NULL;

    TRAP(r, dcfFile = CDcfCommon::NewL(aVirtualPath.URI()));
    if( r )
        {
        return r;
        }
    aValue = TOma2AgentAttributes::GetAttribute(*dcfFile, aAttribute, aVirtualPath);
    delete dcfFile;
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::GetAttributeSet
// -----------------------------------------------------------------------------
//
TInt COma2AgentManager::GetAttributeSet(
    RAttributeSet& aAttributeSet,
    const TVirtualPathPtr& aVirtualPath)
    {
    TInt r = KErrNone;
    CDcfCommon *dcfFile = NULL;

    TRAP(r, dcfFile = CDcfCommon::NewL(aVirtualPath.URI()));
    if( r )
        {
        return r;
        }
    r = TOma2AgentAttributes::GetAttributeSet(*dcfFile, aAttributeSet, aVirtualPath);
    delete dcfFile;
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::GetStringAttributeSet
// -----------------------------------------------------------------------------
//
TInt COma2AgentManager::GetStringAttributeSet(
    RStringAttributeSet& aAttributeSet,
    const TVirtualPathPtr& aVirtualPath)
    {
    TInt r = KErrNone;
    CDcfCommon *dcfFile = NULL;

    TRAP(r, dcfFile = CDcfCommon::NewL(aVirtualPath.URI()));
    if( r )
        {
        return r;
        }
    r = TOma2AgentAttributes::GetStringAttributeSet(
        *dcfFile, aAttributeSet, aVirtualPath);
    delete dcfFile;
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::GetStringAttribute
// -----------------------------------------------------------------------------
//
TInt COma2AgentManager::GetStringAttribute(
    TInt aAttribute,
    TDes& aValue,
    const TVirtualPathPtr& aVirtualPath)
    {
    TInt r = KErrNone;
    CDcfCommon *dcfFile = NULL;

    TRAP(r, dcfFile = CDcfCommon::NewL(aVirtualPath.URI()));
    if( r )
        {
        return r;
        }
    r = TOma2AgentAttributes::GetStringAttribute(
        *dcfFile, aAttribute, aValue, aVirtualPath);
    delete dcfFile;
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::NotifyStatusChange
// -----------------------------------------------------------------------------
//
void COma2AgentManager::NotifyStatusChange(
    const TDesC& aUri,
    TEventMask aEventMask,
    TRequestStatus& aStatus)
    {
    CDcfCommon* dcf = NULL;
    TInt r = 0;

    if( iWatchedId != NULL )
        {
        if( iNotifier )
            {
            TRAP_IGNORE(iNotifier->UnRegisterEventObserverL(*this, KEventAddRemove, *iWatchedId));
            TRAP_IGNORE(iNotifier->UnRegisterEventObserverL(*this, KEventModify, *iWatchedId));
            }
        delete iWatchedId;
        }
    iWatchedEvents = TEventMask(0);

    if (iNotifier == NULL)
        {
        TRAP(r, iNotifier = CDRMNotifier::NewL());
        }
    TRAP(r, dcf = CDcfCommon::NewL(aUri));
    if (iNotifier != NULL && dcf != NULL)
        {
        iStatus = &aStatus;
        iWatchedEvents = aEventMask;
        TRAP_IGNORE( iWatchedId = dcf->iContentID->AllocL() );
        TRAP(r, iNotifier->RegisterEventObserverL(*this, KEventAddRemove, *iWatchedId));
        TRAP(r, iNotifier->RegisterEventObserverL(*this, KEventModify, *iWatchedId));
        *iStatus = KRequestPending;
        delete dcf;
        }
    if (r != KErrNone)
        {
        User::RequestComplete(iStatus, r);
        }
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::CancelNotifyStatusChange
// -----------------------------------------------------------------------------
//
TInt COma2AgentManager::CancelNotifyStatusChange(
    const TDesC& /*aUri*/,
    TRequestStatus& aStatus)
    {
    iStatus = &aStatus;
    if (iWatchedId != NULL)
        {
        if( iNotifier )
            {
            TRAP_IGNORE(iNotifier->UnRegisterEventObserverL(*this, KEventAddRemove, *iWatchedId));
            TRAP_IGNORE(iNotifier->UnRegisterEventObserverL(*this, KEventModify, *iWatchedId));
            }
        delete iWatchedId;
        iWatchedId = NULL;
        }
    iWatchedEvents = TEventMask(0);
    User::RequestComplete(iStatus, KErrCancel);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::SetProperty
// -----------------------------------------------------------------------------
//
TInt COma2AgentManager::SetProperty(
    TAgentProperty /*aProperty*/,
    TInt /*aValue*/)
    {
    return KErrCANotSupported;
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::DisplayInfoL
// -----------------------------------------------------------------------------
//
void COma2AgentManager::DisplayInfoL(
    TDisplayInfo /*aInfo*/,
    const TVirtualPathPtr& /*aVirtualPath*/)
    {
    User::Leave(KErrCANotSupported);
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::IsRecognizedL
// -----------------------------------------------------------------------------
//
TBool COma2AgentManager::IsRecognizedL(
    const TDesC& aUri,
    TContentShareMode /*aShareMode*/) const
    {
    TBuf8<40> buffer; // Size increased to 40
    TBool r = EFalse;

    User::LeaveIfError(iFs.ReadFileSection(aUri, 0, buffer, buffer.MaxLength()));
    if (COma1Dcf::IsValidDcf(buffer) || COma2Dcf::IsValidDcf(buffer))
        {
        r = ETrue;
        }
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::IsRecognizedL
// -----------------------------------------------------------------------------
//
TBool COma2AgentManager::IsRecognizedL(
    RFile& aFile) const
    {
    TBuf8<40> buffer; // Size increased to 40
    TBool r = EFalse;
    TInt pos = 0;

    User::LeaveIfError(aFile.Seek(ESeekStart, pos));
    User::LeaveIfError(aFile.Read(buffer, buffer.MaxLength()));
    if (COma1Dcf::IsValidDcf(buffer) || COma2Dcf::IsValidDcf(buffer))
        {
        r = ETrue;
        }
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::RecognizeFileL
// Only check from the file in case the buffer is not large enough to give
// a definitive answer.
// -----------------------------------------------------------------------------
//
TBool COma2AgentManager::RecognizeFileL(
    const TDesC& aFileName,
    const TDesC8& aBuffer,
    TDes8& aFileMimeType,
    TDes8& aContentMimeType) const
    {
    TBool r = EFalse;
    TInt err = KErrNone;
    CDcfCommon* dcf = NULL;

#ifdef __DRM_OMA2
    if ( !aFileName.Right(4).CompareF( KOma2DcfExtension ) ||
        !aFileName.Right(4).CompareF( KOma2DcfExtensionAudio ) ||
        !aFileName.Right(4).CompareF( KOma2DcfExtensionVideo ) ||
        COma2Dcf::IsValidDcf(aBuffer) )
        {
        aFileMimeType.Copy(KOma2DcfContentType);
        aContentMimeType.Copy(KCafMimeType);
        r = ETrue; // file was recognized as OMA2 DCF
        }
    else
        {
#endif
        // Check if the given buffer if a valid buffer, or if it's too short:
        if (COma1Dcf::IsValidDcf(aBuffer) ||
            aBuffer.Length() < KMinimumOma1DcfLength )
            {

            // Check if we can get all the information we need from the buffer.
            TRAP(err, dcf = COma1Dcf::NewL(aBuffer));

            // if we can't and we have a filename check from the file
            if (dcf == NULL && aFileName != KNullDesC)
                {
                dcf = CDcfCommon::NewL(aFileName);
                }

            // If the dcf object creation worked get the information needed and
            // mark the file as recognized. Check for specific mimetypes
            if (dcf != NULL)
                {
                CleanupStack::PushL(dcf);
                if ((dcf->iMimeType->Des()).CompareF(*iOmaBasedMimeType))
                    {
                    aFileMimeType.Copy(KOma1DcfContentType);
                    aContentMimeType.Copy(*dcf->iMimeType);
                    r = ETrue; // file was recognized as OMA1 DCF
                    }
                CleanupStack::PopAndDestroy();
                }

            }
#ifdef __DRM_OMA2
        }
#endif
    return r;
    }


// -----------------------------------------------------------------------------
// COma2AgentManager::AgentSpecificCommand
// -----------------------------------------------------------------------------
//
TInt COma2AgentManager::AgentSpecificCommand(
    TInt aCommand,
    const TDesC8& aInputBuffer,
    TDes8& aOutputBuffer)
    {
    TInt r = KErrCANotSupported;
    RDRMRightsClient client;
    TInt value = 0;
    TInt size = 0;
    TTimeIntervalSeconds interval;
    TPtrC8 contentUri;
    TPtrC16 contentName;

    switch( aCommand )
        {
        case EOmaDrmMethods:
                {
                aOutputBuffer.Copy(_L8("FL CD SD"));
#ifdef __DRM_OMA2
                aOutputBuffer.Append(_L8(" OMADRM2"));
#endif
                }
            break;
        case ESetPendingRightsETA:
                {
                Mem::Copy( &value, aInputBuffer.Ptr(), sizeof(TInt) );
                interval = value;
                contentUri.Set( aInputBuffer.Ptr() + sizeof(TInt),
                    aInputBuffer.Size()-sizeof(TInt) );

                r = client.Connect();
                if( !r )
                    {
                    r = client.SetEstimatedArrival( contentUri,
                        interval );
                    }
                // close the handle:
                client.Close();
                }
            break;
        case EBufferContainsOma1Dcf:
            if (COma1Dcf::IsValidDcf(aInputBuffer))
                {
                r = KErrNone;
                }
            else
                {
                r = KErrNotFound;
                }
            break;
        case EDecryptOma1DcfBuffer:
            r = KErrNone;
            TRAP(r, DecryptL(aInputBuffer, aOutputBuffer));
            break;
        case ESetContentName:
            Mem::Copy( &value, aInputBuffer.Ptr(), sizeof(TInt));
            contentName.Set(
                reinterpret_cast<TUint16*>(
                    const_cast<TUint8*>(aInputBuffer.Ptr() + sizeof(TInt))),
                value );

            size = aInputBuffer.Size();
            size -= value*2;
            size -= sizeof(TInt);

            contentUri.Set( aInputBuffer.Ptr() + sizeof(TInt) + value*2,
                size );

            r = SetName( contentUri, contentName );
            break;
        default:
            break;
        }

    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::AgentSpecificCommand
// -----------------------------------------------------------------------------
//
void COma2AgentManager::AgentSpecificCommand(
    TInt aCommand,
    const TDesC8& aInputBuffer,
    TDes8& aOutputBuffer,
    TRequestStatus& aStatus)
    {
    TRequestStatus *ptr = &aStatus;
    User::RequestComplete(ptr, AgentSpecificCommand(aCommand, aInputBuffer,
            aOutputBuffer));
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::PrepareHTTPRequestHeaders
// -----------------------------------------------------------------------------
//
void COma2AgentManager::PrepareHTTPRequestHeaders(
    RStringPool& aStringPool,
    RHTTPHeaders& aRequestHeaders) const
    {
    TRAP_IGNORE( PrepareHTTPRequestHeadersL(aStringPool, aRequestHeaders) );
    }

// -----------------------------------------------------------------------------
// COma2AgentManager::PrepareHTTPRequestHeadersL
// -----------------------------------------------------------------------------
//
void COma2AgentManager::PrepareHTTPRequestHeadersL(
    RStringPool& aStringPool,
    RHTTPHeaders& aRequestHeaders) const
    {
    TBuf8<KMaxDataTypeLength> mimeType;
    RStringF string;
    THTTPHdrVal header;

    mimeType.Copy(KOma1DrmMessageContentType);
    string = aStringPool.OpenFStringL(mimeType);
    CleanupClosePushL(string);
    header.SetStrF(string);
    aRequestHeaders.SetFieldL(aStringPool.StringF(HTTP::EAccept,
            RHTTPSession::GetTable()), header);
    CleanupStack::PopAndDestroy();

    mimeType.Copy(KOma1DcfContentType);
    string = aStringPool.OpenFStringL(mimeType);
    CleanupClosePushL(string);
    header.SetStrF(string);
    aRequestHeaders.SetFieldL(aStringPool.StringF(HTTP::EAccept,
            RHTTPSession::GetTable()), header);
    CleanupStack::PopAndDestroy();

    mimeType.Copy(KOma2DcfContentType);
    string = aStringPool.OpenFStringL(mimeType);
    CleanupClosePushL(string);
    header.SetStrF(string);
    aRequestHeaders.SetFieldL(aStringPool.StringF(HTTP::EAccept,
            RHTTPSession::GetTable()), header);
    CleanupStack::PopAndDestroy();

    mimeType.Copy(KOma2RoContentType);
    string = aStringPool.OpenFStringL(mimeType);
    CleanupClosePushL(string);
    header.SetStrF(string);
    aRequestHeaders.SetFieldL(aStringPool.StringF(HTTP::EAccept,
            RHTTPSession::GetTable()), header);
    CleanupStack::PopAndDestroy();

    mimeType.Copy(KOma2TriggerContentType);
    string = aStringPool.OpenFStringL(mimeType);
    CleanupClosePushL(string);
    header.SetStrF(string);
    aRequestHeaders.SetFieldL(aStringPool.StringF(HTTP::EAccept,
            RHTTPSession::GetTable()), header);
    CleanupStack::PopAndDestroy();
    }




// -----------------------------------------------------------------------------
// COma2AgentContent::HandleEventL
// Handle notifier events here. Not much logic, consider all events as rights
// changes.
// -----------------------------------------------------------------------------
//
void COma2AgentManager::HandleEventL(
    MDRMEvent* /*aEvent*/)
    {
    RDRMRightsClient client;
    TInt r;
    TUint32 reason = 0;

    if (client.Connect() == KErrNone && iWatchedId != NULL)
        {
        r = client.CheckRights(EUnknown, *iWatchedId, reason);
        if (r == KErrNone && (iWatchedEvents & ERightsAvailable) ||
            r != KErrNone && (iWatchedEvents & ERightsExpired))
            {
            iNotifier->UnRegisterEventObserverL(*this, KEventAddRemove, *iWatchedId);
            iNotifier->UnRegisterEventObserverL(*this, KEventModify, *iWatchedId);
            delete iWatchedId;
            iWatchedId = NULL;
            iWatchedEvents = TEventMask(0);
            User::RequestComplete(iStatus, KErrNone);
            }
        client.Close();
        }
    }


// -----------------------------------------------------------------------------
// COma2AgentManager::DisplayManagementInfoL
// -----------------------------------------------------------------------------
//
void COma2AgentManager::DisplayManagementInfoL()
    {
    User::Leave(KErrCANotSupported);
  	}

//  End of File
