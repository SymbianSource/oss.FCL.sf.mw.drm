
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


#ifndef GPSWATCHER_H_
#define GPSWATCHER_H_

#include <lbs.h>
#include <epos_intgpshwstatus.h>

class CGPSTimeUpdater;
class CDRMClock;


class CGPSWatcher : public CTimer, public MPosIntGpsHwStatusObserver
	{
public:
	~CGPSWatcher();
	static CGPSWatcher* NewL( CDRMClock* aClock );

public: // from MPosIntGpsHwStatusObserver
	/**
	 * Notify observer of a GPS hardware status event
	 * @param aStatus The GPS hardware status.
	 * @param aError KErrNone if successful. Other possible error values defined by RProperty::Get()
	 */
    void OnStatusUpdateEvent(CPosIntGpsHwStatus::TIntGpsHwStatus aStatus, TInt aError);

private:
	CGPSWatcher( CDRMClock* aClock );
	void ConstructL();
	
	void CheckSatellites();
	TInt CheckModules();
	void RunL();
	void DoCancel();
	TInt RunError( TInt aError );
	
private:
	// owned:
	RPointerArray<CGPSTimeUpdater> iTimeUpdaters;
	
	// not owned:
	CDRMClock* iClock;
	
	// GPS status information:
	CPosIntGpsHwStatus* iStatusUpdater;
	
	// status received from GPS:
	CPosIntGpsHwStatus::TIntGpsHwStatus iGpsStatus;
	
	// List of active satellite modules:   
	RPointerArray<TPositionModuleInfo> iActiveSatelliteModules;   
	 
	};

#endif /* GPSWATCHER_H_ */
