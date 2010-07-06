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
* Description:
*
*/


#ifndef QDRMUTILITYDMGREVENTHANDLER_H
#define QDRMUTILITYDMGREVENTHANDLER_H

#include <qobject.h>

namespace WRT
	{
	class Download;
	class DownloadEvent;
	}

class CDrmUtilityDmgrWrapper;

class QDrmUtilityDmgrEventHandler : public QObject
    	{
    	Q_OBJECT
    	
    public:
    	QDrmUtilityDmgrEventHandler( CDrmUtilityDmgrWrapper& aDrmUtilityDmgrWrapper , WRT::Download& aDownload );
	
    public slots:
    	
        /**
        * Handle download manager events
        *
        * @param aEvent the event
        */
        void HandleDownloadEventL( WRT::DownloadEvent* aEvent );
        
    private:
        WRT::Download& iDownload;
        CDrmUtilityDmgrWrapper& iDrmUtilityDmgrWrapper;
    	
    	};

#endif // QDRMUTILITYDMGREVENTHANDLER_H
