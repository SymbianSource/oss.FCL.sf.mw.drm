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
* Description:  Encryption/Decryption support functions
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include <e32math.h>
#include <s32buf.h>
#include <s32crypt.h>

#include "LMSecurity.h"
#include "LMSecurityEncrypt.h"
#include "LMSecurityDecrypt.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLMSecurity::NewEncryptL
// Return a new encryptor
// -----------------------------------------------------------------------------
//
CSecurityEncryptBase* CLMSecurity::NewEncryptL(
    const TDesC8& init) const
    {
    return new CLMSecurityEncrypt(init);
    };

// -----------------------------------------------------------------------------
// CLMSecurity::NewDecryptL
// Return a new decryptor
// -----------------------------------------------------------------------------
//
CSecurityDecryptBase* CLMSecurity::NewDecryptL(
    const TDesC8& init) const
    {
    return new CLMSecurityDecrypt(init);
    };

// -----------------------------------------------------------------------------
// CLMSecurity::SetL
// Password functions not supported (handled via the encrypt/decrypt classes)
// -----------------------------------------------------------------------------
//
void CLMSecurity::SetL(
    const TDesC& /* aOldPassword */,
    const TDesC& /* aNewPassword */)
    {
    };

// -----------------------------------------------------------------------------
// CLMSecurity::SecurityData
// Password functions not supported (handled via the encrypt/decrypt classes)
// -----------------------------------------------------------------------------
//
TPtrC8 CLMSecurity::SecurityData() const
    {
    return TPtrC8(0, 0);
    };

// -----------------------------------------------------------------------------
// CLMSecurity::PrepareL
// Password functions not supported (handled via the encrypt/decrypt classes)
// -----------------------------------------------------------------------------
//
void CLMSecurity::PrepareL(
    const TDesC& /* aPassword */)
    {
    };

// -----------------------------------------------------------------------------
// CLMSecurity::IsEnabled
// Password functions not supported (handled via the encrypt/decrypt classes)
// -----------------------------------------------------------------------------
//
TInt CLMSecurity::IsEnabled() const
    {
    return ETrue;
    };

// -----------------------------------------------------------------------------
// CLMSecurity::SetEnabledL
// Password functions not supported (handled via the encrypt/decrypt classes)
// -----------------------------------------------------------------------------
//
void CLMSecurity::SetEnabledL(
    const TDesC& /* aPassword */,
    TBool /* aIsEnabled */)
    {
    };
