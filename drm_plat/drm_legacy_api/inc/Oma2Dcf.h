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
        
        /**
        * Set the transaction id of the OMA DRM 2 dcf file
        *
        * @param    aTransactionId  The transaction id to be set
        * @leave    Symbian OS error code
        * @return   None
        */
        IMPORT_C void SetTransactionIdL(
            const TDesC8& aTransactionId);

        /**
        * Set rights objects to the end of the OMA DRM 2 dcf
        * into the mutable box
        *
        * @param    aRoList  List of XML format rights objects
        * @leave    Symbian OS error code
        * @return   None
        */        
        IMPORT_C void SetRightsObjectsL(
            RPointerArray<HBufC8>& aRoList);
        
        /**
        * Not implemented
        *
        * @leave    KErrNotSupported
        * @return   None
        */
        IMPORT_C void GetHashL();
        
        /**
        * Checks if the provided fragment is a piece of a valid dcf file
        *
        * @param    aDcfFragment    Fragment of a Dcf file
        * @leave    Symbian OS error code
        * @return   ETrue           If the fragment is a part of a dcf file
        *           EFalse          If the fragment is not part of a dcf file
        */                
        IMPORT_C static TBool IsValidDcf(
            const TDesC8& aDcfFragment);

        /**
        * Read the parts of an OMA DRM 2 dcf file
        * Uses the following methods for reading through all the parts
        *
        * @leave    Symbian OS error code
        * @return   None
        */           
        void ReadPartsL(void);


        /**
        * Reads the container at the given offset and returns the size
        *
        * @param    aOffset     Offset to the start of the container
        * @param    aSize       Return parameter for the size of the container
        * @leave    Symbian OS error code
        * @return   None
        */   
        void ReadContainerL(
            TInt64 aOffset,
            TInt64& aSize);

        /**
        * Reads the discrete media header box from the offset and returns the size
        *
        * @param    aOffset     Offset to the start of the container
        * @param    aSize       Return parameter for the size of the container
        * @leave    Symbian OS error code
        * @return   None
        */        
        void ReadDiscreteMediaHeaderL(
            TInt64 aOffset,
            TInt64& aSize);

        /**
        * Reads the content object box from the offset and returns the size
        *
        * @param    aOffset     Offset to the start of the container
        * @param    aSize       Return parameter for the size of the container
        * @leave    Symbian OS error code
        * @return   None
        */           
        void ReadContentObjectL(
            TInt64 aOffset,
            TInt64& aSize);

        /**
        * Reads the common headers box from the offset and returns the size
        *
        * @param    aOffset     Offset to the start of the container
        * @param    aSize       Return parameter for the size of the container
        * @leave    Symbian OS error code
        * @return   None
        */          
        void ReadCommonHeadersL(
            TInt64 aOffset,
            TInt64& aSize);

        /**
        * Reads the container at the given offset and returns the size
        *
        * @param    aOffset     Offset to the start of the container
        * @param    aSize       Return parameter for the size of the container
        * @leave    Symbian OS error code
        * @return   None
        */         
        void ReadMutableInfoL(
            TInt64 aOffset,
            TInt64& aSize);

        /**
        * Reads the extended headers box at the given offset
        * 
        * @param    aOffset     Offset to the start of the container
        * @param    aEndOfBox   Offset for the end of the extended headers box
        * @leave    Symbian OS error code
        * @return   None
        */          
        void ReadExtendedHeadersL(
            TInt64 aOffset,
            TInt64 aEndOfBox);

        /**
        * Reads the size of the box
        *
        * @param    aOffset     Offset to the start of the container
        * @param    aSize       Return parameter for the size of the container
        * @param    aType       Return parameter for the type of the box
        * @param    aHeadersize Return parameter for the size of the container headers
        * @leave    Symbian OS error code
        * @return   None
        */            
        void ReadBoxSizeAndTypeL(
            TInt64 aOffset,
            TInt64& aSize,
            TUint32& aType,
            TUint32& aHeaderSize);

        /**
        * Parses the textual headers from the memory block
        *
        * @leave    Symbian OS error code
        * @return   None
        */ 
        void ParseTextualHeadersL(
            const TDesC8& aMemoryBlock);

        /**
        * Re-writes the mutable box to the filesystem
        *
        * @param    aTransactionId  Transaction id to be updated
        * @param    aRoList         List of rights objects to be written
        *                           to the box
        * @leave    Symbian OS error code
        * @return   None
        */             
    	void RewriteMutableInfoL(
    	    const TDesC8& aTransactionId,
    	    RPointerArray<HBufC8>& aRoList);

public: // From base class, overriding their implementation    	

        /**
        * Checks if the unique content id is any of the id's in the
        * OMA DRM 2 Dcf file
        *
        * @param    aUniqueId   Requested unique content id to be checked  
        * @return   Symbian OS error code
        */
        TInt CheckUniqueId(
            const TDesC& aUniqueId);

        /**
        * Open part with the requested unique content id
        *
        * @param    aUniqueId   Content id of the requested part
        * @return   Symbian OS error code
        */	            
        TInt OpenPart(
            const TDesC& aUniqueId);

        /**
        * Open part at the requested index
        *
        * @param    aPart   Index of the part in the file
        * @return   Symbian OS error code
        */	              
        TInt OpenPart(
            TInt aPart);

        /**
        * List the parts of the Dcf file
        *
        * @param    aPartList   Pointer array of parts, caller is responsible
        *                       for deletion of the list and objects
        * @leave    Symbian OS error code
        * @return   None
        */             
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

        /**
        * Read a part info object from the offset
        *
        * @param    aPart   Pointer to the part
        * @param    aOffset Offset of the part
        * @leave    Symbian OS error code
        * @return   None
        */	    
    	void ReadPartInfoL(
    	    COma2DcfPartInfo* aPart,
    	    TInt64 aOffset);

        /**
        * Read user data from the offset and return the size
        *
        * @param    aOffset Offset of the user data
        * @param    aSize   Return parameter for the size
        * @leave    Symbian OS error code
        * @return   None
        */	 
        void ReadUserDataL(
            TInt64 aOffset,
            TInt64& aSize);

        /**
        * Parse user data sub boxes
        *
        * @param    aMemoryBlock    Memory block containing user data sub boxes
        * @leave    Symbian OS error code
        * @return   None
        */
    	void ParseUserDataSubBoxesL(
            const TDesC8& aMemoryBlock);

        /**
        * Read one user data box
        *
        * @param    aBlock  Block of data containing the box
        * @param    aName   Name of the box to read
        * @param    aBuf    Return parameter for the box caller is responsible
        *                   for deleting the object
        * @leave    Symbian OS error code
        * @return   None
        */            
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
    public:     
        // Data
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

    private:    // Data
        TBool iUserDataLanguageDefined;
    };

#endif      // OMA2DCF_H   
            
// End of File
