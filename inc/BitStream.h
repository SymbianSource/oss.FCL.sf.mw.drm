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



#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <bigint.h>

#define INLINE

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  TBitStream: Stream of bits
*
*  @lib    -
*  @since  3.0
*/
template<TInt S>
class TBitStream
    {
public: // New functions

    INLINE void Clear();

    INLINE void SetBit(
        const TInt aBit);
        
    INLINE void SetBitValue(
        const TInt aBit,
        const TInt aValue);
        
    INLINE void ClearBit(
        const TInt aBit);
        
    INLINE TInt GetBit(
        const TInt aBit) const;
        
    INLINE void XorBit(
        const TInt aBit,
        const TInt aValue);
        
    INLINE TPtrC8 Des() const;
    
    INLINE void Set(
        const TDesC8& aInput);
    
    INLINE void Copy(
        TBitStream<S> aBitStream);
        
    INLINE void Or(
        TBitStream<S> aBitStream);
        
    INLINE void Xor(
        TBitStream<S> aBitStream);
        
    INLINE void And(
        TBitStream<S> aBitStream);
        
    INLINE void Not();
    
    INLINE TInt operator[](
        const TInt aBit) const;
    
protected: // New functions

protected: // Data

    // Buffer with the data
    TBuf8<S / 8> iData;
    };

#include "BitStream.inl"

#endif      // BITSTREAM_H   
            
// End of File
