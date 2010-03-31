/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DrmUtilityDownloadManager implementation
*
*/


// INCLUDE FILES
#include "DrmUtilityDownloadManager.h"
#include "DrmUtilityDmgrWrapper.h"

// CONSTANTS
_LIT( KDrmUtilityDmgrHandlerName, "DrmUtilityDmgrWrapper.dll" );

typedef TAny* (*NewDMgrL)();

const TInt KFirstFunction( 1 );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDrmUtilityDownloadManager::CDrmUtilityDownloadManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDrmUtilityDownloadManager::CDrmUtilityDownloadManager()
    {
    }

// -----------------------------------------------------------------------------
// CDrmUtilityDownloadManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDrmUtilityDownloadManager::ConstructL()
    {
    User::LeaveIfError( iDMgrDll.Load( KDrmUtilityDmgrHandlerName ) );
    NewDMgrL createDMgr = (NewDMgrL) iDMgrDll.Lookup( KFirstFunction );
    if ( !createDMgr )
        {
        User::Leave( KErrGeneral );
        }
    // Create the class, leaves in case of failure
    iDMgrHandler = (CDrmUtilityDmgrWrapper*) (*createDMgr)();
    }

// -----------------------------------------------------------------------------
// CDrmUtilityDownloadManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDrmUtilityDownloadManager* CDrmUtilityDownloadManager::NewL()
    {
    CDrmUtilityDownloadManager* self(
                                new( ELeave ) CDrmUtilityDownloadManager() );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// Destructor
CDrmUtilityDownloadManager::~CDrmUtilityDownloadManager()
    {
    delete iDMgrHandler;
    iDMgrDll.Close();
    }

// -----------------------------------------------------------------------------
// CDrmUtilityDownloadManager::DownloadAndHandleRoapTriggerL
//
// -----------------------------------------------------------------------------
//
void CDrmUtilityDownloadManager::DownloadAndHandleRoapTriggerL(
    const HBufC8* aUrl, CCoeEnv& aCoeEnv )
    {
    iDMgrHandler->DownloadAndHandleRoapTriggerL( aUrl, aCoeEnv );
    }

// -----------------------------------------------------------------------------
// CDrmUtilityDownloadManager::DownloadAndHandleRoapTriggerL
//
// -----------------------------------------------------------------------------
//
void CDrmUtilityDownloadManager::DownloadAndHandleRoapTriggerL(
    const HBufC8* aUrl )
    {
    iDMgrHandler->DownloadAndHandleRoapTriggerL( aUrl );
    }

// -----------------------------------------------------------------------------
// CDrmUtilityDownloadManager::GetErrorUrlL
//
// -----------------------------------------------------------------------------
//
HBufC8* CDrmUtilityDownloadManager::GetErrorUrlL()
    {
    return iDMgrHandler->GetErrorUrlL();
    }

