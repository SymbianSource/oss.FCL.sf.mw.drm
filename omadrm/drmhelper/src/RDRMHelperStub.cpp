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
* Description:  Implementation of the Helper Client session functionality 
*
*/


// INCLUDE FILES
#include <e32std.h>
#include "RDRMHelper.h"
#include "drmhelpercommon.h"

// LOCAL CONSTANTS AND MACROS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RDRMHelper::RDRMHelper
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
RDRMHelper::RDRMHelper() : RSessionBase()
    {
    }

// -----------------------------------------------------------------------------
// RDRMHelper::Connect
// 
// Connect to the server session
// -----------------------------------------------------------------------------
//
TInt RDRMHelper::Connect()
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RDRMHelper::Version
// 
// return server version
// -----------------------------------------------------------------------------
//
TVersion RDRMHelper::Version() const
    {
	return( TVersion( KDRMHSMajorVersionNumber, KDRMHSMinorVersionNumber, 
        KDRMHSBuildVersionNumber ) );
    }

// -----------------------------------------------------------------------------
// RDRMHelper::SetAutomated
// 
// Register one content uri to the helper server
// -----------------------------------------------------------------------------
//
TInt RDRMHelper::SetAutomated( 
    const TDesC8& /*aUri*/, 
    TBool /*aActive*/, 
    TInt /*aAutomatedType*/, 
    TInt /*aIntent*/ ) const
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RDRMHelper::RemoveAutomated
// 
// Unregister one content uri 
// -----------------------------------------------------------------------------
//
TInt RDRMHelper::RemoveAutomated( 
    const TDesC8& /*aUri*/, 
    TBool /*aActive*/, 
    TInt /*aAutomatedType*/, 
    TInt /*aIntent*/ ) const
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RDRMHelper::RemoveAutomatedAll
// 
// Unregister one content uri 
// -----------------------------------------------------------------------------
//
TInt RDRMHelper::RemoveAutomatedAll( 
    const TDesC8& /*aUri*/, 
    TBool /*aActive*/, 
    TInt /*aAutomatedType*/, 
    TInt /*aIntent*/ ) const
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RDRMHelper::IndicateIdle
// -----------------------------------------------------------------------------
//
TInt RDRMHelper::IndicateIdle() const
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RDRMHelper::IsAutomated
// -----------------------------------------------------------------------------
//
TInt RDRMHelper::IsAutomated( 
    const TDesC8& /*aUri*/, 
    TInt /*aAutomatedType*/, 
    TInt /*aIntent*/, 
    TBool& aAutomated, 
    TInt& /*aType*/ ) const
    {
    aAutomated = EFalse;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RDRMHelper::StartServer
// -----------------------------------------------------------------------------
//
TInt RDRMHelper::StartServer()
    {
    return  KErrNone;
    }

// -----------------------------------------------------------------------------
// RDRMHelper::CreateServerProcess
// -----------------------------------------------------------------------------
//
TInt RDRMHelper::CreateServerProcess()
    {
    return  KErrNone;
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File
