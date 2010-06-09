/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef DCFCOMMON_H
#define DCFCOMMON_H

//  INCLUDES
#include <apmstd.h>
#include <f32file.h>
#include <Oma2Agent.h>
#include <DRMTypes.h>

using namespace ContentAccess;
// CONSTANTS

// FORWARD DECLARATIONS
class RFile;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CDcfCommon : public CBase
    {
    public:  // Enumerations

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CDcfCommon* NewL(
            const RFile& aFile);
            
        IMPORT_C static CDcfCommon* NewL(
            const TDesC& aFileName,
            RFs* aFs = NULL);
            
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CDcfCommon();

    public: // New functions
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
		virtual TInt CheckUniqueId(const TDesC& aUniqueId);
        virtual TInt OpenPart(const TDesC& aUniqueId) = 0;
        virtual TInt OpenPart(TInt Index) = 0;
        virtual void GetPartIdsL(RPointerArray<HBufC8>& aPartList) = 0;

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

    protected:

        /**
        * C++ default constructor.
        */
        CDcfCommon();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL(
            const RFile& aFile);
            
        void ConstructL(
            const TDesC8& aMemoryBlock);
            
        // Prohibit copy constructor if not deriving from CBase.
        // CDcfCommon( const CDcfCommon& );
        // Prohibit assigment operator if not deriving from CBase.
        // CDcfCommon& operator=( const CDcfCommon& );

    public:     // Data
        // File to be used for reading
        RFile iFile;

        // Offset of the data part from the beginning of the file
        TInt iOffset;

        // The data part, used only when opening from a memory block
        HBufC8* iData;
    
        // DCF version
        TUint iVersion;

        // Size of the DCF itself
        TInt iLength;

        // Size of the encrypted data
        TUint iDataLength;

        // Size of the decrypted data
        TUint iPlainTextLength;

        // Flag indicating if the padding is removed from the plaintext length
        TBool iPlainTextLengthValid;

        // Original MIME type
        HBufC8* iMimeType;

        // Unique OMA DCF content ID
        HBufC8* iContentID;

        // Rights issuer URL for rights refresh
        HBufC8* iRightsIssuerURL;

        // Size of the padding for the encrypted data, negative if not valid
        TInt iPadding;
        
        // Encryption method (NULL, CBC or CTR)
        TEncryptionMethod iEncryptionMethod;

        // Title of the content (content name for OMA DRM 1.0 DCF files)
        HBufC8* iTitle;
        
        // Content description
        HBufC8* iDescription;
        
        // URI for a descriptive icon
        HBufC8* iIconUri;

    protected:  // Data

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

#endif      // DCFCOMMON_H   
            
// End of File
