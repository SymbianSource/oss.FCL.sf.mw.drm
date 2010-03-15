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
#include <e32std.h>
#include "Base64.h"

// LOCAL CONSTANTS AND MACROS
LOCAL_C const TUint8* const KBase64Chars =
    _S8("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/");

// LOCAL FUNCTION PROTOTYPES
LOCAL_C TUint8 FindBase64Char(
    TUint aChar);

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
LOCAL_C TUint8 FindBase64Char(
    TUint aChar)
    {
    TUint8 i;

    for (i = 0; i < 64; i++)
        {
        if (aChar == KBase64Chars[i]) break;
        }
    return i;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ?function_name implements...
// ?implementation_description.
// Returns: ?value_1: ?description
//          ?value_n: ?description
//                    ?description
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* Base64EncodeL(
    const TDesC8& aInput)
    {
    HBufC8* ret;
    TInt i = 0;
    TInt j = 0;
    TUint8 charArray3[3];
    TUint8 charArray4[4];
    TInt l = 0;

    ret = HBufC8::NewL((aInput.Length() * 4 + 1) / 3);
    TPtr8 des = ret->Des();
    while (l < aInput.Length())
        {
        charArray3[i++] = aInput[l];
        l++;
        if (i == 3)
            {
            charArray4[0] = static_cast<TUint8>((charArray3[0] & 0xfc) >> 2);
            charArray4[1] = static_cast<TUint8>(
                ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4));
            charArray4[2] = static_cast<TUint8>(
                ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6));
            charArray4[3] = static_cast<TUint8>(charArray3[2] & 0x3f);

            for (i = 0; i <4; i++)
                {
                des.Append(KBase64Chars[charArray4[i]]);
                }
            i = 0;
            }
        }

    if (i != 0)
        {
        for (j = i; j < 3; j++)
            {
            charArray3[j] = '\0';
            }

        charArray4[0] = static_cast<TUint8>((charArray3[0] & 0xfc) >> 2);
        charArray4[1] = static_cast<TUint8>(
            ((charArray3[0] & 0x03) << 4) +((charArray3[1] & 0xf0) >> 4));
        charArray4[2] = static_cast<TUint8>(
            ((charArray3[1] & 0x0f) << 2) +((charArray3[2] & 0xc0) >> 6));
        charArray4[3] = static_cast<TUint8>(charArray3[2] & 0x3f);

        for (j = 0; j < i + 1; j++)
            {
            des.Append(KBase64Chars[charArray4[j]]);
            }

        while ((i++ < 3))
            {
            des.Append('=');
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// ?function_name implements...
// ?implementation_description.
// Returns: ?value_1: ?description
//          ?value_n: ?description
//                    ?description
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* Base64DecodeL(
    const TDesC8& aInput)
    {
    TInt len = aInput.Length();
    TInt i = 0;
    TInt j = 0;
    TInt in = 0;
    TUint8 charArray4[4], charArray3[3];
    HBufC8* ret;

    ret = HBufC8::NewL((aInput.Length() * 3 + 1) / 4);
    TPtr8 des = ret->Des();
    while (len-- && aInput[in] != '=')
        {
        if (FindBase64Char(aInput[in]) < 64)
            {
            charArray4[i++] = aInput[in]; in++;
            }
        else
            {
            in++;
            }

        if (i == 4)
            {
            for (i = 0; i < 4; i++)
                {
                charArray4[i] = FindBase64Char(charArray4[i]);
                }

            charArray3[0] = static_cast<TUint8>(
                (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4));
            charArray3[1] = static_cast<TUint8>(
                ((charArray4[1] & 0xf) << 4) + ((charArray4[2] & 0x3c) >> 2));
            charArray3[2] = static_cast<TUint8>(
            ((charArray4[2] & 0x3) << 6) + charArray4[3]);

            for (i = 0; i < 3; i++)
                {
                des.Append(charArray3[i]);
                }
            i = 0;
            }
        }

    if (i != 0)
        {
        for (j = i; j < 4; j++)
            {
            charArray4[j] = 0;
            }

        for (j = 0; j < 4; j++)
            {
            charArray4[j] = FindBase64Char(charArray4[j]);
            }

        charArray3[0] = static_cast<TUint8>(
            (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4));
        charArray3[1] = static_cast<TUint8>(
            ((charArray4[1] & 0xf) << 4) + ((charArray4[2] & 0x3c) >> 2));
        charArray3[2] = static_cast<TUint8>(
            ((charArray4[2] & 0x3) << 6) + charArray4[3]);

        for (j = 0; j < i - 1; j++)
            {
            des.Append(charArray3[j]);
            }
        }

    return ret;
    }

//  End of File
