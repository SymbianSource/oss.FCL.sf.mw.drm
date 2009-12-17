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



#ifndef RIHELLO_H
#define RIHELLO_H

//  INCLUDES

#include <hash.h>
#include "RoapMessage.h"

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
class CRIHello : public CRoapMessage
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CRIHello* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CRIHello();

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
        CRIHello();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // CRIHello( const CRIHello& );
        // Prohibit assigment operator if not deriving from CBase.
        // CRIHello& operator=( const CRIHello& );

    public:     // Data
        // ?one_line_short_description_of_data

        TRoapStatus iStatus;
        HBufC8* iSession;
        TBuf8<KMaxRoapVersionLength> iSelectedVersion;
        TBuf8<SHA1_HASH> iRiId;
        HBufC8* iRiNonce;
        HBufC8* iServerInfo;
        HBufC8* iErrorUrl;
        TBool iNeedDeviceDetails;
        TBool iPeerKeyIdentifier;
        TBuf8<SHA1_HASH> iPeerKeyId;
        TBool iCertificateCaching;
        RPointerArray<HBufC8> iAlgorithms;
        RPointerArray<HBufC8> iTrustedAuthorities;
        
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

#endif      // RIHELLO_H   
            
// End of File
