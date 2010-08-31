
#include <download.h>

#include "qrohandlerdmgreventhandler.h"
#include "rohandlerdmgrwrapper.h"

QRoHandlerDMgrEventHandler::QRoHandlerDMgrEventHandler( 
	CRoHandlerDMgrWrapper& aRoHandlerDMgrWrapper, 
	WRT::Download& aDownload )
	: iDownload(aDownload),
	  iRoHandlerDMgrWrapper(aRoHandlerDMgrWrapper)
	{
	QT_TRAP_THROWING
			(
			QObject::connect
					(
					&iDownload, SIGNAL(downloadEvent(DownloadEvent*)), 
					this, SLOT(HandleDownloadEventL(DownloadEvent*))
					)
			);
	}

void QRoHandlerDMgrEventHandler::HandleDownloadEventL( WRT::DownloadEvent* aEvent )
	{
	iRoHandlerDMgrWrapper.HandleDownloadEventL(aEvent);
	}
