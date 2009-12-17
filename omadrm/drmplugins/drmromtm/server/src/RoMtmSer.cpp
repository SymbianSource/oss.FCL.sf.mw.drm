/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of Rights Object Server side MTM
*
*/



// INCLUDE FILES
#if !defined(__MSVENTRY_H__)
#include <msventry.h>
#endif

#if !defined(__TXTRICH_H__)
#include <txtrich.h>
#endif

#include "RoMtmSer.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS


// ============================= LOCAL FUNCTIONS ===============================
//
// ---------------------------------------------------------
// E32Dll implements
// function that is called when a dll is loaded.
// This function must be present in all dlls.
// Returns: error code: KErrNone
//


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CRightsObjectServerMtm::CRightsObjectServerMtm
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRightsObjectServerMtm::CRightsObjectServerMtm( CRegisteredMtmDll& aRegisteredMtmDll , CMsvServerEntry* aInitialEntry )
	: 	CBaseServerMtm( aRegisteredMtmDll , aInitialEntry )
	{
	}

// -----------------------------------------------------------------------------
// CRightsObjectServerMtm::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRightsObjectServerMtm::ConstructL()
	{
	CActiveScheduler::Add( this );
	}

// -----------------------------------------------------------------------------
// CRightsObjectServerMtm::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRightsObjectServerMtm* CRightsObjectServerMtm::NewL( CRegisteredMtmDll& aRegisteredMtmDll , CMsvServerEntry* aInitialEntry )
// Exported factory function
	{
	CleanupStack::PushL( aInitialEntry );
	CRightsObjectServerMtm* self=new ( ELeave ) CRightsObjectServerMtm( aRegisteredMtmDll , aInitialEntry );
	CleanupStack::Pop();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Destructor
CRightsObjectServerMtm::~CRightsObjectServerMtm()
	{
	Cancel();
	}

//
// Copy and move functions
//

// -----------------------------------------------------------------------------
// CRightsObjectServerMtm::CopyToLocalL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
void CRightsObjectServerMtm::CopyToLocalL( const CMsvEntrySelection& /*aSelection*/ , TMsvId /*aDestination*/ , 
								  TRequestStatus& /*aStatus*/ )
// Get remote entries (by copying)
	{
	User::Leave( KErrNotSupported );
	}

// -----------------------------------------------------------------------------
// CRightsObjectServerMtm::CopyFromLocalL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
void CRightsObjectServerMtm::CopyFromLocalL( const CMsvEntrySelection& /*aSelection*/ , TMsvId /*aDestination*/ , 
									TRequestStatus& /*aStatus*/ )
// Transfer to remote (by copying)
	{
	User::Leave( KErrNotSupported );
	}

// -----------------------------------------------------------------------------
// CRightsObjectServerMtm::CopyWithinServiceL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
void CRightsObjectServerMtm::CopyWithinServiceL( const CMsvEntrySelection& /*aSelection*/ , TMsvId /*aDestination*/ , 
										TRequestStatus& /*aStatus*/ )
// Copy entries within service 
	{
	User::Leave( KErrNotSupported );
	}

// -----------------------------------------------------------------------------
// CRightsObjectServerMtm::MoveToLocalL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
void CRightsObjectServerMtm::MoveToLocalL( const CMsvEntrySelection& /*aSelection*/ , TMsvId /*aDestination*/ , 
								  TRequestStatus& /*aStatus*/ )
// Get remote entries (by moving)
	{
	User::Leave( KErrNotSupported );
	}

// -----------------------------------------------------------------------------
// CRightsObjectServerMtm::MoveFromLocalL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
void CRightsObjectServerMtm::MoveFromLocalL( const CMsvEntrySelection& /*aSelection*/ , TMsvId /*aDestination*/ , 
									TRequestStatus& /*aStatus*/ )
// Transfer to remote (by moving)
	{
	User::Leave( KErrNotSupported );
	}

// -----------------------------------------------------------------------------
// CRightsObjectServerMtm::MoveWithinServiceL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
void CRightsObjectServerMtm::MoveWithinServiceL( const CMsvEntrySelection& /*aSelection*/ , TMsvId /*aDestination*/ , 
										TRequestStatus& /*aStatus*/ )
// Move entries within service 
	{
	User::Leave( KErrNotSupported );
	}

//
// Create, change, delete functions
//

// -----------------------------------------------------------------------------
// CRightsObjectServerMtm::DeleteAllL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
void CRightsObjectServerMtm::DeleteAllL( const CMsvEntrySelection& /*aSelection*/ , TRequestStatus& /*aStatus*/ )
// Delete entries recursively 
	{
	User::Leave( KErrNotSupported );
	}

// -----------------------------------------------------------------------------
// CRightsObjectServerMtm::CreateL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
void CRightsObjectServerMtm::CreateL(TMsvEntry /*aNewEntry*/, TRequestStatus& /*aStatus*/ )
// Remote creation (other than by copying) not supported 
//
	{
	User::Leave( KErrNotSupported );
	}

// -----------------------------------------------------------------------------
// CRightsObjectServerMtm::ChangeL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
void CRightsObjectServerMtm::ChangeL(TMsvEntry /*aNewEntry*/ , TRequestStatus& /*aStatus*/ )
// Change entry 
//
// This function could change file names in the file system. It would then also have
// to change the details field of all the children of a changed folder name.
// Not supported for now.
//
	{
	User::Leave( KErrNotSupported );
	}



//
// Command and progress functions
//

// -----------------------------------------------------------------------------
// CRightsObjectServerMtm::StartCommandL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
void CRightsObjectServerMtm::StartCommandL(
								CMsvEntrySelection& /*aSelection*/ ,
								TInt /*aCommand*/ , 
								const TDesC8& /*aParameter*/, 
								TRequestStatus& /*aStatus*/ 
								)
// Run MTM-specific command on selection of entries 
// Only command supported is Refresh
	{
    User::Leave( KErrNotSupported );
	}

// -----------------------------------------------------------------------------
// CRightsObjectServerMtm::CommandExpected
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
TBool CRightsObjectServerMtm::CommandExpected()
// Prevent object deletion on command completion
// Allow unloading
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CRightsObjectServerMtm::Progress
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
const TDesC8& CRightsObjectServerMtm::Progress()
// Pass-back progress information
	{
    return KNullDesC8;
	}

//
//  Active object completion functions
//

// -----------------------------------------------------------------------------
// CRightsObjectServerMtm::DoCancel
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
void CRightsObjectServerMtm::DoCancel()
// Cancel current operation
	{
	}

// -----------------------------------------------------------------------------
// CRightsObjectServerMtm::DoComplete
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
void CRightsObjectServerMtm::DoComplete(TInt /*aError*/ )
// Active object complete leave handler
//
// Never expect this to be called
// as CRightsObjectServerMtm::DoRunL() does not leave
	{
	}

// -----------------------------------------------------------------------------
// CRightsObjectServerMtm::DoRunL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
void CRightsObjectServerMtm::DoRunL()
// Active object completion
//
// Run is used in this object to clean up after operations have finished.
//
	{
	}



