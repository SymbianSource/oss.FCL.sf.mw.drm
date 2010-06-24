/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DRM Browser Launcher class
*
*/


// INCLUDE FILES
#include "drmbrowserlauncher.h"
#include <QDesktopServices>
#include <QString>
#include <QUrl>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------
//
DRM::CDrmBrowserLauncher::CDrmBrowserLauncher()
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
DRM::CDrmBrowserLauncher::~CDrmBrowserLauncher()
    {
    }

// -----------------------------------------------------------------------------
// CDrmBrowserLauncher::NewLC
// First phase constructor
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmBrowserLauncher* DRM::CDrmBrowserLauncher::NewLC()
    {
    DRM::CDrmBrowserLauncher* self( new( ELeave ) CDrmBrowserLauncher );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CDrmBrowserLauncher::NewL
// First phase constructor
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmBrowserLauncher* DRM::CDrmBrowserLauncher::NewL()
    {
    DRM::CDrmBrowserLauncher* self( NewLC() );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CDrmBrowserLauncher::LaunchUrlL
// Static method for launching the Browser with a given URL.
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmBrowserLauncher::LaunchUrlL(TDesC &aUrl)
		{
		// convert given URL to QUrl format
		QString urlString = QString::fromUtf16(aUrl.Ptr(), aUrl.Length());
		QUrl* url = new QUrl(urlString);
		bool ret = QDesktopServices::openUrl(*url);
		if(!ret)
				{
				User::Leave(KErrNotFound);
				}
			
		}

void DRM::CDrmBrowserLauncher::ConstructL()
    {
    }
    


//  End of File
