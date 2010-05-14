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
* Description:  Implementation of the GPS Watcher
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <e32svr.h>

#include "GPSWatcher.h"
#include "GPSTimeUpdater.h"
#include "DRMClock.h"
#include "drmlog.h"

// Wait time in microseconds: wait 5 minutes

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CGPSWatcher::CGPSWatcher
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//   
CGPSWatcher::CGPSWatcher( CDRMClock* aClock ) :
	CTimer(EPriorityHigh),
	iClock( aClock ),
	iStatusUpdater( NULL ),
	iGpsStatus( CPosIntGpsHwStatus::EStatusUnknown )
	{
	CActiveScheduler::Add(this);
	}

// -----------------------------------------------------------------------------
// CGPSWatcher::~CGPSWatcher
// C++ destructor
// -----------------------------------------------------------------------------
// 
CGPSWatcher::~CGPSWatcher()
	{
	Cancel();

    iTimeUpdaters.ResetAndDestroy();
	iActiveSatelliteModules.ResetAndDestroy();
    
    delete iStatusUpdater;
	}

// -----------------------------------------------------------------------------
// CGPSWatcher::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CGPSWatcher* CGPSWatcher::NewL( CDRMClock* aClock )
	{
	CGPSWatcher* self = new (ELeave) CGPSWatcher( aClock );
	
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	
	return self;
	}

// -----------------------------------------------------------------------------
// CGPSWatcher::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGPSWatcher::ConstructL()
	{
    TRequestStatus* status = &iStatus;    	    
	CTimer::ConstructL();

    // Create the status updater
    iStatusUpdater = CPosIntGpsHwStatus::NewL(*this);
        
    // Get the status manually, if we can, it's ok if we cant
    TRAP_IGNORE( iStatusUpdater->GetStatusL( iGpsStatus ) );
    
    if( !IsActive() )
        {
        SetActive();    
        }
        
    User::RequestComplete(status, KErrNone);   
	}

// -----------------------------------------------------------------------------
// CGPSWatcher::OnStatusUpdateEvent
// Inherited from MPosIntGpsHwStatusObserver
// -----------------------------------------------------------------------------
//
void CGPSWatcher::OnStatusUpdateEvent(CPosIntGpsHwStatus::TIntGpsHwStatus aStatus, TInt aError)
    {
    TRequestStatus* status = &iStatus;    
    
    // store the status from the Gps:
    iGpsStatus = aStatus;
        
    User::RequestComplete(status, aError);

    if( !IsActive() )
        {        
        SetActive();
        }    
    }

// -----------------------------------------------------------------------------
// CGPSWatcher::RunL
// Inherited from CActive
// -----------------------------------------------------------------------------
//
void CGPSWatcher::RunL()
	{
    TInt error = KErrNone;
    
	// If there are errors just leave and stop watching:    
	DRMLOG2(_L("CGPSWatcher::RunL: status: '%d'"), iStatus.Int());   

    switch( iGpsStatus )
        {
        // GPS HW is used and is receiving location fixes.
        case CPosIntGpsHwStatus::EStatusOn:
            DRMLOG(_L("CGPSWatcher::RunL: CPosIntGpsHwStatus::EStatusOn: Checking modules"));
            // No active modules, check again:
            if( !iActiveSatelliteModules.Count() )
                {
                DRMLOG(_L("CGPSWatcher::RunL: CPosIntGpsHwStatus::EStatusOn: No active modules in list Checking modules"));    
                error = CheckModules();    
                }
            
            // if we have some or found some check the satellites for updates:    
            if( !error && iActiveSatelliteModules.Count() )
                {
                DRMLOG2(_L("CGPSWatcher::RunL: CPosIntGpsHwStatus::EStatusOn: Active modules available (%d), check satellites"), iActiveSatelliteModules.Count());                     
                // Get the time update:
                CheckSatellites();   
                }
            else
                {
                DRMLOG2(_L("CGPSWatcher::RunL: CPosIntGpsHwStatus::EStatusOn: Still no active modules in list or error (%d) occurred"), error);                     
                }        
            break;  
        // GPS HW is used and is searching for location.
        case CPosIntGpsHwStatus::EStatusActive:
            // Get a list of active GPS modules:
            DRMLOG(_L("CGPSWatcher::RunL: CPosIntGpsHwStatus::EStatusActive: Checking modules"));
            
            // return value not important:
            CheckModules();
            break;
        case CPosIntGpsHwStatus::EStatusUnknown:
        case CPosIntGpsHwStatus::EStatusOff:
            // Clear the list of modules:
            DRMLOG2(_L("CGPSWatcher::RunL: CPosIntGpsHwStatus::EStatusUnknown,Off or default: (%d) Checking modules"), iGpsStatus);            
            iActiveSatelliteModules.ResetAndDestroy();  
            iTimeUpdaters.ResetAndDestroy();         
        default:
            // Do nothing    
            break;   
        }		
	}


// -----------------------------------------------------------------------------
// CGPSWatcher::DoCancel
// Inherited from CActive
// -----------------------------------------------------------------------------
//
void CGPSWatcher::DoCancel()
	{
	}

// -----------------------------------------------------------------------------
// CGPSWatcher::RunError
// Inherited from CActive
// -----------------------------------------------------------------------------
//
TInt CGPSWatcher::RunError( TInt /*aError*/ )
    {
    // ignore errors    
    return KErrNone;    
    }
	
// -----------------------------------------------------------------------------
// CGPSWatcher::CheckModules
// Check what modules are present and act accordingly
// -----------------------------------------------------------------------------
//
TInt CGPSWatcher::CheckModules()
	{
	TUint numModules = 0;
	TPositionModuleInfo* moduleInfo = NULL;
	TInt error = KErrNone;
	RPositionServer posServer;
	
	// Destroy the old list:
	iActiveSatelliteModules.ResetAndDestroy();
		    
	// open connection to the positioning server:	    
    error = posServer.Connect();		    
	if( error )
	    {
	    return error;    
	    }	    
		    
    error = posServer.GetNumModules( numModules );		    
		    
	if(error || !numModules)
	    {
        DRMLOG2(_L("CheckModules: modules available (%d)"), numModules);	  
        DRMLOG2(_L("CheckModules: error code (%d)"), error);	          
      
		return KErrNotFound;
        }
        
	for( TUint i = 0; i < numModules; i++)
		{
		if(!moduleInfo)
			{
			moduleInfo = new TPositionModuleInfo;
            if( !moduleInfo )
                {
                // in practice OOM situation
                posServer.Close();    
                return KErrNoMemory;    
                }
			}
		
		// Get module info from the server
		if(posServer.GetModuleInfoByIndex(i, *moduleInfo) != KErrNone)
		    {
			continue;
            }

        DRMLOG(_L("CheckModules: Checking for internal & satellite capable"));	
                        
		// Check if the module is internal and satellite capable		
		if(! (moduleInfo->DeviceLocation() & TPositionModuleInfo::EDeviceInternal) ||
		   ! (moduleInfo->Capabilities() & TPositionModuleInfo::ECapabilitySatellite) )
			{
			// Not internal or satellite capable
			continue;
			}
		
		// Get module status and check if the module is actually active:


        DRMLOG(_L("CheckModules: Checking module status"));	
        		
		TPositionModuleStatus moduleStatus;
		
		if(posServer.GetModuleStatus(moduleStatus, moduleInfo->ModuleId()) != KErrNone)
		    {
			continue;
		    }

		// Check if the module is active or ready as all might not use the active state:
		if(!(moduleStatus.DeviceStatus() == TPositionModuleStatus::EDeviceActive ||
		     moduleStatus.DeviceStatus() == TPositionModuleStatus::EDeviceReady) ) 
		    {
			continue;
		    }


        DRMLOG(_L("CheckModules: Module is ready or active"));	
			
		// Active internal satellite device, try to append in the array
		if(iActiveSatelliteModules.Append(moduleInfo) == KErrNone)
		    {
		    // Set this to 0 so that on the next round a new one will be created
		    // old one will be in the list:   
            DRMLOG2(_L("CheckModules: Appended modules to list available (%d) active now available"), iActiveSatelliteModules.Count());		     
			moduleInfo = 0;
		    }
		}
	
	// There is a module created which was not added, delete it.	
	if( moduleInfo )
	    {
	    delete moduleInfo;
	    moduleInfo = NULL;    
	    }	
	
	// close connection to the positioning server:    
	posServer.Close();
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CGPSWatcher::CheckSatellites()
// Check what modules are present and act accordingly
// -----------------------------------------------------------------------------
//
void CGPSWatcher::CheckSatellites()
    {
    CGPSTimeUpdater* updater = NULL;
        
    iTimeUpdaters.ResetAndDestroy();
        
    // Start as many updaters as needed:    
    for( TInt i = 0; i < iActiveSatelliteModules.Count(); i++ )
        {
        updater = CGPSTimeUpdater::New( iActiveSatelliteModules[i]->ModuleId(), iClock );
        if( updater )
            {
            if( iTimeUpdaters.Append(updater) ) 
                {
                delete updater;
                updater = NULL;    
                }
            }
        }    
    }
    
// End of File
