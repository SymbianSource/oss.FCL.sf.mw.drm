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
* Description:  Server definitions
*
*/


#ifndef __WMDRMFILESERVER_H
#define __WMDRMFILESERVER_H

#include <e32base.h>
#include <f32file.h>
#include "wmdrmfileserver.h"
#include "wmdrmfileserverclient.h"
#include "wmdrmfsclientserver.h"

// Max size of the filenames handled by this server
const TInt KFileNameSize = 128;

// Panic codes for session errors
enum TWmDrmPanic
	{
	EPanicBadDescriptor,
	EPanicIllegalFunction
	};

void PanicClient( const RMessagePtr2& aMessage,TWmDrmPanic TWmDrmPanic );

class CWmDrmFileServer : public CServer2
	{
public:
    /* Create a new file server object and push it onto the cleanup stack
     */
	static CServer2* NewLC();

    /* Destructor.
     */
    ~CWmDrmFileServer();

    /* Return a reference to the shared file server handle
     *
     * @return File server handle
     */
    RFs& Fs();

private:
    /* Private constructor.
     */
    CWmDrmFileServer();
    
    /* Second level constructor.
     */
	void ConstructL();

    /* Create a new session
     *
     * @param aVersion  Client version
     * @param aMessage  IPC message (ignored)
     * @return new session object
     */
	CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;

    // Own: Common file server object
	RFs iFs;
	};

#endif
