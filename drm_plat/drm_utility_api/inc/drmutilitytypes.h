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
* Description:  Enumerations and constants used in the DRM Utility
 *
*/


#ifndef DRMUTILITYTYPES_H
#define DRMUTILITYTYPES_H

namespace DRM
    {

    /*** Names:

      EAU = Automated usage
      EUH = UI handling
      ERI = Rights Info
      EU  = Utility / Common

     ***/


    /** Automated usage types */
    enum TDrmAutomatedType
        {
        //This is a new type
        EAUAutomatedTypeVideoRingingTone,
        //These are from TDRMHelperAutomatedType
        EAUAutomatedTypeRingingTone,
        EAUAutomatedTypeMessageAlert,
        EAUAutomatedTypeEmailAlert,
        EAUAutomatedTypeIMAlert,
        EAUAutomatedTypeCalendarAlarm,
        EAUAutomatedTypeScreenSaver,
        EAUAutomatedTypeWallpaper,
        EAUAutomatedTypeTheme,
        EAUAutomatedTypeClockAlarm,
        EAUAutomatedTypeOther = KMaxTUint8
        };


    /**
     * Automated service types
     *     EAUActive   registering application listens to expiry notifications
     *     EAUPassive  registering application does not listen to expiry
     *                 notifications
     */
    enum TDrmAutomatedServiceType
        {
        EAUPassive = 0,
        EAUActive
        };



    /**  Automated usage events */
    enum TAutomatedUsageEvent
        {
        EAUAutomatedContentSetOK = 0,             // Content was successfully
                                                  // set as automated
        EAUAutomatedContentExpired = 1,           // Automated content was
                                                  // expired, remove from use
        EAUAutomatedContentIdleAfterExpired = 2,  // idle was activated, remove
                                                  // from automated usage.
        EAUxxx = 3
        };

    /**  Return code to indicate how to proceed */
    enum TAutomatedUsageAction
        {
        EAUActionIgnore = KErrNotFound, // Nothing to be done.
        EAUActionDefault = 0,           // DRM Utility performs default
                                        // handling
        EAUActionRightsAquisition = 1,  // Purchase new rights for the
                                        // automated content
        EAUActionClientHandles = 2      // client takes care of everything.
        };


    /**
     * Return code to indicate how to proceed when embedded preview is
     * available
     */
    enum TEmbeddedPreviewAction
        {
        EUEmbeddedPreviewActionDefault,      // < DRM Utility asks if user wants
                                             //   to play preview or acquire new
                                             //   rights
        EUEmbeddedPreviewActionDefaultAudio, // < DRM Utility asks if user wants
                                             //   to play preview or acquire new
                                             //   rights for audio
        EUEmbeddedPreviewActionDefaultVideo, // < DRM Utility asks if user wants
                                             //   to play preview or acquire new
                                             //   rights for video
        EUEmbeddedPreviewActionFetchRights,  // < DRM Utility acquires new
                                             //   rights, no query shown
        EUEmbeddedPreviewActionPlayPreview,  // < Observer takes care of playing
                                             //   preview
        EUEmbeddedPreviewActionIgnore        // < Nothing to be done.
        };

    /**
     * Return code to indicate how to proceed when preview rights are
     * available
     */
    enum TPreviewRightsAction
        {
        EUPreviewRightsActionDefault,        // < DRM Utility asks if user wants
                                             //   to acquire preview rights or
                                             //   normal rights and acquires them
        EUPreviewRightsActionDefaultAudio,   // < DRM Utility asks if user wants
                                             //   to acquire preview rights or
                                             //   normal rights for audio and 
                                             //   acquires them
        EUPreviewRightsActionDefaultVideo,   // < DRM Utility asks if user wants
                                             //   to acquire preview rights or
                                             //   normal rights for video and
                                             //   acquires them
        EUPreviewRightsActionFetchRights,    // < DRM Utility acquires normal
                                             //   rights, no query shown
        EUPreviewRightsActionFetchPreviewRights,  // < Observer acquires
                                                  //   preview rights
        EUPreviewRightsActionIgnore          // < Nothing to be done
        };

    /**
     * Return code to indicate how to proceed when silent rights are
     * available
     */
    enum TSilentRightsAction
        {
        EUSilentRightsActionDefault,           // < DRM Utility performs the
                                               //   default silent rights 
                                               //   handling
        EUSilentRightsActionFetchRights,       // < DRM Utility acquires normal
                                               //   rights
        EUSilentRightsActionFetchSilentRights, // < Observer acquires silent
                                               //   rights
        EUSilentRightsActionIgnore             // < Nothing to be done
        };

    /**
     * Return code to indicate how to proceed when rights are
     * available
     */
    enum TRightsAction
        {
        EURightsActionDefault,       // < DRM Utility performs the default
                                     //   rights handling
        EURightsActionFetchRights,   // < DRM Utility acquires normal rights
        EURightsActionIgnore         // < Nothing to be done
        };

    /**
     * Information about rights status
     */
    enum TDrmRightsInfo
        {
        EURightsInfoValid = 0,
        EURightsInfoExpired = 1,
        EURightsInfoMissing = 2,
        EURightsInfoFuture = 3,
        };



    /**  Return code to indicate how to proceed */
    enum TCheckRightsAction
        {
        EUHCheckRightsActionDefault,          // < DRM Utility performs the
                                              //   default handling
        EUHCheckRightsActionOpenDetailsView,  // < Details view is opened, no
                                              //   notes shown by DRM Utility
        EUHCheckRightsActionAcquireNewRights, // < New rights are acquired, no
                                              //   notes shown by DRM Utility
        EUHCheckRightsActionAcquirePreviewRights, // < Preview rights are
                                                  //   acquired, no notes shown 
                                                  //   by DRM Utility
        EUHCheckRightsActionIgnore            // < DRM Utility does not do
                                              //   anything and stops the
                                              //   operation
        };

    /**  Reason why rights are not valid */
    enum TCheckRightsStatus
        {
        EUHCheckRightsExpiredRights,  // < Rights have been expired
        EUHCheckRightsNoRights,       // < No rights at all
        EUHCheckRightsFutureRights    // < Rights will become valid in future
        };


    /** Url types for HandleUrl */
    enum TDrmUiUrlType
        {
        EUHRightsIssuerUrl   = 0x00000001,
        EUHPreviewRightsUrl  = 0x00000002,
        EUHSilentRightsUrl   = 0x00000004,
        EUHInfoUrl           = 0x00000008,
        EUHDomainRiUrl       = 0x00000010
        };

    /** Protection status of the drm content */
    enum TDrmProtectionStatus
        {
        EUUnknown = KErrUnknown,
        EUUnprotected = 0,
        EUProtected = 1
        };


    // Rejection reason
    enum TRejectionType
        {
        EURejectionNone         = 0x00000000,
        EURejectionStartTime    = 0x00000001,
        EURejectionEndTime      = 0x00000002,
        EURejectionInterval     = 0x00000004,
        EURejectionCounter      = 0x00000008,
        EURejectionTopLevel     = 0x00000010,
        EURejectionTimedCounter = 0x00000020,
        EURejectionAccumulated  = 0x00000040,
        EURejectionIndividual   = 0x00000080,
        EURejectionSystem       = 0x00000100,
        EURejectionVendor       = 0x00000200,
        EURejectionSoftware     = 0x00000400,
        EURejectionMetering     = 0x00000800,
        EURejectionDrmTime      = 0x10000000
        };

    }

#endif // DRMUTILITYTYPES_H
