/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Stub implementation of CAgentRightsManager
*
*/



// INCLUDE FILES
#include <caf.h>
#include <utf.h>
#include "wmdrmagentrightsmanager.h"
#include "rightsinfo.h"
#include "asf.h"

#include "logfn.h"

using namespace ContentAccess;
// ============================ LOCAL FUNCTIONS ================================

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
// CWmDrmAgentRightsManager::CWmDrmAgentRightsManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWmDrmAgentRightsManager::CWmDrmAgentRightsManager()
    {
    LOGFN( "CWmDrmAgentRightsManager::CWmDrmAgentRightsManager" );
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentRightsManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWmDrmAgentRightsManager::ConstructL()
    {
    LOGFN( "CWmDrmAgentRightsManager::ConstructL" );
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentRightsManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWmDrmAgentRightsManager* CWmDrmAgentRightsManager::NewL()
    {
    LOGFN( "CWmDrmAgentRightsManager::NewL" );
    CWmDrmAgentRightsManager* self = new( ELeave ) CWmDrmAgentRightsManager;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentRightsManager::~CWmDrmAgentRightsManager
// Destructor
// -----------------------------------------------------------------------------
//

CWmDrmAgentRightsManager::~CWmDrmAgentRightsManager()
    {
    LOGFN( "CWmDrmAgentRightsManager::~CWmDrmAgentRightsManager" );
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentRightsManager::ListAllRightsL
// -----------------------------------------------------------------------------
//
void CWmDrmAgentRightsManager::ListAllRightsL(
    RStreamablePtrArray<CRightsInfo>& /*aArray*/) const
    {
    LOGFN( "CWmDrmAgentRightsManager::ListAllRightsL" );
    User::Leave(KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentRightsManager::ListRightsL
// -----------------------------------------------------------------------------
//
void CWmDrmAgentRightsManager::ListRightsL(
    RStreamablePtrArray<CRightsInfo>& /*aArray*/,
    const TDesC& /*aUri*/) const
    {
    LOGFN( "CWmDrmAgentRightsManager::ListRightsL" );
    User::Leave(KErrCANotSupported);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentRightsManager::ListRightsL
// -----------------------------------------------------------------------------
//
void CWmDrmAgentRightsManager::ListRightsL(
    RStreamablePtrArray<CRightsInfo>& aArray,
    TVirtualPathPtr& aVirtualPath) const
    {
    LOGFN( "CWmDrmAgentRightsManager::ListRightsL (2)" );
    ListRightsL( aArray, aVirtualPath.URI() );
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentRightsManager::ListContentL
// -----------------------------------------------------------------------------
//
void CWmDrmAgentRightsManager::ListContentL(
    RStreamablePtrArray<CVirtualPath>& /*aArray*/,
    CRightsInfo& /*aRightsInfo*/) const
    {
    LOGFN( "CWmDrmAgentRightsManager::ListContentL" );
    User::Leave(KErrCANotSupported);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentRightsManager::GetRightsDataL
// -----------------------------------------------------------------------------
//
MAgentRightsBase* CWmDrmAgentRightsManager::GetRightsDataL(
    const CRightsInfo& /*aRightsInfo*/) const
    {
    LOGFN( "CWmDrmAgentRightsManager::GetRightsDataL" );
    User::Leave(KErrCANotSupported);
    return NULL;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentRightsManager::DeleteRightsObject
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentRightsManager::DeleteRightsObject(
    const CRightsInfo& /*aRightsInfo*/)
    {
    LOGFN( "CWmDrmAgentRightsManager::DeleteRightsObject" );
    return KErrCANotSupported;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentRightsManager::DeleteAllRightsObjects
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentRightsManager::DeleteAllRightsObjects(
    const TVirtualPathPtr& /*aVirtualPath*/)
    {
    LOGFN( "CWmDrmAgentRightsManager::DeleteAllRightsObjects" );
    return KErrCANotSupported;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentRightsManager::SetProperty
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentRightsManager::SetProperty(
    TAgentProperty /*aProperty*/,
    TInt /*aValue*/)
    {
    LOGFN( "CWmDrmAgentRightsManager::SetProperty" );
    return KErrCANotSupported;
    }
