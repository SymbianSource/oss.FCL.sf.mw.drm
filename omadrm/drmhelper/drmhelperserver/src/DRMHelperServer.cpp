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
* Description:  Implementation of the Helper Server functionality
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <DrmPermission.h> // CDRMPermission
#include <DrmConstraint.h> // CDRMConstraint
#include <s32file.h>
#include <DRMNotifier.h>
#include <DRMEventAddRemove.h>
#include <DRMTypes.h>
#include <bacntf.h> //CEnvironmentChangeNotifier
#include <centralrepository.h> // link against centralrepository.lib

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include "DRMEventModify.h"
#include "DRMHelperServerInternalCRKeys.h"
#include "DRMHelperCommon.h"
#include "DRMHelperServer.h"
#include "DRMHelperSession.h"
#include "EndTimeFactory.h"
#include "EndTimeBased.h"
#include "IdleObserver.h"





// LOCAL CONSTANTS AND MACROS
_LIT8(KExpiredMark , "E");

#ifdef RD_MULTIPLE_DRIVE
_LIT( KHelperServerDataStorage , "%c:DRMHS.dat" );
#else
_LIT( KHelperServerDataStorage , "c:DRMHS.dat" );
#endif

// #define _MEM_HEAP_USAGE


// ============================ LOCAL FUNCTIONS ===============================
#ifdef _DRM_TESTING
LOCAL_C void WriteLogL( const TDesC8& text , RFs &aFs );
LOCAL_C void WriteFileL( const TDesC8& text , RFs &aFs , const TDesC& aName );
LOCAL_C void CreateLogL();
LOCAL_C void WriteL( const TDesC8& aText );
LOCAL_C void WriteTimeL( TTime aTime );
LOCAL_C void WriteCurrentTimeL();
LOCAL_C void WriteL( const TDesC8& aText , TInt aErr );

#ifdef _MEM_HEAP_USAGE
LOCAL_C void WriteHeapAllocL();
#endif //_MEM_HEAP_USAGE

#endif

// -----------------------------------------------------------------------------
// Testing stuff
// -----------------------------------------------------------------------------
//
#ifdef _DRM_TESTING
LOCAL_C void WriteLogL( const TDesC8& text , RFs &aFs )
    {
    _LIT( KLogFile , "c:\\HSLog.txt" );
    WriteFileL( text , aFs , KLogFile );
    }

LOCAL_C void WriteFileL( const TDesC8& text , RFs &aFs , const TDesC& aName )
    {
    RFile file;
    TInt size;
    User::LeaveIfError( file.Open( aFs, aName , EFileWrite ) );
    CleanupClosePushL( file );
    User::LeaveIfError( file.Size( size ) );
    User::LeaveIfError( file.Write( size, text ) );
    CleanupStack::PopAndDestroy(&file); //file
    }

LOCAL_C void CreateLogL()
    {
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    RFile file;
    TInt err = file.Open( fs , _L("c:\\HSLog.txt") , EFileWrite );
    if (err)
        {
        err = file.Replace( fs , _L("c:\\HSLog.txt") , EFileWrite );
        }
    file.Close();
    CleanupStack::PopAndDestroy(&fs); //fs
    }

LOCAL_C void WriteL( const TDesC8& aText )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    HBufC8* text = HBufC8::NewLC(1000);
    TPtr8 textptr(text->Des() );
    textptr.Append( aText );
    textptr.Append( _L8("\r\n") );
    WriteLogL(textptr , fs);
    CleanupStack::PopAndDestroy(text);
    CleanupStack::PopAndDestroy(&fs); //fs
    WriteCurrentTimeL();
#ifdef _MEM_HEAP_USAGE
    WriteHeapAllocL();
#endif //_MEM_HEAP_USAGE
    }

LOCAL_C void WriteTimeL( TTime aTime )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    HBufC8* text = HBufC8::NewLC(100);
    TPtr8 textptr(text->Des() );
// Date and Time display
    TBuf<256> dateString;
    _LIT(KDate,"%*E%*D%X%*N%*Y %1 %2 '%3");
    aTime.FormatL(dateString,KDate);
    textptr.Append(_L( "\r\n\t\t\t\tData:\t" ) );
    textptr.Append( dateString );
    _LIT(KTime,"%-B%:0%J%:1%T%:2%S%:3%+B");
    aTime.FormatL(dateString,KTime);
    textptr.Append(_L( "\r\n\t\t\t\tTime:\t" ) );
    textptr.Append( dateString );
    textptr.Append(_L( "\r\n" ) );
    textptr.Append(_L( "\r\n" ) );
    WriteLogL(textptr , fs);
    CleanupStack::PopAndDestroy(text);
    CleanupStack::PopAndDestroy(&fs); //fs
    }

LOCAL_C void WriteCurrentTimeL()
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    HBufC8* text = HBufC8::NewLC(100);
    TPtr8 textptr(text->Des() );
// Date and Time display
    TTime time;
    time.HomeTime();
    TBuf<256> dateString;
    _LIT(KDate,"%*E%*D%X%*N%*Y %1 %2 '%3");
    time.FormatL(dateString,KDate);
    textptr.Append(_L( "\r\n\t\tData:\t" ) );
    textptr.Append( dateString );
    _LIT(KTime,"%-B%:0%J%:1%T%:2%S%:3%+B");
    time.FormatL(dateString,KTime);
    textptr.Append(_L( "\r\n\t\tTime:\t" ) );
    textptr.Append( dateString );
    textptr.Append(_L( "\r\n" ) );
    textptr.Append(_L( "\r\n" ) );
    WriteLogL(textptr , fs);
    CleanupStack::PopAndDestroy(text);
    CleanupStack::PopAndDestroy(&fs); //fs
    }

LOCAL_C void WriteL( const TDesC8& aText , TInt aErr )
    {
    _LIT8(KErr,": %d");
    HBufC8* text = HBufC8::NewLC(1000+20);
    TBuf8<20> num;
    TPtr8 textptr(text->Des());
    textptr.Append(aText);
    num.Format(KErr(),aErr);
    textptr.Append(num);
    WriteL(textptr);
    CleanupStack::PopAndDestroy(text);
#ifdef _MEM_HEAP_USAGE
    WriteHeapAllocL();
#endif //_MEM_HEAP_USAGE
    }

#ifdef _MEM_HEAP_USAGE
LOCAL_C void WriteHeapAllocL()
    {
    _LIT8(KHeapUsage,"\r\n***** Heap cells allocated: %d *****\r\n");
    TBuf8<256> string;
    TInt size = 0;
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    User::Heap().AllocSize(size);
    string.Format(KHeapUsage(),size);
    WriteLogL(string , fs);
    CleanupStack::PopAndDestroy(&fs); //fs
    }
#endif //_MEM_HEAP_USAGE

#endif //_DRM_TESTING



// -----------------------------------------------------------------------------
// DataFileL
// return data filename as a TParse
// -----------------------------------------------------------------------------
//
LOCAL_C TParse DataFileL(RFs& aFs)
    {
    TBuf<256> path;
    TParse p;
    User::LeaveIfError(aFs.PrivatePath(path));

#ifndef RD_MULTIPLE_DRIVE

    p.Set(KHelperServerDataStorage,&path,NULL);

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
    aFs.DriveToChar( driveNumber, driveLetter );

    TFileName helperServerDataStorage;
    helperServerDataStorage.Format(
                    KHelperServerDataStorage, (TUint)driveLetter );

    p.Set( helperServerDataStorage, &path, NULL );

#endif

    return p;
    }

// -----------------------------------------------------------------------------
// CreatePrivatePathL
// Create private path
// -----------------------------------------------------------------------------
//
LOCAL_C void CreatePrivatePathL(RFs& aFs)
    {
#ifdef _DRM_TESTING
    TRAPD( r , WriteL(_L8("CreatePrivatePathL")) );
#endif

#ifndef RD_MULTIPLE_DRIVE

    TInt err = aFs.CreatePrivatePath(EDriveC);

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );

    TInt err = aFs.CreatePrivatePath(driveNumber);

#endif

    if (err == KErrAlreadyExists)
        {
        err = KErrNone;
        }
    User::LeaveIfError(err);

#ifdef _DRM_TESTING
    TRAP( r , WriteL(_L8("CreatePrivatePathL->End")) );
#endif
    return;

    }

// -----------------------------------------------------------------------------
// CreateCRKeyL
// Create CenRep Key
// -----------------------------------------------------------------------------
//
LOCAL_C void CreateCRKeyL()
    {
#ifdef _DRM_TESTING
    TRAPD( r , WriteL(_L8("CreateCRKeyL")) );
#endif

  CRepository* repository = CRepository::NewL(KCRUidDRMHelperServer);
    CleanupStack::PushL( repository );
    TInt err = repository->Create(KDRMHelperServerNotification, KNullDesC8);
    if (err == KErrAlreadyExists)
        {
        err = KErrNone;
        }
#ifdef _DRM_TESTING
    TRAP( r , WriteL(_L8("CreateCRKeyL->Create1"),err) );
#endif

    User::LeaveIfError(err);
    err = repository->Create(KDRMHelperServerNotificationPassive, KNullDesC8);
    if (err == KErrAlreadyExists)
        {
        err = KErrNone;
        }
#ifdef _DRM_TESTING
    TRAP( r , WriteL(_L8("CreateCRKeyL->Create2"),err) );
#endif
    User::LeaveIfError(err);
    CleanupStack::PopAndDestroy(repository); // repository

#ifdef _DRM_TESTING
    TRAP( r , WriteL(_L8("CreateCRKeyL->End")) );
#endif
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMHelperServer::CDRMHelperServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMHelperServer::CDRMHelperServer() :
    CServer2( EPriorityStandard ) ,
                    iEndTimeFactory(NULL) ,
                    iNotifier(NULL),
                    iIdleObserver(NULL) ,
                    iUiTimeNotifier(NULL)
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMHelperServer* CDRMHelperServer::NewLC()
    {

#ifdef _DRM_TESTING
    CreateLogL();
    TRAPD( err , WriteL(_L8("NewLC")) );
#endif
    CDRMHelperServer* self = new(ELeave) CDRMHelperServer;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    };

// -----------------------------------------------------------------------------
// CDRMHelperServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::ConstructL()
    {
    TInt err = 0;

#ifdef _DRM_TESTING
    TRAPD( r , WriteL(_L8("ConstructL")) );
#endif


    TUint8 waitCount = 0;
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iDrm.StartServer());
    User::LeaveIfError(iDrm.Connect());
    StartL(KDRMHelperServerName);
    //ignore errors
    err = User::RenameProcess( KDRMHSServerFileName );
    err = User::RenameThread(KDRMHSServerFileName);


    CreatePrivatePathL(iFs); // create path for internal data file
    iEndTimeFactory = CEndTimeFactory::NewL( *this );
    err = KErrNotFound;
    while( err!=0 && waitCount < 30 ) // wait maximum of 6s
        {
        TRAP( err , iNotifier = CDRMNotifier::NewL() );
        if ( err ) // wait only if the server isn't running
            {
            User::After(200000); // 0.2s
            waitCount++;
            }
        if (iNotifier)
            {
            iNotifier->RegisterEventObserverL( *this , KEventTimeChange );
            }
        }
#ifdef _DRM_TESTING
    TRAP( r , WriteL(_L8("ConstructL->Notifier")) );
#endif
    User::LeaveIfError( err ); // None can do, throw an exception.

    iUiTimeNotifier = CEnvironmentChangeNotifier::NewL(
        EPriorityHigh,
        TCallBack(UiTimeChanged,this)
        );
    iUiTimeNotifier->Start();

    iIdleObserver = CIdleObserver::NewL( *this );

    iIdleObserver->StartL();
#ifdef _DRM_TESTING
    TRAP( r , WriteL(_L8("ConstructL->IdleObserver")) );
#endif

    CreateCRKeyL();
    RestoreL();

    iEndTimeFactory->HandleExpiredL();

#ifdef _DRM_TESTING
    TRAP( r , WriteL(_L8("ConstructL->End")) );
#endif
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::~CDRMHelperServer
// Destructor.
// -----------------------------------------------------------------------------
//
CDRMHelperServer::~CDRMHelperServer()
    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L8("destructor")) );
#endif


    if ( iEndTimeFactory )
        {
        TRAPD( error , StoreL() );
        TRAP( error , RemoveAllL() );
        }
    delete iEndTimeFactory;
    if (iNotifier)
        {
        TRAP_IGNORE( iNotifier->UnRegisterEventObserverL( *this , KEventTimeChange ) );
        }
    delete iNotifier;
    if(iUiTimeNotifier)
        {
        iUiTimeNotifier->Cancel();
        delete iUiTimeNotifier;
        }
    delete iIdleObserver;
    iFs.Close();
    iDrm.Close();
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::NewSessionL
// Called when a client requires a new instance.
// -----------------------------------------------------------------------------
CSession2* CDRMHelperServer::NewSessionL(const TVersion &aVersion,
                                         const RMessage2& /*aMessage*/ ) const
    {
    // check we're the right version
    if (!User::QueryVersionSupported(TVersion(KDRMHSMajorVersionNumber,
                                              KDRMHSMinorVersionNumber,
                                              KDRMHSBuildVersionNumber),
                                     aVersion))
        {
        User::Leave(KErrNotSupported);
        }

    // make new session
    return CDRMHelperSession::NewL();
    }


// -----------------------------------------------------------------------------
// CDRMHelperServer::StartupL().
// This function starts the actual DRM Helper server after initializing
// the cleanup stack and active scheduler.
// Returns: TInt: Symbian OS error code.
// -----------------------------------------------------------------------------
//

void CDRMHelperServer::StartupL()
    {
    // Construct active scheduler
    CActiveScheduler* activeScheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL(activeScheduler) ;

    // Install active scheduler
    // We don't need to check whether an active scheduler is already installed
    // as this is a new thread, so there won't be one
    CActiveScheduler::Install(activeScheduler);

    // Construct our server
    CDRMHelperServer::NewLC();

    RSemaphore semaphore;
    User::LeaveIfError(semaphore.OpenGlobal(KDRMHelperServerSemaphoreName));

    // Semaphore opened ok
    semaphore.Signal();
    semaphore.Close();

    // Start handling requests
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy(2, activeScheduler);    //  anonymous CDRMHelperServer
    }


void CDRMHelperServer::PanicServer(TDRMHelperServPanic aPanic)
    {
    User::Panic(KDRMHSServer, aPanic);
    }


// -----------------------------------------------------------------------------
// CDRMHelperServer::Startup().
// This function starts the actual DRM Rights server after initializing
// the cleanup stack and active scheduler.
// Returns: TInt: Symbian OS error code.
// -----------------------------------------------------------------------------
//

TInt CDRMHelperServer::Startup()
    {
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (!cleanupStack)
        {
        PanicServer(ECreateTrapCleanup);
        }
    TRAPD(err, StartupL());
    if (err != KErrNone)
        {
        PanicServer(ESrvCreateServer);
        }
    delete cleanupStack;
    cleanupStack = NULL;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::RunError().
// This function handle errors from CActive
// -----------------------------------------------------------------------------
//
TInt CDRMHelperServer::RunError(TInt aError)
    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L8("RunError")) );
#endif


    Message().Complete(aError);
    //
    // The leave will result in an early return from CServer::RunL(), skipping
    // the call to request another message. So do that now in order to keep the
    // server running.
    ReStart();
    return KErrNone;    // handled the error fully
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::HandleExpiredL().
// This function send expiration info out through CenRep
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::HandleExpiredL( CEndTimeBased*& aItem )
    {
#ifdef _DRM_TESTING
    //test
    _LIT8(KShowTimes , "Informed Times: %d");
    TBuf8<40> timesBuf;
    timesBuf.Format(KShowTimes , aItem->Count());
    WriteL(_L8("HandleExpiredL"));
    WriteL(aItem->Uri()->Des());
    WriteL(timesBuf);
#endif

    HBufC8* buf = NULL;
    TPtr8 ptr(NULL,0);
    FormatKeyValueLC( buf , aItem , KExpiredMark ); // buf get pushed into cleanupstack
    SetKeyValueL( *buf , aItem->RegType() );
    CleanupStack::PopAndDestroy(buf);
    }


// -----------------------------------------------------------------------------
// CDRMHelperServer::FormatKeyValueLC().
// This function format key for CenRep
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::FormatKeyValueLC(
        HBufC8*& aBuf ,
        CEndTimeBased*& aItem ,
        const TDesC8& aMark )
    {
    TPtr8 ptr(NULL,0,0);

    aBuf = HBufC8::NewLC( aItem->Uri()->Des().Length() + 4 );
    ptr.Set(aBuf->Des());
    ptr.Append(aItem->Count());
    ptr.Append( aMark );
    ptr.Append(aItem->PermType());
    ptr.Append(aItem->AutoType());
    ptr.Append( aItem->Uri()->Des() );

    // a walk around for the symbian bugs in cenrep
    TInt length = ptr.Length();
    if (length&1)
        {
        ptr.SetLength(length+1);
        ptr[length] = 0;
        }
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::HandleIdleL().
// This function is called when phone enter idle status
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::HandleIdleL()
    {
#ifdef _DRM_TESTING
    //test
    WriteL(_L8("HandleIdleL"));
#endif
    iEndTimeFactory->HandleIdleL();
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::RegisterL().
// This function register a item
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::RegisterL(
                        const TDesC8& aUri ,
                        const TUint8& aPermType ,
                        const TUint8& aRegType ,
                        const TUint8& aAutoType )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("RegisterL"));
    WriteL(aUri);
#endif
    TTime endTime = Time::MinTTime();
    CheckExpirationL( aUri , aPermType , aRegType , endTime );
    iEndTimeFactory->AddL( aUri , endTime , aPermType , aRegType , aAutoType );
    GetNotificationL( aUri );
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::IsRegistered().
// This function check if item is registered
// -----------------------------------------------------------------------------
//
TBool CDRMHelperServer::IsRegistered(
        const TDesC8& aUri ,
        const TUint8& aPermType ,
        const TUint8& aRegType ,
        const TUint8& aAutoType )
    {
#ifdef _DRM_TESTING
    TRAPD(err,WriteL(_L8("IsRegisteredL(uri,perm,reg,auto)")));
    TRAP(err,WriteL(aUri));
#endif

    return iEndTimeFactory->IsRegistered( aUri , aPermType , aRegType , aAutoType );
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::IsRegistered().
// This function check if item is registered
// -----------------------------------------------------------------------------
//
TBool CDRMHelperServer::IsRegistered(
        const TDesC8& aUri ,
        const TUint8& aPermType ,
        const TUint8& aRegType )
    {
#ifdef _DRM_TESTING
    TRAPD(err,WriteL(_L8("IsRegisteredL(uri,perm,reg)")));
    TRAP(err,WriteL(aUri));
#endif

    return iEndTimeFactory->IsRegistered( aUri , aPermType , aRegType );
    }


// -----------------------------------------------------------------------------
// CDRMHelperServer::IsRegistered().
// This function check if item is registered
// -----------------------------------------------------------------------------
//
TBool CDRMHelperServer::IsRegistered2(
        const TDesC8& aUri ,
        const TUint8& aRegType,
        const TUint8& aAutoType )
    {
#ifdef _DRM_TESTING
    TRAPD(err,WriteL(_L8("IsRegisteredL(uri,reg,auto)")));
    TRAP(err,WriteL(aUri));
#endif

    return iEndTimeFactory->IsRegistered2( aUri, aRegType, aAutoType );
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::UpdateL().
// This function update all items
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::UpdateL()
    {
#ifdef _DRM_TESTING
    WriteL(_L8("UpdateL(all)"));
#endif

    iEndTimeFactory->UpdateL();
    }


// -----------------------------------------------------------------------------
// CDRMHelperServer::UpdateL().
// This function update items with a URI
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::UpdateL( const TDesC8& aUri )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("UpdateL(aUri)"));
    WriteL(aUri);
#endif

    UpdateL( aUri , ContentAccess::EView);
    UpdateL( aUri , ContentAccess::EPlay);
    UpdateL( aUri , ContentAccess::EExecute);
    UpdateL( aUri , ContentAccess::EPrint);

    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::UpdateL().
// This function update items with a URI and a permission type
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::UpdateL(
            const TDesC8& aUri ,
            const TUint8& aPermType )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("UpdateL(aUri,aPerm)"));
    WriteL(aUri);
#endif

    UpdateL( aUri , aPermType , CDRMHelperServer::EActive );
    UpdateL( aUri , aPermType , CDRMHelperServer::EPassive );

    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::UpdateL().
// This function update items with a URI, a permission type and a register type
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::UpdateL(
            const TDesC8& aUri ,
            const TUint8& aPermType ,
            const TUint8& aRegType )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("UpdateL(aUri,aPerm,aReg)"));
    WriteL(aUri);
#endif

    TTime endTime;
    TBool reg = IsRegistered(aUri , aPermType , aRegType);
    if (reg)
        {
        CheckExpirationL( aUri , aPermType , aRegType , endTime );
        iEndTimeFactory->UpdateEndTimeL( aUri , endTime , aPermType , aRegType );
        }
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::UpdateL().
// This function update items with a URI, a permission type and a register type
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::UpdateL(
            const TDesC8& aUri ,
            const TUint8& aPermType ,
            const TUint8& aRegType ,
            const TUint8& aAutoType )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("UpdateL(aUri,aPerm,aReg,aAutoType)"));
    WriteL(aUri);
#endif

    TTime endTime;
    CheckExpirationL( aUri , aPermType , aRegType , endTime );
    iEndTimeFactory->UpdateEndTimeL( aUri , endTime , aPermType , aRegType , aAutoType );
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::RemoveL().
// This function update an item
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::RemoveL(
                        const TDesC8& aUri ,
                        const TUint8& aPermType ,
                        const TUint8& aRegType ,
                        const TUint8& aAutoType )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("RemoveL"));
    WriteL(aUri);
#endif

    TBool found = iEndTimeFactory->IsRegistered( aUri , aPermType , aRegType , aAutoType );
    if (found)
        {
        iEndTimeFactory->Remove( aUri , aPermType , aRegType , aAutoType );
        iEndTimeFactory->ResetAlarm();
        if ( !iEndTimeFactory->IsRegistered( aUri ) )
            {
            RemoveNotificationL( aUri );
            }
        }
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::RemoveAllL().
// This function removes all items
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::RemoveAllL()
    {
#ifdef _DRM_TESTING
    WriteL(_L8("RemoveAllL"));
#endif

    iEndTimeFactory->RemoveAllL();
    }


ContentAccess::TIntent CDRMHelperServer::MapPermissionType(TUint8 aPermType)
    {
    switch(aPermType)
        {
        case ContentAccess::EPlay:
            return ContentAccess::EPlay;
        case ContentAccess::EView:
            return ContentAccess::EView;
        case ContentAccess::EExecute:
            return ContentAccess::EExecute;
        case ContentAccess::EPrint:
            return ContentAccess::EPrint;
        default:
            break;
        }
    return ContentAccess::EUnknown;
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::CheckPermission().
// This function check timebased contraints for a permission
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::CheckPermission(
                CDRMPermission* aPermission ,
                const TUint8& aPermType ,
                TTimeBased& aTime ,
                TTimeIntervalSeconds& aInterval,
                const RPointerArray<HBufC8>& aIndividual )
    {
#ifdef _DRM_TESTING
    TRAPD(err,WriteL(_L8("CheckPermission")));
#endif

    CDRMConstraint* c = NULL;
    ContentAccess::TIntent type = MapPermissionType(aPermType);
    TTime temp;
    temp.HomeTime();
    TUint32 reason( 0 );
    TBool validity = EFalse;

    aTime.SetStart(Time::MinTTime());
    aTime.SetEnd(Time::MinTTime());
    if (aPermission->iAvailableRights & type)
        {
        c = aPermission->ConstraintForIntent(type);
        if (!c)
            {
            return;
            }
        if (aPermission->iAvailableRights & ERightsTopLevel)
            {
            CDRMConstraint* temp = aPermission->TopLevelConstraint();
            c->Merge(*temp);
            }

        validity = c->Valid( temp, aIndividual, reason );
        if ( !validity && reason & EConstraintIndividual )
            {
            return;
            }
        if (c->iActiveConstraints==EConstraintNone ||
            c->iActiveConstraints == EConstraintIndividual )
            {
            aTime.SetStart(Time::MinTTime());
            aTime.SetEnd(Time::MaxTTime());
            return;
            }
        if (c->iActiveConstraints & EConstraintInterval)
            {
            if (c->iIntervalStart == Time::NullTTime())
                {
                aInterval = c->iInterval;
                }
            else
                {
                aTime.SetStart(c->iIntervalStart);
                aTime.SetEnd(c->iIntervalStart + c->iInterval);
                }
            }
        if (c->iActiveConstraints & EConstraintStartTime)
            {
            if ( !(c->iActiveConstraints & EConstraintInterval ) )
                {
                aTime.SetStart( c->iStartTime );
                if (c->iActiveConstraints&EConstraintEndTime)
                    {
                    aTime.SetEnd( c->iEndTime );
                    }
                else
                    {
                    aTime.SetEnd( Time::MaxTTime() );
                    }
                }
            else
                {
                aTime.SetStart( aTime.StartTime() > c->iStartTime ? aTime.StartTime() : c->iStartTime );
                if (c->iActiveConstraints&EConstraintEndTime)
                    {
                    aTime.SetEnd( aTime.EndTime() < c->iEndTime ? aTime.EndTime() : c->iEndTime );
                    }
                }
            }
        else
            {
            if (c->iActiveConstraints==EConstraintEndTime)
                {
                if ( !(c->iActiveConstraints & EConstraintInterval ) )
                    {
                    aTime.SetStart(Time::MinTTime());
                    aTime.SetEnd( c->iEndTime );
                    }
                else
                    {
                    aTime.SetEnd( aTime.EndTime() < c->iEndTime ? aTime.EndTime() : c->iEndTime );
                    }
                }
            }
        }
#ifdef _DRM_TESTING
    TRAP(err,WriteL(_L8("Start Time:")));
    TRAP(err,WriteTimeL(aTime.StartTime()));
    TRAP(err,WriteL(_L8("End Time:")));
    TRAP(err,WriteTimeL(aTime.EndTime()));
#endif
    }


// -----------------------------------------------------------------------------
// CDRMHelperServer::CheckExpirationL().
// This function check the time for rights to expire
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::CheckExpirationL(
        const TDesC8& aUri ,
        const TUint8& aPermType ,
        const TUint8& aRegType ,
        TTime& aEndTime )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("CheckExpiration"));
#endif


    RPointerArray<CDRMPermission> rights;
    RArray<TTimeBased> timeList;
    TBool stop = EFalse;
    TInt i = 0;
    TInt err = 0;
    TTimeIntervalYears years(KTimeIntervalYears);
    TTime time = Time::MinTTime();
    TTimeBased item;
    TTimeIntervalSeconds interval = 0;
    RPointerArray<HBufC8> imsi;

    aEndTime = Time::MinTTime();


    TRAP( err , iDrm.GetDBEntriesL(aUri, rights) );
    CleanupClosePushL(rights);
    if ( err == KErrCANoRights || err == KErrCANoPermission )
        {
        aEndTime = Time::MinTTime();
        err = KErrNone;
        stop = ETrue;
        }
    User::LeaveIfError(err);

    TRAP( err, iDrm.GetSupportedIndividualsL( imsi ) );

    if ( err )
        {
        imsi.ResetAndDestroy();
        }

    time.HomeTime();
    // Goes through all the rights associated to this specific URI
    CleanupClosePushL(timeList);

    for (i = 0; i < rights.Count() && !stop; i++)
        {
        CheckPermission( rights[i], aPermType, item, interval ,imsi );
        if (interval.Int()>0 ||
            (item.StartTime()==Time::MinTTime()&&item.EndTime()==Time::MaxTTime()))
            {
            stop = ETrue;
            aEndTime = Time::MaxTTime();
            }
        else
            {
            if (aRegType == CDRMHelperServer::EActive && item.StartTime()>time)
                {
                // dont count future time for active usage
                }
            else
                {
                err = timeList.Append( item );
                if ( err )
                    {
                    rights.ResetAndDestroy();
                    imsi.ResetAndDestroy();
                    }
                User::LeaveIfError(err);
                }
            }
        }
    rights.ResetAndDestroy();
    imsi.ResetAndDestroy();

    // if there were no inactivated interval- or full-rights then calculate the expiration date
    // based on what we stored to timeList
    if( !stop )
        {
        time.HomeTime(); // preset time to current time. This is what we compare against.
        TTime temp;
        TTime pastTime;
        temp = time;
        pastTime = Time::MinTTime();

        TBool action = ETrue;

        // Loop while there are still items in the list and we have done something
        while( action && timeList.Count() > 0 )
            {
            action = EFalse;
            for ( i = 0 ; i < timeList.Count() ; i++ ) // go through the whole timeList
                {
#ifdef _DRM_TESTING
                _LIT8(KCount , "time list count: %d");
                TBuf8<40> buf;
                buf.Format( KCount , timeList.Count());
                WriteL( buf );
#endif
                if ( timeList[i].StartTime() <= time && timeList[i].EndTime() > time )
                    {
                    // Case1: valid rights
                    time = timeList[i].EndTime();
                    timeList.Remove(i);
                    action = ETrue;
#ifdef _DRM_TESTING
                    WriteL(_L8("case 1:"));
                    WriteTimeL( time );
#endif
                    }
                else if ( timeList[i].StartTime() <= time && timeList[i].EndTime() <= time )
                    {
                    // Case2: expired rights
                    if (timeList[i].EndTime()>pastTime) // just in case there is no valid rights
                        {
                        pastTime = timeList[i].EndTime(); // save the latest end time from the expired rights
                        }
                    timeList.Remove(i);
                    action = ETrue;
#ifdef _DRM_TESTING
                    WriteL(_L8("case 2:"));
#endif
                    }
                else if ( timeList[i].StartTime() > time && timeList[i].EndTime() <= time )
                    {
                    // Case3: Illegal case. Start time after end-time.
                    timeList.Remove(i);
                    action = ETrue;
#ifdef _DRM_TESTING
                    WriteL(_L8("case 3:"));
#endif
                    }
                else
                    {
                    // Case4: Only thing left is the not yet valid -rights
#ifdef _DRM_TESTING
                    WriteL(_L8("case 4:"));
#endif
                    }
                }
            }

        if (temp<time)
            {
            aEndTime = time;   // time has been changed, so we use it, otherwise it means there is no valid rights
            }
        else
            {
            aEndTime = pastTime;
            }


#ifdef _DRM_TESTING
        WriteL(_L8("expiration date:"));
        WriteTimeL( aEndTime );
#endif
        }

#ifdef _DRM_TESTING
    WriteL(_L8("Endtime calculation is done:"));
    WriteTimeL( aEndTime );
#endif

    timeList.Reset();
    CleanupStack::PopAndDestroy(&timeList); // timeList
    CleanupStack::PopAndDestroy(&rights); // rights
    }


// -----------------------------------------------------------------------------
// CDRMHelperServer::SetKeyValueL().
// This function set CenRep key value
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::SetKeyValueL( const TDesC8& aKeyValue , const TUint8& aRegType )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("SetKeyValueL"));
    WriteL( aKeyValue );
#endif

    // Connecting and initialization:
    CRepository* repository = CRepository::NewL(KCRUidDRMHelperServer);
    CleanupStack::PushL( repository );
    if ( aRegType == CDRMHelperServer::EActive )
        {
        User::LeaveIfError(repository->Set(KDRMHelperServerNotification, aKeyValue));
        }
    else if ( aRegType == CDRMHelperServer::EPassive )
        {
        User::LeaveIfError(repository->Set(KDRMHelperServerNotificationPassive, aKeyValue));
        }
    CleanupStack::PopAndDestroy(repository); // repository
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::StoreL().
// This function store internal list into data file
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::StoreL()
    {

#ifdef _DRM_TESTING
    WriteL(_L8("StoreL"));
#endif

    RFileWriteStream file;
    User::LeaveIfError(file.Replace( iFs , DataFileL(iFs).FullName() , EFileWrite ));
    file.PushL();
    ExternalizeL(file);
    file.CommitL();
    CleanupStack::PopAndDestroy(&file);//file
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::RestoreL().
// This function restore internal list from data file
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::RestoreL()
    {
#ifdef _DRM_TESTING
    WriteL(_L8("RestoreL"));
#endif

    TInt err = 0;
    RFileReadStream file;

    RFile test;
    err = test.Open( iFs , DataFileL(iFs).FullName() , EFileRead );
    if ( !err )
        {
        CleanupClosePushL(test);
        TInt size = 0;
        err = test.Size(size);
        User::LeaveIfError(err);
        CleanupStack::PopAndDestroy(&test); //test
        if (size == 0)
            {
            StoreL();
            }
        }
    else if ( err == KErrNotFound )
        {
        StoreL();
        }
    else
        {
        User::Leave(err);
        }

    User::LeaveIfError(file.Open( iFs , DataFileL(iFs).FullName() , EFileRead ));
    file.PushL();
    InternalizeL(file);
    CleanupStack::PopAndDestroy(&file); //file

#ifdef _DRM_TESTING
    WriteL(_L8("RestoreL->End"));
#endif
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::ExternalizeL().
// This function externalize internal list
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::ExternalizeL(RWriteStream& aStream) const
    {
#ifdef _DRM_TESTING
    WriteL(_L8("ExternalizeL"));
#endif
    aStream << *iEndTimeFactory;
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::InternalizeL().
// This function internalizeL internal list
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::InternalizeL(RReadStream& aStream)
    {
#ifdef _DRM_TESTING
    WriteL(_L8("InternalizeL"));
#endif
    iEndTimeFactory->InternalizeL(aStream);
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::GetNotificationL().
// This function register event listening to DRM notifier
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::GetNotificationL(const TDesC8& aUri)
    {
#ifdef _DRM_TESTING
    WriteL(_L8("GetNotificationL"));
#endif
    iNotifier->RegisterEventObserverL( *this , KEventAddRemove , aUri );
    iNotifier->RegisterEventObserverL( *this , KEventModify , aUri );
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::RemoveNotificationL().
// This function remove event listening to DRM notifier
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::RemoveNotificationL(const TDesC8& aUri)
    {
#ifdef _DRM_TESTING
    WriteL(_L8("RemoveNotificationL"));
#endif
    iNotifier->UnRegisterEventObserverL( *this , KEventAddRemove , aUri );
    iNotifier->UnRegisterEventObserverL( *this , KEventModify , aUri );
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::HandleEventL().
// This function handle event from DRM notifier
// -----------------------------------------------------------------------------
//
void CDRMHelperServer::HandleEventL( MDRMEvent* aEvent )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("HandleEventL"));
#endif

    HBufC8* contentID = 0;
    TDRMEventType event;

    aEvent->GetEventType(event);

#ifdef _DRM_TESTING
            WriteL(_L8("Notifier:"),event);
#endif
    switch(event)
        {
        case KEventAddRemove:
            {
#ifdef _DRM_TESTING
            WriteL(_L8("Notifier: EventAddRemove"));
#endif
            contentID = REINTERPRET_CAST( CDRMEventAddRemove* , aEvent )->GetContentIDL();
            CleanupStack::PushL( contentID );
#ifdef _DRM_TESTING
            WriteL(contentID->Des());
#endif
            switch( REINTERPRET_CAST( CDRMEventAddRemove* , aEvent )->Status() )
                {
                case ERightsObjectRecieved:
                    {
#ifdef _DRM_TESTING
                    WriteL(_L8("Notifier: RightsObjectRecieved"));
#endif
                    UpdateL( contentID->Des() );
                    }
                    break;
                case ERightsObjectDeleted:
                    {
#ifdef _DRM_TESTING
                    WriteL(_L8("Notifier: RightsObjectDeleted"));
#endif
                    UpdateL( contentID->Des() );
                    }
                    break;
                case ERightsObjectDeletedAll:
                    {
#ifdef _DRM_TESTING
                    WriteL(_L8("Notifier: RightsObjectDeletedAll"));
#endif
                    UpdateL( contentID->Des() );
                    }
                    break;
                default:
                    {
#ifdef _DRM_TESTING
                    WriteL(_L8("Notifier: Unknown event"));
#endif
                    }
                    break;
                }
            CleanupStack::PopAndDestroy( contentID );
            }
            break;
        case KEventModify:
            {
#ifdef _DRM_TESTING
            WriteL(_L8("Notifier: EventModify"));
#endif
            contentID = REINTERPRET_CAST( CDRMEventModify* , aEvent )->GetContentIDL();
            CleanupStack::PushL( contentID );
#ifdef _DRM_TESTING
            WriteL(contentID->Des());
#endif
            UpdateL( contentID->Des() );
            CleanupStack::PopAndDestroy( contentID );
            }
            break;
        case KEventTimeChange:
            {
#ifdef _DRM_TESTING
            WriteL(_L8("Notifier: EventTimeChange"));
#endif
            UpdateL();
            }
            break;
        default:
            {
#ifdef _DRM_TESTING
            WriteL(_L8("Notifier: Unknown event"));
#endif
            }
            break;

        }
    return;
    }

// -----------------------------------------------------------------------------
// CDRMHelperServer::UiTimeChanged
// CPeriodic callback function to update the clock
// -----------------------------------------------------------------------------
//
TInt CDRMHelperServer::UiTimeChanged(void* aPointer)
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("UiTimeChanged")));
#endif
    CDRMHelperServer* current = static_cast<CDRMHelperServer*>(aPointer);
    TInt err = 0;
    if(current)
        {
        if(current->iUiTimeNotifier->Change() & EChangesSystemTime)
            {
            TRAP_IGNORE( current->UpdateL() );
            }
        if(current->iUiTimeNotifier->Change() & EChangesLocale)
            {
            // time zone is changed
            }
        }
#ifdef _DRM_TESTING
    TRAP(r,WriteL(_L8("UiTimeChanged:->End"),current->iUiTimeNotifier->Change()));
#endif
    return err;
    };


TInt E32Main()
    {
    return CDRMHelperServer::Startup();
    }
//  End of File
