/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  observe the backup restore status event
*
*/



// INCLUDE FILES
#include    <e32base.h>
#include    <e32std.h>
#include    <f32file.h>
#include    <connect/sbdefs.h> //for KUidBackupRestoreStatus
#include    "DRMBackupObserver.h"
#include    "DRMRightsServer.h"
/*
#include    <flogger.h>

_LIT( KLogDir, "drm");
_LIT( KLogName, "backup.log");
*/

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
// CDRMBackupObserver::CDRMBackupObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMBackupObserver::CDRMBackupObserver( CDRMRightsServer& aServer ):
                    CActive(CActive::EPriorityStandard),
                        iServer( aServer )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CDRMBackupObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDRMBackupObserver::ConstructL()
    {
    User::LeaveIfError( iProperty.Attach( KUidSystemCategory, conn::KUidBackupRestoreKey ) );
    }
                    
// -----------------------------------------------------------------------------
// CDRMBackupObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMBackupObserver* CDRMBackupObserver::NewL(CDRMRightsServer& aServer)
    {
    CDRMBackupObserver* self = new( ELeave ) CDRMBackupObserver(aServer);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CDRMBackupObserver::~CDRMBackupObserver()
    {
    Cancel();
    iProperty.Close();
    }


// -----------------------------------------------------------------------------
// CDRMBackupObserver::Start
// Start the system agent to listen to the event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDRMBackupObserver::Start()
    {
    Cancel();
    iProperty.Subscribe( iStatus );
    SetActive();
    }


// -----------------------------------------------------------------------------                        
// CDRMBackupObserver::RunError
// -----------------------------------------------------------------------------
//  
TInt CDRMBackupObserver::RunError(TInt aError)
    {
    if( aError != KErrCancel )
        {
        Start();           
        }
    else 
        {
        Cancel();
        iProperty.Close();    
        }
    return KErrNone;
    };


// -----------------------------------------------------------------------------
// CDRMBackupObserver::RunL
// from CActive
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//


void CDRMBackupObserver::RunL()
    {
	//RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("RunL Observer Called\n\r"));     
    // Resubscribe before processing new value to prevent missing updates
    TInt backupStatus = 0;
    TInt err( iStatus.Int() );
    if (err == KErrNone)
        {
	    //RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("RunL Observer Called: Ok\n\r"));    
                    
        
        Start();
        User::LeaveIfError( iProperty.Get( 
                KUidSystemCategory, 
                conn::KUidBackupRestoreKey, backupStatus ) );
        // Do stuff according to the backup status
        iServer.HandleBackupEventL( backupStatus );
        }
    else if ( err != KErrCancel ) // Ignore all errors except cancel
        {
	    //RFileLogger::Write(KLogDir, KLogName, EFileLoggingModeAppend, _L8("RunL Observer Called: Error\n\r"));         
        Start();
        }
    else // When cancel occurs, stop everything
        {
        User::Leave(KErrCancel);    
        }
    }

// -----------------------------------------------------------------------------
// CDRMBackupObserver::DoCancel
// From CActive
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDRMBackupObserver::DoCancel()
    {
    iProperty.Cancel();
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
//  End of File  
