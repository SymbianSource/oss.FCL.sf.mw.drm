/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handling end time based automated content
*
*/



#ifndef DRMHELPERSERVERCONFIGINTERNALCRKEYS_H
#define DRMHELPERSERVERCONFIGINTERNALCRKEYS_H

//#include <DRMHelperServerConfigSDKCRKeys.h>

const TUid KCRUidDRMHelperServerConfig = {0x10205CA7};

/* Key format: <TUint8>
0	No notification for RO about to expire
1..*	permission about to expire notification will come N days before permission expires

  
*/ 

const TUint32 KDRMHSInformingInterval = 0x00000001;


#endif      // DRMHELPERSERVERCONFIGINTERNALCRKEYS_H

