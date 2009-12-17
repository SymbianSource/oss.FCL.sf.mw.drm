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
* Description:  WMDRM File Server implementation
*
*/


#include "wmdrmfileserversession.h"
#include "wmdrmfileserver.h"

//#define _LOGGING_FILE L"wmdrmfileserver.txt"

//#include "flogger.h"
//#include "logfn.h"

CWmDrmFileServer::CWmDrmFileServer():
	CServer2( CActive::EPriorityStandard,  ESharableSessions )
	{
	}

CServer2* CWmDrmFileServer::NewLC()
	{
	//LOGFN( "CWmDrmFileServer::NewLC" );
	CWmDrmFileServer* self = new( ELeave ) CWmDrmFileServer;
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

CWmDrmFileServer::~CWmDrmFileServer()
    {
	//LOGFN( "CWmDrmFileServer::~CWmDrmFileServer" );
    iFs.Close();
    }

void CWmDrmFileServer::ConstructL()
	{
	//LOGFN( "CWmDrmFileServer::ConstructL" );
	StartL( KWmDrmFileServerName );
	User::LeaveIfError( iFs.Connect() );
	}

CSession2* CWmDrmFileServer::NewSessionL(
    const TVersion& /*aVersion*/,
    const RMessage2& /*aMessage*/ ) const
	{
	//LOGFN( "CWmDrmFileServer::NewSessionL" );
	return new( ELeave ) CWmDrmFileServerSession();
	}

RFs& CWmDrmFileServer::Fs()
	{
	return iFs;
	}

void PanicClient(
    const RMessagePtr2& aMessage,
    TWmDrmPanic aPanic )
	{
	//LOGFN( "PanicClient" );
	_LIT( KPanic, "WmDrmServer" );
	aMessage.Panic( KPanic, aPanic );
	}

static void RunServerL()
	{
	//LOGFN( "RunServerL" );

	User::LeaveIfError( RThread::RenameMe( KWmDrmFileServerName ) );
	CActiveScheduler* s = new( ELeave ) CActiveScheduler;
	CleanupStack::PushL( s );
	CActiveScheduler::Install( s );
	CWmDrmFileServer::NewLC();
	RProcess::Rendezvous( KErrNone );
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy( 2 ); // server, s
	}

TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt r = KErrNoMemory;
	if ( cleanup )
		{
		TRAP( r, RunServerL() );
		delete cleanup;
		}
	__UHEAP_MARKEND;
	return r;
	}
