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
* Description: 
*
*/
#ifndef   BUFFERCONTAINERS_INL
#define   BUFFERCONTAINERS_INL

// ---------------------------------------------------------------------------
// DRM::CBufferContainer::NewL
// ---------------------------------------------------------------------------
//
template <typename T>
inline DRM::CBufferContainer<T>* DRM::CBufferContainer<T>::NewL()
{
    CBufferContainer<T>* self( CBufferContainer<T>::NewLC() );
    CleanupStack::Pop( self );
    return self;
}

// ---------------------------------------------------------------------------
// DRM::CBufferContainer::NewLC
// ---------------------------------------------------------------------------
//
template <typename T>
inline DRM::CBufferContainer<T>* DRM::CBufferContainer<T>::NewLC()
{
    CBufferContainer<T>* self( new (ELeave) CBufferContainer<T>() );
    CleanupStack::PushL( self );
    return self;
}

// ---------------------------------------------------------------------------
// DRM::CBufferContainer::CBufferContainer
// ---------------------------------------------------------------------------
//
template <typename T>
inline DRM::CBufferContainer<T>::CBufferContainer()
{}

// ---------------------------------------------------------------------------
// DRM::CBufferContainer::~CBufferContainer
// ---------------------------------------------------------------------------
//
template <typename T>
inline DRM::CBufferContainer<T>::~CBufferContainer()
{}


#endif // BUFFERCONTAINERS_INL
