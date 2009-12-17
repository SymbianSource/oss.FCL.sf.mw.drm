/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Datatype for the Metering Database Data
*
*/


// INCLUDE FILES
#include "drmmeteringdbdata.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDrmMeteringDbData::NewLC
// Constructor. Leaves the object in the cleanup stack.
// -----------------------------------------------------------------------------
//
CDrmMeteringDbData* CDrmMeteringDbData::NewLC()
    {
    CDrmMeteringDbData* self = new ( ELeave ) CDrmMeteringDbData();
    CleanupStack::PushL( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CDrmMeteringDbData::NewL
// Constructor
// -----------------------------------------------------------------------------
//
CDrmMeteringDbData* CDrmMeteringDbData::NewL()
    {
    CDrmMeteringDbData* self = NewLC();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// Default Constructor - First phase
// Can be used by itself to generate an empty object
// -----------------------------------------------------------------------------
//
CDrmMeteringDbData::CDrmMeteringDbData():
    iContentId( NULL ),
    iCount( 0 ),
    iAccumulatedTime( 0 ),
    iPermission( ContentAccess::EPlay ),
    iParentUid( NULL )
    {
    iRiId.SetLength( KRiIdSize );
    iRiId.Fill( 0 );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CDrmMeteringDbData::~CDrmMeteringDbData()
    {
    if( iContentId )
        {
        delete iContentId;
        iContentId = NULL;
        }

    if( iParentUid )
        {
        delete iParentUid;
        iParentUid = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CDrmMeteringDbData::ConstructL
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CDrmMeteringDbData::ConstructL()
    {
    }

// End of File
