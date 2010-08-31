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
* Description:  DRM Utility common UI
*
*/


// INCLUDE FILES

// multiple drive support
#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

// notes and queries
#include <coemain.h>
#include <bautils.h>
#include <StringLoader.h>
#include <stringresourcereader.h>
#include <aknnotewrappers.h>
#include <AknGlobalNote.h>
#include <AknQueryDialog.h>
#include <aknlistquerydialog.h>

// secondary display support
#include <AknMediatorFacade.h>
#include <featmgr.h>
#include <aknSDData.h>
#include <MediatorEventProvider.h>
#include <MediatorDomainUIDs.h>

// caf
#include <caf/caftypes.h>
#include <caf/data.h>

// resources
#include <data_caging_path_literals.hrh>
#include <drmutility.rsg>

// drm
#include <DrmConstraint.h>
#include <Oma2Agent.h>
#include <drmutilitytypes.h>

#include "drmutilityui.h"
#include "drmutilitysecondarydisplay.h"
#include "DrmUtilityGlobalNoteWrapper.h"
#include "DrmUtilityInfoNoteWrapper.h"

#include "drmuidialogids.h"
#include "drmuidialogs.h"

// CONSTANTS
#ifndef RD_MULTIPLE_DRIVE
_LIT( KDriveZ, "z:" );
#else
_LIT( KRomDriveFormatter, "%c:" );
#endif

_LIT( KCDrmUtilityResFileName,"drmutility.rsc" );
_LIT( KAvkonResFileName, "avkon.rsc" );

// buffer sizes
const TInt KDRMUtilityMaxDateFormatLen( 30 );
const TInt KDRMUtilityMaxDateLen( 30 );
const TInt KDRMUtilityMaxTimeFormatLen( 30 );
const TInt KDRMUtilityMaxTimeLen( 30 );
const TInt KDRMUtilityNoteMaxSize ( 256 );
const TInt KNoValue = -1;

#ifdef _DEBUG
// debug panic
_LIT( KDRMUtilityDebugPanicMessage, "DrmUtilityCommonDebugPanic" );
const TInt KDRMUtilityDebugPanicCode( 1 );
#endif

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDrmUtilityUI::CDrmUtilityUI
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
DRM::CDrmUtilityUI::CDrmUtilityUI(
    CCoeEnv* aCoeEnv ) : iCoeEnv( aCoeEnv )
    {
    }

// -----------------------------------------------------------------------------
// CDrmUtilityUI::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityUI::ConstructL()
    {
    if ( !iCoeEnv )
        {
        User::LeaveIfError( iFs.Connect() );
        User::LeaveIfError( iFs.ShareAuto() );
        }

    TPtr utilityResourcePtr( NULL, 0 );
    iUtilityResourceFile = HBufC::NewL( KMaxFileName );
    utilityResourcePtr.Set( iUtilityResourceFile->Des() );

    TPtr avkonResourcePtr( NULL, 0 );
    iAvkonResourceFile = HBufC::NewL( KMaxFileName );
    avkonResourcePtr.Set( iAvkonResourceFile->Des() );

#ifndef RD_MULTIPLE_DRIVE

    utilityResourcePtr.Append( KDriveZ );
    avkonResourcePtr.Append( KDriveZ );

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, driveNumber );

    if ( iCoeEnv )
        {
        iCoeEnv->FsSession().DriveToChar( driveNumber, driveLetter );
        }
    else
        {
        iFs.DriveToChar( driveNumber, driveLetter );
        }

    utilityResourcePtr.AppendFormat( KRomDriveFormatter, (TUint)driveLetter );
    avkonResourcePtr.AppendFormat( KRomDriveFormatter, (TUint)driveLetter );

#endif

    utilityResourcePtr.Append( KDC_RESOURCE_FILES_DIR );
    utilityResourcePtr.Append( KCDrmUtilityResFileName );

    avkonResourcePtr.Append( KDC_RESOURCE_FILES_DIR );
    avkonResourcePtr.Append( KAvkonResFileName );

    TFileName resourceFile;
    if ( iCoeEnv )
        {
        resourceFile = *iUtilityResourceFile;
        BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), resourceFile );
        iUtilityResourceFileOffset = iCoeEnv->AddResourceFileL( resourceFile );

        resourceFile = *iAvkonResourceFile;
        BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), resourceFile );
        iAvkonResourceFileOffset = iCoeEnv->AddResourceFileL( resourceFile );
        }
    else
        {
        resourceFile = *iUtilityResourceFile;
        iUtilityStringResourceReader = CStringResourceReader::NewL( resourceFile, iFs );

        resourceFile = *iAvkonResourceFile;
        iAvkonStringResourceReader = CStringResourceReader::NewL( resourceFile, iFs );
        }

    FeatureManager::InitializeLibL();

    if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) )
        {
        iEventProvider = CMediatorEventProvider::NewL();
        }
    }


// -----------------------------------------------------------------------------
// CDrmUtilityUI::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmUtilityUI* DRM::CDrmUtilityUI::NewLC(
    CCoeEnv* aCoeEnv )
    {
    DRM::CDrmUtilityUI* self( new( ELeave ) CDrmUtilityUI( aCoeEnv ) );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// -----------------------------------------------------------------------------
// CDrmUtilityUI::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmUtilityUI* DRM::CDrmUtilityUI::NewL(
    CCoeEnv* aCoeEnv )
    {
    DRM::CDrmUtilityUI* self( NewLC( aCoeEnv ) );

    CleanupStack::Pop( self );

    return self;
    }

// Destructor
DRM::CDrmUtilityUI::~CDrmUtilityUI()
    {
    if ( iCoeEnv )
        {
        iCoeEnv->DeleteResourceFile( iUtilityResourceFileOffset );
        iCoeEnv->DeleteResourceFile( iAvkonResourceFileOffset );
        }
    else
        {
        delete iUtilityStringResourceReader;
        delete iAvkonStringResourceReader;
        iFs.Close();
        }

    delete iUtilityResourceFile;
    delete iAvkonResourceFile;
    delete iEventProvider;

    iNoteList.ResetAndDestroy();
    iNoteList.Close();

    FeatureManager::UnInitializeLib();
    }

// -----------------------------------------------------------------------------
// CDrmUtilityUI::DisplayQueryWithIdL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUtilityUI::DisplayQueryWithIdL(
    TInt aTextResourceId,
    TInt aQueryResourceId )
    {
    TInt button( 0 );

    if ( iCoeEnv )
        {
        HBufC* msgText( StringLoader::LoadLC( aTextResourceId, iCoeEnv ) );
        button = DisplayQueryL( *msgText, aQueryResourceId );
        CleanupStack::PopAndDestroy( msgText );
        }
    else
        {
        TInt buttonsId(
            aQueryResourceId == R_DRMUTILITY_WAITING_RIGHTS_CONFIRMATION_QUERY ?
                                                R_AVKON_SOFTKEYS_OK_EMPTY__OK :
                                                R_AVKON_SOFTKEYS_YES_NO__YES );

        DRM::CDrmUtilityGlobalNoteWrapper* noteWrapper(
            DRM::CDrmUtilityGlobalNoteWrapper::NewLC( iUtilityStringResourceReader ) );

        button = noteWrapper->ShowNoteWithButtonsL( aTextResourceId, buttonsId );

        CleanupStack::PopAndDestroy( noteWrapper );
        }

    return button;
    }

// -----------------------------------------------------------------------------
// CDrmUtilityUI::DisplayQueryWithIdValueL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUtilityUI::DisplayQueryWithIdValueL(
    TInt aTextResourceId,
    TInt aQueryResourceId,
    const TDesC& aString )
    {
    TInt button( 0 );

    if ( iCoeEnv )
        {
        HBufC* msgText( StringLoader::LoadLC( aTextResourceId,
                                              aString,
                                              iCoeEnv ) );
        button = DisplayQueryL( *msgText, aQueryResourceId );
        CleanupStack::PopAndDestroy( msgText );
        }
    else
        {
        TInt buttonsId(
            aQueryResourceId == R_DRMUTILITY_WAITING_RIGHTS_CONFIRMATION_QUERY ?
                                                R_AVKON_SOFTKEYS_OK_EMPTY__OK :
                                                R_AVKON_SOFTKEYS_YES_NO__YES );

        DRM::CDrmUtilityGlobalNoteWrapper* noteWrapper(
            DRM::CDrmUtilityGlobalNoteWrapper::NewLC( iUtilityStringResourceReader ) );

        button = noteWrapper->ShowNoteWithButtonsL( aTextResourceId,
                                                    buttonsId,
                                                    aString );
        CleanupStack::PopAndDestroy( noteWrapper );
        }

    return button;
    }

// -----------------------------------------------------------------------------
// CDrmUtilityUI::DisplayQueryL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUtilityUI::DisplayQueryL(
    TInt aTextResourceId,
    TInt aValue )
    {
    TInt resultCode( ECancelled );

    CDrmUIDialogs* drmUiDialog( CDrmUIDialogs::NewLC() ); 
    resultCode = drmUiDialog->ShowNoteL( aTextResourceId, KNullDesC, aValue );
    CleanupStack::PopAndDestroy( drmUiDialog );
    return resultCode;
    }

// -----------------------------------------------------------------------------
// CDrmUtilityUI::DisplayQueryL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUtilityUI::DisplayQueryL(
    TInt aTextResourceId,
    const TDesC& aString )
    {
    TInt resultCode( ECancelled );

    CDrmUIDialogs* drmUiDialog( CDrmUIDialogs::NewLC() );
    resultCode = drmUiDialog->ShowNoteL( aTextResourceId, aString, KNoValue );
    CleanupStack::PopAndDestroy( drmUiDialog );
    return resultCode;
    }

// -----------------------------------------------------------------------------
// CDrmUtilityUI::DisplayQueryL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUtilityUI::DisplayQueryL(
    TInt aTextResourceId,
    const TDesC& aString,
    TInt aValue,
    TInt aStringPos,
    TInt aValuePos )
    {
    TInt buttonCode( 0 );

    if ( iCoeEnv )
        {
        TPtr bufPtr( NULL, 0 );

        TBuf<KDRMUtilityNoteMaxSize> textBuffer;
        TBuf<KDRMUtilityNoteMaxSize> textBuffer2;
        StringLoader::Load( textBuffer, aTextResourceId, iCoeEnv );
        StringLoader::Format( textBuffer2, textBuffer, aValuePos, aValue );
        textBuffer = textBuffer2;
        StringLoader::Format( textBuffer2, textBuffer, aStringPos, aString );

        bufPtr.Set( const_cast <TUint16*>( textBuffer2.Ptr() ),
                                           textBuffer2.Length(),
                                           textBuffer2.Length() );

        AknTextUtils::LanguageSpecificNumberConversion( bufPtr );

        CAknQueryDialog* dlg(
            CAknQueryDialog::NewL( CAknQueryDialog::ENoTone ) );

        PrepareSecondaryDisplayL( *dlg, aTextResourceId, aString, aValue );

        buttonCode =
            dlg->ExecuteLD( R_DRMUTILITY_CONFIRMATION_QUERY, textBuffer2 );

        CancelSecondaryDisplayL( aTextResourceId );
        }
    else
        {
        DRM::CDrmUtilityGlobalNoteWrapper* noteWrapper(
            DRM::CDrmUtilityGlobalNoteWrapper::NewLC( iUtilityStringResourceReader ) );

        buttonCode = noteWrapper->ShowNoteL( aTextResourceId,
                                             aString,
                                             aValue,
                                             aStringPos,
                                             aValuePos );

        CleanupStack::PopAndDestroy( noteWrapper );
        }

    return buttonCode;
    }

// -----------------------------------------------------------------------------
// CDrmUtilityUI::DisplayQueryL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUtilityUI::DisplayQueryL(
    TDesC& aPromptText,
    TInt aQueryResourceId )
    {
    CAknQueryDialog* dlg( new (ELeave) CAknQueryDialog(
                                                aPromptText,
                                                CAknQueryDialog::ENoTone ) );

    TPtr bufPtr( NULL, 0 );
    bufPtr.Set( const_cast <TUint16*>( aPromptText.Ptr() ),
                                       aPromptText.Length(),
                                       aPromptText.Length() );

    AknTextUtils::LanguageSpecificNumberConversion( bufPtr );

    PrepareSecondaryDisplayL( *dlg, aQueryResourceId, aPromptText, -1 );

    TInt ret( dlg->ExecuteLD( aQueryResourceId ) );

    CancelSecondaryDisplayL( aQueryResourceId );

    return ret;
    }

// -----------------------------------------------------------------------------
// CDrmUtilityUI::SetAutomatedQuery
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DRM::CDrmUtilityUI::SetAutomatedQueryL(
    CDRMConstraint* aConstraint )
    {
    __ASSERT_DEBUG( aConstraint, User::Panic( KDRMUtilityDebugPanicMessage,
                                              KDRMUtilityDebugPanicCode ) );

    TInt buttonCode( EOk );

    if ( aConstraint->iActiveConstraints & EConstraintEndTime )
        {
        TBuf<KDRMUtilityMaxDateLen> dateFormat;
        TBuf<KDRMUtilityMaxDateLen> endDate;
        TTime endTime( aConstraint->iEndTime );

        if ( iCoeEnv )
            {
            StringLoader::Load( dateFormat,
                                R_QTN_DATE_USUAL_WITH_ZERO,
                                iCoeEnv );
            }
        else
            {
            dateFormat = iAvkonStringResourceReader->ReadResourceString(
                                                R_QTN_DATE_USUAL_WITH_ZERO );
            }

        endTime.FormatL( endDate, dateFormat );

        // Qt dialog not implmented yet.
        buttonCode = DisplayQueryL( EQueryLicenceValidUntil, endDate );
        }
    else if ( aConstraint->iActiveConstraints & EConstraintInterval )
        {
        // Qt dialog not implemented yet.
        buttonCode = DisplayQueryL( EQueryValidForLimitedTime, KNoValue );
        }
    return buttonCode;
    }

// -----------------------------------------------------------------------------
// CDrmUtilityUI::ShowFutureRightsNoteL
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUtilityUI::ShowFutureRightsNoteL(
    CDRMConstraint* aConstraint)
    {
    __ASSERT_DEBUG( aConstraint, User::Panic( KDRMUtilityDebugPanicMessage,
                                              KDRMUtilityDebugPanicCode ) );
    _LIT( KSpace, " " );
    TTime startTime( Time::NullTTime() );

    User::LeaveIfError( GetOmaStartTime( aConstraint, startTime ) );

    TBuf<KDRMUtilityMaxDateFormatLen> dateFormat;
    TBuf<KDRMUtilityMaxTimeFormatLen> timeFormat;
    TBuf<KDRMUtilityMaxDateLen + KDRMUtilityMaxTimeLen + 1> startDateBuf;
    TBuf<KDRMUtilityMaxTimeLen> startTimeBuf;

    if ( iCoeEnv )
        {
        StringLoader::Load( dateFormat, R_QTN_DATE_USUAL_WITH_ZERO, iCoeEnv );
        StringLoader::Load( timeFormat, R_QTN_TIME_USUAL_WITH_ZERO, iCoeEnv );
        }
    else
        {
        dateFormat = iAvkonStringResourceReader->ReadResourceString(
                                            R_QTN_DATE_USUAL_WITH_ZERO );
        timeFormat = iAvkonStringResourceReader->ReadResourceString(
                                            R_QTN_TIME_USUAL_WITH_ZERO );
        }

    // format date and time
    startTime.FormatL( startDateBuf, dateFormat );
    startTime.FormatL( startTimeBuf, timeFormat );
    startDateBuf.Append( KSpace );
    startDateBuf.Append( startTimeBuf );

    // display note with start date
    // Qt dialog not implemented yet
    DisplayNoteL( EConfUnableToUse, startDateBuf );
    }

// -----------------------------------------------------------------------------
// CDrmUtilityUI::DisplayNoteL
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUtilityUI::DisplayNoteL( TInt aTextResourceId )
    {
    CDrmUIDialogs* drmUiDialog( CDrmUIDialogs::NewLC() );
    drmUiDialog->ShowNoteL( aTextResourceId, KNullDesC, KNoValue );
    CleanupStack::PopAndDestroy( drmUiDialog );
    }

// -----------------------------------------------------------------------------
// CDrmUtilityUI::DisplayNoteL
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUtilityUI::DisplayNoteL(
    TInt aTextResourceId,
    const TDesC& aString )
    {
    CDrmUIDialogs* drmUiDialog( CDrmUIDialogs::NewLC() );
    drmUiDialog->ShowNoteL( aTextResourceId, aString, KNoValue );
    CleanupStack::PopAndDestroy( drmUiDialog );
    }

// -----------------------------------------------------------------------------
// CDrmUtilityUI::DisplayNoteL
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUtilityUI::DisplayNoteL(
    TInt aTextResourceId,
    TInt aValue )
    {
    CDrmUIDialogs* drmUiDialog( CDrmUIDialogs::NewLC() );
    drmUiDialog->ShowNoteL( aTextResourceId, KNullDesC, aValue );
    CleanupStack::PopAndDestroy( drmUiDialog );
    }

// -----------------------------------------------------------------------------
// CDrmUtilityUI::DisplayNoteL
// -----------------------------------------------------------------------------
//
EXPORT_C void DRM::CDrmUtilityUI::DisplayNoteL(
    TDesC& aPromptText,
    TInt aResourceId,
    const TDesC& aString,
    TInt aValue )
    {
    TPtr bufPtr( NULL, 0 );
    bufPtr.Set( const_cast <TUint16*>( aPromptText.Ptr() ),
                                       aPromptText.Length(),
                                       aPromptText.Length() );

    AknTextUtils::LanguageSpecificNumberConversion( bufPtr );

    if ( iCoeEnv )
        {
        CAknResourceNoteDialog* note( CreateNoteForResourceL( aResourceId ) );
        PrepareSecondaryDisplayL( *note, aResourceId, aString, aValue );
        note->ExecuteLD( aPromptText );
        CancelSecondaryDisplayL( aResourceId );
        }
    else
        {
        DRM::CDrmUtilityInfoNoteWrapper* note(
                                DRM::CDrmUtilityInfoNoteWrapper::NewL() );

        //a list for simultanous notes
        User::LeaveIfError( iNoteList.Append( note ) );

        note->ShowNoteL( GlobalNoteTypeForResource( aResourceId ),
                         aPromptText,
                         aResourceId,
                         aString,
                         aValue );
        }
    }

// -----------------------------------------------------------------------------
// CDrmUtilityUI::DisplayPopupWindowsForPreviewL
// -----------------------------------------------------------------------------
//
#ifdef RD_DRM_PREVIEW_RIGHT_FOR_AUDIO

EXPORT_C TInt DRM::CDrmUtilityUI::DisplayPopupWindowsForPreviewL(
    ContentAccess::CData& aContent,
    TInt aPreviewType,
    TInt aMediaType )
    {
    TInt buttonCode( 0 );
    TInt resourceId( 0 );
    TInt index( 0 );
    TInt answer( 0 );

    // if media type is not given, try to get if from MIME type
    if( aMediaType == DRM::EUPreviewRightsActionDefault ||
        aMediaType == DRM::EUEmbeddedPreviewActionDefault )
        {
        aMediaType = EvaluatePreviewMediaTypeL( aContent, aPreviewType );
        }

    if ( aPreviewType == ContentAccess::EPreviewRights )
        {
        // preview rights for audio
        if ( aMediaType == DRM::EUPreviewRightsActionDefaultAudio )
            {
            resourceId = R_DRMUTILITY_PREV_AUDIO_GET_LIST_QUERY;
            }
        // preview rights for video
        else if ( aMediaType == DRM::EUPreviewRightsActionDefaultVideo )
            {
            resourceId = R_DRMUTILITY_PREV_VIDEO_GET_LIST_QUERY;
            }
        }
    else if ( aPreviewType == ContentAccess::EInstantPreview )
        {
        // embedded preview for audio
        if ( aMediaType == DRM::EUEmbeddedPreviewActionDefaultAudio )
            {
            resourceId = R_DRMUTILITY_PREV_AUDIO_PLAY_LIST_QUERY;
            }
        // embedded preview for video
        else if ( aMediaType == DRM::EUEmbeddedPreviewActionDefaultVideo )
            {
            resourceId = R_DRMUTILITY_PREV_VIDEO_PLAY_LIST_QUERY;
            }
        }

    if ( iCoeEnv && resourceId )
        {
        CAknListQueryDialog* dlg = new( ELeave ) CAknListQueryDialog( &index );

        PrepareSecondaryDisplayL( *dlg, resourceId );
        answer = dlg->ExecuteLD( resourceId );
        CancelSecondaryDisplayL( resourceId );

        if ( answer )
            {
            buttonCode = index + 1;
            }
        }
    else if( resourceId )
        {
        DRM::CDrmUtilityGlobalNoteWrapper* noteWrapper(
            DRM::CDrmUtilityGlobalNoteWrapper::NewLC( iUtilityStringResourceReader ) );

        buttonCode = noteWrapper->ShowPreviewListQueryL( resourceId );

        CleanupStack::PopAndDestroy( noteWrapper );
        }

    return buttonCode;
    }

#else

EXPORT_C TInt DRM::CDrmUtilityUI::DisplayPopupWindowsForPreviewL(
    ContentAccess::CData& /*aContent*/,
    TInt /*aPreviewType*/,
    TInt /*aMediaType*/ )
    {
    return 0;
    }

#endif  // RD_DRM_PREVIEW_RIGHT_FOR_AUDIO

// -----------------------------------------------------------------------------
// CDrmUtilityUI::CreateNoteForResourceL
// -----------------------------------------------------------------------------
//
CAknResourceNoteDialog* DRM::CDrmUtilityUI::CreateNoteForResourceL(
    TInt aResId )
    {
    CAknResourceNoteDialog* resourceDialog( NULL );

    switch ( aResId )
        {
        case R_DRM_ERR_OPENING_FAIL_PERM:

            resourceDialog = new ( ELeave ) CAknErrorNote( ETrue );

            break;

        case R_DRM_WARN_NO_CONN_DEFINED:
        case R_DRM_WARN_INVALID_OR_NO_AP:

            resourceDialog = new  ( ELeave ) CAknWarningNote( ETrue );

            break;

        default:

            resourceDialog = new ( ELeave ) CAknInformationNote( ETrue );

            break;
        }

    return resourceDialog;
    }

// -----------------------------------------------------------------------------
// CDrmUtilityUI::GlobalNoteTypeForResource
// -----------------------------------------------------------------------------
///
TAknGlobalNoteType DRM::CDrmUtilityUI::GlobalNoteTypeForResource(
    TInt aResId )
    {
    TAknGlobalNoteType noteType( EAknGlobalInformationNote );

    switch ( aResId )
        {
        case R_DRM_ERR_OPENING_FAIL_PERM:

            noteType = EAknGlobalErrorNote;

            break;

        case R_DRM_WARN_NO_CONN_DEFINED:
        case R_DRM_WARN_INVALID_OR_NO_AP:

            noteType = EAknGlobalWarningNote;

            break;

        default:

            break;
        }

    return noteType;
    }

// -----------------------------------------------------------------------------
// CDrmUtilityUI::GetOmaStartTime
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmUtilityUI::GetOmaStartTime(
    CDRMConstraint* aConstraint,
    TTime& aStartTime)
    {
    __ASSERT_DEBUG( aConstraint, User::Panic( KDRMUtilityDebugPanicMessage,
                                              KDRMUtilityDebugPanicCode ) );
    TInt err( KErrNone );

    // Activated interval is always the same or later than
    // the start time of the start-end
    if ( aConstraint->iActiveConstraints & EConstraintInterval &&
        (aConstraint->iIntervalStart != Time::NullTTime() ) )
        {
        aStartTime = aConstraint->iIntervalStart;
        }
    else if ( aConstraint->iActiveConstraints & EConstraintStartTime )
        {
        aStartTime = aConstraint->iStartTime;
        }
    else
        {
        err = KErrCANoPermission;
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CDrmUtilityUI::EvaluatePreviewMediaType
// -----------------------------------------------------------------------------
//
#ifdef RD_DRM_PREVIEW_RIGHT_FOR_AUDIO

TInt DRM::CDrmUtilityUI::EvaluatePreviewMediaTypeL(
    ContentAccess::CData& aContent,
    TInt aPreviewType )
    {
    _LIT( KDRMUtilityMimeTypeVideo, "Video" );
    _LIT( KDRMUtilityMimeTypeRealMedia, "Application/x-pn-realmedia" );
    _LIT( KDRMUtilityMimeTypeVndRealMedia, "Application/vnd.rn-realmedia" );
    TInt mediaType( 0 );
    HBufC* mimeType( HBufC::NewLC( KMaxDataTypeLength ) );
    TPtr mimePtr( mimeType->Des() );

    User::LeaveIfError( aContent.GetStringAttribute( ContentAccess::EMimeType, mimePtr ) );

    if( mimePtr.FindF( KDRMUtilityMimeTypeVideo ) == KErrNotFound &&
        mimePtr.FindF( KDRMUtilityMimeTypeRealMedia ) == KErrNotFound &&
        mimePtr.FindF( KDRMUtilityMimeTypeVndRealMedia ) == KErrNotFound )
        {
        if ( aPreviewType == ContentAccess::EPreviewRights )
            {
            mediaType = DRM::EUPreviewRightsActionDefaultAudio;
            }
        else if ( aPreviewType == ContentAccess::EInstantPreview )
            {
            mediaType = DRM::EUEmbeddedPreviewActionDefaultAudio;
            }
        }
    else
        {
        if ( aPreviewType == ContentAccess::EPreviewRights )
            {
            mediaType = DRM::EUPreviewRightsActionDefaultVideo;
            }
        else if ( aPreviewType == ContentAccess::EInstantPreview )
            {
            mediaType = DRM::EUEmbeddedPreviewActionDefaultVideo;
            }
        }

    CleanupStack::PopAndDestroy( mimeType );
    return mediaType;
    }

#else

TInt DRM::CDrmUtilityUI::EvaluatePreviewMediaTypeL(
    ContentAccess::CData& /*aContent*/,
    TInt /*aPreviewType*/ )
    {
    return 0;
    }

#endif // RD_DRM_PREVIEW_RIGHT_FOR_AUDIO

// -----------------------------------------------------------------------------
// CDrmUtilityUI::PrepareSecondaryDisplayL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityUI::PrepareSecondaryDisplayL(
    CEikDialog& aDialog,
    TInt aResourceId,
    const TDesC& aString,
    TInt aValue )
    {
    // Check if we have cover display and shall we display something
    if ( !FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) ||
         !DRM::CDrmUtilityInfoNoteWrapper::EvaluateCoverResourceId(
                                                                aResourceId ) )
        {
        return;
        }

    RProcess myProcess;
    TUid myProcessUid( KNullUid );

    RThread().Process( myProcess );
    myProcessUid = myProcess.Identity();

    aDialog.PublishDialogL( aResourceId, KUidCoverUiCategoryDrmUtility );

    // fetch akn utility for mediator support
    CAknMediatorFacade* covercl( AknMediatorFacade( &aDialog ) );

    if ( covercl ) // returns null if cover display is not available
        {
        // Package dialog data
        // <oem/SecondaryDisplay/DrmUtilitySecondaryDisplay.h>
        TUtilitySDData utilityData;

        // First field is DrmUtility's Uid
        utilityData.iUtilityUid = KUidCoverUiCategoryDrmUtility;
        // ProcessId which uses DrmUtility
        utilityData.iHandlerProcessId = myProcessUid;
        if ( aString.Compare( KNullDesC ) )
            {
            // If there is filename given, it's always in the PrimaryString
            utilityData.iStringParam.Append( aString );
            }
        if ( aValue >= 0 )
            {
            // If there is no other than numeric data, put it as NumericString
            utilityData.iNumParam.AppendNum( aValue );
            }
        TUtilitySDDataPckg pckg( utilityData );

        covercl->BufStream() << pckg;   // Write the package data
        covercl->BufStream().CommitL(); // no more data to send so commit buf
        }
    }

// -----------------------------------------------------------------------------
// CDrmUtilityUI::CancelSecondaryDisplayL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityUI::CancelSecondaryDisplayL( TInt aResourceId )
    {

    // Check if we have cover display and shall we display something
    if( !FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) ||
        !DRM::CDrmUtilityInfoNoteWrapper::EvaluateCoverResourceId(
                                                                aResourceId ) )
        {
        return;
        }

    RProcess myProcess;
    TUid myProcessUid( KNullUid );

    RThread().Process( myProcess );
    myProcessUid = myProcess.Identity();

    TUtilitySDDataCancel cancelData;
    cancelData.iHandlerProcessId = myProcessUid;
    cancelData.iNoteId = aResourceId;

    TPckgBuf<TUtilitySDDataCancel> cancel( cancelData );

    HBufC8* buf(
        HBufC8::NewLC( sizeof( TUtilitySDDataCancel ) + sizeof( TInt ) ) );
    TPtr8 ptr( buf->Des() );

    RDesWriteStream stream( ptr );
    stream << cancel;
    stream.CommitL();

    iEventProvider->RaiseEvent( KMediatorSecondaryDisplayDomain,
                                KUidCoverUiCategoryDrmUtility,
                                ECover_dialog_cancel,
                                TVersion( 0, 0, 0 ),
                                *buf );

    stream.Close();
    CleanupStack::PopAndDestroy( buf );
    }

//  End of File
