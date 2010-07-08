/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: OMA DRM ContentAccess::CContent implementation
*
*/



// INCLUDE FILES
#include <caf/caf.h>
#include <caf/cafplatform.h>
#include <caf/bitset.h>
#include <caf/attribute.h>
#include <e32test.h>
#include <utf.h>

#include <drmbrowserlauncher.h>

#include "Oma2AgentContent.h"
#include "Oma2AgentAttributes.h"
#include "Oma1Dcf.h"
#include "oma2dcf.h"
#include "symmetric.h"
#include "DRMRights.h"
#include "Oma2DcfPartInfo.h"
#include "DRMProtectedRoParser.h"

using namespace ContentAccess;

// LOCAL FUNCTION PROTOTYPES
LOCAL_C TInt MapContentShareMode(TContentShareMode aMode);
template<class S>
LOCAL_C void PointerArrayResetDestroyAndClose(TAny* aPtr);

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// MapContentShareMode
// Maps the CAF specific file share mode to the RFs/RFile sharing mode
// -----------------------------------------------------------------------------
//
TInt MapContentShareMode(TContentShareMode aMode)
    {
    TInt r = EFileRead | EFileShareAny;

    switch (aMode)
        {
        case EContentShareReadOnly:
            r = EFileRead | EFileShareReadersOnly;
            break;
        case EContentShareReadWrite:
            r = EFileRead | EFileShareReadersOrWriters;
            break;
        case EContentShareExclusive:
            r = EFileRead | EFileShareExclusive;
            break;
        }
    return r;
    }

// -----------------------------------------------------------------------------
// PointerArrayResetDestroyAndClose
// Template method used to push RPointerArrays to the cleanup stack. Takes
// care of deleting all pointers in the array.
// -----------------------------------------------------------------------------
//
template<class S>
void PointerArrayResetDestroyAndClose(TAny* aPtr)
    {
    (reinterpret_cast<RPointerArray<S>*>(aPtr))->ResetAndDestroy();
    (reinterpret_cast<RPointerArray<S>*>(aPtr))->Close();
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// COma2AgentContent::COma2AgentContent
// Reset all member variables
// -----------------------------------------------------------------------------
//
COma2AgentContent::COma2AgentContent(void):
    iDcf(NULL),
    iFilePosition(0),
    iDataPosition(0),
    iUri(NULL),
    iCurrentContainer(NULL),
    iNotifier(NULL),
    iWatchedId(NULL)
    {
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::ConstructL
// Initialize the agent from a URI (file name) and file mode, opens the
// the file and takes membership of the handle
// -----------------------------------------------------------------------------
//
void COma2AgentContent::ConstructL(
    const TDesC& aUri,
    TContentShareMode aShareMode)
    {
    iUri = aUri.AllocL();
    User::LeaveIfError(iRdb.Connect());
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFs.ShareAuto());
    User::LeaveIfError(iFile.Open(iFs, aUri, MapContentShareMode(aShareMode)));
    iDcf = CDcfCommon::NewL(iFile);
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::ConstructL
// Initialize the agent from a file handle, duplicates the handle
// -----------------------------------------------------------------------------
//
void COma2AgentContent::ConstructL(RFile& aFile)
    {
    TFileName name;
    User::LeaveIfError(iRdb.Connect());
    User::LeaveIfError(iFile.Duplicate(aFile));
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFs.ShareAuto());
    iFile.Name(name);
    iUri = name.AllocL();
    iDcf = CDcfCommon::NewL(iFile);
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::NewL
// Two-phased constructors
// -----------------------------------------------------------------------------
//
COma2AgentContent* COma2AgentContent::NewL(
    const TDesC& aUri,
    TContentShareMode aShareMode)
    {
    COma2AgentContent* self = NewLC(aUri, aShareMode);
    CleanupStack::Pop(self);
    return self;
    }

COma2AgentContent* COma2AgentContent::NewLC(
    const TDesC& aUri,
    TContentShareMode aShareMode)
    {
    COma2AgentContent* self=new(ELeave) COma2AgentContent();
    CleanupStack::PushL(self);
    self->ConstructL(aUri, aShareMode);
    return self;
    }

COma2AgentContent* COma2AgentContent::NewL(RFile& aFile)
    {
    COma2AgentContent* self = NewLC(aFile);
    CleanupStack::Pop(self);
    return self;
    }

COma2AgentContent* COma2AgentContent::NewLC(RFile& aFile)
    {
    COma2AgentContent* self=new(ELeave) COma2AgentContent();
    CleanupStack::PushL(self);
    self->ConstructL(aFile);
    return self;
    }


// Destructor
COma2AgentContent::~COma2AgentContent()
    {
    iRdb.Close();
    iFile.Close();
    iFs.Close();
    delete iDcf;
    delete iUri;
    delete iCurrentContainer;
    if (iNotifier != NULL)
        {
        if( iWatchedId )
            {

            TRAP_IGNORE( iNotifier->UnRegisterEventObserverL(*this, KEventAddRemove, *iWatchedId) );
            TRAP_IGNORE( iNotifier->UnRegisterEventObserverL(*this, KEventModify, *iWatchedId) );

            }

        delete iNotifier;
        }
    delete iWatchedId;
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::OpenContainer
// Checks if the part ID is valid and sets the current ID to the part ID
// -----------------------------------------------------------------------------
//
TInt COma2AgentContent::OpenContainer(
    const TDesC& aUniqueId)
    {
    TInt r = KErrNotFound;
    TInt err = KErrNone;

    // Only the outermost file can be opened, parts within the file don't have
    // subparts, only attributes.
    if (iCurrentContainer == NULL)
        {
        r = iDcf->CheckUniqueId(aUniqueId);
        if (r == KErrNone)
            {

            TRAP(err, iCurrentContainer = aUniqueId.AllocL() );
            if(err)
                {
                return err;
                }
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::CloseContainer
// Resets the current part ID
// -----------------------------------------------------------------------------
//
TInt COma2AgentContent::CloseContainer()
    {
    TInt r = KErrNone;

    if (iCurrentContainer != NULL)
        {
        delete iCurrentContainer;
        iCurrentContainer = NULL;
        }
    else
        {
        r = KErrNotFound;
        }
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::GetEmbeddedObjectsL
// Returns all parts embedded in the current part
// -----------------------------------------------------------------------------
//
void COma2AgentContent::GetEmbeddedObjectsL(
    RStreamablePtrArray<CEmbeddedObject>& aArray)
    {
    TInt i;
    CEmbeddedObject* part = NULL;
    HBufC* id = NULL;
    COma2Dcf* dcf2 = NULL;

    aArray.ResetAndDestroy();
    if (iCurrentContainer == NULL)
        {
        if (iDcf->iVersion == EOma2Dcf)
            {
            dcf2 = static_cast<COma2Dcf*>(iDcf);
            for (i = 0; i < dcf2->iParts.Count(); i++)
                {
                id = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
                    *dcf2->iParts[i]->iContentId);
                CleanupStack::PushL(id);
                part = CEmbeddedObject::NewL(*id, *dcf2->iParts[i]->iMimeType,
                    EContentObject);
                CleanupStack::PopAndDestroy(id);
                aArray.AppendL(part);
                }
            }
        else
            {
            id = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
                *iDcf->iContentID);
            CleanupStack::PushL(id);
            part = CEmbeddedObject::NewL(*id, *iDcf->iMimeType,
                EContentObject);
            CleanupStack::PopAndDestroy(id);
            aArray.AppendL(part);
            }
        }
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::GetEmbeddedObjectsL
// Returns all parts embedded in the current part, filtered by type
// -----------------------------------------------------------------------------
//
void COma2AgentContent::GetEmbeddedObjectsL(
    RStreamablePtrArray<CEmbeddedObject>& aArray,
    TEmbeddedType aType)
    {
    if (aType == EContentObject)
        {
        GetEmbeddedObjectsL(aArray);
        }
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::Search
//
// -----------------------------------------------------------------------------
//
TInt COma2AgentContent::Search(
    RStreamablePtrArray<CEmbeddedObject>& aArray,
    const TDesC8& aMimeType,
    TBool /*aRecursive*/)
    {
    TInt i;
    TInt r = KErrNone;
    RStreamablePtrArray<CEmbeddedObject> parts;
    CEmbeddedObject* item = NULL;

    TRAP(r, GetEmbeddedObjectsL(parts));
    for (i = 0; r == KErrNone && i < parts.Count(); i++)
        {
        if (parts[i]->MimeType().CompareF(aMimeType) == 0)
            {

            TRAP_IGNORE( item = CEmbeddedObject::NewL(
                parts[i]->UniqueId(), parts[i]->Name(),
                parts[i]->MimeType(), parts[i]->Type()) );

            TRAP(r, aArray.AppendL(item));
            }
        }
    parts.ResetAndDestroy();
    parts.Close();
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::GetAttribute
//
// -----------------------------------------------------------------------------
//
TInt COma2AgentContent::GetAttribute(
    TInt aAttribute,
    TInt& aValue,
    const TDesC& aUniqueId)
    {
    TInt r;

    r = iDcf->OpenPart(aUniqueId);
    if (r == KErrNone)
        {
        TRAP(r,aValue = TOma2AgentAttributes::GetAttribute(*iDcf, aAttribute,
            TVirtualPathPtr(*iUri, aUniqueId), &iRdb));
        }
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::GetAttributeSet
//
// -----------------------------------------------------------------------------
//
TInt COma2AgentContent::GetAttributeSet(
    RAttributeSet& aAttributeSet,
    const TDesC& aUniqueId)
    {
    TInt r;
    TInt err = KErrNone;

    r = iDcf->OpenPart(aUniqueId);
    if (r == KErrNone)
        {
        TRAP(err,r = TOma2AgentAttributes::GetAttributeSet(*iDcf, aAttributeSet,
            TVirtualPathPtr(*iUri, aUniqueId), &iRdb));
        if (err != KErrNone)
            {
            return err;
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::GetStringAttribute
//
// -----------------------------------------------------------------------------
//
TInt COma2AgentContent::GetStringAttribute(
    TInt aAttribute,
    TDes& aValue,
    const TDesC& aUniqueId)
    {
    TInt r = KErrNone;
    TInt err = KErrNone;

    r = iDcf->OpenPart(aUniqueId);
    if (r == KErrNone)
        {
        TRAP(err, r = TOma2AgentAttributes::GetStringAttribute(*iDcf, aAttribute,
            aValue, TVirtualPathPtr(*iUri, aUniqueId), &iRdb));
        if(err != KErrNone)
            {
            return err;
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::GetStringAttributeSet
//
// -----------------------------------------------------------------------------
//
TInt COma2AgentContent::GetStringAttributeSet(
    RStringAttributeSet& aStringAttributeSet,
    const TDesC& aUniqueId)
    {
    TInt r;
    TInt err = KErrNone;

    r = iDcf->OpenPart(aUniqueId);
    if (r == KErrNone)
        {
        TRAP(err, r = TOma2AgentAttributes::GetStringAttributeSet(*iDcf,
            aStringAttributeSet, TVirtualPathPtr(*iUri, aUniqueId), &iRdb));
        if(err != KErrNone)
            {
            return err;
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::AgentSpecificCommand
//
// -----------------------------------------------------------------------------
//
TInt COma2AgentContent::AgentSpecificCommand(
    TInt aCommand,
    const TDesC8& /*aInputBuffer*/,
    TDes8& /*aOutputBuffer*/)
    {
    TInt r = KErrCANotSupported;

    if (aCommand == EEmbedDomainRo)
        {
        TRAP(r, EmbedDomainRoL());
        }
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::AgentSpecificCommand
//
// -----------------------------------------------------------------------------
//
void COma2AgentContent::AgentSpecificCommand(
    TInt aCommand,
    const TDesC8& aInputBuffer,
    TDes8& aOutputBuffer,
    TRequestStatus& aStatus)
    {
    TRequestStatus *ptr = &aStatus;
    User::RequestComplete(ptr, AgentSpecificCommand(aCommand, aInputBuffer,
        aOutputBuffer));
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::NotifyStatusChange
//
// -----------------------------------------------------------------------------
//
void COma2AgentContent::NotifyStatusChangeL(
    TEventMask aEventMask,
    TRequestStatus& aStatus,
    const TDesC& aUniqueId)
    {
    TInt r = KErrNone;
    TInt i;
    TInt part = KErrNotFound;
    COma2Dcf* dcf2 = NULL;
    HBufC8* uniqueId = NULL;

    iStatus = &aStatus;
    iWatchedEvents = aEventMask;
    if (iWatchedId != NULL)
        {
        if( iNotifier )
            {
            iNotifier->UnRegisterEventObserverL(*this, KEventAddRemove, *iWatchedId);
            iNotifier->UnRegisterEventObserverL(*this, KEventModify, *iWatchedId);
            }
        delete iWatchedId;
        iWatchedId = NULL;
        }
    if (iDcf->iVersion == EOma1Dcf || aUniqueId.Compare(KDefaultContentObject) == 0)
        {
        iWatchedId = iDcf->iContentID->AllocL();
        }
    else
        {
        dcf2 = static_cast<COma2Dcf*>(iDcf);
        uniqueId = CnvUtfConverter::ConvertFromUnicodeToUtf8L(aUniqueId);
        CleanupStack::PushL(uniqueId);
        for (i = 0; part == KErrNotFound && i < dcf2->iParts.Count(); i++)
            {
            if (dcf2->iParts[i]->iContentId->CompareF(*uniqueId) == 0)
                {
                iWatchedId = dcf2->iParts[i]->iContentId->AllocL();
                }
            }
        CleanupStack::PopAndDestroy();
        }
    if (iWatchedId != NULL)
        {
        if (iNotifier == NULL)
            {
            iNotifier = CDRMNotifier::NewL();
            }
        if (iNotifier != NULL)
            {
            iNotifier->RegisterEventObserverL(*this, KEventAddRemove, *iWatchedId);
            iNotifier->RegisterEventObserverL(*this, KEventModify, *iWatchedId);
            *iStatus = KRequestPending;
            }
        else
            {
            User::Leave(r);
            }
        }
    else
        {
        User::Leave(KErrNotFound);
        }
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::NotifyStatusChange
// -----------------------------------------------------------------------------
//
void COma2AgentContent::NotifyStatusChange(
    TEventMask aEventMask,
    TRequestStatus& aStatus,
    const TDesC& aUniqueId)
    {
    TInt err = KErrNone;
    TRequestStatus *status = 0;
    TRAP(err, NotifyStatusChangeL(  aEventMask,
                                    aStatus,
                                    aUniqueId ));
    if(err)
        {
        status = &aStatus;
        User::RequestComplete(status, err);
        }
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::CancelNotifyStatusChange
//
// -----------------------------------------------------------------------------
//
TInt COma2AgentContent::CancelNotifyStatusChange(
    TRequestStatus& aStatus,
    const TDesC& /*aUniqueId*/)
    {
    iStatus = &aStatus;
    if(iNotifier == NULL)
         {
         User::RequestComplete(iStatus, KErrNotFound);
         return KErrNotFound;
         }

    TRAP_IGNORE( iNotifier->UnRegisterEventObserverL(*this, KEventAddRemove, *iWatchedId) );
    TRAP_IGNORE( iNotifier->UnRegisterEventObserverL(*this, KEventModify, *iWatchedId) );

    if (iWatchedId != NULL)
        {
        delete iWatchedId;
        iWatchedId = NULL;
        }
    User::RequestComplete(iStatus, KErrCancel);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::RequestRights
// Opens the browser with the rights issuer URL
// -----------------------------------------------------------------------------
//

void COma2AgentContent::RequestRights(
    TRequestStatus& aStatus,
    const TDesC& aUniqueId)
    {
    TRequestStatus *ptr = &aStatus;
    TInt r;
    HBufC* b = NULL;

    r = KErrNotSupported;

    r = iDcf->OpenPart(aUniqueId);
    if (r == KErrNone && iDcf->iRightsIssuerURL != NULL)
        {
        TRAP(r, b = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
            *iDcf->iRightsIssuerURL));
        if (b != NULL)
            {
    				DRM::CDrmBrowserLauncher* browserLauncher = DRM::CDrmBrowserLauncher::NewLC();
    	
				    browserLauncher->LaunchUrlL(*b);
    
    				CleanupStack::PopAndDestroy(); // browserLauncher
   
            delete b;
            }
        }

		User::RequestComplete(ptr, r);
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::CancelRequestRights
// Not supported, getting rights is handled outside the scope of the agent and
// cannot be cancelled
// -----------------------------------------------------------------------------
//

TInt COma2AgentContent::CancelRequestRights(
    TRequestStatus& aStatus,
    const TDesC& /*aUniqueId*/)
    {
    TRequestStatus *ptr = &aStatus;
    User::RequestComplete(ptr, KErrNone);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::DisplayInfoL
//
// -----------------------------------------------------------------------------
//
void COma2AgentContent::DisplayInfoL(
    TDisplayInfo /*aDisplayInfo*/,
    const TDesC& /*aUniqueId*/)
    {
    User::Leave(KErrCANotSupported);
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::SetProperty
//
// -----------------------------------------------------------------------------
//
TInt COma2AgentContent::SetProperty(
    TAgentProperty /*aProperty*/,
    TInt /*aValue*/)
    {
    return KErrCANotSupported;
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::EmbedDomainRoL
//
// -----------------------------------------------------------------------------
//
void COma2AgentContent::EmbedDomainRoL()
    {
    RPointerArray<HBufC8> roList;
    TCleanupItem listCleanup(PointerArrayResetDestroyAndClose<HBufC8>,
        &roList);
    COma2Dcf* dcf;
    CDrmProtectedRoParser* parser = NULL;
    RPointerArray<CDRMRights> rights;

    if (iDcf->iVersion == EOma2Dcf)
        {
        dcf = static_cast<COma2Dcf*>(iDcf);

        __UHEAP_MARK;
        for (TInt i = 0; i < dcf->iRightsObjects.Count(); i++)
            {
            parser = CDrmProtectedRoParser::NewL();
            CleanupStack::PushL(parser);
            TRAP_IGNORE( parser->ParseAndStoreL(*dcf->iRightsObjects[i],
                    rights));
            CleanupStack::PopAndDestroy(); // parser
            rights.ResetAndDestroy();
            rights.Close();
            }
        __UHEAP_MARKEND;

        iRdb.GetDomainRosForCidL(*dcf->iContentID, roList);
        CleanupStack::PushL(listCleanup);
        if(roList.Count())
            {
            // this is not replaceable in UHEAP_MARK delimiters
            dcf->SetRightsObjectsL(roList);
            }
        CleanupStack::PopAndDestroy(); // listCleanup
        }
    else
        {
        User::Leave(KErrArgument);
        }
    }

// -----------------------------------------------------------------------------
// COma2AgentContent::HandleEventL
// Handle notifier events here. Not much logic, consider all events as rights
// changes.
// -----------------------------------------------------------------------------
//
void COma2AgentContent::HandleEventL(
    MDRMEvent* /*aEvent*/)
    {
    TInt r;
    TUint32 reason = 0;

    if ( iWatchedId != NULL)
        {
        r = iRdb.CheckRights(EUnknown, *iWatchedId, reason);
        if (r == KErrNone && (iWatchedEvents & ERightsAvailable) ||
            r != KErrNone && (iWatchedEvents & ERightsExpired))
            {
            iNotifier->UnRegisterEventObserverL(*this, KEventAddRemove, *iWatchedId);
            iNotifier->UnRegisterEventObserverL(*this, KEventModify, *iWatchedId);
            delete iWatchedId;
            iWatchedId = NULL;
            User::RequestComplete(iStatus, KErrNone);
            }
        }
    }

//  End of File
