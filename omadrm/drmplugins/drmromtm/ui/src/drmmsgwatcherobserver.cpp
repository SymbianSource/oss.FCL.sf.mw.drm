/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*     Msv operation for drm viewing
*
*/



#include "drmmsgwatcherobserver.h"


// CONSTANTS
const TInt KiProgressBufferSize = 1;


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CDrmMsgWatcherObserver::NewL
// ---------------------------------------------------------
//
CDrmMsgWatcherObserver* CDrmMsgWatcherObserver::NewL( 
	CMsvSession& aMsvSession,
    TInt aPriority,
    TRequestStatus& aObserverRequestStatus,
    TUid aMtm,
    CAiwGenericParamList* aParamList)
	{
	CDrmMsgWatcherObserver* self = 
		new(ELeave) CDrmMsgWatcherObserver( 
			aMsvSession,
			aPriority,
			aObserverRequestStatus,
			aMtm);
	CleanupStack::PushL(self);
    self->ConstructL( aParamList );
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------
// CDrmMsgWatcherObserver::NewL
// ---------------------------------------------------------
//
CDrmMsgWatcherObserver* CDrmMsgWatcherObserver::NewL( 
	CMsvSession& aMsvSession,
    TInt aPriority,
    TRequestStatus& aObserverRequestStatus,
    TUid aMtm,
    const TDesC& aFileName,
    TDataType& aDataType )
	{
	CDrmMsgWatcherObserver* self = 
		new(ELeave) CDrmMsgWatcherObserver( 
			aMsvSession,
			aPriority,
			aObserverRequestStatus,
			aMtm);
	CleanupStack::PushL(self);
    self->ConstructL( aFileName, aDataType );
	CleanupStack::Pop(self);
	return self;
	}


// ---------------------------------------------------------
// CDrmMsgWatcherObserver::ConstructL
// ---------------------------------------------------------
//
void CDrmMsgWatcherObserver::ConstructL( CAiwGenericParamList* aParamList )
    {
    const TUid KUidDRMUI = { 0x101f85c7 };        
    iService = CAknLaunchAppService::NewL( KUidDRMUI, this, aParamList );        
    iObserverRequestStatus = KRequestPending;
    Start();
    }

// ---------------------------------------------------------
// CDrmMsgWatcherObserver::ConstructL
// ---------------------------------------------------------
//
void CDrmMsgWatcherObserver::ConstructL( const TDesC& aFileName, TDataType& aDataType )
    {
    iDocHandler = CDocumentHandler::NewL();

    iDocHandler->SetExitObserver(this);

    iDocHandler->OpenFileEmbeddedL(aFileName, aDataType);
     
    iObserverRequestStatus = KRequestPending;
    Start();
    }

// ---------------------------------------------------------
// CDrmMsgWatcherObserver::CDrmMsgWatcherObserver
// ---------------------------------------------------------
//
CDrmMsgWatcherObserver::CDrmMsgWatcherObserver
	(
    CMsvSession& aMsvSession, 
    TInt aPriority, 
    TRequestStatus& aObserverRequestStatus, 
    TUid aMtm
    )
    :   
    CMsvOperation(
    aMsvSession, 
    aPriority, 
    aObserverRequestStatus )
    {
    iMtm = aMtm;        
    }

// ---------------------------------------------------------
// CDrmMsgWatcherObserver::~CDrmMsgWatcherObserver
// ---------------------------------------------------------
//
CDrmMsgWatcherObserver::~CDrmMsgWatcherObserver()
    {
    Cancel();
    if( iService )
        {
        delete iService;
        iService = NULL;
        } 
    if( iDocHandler )
        {
        delete iDocHandler;
        iDocHandler = NULL;
        }         
    delete iProgress;    
    }

// -----------------------------------------------------------------------------
// CDrmMsgWatcherObserver::DoCancel
// From CActive
// -----------------------------------------------------------------------------
//
void CDrmMsgWatcherObserver::DoCancel()
    {
    if ( iStatus == KRequestPending )
        {
        TRequestStatus* pstat = &iStatus;
        User::RequestComplete( pstat, KErrCancel );
        }
    CompleteObserver( KErrCancel );
    }


// -----------------------------------------------------------------------------
// CMsgEditorServerWatchingOperation::ProgressL
// From CMsvOperation
// -----------------------------------------------------------------------------
//
const TDesC8& CDrmMsgWatcherObserver::ProgressL()
    {
    if ( !iProgress )
        {
        iProgress = HBufC8::NewL( KiProgressBufferSize );
        }
    return *iProgress;
    }


// -----------------------------------------------------------------------------
// CMsgEditorServerWatchingOperation::FinalProgress
// From CMsvOperation
// -----------------------------------------------------------------------------
//
const TDesC8& CDrmMsgWatcherObserver::FinalProgress()
    {
    return *iProgress;
    }


// -----------------------------------------------------------------------------
// CDrmMsgWatcherObserver::RunL
// From CActive
// -----------------------------------------------------------------------------
//
void CDrmMsgWatcherObserver::RunL()
    {
    CompleteObserver( iStatus.Int( ) );
    }


// -----------------------------------------------------------------------------
// CDrmMsgWatcherObserver::Start
//
// -----------------------------------------------------------------------------
//
void CDrmMsgWatcherObserver::Start()
    {
    CActiveScheduler::Add( this );
    iStatus = KRequestPending;
    SetActive();
    }


// -----------------------------------------------------------------------------
// CDrmMsgWatcherObserver::CompleteObserver
// Completes observer with the completion code
// -----------------------------------------------------------------------------
//
void CDrmMsgWatcherObserver::CompleteObserver( TInt aCode )
    {
    TRequestStatus* status = &iObserverRequestStatus;
    
    if( ( *status ) == KRequestPending )
        {
        User::RequestComplete( status, aCode );
        }
    }



// ---------------------------------------------------------
// CDrmMsgWatcherObserver::HandleServerAppExit
// ---------------------------------------------------------
//
void CDrmMsgWatcherObserver::HandleServerAppExit( TInt aReason )
	{
    TInt exitMode = aReason;
    if ( iStatus == KRequestPending )
    	{
    	TRequestStatus* pstat = &iStatus;
    	User::RequestComplete( pstat, exitMode );
    	}
	MAknServerAppExitObserver::HandleServerAppExit( aReason );	
	}

// End of file
