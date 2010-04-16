/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the CAF rights manager
*
*/



// INCLUDE FILES
#include <caf/caf.h>
#include <caf/cafplatform.h>
#include <utf.h>
#include "DRMRights.h"
#include "Oma2AgentRightsManager.h"
#include "DRMRightsClient.h"
#include "DcfRep.h"
#include "DcfEntry.h"

using namespace ContentAccess;

// ============================= LOCAL FUNCTIONS ===============================

void DoDeleteAllRightsObjectsL( const TVirtualPathPtr& aVirtualPath )
    {
    RDRMRightsClient client;
    CDcfCommon* dcf = NULL;
    RFs fs;

    User::LeaveIfError(client.Connect());
    CleanupClosePushL(client);
    User::LeaveIfError(fs.Connect());
    User::LeaveIfError(fs.ShareAuto());
    CleanupClosePushL(fs);
    dcf = CDcfCommon::NewL(aVirtualPath.URI(), &fs);
    CleanupStack::PushL(dcf);
    User::LeaveIfError( dcf->OpenPart(aVirtualPath.UniqueId()));
    User::LeaveIfError( client.DeleteDbEntry(*dcf->iContentID));
    CleanupStack::PopAndDestroy(3); // dcf, fs, client
    }

void DoDeleteAllRightsObjects( const TVirtualPathPtr& aVirtualPath )
    {
    TRAP_IGNORE( DoDeleteAllRightsObjectsL( aVirtualPath ) );
    }

void DoDeleteRightsObjectL(
    const CRightsInfo& aRightsInfo)
    {
    RDRMRightsClient client;
    TInt uniqueId;
    HBufC8* contentId = NULL;
    TInt n;

    User::LeaveIfError(client.Connect());
    CleanupClosePushL(client);
    n = aRightsInfo.UniqueId().Locate(0);
    if (n == KErrNotFound)
        {
        contentId = CnvUtfConverter::ConvertFromUnicodeToUtf8L(
            aRightsInfo.UniqueId());
        CleanupStack::PushL(contentId);
        User::LeaveIfError( client.DeleteDbEntry(*contentId) );
        }
    else
        {
        contentId = CnvUtfConverter::ConvertFromUnicodeToUtf8L(
            aRightsInfo.UniqueId().Left(n));
        CleanupStack::PushL(contentId);
        TLex lex(aRightsInfo.UniqueId().Right(
            aRightsInfo.UniqueId().Length() - n - 1));
        User::LeaveIfError( lex.Val(uniqueId) );
        User::LeaveIfError( client.DeleteDbEntry(*contentId, uniqueId) );
        }
    CleanupStack::PopAndDestroy(2); // contentId, client
    }

void DoDeleteRightsObject(
    const CRightsInfo& aRightsInfo)
    {
    TRAP_IGNORE( DoDeleteRightsObjectL( aRightsInfo ) );
    }


template<class S>
void PointerArrayResetDestroyAndClose(TAny* aPtr)
    {
    (reinterpret_cast<RPointerArray<S>*>(aPtr))->ResetAndDestroy();
    (reinterpret_cast<RPointerArray<S>*>(aPtr))->Close();
    }

TBool IsValid(
    CDRMPermission* aPermission,
    TIntent aIntent,
    TTime aTime)
    {
    TBool r = EFalse;
    CDRMConstraint* constraint;

    constraint = aPermission->ConstraintForIntent(aIntent);
    if (constraint != NULL && !constraint->Expired(aTime))
        {
        r = ETrue;
        }
    return r;
    }

TRightsStatus PermissionStatus(
    CDRMPermission* aPermission)
    {
    TRightsStatus r = ERightsStatusNone;
    TTime time;

    time.HomeTime();
    if ((!(aPermission->iAvailableRights & ERightsTopLevel) ||
        !aPermission->iTopLevel->Expired(time))

        &&

        (IsValid(aPermission, EPlay, time) ||
        IsValid(aPermission, EView, time) ||
        IsValid(aPermission, EPrint, time) ||
        IsValid(aPermission, EExecute, time)))
        {
        r = ERightsStatusValid;
        }
    return r;
    }

CRightsInfo* ConvertToRightsInfoL(
    CDRMPermission* aPermission,
    const TDesC8& aContentId)
    {
    TRightsTypeMask type;
    TPtr ptr(NULL, 0);
    HBufC* id = NULL;
    CRightsInfo* r = NULL;

    id = HBufC::NewLC(aContentId.Length() + 20);
    ptr.Set(id->Des());
    ptr.Copy(aContentId);
    ptr.SetLength(aContentId.Length());
    ptr.Append(0);
    ptr.AppendNum(aPermission->iUniqueID);
    if (aPermission->Stateful())
        {
        type = ERightsTypeConsumable;
        }
    else
        {
        type = ERightsTypeStateless;
        }
    r = CRightsInfo::NewL(KNullDesC, *id, type, PermissionStatus(aPermission));
    CleanupStack::PopAndDestroy(); // id
    return r;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// COma2AgentRightsManager::COma2AgentRightsManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
COma2AgentRightsManager::COma2AgentRightsManager()
    {
    }

// -----------------------------------------------------------------------------
// COma2AgentRightsManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void COma2AgentRightsManager::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// COma2AgentRightsManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
COma2AgentRightsManager* COma2AgentRightsManager::NewL()
    {
    COma2AgentRightsManager* self = new( ELeave ) COma2AgentRightsManager;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
COma2AgentRightsManager::~COma2AgentRightsManager()
    {
    }


// -----------------------------------------------------------------------------
// COma2AgentRightsManager::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void COma2AgentRightsManager::ListAllRightsL(
    RStreamablePtrArray<CRightsInfo>& aArray) const
    {
    RDRMRightsClient client;
    RPointerArray<CDRMPermission> permissions;
    RPointerArray<HBufC8> idList;
    TCleanupItem permCleanup(PointerArrayResetDestroyAndClose<CDRMPermission>,
        &permissions);
    TCleanupItem idCleanup(PointerArrayResetDestroyAndClose<HBufC8>,
        &idList);
    TInt i;
    TInt j;

    User::LeaveIfError(client.Connect());
    CleanupClosePushL(client);
    User::LeaveIfError(client.ExportContentIDList(idList));
    CleanupStack::PushL(idCleanup);

    for (j = 0; j < idList.Count(); j++)
        {
        client.GetDBEntriesL(*idList[j], permissions);
        CleanupStack::PushL(permCleanup);
        for (i = 0; i < permissions.Count(); i++)
            {
            aArray.AppendL(ConvertToRightsInfoL(permissions[i], *idList[j]));
            }

        CleanupStack::PopAndDestroy(1); // permCleanup
        }
    CleanupStack::PopAndDestroy(2); // idCleanup, client
    }

void COma2AgentRightsManager::ListRightsL(
    RStreamablePtrArray<CRightsInfo>& aArray,
    const TDesC& aUri) const
    {
    TVirtualPathPtr ptr(aUri, KDefaultContentObject);
    ListRightsL(aArray, ptr);
    }

void COma2AgentRightsManager::ListRightsL(
    RStreamablePtrArray<CRightsInfo>& aArray,
    TVirtualPathPtr& aVirtualPath) const
    {
    RPointerArray<CDRMPermission> permissions;
    RDRMRightsClient client;
    TCleanupItem listCleanup(PointerArrayResetDestroyAndClose<CDRMPermission>,
        &permissions);
    CDcfCommon* dcf = NULL;
    CDRMPermission* permission = NULL;
    RFs fs;
    TInt i;
    TInt error;
    TUint32 reason = 0;

    User::LeaveIfError(client.Connect());
    CleanupClosePushL(client);
    User::LeaveIfError(fs.Connect());
    User::LeaveIfError(fs.ShareAuto());
    CleanupClosePushL(fs);
    dcf = CDcfCommon::NewL(aVirtualPath.URI(), &fs);
    if (dcf == NULL)
        {
        User::Leave(KErrArgument);
        }
    CleanupStack::PushL(dcf);
    User::LeaveIfError(dcf->OpenPart(aVirtualPath.UniqueId()));

    TRAP(error, permission = client.GetActiveRightsL(EUnknown, *dcf->iContentID, reason));
    if (permission != NULL)
        {
        CleanupStack::PushL(permission);
        aArray.AppendL(ConvertToRightsInfoL(permission, *dcf->iContentID));
        CleanupStack::PopAndDestroy(); // permission
        }

    TRAP(error, client.GetDBEntriesL(*dcf->iContentID, permissions));
    CleanupStack::PushL(listCleanup);

    for (i = 0; i < permissions.Count(); i++)
        {
        aArray.AppendL(ConvertToRightsInfoL(permissions[i], *dcf->iContentID));
        }

    CleanupStack::PopAndDestroy(4); // listCleanup, dcf, fs, client
    }

void COma2AgentRightsManager::ListContentL(
    RStreamablePtrArray<CVirtualPath>& aArray,
    CRightsInfo& aRightsInfo) const
    {
    RDRMRightsClient client;
    HBufC8* contentId = NULL;
    CDcfRep* rep = NULL;
    CDcfEntry* entry = NULL;
    CVirtualPath* path = NULL;
    TInt n;

    User::LeaveIfError(client.Connect());
    CleanupClosePushL(client);
    n = aRightsInfo.UniqueId().Locate(0);
    if (n == KErrNotFound)
        {
        n = aRightsInfo.UniqueId().Length();
        }
    contentId = CnvUtfConverter::ConvertFromUnicodeToUtf8L(
        aRightsInfo.UniqueId().Left(n));
    CleanupStack::PushL(contentId);
    rep = CDcfRep::NewL();
    CleanupStack::PushL(rep);
    rep->OrderListL();
    entry = rep->NextL();
    while (entry != NULL)
        {
        CleanupStack::PushL(entry);
        path = CVirtualPath::NewL(TVirtualPathPtr(entry->FileName(),
            KDefaultContentObject));
        aArray.AppendL(path);
        CleanupStack::PopAndDestroy(); // entry
        entry = rep->NextL();
        }

    CleanupStack::PopAndDestroy(3); // rep, contentId, client
    }

MAgentRightsBase* COma2AgentRightsManager::GetRightsDataL(
    const CRightsInfo& aRightsInfo) const
    {
    RDRMRightsClient client;
    CDRMPermission* permission = NULL;
    TInt uniqueId;
    HBufC8* contentId = NULL;
    CDRMRights* rights = NULL;
    CDRMAsset* asset = NULL;
    TInt n;

    User::LeaveIfError(client.Connect());
    CleanupClosePushL(client);
    rights = CDRMRights::NewL();
    CleanupStack::PushL(rights);
    n = aRightsInfo.UniqueId().Locate(0);
    contentId = CnvUtfConverter::ConvertFromUnicodeToUtf8L(
        aRightsInfo.UniqueId().Left(n));
    CleanupStack::PushL(contentId);
    TLex lex(aRightsInfo.UniqueId().Right(
        aRightsInfo.UniqueId().Length() - n - 1));
    User::LeaveIfError(lex.Val(uniqueId));
    permission = client.GetDbEntryL(*contentId, uniqueId);
    if (permission != NULL)
        {
        CleanupStack::PushL(permission);
        asset = CDRMAsset::NewL();
        CleanupStack::PushL(asset);
        asset->iUid = contentId->AllocL();
        if (permission->iParentUID)
            {
            asset->iParentRights = permission->iParentUID->AllocL();
            }
        rights->SetPermissionL(*permission);
        rights->SetAssetL(*asset);
        CleanupStack::PopAndDestroy(2); // asset, permission
        }
    CleanupStack::Pop(); // rights
    CleanupStack::PopAndDestroy(2); // contentId, client

    return rights;
    }

TInt COma2AgentRightsManager::DeleteRightsObject(
    const CRightsInfo& aRightsInfo)
    {
    TInt error = KErrNone;
        TRAP( error, DoDeleteRightsObject(aRightsInfo));
        return error;
    }

TInt COma2AgentRightsManager::DeleteAllRightsObjects(
    const TVirtualPathPtr& aVirtualPath)
    {
    TInt error = KErrNone;
        TRAP( error, DoDeleteAllRightsObjectsL(aVirtualPath));
        return error;
    }

TInt COma2AgentRightsManager::SetProperty(
    TAgentProperty /*aProperty*/,
    TInt /*aValue*/)
    {
    return KErrCANotSupported;
    }



