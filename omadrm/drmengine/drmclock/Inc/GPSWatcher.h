
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


class CGPSTimeUpdater;
class CDRMClock;

class CGPSWatcher : public CTimer
	{
public:
	~CGPSWatcher();
	static CGPSWatcher* NewL( CDRMClock* aClock );

private:
	CGPSWatcher( CDRMClock* aClock );
	void ConstructL();
	
	void CheckModules();
	void Subscribe();
	void RunL();
	void DoCancel();
	TInt RunError( TInt aError );
	
private:
	RPositionServer iPosServer;
	TPositionModuleStatusEvent iStatusEvent;
	
	// owned
	CGPSTimeUpdater *iTimeUpdater;
	
	// not owned
	CDRMClock* iClock;

	// Retry counter
	TInt iRetryCounter;
	};

#endif /* GPSWATCHER_H_ */
