/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <f32file.h>
#include <utf.h>
#include <wspdecoder.h>
#include "Oma1Dcf.h"
#include "DRMRightsClient.h"
#include <centralrepository.h>

#include "DrmUtilityInternalcrkeys.h"      // Cenrep extension for OmaBased

// LOCAL CONSTANTS AND MACROS
_LIT8(KMimeApplication, "application");
_LIT8(KMimeText, "text");
_LIT8(KMimeAudio, "audio");
_LIT8(KMimeImage, "image");
_LIT8(KMimeVideo, "video");
_LIT8(KMimeModel, "model");
_LIT8(KMimeMessage, "message");
_LIT8(KMimeMultipart, "multipart");
_LIT8(KXHeader, "x-");
_LIT8(KColon, ":");
//_LIT8(KSemiColon, ";");
//_LIT8(KNewLine, "\n");
_LIT8(KEndLine, "\r\n");

const TInt KCenRepDataLength( 50 );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// COma1Dcf::COma1Dcf
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
COma1Dcf::COma1Dcf():
    iHeaders(NULL)
    {
    }

// -----------------------------------------------------------------------------
// COma1Dcf::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void COma1Dcf::ConstructL(const RFile& aFile)
    {
    CDcfCommon::ConstructL(aFile);
    User::LeaveIfError(ReadHeaderL());
    }

void COma1Dcf::ConstructL(const TDesC8& aMemoryBlock)
    {
    User::LeaveIfError(ReadHeaderL(aMemoryBlock));
    iData = aMemoryBlock.AllocL();
    }

// -----------------------------------------------------------------------------
// COma1Dcf::FetchOmaBasedInfoL
// -----------------------------------------------------------------------------
//
HBufC8* COma1Dcf::FetchOmaBasedInfoL()
    {
    CRepository* repository( NULL );
    RBuf bOmaBasedMimeType;
    HBufC8* mimetype = NULL;

    CleanupClosePushL(bOmaBasedMimeType);
    bOmaBasedMimeType.CreateL( KCenRepDataLength );

    repository = CRepository::NewL( KCRUidOmaBased );

    CleanupStack::PushL( repository );

    User::LeaveIfError(repository->Get( KOmaBasedMimeType, bOmaBasedMimeType ));
    mimetype = CnvUtfConverter::ConvertFromUnicodeToUtf8L( bOmaBasedMimeType );
    CleanupStack::PopAndDestroy( repository );

    CleanupStack::PopAndDestroy();

    return mimetype;

    }

// -----------------------------------------------------------------------------
// COma1Dcf::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C COma1Dcf* COma1Dcf::NewL(const RFile& aFile)
    {
    COma1Dcf* self = new(ELeave) COma1Dcf;

    CleanupStack::PushL(self);
    self->ConstructL(aFile);
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// COma1Dcf::
//
// -----------------------------------------------------------------------------
//
EXPORT_C COma1Dcf* COma1Dcf::NewL(const TDesC8& aMemoryBlock)
    {
    COma1Dcf* self = new(ELeave) COma1Dcf;

    CleanupStack::PushL(self);
    self->ConstructL(aMemoryBlock);
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// COma1Dcf::
//
// -----------------------------------------------------------------------------
//
EXPORT_C COma1Dcf::~COma1Dcf()
    {
    delete iHeaders;
    }

// -----------------------------------------------------------------------------
// COma1Dcf::
//
// -----------------------------------------------------------------------------
//
TInt COma1Dcf::ReadHeaderL(const TDesC8& aMemoryBlock)
    {
    TInt r = KErrNone;
    TInt cidLength = 0;
    TInt mimeLength = 0;
    TInt fieldLength = 0;
    TBuf8<10> lengthFields;
    TUint32 length;
    TPtr8 ptr(NULL,0,0);

    if (aMemoryBlock.Length()<3)
        {
        User::Leave(KErrArgument);
        }

    iVersion = aMemoryBlock[0];

    if (iVersion!=1)
        {
        // for OMA Version 1, DCF version must be 1
        User::Leave(KErrArgument);
        }

    mimeLength = aMemoryBlock[1];
    cidLength = aMemoryBlock[2];

    if (mimeLength + cidLength + 3 + 5 + 5 > aMemoryBlock.Length())
        {
        User::Leave(KErrArgument);
        }


    if (mimeLength!=0)
        {
        iMimeType = aMemoryBlock.Mid(3, mimeLength).AllocL();
        ptr.Set(iMimeType->Des());
        r = ptr.FindF(_L8("/"));
        if (r==KErrNotFound)
            {
            User::Leave(KErrArgument);
            }
        }
    else
        {
        User::Leave(KErrArgument);
        }


    if (cidLength!=0)
        {
        iContentID = aMemoryBlock.Mid(3 + mimeLength, cidLength).AllocL();
        }
    else
        {
        User::Leave(KErrArgument);
        }


    lengthFields.Copy(aMemoryBlock.Mid(3 + mimeLength + cidLength, 10));
    TWspPrimitiveDecoder decoder(lengthFields);
    fieldLength = decoder.UintVar(length);
    if( fieldLength < 0 )
        {
        User::Leave(KErrArgument);
        }
    iHeaderLength = length;
    fieldLength += decoder.UintVar(length);
    if( fieldLength < 0 )
        {
        User::Leave(KErrArgument);
        }

    iDataLength = length;
    iOffset = 3 + mimeLength + cidLength + fieldLength + iHeaderLength;
    if (iDataLength == 0)
        {
        iDataLength = aMemoryBlock.Length() - iOffset;
        }
    iPlainTextLength = iDataLength - KDCFKeySize;
    iPlainTextLengthValid = EFalse;

    if (mimeLength + cidLength + 3 + 5 + 5 + iHeaderLength <=
        aMemoryBlock.Length())
        {
        iHeaders = aMemoryBlock.Mid(3 + mimeLength + cidLength +
                                    fieldLength, iHeaderLength).AllocL();

        ReadHeaderValuesL();
        }
    return r;
    }

// -----------------------------------------------------------------------------
// COma1Dcf::ReadHeaderValuesL
//
// -----------------------------------------------------------------------------
//
void COma1Dcf::ReadHeaderValuesL(void)
    {
    TPtrC8 ptr(NULL, 0);

    if (GetHeaderL(KRightsIssuer, ptr) == KErrNone)
        {
        if( iRightsIssuerURL )
            {
            delete iRightsIssuerURL;
            iRightsIssuerURL = NULL;
            }
        iRightsIssuerURL = ptr.AllocL();
        }
    if (GetHeaderL(KContentName, ptr) == KErrNone)
        {
        if( iTitle )
            {
            delete iTitle;
            iTitle = NULL;
            }
        iTitle = ptr.AllocL();
        }
    if (GetHeaderL(KContentDescription, ptr) == KErrNone)
        {
        if( iDescription )
            {
            delete iDescription;
            iDescription = NULL;
            }
        iDescription = ptr.AllocL();
        }
    if (GetHeaderL(KIconURI, ptr) == KErrNone)
        {
        if( iIconUri )
            {
            delete iIconUri;
            iIconUri = NULL;
            }
        iIconUri = ptr.AllocL();
        }
    }

// -----------------------------------------------------------------------------
// COma1Dcf::
//
// -----------------------------------------------------------------------------
//
TInt COma1Dcf::ReadHeaderL(void)
    {
    TInt r = KErrNone;
    TInt pos = 0;
    TUint8 v = 0;
    TPtr8 p(&v, 1, 1);
    TInt cidLength = 0;
    TInt mimeLength = 0;
    TInt fieldLength = 0;
    TBuf8<10> lengthFields;
    TUint32 length;
    TInt size;

    iFile.Size(size);
    if (size<3)
        {
        User::Leave(KErrArgument);
        }
    iFile.Seek(ESeekStart, pos);

    iFile.Read(p);
    iVersion = v;
    if (iVersion!=1)
        {
        // for OMA Version 1, DCF version must be 1
        User::Leave(KErrArgument);
        }

    iFile.Read(p);
    mimeLength = v;
    if (mimeLength==0)
        {
        User::Leave(KErrArgument);
        }

    iFile.Read(p);
    cidLength = v;
    if (cidLength==0)
        {
        User::Leave(KErrArgument);
        }

    iMimeType = HBufC8::NewMax(mimeLength);
    User::LeaveIfNull(iMimeType);
    p.Set(iMimeType->Des());
    iFile.Read(p, mimeLength);
    p.Set(iMimeType->Des());
    r = p.FindF(_L8("/"));
    if (r==KErrNotFound)
        {
        User::Leave(KErrArgument);
        }

    iContentID = HBufC8::NewMax(cidLength);
    User::LeaveIfNull(iContentID);
    p.Set(iContentID->Des());
    iFile.Read(p, cidLength);

    pos = 0;
    iFile.Seek(ESeekCurrent, pos);
    iFile.Read(lengthFields);
    TWspPrimitiveDecoder decoder(lengthFields);
    fieldLength = decoder.UintVar(length);
    if( fieldLength < 0 )
        {
        User::Leave(KErrArgument);
        }
    iHeaderLength = length;
    fieldLength += decoder.UintVar(length);
    if( fieldLength < 0 )
        {
        User::Leave(KErrArgument);
        }

    iDataLength = length;
    iOffset = pos + fieldLength + iHeaderLength;
    if (iDataLength == 0)
        {
        iDataLength = size - iOffset;
        }
    iPlainTextLength = iDataLength - KDCFKeySize;
    iPlainTextLengthValid = EFalse;

    pos = pos + fieldLength;
    iFile.Seek(ESeekStart, pos);
    iHeaders = HBufC8::NewL(iHeaderLength);
    p.Set(iHeaders->Des());
    iFile.Read(p, iHeaderLength);

    ReadHeaderValuesL();

    return r;
    }

// -----------------------------------------------------------------------------
// COma1Dcf::
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool COma1Dcf::IsValidDcf(const TDesC8& aDcfFragment)
    {
    TPtrC8 mimeType;
    TInt i;
    TBool r = EFalse;
    TInt error = KErrNone;
    HBufC8* omaBasedBuf = NULL;

    if (aDcfFragment.Length() > 15)
        {
        i = aDcfFragment.Right(aDcfFragment.Length() - 3).Locate('/');
        if (aDcfFragment[0] == 1 && i != KErrNotFound)
            {
            mimeType.Set(aDcfFragment.Mid(3, i));
            if (mimeType.CompareF(KMimeImage) == 0 ||
                mimeType.CompareF(KMimeAudio) == 0 ||
                mimeType.CompareF(KMimeApplication) == 0 ||
                mimeType.CompareF(KMimeText) == 0 ||
                mimeType.CompareF(KMimeModel) == 0 ||
                mimeType.CompareF(KMimeMessage) == 0 ||
                mimeType.CompareF(KMimeVideo) == 0 ||
                mimeType.CompareF(KMimeMultipart) == 0 ||
                aDcfFragment.Mid(3, 2).CompareF(KXHeader) == 0)
                {
                r = ETrue;
                // Check for specific mimetype

                TRAP( error, omaBasedBuf = FetchOmaBasedInfoL() );

                if( !error )
                    {
                    mimeType.Set(aDcfFragment.Mid(3,
                            omaBasedBuf->Length()));


                    if (mimeType.CompareF( *omaBasedBuf ) == 0)
                        {
                        r = EFalse;
                        }
                    }
                delete omaBasedBuf;
                }
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// COma1Dcf::
//
// -----------------------------------------------------------------------------
//
TInt COma1Dcf::CheckUniqueId(const TDesC& aUniqueId)
    {
    TInt r = CDcfCommon::CheckUniqueId(aUniqueId);

    if (r == KErrNotFound)
        {
        HBufC8* id = NULL;
        TRAPD(err, id = CnvUtfConverter::ConvertFromUnicodeToUtf8L(aUniqueId));
        if (err == KErrNone)
            {
            if (iContentID->Compare(*id) == 0)
                {
                r = 0;
                }
            }
        else
            {
            r = err;
            }
        delete id;
        }
    return r;
    }

// -----------------------------------------------------------------------------
// COma1Dcf::
//
// -----------------------------------------------------------------------------
//
TInt COma1Dcf::OpenPart(const TDesC& aUniqueId)
    {
    return OpenPart(CheckUniqueId(aUniqueId));
    }

// -----------------------------------------------------------------------------
// COma1Dcf::
//
// -----------------------------------------------------------------------------
//
TInt COma1Dcf::OpenPart(TInt aPart)
    {
    if (aPart == 0)
        {
        return KErrNone;
        }
    else
        {
        return KErrNotFound;
        }
    }

// -----------------------------------------------------------------------------
// COma1Dcf::
//
// -----------------------------------------------------------------------------
//
void COma1Dcf::GetPartIdsL(RPointerArray<HBufC8>& aPartList)
    {
    aPartList.ResetAndDestroy();
    aPartList.AppendL(iContentID->Des().AllocL());
    }

// -----------------------------------------------------------------------------
// COma1Dcf::GetHeaderL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt COma1Dcf::GetHeaderL(
    const TDesC8& aHeaderName,
    TPtrC8& aHeaderValue)
    {
    TInt i;
    TInt j;
    TPtrC8 ptr( iHeaders->Des() );

    // Add Room for CRLF and Semicolon:
    HBufC8* buffer = HBufC8::NewMaxLC( aHeaderName.Length() + 3 );
    TPtr8 searchBuf( const_cast<TUint8*>(buffer->Ptr()), 0, buffer->Des().MaxSize() );

    searchBuf.Copy(aHeaderName);
    searchBuf.Append(KColon);

    // First see if the
    i = ptr.Find(searchBuf);
    if( i < 0 )
        {
        CleanupStack::PopAndDestroy(); // buffer
        return KErrNotFound;
        }

   if( i > 0 )
        {
        // if it's not the first one, use the search buffer:
        // Create the search buffer
        searchBuf.Copy(KEndLine);
        searchBuf.Append(aHeaderName);
        searchBuf.Append(KColon);

        // First see if the
        i = ptr.Find(searchBuf);
        if ( i < 0 )
            {
            CleanupStack::PopAndDestroy(); // buffer
            return KErrNotFound;
            }
        }
    // Move search buffer
    i += searchBuf.Length();

    j = ptr.Mid(i).Find(KEndLine);
    if( j < 0 )
        {
        CleanupStack::PopAndDestroy(); // buffer
        return KErrNotFound;
        }

    aHeaderValue.Set( ptr.Mid(i, j) );

    CleanupStack::PopAndDestroy(); // buffer
    return KErrNone;
    }

//  End of File
