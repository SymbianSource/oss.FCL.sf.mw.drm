/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  End time based item data structure
*
*/



// INCLUDE FILES
#include    <e32base.h>
#include    <e32std.h>
#include    "EndTimeBased.h"
#include    "DRMHelperCommon.h"

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

// -----------------------------------------------------------------------------
// CEndTimeBased::CEndTimeBased
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEndTimeBased::CEndTimeBased( 
        const TUint8& aPermType , 
        const TUint8& aRegType , 
        const TUint8& aAutoType)
            :iIsExpired(EFalse),
                iCount(-1),
                iCountReg(1),
                iPermType(aPermType),
                iRegType(aRegType),
                iAutoType(aAutoType)
    {
    TTimeIntervalYears years(KTimeIntervalYears);
    iEndTime.HomeTime();
    iEndTime += years;
    }

// -----------------------------------------------------------------------------
// CEndTimeBased::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEndTimeBased::ConstructL(const TDesC8& aUri)
    {
    iUri = HBufC8::NewL( aUri.Length() );
    *iUri = aUri;
    }

// -----------------------------------------------------------------------------
// CEndTimeBased::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CEndTimeBased* CEndTimeBased::NewL( 
            const TDesC8& aUri , 
            const TUint8& aPermType , 
            const TUint8& aRegType , 
            const TUint8& aAutoType )
    {
    CEndTimeBased* self = 
        new( ELeave ) CEndTimeBased(aPermType , aRegType , aAutoType);
    
    CleanupStack::PushL( self );
    self->ConstructL(aUri);
    CleanupStack::Pop(self);

    return self;
    }

    
// Destructor
CEndTimeBased::~CEndTimeBased()
    {
    delete iUri;
    }


// -----------------------------------------------------------------------------
// CEndTimeBased::SetIsExpired
// set whether the content rights is expired
// -----------------------------------------------------------------------------
//
void CEndTimeBased::SetIsExpired( TBool aIsExpired )
    {
    iIsExpired = aIsExpired;
    }


// -----------------------------------------------------------------------------
// CEndTimeBased::SetIsExpired
// check if the content rights is expired
// -----------------------------------------------------------------------------
//
TBool CEndTimeBased::IsExpired() const 
    {
    return iIsExpired;
    }


// -----------------------------------------------------------------------------
// CEndTimeBased::SetCount
// Set the value of count
// -----------------------------------------------------------------------------
//
void CEndTimeBased::SetCount( TInt8 aCount )
    {
    iCount = aCount;
    }

// -----------------------------------------------------------------------------
// CEndTimeBased::IncreaseCount
// Set the value of count
// -----------------------------------------------------------------------------
//
void CEndTimeBased::IncreaseCount()
    {
    iCount++;
    }

// -----------------------------------------------------------------------------
// CEndTimeBased::Count
// get the value of count
// -----------------------------------------------------------------------------
//
TInt8 CEndTimeBased::Count() const
    {
    return iCount;
    }


// -----------------------------------------------------------------------------
// CEndTimeBased::DecreaseRegTimes
// Set the value of registered times
// -----------------------------------------------------------------------------
//
void CEndTimeBased::DecreaseRegTimes()
    {
    iCountReg--;
    }

// -----------------------------------------------------------------------------
// CEndTimeBased::IncreaseRegTimes
// Set the value of registered times
// -----------------------------------------------------------------------------
//
void CEndTimeBased::IncreaseRegTimes()
    {
    iCountReg++;
    }

// -----------------------------------------------------------------------------
// CEndTimeBased::SetRegTimes
// Set the value of count
// -----------------------------------------------------------------------------
//
void CEndTimeBased::SetRegTimes( TInt aRegTimes )
    {
    iCountReg = aRegTimes;
    }

// -----------------------------------------------------------------------------
// CEndTimeBased::RegTimes
// get the value of registered times
// -----------------------------------------------------------------------------
//
TInt CEndTimeBased::RegTimes() const
    {
    return iCountReg;
    }

// -----------------------------------------------------------------------------
// CEndTimeBased::RegType
// get the value of register type
// -----------------------------------------------------------------------------
//
TUint8 CEndTimeBased::RegType() const
    {
    return iRegType;
    }
    

// -----------------------------------------------------------------------------
// CEndTimeBased::AutoType
// get the value of automated content type
// -----------------------------------------------------------------------------
//
TUint8 CEndTimeBased::AutoType() const
    {
    return iAutoType;
    }
    
// -----------------------------------------------------------------------------
// CEndTimeBased::PermType
// get the value of permission type
// -----------------------------------------------------------------------------
//
TUint8 CEndTimeBased::PermType() const 
    {
    return iPermType;
    }    
    
// -----------------------------------------------------------------------------
// CEndTimeBased::SetEndTime
// Set the value of end time
// -----------------------------------------------------------------------------
//
void CEndTimeBased::SetEndTime( TTime aEndTime )
    {
    iEndTime = aEndTime;
    }


// -----------------------------------------------------------------------------
// CEndTimeBased::EndTime
// get the value of end time
// -----------------------------------------------------------------------------
//
TTime CEndTimeBased::EndTime() const
    {
    return iEndTime;
    }


// -----------------------------------------------------------------------------
// CEndTimeBased::EndTime
// get the value of content uri
// -----------------------------------------------------------------------------
//
HBufC8* CEndTimeBased::Uri() const
    {
    return iUri;      
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
