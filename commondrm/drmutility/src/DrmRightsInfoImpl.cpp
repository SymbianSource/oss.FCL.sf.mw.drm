/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DRM Rights Information handling implementation class
*
*/



// INCLUDE FILES
#include <caf/caftypes.h>
#include <Oma2Agent.h>
#include <utf.h>
#include <drmutilitytypes.h>
#include <drmasyncobserver.h>

#include "DrmRightsInfoImpl.h"
#include "DrmRightsInfoData.h"
#include "drmutilitycommon.h"

// CONSTANTS
const TInt KCommandCheckRights( 1 );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDrmRightsInfoImpl::CDrmRightsInfoImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
DRM::CDrmRightsInfoImpl::CDrmRightsInfoImpl(): CActive( EPriorityStandard )
    {
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfoImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void DRM::CDrmRightsInfoImpl::ConstructL()
    {
    iDrmUtilityCommon = DRM::CDrmUtilityCommon::NewL();

    User::LeaveIfError( iOmaClient.Connect() );

    User::LeaveIfError( iSemaphore.CreateLocal( 1 ) );
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfoImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmRightsInfoImpl* DRM::CDrmRightsInfoImpl::NewL()
    {
    DRM::CDrmRightsInfoImpl* self( NewLC() );

    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfoImpl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmRightsInfoImpl* DRM::CDrmRightsInfoImpl::NewLC()
    {
    DRM::CDrmRightsInfoImpl* self( new( ELeave ) CDrmRightsInfoImpl );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }


// Destructor
DRM::CDrmRightsInfoImpl::~CDrmRightsInfoImpl()
    {
    DRM::CDrmRightsInfoData* data( PopFront() );

    // Empty the queue:
    while ( data )
        {
        // Complete the outstanding requestest with cancel
        data->iObserver->OperationCompleted( data->iOperationId, KErrCancel );
        delete data;
        data = PopFront();
        }

    delete iDrmUtilityCommon;

    iOmaClient.Close();

    // Close the semaphone
    iSemaphore.Close();

    // Remove the object from active scheduler etc.
    if ( IsAdded() )
        {
        Deque();
        }
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfo::CheckRightsL
// Syncronous method
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmRightsInfoImpl::CheckRightsL(
    const TDesC& aUniqueId,
    ContentAccess::TIntent aIntent,
    DRM::TDrmRightsInfo& aDetails )
    {
    DRM::CDrmRightsInfoData* data( DRM::CDrmRightsInfoData::NewLC() );

    data->iUniqueId = aUniqueId.AllocL();
    TRAPD( err, data->iUniqueId8 =
                CnvUtfConverter::ConvertFromUnicodeToUtf8L( aUniqueId ) );
    if ( err )
        {
        data->iUniqueId8 = NULL;
        }
    data->iIntent = aIntent;
    data->iDetails = &aDetails;

    PerformCheckRightsL( *data );

    CleanupStack::PopAndDestroy( data );
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfo::CheckRightsAsyncL
// Asyncronous method
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmRightsInfoImpl::CheckRightsAsyncL(
    const TDesC& aUniqueId,
    ContentAccess::TIntent aIntent,
    DRM::TDrmRightsInfo& aDetails,
    DRM::MDrmAsyncObserver& aObserver )
    {
    // Create Queue object:
    DRM::CDrmRightsInfoData* data( DRM::CDrmRightsInfoData::NewLC() );
    TRequestStatus* status( &iStatus );

    data->iObserver = &aObserver;
    data->iOperationId = reinterpret_cast<TInt>( data );
    data->iOperation = KCommandCheckRights;
    data->iUniqueId = aUniqueId.AllocL();
    TRAPD( err, data->iUniqueId8 =
                CnvUtfConverter::ConvertFromUnicodeToUtf8L( aUniqueId ) );
    if ( err )
        {
        data->iUniqueId8 = NULL;
        }
    data->iIntent = aIntent;
    data->iDetails = &aDetails;

    // Add it to the queue:
    AppendToQueue( data );

    // Pop it from the cleanup stack
    CleanupStack::Pop( data );

    if( !IsAdded() )
        {
        CActiveScheduler::Add( this );
        }

    if ( !IsActive() )
        {
        SetActive();

        // complete internal request:
        User::RequestComplete( status, KErrNone );

        }
    return data->iOperationId;
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfoImpl::CancelOperation
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmRightsInfoImpl::CancelOperation( TInt aOperationId )
    {
    // Critical area start:
    iSemaphore.Wait();

    TBool found( EFalse );

    DRM::CDrmRightsInfoData* previousData( NULL );
    DRM::CDrmRightsInfoData* currentData( iFirst );

    while ( currentData )
        {
        if ( currentData->iOperationId == aOperationId )
            {
            found = ETrue;

            //Update the previous link
            if ( previousData )
                {
                previousData->iNext = currentData->iNext;
                }
            else
                {
                iFirst = currentData->iNext;
                }

            //If the current is the only one, set iLast NULL
            if ( !iFirst )
                {
                iLast = NULL;
                }
            //If only one left, set it also to be the iLast
            else if ( iFirst && !currentData->iNext )
                {
                iLast = iFirst;
                }

            //Delete the cancelled operation
            currentData->iObserver->OperationCompleted( currentData->iOperationId, KErrCancel );
            delete currentData;
            currentData = NULL;
            }
        else
            {
            previousData = currentData;
            currentData = currentData->iNext;
            }
        }

    if( !found )
        {
        // Critical Area End
        iSemaphore.Signal();
        return KErrNotFound;
        }
    // Critical Area End
    iSemaphore.Signal();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfoImpl::DoCancel
// -----------------------------------------------------------------------------
//
void DRM::CDrmRightsInfoImpl::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfoImpl::RunL
// -----------------------------------------------------------------------------
//
void DRM::CDrmRightsInfoImpl::RunL()
    {
    DRM::CDrmRightsInfoData* data( PopFront() );
    TRequestStatus* status( &iStatus );

    if( !data )
        {
        return;
        }

    CleanupStack::PushL( data );

    switch ( data->iOperation )
        {
        case KCommandCheckRights:

          // Take this into the "current" variable in case an error occurs
          iObserver = data->iObserver;
          iOperationId = data->iOperationId;

          // Perform the operation
          PerformCheckRightsL( *data );

          // Compelete the client request
          iObserver->OperationCompleted( iOperationId, KErrNone );

          break;
        default:
          // Do nothing
          break;
        }

    // destroy the object:
    CleanupStack::PopAndDestroy( data );


    // Get ready for another round:
    SetActive();

    // complete internal request:
    User::RequestComplete( status, KErrNone );
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfoImpl::RunError
// If an error occurs complete the async request with it
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmRightsInfoImpl::RunError( TInt aError )
    {
    iObserver->OperationCompleted( iOperationId, aError );
    iObserver = NULL;
    iOperationId = NULL;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfoImpl::PerformCheckRightsL
// -----------------------------------------------------------------------------
//
void DRM::CDrmRightsInfoImpl::PerformCheckRightsL(
    DRM::CDrmRightsInfoData& aData )
    {
    // Check for Oma Drm Rights, if not found, check for WmDrm Rights
    TInt omaerror( KErrNone );
    TInt wmerror( KErrNone );
    TUint32 reason( 0 );

    // if the outcome is KErrCANoRights we have the check both:
    omaerror = iOmaClient.CheckRights( aData.iIntent, *aData.iUniqueId8,  reason );
    if ( omaerror == KErrCANoRights )
        {
        *aData.iDetails = DRM::EURightsInfoMissing;
        // Check Also WmDrm
        // wmerror = check rights for WMDRM
        // Show info for wm drm error
        if ( !wmerror )
            {

            }
        }
    else
        {
        if ( !omaerror )
            {
            *aData.iDetails = DRM::EURightsInfoValid;
            }
        else
            {
            *aData.iDetails = iDrmUtilityCommon->GetOmaRightsStatusL(
                                                        aData.iUniqueId8,
                                                        aData.iIntent,
                                                        NULL );
            }
        }
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfoImpl::AppendToQueue
// -----------------------------------------------------------------------------
//
void DRM::CDrmRightsInfoImpl::AppendToQueue( DRM::CDrmRightsInfoData* aData )
    {
    // Critical area start:
    iSemaphore.Wait();

    if ( !iLast )
        {
        iFirst = aData;
        iLast = iFirst;
        }
    else
        {
        iLast->iNext = aData;
        iLast = aData;
        }

    // Critical area end
    iSemaphore.Signal();
    }

// -----------------------------------------------------------------------------
// CDrmRightsInfoImpl::PopFront
// -----------------------------------------------------------------------------
//
DRM::CDrmRightsInfoData* DRM::CDrmRightsInfoImpl::PopFront()
    {
    DRM::CDrmRightsInfoData* data = NULL;

    // If there is nothing in the queue return NULL
    if ( !iFirst )
        {
        return data;
        }

    // Critical area start:
    iSemaphore.Wait();

    data = iFirst;

    // Set the first and the next
    iFirst = iFirst->iNext;

    // reset the pointer on data
    data->iNext = NULL;

    // if there is no next in iFirst, this is the last or if there is no iFirst
    if ( !iFirst || !iFirst->iNext )
        {
        iLast = iFirst;
        }

    // Critical Area End
    iSemaphore.Signal();

    return data;
    }

//  End of File
