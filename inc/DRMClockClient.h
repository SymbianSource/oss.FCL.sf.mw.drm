/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is the client side handle of DRM Clock Server
*
*/



#ifndef RDRMCLOCKCLIENT_H
#define RDRMCLOCKCLIENT_H

//  INCLUDES
#include <e32std.h>
#include <DRMTypes.h>

// CONSTANTS
// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  This class is the client side interface for DRM Clock Server.
*
*  @lib DRM Core
*  @since S60 Release 2.5
*/
NONSHARABLE_CLASS( RDRMClockClient ) : public RSessionBase
    {
    public:  // Constructor & destructor
        /**
         * C++ default constructor.
         */
        IMPORT_C RDRMClockClient();

        /**
         * Destructor.
         */
        IMPORT_C virtual ~RDRMClockClient();

        /**
         * This method opens the connection between the client and the server.
         * @since 2.5
         * @return Error code. KErrNone if the operation is successful.
         */
        IMPORT_C TInt Connect();

        /**
         * This function closes the connection between the client and the server.
         * It is safe to call this method even if connection is not established.
         * @since S60Rel2.5
         */
        IMPORT_C void Close();

    public: // New functions

        /**
         * Get the secure time.
         * @since 2.6
         * @param aTime           Output parameter containing the
         *                        secure time in UTC.
         * @param aTimeZone       Time zone information including summer/winter time
         *                        in +/- 15 minutes
         * @param aSecurityLevel  Output parameter Securiry level.
         * @return Symbian OS error code if any.
         */
        IMPORT_C TInt GetSecureTime( TTime& aTime,
                            TInt& aTimeZone,
                            DRMClock::ESecurityLevel& aSecurityLevel ) const;

        /**
         * Update the secure time.
         * @since 2.6
         * @param aTime           Input parameter containing the
         *                        secure time in UTC.
         * @param aTimeZone       Time zone information including summer/winter time
         *                        in +/- 15 minutes
         * @return Symbian OS error code if any.
         */
        IMPORT_C TInt UpdateSecureTime( const TTime& aTime, const TInt& aTimeZone );


    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:
        // Prohibit copy constructor if not deriving from CBase.
        RDRMClockClient( const RDRMClockClient& );
        // Prohibit assigment operator if not deriving from CBase.
        RDRMClockClient& operator=( const RDRMClockClient& );

    public:     // Data

    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes
    };

#endif      // RDRMCLOCKCLIENT_H

// End of File
