
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


#ifndef GPSTIMEUPDATER_H_
#define GPSTIMEUPDATER_H_

#include <lbs.h>
#include <lbssatellite.h>

class CDRMClock;

class CGPSTimeUpdater : public CActive
	{
public:
	~CGPSTimeUpdater();
	static CGPSTimeUpdater* New( const TPositionModuleId& aModuleId,
	                             CDRMClock* aClock );
	
	inline const TPositionModuleId& ModuleId() { return iModuleId; }
	
	inline const TBool TimeReceived() { return iTimeReceived; }
	
private:
	CGPSTimeUpdater( const TPositionModuleId& aModuleId, 
	                 CDRMClock* aClock );
	void ConstructL();
	
	void RunL();
	void DoCancel();
	TInt RunError( TInt aError );
	
private:
	RPositionServer iPosServer;
	RPositioner iPositioner;
	TPositionModuleId iModuleId;
	
	TPositionSatelliteInfo iSatelliteInfo;
	CDRMClock* iClock;
	TBool iTimeReceived;
	};

#endif
