/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  WMDRM Fileserver Client implementation
*
*/


#include "wmdrmfileserverclient.h"
#include "wmdrmfsclientserver.h"

//#define _LOGGING_FILE L"wmdrmfileserver.txt"

//#include "logfn.h"

// Message slots a client can have open: taken from the global slot pool
const TInt KServerDefaultMessageSlots = -1;

// Number of times the server is attempted to start if it is not running
const TInt KServerRetryCount = 3;

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Try to start the server if it is not running
// ---------------------------------------------------------------------------
//
static TInt StartServer()
	{
	RProcess server;
	TInt r = server.Create( KWmDrmFileServerImg, KNullDesC );
	if ( r != KErrNone )
		return r;
	TRequestStatus stat;
	server.Rendezvous( stat );
	if ( stat != KRequestPending )
	    {
		server.Kill( 0 );
		}
	else
	    {
		server.Resume();
		}
	User::WaitForRequest( stat );
	r = ( server.ExitType() == EExitPanic ) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}

// ======== MEMBER FUNCTIONS ========

EXPORT_C TInt RWmDrmFileServerClient::Open( const TDesC& aFilename )
    {
    TInt r = KErrNone;

    //LOGFNR( "RWmDrmFileServerClient::Open", r );
    r = SendReceive( EFsOpen, TIpcArgs( &aFilename ) );
    return r;
    }

EXPORT_C TInt RWmDrmFileServerClient::Create( const TDesC& aFilename )
    {
    TInt r = KErrNone;

    //LOGFNR( "RWmDrmFileServerClient::Create", r );
    r = SendReceive( EFsCreate, TIpcArgs( &aFilename ) );
    return r;
    }

EXPORT_C TInt RWmDrmFileServerClient::Read( TDes8& aBuffer )
    {
    TInt r = KErrNone;
    TInt amountRead;
    TPckg<TInt> amountPckg( amountRead );

    //LOGFNR( "RWmDrmFileServerClient::Read", r );
    r = SendReceive( EFsRead, TIpcArgs( &aBuffer, &amountPckg ) );
    if ( r == KErrNone )
        {
        aBuffer.SetLength( amountRead );
        }
    return r;
    }

EXPORT_C TInt RWmDrmFileServerClient::Write( const TDesC8& aBuffer )
    {
    TInt r = KErrNone;

    //LOGFNR( "RWmDrmFileServerClient::Write", r );
    r = SendReceive( EFsWrite, TIpcArgs( &aBuffer ) );
    return r;
    }

EXPORT_C TInt RWmDrmFileServerClient::Size( TInt& aSize )
    {
    TInt r = KErrNone;
    TPckg<TInt> sizePckg( aSize );

    //LOGFNR( "RWmDrmFileServerClient::Size", r );
    r = SendReceive( EFsSize, TIpcArgs( &sizePckg ) );
    return r;
    }

EXPORT_C TInt RWmDrmFileServerClient::Delete( const TDesC& aFilename )
    {
    TInt r = KErrNone;

    //LOGFNR( "RWmDrmFileServerClient::Delete", r );
    r = SendReceive( EFsDelete, TIpcArgs( &aFilename ) );
    return r;
    }

EXPORT_C TBool RWmDrmFileServerClient::IsOpen()
    {
    TInt r = KErrNone;

    //LOGFNR( "RWmDrmFileServerClient::IsOpen", r );
    r = SendReceive( EFsIsOpen );
    return (r == KErrNone ? ETrue : EFalse);
    }

EXPORT_C void RWmDrmFileServerClient::CloseFile()
    {
    //LOGFN( "RWmDrmFileServerClient::CloseFile" );
    SendReceive( EFsCloseFile );
    }

EXPORT_C TInt RWmDrmFileServerClient::MkDirAll( const TDesC& aPath, TBool aCreateIfMissing )
    {
    TInt r = KErrNone;
    
    //LOGFNR( "RWmDrmFileServerClient::MkDirAll", r );
    r = SendReceive( EFsMkDirAll, TIpcArgs( &aPath, aCreateIfMissing ) );
    return r;
    }

EXPORT_C TInt RWmDrmFileServerClient::RmDirAll( const TDesC& aPath )
    {
    TInt r = KErrNone;
    
    //LOGFNR( "RWmDrmFileServerClient::RmDirAll", r );
    r = SendReceive( EFsRmDirAll, TIpcArgs( &aPath ) );
    return r;
    }

EXPORT_C TInt RWmDrmFileServerClient::RmDir( const TDesC& aPath )
    {
    TInt r = KErrNone;
    
    //LOGFNR( "RWmDrmFileServerClient::RmDir", r );
    r = SendReceive( EFsRmDir, TIpcArgs( &aPath ) );
    return r;
    }

EXPORT_C TInt RWmDrmFileServerClient::DeleteRights()
    {
    TInt r = KErrNone;
    
    //LOGFNR( "RWmDrmFileServerClient::DeleteRights", r );
    r = SendReceive( EFsDeleteRights );
    return r;
    }

EXPORT_C TInt RWmDrmFileServerClient::Connect()
	{
	TInt r = KErrNone;
	
	//LOGFNR( "RWmDrmFileServerClient::Connect", r );
	TInt retry = KServerRetryCount;
	for ( ;; )
		{
		r = CreateSession( KWmDrmFileServerName, TVersion( 0, 0, 0 ), KServerDefaultMessageSlots );
		if ( r != KErrNotFound && r != KErrServerTerminated )
		    {
			return r;
			}
		if ( --retry == 0 )
		    {
			return r;
			}
		r = StartServer();
		if ( r != KErrNone && r != KErrAlreadyExists )
		    {
			return r;
			}
		}
	}


EXPORT_C TInt RWmDrmFileServerClient::UpdateSecureTime( TTime& aHomeTime, TTime& aUTCTime )
    {
    TInt r = KErrNone;
    TPckg<TTime> homeTime( aHomeTime );
    TPckg<TTime> utcTime( aUTCTime );
    
    //LOGFNR( "RWmDrmFileServerClient::UpdateSecureTime", r );
    r = SendReceive( EFsUpdateSecureTime, TIpcArgs( &homeTime, &utcTime ) );
    return r;    
    }
