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
*     
*
*/


#ifndef ROMTMUIDATACONST_H
#define ROMTMUIDATACONST_H


enum TRoMtmUdPanic
	{
	ERoMtmUdWrongMtm,
	ERoMtmUdFoldersNotSupported,
	ERoMtmUdAttachmentsNotSupported,
	ERoMtmUdNoIconForAttachment,
	ERoMtmUdRootEntryGiven,
	ERoMtmUdUnknownOperationId
	};

// Correct path is added to literal when it is used.
_LIT(KRoMtmUiDataBitmapFile, "z:MUIU.MBM");

LOCAL_C const TInt KBitmapArraySize = 2;
LOCAL_C const TInt KBitmapIndex = 0;     // only one bitmap for create email

#endif      // ROMTMUIDATACONST_H
            
// End of File
