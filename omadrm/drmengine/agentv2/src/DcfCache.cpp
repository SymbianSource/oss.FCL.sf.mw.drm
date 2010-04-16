/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the DCF cache class
*
*/

// INCLUDE FILES
#include <f32file.h>
#include <e32base.h>
#include <symmetric.h>
#include <rijndael.h>
#include "DcfCache.h"
#include "DcfCommon.h"
#include "DRMRightsClient.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDcfCache::CDcfCache
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
#ifdef ASYNC_READ

CDcfCache::CDcfCache(
    RDRMRightsClient& aRightsClient,
    RFile& aFile,
    CDcfCommon& aDcf,
    TInt aPageSize,
    TInt aPageCount ) :
CActive( CActive::EPriorityStandard ),
iFile( aFile ),
iRightsClient(aRightsClient ),
iDcf( aDcf ),
iPageSize( aPageSize ),
iPageCount( aPageCount ),
iDecryptionMode( EServerSide ),
iDes( 0 ),
iAsyncReadingOngoing( EFalse )
    {
    }

#else

CDcfCache::CDcfCache(
    RDRMRightsClient& aRightsClient,
    RFile& aFile,
    CDcfCommon& aDcf,
    TInt aPageSize,
    TInt aPageCount ) :
iFile( aFile ),
iRightsClient( aRightsClient ),
iDcf( aDcf ),
iPageSize( aPageSize ),
iPageCount( aPageCount ),
iDecryptionMode( EServerSide )
    {
    }
#endif
// -----------------------------------------------------------------------------
// CDcfCache::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDcfCache::ConstructL()
    {
    for ( TInt i = 0; i < iPageCount; i++ )
        {
        iPage.AppendL( new ( ELeave ) TUint8[iPageSize] );
        iPageUsageCount.AppendL( -1 );
        iPagePosition.AppendL( -1 );
        }
    }

// -----------------------------------------------------------------------------
// CDcfCache::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDcfCache* CDcfCache::NewL( RDRMRightsClient& aRightsClient, RFile& aFile,
    CDcfCommon& aDcf, TInt aPageSize, TInt aPageCount )
    {
    CDcfCache* self = new ( ELeave ) CDcfCache( aRightsClient, aFile, aDcf,
        aPageSize, aPageCount );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CDcfCache::~CDcfCache
// Destructor
// -----------------------------------------------------------------------------
//
CDcfCache::~CDcfCache()
    {

#ifdef ASYNC_READ
    if ( IsActive() )
        {
        if ( iAsyncReadingOngoing )
            {
            Cancel();
            }
        else
            {
            TRequestStatus* status = &iStatus;
            User::RequestComplete( status, KErrNone );
            Cancel();
            }
        }
#endif

    iPage.ResetAndDestroy();
    iPage.Close();
    iPagePosition.Close();
    iPageUsageCount.Close();
    }

// -----------------------------------------------------------------------------
// CDcfCache::Read
// -----------------------------------------------------------------------------
//
TInt CDcfCache::Read( TInt& aPos, TDes8& aDes, TInt aLength )
    {
    TInt r = KErrNone;

    // Check that the position is valid and the length is valid
    if ( aPos < 0 || aLength < 0 )
        {
        return KErrArgument;
        }

    if ( iPageCount > 0 )
        {
        TRAP(r, CachedReadL(aPos, aDes, aLength));
        }
    else
        {
        TRAP(r, UncachedReadL(aPos, aDes, aLength));
        }
    if ( r != KErrNone )
        {
        RDebug::Printf( "CDcfCache::Read error: %d", r );
        }
    return r;
    }

// -----------------------------------------------------------------------------
// CDcfCache::Read
// -----------------------------------------------------------------------------
//

#ifdef ASYNC_READ

TInt CDcfCache::Read( TInt aPos, TDes8& aDes, TInt aLength,
    TRequestStatus& aStatus )
    {

    // Check that the position is valid and the length is valid
    if ( aPos < 0 || aLength < 0 )
        {
        return KErrArgument;
        }

    if ( !IsAdded() )
        {
        CActiveScheduler::Add( this );
        }

    if ( iAsyncReadingOngoing )
        {
        return KErrInUse;
        }

    iError = KErrNone;

    iAsyncReadingOngoing = ETrue;

    iPos = aPos;
    iLength = aLength;
    iDes = &aDes;
    iOperation = EPosRead;

    iAsyncStatus = &aStatus;
    *iAsyncStatus = KRequestPending;

    TRequestStatus* ownStatus = &iStatus;
    *ownStatus = KRequestPending;

    SetActive();
    User::RequestComplete( ownStatus, KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDcfCache::ReadCancel
// -----------------------------------------------------------------------------
//
void CDcfCache::ReadCancel( TRequestStatus& aStatus )
    {
    // only cancel if needed
    TRequestStatus* status( &aStatus );
    if ( IsActive() && iAsyncStatus == status )
        {
        if ( iAsyncReadingOngoing )
            {
            Cancel();
            iAsyncReadingOngoing = EFalse;
            iAsyncStatus = NULL;
            User::RequestComplete( status, KErrCancel );
            }
        }
    }

#endif

// -----------------------------------------------------------------------------
// CDcfCache::CachedReadL
// -----------------------------------------------------------------------------
//
void CDcfCache::CachedReadL( TInt& aPos, TDes8& aDes, TInt aLength )
    {
    TInt i;

    // Check that the position is valid and the length is valid
    if ( aPos < 0 || aLength < 0 )
        {
        User::Leave( KErrArgument );
        }

    aDes.SetLength( 0 );
    while ( aLength > 0 )
        {
        for ( i = 0; aLength > 0 && i < iPageCount; i++ )
            {
            if ( InPage( i, aPos ) )
                {
                CopyOut( i, aDes, aPos, aLength );
                }
            }
        if ( i == iPageCount && aLength > 0 )
            {
            i = GetFreePage();
            if ( iDcf.iEncryptionMethod == EMethodAES_128_CBC )
                {
                ReadAndDecryptPageL( i, aPos );
                CopyOut( i, aDes, aPos, aLength );
                }
            else
                {
                ReadPageL( i, aPos );
                CopyOut( i, aDes, aPos, aLength );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CDcfCache::UncachedReadL
// -----------------------------------------------------------------------------
//
void CDcfCache::UncachedReadL( TInt& aPos, TDes8& aDes, TInt aLength )
    {
    HBufC8* buffer = NULL;
    TPtr8 ptr( NULL, 0 );
    TBuf8<KDCFKeySize> iv;
    TInt pos;
    TInt length;

    // Check that the position is valid and the length is valid
    if ( aPos < 0 || aLength < 0 )
        {
        User::Leave( KErrArgument );
        }

    if ( aPos + aLength > iDcf.iPlainTextLength )
        {
        if ( aPos < iDcf.iPlainTextLength )
            {
            aLength = iDcf.iPlainTextLength - aPos;
            }
        else
            {
            aLength = 0;
            }
        }

    if ( iDcf.iEncryptionMethod == EMethodAES_128_CBC )
        {

        // Nothing to read:
        if ( aLength == 0 )
            {
            aDes.SetLength( aLength );
            return;
            }

        buffer = HBufC8::NewLC( aLength + 2 * KDCFKeySize );
        ptr.Set( buffer->Des() );
        pos = ( ( aPos / KDCFKeySize ) * KDCFKeySize ) + iDcf.iOffset;
        User::LeaveIfError( iFile.Read( pos, iv, KDCFKeySize ) );
        length = ( ( aLength / KDCFKeySize ) * KDCFKeySize ) + 2
            * KDCFKeySize;
        User::LeaveIfError( iFile.Read( ptr, length ) );

        // Make sure we have the correct amount of data
        if ( ptr.Length() % KDCFKeySize )
            {
            length = ( ( ptr.Length() / KDCFKeySize ) * KDCFKeySize );
            ptr.SetLength( length );
            }

        // If not enough was read to get any content return an empty descriptor
        if ( ptr.Length() == 0 )
            {
            aDes.SetLength( 0 );
            CleanupStack::PopAndDestroy( buffer );
            return;
            }

        DecryptL( iv, ptr );
        length = Min( aDes.MaxLength(), ptr.Length() - aPos % KDCFKeySize );
        aDes.Copy( ptr.Mid( aPos % KDCFKeySize, length ) );
        if ( aDes.Length() > aLength )
            {
            aDes.SetLength( aLength );
            }
        CleanupStack::PopAndDestroy( buffer );
        }
    else
        {
        User::LeaveIfError( iFile.Read( aPos + iDcf.iOffset, aDes, aLength ) );
        }
    aPos += aDes.Length();
    }

// -----------------------------------------------------------------------------
// CDcfCache::GetFreePage
// -----------------------------------------------------------------------------
//
TInt CDcfCache::GetFreePage()
    {
    TInt i;
    TInt n = -1;
    TUint c = KMaxTUint32;

    for ( i = 0; i < iPageCount; i++ )
        {
        if ( iPageUsageCount[i] == -1 )
            {
            n = i;
            break;
            }
        else if ( iPageUsageCount[i] < c )
            {
            c = iPageUsageCount[i];
            n = i;
            }
        }
    return n;
    }

// -----------------------------------------------------------------------------
// CDcfCache::ReadPageL
// -----------------------------------------------------------------------------
//
void CDcfCache::ReadPageL( TInt aPage, TInt aPosition )
    {
    TInt pos;
    TBuf8<KDCFKeySize> iv;

    iPageUsageCount[aPage] = 0;
    iPagePosition[aPage] = ( aPosition / iPageSize ) * iPageSize;
    pos = iPagePosition[aPage] + iDcf.iOffset;
    iFile.Seek( ESeekStart, pos );
    TPtr8 ptr( iPage[aPage], iPageSize );
    User::LeaveIfError( iFile.Read( ptr ) );
    }

// -----------------------------------------------------------------------------
// CDcfCache::ReadAndDecryptPageL
// -----------------------------------------------------------------------------
//
void CDcfCache::ReadAndDecryptPageL( TInt aPage, TInt aPosition )
    {
    TInt pos;
    TBuf8<KDCFKeySize> iv;

    iPageUsageCount[aPage] = 0;
    iPagePosition[aPage] = ( aPosition / iPageSize ) * iPageSize;
    pos = iPagePosition[aPage] + iDcf.iOffset;
    User::LeaveIfError( iFile.Seek( ESeekStart, pos ) );
    User::LeaveIfError( iFile.Read( iv ) );
    TPtr8 ptr( iPage[aPage], iPageSize );
    pos += KDCFKeySize;
    User::LeaveIfError( iFile.Read( ptr ) );
    ptr.SetLength( iPageSize );
    DecryptL( iv, ptr );
    }

// -----------------------------------------------------------------------------
// CDcfCache::CopyOut
// -----------------------------------------------------------------------------
//
void CDcfCache::CopyOut( TInt aPage, TDes8& aDes, TInt& aPosition,
    TInt& aLength )
    {
    TInt n;
    TInt offset;

    aLength = Min( aLength, iDcf.iPlainTextLength - aPosition );
    aLength = Min( aLength, aDes.MaxLength() - aDes.Length() );
    offset = aPosition - iPagePosition[aPage];
    n = Min( aLength, iPageSize - offset );
    aDes.Append( iPage[aPage] + offset, n );
    aLength -= n;
    aPosition += n;
    iPageUsageCount[aPage]++;
    }

// -----------------------------------------------------------------------------
// CDcfCache::InPage
// -----------------------------------------------------------------------------
//
TBool CDcfCache::InPage( TInt aPage, TInt aPosition )
    {
    return iPagePosition[aPage] != -1 && iPagePosition[aPage] <= aPosition
        && aPosition < iPagePosition[aPage] + iPageSize ? ETrue : EFalse;
    }

// -----------------------------------------------------------------------------
// CDcfCache::SetKey
// -----------------------------------------------------------------------------
//
void CDcfCache::SetKey( const TDesC8& aKey )
    {
    iKey.Copy( aKey );
    iDecryptionMode = EClientSide;
    }

// -----------------------------------------------------------------------------
// CDcfCache::DecryptL
// -----------------------------------------------------------------------------
//
void CDcfCache::DecryptL( const TDesC8& aIv, TPtr8& aPtr )
    {
    if ( iDecryptionMode == EClientSide )
        {
        CModeCBCDecryptor* cbc( NULL );
        CAESDecryptor* aes( CAESDecryptor::NewLC( iKey ) );
        cbc = CModeCBCDecryptor::NewL( aes, aIv );
        CleanupStack::Pop( aes );
        aes = NULL;
        CleanupStack::PushL( cbc );
        for ( TInt count = 0; count < aPtr.Length(); count += KDCFKeySize )
            {
            TPtr8 d( aPtr.MidTPtr( count, KDCFKeySize ) );
            cbc->Transform( d );
            }
        CleanupStack::PopAndDestroy( cbc );
        }
    else
        {
        User::LeaveIfError( iRightsClient.Decrypt( aIv, aPtr ) );
        }
    }

#ifdef ASYNC_READ

// -----------------------------------------------------------------------------
// CDcfCache::RunL
// -----------------------------------------------------------------------------
//
void CDcfCache::RunL()
    {

    switch ( iOperation )
        {
        case ENonPosRead:
            {
            break;
            }

        case EPosRead:
            {
            ReadAsyncL();
            iAsyncReadingOngoing = EFalse;
            User::RequestComplete( iAsyncStatus, KErrNone );
            iAsyncStatus = NULL;
            break;
            }

        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CDcfCache::DoCancel
// -----------------------------------------------------------------------------
//
void CDcfCache::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CDcfCache::RunError
// -----------------------------------------------------------------------------
//
TInt CDcfCache::RunError( TInt aError )
    {
    iError = aError;
    User::RequestComplete( iAsyncStatus, iError );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDcfCache::ReadAsyncL
// -----------------------------------------------------------------------------
//

void CDcfCache::ReadAsyncL()
    {
    if ( iPageCount > 0 )
        {
        TRAP(iError, CachedReadL(iPos, *iDes, iLength));
        }
    else
        {
        TRAP(iError, UncachedReadL(iPos, *iDes, iLength));
        }
    }
#endif //ASYNC_READ
//  End of File
