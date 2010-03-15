/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the parent storage for Decision Making Machine
*
*/


// INCLUDE FILES
#include "drmparentstorage.h"
#include "DrmPermission.h"
#include "drmlog.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
LOCAL_C const TUint KDefaultGranularity = 4;

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMParentStorage::CDRMParentStorage
//
// Default constructor
// -----------------------------------------------------------------------------
//
CDRMParentStorage::CDRMParentStorage():
iParents( 1 )
    {
    }

// -----------------------------------------------------------------------------
// CDRMParentStorage::~CDRMParentStorage
//
// Desetructor.
// -----------------------------------------------------------------------------
//
CDRMParentStorage::~CDRMParentStorage()
    {
    if ( iParentIDs )
        {
        iParentIDs->Reset();
        delete iParentIDs;
        }

    // The lists inside are autocleaning, so deletion causes
    // the contents to also be deleted
    iParents.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CDRMParentStorage::NewLC
//
// Two-phase constructor
// -----------------------------------------------------------------------------
//
CDRMParentStorage* CDRMParentStorage::NewLC()
    {
    CDRMParentStorage* self = new( ELeave ) CDRMParentStorage;
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// -----------------------------------------------------------------------------
// CDRMParentStorage::NewL
//
// Two-phase constructor
// -----------------------------------------------------------------------------
//
CDRMParentStorage* CDRMParentStorage::NewL()
    {
    CDRMParentStorage* self = CDRMParentStorage::NewLC();
    CleanupStack::Pop(); // sefl

    return self;
    }

// -----------------------------------------------------------------------------
// CDRMParentStorage::ConstructL
//
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CDRMParentStorage::ConstructL()
    {
    iParentIDs = new( ELeave ) CDesC8ArraySeg( KDefaultGranularity );
    }

// -----------------------------------------------------------------------------
// CDRMParentStorage::NewListL
//
// Insert a new list to the storage, and return a handle to it.
// -----------------------------------------------------------------------------
//
CDRMPermissionList& CDRMParentStorage::NewListL()
    {
    __ASSERT_DEBUG( iParents.Count() >= iParentIDs->Count(), User::Invariant() );

    if ( iParents.Count() == iParentIDs->Count() )
        {
        // Balanced lists.
        CDRMPermissionList* newList = CDRMPermissionList::NewLC();
        newList->SetAutoCleanup( ETrue );

        iParents.AppendL( newList );
        CleanupStack::Pop();
        }

    return *( iParents[ iParents.Count() - 1 ] );
    }

// -----------------------------------------------------------------------------
// CDRMParentStorage::AddL
//
// Add an entry to the ID table.
// -----------------------------------------------------------------------------
//
void CDRMParentStorage::AddL( const TDesC8& aCID )
    {
    TInt pos = iParentIDs->InsertIsqL( aCID );
    if ( pos != iParentIDs->Count() - 1 )
        {
        // Sort iParents
        CDRMPermissionList* list = iParents[ iParents.Count() - 1 ];
        User::LeaveIfError( iParents.Insert( list, pos ) );
        iParents.Remove( iParents.Count() -1 );
        }
    }

// -----------------------------------------------------------------------------
// CDRMParentStorage::HasPermissions
//
// Check the count of the stored permissions for an id
// -----------------------------------------------------------------------------
//
TBool CDRMParentStorage::HasPermissions( const TDesC8& aCID )
    {
    TBool r = EFalse;
    TInt pos;

    if ( iParentIDs->FindIsq( aCID, pos ) == KErrNone )
        {
        r = ETrue;
        }
    return r;
    }

// -----------------------------------------------------------------------------
// CDRMParentStorage::operator[]
//
// Operator.
// -----------------------------------------------------------------------------
//
CDRMPermissionList& CDRMParentStorage::operator[]( const TDesC8& aCID )
    {
    TInt pos;

    iParentIDs->FindIsq( aCID, pos );
    return *( iParents[ pos ] );
    }

// End of File
