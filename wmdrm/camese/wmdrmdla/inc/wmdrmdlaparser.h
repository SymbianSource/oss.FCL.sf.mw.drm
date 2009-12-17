/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DRM Header and License Response Parser
*
*/


#ifndef C_WMDRMDLAPARSER_H
#define C_WMDRMDLAPARSER_H

// INCLUDES
#include <xml/contenthandler.h>
#include <xml/parser.h>

/**
*  CWmDrmDlaParser
*  DRM Header and License Response Parser for the WMDRM DLA
*
*  @lib wmdrmdla.dll
*  @since 9.1
*/
NONSHARABLE_CLASS( CWmDrmDlaParser ) 
    : public CBase, public Xml::MContentHandler
    {
    public:
        /**
         * Two-phased constructor.
         * @return address of an instance of this class
         */
        static CWmDrmDlaParser* NewL();

        /**
         * Destructor.
         */
        virtual ~CWmDrmDlaParser();

        /**
         * Extract the TID and content URL from the license response
         * @param aLicenseResponse The license response to parse
         * @param aTID The TID that was extracted and returned. 
         *             If no TID found the parameter is set to NULL
         * @param aContentUrl The content URL that was extracted and returned. 
         *                    If no content URL found the parameter is set to NULL
         */
        TInt ProcessLicenseResponse( const TDesC8& aLicenseResponse, 
                                     HBufC8*& aTID, HBufC8*& aContentURL );
        /**
         * Extract the server Url from the DRM Heasder
         * @param aDrmHeader The DRM Header to parse
         * @param aServerUrl The server URL that was extracted and returned. 
         *                   If no server URL found the parameter is set to NULL
         */
        TInt GetLicenseServerURLFromDRMHeader( const TDesC8& aDrmHeader, 
                                               HBufC8*& aServerURL );

    private:  // From Xml::MContentHandler
        virtual void OnStartDocumentL( const Xml::RDocumentParameters& aDocParam, 
                                       TInt aErrorCode );
        virtual void OnEndDocumentL( TInt aErrorCode );
        virtual void OnStartElementL( const Xml::RTagInfo& aElement, 
                                      const Xml::RAttributeArray& aAttributes,
                                      TInt aErrorCode );
        virtual void OnEndElementL( const Xml::RTagInfo& aElement, TInt aErrorCode);
        virtual void OnContentL( const TDesC8& aBytes, TInt aErrorCode);
        virtual void OnStartPrefixMappingL( const RString& aPrefix, 
                                            const RString& aUri, TInt aErrorCode );
        virtual void OnEndPrefixMappingL( const RString& aPrefix, TInt aErrorCode);
        virtual void OnIgnorableWhiteSpaceL( const TDesC8& aBytes, TInt aErrorCode);
        virtual void OnSkippedEntityL( const RString& aName, TInt aErrorCode );
        virtual void OnProcessingInstructionL( const TDesC8& aTarget, 
                                               const TDesC8& aData, TInt aErrorCode );
        virtual void OnError( TInt aErrorCode );
        virtual TAny* GetExtendedInterface( const TInt32 aUid );

    private:
        /**
         * Constructor
         */
        CWmDrmDlaParser();
        /**
         * Symbian OS 2nd phase constructor.
         */
        void ConstructL();

    private:
        // This member is used when parsing a DRM Header and is not owned by this class 
        // and should NOT be deleted
        // If this memebers is filled when parsing a license response it WILL be deleted
        HBufC8**      iServerUrl;

        // These members are used when parsing a license response and are owned by this 
        // class and should NOT be deleted
        // If these memebers are filled when parsing a DRM Header they WILL be deleted
        HBufC8**      iContentUrl;
        HBufC8**      iTID;

        // These members are owned by this class and should be deleted
        Xml::CParser* iParser;
        HBufC8*       iBuffer;
        
        TInt          iErrorCode;
    };

#endif // C_WMDRMDLAPARSER_H

// End of File
