/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DRM Automated Usage handling
*
*/


// INCLUDE FILES
#include    <caf/caftypes.h>
#include    <caf/data.h>
#include    <Oma2Agent.h>
#include    <utf.h>
#include    <DRMRights.h>
#include    <drmagents.h>
#include    <e32cmn.h>
#include    <drmutility.h>

#include    "DrmAutomatedUsageImpl.h"
#include    "drmautomatedusagedata.h"
#include    "drmutilityui.h"

// CONSTANTS
_LIT( KAuClassificationRingingTone, "Ringtone" );
_LIT( KAuClassificationVideoTone, "Videotone" );

const TInt KUrlMaxLen = 1024;
const TInt KTimeMarginal = 5000000; // 5 seconds

// FORWARD DECLARATIONS
class CDRMConstraint;

// ============================= LOCAL FUNCTIONS ===============================

LOCAL_C TBool HasInterval( const CDRMConstraint *aConstraint )
    {
    return aConstraint->iActiveConstraints & EConstraintInterval;
    }

LOCAL_C TBool HasEndTime( const CDRMConstraint *aConstraint )
    {
    return aConstraint->iActiveConstraints & EConstraintEndTime;
    }

LOCAL_C TBool EndTimesEqual( const CDRMConstraint *aFirst,
                             const CDRMConstraint *aSecond )
    {
    TBool equal( EFalse );
    TInt64 difference( aFirst->iEndTime.Int64() - aSecond->iEndTime.Int64() );
    if( -KTimeMarginal <= difference && difference <= KTimeMarginal )
        {
        equal = ETrue;
        }
    return equal;
    }

LOCAL_C TBool IntervalsEqual( const CDRMConstraint *aFirst,
                              const CDRMConstraint *aSecond )
    {
    TBool equal( EFalse );
    TInt64 startDifference(
        aFirst->iIntervalStart.Int64() - aSecond->iIntervalStart.Int64() );

    if( aFirst->iInterval == aSecond->iInterval &&
        -KTimeMarginal <= startDifference  &&
        startDifference <= KTimeMarginal )
        {
        equal = ETrue;
        }
    return equal;
    }


LOCAL_C HBufC8*  GetContentIdL( const ContentAccess::CData& aData )
    {
    HBufC* uniqueId( NULL );
    HBufC8* uniqueId8( NULL );
    TPtr value( NULL, 0 );

    uniqueId = HBufC::NewLC( ContentAccess::KMaxCafContentName );

    value.Set( uniqueId->Des() );

    User::LeaveIfError( aData.GetStringAttribute( ContentAccess::EContentID,
                                                  value ) );

    uniqueId8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( value );
    CleanupStack::PopAndDestroy( uniqueId );

    return uniqueId8;
    }

LOCAL_C TBool HasCountOrAccumulated( const CDRMConstraint* aConstraint )
    {
    // constraint has counters, timed counters or accumulated
    if ( aConstraint->iActiveConstraints &  ( EConstraintCounter |
                                              EConstraintTimedCounter |
                                              EConstraintAccumulated ) )
        {
        return ETrue;
        }
    return EFalse;
    }

#pragma mark -
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageImpl::CDrmAutomatedUsageImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
DRM::CDrmAutomatedUsageImpl::CDrmAutomatedUsageImpl(
    CCoeEnv* aCoeEnv,
    DRM::CDrmUtility* aDrmUtility ): CActive( EPriorityStandard ),
                                     iAuObserver( NULL ),
                                     iCoeEnv( aCoeEnv ),
                                     iDrmUtility( aDrmUtility )
    {

    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void DRM::CDrmAutomatedUsageImpl::ConstructL()
    {
    iDrmUtilityUi = DRM::CDrmUtilityUI::NewL( iCoeEnv );

    iDrmQueue = CDrmQueue< DRM::CDrmAutomatedUsageData >::NewL();

    User::LeaveIfError( iOmaClient.Connect() );

    //User::LeaveIfError( iWmClient.Connect() );

    User::LeaveIfError( iDrmHelperClient.Connect() );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmAutomatedUsageImpl* DRM::CDrmAutomatedUsageImpl::NewL(
    CCoeEnv* aCoeEnv,
    DRM::CDrmUtility* aDrmUtility )
    {
    DRM::CDrmAutomatedUsageImpl* self(
                        DRM::CDrmAutomatedUsageImpl::NewLC( aCoeEnv,
                                                            aDrmUtility ) );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageImpl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmAutomatedUsageImpl* DRM::CDrmAutomatedUsageImpl::NewLC(
    CCoeEnv* aCoeEnv, DRM::CDrmUtility* aDrmUtility )
    {
    DRM::CDrmAutomatedUsageImpl* self(
                     new( ELeave ) DRM::CDrmAutomatedUsageImpl( aCoeEnv,
                                                                aDrmUtility ) );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// Destructor
DRM::CDrmAutomatedUsageImpl::~CDrmAutomatedUsageImpl()
    {
    DRM::CDrmAutomatedUsageData* data( iDrmQueue->PopFront() );

    // Empty the queue:
    while ( data )
        {
        // Complete the outstanding requestest with cancel
        data->iObserver->OperationCompleted( data->iOperationId, KErrCancel );
        delete data;
        data = iDrmQueue->PopFront();
        }

    delete iDrmQueue;

    delete iDrmUtilityUi;

    iDrmHelperClient.Close();

    //iWmClient.Close();

    iOmaClient.Close();

    // Remove the object from active scheduler etc.
    if ( IsAdded() )
        {
        Deque();
        }
    }


// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageImpl::CanSetAutomated
// Implementation of file-based CanSetAutomated
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C TBool DRM::CDrmAutomatedUsageImpl::CanSetAutomatedL(
    RFile& aFile,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType )
    {
    TBool returnValue( EFalse );
    TPtrC drmAgent( KNullDesC );
    DRM::TDrmProtectionStatus protectedFile( DRM::EUUnknown );

    returnValue = iDrmUtility->GetDrmInfoL( aFile, drmAgent, protectedFile );
    if ( !returnValue || protectedFile == DRM::EUUnprotected )
        {
        // not DRM file at all or not protected DRM file.
        // So can be set as automated.
        return ETrue;
        }
    if ( drmAgent == DRM::KDrmWMAgentName )
        {
        //WMDRM not supported as automated. So no need to continue.
        return EFalse;
        }

    ContentAccess::CData* data(
            ContentAccess::CData::NewLC( aFile,
                                         ContentAccess::KDefaultContentObject,
                                         ContentAccess::EPeek ) );

    returnValue = CanSetAutomatedL( *data, aIntent, aAutomatedType );
    CleanupStack::PopAndDestroy( data );
    return returnValue;
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageImpl::CanSetAutomated
// Implementation of ContentAccess::CData-based CanSetAutomated
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C TBool DRM::CDrmAutomatedUsageImpl::CanSetAutomatedL(
    const ContentAccess::CData& aData,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType )
    {
    TInt err( 0 );
    TInt agentUid( 0 );
    HBufC8* uniqueId8( NULL );
    TBool canSetAutomated( EFalse );
    TBool protectedFile( EFalse );

    User::LeaveIfError( aData.GetAttribute( ContentAccess::EIsProtected,
                                            protectedFile ) );
    if ( !protectedFile )
        {
        // not protected. So can be set as automated.
        return ETrue;
        }

    User::LeaveIfError( aData.GetAttribute( DRM::EDrmAgentUid, agentUid ) );

    if ( agentUid == DRM::EDrmWmAgent )
        {
        //WMDRM not supported as automated. So no need to continue.
        return EFalse;
        }

    uniqueId8 = GetContentIdL( aData );
    CleanupStack::PushL( uniqueId8 );

    // now check whether there is OMA RO
    // suitable for automated use
    canSetAutomated = DoCanSetAutomatedL( *uniqueId8, aIntent, aAutomatedType );

    if( canSetAutomated )
        {
        TInt filetype( 0 );
        User::LeaveIfError( aData.GetAttribute( EFileType, filetype ) );
        if ( aIntent == ContentAccess::EPlay && filetype == EOma2Dcf )
            {
            // If Content type == OMA2 && intent == Play
            // let's look at classification info
            TPtr value( NULL, 0 );
            HBufC* classificationInfo( HBufC::NewLC( KUrlMaxLen ) );
            value.Set( classificationInfo->Des() );

            err = aData.GetStringAttribute( EClassificationInfo, value );
            if( err && err != KErrCANotSupported  )
                {
                User::Leave( err );
                }

            if ( classificationInfo->
                 FindF( KAuClassificationRingingTone ) == KErrNotFound &&
                 classificationInfo->
                 FindF( KAuClassificationVideoTone ) == KErrNotFound )
                {
                canSetAutomated = EFalse;
                }
            CleanupStack::PopAndDestroy( classificationInfo );
            }
        }

    CleanupStack::PopAndDestroy( uniqueId8 );

    return canSetAutomated;
    }
#pragma mark -

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageImpl::SetAutomatedL
// Implementation of file-based SetAutomated
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmAutomatedUsageImpl::SetAutomatedL(
    RFile& aFile,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType,
    const DRM::TDrmAutomatedServiceType aServiceType )
    {
    ContentAccess::CData* data =
        ContentAccess::CData::NewLC( aFile,
                                     ContentAccess::KDefaultContentObject,
                                     ContentAccess::EPeek );
    SetAutomatedL( *data, aIntent, aAutomatedType, aServiceType);
    CleanupStack::PopAndDestroy( data );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageImpl::SetAutomatedAsyncL
// Implementation of file-based SetAutomated
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmAutomatedUsageImpl::SetAutomatedAsyncL(
    RFile& aFile,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType,
    DRM::MDrmAsyncObserver& aObserver,
    const DRM::TDrmAutomatedServiceType aServiceType )
    {
    TInt ret( KErrNone );

    ContentAccess::CData* data =
        ContentAccess::CData::NewLC( aFile,
                                     ContentAccess::KDefaultContentObject,
                                     ContentAccess::EPeek );

    ret = SetAutomatedAsyncL( *data, aIntent, aAutomatedType, aObserver, aServiceType );

    CleanupStack::PopAndDestroy( data );

    return ret;
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageImpl::SetAutomatedL
// Implementation of ContentAccess::CData-based SetAutomated
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmAutomatedUsageImpl::SetAutomatedL(
    const ContentAccess::CData& aData,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType,
    const DRM::TDrmAutomatedServiceType aServiceType )
    {
    if ( !CanSetAutomatedL( aData, aIntent, aAutomatedType ) )
        {
        User::Leave( KErrArgument );
        }

    DRM::CDrmAutomatedUsageData* data(
        DRM::CDrmAutomatedUsageData::NewLC(
                                aData,
                                aIntent,
                                aAutomatedType,
                                NULL,
                                aServiceType,
                                DRM::CDrmAutomatedUsageData::ESetAutomated ) );

    User::LeaveIfError ( DoSetAutomatedL( data ) );

    CleanupStack::PopAndDestroy( data );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageImpl::SetAutomatedAsyncL
// Implementation of ContentAccess::CData-based SetAutomated
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmAutomatedUsageImpl::SetAutomatedAsyncL(
    const ContentAccess::CData& aData,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType,
    DRM::MDrmAsyncObserver& aObserver,
    const DRM::TDrmAutomatedServiceType aServiceType )
    {
    if ( !CanSetAutomatedL( aData, aIntent, aAutomatedType ) )
        {
        User::Leave( KErrArgument );
        }

    DRM::CDrmAutomatedUsageData* data(
        DRM::CDrmAutomatedUsageData::NewL(
                                aData,
                                aIntent,
                                aAutomatedType,
                                &aObserver,
                                aServiceType,
                                DRM::CDrmAutomatedUsageData::ESetAutomated ) );

    iDrmQueue->AppendToQueueL( data );

    TRequestStatus* status( &iStatus );

    Activate( status );

    return data->iOperationId;
    }

#pragma mark -
// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageImpl::RemoveAutomated
// Removes content from automated use
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmAutomatedUsageImpl::RemoveAutomatedL(
    RFile& aFile,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType,
    const DRM::TDrmAutomatedServiceType aServiceType )
    {
    ContentAccess::CData* data(
            ContentAccess::CData::NewLC( aFile,
                                         ContentAccess::KDefaultContentObject,
                                         ContentAccess::EPeek ) );

    RemoveAutomatedL( *data, aIntent, aAutomatedType, aServiceType );
    CleanupStack::PopAndDestroy( data );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageImpl::RemoveAutomatedAsyncL
// Removes content from automated use
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmAutomatedUsageImpl::RemoveAutomatedAsyncL(
    RFile& aFile,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType,
    DRM::MDrmAsyncObserver& aObserver,
    const DRM::TDrmAutomatedServiceType aServiceType )
    {
    TInt ret( KErrNone );

    ContentAccess::CData* data(
            ContentAccess::CData::NewLC( aFile,
                                         ContentAccess::KDefaultContentObject,
                                         ContentAccess::EPeek ) );

    ret = RemoveAutomatedAsyncL( *data,
                                 aIntent,
                                 aAutomatedType,
                                 aObserver,
                                 aServiceType );

    CleanupStack::PopAndDestroy( data );
    return ret;
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageImpl::RemoveAutomatedL
// Removes content from automated use
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmAutomatedUsageImpl::RemoveAutomatedL(
    const ContentAccess::CData& aData,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType,
    const DRM::TDrmAutomatedServiceType aServiceType )
    {
    DRM::CDrmAutomatedUsageData* data (
        DRM::CDrmAutomatedUsageData::NewLC(
                            aData,
                            aIntent,
                            aAutomatedType,
                            NULL,
                            aServiceType,
                            DRM::CDrmAutomatedUsageData::ERemoveAutomated ) );

    User::LeaveIfError( DoRemoveAutomated( data ) );

    CleanupStack::PopAndDestroy( data );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageImpl::RemoveAutomatedAsyncL
// Removes content from automated use
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmAutomatedUsageImpl::RemoveAutomatedAsyncL(
    const ContentAccess::CData& aData,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType,
    DRM::MDrmAsyncObserver& aObserver,
    const DRM::TDrmAutomatedServiceType aServiceType )
    {
    DRM::CDrmAutomatedUsageData* data(
        DRM::CDrmAutomatedUsageData::NewL(
                            aData,
                            aIntent,
                            aAutomatedType,
                            &aObserver,
                            aServiceType,
                            DRM::CDrmAutomatedUsageData::ERemoveAutomated ) );

    iDrmQueue->AppendToQueueL( data );

    TRequestStatus* status( &iStatus );

    Activate( status );

    return data->iOperationId;
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageImpl::RemoveAutomatedL
// Removes content from automated use
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmAutomatedUsageImpl::RemoveAutomatedL(
    const TDesC& aUniqueId,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType,
    const DRM::TDrmAutomatedServiceType aServiceType )
    {
    DRM::CDrmAutomatedUsageData* data(
        DRM::CDrmAutomatedUsageData::NewLC(
                            aUniqueId,
                            aIntent,
                            aAutomatedType,
                            NULL,
                            aServiceType,
                            DRM::CDrmAutomatedUsageData::ERemoveAutomated ) );

    User::LeaveIfError( DoRemoveAutomated( data ) );

    CleanupStack::PopAndDestroy( data );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageImpl::RemoveAutomatedAsyncL
// Removes content from automated use
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmAutomatedUsageImpl::RemoveAutomatedAsyncL(
    const TDesC& aUniqueId,
    const ContentAccess::TIntent aIntent,
    const TDrmAutomatedType aAutomatedType,
    MDrmAsyncObserver& aObserver,
    const TDrmAutomatedServiceType aServiceType )
    {
    DRM::CDrmAutomatedUsageData* data(
        DRM::CDrmAutomatedUsageData::NewL(
                            aUniqueId,
                            aIntent,
                            aAutomatedType,
                            &aObserver,
                            aServiceType,
                            DRM::CDrmAutomatedUsageData::ERemoveAutomated ) );

    iDrmQueue->AppendToQueueL( data );

    TRequestStatus* status( &iStatus );

    Activate( status );

    return data->iOperationId;
    }

#pragma mark -
// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageImpl::RegisterEventObserverL
// Registers the client to listen for automated usage events
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmAutomatedUsageImpl::RegisterEventObserverL(
    DRM::MDrmAutomatedUsageObserver& aObserver )
    {
    if ( iAuObserver )
        {
        User::Leave( KErrInUse );
        }
    iAuObserver = &aObserver;
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageImpl::UnregisterEventObserverL
// Registers the client to listen for automated usage events
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmAutomatedUsageImpl::UnregisterEventObserverL(
    DRM::MDrmAutomatedUsageObserver& aObserver )
    {
    if ( iAuObserver == &aObserver )
        {
        iAuObserver=NULL;
        }
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageImpl::CancelOperation
// Cancels the async operation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TInt DRM::CDrmAutomatedUsageImpl::CancelOperation( TInt aOperationId )
    {
    TInt returnValue( KErrNotFound );
    DRM::CDrmAutomatedUsageData* data( iDrmQueue->PopItem( aOperationId ) );

    if ( data )
        {
        data->iObserver->OperationCompleted( aOperationId, KErrCancel );
        delete data;
        returnValue = KErrNone;
        }

    return returnValue;
    }
#pragma mark -
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
EXPORT_C TInt DRM::CDrmAutomatedUsageImpl::HandleErrorAsyncL(
    RFile& /* aFile */,
    ContentAccess::TIntent /* aIntent */,
    TInt /* aError */,
    DRM::MDrmHandleErrorObserver& /* aObserver */ )
    {
    User::Leave( KErrNotSupported );
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
EXPORT_C void DRM::CDrmAutomatedUsageImpl::HandleErrorL(
    RFile& /* aFile */,
    ContentAccess::TIntent /* aIntent */,
    TInt /* aError */,
    DRM::MDrmHandleErrorObserver* /* aObserver */ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
EXPORT_C TInt DRM::CDrmAutomatedUsageImpl::HandleErrorAsyncL(
    ContentAccess::CData& /* aFile */,
    ContentAccess::TIntent /* aIntent */,
    TInt /* aError */,
    DRM::MDrmHandleErrorObserver& /* aObserver */ )
    {
    User::Leave( KErrNotSupported );
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
EXPORT_C void DRM::CDrmAutomatedUsageImpl::HandleErrorL(
    ContentAccess::CData& /* aFile */,
    ContentAccess::TIntent /* aIntent */,
    TInt /* aError */,
    DRM::MDrmHandleErrorObserver* /* aObserver */ )
    {
    }
#pragma mark -

// -----------------------------------------------------------------------------
// CDrmAutomatedUsageImpl::DoSetAutomatedL
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmAutomatedUsageImpl::DoSetAutomatedL(
    DRM::CDrmAutomatedUsageData* aAutomatedUsageData )
    {
    if ( DoCanSetAutomatedL(
            aAutomatedUsageData->iUniqueId8->Des(),
            aAutomatedUsageData->iIntent,
            aAutomatedUsageData->iAutomatedType,
            ETrue ) )
        {
        return iDrmHelperClient.SetAutomated(
            aAutomatedUsageData->iUniqueId8->Des(),
            aAutomatedUsageData->iAutomatedType,
            aAutomatedUsageData->iIntent,
            aAutomatedUsageData->iServiceType );
        }
    else
        {
        return KErrCancel;
        }
    }

#pragma mark -

// -----------------------------------------------------------------------------
// CDrmAutomatedUsageImpl::DoCanSetAutomatedL
// -----------------------------------------------------------------------------
//
TBool DRM::CDrmAutomatedUsageImpl::DoCanSetAutomatedL(
    const TDesC8& aUniqueId8,
    const ContentAccess::TIntent aIntent,
    const DRM::TDrmAutomatedType aAutomatedType,
    const TBool aUseQueries )
    {
    TBool canSetAutomated( EFalse );
    CDRMPermission* activePermission( NULL );
    TUint32 rejection( KErrNone );
    CDRMConstraint* constraint( NULL );
    CDRMConstraint* toplevel( NULL );

    // First, fetch active permission from rights server
    activePermission = iOmaClient.GetActiveRightsL(
        aIntent, aUniqueId8, rejection );

    if ( !activePermission )
        {
        // No permission found so not possible to set as automated
        return EFalse;
        }

    CleanupStack::PushL( activePermission );

    toplevel = activePermission->TopLevelConstraint();
    constraint = activePermission->ConstraintForIntent( aIntent );

    if ( toplevel )
        {
        constraint->Merge( *toplevel );
        }

    // Active permission for given intent must have valid constraint
    // and it must not be count or accumulated time based
    canSetAutomated = !HasCountOrAccumulated( constraint );

    if ( canSetAutomated && aAutomatedType == DRM::EAUAutomatedTypeTheme )
        {
        // Automated type is theme so must check whether active RO suits
        // for theme use
        canSetAutomated = DoCanSetAutomatedThemeL( aUniqueId8 );
        }

    if ( canSetAutomated && aUseQueries )
        {
        canSetAutomated = iDrmUtilityUi->SetAutomatedQueryL( constraint );
        }
    CleanupStack::PopAndDestroy( activePermission );
    return canSetAutomated;
    }

// -----------------------------------------------------------------------------
// CDrmAutomatedUsageImpl::DoCanSetAutomatedThemeL
// -----------------------------------------------------------------------------
//
TBool DRM::CDrmAutomatedUsageImpl::DoCanSetAutomatedThemeL(
    const TDesC8& aUniqueId8 )
    {
    TBool canSetAutomatedTheme( ETrue );
    TUint32 rejection( KErrNone );
    CDRMPermission* playPermission( NULL );
    CDRMConstraint* playConstraint( NULL );
    CDRMConstraint* playToplevel( NULL );

    CDRMPermission* displayPermission( NULL );
    CDRMConstraint* displayConstraint( NULL );
    CDRMConstraint* displayToplevel( NULL );

    CDRMPermission* executePermission( NULL );
    CDRMConstraint* executeConstraint( NULL );
    CDRMConstraint* executeToplevel( NULL );

    //Check are there active play rights
    playPermission = iOmaClient.GetActiveRightsL(
        ContentAccess::EPlay, aUniqueId8, rejection );

    if( !playPermission )
        {
        return EFalse;
        }

    CleanupStack::PushL( playPermission );

    //Check are there active view rights
    displayPermission = iOmaClient.GetActiveRightsL(
        ContentAccess::EView, aUniqueId8, rejection );

    if( !displayPermission )
        {
        CleanupStack::PopAndDestroy( playPermission );
        return EFalse;
        }

    CleanupStack::PushL( displayPermission );

    //Check are there active execute rights
    executePermission = iOmaClient.GetActiveRightsL(
        ContentAccess::EExecute, aUniqueId8, rejection );

    if( !executePermission )
        {
        CleanupStack::PopAndDestroy( 2 ); //playPermission, displayPermission
        return EFalse;
        }

    CleanupStack::PushL( executePermission );

    //If Play, View and Execute rights are valid, test constraints
    playConstraint =
        playPermission->ConstraintForIntent( ContentAccess::EPlay );
    playToplevel = playPermission->TopLevelConstraint();
    if ( playToplevel )
        {
        playConstraint->Merge( *playToplevel );
        }

    displayConstraint =
        displayPermission->ConstraintForIntent( ContentAccess::EView );
    displayToplevel = displayPermission->TopLevelConstraint();
    if ( displayToplevel )
        {
        displayConstraint->Merge( *displayToplevel );
        }

    executeConstraint =
        executePermission->ConstraintForIntent( ContentAccess::EExecute );
    executeToplevel = executePermission->TopLevelConstraint();
    if ( executeToplevel )
        {
        executeConstraint->Merge( *executeToplevel );
        }

    // Check that all the constraints are not statefull
    // from stateful ROs, uncactivated interval is ok.
    if ( HasCountOrAccumulated( playConstraint ) ||
         HasCountOrAccumulated( displayConstraint ) ||
         HasCountOrAccumulated( executeConstraint ) )
        {
        canSetAutomatedTheme = EFalse;
        }

    // For end time based, end times must match
    if ( canSetAutomatedTheme &&
         ( HasEndTime( playConstraint ) ||
           HasEndTime( displayConstraint ) ||
           HasEndTime( executeConstraint ) ) )
        {
        canSetAutomatedTheme =
            HasEndTime( playConstraint ) &&
            HasEndTime( displayConstraint ) &&
            HasEndTime( executeConstraint ) &&
            EndTimesEqual( playConstraint, displayConstraint ) &&
            EndTimesEqual( playConstraint, executeConstraint );
        }

    // For Interval based, intervals and interval activation times must match
    if ( canSetAutomatedTheme &&
         ( HasInterval( playConstraint ) ||
           HasInterval( displayConstraint ) ||
           HasInterval( executeConstraint ) ) )
        {
        canSetAutomatedTheme =
            HasInterval( playConstraint ) &&
            HasInterval( displayConstraint ) &&
            HasInterval( executeConstraint ) &&
            IntervalsEqual( playConstraint, displayConstraint ) &&
            IntervalsEqual( playConstraint, executeConstraint );
        }

    CleanupStack::PopAndDestroy( 3 ); //playPermission, viewPermission,
    //executePermission

    return canSetAutomatedTheme;
    }

#pragma mark -

// -----------------------------------------------------------------------------
// CDrmAutomatedUsageImpl::DoRemoveAutomated
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmAutomatedUsageImpl::DoRemoveAutomated(
    DRM::CDrmAutomatedUsageData* aAutomatedUsageData )
    {
    return iDrmHelperClient.RemoveAutomated(
        aAutomatedUsageData->iUniqueId8->Des(),
        aAutomatedUsageData->iAutomatedType,
        aAutomatedUsageData->iIntent,
        aAutomatedUsageData->iServiceType );
    }



// -----------------------------------------------------------------------------
// CDrmAutomatedUsageImpl::DoCancel
// -----------------------------------------------------------------------------
//
void DRM::CDrmAutomatedUsageImpl::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CDrmAutomatedUsageImpl::RunL
// -----------------------------------------------------------------------------
//
void DRM::CDrmAutomatedUsageImpl::RunL()
    {
    DRM::CDrmAutomatedUsageData* data( iDrmQueue->PopFront() );
    TRequestStatus *status( &iStatus );

    if ( !data )
        {
        return;
        }

    CleanupStack::PushL( data );

    // Take this into the "current" variable in case an error occurs
    iObserver = data->iObserver;
    iOperationId = data->iOperationId;

    TInt err( KErrNone );

    switch ( data->iOperation )
        {
        case DRM::CDrmAutomatedUsageData::ESetAutomated:

            err = DoSetAutomatedL( data );

            break;

        case DRM::CDrmAutomatedUsageData::ERemoveAutomated:

            err = DoRemoveAutomated( data );

            break;

        default:

            err = KErrArgument;

            break;
        }

    iObserver->OperationCompleted( iOperationId, err );

    CleanupStack::PopAndDestroy( data );

    // Get ready for another round:
    SetActive();

    // complete internal request:
    User::RequestComplete( status, KErrNone );
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageImpl::RunError
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmAutomatedUsageImpl::RunError(
    TInt aError )
    {
    iObserver->OperationCompleted( iOperationId, aError );
    iObserver = NULL;
    iOperationId = NULL;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmAutomatedUsageImpl::Activate
// -----------------------------------------------------------------------------
//
void DRM::CDrmAutomatedUsageImpl::Activate( TRequestStatus*& aStatus )
    {
    if ( !IsAdded() )
        {
        CActiveScheduler::Add( this );
        }

    if ( !IsActive() )
        {
        SetActive();

        // complete internal request:
        User::RequestComplete( aStatus, KErrNone );
        }
    }

//  End of File
