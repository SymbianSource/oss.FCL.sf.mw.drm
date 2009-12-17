/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  wmdrm private cenrep keys
*
*/


#ifndef WMDRMPRIVATECRKEYS_H
#define WMDRMPRIVATECRKEYS_H

const TUid KCRUidWMDRM = {0x20011118};

const TUint32 KWMDRMLicStoreSizeRatio = 0x00000001;
const TUint32 KWMDRMLicStoreLowMem = 0x00000002;
const TUint32 KWMDRMLicStoreReservedSpace = 0x00000003;

// The following constants are meant for the internal mass drive
// of the configured storing of WMDRM rights feature with the
// WMDRM database split enabled.
const TUint32 KWMDRM2LicStoreSizeRatio = 0x00000004;
const TUint32 KWMDRM2LicStoreLowMem = 0x00000005;
const TUint32 KWMDRM2LicStoreReservedSpace = 0x00000006;

#endif      // WMDRMPRIVATECRKEYS_H
