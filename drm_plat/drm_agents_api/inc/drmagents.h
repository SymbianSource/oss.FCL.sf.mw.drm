/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains agent specific data types, commands
*                and attributes that are common DRM agents
*
*/


#ifndef DRMAGENTS_H
#define DRMAGENTS_H

#include <caf/caftypes.h>

namespace DRM 
{

/**
* OMA DRM CAF Agent name
*/
_LIT(KDrmOmaAgentName, "OMA DRM Agent");

/**
* WMDRM CAF Agent name
*/
_LIT(KDrmWMAgentName, "WM DRM Agent");

/**
* Common attributes for DRM CAF Agents
*/
enum TDrmAgentAttribute
    {
    EDrmFileType = ContentAccess::EAgentSpecificAttributeBase + ContentAccess::EAgentSpecificAttributeBase + 1,
    EDrmAgentUid,
    EDrmAllowedOutputs
    };

/**
* Common string attributes for DRM CAF Agents
*/
enum TDrmAgentStringAttribute
    {
    EDrmAgentName = ContentAccess::EAgentSpecificAttributeBase + ContentAccess::EAgentSpecificAttributeBase + 1,
    EDrmFileName,
    EDrmOriginalArtist,
    EDrmRatingInfo,
    EDrmTrackNumber,
    EDrmRecordingYear,
    EDrmFullName
    };

/**
* DRM protection type of the file
*/
enum TDrmFileType
    {
    EDrmNone             = 0,    ///< No DRM
    EDrmOma1Dcf          = 1,    ///< OMA DRM v1
    EDrmOma2Dcf          = 2,    ///< OMA DRM v2
    EDrmWMFile           = 3,    ///< WMDRM
    EDrmOma1DcfBased     = 4     ///< OMA DRM v1 DCF based format with extensions
    };

/**
* DRM CAF Agent UIDs
*/
enum TDrmAgentUid
    {
    EDrmOmaAgent    = 0x101F6DB4,   ///< OMA DRM CAF Agent UID
    EDrmWmAgent     = 0x10205CB6,   ///< WMDRM CAF Agent UID
    };

/**
* Allowed outputs for the content. The values below are used as bitmasks and
* can be combined to allow multiple allowed outputs. Note that not all possible
* outputs are listed here. The outputs will be added once their impact to the
* adpatation layer and their compliance rule impact is understood. Only
* outputs which have been analysed before are listed here.
*/
enum TDrmAllowedOutputs
    {
    /**
    * Allow all audio and video outputs
    */
    EDrmAllowAll                = 0xFFFFFFFF,
    
    /**
    * Allow none of the outputs
    */
    EDrmAllowNone               = 0x00000000,
    
    /**
    * Allow analog audio output via line out or the built-in speaker
    */
    EDrmAllowAudioAnalog        = 0x00000001,
    
    /**
    * Allow playback via an analog radio transmitter broadcasting device,
    * commonly called FM Transmitter
    */
    EDrmAllowAudioFmTransmitter = 0x00000002,
    
    /**
    * Allow playback via a mono Bluetooth link using the Handset or
    * Handsfree Profiles (HSP, HFP), or using an A2DP profile, but in neither
    * case playing back with more than 48kHz sample rate, 16 bits sample width,
    * 2 channels and a playback speed of 1.5 times the original speed
    */
    EDrmAllowAudioBluetooth     = 0x00000004,
    
    /**
    * Allow mixing the audio stream upwards into other audio playback streams,
    * e.g. into a phone call
    */
    EDrmAllowAudioUplink        = 0x00000008,
    
    /**
    * Allow output of the video signal to an analog video output device,
    * e.g. S-Video
    */
    EDrmAllowVideoAnalog        = 0x00000010,
    
    /**
    * Allow video output over an anlog output channel which has MacroVision
    * video protection
    */
    EDrmAllowVideoMacroVision   = 0x00000020,
    
    /**
    * Allow audio output via USB
    */
    EDrmAllowAudioUsb           = 0x00000040,
    
    /**
    * Allow audio output to devices using HDCP, e.g. DVD players and HDTV.
    */
    EDrmAllowAudioHdcp              = 0x00000080,
    
    /**
    * Licensed product is required to engage HDCP to protect the audio.
    * If HDCP is not engaged or can't be engaged the audio must not be passed through to HDMI.
    */
    EDrmAllowAudioHdmiHdcpRequired  = 0x00000080,
    
    /**
    * Allow audio output to devices using HDMI, e.g. DVD players and HDTV.
    */
    EDrmAllowAudioHdmi              = 0x00000100,

    /**
    * Allow the passing the video data to devices using HDMI, eg DVD players and HDTV
    */
    EDrmAllowVideoHDMI              = 0x00000200,
            
    /**
    * Licensed product must attempt to engage HDCP to protect the video. 
    * However it should be passed through to HDMI even if HDCP is not engaged or fails to engage.
    */
    EDrmAllowVideoHdmiHdcpRequested = 0x00000400,

        
    /**
    * Licensed product is required to engage HDCP to protect the video.
    * If HDCP is not engaged or can not be engaged the video must not be passed through to HDMI.
    */
    EDrmAllowVideoHdmiHdcpRequired  = 0x00000800

    };
}

#endif // DRMAGENTS_H
