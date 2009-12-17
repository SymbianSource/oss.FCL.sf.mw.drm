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
#include "RIHello.h"
#include "RIHelloParser.h"

using namespace Roap;

// LOCAL CONSTANTS AND MACROS
_LIT8(KStatus, "status");
_LIT8(KType, "type");
_LIT8(KSessionId, "sessionId");
_LIT8(KDeviceDetails, "DeviceDetails");
_LIT8(KPeerKeyIdentifier, "PeerKeyIdentifier");
_LIT8(KCertificateCaching, "CertificateCaching");
_LIT8(KErrorUrl, "errorRedirectURL");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TRIHelloParser::TRIHelloParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TRIHelloParser::TRIHelloParser(
    CRIHello* aResponse)
    {
    iResponse = aResponse;
    }

// -----------------------------------------------------------------------------
// TRIHelloRespParser::OnStartElementL
// 
// -----------------------------------------------------------------------------
//
void TRIHelloParser::OnStartElementL(
    CRoapParser& aParser,
    TInt aState,
    const RTagInfo& /*aElement*/,
    const RAttributeArray& aAttributes)
    {
    HBufC8* buffer = NULL;

    switch (aState)
        {
        case ERiHelloState:
            {
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
        case EExtensionState:
            {
            buffer = aParser.GetAttributeValueL(aAttributes, KType );
            if(buffer != NULL)
                {
                if(buffer->Find(KDeviceDetails) >= KErrNone)
                    {
                    iResponse->iNeedDeviceDetails = ETrue;
                    }
                 if(buffer->Find(KPeerKeyIdentifier) >= KErrNone)
                    {
                    iResponse->iPeerKeyIdentifier = ETrue;
                    }
                 if(buffer->Find(KCertificateCaching) >= KErrNone)
                    {
                    iResponse->iCertificateCaching = ETrue;
                    }
                delete buffer;
                }
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// TRIHelloRespParser::OnEndElementL
// 
// -----------------------------------------------------------------------------
//
void TRIHelloParser::OnEndElementL(
    CRoapParser& aParser,
    TInt aState,
    const RTagInfo& /*aElement*/)
    {
    HBufC8* buffer = NULL;
    
    if( !aParser.iContent )
        {
        return;
        }

    switch (aState)
        {
        case ESelectedVersionState:
            if ( aParser.iContent->Length() > KMaxRoapVersionLength )
                {
                User::Leave( KErrCorrupt );
                }
            iResponse->iSelectedVersion.Copy(*aParser.iContent);
            break;
        case ERiNonceState:
            iResponse->iRiNonce = Base64DecodeL(*aParser.iContent);
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
        case ESelectedAlgorithmState:
            buffer = aParser.iContent->AllocLC();
            iResponse->iAlgorithms.AppendL( buffer );
            CleanupStack::Pop();
            break;
        case EServerInfoState:
            iResponse->iServerInfo = aParser.iContent->AllocL();   
            break;
        case ETrustedAuthoritiesState:
            buffer = Base64DecodeL(*aParser.iContent);
            iResponse->iTrustedAuthorities.AppendL(buffer);
            break;
        case EPeerKeyIdentifierRIHelloState:
            buffer = Base64DecodeL(*aParser.iContent);
            if ( buffer->Length() > SHA1_HASH )
                {
                delete buffer;
                buffer = NULL;
                User::Leave( KErrCorrupt );
                }
            iResponse->iPeerKeyId.Copy(*buffer);
            delete buffer;
            break;
        }
    }

//  End of File  
