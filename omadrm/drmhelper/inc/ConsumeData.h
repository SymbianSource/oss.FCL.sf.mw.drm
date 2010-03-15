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
* Description:  the client side class for DRMHelperServer
*
*/


#ifndef CONSUMEDATA_H
#define CONSUMEDATA_H

// INCLUDES
#include <e32base.h>
#include <caf/caf.h>
#include <DRMRightsClient.h>

// DATA TYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
*  This class provides the client-side interface to the server session
*
*  @since Series 60 2.6
*/

NONSHARABLE_CLASS( CConsumeData )  : public CBase
    {
    public:  // Constructors and destructor

        /**
        * NewLC
        *
        * Created a new instance of the CConsumeData class and leaves
        * it into the cleanup stack
        *
        * @param aUri: Content identifier of the content in question
        * @return Functional CConsumeData -object
        */
        static CConsumeData* NewLC( const TDesC8& aUri );


        /**
        * NewL
        *
        * Created a new instance of the CConsumeData class
        *
        * @param aUri: Content identifier of the content in question
        * @return Functional CConsumeData -object
        */
        static CConsumeData* NewL( const TDesC8& aUri );

        /**
        * Destructor
        */
        ~CConsumeData();

    public:  // New functions

        /**
        * Consume
        *
        * Calls client consumption, if you call this with stop, the session
        * is deleted
        *
        * @param aIntent: Intent to be used for consumption
        * @return Symbian OS error code
        */
        TInt Consume( const ContentAccess::TIntent& aIntent );

        /**
        * CompateUri();
        *
        * Compares the uri as if compared with string compare
        *
        * @param aIntent: Intent to be used for consumption
        * @return Symbian OS error code
        */
        TInt CompareUri( const TDesC8& aUri ) const;

    private:
        /**
        * C++ default constructor.
        */
        CConsumeData();

        /**
        * Second phase constructor
        */
        void ConstructL( const TDesC8& aUri );

        // Content uri of the content being used
        HBufC8* iContentURI8;

        // Rights Client instance
        RDRMRightsClient iRdb;

        // Connection status
        TBool iConnectionStatus;
    };



#endif      // CONSUMEDATA_H

// End of File
