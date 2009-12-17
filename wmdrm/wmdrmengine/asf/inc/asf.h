/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef ASF_H
#define ASF_H

//  INCLUDES
#include <f32file.h>

// LOCAL CONSTANTS AND MACROS
    
// CLASS DECLARATION
class CAsfPartInfo;
/**
*  Encapsulates an WMDRM file
*
*  @lib DrmAsf.lib
*  @since Series 60 3.1
*/
class CAsf: public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CAsf* NewL(
            const RFile& aFile );
            
        IMPORT_C static CAsf* NewL(
            const TDesC& aFileName );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CAsf();
        
    public: // Functions from base classes
    
        
    public: // New functions
                
        /**
		* Validates whether ASF content is DRM protected or not.
		*/
        IMPORT_C static TBool IsProtected(
        	const TDesC8& aAsfHeader);

    protected: 

        /**
        * C++ default constructor.
        */
        CAsf();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL(
            const RFile& aFile );
            
		void ConstructL(
    		const TDesC& aFileName );
    
    private: // New functions
    
  		/**
  		* ValidateL();
		* Validates whether content is ASF format or not.
		*/
		void ValidateL();
		
		void InitializeL();
		
        /**
        * FormatGUID(TDes8 &aGUID);
        * Formats Global UID
        * @return void
		*/
        void FormatGUID( TDes8 &aGUID );
 
    	/**
        * Parses all entries in Content Description Object
        * @return void
		*/
		void ParseContentDescriptionObjectL();

    	/**
        * Parses all entries in Content Encryption Object
        * @return void
		*/
		void ParseContentEncryptionObjectL();

		/**
        * Parses all entries in Extended Content Description Object
        * @return void
		*/
		void ParseExtendedContentDescriptionObjectL();

		/**
        * read Extended Content Description Object
        * @return void
		*/
		HBufC16* ReadExtendedContentObjectL( TInt aOffset );

    public:    // Data
        
        // File to be used for reading
        RFs iFs;
        RFile iFile;
        
        // Size of the ASF itself
        TInt iLength;
        
        // metadata from Content Description Object
        // All metadata is UTF-16 format.
        HBufC* iTitle;
        HBufC* iAuthor;
        HBufC* iCopyright;
        HBufC* iDescription;
        HBufC* iRating;
        
        // metadata from Extended Content Description Object
        HBufC* iAlbumTitle;
        HBufC* iPicture;
        HBufC* iText;
        HBufC* iComposer;
        HBufC* iGenre;
        HBufC* iOriginalArtist;
        HBufC* iTrackNumber;
        HBufC* iUniqueFileID;
        HBufC* iAudioFileUrl;
        HBufC* iSharedUserRating;
        HBufC* iDate;
        HBufC* iYear;

        // DRM protected content
        TBool iIsDrmProtected;
        
        // ASF validated content
        TBool iIsValidated;
        
        // Header data
		HBufC8* iHeaderData;
		
		// Data from ContentEncryption Object
		HBufC8* iSecretData;
		HBufC8* iProtectionType;
		HBufC8* iKeyId;
		HBufC8* iLicenseUrl;

		// Data from ExtendedContentEncryption Object
		HBufC8* iExtendedContentEncryptionObject;
		
		// Data from DigitalSignature Object
		HBufC8* iDigitalSignatureObject;
		HBufC8* iSignedData;

		// Character Set Id used during Unicode conversion
		
		// mainly for future use: Nbr of all Objects found from Hdr
		TInt iNbrOfObjects;
		TInt iContentDescriptionOffset;
		TInt iFilePropertiesOffset;
		TInt iExtendedContentDescriptionOffset;
		TInt iContentEncryptionOffset;
		TInt iExtendedContentEncryptionOffset;
		TInt iDigitalSignatureOffset;

		TInt iExtendedContentDescriptionCount;
        TInt iHeaderSize;
		TInt iTitleLength;
		TInt iAuthorLength;
		TInt iCopyrightLength;
		TInt iDescriptionLength;
		TInt iRatingLength;
		
		// Details from Content Encryption Object
		TInt iSecretDataLength;
		TInt iProtectionTypeLength;
		TInt iKeyIDLength;
		TInt iLicenceUrlLength;

		TBool iContentDescriptionObjectExists;
		TBool iFilePropertiesObjectExists;
		TBool iExtendedContentDescriptionObjectExists;
		TBool iExtendedContentEncryptionObjectExists;
		TBool iContentEncryptionObjectExists;
		TBool iDigitalSignatureObjectExists;
		
		TPtrC iMimeType;

        // End offset of file properties object
        // Used to decide start point of signed data
        TInt iFilePropertiesEndOffset;
		
        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // ASF_H   
            
// End of File
