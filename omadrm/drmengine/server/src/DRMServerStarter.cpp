/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file is used only if RDRMRightsClient starts the
*                DRM Rights Database server. 
*
*/

// INCLUDE FILES
#include   <e32std.h>
#include    <e32uid.h>
#include <f32file.h>

#ifdef RD_MULTIPLE_DRIVE
#include <DriveInfo.h>
#endif

#include "drmengineclientserver.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
#ifdef __WINS__
LOCAL_C const TUint KServerMinHeapSize =  0x1000;  
LOCAL_C const TUint KServerMaxHeapSize = 0x300000; 
_LIT( KRightsServerFile, "RightsServer" );
#else

#ifdef RD_MULTIPLE_DRIVE
_LIT( KRightsServerFile, "%c:\\RightsServer.exe" );
#else
_LIT( KRightsServerFile, "e:\\RightsServer.exe" );
#endif
#endif

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
LOCAL_C TInt CreateServer( void );

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CreateServer launches the DRM Rights server [process (THUMB/ARMI)|thread (WINS)]. 
// Returns: KErrNone: No errors.
//          <KErrNone: Symbian wide error code.
// -----------------------------------------------------------------------------
//
LOCAL_C TInt CreateServer( void )
    {
    TInt error = KErrNone;
    
        RProcess server;
        
#ifndef RD_MULTIPLE_DRIVE
    
        error = server.Create( KRightsServerFile, 
                               KNullDesC );
    
#else //RD_MULTIPLE_DRIVE
        
        RFs fs;
        TInt driveNumber( -1 );
        TChar driveLetter;
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultMassStorage, driveNumber );
        
        error = fs.Connect();
        if( error != KErrNone )
            {
            fs.Close();
            return error;
            }
        
        fs.DriveToChar( driveNumber, driveLetter );
        fs.Close();
	
        TFileName rightsServerFile;
        rightsServerFile.Format( KRightsServerFile, (TUint)driveLetter );
    
        error = server.Create( rightsServerFile, 
                               KNullDesC );
    
#endif
        
        if ( !error )
            {
            // Give some time to the process to start.
            User::After( 1000 );
            
        // Kick the server up & running.
        server.Resume();
            
        // Local handle not needed anymore.
        server.Close();
        }
        
    return error;
    }
    
// ============================ MEMBER FUNCTIONS ===============================
    
// ========================== OTHER EXPORTED FUNCTIONS =========================
    
// -----------------------------------------------------------------------------
// DRMServerStarter starts the actual server.
// Returns: KErrNone: All went OK, no errors.
//          <KErrNone: Symbian wide error code.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRMServerStarter()
    {
    RSemaphore semaphore;
    TFindServer server( DRMEngine::KServerName );
    TFullName name;
    
    // Check if the server is already running.
    TInt error = server.Next( name );
    
    if ( !error )
        {
        // Yep, it's already running.
        return KErrNone;
        }

    error = semaphore.CreateGlobal( DRMEngine::KDRMSemaphore,   // name
                                    0 ,              // count
                                    EOwnerThread ); // owner
    
    if ( error == KErrAlreadyExists )
        {
        error = semaphore.OpenGlobal( DRMEngine::KDRMSemaphore );
        }
        
    if ( !error )
        {
        error = CreateServer();
        if ( !error )
            {
            // Wait until server has done all its things.
            semaphore.Wait();
            
            // Signal the (possible) next one waiting in line. Server
            // only signals the semaphore once but there might be several
            // clients waiting for this semaphore, in theory.
            semaphore.Signal();
            }
        
        // Semaphore can be destroyed.
        semaphore.Close();
        }
        
    return error; 
    }
//  End of File
