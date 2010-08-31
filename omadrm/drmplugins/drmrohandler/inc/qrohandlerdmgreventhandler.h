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


#ifndef QROHANDLERDMGREVENTHANDLER_H
#define QROHANDLERDMGREVENTHANDLER_H

#include <qobject.h>

namespace WRT
	{
	class Download;
	class DownloadEvent; 
	}

class CRoHandlerDMgrWrapper;

class QRoHandlerDMgrEventHandler : public QObject
	{
	Q_OBJECT
	
	public:
	QRoHandlerDMgrEventHandler( CRoHandlerDMgrWrapper& aRoHandlerDMgrWrapper , WRT::Download& aDownload );
	
	private slots:
		/**
		* Handle download manager events
		*
		* @param aEvent the event
		*/
		void HandleDownloadEventL( WRT::DownloadEvent* aEvent );
		
	private:
		WRT::Download& iDownload;
		CRoHandlerDMgrWrapper& iRoHandlerDMgrWrapper;
	
	};

#endif // QROHANDLERDMGREVENTHANDLER_H

