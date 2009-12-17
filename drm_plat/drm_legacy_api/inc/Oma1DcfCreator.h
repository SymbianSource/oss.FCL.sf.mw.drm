/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of class DRMCommon for accessing common
*     DRM functions
*
*/


#ifndef OMADCF1CREATOR_H
#define OMADCF1CREATOR_H

// INCLUDES
#include <f32file.h>
#include <DRMRightsClient.h>

// CONSTANTS
const TUint KBlockSize = 16;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class RWriteStream;
class CDRMRights;
class CSymmetricCipher;

// CLASS DECLARATION

/**
*  Accessing content and rights.
*
*  @lib COma1DcfCreator.dll
*  @since Series 60 2.0
*/
NONSHARABLE_CLASS(COma1DcfCreator): public CBase
    {
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    IMPORT_C static COma1DcfCreator* NewL(void);

    /**
    * Destructor.
    */
    IMPORT_C virtual ~COma1DcfCreator(void);
    
public: // New functions
    
    IMPORT_C void EncryptInitializeL(
        RWriteStream& anOutput,
        const TDesC8& aMIMEType, 
        CDRMRights* aRightsObject);
    
    /**
    * EncryptUpdateL
    *
    * add processed content into stream
    *
    * @since  2.5
    * @param  aMessageData: contains processed content 
    * @return 
    */
	IMPORT_C void EncryptUpdateL( 
		const TDesC8& aMessageData);


    /**
    * EncryptFinalizeL
    *
    * finalize the encryption
    *
    * @since  2.0
    * @return 
    */
	IMPORT_C void EncryptFinalizeL();
  
    /**
    * EncryptContentL
    *
    * Takes a memory buffer with plain text closed content and turns it
    * into a DCF by adding the necessary headers.
    *
    * @since  3.0
    * @param  aContent: buffer containing the plain content, this will 
    *         be overwritten with the resulting DCF data
    * @param  aMIMEType: MIME type to use for the DCF
    * @param  aRightsObject: Rights and content URI for the new content.
    * @return -
    */
    IMPORT_C void EncryptContentL(
        HBufC8*& aContent, 
        const TDesC8& aMIMEType, 
        CDRMRights* aRightsObject = NULL);
    
    /**
    * EncryptContentToFileL
    *
    * Takes a memory buffer with plain text closed content and turns it
    * into a DCF by adding the necessary headers.
    *
    * @since  3.0
    * @param  aPlainContent: buffer containing the plain content.
    * @param  aFileName: The file name for  the DCF 
    * @param  aMIMEType: MIME type to use for the DCF 
    * @param  aRightsObject: Rights and content URI for the new content.
    * @return -
    */
    IMPORT_C void EncryptContentToFileL(
        const TDesC8& aContent, 
        const TDesC& aFileName, 
        const TDesC8& aMIMEType, 
        CDRMRights* aRightsObject = NULL);
    
    /**
    * EncryptFileL
    *
    * Takes the name of a file with plain text closed content and turns it
    * into a DCF by adding the necessary headers.
    *
    * @since  3.0
    * @param  aFileName: The file name for the original content.
    * @param  aDCFFile: The file name for the DCF
    * @param  aMIMEType: MIME type to use for the DCF.
    * @param  aRightsObject: Rights and content URI for the new content.
    * @return -
    */
    IMPORT_C void EncryptFileL(
        const TDesC& aFileName, 
        const TDesC& aDCFFileName, 
        const TDesC8& aMIMEType, 
        CDRMRights* aRightsObject = NULL);

    /**
    * GetBytesWritten
    *
    * Returns the number of bytes written in the encryption process
    *
    * @since  3.0
    * @return The number of bytes written.
    */
    IMPORT_C TUint GetBytesWritten(void);
    
protected:
    
    /**
    * C++ default constructor.
    */
    IMPORT_C COma1DcfCreator(void);
    
private:
    
    /**
    * By default Symbian 2nd phase constructor is private.
    */
    IMPORT_C void ConstructL(void);

private: //data
    RWriteStream* iEncryptionStream;
    TUint iBytesWritten;
    RDRMRightsClient iRdb;
    TBuf8<KDCFKeySize> iIv;
    TBuf8<KDCFKeySize> iRemainder;
    HBufC8* iEncryptionBuffer;
    };

#endif      // OMADCF1CREATOR_H

// End of File

