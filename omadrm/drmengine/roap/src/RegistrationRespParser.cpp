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
#include "RoapParser.h"
#include "RegistrationResp.h"
#include "RegistrationRespParser.h"

using namespace Roap;

// LOCAL CONSTANTS AND MACROS
_LIT8(KStatus, "status");
_LIT8(KSessionId, "sessionId");
_LIT8(KErrorUrl, "errorRedirectURL");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TRegistrationRespParser::TRegistrationRespParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TRegistrationRespParser::TRegistrationRespParser(
    CRegistrationResp* aResponse)
    {
    iResponse = aResponse;
    }

// -----------------------------------------------------------------------------
// TRegistrationRespParser::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TRegistrationRespParser::OnStartElementL(
    CRoapParser& aParser,
    TInt aState,
    const RTagInfo& /*aElement*/,
    const RAttributeArray& aAttributes)
    {
    HBufC8* buffer = NULL;

    switch (aState)
        {
        case ERegistrationResponseState:
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
            iResponse->iSession =
                aParser.GetAttributeValueL(aAttributes, KSessionId);
                            
            iResponse->iErrorUrl = aParser.GetAttributeValueL(aAttributes, KErrorUrl);
            break;
        }
    }

// -----------------------------------------------------------------------------
// TRegistrationRespParser::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TRegistrationRespParser::OnEndElementL(
    CRoapParser& aParser,
    TInt aState,
    const RTagInfo& /*aElement*/)
    {
    switch (aState)
        {
        case ECertificateState:
            if( aParser.iContent->Length() )
                iResponse->iCertificateChain.Append(
                    Base64DecodeL(*aParser.iContent));
            break;
        case EOcspResponseState:
            if( aParser.iContent->Length() )
                iResponse->iOcspResponse.Append(Base64DecodeL(*aParser.iContent));
            break;
        case ERiUrlState:
            if( aParser.iContent->Length() )
                iResponse->iRiUrl = aParser.iContent->AllocL();
            break;
        case ESignatureState:
            if( aParser.iContent->Length() )
                iResponse->iSignature = Base64DecodeL(*aParser.iContent);
            break;
        case EWhiteListState:
            if( aParser.iContent->Length() )
                iResponse->iWhiteList.Append(aParser.iContent->AllocL());
            break;
        }
    }

//  End of File  
