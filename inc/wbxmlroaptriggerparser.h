/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Parser for Oma Drm 2.1 WBXML RoapTrigger messages
*
*/



#ifndef WBXMLROAPTRIGGERPARSER_H
#define WBXMLROAPTRIGGERPARSER_H

//  INCLUDES
#include <xml/parser.h>
#include <xml/contenthandler.h>
#include <xml/xmlparsererrors.h>

namespace DRM
{

NONSHARABLE_CLASS( CWbxmlRoapTriggerParser ): public CBase, public Xml::MContentHandler
    {

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CWbxmlRoapTriggerParser* NewL();

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CWbxmlRoapTriggerParser* NewLC();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CWbxmlRoapTriggerParser();

        /**
        * Parses the WBXML RoapTrigger to XML
        *
        * @param aMessage WBXML RoapTrigger to be parsed
        *
        * @return parsed XML RoapTrigger in HBufC8
        */
        IMPORT_C HBufC8* ParseL( const TDesC8& aMessage );

    public: // Functions from base classes

        /**
        * From MContentHandler
        *
        * @param aDocParam Specifies the various parameters of the document.
        * @arg   aDocParam.iCharacterSetName The character encoding of the document.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnStartDocumentL( const Xml::RDocumentParameters& aDocParam,
                               TInt aErrorCode );

        /**
        * From MContentHandler
        *
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnEndDocumentL( TInt aErrorCode);

        /**
        * From MContentHandler
        *
        * @param aElement is a handle to the element's details.
        * @param aAttributes contains the attributes for the element.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnStartElementL( const Xml::RTagInfo& aElement,
                              const Xml::RAttributeArray& aAttributes,
                              TInt aErrorCode );

        /**
        * From MContentHandler
        *
        * @param aElement is a handle to the element's details.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnEndElementL( const Xml::RTagInfo& aElement,
                            TInt aErrorCode );

        /**
        * From MContentHandler
        *
        * @param aBytes is the raw content data for the element.
        *        The client is responsible for converting the data to the
        *        required character set if necessary.
        *        In some instances the content may be binary and must not be converted.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnContentL( const TDesC8& aBytes,
                         TInt aErrorCode );

        /**
        * From MContentHandler
        *
        * @param aPrefix is the Namespace prefix being declared.
        * @param aUri is the Namespace URI the prefix is mapped to.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnStartPrefixMappingL( const RString& aPrefix,
                                    const RString& aUri,
                                    TInt aErrorCode );

        /**
        * From MContentHandler
        *
        * @param aPrefix is the Namespace prefix that was mapped.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnEndPrefixMappingL( const RString& aPrefix,
                                  TInt aErrorCode );

        /**
        * From MContentHandler
        *
        * @param aBytes are the ignored bytes from the document being parsed.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnIgnorableWhiteSpaceL( const TDesC8& aBytes,
                                     TInt aErrorCode );

        /**
        * From MContentHandler
        *
        * @param aName is the name of the skipped entity.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnSkippedEntityL( const RString& aName,
                               TInt aErrorCode );

        /**
        * From MContentHandler
        *
        * @param aTarget is the processing instruction target.
        * @param aData is the processing instruction data. If empty none was supplied.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        void OnProcessingInstructionL( const TDesC8& aTarget,
                                       const TDesC8& aData,
                                       TInt aErrorCode );

        /**
        * From MContentHandler
        */
        void OnOutOfData();

        /**
        * From MContentHandler
        *
        * @param aError is the error code
        */
        void OnError( TInt aErrorCode );

        /**
        * From MContentHandler
        *
        * @param aUid the uid identifying the required interface.
        *
        * @return  0 if no interface matching the uid is found.
        *          Otherwise, the this pointer cast to that interface.
        */
        TAny* GetExtendedInterface( const TInt32 aUid );

    private: // Functions from base classes

        /**
        * C++ default constructor.
        */
        CWbxmlRoapTriggerParser();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    private:     // Data

        Xml::CParser* iParser;
        CBufFlat* iContent;

    };
}

#endif // WBXMLROAPTRIGGERPARSER_H

// End of File
