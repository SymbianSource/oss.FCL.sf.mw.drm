/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements consume data encapsulation
*
*/


// INCLUDE FILES
#include <e32std.h>
#include "ConsumeData.h"

// LOCAL CONSTANTS AND MACROS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConsumeData::ConsumeData
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CConsumeData::CConsumeData() : CBase()
    {
    }

// -----------------------------------------------------------------------------
// CConsumeData::~ConsumeData
// C++ default destructor
// -----------------------------------------------------------------------------
//
CConsumeData::~CConsumeData()
    {
    // If the connection is still open call finish
    if( iConnectionStatus && iContentURI8 )
        {
        iRdb.Consume( ContentAccess::EStop, *iContentURI8 );
        }

    // Close the connection to the server:
    iRdb.Close();

    // Reset the connection status
    iConnectionStatus = EFalse;

    // Delete the content id
    if( iContentURI8 )
        {
        delete iContentURI8;
        iContentURI8 = NULL;
        }
    }


// -----------------------------------------------------------------------------
// CConsumeData::NewLC
// 2-phase constructor
// -----------------------------------------------------------------------------
//
CConsumeData* CConsumeData::NewLC( const TDesC8& aUri )
    {
    CConsumeData* self = new (ELeave) CConsumeData();
    CleanupStack::PushL( self );
    self->ConstructL( aUri );
    return self;
    }

// -----------------------------------------------------------------------------
// CConsumeData::NewL
// 2-phase constructor
// -----------------------------------------------------------------------------
//
CConsumeData* CConsumeData::NewL( const TDesC8& aUri )
    {
    CConsumeData* self = NewLC( aUri );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CConsumeData::Consume
// -----------------------------------------------------------------------------
//
TInt CConsumeData::Consume( const ContentAccess::TIntent& aIntent )
    {
    TInt retVal = KErrNone;

    if( !iConnectionStatus )
        {
        return KErrNotReady;
        }

    retVal = iRdb.Consume( aIntent, *iContentURI8 );

    if( aIntent == ContentAccess::EStop )
        {
        iRdb.Close();
        iConnectionStatus = EFalse;
        }

    return retVal;
    }
// -----------------------------------------------------------------------------
// CConsumeData::CompareUri
// -----------------------------------------------------------------------------
//
TInt CConsumeData::CompareUri( const TDesC8& aUri ) const
    {
    if( !iContentURI8 )
        {
        return KErrNotReady;
        }

    return iContentURI8->Des().Compare( aUri );
    }

// -----------------------------------------------------------------------------
// CConsumeData::ConstructL
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CConsumeData::ConstructL( const TDesC8& aUri )
    {
    // Connect the rights client
    User::LeaveIfError( iRdb.Connect() );

    // Set the connection status
    iConnectionStatus = ETrue;

    // copy the url for internal use
    iContentURI8 = aUri.AllocL();
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File
