
#include <download.h>

#include "qdrmutilitydmgreventhandler.h"
#include "drmutilitydmgrwrapper.h"

QDrmUtilityDmgrEventHandler::QDrmUtilityDmgrEventHandler( CDrmUtilityDmgrWrapper& aDrmUtilityDmgrWrapper, WRT::Download& aDownload )
	: iDownload(aDownload),
	  iDrmUtilityDmgrWrapper(aDrmUtilityDmgrWrapper)
	{
	QT_TRAP_THROWING(
					QObject::connect(	&iDownload, 
										SIGNAL(downloadEvent(DownloadEvent*)), 
										this, SLOT(HandleDownloadEventL(DownloadEvent*))
									)
					);
	}

void QDrmUtilityDmgrEventHandler::HandleDownloadEventL( WRT::DownloadEvent* aEvent )
	{
	iDrmUtilityDmgrWrapper.HandleDownloadEventL(aEvent);
	}
