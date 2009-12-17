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
#include "RightsResp.h"
#include "RightsRespParser.h"

using namespace Roap;

// LOCAL CONSTANTS AND MACROS
_LIT8(KStatus, "status");
_LIT8(KSessionId, "sessionId");
_LIT8(KErrorUrl, "errorRedirectURL");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TRightsRespParser::TRightsRespParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TRightsRespParser::TRightsRespParser(
    CRightsResp* aResponse)
    {
    iResponse = aResponse;
    }

// -----------------------------------------------------------------------------
// TRightsRespParser::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TRightsRespParser::OnStartElementL(
    CRoapParser& aParser,
    TInt aState,
    const RTagInfo& /*aElement*/,
    const RAttributeArray& aAttributes)
    {
    HBufC8* buffer = NULL;

    switch (aState)
        {
        case ERoResponseState:
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
// TRightsRespParser::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TRightsRespParser::OnEndElementL(
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
        case ECertificateState:
            buffer = Base64DecodeL(*aParser.iContent);
            CleanupStack::PushL(buffer);
            iResponse->iCertificateChain.AppendL(buffer);
            CleanupStack::Pop(); // buffer
            break;
        case EOcspResponseState:
            buffer = Base64DecodeL(*aParser.iContent);
            CleanupStack::PushL(buffer);
            iResponse->iOcspResponse.AppendL(buffer);
            CleanupStack::Pop(); // buffer
            break;
        case ERiIdRoResponseState:
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
        case ENonceState:
            iResponse->iNonce = Base64DecodeL(*aParser.iContent);
            break;
        case ESignatureRoResponseState:
            iResponse->iSignature = Base64DecodeL(*aParser.iContent);
            break;
        case EEncKeyState:
            iResponse->iRoEncryptionKey = Base64DecodeL(*aParser.iContent);
            break;
        case ETransactionIdContentIdState:
            buffer = aParser.iContent->AllocLC();
            iResponse->iContentIDs.AppendL( buffer );
            CleanupStack::Pop(); // buffer
            break;
        case ETransactionIdState:
            buffer = aParser.iContent->AllocLC();
            iResponse->iTransTrackIDs.AppendL( buffer );
            CleanupStack::Pop(); // buffer
            break;
        }
    }
