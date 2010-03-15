/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of the OMA DRM rigths parser
*
*/


#ifndef DRMRIGHTSPARSER_H
#define DRMRIGHTSPARSER_H

//  INCLUDES
#include <xml/parser.h>
#include <xml/contenthandler.h>
#include <xml/xmlparsererrors.h>

using namespace Xml;

// FORWARD DECLARATIONS
class CDRMRights;

// CLASS DECLARATION

/**
*  Parser for OMA DRM rights objects (verion 1.0 and 2.0)
*
*  @lib DrmParsers.dll
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS(CDrmRightsParser): public CBase, public MContentHandler
    {
    public:

        class CParsedRightsObject;
        class CParsedAsset;
        class CParsedPermission;

        enum TParserType
            {
            EXmlParser,
            EWbxmlParser
            };

        enum TSoftwareSchemaType
            {
            ESymbianSid,
            ESymbianVid
            };

        static const TInt KMaxElementCount = 40;
        static const TInt KMaxElementNesting = 16;

    public:  // Constructors and destructor

        /**
        * Create a parser object
        * @since Series 60 3.0
        * @param Type of the parser (XML or WBXML)
        * @return Allocated parser
        */
        IMPORT_C static CDrmRightsParser* NewL(
            TParserType aType = EXmlParser);

        /**
        * Destructor.
        */
        IMPORT_C ~CDrmRightsParser();

    public: // New functions

        /**
        * Parse one rights object
        * @since Series 60 3.0
        * @param aRightsObject XML or WBXML representation of the rights
        * @param aResultRigths Parsed rights
        */
        IMPORT_C void ParseL(
            const TDesC8& aRightsObject,
            RPointerArray<CDRMRights>& aResultRights);

        /**
        * Parse one rights object and store it in the rights database
        * @since Series 60 3.0
        * @param aRightsObject XML or WBXML representation of the rights
        * @param aResultRigths Parsed rights
        */
        IMPORT_C void ParseAndStoreL(
            const TDesC8& aRightsObject,
            RPointerArray<CDRMRights>& aResultRights);

    protected: // Functions from base classes

        /**
        * From MContentHandler
        * @param aDocParam Specifies the various parameters of the document.
        * @arg   aDocParam.iCharacterSetName The character encoding of the document.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnStartDocumentL(
            const RDocumentParameters& aDocParam,
            TInt aErrorCode);

        /**
        * From MContentHandler
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnEndDocumentL(
            TInt aErrorCode);

        /**
        * From MContentHandler
        * @param aElement is a handle to the element's details.
        * @param aAttributes contains the attributes for the element.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnStartElementL(
            const RTagInfo& aElement,
            const RAttributeArray& aAttributes,
            TInt aErrorCode);

        /**
        * From MContentHandler
        * @param aElement is a handle to the element's details.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnEndElementL(
            const RTagInfo& aElement,
            TInt aErrorCode);

        /**
        * From MContentHandler
        * @param aBytes is the raw content data for the element.
        *        The client is responsible for converting the data to the
        *        required character set if necessary.
        *        In some instances the content may be binary and must not be converted.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnContentL(
            const TDesC8& aBytes,
            TInt aErrorCode);

        /**
        * From MContentHandler
        * @param aPrefix is the Namespace prefix being declared.
        * @param aUri is the Namespace URI the prefix is mapped to.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnStartPrefixMappingL(
            const RString& aPrefix,
            const RString& aUri,
            TInt aErrorCode);

        /**
        * From MContentHandler
        * @param aPrefix is the Namespace prefix that was mapped.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnEndPrefixMappingL(
            const RString& aPrefix,
            TInt aErrorCode);

        /**
        * From MContentHandler
        * @param aBytes are the ignored bytes from the document being parsed.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnIgnorableWhiteSpaceL(
            const TDesC8& aBytes,
            TInt aErrorCode);

        /**
        * From MContentHandler
        * @param aName is the name of the skipped entity.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnSkippedEntityL(
            const RString& aName,
            TInt aErrorCode);

        /**
        * From MContentHandler
        * @param aTarget is the processing instruction target.
        * @param aData is the processing instruction data. If empty none was supplied.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnProcessingInstructionL(
            const TDesC8& aTarget,
            const TDesC8& aData,
            TInt aErrorCode);

        /**
        * From MContentHandler
        */
        void OnOutOfData();


        /**
        * From MContentHandler
        * @param aError is the error code
        */
        void OnError(
            TInt aErrorCode);


        /**
        * From MContentHandler
        * @return 0 if no interface matching the uid is found.
        *         Otherwise, the this pointer cast to that interface.
        * @param aUid the uid identifying the required interface.
        */
        TAny* GetExtendedInterface(
            const TInt32 aUid);

    protected:  // New functions

        /**
        * Match the state of the current stack to the predefined states
        * @since Series 60 3.0
        * @return KErrNotFound if no state is matched, otherwise the number
        *         of the state
        */
        TInt MatchStackState(void);

        /**
        * Transform the result of the parsing into an API level rights object
        * @since Series 60 3.0
        * @param aResult Out parameter for the resul
        */
        void TransformRightsObjectL(
            RPointerArray<CDRMRights>& aResult);

        /**
        * Return the value of an attribute as a buffer
        * @since Series 60 3.0
        * @param aAttrList Atrribute list
        * @param aAttrName Name of the attribute
        * @return value of the attribute or NULL
        */
        HBufC8* GetAttributeValueL(
            const RAttributeArray& aAttrList,
            const TDesC8& aAttrName);

    private:

        /**
        * C++ default constructor.
        */
        CDrmRightsParser();

        void ConstructL(
            TParserType aType);


        /**
        * Parse the time string and return the TTime
        */
        TTime ParseRelTimeL(TDesC8& aRelTimeString);

        /**
        * Parse the interval string and return the interval
        */
        TTimeIntervalSeconds ParseRelInterval(TDesC8& aRelTimeString);


        /**
        * Validate time string variables
        */
        TBool ValidTimeValues( TInt aYear, TMonth aMonth, TInt aDay, TInt aHour,
                               TInt aMinute, TInt aSecond, TInt aMicrosecond );

        /**
        * Validate day
        */
        TBool ValidateDay( TInt aYear, TMonth aMonth, TInt aDay );



    protected:  // Data
        // XML Parser
        CParser* iParser;

        // Parsed rigths object
        CParsedRightsObject* iRights;

        // Predefined XML element names
        RString iElements[KMaxElementCount];

        // Current parsing stack
        TInt iElementStack[KMaxElementNesting];

        // Depth of the current parsing stack
        TInt iElementStackDepth;

        // Content between elements
        HBufC8* iContent;

        // Determines if the input is XML or WBXML
        TParserType iParserType;

        // Determines what kind of software constraint the RO contains
        TSoftwareSchemaType iSoftwareSchemeType;

        // In increments of +- 15 minutes
        TInt iTimeZone;

        // Tagname of unkown tag
        HBufC8* iUnknownTag;
    };

#endif      // DRMRIGHTSPARSER_H

// End of File
