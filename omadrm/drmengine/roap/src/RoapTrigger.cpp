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

#include "RoapTrigger.h"

using namespace Roap;

// ================= CONSTANTS =======================
_LIT8( KTriggerHeader, "<roap-trigger:roapTrigger>" );
_LIT8(KRegistrationStart, "<registrationRequest>");
_LIT8(KRegistrationEnd, "</registrationRequest>");
_LIT8(KRoAcquisitionStart, "<roAcquisition>");
_LIT8(KRoAcquisitionEnd, "</roAcquisition>");
_LIT8(KJoinDomianStart, "<joinDomain>");
_LIT8(KJoinDomianEnd, "</joinDomain>");
_LIT8(KLeaveDomianStart, "<leaveDomain>");
_LIT8(KLeaveDomianEnd, "</leaveDomain>");
#ifdef RD_DRM_METERING
_LIT8(KExtendedTriggerStart, "<extendedTrigger ");
_LIT8(KIdStart, "id=\"");
_LIT8(KIdEnd, "\" " );
_LIT8(KMeteringType, "type=\"meteringReport\">");
_LIT8(KExtendedTriggerEnd, "</extendedTrigger>");
#endif
_LIT8(KRiIdStart, "<riID><keyIdentifier xsi:type=\"roap:X509SPKIHash\"><hash>");
_LIT8(KRiIdEnd, "</hash></keyIdentifier></riID>");
_LIT8(KRiAliasStart, "<riAlias>");
_LIT8(KRiAliasEnd, "</riAlias>");
_LIT8(KRoapUrlStart, "<roapURL>");
_LIT8(KRoapUrlEnd, "</roapURL>");
_LIT8(KDomainIdStart, "<domainID>");
_LIT8(KDomainIdEnd, "</domainID>");
_LIT8(KDomainAliasStart, "<domainAlias>");
_LIT8(KDomainAliasEnd, "</domainAlias>");
_LIT8(KRoIdStart, "<roID>");
_LIT8(KRoIdEnd, "</roID>");
_LIT8(KContentIdStart, "<contentID>");
_LIT8(KContentIdEnd, "</contentID>");  
_LIT8( KTriggerEnd,"</roap-trigger:roapTrigger>");


// FORWARD DECLARATIONS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CRoapTrigger::NewL()
// ---------------------------------------------------------
//
EXPORT_C CRoapTrigger* CRoapTrigger::NewL()
    {
    CRoapTrigger* data = new (ELeave) CRoapTrigger();
    CleanupStack::PushL( data );
    data->ConstructL();
    CleanupStack::Pop( data );
    return data;
    }
    
// ---------------------------------------------------------
// CRoapTrigger::NewL()
// ---------------------------------------------------------
//
EXPORT_C CRoapTrigger* CRoapTrigger::NewL( const TTriggerType& aTriggerType,
                                           const TDesC8& aRiId,
                                           const TDesC8& aDomainId,
                                           const TDesC8& aRoapUrl,
                                           const RPointerArray<HBufC8>& aRoIdList,
                                           const RPointerArray<HBufC8>& aContentIdList )
    {
    CRoapTrigger* data = new (ELeave) CRoapTrigger();
    CleanupStack::PushL( data );
    data->ConstructL( aTriggerType, aRiId, aDomainId, aRoapUrl, aRoIdList, aContentIdList );
    CleanupStack::Pop( data );
    return data;
    }

// ---------------------------------------------------------
// CRoapTrigger::~CRoapTrigger()
// ---------------------------------------------------------
//
EXPORT_C CRoapTrigger::~CRoapTrigger()
    {
    delete iId;
    delete iDomainId;
    delete iRoapUrl;
    delete iSignature;
    delete iEncKey;
    delete iNonce;
    delete iDigestValue;
    delete iRiAlias;
    delete iDomainAlias;
    iRoIdList.ResetAndDestroy();
    iContentIdList.ResetAndDestroy();
    }

// ---------------------------------------------------------
// CRoapTrigger::CRoapTrigger()
// ---------------------------------------------------------
//
CRoapTrigger::CRoapTrigger():
    iId(NULL),
    iDomainId(NULL),
    iRoapUrl(NULL),
    iSignature(NULL),
    iEncKey(NULL),
    iNonce(NULL),
    iDigestValue(NULL),
    iRiAlias(NULL)
    {
    }

// ---------------------------------------------------------
// CRoapTrigger::ConstructL()
// ---------------------------------------------------------
//
void CRoapTrigger::ConstructL()
    {  
    }
    
// ---------------------------------------------------------
// CRoapTrigger::ConstructL()
// ---------------------------------------------------------
//
void CRoapTrigger::ConstructL( const TTriggerType& aTriggerType,
                               const TDesC8& aRiId,
                               const TDesC8& aDomainId,
                               const TDesC8& aRoapUrl,
                               const RPointerArray<HBufC8>& aRoIdList,
                               const RPointerArray<HBufC8>& aContentIdList )
    {
    HBufC8* temp = NULL;
    
    iTriggerType = aTriggerType;
    iRiId.Copy( aRiId );
    iDomainId = aDomainId.AllocL();
    iRoapUrl = aRoapUrl.AllocL();
    
    for( TInt i = 0; i < aRoIdList.Count(); i++ )
        {
        temp = aRoIdList[i]->AllocLC();
        iRoIdList.AppendL( temp );
        CleanupStack::Pop(); //temp
        }
        
    for( TInt i = 0; i < aContentIdList.Count(); i++ )
        {
        temp = aContentIdList[i]->AllocLC();
        iContentIdList.AppendL( temp );
        CleanupStack::Pop(); //temp
        }    
    }
    
// -----------------------------------------------------------------------------
// CRoapTrigger::MessageAsXmlL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CRoapTrigger::MessageAsXmlL(void)
    {
    HBufC8* r = NULL;
    CBufFlat* b = NULL;
    TInt i;

    b = CBufFlat::NewL(128);
    CleanupStack::PushL(b);
    BufAppendL(b, KTriggerHeader);
    
    switch ( iTriggerType )
        {
        case ERegistrationTrigger:
            BufAppendL(b, KRegistrationStart);
            break;
        case ERoAcquisitionTrigger:
            BufAppendL(b, KRoAcquisitionStart);
            break;
        case EJoinDomainTrigger:
            BufAppendL(b, KJoinDomianStart);
            break;
        case ELeaveDomainTrigger:
            BufAppendL(b, KLeaveDomianStart);
            break;
#ifdef RD_DRM_METERING            
        case EExtendedTrigger:
            BufAppendL(b, KExtendedTriggerStart);
            BufAppendL(b, KIdStart);
            BufAppendL(b, *iId );
            BufAppendL(b, KIdEnd);
            BufAppendL(b, KMeteringType );
            break;
#endif        
        }
        
    if (iRiId.Length() > 0)
        {
        BufAppendL(b, KRiIdStart);
        BufAppendBase64L(b, iRiId);
        BufAppendL(b, KRiIdEnd);
        }
        
    if (iRiAlias)
        {
        BufAppendL(b, KRiAliasStart);
        BufAppendBase64L(b, *iRiAlias);
        BufAppendL(b, KRiAliasEnd);
        }
        
    if (iDomainId)
        {
        BufAppendL(b, KDomainIdStart);
        BufAppendL(b, *iDomainId);
        BufAppendL(b, KDomainIdEnd);
        }
        
     if (iDomainAlias)
        {
        BufAppendL(b, KDomainAliasStart);
        BufAppendBase64L(b, *iDomainAlias);
        BufAppendL(b, KDomainAliasEnd);
        }
        
     if (iRoapUrl)
        {
        BufAppendL(b, KRoapUrlStart);
        BufAppendL(b, *iRoapUrl);
        BufAppendL(b, KRoapUrlEnd);
        }
        
    for (i = 0; i < iRoIdList.Count(); i++)
        {
        BufAppendL(b, KRoIdStart);
        BufAppendL(b, *iRoIdList[i]);
        BufAppendL(b, KRoIdEnd);
        }
        
    for (i = 0; i < iContentIdList.Count(); i++)
        {
        BufAppendL(b, KContentIdStart);
        BufAppendL(b, *iContentIdList[i]);
        BufAppendL(b, KContentIdEnd);
        }
         
    switch ( iTriggerType )
        {
        case ERegistrationTrigger:
            BufAppendL(b, KRegistrationEnd);
            break;
        case ERoAcquisitionTrigger:
            BufAppendL(b, KRoAcquisitionEnd);
            break;
        case EJoinDomainTrigger:
            BufAppendL(b, KJoinDomianEnd);
            break;
        case ELeaveDomainTrigger:
            BufAppendL(b, KLeaveDomianEnd);
            break;
#ifdef RD_DRM_METERING            
        case EExtendedTrigger:
            BufAppendL(b, KExtendedTriggerEnd);
            break;
#endif            
        }

    BufAppendL(b, KTriggerEnd);

    r = b->Ptr(0).AllocL();
    CleanupStack::PopAndDestroy(b);
    return r;
    }
    
// ---------------------------------------------------------
// CRoapTrigger::ValidTrigger()
// ---------------------------------------------------------
//    
EXPORT_C TBool CRoapTrigger::ValidTrigger()
    {
    if ( iTriggerType == EJoinDomainTrigger ||
         iTriggerType == ELeaveDomainTrigger )
        {
        if ( !iDomainId )
            {
            return EFalse;
            }
        }
    if ( iRiId.Length() > 0 && iRoapUrl )
        {
        return ETrue;
        }
    return EFalse;   
    }

