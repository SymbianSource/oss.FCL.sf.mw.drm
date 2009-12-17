/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains the WM DRM CAF Agent specific data types, commands
*                and attributes
*
*/



#ifndef WMDRMAGENT_H
#define WMDRMAGENT_H

//  INCLUDES
#include <caf/caftypes.h>

// CONSTANTS
_LIT(KWmDrmAgentName, "WM DRM Agent");

_LIT8(KWmTriggerContentType, "application/asf.wmdrm.roap-trigger+xml");
_LIT8(KWmfImportContentType, "application/x-asf.drm.import");
_LIT8(KWmfContentType, "application/vnd.drm.asf");
_LIT8(KWmaContentType, "audio/x-ms-wma");
_LIT8(KWmvContentType, "video/x-ms-wmv");
_LIT8(KAsfFileType, "application/vnd.ms-asf" );

_LIT(KWmDrmWmaExtension, ".wma");
_LIT(KWmDrmWmvExtension, ".wmv");
_LIT(KWmDrmAsfExtension, ".asf");


// DATA TYPES

namespace ContentAccess
{
// Agent specific attribute identifiers

enum TWmDrmAgentStringAttribute
    {
    ERating = EAgentSpecificAttributeBase + 1,
    EPicture,
    EText,
    EComposer,
    EYear,
    EOriginalArtist,
    EWmTrackNumber,
    EUniqueFileID,
    EAudioFileUrl,
    ESharedUserRating,
    EDate
    };

// File types
    
enum
    {
    EAsf = 3, // next in line after OMA DRM file types
    };
    
}

namespace DRM
    {
    enum TWmDrmAgentManagerCommands
        {
        EWmDrmDeleteRights
        };
    }

#endif      // WMDRMAGENT_H   
            
// End of File
