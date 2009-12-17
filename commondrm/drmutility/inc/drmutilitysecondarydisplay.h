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
* Description:  The set of messages provided to Secondary Display software by
*              : DRMUtility
*
*/


#ifndef T_DRMUTILITYSECONDARYDISPLAY_H
#define T_DRMUTILITYSECONDARYDISPLAY_H

// INCLUDES
#include <e32base.h>

// The category UID for the messages in this header file.
// UID of the DrmUtilityCommon.Dll
const TUid KUidCoverUiCategoryDrmUtility = { 0x10283302 };

/*
* ==============================================================================
* Dialogs shown by DrmUtility. These messages are handled using the
* Secondary Display support in Avkon.
* ==============================================================================
*/
enum TSecondaryDisplayDRMUtilityDialogs
    {
    ECover_dialog_cancel = 1,
    ECover_confirmation_query,
    ECover_waiting_rights_confirmation_query,
    ECover_usage_rights_expired_buy_new,
    ECover_usage_rights_expired,
    ECover_no_usage_rights_buy_new,
    ECover_no_usage_rights,
    ECover_database_corrupted,
    ECover_set_automated,
    ECover_activate_and_set_automated,
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
    ECover_waiting_for_rights,
    ECover_rights_should_have_come,
    ECover_rights_should_have_come_no_ri,
    ECover_automated_sd_invalid_sim,
    ECover_automated_sd_expired,
    ECover_automated_cd_expired,
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
    ECover_no_usage_rights_connect_buy_new
};

/*
* ==============================================================================
* Parameter definitions for the messages in this file.
* ==============================================================================
*/

// Package definition for Matched Item
struct TUtilitySDData
    {
    TUid iUtilityUid;        // DrmUtilityCommon.dll process Id
    TUid iHandlerProcessId;  // Process Id which is using DrmUtilityCommon.dll
    TBuf<256> iStringParam;  // Used to transfer string note info
                             // (like server names, filenames etc..)
    TBuf<32> iNumParam;      // Used to transfer numeric note info data
                             // (like days, counts, time)
                                
    };    

typedef TPckgBuf<TUtilitySDData> TUtilitySDDataPckg;

// Package definition for cancel event
struct TUtilitySDDataCancel
    {    
    TUid iHandlerProcessId;     // Process Id which is using DrmUtilityCommon.dll
    TInt iNoteId;               // Dialog id of the cancelled event
    };

typedef TPckgBuf<TUtilitySDDataCancel> TUtilitySDDataPckgCancel;

#endif // T_DRMUTILITYSECONDARYDISPLAY_H
