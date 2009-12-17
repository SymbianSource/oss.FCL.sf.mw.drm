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
* Description:  Access to content and data attributes
*
*/


// Includes
#include <apgcli.h>
#include <f32file.h>
#include <charconv.h>

#include <caf.h>
#include <caf/attributeset.h>
#include <caf/stringattributeset.h>
#include <caf/virtualpath.h>
#include <caf/caferr.h>
#include <caf/caftypes.h>
#include <utf.h>
#include <drmagents.h>
#include <oma2agent.h>
#include <wmdrmagent.h>
#include "wmdrmagentattributes.h"
#include "asf.h"
#include "logfn.h"

using namespace ContentAccess;

// ============================ MEMBER FUNCTIONS ===============================

// PointerArrayResetDestroyAndClose
// Template method used to push RPointerArrays to the cleanup stack. Takes
// care of deleting all pointers in the array.
// -----------------------------------------------------------------------------

template<class S>
void PointerArrayResetDestroyAndClose(TAny* aPtr)
        {
    (reinterpret_cast<RPointerArray<S>*>(aPtr))->ResetAndDestroy();
    (reinterpret_cast<RPointerArray<S>*>(aPtr))->Close();
        }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
TInt TWmDrmAgentAttributes::GetAttributeL(
    CAsf* /*aAsfFile*/,
    TInt aAttribute,
    TInt& aValue,
    const TVirtualPathPtr& /*aVirtualPath*/,
    TBool /*aReusePreviousQuery*/ )
    {
    TInt r = KErrNone;

    LOGFNR( "TWmDrmAgentAttributes::GetAttributeL", r );
    aValue = ETrue;
    switch ( aAttribute )
        {
        case DRM::EDrmAgentUid:
            aValue = DRM::EDrmWmAgent;
            break;
        case ECanPlay:
        case ECanView:
            aValue = EFalse;
            r = KErrCANotSupported;
            break;
        case ERightsStateless:
        case ERightsConsumable:
            aValue = EFalse;
            r = KErrCANotSupported;
            break;
        case ECanAutomaticConsume:
            aValue = EFalse;
            r = KErrCANotSupported;
            break;
        case EIsForwardable:
        case EIsCopyable:
        case ECanRewind:
        case ECopyPaste:
        case ECanMove:
        case ECanRename:
            aValue = ETrue;
            break;
        case EIsModifyable:
        case ECanPrint:
        case ECanExecute:
        case EPreviewAvailable:
            aValue = EFalse;
            break;
        case EIsProtected:
            r = KErrCANotSupported;
            break;
        case EContentCDataInUse:
            aValue = ETrue;
            break;
        case EFileType:
            aValue = EAsf;
            break;
        case DRM::EDrmFileType:
            aValue = DRM::EDrmWMFile;
            break;
        case DRM::EDrmAllowedOutputs:
            aValue = 0;
            r = KErrCANotSupported;
           break;
        default:
            r = KErrCANotSupported;
            break;
        };
    LOG3( "Attribute: %d, value: %d", aAttribute, aValue );
    return r;
    }

// -----------------------------------------------------------------------------
TInt TWmDrmAgentAttributes::GetAttributeSetL(
    CAsf* aAsfFile,
    RAttributeSet& aAttributeSet,
    const TVirtualPathPtr& aVirtualPath )
    {
    TInt i = 0;
    TInt attribute = 0;
    TInt value=0;
    TInt err = KErrNone;
    TInt numAttributes = aAttributeSet.Count();

    LOGFN( "TWmDrmAgentAttributes::GetAttributeSetL" );

    // loop through all the attriutes in the set and find their values
    for ( i = 0; i < numAttributes && err == KErrNone; i++ )
        {
        attribute = aAttributeSet[i];
        err = GetAttributeL( aAsfFile, attribute,
            value, aVirtualPath, ETrue );
        err = aAttributeSet.SetValue( attribute, value, err );
        }
    return err;
    }

// -----------------------------------------------------------------------------
TInt TWmDrmAgentAttributes::GetStringAttributeL(
    CAsf* aAsfFile,
    TInt aAttribute,
    TDes& aValue,
    const TVirtualPathPtr& /*aVirtualPath*/,
    TBool /*aReusePreviousQuery*/ )
    {
    TInt err = KErrNone;
    HBufC* b = NULL;
    HBufC* allocated = NULL;
    TFileName fileName;

    LOGFNR( "TWmDrmAgentAttributes::GetStringAttributeL", err );
    // Initialise the attribute value to be zero length in case it's not
    // supported or an error occurs
    aValue.SetLength( 0 );

    switch( aAttribute )
        {
        case EIconURI:
        case EPreviewURI:
        case EContentURI:
        case EInfoURL:
        case EPendingRightsETA:
            err = KErrCANotSupported;
            break;

        case EMimeType:
            aValue.Copy( aAsfFile->iMimeType );
            break;
        case EDescription:
            if (aAsfFile->iDescription)
                {
                b = aAsfFile->iDescription;
                }
            break;
        case ETitle:
            if (aAsfFile->iTitle)
                {
                b = aAsfFile->iTitle;
                }
            break;
        case EPerformer:
        case EAuthor:
            if (aAsfFile->iAuthor)
                {
                b = aAsfFile->iAuthor;
                }
            break;
        case EContentID:
            if (aAsfFile->iUniqueFileID)
                {
                b = aAsfFile->iUniqueFileID;
                }
            break;
        case ECopyright:
            if (aAsfFile->iCopyright)
                {
                b = aAsfFile->iCopyright;
                }
            break;
        case ERating:
        case DRM::EDrmRatingInfo:
            if (aAsfFile->iRating)
                {
                b = aAsfFile->iRating;
                }
            break;
        case EAlbumTitle:
            if (aAsfFile->iAlbumTitle)
                {
                b = aAsfFile->iAlbumTitle;
                }
            break;
        case EPicture:
            if (aAsfFile->iPicture)
                {
                b = aAsfFile->iPicture;
                }
            break;
        case EText:
            if (aAsfFile->iText)
                {
                b = aAsfFile->iText;
                }
            break;
        case EComposer:
            if (aAsfFile->iComposer)
                {
                b = aAsfFile->iComposer;
                }
            break;
        case EGenre:
            if (aAsfFile->iGenre)
                {
                b = aAsfFile->iGenre;
                }
            break;
        case EYear:
        case DRM::EDrmRecordingYear:
            if (aAsfFile->iYear)
                {
                b = aAsfFile->iYear;
                }
            break;
        case EOriginalArtist:
        case DRM::EDrmOriginalArtist:
            if (aAsfFile->iOriginalArtist)
                {
                b = aAsfFile->iOriginalArtist;
                }
            break;
        case EWmTrackNumber:
        case DRM::EDrmTrackNumber:
            if (aAsfFile->iTrackNumber)
                {
                b = aAsfFile->iTrackNumber;
                }
            break;
        case EAudioFileUrl:
            if (aAsfFile->iAudioFileUrl)
                {
                b = aAsfFile->iAudioFileUrl;
                }
            break;
        case ESharedUserRating:
            if (aAsfFile->iSharedUserRating)
                {
                b = aAsfFile->iSharedUserRating;
                }
            break;
        case EDate:
            if (aAsfFile->iDate)
                {
                b = aAsfFile->iDate;
                }
            break;
        case DRM::EDrmAgentName:
            allocated = DRM::KDrmWMAgentName().AllocL();
            break;
        case DRM::EDrmFileName:
            err = aAsfFile->iFile.Name( fileName );
            if( aValue.MaxLength() < fileName.Length() )
                {
                return KErrOverflow;
                }
            aValue.Copy( fileName );
            break;
        case DRM::EDrmFullName:
            err = aAsfFile->iFile.FullName( fileName );
            if ( aValue.MaxLength() < fileName.Length() )
                {
                return KErrOverflow;
                }
            aValue.Copy( fileName );
            break;
        default:
            err = KErrCANotSupported;
            break;
        };

    if (b != NULL)
        {
        if( b->Length() > aValue.MaxLength() )
            {
            return KErrOverflow;
            }
        err = KErrNone;
        aValue.Copy(*b);
        }
    else if( allocated )
        {
        if( allocated->Length() <= aValue.MaxLength() )
            {
            err = KErrNone;
            aValue.Copy(*allocated);
            }
        else
            {
            err = KErrOverflow;
            }
        delete allocated;
        allocated = NULL;
        }

    LOG2( "Attribute: %d", aAttribute );
    return err;
    }

// -----------------------------------------------------------------------------
TInt TWmDrmAgentAttributes::GetStringAttributeSetL(
    CAsf* aAsfFile,
    RStringAttributeSet& aStringAttributeSet,
    const TVirtualPathPtr& aVirtualPath )
    {
    TInt i = 0;
    TInt attribute = 0;
    TInt err = KErrNone;
    TBuf <KMaxDataTypeLength> buf;

    LOGFN( "TWmDrmAgentAttributes::GetStringAttributeSetL" );
    TInt numAttributes = aStringAttributeSet.Count();

    // loop through all the attriutes in the set and find their values
    for ( i = 0; i < numAttributes && err==KErrNone; i++ )
        {
        buf.SetLength( 0 );
        attribute = aStringAttributeSet[i];
        err = GetStringAttributeL( aAsfFile, attribute, buf, aVirtualPath );
        err = aStringAttributeSet.SetValue(attribute,buf, err);
        }
    return err;
    }


// End of file
