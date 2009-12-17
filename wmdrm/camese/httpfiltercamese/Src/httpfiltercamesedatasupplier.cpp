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
* Description:  Data supplier for responses
*
*/


// INCLUDE FILES
#include "httpfiltercamesedatasupplier.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpFilterCameseDataSupplier::CHttpFilterCameseDataSupplier
// C++ constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CHttpFilterCameseDataSupplier::CHttpFilterCameseDataSupplier(MHTTPDataSupplier* aOriginalDataSupplier)
      : iOriginalDataSupplier(aOriginalDataSupplier)
    {
    }

// -----------------------------------------------------------------------------
// CHttpFilterCameseDataSupplier::ConstructL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CHttpFilterCameseDataSupplier::ConstructL(const TDesC8& aData)
    {    
    iData = aData.AllocL();
    }

// -----------------------------------------------------------------------------
// CHttpFilterCameseDataSupplier::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpFilterCameseDataSupplier* CHttpFilterCameseDataSupplier::NewL( 
    MHTTPDataSupplier* aOriginalDataSupplier,
    const TDesC8& aData )
    {
    CHttpFilterCameseDataSupplier* self = 
        new(ELeave) CHttpFilterCameseDataSupplier( aOriginalDataSupplier );
    CleanupStack::PushL( self );
    self->ConstructL( aData );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CHttpFilterCameseDataSupplier::~CHttpFilterCameseDataSupplier()
    {
    delete iData;
    }

// -----------------------------------------------------------------------------
// CHttpFilterCameseDataSupplier::GetNextDataPart
// Return the next chunk of response body
// -----------------------------------------------------------------------------
//
TBool CHttpFilterCameseDataSupplier::GetNextDataPart(TPtrC8 &aDataChunk)
    {
    aDataChunk.Set(iData->Des());    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CHttpFilterCameseDataSupplier::ReleaseData
// Releases the current data part
// -----------------------------------------------------------------------------
//
void CHttpFilterCameseDataSupplier::ReleaseData()
    {
    if (iOriginalDataSupplier)
        {            
        iOriginalDataSupplier->ReleaseData();
        }
    }

// -----------------------------------------------------------------------------
// CHttpFilterCameseDataSupplier::OverallDataSize
// Returns the total data size
// -----------------------------------------------------------------------------
//
TInt CHttpFilterCameseDataSupplier::OverallDataSize ()
    {
    return iData->Length();
    }
    
// -----------------------------------------------------------------------------
// CHttpFilterCameseDataSupplier::Reset
// Resets data supplier.
// -----------------------------------------------------------------------------
//    
TInt CHttpFilterCameseDataSupplier::Reset()
    {
    // Not supported
    return KErrNotFound;
    }

//  End of File
