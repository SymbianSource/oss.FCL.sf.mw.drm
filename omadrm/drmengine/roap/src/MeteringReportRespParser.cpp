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
* Description:  MeteringReportResponse Parser
*
*/



// INCLUDE FILES
#include <e32base.h>
#include "Base64.h"
#include "RoapParser.h"
#include "MeteringReportResp.h"
#include "MeteringReportRespParser.h"

using namespace Roap;

// LOCAL CONSTANTS AND MACROS
_LIT8(KStatus, "status");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TRegistrationRespParser::TRegistrationRespParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TMeteringRespParser::TMeteringRespParser(
    CMeteringResp* aResponse)
    {
    iResponse = aResponse;
    }

// -----------------------------------------------------------------------------
// TMeteringRespParser::OnStartElementL
// -----------------------------------------------------------------------------
//
void TMeteringRespParser::OnStartElementL(
    CRoapParser& aParser,
    TInt aState,
    const RTagInfo& /*aElement*/,
    const RAttributeArray& aAttributes)
    {
    HBufC8* buffer = NULL;

    switch (aState)
        {
        case EMeteringReportResponseState:
            buffer = aParser.GetAttributeValueL(aAttributes, KStatus);
            if ( buffer )
                {
                CleanupStack::PushL( buffer );

                // In case of a malformer request
                _LIT8(KStatus, "MalFormedRequest");
                if ( buffer->Compare( KStatus ) == 0 )
                    {
                    iResponse->iStatus = EMalformedRequest;
                    }
                else
                    {
                    iResponse->iStatus = aParser.ConvertRoapStatus( *buffer );
                    }
                CleanupStack::PopAndDestroy( buffer );
                }
            else
                {
                iResponse->iStatus = EUnknownStatus;
                }
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// TMeteringRespParser::OnEndElementL
// -----------------------------------------------------------------------------
//
void TMeteringRespParser::OnEndElementL(
    CRoapParser& aParser,
    TInt aState,
    const RTagInfo& /*aElement*/)
    {
    HBufC8* buffer = NULL;

    if ( !aParser.iContent )
        {
        return;
        }

    switch (aState)
        {
        case EDeviceIdState:
            buffer = Base64DecodeL( *aParser.iContent );
            CleanupStack::PushL( buffer );
            if ( buffer->Length() != SHA1_HASH )
                {
                CleanupStack::PopAndDestroy( buffer );
                User::Leave( KErrCorrupt );
                }
            iResponse->iDeviceId.Copy( *buffer );
            CleanupStack::PopAndDestroy( buffer );
            break;
        case ERiIdState:
            buffer = Base64DecodeL( *aParser.iContent );
            CleanupStack::PushL( buffer );
            if ( buffer->Length() != SHA1_HASH )
                {
                User::Leave( KErrCorrupt );
                }
            iResponse->iRiId.Copy( *buffer );
            CleanupStack::PopAndDestroy( buffer );
            buffer=NULL;
            break;
        case ENonceState:
            if( aParser.iContent->Length() )
                {
                buffer = Base64DecodeL( *aParser.iContent );
                CleanupStack::PushL( buffer );
                if ( buffer->Length() != KDeviceNonceLength )
                    {
                    User::Leave( KErrCorrupt );
                    }
                CleanupStack::Pop( buffer );
                iResponse->iDeviceNonce = buffer;
                buffer=NULL;
                }
            else
                {
                User::Leave( KErrCorrupt );
                }
            break;
        case ESignatureState:
            iResponse->iSignature = Base64DecodeL( *aParser.iContent );
            break;
        case ECertificateState:
            buffer = Base64DecodeL( *aParser.iContent );
            CleanupStack::PushL( buffer );
            iResponse->iCertificateChain.AppendL( buffer );
            CleanupStack::Pop( buffer );
            break;
        case EOcspResponseState:
            buffer = Base64DecodeL(*aParser.iContent);
            CleanupStack::PushL(buffer);
            iResponse->iOcspResponse.AppendL(buffer);
            CleanupStack::Pop( buffer );
            break;
        case EPostResponseUrlState:
            if ( aParser.iContent )
                {
                iResponse->iPrUrl = aParser.iContent->AllocL();
                }
            else
                {
                User::Leave( KErrCorrupt );
                }
            break;
        default:
            break;
        }
    }

//  End of File
