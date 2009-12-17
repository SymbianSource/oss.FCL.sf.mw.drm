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



#ifndef ROAPSIGNER_H
#define ROAPSIGNER_H

//  INCLUDES

class CSHA1;

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

// CLASS DECLARATION
class RRoapStorageClient;

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CRoapSigner: public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CRoapSigner* NewL(RRoapStorageClient& aStorageClient);
        
        /**
        * Destructor.
        */
        virtual ~CRoapSigner();

    public: // New functions
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
        
        void ResetResponses(void);
        
        void AddResponseL(
            const TDesC8& aResponse);

        TBool VerifyAndAddResponseL(
            const TDesC8& aResponse,
            const TDesC8& aSignature,
            const RPointerArray<HBufC8>& aCertificateChain);
        
        void ResetRequests(void);

        void AddRequestL(
            const TDesC8& aRequest);

        HBufC8* SignAndAddRequestL(
            const TDesC8& aRequest);
        
    public: // Functions from base classes

        /**
        * From ?base_class ?member_description.
        * @param 
        * @param 
        */

    protected:  // New functions
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        
        /**
        * C++ default constructor.
        */
        CRoapSigner();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL(RRoapStorageClient& aStorageClient);

    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        // Prohibit copy constructor if not deriving from CBase.
        // CRoapSigner( const CRoapStorageClient& );
        // Prohibit assigment operator if not deriving from CBase.
        // CRoapSigner& operator=( const CRoapStorageClient& );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data

    private:    // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
        
        CSHA1* iHash;
        RPointerArray<HBufC8> iRequests;
        RPointerArray<HBufC8> iResponses;
        RRoapStorageClient* iStorage;
         
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

#endif      // ROAPSIGNER_H
            
// End of File
