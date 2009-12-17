/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  WMDRM PK Server implementation
*
*/

#include "wmdrmpkserverresources.h"

// ============================= LOCAL FUNCTIONS ==============================

// ============================= MEMBER FUNCTIONS =============================

void CWmDrmPkSession::CreateL()
	{
	}

CWmDrmPkSession::CWmDrmPkSession()
    {
	}
	
CWmDrmPkSession::~CWmDrmPkSession()
	{
	}

void CWmDrmPkSession::ServiceL( const RMessage2& aMessage )
	{
	TInt r = KErrNone;
	
    if ( !aMessage.IsNull() )
        {
        aMessage.Complete( r );
	    }
	}

void CWmDrmPkSession::ServiceError( const RMessage2& aMessage, TInt aError )
	{
	if ( !aMessage.IsNull() )
        {
        CSession2::ServiceError( aMessage, aError );
	    }
	}

CServer2* CWmDrmPkServer::NewLC()
	{
	CWmDrmPkServer* self = new( ELeave ) CWmDrmPkServer;
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

CWmDrmPkServer::CWmDrmPkServer():
	CServer2( CActive::EPriorityStandard,  ESharableSessions )
    {
    }

CWmDrmPkServer::~CWmDrmPkServer()
    {
    }

void CWmDrmPkServer::ConstructL()
	{
	StartL( KWmDrmPkServerName );
	}

CSession2* CWmDrmPkServer::NewSessionL(
    const TVersion& /*aVersion*/,
    const RMessage2& /*aMessage*/ ) const
	{
	return new( ELeave ) CWmDrmPkSession();
	}

CWmDrmPkServer& CWmDrmPkSession::Server()
	{
	return *static_cast<CWmDrmPkServer*>( const_cast<CServer2*>( CSession2::Server() ) );
	}

static void RunServerL()
	{
	User::LeaveIfError( RThread::RenameMe( KWmDrmPkServerName ) );
	CActiveScheduler* s = new( ELeave ) CActiveScheduler;
	CleanupStack::PushL( s );
	CActiveScheduler::Install( s );
	CWmDrmPkServer::NewLC();
	RProcess::Rendezvous( KErrNone );
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy( 2 );
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
