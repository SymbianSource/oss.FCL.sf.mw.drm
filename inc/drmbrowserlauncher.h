/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  API for QT browser launcher
*
*/


#ifndef DRMBROWSERLAUNCHER_H
#define DRMBROWSERLAUNCHER_H

// INCLUDES
#include <e32base.h>
// DATA TYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

namespace DRM
{


/**
*  API for QT browser launcher
*  
*
*  @lib drmbrowserlauncher.dll
*  @since Symbian V4
*/
NONSHARABLE_CLASS( CDrmBrowserLauncher ): public CBase
{
    public:  // Constructors and destructor
   			
   			/**
        * Destructor.
        * @since Symbian V4
        */
        ~CDrmBrowserLauncher();
   			
        /**
        * Two-phased constructor.
        * @since Symbian V4
        * 
        * @return New drmbrowserlauncher instace.
        */
        IMPORT_C static CDrmBrowserLauncher* NewLC();
        
        /**
        * Two-phased constructor.
        * @since Symbian V4
        * 
        * @return New drmbrowserlauncher instace.
        */
        IMPORT_C static CDrmBrowserLauncher* NewL();
        
        /**
        * Static method for launching the Browser with a given URL.
        * @since Symbian V4
        * @param &aUrl URL to be launched in browser.
        */
        IMPORT_C void LaunchUrlL(TDesC &aUrl);       
   
   	private:
   		
   			/**
        * Constructor.
        * @since Symbian V4
        */
   			CDrmBrowserLauncher();
   
   			/**
        * Two-phased constructor second part.
        * @since Symbian V4
        */
        void ConstructL();
        
};

} // namespace DRM

#endif      // DRMHELPERSESSION_H   
            
// End of File
