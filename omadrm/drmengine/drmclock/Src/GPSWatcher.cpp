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


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CGPSWatcher::CGPSWatcher
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//   
CGPSWatcher::CGPSWatcher( CDRMClock* aClock ) :
	CActive(EPriorityHigh),
	iClock( aClock )
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

	delete iTimeUpdater; 
	iTimeUpdater = 0; 
	
	iPosServer.Close();
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
	User::LeaveIfError(iPosServer.Connect());
	
	// Immediately subscribe for module status events
	iStatusEvent.SetRequestedEvents(TPositionModuleStatusEventBase::EEventDeviceStatus);
	Subscribe();

	// Check initial state
	CheckModules();
	}

// -----------------------------------------------------------------------------
// CGPSWatcher::Subscribe
// Subscribe to position events
// -----------------------------------------------------------------------------
//
void CGPSWatcher::Subscribe()
	{
	iPosServer.NotifyModuleStatusEvent(iStatusEvent, iStatus);
	SetActive();
	}

// -----------------------------------------------------------------------------
// CGPSWatcher::RunL
// Inherited from CActive
// -----------------------------------------------------------------------------
//
void CGPSWatcher::RunL()
	{
	// If we already updated the time:
	if( iTimeUpdater && iTimeUpdater->TimeReceived() )
	    {
	    DRMLOG( _L("CGPSWatcher::RunL: time updater exists, time received, deleting"));    
	    iClock->Notify( CDRMClock::ENotifyGPSTimeReceived );    
	    DRMLOG( _L("CGPSWatcher::RunL: object deleted, returning"));
	    return;    
	    }
	    
	Subscribe();
	TPositionModuleStatusEventBase::TModuleEvent occurredEvents = iStatusEvent.OccurredEvents();
	DRMLOG2(_L("CGPSWatcher::RunL: occurredEvents = 0x%X"), occurredEvents);
	
	// If time updater is not running, check module statuses
	if(!iTimeUpdater)
		CheckModules();
	}


// -----------------------------------------------------------------------------
// CGPSWatcher::DoCancel
// Inherited from CActive
// -----------------------------------------------------------------------------
//
void CGPSWatcher::DoCancel()
	{
	iPosServer.CancelRequest(EPositionServerNotifyModuleStatusEvent);
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
void CGPSWatcher::CheckModules()
	{
	// Get number of modules
	TUint numModules;
	if(iPosServer.GetNumModules(numModules)!=KErrNone || !numModules)
		return;
	
	// Collect all active and satellite capable modules
	TFullName moduleName;
	RPointerArray<TPositionModuleInfo> satelliteModules;
	
	TPositionModuleInfo *moduleInfo(0);
	
	for(TUint i=0; i<numModules; i++)
		{
		if(!moduleInfo)
			{
			moduleInfo = new TPositionModuleInfo;
			if(!moduleInfo)
			    {   
				continue;
			    }
			}
		
		// Get module info from the server
		if(iPosServer.GetModuleInfoByIndex(i, *moduleInfo) != KErrNone)
		    {
			continue;
            }
            
		// Check if the module is internal and satellite capable		
		if(! (moduleInfo->DeviceLocation() & TPositionModuleInfo::EDeviceInternal) ||
		   ! (moduleInfo->Capabilities() & TPositionModuleInfo::ECapabilitySatellite) )
			{
			// Not internal or satellite capable
			continue;
			}
		
		// Get module status
		TPositionModuleStatus moduleStatus;
		if(iPosServer.GetModuleStatus(moduleStatus, moduleInfo->ModuleId()) != KErrNone)
		    {
			continue;
		    }
		// Check if the module is active
		if(moduleStatus.DeviceStatus() != TPositionModuleStatus::EDeviceActive) 
		    {
			continue;
		    }

		moduleInfo->GetModuleName(moduleName);
		
		DRMLOG6( _L("Module %d: id=0x%08X, name = %S, cap=0x%08X, status=%d"), i, moduleInfo->ModuleId().iUid, &moduleName, moduleInfo->Capabilities(), moduleStatus.DeviceStatus());
			
		// Active internal satellite device, try to append in the array
		if(satelliteModules.Append(moduleInfo) == KErrNone)
		    {
			moduleInfo = 0;
		    }
		}
	
	delete moduleInfo;
	
	if(satelliteModules.Count())
		{
		DRMLOG( _L("Active satellite module available") );    
		// We have an active satellite module available
		TPositionModuleId moduleId = satelliteModules[0]->ModuleId();
		
		// Just allocating/deleting the time updater will do the trick
		iTimeUpdater = CGPSTimeUpdater::New(iPosServer, moduleId, iClock);
		}
	else
		{
		DRMLOG( _L("No active satellite modules available") );    
		// Delete time updater since no active GPS anymore
		delete iTimeUpdater; iTimeUpdater = 0;
		}
	
	satelliteModules.ResetAndDestroy();
	satelliteModules.Close();
	}
