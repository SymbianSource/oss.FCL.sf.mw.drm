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
* Description:  Implementation of CDrmUtilityInfoNoteWrapper class
*
*/


// INCLUDE FILES
#include <StringLoader.h>
#include <AknGlobalConfirmationQuery.h>
#include <avkon.hrh> // EAknSoftkeyNo
#include <AknGlobalNote.h>

#include <AknMediatorFacade.h>
#include <featmgr.h>
#include <aknSDData.h>
#include <drmutility.rsg>

#include "drmutilitysecondarydisplay.h"
#include "DrmUtilityInfoNoteWrapper.h"

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDrmUtilityInfoNoteWrapper::CDrmUtilityInfoNoteWrapper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
DRM::CDrmUtilityInfoNoteWrapper::CDrmUtilityInfoNoteWrapper()
    : CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CDrmUtilityInfoNoteWrapper::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityInfoNoteWrapper::ConstructL()
    {
    iNote = CAknGlobalNote::NewL();
    }

// -----------------------------------------------------------------------------
// CDrmUtilityInfoNoteWrapper::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
DRM::CDrmUtilityInfoNoteWrapper* DRM::CDrmUtilityInfoNoteWrapper::NewLC()
    {
    DRM::CDrmUtilityInfoNoteWrapper* self(
                                new (ELeave) CDrmUtilityInfoNoteWrapper() );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CDrmUtilityInfoNoteWrapper::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
DRM::CDrmUtilityInfoNoteWrapper* DRM::CDrmUtilityInfoNoteWrapper::NewL()
    {
    DRM::CDrmUtilityInfoNoteWrapper* self( NewLC() );
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
DRM::CDrmUtilityInfoNoteWrapper::~CDrmUtilityInfoNoteWrapper()
    {
    Cancel(); // Cancel active object
    delete iNote;
    }


// -----------------------------------------------------------------------------
// CDrmUtilityInfoNoteWrapper::ShowNoteL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityInfoNoteWrapper::ShowNoteL( TAknGlobalNoteType aType ,
                                                 const TDesC& aNoteText,
                                                 TInt aResourceId,
                                                 const TDesC& aString,
                                                 TInt aValue )
    {
    RProcess myProcess;
    TUid myProcessUid( KNullUid );

    // Convert primary display resource ID to Cover Ui
    // do nothing if not found
    if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) &&
         EvaluateCoverResourceId( aResourceId ) )
        {
        RThread().Process( myProcess );
        myProcessUid = myProcess.Identity();

        TUtilitySDData utilityData;
        // First field is DrmUtility's Uid
        utilityData.iUtilityUid = KUidCoverUiCategoryDrmUtility;
        // ProcessId which uses DrmUtility
        utilityData.iHandlerProcessId = myProcessUid;
        if ( aNoteText.Compare( KNullDesC ) )
            {
            // If there is filename given, it's always in the PrimaryString
            utilityData.iStringParam.Append( aString );
            }
        if ( aValue > -1 )
            {
            // If there is filename given, it's always in the PrimaryString
            utilityData.iStringParam.Append( aValue );
            }

        TUtilitySDDataPckg pckg( utilityData );
        CAknSDData* sd( CAknSDData::NewL( KUidCoverUiCategoryDrmUtility,
                                          aResourceId,
                                          pckg ) );
        iNote->SetSecondaryDisplayData( sd ); // ownership to notifier client
        }

    iNote->ShowNoteL( iStatus , aType , aNoteText );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CDrmUtilityInfoNoteWrapper::DoCancel
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityInfoNoteWrapper::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CDrmUtilityInfoNoteWrapper::RunL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityInfoNoteWrapper::RunL()
    {
    }

// ---------------------------------------------------------
// CDrmUtilityInfoNoteWrapper::EvaluateCoverResourceId
// ---------------------------------------------------------
//
TBool DRM::CDrmUtilityInfoNoteWrapper::EvaluateCoverResourceId(
    TInt& aResourceId )
    {
    switch ( aResourceId )
        {
        case R_DRMUTILITY_CONFIRMATION_QUERY:
            {
            aResourceId = ECover_confirmation_query;
            }
            break;
        case R_DRMUTILITY_WAITING_RIGHTS_CONFIRMATION_QUERY:
            {
            aResourceId = ECover_waiting_rights_confirmation_query;
            }
            break;
        case R_DRMUTILITY_INVALID_SIM:
            {
            aResourceId = ECover_invalid_sim;
            }
            break;
        /*
        case R_DRMUTILITY_USAGE_RIGHTS_EXPIRED_BUY_NEW:
            {
            aResourceId = ECover_usage_rights_expired_buy_new;
            }
            break;
        case R_DRMUTILITY_USAGE_RIGHTS_EXPIRED:
            {
            aResourceId = ECover_usage_rights_expired;
            }
            break;
        case R_DRMUTILITY_NO_USAGE_RIGHTS_BUY_NEW:
            {
            aResourceId = ECover_no_usage_rights_buy_new;
            }
            break;
        case R_DRMUTILITY_NO_USAGE_RIGHTS:
            {
            aResourceId = ECover_no_usage_rights;
            }
            break;
        case R_DRMUTILITY_DATABASE_CORRUPTED:
            {
            aResourceId = ECover_database_corrupted;
            }
            break;
        case R_DRMUTILITY_SET_AUTOMATED:
            {
            aResourceId = ECover_set_automated;
            }
            break;
        case R_DRMUTILITY_ACTIVATE_AND_SET_AUTOMATED:
            {
            aResourceId = ECover_activate_and_set_automated;
            }
            break;
        case R_DRMUTILITY_CAL_ALERT_INVALID_SIM_BUY_NEW:
            {
            aResourceId = ECover_cal_alert_invalid_sim_buy_new;
            }
            break;
        case R_DRMUTILITY_CAL_ALERT_EXPIRED_BUY_NEW:
            {
            aResourceId = ECover_cal_alert_expired_buy_new;
            }
            break;
        case R_DRMUTILITY_CAL_ALERT_EXPIRED:
            {
            aResourceId = ECover_cal_alert_expired;
            }
            break;
        case R_DRMUTILITY_CLOCK_ALARM_INVALID_SIM_BUY_NEW:
            {
            aResourceId = ECover_clock_alarm_invalid_sim_buy_new;
            }
            break;
        case R_DRMUTILITY_CLOCK_ALARM_EXPIRED_BUY_NEW:
            {
            aResourceId = ECover_clock_alarm_expired_buy_new;
            }
            break;
        case R_DRMUTILITY_CLOCK_ALARM_EXPIRED:
            {
            aResourceId = ECover_clock_alarm_expired;
            }
            break;
        case R_DRMUTILITY_EMAIL_TONE_EXPIRED_BUY_NEW:
            {
            aResourceId = ECover_email_tone_expired_buy_new;
            }
            break;
        case R_DRMUTILITY_EMAIL_TONE_INVALID_SIM_BUY_NEW:
            {
            aResourceId = ECover_email_tone_invalid_sim_buy_new;
            }
            break;
        case R_DRMUTILITY_EMAIL_TONE_EXPIRED:
            {
            aResourceId = ECover_email_tone_expired;
            }
            break;
        case R_DRMUTILITY_IM_TONE_EXPIRED_BUY_NEW:
            {
            aResourceId = ECover_im_tone_expired_buy_new;
            }
            break;
        case R_DRMUTILITY_IM_TONE_EXPIRED:
            {
            aResourceId = ECover_im_tone_expired;
            }
            break;
        case R_DRMUTILITY_MESSAGE_TONE_INVALID_SIM_BUY_NEW:
            {
            aResourceId = ECover_message_tone_invalid_sim_buy_new;
            }
            break;
        case R_DRMUTILITY_MESSAGE_TONE_EXPIRED_BUY_NEW:
            {
            aResourceId = ECover_message_tone_expired_buy_new;
            }
            break;
        case R_DRMUTILITY_MESSAGE_TONE_EXPIRED:
            {
            aResourceId = ECover_message_tone_expired;
            }
            break;
        case R_DRMUTILITY_RINGING_TONE_EXPIRED_BUY_NEW:
            {
            aResourceId = ECover_ringing_tone_expired_buy_new;
            }
            break;
        case R_DRMUTILITY_RINGING_TONE_INVALID_SIM_BUY_NEW:
            {
            aResourceId = ECover_ringing_tone_invalid_sim_buy_new;
            }
            break;
        case R_DRMUTILITY_RINGING_TONE_EXPIRED:
            {
            aResourceId = ECover_ringing_tone_expired;
            }
            break;
        case R_DRMUTILITY_SCREENSAVER_INVALID_SIM_BUY_NEW:
            {
            aResourceId = ECover_screensaver_invalid_sim_buy_new;
            }
            break;
        case R_DRMUTILITY_SCREENSAVER_EXPIRED_BUY_NEW:
            {
            aResourceId = ECover_screensaver_expired_buy_new;
            }
            break;
        case R_DRMUTILITY_SCREENSAVER_EXPIRED:
            {
            aResourceId = ECover_screensaver_expired;
            }
            break;
        case R_DRMUTILITY_WALLPAPER_INVALID_SIM_BUY_NEW:
            {
            aResourceId = ECover_wallpaper_invalid_sim_buy_new;
            }
            break;
        case R_DRMUTILITY_WALLPAPER_EXPIRED_BUY_NEW:
            {
            aResourceId = ECover_wallpaper_expired_buy_new;
            }
            break;
        case R_DRMUTILITY_WALLPAPER_EXPIRED:
            {
            aResourceId = ECover_wallpaper_expired;
            }
            break;
        case R_DRMUTILITY_THEME_INVALID_SIM_BUY_NEW:
            {
            aResourceId = ECover_theme_invalid_sim_buy_new;
            }
            break;
        case R_DRMUTILITY_THEME_EXPIRED_BUY_NEW:
            {
            aResourceId = ECover_theme_expired_buy_new;
            }
            break;
        case R_DRMUTILITY_THEME_EXPIRED:
            {
            aResourceId = ECover_theme_expired;
            }
            break;
        case R_DRMUTILITY_USAGE_RIGHTS_IN_FUTURE:
            {
            aResourceId = ECover_usage_rights_in_future;
            }
            break;
        case R_DRMUTILITY_ACTIVATE_AFTER_PREVIEW:
            {
            aResourceId = ECover_activate_after_preview;
            }
            break;
        case R_DRMUTILITY_WAITING_FOR_RIGHTS:
            {
            aResourceId = ECover_waiting_for_rights;
            }
            break;
        case R_DRMUTILITY_RIGHTS_SHOULD_HAVE_COME:
            {
            aResourceId = ECover_rights_should_have_come;
            }
            break;
        case R_DRMUTILITY_RIGHTS_SHOULD_HAVE_COME_NO_RI:
            {
            aResourceId = ECover_rights_should_have_come_no_ri;
            }
            break;
        case R_DRMUTILITY_AUTOMATED_SD_INVALID_SIM:
            {
            aResourceId = ECover_automated_sd_invalid_sim;
            }
            break;
        case R_DRMUTILITY_AUTOMATED_SD_EXPIRED:
            {
            aResourceId = ECover_automated_sd_expired;
            }
            break;
        case R_DRMUTILITY_AUTOMATED_CD_EXPIRED:
            {
            aResourceId = ECover_automated_cd_expired;
            }
            break;
        */

#ifdef RD_DRM_PREVIEW_RIGHT_FOR_AUDIO

        case R_DRMUTILITY_PREV_AUDIO_GET_LIST_QUERY:
            {
            aResourceId = ECover_prev_audio_get_list_query;
            }
            break;
        case R_DRMUTILITY_PREV_VIDEO_GET_LIST_QUERY:
            {
            aResourceId = ECover_prev_video_get_list_query;
            }
            break;
        case R_DRMUTILITY_PREV_AUDIO_PLAY_LIST_QUERY:
            {
            aResourceId = ECover_prev_audio_play_list_query;
            }
            break;
        case R_DRMUTILITY_PREV_VIDEO_PLAY_LIST_QUERY:
            {
            aResourceId = ECover_prev_video_play_list_query;
            }
            break;
        case R_DRMUTILITY_AUDIO_PREV_GET_LIST:
            {
            aResourceId = ECover_audio_prev_get_list;
            }
            break;
        case R_DRMUTILITY_VIDEO_PREV_GET_LIST:
            {
            aResourceId = ECover_video_prev_get_list;
            }
            break;
        case R_DRMUTILITY_AUDIO_PREV_PLAY_LIST:
            {
            aResourceId = ECover_audio_prev_play_list;
            }
            break;
        case R_DRMUTILITY_VIDEO_PREV_PLAY_LIST:
            {
            aResourceId = ECover_video_prev_play_list;
            }
            break;

#endif // RD_DRM_PREVIEW_RIGHT_FOR_AUDIO

        default:
            return EFalse; // No proper dialog found, skipping
        }
    return ETrue; // Dialog found
    }

//  End of File
