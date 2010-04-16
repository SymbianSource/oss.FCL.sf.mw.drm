/*
* Copyright (c) 2004 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CDRMHelperInfoNoteWrapper class
*
*/



// INCLUDE FILES
#include "DRMHelperInfoNoteWrapper.h"

#include <StringLoader.h>
#include <AknGlobalConfirmationQuery.h>
#include <avkon.hrh> // EAknSoftkeyNo
#include <AknGlobalNote.h>

#include <SecondaryDisplay/DRMHelperSecondaryDisplay.h>
#include <AknMediatorFacade.h>
#include <featmgr.h>
#include <aknSDData.h>
#include <DRMHelper.h>
#include <Drmhelper.rsg>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMHelperInfoNoteWrapper::CDRMHelperInfoNoteWrapper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMHelperInfoNoteWrapper::CDRMHelperInfoNoteWrapper() : CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CDRMHelperInfoNoteWrapper::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDRMHelperInfoNoteWrapper::ConstructL()
    {
    iNote = CAknGlobalNote::NewL();
    }

// -----------------------------------------------------------------------------
// CDRMHelperInfoNoteWrapper::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMHelperInfoNoteWrapper* CDRMHelperInfoNoteWrapper::NewLC()
    {
    CDRMHelperInfoNoteWrapper* self = new (ELeave) CDRMHelperInfoNoteWrapper();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CDRMHelperInfoNoteWrapper* CDRMHelperInfoNoteWrapper::NewL()
    {
    CDRMHelperInfoNoteWrapper* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
CDRMHelperInfoNoteWrapper::~CDRMHelperInfoNoteWrapper()
    {
    Cancel(); // Cancel active object
    delete iNote;
    }


// -----------------------------------------------------------------------------
// CDRMHelperInfoNoteWrapper::ShowNoteL
// -----------------------------------------------------------------------------
//
void CDRMHelperInfoNoteWrapper::ShowNoteL( TAknGlobalNoteType aType ,
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
        CDRMHelperInfoNoteWrapper::EvaluateCoverResourceId( aResourceId ) )
        {
        RThread().Process( myProcess );
        myProcessUid = myProcess.Identity();

        THelperSDData helperData;
        helperData.iHelperUid = KUidCoverUiCategoryDRMHelper; // First field is DRMHelper's Uid
        helperData.iHandlerProcessId = myProcessUid; // ProcessId which uses DRMHelper
        if( aNoteText.Compare( KNullDesC ) )
            {
            // If there is filename given, it's always in the PrimaryString
            helperData.iStringParam.Append( aString );
            }
        if( aValue > -1 )
            {
            // If there is filename given, it's always in the PrimaryString
            helperData.iStringParam.Append( aValue );
            }

        THelperSDDataPckg pckg( helperData );
        CAknSDData* sd = CAknSDData::NewL( KUidCoverUiCategoryDRMHelper, aResourceId, pckg );
        iNote->SetSecondaryDisplayData( sd ); // ownership to notifier client
        }

    iNote->ShowNoteL( iStatus , aType , aNoteText );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CDRMHelperInfoNoteWrapper::DoCancel
// -----------------------------------------------------------------------------
//
void CDRMHelperInfoNoteWrapper::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelperInfoNoteWrapper::RunL
// -----------------------------------------------------------------------------
//
void CDRMHelperInfoNoteWrapper::RunL()
    {
    }

// ---------------------------------------------------------
// CDRMHelperInfoNoteWrapper::EvaluateCoverResourceId
// ---------------------------------------------------------
//
TBool CDRMHelperInfoNoteWrapper::EvaluateCoverResourceId( TInt& aResourceId )
    {
    switch ( aResourceId )
        {
        case R_DRMHELPER_CONFIRMATION_QUERY:
            {
            aResourceId = ECover_confirmation_query;
            }
            break;
        case R_DRMHELPER_WAITING_RIGHTS_CONFIRMATION_QUERY:
            {
            aResourceId = ECover_waiting_rights_confirmation_query;
            }
            break;
        case R_DRMHELPER_USAGE_RIGHTS_EXPIRED_BUY_NEW:
            {
            aResourceId = ECover_usage_rights_expired_buy_new;
            }
            break;
        case R_DRMHELPER_USAGE_RIGHTS_EXPIRED:
            {
            aResourceId = ECover_usage_rights_expired;
            }
            break;
        case R_DRMHELPER_NO_USAGE_RIGHTS_BUY_NEW:
            {
            aResourceId = ECover_no_usage_rights_buy_new;
            }
            break;
        case R_DRMHELPER_NO_USAGE_RIGHTS:
            {
            aResourceId = ECover_no_usage_rights;
            }
            break;
#ifndef RD_DRM_REMOVAL_OF_EXPIRATION_WARNING_PROMPTS
        case R_DRMHELPER_USAGE_RIGHTS_TO_EXPIRE:
            {
            aResourceId = ECover_usage_rights_to_expire;
            }
            break;
#endif
        case R_DRMHELPER_SET_AUTOMATED:
            {
            aResourceId = ECover_set_automated;
            }
            break;
        case R_DRMHELPER_ACTIVATE_AND_SET_AUTOMATED:
            {
            aResourceId = ECover_activate_and_set_automated;
            }
            break;
        case R_DRMHELPER_CAL_ALERT_INVALID_SIM_BUY_NEW:
            {
            aResourceId = ECover_cal_alert_invalid_sim_buy_new;
            }
            break;
        case R_DRMHELPER_CAL_ALERT_EXPIRED_BUY_NEW:
            {
            aResourceId = ECover_cal_alert_expired_buy_new;
            }
            break;
        case R_DRMHELPER_CAL_ALERT_EXPIRED:
            {
            aResourceId = ECover_cal_alert_expired;
            }
            break;
        case R_DRMHELPER_CLOCK_ALARM_INVALID_SIM_BUY_NEW:
            {
            aResourceId = ECover_clock_alarm_invalid_sim_buy_new;
            }
            break;
        case R_DRMHELPER_CLOCK_ALARM_EXPIRED_BUY_NEW:
            {
            aResourceId = ECover_clock_alarm_expired_buy_new;
            }
            break;
        case R_DRMHELPER_CLOCK_ALARM_EXPIRED:
            {
            aResourceId = ECover_clock_alarm_expired;
            }
            break;
        case R_DRMHELPER_EMAIL_TONE_EXPIRED_BUY_NEW:
            {
            aResourceId = ECover_email_tone_expired_buy_new;
            }
            break;
        case R_DRMHELPER_EMAIL_TONE_INVALID_SIM_BUY_NEW:
            {
            aResourceId = ECover_email_tone_invalid_sim_buy_new;
            }
            break;
        case R_DRMHELPER_EMAIL_TONE_EXPIRED:
            {
            aResourceId = ECover_email_tone_expired;
            }
            break;
        case R_DRMHELPER_IM_TONE_EXPIRED_BUY_NEW:
            {
            aResourceId = ECover_im_tone_expired_buy_new;
            }
            break;
        case R_DRMHELPER_IM_TONE_EXPIRED:
            {
            aResourceId = ECover_im_tone_expired;
            }
            break;
        case R_DRMHELPER_MESSAGE_TONE_INVALID_SIM_BUY_NEW:
            {
            aResourceId = ECover_message_tone_invalid_sim_buy_new;
            }
            break;
        case R_DRMHELPER_MESSAGE_TONE_EXPIRED_BUY_NEW:
            {
            aResourceId = ECover_message_tone_expired_buy_new;
            }
            break;
        case R_DRMHELPER_MESSAGE_TONE_EXPIRED:
            {
            aResourceId = ECover_message_tone_expired;
            }
            break;
        case R_DRMHELPER_RINGING_TONE_EXPIRED_BUY_NEW:
            {
            aResourceId = ECover_ringing_tone_expired_buy_new;
            }
            break;
        case R_DRMHELPER_RINGING_TONE_INVALID_SIM_BUY_NEW:
            {
            aResourceId = ECover_ringing_tone_invalid_sim_buy_new;
            }
            break;
        case R_DRMHELPER_RINGING_TONE_EXPIRED:
            {
            aResourceId = ECover_ringing_tone_expired;
            }
            break;
        case R_DRMHELPER_SCREENSAVER_INVALID_SIM_BUY_NEW:
            {
            aResourceId = ECover_screensaver_invalid_sim_buy_new;
            }
            break;
        case R_DRMHELPER_SCREENSAVER_EXPIRED_BUY_NEW:
            {
            aResourceId = ECover_screensaver_expired_buy_new;
            }
            break;
        case R_DRMHELPER_SCREENSAVER_EXPIRED:
            {
            aResourceId = ECover_screensaver_expired;
            }
            break;
        case R_DRMHELPER_WALLPAPER_INVALID_SIM_BUY_NEW:
            {
            aResourceId = ECover_wallpaper_invalid_sim_buy_new;
            }
            break;
        case R_DRMHELPER_WALLPAPER_EXPIRED_BUY_NEW:
            {
            aResourceId = ECover_wallpaper_expired_buy_new;
            }
            break;
        case R_DRMHELPER_WALLPAPER_EXPIRED:
            {
            aResourceId = ECover_wallpaper_expired;
            }
            break;
        case R_DRMHELPER_THEME_INVALID_SIM_BUY_NEW:
            {
            aResourceId = ECover_theme_invalid_sim_buy_new;
            }
            break;
        case R_DRMHELPER_THEME_EXPIRED_BUY_NEW:
            {
            aResourceId = ECover_theme_expired_buy_new;
            }
            break;
        case R_DRMHELPER_THEME_EXPIRED:
            {
            aResourceId = ECover_theme_expired;
            }
            break;
        case R_DRMHELPER_USAGE_RIGHTS_IN_FUTURE:
            {
            aResourceId = ECover_usage_rights_in_future;
            }
            break;
        case R_DRMHELPER_ACTIVATE_AFTER_PREVIEW:
            {
            aResourceId = ECover_activate_after_preview;
            }
            break;
        case R_DRMHELPER_WAITING_FOR_RIGHTS:
            {
            aResourceId = ECover_waiting_for_rights;
            }
            break;
        case R_DRMHELPER_RIGHTS_SHOULD_HAVE_COME:
            {
            aResourceId = ECover_rights_should_have_come;
            }
            break;
        case R_DRMHELPER_RIGHTS_SHOULD_HAVE_COME_NO_RI:
            {
            aResourceId = ECover_rights_should_have_come_no_ri;
            }
            break;
        case R_DRMHELPER_AUTOMATED_SD_INVALID_SIM:
            {
            aResourceId = ECover_automated_sd_invalid_sim;
            }
            break;
        case R_DRMHELPER_AUTOMATED_SD_EXPIRED:
            {
            aResourceId = ECover_automated_sd_expired;
            }
            break;
        case R_DRMHELPER_AUTOMATED_CD_EXPIRED:
            {
            aResourceId = ECover_automated_cd_expired;
            }
            break;
#ifndef RD_DRM_REMOVAL_OF_EXPIRATION_WARNING_PROMPTS
        case R_DRMHELPER_INFO_EXP_1_USE:
            {
            aResourceId = ECover_info_exp_1_use;
            }
            break;
        case R_DRMHELPER_INFO_EXP_N_USES:
            {
            aResourceId = ECover_info_exp_n_uses;
            }
            break;
        case R_DRMHELPER_INFO_EXP_1_DAY:
            {
            aResourceId = ECover_info_exp_1_day;
            }
            break;
        case R_DRMHELPER_INFO_EXP_N_DAYS:
            {
            aResourceId = ECover_info_exp_n_days;
            }
            break;
        case R_DRMHELPER_QUERY_EXP_1_USE_GET_NEW:
            {
            aResourceId = ECover_query_exp_1_use_get_new;
            }
            break;
        case R_DRMHELPER_QUERY_EXP_N_USE_GET_NEW:
            {
            aResourceId = ECover_query_exp_n_use_get_new;
            }
            break;
        case R_DRMHELPER_QUERY_EXP_1_DAY_GET_NEW:
            {
            aResourceId = ECover_query_exp_1_day_get_new;
            }
            break;
        case R_DRMHELPER_QUERY_EXP_N_DAY_GET_NEW:
            {
            aResourceId = ECover_query_exp_n_day_get_new;
            }
            break;
        case R_DRMHELPER_QUERY_EXP_1_MIN_GET_NEW:
            {
            aResourceId = ECover_query_exp_1_min_get_new;
            }
            break;
        case R_DRMHELPER_QUERY_EXP_N_MIN_GET_NEW:
            {
            aResourceId = ECover_query_exp_n_min_get_new;
            }
            break;
        case R_DRMHELPER_QUERY_EXP_MANY_CONSTR:
            {
            aResourceId = ECover_query_exp_many_constr;
            }
            break;
#endif //RD_DRM_REMOVAL_OF_EXPIRATION_WARNING_PROMPTS
#ifdef RD_DRM_PREVIEW_RIGHT_FOR_AUDIO
        case R_DRMHELPER_PREV_AUDIO_GET_LIST_QUERY:
            {
            aResourceId = ECover_prev_audio_get_list_query;
            }
            break;
        case R_DRMHELPER_PREV_VIDEO_GET_LIST_QUERY:
            {
            aResourceId = ECover_prev_video_get_list_query;
            }
            break;
        case R_DRMHELPER_PREV_AUDIO_PLAY_LIST_QUERY:
            {
            aResourceId = ECover_prev_audio_play_list_query;
            }
            break;
        case R_DRMHELPER_PREV_VIDEO_PLAY_LIST_QUERY:
            {
            aResourceId = ECover_prev_video_play_list_query;
            }
            break;
        case R_DRMHELPER_AUDIO_PREV_GET_LIST:
            {
            aResourceId = ECover_audio_prev_get_list;
            }
            break;
        case R_DRMHELPER_VIDEO_PREV_GET_LIST:
            {
            aResourceId = ECover_video_prev_get_list;
            }
            break;
#endif //RD_DRM_PREVIEW_RIGHT_FOR_AUDIO
        case R_DRMHELPER_INVALID_SIM:
            {
            aResourceId = ECover_invalid_sim;
            }
            break;
        case R_DRMHELPER_AUDIO_PREV_PLAY_LIST:
            {
            aResourceId = ECover_audio_prev_play_list;
            }
            break;
        case R_DRMHELPER_VIDEO_PREV_PLAY_LIST:
            {
            aResourceId = ECover_video_prev_play_list;
            }
            break;
        case R_DRM_ERR_OPENING_FAIL_PERM:
            {
            aResourceId = ECover_err_opening_fail_perm;
            }
            break;
        case R_DRM_WARN_NO_CONN_DEFINED:
            {
            aResourceId = ECover_warn_no_conn_defined;
            }
            break;
        case R_DRM_WARN_INVALID_OR_NO_AP:
            {
            aResourceId = ECover_warn_invalid_or_no_ap;
            }
            break;
        default:
            return EFalse; // No proper dialog found, skipping
        }
    return ETrue; // Dialog found
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
