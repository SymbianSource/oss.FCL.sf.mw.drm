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

#ifndef _TR1SIDO_1_CLEANUPRESETANDDESTROY_H
#define _TR1SIDO_1_CLEANUPRESETANDDESTROY_H
// ----------------------------------------------------------------------------
// CleanupUtility for RpointerArrays
// Does RPointerArray< >->ResetAndDestroy() for the given array aPtr.
//
// Note: when using this class ensure that you include the header <e32base.h>
// and you link against euser.lib
// ----------------------------------------------------------------------------
//
template<typename T>
class CleanupResetAndDestroy
    {
public:
    /**
     Puts an item on the cleanup stack.

     @param  aRef
     The implementation information to be put on the cleanup stack.
     */
    inline static void PushL( T& aRef );
private:
    static void ResetAndDestroy( TAny *aPtr );
    };

#include "cleanupresetanddestroy.inl"
//
// ----------------------------------------------------------------------------
// End of CleanupUtility for RpointerArrays
// ----------------------------------------------------------------------------


#endif // _TR1SIDO_1_CLEANUPRESETANDDESTROY_H
