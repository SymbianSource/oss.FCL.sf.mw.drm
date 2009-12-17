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
*  wmdrmaccessecominterface.h
*
*/

#ifndef _WMDRMACCESSECOMINTERFACE_H_
#define _WMDRMACCESSECOMINTERFACE_H_

#include <e32base.h>

// Class declaration
// CWmDrmAccessEcomInterface
// ecom plugin interface for WM DRM access implementation
// @see CWmDrmAccess
NONSHARABLE_CLASS( CWmDrmAccessEcomInterface ) : public CBase
    {
public:
    // Constructors
    static CWmDrmAccessEcomInterface* NewL();
    static CWmDrmAccessEcomInterface* NewLC();

    // Destructor
    virtual ~CWmDrmAccessEcomInterface();

    // Initialises decryption context.
    // @see CWmDrmAccess::Initialize
    // @post class ready for decrypting WM DRM protected file,
    //  header of which is given in aHeader.
    virtual TInt Initialize( const TDesC8& aHeader ) = 0;

    // Decrypts encrypted data in place.
    // @see CWmDrmAccess::Decrypt
    // @pre Succesful call of Initialize, data to be decrypted in aBuffer
    // @post decrypted data in aBuffer
    virtual TInt Decrypt( TDes8& aBuffer ) = 0;

    // Closes decryption context.
    // @see CWmDrmAccess::Close
    virtual TInt Close() = 0;

private:
    // internal variable used on freeing ecom related resources
    TUid iDtor_ID_Key;
    };

#include "wmdrmaccessecominterface.inl"
#endif _WMDRMACCESSECOMINTERFACE_H_
