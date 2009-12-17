/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class implements the base64 en/decoding.
*
*/


#ifndef __BASE64_H_
#define __BASE64_H_

TInt b64decode(TUint8 *source, TUint32 length, TUint8 *destination, TUint32 *destination_length);

#endif // __BASE64_H_
// End of File
