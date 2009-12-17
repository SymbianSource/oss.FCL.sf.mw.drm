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
* Description:  Common client server definitions
*
*/


#ifndef __WMDRMFSCLIENTSERVER_H
#define __WMDRMFSCLIENTSERVER_H

#include <e32std.h>

// List of all possible messages to the WMDRM file server
enum TWmDrmFileServerMessages
	{
    EFsOpen,            // Open a file
    EFsCreate,          // Create a file
    EFsRead,            // Read the contents of an open file
    EFsWrite,           // Replace the contents of an open file
    EFsSize,            // Return the size of an open file
    EFsDelete,          // Delete a file
    EFsIsOpen,          // Check whether a file is open
    EFsCloseFile,       // Close an open file
    EFsMkDirAll,        // Conditionally create a full file path
    EFsRmDirAll,        // Remove a full path including content
    EFsRmDir,           // Remove a non-empty directory
    EFsUpdateSecureTime,// Update the secure time
    EFsDeleteRights     // Delete all rights
	};

#endif //  __WMDRMFSCLIENTSERVER_H
