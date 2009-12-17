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
* Description: 
*
*/



// INCLUDE FILES
#include <caf.h>
#include <bitset.h>
#include <attribute.h>
#include <caf/attributeset.h>
#include <caf/stringattributeset.h>
#include <e32test.h>
#include <utf.h>
#include "wmdrmagentcontent.h"
#include "wmdrmagentattributes.h"
#include "asf.h"
#include "logfn.h"

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
// CWmDrmAgentContent::CWmDrmAgentContent
// Reset all member variables
// -----------------------------------------------------------------------------
//
CWmDrmAgentContent::CWmDrmAgentContent():
    iURI(NULL),
    iAsf(NULL)
    {
    LOGFN( "CWmDrmAgentContent::CWmDrmAgentContent" );
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentContent::NewL
// Two-phased constructors
// -----------------------------------------------------------------------------
//
CWmDrmAgentContent* CWmDrmAgentContent::NewL(const TDesC& aURI, TContentShareMode aShareMode)
    {
    LOGFN( "CWmDrmAgentContent::NewL" );
    CWmDrmAgentContent* self = new (ELeave) CWmDrmAgentContent();
    CleanupStack::PushL(self);
    self->ConstructL(aURI, aShareMode);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentContent::NewLC
// Two-phased constructors
// -----------------------------------------------------------------------------
//
CWmDrmAgentContent* CWmDrmAgentContent::NewLC(
    const TDesC& aUri,
    TContentShareMode aShareMode)
    {
    LOGFN( "CWmDrmAgentContent::NewLC" );
    CWmDrmAgentContent* self = new (ELeave) CWmDrmAgentContent();
    CleanupStack::PushL(self);
    self->ConstructL(aUri, aShareMode);
    return self;
    }


// -----------------------------------------------------------------------------
// CWmDrmAgentContent::NewL
// Two-phased constructors
// -----------------------------------------------------------------------------
//
//

CWmDrmAgentContent* CWmDrmAgentContent::NewL(RFile& aFile)
    {
    LOGFN( "CWmDrmAgentContent::NewL (2)" );
    CWmDrmAgentContent* self = new (ELeave) CWmDrmAgentContent();
    CleanupStack::PushL(self);
    self->ConstructL(aFile);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentContent::NewLC
// Two-phased constructors
// -----------------------------------------------------------------------------
//
//

CWmDrmAgentContent* CWmDrmAgentContent::NewLC(RFile& aFile)
    {
    LOGFN( "CWmDrmAgentContent::NewLC (2)" );
    CWmDrmAgentContent* self = new (ELeave) CWmDrmAgentContent();
    CleanupStack::PushL(self);
    self->ConstructL(aFile);
    return self;
    }



// -----------------------------------------------------------------------------
// CWmDrmAgentContent::~CWmDrmAgentContent
// -----------------------------------------------------------------------------
//
CWmDrmAgentContent::~CWmDrmAgentContent()
    {
    LOGFN( "CWmDrmAgentContent::~CWmDrmAgentContent" );
    iFile.Close();
    iFs.Close();
    delete iURI;
    delete iAsf;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentContent::ConstructL
// Initialize the agent from a URI (file name) and file mode, opens the
// the file and takes membership of the handle
// -----------------------------------------------------------------------------
//
void CWmDrmAgentContent::ConstructL(const TDesC& aUri, TContentShareMode aShareMode)
    {
    LOGFN( "CWmDrmAgentContent::ConstructL" );
    iURI = aUri.AllocL();
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFile.Open(iFs, aUri, MapContentShareMode(aShareMode)));
    iAsf = CAsf::NewL(iFile);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentContent::ConstructL
// Initialize the agent from a file handle, duplicates the handle
// -----------------------------------------------------------------------------
//
void CWmDrmAgentContent::ConstructL(RFile& aFile)
    {
    TFileName name;

    LOGFN( "CWmDrmAgentContent::ConstructL (2)" );
    User::LeaveIfError(iFile.Duplicate(aFile));
    User::LeaveIfError(iFs.Connect());
    iFile.Name(name);
    iURI = name.AllocL();
    iAsf = CAsf::NewL(iFile);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentContent::OpenContainer
// -----------------------------------------------------------------------------
//

TInt CWmDrmAgentContent::OpenContainer(const TDesC&)
    {
    LOGFN( "CWmDrmAgentContent::OpenContainer" );
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentContent::CloseContainer
// -----------------------------------------------------------------------------
//

TInt CWmDrmAgentContent::CloseContainer()
    {
    LOGFN( "CWmDrmAgentContent::CloseContainer" );
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentContent::GetEmbeddedObjectsL
// -----------------------------------------------------------------------------
//

void CWmDrmAgentContent::GetEmbeddedObjectsL(RStreamablePtrArray<CEmbeddedObject>& /*aArray*/)
    {
    LOGFN( "CWmDrmAgentContent::GetEmbeddedObjectsL" );
    User::Leave(KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentContent::GetEmbeddedObjectsL
// -----------------------------------------------------------------------------
//

void CWmDrmAgentContent::GetEmbeddedObjectsL(RStreamablePtrArray<CEmbeddedObject>& aArray, TEmbeddedType aType)
    {
    LOGFN( "CWmDrmAgentContent::GetEmbeddedObjectsL" );
    // the only embedded object is the file itself
    if(aType == EContentObject)
        {
        // just get the default object since there is only one content object
        GetEmbeddedObjectsL(aArray);
        }
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentContent::Search
// -----------------------------------------------------------------------------
//

TInt CWmDrmAgentContent::Search(RStreamablePtrArray<CEmbeddedObject>& /*aArray*/, const TDesC8& /*aMimeType*/, TBool )
    {
    LOGFN( "CWmDrmAgentContent::Search" );
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentContent::GetAttribute
// -----------------------------------------------------------------------------
//

TInt CWmDrmAgentContent::GetAttribute(TInt aAttribute, TInt& aValue, const TDesC& aUniqueId)
    {
    TInt r = KErrNone;
    TInt err = KErrNone;

    LOGFN( "CWmDrmAgentContent::GetAttribute" );
    TRAP(err, r = TWmDrmAgentAttributes::GetAttributeL(
              iAsf, aAttribute, aValue, TVirtualPathPtr(*iURI, aUniqueId)));
    if (err != KErrNone)
        {
        r = err;
        }
    return r;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentContent::GetAttributeSet
// -----------------------------------------------------------------------------
//

TInt CWmDrmAgentContent::GetAttributeSet(RAttributeSet& aAttributeSet, const TDesC& aUniqueId)
    {
    TInt r = KErrNone;
    TInt err = KErrNone;

    LOGFN( "CWmDrmAgentContent::GetAttributeSet" );
    TRAP(err,r = TWmDrmAgentAttributes::GetAttributeSetL(iAsf, aAttributeSet,
        TVirtualPathPtr(*iURI, aUniqueId)));
    if (err != KErrNone)
        {
        r = err;
        }
    return r;
    }


// -----------------------------------------------------------------------------
// CWmDrmAgentContent::GetStringAttribute
// -----------------------------------------------------------------------------
//

TInt CWmDrmAgentContent::GetStringAttribute(TInt aAttribute, TDes& aValue, const TDesC& aUniqueId)
    {

    TInt r = KErrNone;
    TInt err = KErrNone;

    LOGFN( "CWmDrmAgentContent::GetStringAttribute" );
    TRAP(err, r = TWmDrmAgentAttributes::GetStringAttributeL(iAsf, aAttribute,
        aValue, TVirtualPathPtr(*iURI, aUniqueId)));
    if (err != KErrNone)
        {
        r = err;
        }
    return r;

    }

// -----------------------------------------------------------------------------
// CWmDrmAgentContent::GetStringAttributeSet
// -----------------------------------------------------------------------------
//

TInt CWmDrmAgentContent::GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, const TDesC& aUniqueId)
    {
    TInt r = KErrNone;
    TInt err = KErrNone;

    LOGFN( "CWmDrmAgentContent::GetStringAttributeSet" );
    TRAP(err,r = TWmDrmAgentAttributes::GetStringAttributeSetL(
        iAsf, aStringAttributeSet,
        TVirtualPathPtr(*iURI, aUniqueId)));
    if (err != KErrNone)
        {
        r = err;
        }
    return r;

    }

// -----------------------------------------------------------------------------
// CWmDrmAgentContent::AgentSpecificCommand
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentContent::AgentSpecificCommand(
    TInt /*aCommand*/,
    const TDesC8& /*aInputBuffer*/,
    TDes8& /*aOutputBuffer*/)
    {
    LOGFN( "CWmDrmAgentContent::AgentSpecificCommand" );
    return KErrCANotSupported;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentContent::AgentSpecificCommand
// -----------------------------------------------------------------------------
//
void CWmDrmAgentContent::AgentSpecificCommand(
    TInt aCommand,
    const TDesC8& aInputBuffer,
    TDes8& aOutputBuffer,
    TRequestStatus& aStatus)
    {
    LOGFN( "CWmDrmAgentContent::AgentSpecificCommand (2)" );
    TRequestStatus *ptr = &aStatus;
    User::RequestComplete(ptr, AgentSpecificCommand(aCommand, aInputBuffer,
        aOutputBuffer));
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentContent::NotifyStatusChange
// -----------------------------------------------------------------------------
//
void CWmDrmAgentContent::NotifyStatusChange(TEventMask , TRequestStatus& aStatus, const TDesC& )
    {
    LOGFN( "CWmDrmAgentContent::NotifyStatusChange" );
    TRequestStatus* ptr = &aStatus;
    User::RequestComplete(ptr, KErrCANotSupported);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentContent::CancelNotifyStatusChange
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentContent::CancelNotifyStatusChange(
    TRequestStatus& /*aStatus*/,
    const TDesC& /*aUniqueId*/)
    {
    LOGFN( "CWmDrmAgentContent::CancelNotifyStatusChange" );
    return KErrCANotSupported;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentContent::RequestRights
// -----------------------------------------------------------------------------
//
void CWmDrmAgentContent::RequestRights(TRequestStatus& aStatus, const TDesC& )
    {
    LOGFN( "CWmDrmAgentContent::RequestRights" );
    TRequestStatus* ptr = &aStatus;
    User::RequestComplete(ptr, KErrCANotSupported);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentContent::CancelRequestRights
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentContent::CancelRequestRights(
    TRequestStatus& aStatus,
    const TDesC& /*aUniqueId*/)
    {
    LOGFN( "CWmDrmAgentContent::CancelRequestRights" );
    TRequestStatus *ptr = &aStatus;
    User::RequestComplete(ptr, KErrCANotSupported);
    return KErrCANotSupported;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentContent::DisplayInfoL
// -----------------------------------------------------------------------------
//
void CWmDrmAgentContent::DisplayInfoL(
    TDisplayInfo /*aDisplayInfo*/,
    const TDesC& /*aUniqueId*/)
    {
    LOGFN( "CWmDrmAgentContent::DisplayInfoL" );
    User::Leave(KErrCANotSupported);
    }


// -----------------------------------------------------------------------------
// CWmDrmAgentContent::SetProperty
// Reset all member variables
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentContent::SetProperty(
    TAgentProperty /*aProperty*/,
    TInt /*aValue*/)
    {
    LOGFN( "CWmDrmAgentContent::SetProperty" );
    return KErrCANotSupported;
    }


