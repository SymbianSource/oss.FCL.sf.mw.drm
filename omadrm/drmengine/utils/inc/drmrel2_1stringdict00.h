/*
* Copyright (c) 2007 - 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  String Dictionary for OMA 2.1 Drm
*
*/


#ifndef C_DRMREL2_1STRINGDICT00_H
#define C_DRMREL2_1STRINGDICT00_H

#include <e32std.h>
#include <stringpool.h>
#include <xml/plugins/stringdictionary.h>


class CDrmRel2_1StringDict00 : public CBase, public Xml::MStringDictionary
/**
The DrmRel2_1StringDict00 class is a derived class defining the API of a single string dictionary. 

Derived classes must add all elements, attributes names, and attribute values that belong
to this string dictionary as static string tables to the StringPool. WBXML token mappings between 
strings and WBXML tokens should also be maintained.

@internalTechnology
@interim This is the first drop of this component.
*/
	{
public:
	static Xml::MStringDictionary* NewL( TAny* aStringPool );
	virtual ~CDrmRel2_1StringDict00();
	void ConstructL();

	void ElementL( TInt aToken, RString& aElement ) const;
	void AttributeL( TInt aToken, RString& aAttribute ) const;
	void AttributeValuePairL( TInt aToken, RString& aAttribute, RString& aValue ) const;
	void AttributeValueL( TInt aToken, RString& aValue ) const;

	TBool CompareThisDictionary( const RString& aDictionaryDescription ) const;

	TInt SwitchCodePage( TInt aCodePage );
	void PublicIdentifier( RString& aPubId );
	void NamespaceUri( RString& aUri );

	void Release();

public:

	// CodePage tokens.
	//
	// Note: There will be correlation between these token values and their
	// strings via the CDictionaryCodePage using the RStringPool.
    enum TTagToken
        {
		// CodePage 00
		// -//OMA/DRM 2.1//EN
        ERoapTrigger = 0x05,
        ERoapTriggerWithContent = 0x45,
        ERoapTriggerWithAttributes = 0x85,
        ERoapTriggerWithContentAndAttributes = 0xC5,
        ERegistrationRequest = 0x06,
        ERegistrationRequestWithContent = 0x46,
        ERegistrationRequestWithAttributes = 0x86,
        ERegistrationRequestWithContentAndAttributes = 0xC6,
        EROAcquisition = 0x07,
        EROAcquisitionWithContent = 0x47,
        EROAcquisitionWithAttributes = 0x87,
        EROAcquisitionWithContentAndAttributes = 0xC7,
        EJoinDomain = 0x08,
        EJoinDomainWithContent = 0x48,
        EJoinDomainWithAttributes = 0x88,
        EJoinDomainWithContentAndAttributes = 0xC8,
        ELeaveDomain = 0x09,
        ELeaveDomainWithContent = 0x49,
        ELeaveDomainWithAttributes = 0x89,
        ELeaveDomainWithContentAndAttributes = 0xC9,
        ESignature = 0x0A,
        ESignatureWithContent = 0x4A,
        ESignatureWithAttributes = 0x8A,
        ESignatureWithContentAndAttributes = 0xCA,
        EEncKey = 0x0B,
        EEncKeyWithContent = 0x4B,
        EEncKeyWithAttributes = 0x8B,
        EEncKeyWithContentAndAttributes = 0xCB,
        ERiId = 0x0C,
        ERiIdWithContent = 0x4C,
        ERiIdWithAttributes = 0x8C,
        ERiIdWithContentAndAttributes = 0xCC,
        ERiAlias = 0x0D,
        ERiAliasWithContent = 0x4D,
        ERiAliasWithAttributes = 0x8D,
        ERiAliasWithContentAndAttributes = 0xCD,
        ENonce = 0x0E,
        ENonceWithContent = 0x4E,
        ENonceWithAttributes = 0x8E,
        ENonceWithContentAndAttributes = 0xCE,
        ERoapUrl = 0x0F,
        ERoapUrlWithContent = 0x4F,
        ERoapUrlWithAttributes = 0x8F,
        ERoapUrlWithContentAndAttributes = 0xCF,
        EDomainId = 0x10,
        EDomainIdWithContent = 0x50,
        EDomainIdWithAttributes = 0x90,
        EDomainIdWithContentAndAttributes = 0xD0,
        EDomainAlias = 0x11,
        EDomainAliasWithContent = 0x51,
        EDomainAliasWithAttributes = 0x91,
        EDomainAliasWithContentAndAttributes = 0xD1,
        ERoapDomainId = 0x12,
        ERoapDomainIdWithContent = 0x52,
        ERoapDomainIdWithAttributes = 0x92,
        ERoapDomainIdWithContentAndAttributes = 0xD2,
        ERoId = 0x13,
        ERoIdWithContent = 0x53,
        ERoIdWithAttributes = 0x93,
        ERoIdWithContentAndAttributes = 0xD3,
        ERoAlias = 0x14,
        ERoAliasWithContent = 0x54,
        ERoAliasWithAttributes = 0x94,
        ERoAliasWithContentAndAttributes = 0xD4,
        EContentId = 0x15,
        EContentIdWithContent = 0x55,
        EContentIdWithAttributes = 0x95,
        EContentIdWithContentAndAttributes = 0xD5,
        ERoapX509SPKIHash = 0x16,
        ERoapX509SPKIHashWithContent = 0x56,
        ERoapX509SPKIHashWithAttributes = 0x96,
        ERoapX509SPKIHashWithContentAndAttributes = 0xD6,
        EKeyIdentifier = 0x17,
        EKeyIdentifierWithContent = 0x57,
        EKeyIdentifierWithAttributes = 0x97,
        EKeyIdentifierWithContentAndAttributes = 0xD7,
        EHash = 0x18,
        EHashWithContent = 0x58,
        EHashWithAttributes = 0x98,
        EHashWithContentAndAttributes = 0xD8,
        EDsSignedInfo = 0x19,
        EDsSignedInfoWithContent = 0x59,
        EDsSignedInfoWithAttributes = 0x99,
        EDsSignedInfoWithContentAndAttributes = 0xD9,
        EDsSignatureValue = 0x1A,
        EDsSignatureValueWithContent = 0x5A,
        EDsSignatureValueWithAttributes = 0x9A,
        EDsSignatureValueWithContentAndAttributes = 0xDA,
        EDsKeyInfo = 0x1B,
        EDsKeyInfoWithContent = 0x5B,
        EDsKeyInfoWithAttributes = 0x9B,
        EDsKeyInfoWithContentAndAttributes = 0xDB,
        EDsCanonicalisationMethod = 0x1C,
        EDsCanonicalisationMethodWithContent = 0x5C,
        EDsCanonicalisationMethodWithAttributes = 0x9C,
        EDsCanonicalisationMethodWithContentAndAttributes = 0xDC,
        EDsSignatureMethod = 0x1D,
        EDsSignatureMethodWithContent = 0x5D,
        EDsSignatureMethodWithAttributes = 0x9D,
        EDsSignatureMethodWithContentAndAttributes = 0xDD,
        EDsReference = 0x1E,
        EDsReferenceWithContent = 0x5E,
        EDsReferenceWithAttributes = 0x9E,
        EDsReferenceWithContentAndAttributes = 0xDE,
        EDsRetrievalMethod = 0x1F,
        EDsRetrievalMethodWithContent = 0x5F,
        EDsRetrievalMethodWithAttributes = 0x9F,
        EDsRetrievalMethodWithContentAndAttributes = 0xDF,
        EDsTransforms = 0x20,
        EDsTransformsWithContent = 0x60,
        EDsTransformsWithAttributes = 0xA0,
        EDsTransformsWithContentAndAttributes = 0xE0,
        EDsDigestMethod = 0x21,
        EDsDigestMethodWithContent = 0x61,
        EDsDigestMethodWithAttributes = 0xA1,
        EDsDigestMethodWithContentAndAttributes = 0xE1,
        EDsDigestValue = 0x22,
        EDsDigestValueWithContent = 0x62,
        EDsDigestValueWithAttributes = 0xA2,
        EDsDigestValueWithContentAndAttributes = 0xE2,
        EDsTransform = 0x23,
        EDsTransformWithContent = 0x63,
        EDsTransformWithAttributes = 0xA3,
        EDsTransformWithContentAndAttributes = 0xE3,
        EXencEncryptionMethod = 0x24,
        EXencEncryptionMethodWithContent = 0x64,
        EXencEncryptionMethodWithAttributes = 0xA4,
        EXencEncryptionMethodWithContentAndAttributes = 0xE4,
        EXencCipherData = 0x25,
        EXencCipherDataWithContent = 0x65,
        EXencCipherDataWithAttributes = 0xA5,
        EXencCipherDataWithContentAndAttributes = 0xE5,
        EXencCipherValue = 0x26,
        EXencCipherValueWithContent = 0x66,
        EXencCipherValueWithAttributes = 0xA6,
        EXencCipherValueWithContentAndAttributes = 0xE6,
        EExtentedTrigger = 0x27,
        EExtentedTriggerWithContent = 0x67,
        EExtentedTriggerWithAttributes = 0xA7,
        EExtentedTriggerWithContentAndAttributes = 0xE7,
        ETrgLeaveDomain = 0x28,
        ETrgLeaveDomainWithContent = 0x68,
        ETrgLeaveDomainWithAttributes = 0xA8,
        ETrgLeaveDomainWithContentAndAttributes = 0xE8,
        EDeviceId = 0x29,
        EDeviceIdWithContent = 0x69,
        EDeviceIdWithAttributes = 0xA9,
        EDeviceIdWithContentAndAttributes = 0xE9
        };


	enum TAttributeToken
        {
		// CodePage 00
		// -//OMA/DRM 2.1//EN
        EXsiType = 0x05,
        EXmlnsRoap = 0x06,
        EXmlnsXsi = 0x07,
        EXmlnsXenc = 0x08,
        EXmlnsDs = 0x09,
        EXmlnsOex = 0x0A,
        EXmlnsOdd = 0x0B,
        EXmlnsOmadd = 0x0C,
        EVersion = 0x0D,
        EProxy = 0x0E,
        EId = 0x0F, 
        EId2 = 0x10,
        ELCAlgorithm = 0x11,
        EHCAlgorithm = 0x12,
        EUri = 0x13,
        EType = 0x14
        };



	enum TAttributeValueToken
        {
		// CodePage 00
		// -//OMA/DRM 2.1//EN
        EXsiTypeValue = 0x05,
        EXmlnsRoapValue = 0x06,
        EXmlnsXsiValue = 0x07,
        EXmlnsXencValue = 0x08,
        EXmlnsDsValue = 0x09,
        EXmlnsOexValue = 0x0A,
        EXmlnsOddValue = 0x0B,
        EXmlnsOmaddValue = 0x0C,
        EVersionValue = 0x0D,
        EProxyValue = 0x0E,
        EIdValue = 0x0F, 
        EId2Value = 0x10,
        ELCAlgorithmValue = 0x11,
        EHCAlgorithmValue = 0x12,
        EUriValue = 0x13,
        ETypeValue = 0x14,
        EUrn = 0x85,
        EOdrlEx = 0x86,
        EOdrlDd = 0x87,
        EOmaDd = 0x88,
        EXmldSig = 0x89,
        EXmlEnc = 0x8A,
        EXmlSchema = 0x8B,
        ERoapX509SPKIHashValue = 0x8C,
        EXmldSigSha1 = 0x8D,
        EXmlExcC14 = 0x8E,
        EXmldSigHmacSha1 = 0x8F,
        EVersion1 = 0x90,
        EVersion2 = 0x91,
        EVersion21 = 0x92,
        EIdentificationRequest = 0x93,
        ERoUploadRequest = 0x94,
        EMeteringReport = 0x95,
        ELeaveDomainValue = 0x96
	    };



private:

	CDrmRel2_1StringDict00( RStringPool* aStringPool );

private:

    /**
    The StringPool for this string dictionary.
    We don't own this.
    */
	RStringPool				iStringPool;

	Xml::CDictionaryCodePage*	iCodepage00Table;

	TInt					iCodePage;

	};


const TInt iTagCodePage00[] = 
	{
    CDrmRel2_1StringDict00::ERoapTrigger,
    CDrmRel2_1StringDict00::ERoapTriggerWithContent,
    CDrmRel2_1StringDict00::ERoapTriggerWithAttributes,
    CDrmRel2_1StringDict00::ERoapTriggerWithContentAndAttributes,
    CDrmRel2_1StringDict00::ERegistrationRequest,
    CDrmRel2_1StringDict00::ERegistrationRequestWithContent,
    CDrmRel2_1StringDict00::ERegistrationRequestWithAttributes,
    CDrmRel2_1StringDict00::ERegistrationRequestWithContentAndAttributes,
    CDrmRel2_1StringDict00::EROAcquisition,
    CDrmRel2_1StringDict00::EROAcquisitionWithContent,
    CDrmRel2_1StringDict00::EROAcquisitionWithAttributes,
    CDrmRel2_1StringDict00::EROAcquisitionWithContentAndAttributes,
    CDrmRel2_1StringDict00::EJoinDomain,
    CDrmRel2_1StringDict00::EJoinDomainWithContent,
    CDrmRel2_1StringDict00::EJoinDomainWithAttributes,
    CDrmRel2_1StringDict00::EJoinDomainWithContentAndAttributes,
    CDrmRel2_1StringDict00::ELeaveDomain,
    CDrmRel2_1StringDict00::ELeaveDomainWithContent,
    CDrmRel2_1StringDict00::ELeaveDomainWithAttributes,
    CDrmRel2_1StringDict00::ELeaveDomainWithContentAndAttributes,
    CDrmRel2_1StringDict00::ESignature,
    CDrmRel2_1StringDict00::ESignatureWithContent,
    CDrmRel2_1StringDict00::ESignatureWithAttributes,
    CDrmRel2_1StringDict00::ESignatureWithContentAndAttributes,
    CDrmRel2_1StringDict00::EEncKey,
    CDrmRel2_1StringDict00::EEncKeyWithContent,
    CDrmRel2_1StringDict00::EEncKeyWithAttributes,
    CDrmRel2_1StringDict00::EEncKeyWithContentAndAttributes,
    CDrmRel2_1StringDict00::ERiId,
    CDrmRel2_1StringDict00::ERiIdWithContent,
    CDrmRel2_1StringDict00::ERiIdWithAttributes,
    CDrmRel2_1StringDict00::ERiIdWithContentAndAttributes,
    CDrmRel2_1StringDict00::ERiAlias,
    CDrmRel2_1StringDict00::ERiAliasWithContent,
    CDrmRel2_1StringDict00::ERiAliasWithAttributes,
    CDrmRel2_1StringDict00::ERiAliasWithContentAndAttributes,
    CDrmRel2_1StringDict00::ENonce,
    CDrmRel2_1StringDict00::ENonceWithContent,
    CDrmRel2_1StringDict00::ENonceWithAttributes,
    CDrmRel2_1StringDict00::ENonceWithContentAndAttributes,
    CDrmRel2_1StringDict00::ERoapUrl,
    CDrmRel2_1StringDict00::ERoapUrlWithContent,
    CDrmRel2_1StringDict00::ERoapUrlWithAttributes,
    CDrmRel2_1StringDict00::ERoapUrlWithContentAndAttributes,
    CDrmRel2_1StringDict00::EDomainId,
    CDrmRel2_1StringDict00::EDomainIdWithContent,
    CDrmRel2_1StringDict00::EDomainIdWithAttributes,
    CDrmRel2_1StringDict00::EDomainIdWithContentAndAttributes,
    CDrmRel2_1StringDict00::EDomainAlias,
    CDrmRel2_1StringDict00::EDomainAliasWithContent,
    CDrmRel2_1StringDict00::EDomainAliasWithAttributes,
    CDrmRel2_1StringDict00::EDomainAliasWithContentAndAttributes,
    CDrmRel2_1StringDict00::ERoapDomainId,
    CDrmRel2_1StringDict00::ERoapDomainIdWithContent,
    CDrmRel2_1StringDict00::ERoapDomainIdWithAttributes,
    CDrmRel2_1StringDict00::ERoapDomainIdWithContentAndAttributes,
    CDrmRel2_1StringDict00::ERoId,
    CDrmRel2_1StringDict00::ERoIdWithContent,
    CDrmRel2_1StringDict00::ERoIdWithAttributes,
    CDrmRel2_1StringDict00::ERoIdWithContentAndAttributes,
    CDrmRel2_1StringDict00::ERoAlias,
    CDrmRel2_1StringDict00::ERoAliasWithContent,
    CDrmRel2_1StringDict00::ERoAliasWithAttributes,
    CDrmRel2_1StringDict00::ERoAliasWithContentAndAttributes,
    CDrmRel2_1StringDict00::EContentId,
    CDrmRel2_1StringDict00::EContentIdWithContent,
    CDrmRel2_1StringDict00::EContentIdWithAttributes,
    CDrmRel2_1StringDict00::EContentIdWithContentAndAttributes,
    CDrmRel2_1StringDict00::ERoapX509SPKIHash,
    CDrmRel2_1StringDict00::ERoapX509SPKIHashWithContent,
    CDrmRel2_1StringDict00::ERoapX509SPKIHashWithAttributes,
    CDrmRel2_1StringDict00::ERoapX509SPKIHashWithContentAndAttributes,
    CDrmRel2_1StringDict00::EKeyIdentifier,
    CDrmRel2_1StringDict00::EKeyIdentifierWithContent,
    CDrmRel2_1StringDict00::EKeyIdentifierWithAttributes,
    CDrmRel2_1StringDict00::EKeyIdentifierWithContentAndAttributes,
    CDrmRel2_1StringDict00::EHash,
    CDrmRel2_1StringDict00::EHashWithContent,
    CDrmRel2_1StringDict00::EHashWithAttributes,
    CDrmRel2_1StringDict00::EHashWithContentAndAttributes,
    CDrmRel2_1StringDict00::EDsSignedInfo,
    CDrmRel2_1StringDict00::EDsSignedInfoWithContent,
    CDrmRel2_1StringDict00::EDsSignedInfoWithAttributes,
    CDrmRel2_1StringDict00::EDsSignedInfoWithContentAndAttributes,
    CDrmRel2_1StringDict00::EDsSignatureValue,
    CDrmRel2_1StringDict00::EDsSignatureValueWithContent,
    CDrmRel2_1StringDict00::EDsSignatureValueWithAttributes,
    CDrmRel2_1StringDict00::EDsSignatureValueWithContentAndAttributes,
    CDrmRel2_1StringDict00::EDsKeyInfo,
    CDrmRel2_1StringDict00::EDsKeyInfoWithContent,
    CDrmRel2_1StringDict00::EDsKeyInfoWithAttributes,
    CDrmRel2_1StringDict00::EDsKeyInfoWithContentAndAttributes,
    CDrmRel2_1StringDict00::EDsCanonicalisationMethod,
    CDrmRel2_1StringDict00::EDsCanonicalisationMethodWithContent,
    CDrmRel2_1StringDict00::EDsCanonicalisationMethodWithAttributes,
    CDrmRel2_1StringDict00::EDsCanonicalisationMethodWithContentAndAttributes,
    CDrmRel2_1StringDict00::EDsSignatureMethod,
    CDrmRel2_1StringDict00::EDsSignatureMethodWithContent,
    CDrmRel2_1StringDict00::EDsSignatureMethodWithAttributes,
    CDrmRel2_1StringDict00::EDsSignatureMethodWithContentAndAttributes,
    CDrmRel2_1StringDict00::EDsReference,
    CDrmRel2_1StringDict00::EDsReferenceWithContent,
    CDrmRel2_1StringDict00::EDsReferenceWithAttributes,
    CDrmRel2_1StringDict00::EDsReferenceWithContentAndAttributes,
    CDrmRel2_1StringDict00::EDsRetrievalMethod,
    CDrmRel2_1StringDict00::EDsRetrievalMethodWithContent,
    CDrmRel2_1StringDict00::EDsRetrievalMethodWithAttributes,
    CDrmRel2_1StringDict00::EDsRetrievalMethodWithContentAndAttributes,
    CDrmRel2_1StringDict00::EDsTransforms,
    CDrmRel2_1StringDict00::EDsTransformsWithContent,
    CDrmRel2_1StringDict00::EDsTransformsWithAttributes,
    CDrmRel2_1StringDict00::EDsTransformsWithContentAndAttributes,
    CDrmRel2_1StringDict00::EDsDigestMethod,
    CDrmRel2_1StringDict00::EDsDigestMethodWithContent,
    CDrmRel2_1StringDict00::EDsDigestMethodWithAttributes,
    CDrmRel2_1StringDict00::EDsDigestMethodWithContentAndAttributes,
    CDrmRel2_1StringDict00::EDsDigestValue,
    CDrmRel2_1StringDict00::EDsDigestValueWithContent,
    CDrmRel2_1StringDict00::EDsDigestValueWithAttributes,
    CDrmRel2_1StringDict00::EDsDigestValueWithContentAndAttributes,
    CDrmRel2_1StringDict00::EDsTransform,
    CDrmRel2_1StringDict00::EDsTransformWithContent,
    CDrmRel2_1StringDict00::EDsTransformWithAttributes,
    CDrmRel2_1StringDict00::EDsTransformWithContentAndAttributes,
    CDrmRel2_1StringDict00::EXencEncryptionMethod,
    CDrmRel2_1StringDict00::EXencEncryptionMethodWithContent,
    CDrmRel2_1StringDict00::EXencEncryptionMethodWithAttributes,
    CDrmRel2_1StringDict00::EXencEncryptionMethodWithContentAndAttributes,
    CDrmRel2_1StringDict00::EXencCipherData,
    CDrmRel2_1StringDict00::EXencCipherDataWithContent,
    CDrmRel2_1StringDict00::EXencCipherDataWithAttributes,
    CDrmRel2_1StringDict00::EXencCipherDataWithContentAndAttributes,
    CDrmRel2_1StringDict00::EXencCipherValue,
    CDrmRel2_1StringDict00::EXencCipherValueWithContent,
    CDrmRel2_1StringDict00::EXencCipherValueWithAttributes,
    CDrmRel2_1StringDict00::EXencCipherValueWithContentAndAttributes,
    CDrmRel2_1StringDict00::EExtentedTrigger,
    CDrmRel2_1StringDict00::EExtentedTriggerWithContent,
    CDrmRel2_1StringDict00::EExtentedTriggerWithAttributes,
    CDrmRel2_1StringDict00::EExtentedTriggerWithContentAndAttributes,
    CDrmRel2_1StringDict00::ETrgLeaveDomain,
    CDrmRel2_1StringDict00::ETrgLeaveDomainWithContent,
    CDrmRel2_1StringDict00::ETrgLeaveDomainWithAttributes,
    CDrmRel2_1StringDict00::ETrgLeaveDomainWithContentAndAttributes,
    CDrmRel2_1StringDict00::EDeviceId,
    CDrmRel2_1StringDict00::EDeviceIdWithContent,
    CDrmRel2_1StringDict00::EDeviceIdWithAttributes,
    CDrmRel2_1StringDict00::EDeviceIdWithContentAndAttributes,
	0 // ok to end with zero as this is used by a global token
    }; 


const TInt iAttributeCodePage00[] = 
	{
    CDrmRel2_1StringDict00::EXsiType,
    CDrmRel2_1StringDict00::EXmlnsRoap,
    CDrmRel2_1StringDict00::EXmlnsXsi,
    CDrmRel2_1StringDict00::EXmlnsXenc,
    CDrmRel2_1StringDict00::EXmlnsDs,
    CDrmRel2_1StringDict00::EXmlnsOex,
    CDrmRel2_1StringDict00::EXmlnsOdd,
    CDrmRel2_1StringDict00::EXmlnsOmadd,
    CDrmRel2_1StringDict00::EVersion,
    CDrmRel2_1StringDict00::EProxy,
    CDrmRel2_1StringDict00::EId,
    CDrmRel2_1StringDict00::EId2,
    CDrmRel2_1StringDict00::ELCAlgorithm,
    CDrmRel2_1StringDict00::EHCAlgorithm,
    CDrmRel2_1StringDict00::EUri,
    CDrmRel2_1StringDict00::EType,
	0  // ok to end with zero as this is used by a global token
    };



const TInt iAttributeValueCodePage00[] = 
	{
	CDrmRel2_1StringDict00::EXsiTypeValue,
    CDrmRel2_1StringDict00::EXmlnsRoapValue,
    CDrmRel2_1StringDict00::EXmlnsXsiValue,
    CDrmRel2_1StringDict00::EXmlnsXencValue,
    CDrmRel2_1StringDict00::EXmlnsDsValue,
    CDrmRel2_1StringDict00::EXmlnsOexValue,
    CDrmRel2_1StringDict00::EXmlnsOddValue,
    CDrmRel2_1StringDict00::EXmlnsOmaddValue,
    CDrmRel2_1StringDict00::EVersionValue,
    CDrmRel2_1StringDict00::EProxyValue,
    CDrmRel2_1StringDict00::EIdValue,
    CDrmRel2_1StringDict00::EId2Value,
    CDrmRel2_1StringDict00::EHCAlgorithmValue,
    CDrmRel2_1StringDict00::ELCAlgorithmValue,
    CDrmRel2_1StringDict00::EUriValue,
    CDrmRel2_1StringDict00::ETypeValue,
    CDrmRel2_1StringDict00::EUrn,
    CDrmRel2_1StringDict00::EOdrlEx,
    CDrmRel2_1StringDict00::EOdrlDd,
    CDrmRel2_1StringDict00::EOmaDd,
    CDrmRel2_1StringDict00::EXmldSig,
    CDrmRel2_1StringDict00::EXmlEnc,
    CDrmRel2_1StringDict00::EXmlSchema,
    CDrmRel2_1StringDict00::ERoapX509SPKIHashValue,
    CDrmRel2_1StringDict00::EXmldSigSha1,
    CDrmRel2_1StringDict00::EXmlExcC14,
    CDrmRel2_1StringDict00::EXmldSigHmacSha1,
    CDrmRel2_1StringDict00::EVersion1,
    CDrmRel2_1StringDict00::EVersion2,
    CDrmRel2_1StringDict00::EVersion21,
    CDrmRel2_1StringDict00::EIdentificationRequest,
    CDrmRel2_1StringDict00::ERoUploadRequest,
    CDrmRel2_1StringDict00::EMeteringReport,
    CDrmRel2_1StringDict00::ELeaveDomainValue,
	0 // ok to end with zero as this is used by a global token
    }; 


#endif // C_DRMREL2_1STRINGDICT00_H
