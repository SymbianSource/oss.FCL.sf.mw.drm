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
* Description:  Async command storage class for DRM::DrmAutomatedUsageImpl
*
*/



// INCLUDE FILES
#include    <utf.h>

#include "drmautomatedusagedata.h"

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageData::NewL
// Two-Phase constructor
// -----------------------------------------------------------------------------
//
DRM::CDrmAutomatedUsageData* DRM::CDrmAutomatedUsageData::NewL(
    const ContentAccess::CData& aData,
    ContentAccess::TIntent aIntent,
    DRM::TDrmAutomatedType aAutomatedType,
    DRM::MDrmAsyncObserver* aObserver,
    DRM::TDrmAutomatedServiceType aServiceType,
    TOperation aOperation )
    {
    DRM::CDrmAutomatedUsageData* self( NewLC( aData, 
                                              aIntent, 
                                              aAutomatedType, 
                                              aObserver, 
                                              aServiceType, 
                                              aOperation ) );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageData::NewLC
// Two-Phase constructor
// -----------------------------------------------------------------------------
//
DRM::CDrmAutomatedUsageData* DRM::CDrmAutomatedUsageData::NewLC(
    const ContentAccess::CData& aData,
    ContentAccess::TIntent aIntent,
    DRM::TDrmAutomatedType aAutomatedType,
    DRM::MDrmAsyncObserver* aObserver,
    DRM::TDrmAutomatedServiceType aServiceType,
    TOperation aOperation )
    {
    DRM::CDrmAutomatedUsageData* self(
        new (ELeave) DRM::CDrmAutomatedUsageData( aIntent, 
                                                  aAutomatedType, 
                                                  aObserver, 
                                                  aServiceType, 
                                                  aOperation ) );
    CleanupStack::PushL( self );
    self->ConstructL( aData );
    return self;
    }


// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageData::NewL
// Two-Phase constructor
// -----------------------------------------------------------------------------
//
DRM::CDrmAutomatedUsageData* DRM::CDrmAutomatedUsageData::NewL(
    const TDesC& aUniqueId,
    ContentAccess::TIntent aIntent,
    DRM::TDrmAutomatedType aAutomatedType,
    DRM::MDrmAsyncObserver* aObserver,
    DRM::TDrmAutomatedServiceType aServiceType,
    TOperation aOperation )
    {
    DRM::CDrmAutomatedUsageData* self( NewLC( aUniqueId, 
                                              aIntent, 
                                              aAutomatedType, 
                                              aObserver, 
                                              aServiceType,
                                              aOperation ) );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageData::NewLC
// Two-Phase constructor
// -----------------------------------------------------------------------------
//
DRM::CDrmAutomatedUsageData* DRM::CDrmAutomatedUsageData::NewLC(
    const TDesC& aUniqueId,
    ContentAccess::TIntent aIntent,
    DRM::TDrmAutomatedType aAutomatedType,
    DRM::MDrmAsyncObserver* aObserver,
    DRM::TDrmAutomatedServiceType aServiceType,
    TOperation aOperation )
    {
    DRM::CDrmAutomatedUsageData* self(
        new (ELeave) DRM::CDrmAutomatedUsageData( aIntent, 
                                                  aAutomatedType, 
                                                  aObserver, 
                                                  aServiceType, 
                                                  aOperation ) );
    CleanupStack::PushL( self );
    self->ConstructL( aUniqueId );
    return self;
    }


// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
DRM::CDrmAutomatedUsageData::CDrmAutomatedUsageData(
    ContentAccess::TIntent aIntent,
    DRM::TDrmAutomatedType aAutomatedType,
    DRM::MDrmAsyncObserver* aObserver,
    DRM::TDrmAutomatedServiceType aServiceType,
    TOperation aOperation ) : iOperation( aOperation ), 
                              iIntent( aIntent ),
                              iAutomatedType( aAutomatedType ), 
                              iObserver( aObserver ),
                              iServiceType( aServiceType )
    {
    }

// -----------------------------------------------------------------------------
// Copy constructor
// -----------------------------------------------------------------------------
//
DRM::CDrmAutomatedUsageData::CDrmAutomatedUsageData(
    const DRM::CDrmAutomatedUsageData& )
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
DRM::CDrmAutomatedUsageData::~CDrmAutomatedUsageData()
    {
    delete iUniqueId;
    delete iUniqueId8;
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageData::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
void DRM::CDrmAutomatedUsageData::ConstructL(
    const ContentAccess::CData& aData )
    {
    TPtr value( NULL, 0 );
    HBufC* uniqueId( HBufC::NewLC( ContentAccess::KMaxCafContentName ) );
    value.Set( uniqueId->Des() );

    User::LeaveIfError(aData.GetStringAttribute( ContentAccess::EContentID, 
                                                 value ) );

    iUniqueId8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( value );
    iUniqueId = uniqueId->AllocL();
    CleanupStack::PopAndDestroy( uniqueId );
    SetOperationId();
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageData::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
void DRM::CDrmAutomatedUsageData::ConstructL( const TDesC& aUniqueId )
    {
    iUniqueId = aUniqueId.AllocL();
    iUniqueId8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( aUniqueId );
    SetOperationId();
    }

// -----------------------------------------------------------------------------
// DRM::CDrmAutomatedUsageData::SetOperationId
// Calculates unique operationId related to this object's parameters
// -----------------------------------------------------------------------------
//
void DRM::CDrmAutomatedUsageData::SetOperationId()
    {
    iOperationId = reinterpret_cast< TInt >( this );
    }
//  End of File
