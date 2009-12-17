/*
* Copyright (c) 2006 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <caf.h>
#include <http.h>
#include <http/rhttpheaders.h>
#include <stringpool.h>
#include <w32std.h>
#include <apparc.h>
#include <apgtask.h>
#include <apgcli.h>
#include <apaserverapp.h>
#include <sysutil.h>
#include <wmdrmagent.h>

#include "wmdrmclient.h"
#include "wmdrmagentmanager.h"
#include "wmdrmagentattributes.h"
#include "asf.h"
#include "logfn.h"

using namespace ContentAccess;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::CWmDrmAgentManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWmDrmAgentManager::CWmDrmAgentManager()
    {
    LOGFN( "CWmDrmAgentManager::CWmDrmAgentManager" );
    }


// -----------------------------------------------------------------------------
// CWmDrmAgentManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWmDrmAgentManager* CWmDrmAgentManager::NewL()
    {
    LOGFN( "CWmDrmAgentManager::NewL" );
    CWmDrmAgentManager* self=new(ELeave) CWmDrmAgentManager();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWmDrmAgentManager* CWmDrmAgentManager::NewLC()
    {
    LOGFN( "CWmDrmAgentManager::NewLC" );
    CWmDrmAgentManager* self=new(ELeave) CWmDrmAgentManager();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::~CWmDrmAgentManager
// Destructor
// -----------------------------------------------------------------------------
//

CWmDrmAgentManager::~CWmDrmAgentManager()
    {
    LOGFN( "CWmDrmAgentManager::~CWmDrmAgentManager" );
    delete iFileMan;
    iFs.Close();
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWmDrmAgentManager::ConstructL()
    {
    LOGFN( "CWmDrmAgentManager::ConstructL" );
    User::LeaveIfError(iFs.Connect());
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::DeleteFile
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::DeleteFile(const TDesC& aFileName)
    {
    LOGFN( "CWmDrmAgentManager::DeleteFile" );
    return iFs.Delete(aFileName);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::CopyFile
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::CopyFile(const TDesC& aSource,
    const TDesC& aDestination)
    {
    LOGFN( "CWmDrmAgentManager::CopyFile" );
    TInt driveNumber = 0;
    TChar drive( aDestination[0] );
    RFile file;
    TInt size = 0;
    TInt result = KErrNone;

    TInt r = KErrNone;
    TBool retval(EFalse);

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
    TRAP(r, retval = SysUtil::DiskSpaceBelowCriticalLevelL( &iFs,
                                                             size,
                                                             driveNumber ) );

    if( retval )
        {
        return KErrDiskFull;
        }

    result = SetFileMan();

    if( result )
        {
        return result;
        }

    return iFileMan->Copy(aSource, aDestination);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::CopyFile
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::CopyFile(RFile& aSource,
    const TDesC& aDestination)
    {
    LOGFN( "CWmDrmAgentManager::CopyFile (2)" );
    TInt driveNumber = 0;
    TChar drive( aDestination[0] );
    TInt size = 0;
    TInt result = KErrNone;
    RFile output;
    TFileName fileName;
    TInt r = KErrNone;
    TBool retval(EFalse);

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
    TRAP(r, retval = SysUtil::DiskSpaceBelowCriticalLevelL( &iFs,
                                                            size,
                                                            driveNumber ) );
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

        result = SetFileMan();

        if( result )
            {
            return result;
            }

        result = iFileMan->Copy(aSource, aDestination);
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::RenameFile
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::RenameFile(const TDesC& aSource,
    const TDesC& aDestination)
    {
    LOGFN( "CWmDrmAgentManager::RenameFile" );
    TInt driveNumber = 0;
    TChar drive( aDestination[0] );
    RFile file;
    TInt size = 0;
    TInt result = KErrNone;
    TInt r = KErrNone;
    TBool retval(EFalse);

    result = SetFileMan();

    if( result )
        {
        return result;
        }

    result = iFileMan->Rename(aSource, aDestination);

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
        TRAP(r, retval = SysUtil::DiskSpaceBelowCriticalLevelL( &iFs,
                                                                size,
                                                                driveNumber ) );

        if( retval )
            {
            return KErrDiskFull;
            }

        result = iFileMan->Copy(aSource,aDestination);
        if (result == KErrNone)
            {
            // If the copy was successful try and delete the original
            result = iFileMan->Delete(aSource);
            if (result != KErrNone)
                {
                // Delete failed so try to cleanup the destination file
                // as we're going to exit with an error
                // We can safely ignore any error from this as the previous error
                // is more important to propagate, since this is just cleanup
                iFileMan->Delete(aDestination);
                }
            }
        }
    return result;
    }


// -----------------------------------------------------------------------------
// CWmDrmAgentManager::MkDir
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::MkDir(const TDesC& aPath)
    {
    LOGFN( "CWmDrmAgentManager::MkDir" );
    return iFs.MkDir( aPath );
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::MkDirAll
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::MkDirAll(const TDesC& aPath)
    {
    LOGFN( "CWmDrmAgentManager::MkDirAll" );
    return iFs.MkDirAll( aPath );
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::RmDir
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::RmDir(const TDesC& aPath)
    {
    LOGFN( "CWmDrmAgentManager::RmDir" );
    TInt result = KErrNone;

    result = SetFileMan();

    if( result )
        {
        return result;
        }

    return iFileMan->RmDir( aPath );
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::GetDir
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::GetDir(const TDesC& aName, TUint aEntryAttMask,
    TUint aEntrySortKey, CDir*& aEntryList) const
    {
    LOGFN( "CWmDrmAgentManager::GetDir" );
    return iFs.GetDir( aName, aEntryAttMask, aEntrySortKey, aEntryList );
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::GetDir
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::GetDir(const TDesC& aName,TUint aEntryAttMask,
    TUint aEntrySortKey, CDir*& aEntryList,CDir*& aDirList) const
    {
    LOGFN( "CWmDrmAgentManager::GetDir (2)" );
    return iFs.GetDir( aName, aEntryAttMask, aEntrySortKey, aEntryList, aDirList );
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::GetDir
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::GetDir(const TDesC& aName,
    const TUidType& aEntryUid,TUint aEntrySortKey,
    CDir*& aFileList) const
    {
    LOGFN( "CWmDrmAgentManager::GetDir (3)" );
    return iFs.GetDir( aName, aEntryUid, aEntrySortKey, aFileList );
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::GetAttribute
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::GetAttribute(TInt aAttribute, TInt& aValue,
    const TVirtualPathPtr& aVirtualPath)
    {

    TInt r = KErrNone;
    TInt err = KErrNone;

    LOGFN( "CWmDrmAgentManager::GetAttribute" );
    TRAP(err, r = CWmDrmAgentManager::GetAttributeCreateFileL(
            aAttribute, aValue, aVirtualPath));

    if(err != KErrNone)
        {
        return err;
        }

    return r;
    }
// -----------------------------------------------------------------------------
// CWmDrmAgentManager::GetAttributeCreateFileL
//
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::GetAttributeCreateFileL(TInt aAttribute, TInt& aValue,
    const TVirtualPathPtr& aVirtualPath)
    {
    TInt r = KErrNone;

    CAsf *asfFile = CAsf::NewL(aVirtualPath.URI());
    CleanupStack::PushL(asfFile);
    r = TWmDrmAgentAttributes::GetAttributeL(asfFile,
        aAttribute, aValue, aVirtualPath);
    CleanupStack::PopAndDestroy();
    return r;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::GetAttributeSet
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::GetAttributeSet(RAttributeSet& aAttributeSet,
    const TVirtualPathPtr& aVirtualPath)
    {

    TInt r = KErrNone;
    TInt err = KErrNone;

    LOGFN( "CWmDrmAgentManager::GetAttributeSet" );
    TRAP(err, r = CWmDrmAgentManager::GetAttributeSetCreateFileL(
            aAttributeSet, aVirtualPath));

    if(err != KErrNone)
        {
        return err;
        }
    return r;
    }
// -----------------------------------------------------------------------------
// CWmDrmAgentManager::GetAttributeSetCreateFileL
//
// -----------------------------------------------------------------------------
TInt CWmDrmAgentManager::GetAttributeSetCreateFileL(
    RAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath)
    {
    TInt r = KErrNone;

    CAsf *asfFile = CAsf::NewL(aVirtualPath.URI());
    CleanupStack::PushL(asfFile);
    r = TWmDrmAgentAttributes::GetAttributeSetL(asfFile,
        aAttributeSet, aVirtualPath);
    CleanupStack::PopAndDestroy();
    return r;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::GetStringAttributeSet
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::GetStringAttributeSet(
    RStringAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath)
    {
    TInt r = KErrNone;
    TInt err = KErrNone;
    RFile file;

    LOGFN( "CWmDrmAgentManager::GetStringAttributeSet" );
    TRAP(err, r = CWmDrmAgentManager::GetStringAttributeSetCreateFileL(
            aAttributeSet, aVirtualPath ));

    if(err != KErrNone)
        {
        return err;
        }
    return r;
    }
// -----------------------------------------------------------------------------
// CWmDrmAgentManager::GetStringAttributeSetCreateFileL
//
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::GetStringAttributeSetCreateFileL(
    RStringAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath)
    {
    TInt r = KErrNone;
    CAsf *asfFile = CAsf::NewL(aVirtualPath.URI());
    CleanupStack::PushL(asfFile);
    r = TWmDrmAgentAttributes::GetStringAttributeSetL(
              asfFile, aAttributeSet, aVirtualPath);
    CleanupStack::PopAndDestroy();
    return r;
    }


// -----------------------------------------------------------------------------
// CWmDrmAgentManager::GetStringAttribute
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::GetStringAttribute(TInt aAttribute, TDes& aValue,
    const TVirtualPathPtr& aVirtualPath)
    {
    TInt r = KErrNone;
    TInt err = KErrNone;

    LOGFN( "CWmDrmAgentManager::GetStringAttribute" );
    TRAP(err, r = CWmDrmAgentManager::GetStringAttributeCreateFileL(
              aAttribute, aValue, aVirtualPath ));
    if(err != KErrNone)
        {
        return err;
        }
    return r;
    }
// -----------------------------------------------------------------------------
// CWmDrmAgentManager::GetStringAttributeCreateFileL
//
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::GetStringAttributeCreateFileL(TInt aAttribute, TDes& aValue,
    const TVirtualPathPtr& aVirtualPath)
    {
    TInt r = KErrNone;
    CAsf *asfFile = CAsf::NewL(aVirtualPath.URI());
    CleanupStack::PushL(asfFile);
    r = TWmDrmAgentAttributes::GetStringAttributeL(
              asfFile, aAttribute, aValue, aVirtualPath);
    CleanupStack::PopAndDestroy();
    return r;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::NotifyStatusChange
// -----------------------------------------------------------------------------
//
void CWmDrmAgentManager::NotifyStatusChange(const TDesC& , TEventMask ,
    TRequestStatus& aStatus)
    {
    LOGFN( "CWmDrmAgentManager::NotifyStatusChange" );
    TRequestStatus* ptr = &aStatus;
    User::RequestComplete(ptr, KErrCANotSupported);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::CancelNotifyStatusChange
//
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::CancelNotifyStatusChange(const TDesC& ,
    TRequestStatus& )
    {
    LOGFN( "CWmDrmAgentManager::CancelNotifyStatusChange" );
    return KErrCANotSupported;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::SetProperty
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::SetProperty(TAgentProperty /*aProperty*/,
    TInt /*aValue*/)
    {
    LOGFN( "CWmDrmAgentManager::SetProperty" );
    return KErrCANotSupported;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::DisplayInfoL
// -----------------------------------------------------------------------------
//
void CWmDrmAgentManager::DisplayInfoL(TDisplayInfo /*aInfo*/,
    const TVirtualPathPtr& /*aVirtualPath*/)
    {
    LOGFN( "CWmDrmAgentManager::DisplayInfoL" );
    User::Leave(KErrCANotSupported);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::IsRecognizedL
// -----------------------------------------------------------------------------
//
TBool CWmDrmAgentManager::IsRecognizedL(
    const TDesC& /*aUri*/,
    TContentShareMode /*aShareMode*/) const
    {
    LOGFN( "CWmDrmAgentManager::IsRecognizedL" );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::IsRecognizedL
// -----------------------------------------------------------------------------
//
TBool CWmDrmAgentManager::IsRecognizedL(RFile& /*aFile*/) const
    {
    LOGFN( "CWmDrmAgentManager::IsRecognizedL (2)" );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::RecognizeFileL
// -----------------------------------------------------------------------------
//
TBool CWmDrmAgentManager::RecognizeFileL(
    const TDesC& /*aFileName*/,
    const TDesC8& /*aBuffer*/,
    TDes8& /*aFileMimeType*/,
    TDes8& /*aContentMimeType*/) const
    {
    LOGFN( "CWmDrmAgentManager::RecognizeFileL" );
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CWmDrmAgentManager::AgentSpecificCommand
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::AgentSpecificCommand(TInt aCommand,
                                              const TDesC8& /*iInputBuffer*/,
                                              TDes8& /*aOutputBuffer*/ )
    {
    TInt r = KErrCANotSupported;
    RWmDrmClient client;

    switch( aCommand )
        {
        case DRM::EWmDrmDeleteRights:
            r = client.Connect();
            if( !r )
                {
                r = client.DeleteRights();
                }
            client.Close();
            break;
        default:
            break;
        }

    return r;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::AgentSpecificCommand
// -----------------------------------------------------------------------------
//
void CWmDrmAgentManager::AgentSpecificCommand(TInt , const TDesC8& ,
    TDes8& , TRequestStatus& aStatus)
    {
    LOGFN( "CWmDrmAgentManager::AgentSpecificCommand (2)" );
    TRequestStatus *ptr = &aStatus;
    User::RequestComplete(ptr, KErrCANotSupported);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::
// -----------------------------------------------------------------------------
//
void CWmDrmAgentManager::DisplayManagementInfoL()
    {
    LOGFN( "CWmDrmAgentManager::DisplayManagementInfoL" );
    User::Leave(KErrCANotSupported);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::PrepareHTTPRequestHeaders
// -----------------------------------------------------------------------------
//
void CWmDrmAgentManager::PrepareHTTPRequestHeaders(RStringPool& /*aStringPool*/,
    RHTTPHeaders& /*aRequestHeaders*/) const
    {
    LOGFN( "CWmDrmAgentManager::PrepareHTTPRequestHeaders" );
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::RenameDir
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::RenameDir(const TDesC& /*aOldName*/,
    const TDesC& /*aNewName*/)
    {
    LOGFN( "CWmDrmAgentManager::RenameDir" );
    return KErrCANotSupported;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::IsProtected
// -----------------------------------------------------------------------------
//
TBool CWmDrmAgentManager::IsProtectedL(const TDesC& aFileName)
    {
    TBool r = EFalse;
    CAsf* asf = NULL;
    LOGFN( "CWmDrmAgentManager::IsProtectedL" );
    TRAP_IGNORE(asf = CAsf::NewL(aFileName));
    if (asf != NULL && asf->iIsDrmProtected)
        {
        r = ETrue;
        }
    delete asf;
    return r;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentManager::IsProtected
// -----------------------------------------------------------------------------
//
TBool CWmDrmAgentManager::IsProtectedL(RFile& aFile)
    {
    TBool r = EFalse;
    CAsf* asf = NULL;
    LOGFN( "CWmDrmAgentManager::IsProtectedL (2)" );
    TRAP_IGNORE(asf = CAsf::NewL(aFile));
    if (asf != NULL && asf->iIsDrmProtected)
        {
        r = ETrue;
        }
    delete asf;
    return r;
    }


// -----------------------------------------------------------------------------
// CWmDrmAgentManager::SetFileMan
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentManager::SetFileMan()
    {
    TInt err = KErrNone;
    if( !iFileMan )
        {
        TRAP(err, iFileMan = CFileMan::NewL(iFs) );
        }
    return err;
    }

//  End of File
