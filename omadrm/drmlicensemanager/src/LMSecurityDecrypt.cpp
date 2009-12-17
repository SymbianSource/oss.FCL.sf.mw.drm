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
* Description:  Encryption/Decryption support functions
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include <e32math.h>
#include <s32buf.h>
#include <s32crypt.h>

#include "LMSecurity.h"
#include "LMSecurityDecrypt.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// -----------------------------------------------------------------------------
//
CLMSecurityDecrypt::CLMSecurityDecrypt(
    const TDesC8& init)
    {
    TPckg<TUint> p(0);

    p.Copy(init);
    iKey = p();
    Math::Rand(iKey);
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// -----------------------------------------------------------------------------
//
TInt CLMSecurityDecrypt::DecryptL(
    TDes8& aOutput,
    const TDesC8& aInput)
    {
    TInt i;
    TUint8* ptr;

    ptr = const_cast<TUint8*>(aOutput.Ptr());
    for (i = 0; i < aInput.Size(); i++)
        {
        ptr[i] = static_cast<TUint8>(aInput[i] ^ Math::Rand(iKey));
        }
    aOutput.SetLength(aInput.Size());
    return aInput.Size();
    }
