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



// INCLUDE FILES
#include <e32base.h>
#include "Base64.h"
#include "OmaCrypto.h"
#include "CmlaCrypto.h"
#include "RoapParser.h"
#include "JoinDomainResp.h"
#include "JoinDomainRespParser.h"

using namespace Roap;

// LOCAL CONSTANTS AND MACROS
_LIT8(KStatus, "status");
_LIT8(KErrorUrl, "errorRedirectURL");
_LIT8(KAlgorithmAttr, "Algorithm");
_LIT8(KInfinite, "Infinite");
_LIT8(KId, "Id");
_LIT8(KHashChainSupport, "HashChainSupport");
_LIT8(KType, "type");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TJoinDomainRespParser::TJoinDomainRespParser
// -----------------------------------------------------------------------------
//
TJoinDomainRespParser::TJoinDomainRespParser(
    CJoinDomainResp* aResponse)
    {
    iResponse = aResponse;
    }

// -----------------------------------------------------------------------------
// TJoinDomainRespParser::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TJoinDomainRespParser::OnStartElementL(
    CRoapParser& aParser,
    TInt aState,
    const RTagInfo& /*aElement*/,
    const RAttributeArray& aAttributes)
    {
    HBufC8* buffer = NULL;

    switch (aState)
        {
        case EJoinDomainResponseState:
            buffer = aParser.GetAttributeValueL(aAttributes, KStatus);
            if (buffer != NULL)
                {
                iResponse->iStatus = aParser.ConvertRoapStatus(*buffer);
                delete buffer;
                }
            else
                {
                iResponse->iStatus = EUnknownStatus;
                }
                
            iResponse->iErrorUrl = aParser.GetAttributeValueL(aAttributes, KErrorUrl);
            break;
        case EDomainKeyEncryptionMethodState:
            buffer = aParser.GetAttributeValueL(aAttributes, KAlgorithmAttr);
            if (buffer )
                {
                iResponse->iTransportScheme =
                    CmlaCrypto::AlgorithmIdToTransportScheme(*buffer);
                delete buffer;
                }
            break;
        case EDomainKeyEncKeyState:
            buffer = aParser.GetAttributeValueL(aAttributes, KId);
            if ( buffer )
                {
                CleanupStack::PushL(buffer);
                iResponse->iDomainKeyIDs.AppendL(buffer);
                CleanupStack::Pop();
                }
        case EExtensionState:
            buffer = aParser.GetAttributeValueL(aAttributes, KType );
            if(buffer != NULL)
                {
                if(buffer->Find(KHashChainSupport) >= KErrNone)
                    {
                    iResponse->iHashChainSupport = ETrue;
                    }
                delete buffer;
                }
        break;
        }
    }

// -----------------------------------------------------------------------------
// TJoinDomainRespParser::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TJoinDomainRespParser::OnEndElementL(
    CRoapParser& aParser,
    TInt aState,
    const RTagInfo& /*aElement*/)
    {
    HBufC8* buffer = NULL;
    
    if (!aParser.iContent)
        {
        return;
        }

    switch (aState)
        {
        case ECertificateState:
            buffer = Base64DecodeL(*aParser.iContent);
            CleanupStack::PushL(buffer);
            iResponse->iCertificateChain.AppendL(buffer);
            CleanupStack::Pop(); //buffer
            break;
        case EOcspResponseState:
            buffer = Base64DecodeL(*aParser.iContent);
            CleanupStack::PushL(buffer);
            iResponse->iOcspResponse.AppendL(buffer);
            CleanupStack::Pop(); // buffer
            break;
        case ERiIdState:
            buffer = Base64DecodeL(*aParser.iContent);
            if ( buffer->Length() > SHA1_HASH )
                {
                delete buffer;
                buffer = NULL;
                User::Leave( KErrCorrupt );
                }
            iResponse->iRiId.Copy(*buffer);
            delete buffer;
            break;
        case ERiIdJoinDomainResponseState:
            buffer = Base64DecodeL(*aParser.iContent);
            if ( buffer->Length() > SHA1_HASH )
                {
                delete buffer;
                buffer = NULL;
                User::Leave( KErrCorrupt );
                }
            iResponse->iDomainKeyRiId.Copy(*buffer);
            delete buffer;
            break;
        case EDeviceIdState:
            buffer = Base64DecodeL(*aParser.iContent);
            if ( buffer->Length() > SHA1_HASH )
                {
                delete buffer;
                buffer = NULL;
                User::Leave( KErrCorrupt );
                }
            iResponse->iDeviceId.Copy(*buffer);
            delete buffer;
            break;
        case ESignatureState:
            iResponse->iSignature = Base64DecodeL( *aParser.iContent );
            break;
        case EEncKeyState:
            buffer = Base64DecodeL( *aParser.iContent );
            CleanupStack::PushL( buffer );
            iResponse->iDomainKeys.AppendL( buffer );
            CleanupStack::Pop(); // buffer
            break;
        case ENotAfterState:
            if ( aParser.iContent )
                {
                if (aParser.iContent->CompareF( KInfinite() ) == KErrNone )
                    {
                    iResponse->iDomainExpiration = Time::NullTTime();
                    }
                else
                    {
                    iResponse->iDomainExpiration = iResponse->Iso8601ToTime(
                        *aParser.iContent);   
                    }
                }
            break;
        case EMacJoinDomainResponseState:
            buffer = Base64DecodeL( *aParser.iContent );
            CleanupStack::PushL( buffer );
            iResponse->iMacs.AppendL( buffer );
            CleanupStack::Pop(); // buffer
            break;
        }
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ?function_name implements...
// ?implementation_description.
// Returns: ?value_1: ?description
//          ?value_n: ?description
//                    ?description
// -----------------------------------------------------------------------------
//

//  End of File  
