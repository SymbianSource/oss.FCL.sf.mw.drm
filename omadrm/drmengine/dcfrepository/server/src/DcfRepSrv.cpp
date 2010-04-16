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
* Description:  server implementation
*
*/


// INCLUDE FILES
#include    "DcfRepSrv.h"
#include    "DcfRepSrvSes.h"
#include    "DcfRepCommon.h"
#include    "Pair.h"
#include    "FileScan.h"
#include    <e32std.h>
#include    <e32base.h>
#include    <f32file.h>
#include    <caf/data.h>
#include    <caf/content.h>
#include    <Oma2Agent.h> // oma2agent
#include    "oma2dcf.h"
#include    "Oma1Dcf.h"
#include    "drmlog.h"
#include <starterclient.h>

#ifdef RD_MULTIPLE_DRIVE
#include    <driveinfo.h>
#endif

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
_LIT(KSqlUpdate1 , "select filename,position,cid,group,ttid from dcf where filename = '");
_LIT(KSqlUpdate2 , "' and position = ");
_LIT(KSqlInsert , "select filename,position,cid,group,ttid from dcf");
_LIT(KSqlDeleteAll , "delete from dcf");
_LIT(KGroupIdReplacement, "cid:group@localhost");
// MACROS
// LOCAL CONSTANTS AND MACROS

#ifdef RD_MULTIPLE_DRIVE
_LIT( KDbFileLoc, "%c:\\system\\data\\DcfRepDb" );
#endif

// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
LOCAL_C TInt CreateDataBase(RFs& aFs);

#ifndef RD_MULTIPLE_DRIVE
LOCAL_C TParse DataFile();
#else // RD_MULTIPLE_DRIVE
LOCAL_C TParse DataFile( RFs& aFs );
#endif

LOCAL_C void CreateTablesL(RDbDatabase& aDatabase);
LOCAL_C TInt CreateDataBasePath(RFs& aFs);
// FORWARD DECLARATIONS

//#define _DRM_TESTING
//#define _MEM_HEAP_USAGE

// ============================ LOCAL FUNCTIONS ===============================
#ifdef _DRM_TESTING
LOCAL_C void WriteLogL( const TDesC8& text , RFs &aFs );
LOCAL_C void WriteFileL( const TDesC8& text , RFs &aFs , const TDesC& aName );
LOCAL_C void CreateLogL();
LOCAL_C void WriteL( const TDesC8& aText );
LOCAL_C void WriteL( const TDesC8& aText , TInt aErr );
LOCAL_C void WriteCurrentTimeL();

#ifdef _MEM_HEAP_USAGE
LOCAL_C void WriteHeapAllocL();
#endif //_MEM_HEAP_USAGE

#endif //_DRM_TESTING

// -----------------------------------------------------------------------------
// Testing stuff
// -----------------------------------------------------------------------------
//
#ifdef _DRM_TESTING
LOCAL_C void WriteLogL( const TDesC8& text , RFs &aFs )
    {
    _LIT( KLogFile , "c:\\CDcfRepSrv.txt" );
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
    User::LeaveIfError( file.Replace( fs , _L("c:\\CDcfRepSrv.txt") , EFileWrite ) );
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
// DataFile
// Return TParse which contain database file name
// -----------------------------------------------------------------------------
//
#ifdef RD_MULTIPLE_DRIVE
LOCAL_C TParse DataFile( RFs& aFs )
#else
LOCAL_C TParse DataFile()
#endif
    {
#ifdef _DRM_TESTING
    TRAP_IGNORE( WriteL(_L8("DataFile")) );
#endif

    TParse p;

#ifndef RD_MULTIPLE_DRIVE

    p.Set(KDbFileLocation,NULL,NULL);

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );

    aFs.DriveToChar( driveNumber, driveLetter );

    TFileName dbFile;
    dbFile.Format( KDbFileLoc, (TUint)driveLetter );

    p.Set(dbFile,NULL,NULL);

#endif

    return p;
    }


// -----------------------------------------------------------------------------
// CreateTablesL
// Create table through database session
// -----------------------------------------------------------------------------
//
LOCAL_C void CreateTablesL(RDbDatabase& aDatabase)
    {
#ifdef _DRM_TESTING
    WriteL(_L8("CreateTablesL"));
#endif

    CDbColSet* columns = CDbColSet::NewLC();

    //filename columns
    TDbCol filename(KColFilename,EDbColLongText16);
    filename.iAttributes = TDbCol::ENotNull;
    columns->AddL(filename);

    //position columns
    TDbCol position(KColPosition,EDbColUint16);
    position.iAttributes = TDbCol::ENotNull;
    columns->AddL(position);

    //content id columns
    TDbCol cid(KColCid,EDbColLongText16);
    cid.iAttributes = TDbCol::ENotNull;
    columns->AddL(cid);

    //content group id columns
    TDbCol group(KColGroupId,EDbColLongText16);
    columns->AddL(group);

    //transaction id columns
    TDbCol ttid(KColTtid,EDbColText16,KTtidLen);
    columns->AddL(ttid);

    aDatabase.CreateTable( KTable,*columns);
    CleanupStack::PopAndDestroy(columns); //columns

    }

// -----------------------------------------------------------------------------
// CreateDataBasePath
// Create database with client side database access
// -----------------------------------------------------------------------------
//
LOCAL_C TInt CreateDataBasePath(RFs& aFs)
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("CreateDataBasePath")));
#endif

    TInt err = 0;

#ifndef RD_MULTIPLE_DRIVE

    err = aFs.MkDirAll( DataFile(). DriveAndPath() );

#else //RD_MULTIPLE_DRIVE

    err = aFs.MkDirAll( DataFile( aFs ). DriveAndPath() );

#endif

    return err;
    }

// -----------------------------------------------------------------------------
// CreateDataBase
// Create database with client side database access
// -----------------------------------------------------------------------------
//
LOCAL_C TInt CreateDataBase(RFs& aFs)
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("CreateDataBase")));
#endif

    RDbNamedDatabase database;
    TInt err = 0;
    TFileName file;

#ifndef RD_MULTIPLE_DRIVE

    file = DataFile().FullName();


#else // RD_MULTIPLE_DRIVE

    file = DataFile( aFs ).FullName();

#endif

    if (!err)
        {
#ifdef _DRM_TESTING
        TRAP(r,WriteL(_L8("CreateDataBase->DataFile"),err));
#endif
        err = CreateDataBasePath(aFs);

        err = database.Create(aFs,file);
        if (!err)
            {
#ifdef _DRM_TESTING
            TRAP(r,WriteL(_L8("CreateDataBase->database.Create"),err));
#endif
            TRAP( err , CreateTablesL(database) );
#ifdef _DRM_TESTING
            TRAP(r,WriteL(_L8("CreateDataBase->CreateTablesL"),err));
#endif
            }
        database.Close();
        }
    return err;
    }

// -----------------------------------------------------------------------------
// From8To16
// transfer buf from 8 bit to 16 bit
// -----------------------------------------------------------------------------
//
LOCAL_C TInt From8To16( const TDesC8& a8 , HBufC16*& a16 )
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("From8To16")));
#endif

    TInt err = KErrNone;
    a16 = HBufC::NewMax( a8.Length() );
    if ( a16 )
        {
        TPtr ptr( a16->Des() );
        ptr.SetLength( a8.Length() );
        for (TInt i = 0 ; i<a8.Length() ; i++ )
            {
            ptr[i] = ( unsigned char ) a8[i];
            }
        }
    else err = KErrNoMemory;
    return err;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDcfRepSrv::CDcfRepSrv
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDcfRepSrv::CDcfRepSrv():CServer2( EPriorityStandard ),
    iScan(NULL),
    iCidList(NULL),
    iPairList(NULL),
    iState(EStateIdle),
    iArmed( EFalse )
    {

    }

// -----------------------------------------------------------------------------
// CDcfRepSrv::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDcfRepSrv::ConstructL()
    {
#ifdef _DRM_TESTING
    CreateLogL();
    WriteL(_L8("ConstructL"));
#endif
    StartL(KDcfRepSrvName);
    User::LeaveIfError(iFs.Connect());
    iScan = CFileScan::NewL(iFs);
    OpenDatabaseL();

    User::RenameThread(KDcfRepSrvName);

    iProcWatcher = CProcWatcher::NewL( *this, _L( "*RightsServer*" ), _L( "RightsServer" ) );
    iProcWatcher->StartWatching();
    iArmed = ETrue;
    }


// -----------------------------------------------------------------------------
// CDcfRepSrv::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDcfRepSrv* CDcfRepSrv::NewLC()
    {
    CDcfRepSrv* self = new( ELeave ) CDcfRepSrv;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// Destructor
CDcfRepSrv::~CDcfRepSrv()
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("~CDcfRepSrv")));
#endif

    delete iScan;
    CleanScanInternal();
    CleanDatabase();
    iFs.Close();
    for (TInt i = 0 ; i < iMessageList.Count() ; i++ )
        {
        if ( iMessageList[i]!=NULL && !(iMessageList[i]->IsNull()) )
            {
            iMessageList[i]->Complete(KErrCancel);
            }

        }
    iMessageList.ResetAndDestroy();
    iMessageList.Close();
    }

// -----------------------------------------------------------------------------
// CDcfRepSrv::NewSessionL
// Called when a client requires a new instance.
// -----------------------------------------------------------------------------
CSession2* CDcfRepSrv::NewSessionL(const TVersion &aVersion,
    const RMessage2& /*aMessage*/ ) const
    {
#ifdef _DRM_TESTING
    WriteL(_L8("NewSessionL"));
#endif
    // check we're the right version
    if (!User::QueryVersionSupported(TVersion(KDcfRepMajorVersionNumber,
                KDcfRepMinorVersionNumber,
                KDcfRepBuildVersionNumber),
            aVersion))
        {
        User::Leave(KErrNotSupported);
        }

    // make new session
    return CDcfRepSrvSes::NewL();
    }


// -----------------------------------------------------------------------------
// Startup().
// This function starts the actual DCF Repository server after initializing
// the cleanup stack and active scheduler.
// Returns: TInt: Symbian OS error code.
// -----------------------------------------------------------------------------
//

TInt CDcfRepSrv::Startup( void )
    {
    TInt err = 0;
    TBool clientIsWaiting = EFalse;
    RSemaphore semaphore;

    CTrapCleanup* cleanupStack = CTrapCleanup::New();

    if (cleanupStack == NULL)
        {
        PanicServer(ECreateTrapCleanup);
        }

    // check if the client wants to be signaled that we are ready
    if ( semaphore.OpenGlobal(KDcfRepSemaphoreName) == KErrNone )
        {
        clientIsWaiting = ETrue;
        }

    TRAP(err, StartupL());

    // release client side waiting
    if (clientIsWaiting)
        {
        semaphore.Signal();
        semaphore.Close();
        }

    if (err != KErrNone)
        {
        PanicServer(ESrvCreateServer);
        }

    delete cleanupStack;
    cleanupStack = NULL;
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// StartupL().
// This function starts the actual DCF Repository after initializing scheduler
// -----------------------------------------------------------------------------
//

void CDcfRepSrv::StartupL()
    {
    // Construct active scheduler
    CActiveScheduler* activeScheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL(activeScheduler) ;

    // Install active scheduler
    // We don't need to check whether an active scheduler is already installed
    // as this is a new thread, so there won't be one
    CActiveScheduler::Install(activeScheduler);

    // Construct our server
    CDcfRepSrv::NewLC();

    // Start handling requests
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy(2);    //  activeScheduler and anonymous CDcfRepSrv
    }

void CDcfRepSrv::PanicServer(TDcfRepSrvPanic aPanic)
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("PanicServer")));
    r =  0;
#endif

    User::Panic(KDcfRepSrv, aPanic);
    }


// -----------------------------------------------------------------------------
// AddFileL()
// The function will add a file info into database
// -----------------------------------------------------------------------------
//
void CDcfRepSrv::AddFileL( const TDesC& aFile )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("AddFileL"));
#endif

    TInt err = 0;
    TInt state = iState;
    TBool flag = EFalse;

    // to process the file as doing scanning fully for file system
    iState = EStateFullScan;
    err = ProcessFile(aFile , flag);
    // change the state back
    iState = state;
    User::LeaveIfError(err);
    if ( flag == ENoDcf )
        {
        User::Leave(KErrNotSupported);
        }
    else if (flag == EOma2Dcf)
        {
        AddDomainRoL(aFile);
        }
    }

// -----------------------------------------------------------------------------
// AddDomainRoL()
// The function will add a file info into database
// -----------------------------------------------------------------------------
//
void CDcfRepSrv::AddDomainRoL( const TDesC& aFile )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("AddDomainRoL"));
#endif

    __UHEAP_MARK;
    RFile file;
    TPtr8 ptr(NULL,0,0);
    CContent* content =NULL;
    User::LeaveIfError(file.Open(iFs, aFile, EFileShareReadersOrWriters | EFileWrite ) );
    CleanupClosePushL(file);
    content = CContent::NewL(file);
    CleanupStack::PushL(content);
    User::LeaveIfError(content->AgentSpecificCommand(EEmbedDomainRo,
            KNullDesC8, ptr ));
    CleanupStack::PopAndDestroy(2); // content, file
    __UHEAP_MARKEND;
    }


// -----------------------------------------------------------------------------
// OpenDatabaseL()
// The function will open database
// -----------------------------------------------------------------------------
//
void CDcfRepSrv::OpenDatabaseL()
    {
#ifdef _DRM_TESTING
    WriteL(_L8("OpenDatabaseL"));
#endif


    TInt err = CreateDataBase(iFs);
    if ( err==KErrAlreadyExists )
        {
        err = KErrNone;
        }

    User::LeaveIfError(iDbs.Connect());

#ifndef RD_MULTIPLE_DRIVE

    User::LeaveIfError(iDb.Open(iDbs,DataFile().FullName()));

#else // RD_MULTIPLE_DRIVE

    User::LeaveIfError(iDb.Open(iDbs,DataFile(iFs).FullName()));

#endif
    }


// -----------------------------------------------------------------------------
// CleanTableL()
// The function will empty the talbe
// -----------------------------------------------------------------------------
//
void CDcfRepSrv::CleanTableL()
    {
#ifdef _DRM_TESTING
    WriteL(_L8("CleanTableL"));
#endif

    User::LeaveIfError(iDb.Execute(KSqlDeleteAll()));
    }

// -----------------------------------------------------------------------------
// ProcessFile()
// This function check if the file is DRM protected and add it into database.
// -----------------------------------------------------------------------------
//
TInt CDcfRepSrv::ProcessFile(const TDesC& aFile , TInt& aType)
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("ProcessFile")));
#endif

    TInt err = 0;
    aType = ENoDcf;

    TRAP(err , CheckFileL(aFile , aType));
    if (aType)
        {
        TRAP(err , StoreFileL(aFile,aType));
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CheckFileL()
// check if it is target file
// -----------------------------------------------------------------------------
//
void CDcfRepSrv::CheckFileL(const TDesC& aFile , TInt& aType)
    {
#ifdef _DRM_TESTING
    WriteL(_L8("CheckFileL"));
#endif

    RFile f;
    TInt pos = 0;
    TBuf8<256> buf;

    switch(iState)
        {
        case EStateFullScan:
        case EStateScan:
                {
                User::LeaveIfError(f.Open(iFs,aFile,EFileRead|EFileShareReadersOrWriters));
                CleanupClosePushL(f);
                User::LeaveIfError(f.Seek(ESeekStart,pos));
                User::LeaveIfError(f.Read(0,buf));
                CleanupStack::PopAndDestroy(&f);
                if (COma1Dcf::IsValidDcf(buf))
                    {
                    aType = EOma1Dcf;
                    }
#ifdef __DRM_OMA2
                else if (COma2Dcf::IsValidDcf(buf))
                    {
                    aType = EOma2Dcf;
                    }
#endif
                else
                    {
                    aType = ENoDcf;
                    }

                }
            break;
        case EStateSetTtid:
                {
                TParse p;
                User::LeaveIfError(p.Set(aFile,NULL,NULL));
                if ( !p.Ext().Compare( KOma2DcfExtension ) ||
                    !p.Ext().Compare( KOma2DcfExtensionAudio ) ||
                    !p.Ext().Compare( KOma2DcfExtensionVideo ) )
                    {
                    User::LeaveIfError(f.Open(iFs,aFile,EFileRead|EFileShareReadersOrWriters));
                    CleanupClosePushL(f);
                    User::LeaveIfError(f.Seek(ESeekStart,pos));
                    User::LeaveIfError(f.Read(0,buf));
                    CleanupStack::PopAndDestroy(&f);
                    if (COma1Dcf::IsValidDcf(buf))
                        {
                        aType = EOma1Dcf;
                        }
#ifdef __DRM_OMA2
                    else if (COma2Dcf::IsValidDcf(buf))
                        {
                        aType = EOma2Dcf;
                        }
#endif
                    else
                        {
                        aType = ENoDcf;
                        }
                    }
                }
            break;
        default:
            ;
        }
    }

// -----------------------------------------------------------------------------
// StoreFile()
// save file info into database
// -----------------------------------------------------------------------------
//
void CDcfRepSrv::StoreFileL(const TDesC& aFile , TInt aType)
    {
#ifdef _DRM_TESTING
    WriteL(_L8("StoreFileL"));
#endif

    RFile f;
    User::LeaveIfError(f.Open(iFs,aFile,EFileRead|EFileShareReadersOrWriters));
    CleanupClosePushL(f);
    TInt i = 0;
    TInt setTtid = -1;
    HBufC16* cid = NULL;
    HBufC16* ttid = NULL;
    HBufC16* group = NULL;

    if (aType == EOma1Dcf)
        {
        COma1Dcf* d = COma1Dcf::NewL(f);
        CleanupStack::PushL(d);
        User::LeaveIfError( From8To16( *(d->iContentID) , cid ) );
        CleanupStack::PopAndDestroy(d);
        CleanupStack::PushL(cid);
        ttid = HBufC::NewMaxLC(16);
        *ttid = KNullDesC16;
        UpdateDatabaseL(aFile , KStartPosition , *cid , KNullDesC() , *ttid);
        if (iCidList)
            {
            for (i = 0; i<iCidList->Count(); i++)
                {
                HBufC* temp = NULL;
                User::LeaveIfError( From8To16(*((*iCidList)[i]) , temp ) );
                CleanupStack::PushL(temp);
                if (!temp->Compare(*cid))
                    {
                    delete (*iCidList)[i];
                    iCidList->Remove(i);
                    CleanupStack::PopAndDestroy(temp);
                    break;
                    }
                CleanupStack::PopAndDestroy(temp); //temp
                }
            if (iCidList->Count()<=0)
                {
                CompleteScanning(KErrNone);
                }
            }
        CleanupStack::PopAndDestroy(2); //cid,ttid
        }
    else if (aType == EOma2Dcf)
        {
        COma2Dcf* d = COma2Dcf::NewL(f);
        CleanupStack::PushL(d);

        // Set group ID and content ID for this entry
        if (d->iGroupId)
            {
            User::LeaveIfError( From8To16( *(d->iGroupId) , group ) );

            // replace the content ID for this entry with a placeholder to prevent
            // that the file is listed wrongly under the group ID
            cid = KGroupIdReplacement().AllocL();
            }
        else
            {
            User::LeaveIfError( From8To16( *(d->iContentID) , cid ) );
            group = HBufC::NewMaxL(16);
            *group = KNullDesC16;
            }
        CleanupStack::PushL(cid);
        CleanupStack::PushL(group);

        if (iState == EStateSetTtid)
            {
            for (i = 0; iPairList && i<iPairList->Count() && !ttid; i++)
                {
                HBufC* temp = NULL;
                User::LeaveIfError( From8To16(*((*iPairList)[i]->iCid) , temp ) );
                CleanupStack::PushL(temp);
                if (!temp->Compare(*cid))
                    {
                    User::LeaveIfError( From8To16(*((*iPairList)[i]->iTtid) , ttid ) );

                    // EFileWrite is needed for this case
                    // So we cannot do it here. we must close the file
                    setTtid = i;
                    }
                CleanupStack::PopAndDestroy(temp); //temp
                }
            }
        if (!ttid)
            {
            if (d->iTransactionTracking)
                {
                User::LeaveIfError( From8To16( *(d->iTransactionTracking) , ttid ) );
                }
            else
                {
                ttid = HBufC::NewMaxL(16);
                *ttid = KNullDesC16;
                }
            }
        CleanupStack::PushL(ttid);

        UpdateDatabaseL(aFile , KStartPosition , *cid , *group , *ttid);
        if (iCidList)
            {
            for (i = 0; i<iCidList->Count(); i++)
                {
                HBufC* temp = NULL;
                User::LeaveIfError( From8To16(*((*iCidList)[i]) , temp ) );
                CleanupStack::PushL(temp);
                if (!temp->Compare(*cid))
                    {
                    delete (*iCidList)[i];
                    iCidList->Remove(i);
                    CleanupStack::PopAndDestroy(temp);
                    break;
                    }
                CleanupStack::PopAndDestroy(temp); //temp
                }
            if (iCidList->Count()<=0)
                {
                CompleteScanning(KErrNone);
                }
            }
        CleanupStack::PopAndDestroy(4); // group,ttid,cid,d
        }

    CleanupStack::PopAndDestroy(&f); // f

    if (setTtid>=0)
        {
        ResetTtidL( aFile , *((*iPairList)[setTtid]->iTtid));
        }
    }

// -----------------------------------------------------------------------------
// ResetTtidL()
// save new ttid into file
// -----------------------------------------------------------------------------
//
void CDcfRepSrv::ResetTtidL(
    const TDesC& aFile,
    const TDesC8& aTtid
    )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("ResetTtidL"));
#endif

    RFile f;
    User::LeaveIfError(f.Open(iFs,aFile, EFileWrite|EFileShareReadersOrWriters ));
    CleanupClosePushL(f);
    COma2Dcf* d = COma2Dcf::NewL(f);
    CleanupStack::PushL(d);
    ResetTtidL(d,aTtid);
    CleanupStack::PopAndDestroy(2); // f,d
    }

// -----------------------------------------------------------------------------
// ResetTtidL()
// save new ttid into file
// -----------------------------------------------------------------------------
//
void CDcfRepSrv::ResetTtidL(
    COma2Dcf* aDcf,
    const TDesC8& aTtid
    )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("ResetTtidL"));
#endif

    aDcf->SetTransactionIdL(aTtid);
    }

// -----------------------------------------------------------------------------
// UpdateDatabaseL()
// save info into database
// -----------------------------------------------------------------------------
//
void CDcfRepSrv::UpdateDatabaseL(
    const TDesC& aFile,
    TInt aPos,
    const TDesC& aCid ,
    const TDesC& aGroupId ,
    const TDesC& aTtid)
    {
#ifdef _DRM_TESTING
    WriteL(_L8("UpdateDatabaseL"));
#endif

    HBufC* sql = NULL;
    TPtr ptr(NULL,0,0);
    TBuf<4> num;
    sql = HBufC::NewMaxLC(aFile.Length()+200);
    ptr.Set(sql->Des());
    ptr.SetLength(0);
    ptr.Append(KSqlUpdate1);
    ptr.Append(aFile);
    ptr.Append(KSqlUpdate2);
    num.Num(aPos);
    ptr.Append(num);

    iView.Close();

    User::LeaveIfError(
        iView.Prepare(
            iDb,TDbQuery(ptr),
            TDbWindow::EUnlimited,
            RDbView::EUpdatable
            )
        );
    User::LeaveIfError(iView.EvaluateAll());
    if (iView.FirstL())
        {
        iView.UpdateL();
        iView.SetColL(KOrdCid,aCid);
        iView.SetColL(KOrdGroupId,aGroupId);
        iView.SetColL(KOrdTtid,aTtid);
        iView.PutL();
        }
    else
        {
        iView.Reset();
        iView.Close();
        iView.Prepare(
            iDb,TDbQuery(KSqlInsert),
            TDbWindow::EUnlimited,
            RDbView::EInsertOnly
            );
        iView.InsertL();
        iView.SetColL(KOrdFilename,aFile);
        iView.SetColL(KOrdPosition,aPos);
        iView.SetColL(KOrdCid,aCid);
        iView.SetColL(KOrdGroupId,aGroupId);
        iView.SetColL(KOrdTtid,aTtid);
        iView.PutL();
        }
    iView.Reset();
    iView.Close();
    CleanupStack::PopAndDestroy(sql); //sql
    }

// -----------------------------------------------------------------------------
// State()
// This function returns the state of the Server
// -----------------------------------------------------------------------------
//
TInt CDcfRepSrv::State() const
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("State")));
#endif

    return iState;
    }

// -----------------------------------------------------------------------------
// SetState()
// This function set the state of the Server
// -----------------------------------------------------------------------------
//
void CDcfRepSrv::SetState(TServerState aMode)
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("SetState")));
#endif

    iState = aMode;
    }


// -----------------------------------------------------------------------------
// CompleteScanning().
// This function complete request from client
// -----------------------------------------------------------------------------
//
void CDcfRepSrv::CompleteScanning(TInt aRet)
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("CompleteScanning")));
#endif

    for (TInt i = 0 ; i<iMessageList.Count() ; i++ )
        {
        if ( iMessageList[i] && !(iMessageList[i]->IsNull()) )
            {
            iMessageList[i]->Complete(aRet);
            }
        }
    iMessageList.ResetAndDestroy();

    CleanScanInternal();
    iState = EStateIdle;
    }

// -----------------------------------------------------------------------------
// SetMessage().
// This function save the incomplete message to server
// -----------------------------------------------------------------------------
//
RMessage2* CDcfRepSrv::SetMessageL(const RMessage2& aMessage)
    {
#ifdef _DRM_TESTING
    WriteL(_L8("SetMessageL"));
#endif
    RMessage2* m = NULL;
    m = new (ELeave) RMessage2(aMessage);
    iMessageList.AppendL(m);
    return m;
    }


// -----------------------------------------------------------------------------
// RemoveMessage().
// This function to remove the imcomplete message from server if it is still there
// when session is deleted
// -----------------------------------------------------------------------------
//
void CDcfRepSrv::RemoveMessageL( const RMessage2* aMessage )
    {
    for (TInt i = 0 ; i<iMessageList.Count() ; i++ )
        {
        if ( iMessageList[i] == aMessage )
            {
            delete iMessageList[i];
            iMessageList[i] = NULL;
            iMessageList.Remove( i );
            return;
            }
        }
    return;
    };


// -----------------------------------------------------------------------------
// SetCidList().
// This function set content ID List
// -----------------------------------------------------------------------------
//
void CDcfRepSrv::SetCidList(RPointerArray<HBufC8>*& aList)
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("SetCidList")));
#endif

    if (iCidList)
        {
        iCidList->ResetAndDestroy();
        iCidList->Close();
        delete iCidList;
        }
    iCidList = aList;
    }

// -----------------------------------------------------------------------------
// SetPairList().
// This function set list of pairs for content ID and transation ID
// -----------------------------------------------------------------------------
//
void CDcfRepSrv::SetPairList(RPointerArray<CPair>*& aList)
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("SetPairList")));
#endif

    if (iPairList)
        {
        iPairList->ResetAndDestroy();
        iPairList->Close();
        delete iPairList;
        }
    iPairList = aList;
    }


// -----------------------------------------------------------------------------
// Scan()
// This function scan the file system
// -----------------------------------------------------------------------------
//
TInt CDcfRepSrv::Scan()
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("Scan")));
#endif

    TInt err = 0;
    if (iState == EStateFullScan)
        {
        TRAP(err,CleanTableL());
        }
    err = iScan->SearchContent(this);
    if (err)
        {
        CleanScanInternal();
        iState = EStateIdle;
        }
    return err;
    }

void CDcfRepSrv::CleanScanInternal()
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("CleanScanInternal")));
#endif

    if (iCidList)
        {
        iCidList->ResetAndDestroy();
        iCidList->Close();
        delete iCidList;
        iCidList = NULL;
        }
    if (iPairList)
        {
        iPairList->ResetAndDestroy();
        iPairList->Close();
        delete iPairList;
        iPairList = NULL;
        }
    }

void CDcfRepSrv::CleanDatabase()
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("CleanDatabase")));
#endif

    iView.Close();
    iDb.Close();
    iDbs.Close();
    }

// -----------------------------------------------------------------------------
// CDcfRepSrv::WatchedObjectChangedL
// -----------------------------------------------------------------------------
//
void CDcfRepSrv::WatchedObjectChangedL( const TDesC& aObject )
    {
    DRMLOG( _L( "CDcfRepSrv::WatchedObjectChangedL ->" ) );
    DRMLOG( aObject );

    if ( aObject.Left( KProcIdentifier().Length() ) == KProcIdentifier && iArmed )
        {
#ifdef _DEBUG
        DRMLOG( _L( "Peer process killed (DEBUG mode, not rebooting)" ) );
#else
        DRMLOG( _L( "Peer process killed, rebooting" ) );
        RStarterSession starter;
        User::LeaveIfError( starter.Connect() );
        starter.Shutdown();
        starter.Close();
#endif
        }

    DRMLOG( _L( "CDRMRightsServer::WatchedObjectChangedL <-" ) );
    }

// -----------------------------------------------------------------------------
// CDcfRepSrv::StopWatchingL
// -----------------------------------------------------------------------------
//
void CDcfRepSrv::StopWatchingL()
    {
    DRMLOG( _L( "CDcfRepSrv::StopWatchingL ->" ) );
    iArmed = EFalse;
    DRMLOG( _L( "CDRMRightsServer::StopWatchingL <-" ) );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// E32 Entry for Executable
// -----------------------------------------------------------------------------

TInt E32Main()
    {
    return CDcfRepSrv::Startup();
    }

//  End of File
