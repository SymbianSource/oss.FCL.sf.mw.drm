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
#include "RoapTrigger.h"
#include "RoapTriggerParser.h"

using namespace Roap;

// LOCAL CONSTANTS AND MACROS

_LIT8(KId, "id");
#ifdef RD_DRM_METERING  
_LIT8(KType, "type");
_LIT8(KMeteringReport, "meteringReport" );
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TRoapTriggerParser::TRoapTriggerParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TRoapTriggerParser::TRoapTriggerParser(
    CRoapTrigger* aResponse)
    {
    iResponse = aResponse;
    }

// -----------------------------------------------------------------------------
// TRoapTriggerRespParser::OnStartElementL
// 
// -----------------------------------------------------------------------------
//
void TRoapTriggerParser::OnStartElementL(
    CRoapParser& aParser,
    TInt aState,
    const RTagInfo& /*aElement*/,
    const RAttributeArray& aAttributes)
    {
    if (aState == ERegistrationRequestState ||
        aState == ERoAcquisitionState ||
        aState == EJoinDomainState ||
        aState == ELeaveDomainState
#ifdef RD_DRM_METERING            
        || aState == EExtendedTriggerElementState 
#endif  
        )
        {
        iResponse->iId =
            aParser.GetAttributeValueL(aAttributes, KId);

        }
    switch (aState)
        {
        case ERegistrationRequestState:
            iResponse->iTriggerType = ERegistrationTrigger;
            break;
        case ERoAcquisitionState:
            iResponse->iTriggerType = ERoAcquisitionTrigger;
            break;
        case EJoinDomainState:
            iResponse->iTriggerType = EJoinDomainTrigger;
            break;
        case ELeaveDomainState:
            iResponse->iTriggerType = ELeaveDomainTrigger;
            break;
#ifdef RD_DRM_METERING                   
        case EExtendedTriggerElementState:
            HBufC8* type = aParser.GetAttributeValueL(aAttributes, KType);
            CleanupStack::PushL( type );
            if ( type->Compare( KMeteringReport ) == 0 )
                {
                iResponse->iTriggerType = EMeteringReportTrigger;                     
                }
            else
                { // We have an unsupported extendedTrigger
                iResponse->iTriggerType = EExtendedTrigger;
                }
            CleanupStack::PopAndDestroy( type ); // Not needed anymore since
            break;                               // triggerType is set
#endif                 
        }
    }

// -----------------------------------------------------------------------------
// TRoapTriggerRespParser::OnEndElementL
// 
// -----------------------------------------------------------------------------
//
void TRoapTriggerParser::OnEndElementL(
    CRoapParser& aParser,
    TInt aState,
    const RTagInfo& /*aElement*/)
    {
    HBufC8* buffer = NULL;

    switch (aState)
        {
        case ERoapUrlState:
            if( aParser.iContent )
                iResponse->iRoapUrl = aParser.iContent->AllocL();                    
            break;
        case EDomainIdState:
            if( aParser.iContent && !iResponse->iDomainId )
                iResponse->iDomainId = aParser.iContent->AllocL();                    
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
        case ERoIdState:
            if( aParser.iContent )
                {
                buffer = aParser.iContent->AllocLC();
                iResponse->iRoIdList.AppendL( buffer );
                CleanupStack::Pop();
                }
            break;
        case EContentIdState:
            if( aParser.iContent )
                {
                buffer = aParser.iContent->AllocLC();
                iResponse->iContentIdList.AppendL( buffer );
                CleanupStack::Pop();
                }                    
            break;
        case ESignatureValueState:
            iResponse->iSignature = Base64DecodeL(*aParser.iContent);
            break;
        case EEncKeyState:
            iResponse->iEncKey = Base64DecodeL(*aParser.iContent);
            break;
        case ENonceState:
            if( aParser.iContent )
                iResponse->iNonce = aParser.iContent->AllocL();                   
            break;
        case EDigestValueState:
            if( aParser.iContent )
                iResponse->iDigestValue = Base64DecodeL(*aParser.iContent);                   
            break;
        case ERiAliasState:
            if( aParser.iContent )
                   iResponse->iRiAlias = aParser.iContent->AllocL();
            break;
        case EDomainAliasState:
            if( aParser.iContent )
                   iResponse->iDomainAlias = aParser.iContent->AllocL();
            break;
        }
    }

//  End of File  
