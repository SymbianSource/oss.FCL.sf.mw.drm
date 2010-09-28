/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DRM Utility
*
*/


// INCLUDE FILES
#include <oma2dcf.h>
#include <drmagents.h>
#include <drmutility.h>
#include <centralrepository.h>
#include <utf.h>

#include "Oma1Dcf.h"

#include "DrmUtilityInternalcrkeys.h"      // Cenrep extension for OmaBased

// CONSTANTS
_LIT8( KASFHeaderObject, "75B22630668E11CFA6D900AA0062CE6C" );
_LIT8( KWrmHeader, "W\0R\0M\0H\0E\0A\0D\0E\0R\0" );

const TInt KMinContentLength( 16 );
const TInt KOma2EncryptionFieldOffset( 8 );
const TInt KOmaHeaderLength( 512 );
// This constant is for OMA1 case in which a buffer of 35-40
// bytes is needed to recognize the mime type.
const TInt KMinContentLengthOma1Based( 40 );
const TInt KWMHeaderObjectLength( 8 );
const TInt KWMTopLevelHeaderObjectLength( 30 );
const TInt KMaxWMHeaderLength( ( KMaxTInt32 / 2 ) - 1 );

const TInt KCenRepDataLength( 50 );

// ============================ LOCAL FUNCTIONS ================================

// -----------------------------------------------------------------------------
// FormatGUID
// -----------------------------------------------------------------------------
//
LOCAL_C void FormatGUID( TDes8 &aGUID )
    {
    TBuf8<16> copyGUID( aGUID );
    TInt i( 0 );
    for ( i = 0; i < 4; i++ )
        {
        copyGUID[i] = aGUID[3-i];
        }
    for ( i = 4; i < 6; i++ )
        {
        copyGUID[i] = aGUID[9 - i];
        }
    for ( i = 6; i < 8; i++ )
        {
        copyGUID[i] = aGUID[13 - i];
        }
    for ( i = 8; i < 16 ; i++)
        {
        copyGUID[i] = aGUID[i];
        }
    aGUID.Delete( 0, 32 );
    for ( i = 0; i < 16; i++ )
        {
        aGUID.AppendNumFixedWidthUC( copyGUID[i], EHex, 2 );
        }
    }

// -----------------------------------------------------------------------------
// ConvertToInt64
// -----------------------------------------------------------------------------
//
LOCAL_C TInt64 ConvertToInt64( TDesC8& aDes )
    {
    TInt64 num( 0 );
    TInt i( 0 );
    for ( i = 7 ; i >= 0; i-- )
        {
        num <<= 8;
        num |= aDes[i];
        }
    return num;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------
//
DRM::CDrmUtility::CDrmUtility()
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
DRM::CDrmUtility::~CDrmUtility()
    {
    delete iOmaBasedAgentName;
    delete iOmaBasedMimeType;
    }

// -----------------------------------------------------------------------------
// CDrmUtility::NewLC
// First phase constructor
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmUtility* DRM::CDrmUtility::NewLC()
    {
    DRM::CDrmUtility* self( new( ELeave ) CDrmUtility );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CDrmUtility::NewL
// First phase constructor
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmUtility* DRM::CDrmUtility::NewL()
    {
    DRM::CDrmUtility* self( NewLC() );
    CleanupStack::Pop();
    return self;
    }

void DRM::CDrmUtility::ConstructL()
    {
    TInt err( KErrNone );

     TRAP(err, FetchOmaBasedInfoL() );
     if( err)
         {
         if( iOmaBasedAgentName )
             {
             delete iOmaBasedAgentName;
             }
         iOmaBasedAgentName = NULL;
         if( iOmaBasedMimeType )
             {
             delete iOmaBasedMimeType;
             }
         iOmaBasedMimeType = NULL;
         }
    }

// -----------------------------------------------------------------------------
// CDrmUtility::FetchOmaBasedInfoL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtility::FetchOmaBasedInfoL()
    {
    TInt err = KErrNone;
    CRepository* repository( NULL );
    RBuf bOmaBasedAgentName;
    RBuf bOmaBasedMimeType;

    CleanupClosePushL(bOmaBasedAgentName);
    CleanupClosePushL(bOmaBasedMimeType);
    bOmaBasedAgentName.CreateL( KCenRepDataLength );
    bOmaBasedMimeType.CreateL( KCenRepDataLength );

    TRAP( err, repository = CRepository::NewL( KCRUidOmaBased ) );
    if ( !err )
        {
        CleanupStack::PushL( repository );

        err = repository->Get( KDrmOmaBasedName, bOmaBasedAgentName );
        if( !err )
            {
            iOmaBasedAgentName = bOmaBasedAgentName.AllocL();
            }

        err = repository->Get( KOmaBasedMimeType, bOmaBasedMimeType );
        if( !err )
            {
            iOmaBasedMimeType = CnvUtfConverter::ConvertFromUnicodeToUtf8L( bOmaBasedMimeType );
            }
        CleanupStack::PopAndDestroy( repository );
        }

    CleanupStack::PopAndDestroy(2);

    User::LeaveIfError( err );
    }


// -----------------------------------------------------------------------------
// CDrmUtility::GetDrmInfoL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool DRM::CDrmUtility::GetDrmInfoL(
    RFile& aFileHandle,
    TPtrC& aAgent,
    DRM::TDrmProtectionStatus& aProtected ) const
    {
    TInt r( KErrNone );
    HBufC8* buffer( NULL );
    TInt pos( 0 );
    RFile file;
    TBuf8< 32 > header;
    TInt64 headerSize( 0 );
    TBool isDrmFile( EFalse );
    TPtr8 headerPtr( NULL, 0 );
    TInt dataLength = 0;

    aProtected = DRM::EUUnprotected;
    aAgent.Set( KNullDesC );

    CheckFileHandleL( aFileHandle );
    User::LeaveIfError( file.Duplicate( aFileHandle ) );
    CleanupClosePushL( file );

    User::LeaveIfError( file.Seek( ESeekStart, pos ) );

    // Check if the file is an ASF file
    // To be Checked on runtime wether WM DRM is supporeted or not
    User::LeaveIfError( file.Read( 0, header, KMinContentLength ) );
    if ( header.Length() < KMinContentLength )
        {
        User::Leave( KErrArgument );
        }

    FormatGUID( header );

    if ( header == KASFHeaderObject )
        {
        // It's ASF, check still whether it's WM DRM protected or not
        file.Read( header, KWMHeaderObjectLength );
        headerSize = ConvertToInt64( header );
        if( headerSize <= KWMTopLevelHeaderObjectLength ||
            headerSize > KMaxWMHeaderLength )
            {
            User::Leave( KErrArgument );
            }
        buffer = HBufC8::NewLC( headerSize );

        headerPtr.Set( buffer->Des() );
        User::LeaveIfError( file.Read( headerPtr,
                headerSize - ( KMinContentLength + KWMHeaderObjectLength ) ) );

        r = headerPtr.Find( KWrmHeader );
        if ( r == KErrNotFound )
            {
            aProtected = DRM::EUUnprotected;
            }
        else
            {
            isDrmFile = ETrue;
            aProtected = DRM::EUProtected;
            aAgent.Set( DRM::KDrmWMAgentName );
            }
        CleanupStack::PopAndDestroy( buffer ); // buffer
        }
    else
        {
        // Check whether it's OMA DRM protected or not
        buffer = HBufC8::NewLC( KOmaHeaderLength );

        headerPtr.Set( buffer->Des() );
        User::LeaveIfError( file.Read( 0, headerPtr ));

        if ( COma1Dcf::IsValidDcf( headerPtr ) )
            {
            isDrmFile = ETrue;
            aProtected = DRM::EUProtected;
            aAgent.Set( DRM::KDrmOmaAgentName );
            }
        else if ( COma2Dcf::IsValidDcf( headerPtr ) )
            {
            isDrmFile = ETrue;
            _LIT8( KCommonHeadersBox, "ohdr" );
            pos = headerPtr.Find( KCommonHeadersBox );

            // If no box can be found or if there isn't enough data
            // set protection as unknown
            if( pos == KErrNotFound ||
                headerPtr.Length() < pos + KOma2EncryptionFieldOffset )
                {
                aProtected = DRM::EUUnknown;
                }
            // If encryption field is 0, then content isn't protected
            else if ( !headerPtr[pos + KOma2EncryptionFieldOffset] )
                {
                aProtected = DRM::EUUnprotected;
                }
            else
                {
                aProtected = DRM::EUProtected;
                }
            aAgent.Set( DRM::KDrmOmaAgentName );
            }
        else if ( (buffer->Des())[0] == 1)
            {
            // set the mimetype from the buffer which is in the beginning
            // starting from byte 3 with the length specified at position 2
            // also make sure the data length is greater or equal to the
            // position + the 2nd byte of the content
            dataLength = 3 + (buffer->Des())[1];
            if( buffer->Length() >= dataLength )
                {           
                TPtrC8 mimeType( buffer->Des().Mid(3, (buffer->Des())[1]) );
                if( !mimeType.CompareF( *iOmaBasedMimeType ) )
                    {
                    aAgent.Set( *DRM::CDrmUtility::iOmaBasedAgentName );
                    isDrmFile = ETrue;
                    }
                }
            }
        CleanupStack::PopAndDestroy( buffer );
        }
    CleanupStack::PopAndDestroy( &file ); // file
    return isDrmFile;
    }

// -----------------------------------------------------------------------------
// CDrmUtility::GetDrmInfoL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool DRM::CDrmUtility::GetDrmInfoL(
    const TDesC8& aContent,
    TPtrC& aAgent,
    DRM::TDrmProtectionStatus& aProtected ) const
    {
    TInt r( KErrNone );
    RFile file;
    TPtr8 asfPtr( NULL, 0 );
    TBuf8< 32 > asfGuidHex;
    TBool isDrmFile( EFalse );
    TInt dataLength = 0;

    aProtected = DRM::EUUnprotected;
    aAgent.Set( KNullDesC );

    if ( aContent.Length() < KMinContentLength )
        {
        User::Leave( KErrArgument );
        }

    // Check if the file is an ASF file
    asfPtr.Set(
        const_cast<TUint8*>( asfGuidHex.Ptr() ), 0, KMinContentLength * 2 );
    asfPtr.Copy( aContent.Left( KMinContentLength ) );
    FormatGUID( asfPtr );

    if ( asfPtr == KASFHeaderObject )
        {
        // It's ASF, check still whether it's WM DRM protected or not
        r = aContent.Find( KWrmHeader );
        if ( r == KErrNotFound )
            {
            aProtected = DRM::EUUnprotected;
            }
        else
            {
            isDrmFile = ETrue;
            aProtected = DRM::EUProtected;
            aAgent.Set( DRM::KDrmWMAgentName );
            }
        }
    else
        {
        // Check whether it's OMA DRM protected or not.
        if ( ( aContent.Length() >= KMinContentLengthOma1Based ) &&
            ( COma1Dcf::IsValidDcf( aContent ) ) )
            {
            isDrmFile = ETrue;
            aProtected = DRM::EUProtected;
            aAgent.Set( DRM::KDrmOmaAgentName );
            }
        else if ( COma2Dcf::IsValidDcf( aContent ) )
            {
            isDrmFile = ETrue;
            _LIT8( KCommonHeadersBox, "ohdr" );
            TInt pos( aContent.Find( KCommonHeadersBox ) );

            // If no box can be found or if there isn't enough data
            // set protection as unknown
            if ( pos == KErrNotFound ||
                 aContent.Length() < pos + KOma2EncryptionFieldOffset )
                {
                aProtected = DRM::EUUnknown;
                }
            // If encryption field is 0, then content isn't protected
            else if ( !aContent[pos + KOma2EncryptionFieldOffset] )
                {
                aProtected = DRM::EUUnprotected;
                }
            else
                {
                aProtected = DRM::EUProtected;
                }
            aAgent.Set( DRM::KDrmOmaAgentName );
            }
        else if ( (aContent)[0] == 1)
            {
            // set the mimetype from the buffer which is in the beginning
            // starting from byte 3 with the length specified at position 2
            // also make sure the data length is greater or equal to the
            // position + the 2nd byte of the content
            dataLength = 3 + (aContent)[1];
            if( aContent.Length() >= dataLength )
                {
                TPtrC8 mimeType( aContent.Mid(3, (aContent)[1]) );
                if( !mimeType.CompareF( *iOmaBasedMimeType ) )
                    {
                    aAgent.Set( *DRM::CDrmUtility::iOmaBasedAgentName );
                    isDrmFile = ETrue;
                    aProtected = DRM::EUProtected;
                    }
                }
            }
        }
    return isDrmFile;
    }

// -----------------------------------------------------------------------------
// CDrmUtility::IsProtectedL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool DRM::CDrmUtility::IsProtectedL( RFile& aFileHandle ) const
    {
    DRM::TDrmProtectionStatus protection( DRM::EUUnknown );
    TPtrC agent( KNullDesC );
    TBool isDrmFile( EFalse );
    TBool isProtected( EFalse );

    isDrmFile = GetDrmInfoL( aFileHandle, agent, protection );

    if ( isDrmFile && protection == DRM::EUProtected )
        {
        isProtected = ETrue;
        }
    return isProtected;
    }


// -----------------------------------------------------------------------------
// CDrmUtility::IsProtectedL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool DRM::CDrmUtility::IsProtectedL( const TDesC8& aContent ) const
    {
    DRM::TDrmProtectionStatus protection( DRM::EUUnknown );
    TPtrC agent( KNullDesC );
    TBool isDrmFile( EFalse );
    TBool isProtected( EFalse );

    isDrmFile = GetDrmInfoL( aContent, agent, protection );

    if( isDrmFile && protection == DRM::EUProtected )
        {
        isProtected = ETrue;
        }
    return isProtected;
    }


// -----------------------------------------------------------------------------
// CDrmUtility::GetAgentL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool DRM::CDrmUtility::GetAgentL(
    RFile& aFileHandle,
    TPtrC& aAgent ) const
    {
    DRM::TDrmProtectionStatus protection( DRM::EUUnknown );
    TBool isDrmFile( EFalse );

    isDrmFile = GetDrmInfoL( aFileHandle, aAgent, protection );

    return isDrmFile;
    }

// -----------------------------------------------------------------------------
// CDrmUtility::GetAgentL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool DRM::CDrmUtility::GetAgentL(
    const TDesC8& aContent,
    TPtrC& aAgent ) const
    {
    DRM::TDrmProtectionStatus protection( DRM::EUUnknown );
    TBool isDrmFile( EFalse );

    isDrmFile = GetDrmInfoL( aContent, aAgent, protection );

    return isDrmFile;
    }


// -----------------------------------------------------------------------------
// CDrmUtility::CheckFileHandlerL
// Checks whether given filehandle is valid if not leaves with KErrArgument
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUtility::CheckFileHandleL( RFile& aFileHandle ) const
    {
    if ( !aFileHandle.SubSessionHandle() )
        {
        User::Leave( KErrBadHandle );
        }
    }

//  End of File
