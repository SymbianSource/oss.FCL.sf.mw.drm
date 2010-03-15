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
* Description:  Rights Object Client side MTM
*
*/



// INCLUDE FILES

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mtmuidsdef.hrh>
#endif
#include <e32std.h>
#include <txtrich.h>
#include <msvreg.h>
#include <msvstore.h>    // cmsvstore
#include <msvids.h>      // kmsvlocalserviceindexentryid
#include <mtmdef.h>      // kmsvmessagepartbody
#include <msvuids.h>     // kuidmsvmessageentry
#include <sysutil.h>     // disk space checking

#include    "RoMtmCli.h"
#include    "RoMTMCC.h"
#include    "RoMTMCommon.h"
GLREF_C void Panic( TRoMtmCliPanic aPanic );

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================



/*************************************
*
*        Construction and destruction
*
**************************************/


// -----------------------------------------------------------------------------
// CRightsObjectMtmClient::CRightsObjectMtmClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRightsObjectMtmClient::CRightsObjectMtmClient(
    CRegisteredMtmDll& aRegisteredMtmDll ,
    CMsvSession& aMsvSession
    )
: CBaseMtm(aRegisteredMtmDll , aMsvSession )
    {
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmClient::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRightsObjectMtmClient::ConstructL()
    {
    SwitchCurrentEntryL( KMsvRootIndexEntryId );
    }


// -----------------------------------------------------------------------------
// CRightsObjectMtmClient* CRightsObjectMtmClient::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRightsObjectMtmClient* CRightsObjectMtmClient::NewL(
    CRegisteredMtmDll& aRegisteredMtmDll ,
    CMsvSession& aMsvSession
    )
// Factory function
    {
    CRightsObjectMtmClient* self = new ( ELeave ) CRightsObjectMtmClient( aRegisteredMtmDll , aMsvSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CRightsObjectMtmClient::~CRightsObjectMtmClient()
    {
    }

/*************************************
*
*        from CBaseMtm
*
**************************************/

// -----------------------------------------------------------------------------
// CRightsObjectMtmClient::AddAddresseeL
//
// Not supported
// -----------------------------------------------------------------------------
//
void CRightsObjectMtmClient::AddAddresseeL( const TDesC& /*aRealAddress*/ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmClient::AddAddresseeL
//
// Not supported
// -----------------------------------------------------------------------------
//
void CRightsObjectMtmClient::AddAddresseeL( const TDesC& /*aRealAddress*/ ,
    const TDesC& /*aAlias*/ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmClient::RemoveAddressee
//
// Not supported
// -----------------------------------------------------------------------------
//
void CRightsObjectMtmClient::RemoveAddressee( TInt /*aIndex*/ )
    {
    // Not supported.
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmClient::SaveMessageL
//
// Commits cached changes to the storage controlled by the Message Server
// can only be called on message contexts.
// -----------------------------------------------------------------------------
//
void CRightsObjectMtmClient::SaveMessageL()
    // Store entry data
    {
    TMsvEntry indexEntry( iMsvEntry->Entry() );
    RFs& fs( Session().FileSession() );
    const CRichText* body( &Body() );

    if ( !body )
        {
        User::Leave( KErrArgument );
        }
    // check if there is space for text to be saved
    if ( SysUtil::DiskSpaceBelowCriticalLevelL(
            &fs,
            body->DocumentLength() * sizeof( TText ),
            RFs::GetSystemDrive() ) )
        {
        User::Leave( KErrDiskFull );
        }


    // get an editable message store
    CMsvStore* store( iMsvEntry->EditStoreL() );
    CleanupStack::PushL( store );

    switch ( iMsvEntry->Entry().iType.iUid )
        {
        case KUidMsvMessageEntryValue:
                {
                StoreBodyL( *store );
                break;
                }
        default:
                {
                User::Leave( KErrNotSupported );
                break;
                }
        }
    // commit the index changes.
    iMsvEntry->ChangeL( indexEntry );
    store->CommitL();
    CleanupStack::PopAndDestroy( store );
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmClient::LoadMessageL
//
// Loads the cache with the message data for the current context
// can only be called on message contexts.
// -----------------------------------------------------------------------------
//
void CRightsObjectMtmClient::LoadMessageL()
    {
    TMsvEntry indexEntry( iMsvEntry->Entry() );
    switch ( indexEntry.iType.iUid )
        {
        case KUidMsvMessageEntryValue:
                {
                // get read-only message store
                Body().Reset();
                CMsvStore* store( iMsvEntry->ReadStoreL() );
                CleanupStack::PushL( store );
                RestoreBodyL( *store );
                CleanupStack::PopAndDestroy( store );
                break;
                }
        default:
            // if we fall through here, we should panic because
            // we have wrong entry type
                {
                User::Leave( KErrNotSupported );
                break;
                }
        }
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmClient::ReplyL
//
// Not supported
// -----------------------------------------------------------------------------
//
CMsvOperation* CRightsObjectMtmClient::ReplyL (
    TMsvId /*aReplyEntryId*/ ,
    TMsvPartList /*aPartlist*/ ,
    TRequestStatus& /*aCompletionStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmClient::ForwardL
//
// Not supported
// -----------------------------------------------------------------------------
//
CMsvOperation* CRightsObjectMtmClient::ForwardL(
    TMsvId /*aForwardEntryId*/ ,
    TMsvPartList /*aPartList*/ ,
    TRequestStatus& /*aCompletionStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmClient::ValidateMessage
//
// Not supported
// -----------------------------------------------------------------------------
//
TMsvPartList CRightsObjectMtmClient::ValidateMessage( TUint /*PartList*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmClient::Find
//
// Not supported
// -----------------------------------------------------------------------------
//
TMsvPartList CRightsObjectMtmClient::Find( const TDesC& /*aTextToFind*/ , TMsvPartList /*aPartList*/ )
    // Find text in entry
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmClient::QueryCapability
//
// Queries if the MTM supports a particular capability, specified by a UID.
// -----------------------------------------------------------------------------
//
TInt CRightsObjectMtmClient::QueryCapability( TUid aCapability , TInt& aResponse )
    // Query for capability
    {
    TInt error( KErrNone );
    switch ( aCapability.iUid )
        {
        case KUidMtmQueryMaxBodySizeValue:
        case KUidMtmQueryMaxTotalMsgSizeValue:
                {
                aResponse = KMaxRoMessageSize;
                break;
                }
        case KUidMtmQuerySupportedBodyValue:
                {
                aResponse = KMtm7BitBody | KMtm8BitBody |
                    KMtm16BitBody | KMtmBinaryBody;
                break;
                }
        case KUidMtmQuerySupportSubjectValue:
        case KUidMtmQueryOffLineAllowedValue:
                {
                aResponse=ETrue;
                break;
                }
        default:
                {
                error = KErrNotSupported;
                }
        }
    return error;
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmClient::InvokeSyncFunctionL
//
// Not supported
// -----------------------------------------------------------------------------
//
void CRightsObjectMtmClient::InvokeSyncFunctionL(
    TInt /*aFunctionId*/ ,
    const CMsvEntrySelection& /*aSelection*/ ,
    TDes8& /*aParameter*/ )
// Call MTM-specific operation synchronously
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmClient::InvokeAsyncFunctionL
//
// Not supported
// -----------------------------------------------------------------------------
//
CMsvOperation* CRightsObjectMtmClient::InvokeAsyncFunctionL(
    TInt /*aFunctionId*/ ,
    const CMsvEntrySelection& /*aSelection*/ ,
    TDes8& /*aParameter*/ ,
    TRequestStatus& /*aCompletionStatus*/ )
// Call MTM-specific operation asynchronously
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmClient::ContextEntrySwitched
//
// Not supported
// -----------------------------------------------------------------------------
//
void CRightsObjectMtmClient::ContextEntrySwitched()
    // Context change notification
    //
    // No need to know entry changes
    //
    {
    }


