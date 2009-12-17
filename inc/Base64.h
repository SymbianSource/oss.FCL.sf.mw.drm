/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



#ifndef BASE64_H
#define BASE64_H

// FUNCTION PROTOTYPES
IMPORT_C HBufC8* Base64EncodeL(
    const TDesC8& aInput);

IMPORT_C HBufC8* Base64DecodeL(
    const TDesC8& aInput);

#endif      // BASE64_H   
            
// End of File
