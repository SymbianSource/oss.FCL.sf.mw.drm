/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CBase derived container classes for easening
*               dynamic memory allocation of TBuf<n> type variables
*               (TName etc.)
*/
#ifndef   BUFFERCONTAINERS_H
#define   BUFFERCONTAINERS_H

#include <e32std.h>
namespace DRM
{
template <typename T>
NONSHARABLE_CLASS( CBufferContainer ): public CBase
    {
    // Note, this class is not intendend to be part of any API
    public:
        inline static CBufferContainer<T>* NewL();
        inline static CBufferContainer<T>* NewLC();

        inline virtual ~CBufferContainer();
    private:
        inline CBufferContainer();

        //data
    public:
        // Contained TBuf member
        T iBuffer;
    };

//Containers for some TBuf based types defined at e32cmn.h
typedef CBufferContainer<TName> CNameContainer;
typedef CBufferContainer<TFullName> CFullNameContainer;
typedef CBufferContainer<TExitCategoryName> CExitCategoryNameContainer;
typedef CBufferContainer<TFileName> CFileNameContainer;
typedef CBufferContainer<TPath> CPathContainer;
typedef CBufferContainer<TVersionName> CVersionNameContainer;
typedef CBufferContainer<TUidName> CUidNameContainer;
}

#include "buffercontainers.inl"
#endif // BUFFERCONTAINERS_H
