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



#ifndef DEVICEHELLO_H
#define DEVICEHELLO_H

//  INCLUDES

#include "hash.h"
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
class CDeviceHello : public CRoapMessage
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CDeviceHello* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CDeviceHello();

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
        virtual void GetDeviceIdsAsXmlL(CBufFlat* aBuffer);

    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();
        
    private:

        /**
        * C++ default constructor.
        */
        CDeviceHello();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // CDeviceHello( const CDeviceHello& );
        // Prohibit assigment operator if not deriving from CBase.
        // CDeviceHello& operator=( const CDeviceHello& );

    public:     // Data
        
        // Device ID. SHA1 hash array of the public keys
        RArray< TBuf8<SHA1_HASH> > iDeviceIdArray;

        // Supported ROAP Version
        TBuf8<KMaxRoapVersionLength> iVersion;

        // Optional nonce from the trigger
        HBufC8* iTriggerNonce;
        
        // Supported algorithms
        RArray<TPtrC8> iAlgorithms;

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

#endif      // DEVICEHELLO_H   
            
// End of File
