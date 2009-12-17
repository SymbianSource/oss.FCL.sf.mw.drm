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
* Description:  Server definitions
*
*/


#ifndef __WMDRMPKSERVERRESOURCES_H
#define __WMDRMPKSERVERRESOURCES_H

#include <e32base.h>
#include <f32file.h>
#include "wmdrmpkserverresources.h"

_LIT( KWmDrmPkServerName, "!wmdrmpkserver.exe" );
_LIT( KWmDrmPkServerImg, "wmdrmpkserver" );

class CWmDrmPkServer : public CServer2
	{
public:
	static CServer2* NewLC();
	void Send( const TDesC& aMessage );
	~CWmDrmPkServer();

private:
	CWmDrmPkServer();
	void ConstructL();
	CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;
private:
	};

class CWmDrmPkSession : public CSession2
	{
public:
	CWmDrmPkSession();
	void CreateL();
	
private:
	~CWmDrmPkSession();
	inline CWmDrmPkServer& Server();
	void ServiceL(const RMessage2& aMessage);
	void ServiceError(const RMessage2& aMessage, TInt aError);
	};

#endif
