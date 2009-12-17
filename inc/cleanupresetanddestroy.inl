/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility class for putting to cleanup stack
*                RPointerArray class which owns its members
*
*/

#ifndef _TR1SIDO_1_CLEANUPRESETANDDESTROY_INL
#define _TR1SIDO_1_CLEANUPRESETANDDESTROY_INL

template<typename T>
inline void CleanupResetAndDestroyPushL( T& aRef );

// ---------------------------------------------------------------------------
// CleanupResetAndDestroy::PushL
// ---------------------------------------------------------------------------
template<typename T>
inline void CleanupResetAndDestroy<T>::PushL( T& aRef )
    {
    CleanupStack::PushL( TCleanupItem( &ResetAndDestroy, &aRef ) );
    }

// ---------------------------------------------------------------------------
// CleanupResetAndDestroy::ResetAndDestroy
// ---------------------------------------------------------------------------
template<typename T>
void CleanupResetAndDestroy<T>::ResetAndDestroy( TAny *aPtr )
    {
    ( static_cast<T*> ( aPtr ) )->ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CleanupResetAndDestroyPushL
// ---------------------------------------------------------------------------
template<typename T>
inline void CleanupResetAndDestroyPushL( T& aRef )
    {
    CleanupResetAndDestroy<T>::PushL( aRef );
    }

#endif // _TR1SIDO_1_CLEANUPRESETANDDESTROY_INL
