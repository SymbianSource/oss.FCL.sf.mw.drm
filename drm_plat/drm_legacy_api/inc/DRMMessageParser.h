/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface to BbB encryption.
*
*/



#ifndef CDRMMESSAGEPARSER_H
#define CDRMMESSAGEPARSER_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class COma1DcfCreator;
class CDRMRights;
class RDRMRightsClient;

// CLASS DECLARATION

/**
*  This class handles all DRM Message parsing and B64 decoding, 
*  and feeds the resulting data either to 
*  DRMAuthenticated::Encrypt[Initialize|Update|Finalize]L or writes the 
*  data directly to given output stream (DCF in DRM Message).
*  The internal state iState is modified in quite many places in the
*  class methods.
*
*  @lib DRMAuthenticated.dll
*  @since S60Rel2.5
*/
NONSHARABLE_CLASS(CDRMMessageParser): public CBase
    {
public:  // Constructors and destructor
     enum TDRMMessageParserState
         {
         ESearchingBoundary =       0x00, // The first operation to do.
         EGotBoundary =             0x01,
         EReadingHeaderPart =       0x02,
         EReadingRightsPart =       0x04,
         EGotRightsPart =           0x08,
         EReadingContentPart =      0x10,
         EGotContentPart =          0x20,
         EGotContentEncoding =      0x40,
         EBase64 =                  0x80,
         EGotContentType =          0x100,
         EDCFFile =                 0x200,
         EDCFURIModified =          0x400,
         EDCFHeadersModified =      0x800,
         EEncryptStreamOk =		   0x1000,
         EFLContent =               0x2000,
         EAllDone =                 0x4000,
         EBroken =                  0x8000
         };
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CDRMMessageParser* NewL( void );
    
    /**
     * Destructor.
     */
    IMPORT_C virtual ~CDRMMessageParser( void );
    
    
public: // New functions
    
    /**
    * InitializeMessageParserL
    *
    * @since 2.0
    * DRM message Block-by-Block parser initialization method must be called 
    * once before ProcessMessageDataL can be called. All processed data is 
    * written to given write stream when calling ProcessMessageDataL and 
    * FinalizeMessageParserL methods.
    * 
    * InitializeMessageParserL, ProcessMessageDataL and FinalizeMessageParserL
    * use EncryptInitializeL, EncryptUpdateL and EncryptFinalizeL internally,
    * so one instance of DRMAuthenticated can only process one encryption
    * session at a time.
    * @param aOutput Write stream where the processed data is written.
    */
    IMPORT_C void InitializeMessageParserL( 
        RWriteStream& aOutput );

    /**
    * ProcessMessageDataL
    *
    * @since 2.0
    * This method processes the given part of DRM message data. If 
    * there is some data to be returned (there might not always be),
    * the processed data is written to the output stream.
    * @param aMessageData Part of DRM message data to be processed.
    */
    IMPORT_C void ProcessMessageDataL( 
        const TDesC8& aMessageData );

    /**
    * FinalizeMessageParserL
    * 
    * @since 2.0
    * This method finalizes the BbB parsing of DRM message. The method
    * flushes the internal buffers returns any remaining processed data
    * to output stream depending on the initialization. After calling this 
    * method the parser needs to 
    * be re-initialized in order to process more data.
    */ 
    IMPORT_C void FinalizeMessageParserL();
    
    /**
    * ProcessMessage
    *
    * Takes a memory buffer containing a DRM message, splits the message 
    * into the contained parts and processes the parts. After completion 
    * of the function, the input buffer contains the DCF from the message, 
    * the Rights Object is saved to the rights database..
    *
    * @since  2.0
    * @param  aDRMMessage: In/out parameter containing the DRM message 
    *         before the function call and the DCF after the function call 
    * @return EOk if the message has been processed successfully
    */
    IMPORT_C TInt ProcessMessage(
        HBufC8*& aDRMMessage);
    
    /**
    * ProcessRightsObject
    *
    * Takes a memory buffer containing a Rights Object and saves it to the 
    * rights database. Returns the detailed rights specification of the
    * Rights Object.
    *
    * @since  2.0
    * @param  aRightsObject: Buffer containing the rights object
    * @param  aRightsDetail: Out parameter for the details of the rights
    *         in the rights object
    * @return EOk if the rights object could be stored successfully
    */
    IMPORT_C TInt ProcessRightsObject(
        const TDesC8& aRightsObject, 
        RPointerArray<CDRMRights>& aRightsDetail);
    
        /** 
        * Cleans up the internal storages in case of 
        * CleanupStack::PopAndDestroy()
        */
        void Release( void );

public: // Functions from base classes
    
protected:  // New functions
    
    /**
    * C++ default constructor.
    */
    CDRMMessageParser( void );

protected:  // Functions from base classes
    
private:
    
    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL( void );
    
    /**
    * Processes the data from DRM Message body.
    */
    void HandleContentDataL( void );

    /**
    * Processes the rights object data part from DRM Message.
    */
    void HandleRightsDataL( void );

    /**
    * Tries to locate the boundary separator from the beginning of the 
    * message, and if a boundary is found, it is stored.
    */
    void FindBoundaryL( void );

    /**
    * Parses the DRM Message header part.
    */
    void ReadHeaderL( void );
    
    /**
    * Returns a single line from the message, ending to \n.
    * @return Reference to a single line in the message.
    */
    TPtrC8 GetLineL( void );

    /**
    * Extracts a MIME header value from the given line. Ignores
    * possible parameters.
    * @param aLine Line to be searched for the header value.
    * @return Reference to the header value found.
    */
    TPtrC8 HeaderValueL( const TDesC8& aLine );
    
    /**
    * Compresses the internal buffers discarding all already used
    * data.
    */
    void CompressInputBuffer( void );
    
    /**
    * Prepares the content data to be encrypted/processed, i.e.
    * constructs a descriptor to iInputBuffer which can then be
    * processed.
    */
    void PrepareContentDataL( void );

    /**
    * Do base64 decoding to the given descriptor. 
    * @param aData in/out descriptor: after successful call, the 
    * descriptor contains base64 decoded data.
    * @return How many base64 encoded bytes from aData was consumed.
    */ 
    TInt HandleBase64DataL( TPtrC8& aData );

    /**
    * Extracts the endline characters from the given descriptor.
    * @param aBuf A single line to be stripped from endline characters.
    * The descriptor length is changed to corresponding value.
    */
    void StripEndLineL( TPtrC8& aBuf );

    /**
    * Tells whether the given line is a boundary line, and if so, is it
    * the final boundary marking the end of the message.
    * @param aLine Line.
    * @param aLast Out-parameter: if the line is a boundary line and this
    * is the last boundary, aLast == ETrue after the call.
    * @return Truth value.
    */
    TBool IsBoundary( const TDesC8& aLine, 
                      TBool& aLast ) const;
    
    /**
    * This method generates a FL rights object if the message being
    * processed is a FL message. If the message is a CD message,
    * no rights object. Saves the rights object into rights database.
    */
    void HandleFlContentL( void );
    
    /**
    * Helper method for HandleFlContentL: saves the RO to the database.
    * @param aCID Content-ID.
    * @param aData rights data.
    */
    void ProcessRightsDataL( const TDesC8& aCID,
                            const TDesC8& aData );
                            
    /**
    * Writes the (base64 decoded) data to EncryptUpdateL or directly to 
    * the output stream. Modifies the content-type of DCF file in case 
    * "DCF inside DRM Message" case.
    * @param aData Data to be encrypted / written to the stream.
    */
    void ProcessContentDataL( TPtrC8& aData );

    void EnsureFLRightsExistL( RDRMRightsClient& aClient, CDRMRights** aOutput );
    /**
    * Checks whether the given character is valid base64 encoded character.
    * @param aChar Character to be checked.
    * @return Truth value.
    */
    TBool ValidB64CharL( const TUint8 aChar );

    /** 
    * Sets an internal state bit.
    * @param aBit New state bit to be enabled.
    */
     void SetBit( TUint32 aBit );

    /**
    * Clears an internal state bit.
    * @param aBit The state bit to be cleared.
    */        
     void ClearBit( TUint32 aBit );

    /**
    * Do cleanup operations & update internal state to indicate an 
    * error occured.
    * @param aError Error code which is thrown. KErrNone does not leave.
    */
    void SetBrokenStateL( const TInt aError );
    
    void Reset();
    
    void DoProcessMessageL( HBufC8*& aDRMMessage );
    void DoProcessRightsObjectL( const TDesC8& aRightsObject, 
                               RPointerArray<CDRMRights>& aRightsDetail );
    void DeletePermission();
    void InitDCFBufferL();
    
    void CreateCDCIDL( HBufC8*& aCID );
    TInt RetrieveFlRights();
    void ProcessDCFDataL( TPtrC8& aData );
    
    void FillDCFBufferL( TPtrC8& aData );
    void CompressDCFBuffer( const TInt aHowMuch );
    TBool FindDCFHeader( const TDesC8& aString,
                         TUint& aOffset,
                         TUint& aLength );

    HBufC8* EncryptDCFFieldLC( const TDesC8& aOldHeader );
    
    // Prohibit copy constructor if not deriving from CBase.
    CDRMMessageParser( const CDRMMessageParser& );
    // Prohibit assigment operator if not deriving from CBase.
    CDRMMessageParser& operator=( const CDRMMessageParser& );
    
public:     // Data
    TBool iDoPop;
    
protected:  // Data
    
private:    // Data
    // Given stream
    RWriteStream iOutputStream;
    
    COma1DcfCreator* iDcfCreator;
    
    // CD rights object.
    HBufC8* iRightsData;
    CDRMRights* iRightsObject;
    
    // Boundary obtained from the given data.
    HBufC8* iBoundary;
    
    // Content type from the given data.
    HBufC8* iContentType;
    
    // Temporary store for DCF data.
    HBufC8* iDCFBuffer;
    
    // Current internal state.
    TUint32 iState;
    
    // DCF header sizes.
    TUint32 iDCFHeaderSize[ 2 ];
    
    // Input cache & counter.
    TPtr8 iInputBuffer;
    TInt iUsedFromInput;
    
    // Temporary storage & counter
    TPtrC8 iInputData;

public:     // Friend classes
protected:  // Friend classes
private:    // Friend classes
};

#endif      // CDRMMESSAGEPARSER_H   

// End of File
