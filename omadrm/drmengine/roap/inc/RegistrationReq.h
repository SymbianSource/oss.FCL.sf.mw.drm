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



#ifndef REGISTRATIONREQ_H
#define REGISTRATIONREQ_H

//  INCLUDES

#include <hash.h>
#include "RoapMessage.h"

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
class CRegistrationReq : public CRoapMessage
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CRegistrationReq* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CRegistrationReq();

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
        CRegistrationReq();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // CRegistrationReq( const CRegistrationReq& );
        // Prohibit assigment operator if not deriving from CBase.
        // CRegistrationReq& operator=( const CRegistrationReq& );

    public:     // Data
        // ?one_line_short_description_of_data

        HBufC8* iSession;
        TBuf8<KDeviceNonceLength> iNonce;

        // Device time in UTC
        TTime iTime;
        RPointerArray<HBufC8> iCertificateChain;
        HBufC8* iServerInfo;
        HBufC8* iSignature;
        HBufC8* iDeviceDetailsManufacturer;
        HBufC8* iDeviceDetailsModel;
        HBufC8* iDeviceDetailsVersion;
        HBufC8* iOcspResponderKeyId;
        TBuf8<SHA1_HASH> iPeerKeyIdentifier;
        TBool iOcspInfoStored;
        RPointerArray<HBufC8> iTrustedAuthorities;
    
        // Optional nonce from the trigger
        HBufC8* iTriggerNonce;

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

#endif      // REGISTRATIONREQ_H   
            
// End of File
