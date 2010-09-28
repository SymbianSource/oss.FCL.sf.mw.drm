/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Access to content and data attributes
*
*/


// Includes
#include <apgcli.h>
#include <f32file.h>
#include <charconv.h>

#include <caf/caf.h>
#include <caf/cafplatform.h>
#include <caf/attributeset.h>
#include <caf/stringattributeset.h>
#include <caf/virtualpath.h>
#include <caf/caferr.h>
#include <utf.h>
#include <drmagents.h>
#include "Oma2Agent.h"
#include "Oma2AgentAttributes.h"
#include "Oma1Dcf.h"
#include "oma2dcf.h"
#include "DRMRightsClient.h"
#include "DRMProtectedRoParser.h"

using namespace ContentAccess;

// LOCAL FUNCTION PROTOTYPES
LOCAL_C HBufC* CollectEmbeddedRosL(
    COma2Dcf* aDcf);

LOCAL_C HBufC* GetDomainRiUrlL(
    COma2Dcf* aDcf);

LOCAL_C HBufC* GetDomainIdL(
    COma2Dcf* aDcf);

LOCAL_C HBufC* GetDomainRiIdL(
    COma2Dcf* aDcf);

LOCAL_C TInt CheckIntentL(
    const TDesC8& aContentId,
    TIntent aIntent,
    RDRMRightsClient* aRightsClient);

LOCAL_C TInt GetEstimatedArrivalTime(
    const TDesC8& aContentId,
    RDRMRightsClient* aRightsClient);

LOCAL_C TInt GetContentName(
    const TDesC8& aContentId,
    HBufC*& aContentName,
    RDRMRightsClient* aRightsClient );

// LOCAL CONSTANTS

_LIT16(KOma1DcfContentType16, "application/vnd.oma.drm.content");
_LIT16(KOma2DcfContentType16, "application/vnd.oma.drm.dcf");
const TInt KMaxAlbumTrack = 3;
const TInt KMaxRecordingYear = 6;

const TInt KAllowAllDefined =  
    DRM::EDrmAllowAudioAnalog |
    DRM::EDrmAllowAudioFmTransmitter |
    DRM::EDrmAllowAudioBluetooth |
    DRM::EDrmAllowAudioUplink |
    DRM::EDrmAllowVideoAnalog |
    DRM::EDrmAllowVideoMacroVision |
    DRM::EDrmAllowAudioUsb |   
    DRM::EDrmAllowAudioHdmiHdcpRequired |
    DRM::EDrmAllowVideoHdmiHdcpRequired;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CollectEmbeddedRosL
// Concatenate all rights objects embedded in a DCF into one buffer
// -----------------------------------------------------------------------------
//
LOCAL_C HBufC* CollectEmbeddedRosL(
    COma2Dcf* aDcf)
    {
    HBufC* r = NULL;
    HBufC* ro = NULL;
    TPtr ptr(NULL, 0);
    TInt i;
    TInt n;

    if (aDcf != NULL && aDcf->iRightsObjects.Count() > 0)
        {
        n = 0;
        for (i = 0; i < aDcf->iRightsObjects.Count(); i++)
            {
            n += aDcf->iRightsObjects[i]->Length();
            }
        r = HBufC::NewLC(n);
        ptr.Set(r->Des());
        for (i = 0; i < aDcf->iRightsObjects.Count(); i++)
            {
            ro = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
                *aDcf->iRightsObjects[i]);
            ptr.Append(*ro);
            delete ro;
            }
        CleanupStack::Pop(r);
        }
    return r;
    }

// -----------------------------------------------------------------------------
// GetDomainRiUrlL
// Returns the RI URL from the last embedded domain RO, to be used for
// automatically joining a domain
// -----------------------------------------------------------------------------
//
LOCAL_C HBufC* GetDomainRiUrlL(
    COma2Dcf* aDcf)
    {
    HBufC8* url = NULL;
    HBufC* r = NULL;
    CDrmProtectedRoParser* parser = NULL;

    if (aDcf != NULL && aDcf->iRightsObjects.Count() > 0)
        {
        parser = CDrmProtectedRoParser::NewL();
        CleanupStack::PushL(parser);
        url = parser->GetRiUrlL(*aDcf->iRightsObjects[ 0 ]);
        if (url != NULL)
            {
            CleanupStack::PushL(url);
            r = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*url);
            CleanupStack::PopAndDestroy(); // url
            }
        CleanupStack::PopAndDestroy(); // parser
        }
    return r;
    }

// -----------------------------------------------------------------------------
// GetDomainRiIdL
// Returns the RI ID from the last embedded domain RO, to be used for
// automatically joining a domain
// -----------------------------------------------------------------------------
//
LOCAL_C HBufC* GetDomainRiIdL(
    COma2Dcf* aDcf)
    {
    HBufC8* id = NULL;
    HBufC* r = NULL;
    CDrmProtectedRoParser* parser = NULL;
    TInt i;
    TPtr ptr(NULL, 0);

    if (aDcf != NULL && aDcf->iRightsObjects.Count() > 0)
        {
        parser = CDrmProtectedRoParser::NewL();
        CleanupStack::PushL(parser);
        id = parser->GetRiIdL(*aDcf->iRightsObjects[ 0 ]);
        if (id != NULL)
            {
            CleanupStack::PushL(id);
            r = HBufC::NewL(id->Length() * 2);
            ptr.Set(r->Des());
            for (i = 0; i < id->Length(); i++)
                {
                ptr.Append((*id)[i]);
                }
            CleanupStack::PopAndDestroy(); // url
            }
        CleanupStack::PopAndDestroy(); // parser
        }
    return r;
    }

// -----------------------------------------------------------------------------
// GetDomainIdL
// Returns the domain ID from the last embedded domain RO, to be used for
// automatically joining a domain
// -----------------------------------------------------------------------------
//
LOCAL_C HBufC* GetDomainIdL(
    COma2Dcf* aDcf)
    {
    HBufC8* id = NULL;
    HBufC* r = NULL;
    CDrmProtectedRoParser* parser = NULL;

    if (aDcf != NULL && aDcf->iRightsObjects.Count() > 0)
        {
        parser = CDrmProtectedRoParser::NewL();
        CleanupStack::PushL(parser);
        id = parser->GetDomainIdL(*aDcf->iRightsObjects[ 0 ]);
        if (id != NULL)
            {
            CleanupStack::PushL(id);
            r = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*id);
            CleanupStack::PopAndDestroy(); // id
            }
        CleanupStack::PopAndDestroy(); // parser
        }
    return r;
    }

// -----------------------------------------------------------------------------
// CheckIntentL
// Check the intent for a content ID via the rights server
// -----------------------------------------------------------------------------
//
LOCAL_C TInt CheckIntentL(
    const TDesC8& aContentId,
    TIntent aIntent,
    RDRMRightsClient* aRightsClient)
    {
    RDRMRightsClient client;
    TInt r;
    TBool retVal = EFalse;
    TUint32 reason = 0;

    if (aRightsClient == NULL)
        {
        User::LeaveIfError(client.Connect());
        r = client.CheckRights(aIntent, aContentId, reason);
        client.Close();
        }
    else
        {
        r = aRightsClient->CheckRights(aIntent, aContentId, reason);
        }

    // If the rights exist return ETrue
    if( r == KErrNone )
        {
        retVal = ETrue;
        }

    return retVal;
    }

// -----------------------------------------------------------------------------
// GetEstimatedArrivalTime
// Get the estimated arrival time for the rights (value of the
// X-OMA-Separate-Delivery header)
// -----------------------------------------------------------------------------
//
LOCAL_C TInt GetEstimatedArrivalTime(
    const TDesC8& aContentId,
    RDRMRightsClient* aRightsClient)
    {
    RDRMRightsClient client;
    TInt r;
    TTimeIntervalSeconds n;

    if (aRightsClient == NULL)
        {
        r = client.Connect();
        if (r != KErrNone)
            {
            return r;
            }

        r = client.GetEstimatedArrival(aContentId, n);
        client.Close();
        }
    else
        {
        r = aRightsClient->GetEstimatedArrival(aContentId, n);
        }
    if (r == KErrNone)
        {
        r = n.Int();

        // -1 is the special case when the arrival time has passed,
        // the first caller gets the KErrCAPendingRights meaning
        // the rights are still pending
        if( r == -1 )
            {
            r = KErrCAPendingRights;
            }
        }

    return r;
    }


// -----------------------------------------------------------------------------
// GetContentName
// Get the content name stored into the rights database
// -----------------------------------------------------------------------------
//

LOCAL_C TInt GetContentName(
    const TDesC8& aContentId,
    HBufC*& aContentName,
    RDRMRightsClient* aRightsClient )
    {
    RDRMRightsClient client;
    TInt r;

    if (aRightsClient == NULL)
        {
        r = client.Connect();
        if( !r )
            {
            r = client.GetName(aContentId, aContentName);
            }
        client.Close();
        }
    else
        {
        r = aRightsClient->GetName(aContentId, aContentName);
        }

    return r;
    };


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TOma2AgentAttributes::
//
// -----------------------------------------------------------------------------
//
TInt TOma2AgentAttributes::GetAttribute(
    CDcfCommon& aDcfFile,
    TInt aAttribute,
    const TVirtualPathPtr& aVirtualPath,
    RDRMRightsClient* aRightsClient)
    {
    RDRMRightsClient client;
    TInt value = KErrCANotSupported;
    COma2Dcf* dcf2 = NULL;
    TBool protection = ETrue;
    HBufC8* buffer;
    TInt traperror = KErrNone;

    if (aDcfFile.iVersion == 2)
        {
        dcf2 = static_cast<COma2Dcf*>(&aDcfFile);
        if (dcf2->iEncryptionMethod == EMethodNULL)
            {
            protection = EFalse;
            }
        }
    if(aVirtualPath.UniqueId().Length() == 0)
        {
        // The DCF file itself
        switch(aAttribute)
            {
            case DRM::EDrmAgentUid:
                value = DRM::EDrmOmaAgent;
                break;
            case EIsProtected:
                value = EFalse;
                break;
            case EIsForwardable:
                value = ETrue;
                break;
            case EIsModifyable:
                value = EFalse;
                break;
            case EIsCopyable:
                value = ETrue;
                break;
            case EContentCDataInUse:
                value = KErrCANotSupported;
                break;
            case ECopyPaste:
                value = ETrue;
                break;
            case ECanMove:
                value = ETrue;
                break;
            case ECanRename:
                value = ETrue;
                break;
            case EFileType:
            case DRM::EDrmFileType:
                value = aDcfFile.iVersion;
                break;
            case ESilentRightsType:
                value = 0;
                break;
            case EDeliveryMethod:
                if (aDcfFile.iVersion == 2)
                    {
                    value = EOmaDrm2;
                    }
                else
                    {
                    if (aDcfFile.iContentID->Left(4).Compare(_L8("flk:")) == 0)
                        {
                        if (aRightsClient == NULL)
                            {
                            value = client.Connect();
                            if (value != KErrNone)
                                {
                                break;
                                }
                            if (client.ForwardLockURI(buffer) == KErrNone &&
                                buffer != NULL)
                                {
                                if (aDcfFile.iContentID->Compare(*buffer) == 0)
                                    {
                                    value = EOmaDrm1ForwardLock;
                                    }
                                else
                                    {
                                    value = EOmaDrm1CombinedDelivery;
                                    }
                                delete buffer;
                                }
                            else
                                {
                                value = KErrGeneral;
                                }
                            client.Close();
                            break;
                            }
                        else
                            {
                            if (aRightsClient->ForwardLockURI(buffer) == KErrNone &&
                            buffer != NULL)
                                {
                                if (aDcfFile.iContentID->Compare(*buffer) == 0)
                                    {
                                    value = EOmaDrm1ForwardLock;
                                    }
                                else
                                    {
                                    value = EOmaDrm1CombinedDelivery;
                                    }
                                delete buffer;
                                }
                            else
                                {
                                value = KErrGeneral;
                                }
                            }
                        }
                    else if (aDcfFile.iContentID->Left(4).Compare(_L8("ldf:")) == 0)
                        {
                        value = EOmaDrm1LocalDataFile;
                        }
                    else
                        {
                        value = EOmaDrm1SeparateDelivery;
                        }
                    }
                break;
            case DRM::EDrmAllowedOutputs:
                if( dcf2 )
                    {
                    value = DRM::EDrmAllowAudioAnalog | 
                            DRM::EDrmAllowAudioBluetooth | 
                            DRM::EDrmAllowVideoMacroVision | 
                            DRM::EDrmAllowAudioFmTransmitter | 
                            DRM::EDrmAllowAudioUsb |
                            DRM::EDrmAllowAudioHdmiHdcpRequired |
                            DRM::EDrmAllowVideoHdmiHdcpRequired;    
                    }
                else
                    {
                    value = KAllowAllDefined;
                    }
                break;
            default:
                value = KErrCANotSupported;
                break;
            }
        }
    else
        {
        switch(aAttribute)
            {
            case DRM::EDrmAgentUid:
                value = DRM::EDrmOmaAgent;
                break;
            case EIsProtected:
                value = protection;
                break;
            case EIsForwardable:
                if ((aDcfFile.iContentID->Left(4).Compare(_L8("flk:")) == 0)
                    ||
                    (aDcfFile.iContentID->Left(4).Compare(_L8("ldf:")) == 0))
                    {
                    value = EFalse;
                    }
                else
                    {
                    if (aDcfFile.iRightsIssuerURL == NULL )
                        {
                        value = EFalse;
                        }
                     else
                        {
                        value = ETrue;
                        }
                    }
                break;
            case EIsModifyable:
                value = protection ? EFalse : ETrue;
                break;
            case EIsCopyable:
                value = protection ? EFalse : ETrue;
                break;
            case ECanPlay:
                if (!protection)
                    {
                    value = ETrue;
                    }
                else
                    {

                    TRAP(traperror, value = CheckIntentL(*aDcfFile.iContentID, EPlay,
                        aRightsClient));
                    if( traperror )
                        {
                        value = traperror;
                        }
                    }
                break;
            case ECanPrint:
                if (!protection)
                    {
                    value = ETrue;
                    }
                else
                    {
                    TRAP(traperror, value = CheckIntentL(*aDcfFile.iContentID, EPrint,
                        aRightsClient));
                    if( traperror )
                        {
                        value = traperror;
                        }
                    }
                break;
            case ECanExecute:
                if (!protection)
                    {
                    value = ETrue;
                    }
                else
                    {
                    TRAP(traperror, value = CheckIntentL(*aDcfFile.iContentID, EExecute,
                        aRightsClient));
                    if( traperror )
                        {
                        value = traperror;
                        }
                    }
                break;
            case ECanView:
                if (!protection)
                    {
                    value = ETrue;
                    }
                else
                    {
                    TRAP(traperror, value = CheckIntentL(*aDcfFile.iContentID, EView,
                        aRightsClient));
                    if( traperror )
                        {
                        value = traperror;
                        }
                    }
                break;
            case EPreviewAvailable:
                if (dcf2 != NULL && dcf2->iPreview != ENoPreview)
                    {
                    value = ETrue;
                    }
                else
                    {
                    value = EFalse;
                    }
                break;
            case EPreviewType:
                value = ENoPreview;
                if ( dcf2 != NULL && dcf2->iSilentRefresh == ENoSilentRefresh )
                    {
                    value = dcf2->iPreview;
                    }
                else if ( dcf2 != NULL && dcf2->iPreviewOverridesSilentRefresh )
                    {
                    value = dcf2->iPreview;
                    }
                break;
            case ESilentRightsType:
                value = ENoSilentRefresh;
                if( dcf2 != NULL && dcf2->iPreview == ENoPreview )
                    {
                    value = dcf2->iSilentRefresh;
                    }
                else if (dcf2 != NULL && !dcf2->iPreviewOverridesSilentRefresh)
                    {
                    value = dcf2->iSilentRefresh;
                    }
                break;
            case EContentCDataInUse:
                value = KErrCANotSupported;
                break;
            case ECanRewind:
                value = ETrue;
                break;
            case ECopyPaste:
                value = protection;
                break;
            case ECanMove:
                value = ETrue;
                break;
            case ECanRename:
                value = ETrue;
                break;
            case ECanAutomaticConsume:
                value = ETrue;
                break;
            case EFileType:
            case DRM::EDrmFileType:
                value = aDcfFile.iVersion;
                break;
            case EDeliveryMethod:
                if (aDcfFile.iVersion == 2)
                    {
                    value = EOmaDrm2;
                    }
                else
                    {
                    if (aDcfFile.iContentID->Left(4).Compare(_L8("flk:")) == 0)
                        {
                        if (aRightsClient == NULL)
                            {
                            value = client.Connect();
                            if (value != KErrNone)
                                {
                                break;
                                }
                            if (client.ForwardLockURI(buffer) == KErrNone &&
                                buffer != NULL)
                                {
                                if (aDcfFile.iContentID->Compare(*buffer) == 0)
                                    {
                                    value = EOmaDrm1ForwardLock;
                                    }
                                else
                                    {
                                    value = EOmaDrm1CombinedDelivery;
                                    }
                                delete buffer;
                                }
                            else
                                {
                                value = KErrGeneral;
                                }
                            client.Close();
                            break;
                            }
                        else
                            {
                            if (aRightsClient->ForwardLockURI(buffer) == KErrNone &&
                            buffer != NULL)
                                {
                                if (aDcfFile.iContentID->Compare(*buffer) == 0)
                                    {
                                    value = EOmaDrm1ForwardLock;
                                    }
                                else
                                    {
                                    value = EOmaDrm1CombinedDelivery;
                                    }
                                delete buffer;
                                }
                            else
                                {
                                value = KErrGeneral;
                                }
                            }
                        }
                    else if (aDcfFile.iContentID->Left(4).Compare(_L8("ldf:")) == 0)
                        {
                        value = EOmaDrm1LocalDataFile;
                        }
                    else
                        {
                        value = EOmaDrm1SeparateDelivery;
                        }
                    }
                break;
            case ERecordingYear:
                value = KErrNotSupported;
                 if (dcf2 != NULL && dcf2->iRecordingYear )
                     {
                     value = dcf2->iRecordingYear;
                     }
                break;
            case DRM::EDrmAllowedOutputs:
                if( dcf2 )
                    {
                    value = DRM::EDrmAllowAudioAnalog | 
                            DRM::EDrmAllowAudioBluetooth | 
                            DRM::EDrmAllowVideoMacroVision | 
                            DRM::EDrmAllowAudioFmTransmitter | 
                            DRM::EDrmAllowAudioUsb |
                            DRM::EDrmAllowAudioHdmiHdcpRequired |
                            DRM::EDrmAllowVideoHdmiHdcpRequired;    
                    }
                else
                    {
                    value = KAllowAllDefined;
                    }
                break;
            case ERightsNone:
                if (aRightsClient == NULL)
                    {
                    value = client.Connect();
                    if (value != KErrCANoRights)
                        {
                        break;
                        }
                    TUint32 reason = 0;
                    value = client.CheckRights(EPeek, *aDcfFile.iContentID, reason);
                    if(value == KErrCANoRights  || KErrCANoPermission)
                        {
                        value = ETrue;
                        }
                    client.Close();
                    }
                else
                    {
                    TUint32 reason = 0;
                    value = aRightsClient->CheckRights(EPeek, *aDcfFile.iContentID, reason);
                    if(value == KErrCANoRights  || KErrCANoPermission)
                        {
                        value = ETrue;
                        }
                    }
                break;
            case ERightsHaveExpired:
                if (aRightsClient == NULL)
                    {
                    value = client.Connect();
                    if (value != KErrNone)
                        {
                        break;
                        }
                    value = ETrue;
                    TUint32 reason = 0;
                    value = client.CheckRights(EPlay, *aDcfFile.iContentID, reason);
                    if(value != KErrCANoRights  || KErrCANoPermission)
                        {
                        value = EFalse;
                        }
                    value = client.CheckRights(EView, *aDcfFile.iContentID, reason);
                    if(value != KErrCANoRights  || KErrCANoPermission)
                        {
                        value = EFalse;
                        }
                    value = client.CheckRights(EExecute, *aDcfFile.iContentID, reason);
                    if(value != KErrCANoRights  || KErrCANoPermission)
                        {
                        value = EFalse;
                        }
                    value = client.CheckRights(EPrint, *aDcfFile.iContentID, reason);
                    if(value != KErrCANoRights  || KErrCANoPermission)
                        {
                        value = EFalse;
                        }
                    client.Close();
                    }
                else
                    {
                    value = ETrue;
                    TUint32 reason = 0;
                    value = aRightsClient->CheckRights(EPlay, *aDcfFile.iContentID, reason);
                    if(value != KErrCANoRights  || KErrCANoPermission)
                        {
                        value = EFalse;
                        }
                    value = aRightsClient->CheckRights(EView, *aDcfFile.iContentID, reason);
                    if(value != KErrCANoRights  || KErrCANoPermission)
                        {
                        value = EFalse;
                        }
                    value = aRightsClient->CheckRights(EExecute, *aDcfFile.iContentID, reason);
                    if(value != KErrCANoRights  || KErrCANoPermission)
                        {
                        value = EFalse;
                        }
                    value = aRightsClient->CheckRights(EPrint, *aDcfFile.iContentID, reason);
                    if(value != KErrCANoRights  || KErrCANoPermission)
                        {
                        value = EFalse;
                        }

                    }
                break;
            default:
                value = KErrCANotSupported;
                break;
            };
        }
    return value;
    }

// -----------------------------------------------------------------------------
// TOma2AgentAttributes::
//
// -----------------------------------------------------------------------------
//
TInt TOma2AgentAttributes::GetAttributeSet(
    CDcfCommon& aDcfFile,
    RAttributeSet& aAttributeSet,
    const TVirtualPathPtr& aVirtualPath,
    RDRMRightsClient* aRightsClient)
    {
    TInt i = 0;
    TInt attribute = 0;
    TInt value = 0;
    TInt numAttributes = aAttributeSet.Count();

    // loop through all the attriutes in the set and find their values
    for(i = 0; i < numAttributes; i++)
        {
        attribute = aAttributeSet[i];
        value = GetAttribute(aDcfFile, attribute, aVirtualPath, aRightsClient);
        aAttributeSet.SetValue(attribute, value, KErrNone);
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// TOma2AgentAttributes::
//
// -----------------------------------------------------------------------------
//
TInt TOma2AgentAttributes::GetStringAttribute(
    CDcfCommon& aDcfFile,
    TInt aAttribute,
    TDes& aValue,
    const TVirtualPathPtr& aVirtualPath,
    RDRMRightsClient* aRightsClient)
    {
    TInt err = KErrCANotSupported;
    TInt ret = KErrNone;
    HBufC* b = NULL;
    COma1Dcf* dcf1 = NULL;
    COma2Dcf* dcf2 = NULL;
    TPtrC8 ptr(NULL, 0);
    TPtr ptr16(NULL, 0, 0);
    TInt n;
    HBufC* contentName = NULL;
    TFileName fileName;

    aValue.Copy(KNullDesC);
    if (aDcfFile.iVersion == 2)
        {
        dcf2 = static_cast<COma2Dcf*>(&aDcfFile);
        }
    else
        {
        dcf1 = static_cast<COma1Dcf*>(&aDcfFile);
        }
    if (aVirtualPath.UniqueId().Length() == 0)
        {
        switch (aAttribute)
            {
            case DRM::EDrmAgentName:
                TRAP(err, b = DRM::KDrmOmaAgentName().AllocL());
                break;
            case EMimeType:
                if (aDcfFile.iVersion == 1)
                    {
                    TRAP(err, b = KOma1DcfContentType16().AllocL());
                    }
                else if (aDcfFile.iVersion == 2)
                    {
                    TRAP(err, b = KOma2DcfContentType16().AllocL());
                    }
                break;
            case ETransactionTrackingId:
                if (dcf2 != NULL && dcf2->iTransactionTracking)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iTransactionTracking));
                    }
                break;
            case EEmbeddedRightsObject:
                if (dcf2 != NULL)
                    {
                    TRAP(err, b = CollectEmbeddedRosL(dcf2));
                    }
                break;
            case EDomainRightsIssuerUrl:
                if (dcf2 != NULL)
                    {
                    TRAP(err, b = GetDomainRiUrlL(dcf2));
                    }
                break;
            case EDomainRightsIssuerId:
                if (dcf2 != NULL)
                    {
                    TRAP(err, b = GetDomainRiIdL(dcf2));
                    }
                break;
            case EDomainId:
                if (dcf2 != NULL)
                    {
                    TRAP(err, b = GetDomainIdL(dcf2));
                    }
                break;
            case EFileName:
            case DRM::EDrmFileName:
                err = aDcfFile.iFile.Name(fileName);
                if( aValue.MaxLength() < fileName.Length() )
                    {
                    return KErrOverflow;
                    }
                aValue.Copy(fileName);
                break;
            case EContentName:
            case EDescription:
            case ETitle:
            case EAuthor:
            case EIconURI:
            case EPreviewURI:
            case EInstantPreviewUri:
            case EPreviewRightsUri:
            case EContentURI:
            case EContentID:
            case EInfoURL:
            case EPendingRightsETA:
            case ERightsIssuerUrl:
            case ESilentRightsUrl:
            default:
                break;
            }
        }
    else
        {
        switch (aAttribute)
            {
            case DRM::EDrmAgentName:
                TRAP(err, b = DRM::KDrmOmaAgentName().AllocL());
                break;
            case EMimeType:
                TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*aDcfFile.iMimeType));
                break;
            case EDescription:
                if (aDcfFile.iDescription)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*aDcfFile.iDescription));
                    }
                break;
            case ETitle:
                if (aDcfFile.iTitle)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*aDcfFile.iTitle));
                    }
                break;
            case EAuthor:
                if (dcf2 != NULL && dcf2->iAuthor)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iAuthor));
                    }
                break;
            case EIconURI:
                if (aDcfFile.iIconUri)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*aDcfFile.iIconUri));
                    }
                break;
            case EPreviewURI:
                if (dcf2 != NULL && dcf2->iPreviewParameter)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iPreviewParameter));
                    }
                break;
            case EContentURI:
                if (dcf2 != NULL && dcf2->iContentUrl)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iContentUrl));
                    }
                break;
            case EContentID:
                TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*aDcfFile.iContentID));
                break;
            case EInfoURL:
                if (dcf2 != NULL && dcf2->iInfoUri)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iInfoUri));
                    }
                break;
            case EInstantPreviewUri:
                if (dcf2 != NULL && dcf2->iPreview == EInstantPreview &&
                    dcf2->iPreviewParameter)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iPreviewParameter));
                    }
                break;
            case EPreviewRightsUri:
                if (dcf2 != NULL && dcf2->iPreview == EPreviewRights &&
                    dcf2->iPreviewParameter)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iPreviewParameter));
                    }
                break;
            case ESilentRightsUrl:
                if (dcf2 != NULL && dcf2->iSilentParameter &&
                    dcf2->iSilentParameter)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iSilentParameter));
                    }
                break;
            case ERightsIssuerUrl:
#ifndef __DRM_FULL
                err = KErrNotSupported;
#else
                if (aDcfFile.iRightsIssuerURL != NULL)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*aDcfFile.iRightsIssuerURL));
                    }
#endif
                break;
            case ETransactionTrackingId:
                if (dcf2 != NULL && dcf2->iTransactionTracking)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iTransactionTracking));
                    }
                break;
            case EEmbeddedRightsObject:
                if (dcf2 != NULL)
                    {
                    TRAP(err, b = CollectEmbeddedRosL(dcf2));
                    }
                break;
            case EDomainRightsIssuerUrl:
                if (dcf2 != NULL)
                    {
                    TRAP(err, b = GetDomainRiUrlL(dcf2));
                    }
                break;
            case EDomainRightsIssuerId:
                if (dcf2 != NULL)
                    {
                    TRAP(err, b = GetDomainRiIdL(dcf2));
                    }
                break;
            case EDomainId:
                if (dcf2 != NULL)
                    {
                    TRAP(err, b = GetDomainIdL(dcf2));
                    }
                break;
            case EContentVendor:
                if (dcf1 != NULL)
                    {
                    TRAP(err, dcf1->GetHeaderL(KContentVendor, ptr));
                    if (ptr.Length() > 0 && err == KErrNone)
                        {
                        TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(ptr));
                        }
                    }
                break;
            case ECopyright:
                if (dcf2 != NULL && dcf2->iCopyRight)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iCopyRight));
                    }
                break;
            case EPendingRightsETA:
                n = GetEstimatedArrivalTime(*aDcfFile.iContentID, aRightsClient);
                TRAP(err, b = HBufC::NewL(16));
                if( err == KErrNone )
                    {
                    ptr16.Set(b->Des());
                    ptr16.AppendNum(n);
                    }
                break;
            case EContentName:
                contentName = NULL;
                err = GetContentName( *aDcfFile.iContentID, contentName, aRightsClient );
                if( !err )
                    {
                    b = contentName;
                    }
                break;

            case EOmaDrmContentLocation:
                break;
            case EOmaDrmContentVersion:
                break;
            case EPerformer:
            case DRM::EDrmOriginalArtist:
                 if (dcf2 != NULL && dcf2->iPerformer)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iPerformer));
                    }
                break;
            case EGenre: // flow through
            case EOmaDrmGenre:
                 if (dcf2 != NULL && dcf2->iGenre)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iGenre));
                    }
                break;
            case ERatingInfo:
            case DRM::EDrmRatingInfo:
                 if (dcf2 != NULL && dcf2->iRatingInfo)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iRatingInfo));
                    }
                break;
            case EClassificationInfo:
                 if (dcf2 != NULL && dcf2->iClassificationInfo)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iClassificationInfo));
                    }
                break;
            case EKeyword:
                 if (dcf2 != NULL && dcf2->iKeyword)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iKeyword));
                    }
                break;
            case ELocInfoName:
                 if (dcf2 != NULL && dcf2->iLocInfoName)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iLocInfoName));
                    }
                break;
            case ELocInfoAstronomicalBody:
                 if (dcf2 != NULL && dcf2->iLocInfoAstronomicalBody)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iLocInfoAstronomicalBody));
                    }
                break;
            case ELocInfoAdditionalNotes:
                 if (dcf2 != NULL && dcf2->iLocInfoAdditionalNotes)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iLocInfoAdditionalNotes));
                    }
                break;
            case EAlbumTitle:
                 if (dcf2 != NULL && dcf2->iAlbumTitle)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iAlbumTitle));
                    }
                break;
            case EAlbumTrack:
            case DRM::EDrmTrackNumber:
                if( dcf2 != NULL && dcf2->iAlbumTrack)
                    {
                    TRAP(err, b = HBufC::NewL(KMaxAlbumTrack));
                    if( err == KErrNone )
                        {
                        b->Des().AppendNum( dcf2->iAlbumTrack );
                        }
                    }
                break;
            case ECoverUri:
                 if (dcf2 != NULL && dcf2->iCoverUri)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iCoverUri));
                    }
                break;
            case ELyricsUrl:
                 if (dcf2 != NULL && dcf2->iLyricsURL)
                    {
                    TRAP(err, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*dcf2->iLyricsURL));
                    }
                break;
            case EFileName:
            case DRM::EDrmFileName:
                err = aDcfFile.iFile.Name(fileName);
                if( aValue.MaxLength() < fileName.Length() )
                    {
                    return KErrOverflow;
                    }
                aValue.Copy(fileName);
                break;
            case DRM::EDrmFullName:
                err = aDcfFile.iFile.FullName(fileName);
                if ( aValue.MaxLength() < fileName.Length() )
                    {
                    return KErrOverflow;
                    }
                aValue.Copy(fileName);
                break;
            case DRM::EDrmRecordingYear:
                if( dcf2 != NULL && dcf2->iRecordingYear)
                    {
                    TRAP(err, b = HBufC::NewL(KMaxRecordingYear));
                    if( err == KErrNone )
                        {
                        b->Des().AppendNum( dcf2->iRecordingYear );
                        }
                    }
                break;
            default:
                break;
            }
        }


    if (b != NULL)
        {
        if( b->Length() > aValue.MaxLength() )
            {
            delete b;
            return KErrOverflow;
            }
        err = KErrNone;
        aValue.Copy(*b);
        delete b;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// TOma2AgentAttributes::
//
// -----------------------------------------------------------------------------
//
TInt TOma2AgentAttributes::GetStringAttributeSet(
    CDcfCommon& aDcfFile,
    RStringAttributeSet& aStringAttributeSet,
    const TVirtualPathPtr& aVirtualPath,
    RDRMRightsClient* aRightsClient)
    {
    TInt i = 0;
    TInt attribute = 0;
    TInt err = KErrNone;
    TBuf<KMaxDataTypeLength> buf;
    TInt numAttributes = aStringAttributeSet.Count();

    for(i = 0; err == KErrNone && i < numAttributes; i++)
        {
        attribute = aStringAttributeSet[i];
        err = GetStringAttribute(aDcfFile, attribute, buf, aVirtualPath,
            aRightsClient);
        err = aStringAttributeSet.SetValue(attribute, buf, err);
        }
    return err;
    }

// End of file
