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
* Description:  Enumerations and constants used in DrmAutomatedUsage
*
*/


#ifndef DRMAUTOMATEDTYPES_H
#define DRMAUTOMATEDTYPES_H
namespace DRM
    {
    /** Automated usage types */
    enum TDrmAutomatedType
        {
        //This is a new type
        EAutomatedTypeVideoRingingTone,
        //These are from TDRMHelperAutomatedType
        EAutomatedTypeRingingTone,
        EAutomatedTypeMessageAlert,
        EAutomatedTypeEmailAlert,
        EAutomatedTypeIMAlert,
        EAutomatedTypeCalendarAlarm,
        EAutomatedTypeScreenSaver,
        EAutomatedTypeWallpaper,
        EAutomatedTypeTheme,
        EAutomatedTypeClockAlarm,
        EAutomatedTypeOther = KMaxTUint8
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
    }
#endif // DRMAUTOMATEDTYPES_H
