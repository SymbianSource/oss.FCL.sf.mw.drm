/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



#ifndef METERINGREPORTREQ_H
#define METERINGREPORTREQ_H

//  INCLUDES

#include <hash.h>
#include <Oma2Agent.h>
#include "RoapMessage.h"
#include "RoapDef.h"
#include "OmaCrypto.h"
#include "CmlaCrypto.h"

namespace Roap
{

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
NONSHARABLE_CLASS( CMeteringReportReq ): public CRoapMessage
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMeteringReportReq* NewL();

        /**
        * Destructor.
        */
        virtual ~CMeteringReportReq();

    public: // New functions

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

        /**
        * Calculate MAC over <encryptedMeteringReport>
        * @since Series 60 3.2
        * @param None
        * @return None
        */
        void InsertMacL( const TDesC8& aMacKey );

    public: // Functions from base classes

        /**
        * From ?base_class ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        virtual HBufC8* MessageAsXmlL(void);

    protected:  // New functions

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CMeteringReportReq();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // CRightsReq( const CRightsReq& );
        // Prohibit assigment operator if not deriving from CBase.
        // CRightsReq& operator=( const CRightsReq& );

    public:     // Data
        // ?one_line_short_description_of_data

        TBuf8<SHA1_HASH> iDeviceId;
        TBuf8<SHA1_HASH> iRiId;
        TBuf8<SHA1_HASH> iEncKeyHash;
        TBuf8<KDeviceNonceLength> iNonce;
        TBuf8<KDeviceNonceLength> iReportNonce;
        TTime iTime;
        RPointerArray<HBufC8> iCertificateChain;
        HBufC8* iSignature;
        HBufC8* iCipherValue;
        HBufC8* iEncryptedMekAndMak;
        HBufC8* iMac;
        // Optional nonce from the trigger
        HBufC8* iTriggerNonce;
        TKeyTransportScheme iAlgorithmInUse;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };
}

#endif      // METERINGREPORTREQ_H

// End of File
