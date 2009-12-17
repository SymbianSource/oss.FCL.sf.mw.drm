/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include <e32base.h>
#include "RoapMessage.h"
#include "Base64.h"

using namespace Roap;

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
const TInt KIso8601TimeLength = 20;
_LIT8(KIso8601TimeFormat, "%04d-%02d-%02dT%02d:%02d:%02dZ");

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRoapMessage::CRoapMessage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRoapMessage::CRoapMessage()
    {
    }

// -----------------------------------------------------------------------------
// CRoapMessage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRoapMessage::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CRoapMessage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRoapMessage* CRoapMessage::NewL()
    {
    CRoapMessage* self = new( ELeave ) CRoapMessage;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CRoapMessage::~CRoapMessage()
    {
    delete iXmlData;
    }


// -----------------------------------------------------------------------------
// CRoapMessage::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CRoapMessage::MessageAsXmlL(void)
    {
    return iXmlData->AllocL();
    }

// -----------------------------------------------------------------------------
// CRoapMessage::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRoapMessage::SetXmlData(
    HBufC8* aData)
    {
    iXmlData = aData;
    }

// -----------------------------------------------------------------------------
// CRoapMessage::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CRoapMessage::TimeToIso8601L(
    TTime& aTime)
    {
    HBufC8* r = NULL;
    TDateTime t;
    
    t = aTime.DateTime();
    r = HBufC8::NewMax(KIso8601TimeLength);
    User::LeaveIfNull(r);
    TPtr8 des = r->Des();
    des.Format(KIso8601TimeFormat, t.Year(), t.Month() + 1, t.Day() + 1,
        t.Hour(), t.Minute(), t.Second());
    return r;
    }

// -----------------------------------------------------------------------------
// CRoapMessage::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TTime CRoapMessage::Iso8601ToTime(TDesC8& aTimeString)
    {
    TLex8 lex;
    TInt year = 0;
    TInt month = 0;
    TInt day = 0;
    TInt hour = 0;
    TInt minute = 0;
    TInt second = 0;
    TTime r = Time::NullTTime();
    TLocale l;
    TTimeIntervalSeconds offset(l.UniversalTimeOffset());

    if (aTimeString.Length() > 0)
        {    
        lex = aTimeString;
        lex.Val(year);
        lex.Inc();
        lex.Val(month);
        lex.Inc();
        lex.Val(day);
        lex.Inc();
        lex.Val(hour);
        lex.Inc();
        lex.Val(minute);
        lex.Inc();
        lex.Val(second);
        r = TTime(TDateTime(year, static_cast<TMonth>(month - 1), day - 1, hour,
            minute, second, 0));
        if (lex.Get() != 'Z')
            {
            r += offset;
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// CRoapMessage::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRoapMessage::BufAppendL(
    CBufFlat* aBuffer,
    const TDesC8& aConst)
    {
    aBuffer->InsertL(aBuffer->Size(), aConst);
    }

// -----------------------------------------------------------------------------
// CRoapMessage::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRoapMessage::BufAppendBase64L(
    CBufFlat* aBuffer,
    const TDesC8& aConst)
    {
    HBufC8* buffer = NULL;

    buffer = Base64EncodeL(aConst);
    CleanupStack::PushL(buffer);
    aBuffer->InsertL(aBuffer->Size(), *buffer);
    CleanupStack::PopAndDestroy(buffer);
    }

// -----------------------------------------------------------------------------
// CRoapMessage::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRoapMessage::BufAppendTimeL(
    CBufFlat* aBuffer,
    TTime& aTime)
    {
    HBufC8* buffer = NULL;

    buffer = TimeToIso8601L(aTime);
    CleanupStack::PushL(buffer);
    aBuffer->InsertL(aBuffer->Size(), *buffer);
    CleanupStack::PopAndDestroy(buffer);
    }

//  End of File  
