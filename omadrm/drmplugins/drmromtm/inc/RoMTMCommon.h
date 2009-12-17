/*
* Copyright (c) 2002, 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*     Common constants for various RoMTM dll's 
*
*/


#ifndef ROMTMCOMMON_H
#define ROMTMCOMMON_H



// RoMTM UIDs
LOCAL_D const TInt KUidMsgTypeRoValue = 0x101F6DC2; 
LOCAL_D const TUid KUidMsgTypeRo = { KUidMsgTypeRoValue};
LOCAL_D const TInt KUidTechnologyTypeRoValue = 0x10009157;
LOCAL_D const TUid theTechnologyTypeUid	= { KUidTechnologyTypeRoValue };

LOCAL_D const TUid KUidMsvMtmRoServerDLL	={0x101F6DC1};
LOCAL_D const TUid KUidMsvMtmRoClientDLL	={0x101F6DC0};
LOCAL_D const TUid KUidMsvMtmRoUiDLL		={0x101F6DBE};
LOCAL_D const TUid KUidMsvMtmRoUiDataDLL	={0x101F6DBF};

// Constants
LOCAL_D const TInt KMaxRoMessageSize = 0xFFFF;

#endif      // ROMTMCOMMON_H
            
// End of File
