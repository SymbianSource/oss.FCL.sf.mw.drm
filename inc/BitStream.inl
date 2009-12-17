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


// -----------------------------------------------------------------------------
// TBitStream::
// 
// -----------------------------------------------------------------------------
//
template<TInt S>
void TBitStream<S>::Clear()
    {
    iData.SetLength(iData.MaxLength());
    iData.FillZ();
    }

template<TInt S>
void TBitStream<S>::SetBit(
    const TInt aBit)
    {
    iData[aBit / 8] |= (1 << (7 - aBit % 8));
    }
    
template<TInt S>
void TBitStream<S>::ClearBit(
    const TInt aBit)
    {
    iData[aBit / 8] &= ~(1 << (7 - aBit % 8));
    }
    
template<TInt S>
void TBitStream<S>::SetBitValue(
    const TInt aBit,
    const TInt aValue)
    {
    if (aValue)
        {
        SetBit(aBit);
        }
    else
        {
        ClearBit(aBit);
        }
    }
    
template<TInt S>
TInt TBitStream<S>::GetBit(
    const TInt aBit) const
    {
    return (iData[aBit / 8] & (1 << (7 - aBit % 8))) ? 1 : 0;
    }
    
template<TInt S>
void TBitStream<S>::XorBit(
    const TInt aBit,
    const TInt aValue)
    {
    iData[aBit / 8] ^= (1 << (7 - aBit % 8));
    }
    
template<TInt S>
TPtrC8 TBitStream<S>::Des() const
    {
    return TPtrC8(iData.Ptr(), iData.Length());
    }

template<TInt S>
void TBitStream<S>::Set(
    const TDesC8& aInput)
    {
    TInt i;
    
    iData.SetLength(aInput.Length());
    for (i = 0; i < S / 8; i++)
        {
        iData[i] = aInput[i];
        }
    }

template<TInt S>
void TBitStream<S>::Copy(
    TBitStream<S> aBitStream)
    {
    iData.Copy(aBitStream.Des());
    }
    
template<TInt S>
void TBitStream<S>::Xor(
    TBitStream<S> aBitStream)
    {
    TPtrC8 des(aBitStream.Des());
    TInt i;
    
    for (i = 0; i < S / 8; i++)
        {
        iData[i] ^= des[i];
        }
    }
    
template<TInt S>
void TBitStream<S>::Or(
    TBitStream<S> aBitStream)
    {
    TPtrC8 des(aBitStream.Des());
    TInt i;
    
    for (i = 0; i < S / 8; i++)
        {
        iData[i] |= des[i];
        }
    }
    
template<TInt S>
void TBitStream<S>::And(
    TBitStream<S> aBitStream)
    {
    TPtrC8 des(aBitStream.Des());
    TInt i;
    
    for (i = 0; i < S / 8; i++)
        {
        iData[i] &= des[i];
        }
    }
    
template<TInt S>
void TBitStream<S>::Not()
    {
    TInt i;
    
    for (i = 0; i < S / 8; i++)
        {
        iData[i] = ~iData[i];
        }
    }
    
template<TInt S>
TInt TBitStream<S>::operator[](
    const TInt aBit) const
    {
    return GetBit(aBit);
    }
    
//  End of File  
