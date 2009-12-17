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
* Description:  DRM3 Notifier manages all notifications out of the system.
*
*/



#ifndef CDRMNOTIFIERSERVER_H
#define CDRMNOTIFIERSERVER_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "DRMMessageStorage.h"
// #include "DRMClientServer.h"
#include "DRMNotifierSession.h"

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
TInt StartupNotifier( TAny* );
 
// FORWARD DECLARATIONS
class CLogFile;

// CLASS DECLARATION

/**
*  This class implements the DRM3 Notifier Server functionality.
*
*  @lib RightsServer.exe
*  @since S60Rel2.6
*/
NONSHARABLE_CLASS( CDRMNotifierServer ) : public CServer2
    {
    public:  // Constructors and destructor
        
        /**
         * Two-phased constructor.
         * @since S60Rel2.6
         * @return Pointer to newly created server instance.
         */
        static CDRMNotifierServer* NewL();
        
        /**
         * Destructor.
         * @since S60Rel2.6
         */
        ~CDRMNotifierServer();
        
    public: // Functions from base classes

    protected:  // New functions
        
    protected:  // Functions from base classes
        //class CServer
        /**
         * RunError.
         * @since S60Rel2.6
         * @param aError Error code from RunL function.
         * @return An error is returned if RunError() cannot process the
         * error.
         */
        TInt RunError( TInt aError );
        
    private:
        /**
         * C++ default constructor.
         * @since S60Rel2.6
         */
        CDRMNotifierServer();
        
        /**
         * By default Symbian 2nd phase constructor is private.
         * @since S60Rel2.6
         */
        void ConstructL();
        
    private: // Functions from base classes
        // Class CServer
        /** 
         * NewSessionL
         * @since S60Rel2.6
         */
        CSession2* NewSessionL( const TVersion& aVersion,
                                const RMessage2& aMessage) const;
        
    public:     // Data
        
    protected:  // Data
        
    private:    // Data
        CDRMMessageStorage* iStorage;

        
    public:     // Friend classes
        
    protected:  // Friend classes
        
    private:    // Friend classes
        
};

#endif      // CDRMRIGHTSSERVER_H   

// End of File
