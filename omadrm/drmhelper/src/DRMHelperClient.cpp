/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the Helper Client session functionality 
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <e32math.h>
#include <drmcommon.h>
#include "DRMHelperCommon.h"
#include "DRMHelperClient.h"


// LOCAL CONSTANTS AND MACROS
// Number of message slots to reserve for this client server session.
// Since we only communicate synchronously here, we never have any
// outstanding asynchronous requests.
LOCAL_C const TUint KDefaultMessageSlots = 0;
LOCAL_C const TUid KServerUid3 = {0x101F6DC5};

#ifdef __WINS__
LOCAL_C const TUint KServerMinHeapSize =  0x1000;  //  4K
LOCAL_C const TUint KServerMaxHeapSize = 0x10000;  // 64K
#endif

// ============================ LOCAL FUNCTIONS ===============================
#ifdef _DRM_TESTING
LOCAL_C void WriteLogL( const TDesC8& text , RFs &aFs );
LOCAL_C void WriteFileL( const TDesC8& text , RFs &aFs , const TDesC& aName );
LOCAL_C void CreateLogL();
LOCAL_C void WriteL( const TDesC& aText );
LOCAL_C void WriteL( const TDesC8& aText );
LOCAL_C void WriteCurrentTimeL();
#endif


LOCAL_C TInt FromFileNameToUri(const TDesC16& aFileName , HBufC8*& aContentUri );
LOCAL_C TInt CheckExpiration( const TDesC8& aUri , TTime& aEndTime , TInt& aCountLeft );
LOCAL_C TInt StartServer();
LOCAL_C TInt CreateServerProcess();



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RDRMHelperClient::RDRMHelperClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RDRMHelperClient::RDRMHelperClient()
:   RSessionBase()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// RDRMHelperClient::Connect
// 
// Connect to the server session
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMHelperClient::Connect()
    {
#ifdef _DRM_TESTING
    TRAPD( err , CreateLogL() );
    if (err)
        {
        return err;
        }
    TRAP( err , WriteL(_L("Connect")) );
    if (err)
        {
        return err;
        }
    TRAP( err , WriteCurrentTimeL() );
    if (err)
        {
        return err;
        }
#endif
    TInt error = StartServer();

    if (KErrNone == error)
        {

        error = CreateSession(KDRMHelperServerName,
                              Version(),
                              KDefaultMessageSlots);
        }
    return error;
    }

// -----------------------------------------------------------------------------
// RDRMHelperClient::Version
// 
// return server version
// -----------------------------------------------------------------------------
//
EXPORT_C TVersion RDRMHelperClient::Version() const
    {
    return(TVersion(KDRMHSMajorVersionNumber,
                    KDRMHSMinorVersionNumber,
                    KDRMHSBuildVersionNumber));
    }

// -----------------------------------------------------------------------------
// RDRMHelperClient::SetAutomated
// 
// Register one content uri to the helper server
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMHelperClient::SetAutomated( const TDesC8& aUri , const TInt& aType ) const
    {
    TInt temp = aType;
    TPtrC8 descriptor(aUri);

    // This call waits for the server to complete the request before 
    // proceeding. 
    return SendReceive(ERegister, TIpcArgs( &descriptor, temp ) );
    }

// -----------------------------------------------------------------------------
// RDRMHelperClient::SetAutomated
// 
// Register one file to the helper server
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMHelperClient::SetAutomated( const TDesC16& aFileName , const TInt& aType ) const
    {
    TInt temp = aType;
    TPtrC8 descriptor( NULL , 0 );
   
    descriptor.Set( reinterpret_cast<const TUint8*>( aFileName.Ptr() ), aFileName.Length()*2);

    // This call waits for the server to complete the request before 
    // proceeding. 
    return SendReceive(ERegisterFile, TIpcArgs( &descriptor, temp ) );
    }

// -----------------------------------------------------------------------------
// RDRMHelperClient::RemoveAutomated
// 
// Register one content uri to the helper server
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMHelperClient::RemoveAutomated( const TDesC8& aUri , const TInt& aType ) const
    {
    TInt temp = aType;
    TPtrC8 descriptor(aUri);

    // This call waits for the server to complete the request before 
    // proceeding.
    return SendReceive(ERemove, TIpcArgs( &descriptor, temp ) );
    }

// -----------------------------------------------------------------------------
// RDRMHelperClient::RemoveAutomated
// 
// Register one file to the helper server
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMHelperClient::RemoveAutomated( const TDesC16& aFileName , const TInt& aType ) const
    {
    TInt temp = aType;
    TPtrC8 descriptor( NULL , 0 );

    descriptor.Set( reinterpret_cast<const TUint8*>( aFileName.Ptr() ), aFileName.Length()*2);

    // This call waits for the server to complete the request before 
    // proceeding. 
    return SendReceive(ERemoveFile, TIpcArgs( &descriptor ) );
    }

// -----------------------------------------------------------------------------
// RDRMHelperClient::IndicateIdle
// 
// Register one file to the helper server
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMHelperClient::IndicateIdle() const
    {
    return SendReceive(EIndicateIdle, TIpcArgs());
    }


// -----------------------------------------------------------------------------
// RDRMHelperClient::CanSetAutomated
// 
// Register one content uri to the helper server
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMHelperClient::CanSetAutomated( const TDesC8& aUri , TBool& aValue ) const
    {
    TTime endTime;
    TTime temp;
    TInt countsLeft = 0;
    TInt err = 0;
    temp.Set( KNullDate );
    endTime.Set( KNullDate );

    err = CheckExpiration( aUri , endTime , countsLeft );
    if ( endTime != temp )
        {
        aValue = ETrue;
        }
    else
        {
        aValue = EFalse;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// RDRMHelperClient::CanSetAutomated
// 
// Register one file to the helper server
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMHelperClient::CanSetAutomated( const TDesC16& aFileName , TBool& aValue ) const
    {
    HBufC8* contentUri = NULL;
    TInt err = 0;
    err = FromFileNameToUri( aFileName , contentUri );
    if (err)
        {
        return err;
        }
    err = CanSetAutomated( contentUri->Des() , aValue );
    delete contentUri;
    return err;
    }


// -----------------------------------------------------------------------------
// RDRMHelperClient::IsAutomated
// 
// Register one content uri to the helper server
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMHelperClient::IsAutomated( const TDesC8& aUri , TInt& aType , TBool& aIs ) 
    {
    TPtr8 type( reinterpret_cast< TUint8* >( &aType ),
               0, 
               sizeof( TInt ) );
    TPtr8 flag( reinterpret_cast< TUint8* >( &aIs ),
               0, 
               sizeof( TInt ) );

    TPtrC8 descriptor(aUri);

    // This call waits for the server to complete the request before 
    // proceeding. 
    return SendReceive(EIsRegistered, TIpcArgs( &type, &descriptor, &flag ) );
    }

// -----------------------------------------------------------------------------
// RDRMHelperClient::IsAutomated
// 
// Register one file to the helper server
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMHelperClient::IsAutomated( const TDesC16& aFileName , TInt& aType , TBool& aIs ) 
    {
    TPtr8 type( reinterpret_cast< TUint8* >( &aType ),
               0, 
               sizeof( TInt ) );
    TPtr8 flag( reinterpret_cast< TUint8* >( &aIs ),
               0, 
               sizeof( TInt ) );

    TPtrC8 descriptor( NULL , 0 );

    
    descriptor.Set( reinterpret_cast<const TUint8*>( aFileName.Ptr() ), aFileName.Length()*2);

    // This call waits for the server to complete the request before 
    // proceeding. 
    return SendReceive(EIsRegisteredFile, TIpcArgs( &type, &descriptor, &flag ) );

    }



// ============================= LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// StartServer
// 
// Start the helper server
// -----------------------------------------------------------------------------
//
LOCAL_C TInt StartServer()
    {

#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L("StartServer")) );
    if (err)
        {
        return err;
        }
    TRAP( err , WriteCurrentTimeL() );
    if (err)
        {
        return err;
        }
#endif
    TInt result = 0;

    TFindServer findHelperServer(KDRMHelperServerName);
    TFullName name;

    result = findHelperServer.Next(name);
    if (result == KErrNone)
        {
        // Server already running
        return KErrNone;
        }

    RSemaphore semaphore;       
    result = semaphore.CreateGlobal(KDRMHelperServerSemaphoreName, 0);
    if (result != KErrNone)
        {
        return  result;
        }

    result = CreateServerProcess();
    if (result != KErrNone)
        {
        semaphore.Close(); 
        return  result;
        }

    semaphore.Wait();
    semaphore.Close();       

    return  KErrNone;
    }

LOCAL_C TInt CreateServerProcess()
    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L("CreateServerProcess")) );
    if (err)
        {
        return err;
        }
    TRAP( err , WriteCurrentTimeL() );
    if (err)
        {
        return err;
        }
#endif
    TInt result;

    const TUidType serverUid(KNullUid, KNullUid, KServerUid3);

#ifdef __WINS__

    RLibrary lib;
    result = lib.Load( KDRMHSServerFileName , serverUid );
    if (result != KErrNone)
        {
        return  result;
        }

#ifdef _DRM_TESTING
    TRAP( err , WriteL(_L("library is loaded")) );
    if (err)
        {
        return err;
        }
    TRAP( err , WriteCurrentTimeL() );
    if (err)
        {
        return err;
        }
#endif

    //  Get the WinsMain function
    TLibraryFunction functionWinsMain = lib.Lookup(1);

    //  Call it and cast the result to a thread function
    TThreadFunction serverThreadFunction = reinterpret_cast<TThreadFunction>(functionWinsMain());

    TName threadName(KDRMHelperServerName);

    // Append a random number to make it unique
    threadName.AppendNum(Math::Random(), EHex);

    RThread server;

    result = server.Create(threadName,   // create new server thread
                             serverThreadFunction, // thread's main function
                             KDefaultStackSize,
                             NULL,
                             &lib,
                             NULL,
                             KServerMinHeapSize,
                             KServerMaxHeapSize,
                             EOwnerProcess);



    lib.Close();    // if successful, server thread has handle to library now

    if (result != KErrNone)
        {
        return  result;
        }
#ifdef _DRM_TESTING
    TRAP( err , WriteL(_L("server thread is created")) );
    if (err)
        {
        return err;
        }
    TRAP( err , WriteCurrentTimeL() );
    if (err)
        {
        return err;
        }
#endif
    server.SetPriority(EPriorityMore);


#else

    RProcess server;
    result = server.Create( KDRMHSServerFileName, KNullDesC, serverUid);
    if (result != KErrNone)
        {
        return  result;
        }
#ifdef _DRM_TESTING
    TRAP( err , WriteL(_L("server thread is created")) );
    if (err)
        {
        return err;
        }
    TRAP( err , WriteCurrentTimeL() );
    if (err)
        {
        return err;
        }
#endif

#endif

    server.Resume();
    server.Close();

    return  KErrNone;
    }

LOCAL_C TInt FromFileNameToUri(const TDesC16& aFileName , HBufC8*& aContentUri )
    {
    DRMAuthenticated* c = NULL;
    DRMCommon::TContentProtection protection;
    HBufC8* mimeType = NULL;
    TUint dataLen = 0;
    TRAPD(err , c = DRMAuthenticated::NewL());
    if (err)
        {
        return err;
        }
    err = c->GetFileInfo(
        aFileName,
        protection,
        mimeType,
        aContentUri,
        dataLen);
    delete mimeType;
    if (err)
        {
        delete aContentUri;
        aContentUri = NULL;
        }
    return err;
    }


LOCAL_C TInt CheckExpiration( const TDesC8& aUri , TTime& aEndTime , TInt& aCountLeft )
    {
    DRMAuthenticated* c = NULL;
    CDRMRights* right = NULL;
    TRAPD( err  , c = DRMAuthenticated::NewL() );
    if (err)
        {
        return err;
        }
    err = c->GetActiveRights( aUri , 
        DRMCommon::EPlay | DRMCommon::EExecute | DRMCommon::EPrint | DRMCommon::EDisplay , 
        right );
    if (!err)
        {
        err = right->GetExpirationDetails(
            DRMCommon::EPlay | DRMCommon::EExecute | DRMCommon::EPrint | DRMCommon::EDisplay,
            aEndTime,
            aCountLeft);
        }
    delete right;
    delete c;
    return err;
    }

#ifdef _DRM_TESTING

LOCAL_C void WriteLogL( const TDesC8& text , RFs &aFs )
    {
    _LIT( KLogFile , "c:\\HSClientLog.txt" );
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
    CleanupStack::PopAndDestroy(); //file
    }

LOCAL_C void CreateLogL()
    {
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    RFile file;
    User::LeaveIfError( file.Replace( fs , _L("c:\\HSClientLog.txt") , EFileWrite ) );
    file.Close();
    CleanupStack::PopAndDestroy(); //fs
    }

LOCAL_C void WriteL( const TDesC& aText )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    HBufC8* text = HBufC8::NewLC(1000);
    TPtr8 textptr(text->Des() );
    textptr.Append( aText );
    textptr.Append( _L("\r\n") );
    WriteLogL(textptr , fs);
    CleanupStack::PopAndDestroy(text);
    CleanupStack::PopAndDestroy(); //fs
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
    CleanupStack::PopAndDestroy(); //fs
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
    CleanupStack::PopAndDestroy(); //fs
    }
#endif

// ========================== OTHER EXPORTED FUNCTIONS =========================

// Epoc DLL entry point, return that everything is ok
GLDEF_C TInt E32Dll(TDllReason)
    {
    return KErrNone;
    }

//  End of File
