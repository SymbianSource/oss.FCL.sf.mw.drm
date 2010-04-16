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
* Description:  DRMHelperDownloadManager implementation
*
*/


// INCLUDE FILES
#include "DRMHelperDownloadManager.h"
#include "DRMHelperDMgrWrapper.h"

// CONSTANTS
_LIT( KDRMHelperDMgrHandlerName, "\\system\\libs\\drmhelperdmgrwrapper.dll" );

typedef TAny* (*NewDMgrL)();

const TInt KFirstFunction = 1;

// ======== MEMBER FUNCTIONS ========

CDrmHelperDownloadManager::CDrmHelperDownloadManager()
    {
    }

void CDrmHelperDownloadManager::ConstructL()
    {
    User::LeaveIfError( iDMgrDll.Load( KDRMHelperDMgrHandlerName ) );
    NewDMgrL createDMgr = (NewDMgrL) iDMgrDll.Lookup( KFirstFunction );
    if ( !createDMgr )
        {
        User::Leave( KErrGeneral );
        }
    // Create the class, leaves in case of failure
    iDMgrHandler = (CDRMHelperDMgrWrapper*) (*createDMgr)();
    }

CDrmHelperDownloadManager* CDrmHelperDownloadManager::NewL()
    {
    CDrmHelperDownloadManager* self = new( ELeave ) CDrmHelperDownloadManager();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

CDrmHelperDownloadManager::~CDrmHelperDownloadManager()
    {
    delete iDMgrHandler;
    iDMgrDll.Close();
    }

void CDrmHelperDownloadManager::DownloadAndHandleRoapTriggerL(
    const HBufC8* aUrl, CCoeEnv& aCoeEnv )
    {
    iDMgrHandler->DownloadAndHandleRoapTriggerL( aUrl, aCoeEnv );
    }

void CDrmHelperDownloadManager::DownloadAndHandleRoapTriggerL(
    const HBufC8* aUrl )
    {
    iDMgrHandler->DownloadAndHandleRoapTriggerL( aUrl );
    }

HBufC8* CDrmHelperDownloadManager::GetErrorUrlL()
    {
    return iDMgrHandler->GetErrorUrlL();
    }

