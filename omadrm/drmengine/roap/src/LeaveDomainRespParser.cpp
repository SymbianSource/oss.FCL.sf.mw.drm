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
#include "LeaveDomainResp.h"
#include "LeaveDomainRespParser.h"

using namespace Roap;

// LOCAL CONSTANTS AND MACROS
_LIT8(KStatus, "status");
_LIT8(KErrorUrl, "errorRedirectURL");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TLeaveDomainRespParser::TLeaveDomainRespParser
// 
// -----------------------------------------------------------------------------
//
TLeaveDomainRespParser::TLeaveDomainRespParser(
    CLeaveDomainResp* aResponse)
    {
    iResponse = aResponse;
    }

// -----------------------------------------------------------------------------
// TLeaveDomainRespParser::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TLeaveDomainRespParser::OnStartElementL(
    CRoapParser& aParser,
    TInt aState,
    const RTagInfo& /*aElement*/,
    const RAttributeArray& aAttributes)
    {
    HBufC8* buffer = NULL;

    switch (aState)
        {
        case ELeaveDomainResponseState:
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
        }
    }

// -----------------------------------------------------------------------------
// TLeaveDomainRespParser::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TLeaveDomainRespParser::OnEndElementL(
    CRoapParser& aParser,
    TInt aState,
    const RTagInfo& /*aElement*/)
    {
    if (!aParser.iContent)
        {
        return;
        }
        
    switch (aState)
        {
        case EDomainIdState:
            iResponse->iDomainId = aParser.iContent->AllocL();
            break;
        }
    }

//  End of File  
