/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the parent storage for Decision Making Machine
*
*/


// INCLUDE FILES
#include "drmactivedeletion.h"
#include "drmrightsdb.h"
#include "drmrightscleaner.h"
#include "drmdbsession.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// ============================= LOCAL FUNCTIONS ===============================

    
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMActiveDeletion::NewLC
//
// Two-phase constructor.
// -----------------------------------------------------------------------------
//
CDRMActiveDeletion* CDRMActiveDeletion::NewLC( const RMessagePtr2& aMessage,
                                               CDRMDbSession& aSession )
    {
    CDRMActiveDeletion* self = new( ELeave ) CDRMActiveDeletion( aMessage,
                                                                 aSession );
    CleanupStack::PushL( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// CDRMActiveDeletion::~CDRMActiveDeletion
//
// Destructor.
// -----------------------------------------------------------------------------
//
CDRMActiveDeletion::~CDRMActiveDeletion()
    {
    if ( iActiveOperation )
        {
        // Construction was successful, but 
        // something has went wrong.

        iActiveOperation->Cancel(); 
        iMessage.Complete( KErrCancel );
        }
    }
    
// -----------------------------------------------------------------------------
// CDRMActiveDeletion::ActivateL
//
// Activate the thing by issuing a request to the DB and executing it also.
// -----------------------------------------------------------------------------
//
void CDRMActiveDeletion::ActivateL( const TTime& aTime,
                                    CDRMRightsDB& aDb )
    {
    CActiveScheduler::Add( this );
    
    CDRMRightsCleaner* cleaner = 
        aDb.DeleteExpiredPermissionsL( aTime, iStatus );
    CleanupStack::PushL( cleaner );
    cleaner->ExecuteCleanupLD();
    CleanupStack::Pop();
    
    SetActive();
    iActiveOperation = cleaner;    
    }

// -----------------------------------------------------------------------------
// CDRMActiveDeletion::RunL
//
// Handles the completition of the request.
// -----------------------------------------------------------------------------
//
void CDRMActiveDeletion::RunL()
    {
    // All done.
    iMessage.Complete( iStatus.Int() );
    
    // iActiveOperation deletes itself.
    iActiveOperation = NULL;
    
    Deque();
    
    iSession.AsyncOperationDone();
    }
    
// -----------------------------------------------------------------------------
// CDRMActiveDeletion::DoCancel
//
// Cancels the operation.
// -----------------------------------------------------------------------------
//
void CDRMActiveDeletion::DoCancel()
    {
    iActiveOperation->Cancel();
    iActiveOperation = NULL;
    }

// -----------------------------------------------------------------------------
// CDRMActiveDeletion::CDRMActiveDeletion
//
// Constructor.
// -----------------------------------------------------------------------------
//
CDRMActiveDeletion::CDRMActiveDeletion( const RMessagePtr2& aMessage,
                                        CDRMDbSession& aSession ):
CActive( EPriorityLow ),
iMessage( aMessage ),
iSession( aSession )
    {
    // Nothing
    }
    
// End of file
