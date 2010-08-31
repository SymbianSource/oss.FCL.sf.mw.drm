/*
* Copyright (c) 2002 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of class representeting discrete OMA2 DCF
*
*/



// INCLUDE FILES
#include <e32const.h> // KMaxTUint32
#include <e32cmn.h> // Max, Min
#include <e32base.h>
#include <f32file.h>
#include <utf.h>
#include <caf/caf.h>
#include <caf/cafplatform.h>
#include "Oma2Agent.h"
#include "oma2dcf.h"
#include "Oma2DcfPartInfo.h"

// LOCAL CONSTANTS AND MACROS

#define M4CC( a ) ( ( a[ 0 ] << 24 ) + ( a[ 1 ] << 16 ) + ( a[ 2 ] << 8 ) + a[ 3 ] )

const TUint32 KDiscreteMediaHeaders( M4CC( "odhe" ) );
const TUint32 KContainer( M4CC( "odrm" ) );
const TUint32 KContentObject( M4CC( "odda" ) );
const TUint32 KCommonHeaders( M4CC( "ohdr" ) );
const TUint32 KMutableInfo( M4CC( "mdri" ) );
const TUint32 KTransactionTracking( M4CC( "odtt" ) );
const TUint32 KRightsObject( M4CC( "odrb" ) );
const TUint32 KGroupId( M4CC( "grpi" ) );
const TUint32 KUserData( M4CC( "udta" ) );

// supported user data boxes
_LIT8( KTitle, "titl" );
_LIT8( KDescription, "dscp" );
_LIT8( KCopyright, "cprt" );
_LIT8( KAuthor, "auth" );
_LIT8( KIconUri, "icnu" );
_LIT8( KInfoURL, "infu" );
_LIT8( KPerformer, "perf" );
_LIT8( KGenre, "gnre" );
_LIT8( KRating, "rtng" );
_LIT8( KClassification, "clsf" );
_LIT8( KKeyword, "kywd" );
_LIT8( KLocationInformation, "loci" );
_LIT8( KAlbum, "albm" );
_LIT8( KRecordingYear, "yrrc" );
_LIT8( KCoverUri, "cvru" );
_LIT8( KLyricsURL, "lrcu" );


_LIT8( KMdri, "mdri" );
_LIT8( KOdtt, "odtt" );
_LIT8( KOdrb, "odrb" );

// textual headers
_LIT8( KSilentHeader, "Silent" );
_LIT8( KPreviewHeader, "Preview" );
_LIT8( KContentURLHeader, "ContentURL" );
_LIT8( KContentVersionHeader, "ContentVersion" );
_LIT8( KContentLocationHeader, "Content-Location" );

// supported custom textual headers
_LIT8( KIconUriHeader, "IconURI" );
_LIT8( KTitleHeader, "Title" );
_LIT8( KDescriptionHeader, "Description" );
_LIT8( KAuthorHeader, "Author" );
_LIT8( KCopyrightHeader, "Copyright" );
_LIT8( KInfoURLHeader, "InfoURL" );

// textual headers, methods
_LIT8( KSilentMethodOnDemand, "on-demand" );
_LIT8( KSilentMethodInAdvance, "in-advance" );
_LIT8( KPreviewMethodInstant, "instant" );
_LIT8( KPreviewMethodPreviewRights, "preview-rights" );
_LIT8( KEmptyTransactionId, "0000000000000000" );

_LIT8( KTerminator, "\0" );
_LIT8( KSeparator, ":" );
_LIT8( KParamSeparator, ";" );

const TInt KBrandingSize( 20 );
const TInt KMaxBoxHeaderSize( 16 );
const TInt KLengthBoxSize( 4 );
const TInt KLengthBoxType( 4 );
const TInt KLengthBoxSize64( 8 );
const TInt KLengthVersion( 1 );
const TInt KLengthFlags( 3 );
const TInt KLengthEncMethod( 1 );
const TInt KLengthPadType( 1 );
const TInt KLengthPlainTextSize( 8 );
const TInt KLengthDataLengthSize( 8 );
const TInt KLengthContentIdSize( 2 );
const TInt KLengthRiUrlSize( 2 );
const TInt KLengthTextHeadersSize( 2 );
const TInt KLengthTerminator( 1 );

// Smallest possible ISO based media box size
// See ISO/IEC 14496-12 and DRM-DCF-CLI-12 in OMA-TS-DRM-DCF-V2_1
const TUint32 KMinBoxSize( KLengthBoxSize + KLengthBoxType );
// Largest supported ISO based media box size
const TUint32 KMaxBoxSize( KMaxTUint32 );

const TUint KUserDataBoxMask( 0x000001 );

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ReadUint32FromBlock
// -----------------------------------------------------------------------------
//
LOCAL_C TUint32 ReadUint32FromBlock( const TDesC8& aBlock, TInt aOffset )
    {
    return ( aBlock[ aOffset ] << 24 ) +
        ( aBlock[ aOffset + 1 ] << 16 ) +
        ( aBlock[ aOffset + 2 ] << 8 ) +
        aBlock[ aOffset + 3 ];
    }

// -----------------------------------------------------------------------------
// ReadUint16FromBlock
// -----------------------------------------------------------------------------
//
LOCAL_C TUint16 ReadUint16FromBlock( const TDesC8& aBlock, TInt aOffset )
    {
    return ( ( aBlock[ aOffset ] << 8 ) + aBlock[ aOffset + 1 ] );
    }

// -----------------------------------------------------------------------------
// WriteIntToBlock
// -----------------------------------------------------------------------------
//
LOCAL_C void WriteIntToBlock( TInt aValue, TDes8& aBlock, TInt aOffset )
    {
    aBlock.SetLength( 4 );
    aBlock[ aOffset ] =     ( aValue & 0xff000000 ) >> 24;
    aBlock[ aOffset + 1 ] = ( aValue & 0x00ff0000 ) >> 16;
    aBlock[ aOffset + 2 ] = ( aValue & 0x0000ff00 ) >> 8;
    aBlock[ aOffset + 3 ] = ( aValue & 0x000000ff );
    }

// -----------------------------------------------------------------------------
// ValidateBoxSizeL
// -----------------------------------------------------------------------------
//
LOCAL_C void ValidateBoxSizeL(
    const TUint32 aSize,
    const TUint32 aMinSize = KMinBoxSize,
    const TUint32 aMaxSize = KMaxBoxSize )
    {
    TUint32 minSize( Max( aMinSize, KMinBoxSize) );
    TUint32 maxSize( Min( aMaxSize, KMaxBoxSize ) );
    if ( aSize < minSize )
        {
        User::Leave( KErrArgument );
        }
    if ( aSize > maxSize )
        {
        User::Leave( KErrArgument );
        }
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// COma2Dcf::COma2Dcf
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
COma2Dcf::COma2Dcf():
    iEncrytionPadding( EPaddingNone ),
    iSilentRefresh( ENoSilentRefresh ),
    iPreview( ENoPreview ),
    iPreviewParameter( NULL ),
    iSilentParameter( NULL ),
    iTextualHeaders( NULL ),
    iContentUrl( NULL ),
    iContentVersion( NULL ),
    iContentLocation( NULL ),
    iTransactionTracking( NULL ),
    iUserData( NULL ),
    iAuthor( NULL ),
    iCopyRight( NULL ),
    iInfoUri( NULL ),
    iGroupId( NULL ),
    iGroupKey( NULL ),
    iGkEncryptionMethod( EMethodNULL ),
    iMutablePart( NULL ),
    iPerformer( NULL ),
    iGenre( NULL ),
    iRatingInfo( NULL ),
    iClassificationInfo( NULL ),
    iKeyword( NULL ),
    iLocInfoName( NULL ),
    iLocInfoAstronomicalBody( NULL ),
    iLocInfoAdditionalNotes( NULL ),
    iAlbumTitle( NULL ),
    iCoverUri( NULL ),
    iLyricsURL( NULL ),
    iRecordingYear( 0 ),
    iAlbumTrack( 0 )
    {
    }

// -----------------------------------------------------------------------------
// COma2Dcf::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void COma2Dcf::ConstructL(
    const RFile& aFile,
    TInt aPart )
    {
    CDcfCommon::ConstructL( aFile );
    ReadPartsL();
    User::LeaveIfError( OpenPart( aPart ) );
    }

// -----------------------------------------------------------------------------
// COma2Dcf::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C COma2Dcf* COma2Dcf::NewL(
    const RFile& aFile,
    TInt aPart )
    {
    COma2Dcf* self( new ( ELeave ) COma2Dcf );

    CleanupStack::PushL( self );
    self->ConstructL( aFile, aPart );
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
EXPORT_C COma2Dcf::~COma2Dcf()
    {
    delete iPreviewParameter;
    delete iSilentParameter;
    delete iTextualHeaders;
    delete iContentUrl;
    delete iContentVersion;
    delete iContentLocation;
    delete iTransactionTracking;
    delete iUserData;
    delete iAuthor;
    delete iCopyRight;
    delete iInfoUri;
    delete iGroupId;
    delete iGroupKey;
    delete iPerformer;
    delete iGenre;
    delete iRatingInfo;
    delete iClassificationInfo;
    delete iKeyword;
    delete iLocInfoName;
    delete iLocInfoAstronomicalBody;
    delete iLocInfoAdditionalNotes;
    delete iAlbumTitle;
    delete iCoverUri;
    delete iLyricsURL;
    delete iMutablePart;

    iRightsObjects.ResetAndDestroy();
    iRightsObjects.Close();
    iParts.ResetAndDestroy();
    iParts.Close();
    }

// -----------------------------------------------------------------------------
// COma2Dcf::IsValidDcf
// -----------------------------------------------------------------------------
//
EXPORT_C TBool COma2Dcf::IsValidDcf(
    const TDesC8& aDcfFragment )
    {
    TBool r( EFalse );

    if ( aDcfFragment.Length() >= KBrandingSize &&
        aDcfFragment.Left( KBrandingSize ).Compare( KOma2DcfBranding ) == 0 )
        {
        r = ETrue;
        }
    return r;
    }

// -----------------------------------------------------------------------------
// COma2Dcf::ReadPartsL
// -----------------------------------------------------------------------------
//
void COma2Dcf::ReadPartsL( void )
    {
    TBuf8<KBrandingSize> buffer;
    TInt size;
    TUint32 type;
    TInt offset;
    TInt headerSize;
    COma2DcfPartInfo* part;

    User::LeaveIfError( iFile.Read( 0, buffer, KBrandingSize ) );
    if ( !IsValidDcf( buffer ) )
        {
        User::Leave( KErrArgument );
        }
    iVersion = ReadUint32FromBlock( buffer, 12 );
    offset = KBrandingSize;
    iParts.Reset();
    do
        {
        ReadBoxSizeAndTypeL( offset, size, type, headerSize );
        if ( headerSize > 0 )
            {
            part = COma2DcfPartInfo::NewL();
            part->iType = type;
            part->iSize = size;
            part->iOffset = offset;
            CleanupStack::PushL( part );
            if ( type == KContainer )
                {
                ReadPartInfoL( part, offset );
                iParts.Append( part );
                CleanupStack::Pop( part );
                }
            else if ( type == KMutableInfo )
                {
                iMutablePart = part;
                CleanupStack::Pop( part );
                ReadMutableInfoL( offset, size );
                }
            else
                {
                CleanupStack::PopAndDestroy( part );
                }
            }
        offset += size;
        }
    while ( offset < iLength && headerSize > 0 );

    delete iContentID;
    iContentID = NULL;
    delete iMimeType;
    iMimeType = NULL;

    if ( iParts.Count() == 0 ||
        iParts[ 0 ]->iContentId == NULL ||
        iParts[ 0 ]->iMimeType == NULL )
        {
        User::LeaveIfError( KErrArgument );
        }
    iContentID = iParts[ 0 ]->iContentId->AllocL();
    iMimeType = iParts[ 0 ]->iMimeType->AllocL();
    }

// -----------------------------------------------------------------------------
// COma2Dcf::CheckUniqueId
// -----------------------------------------------------------------------------
//
TInt COma2Dcf::CheckUniqueId( const TDesC& aUniqueId )
    {
    TInt r( CDcfCommon::CheckUniqueId( aUniqueId ) );

    if ( r == KErrNotFound )
        {
        HBufC8* id( NULL );
        TRAPD( err,
            id = CnvUtfConverter::ConvertFromUnicodeToUtf8L( aUniqueId ) );
        if ( err == KErrNone )
            {
            r = KErrNotFound;
            for ( TInt i( 0 );
                i < iParts.Count() && r == KErrNotFound;
                i++ )
                {
                if ( !id->Compare( *iParts[ i ]->iContentId ) )
                    {
                    r = i;
                    }
                }
            delete id;
            }
        else
            {
            r = err;
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// COma2Dcf::OpenPart
// -----------------------------------------------------------------------------
//
TInt COma2Dcf::OpenPart(
    const TDesC& aUniqueId )
    {
    return OpenPart( CheckUniqueId( aUniqueId ) );
    }

// -----------------------------------------------------------------------------
// COma2Dcf::OpenPart
// -----------------------------------------------------------------------------
//
TInt COma2Dcf::OpenPart(
    TInt aPart )
    {
    TInt r( KErrNone );
    TInt size( 0 );

    if ( aPart >= 0 && aPart < iParts.Count() )
        {
        TRAP( r, ReadContainerL( iParts[ aPart ]->iOffset, size ) );
        }
    else
        {
        r = KErrNotFound;
        }
    return r;
    }

// -----------------------------------------------------------------------------
// COma2Dcf::GetPartIdsL
// -----------------------------------------------------------------------------
//
void COma2Dcf::GetPartIdsL( RPointerArray<HBufC8>& aPartList )
    {
    aPartList.ResetAndDestroy();
    for ( TInt i( 0 ); i < iParts.Count(); i++ )
        {
        aPartList.Append( iParts[ i ]->iContentId->AllocL() );
        }
    }

// -----------------------------------------------------------------------------
// COma2Dcf::ReadContainerL
// -----------------------------------------------------------------------------
//
void COma2Dcf::ReadContainerL(
    TInt aOffset,
    TInt& aSize )
    {
    TUint32 type( 0 );
    TInt size( 0 );

    ReadBoxSizeAndTypeL( aOffset, aSize, type, size );
    VerifyTypeL( type, KContainer );
    aOffset += size + KLengthVersion + KLengthFlags;
    ReadDiscreteMediaHeaderL( aOffset, size );
    aOffset += size;
    ReadContentObjectL( aOffset, size );
    }

// -----------------------------------------------------------------------------
// COma2Dcf::ReadDiscreteMediaHeaderL
// -----------------------------------------------------------------------------
//
void COma2Dcf::ReadDiscreteMediaHeaderL(
    TInt aOffset,
    TInt& aSize )
    {
    TBuf8<1> valBuf;
    TBuf8<4> udtaBuf;
    TInt size( 0 );
    TUint32 type( 0 );
    TUint8 length( 0 );
    TPtr8 ptr( 0, 0 );

    TBuf8< KLengthVersion + KLengthFlags > buffer;
    TUint32 version_and_flags( 0 );

    ReadBoxSizeAndTypeL( aOffset, aSize, type, size );
    VerifyTypeL( type, KDiscreteMediaHeaders );

    // read flags and version
    aOffset += size;
    User::LeaveIfError( iFile.Read( aOffset, buffer ) );
    version_and_flags = ReadUint32FromBlock( buffer, 0 );
    aOffset += KLengthVersion + KLengthFlags;

    // read ContentType
    delete iMimeType;
    iMimeType = NULL;

    // input validation
    if ( aOffset < 0 )
        {
        // RFile::Read will panic if offset is negative
        User::Leave( KErrArgument );
        }

    User::LeaveIfError( iFile.Read( aOffset, valBuf, 1 ) );
    length = valBuf[ 0 ];
    aOffset += 1;
    iMimeType = HBufC8::NewMax( length );
    User::LeaveIfNull( iMimeType );
    ptr.Set( iMimeType->Des() );
    User::LeaveIfError( iFile.Read( ptr, length ) );
    aOffset += length;

    // read Common headers
    ReadCommonHeadersL( aOffset, size );

    // user data box would follow here
    if ( version_and_flags & KUserDataBoxMask )
        {
        aOffset += size;
        User::LeaveIfError( iFile.Read( aOffset, udtaBuf, 4 ) );
        size = ReadUint32FromBlock( udtaBuf, 0 );
        ReadUserDataL( aOffset, size );
        }
    }

// -----------------------------------------------------------------------------
// COma2Dcf::ReadContentObjectL
// -----------------------------------------------------------------------------
//
void COma2Dcf::ReadContentObjectL(
    TInt aOffset,
    TInt& aSize )
    {
    TBuf8<KMaxBoxHeaderSize> valBuf;
    TInt size( 0 );
    TUint32 type( 0 );

    ReadBoxSizeAndTypeL( aOffset, aSize, type, size );
    VerifyTypeL( type, KContentObject );
    aOffset += size + KLengthVersion + KLengthFlags;

    // read data length

    // input validation
    if ( aOffset < 0 )
        {
        // RFile::Read will panic if offset is negative
        User::Leave( KErrArgument );
        }

    User::LeaveIfError( iFile.Read( aOffset, valBuf, KLengthDataLengthSize ) );
    if ( ReadUint32FromBlock( valBuf, 0 ) != 0 )
        {
        // the size is larger than 2^32-1, we can't handle this.
        User::Leave( KErrOverflow );
        }
    else
        {
        iDataLength = ReadUint32FromBlock( valBuf, KLengthDataLengthSize / 2 );
        if ( iPlainTextLengthValid )
            {
            iPadding = iDataLength - iPlainTextLength;
            }
        }
    iOffset = aOffset + KLengthDataLengthSize;
    }

// -----------------------------------------------------------------------------
// COma2Dcf::ReadCommonHeadersL
// -----------------------------------------------------------------------------
//
void COma2Dcf::ReadCommonHeadersL(
    TInt aOffset,
    TInt& aSize )
    {
    TBuf8< KMaxBoxHeaderSize > valBuf;
    TInt size( 0 );
    TInt boxEnd( 0 );
    TUint32 type( 0 );
    TInt offset( 0 );
    TUint16 riURLLength( 0 );
    TUint16 contentIdLength( 0 );
    TUint16 textualHeadersLength( 0 );
    TUint16 length( 0 );
    HBufC8* buf( NULL );
    TPtr8 ptr( 0, 0 );

    ReadBoxSizeAndTypeL( aOffset, aSize, type, size );
    VerifyTypeL( type, KCommonHeaders );
    boxEnd = aOffset + aSize;
    aOffset += size + KLengthVersion + KLengthFlags;

    length = KLengthEncMethod + KLengthPadType + KLengthPlainTextSize
        + KLengthContentIdSize + KLengthRiUrlSize + KLengthTextHeadersSize;
    buf = HBufC8::NewLC( length );
    ptr.Set( buf->Des() );

    // input validation
    if ( aOffset < 0 )
        {
        // RFile::Read will panic if offset is negative
        User::Leave( KErrArgument );
        }
    User::LeaveIfError( iFile.Read( aOffset, ptr, length ) );

    offset = 0;
    iEncryptionMethod = static_cast< TEncryptionMethod >( ( *buf )[ 0 ] );
    offset += KLengthEncMethod;
    iEncrytionPadding = static_cast< TEncryptionPadding >( ( *buf )[ 1 ] );
    offset += KLengthPadType;

    iPlainTextLength = ReadUint32FromBlock( ptr, offset );
    offset +=  KLengthPlainTextSize / 2;
    if ( iPlainTextLength != 0 )
        {
        User::Leave( KErrOverflow );
        }
    else
        {
        iPlainTextLength = ReadUint32FromBlock( ptr, offset );
        if ( iPlainTextLength > 0 )
            {
            iPlainTextLengthValid = ETrue;
            }
        else
            {
            iPlainTextLengthValid = EFalse;
            }
        offset +=  KLengthPlainTextSize / 2;
        }

    contentIdLength = ReadUint16FromBlock( ptr, offset );
    offset += KLengthContentIdSize;
    riURLLength = ReadUint16FromBlock( ptr, offset );
    offset += KLengthRiUrlSize;
    textualHeadersLength  = ReadUint16FromBlock( ptr, offset );
    offset += KLengthTextHeadersSize;
    CleanupStack::PopAndDestroy( buf );
    buf = NULL;

    aOffset += offset;
    delete iContentID;
    iContentID = NULL;
    iContentID = HBufC8::NewMax( contentIdLength );
    User::LeaveIfNull( iContentID );
    ptr.Set( iContentID->Des() );
    User::LeaveIfError( iFile.Read( aOffset, ptr, contentIdLength ) );
    aOffset += contentIdLength;

    delete iRightsIssuerURL;
    iRightsIssuerURL = NULL;
    if ( riURLLength > 0 )
        {
        iRightsIssuerURL = HBufC8::NewL( riURLLength );
        ptr.Set( iRightsIssuerURL->Des() );
        User::LeaveIfError( iFile.Read( aOffset, ptr, riURLLength ) );
        aOffset += riURLLength;
        }

    delete iTextualHeaders;
    iTextualHeaders = NULL;
    if ( textualHeadersLength > 0 )
        {
        iTextualHeaders = HBufC8::NewL( textualHeadersLength );
        ptr.Set( iTextualHeaders->Des() );
        User::LeaveIfError( iFile.Read( aOffset, ptr, textualHeadersLength ) );
        aOffset += textualHeadersLength;
        ParseTextualHeadersL( ptr );
        }

    ReadExtendedHeadersL( aOffset, boxEnd );
    }

// -----------------------------------------------------------------------------
// COma2Dcf::ParseTextualHeadersL
// -----------------------------------------------------------------------------
//
void COma2Dcf::ParseTextualHeadersL(
    const TDesC8& aMemoryBlock )
    {
    HBufC8* buf1( NULL );
    HBufC8* method( NULL );
    TInt r( KErrNone );
    TInt silentOffset( 0 );
    TInt previewOffset( 0 );

    silentOffset = ReadOneTextualHeaderL(
        aMemoryBlock, KSilentHeader, buf1, r );
    if ( r != KErrNotFound && buf1 )
        {
        CleanupStack::PushL( buf1 );
        SetHeaderWithParameterL( *buf1, method, iSilentParameter );
        CleanupStack::PopAndDestroy( buf1 );
        buf1 = NULL;
        if ( method )
            {
            if ( !method->Compare( KSilentMethodOnDemand ) )
                {
                iSilentRefresh = EOnDemand;
                }
            else if ( !method->Compare( KSilentMethodInAdvance ) )
                {
                iSilentRefresh = EInAdvance;
                }
            else
                {
                delete method;
                User::Leave( KErrArgument );
                }
            delete method;
            method = NULL;
            }
        }
    else
        {
        iSilentRefresh = ENoSilentRefresh;
        delete iSilentParameter;
        iSilentParameter = 0;
        }

    previewOffset = ReadOneTextualHeaderL(
        aMemoryBlock, KPreviewHeader, buf1, r );
    if ( r != KErrNotFound && buf1 )
        {
        CleanupStack::PushL( buf1 );
        SetHeaderWithParameterL( *buf1, method, iPreviewParameter );
        CleanupStack::PopAndDestroy( buf1 );
        buf1 = NULL;
        if ( method )
            {
            if ( !method->CompareF( KPreviewMethodInstant ) )
                {
                iPreview = EInstantPreview;
                }
            else if ( !method->CompareF( KPreviewMethodPreviewRights ) )
                {
                iPreview = EPreviewRights;
                }
            else
                {
                delete method;
                User::Leave( KErrArgument );
                }
            delete method;
            method = NULL;
            }
        }
    else
        {
        iPreview = ENoPreview;
        delete iPreviewParameter;
        iPreviewParameter = NULL;
        }

    iPreviewOverridesSilentRefresh = EFalse;
    if ( iSilentRefresh != ENoSilentRefresh && iPreview != ENoPreview )
        {
        if ( previewOffset < silentOffset )
            {
            iPreviewOverridesSilentRefresh = ETrue;
            }
        }

    ReadOneTextualHeaderL(
        aMemoryBlock, KContentLocationHeader, iContentLocation, r );
    if ( r == KErrNotFound )
        {
        delete iContentLocation;
        iContentLocation = NULL;
        }

    ReadOneTextualHeaderL( aMemoryBlock, KContentURLHeader, iContentUrl, r );
    if ( r == KErrNotFound )
        {
        delete iContentUrl;
        iContentUrl = NULL;
        }

    ReadOneTextualHeaderL(
        aMemoryBlock, KContentVersionHeader, iContentVersion, r );
    if ( r == KErrNotFound )
        {
        delete iContentVersion;
        iContentVersion = NULL;
        }

    // custom headers ->
    ReadOneTextualHeaderL( aMemoryBlock, KInfoURLHeader, iInfoUri, r );
    if ( r == KErrNotFound )
        {
        delete iInfoUri;
        iInfoUri = NULL;
        }

    // custom header
    ReadOneTextualHeaderL( aMemoryBlock, KTitleHeader, iTitle, r );
    if ( r == KErrNotFound )
        {
        delete iTitle;
        iTitle = NULL;
        }

    ReadOneTextualHeaderL( aMemoryBlock, KIconUriHeader, iIconUri, r );
    if ( r == KErrNotFound )
        {
        delete iIconUri;
        iIconUri = NULL;
        }

    ReadOneTextualHeaderL( aMemoryBlock, KAuthorHeader, iAuthor, r );
    if ( r == KErrNotFound )
        {
        delete iAuthor;
        iAuthor = NULL;
        }

    ReadOneTextualHeaderL( aMemoryBlock, KDescriptionHeader, iDescription, r );
    if ( r == KErrNotFound )
        {
        delete iDescription;
        iDescription = NULL;
        }

    ReadOneTextualHeaderL( aMemoryBlock, KCopyrightHeader, iCopyRight, r );
    if ( r == KErrNotFound )
        {
        delete iCopyRight;
        iCopyRight = NULL;
        }


    }

// -----------------------------------------------------------------------------
// COma2Dcf::SetHeaderWithParameterL
// -----------------------------------------------------------------------------
//
void COma2Dcf::SetHeaderWithParameterL(
    const TDesC8& aValue,
    HBufC8*& aMethod,
    HBufC8*& aParameter )
    {
    TInt offset( 0 );

    delete aMethod;
    aMethod = NULL;
    delete aParameter;
    aParameter = NULL;

    offset = aValue.Find( KParamSeparator );
    if ( offset == KErrNotFound || offset == 0 )
        {
        aMethod = aValue.AllocL();
        }
    else
        {
        aMethod = aValue.Left( offset ).AllocLC();
        aParameter = aValue.Mid( offset + 1 ).AllocL();
        CleanupStack::Pop( aMethod );
        }
    }

// -----------------------------------------------------------------------------
// COma2Dcf::ReadOneTextualHeaderL
// -----------------------------------------------------------------------------
//
TInt COma2Dcf::ReadOneTextualHeaderL(
    const TDesC8& aBlock,
    const TDesC8& aName,
    HBufC8*& aBuf,
    TInt& aError )
    {
    TInt offset( aBlock.Find( aName ) );

    delete aBuf;
    aBuf = NULL;
    if ( offset == KErrNotFound )
        {
        aError = KErrNotFound;
        return offset;
        }
    else
        {
        if ( offset + aName.Length() < aBlock.Length() &&
            aBlock.Mid( offset + aName.Length(), 1 ) == KSeparator )
            {
            TInt length( 0 );
            length =
                aBlock.Mid( offset + aName.Length() + 1 ).Find( KTerminator );
            if ( length == KErrNotFound )
                {
                User::Leave( KErrArgument );
                }
            else
                {
                aBuf = HBufC8::NewL( length );
                *aBuf = aBlock.Mid( offset + aName.Length() + 1, length );
                }
            }
        }
    aError = KErrNone;
    return offset;
    }

// -----------------------------------------------------------------------------
// COma2Dcf::ReadExtendedHeadersL
// -----------------------------------------------------------------------------
//
void COma2Dcf::ReadExtendedHeadersL(
    TInt aOffset,
    TInt aEndOfBox )
    {
    TInt size( 1 );
    TInt groupIdSize( 0 );
    TInt groupKeySize( 0 );
    TUint32 type( 0 );
    TInt headerSize( 0 );
    TInt dataSize( 0 );
    TInt offset( 0 );
    TPtr8 ptr( 0, 0 );
    HBufC8* buffer( NULL );

    delete iGroupId;
    iGroupId = NULL;
    delete iGroupKey;
    iGroupKey = NULL;

    while ( size > 0 && aOffset < aEndOfBox && iGroupId == NULL )
        {
        size = 0;
        ReadBoxSizeAndTypeL( aOffset, size, type, headerSize );
        if ( type == KGroupId )
            {
            dataSize = size - headerSize - KLengthVersion - KLengthFlags;
            offset = aOffset + headerSize + KLengthVersion + KLengthFlags;
            // input validation
            if ( dataSize < 0 || dataSize > KMaxTInt / 2 )
                {
                User::Leave( KErrArgument );
                }
            buffer = HBufC8::NewLC( dataSize );
            ptr.Set( buffer->Des() );
            User::LeaveIfError( iFile.Read( offset, ptr, dataSize ) );
            // input validation
            if ( ptr.Length() == 0 )
                {
                User::Leave( KErrArgument );
                }
            iGkEncryptionMethod = static_cast< TEncryptionMethod >( ptr[ 2 ] );
            groupIdSize = ReadUint16FromBlock( ptr, 0 );
            if ( 5 + groupIdSize > ptr.Length() )
                {
                User::Leave( KErrArgument );
                }
            iGroupId = ptr.Mid( 5, groupIdSize ).AllocL();
            groupKeySize = ReadUint16FromBlock( ptr, 3 );
            if ( 5 + groupIdSize + groupKeySize > ptr.Length() )
                {
                User::Leave( KErrArgument );
                }
            iGroupKey = ptr.Mid( 5 + groupIdSize, groupKeySize ).AllocL();

            // Set the content ID of this file to the group ID
            delete iContentID;
            iContentID = NULL;
            iContentID = iGroupId->AllocL();

            CleanupStack::PopAndDestroy( buffer );
            }
        aOffset += size;
        }
    }

// -----------------------------------------------------------------------------
// COma2Dcf::ReadMutableInfoL
// -----------------------------------------------------------------------------
//
void COma2Dcf::ReadMutableInfoL(
    TInt aOffset,
    TInt& aSize )
    {
    TInt size( 0 );
    TInt dataSize( 0 );
    TUint32 type( 0 );
    TInt headerSize( 0 );
    TPtr8 ptr( 0, 0 );
    HBufC8* buffer1( NULL );
    TInt boxEnd( 0 );
    TInt offset( 0 );

    delete iTransactionTracking;
    iTransactionTracking = NULL;
    iRightsObjects.ResetAndDestroy();

    ReadBoxSizeAndTypeL( aOffset, aSize, type, headerSize );
    VerifyTypeL( type, KMutableInfo );

    if ( !iMutablePart )
        {
        iMutablePart = COma2DcfPartInfo::NewL();
        iMutablePart->iType = type;
        iMutablePart->iSize = aSize;
        iMutablePart->iOffset = aOffset;
        }


    boxEnd = aOffset + aSize;
    aOffset += headerSize;

    do
        {
        ReadBoxSizeAndTypeL( aOffset, size, type, headerSize );
        if ( headerSize > 0 )
            {
            dataSize = size - headerSize - KLengthVersion - KLengthFlags;
            offset = aOffset + headerSize + KLengthVersion + KLengthFlags;
            // input validation
            if ( dataSize < 0 || dataSize > KMaxTInt / 2 )
                {
                User::Leave( KErrArgument );
                }
            if ( type == KTransactionTracking && !iTransactionTracking )
                {
                buffer1 = HBufC8::NewLC( dataSize );
                ptr.Set( buffer1->Des() );
                User::LeaveIfError( iFile.Read( offset, ptr, dataSize ) );
                CleanupStack::Pop( buffer1 );
                iTransactionTracking = buffer1;
                }
            else if ( type == KRightsObject )
                {
                buffer1 = HBufC8::NewLC( dataSize );
                ptr.Set( buffer1->Des() );
                User::LeaveIfError( iFile.Read( offset, ptr, dataSize ) );
                CleanupStack::Pop( buffer1 );
                iRightsObjects.Append( buffer1 );
                }
            }
        aOffset += size;

        }
    while ( headerSize > 0 && aOffset < boxEnd && offset <= iLength );
    }

// -----------------------------------------------------------------------------
// COma2Dcf::ReadBoxSizeAndTypeL
// -----------------------------------------------------------------------------
//
void COma2Dcf::ReadBoxSizeAndTypeL(
    TInt aOffset,
    TInt& aSize,
    TUint32& aType,
    TInt& aHeaderSize )
    {
    TBuf8< KLengthBoxSize + KLengthBoxType > buffer;

    aHeaderSize = 0;
    // input validation
    if ( aOffset < 0 || aOffset > iLength )
        {
        // Valid offset can never be negative
        // or greater than length of the opened file.
        User::Leave( KErrArgument );
        }
    User::LeaveIfError( iFile.Read( aOffset, buffer ) );
    if ( buffer.Length() == KLengthBoxSize + KLengthBoxType )
        {
        aSize = ReadUint32FromBlock( buffer, 0 );
        aType = ReadUint32FromBlock( buffer, 4 );
        aHeaderSize = KLengthBoxSize + KLengthBoxType;

        if ( aSize == 1 )
            {
            buffer.SetLength( 0 );
            User::LeaveIfError( iFile.Read( buffer ) );
            if ( buffer.Length() < KLengthBoxSize + KLengthBoxType )
                {
                User::Leave( KErrArgument );
                }
            if ( ReadUint32FromBlock( buffer, 0 ) > 0 )
                {
                User::Leave( KErrOverflow );
                }
            aSize = ReadUint32FromBlock( buffer, 4 );
            aHeaderSize += KLengthBoxSize64;
            }
        else if ( aSize == 0 )
            {
            aSize = iLength - aOffset;
            }
        else if ( aType == KContainer )
            {
            // oma drm container box must always have large length field set
            // See DRM-DCF-CLI-15 in OMA-TS-DRM-DCF-V2_1.
            User::Leave( KErrArgument );
            }
        }
    // Valid box size can never be smaller than discovered header size.
    // Nor can it be larger than largest supported ISO based media box size.
    ValidateBoxSizeL( aSize, aHeaderSize, KMaxBoxSize - aOffset );
    }

// -----------------------------------------------------------------------------
// COma2Dcf::VerifyTypeL
// -----------------------------------------------------------------------------
//
void COma2Dcf::VerifyTypeL( TUint32 aType, TUint32 aRefType )
    {
    if ( aRefType != aType )
        {
        User::Leave( KErrArgument );
        };
    }

// -----------------------------------------------------------------------------
// COma2Dcf::SetTransactionIdL
// -----------------------------------------------------------------------------
//
EXPORT_C void COma2Dcf::SetTransactionIdL(
    const TDesC8& aTransactionId )
    {
    // Overwrite existing ID, otherwise, recreate the whole MDRI box
    if ( iTransactionTracking )
        {
        TInt size( 0 );
        TUint32 type( 0 );
        TInt headerSize( 0 );
        TInt offset( 0 );

        offset = iMutablePart->iOffset;
        // Read mutable box headers and skip them
        ReadBoxSizeAndTypeL( offset, size, type, headerSize );
        offset += headerSize;
        do
            {
            ReadBoxSizeAndTypeL( offset, size, type, headerSize );
            if ( type != KTransactionTracking )
                {
                offset += size;
                }
            }
        while ( headerSize > 0 &&
            type != KTransactionTracking && offset <= iLength );


        if ( type == KTransactionTracking )
            {
            offset += headerSize + KLengthVersion + KLengthFlags;
            User::LeaveIfError( iFile.Write( offset, aTransactionId ) );
            }
        }
    else
        {
        RewriteMutableInfoL( aTransactionId, iRightsObjects );
        }
    }

// -----------------------------------------------------------------------------
// COma2Dcf::SetRightsObjectsL
// -----------------------------------------------------------------------------
//
EXPORT_C void COma2Dcf::SetRightsObjectsL(
    RPointerArray<HBufC8>& aRoList )
    {
    // Always recreate the whole MDRI box, it's simpler
    if ( iTransactionTracking )
        {
        RewriteMutableInfoL( *iTransactionTracking, aRoList );
        }
    else
        {
        RewriteMutableInfoL( KNullDesC8, aRoList );
        }
    }

// -----------------------------------------------------------------------------
// COma2Dcf::RewriteMutableInfoL
// -----------------------------------------------------------------------------
//
void COma2Dcf::RewriteMutableInfoL(
    const TDesC8& aTransactionId,
    RPointerArray<HBufC8>& aRoList )
    {
    TInt offset( 0 );
    TBuf8<4> buffer;
    TInt sizeOfRoBoxes( 0 );
    TInt roBoxSize( 0 );
    TInt tidBoxSize( 0 );
    TInt mutableBoxSize( 0 );

    // Remove the existing box
    if ( iMutablePart )
        {
        User::LeaveIfError( iFile.SetSize( iMutablePart->iOffset ) );
        }

    // Append the box to the end of the file
    User::LeaveIfError( iFile.Size( offset ) );

    for ( TInt i( 0 ); i < aRoList.Count(); i++ )
        {
        sizeOfRoBoxes += aRoList[ i ]->Size() +
            KLengthBoxSize + KLengthBoxType + KLengthVersion + KLengthFlags;
        }
    tidBoxSize = 16 +
        KLengthBoxSize + KLengthBoxType + KLengthVersion + KLengthFlags;
    mutableBoxSize =
        KLengthBoxSize + KLengthBoxType + tidBoxSize + sizeOfRoBoxes;

    // MDRI box
    WriteIntToBlock( mutableBoxSize, buffer, 0 );
    User::LeaveIfError( iFile.Write( offset, buffer ) );
    User::LeaveIfError( iFile.Write( KMdri ) );

    // ODTT box ( always written )
    WriteIntToBlock( tidBoxSize, buffer, 0 );
    User::LeaveIfError( iFile.Write( buffer ) );
    User::LeaveIfError( iFile.Write( KOdtt ) );
    buffer.FillZ();
    User::LeaveIfError( iFile.Write( buffer ) );
    if ( aTransactionId.Size() > 0 )
        {
        User::LeaveIfError( iFile.Write( aTransactionId ) );
        }
    else
        {
        User::LeaveIfError( iFile.Write( KEmptyTransactionId ) );
        }

    // ODRB box
    if ( sizeOfRoBoxes > 0 )
        {
        for ( TInt i( 0 ); i < aRoList.Count(); i++ )
            {
            roBoxSize = aRoList[ i ]->Size() +
                KLengthBoxSize + KLengthBoxType + KLengthVersion + KLengthFlags;
            WriteIntToBlock( roBoxSize, buffer, 0 );
            User::LeaveIfError( iFile.Write( buffer ) );
            User::LeaveIfError( iFile.Write( KOdrb ) );
            buffer.FillZ();
            User::LeaveIfError( iFile.Write( buffer ) );
            User::LeaveIfError( iFile.Write( *aRoList[ i ] ) );
            }
        }

    // Reread the MDRI box
    User::LeaveIfError( iFile.Size( iLength ) );
    ReadMutableInfoL( offset, mutableBoxSize );
    }


// -----------------------------------------------------------------------------
// COma2Dcf::ReadPartInfoL
// -----------------------------------------------------------------------------
//
void COma2Dcf::ReadPartInfoL(
    COma2DcfPartInfo* aPart,
    TInt aOffset )
    {
    TBuf8<4> buffer;
    TUint32 size( 0 );
    TPtr8 ptr( NULL, 0 );

    // ODRM box header
    buffer.SetLength( 0 );

    // input validation
    if ( aOffset < 0 )
        {
        // RFile::Read will panic if offset is negative
        User::Leave( KErrArgument );
        }

    iFile.Read( aOffset, buffer, 4 );
    size = ReadUint32FromBlock( buffer, 0 );
    aOffset += KLengthBoxSize + KLengthBoxType + KLengthVersion + KLengthFlags;
    if ( size == 1 )
        {
        aOffset += KLengthBoxSize64;
        }

    // Discrete headers box header
    buffer.SetLength( 0 );
    iFile.Read( aOffset, buffer, 4 );
    if ( buffer.Length() < 4 )
        {
        User::Leave( KErrArgument );
        }
    size = ReadUint32FromBlock( buffer, 0 );
    aOffset += KLengthBoxSize + KLengthBoxType + KLengthVersion + KLengthFlags;
    if ( size == 1 )
        {
        aOffset += KLengthBoxSize64;
        }

    // Content type
    buffer.SetLength( 0 );
    iFile.Read( aOffset, buffer, 1 );
    if ( buffer.Length() < 1 )
        {
        User::Leave( KErrArgument );
        }
    aPart->iMimeType = HBufC8::NewL( buffer[ 0 ] );
    ptr.Set( aPart->iMimeType->Des() );
    iFile.Read( ptr, buffer[ 0 ] );
    aOffset += 1 + buffer[ 0 ];

    // Common headers box header
    buffer.SetLength( 0 );
    iFile.Read( aOffset, buffer, 4 );
    if ( buffer.Length() < 4 )
        {
        User::Leave( KErrArgument );
        }
    size = ReadUint32FromBlock( buffer, 0 );
    aOffset += KLengthBoxSize + KLengthBoxType + KLengthVersion + KLengthFlags;
    if ( size == 1 )
        {
        aOffset += KLengthBoxSize64;
        }

    // Content ID
    aOffset += KLengthEncMethod + KLengthPadType + KLengthPlainTextSize;
    buffer.SetLength( 0 );
    iFile.Read( aOffset, buffer, 2 );
    if ( buffer.Length() < 2 )
        {
        User::Leave( KErrArgument );
        }
    aPart->iContentId = HBufC8::NewL( ReadUint16FromBlock( buffer, 0 ) );
    ptr.Set( aPart->iContentId->Des() );
    aOffset += KLengthContentIdSize + KLengthRiUrlSize + KLengthTextHeadersSize;
    iFile.Read( aOffset, ptr, ReadUint16FromBlock( buffer, 0 ) );
    }

// -----------------------------------------------------------------------------
// COma2Dcf::ReadUserDataL
// -----------------------------------------------------------------------------
//
void COma2Dcf::ReadUserDataL(
    TInt aOffset,
    TInt& aSize )
    {
    TInt size( 0 );
    TUint32 type( 0 );
    TPtr8 ptr( 0, 0 );

    ReadBoxSizeAndTypeL( aOffset, aSize, type, size );
    VerifyTypeL( type, KUserData );
    aOffset += size;

    delete iUserData;
    iUserData = NULL;
    iUserData = HBufC8::NewL( aSize );
    User::LeaveIfNull( iUserData );

    ptr.Set( iUserData->Des() );
    User::LeaveIfError( iFile.Read( aOffset, ptr, aSize ) );
    aOffset += aSize;

    ParseUserDataSubBoxesL( ptr );
    }

// -----------------------------------------------------------------------------
// COma2Dcf::ParseUserDataSubBoxesL
// -----------------------------------------------------------------------------
//
void COma2Dcf::ParseUserDataSubBoxesL(
    const TDesC8& aMemoryBlock )
    {

    // there MAY be zero to several boxes
    // depending on language definition.
    // here only the first boxes are read out.

    iUserDataLanguageDefined = ETrue;

    // if not found from textual custom headers, try to read out from udta
    if ( !iTitle )
        {
        ReadOneUserDataBoxL( aMemoryBlock, KTitle, iTitle );
        }

    // if not found from textual custom headers, try to read out from udta
    if ( !iDescription )
        {
        ReadOneUserDataBoxL( aMemoryBlock, KDescription, iDescription );
        }

    // if not found from textual custom headers, try to read out from udta
    if ( !iCopyRight )
        {
        ReadOneUserDataBoxL( aMemoryBlock, KCopyright, iCopyRight );
        }

    // if not found from textual custom headers, try to read out from udta
    if ( !iAuthor )
        {
        ReadOneUserDataBoxL( aMemoryBlock, KAuthor, iAuthor );
        }

    ReadOneUserDataBoxL( aMemoryBlock, KPerformer, iPerformer );

    ReadOneUserDataBoxL( aMemoryBlock, KGenre, iGenre );

    ReadOneUserDataBoxL( aMemoryBlock, KRating, iRatingInfo );

    ReadOneUserDataBoxL( aMemoryBlock, KClassification, iClassificationInfo );

    ReadOneUserDataBoxL( aMemoryBlock, KKeyword, iKeyword );

    ReadOneUserDataBoxL( aMemoryBlock, KLocationInformation, iLocInfoName );

    ReadOneUserDataBoxL( aMemoryBlock, KAlbum, iAlbumTitle );


    // These boxes do not contain language information ->

    iUserDataLanguageDefined = EFalse;

    // if not found from textual custom headers, try to read out from udta
    if ( !iIconUri )
        {
        ReadOneUserDataBoxL( aMemoryBlock, KIconUri, iIconUri );
        }

    ReadOneUserDataBoxL( aMemoryBlock, KInfoURL, iInfoUri );

    ReadOneUserDataBoxL( aMemoryBlock, KCoverUri, iCoverUri );

    ReadOneUserDataBoxL( aMemoryBlock, KLyricsURL, iLyricsURL );


    TInt yearOffset( iUserData->Find( KRecordingYear ) );
    if ( yearOffset != KErrNotFound )
        {
        yearOffset += sizeof( tBoxHeaderStr ) - KLengthBoxSize;
        iRecordingYear = ReadUint16FromBlock( iUserData->Des(), yearOffset );
        }
    }

// -----------------------------------------------------------------------------
// COma2Dcf::ReadOneUserDataBoxL
// -----------------------------------------------------------------------------
//
void COma2Dcf::ReadOneUserDataBoxL(
    const TDesC8& aBlock,
    const TDesC8& aName,
    HBufC8*& aBuffer )
    {

    TInt offset( aBlock.Find( aName ) );
    TUint32 length( 0 );
    HBufC8* subBlock( NULL );
    TInt subOffset( 0 );
    TBuf8<12> buffer;

    delete aBuffer;
    aBuffer = NULL;

    if ( offset == KErrNotFound )
        {
        return;
        }
    else
        {

        TUint8* pData( const_cast< TUint8* >( aBlock.Ptr() ) );
        offset -= KLengthBoxSize;
        pData += offset;

        buffer.SetLength( 0 );
        buffer.Copy( pData, 12 );

        tBoxHeaderStr* SubBox = ( tBoxHeaderStr* ) buffer.Ptr();
        length = SWAP32( SubBox->size );

        subBlock = aBlock.Mid( offset, length ).AllocLC();

        subOffset += sizeof( tBoxHeaderStr );

        if ( aName == KClassification )
            {
            TUint32 classificationEntity(
                ReadUint32FromBlock( subBlock->Des(), subOffset ) );
            subOffset += sizeof( classificationEntity );

            TUint16 classificationTable(
                ReadUint16FromBlock( subBlock->Des(), subOffset ) );
            subOffset += sizeof( classificationTable );
            }

        if ( aName == KRating )
            {
            TUint32 ratingEntity(
                ReadUint32FromBlock( subBlock->Des(), subOffset ) );
            subOffset += sizeof( ratingEntity );

            TUint32 ratingCriteria(
                ReadUint32FromBlock( subBlock->Des(), subOffset ) );
            subOffset += sizeof( ratingCriteria );
            }

        if ( iUserDataLanguageDefined )
            {
            TUint16 PadAndLanguage(
                ReadUint16FromBlock( subBlock->Des(), subOffset ) );
            subOffset += sizeof( PadAndLanguage );
            }


        if ( aName == KKeyword )
            {
            // we return all keywords here
            aBuffer = HBufC8::NewL( length );
            TUint8 keywordCnt( subBlock->Des()[ subOffset ] );
            TUint8 keywordSize( 0 );

            subOffset += sizeof( keywordCnt );

            for ( TInt count( 0 ); count < keywordCnt; count++ )
                {
                keywordSize = subBlock->Des()[ subOffset ];
                subOffset += sizeof( keywordSize );
                aBuffer->Des().Append(
                    subBlock->Des().Mid( subOffset, keywordSize ) );
                subOffset += keywordSize;
                keywordSize = 0;
                }
            }

        else if ( aName == KAlbum )
            {
            TInt tracknbrFound(
                subBlock->Des().Mid( subOffset ).Find( KTerminator ) );

            if ( ( tracknbrFound == KErrNotFound ) ||
                ( tracknbrFound ==
                  ( subBlock->Des().Length()-subOffset ) - 1 ) )
                {
                aBuffer = subBlock->Des().Mid( subOffset ).AllocL();
                iAlbumTrack = NULL;
                }
            else
                {
                aBuffer =
                    subBlock->Des().Mid( subOffset, tracknbrFound ).AllocL();
                subOffset += tracknbrFound + KLengthTerminator;
                iAlbumTrack = subBlock->Des()[ subOffset ];
                }
            }

        else if ( aName == KLocationInformation )
            {
            TInt found( subBlock->Des().Mid( subOffset ).Find( KTerminator ) );
            aBuffer = subBlock->Des().Mid( subOffset, found ).AllocL();
            subOffset += found + KLengthTerminator;

            TUint8 role( subBlock->Des()[ subOffset ] );
            subOffset += sizeof( role );
            TUint32 longitude(
                ReadUint32FromBlock( subBlock->Des(), subOffset ) );
            subOffset += sizeof( longitude );
            TUint32 latitude(
                ReadUint32FromBlock( subBlock->Des(), subOffset ) );
            subOffset += sizeof( latitude );
            TUint32 altitude(
                ReadUint32FromBlock( subBlock->Des(), subOffset ) );
            subOffset += sizeof( altitude );

            found = subBlock->Des().Mid( subOffset ).Find( KTerminator );

            // iLocInfoAstronomicalBody =
            //    subBlock->Des().Mid( subOffset, found ).AllocL();
            subOffset += found + KLengthTerminator;

            found = subBlock->Des().Mid( subOffset ).Find( KTerminator );
            // iLocInfoAdditionalNotes =
            //    subBlock->Des().Mid( subOffset, found ).AllocL();
            }
        else
            {
            aBuffer = subBlock->Des().Mid( subOffset ).AllocL();
            }
        CleanupStack::PopAndDestroy( subBlock );
        }
    }


//  End of File
