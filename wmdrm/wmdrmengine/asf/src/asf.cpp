/*
* Copyright (c) 2006 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  implementation of asf file handler class
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <f32file.h>
#include <utf.h>
#include <caf/caf.h>
#include "asf.h"

#define _LOGGING_FILE L"asf.txt"

#include "logfn.h"

// LOCAL CONSTANTS AND MACROS

#define M4CC(a) ((a[0] << 24) + (a[1] << 16) + (a[2] << 8) + a[3])

_LIT8 (KASFHeaderObject, "75B22630668E11CFA6D900AA0062CE6C");
_LIT8 (KASFFilePropertiesObject, "8CABDCA1A94711CF8EE400C00C205365");
_LIT8 (KASFContentDescriptionObject, "75B22633668E11CFA6D900AA0062CE6C");
_LIT8 (KASFExtendedContentDescriptionObject, "D2D0A440E30711D297F000A0C95EA850");
_LIT8 (KASFContentEncryptionObject, "2211B3FBBD2311D2B4B700A0C955FC6E");
_LIT8 (KASFExtendedContentEncryptionObject, "298AE61426224C17B935DAE07EE9289C");
_LIT8 (KASFDigitalSignatureObject, "2211B3FCBD2311D2B4B700A0C955FC6E");

_LIT(KWMAlbumTitle,           "WM/AlbumTitle\0");
_LIT(KWMPicture,              "WM/Picture\0");
_LIT(KWMText,                 "WM/Text\0");
_LIT(KWMComposer,             "WM/Composer\0");
_LIT(KWMGenre,                "WM/Genre\0");
_LIT(KWMYear,                 "WM/Year\0");
_LIT(KWMYear1,                "WM/OriginalReleaseYear\0");
_LIT(KWMOriginalArtist,       "WM/OriginalArtist\0");
_LIT(KWMTrackNumber,          "WM/TrackNumber\0");
_LIT(KWMUniqueFileIdentifier, "WM/UniqueFileIdentifier\0");
_LIT(KWMAudioFileURL,         "WM/AudioFileURL\0");
_LIT(KWMSharedUserRating,     "WM/SharedUserRating\0");
_LIT(KWMDate,                 "WM/OriginalReleaseTime\0");

_LIT8(KWrmHeader, "W\0R\0M\0H\0E\0A\0D\0E\0R\0");

_LIT(KAsfMimeType, "application/vnd.drm.asf");
_LIT(KWmaMimeType, "audio/x-ms-wma");
_LIT(KWmvMimeType, "video/x-ms-wmv");

_LIT(KAsfExtension, ".asf");
_LIT(KWmaExtension, ".wma");
_LIT(KWmvExtension, ".wmv");

#define SET_PTR16(ptr, descriptor, offset, length) \
    ptr.Set( reinterpret_cast<const TUint16*>( (descriptor).Ptr() + (offset) ), length );

/*
* 16 B Obj GUID
* 8  B Obj Size
* xx B Obj Data
*/
const TInt KObjectID( 16 );
const TInt KObjectSize( 8 );

// ============================= LOCAL FUNCTIONS ===============================

LOCAL_C TUint32 ReadUint64FromBlockL( const TDesC8& aBlock, TInt aOffset );
LOCAL_C TUint32 ReadUint32FromBlockL( const TDesC8& aBlock, TInt aOffset );
LOCAL_C TUint16 ReadUint16FromBlockL( const TDesC8& aBlock, TInt aOffset );

LOCAL_C HBufC16* HBuf16FromBlockL(
    const TDesC8& aBlock,
    TInt aOffset,
    TInt aLength );

LOCAL_C HBufC16* HBuf16IgnoreNullL(
    const TDesC8& aBlock,
    TInt aOffset,
    TInt aLength );

LOCAL_C TUint32 ReadUint64FromBlockL( const TDesC8& aBlock, TInt aOffset )
    {
    if ( aBlock.Length() <= ( aOffset + 3 ) )
        {
        User::Leave( KErrArgument );
        }
    return ( aBlock[aOffset + 3] << 24 ) +
           ( aBlock[aOffset + 2] << 16 ) +
           ( aBlock[aOffset + 1] << 8 ) +
             aBlock[aOffset];
    }

LOCAL_C TUint32 ReadUint32FromBlockL( const TDesC8& aBlock, TInt aOffset )
    {
    if ( aBlock.Length() <= ( aOffset + 3 ) )
        {
        User::Leave( KErrArgument );
        }
    return ( aBlock[aOffset + 3] << 24 ) +
           ( aBlock[aOffset + 2] << 16 ) +
           ( aBlock[aOffset + 1] << 8 ) +
             aBlock[aOffset];
    }

LOCAL_C TUint16 ReadUint16FromBlockL( const TDesC8& aBlock, TInt aOffset )
    {
    if ( aBlock.Length() <= ( aOffset + 1 ) )
        {
        User::Leave( KErrArgument );
        }
    return ( aBlock[aOffset + 1] << 8 ) + 
             aBlock[aOffset];
    }

LOCAL_C HBufC16* HBuf16FromBlockL(
    const TDesC8& aBlock,
    TInt aOffset,
    TInt aLength )
    {
    if ( aBlock.Length() < ( aOffset + aLength ) )
        {
        User::Leave( KErrArgument );
        }
    HBufC16* buffer( HBufC16::NewL( aLength / 2 + 1 ) );
    TPtr ptr( buffer->Des() );
    
    for ( TInt i( 0 ) ; i < aLength; i+=2 )
        {
        ptr.Append( aBlock[aOffset + i] );
	    }
	
	return buffer;
    }

LOCAL_C HBufC16* HBuf16IgnoreNullL(
    const TDesC8& aBlock,
    TInt aOffset,
    TInt aLength )
    {
    if ( aBlock.Length() < ( aOffset + aLength ) )
        {
        User::Leave( KErrArgument );
        }
    HBufC16* buffer( HBufC16::NewL( aLength / 2 + 1 ) );
    TPtr ptr( buffer->Des() );
    
    for ( TInt i( 0 ) ; i < aLength; i+=2 )
        {
        ptr.Append( aBlock[aOffset + i] );
        }
    
    TInt dLength = ptr.Length();
    
    if ( dLength != 0 && ptr[dLength - 1] == '\0' )
        {
        ptr.SetLength( dLength - 1 );
        }
    
    return buffer;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAsf::CAsf
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAsf::CAsf():
    iTitle( NULL ),
    iAuthor( NULL ),
    iCopyright( NULL ),
    iDescription( NULL ),
    iRating( NULL ),
    iAlbumTitle( NULL ),
    iPicture( NULL ),
    iText( NULL ),
    iComposer( NULL ),
    iGenre( NULL ),
    iOriginalArtist( NULL ),
    iTrackNumber( NULL ),
    iUniqueFileID( NULL ),
    iAudioFileUrl( NULL ),
    iSharedUserRating( NULL ),
    iDate( NULL ),
    iYear( NULL ),
    iIsDrmProtected( EFalse ),
    iIsValidated( EFalse ),
    iHeaderData( NULL ),
    iSecretData( NULL ),
    iProtectionType( NULL ),
    iKeyId( NULL ),
    iLicenseUrl( NULL ),
    iExtendedContentEncryptionObject( NULL ),
    iContentDescriptionObjectExists( EFalse ),
    iFilePropertiesObjectExists( EFalse ),
    iExtendedContentDescriptionObjectExists( EFalse ),
    iExtendedContentEncryptionObjectExists( EFalse ),
    iContentEncryptionObjectExists( EFalse ),
    iMimeType( KAsfMimeType )
    {
    LOGFN( "CAsf::CAsf" );
    }

// -----------------------------------------------------------------------------
// CAsf::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAsf::ConstructL( const TDesC& aFileName )
    {
    TInt r = KErrNone;

    LOGFN( "CAsf::ConstructL" );
    LOG( aFileName );
    User::LeaveIfError( iFs.Connect() );
    r = iFile.Open( iFs, aFileName, EFileStream | EFileRead | EFileShareReadersOrWriters );
    if( r == KErrInUse )
        {
        r = iFile.Open( iFs, aFileName, EFileStream | EFileRead | EFileShareAny);
        if ( r == KErrInUse )
            {
            r = iFile.Open( iFs, aFileName, EFileStream | EFileRead |
                EFileShareReadersOnly);
            }
        }
    User::LeaveIfError( r );
    InitializeL();
    }

// -----------------------------------------------------------------------------
// CAsf::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAsf::ConstructL( const RFile& aFile )
    {
    LOGFN( "CAsf::ConstructL (2)" );
    iFile.Duplicate( aFile );
    iFile.Size( iLength );
    InitializeL();
    }

// -----------------------------------------------------------------------------
// CAsf::InitializeL
//
// -----------------------------------------------------------------------------
//
void CAsf::InitializeL()
    {
    TFileName name;

    LOGFN( "CAsf::InitializeL" );
    iFile.FullName( name );
    if ( name.Right( 4 ).CompareF( KWmaExtension ) == 0 )
        {
        iMimeType.Set( KWmaMimeType );
        }
    else if ( name.Right( 4 ).CompareF( KWmvExtension ) == 0 ||
        name.Right( 4 ).CompareF( KAsfExtension ) == 0 )
        {
        iMimeType.Set( KWmvMimeType );
        }

    ValidateL();
    }

// -----------------------------------------------------------------------------
// CAsf::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAsf* CAsf::NewL( const RFile& aFile )
    {
    LOGFN( "CAsf::NewL" );
    CAsf* self = new (ELeave) CAsf;
    CleanupStack::PushL( self );
    self->ConstructL( aFile );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CAsf::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAsf* CAsf::NewL( const TDesC& aFileName )
    {
    LOGFN( "CAsf::NewL (2)" );
    CAsf* self = new (ELeave) CAsf;
    CleanupStack::PushL( self );
    self->ConstructL( aFileName );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CAsf::~CAsf
// Destructor
// -----------------------------------------------------------------------------
//

EXPORT_C CAsf::~CAsf()
    {
    LOGFN( "CAsf::~CAsf" );
    if ( iFile.SubSessionHandle() != KNullHandle )
        {
        iFile.Close();
        }
    if ( iFs.Handle() != KNullHandle )
        {
        iFs.Close();
        }

    delete iHeaderData;
    delete iSecretData;
    delete iProtectionType;
    delete iKeyId;
    delete iLicenseUrl;
    delete iExtendedContentEncryptionObject;
    delete iDigitalSignatureObject;
    delete iSignedData;

    // Content description Object
    delete iTitle;
    delete iAuthor;
    delete iCopyright;
    delete iDescription;
    delete iRating;

    // Extended Content description Object
    delete iAlbumTitle;
    delete iPicture;
    delete iText;
    delete iComposer;
    delete iGenre;
    delete iYear;
    delete iOriginalArtist;
    delete iTrackNumber;
    delete iUniqueFileID;
    delete iAudioFileUrl;
    delete iSharedUserRating;
    delete iDate;
    }

// -----------------------------------------------------------------------------
// CAsf::
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CAsf::IsProtected( const TDesC8& aAsfHeader )
    {
    LOGFN( "CAsf::IsProtected" );
    if ( aAsfHeader.Find( KWrmHeader ) == KErrNotFound )
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }


// -----------------------------------------------------------------------------
// CAsf::ValidateL
//
// -----------------------------------------------------------------------------
//
void CAsf::ValidateL()
    {
    // ASF_Header_Object GUID 128 bits.
    TBuf8<32> header;

    LOGFN( "CAsf::ValidateL" );
    iFile.Read( 0, header, KObjectID );
    if ( header.Length() < KObjectID )
        {
        User::Leave( KErrOverflow );
        }
    FormatGUID( header );
    if ( header !=  KASFHeaderObject )
        {
        User::Leave( KErrArgument );
        }
    
    // read header object size.
    iFile.Read( header, KObjectSize );
    iHeaderSize = ReadUint64FromBlockL( header, 0 );
    if ( iHeaderSize <= 30 || iHeaderSize > KMaxTInt / 2 - 1 )
        {
        User::Leave( KErrOverflow );
        }

    // read header object
    // 2~31 = 2 GB, size of header would not be greater than this,
    // also, HBufC does not have a NewL with TInt64 as arguement.
    iHeaderData = HBufC8::NewL( iHeaderSize );
    TPtr8 headerPtr = iHeaderData->Des();
    iFile.Read( headerPtr, iHeaderSize - ( KObjectID + KObjectSize ) );

    iNbrOfObjects = ReadUint32FromBlockL( *iHeaderData, 0 );
    if ( iNbrOfObjects <= 0 )
        {
        User::Leave( KErrArgument );
        }

    TInt objOffset( 6 );
    if ( iHeaderData->Length() < ( objOffset + KObjectID ) )
        {
        User::Leave( KErrArgument );
        }
    //Read next object GUID
    TBuf8<32> objGUID = iHeaderData->Mid( objOffset, KObjectID );
    FormatGUID( objGUID );
    TBool loop( ETrue );

    //Loop until all needed headers are handled or top level header is finished
    while ( loop )
        {
        //Read current object size
        TUint32 objSize( ReadUint64FromBlockL( *iHeaderData, objOffset + KObjectID ) );
        if ( objSize < 24 )
            {
            User::Leave( KErrArgument );
            }
        
        if ( !iContentDescriptionObjectExists && objGUID == 
             KASFContentDescriptionObject )
            {
            iContentDescriptionObjectExists = ETrue;
            iContentDescriptionOffset = objOffset;
            ParseContentDescriptionObjectL();
            }
        else if ( !iFilePropertiesObjectExists && objGUID == 
             KASFFilePropertiesObject )
            {
            iFilePropertiesObjectExists = ETrue; // must exist
            iFilePropertiesOffset = objOffset;
            iFilePropertiesEndOffset = iFilePropertiesOffset + objSize;
            }
        else if ( !iExtendedContentDescriptionObjectExists && objGUID ==
             KASFExtendedContentDescriptionObject )
            {
            iExtendedContentDescriptionObjectExists = ETrue;
            iExtendedContentDescriptionOffset = objOffset;
            ParseExtendedContentDescriptionObjectL();
            }
        else if ( !iExtendedContentEncryptionObjectExists && objGUID ==
             KASFExtendedContentEncryptionObject )
            {
            iExtendedContentEncryptionObjectExists = ETrue;
            iExtendedContentEncryptionOffset = objOffset;
            iIsDrmProtected = ETrue;
            TInt eCEODataOffset( objOffset + KObjectID + KObjectSize + 4 );
            TInt eCEODataLength( objSize - ( KObjectID + KObjectSize + 4 ) );
            if ( iHeaderData->Length() < eCEODataOffset + eCEODataLength ||
                 eCEODataLength < 0)
                {
                User::Leave( KErrArgument );
                }
            iExtendedContentEncryptionObject = iHeaderData->Mid( eCEODataOffset,
                                                                 eCEODataLength ).AllocL();
            }
        else if ( !iContentEncryptionObjectExists && objGUID ==
             KASFContentEncryptionObject )
            {
            iContentEncryptionObjectExists = ETrue;
            iContentEncryptionOffset = objOffset;
            iIsDrmProtected = ETrue;
            ParseContentEncryptionObjectL();
            }
        else if ( !iDigitalSignatureObjectExists && objGUID ==
             KASFDigitalSignatureObject )
            {
            iDigitalSignatureObjectExists = ETrue;
            iDigitalSignatureOffset = objOffset;
            
            TInt dSODataOffset( objOffset + KObjectID + KObjectSize + 8 );
            TInt dSODataLength( objSize - ( KObjectID + KObjectSize + 8 ) );
            if ( iHeaderData->Length() < dSODataOffset + dSODataLength ||
                 dSODataLength < 0 )
                {
                User::Leave( KErrArgument );
                }
            iDigitalSignatureObject = iHeaderData->Mid( dSODataOffset,
                                                        dSODataLength ).AllocL();
            
            if ( iHeaderData->Length() < iFilePropertiesEndOffset + ( iDigitalSignatureOffset - iFilePropertiesEndOffset ) ||
                 iDigitalSignatureOffset - iFilePropertiesEndOffset < 0 ||
                 iFilePropertiesEndOffset < 0 )
                {
                iDigitalSignatureObjectExists = EFalse;
                iDigitalSignatureOffset = 0;
                delete iDigitalSignatureObject;
                iDigitalSignatureObject = NULL;
                }
            else
                {
                iSignedData = 
                    iHeaderData->Mid( iFilePropertiesEndOffset,
                                      iDigitalSignatureOffset - iFilePropertiesEndOffset ).AllocL();
                }
            }
        
        //Move object offset to the end of the current header object  
        objOffset += objSize;
        //End loop, if top level header is finished or all needed headers are handled
        if ( objOffset >= iHeaderSize - 30 ||
            ( iContentDescriptionObjectExists &&
              iFilePropertiesObjectExists &&
              iExtendedContentDescriptionObjectExists &&
              iExtendedContentEncryptionObjectExists &&
              iDigitalSignatureObjectExists ) )
            {
            loop = EFalse;
            }
        //Loop isn't finished, read next object GUID
        else
            {
            if ( iHeaderData->Length() < ( objOffset + KObjectID ) || objOffset < 0 )
                {
                User::Leave( KErrArgument );
                }
            objGUID = iHeaderData->Mid( objOffset, KObjectID );
            FormatGUID( objGUID );
            }
        }
    if ( iFilePropertiesObjectExists )
        {
        iIsValidated = ETrue;
        }
    }


// -----------------------------------------------------------------------------
// CAsf::FormatGUID
// -----------------------------------------------------------------------------
//
void CAsf::FormatGUID( TDes8 &aGUID )
    {
    LOGFN( "CAsf::FormatGUID" );

    TBuf8<16> copyGUID( aGUID );
    TInt i;
    for( i = 0; i < 4; i++ )
        {
        copyGUID[i] = aGUID[3-i];
        }
    for( i = 4; i < 6; i++ )
        {
        copyGUID[i] = aGUID[9 - i];
        }
    for ( i = 6; i < 8; i++ )
        {
        copyGUID[i] = aGUID[13 - i];
        }
    for( i = 8; i < 16 ; i++ )
        {
        copyGUID[i] = aGUID[i];
        }
    aGUID.Delete( 0, 32 );
    for( i = 0; i < 16; i++ )
        {
        aGUID.AppendNumFixedWidthUC( copyGUID[i], EHex, 2 );
        }
    }

// -----------------------------------------------------------------------------
// CAsf::ParseContentDescriptionObject
// -----------------------------------------------------------------------------
//
void CAsf::ParseContentDescriptionObjectL()
    {
    LOGFN( "CAsf::ParseContentDescriptionObjectL" );
    TInt offset( iContentDescriptionOffset + KObjectID );
    TUint32 objSize( ReadUint64FromBlockL( *iHeaderData, offset ) );

    if ( iHeaderData->Length() < iContentDescriptionOffset + objSize )
        {
        User::Leave( KErrOverflow );
        }

    offset += KObjectSize;
    iTitleLength = ReadUint16FromBlockL( *iHeaderData, offset );
    offset += 2;
    iAuthorLength = ReadUint16FromBlockL( *iHeaderData, offset );
    offset += 2;
    iCopyrightLength = ReadUint16FromBlockL( *iHeaderData, offset );
    offset += 2;
    iDescriptionLength = ReadUint16FromBlockL( *iHeaderData, offset );
    offset += 2;
    iRatingLength = ReadUint16FromBlockL( *iHeaderData, offset );
    offset += 2;

    TInt length( iTitleLength + iAuthorLength + iCopyrightLength + iDescriptionLength + iRatingLength );
    if ( length > objSize - ( KObjectID + KObjectSize + 10 ) )
        {
        User::Leave( KErrOverflow );
        }

    iTitle = HBuf16IgnoreNullL( *iHeaderData, offset, iTitleLength );
    offset += iTitleLength;
    
    iAuthor = HBuf16IgnoreNullL( *iHeaderData, offset, iAuthorLength );
    offset += iAuthorLength;
    
    iCopyright = HBuf16IgnoreNullL( *iHeaderData, offset, iCopyrightLength );
    offset += iCopyrightLength;
    
    iDescription = HBuf16IgnoreNullL( *iHeaderData, offset, iDescriptionLength );
    offset += iDescriptionLength;
    
    iRating = HBuf16IgnoreNullL( *iHeaderData, offset, iRatingLength );
    offset += iRatingLength;
    }

// -----------------------------------------------------------------------------
// CAsf::ParseContentEncryptionObject
// -----------------------------------------------------------------------------
//
void CAsf::ParseContentEncryptionObjectL()
    {
    LOGFN( "CAsf::ParseContentEncryptionObject" );
    TInt offset( iContentEncryptionOffset + KObjectID );
    TUint32 objSize( ReadUint64FromBlockL( *iHeaderData, offset ) );

    if ( iHeaderData->Length() < iContentEncryptionOffset + objSize ) 
        {
        User::Leave( KErrOverflow );
        }

    offset += KObjectSize;
    TInt len( ReadUint32FromBlockL( *iHeaderData, offset ) );
    offset += 4;
    if ( iHeaderData->Length() < ( offset + len ) )
        {
        User::Leave( KErrArgument );
        }
    iSecretData = iHeaderData->Mid( offset, len ).AllocL();
    offset += len;

    len = ReadUint32FromBlockL( *iHeaderData, offset );
    offset += 4;
    if ( iHeaderData->Length() < ( offset + len ) )
        {
        User::Leave( KErrArgument );
        }
    iProtectionType = iHeaderData->Mid( offset, len ).AllocL();
    offset += len;

    len = ReadUint32FromBlockL( *iHeaderData, offset );
    offset += 4;
    if ( iHeaderData->Length() < ( offset + len ) )
        {
        User::Leave( KErrArgument );
        }
    iKeyId = iHeaderData->Mid( offset, len ).AllocL();
    offset += len;

    len = ReadUint32FromBlockL( *iHeaderData, offset );
    offset += 4;
    if ( iHeaderData->Length() < ( offset + len ) )
        {
        User::Leave( KErrArgument );
        }
    iLicenseUrl = iHeaderData->Mid( offset, len ).AllocL();
    }

// -----------------------------------------------------------------------------
// CAsf::ParseExtendedContentDescriptionObjectL
// -----------------------------------------------------------------------------
//
void CAsf::ParseExtendedContentDescriptionObjectL()
    {
    TInt i;
    
    LOGFN( "CAsf::ParseExtendedContentDescriptionObjectL" );
    TInt offset( iExtendedContentDescriptionOffset + KObjectID );
    TUint32 objSize( ReadUint64FromBlockL( *iHeaderData, offset ) );

    if ( iHeaderData->Length() < iExtendedContentDescriptionOffset + objSize )
        {
        User::Leave( KErrOverflow );
        }

    offset += KObjectSize;
    iExtendedContentDescriptionCount = ReadUint16FromBlockL( *iHeaderData, offset );
    offset += 2;

    for ( i = 0 ; i < iExtendedContentDescriptionCount; i++ )
        {
        TInt nameLength( ReadUint16FromBlockL( *iHeaderData, offset ) );
        offset += 2;
        HBufC16* name16( HBuf16FromBlockL( *iHeaderData, offset, nameLength ) );
        offset += nameLength;
        CleanupStack::PushL( name16 );
        if( !name16->CompareF( KWMAlbumTitle ) )
            {
            iAlbumTitle = ReadExtendedContentObjectL( offset );
            }
        else if( !name16->CompareF( KWMPicture ) )
            {
            iPicture = ReadExtendedContentObjectL( offset );
            }
        else if( !name16->CompareF( KWMText) )
            {
            iText = ReadExtendedContentObjectL( offset );
            }
        else if( !name16->CompareF( KWMComposer ) )
            {
            iComposer = ReadExtendedContentObjectL( offset );
            }
        else if( !name16->CompareF( KWMGenre ) )
            {
            iGenre = ReadExtendedContentObjectL( offset );
            }
        else if( !name16->CompareF( KWMYear ) || !name16->CompareF( KWMYear1 ) )
            {
            if ( !iYear )
                {
                iYear = ReadExtendedContentObjectL( offset );
                }
            }
        else if( !name16->CompareF( KWMOriginalArtist) )
            {
            iOriginalArtist = ReadExtendedContentObjectL( offset );
            }
        else if( !name16->CompareF( KWMTrackNumber ) )
            {
            iTrackNumber = ReadExtendedContentObjectL( offset );
            }
        else if( !name16->CompareF( KWMUniqueFileIdentifier ) )
            {
            iUniqueFileID = ReadExtendedContentObjectL( offset );
            }
        else if( !name16->CompareF( KWMAudioFileURL ) )
            {
            iAudioFileUrl = ReadExtendedContentObjectL( offset );
            }
        else if( !name16->CompareF( KWMSharedUserRating ) )
            {
            iSharedUserRating = ReadExtendedContentObjectL( offset );
            }
        else if( !name16->CompareF( KWMDate ) )
            {
            iDate = ReadExtendedContentObjectL( offset );
            }
        CleanupStack::PopAndDestroy( name16 );

        offset += 2; // data type
        TInt valueLength( ReadUint16FromBlockL( *iHeaderData, offset ) );
        offset += 2;
        offset += valueLength;
        }
    }

// -----------------------------------------------------------------------------
// CAsf::ReadExtendedContentObjectL
// -----------------------------------------------------------------------------
//
HBufC16* CAsf::ReadExtendedContentObjectL( TInt aOffset )
    {
    HBufC16* buffer( NULL );

    LOGFN( "CAsf::ReadExtendedContentObjectL" );
    TInt dataTypeInt( ReadUint16FromBlockL( *iHeaderData, aOffset ) );
    if ( dataTypeInt == 0x00 )
        {
        TInt length( ReadUint16FromBlockL( *iHeaderData, aOffset + 2 ) );
        if ( length > 0 )
            {
            buffer = HBuf16FromBlockL( *iHeaderData, aOffset + 4, length - 2 );
            }
        }
    else if ( dataTypeInt == 0x02 || dataTypeInt == 0x03 )
        {
        TUint32 dword( ReadUint32FromBlockL( *iHeaderData, aOffset + 4 ) );
        buffer = HBufC::NewL( 16 );
        TPtr dataString( buffer ->Des() );
        dataString.Num( dword );
        }
    else if ( dataTypeInt == 0x05 )
        {
        TUint16 word( ReadUint16FromBlockL( *iHeaderData, aOffset + 4 ) );
        buffer = HBufC::NewL( 8 );
        TPtr dataString( buffer->Des() );
        dataString.Num( word ) ;
        }
    return buffer;
    }

//  End of File
