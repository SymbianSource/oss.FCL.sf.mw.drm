/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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

// CLASS DECLARATION

/**
*  Class to handle all OMA DRM Dcf file format files
*
*  @lib drmdcf.dll
*  @since 3.0
*/
class CDcfCommon : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CDcfCommon* NewL(
            const RFile& aFile);

        /**
        * Two-phased constructor.
        */            
        IMPORT_C static CDcfCommon* NewL(
            const TDesC& aFileName,
            RFs* aFs = NULL);
            
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CDcfCommon();

    public: // New functions
        
        /**
        * Default implementation checks if the unique content id given
        * is the default content object. Inherited classes can re-implement
        * the method.
        *
        * @since    3.0
        * @param    aUniqueId   Requested unique content id to be checked  
        * @return   Symbian OS error code
        */
		virtual TInt CheckUniqueId(const TDesC& aUniqueId);
		
        /**
        * Open part with the requested unique content id
        *
        * @since    3.0
        * @param    aUniqueId   Content id of the requested part
        * @return   Symbian OS error code
        */		
        virtual TInt OpenPart(const TDesC& aUniqueId) = 0;

        /**
        * Open part at the requested index
        *
        * @since    3.0
        * @param    aIndex   Index of the part in the file
        * @return   Symbian OS error code
        */	      
        virtual TInt OpenPart(TInt aIndex) = 0;
        
        /**
        * List the parts of the Dcf file
        *
        * @since    3.0
        * @param    aPartList   Pointer array of parts, caller is responsible
        *                       for deletion of the list and objects
        * @leave    Symbian OS error code
        * @return   None
        */        
        virtual void GetPartIdsL(RPointerArray<HBufC8>& aPartList) = 0;

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

        /**
        * Symbian 2nd phase constructor.
        */            
        void ConstructL(
            const TDesC8& aMemoryBlock);

    public:     // Data
        // File to be used for reading
        RFile64 iFile;

        // Offset of the data part from the beginning of the file
        TInt64 iOffset;

        // The data part, used only when opening from a memory block
        HBufC8* iData;
    
        // DCF version
        TUint iVersion;

        // Size of the DCF itself
        TInt64 iLength;

        // Size of the encrypted data
        TUint64 iDataLength;

        // Size of the decrypted data
        TUint64 iPlainTextLength;

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

    };

#endif      // DCFCOMMON_H   
            
// End of File
