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



#ifndef JOINDOMAINRESP_H
#define JOINDOMAINRESP_H

//  INCLUDES
#include <hash.h>
#include "RoapMessage.h"
#include "DRMRightsClient.h"

namespace Roap
{

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CJoinDomainResp : public CRoapMessage
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CJoinDomainResp* NewL();

        /**
        * Destructor.
        */
        virtual ~CJoinDomainResp();

    public: // New functions

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From ?base_class ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

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
        CJoinDomainResp();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // CJoinDomainResp( const CJoinDomainResp& );
        // Prohibit assigment operator if not deriving from CBase.
        // CJoinDomainResp& operator=( const CJoinDomainResp& );

    public:     // Data
        // ?one_line_short_description_of_data
        TRoapStatus iStatus;
        TBuf8<SHA1_HASH> iDeviceId;
        TBuf8<SHA1_HASH> iRiId;
        TBuf8<SHA1_HASH> iDomainKeyRiId;
        RPointerArray<HBufC8> iMacs;

        TTime iDomainExpiration;
        TBool iHashChainSupport;
        RPointerArray<HBufC8> iDomainKeys;
        RPointerArray<HBufC8> iCertificateChain;
        RPointerArray<HBufC8> iOcspResponse;
        HBufC8* iSignature;
        HBufC8* iErrorUrl;
        RPointerArray<HBufC8> iDomainKeyIDs;
        TKeyTransportScheme iTransportScheme;

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

#endif      // JOINDOMAINRESP_H

// End of File
