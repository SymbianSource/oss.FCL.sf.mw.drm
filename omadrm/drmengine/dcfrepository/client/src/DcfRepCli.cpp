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
* Description:  Client implementation
*
*/



// INCLUDE FILES
#include    "DcfRepCli.h"
#include    "DcfRepCommon.h"
#include    <e32std.h>
#include    <e32base.h>

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
LOCAL_C const TUint KDefaultMessageSlots = 4;
LOCAL_C const TInt32 KServerUid3 = 0x10205CA8;
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS


//#define _DRM_TESTING



// ============================= LOCAL FUNCTIONS ===============================
// ============================ LOCAL FUNCTIONS ===============================
#ifdef _DRM_TESTING
LOCAL_C void WriteLogL( const TDesC8& text , RFs &aFs );
LOCAL_C void WriteFileL( const TDesC8& text , RFs &aFs , const TDesC& aName );
LOCAL_C void CreateLogL();
LOCAL_C void WriteL( const TDesC8& aText );
LOCAL_C void WriteL( const TDesC8& aText , TInt aErr );
LOCAL_C void WriteCurrentTimeL();
#endif

// -----------------------------------------------------------------------------
// Testing stuff
// -----------------------------------------------------------------------------
//
#ifdef _DRM_TESTING
LOCAL_C void WriteLogL( const TDesC8& text , RFs &aFs )
    {
    _LIT( KLogFile , "c:\\RDcfRepCli.txt" );
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
    User::LeaveIfError( file.Replace( fs , _L("c:\\RDcfRepCli.txt") , EFileWrite ) );
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
#endif






LOCAL_C TInt StartServer();
LOCAL_C TInt CreateServerProcess();
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RDcfRepCli::RDcfRepCli
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RDcfRepCli::RDcfRepCli()
    {
#ifdef _DRM_TESTING
    TRAPD(r,CreateLogL());
#endif
    }

    
// Destructor
EXPORT_C RDcfRepCli::~RDcfRepCli()
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("~RDcfRepCli")));
#endif
    }


// -----------------------------------------------------------------------------
// RDcfRepCli::AddFile
// add an entry into database
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDcfRepCli::AddFile(const TDesC& aFile)
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("AddFile")));
#endif

    TInt err = 0;
    err = SendReceive( EAddFile,TIpcArgs( &aFile ) );
    return err;    
    }

// -----------------------------------------------------------------------------
// RDcfRepCli::Connect
// Connect to DCF Repository Server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDcfRepCli::Connect()
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("Connect")));
#endif

    TInt error = StartServer();

    if (KErrNone == error)
        {

        error = CreateSession(KDcfRepSrvName,
                              Version(),
                              KDefaultMessageSlots);
        }
    return error;
    }
    
// -----------------------------------------------------------------------------
// RDcfRepCli::Close
// Close session to server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RDcfRepCli::Close()
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("Close")));
#endif

    RHandleBase::Close();    
    }

// -----------------------------------------------------------------------------
// RDcfRepCli::Version
// 
// return server version
// -----------------------------------------------------------------------------
//
EXPORT_C TVersion RDcfRepCli::Version() const
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("Version")));
#endif

    return(TVersion(KDcfRepMajorVersionNumber,
                    KDcfRepMinorVersionNumber,
                    KDcfRepBuildVersionNumber));
    }

// -----------------------------------------------------------------------------
// RDcfRepCli::ScanDcf
// triger scaning for file system
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RDcfRepCli::ScanDcf(TRequestStatus& aStatus)
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("ScanDcf(1)")));
#endif

    SendReceive( EFullScan , aStatus );    
    }
    
// -----------------------------------------------------------------------------
// RDcfRepCli::ScanDcf
// triger scaning for file system
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RDcfRepCli::ScanDcf(RPointerArray<HBufC8>& aList , TRequestStatus& aStatus)
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("ScanDcf(2)")));
#endif

    TInt err = 0;
    TInt i = 0;
    TRequestStatus* status = &aStatus;
    err = SendReceive( EScanStart );
    if (err)
        {
        User::RequestComplete( status , err );
        return;
        }
    for (;i<aList.Count();i++)
        {
        err = SendReceive( EScanAdd,TIpcArgs(aList[i]) );
        if (err)
            {
            User::RequestComplete( status , err );
            return;
            }    
        }
    SendReceive(EScanEnd , aStatus);
    }    
// -----------------------------------------------------------------------------
// RDcfRepCli::SetTtid
// Set transaction ID to all Dcfs associated with a specific CID
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RDcfRepCli::SetTtid( 
            RArray<TPair>& aList ,
            TRequestStatus& aStatus
            )
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("SetTtid")));
#endif

    TInt err = 0;
    TInt i = 0;
    TRequestStatus* status = &aStatus;
    err = SendReceive( ESetTtidStart );
    if (err)
        {
        User::RequestComplete( status , err );
        return;
        }
    for (;i<aList.Count();i++)
        {
        if ( aList[i].iCid && aList[i].iTtid != NULL )
            {
            err = SendReceive( ESetTtidAdd,TIpcArgs(aList[i].iCid,aList[i].iTtid) );    
            }
        if (err)
            {
            User::RequestComplete( status , err );
            return;
            }
        }
    SendReceive(ESetTtidEnd , aStatus);
    }

// -----------------------------------------------------------------------------
// RDcfRepCli::NotifyServerIdle
// notify if server is in idle.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RDcfRepCli::NotifyServerIdle( TRequestStatus& aStatus )
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("NotifyServerIdle")));
#endif

    SendReceive( ENotifyIdle , aStatus );        
    }

// -----------------------------------------------------------------------------
// RDcfRepCli::NotifyServerIdle
// notify if server is in idle.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RDcfRepCli::StopWatching()
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("StopWatchingL")));
#endif

    SendReceive( EStopWatching );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// StartServer
// Start the server 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
LOCAL_C TInt StartServer()
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("StartServer")));
#endif    
    
    TInt result = 0;

    TFindServer findDcfRepSrv(KDcfRepSrvName);
    TFullName name;

    result = findDcfRepSrv.Next(name);
    if (result == KErrNone)
        {
        
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("StartServer->Server already running")));
#endif  
        // Server already running
        return KErrNone;
        }

    RSemaphore semaphore;       
    result = semaphore.CreateGlobal(KDcfRepSemaphoreName, 0);
    if (result != KErrNone)
        {
        
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("StartServer->CreateSemaphore"),result));
#endif

        return  result;
        }

    result = CreateServerProcess();
    if (result != KErrNone)
        {
        
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("StartServer->CreateServerProcess"),result));
#endif

        semaphore.Close(); 
        return  result;
        }

    semaphore.Wait();
    semaphore.Close();       

    return  KErrNone;
    }


// -----------------------------------------------------------------------------
// CreateServerProcess
// Create server process 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
LOCAL_C TInt CreateServerProcess()
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("CreateServerProcess")));
#endif    
    TInt result;
    TUid id;
    id.iUid = KServerUid3;
    const TUidType serverUid(KNullUid, KNullUid, id );


    RProcess server;
    result = server.Create( KDcfRepSrvFileName, KNullDesC, serverUid);
    if (result != KErrNone)
        {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("CreateServerProcess->server.Create:"),result));
#endif 
        return  result;
        }
    server.Resume();
    server.Close();

    return  KErrNone;
    }




//  End of File  
