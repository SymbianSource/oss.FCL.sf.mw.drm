/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  AES encryption utility for drm engine use
*
*/



// INCLUDE FILES
#include <e32std.h>

#include <symmetric.h>
#include <rijndael.h>

#include <DRMTypes.h>

#include "drmaescrypto.h"

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// DrmAesCrypto::DrmAesEncryptL
// Encrypt data using a given key and insert initial vector on beginning
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* DrmAesCrypto::DrmAesEncryptL(
    const TDesC8& aKey,
    const TDesC8& aIV,
    const TBool aAddPadding,
    const TDesC8& aData )
    {

    HBufC8* retBuf( NULL );

    CModeCBCEncryptor* cbc( NULL );
    TInt lastBlockStart( 0 );
    TPtr8 data( NULL, 0, 0 );

    if( aIV.Length() != KDCFKeySize || aKey.Length() != KDCFKeySize )
        {
        User::Leave( KErrArgument );
        }

    cbc = CModeCBCEncryptor::NewL( CAESEncryptor::NewLC( aKey ), aIV );
    CleanupStack::Pop(); // CAESEncryptor, owned by cbc.
    CleanupStack::PushL( cbc );

    retBuf = HBufC8::NewLC( aData.Size() + aIV.Size() + KDCFKeySize );
    data.Set( retBuf->Des() );
    data.Copy( aIV );
    data.Append( aData );

    lastBlockStart = data.Length() - ( data.Length() % KDCFKeySize );
    // Loop through the data, excluding aIV
    for ( TInt i = KDCFKeySize; i  < lastBlockStart; i+= KDCFKeySize )
        {
        data.Set( retBuf->Des().MidTPtr( i, KDCFKeySize ) );

        cbc->Transform( data );
        }

    if ( aAddPadding )
        {
        TInt dataLength = retBuf->Length();
        TUint8 padding( static_cast< TUint8 >
            ( lastBlockStart + KDCFKeySize - dataLength ) );

        __ASSERT_DEBUG( lastBlockStart + KDCFKeySize - dataLength <= KDCFKeySize,
            User::Invariant() );

        retBuf->Des().SetLength( lastBlockStart + KDCFKeySize );

        for ( TInt i = dataLength; i < retBuf->Length(); ++i )
            {
            retBuf->Des()[ i ] = padding;
            }

        data.Set( retBuf->Des().MidTPtr( lastBlockStart, KDCFKeySize ) );
        cbc->Transform( data );
        }

    CleanupStack::Pop( retBuf );
    CleanupStack::PopAndDestroy( cbc );
    return retBuf;

    }

//  End of File
