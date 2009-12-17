/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handling storing location of Rights Objects
*
*/


#ifndef DRMSTORINGINTERNALCRKEYS_H
#define DRMSTORINGINTERNALCRKEYS_H

const TUid KCrUidOmaDrmRightsServer = {0x2001FE33};

const TUid KCrUidWmDrmRightsServer = {0x2001FE55};
    
/* Location (drive) for storing the OMA DRM rights objects as desired by the user */
const TUint32 KConfigOmaDrmStoringLocation = 0x00000001;

/* Location (drive) for storing the WM DRM licenses as desired by the user */
const TUint32 KConfigWmDrmStoringLocation = 0x00000001;

#endif      // DRMSTORINGINTERNALCRKEYS_H

