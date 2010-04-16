/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file contains inline functions of class CDRMDbSession
*
*/



// INCLUDE FILES
#include "DrmConstraint.h"
#include "DRMRightsServer.h"

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================
inline TBool CDRMDbSession::Invalid( const CDRMConstraint& aConstraint )
    {
    if ( ( ( aConstraint.iActiveConstraints & EConstraintCounter ) &&
           ( aConstraint.iCounter < 1 ) ) ||
         ( ( aConstraint.iActiveConstraints &
             EConstraintInterval ) &&
           ( aConstraint.iIntervalStart != Time::NullTTime() ) ) )
        {
        return ETrue;
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CDRMDbSession::UpdateSecureTime
// Update secure time.
// -----------------------------------------------------------------------------
inline void CDRMDbSession::UpdateSecureTime()
    {
    iSecureTime = ( ( CDRMRightsServer* )( Server() ) )->
                  GetSecureTime( iTrustedTime );
    }

// -----------------------------------------------------------------------------
// CDRMDbSession::UpdateSecureTime
// Update secure time.
// -----------------------------------------------------------------------------
inline const TDesC8& CDRMDbSession::GetContentId()
    {
    return *iContentId;
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================
//  End of File
