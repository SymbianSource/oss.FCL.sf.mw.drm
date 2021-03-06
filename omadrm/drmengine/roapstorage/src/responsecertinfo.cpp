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


#include "certid.h"
#include "RoapOcsp.h"
#include "RoapDef.h"

#include <asn1dec.h>

const TUint KGoodTag = 0;
const TUint KRevokedTag = 1;
const TUint KUnknownTag = 2;

const TUint KNextUpdateTag = 0;
const TUint KSingleExtensionsTag = 1;

COCSPResponseCertInfo* COCSPResponseCertInfo::NewLC(CArrayPtr<TASN1DecGeneric>& items)
	{
	COCSPResponseCertInfo* self = new (ELeave) COCSPResponseCertInfo;
	CleanupStack::PushL(self);
	self->ConstructL(items);
	return self;
	}


COCSPResponseCertInfo::~COCSPResponseCertInfo()
	{
	delete iNextUpdate;
	delete iRevocationTime;
	delete iCertID;
	}


OCSP::TResult COCSPResponseCertInfo::Status() const
	{
	return iStatus;
	}


TTime COCSPResponseCertInfo::ThisUpdate() const
	{
	return iThisUpdate;
	}


const TTime* COCSPResponseCertInfo::NextUpdate() const
	{
	return iNextUpdate;
	}


const TTime* COCSPResponseCertInfo::RevocationTime() const
	{
	return iRevocationTime;
	}


COCSPCertID* COCSPResponseCertInfo::CertID() const
	{
	return iCertID;
	}


void COCSPResponseCertInfo::ConstructL(CArrayPtr<TASN1DecGeneric>& items)
	{
	// The CertID
	iCertID = COCSPCertID::NewL(items.At(0)->Encoding());
	
	// The cert status - implicitly tagged
	TASN1DecGeneric& statusDec = *items.At(1);
	switch(statusDec.Tag())
		{
		case KGoodTag:
			iStatus = OCSP::EGood;
			break;
		case KRevokedTag:
			{
			iStatus = OCSP::ERevoked;

			// Get revocation time
			TASN1DecSequence seqDec;
			CArrayPtr<TASN1DecGeneric>* revokedInfo = seqDec.DecodeDERLC(statusDec, 1, 2);
			if (revokedInfo)
				{
				TASN1DecGeneric& revocationTimeDec = *revokedInfo->At(0);
				if (revocationTimeDec.Tag() != EASN1GeneralizedTime)
					{
					User::Leave(OCSP::EMalformedResponse);
					}

				TASN1DecGeneralizedTime decGT;
				iRevocationTime = new (ELeave) TTime(decGT.DecodeDERL(revocationTimeDec));

				CleanupStack::PopAndDestroy();  // revokedInfo
				}
			break;
			}
		case KUnknownTag:
			iStatus = OCSP::EUnknown;
			break;
		default:
			User::Leave(OCSP::EMalformedResponse);
		}
		
	// Carry on with thisUpdate
	TASN1DecGeneralizedTime decGT;
	iThisUpdate = decGT.DecodeDERL(*items.At(2));

	// Optional bits...
	TInt numItems = items.Count();
	if (numItems > 3)
		{
		TInt nextItem = 3;

		// Maybe nextUpdate is there too
		TASN1DecGeneric& item4 = *items.At(3);
		if (item4.Tag() == KNextUpdateTag)
			{
			++nextItem;
			TASN1DecGeneralizedTime decGT;
			TInt pos = 0;
			iNextUpdate = new (ELeave) TTime (decGT.DecodeDERL(item4.GetContentDER(), pos));
			}

		// Check for extensions - we don't support any, but we need to leave if there are any marked 'critical'
		if (nextItem < numItems)
			{
			TASN1DecGeneric& extnList = *items.At(nextItem);
			if (extnList.Tag() == KSingleExtensionsTag)
				{
				// OK, we've got extensions, with an explicit tag.  Loop through them...
				TASN1DecSequence decSeq;
				TInt pos = 0;
				CArrayPtr<TASN1DecGeneric>* extns = decSeq.DecodeDERLC(extnList.GetContentDER(), pos);
				TInt numExts = extns->Count();
				for (TInt extIndex = 0; extIndex < numExts; ++extIndex)
					{
					TASN1DecGeneric& ext = *extns->At(extIndex);
					CArrayPtr<TASN1DecGeneric>* terms = decSeq.DecodeDERLC(ext);

					// Check critical flag (may be absent - default value false
					if (terms->Count() == 3)
						{
						TASN1DecBoolean boolDec;
						if (boolDec.DecodeDERL(*terms->At(1)))
							{
							User::Leave(OCSP::EUnknownCriticalExtension);
							}
						}
					CleanupStack::PopAndDestroy(); // Clean up 'terms'
					}
				CleanupStack::PopAndDestroy(); // Clean up 'extns'
				}
			}
		}
	}
