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
* Description:  This class implements the base64 en/decoding.
*
*/


#include <e32base.h>
#include "Base64.h"

/*  6    LOCAL DEFINITIONS */

/*  6.1    Local include files */

/*  6.2    Local constants */

LOCAL_C const TUint8 pBase64[] =
    {
    0x3e, 0x7f, 0x7f, 0x7f, 0x3f, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x7f,
    0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x00, 0x01,
    0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
    0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11,
    0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
    0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x1a, 0x1b,
    0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23,
    0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
    0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33
    };

/*  6.3    Local macros */
#define b64blocks(l) (((l) + 2) / 3 * 4)
/* #define b64octets(l)  ((l) / 4  * 3) */

/*  6.4    Local data types */

/*  6.5    Local data structures */

/*  6.6    Local function prototypes */

LOCAL_C TInt b64valid(TUint8 *);

/*  7    MODULE CODE */

/* ========================================================================= */

/*  7.2 */

/* Functional description
 *
 * This function does base64 decoding. As the outputted buffer is always 33% shorter than
 * the input buffer, the caller may want to reallocate the buffer after calling this function.
 *
 * Parameters
 *
 * Pointer to source containing data to decode
 *
 source
 *
 * Length of source buffer
 *
 length
 *
 * pointer to the destination buffer
 * Note: the user must allocate memory for this buffer that its length at least can not be shorter than
 *       the length of source. The user also can use same buffer for both source and destination.
 *
 destination
 *
 * length of the buffer allocated
 *
 destination_length
 *
 * Return values
 *
 *  byte 0 if OK else > 0
 */

/* ---------------------------------------------------------------------- */

TInt b64decode(TUint8 *source, TUint32 length, TUint8 *destination, TUint32 *destination_length)
    {
    /* Data structures */

    /*  Code  */

    TUint8   *buf/*, *pbuf*/;     /* Decoding buffer pointers. */
    register TUint32 x = 0;          /* General purpose integers. */
    TUint32 buf_index = 0;  /* current index in destination buffer */


    /* buffer to avoid allocating wholly new buffer for output */
    TUint8 localBuffer[4] = {0,0,0,0};
    TUint8* pLocalBufferOutPointer = 0;

    register TUint32 i = 0;
    TUint32 num;

    TUint8   c = 0;          /* Character to decode. */
    TUint8 localBufferIndex = 0;

    /*  int32 buf_length = 0; */ /* new length of destination buf */
    const TUint8 pad = 0x3d; /* '=' */

    if(!source || length == 0 || !destination || !destination_length)
        {
        return KErrArgument;
        }

    /* Collating sequence independant "===". */

    /* Calculate the amount of '=' in the end of the source */
    for(x=0, buf=source + (length-1); *buf == pad; buf --)
        {
        x++;
        }

    /* max allow 3 "===" in the end of content */
    if (x > 3)
        {
        return KErrArgument;
        }

    /* Save the encoded string pointer. */
    // pbuf = destination;

    /* save address for first output block */
    pLocalBufferOutPointer = destination;
    x = 0; /* Initialize index. */

    localBufferIndex = 0;


    Mem::Copy(localBuffer, source, 4);

    for (i = length; i != 0; i-- ) /* Decode every byte of the base 64 string */
        {
        /* c = *pbuf++; */
        c = localBuffer[localBufferIndex++];

        if(localBufferIndex == 4)
            {
            localBufferIndex = 0;

            source = source + 4;

            /* INPUT: copy next input block to local buffer */
            num = i > 4 ? 4 : i - 1;
            if(num > 0)
                {
                Mem::Copy(localBuffer, source, num);
                }
            }

        /* Ignore "=". */
        if  (c == pad)
            {
            break; /* this must be the end of the buffer, or else ve have a invalid character */
            }

        if  (c == '\n' || c == '\r' || c == '\t') /* ignore linefeed tab and cr */
            {
            continue;
            }

        /* Valid Base64 Index? */
        if (!b64valid(&c))
            {
            /* b64free(buf); */
            return KErrArgument;
            }

        /* Decode 4 byte words into 3 byte octets.*/
        switch(x % 4)
            {
            case 0: /* Byte 0 of word.*/
                pLocalBufferOutPointer[buf_index] = (TInt8)(c << 2);
                break;
            case 1: /* Byte 1 of word. */
                pLocalBufferOutPointer[buf_index] |= c >> 4;
                buf_index ++;
                pLocalBufferOutPointer[buf_index] = (TInt8)( (c & 0x0f) << 4 );
                break;
            case 2: /* Byte 2 of word. */
                pLocalBufferOutPointer[buf_index] |= c >> 2;
                buf_index ++;
                pLocalBufferOutPointer[buf_index] = (TInt8)( (c & 0x03) << 6 );
                break;
            case 3: /* Byte 3 of word. */
                pLocalBufferOutPointer[buf_index] |= c;
                buf_index ++;
                break;
            default: /* useless, just to depress warnings */
                break;
            }

        x++; /* Increment word byte. */


        } /* while */


    /* make sure that there is zero at the end of the buffer
       though buf[buf_index] shoule be zero after decoding
    */


    /* pbuf[buf_index+1] = 0; */

    /*buf[buf_index+1]=0*/
    /* don't return the size of the buffer (buf_length), but the data size we put in (buf_index)*/
    *destination_length = buf_index;
    return KErrNone;
    }

/*  7.3 */

/* Functional description
 *
 * This function validates the character according to the base64 rules,
 * and changes it to the corresponding value in the table.
 *
 * Parameters
 *
 * Pointer to character to validate
 *
 c
 *
 * Return values
 *
 *  TRUE if character is valid else FALSE.
 */

/* ---------------------------------------------------------------------- */

LOCAL_C TInt b64valid(TUint8 *c)
    {
    /* Data structures */

    /*  Code  */

    /* If not within the range of the table, return false. */
    if ((*c < 0x2b) || (*c > 0x7a))
        {
        return 0;
        }
    /* If it falls within one of the gaps, return false. */
    if ((*c = pBase64[*c - 0x2b]) == 0x7f)
        {
        return 0;
        }

    return 1; /* Otherwise, return true. */
    }

/*  7.4 */

/* Functional description
 *
 * This function allocates a buffer in the size of the parameter.
 *
 * Parameters
 *
 * Size of buffer to allocate
 *
 length
 *
 * Return values
 *
 *  pointer to buffer
 */

/* ---------------------------------------------------------------------- */
#ifdef BUILD_BASE64_ENCODE

LOCAL TUint8 *b64buffer(TUint32 length)
    {
    /* Data structures */

    /*  Code  */

    TUint8 *buf; /* buffer pointers. */

    if  (!length) /* dont allow buf size  0 */
        {
        return  NULL;
        }

    buf = (TUint8 *) DRM_BLOCK_ALLOC(length);

    return buf; /* Return the pointer or null. */
    }
#endif /* #ifdef BUILD_BASE64_ENCODE */

/* End of File */

