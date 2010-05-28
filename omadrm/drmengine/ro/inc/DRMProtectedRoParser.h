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



#ifndef DRMPROTECTEDROPARSER_H
#define DRMPROTECTEDROPARSER_H

//  INCLUDES
#include <e32std.h>
#include <xml/parser.h>
#include <xml/contenthandler.h>
#include <xml/xmlparsererrors.h>
#include <xml/attribute.h>
#include "DRMRightsClient.h"

using namespace Xml;

// FORWARD DECLARATIONS
class CDRMRights;

// CLASS DECLARATION

/**
*  Parser for protected rights objects
*
*  @lib DrmParsers.dll
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS(CDrmProtectedRoParser): public CBase, public MContentHandler
    {
    public:

        class CParsedProtectedRo;

        enum TParserType
            {
            EXmlParser,
            EWbxmlParser
            };

        static const TInt KMaxElementCount = 40;
        static const TInt KMaxElementNesting = 16;

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CDrmProtectedRoParser* NewL();

        /**
        * Destructor.
        */
        IMPORT_C ~CDrmProtectedRoParser();

    public: // New functions

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        void ParseL(
            const TDesC8& aProtectedRo,
            CParsedProtectedRo*& aResultRights);

        IMPORT_C void ParseAndStoreL(
            const TDesC8& aProtectedRo,
            RPointerArray<CDRMRights>& aResultRights);

        IMPORT_C HBufC8* GetRiUrlL(
            const TDesC8& aProtectedRo);

        IMPORT_C HBufC8* GetDomainIdL(
            const TDesC8& aProtectedRo);

        IMPORT_C HBufC8* GetRiIdL(
            const TDesC8& aProtectedRo);

    public: // Functions from base classes

        /**
        * From ?base_class ?member_description.
        * @param aDocParam Specifies the various parameters of the document.
        * @arg   aDocParam.iCharacterSetName The character encoding of the document.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnStartDocumentL(
            const RDocumentParameters& aDocParam,
            TInt aErrorCode);

        /**
        * From ?base_class ?member_description.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnEndDocumentL(
            TInt aErrorCode);

        /**
        * From ?base_class ?member_description.
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
        * From ?base_class ?member_description.
        * @param aElement is a handle to the element's details.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnEndElementL(
            const RTagInfo& aElement,
            TInt aErrorCode);

        /**
        * From ?base_class ?member_description.
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
        * From ?base_class ?member_description.
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
        * From ?base_class ?member_description.
        * @param aPrefix is the Namespace prefix that was mapped.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnEndPrefixMappingL(
            const RString& aPrefix,
            TInt aErrorCode);

        /**
        * From ?base_class ?member_description.
        * @param aBytes are the ignored bytes from the document being parsed.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnIgnorableWhiteSpaceL(
            const TDesC8& aBytes,
            TInt aErrorCode);

        /**
        * From ?base_class ?member_description.
        * @param aName is the name of the skipped entity.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnSkippedEntityL(
            const RString& aName,
            TInt aErrorCode);

        /**
        * From ?base_class ?member_description.
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
        * From ?base_class ?member_description.
        */
        void OnOutOfData();


        /**
        * From ?base_class ?member_description.
        * @param aError is the error code
        */
        void OnError(
            TInt aErrorCode);

        /**
        * From ?base_class ?member_description.
        * @return               0 if no interface matching the uid is found.
                Otherwise, the this pointer cast to that interface.
        * @param aUid the uid identifying the required interface.
        */
        TAny* GetExtendedInterface(
            const TInt32 aUid);

    protected:  // New functions

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        TInt MatchStackState(void);

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        HBufC8* GetAttributeValueL(
            const RAttributeArray& aAttrList,
            const TDesC8& aAttrName);

        /**
        * Find the next start and end position of an XML element
        * in the given data
        * @since Series 60 3.0
        * @param aElement Element to look for
        * @param aData Input data
        * @param aStart return parameter for the beginning of the element
        * @param aEnd return parameter for the end of the element
        * @return EFalse if no element could be found, ETrue otherwise
        */
        TBool GetElementStartEnd(
            const TDesC8& aElement,
            const TDesC8& aData,
            TInt& aStart,
            TInt& aEnd);

        /**
        * Handle a parsed domain RO which could not be added to the rights DB
        * because the device is not part of the domain.
        * @since Series 60 3.0
        * @param aDomainRoData domain RO as XML data
        * @param aDomainRo parsed domain RO
        */
        void HandleUnregisteredDomainRoL(
            const TDesC8& aContentId,
            const TDesC8& aDomainRoData,
            CParsedProtectedRo* aDomainRo);

        /**
        * Adds ROAP namespace URI to aProtectedRo if not already present
        * @since Series 60 3.0
        * @param aProtectedRo protected RO element
        * @return protected RO element, if namespace was not added NULL
        */    
        HBufC8* AddRoapNamespaceL( TDesC8& aProtectedRo ) const;

    private:

        /**
        * C++ default constructor.
        */
        CDrmProtectedRoParser(void);

        void ConstructL(void);

    private:  // Data
        // The XML parser
        CParser* iParser;

        // Result of parsing the protected RO element
        CParsedProtectedRo* iRights;

        // Expected element names
        RString iElements[KMaxElementCount];

        // Parsing stack
        TInt iElementStack[KMaxElementNesting];

        // Depth of the parsing stack
        TInt iElementStackDepth;

        // Content between elements
        HBufC8* iContent;

    };

#endif      // DRMPROTECTEDROPARSER_H

// End of File
