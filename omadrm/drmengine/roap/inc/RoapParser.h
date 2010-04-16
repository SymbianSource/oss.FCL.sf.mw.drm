/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ROAP parser base calss
*
*/



#ifndef ROAPPARSER_H
#define ROAPPARSER_H

//  INCLUDES
#include <xml/parser.h>
#include <xml/contenthandler.h>
#include <xml/xmlparsererrors.h>
#include "RoapMessage.h"

using namespace Xml;

namespace Roap
{

// CONSTANTS

// MACROS

// DATA TYPES
enum TRoapElementEnum
    {
    ECipherData,
    ECipherValue,
    ECertificate,
    ECertificateChain,
    EContentID,
    EDcfHash,
    EDeviceDetails,
    EDeviceID,
    EDigestValue,
    EDomainAlias,
    EDomainID,
    EDomainInfo,
    EDomainKey,
    EDomainName,
    EEncKey,
    EEncryptionMethod,
    EExtension,
    EExtensions,
    EHash,
    EId,
    EIdentifier,
    EJoinDomain,
    EJoinDomainResponse,
    EKeyIdentifier,
    ELeaveDomain,
    ELeaveDomainResponse,
    EMac,
    EManufacturer,
    EModel,
    ENonce,
    ENotAfter,
    EOcspResponse,
    EProtectedRO,
    ERegistrationRequest,
    ERegistrationResponse,
    ERiAlias,
    ERights,
    ERiHello,
    ERiID,
    ERiNonce,
    ERiURL,
    ERo,
    ERoAcquisition,
    ERoapTrigger,
    ERoapURL,
    ERoID,
    ERoInfo,
    ERoResponse,
    ESelectedAlgorithm,
    ESelectedVersion,
    EServerInfo,
    ESignature,
    ESignatureValue,
    ESupportedAlgorithm,
    ETime,
    ETimeStamp,
    ETrustedAuthorities,
    EVersion,
    EX509SPKIHash,
#ifdef RD_DRM_METERING
    EExtendedTriggerElement,
    EMeteringReportRequest,
    EMeteringReportResponse,
    EPrUrl,
#endif
    ELast
    };

enum TParserStackState
    {
    ERiHelloState,
    ESelectedVersionState,
    ERiIdState,
    ERiNonceState,
    EDeviceDetailsState,
    ECertificateState,
    EOcspResponseState,
    ERiUrlState,
    ESignatureState,
    ERegistrationResponseState,
    EPeerKeyIdentifierRIHelloState,
    ETransactionIdState,
    ETransactionIdContentIdState,
    EWhiteListState,
    EExtensionState,
    ERoResponseState,
    EDeviceIdState,
    ENonceState,
    ERiIdRoResponseState,
    ESignatureRoResponseState,
    EEncKeyState,
    ERiIdJoinDomainResponseState,
    EMacJoinDomainResponseState,
    EJoinDomainResponseState,
    ELeaveDomainResponseState,
    EDomainIdState,
    ENotAfterState,
    EDomainKeyState,
    EDomainKeyEncryptionMethodState,
    ERoapTriggerState,
    ERoapUrlState,
    ERegistrationRequestState,
    ERoAcquisitionState,
    EJoinDomainState,
    ELeaveDomainState,
    ERoIdState,
    EContentIdState,
    ESignatureValueState,
    EDigestValueState,
    EServerInfoState,
    ESupportedAlgorithmState,
    ESelectedAlgorithmState,
    ETrustedAuthoritiesState,
    EDomainKeyEncKeyState,
    EDomainAliasState,
    ERiAliasState,
#ifdef RD_DRM_METERING
    EExtendedTriggerElementState,
    EMeteringReportRequestState,
    EMeteringReportResponseState,
    EDeviceNonceState,
    EMeteringReportNonceState,
    EPostResponseUrlState,
#endif
    EUnknownState = -1,
    };

// FORWARD DECLARATIONS

class CDeviceHello;
class CJoinDomainReq;
class CJoinDomainResp;
class CLeaveDomainReq;
class CLeaveDomainResp;
class CRIHello;
class CRegistrationReq;
class CRegistrationResp;
class CRightsReq;
class CRightsResp;
class CRoapTrigger;
class MRespParser;
class CMeteringResp;

// CLASS DECLARATION

/**
*  ROAP parser base class
*
*  @lib roaphandler.dll
*  @since S60 v3.0
*/
class CRoapParser : public CBase, public MContentHandler
    {
    public: // Constants

        static const TInt KMaxElementCount = ELast;
        static const TInt KMaxElementNesting = 32;

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CRoapParser* NewL();

        /**
        * Destructor.
        */
        virtual ~CRoapParser();

    public: // New functions

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

        CMeteringResp* ParseMeteringRespL(
            const TDesC8& aMessage );
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        CJoinDomainResp* ParseJoinDomainRespL(
            const TDesC8& aMessage,
            RPointerArray<HBufC8>& aDomainKeyElements);

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        CLeaveDomainResp* ParseLeaveDomainRespL(
            const TDesC8& aMessage);

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        CRegistrationResp* ParseRegistrationRespL(
            const TDesC8& aMessage);

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        CRightsResp* ParseRightsRespL(
            const TDesC8& aMessage);

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        CRIHello* ParseRIHelloL(
            const TDesC8& aMessage);

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        CRoapTrigger* ParseRoapTriggerL(
            const TDesC8& aMessage);

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
        * @return
                0 if no interface matching the uid is found.
                Otherwise, the this pointer cast to that interface.
        * @param aUid the uid identifying the required interface.
        */
        TAny* GetExtendedInterface(
            const TInt32 aUid);

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
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        TRoapStatus ConvertRoapStatus(
            const TDesC8& aStatus);

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        TPtrC8 ExtractElement(
            const TDesC8& aMessage,
            const TDesC8& aElement,
            TInt& aOffset ) const;

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        TInt ExtractDomainKeyElements(
            const TDesC8& aMessage,
            RPointerArray<HBufC8>& aDomainKeyElements) const;


    protected:  // New functions

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        TParserStackState MatchStackState(void);

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        void ParseL(
            const TDesC8& aMessage);

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

        /**
        * C++ default constructor.
        */
        CRoapParser();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    private:

        // Prohibit copy constructor if not deriving from CBase.
        // CRoapParser( const CRoapParser& );
        // Prohibit assigment operator if not deriving from CBase.
        // CRoapParser& operator=( const CRoapParser& );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
        CParser* iParser;
        RString iRoapElements[KMaxElementCount];
        TInt iElementStack[KMaxElementNesting];
        TInt iElementStackDepth;
        HBufC8* iContent;
        MRespParser* iResponseParser;

    protected:  // Data
        // ?one_line_short_description_of_data

    private:    // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };
}

#endif      // ROAPPARSER_H

// End of File
