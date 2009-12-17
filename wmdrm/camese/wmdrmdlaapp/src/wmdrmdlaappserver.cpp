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
* Description:  Implementation class for WMDRM DLA App Server
*
*/


// INCLUDE FILES
#include "wmdrmdlaappserver.h"
#include "wmdrmdlaappservicesession.h"
#include "wmdrmdlaappconstants.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CWmDrmDlaAppServer::CreateServiceL
// ---------------------------------------------------------------------------
//
CApaAppServiceBase* CWmDrmDlaAppServer::CreateServiceL( 
    TUid aServiceType ) const
    {
    if ( aServiceType == KWmDrmDlaAppServiceId )
        {
        return new (ELeave) CWmDrmDlaAppServiceSession;
        }
    return CAknAppServer::CreateServiceL( aServiceType );
    }
