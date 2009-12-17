/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The set of messages provided to Secondary Display software by
*              : DRMHelper.
*
*/

#ifndef DRMHELPER_SECONDARY_DISPLAY_H
#define DRMHELPER_SECONDARY_DISPLAY_H

// INCLUDES
#include <e32base.h>

// The category UID for the messages in this header file.
//
const TUid KUidCoverUiCategoryDRMHelper = { 0x101F8647 };   // UID of the DRMHelper Dll


/*
* ==============================================================================
* Dialogs shown by DRMHelper. These messages are handled using the
* Secondary Display support in Avkon.
* ==============================================================================
*/
enum TSecondaryDisplayDRMHelperDialogs
    {
    ECover_confirmation_query = 1,
    ECover_waiting_rights_confirmation_query,
    ECover_usage_rights_expired_buy_new,
    ECover_usage_rights_expired,
    ECover_no_usage_rights_buy_new,
    ECover_no_usage_rights,
    ECover_database_corrupted,
    ECover_usage_rights_to_expire_1_count,
    ECover_usage_rights_to_expire_n_counts,
    ECover_usage_rights_to_expire_1_day,
    ECover_usage_rights_to_expire_n_days,
    ECover_usage_rights_to_expire,
    ECover_set_automated,
    ECover_activate_and_set_automated,
    ECover_cal_alert_to_expire_1_day,
    ECover_cal_alert_to_expire_n_days,
    ECover_clock_alarm_to_expire_1_day,
    ECover_clock_alarm_to_expire_n_days,
    ECover_email_tone_to_expire_1_day,
    ECover_email_tone_to_expire_n_days,
    ECover_im_tone_to_expire_1_day,
    ECover_im_tone_to_expire_n_days,
    ECover_message_tone_to_expire_1_day,
    ECover_message_tone_to_expire_n_days,
    ECover_ringing_tone_to_expire_1_day,
    ECover_ringing_tone_to_expire_n_days,
    ECover_screensaver_to_expire_1_day,
    ECover_screensaver_to_expire_n_days,
    ECover_wallpaper_to_expire_1_day,
    ECover_wallpaper_to_expire_n_days,
    ECover_theme_to_expire_1_day,
    ECover_theme_to_expire_n_days,
    ECover_cal_alert_invalid_sim_buy_new,
    ECover_cal_alert_expired_buy_new,
    ECover_cal_alert_expired,
    ECover_clock_alarm_invalid_sim_buy_new,
    ECover_clock_alarm_expired_buy_new,
    ECover_clock_alarm_expired,
    ECover_email_tone_expired_buy_new,
    ECover_email_tone_invalid_sim_buy_new,
    ECover_email_tone_expired,
    ECover_im_tone_expired_buy_new,
    ECover_im_tone_expired,
    ECover_message_tone_invalid_sim_buy_new,
    ECover_message_tone_expired_buy_new,
    ECover_message_tone_expired,
    ECover_ringing_tone_expired_buy_new,
    ECover_ringing_tone_invalid_sim_buy_new,
    ECover_ringing_tone_expired,
    ECover_screensaver_invalid_sim_buy_new,
    ECover_screensaver_expired_buy_new,
    ECover_screensaver_expired,
    ECover_wallpaper_invalid_sim_buy_new,
    ECover_wallpaper_expired_buy_new,
    ECover_wallpaper_expired,
    ECover_theme_invalid_sim_buy_new,
    ECover_theme_expired_buy_new,
    ECover_theme_expired,
    ECover_usage_rights_in_future,
    ECover_activate_after_preview,
    ECover_usage_rights_to_expire_1_minute,
    ECover_usage_rights_to_expire_n_minutes,
    ECover_automated_to_expire_1_day,
    ECover_automated_to_expire_n_days,
    ECover_waiting_for_rights,
    ECover_rights_should_have_come,
    ECover_rights_should_have_come_no_ri,
    ECover_automated_sd_invalid_sim,
    ECover_automated_sd_expired,
    ECover_automated_cd_expired,
    ECover_info_exp_1_use,
    ECover_info_exp_n_uses,
    ECover_info_exp_1_day,
    ECover_info_exp_n_days,
    ECover_query_exp_1_use_get_new,
    ECover_query_exp_n_use_get_new,
    ECover_query_exp_1_day_get_new,
    ECover_query_exp_n_day_get_new,
    ECover_query_exp_1_min_get_new,
    ECover_query_exp_n_min_get_new,
    ECover_query_exp_many_constr,
    ECover_prev_audio_get_list_query,
    ECover_prev_video_get_list_query,
    ECover_prev_audio_play_list_query,
    ECover_prev_video_play_list_query,
    ECover_invalid_sim,

    ECover_audio_prev_get_list,
    ECover_video_prev_get_list,
    ECover_audio_prev_play_list,
    ECover_video_prev_play_list,
    ECover_r_silent_wait_note,
    ECover_usage_rights_expired_connect_buy_new,
    ECover_no_usage_rights_connect_buy_new,
    ECover_dialog_cancel,
    ECover_warn_no_conn_defined,
    ECover_err_no_coverage,
    ECover_warn_invalid_or_no_ap,
    ECover_err_opening_fail_perm,
    ECover_gen_rights_exp,
    ECover_no_rights_for_some
};

/*
* ==============================================================================
* Parameter definitions for the messages in this file.
* ==============================================================================
*/

// Package definition for Matched Item
struct THelperSDData
    {
    TUid iHelperUid;            // drmhelper.dll process Id {0x101F8647}
    TUid iHandlerProcessId;     // Process Id which is using drmhelper.dll
    TUid iCreatorProcessId;     // Process Id of creator of process
                                // using drmhelper.dll
    TBuf<256> iStringParam;     // Used to transfer string note info
                                // (like server names, filenames etc..)
    TBuf<32> iNumParam;         // Used to transfer numeric note info data
                                // (like days, counts, time)

    };

typedef TPckgBuf<THelperSDData> THelperSDDataPckg;


// Package definition for cancel event
struct THelperSDDataCancel
    {
    TUid iHandlerProcessId;     // Process Id which is using drmhelper.dll
    TUid iCreatorProcessId;     // Process Id of creator of process
                                // using drmhelper.dll
    TInt iNoteId;               // Dialog id of the cancelled event
    };

typedef TPckgBuf<THelperSDDataCancel> THelperSDDataPckgCancel;


#endif      // DRMHELPER_SECONDARY_DISPLAY_H
// End of File
