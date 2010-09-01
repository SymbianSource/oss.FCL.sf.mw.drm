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
* Description:  Model for DRMSettinsPlugin.
*
*/


// INCLUDE FILES
#include <utf.h>
#include <commdb.h>
#include <featmgr.h>

#include "drmsettingsmodel.h"
#include "drmsettingsplugininternalcrkeys.h"
#include "DRMRIContext.h"

// CONSTANTS
#ifdef _DEBUG
// debug panic
_LIT( KDRMSettingsDebugPanicMessage, "DrmSettingsDebugPanic" );
const TInt KDRMSettingsDebugPanicCode( 1 );
#endif

// ================= MEMBER FUNCTIONS =======================


// ----------------------------------------------------------------------------
// CDRMSettingsModel::NewL
//
// EPOC two-phased constructor
// ----------------------------------------------------------------------------
//
CDRMSettingsModel* CDRMSettingsModel::NewL()
    {
    CDRMSettingsModel* self = new( ELeave ) CDRMSettingsModel;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ----------------------------------------------------------------------------
// CDRMSettingsModel::CDRMSettingsModel
//
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CDRMSettingsModel::CDRMSettingsModel()
    {
    }


// ----------------------------------------------------------------------------
// CDRMSettingsModel::ConstructL
//
// EPOC default constructor can leave.
// ----------------------------------------------------------------------------
//
void CDRMSettingsModel::ConstructL()
    {
    // create an instance of Central Repository
    iDRMSettingsRepository = CRepository::NewL( KCRUidDRMSettings );

#ifdef RD_DRM_METERING

    // Create an instance of roap storage client
    iRoapStorageClient = new (ELeave) Roap::RRoapStorageClient;

    // Connect to server
    User::LeaveIfError( iRoapStorageClient->Connect() );

    // Fill the list
    iRoapStorageClient->GetAllRIContextsL( iRIContextList );

#endif // RD_DRM_METERING

    }


// ----------------------------------------------------------------------------
// CDRMSettingsModel::~CDRMSettingsModel
//
// Destructor
// ----------------------------------------------------------------------------
//
CDRMSettingsModel::~CDRMSettingsModel()
    {
    if ( iDRMSettingsRepository )
        {
        delete iDRMSettingsRepository;
        }
    if ( iRoapStorageClient )
        {
        iRoapStorageClient->Close();
        delete iRoapStorageClient;
        }

    iRIContextList.ResetAndDestroy();
    iRIContextList.Close();
    }


// ---------------------------------------------------------------------------
// CDRMSettingsModel::SetTransactionTrackingStateL()
//
// Set transaction tracking state
// ---------------------------------------------------------------------------
//
void CDRMSettingsModel::SetTransactionTrackingStateL( TInt aValue )
    {
    User::LeaveIfError( iDRMSettingsRepository->
                                Set( KDRMSettingsTransactionTracking,
                                     aValue ) );
    }


// ---------------------------------------------------------------------------
// CDRMSettingsModel::TransactionTrackingStateL()
//
// Get transaction tracking state
// ---------------------------------------------------------------------------
//
TInt CDRMSettingsModel::TransactionTrackingStateL()
    {
    TInt value( KErrNone );

    User::LeaveIfError( iDRMSettingsRepository->Get(
                                    KDRMSettingsTransactionTracking,
                                    value ) );

    return value;
    }

// ---------------------------------------------------------------------------
// CDRMSettingsModel::SetAutomaticActivationStateL()
//
// Set automatic activation state
// ---------------------------------------------------------------------------
//
void CDRMSettingsModel::SetAutomaticActivationStateL( TInt aValue )
    {
    User::LeaveIfError( iDRMSettingsRepository->
                                Set( KDRMSettingsSilentRightsAcquisition,
                                     aValue ) );
    }


// ---------------------------------------------------------------------------
// CDRMSettingsModel::AutomaticActivationStateL()
//
// Get automatic activation state
// ---------------------------------------------------------------------------
//
TInt CDRMSettingsModel::AutomaticActivationStateL()
    {
    TInt value( KErrNone );

    User::LeaveIfError( iDRMSettingsRepository->Get(
                                KDRMSettingsSilentRightsAcquisition,
                                value ) );

    return value;
    }


// ---------------------------------------------------------------------------
// CDRMSettingsModel::UsageReportingCount()
//
// Get usage reporting state. Return count of allowed services.
// ---------------------------------------------------------------------------
//
TInt CDRMSettingsModel::UsageReportingCount()
    {
    TInt count( 0 );

    for ( TInt i( 0 ); i < iRIContextList.Count(); i++ )
        {
        if ( iRIContextList[i]->IsMeteringAllowed() )
            {
            count++;
            }
        }

    return count;
    }


// ---------------------------------------------------------------------------
// CDRMSettingsModel::GetSingleRIAliasL()
// ---------------------------------------------------------------------------
//
HBufC* CDRMSettingsModel::GetSingleRIAliasL( TInt aIndex )
    {
    __ASSERT_DEBUG( aIndex >= 0 ||
                    aIndex < iRIContextList.Count(),
                        User::Panic( KDRMSettingsDebugPanicMessage,
                                     KDRMSettingsDebugPanicCode ) );

    HBufC* alias( NULL );

    if ( &iRIContextList[aIndex]->RIAlias() )
        {
        alias =
            CnvUtfConverter::ConvertToUnicodeFromUtf8L(
                iRIContextList[aIndex]->RIAlias() );
        }
    else
        {
        alias =
            CnvUtfConverter::ConvertToUnicodeFromUtf8L(
                iRIContextList[aIndex]->RightsIssuerURL() );
        }

    return alias;
    }

// ---------------------------------------------------------------------------
// CDRMSettingsModel::GetFirstAllowedMeteringRIAliasL()
// ---------------------------------------------------------------------------
//
HBufC* CDRMSettingsModel::GetFirstAllowedMeteringRIAliasL()
    {
    for ( TInt i( 0 ); i < iRIContextList.Count(); i++ )
        {
        if( IsMeteringAllowed( i ) )
            {
            return GetSingleRIAliasL( i );
            }
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CDRMSettingsModel::GetRiContextCount()
// ---------------------------------------------------------------------------
//
TInt CDRMSettingsModel::GetRiContextCount()
    {
    return iRIContextList.Count();
    }

// ---------------------------------------------------------------------------
// CDRMSettingsModel::IsMeteringAllowed()
// ---------------------------------------------------------------------------
//
TBool CDRMSettingsModel::IsMeteringAllowed( TInt aIndex )
    {
    __ASSERT_DEBUG( aIndex >= 0 ||
                    aIndex < iRIContextList.Count(),
                        User::Panic( KDRMSettingsDebugPanicMessage,
                                     KDRMSettingsDebugPanicCode ) );

    return iRIContextList[aIndex]->IsMeteringAllowed();
    }

// ---------------------------------------------------------------------------
// CDRMSettingsModel::IsMeteringAllowedForAll()
// ---------------------------------------------------------------------------
//
TBool CDRMSettingsModel::IsMeteringAllowedForAll()
    {
    TBool isAllowed( ETrue );

    for ( TInt i( 0 ); i < iRIContextList.Count() && isAllowed; i++ )
        {
        if( !IsMeteringAllowed( i ) )
            {
            isAllowed = EFalse;
            }
        }

    return isAllowed;
    }

// ---------------------------------------------------------------------------
// CDRMSettingsModel::SetMeteringStatus()
// ---------------------------------------------------------------------------
//
void CDRMSettingsModel::SetMeteringStatus( TInt aIndex, TBool aIsAllowed )
    {
    __ASSERT_DEBUG( aIndex >= 0 ||
                    aIndex < iRIContextList.Count(),
                        User::Panic( KDRMSettingsDebugPanicMessage,
                                     KDRMSettingsDebugPanicCode ) );

    iRIContextList[aIndex]->SetMeteringStatus( aIsAllowed );
    }

// ---------------------------------------------------------------------------
// CDRMSettingsModel::SaveMeteringChanges()
// ---------------------------------------------------------------------------
//
void CDRMSettingsModel::SaveMeteringChanges()
    {
    for ( TInt i( 0 ); i < iRIContextList.Count(); i++ )
        {
        TRAP_IGNORE(
            iRoapStorageClient->UpdateRIContextL( *iRIContextList[i] ) );
        }
    }

//  End of File
