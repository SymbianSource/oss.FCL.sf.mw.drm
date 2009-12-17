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



#ifndef OMA1DCF_H
#define OMA1DCF_H

//  INCLUDES
#include "DcfCommon.h"

// CONSTANTS
const TUint KOma1DcfVersion = 0x00010000;
_LIT8(KContentName, "Content-Name");
_LIT8(KContentDescription, "Content-Description");
_LIT8(KRightsIssuer, "Rights-Issuer");
_LIT8(KContentVendor, "Content-Vendor");
_LIT8(KIconURI, "Icon-URI");

// FORWARD DECLARATIONS
class RWriteStream;
class RFile;

// CLASS DECLARATION

/**
*  Encapsulates an OMA DRM 1 DCF content file
*  ?other_description_lines
*
*  @lib ?library
*  @since Series 60 3.0
*/
class COma1Dcf : public CDcfCommon
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static COma1Dcf* NewL(
            const RFile& aFile);
            
        IMPORT_C static COma1Dcf* NewL(
            const TDesC8& aMemoryBlock);

        /**
        * Destructor.
        */
        IMPORT_C virtual ~COma1Dcf();

    public: // New functions
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        IMPORT_C static TBool IsValidDcf(
            const TDesC8& aDcfFragment);

        TInt ReadHeaderL(
            const TDesC8& aMemoryBlock);

        TInt ReadHeaderL(void);

        TInt CreateHeaderL(
            RWriteStream&); // use only when you know what you're doing! Writes only the header?
            
        TInt CheckUniqueId(
            const TDesC& aUniqueId);
            
        TInt OpenPart(
            const TDesC& aUniqueId);
            
        TInt OpenPart(
            TInt aPart);
            
        void GetPartIdsL(
            RPointerArray<HBufC8>& aPartList);
            
        void ReadHeaderValuesL(void);
            
        IMPORT_C TInt GetHeaderL(
            const TDesC8& aHeaderName,
            TPtrC8& aHeaderValue);

    public: // Functions from base classes

        /**
        * From ?base_class ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        
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

        COma1Dcf();

        void ConstructL(
            const RFile& aFile);
            
        void ConstructL(
            const TDesC8& aMemoryBlock);

        // Prohibit copy constructor if not deriving from CBase.
        // COma1Dcf( const COma1Dcf& );
        // Prohibit assigment operator if not deriving from CBase.
        // COma1Dcf& operator=( const COma1Dcf& );
        
        static HBufC8* FetchOmaBasedInfoL();

    public:     // Data
        // ?one_line_short_description_of_data
        TUint iHeaderLength;

        HBufC8* iHeaders;

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

#endif      // OMA1DCF_H   
            
// End of File
