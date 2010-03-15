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
#include "DRMActiveOperation.h"
#include "drmrightsdb.h"
#include "DRMRightsCleaner.h"
#include "DRMObsoleteFinder.h"
#include "DRMDbSession.h"

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
// CDRMActiveOperation::NewLC
//
// Two-phase constructor.
// -----------------------------------------------------------------------------
//
CDRMActiveOperation* CDRMActiveOperation::NewLC( const RMessagePtr2& aMessage,
                                               CDRMDbSession& aSession,
                                               TOperationType aOperation )
    {
    CDRMActiveOperation* self = new( ELeave ) CDRMActiveOperation( aMessage,
                                                                 aSession,
                                                                 aOperation );
    CleanupStack::PushL( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CDRMActiveOperation::~CDRMActiveOperation
//
// Destructor.
// -----------------------------------------------------------------------------
//
CDRMActiveOperation::~CDRMActiveOperation()
    {
    // Close the stream
    iFileStream.Close();

    if( iFileName )
        {
        if( iFileServer )
            {
            iFileServer->Delete( *iFileName );
            }
        delete iFileName;
        iFileName = NULL;
        }

    if ( iActiveOperation )
        {
        // Construction was successful, but
        // something has went wrong.

        iActiveOperation->Cancel();
        if( !iMessage.IsNull() )
            {
            iMessage.Complete( KErrCancel );
            }
        }
    }

// -----------------------------------------------------------------------------
// CDRMActiveOperation::ActivateL
//
// Activate the thing by issuing a request to the DB and executing it also.
// -----------------------------------------------------------------------------
//
void CDRMActiveOperation::ActivateL( CDRMRightsDB& aDb,
                                     const TTime& aTime )
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
// CDRMActiveOperation::ActivateL
//
// Activate the thing by creating an object and executing it
// -----------------------------------------------------------------------------
//
void CDRMActiveOperation::ActivateL( CDRMRightsDB& aDb,
                                     RFs& aFileServer,
                                     const TDesC& aTempPath,
                                     const TBool aPerformScan )
    {
    CActiveScheduler::Add( this );

    iFileName = new (ELeave) TFileName;
    iFileServer = &aFileServer;

    User::LeaveIfError(
        iFileStream.Temp( aFileServer,
                         aTempPath,
                         *iFileName,
                         EFileWrite | EFileStream ) );

    CDRMObsoleteFinder* finder = CDRMObsoleteFinder::NewL(aFileServer,
                                                          &aDb,
                                                          iStatus,
                                                          iFileStream,
                                                          aPerformScan );
    CleanupStack::PushL( finder );
    finder->ExecuteFinderLD();
    CleanupStack::Pop();
    SetActive();
    iActiveOperation = finder;
    }

// -----------------------------------------------------------------------------
// CDRMActiveOperation::Remove
//
// Cancels the operation.
// -----------------------------------------------------------------------------
//
void CDRMActiveOperation::Remove()
    {
    switch(iOperation)
        {
        case EOperationExportObsolete:
            {
            static_cast<CDRMObsoleteFinder*>(iActiveOperation)->DoCleanup();
            }
            break;
        default:
            {
            static_cast<CDRMRightsCleaner*>(iActiveOperation)->DoCleanup();
            }
            break;
        }
    }

// -----------------------------------------------------------------------------
// CDRMActiveOperation::RunL
//
// Handles the completition of the request.
// -----------------------------------------------------------------------------
//
void CDRMActiveOperation::RunL()
    {
    TFileName* fileName = NULL;

    // All done.
    if( !iMessage.IsNull() )
        {
        iMessage.Complete( iStatus.Int() );
        }

    // iActiveOperation deletes itself.
    iActiveOperation = NULL;

    Deque();

    if( iOperation == EOperationExportObsolete )
        {
        fileName = iFileName;
        iFileName = NULL;
        iSession.AsyncOperationDone(fileName);
        }
    else
        {
        iSession.AsyncOperationDone();
        }
    }

// -----------------------------------------------------------------------------
// CDRMActiveOperation::DoCancel
//
// Cancels the operation.
// -----------------------------------------------------------------------------
//
void CDRMActiveOperation::DoCancel()
    {
    }



// -----------------------------------------------------------------------------
// CDRMActiveOperation::CDRMActiveOperation
//
// Constructor.
// -----------------------------------------------------------------------------
//
CDRMActiveOperation::CDRMActiveOperation( const RMessagePtr2& aMessage,
                                        CDRMDbSession& aSession,
                                        TOperationType aOperation ):
    CActive( EPriorityLow ),
    iMessage( aMessage ),
    iSession( aSession ),
    iOperation( aOperation ),
    iFileServer( NULL )
    {
    // Nothing
    }

// End of file
