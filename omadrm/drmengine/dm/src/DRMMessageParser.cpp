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
* Description:  This class implements the BbB-functionality.
*
*/


// INCLUDE FILES

#include <e32base.h>
#include <s32file.h>
#include <s32mem.h>
#include <caf/caftypes.h>
#include <wspdecoder.h>
#include <wspencoder.h>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include "DRMRights.h"
#include "Oma1DcfCreator.h"
#include "b64.h"
#include "DRMMessageParser.h"
#include "DrmRightsParser.h"
#include "DRMRightsClient.h"
#include "DrmKeyStorage.h"


// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
LOCAL_C const TInt KDefaultInputBufferSize = 2048;
LOCAL_C const TInt KDRMMessageMalformed = KErrGeneral;

_LIT8( KCIDString, "cid:" );
LOCAL_C const TInt KCIDStringLength = 4;

_LIT8( KFLPrefix, "flk:");
LOCAL_C const TInt KFLKPrefixLength = 4;

_LIT8( KFLSuffix, "@localhost");
LOCAL_C const TUint8 KCDContentIDLength = 25; // 4 + 11 + 10
LOCAL_C const TUint8 KCDPlainIDLength = 11;

LOCAL_C const TInt KInputBufferSize = 2048;
LOCAL_C const TInt KBoundaryMarkLength = 2;

LOCAL_C const TUint KInitialDCFBufferSize = 4096;

#ifdef RD_MULTIPLE_DRIVE
_LIT( KTempPath, "%c:\\system\\temp\\" );
#else
_LIT( KTempPath, "c:\\system\\temp\\" );
#endif

_LIT8( KColon, ":" );
_LIT8( KSemiColon, ";" );
_LIT8( KNewLine, "\n" );
_LIT8( KEndLine, "\r\n" );
LOCAL_C const TUint8 KEndLineLength = 2;

_LIT8( KBoundaryMark, "--" );
_LIT8( KContentType, "Content-Type" );
_LIT8( KContentTransferEncoding, "Content-Transfer-Encoding" );
_LIT8( KEncodingBase64, "base64" );
_LIT8( KEncoding7bit, "7bit" );
_LIT8( KEncoding8bit, "8bit" );
_LIT8( KEncodingBinary, "binary" );
_LIT8( KDRMXMLRightsType, "application/vnd.oma.drm.rights+xml");
_LIT8( KDRMWBXMLRightsType, "application/vnd.oma.drm.rights+wbxml" );
_LIT8( KDRMContentType, "application/vnd.oma.drm.content" );
_LIT8( KRightsIssuer, "Rights-Issuer" );

_LIT8( KRightsStartTag, "<o-ex:rights");

_LIT8( KROPart1, "Content-Type: application/vnd.oma.drm.rights+xml\r\n\
Content-Transfer-Encoding: binary\r\n\r\n\
<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" \
xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" \
xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\">\
<o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context>\
<o-ex:agreement><o-ex:asset><o-ex:context>\
<o-dd:uid>");

_LIT8( KROPart2, "</o-dd:uid></o-ex:context></o-ex:asset><o-ex:permission>\
<o-dd:display/><o-dd:play/><o-dd:execute/><o-dd:print/>\
</o-ex:permission></o-ex:agreement></o-ex:rights>");

const TInt KFLROSize = sizeof(KROPart1) + sizeof(KROPart2);

// MODULE DATA STRUCTURES
struct TDeleteFileData
    {
    RFs aFs;
    TFileName aName;
    };

// LOCAL FUNCTION PROTOTYPES
LOCAL_C void DoResetAndDestroy( TAny* aPtr );
LOCAL_C void DoResetAndDestroy2( TAny* aPtr );
LOCAL_C void DoDeleteFile( TAny* aPtr );
LOCAL_C void ConvertPermissionL( CDRMRights*& aRights,
                                 CDRMPermission& aPermission,
                                 const TDesC8& aURI );
// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// DoResetAndDestroy
// Does ResetAndDestroy() to given RPointerArray< CDRMRights >
// -----------------------------------------------------------------------------
void DoResetAndDestroy( TAny* aPtr )
    {
    __ASSERT_DEBUG( aPtr, User::Invariant() );

    reinterpret_cast< RPointerArray< CDRMRights >* >( aPtr )->ResetAndDestroy();
    }

void DoResetAndDestroy2( TAny* aPtr )
    {
    __ASSERT_DEBUG( aPtr, User::Invariant() );

    reinterpret_cast< RPointerArray< CDRMPermission >* >( aPtr )->ResetAndDestroy();
    }
// -----------------------------------------------------------------------------
// DoDeleteFile
// Delete the file presented by TDeleteFileData pointer.
// -----------------------------------------------------------------------------
void DoDeleteFile( TAny* aPtr )
    {
    __ASSERT_DEBUG( aPtr, User::Invariant() );
    TDeleteFileData* data = reinterpret_cast< TDeleteFileData* >( aPtr );
    data->aFs.Delete( data->aName );
    }

void ConvertPermissionL( CDRMRights*& aRights,
                         CDRMPermission& aPermission,
                         const TDesC8& aURI )
    {
    CDRMAsset* asset( NULL );
    CDRMRights* rights( NULL );

    aRights = NULL;

    rights = CDRMRights::NewL();
    CleanupStack::PushL( rights );

    asset = CDRMAsset::NewL();
    CleanupStack::PushL( asset );

    asset->iUid = aURI.AllocL();

    if ( aPermission.iParentUID )
        {
        asset->iParentRights = aPermission.iParentUID->AllocL();
        }

    rights->SetPermissionL( aPermission );
    rights->SetAssetL( *asset );

    CleanupStack::PopAndDestroy(); // asset
    CleanupStack::Pop(), // rights

    aRights = rights;
    }

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CDRMMessageParser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMMessageParser* CDRMMessageParser::NewL( void )
    {
    CDRMMessageParser* self = new( ELeave ) CDRMMessageParser();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CDRMMessageParser::CDRMMessageParser
// Constructor.
// -----------------------------------------------------------------------------
//
CDRMMessageParser::CDRMMessageParser() :
    iDcfCreator( NULL ),
    iBoundary( NULL ),
    iContentType( NULL ),
    iState( ESearchingBoundary ),
    iInputBuffer( NULL, 0, 0 )
    {
    iDCFHeaderSize[ 0 ] = KMaxTUint32;
    iDCFHeaderSize[ 1 ] = KMaxTUint32;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMMessageParser::~CDRMMessageParser()
    {
    TInt error = KErrNone;
    TRAP( error, FinalizeMessageParserL() );

    Reset();

    User::Free( const_cast< TUint8* >( iInputBuffer.Ptr() ) );
    }

// -----------------------------------------------------------------------------
// CDRMMessageParser::
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMMessageParser::InitializeMessageParserL( RWriteStream& aStream )
    {
    Reset();

    iDcfCreator = COma1DcfCreator::NewL();

    iOutputStream = aStream;
    }

// -----------------------------------------------------------------------------
// CDRMMessageParser::ProcessDataL
// Choose which operation is required.
// -----------------------------------------------------------------------------
EXPORT_C void CDRMMessageParser::ProcessMessageDataL( const TDesC8& aMessageData )
    {
    /*
    * What happens here is:
    *   - a boundary string is located and extracted
    *   - MIME header is read, and based on content-type field the internal
    *     state is set to either EReadingRightsPart or EReadingContentPart.
    *   - after processing the MIME part, internal state is updated again to
    *     EReadingHeaderPart if there are several MIME parts in the DRM
    *     message.
    *   - data is consumed from iInputData in each phase.
    *   - after everything is done, internal state is set to EAllDone.
    */
    if ( iState & EBroken )
        {
        User::Leave( KErrNotReady );
        }

    if ( iState & EAllDone )
        {
        return;
        }

    iInputData.Set( aMessageData );

    while( iInputData.Length() )
        {
        if ( iState & EReadingHeaderPart )
            {
            ReadHeaderL();
            }
        else
            {
            if ( iState & EReadingContentPart )
                {
                HandleContentDataL();
                }
            else
                {
                if ( iState & EReadingRightsPart )
                    {
                    HandleRightsDataL();
                    }
                else
                    {
                    FindBoundaryL();
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CDRMMessageParser::FinalizeL
// Finalize the message parser.
// -----------------------------------------------------------------------------
EXPORT_C void CDRMMessageParser::FinalizeMessageParserL()
    {
    TInt error( KErrNone );

    if ( iState & EEncryptStreamOk )
        {
        if ( iInputBuffer.Length() )
            {
            // Message is not parsed fully ==> error.
            // Try to delete the RO if it is possible.
            DeletePermission();
            error = KDRMMessageMalformed;
            }

        ClearBit( EEncryptStreamOk );
        iDcfCreator->EncryptFinalizeL();

        iOutputStream.CommitL();
        }

    Reset();

    User::LeaveIfError( error );
    }

// -----------------------------------------------------------------------------
// CDRMMessageParser::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDRMMessageParser::ConstructL()
    {
    // Make some extra room for crazy b64decode().
    iInputBuffer.Set( reinterpret_cast< TUint8* >(
                        User::AllocL( KInputBufferSize + 2 ) ),
                      0,
                      KInputBufferSize );
    }

// -----------------------------------------------------------------------------
// CDRMMessageParser::HandleContentDataL
// Process the content data:
// - base64 decoding
// - boundary checks
// - forward the processed data to ProcessContentDataL
// -----------------------------------------------------------------------------
//
void CDRMMessageParser::HandleContentDataL()
    {
   TPtrC8 res( NULL, 0 );
    TBool cont( ETrue );
    TInt remainder( 0 );

    // Loop until
    // - PrepareContentDataL leaves
    // - boundary end marker is found
    // - iInputBuffer is not updated anymore.
    for ( PrepareContentDataL();
          iInputBuffer.Length() &&
              ( remainder != iInputBuffer.Length() ) &&
              cont;
          PrepareContentDataL() )
        {
        TInt pos = iInputBuffer.Find( *iBoundary );

        if ( pos >= 0 )
            {
            if ( pos < KBoundaryMarkLength + 1 )
                {
                SetBrokenStateL( KDRMMessageMalformed );
                }

            res.Set( iInputBuffer.Left( pos - KBoundaryMarkLength ) );

            StripEndLineL( res );

            cont = EFalse;
            }
        else
            {
            // All the data cannot be processed immediately, because
            // there may be only a part of boundary string in this buffer
            // and the rest is got from the next input descriptor.
            remainder = iBoundary->Length() + KBoundaryMarkLength + 1;

            if ( iInputBuffer.Length() <= remainder )
                {
                return;
                }

            res.Set( iInputBuffer.Left( iInputBuffer.Length() -
                                        remainder ) );
            }

        if ( iState & EBase64 )
            {
            iUsedFromInput = HandleBase64DataL( res );
            }

        else
            {
            iUsedFromInput = res.Length();
            }

        ProcessContentDataL( res );

        CompressInputBuffer();

        remainder = iInputBuffer.Length();
        }

    if ( !cont )
        {
        // Discard all the remaining data.
        ClearBit( EReadingContentPart );
        SetBit( EAllDone );
        iInputBuffer.SetLength( 0 );
        iInputData.Set( NULL, 0 );
        }
    }

// -----------------------------------------------------------------------------
// CDRMMessageParser::HandleRightsDataL
// - check if the boundary is reached
// - check if the whole rights part is read, and allocate memory && copy the RO
//   if so
// - save the rights object
// -----------------------------------------------------------------------------
//
void CDRMMessageParser::HandleRightsDataL()
    {
    TPtrC8 res( NULL, 0 );

    do
        {
        res.Set( GetLineL() );

        TInt pos = res.Find( *iBoundary );

        if ( pos >= KBoundaryMarkLength )
            {
            if ( res.Left( KBoundaryMarkLength ) == KBoundaryMark )
                {
                // Allow empty RO here. If it is not allowed by
                // HandleRightsMessagePart(), an error is thrown.
                TBool last = EFalse;

                // Returns always true.
                IsBoundary( res, last );

                if ( last )
                    {
                    TInt error( KErrNone );
                    RPointerArray< CDRMRights > rights;
                    TPtrC8 ptr( iInputBuffer.Ptr(),
                                pos - KBoundaryMarkLength );

                    error = ProcessRightsObject(ptr, rights);
                    rights.ResetAndDestroy();

                    if ( !error )
                        {
                        SetBit( EAllDone );
                        }
                    else
                        {
                        SetBrokenStateL( error );
                        }
                    }
                else
                    {
                    // Save the RO since the CID needs to be either changed
                    // or created.
                    iRightsData = iInputBuffer.Left( iUsedFromInput -
                                                     res.Length() ).AllocL();
                    }

                CompressInputBuffer();

                ClearBit( EReadingRightsPart );
                ClearBit( EGotContentType );
                ClearBit( EGotContentEncoding );

                SetBit( EGotRightsPart );
                SetBit( EReadingHeaderPart );

                res.Set( NULL, 0 );
#ifndef __DRM_FULL
                User::Leave(KErrNotSupported);
#endif
                }
            else
                {
                SetBrokenStateL( KDRMMessageMalformed );
                }
            }
        else
            {
            if ( pos >= 0 )
                {
                SetBrokenStateL( KDRMMessageMalformed );
                }
            }
        } while ( res.Length() );
    }


// -----------------------------------------------------------------------------
// CDRMMessageParser::FindBoundaryL
// Tries to locate the boundary string from the available data in iInputBuffer.
// -----------------------------------------------------------------------------
//
void CDRMMessageParser::FindBoundaryL()
    {
    TPtrC8 line( NULL, 0 );

    FOREVER
        {
        line.Set( GetLineL() );

        if ( line.Length() > KBoundaryMarkLength )
            {
            TInt size = 0;

            if ( line.Left( KBoundaryMarkLength ) == KBoundaryMark )
                {
                size = line.Length() - KBoundaryMarkLength - 1;

                if ( line[ line.Length() - 2 ] == '\r' )
                    {
                    --size;
                    }

                iBoundary = line.Mid( KBoundaryMarkLength, size ).AllocL();

                SetBit( EGotBoundary );
                SetBit( EReadingHeaderPart );

                CompressInputBuffer();

                return;
                }
            }
        else
            {
            if ( line.Length() == 0 )
                {
                return;
                }
            }

        // Something else, not interested.
        CompressInputBuffer();
        }
    }

// ----------------------------------------------------------------------------
// CDRMMessageParser::ReadHeaderL
// The boundary is read and the following part is (should be) either a RO part
// or content part. The data is kept in iInputBuffer until the whole header
// part of the MIME header part is received. After plain "\r\n" line is
// received and content-type is defined, iState is updated.
// ----------------------------------------------------------------------------
//
void CDRMMessageParser::ReadHeaderL()
    {
    TPtrC8 line( NULL, 0 );
    TPtrC8 ptr( NULL, 0 );

    FOREVER
        {
        ///////////////////////////////////////////////////////////////////
        // Process the MIME header line-by-line. Process the lines if they
        // contain some information that is found useful. Update the
        // internal state according to findings.
        ///////////////////////////////////////////////////////////////////
        line.Set( GetLineL() );

        if ( line.Length() )
            {
            if ( line == KEndLine || line == KNewLine )
                {
                ///////////////////////////////////////////////
                // MIME header is read. Check what to do next.
                ///////////////////////////////////////////////
                if ( iState & EGotContentType )
                    {
                    // Sanity check: Either EReadingRightsPart or
                    // EReadingContentPart must defined.
                    __ASSERT_DEBUG( ( iState & EReadingRightsPart ) ||
                        ( iState & EReadingContentPart ),
                        User::Invariant() );

                    ClearBit( EReadingHeaderPart );

                    CompressInputBuffer();

                    // Check which part was read.
                    // If content part is being processed, some checkings
                    // need to be made.
                    if ( iState & EReadingContentPart )
                        {
                        if ( iContentType->CompareF( KDRMContentType ) == 0 )
                            {
                            if ( iState & EGotRightsPart )
                                {
                                // CD DCF.
                                SetBrokenStateL( KErrCANotSupported );
                                }

                            InitDCFBufferL();

                            SetBit( EDCFFile );
                            }
                        else
                            {
                            // Non-DCF FL content or normal CD content.
                            // Create or modify the CID, save the RO.
                            HandleFlContentL();

                             iDcfCreator->EncryptInitializeL(
                                iOutputStream,
                                *iContentType,
                                iRightsObject );
                            SetBit( EEncryptStreamOk );

                            // The RO handle iRightsObject is kept in order
                            // to delete the rights in case of content
                            // encryption error. In that case, this will
                            // generate unnecessary "RO Added / RO Deleted"
                            // notifications, but so what. "More correct"
                            // way of doing would be modifying
                            // EncryptInitialize not to
                            // save the RO, but then the key would have to
                            // be given to it by other means. Since we are
                            // not their "friend" class, we cannot access
                            // their members, and making them to ask our
                            // members when doing EncryptInitialize/Finalize
                            // might cause some problems perhaps.
                            }

                        delete iContentType;
                        iContentType = NULL;
                        }

                    return;
                    }

                // Empty MIME header.
                SetBrokenStateL( KDRMMessageMalformed );
                }

            ///////////////////////////////////
            // Check the line for content-type.
            ///////////////////////////////////
            if ( line.Length() > KContentType().Length()  &&
                !( line.Left( KContentType().Length() ).CompareF( KContentType ) ) )
                {
                if ( iState & EGotContentType )
                    {
                    // Content-type given twice.
                    SetBrokenStateL( KDRMMessageMalformed );
                    }

                ptr.Set( HeaderValueL( line ) );

                SetBit( EGotContentType );

                // Which part this is: rights or the actual content?
                if ( ( ptr.CompareF( KDRMXMLRightsType ) == 0 ) ||
                    ( ptr.CompareF( KDRMWBXMLRightsType ) == 0 ) )
                    {
                    if ( iState & EGotRightsPart )
                        {
                        // Rights are given twice.
                        SetBrokenStateL( KDRMMessageMalformed );
                        }

                    SetBit( EReadingRightsPart );
                    }
                else
                    {
                    if ( iState & EGotContentPart )
                        {
                        SetBrokenStateL( KDRMMessageMalformed );
                        }

                    SetBit( EReadingContentPart );

                    // Content-type is saved for future use.
                    iContentType = ptr.AllocL();
                    }
                }
            else
                {
                ////////////////////////////////////////////////
                // Check the line for content-transfer-encoding.
                ////////////////////////////////////////////////
                if ( line.Length() > KContentTransferEncoding().Length() &&
                    !( line.Left( KContentTransferEncoding().Length() ).
                    CompareF( KContentTransferEncoding ) ) )
                    {
                    if ( iState & EGotContentEncoding )
                        {
                        // Double line.
                        SetBrokenStateL( KDRMMessageMalformed );
                        }

                    ptr.Set( HeaderValueL( line ) );

                    SetBit( EGotContentEncoding );

                    // Throw an error if content-transfer-encoding
                    // is something we don't support.
                    if ( ptr.CompareF( KEncoding8bit ) &&
                        ptr.CompareF( KEncoding7bit) &&
                        ptr.CompareF( KEncodingBinary ) )
                        {
                        if ( ptr.CompareF( KEncodingBase64 ) )
                            {
                            SetBrokenStateL( KErrCANotSupported );
                            }

                        // So it has to be Base64.
                        SetBit( EBase64 );
                        }
                    }
                else
                    {
                    //////////////////////////////////////////
                    // Check the line for end boundary marker.
                    //////////////////////////////////////////
                    TBool final( EFalse );

                    if ( IsBoundary( line, final ) )
                        {
                        SetBrokenStateL( KDRMMessageMalformed );
                        }

                    // Else: some X-field, parameter or something else.
                    // The line is ignored.
                    }
                }

            CompressInputBuffer();
            }

        else
            {
            // No line available yet.
            return;
            }
        }
    }

// -----------------------------------------------------------------------------
// CDRMMessageParser::GetLineL
// Return a descriptor representing a single line ending to \n in iInputBuffer.
// -----------------------------------------------------------------------------
//
TPtrC8 CDRMMessageParser::GetLineL()
    {
    TInt pos = 0;
    TPtrC8 res( NULL, 0 );

    if ( iInputBuffer.Length() > iUsedFromInput )
        {
        pos = iInputBuffer.Mid( iUsedFromInput ).Find( KNewLine );

        if ( pos >= 0 )
            {
            res.Set( iInputBuffer.Mid( iUsedFromInput, pos - iUsedFromInput + 1 ) );
            iUsedFromInput = pos + 1;
            }
        }

    if ( res.Length() == 0 )
        {
        if ( iInputData.Length() )
            {
            pos = iInputData.Find( KNewLine );

            if ( pos < 0 )
                {
                if ( iInputBuffer.MaxSize() - iInputBuffer.Length() <
                    iInputData.Length() )
                    {
                    SetBrokenStateL( KDRMMessageMalformed );
                    }

                iInputBuffer.Append( iInputData );
                iInputData.Set( NULL, 0 );
                }
            else
                {
                if ( iInputBuffer.MaxSize() - iInputBuffer.Length() <
                    pos + 1 )
                    {
                    SetBrokenStateL( KDRMMessageMalformed );
                    }

                iInputBuffer.Append( iInputData.Left( pos + 1 ) );
                res.Set( iInputBuffer.Mid( iUsedFromInput ) );
                iUsedFromInput = iInputBuffer.Length();

                iInputData.Set( iInputData.Mid( pos + 1 ) );
                }
            }
        }

    return res;
    }

// -----------------------------------------------------------------------------
// CDRMMessageParser::HeaderValueL
// Return a descriptor representing a header value in aLine.
// -----------------------------------------------------------------------------
//
TPtrC8 CDRMMessageParser::HeaderValueL( const TDesC8& aLine )
    {
    TInt pos( 0 );
    TPtrC8 res( NULL, 0 );

    pos = aLine.Find( KColon );

    if ( pos <= 0 )
        {
        SetBrokenStateL( KDRMMessageMalformed );
        }

    pos += 1;

    while ( pos < aLine.Length() &&
            TChar( aLine[ pos ] ).IsSpace() )
        {
        ++pos;
        }

    // Don't overindex.
    if ( pos == aLine.Length() )
        {
        // Full of whitespaces.
        SetBrokenStateL( KDRMMessageMalformed );
        }

    // Drop possible parameters.
    res.Set( aLine.Mid( pos ) );
    pos = res.Find( KSemiColon );

    if ( pos >= 0 )
        {
        res.Set( res.Left( pos ) );
        }

    pos = res.Length();

    if ( !pos )
        {
        // Just parameters, no actual value.
        SetBrokenStateL( KDRMMessageMalformed );
        }

    // This can't underflow, since otherwise there would be only
    // semicolon & parameters (checked earlier).
    while( TChar( res[ pos - 1 ] ).IsSpace() )
        {
        --pos;
        }

    return res.Left( pos );
    }

// -----------------------------------------------------------------------------
// CDRMMessageParser::CompressInputBuffer
// Compress iInputBuffer.
// -----------------------------------------------------------------------------
//
void CDRMMessageParser::CompressInputBuffer()
    {
    if ( iUsedFromInput )
        {
        const TInt size = iInputBuffer.Length() - iUsedFromInput;

        Mem::Copy( const_cast< TUint8* >( iInputBuffer.Ptr() ),
            iInputBuffer.Ptr() + iUsedFromInput,
            size );

        iInputBuffer.SetLength( size );
        iUsedFromInput = 0;
        }
    }

// -----------------------------------------------------------------------------
// CDRMMessageParser::PrepareContentDataL
// Fills the iInputBuffer from iInputData.
// -----------------------------------------------------------------------------
//
void CDRMMessageParser::PrepareContentDataL()
    {
    if ( iInputData.Length() )
        {
        if ( iInputBuffer.Length() == iInputBuffer.MaxSize() )
            {
            SetBrokenStateL( KDRMMessageMalformed );
            }

        const TInt size = Min( iInputBuffer.MaxSize() - iInputBuffer.Length(),
                               iInputData.Length() );

        iInputBuffer.Append( iInputData.Left( size ) );

        iInputData.Set( iInputData.Mid( size ) );
        }
    }

// -----------------------------------------------------------------------------
// CDRMMessageParser::HandleBase64DataL
// Decode base64 encoded data from and to aData descriptor.
//
// -----------------------------------------------------------------------------
//
TInt CDRMMessageParser::HandleBase64DataL( TPtrC8& aData )
    {
    TUint8* consumed = const_cast< TUint8* >( aData.Ptr() );
    TUint32 temp1 = 0;
    TUint8 temp2 = 0;

    for ( temp1 = 0, temp2 = 0; ( TInt )temp1 < aData.Length(); ++temp1 )
        {
        if ( ValidB64CharL( *( aData.Ptr() + temp1 ) ) )
            {
            ++temp2;
            if ( temp2 == 4 )
                {
                consumed = const_cast< TUint8* >( aData.Ptr() ) + temp1;
                temp2 = 0;
                }
            }
        }

    if ( consumed != aData.Ptr() )
        {
        User::LeaveIfError( b64decode( const_cast< TUint8* >( aData.Ptr() ),
                                       consumed - aData.Ptr() + 1,
                                       const_cast< TUint8* >( aData.Ptr() ),
                                       &temp1 ) );

        aData.Set( aData.Ptr(), temp1 );

        temp1 = consumed - aData.Ptr() + 1;
        }

    else
        {
        aData.Set( aData.Ptr(), 0 );
        temp1 = 0;
        }

    return static_cast< TInt >( temp1 );
    }

// -----------------------------------------------------------------------------
// CDRMMessageParser::StripEndLineL
// Remove \r\n from the end of the line. Update aBuf accordingly.
// It is allowed that only \n exists, since some WAP gateways used to
// strip \r's.
// -----------------------------------------------------------------------------
//
void CDRMMessageParser::StripEndLineL( TPtrC8& aBuf )
    {
    TInt newSize( aBuf.Length() );

    if ( aBuf.Right( 1 ) == KNewLine )
        {
        --newSize;

        if ( ( aBuf.Length() > 1 ) &&
            ( aBuf.Right( 2 ) == KEndLine ) )
            {
            --newSize;
            }

        aBuf.Set( aBuf.Left( newSize ) );
        }
    else
        {
        SetBrokenStateL( KDRMMessageMalformed );
        }
    }

// -----------------------------------------------------------------------------
// CDRMMessageParser::IsBoundary
// Check if the given line is a boundary line. Also check if the line is
// the end boundary.
// -----------------------------------------------------------------------------
//
TBool CDRMMessageParser::IsBoundary( const TDesC8& aLine, TBool& aLast ) const
    {
    TBool res = EFalse;

    __ASSERT_DEBUG( iBoundary, User::Invariant() );

    if ( iState & EGotBoundary )
        {
        if ( aLine.Length() > KBoundaryMarkLength + iBoundary->Length() )
            {
            TPtrC8 tmp( NULL, 0 );

            if ( ( aLine.Left( KBoundaryMarkLength ) == KBoundaryMark ) &&
                ( aLine.Mid( KBoundaryMarkLength, iBoundary->Length() ).
                Compare( *iBoundary ) == 0 ) )
                {
                res = ETrue;

                tmp.Set( aLine.Right( aLine.Length() -
                    KBoundaryMarkLength -
                    iBoundary->Length() ) );

                if ( ( tmp.Length() >= KBoundaryMarkLength ) &&
                    ( tmp.Left( KBoundaryMarkLength ) == KBoundaryMark ) )
                    {
                    aLast = ETrue;
                    }
                }
            }
        }

    return res;
    }

// -----------------------------------------------------------------------------
// CDRMMessageParser::HandleFlContentL
// Process FL/CD rights object.
// -----------------------------------------------------------------------------
//
void CDRMMessageParser::HandleFlContentL()
    {
    __ASSERT_DEBUG( !( iState & EDCFFile ), User::Invariant() );

    if ( iRightsData )
        {
        HBufC8* cid( NULL );
        HBufC8* buf( NULL );

        // RO was found from DRM message.
        CreateCDCIDL( cid );
        CleanupStack::PushL( cid );

        buf = iRightsData;
        CleanupStack::PushL( buf );
        iRightsData = NULL;

        ProcessRightsDataL( *cid,
                            *buf );

        CleanupStack::PopAndDestroy(); // buf
        CleanupStack::PopAndDestroy(); // cid
        }
    else
        {
        TInt error = RetrieveFlRights();
        if ( error )
            {
            SetBrokenStateL( error );
            }
        }
    }

// -----------------------------------------------------------------------------
// CDRMMessageParser::ProcessRightsData
// Process DRM message -style rights object and fetch it to iRightsObject if
// necessary.
// -----------------------------------------------------------------------------
//
void CDRMMessageParser::ProcessRightsDataL(
                            const TDesC8& aCID,
                            const TDesC8& aData )
    {
    __ASSERT_DEBUG( !iRightsObject, User::Invariant() );

    TInt start = aData.Find(KRightsStartTag);
    TInt end = aData.LocateReverse('>');

    if ( start >= 0 && end > start )
        {
        TDRMUniqueID localID( 0 );
        CDrmRightsParser* parser( NULL );
        CDRMAsset* asset( NULL );
        RDRMRightsClient client;
        TPtrC8 ptr( &aData[start], end - start + 1 );
        TBuf8< KDCFKeySize > key;
        key.SetLength(KDCFKeySize);
        RPointerArray< CDRMRights > rights;

        TCleanupItem cleanup( DoResetAndDestroy, &rights );
        CleanupStack::PushL( cleanup );

        User::LeaveIfError( client.Connect() );
        CleanupClosePushL( client );

        parser = CDrmRightsParser::NewL();
        CleanupStack::PushL( parser );

        parser->ParseL( ptr, rights );

        if (rights.Count()==0)
            {
            User::Leave(KErrCorrupt);
            }

        client.GetRandomDataL(key);

        User::LeaveIfError( client.AddRecord(
                            key,
                            rights[0]->GetPermission(),
                            aCID,
                            localID) );

        iRightsObject = CDRMRights::NewL();

        asset = CDRMAsset::NewLC();
        asset->iUid = aCID.AllocL();

        iRightsObject->SetAssetL(*asset);
        iRightsObject->SetPermissionL(rights[0]->GetPermission());

        CleanupStack::PopAndDestroy( 4 ); // asset, parser, client, rights
        }
    else
        {
        SetBrokenStateL( KErrArgument );
        }
    }

// -----------------------------------------------------------------------------
// CDRMMessageParser::ProcessContentDataL
// Handle DCF file's CID manipulation in "DCF in DRM message" case.
// Send the data to EncryptUpdateL if encryption is needed.
// -----------------------------------------------------------------------------
//
void CDRMMessageParser::ProcessContentDataL( TPtrC8& aData )
    {
    // First, check if the content is DCF stuff.
    if ( iState & EDCFFile )
        {
        // Sanity check: DCF File with RO is illegal.
        __ASSERT_DEBUG( !iRightsObject, User::Invariant() );

        ProcessDCFDataL( aData );
        }
    else
        {
        // No, plain FL content.
        iDcfCreator->EncryptUpdateL( aData );
        }
    }

// -----------------------------------------------------------------------------
// CDRMMessageParser::ValidB64CharL
// Determine if the given character is valid base64 character, and leave
// if it is not.
// -----------------------------------------------------------------------------
//
TBool CDRMMessageParser::ValidB64CharL( const TUint8 aChar )
    {
    // Allowed characters are '0'...'9', 'A'...'Z', 'a'...'z', '+', '/', '='
    if ( ( aChar >= 48 && aChar <= 57 ) ||
         ( aChar >= 65 && aChar <= 90 ) ||
         ( aChar >= 97 && aChar <= 122 ) ||
         ( aChar == 43 ) ||
         ( aChar == 47 ) ||
         ( aChar == 61 ) )
        {
        return ETrue;
        }

    if ( ( aChar != 0x0D ) && ( aChar != 0x0A ) )
        {
        SetBrokenStateL( KDRMMessageMalformed );
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// DRMCommon::ProcessMessage
//
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMMessageParser::ProcessMessage(
    HBufC8*& aDRMMessage)
    {
    TInt error( KErrNone );
    TRAP( error, DoProcessMessageL( aDRMMessage ) );

    return error;
    }

// -----------------------------------------------------------------------------
// DRMCommon::ProcessRightsObject
// Processes a rights objects and saves it in the rights database.
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMMessageParser::ProcessRightsObject(
    const TDesC8& aRightsObject,
    RPointerArray<CDRMRights>& aRightsDetail)
    {
    TInt error( KErrNone );
    TRAP( error, DoProcessRightsObjectL( aRightsObject,
                                         aRightsDetail ) );

    return error;
    }

void CDRMMessageParser::SetBit( TUint32 aBit )
    {
    iState |= aBit;
    }

void CDRMMessageParser::ClearBit( TUint32 aBit )
    {
    iState &= ~aBit;
    }

void CDRMMessageParser::Reset()
    {
    delete iDCFBuffer;
    iDCFBuffer = NULL;

    delete iDcfCreator;
    iDcfCreator = NULL;

    delete iRightsData;
    iRightsData = NULL;

    delete iRightsObject;
    iRightsObject = NULL;

    delete iBoundary;
    iBoundary = NULL;

    delete iContentType;
    iContentType = NULL;

    iInputBuffer.SetLength( 0 );

    iState = ESearchingBoundary;
    iDCFHeaderSize[ 0 ] = KMaxTUint32;
    iDCFHeaderSize[ 1 ] = KMaxTUint32;
    }

void CDRMMessageParser::DoProcessMessageL( HBufC8*& aDRMMessage )
    {
    TDeleteFileData fileData;
    RFileWriteStream output;
    RFile file;
    TInt size( 0 );

    User::LeaveIfError( fileData.aFs.Connect() );
    CleanupClosePushL( fileData.aFs );

    TCleanupItem cleanup( DoDeleteFile, &fileData );
    CleanupStack::PushL( cleanup );

#ifndef RD_MULTIPLE_DRIVE

    User::LeaveIfError( output.Temp( fileData.aFs,
                                     KTempPath,
                                     fileData.aName,
                                     EFileWrite ) );

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
    fileData.aFs.DriveToChar( driveNumber, driveLetter );

    TFileName tempPath;
    tempPath.Format( KTempPath, (TUint)driveLetter );

    User::LeaveIfError( output.Temp( fileData.aFs,
                                   tempPath,
                                   fileData.aName,
                                   EFileWrite ) );

#endif

    CleanupClosePushL( output );

    InitializeMessageParserL( output );
    ProcessMessageDataL( *aDRMMessage );
    FinalizeMessageParserL();

    CleanupStack::PopAndDestroy(); // output

    User::LeaveIfError( file.Open( fileData.aFs, fileData.aName, EFileRead ) );
    CleanupClosePushL( file );

    User::LeaveIfError( file.Size( size ) );

    if ( size > aDRMMessage->Des().MaxSize() )
        {
        HBufC8* tmp( NULL );

        delete aDRMMessage;
        aDRMMessage = NULL;

        tmp = HBufC8::NewLC( size );
        TPtr8 data( tmp->Des() );

        User::LeaveIfError( file.Read( data ) );
        CleanupStack::Pop(); // tmp

        aDRMMessage = tmp;
        }
    else
        {
        TPtr8 data( aDRMMessage->Des() );
        User::LeaveIfError( file.Read( data ) );
        }

    CleanupStack::PopAndDestroy( 3 ); // file, cleanup, fileData.aFs
    }

void CDRMMessageParser::DoProcessRightsObjectL(
    const TDesC8& aRightsObject,
    RPointerArray<CDRMRights>& aRightsDetail )
    {
    CDrmRightsParser* parser = NULL;
    TInt start;
    TInt end;
    TPtrC8 ptr(0, 0);

    start = aRightsObject.Find(KRightsStartTag);
    end = aRightsObject.LocateReverse('>');

    TCleanupItem cleanup( DoResetAndDestroy, &aRightsDetail );
    CleanupStack::PushL( cleanup );

    if ( start != KErrNotFound && end != KErrNotFound )
        {
        // xml
        parser = CDrmRightsParser::NewL();
        CleanupStack::PushL(parser);

        ptr.Set(&aRightsObject[start], end - start + 1);

        parser->ParseAndStoreL(ptr, aRightsDetail);

        if (aRightsDetail.Count() == 0)
            {
            User::Leave( KErrArgument );
            }

        CleanupStack::PopAndDestroy(); // parser
        }
    else if (start == KErrNotFound )
        {
        // wbxml
        parser = CDrmRightsParser::NewL(CDrmRightsParser::EWbxmlParser);
        CleanupStack::PushL(parser);

        parser->ParseAndStoreL(aRightsObject, aRightsDetail);
        if (aRightsDetail.Count() == 0)
            {
            User::Leave( KErrArgument );
            }

        CleanupStack::PopAndDestroy(); // parser
        }
    else
        {
        User::Leave( KErrArgument );
        }

    CleanupStack::Pop(); // cleanup
    }

void CDRMMessageParser::DeletePermission()
    {
    if ( iRightsObject )
        {
        if ( !( iState & EFLContent ) )
            {
            HBufC8* URI = NULL;

            if ( !( iRightsObject->GetContentURI( URI ) ) )
                {
                __ASSERT_DEBUG( URI, User::Invariant() );

                RDRMRightsClient client;

                TInt error = client.Connect();

                if ( !error )
                    {
                    // Don't care if it fails.
                    client.DeleteDbEntry( *URI, iRightsObject->GetLocalID() );

                    client.Close();
                    }

                delete URI;
                URI = NULL;
                }
            }

        delete iRightsObject;
        iRightsObject = NULL;
        }
    }

void CDRMMessageParser::SetBrokenStateL( const TInt aError )
    {
    SetBit( EBroken );
    DeletePermission();

    User::Leave( aError );
    }

void CDRMMessageParser::InitDCFBufferL()
    {
    iDCFBuffer = HBufC8::New( KInitialDCFBufferSize );

    if ( !iDCFBuffer )
        {
        SetBrokenStateL( KErrNoMemory );
        }
    }

void CDRMMessageParser::CreateCDCIDL( HBufC8*& aCID )
    {
    aCID = HBufC8::NewL( KCDContentIDLength );
    TPtr8 des( aCID->Des() );
    TBuf8<KCDPlainIDLength> id;
    id.SetLength(KCDPlainIDLength);

    RDRMRightsClient client;
    CleanupClosePushL(client);
    User::LeaveIfError( client.Connect() );
    client.GetRandomDataL(id);
    CleanupStack::PopAndDestroy();

    des = KFLPrefix;

    for (TInt i = 0; i < KCDPlainIDLength; i++)
        {
        des.AppendNumFixedWidth( id[i], EDecimal, 1 );
        }

    des.Append( KFLSuffix );
    }

TInt CDRMMessageParser::RetrieveFlRights()
    {
    __ASSERT_DEBUG( !iRightsObject, User::Invariant() );

    RDRMRightsClient client;
    TInt error( client.Connect() );

    if ( !error )
        {
        TRAP( error, EnsureFLRightsExistL( client, &iRightsObject ) );

        client.Close();
        }

    return error;
    }

void CDRMMessageParser::ProcessDCFDataL( TPtrC8& aDCFData )
    {
    TBool doCommit( EFalse );

    // Loop until all available data is either cached or processed.
    // What happens is
    // - modify Content-URI
    // - modify DCF header part
    // - dump the rest of the available data to output.
    while ( iDCFBuffer->Length() || aDCFData.Length() )
        {
        if ( !( iState & EDCFURIModified ) )
            {
            //////////////////////////////////////////////////////////////////
            // Modify ContentURI.
            //////////////////////////////////////////////////////////////////
            FillDCFBufferL( aDCFData );

            if ( iDCFBuffer->Length() > 3 )
                {
                TInt pos = 0;
                TUint8* data = const_cast< TUint8* > ( iDCFBuffer->Ptr() );

                if ( data[ 0 ] != 1 )
                    {
                    SetBrokenStateL( KDRMMessageMalformed );
                    }

                // Cache the data until
                // - version,
                // - ContentTypeLen,
                // - ContentURILen,
                // - ContentType and
                // - ContentURI
                // have been received.
                pos = data[ 1 ] + 3;
                if ( iDCFBuffer->Length() < pos + data[ 2 ] )
                    {
                    return;
                    }

                // Sanity check. URI has to be more than four ("cid:") octets.
                if ( data[ 2 ] < 5 )
                    {
                    SetBrokenStateL( KDRMMessageMalformed );
                    }

                if ( Mem::CompareF( &data[ pos ],
                                    KCIDStringLength,
                                    KCIDString().Ptr(),
                                    KCIDStringLength ) == 0 )
                    {
                    // The data length doesn't change, so Mem::Copy() is safe.
                    Mem::Copy( &data[ pos ],
                               KFLPrefix().Ptr(),
                               KFLKPrefixLength );

                    iOutputStream.WriteL( iDCFBuffer->Left( pos + data[ 2 ] ) );

                    doCommit = ETrue;

                    CompressDCFBuffer( pos + data[ 2 ] );

                    SetBit( EDCFURIModified );
                    }
                else
                    {
                    // Discard the broken DCF
                    SetBrokenStateL( KDRMMessageMalformed );
                    }
                }
            else
                {
                // Not enough data yet.
                return;
                }
            }
        else
            {
            //////////////////////////////////////////////////////////////////
            // URI is now modified. Modify DCF headers if necessary.
            //////////////////////////////////////////////////////////////////
            if ( !( iState & EDCFHeadersModified ) )
                {
                FillDCFBufferL( aDCFData );

                // Figure out how much data there is in the header part and/or
                // in the data part.
                if ( ( iDCFHeaderSize[ 0 ] == KMaxTUint32 ) &&
                     ( iDCFHeaderSize[ 1 ] == KMaxTUint32 ) )
                    {
                    TUint8 used( 0 );

                    // uintvar has 1...5 octets and we need two uintvars before
                    // anything can be done. Still, there has to be at least
                    // 12 octets of data in total to make the DCF legal. But here
                    // we just check the two uintvars.
                    if ( iDCFBuffer->Length() < 10 )
                        {
                        // Not enough data yet.
                        // No need to flush aDCFData at this point, since it
                        // simply cannot contain anything at this point.
                        __ASSERT_DEBUG( !aDCFData.Length(), User::Invariant() );
                        break;
                        }

                    // Read header field length & data length.
                    for ( TUint8 count = 0; count < 2; ++count )
                        {
                        TInt size( 0 );

                        TWspPrimitiveDecoder decoder(
                                iDCFBuffer->Mid( used ) );
                        size = decoder.UintVar( iDCFHeaderSize[ count ] );

                        // Sanity check.
                        if ( size < 1 || iDCFHeaderSize[ count ] >= KMaxTInt )
                            {
                            SetBrokenStateL( KDRMMessageMalformed );
                            }

                        used += size;
                        }

                    CompressDCFBuffer( used );
                    }
                else
                    {
                    // We know the original header size now.
                    // Wait until the whole header is read.
                    if ( static_cast< TUint32 >( iDCFBuffer->Length() ) <
                         iDCFHeaderSize[ 0 ] )
                        {
                        if ( !aDCFData.Length() )
                            {
                            // All available data is now processed.
                            break;
                            }
                        }
                    else
                        {
                        TUint offset( 0 );
                        TUint length( 0 );
                        HBufC8* newHeader = NULL;

                        if ( FindDCFHeader( KRightsIssuer,
                                            offset,
                                            length ) )
                            {
                            // The header field exists.
                            TInt pos( 0 );
                            TPtr8 trim( const_cast< TUint8* >(
                                            iDCFBuffer->Ptr() ) + offset,
                                            length,
                                            length );

                            iDCFHeaderSize[ 0 ] -= length; // remove old data.

                            // No colon, no header value.
                            pos = trim.Find( KColon );
                            if ( pos > 0 )
                                {
                                trim.Set( const_cast< TUint8* >( trim.Ptr() ) + pos + 1,
                                          trim.Length() - pos - 1,
                                          trim.Length() - pos - 1 );

                                // Skip whitespaces
                                trim.TrimLeft();
                                trim.TrimRight();

                                if ( trim.Length() )
                                    {
                                    // Something to process
                                    newHeader = EncryptDCFFieldLC( trim );

                                    iDCFHeaderSize[ 0 ] += KRightsIssuer().Length();
                                    iDCFHeaderSize[ 0 ] += newHeader->Length();
                                    iDCFHeaderSize[ 0 ] += 3; // ":" and CRLF
                                    }
                                }
                            }

                        // Write the uintvars to output.
                        for ( TUint8 loop = 0; loop < 2; ++loop )
                            {
                            TWspPrimitiveEncoder encoder;
                            HBufC8* var( encoder.UintVarL( iDCFHeaderSize[ loop ] ) );

                            CleanupStack::PushL( var );

                            iOutputStream.WriteL( *var );

                            CleanupStack::PopAndDestroy(); // var
                            }

                        // Dump the header data to output stream.
                        iOutputStream.WriteL( iDCFBuffer->Left( offset ) );

                        if ( newHeader )
                            {
                            iOutputStream.WriteL( KRightsIssuer );
                            iOutputStream.WriteL( KColon );
                            iOutputStream.WriteL( *newHeader );

                            CleanupStack::PopAndDestroy(); // newHeader
                            newHeader = NULL;

                            iOutputStream.WriteL( KEndLine );
                            }

                        iOutputStream.WriteL(
                            iDCFBuffer->Right( iDCFBuffer->Length() -
                                                offset -
                                                length ) );

                        doCommit = ETrue;

                        CompressDCFBuffer( iDCFBuffer->Length() );
                        SetBit( EDCFHeadersModified );
                        }
                    }
                }
            else
                {
                // Dump the rest of the data.
                __ASSERT_DEBUG( !( iDCFBuffer->Length() ), User::Invariant() );

                iOutputStream.WriteL( aDCFData );

                aDCFData.Set( NULL, 0 );

                doCommit = ETrue;
                }
            }
        }

    if ( doCommit )
        {
        iOutputStream.CommitL();
        }
    }

void CDRMMessageParser::FillDCFBufferL( TPtrC8& aData )
    {
    if ( aData.Length() )
        {
        TInt size( 0 );

        if ( iDCFBuffer->Length() == iDCFBuffer->Des().MaxSize() )
            {
            HBufC8* ptr = iDCFBuffer->ReAlloc( iDCFBuffer->Length() +
                            KDefaultInputBufferSize );

            if ( !ptr )
                {
                SetBrokenStateL( KErrNoMemory );
                }

            iDCFBuffer = ptr;
            }

        size = Min( iDCFBuffer->Des().MaxSize() - iDCFBuffer->Length(),
                    aData.Length() );

        iDCFBuffer->Des().Append( aData.Left( size ) );

        aData.Set( aData.Right( aData.Length() - size ) );
        }
    }

void CDRMMessageParser::CompressDCFBuffer( const TInt aHowMuch )
    {
    __ASSERT_DEBUG( aHowMuch <= iDCFBuffer->Des().MaxSize(), User::Invariant() );

    *iDCFBuffer = iDCFBuffer->Des().Mid( aHowMuch );
    }
    TBool CDRMMessageParser::FindDCFHeader( const TDesC8& aString,
                                        TUint& aOffset,
                                        TUint& aLength )
    {
    TPtrC8 des( const_cast< TUint8* >( iDCFBuffer->Ptr() ),
                Min( iDCFBuffer->Length(), iDCFHeaderSize[ 0 ] ) );

    aOffset = 0;
    aLength = 0;

    while ( des.Length() )
        {
        TInt pos = des.Find( KEndLine );
        if ( pos < 0 )
            {
            // Last header doesn't end to "\r\n"
            pos = des.Length();
            if ( pos < 0 )
                {
                return EFalse;
                }
            }
        else
            {
            pos += KEndLineLength;
            }

        if ( ( pos < aString.Length() ) ||
             ( des.Left( aString.Length() ).CompareF( aString ) != 0 ) )
             {
            // Skip the line, since this can't be the one we're looking for.
            des.Set( des.Right( des.Length() -
                                pos ) );
            }
        else
            {
            aOffset = des.Ptr() - iDCFBuffer->Ptr();
            aLength = pos;

            return ETrue;
            }
        }

    // Never reached.
    return EFalse;
    }

void CDRMMessageParser::EnsureFLRightsExistL(
    RDRMRightsClient& aClient,
    CDRMRights** aOutput )
    {
    HBufC8* cid( NULL );
    CDRMRights* perm( NULL );
    RPointerArray< CDRMPermission > rights( 1 );

    TCleanupItem cleanup( DoResetAndDestroy2, &rights );
    CleanupStack::PushL( cleanup );

    User::LeaveIfError( aClient.ForwardLockURI( cid ) );
    CleanupStack::PushL( cid );

    TRAPD( error, aClient.GetDBEntriesL( *cid, rights ) );

    if ( !error )
        {
        ConvertPermissionL( perm, *( rights[ 0 ] ), *cid );
        // No need to pushl 'perm' into cleanup stack.
        }
    else if ( error == KErrCANoRights )
        {
        HBufC8* fl( NULL );
        RPointerArray< CDRMRights > rightslist( 1 );

        TCleanupItem cleanup2( DoResetAndDestroy, &rightslist );
        CleanupStack::PushL( cleanup2 );

        fl = HBufC8::NewLC( KFLROSize + cid->Length() );

        *fl = KROPart1;
        fl->Des().Append( *cid );
        fl->Des().Append( KROPart2 );

        User::LeaveIfError( ProcessRightsObject( *fl, rightslist ) );

        CleanupStack::PopAndDestroy(); // fl

        perm = rightslist[ 0 ];
        rightslist.Remove( 0 );

        CleanupStack::PopAndDestroy(); // cleanup2

        error = KErrNone;
        }

    CleanupStack::PopAndDestroy(); // cid

    if ( !error )
        {
        // There is something.
        if ( aOutput )
            {
            *aOutput = perm;
            }
        else
            {
            delete perm; perm = NULL;
            }
        }

    CleanupStack::PopAndDestroy(); // cleanup
    }


HBufC8* CDRMMessageParser::EncryptDCFFieldLC( const TDesC8& aOldHeader )
    {
    HBufC8* res = NULL;
    RDRMRightsClient client;

    User::LeaveIfError( client.Connect() );
    CleanupClosePushL( client );

    // Make sure FL rights exists.
    EnsureFLRightsExistL( client, NULL );

    User::LeaveIfError(
        client.EncodeRightsIssuerField( aOldHeader, res ) );

    CleanupStack::PopAndDestroy(); // client

    CleanupStack::PushL( res );

    return res;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
