/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the Camese Utility class
*
*/


// INCLUDES
#include <es_enum.h>
#include <cameseutility.h>
#include <e32def.h>

#include "cameselog.h"

// ----------------------------------------------------------------------------
// CCameseUtility::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CCameseUtility* CCameseUtility::NewL(
    MCameseObserver* aObserver /* = NULL */ )
    {
    CAMESE_LOG( "+CCameseUtility::NewL" );

    CCameseUtility* self = new (ELeave) CCameseUtility();
    CleanupStack::PushL( self );
    self->ConstructL( 0, aObserver );
    CleanupStack::Pop( self );

    CAMESE_LOG( "-CCameseUtility::NewL" );
    return self;
    }

// ----------------------------------------------------------------------------
// CCameseUtility::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CCameseUtility* CCameseUtility::NewL(
    TInt aIapNumber,
    MCameseObserver* aObserver /* = NULL */ )
    {
    CAMESE_LOG( "+CCameseUtility::NewL" );

    CCameseUtility* self = new (ELeave) CCameseUtility();
    CleanupStack::PushL( self );
    self->ConstructL( aIapNumber, aObserver );
    CleanupStack::Pop( self );

    CAMESE_LOG( "-CCameseUtility::NewL" );
    return self;
    }
// ----------------------------------------------------------------------------
// CCameseUtility::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CCameseUtility* CCameseUtility::NewL(
    TInt /*aSockServHandle*/,
    RConnection* aConnPtr,
    MCameseObserver* aObserver /* = NULL */ )
    {
    CAMESE_LOG( "+CCameseUtility::NewL" );

    CCameseUtility* self = new (ELeave) CCameseUtility();
    CleanupStack::PushL( self );

    TInt connectionIapId( 0 );
     
    if ( aConnPtr )
        {
        TConnectionInfo connectionInfo;
        TUint count( 0 );
        User::LeaveIfError( aConnPtr->EnumerateConnections( count ) );

        if ( !count )
            {
            User::Leave( KErrNotReady );
            }
      
        TPckg<TConnectionInfo> pkg( connectionInfo );
        User::LeaveIfError( aConnPtr->GetConnectionInfo( 1, pkg ) );
        
        connectionIapId = connectionInfo.iIapId;
        }
        
    self->ConstructL( connectionIapId, aObserver );
    CleanupStack::Pop( self );

    CAMESE_LOG( "-CCameseUtility::NewL" );
    return self;
    }

// ----------------------------------------------------------------------------
// CCameseUtility::~CCameseUtility
// ----------------------------------------------------------------------------
//
EXPORT_C CCameseUtility::~CCameseUtility()
    {
    CAMESE_LOG( "+CCameseUtility::~CCameseUtility" );

    CAMESE_LOG( "-CCameseUtility::~CCameseUtility" );
    }

// ----------------------------------------------------------------------------
// CCameseUtility::AcquireLicense
// ----------------------------------------------------------------------------
//
EXPORT_C void CCameseUtility::AcquireLicense(
    const TDesC8& /*aDrmHeader*/,
    HBufC8*& /*aUrl*/,
    TRequestStatus& aStatus )
    {
    CAMESE_LOG( "+CCameseUtility::AcquireLicense" );

    aStatus = KRequestPending;
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNotSupported );

    CAMESE_LOG( "-CCameseUtility::AcquireLicense" );
    }

// ----------------------------------------------------------------------------
// CCameseUtility::AcquireLicense
// ----------------------------------------------------------------------------
//
EXPORT_C void CCameseUtility::AcquireLicense(
    const RFile& /*aFileHandle*/,
    HBufC8*& /*aUrl*/,
    TRequestStatus& aStatus )
    {
    CAMESE_LOG( "+CCameseUtility::AcquireLicense" );

    aStatus = KRequestPending;
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNotSupported );

    CAMESE_LOG( "-CCameseUtility::AcquireLicense" );
    }

// ----------------------------------------------------------------------------
// CCameseUtility::AcquireLicense
// ----------------------------------------------------------------------------
//
EXPORT_C void CCameseUtility::AcquireLicense(
    const RFile& /*aFileHandle*/,
    const TDesC8& /*aLicenseResponse*/,
    TRequestStatus& aStatus )
    {
    CAMESE_LOG( "+CCameseUtility::AcquireLicense" );

    aStatus = KRequestPending;
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNotSupported );

    CAMESE_LOG( "-CCameseUtility::AcquireLicense" );
    }


// ----------------------------------------------------------------------------
// CCameseUtility::SendDownloadResult
// ----------------------------------------------------------------------------
//
EXPORT_C void CCameseUtility::SendDownloadResult(
    const TDesC8& /*aContentUrl*/,
    TInt /*aResult*/,
    TRequestStatus& aStatus )
    {
    CAMESE_LOG( "+CCameseUtility::SendDownloadResult" );

    aStatus = KRequestPending;
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNotSupported );

    CAMESE_LOG( "-CCameseUtility::SendDownloadResult" );
    }

// ----------------------------------------------------------------------------
// CCameseUtility::Cancel
// ----------------------------------------------------------------------------
//
EXPORT_C void CCameseUtility::Cancel()
    {
    CAMESE_LOG( "+CCameseUtility::Cancel" );

    CAMESE_LOG( "-CCameseUtility::Cancel" );
    }

// ----------------------------------------------------------------------------
// CCameseUtility::SetIapId
// ----------------------------------------------------------------------------
//
EXPORT_C void CCameseUtility::SetIapId( TInt /*aIapId*/ )
    {
    }

// ----------------------------------------------------------------------------
// CCameseUtility::CCameseUtility
// ----------------------------------------------------------------------------
//
CCameseUtility::CCameseUtility()
    {
    CAMESE_LOG( "+CCameseUtility::CCameseUtility" );
    CAMESE_LOG( "-CCameseUtility::CCameseUtility" );
    }

// ----------------------------------------------------------------------------
// CCameseUtility::ConstructL
// ----------------------------------------------------------------------------
//
void CCameseUtility::ConstructL( TInt /*aIapNumber*/, MCameseObserver* /*aObserver*/ )
    {
    CAMESE_LOG( "+CCameseUtility::ConstructL" );

    CAMESE_LOG( "-CCameseUtility::ConstructL" );
    }
