/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*  wmdrmaccess.cpp
*
*/

#include <e32base.h>
#include <ecom/ecom.h>

#include <wmdrmaccess.h>
#include <wmdrmaccessecominterface.h>

// ---------------------------------------------------------------------------
// CWmDrmAccess::NewL
// ---------------------------------------------------------------------------
EXPORT_C CWmDrmAccess* CWmDrmAccess::NewL()
    {
    CWmDrmAccess* self(CWmDrmAccess::NewLC());
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmAccess::NewLC
// ---------------------------------------------------------------------------
EXPORT_C CWmDrmAccess* CWmDrmAccess::NewLC()
    {
    CWmDrmAccess* self(new (ELeave) CWmDrmAccess);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmAccess::ConstructL
// ---------------------------------------------------------------------------
void CWmDrmAccess::ConstructL()
    {
    iWmDrmAccessEcomInterface = CWmDrmAccessEcomInterface::NewL();
    }


// ---------------------------------------------------------------------------
// CWmDrmAccess::CWmDrmAccess
// ---------------------------------------------------------------------------
CWmDrmAccess::CWmDrmAccess()
    {
    }


// ---------------------------------------------------------------------------
// CWmDrmAccess::~CWmDrmAccess
// ---------------------------------------------------------------------------
EXPORT_C CWmDrmAccess::~CWmDrmAccess()
    {
    if (iWmDrmAccessEcomInterface)
        {
        delete iWmDrmAccessEcomInterface;
        }
    REComSession::FinalClose();
    }


// ---------------------------------------------------------------------------
// CWmDrmAccess::Initialize
// ---------------------------------------------------------------------------
EXPORT_C TInt CWmDrmAccess::Initialize(const TDesC8& aHeader)
    {
    TInt retCode(KErrNotReady);
    if (iWmDrmAccessEcomInterface)
        {
        retCode = iWmDrmAccessEcomInterface->Initialize(aHeader);
        }
    return retCode;
    }


// ---------------------------------------------------------------------------
// CWmDrmAccess::Decrypt
// ---------------------------------------------------------------------------
EXPORT_C TInt CWmDrmAccess::Decrypt(TDes8& aBuffer)
    {
    TInt retCode = KErrNotReady;
    if (iWmDrmAccessEcomInterface)
        {
        retCode = iWmDrmAccessEcomInterface->Decrypt(aBuffer);
        }
    return retCode;
    }


// ---------------------------------------------------------------------------
// CWmDrmAccess::Close
// ---------------------------------------------------------------------------
EXPORT_C TInt CWmDrmAccess::Close()
    {
    TInt retCode(KErrNotReady);
    if (iWmDrmAccessEcomInterface)
        {
        retCode = iWmDrmAccessEcomInterface->Close();
        }
    return retCode;
    }

