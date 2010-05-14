
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
* Description:  Implementation of the GPS time updater
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <e32debug.h>
#include "GPSTimeUpdater.h"
#include "DRMClock.h"
#include "drmlog.h"


_LIT(KDRMClockServerName, "DRMClockServer");

const TInt KGPSUpdateInterval   = 1000000;  // One second
const TInt KGPSUpdateAge        = 500000;   // Half a second
const TInt KGPSUpdateTimeOut    = 30000000; // Thirty seconds

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CGPSTimeUpdater::CGPSTimeUpdater
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//   
CGPSTimeUpdater::CGPSTimeUpdater( const TPositionModuleId& aModuleId,
                                  CDRMClock* aClock ) : 
	CActive(EPriorityHigh),
	iModuleId(aModuleId),
	iClock( aClock ),
	iTimeReceived( EFalse )
	{
	CActiveScheduler::Add(this);
	}

// -----------------------------------------------------------------------------
// CGPSTimeUpdater::~CGPSTimeUpdater
// C++ destructor
// -----------------------------------------------------------------------------
// 
CGPSTimeUpdater::~CGPSTimeUpdater()
	{
	DRMLOG(_L("CGPSTimeUpdater::~CGPSTimeUpdater"));
	Cancel();
	
	iPositioner.Close();
	iPosServer.Close();
	}

// -----------------------------------------------------------------------------
// CGPSTimeUpdater::New
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CGPSTimeUpdater* CGPSTimeUpdater::New( const TPositionModuleId& aModuleId,
                                       CDRMClock* aClock )
	{
	CGPSTimeUpdater* self = new CGPSTimeUpdater(aModuleId, aClock);
	if(self)
		{
		TRAPD(err, self->ConstructL());
		if(err!=KErrNone)
			{
			delete self;
			self = 0;
			}
		}
	return self;
	}


// -----------------------------------------------------------------------------
// CGPSTimeUpdater::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGPSTimeUpdater::ConstructL()
	{
	DRMLOG(_L("CGPSTimeUpdater::ConstructL >>"));
	
	User::LeaveIfError( iPosServer.Connect() );
	
	// Open positioner
	User::LeaveIfError(iPositioner.Open(iPosServer, iModuleId));
	User::LeaveIfError(iPositioner.SetRequestor(CRequestor::ERequestorService,
	                   CRequestor::EFormatApplication, 
	                   KDRMClockServerName));
	
	// Set update options
	TPositionUpdateOptions updateOptions;
	updateOptions.SetAcceptPartialUpdates(ETrue);
	updateOptions.SetMaxUpdateAge(KGPSUpdateAge);
	updateOptions.SetUpdateInterval(TTimeIntervalMicroSeconds(KGPSUpdateInterval));
	updateOptions.SetUpdateTimeOut(TTimeIntervalMicroSeconds(KGPSUpdateTimeOut));
	User::LeaveIfError(iPositioner.SetUpdateOptions(updateOptions));
	
	// Request position update
	iPositioner.NotifyPositionUpdate(iSatelliteInfo, iStatus);
	SetActive();
	
	DRMLOG(_L("CGPSTimeUpdater::ConstructL <<"));
	}

// -----------------------------------------------------------------------------
// CGPSTimeUpdater::RunL
// Inherited from CActive
// -----------------------------------------------------------------------------
//
void CGPSTimeUpdater::RunL()
	{
	DRMLOG(_L("CGPSTimeUpdater::RunL >>"));
	
	DRMLOG2(_L("CGPSTimeUpdater::RunL: iStatus=%d"), iStatus.Int());
	
	// We got some kind of an update:
	if( iStatus == KErrNone || 
	    iStatus == KPositionPartialUpdate )
		{
		DRMLOG(_L("CGPSTimeUpdater::RunL: position updated!"));
		
		DRMLOG2(_L("CGPSTimeUpdater::RunL: satellites used = %d"), iSatelliteInfo.NumSatellitesUsed());
		TTime satelliteTime = iSatelliteInfo.SatelliteTime();
		
		TDateTime dt = satelliteTime.DateTime();
		DRMLOG7(_L("CGPSTimeUpdater::RunL: satellite time = %02d-%02d-%04d, %02d:%02d:%02d"), dt.Day(), dt.Month(), dt.Year(), dt.Hour(), dt.Minute(), dt.Second());
		    
	    // If the time has not been received yet this function will leave and we re-issue the request from run error:    
		iClock->ResetSecureTimeL( satelliteTime, 0 );    
		
		// Mark time as received
		iTimeReceived = ETrue;
		}

    // We only try once, if it fails it fails and we will try again when it is activated the next time.
	iPositioner.Close();
        
	DRMLOG(_L("CGPSTimeUpdater::RunL <<"));
	}

// -----------------------------------------------------------------------------
// CGPSTimeUpdater::DoCancel
// Inherited from CActive
// -----------------------------------------------------------------------------
//	
void CGPSTimeUpdater::DoCancel()
	{
	iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
	}


// -----------------------------------------------------------------------------
// CGPSTimeUpdater::RunError
// Inherited from CActive
// -----------------------------------------------------------------------------
//	
TInt CGPSTimeUpdater::RunError( TInt /*aError*/ )
    {
    // Time received was invalid, wait for more updates
    iPositioner.NotifyPositionUpdate(iSatelliteInfo, iStatus);
    SetActive();
	
    // ignore errors    
    return KErrNone;    
    }
