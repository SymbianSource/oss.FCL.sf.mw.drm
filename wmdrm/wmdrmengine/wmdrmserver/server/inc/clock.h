/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Server definitions
*
*/


#ifndef __CLOCK_H
#define __CLOCK_H

#include <e32base.h>
#include <f32file.h>

class CWmDrmServer;

namespace DRM
    {
    class CDrmServiceApiWrapper;    
    }


class CClock : public CTimer
	{
public:
	static const TInt KClockInterval = 15 * 60 * 1000000;

    static CClock* NewL( CWmDrmServer* aServer );
	~CClock();
	CClock( CWmDrmServer* aServer );

    void ConstructL();

    void Start();

    static TInt ChangeCallback( TAny* aClock );

    void HandleChange();

    TInt ReadTimeL();

    TInt WriteTimeL();

    void EvaluateCurrentTime();

    TBool TimeIsGood();

    void SetTimeAsGoodL( TBool aGood );
    
    void GetTimeL( TTime& aTime, TBool& aValid );

protected:

    void RunL();

	CEnvironmentChangeNotifier* iChangeNotifier;
	TTime iTime;
	TBool iTimeIsGood;
	CWmDrmServer* iServer;

private:
    TInt LoadServiceApi();
    
    TBool iServiceApiLoaded;
	DRM::CDrmServiceApiWrapper* iServiceApi;
	RLibrary iServiceApiLib;

	};

#endif
