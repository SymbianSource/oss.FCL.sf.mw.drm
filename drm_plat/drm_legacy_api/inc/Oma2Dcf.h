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
* Description:  Declaration of the DCF file format class
*
*/



#ifndef OMA2DCF_H
#define OMA2DCF_H

//  INCLUDES
#include "Oma2Agent.h"
#include "DcfCommon.h"

// LOCAL CONSTANTS AND MACROS
/* constant for variable-length definitions */
#define	SRVSEC_ANY_SIZE	             1

#define SWAP32( num ) ( (num) = ( (((num) & 0xff000000) >>24) | (((num) & 0x00ff0000) >>8) | \
			  (((num) & 0x0000ff00) <<8) | (((num) & 0x000000ff) <<24) ) )

// STRUCTURES
typedef struct
    {
    TUint32 size;
    TUint32 type;
    TUint32 versionAndFlags;
    } tBoxHeaderStr;
    
// CLASS DECLARATION
class COma2DcfPartInfo;

/**
*  Encapsulates an OMA DRM 2.0 DCF file
*
*  @lib DrmDcf.lib
*  @since Series 60 3.0
*/
class COma2Dcf : public CDcfCommon
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static COma2Dcf* NewL(
            const RFile& aFile,
            TInt aPart = 0);
            
        /**
        * Destructor.
        */
        IMPORT_C virtual ~COma2Dcf();

    public: // New functions
        
        IMPORT_C void SetTransactionIdL(
            const TDesC8& aTransactionId);
        
        IMPORT_C void SetRightsObjectsL(
            RPointerArray<HBufC8>& aRoList);
        
        IMPORT_C void GetHashL();
        
        IMPORT_C static TBool IsValidDcf(
            const TDesC8& aDcfFragment);

        void ReadPartsL(void);

        void ReadContainerL(
            TInt aOffset,
            TInt& aSize);
        
        void ReadDiscreteMediaHeaderL(
            TInt aOffset,
            TInt& aSize);
        
        void ReadContentObjectL(
            TInt aOffset,
            TInt& aSize);
        
        void ReadCommonHeadersL(
            TInt aOffset,
            TInt& aSize);
        
        void ReadMutableInfoL(
            TInt aOffset,
            TInt& aSize);
        
        void ReadExtendedHeadersL(
            TInt aOffset,
            TInt aEndOfBox);
        
        void ReadBoxSizeAndTypeL(
            TInt aOffset,
            TInt& aSize,
            TUint32& aType,
            TInt& aHeaderSize);

        void ParseTextualHeadersL(
            const TDesC8& aMemoryBlock);
            
    	void RewriteMutableInfoL(
    	    const TDesC8& aTransactionId,
    	    RPointerArray<HBufC8>& aRoList);
    	
        TInt CheckUniqueId(
            const TDesC& aUniqueId);
            
        TInt OpenPart(
            const TDesC& aUniqueId);
            
        TInt OpenPart(
            TInt aPart);
            
        void GetPartIdsL(
            RPointerArray<HBufC8>& aPartList);
        
    private: // New functions 
        /**
        * VerifyTypeL
        * @param aType the type value to be verified
        * @param aRefType the reference type
        */
    	void VerifyTypeL(
    	    TUint32 aType,
    	    TUint32 aRefType);

        /**
        * ReadOneTextualHeaderL
        * @param aBlock header buffer
        * @param aName Textual header name
        * @param aBuf Buffer to save value, if not found, it should be NULL, previous data is discarded
        * @param aError KErrNone if the header was found, otherwise KErrNotFound or other errors
        * @return Offset of the header in the data block
        */    	
    	TInt ReadOneTextualHeaderL(
    	    const TDesC8& aBlock,
    	    const TDesC8& aName,
    	    HBufC8*& aBuf,
    	    TInt& aError);
    	
    	/**
        * SetHeaderWithParameterL
        * @param aValue header value
        * @param aMethod Buffer to save method value, if not found, it should be NULL, previous data is discarded
        * @param aParameter Buffer to save paramter value, if not found, it should be NULL, previous data is discarded
        */   
    	void SetHeaderWithParameterL(
    	    const TDesC8& aValue,
    	    HBufC8*& aMethod,
    	    HBufC8*& aParameter);
    	    
    	void ReadPartInfoL(
    	    COma2DcfPartInfo* aPart,
    	    TInt aOffset);
    	 
        void ReadUserDataL(
            TInt aOffset,
            TInt& aSize);
   
    	void ParseUserDataSubBoxesL(
            const TDesC8& aMemoryBlock);
            
        void ReadOneUserDataBoxL(
            const TDesC8& aBlock,
            const TDesC8& aName,
            HBufC8*& aBuf);
            
    	    
    protected:

        /**
        * C++ default constructor.
        */
        COma2Dcf();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL(
            const RFile& aFile,
            TInt aPart);
    public:     // Data
        TEncryptionPadding iEncrytionPadding;
        TSilentRefresh iSilentRefresh;
        TPreview iPreview;
        HBufC8* iPreviewParameter;
        HBufC8* iSilentParameter;
        HBufC8* iTextualHeaders;
        HBufC8* iContentUrl;
        HBufC8* iContentVersion;
        HBufC8* iContentLocation;
        HBufC8* iTransactionTracking;
        RPointerArray<HBufC8> iRightsObjects;
        HBufC8* iUserData;
        HBufC8* iAuthor;
        HBufC8* iCopyRight;
        HBufC8* iInfoUri;
        HBufC8* iGroupId;
        HBufC8* iGroupKey;
        TEncryptionMethod iGkEncryptionMethod;
        RPointerArray<COma2DcfPartInfo> iParts;
        COma2DcfPartInfo* iMutablePart;
        
        // metadataboxes
        HBufC8* iPerformer;
        HBufC8* iGenre;
        HBufC8* iRatingInfo;
        HBufC8* iClassificationInfo;
        HBufC8* iKeyword;
        HBufC8* iLocInfoName;
        HBufC8* iLocInfoAstronomicalBody;
        HBufC8* iLocInfoAdditionalNotes;
        HBufC8* iAlbumTitle;        
        HBufC8* iCoverUri;
        HBufC8* iLyricsURL;
        TUint16 iRecordingYear;
        TUint8 iAlbumTrack;
        
        // Set to ETrue if both preview and silent headers are present
        // and preview rights are to be used, EFalse otherwise
        TBool iPreviewOverridesSilentRefresh;

    protected:  // Data

    private:    // Data
        TBool iUserDataLanguageDefined;
    };

#endif      // OMA2DCF_H   
            
// End of File
