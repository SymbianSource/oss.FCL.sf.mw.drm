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


#ifndef __OCSP_ROAP_RESPONSEDECODER_H__
#define __OCSP_ROAP_RESPONSEDECODER_H__

#include <e32base.h>

class COCSPResponse;
class TASN1DecGeneric;

/**
 * Handles decoding an OCSP response and creating a COCSPResponse object.
 */

NONSHARABLE_CLASS(COCSPResponseDecoder) : public CBase
	{
public:

	/**
	 * Create a new decoder object and decoder an ocsp response.
	 * Can leave with one of the OCSP::TStatus codes.
	 */
	
	static COCSPResponseDecoder* NewL(const TDesC8& aEncoding);

	~COCSPResponseDecoder();

	COCSPResponse* TakeResponse(); // Will return null if response was invalid

private:

	COCSPResponseDecoder();
	void ConstructL(const TDesC8& aEncoding);

	// Decoding methods
	void DecodeOCSPResponseL(const TDesC8& aEncoding);
	void DecodeResponseBytesL(const TDesC8& aEncoding);
	void DecodeBasicOCSPResponseL(const TDesC8& aEncoding);
	void DecodeResponseDataL(const TDesC8& aEncoding);
	void DecodeCertificatesL(const TDesC8& aEncoding);
	void DecodeResponseExtensionsL(const TDesC8& aEncoding);
	void DecodeResponseExtensionL(const TDesC8& aEncoding);
	void DecodeResponsesL(const TDesC8& aEncoding);
	void DecodeSingleResponseL(const TDesC8& aEncoding);

	CArrayPtr<TASN1DecGeneric>* DecodeSequenceLC(const TDesC8& aEncoding);
	CArrayPtr<TASN1DecGeneric>* DecodeSequenceLC(const TDesC8& aEncoding,
		const TInt aMinTerms, const TInt aMaxTerms);

	COCSPResponse* iResponse;
	};

#endif // __OCSP_ROAP_RESPONSEDECODER_H__
