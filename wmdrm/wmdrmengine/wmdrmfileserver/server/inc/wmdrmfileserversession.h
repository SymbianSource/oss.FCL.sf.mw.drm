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
* Description:  Server definitions
*
*/


#ifndef __WMDRMFILESERVERSESSION_H
#define __WMDRMFILESERVERSESSION_H

#include <e32base.h>
#include <f32file.h>
#include "wmdrmfileserver.h"

class CWmDrmFileServerSession : public CSession2
	{
public:
	CWmDrmFileServerSession();

    /* Create a file
     *
     * @param aMessage  IPC message (file name)
     * @return KErrNone if successful, KErrArgument if the file name is not valid,
     *         KErrDiskFull if system drive doesn't have enough free space
     */
	TInt CreateL( const RMessage2& aMessage );

    /* Open a file
     *
     * @param aMessage  IPC message (file name)
     * @return  KErrNone if successful, KErrNotFound if the file does not exist,
     *          KErrArgument if the file name is not valid
     */
    TInt OpenL( const RMessage2& aMessage );

    /* Close a file
     *
     * @param aMessage  IPC message (not used)
     * @return KErrNone
     */
    TInt CloseFileL( const RMessage2& aMessage );

    /* Read the file content into a buffer
     *
     * @param aMessage  IPC message (output buffer)
     * @return  KErrNone if successful, KErrNotReady if the file is not open,
     *          KErrBadDescriptor if not enough space is available in the buffer.
     */
    TInt ReadL( const RMessage2& aMessage );

    /* Write the file content into a buffer
     *
     * @param aMessage  IPC message (input buffer)
     * @return  KErrNone if successful, KErrNotReady if the file is not open,
     *          KErrDiskFull if system drive doesn't have enough free space
     */
    TInt WriteL( const RMessage2& aMessage );

    /* Delete a file
     *
     * @param aMessage  IPC message (file name)
     * @return  KErrNone if successful, KErrArgument if the file name is not valid
     */
    TInt DeleteL( const RMessage2& aMessage );

    /* Return the size of a file
     *
     * @param aMessage  IPC message (output parameter for the size)
     * @return KErrNone if successful, KErrNotReady if the file is not open
     */
    TInt SizeL( const RMessage2& aMessage );

    /* Checks whether a file is open for this session
     *
     * @param aMessage  IPC message (not used)
     * @return  ETrue if a file is open, EFalse otherwise
     */
    TInt IsOpenL( const RMessage2& aMessage );

    /* Creates a full file path
     *
     * @param aMessage  IPC message (path, flag indicating whether the path should be
     *                  created in case it doesn't exist)
     * @return KErrNone if successful, KErrArgument if the file name is not valid
     *         KErrDiskFull if system drive doesn't have enough free space
     */
    TInt MkDirAllL( const RMessage2& aMessage );

    /* Remove a complete path including contents
     *
     * @param aMessage  IPC message (path to remove)
     * @return  KErrNone if successful, KErrArgument if the file name is not valid
     */
    TInt RmDirAllL( const RMessage2& aMessage );

    /* Remove a non-empty directory
     *
     * @param aMessage  IPC message (directory name)
     * @return  KErrNone if successful, KErrArgument if the file name is not valid
     */
    TInt RmDirL( const RMessage2& aMessage );
    
    /* Update secure time on terminal
     *
     * @param aMessage  IPC message ( TTime, TTime )
     * @return  KErrNone if successful
     */
    TInt UpdateSecureTimeL( const RMessage2& aMessage );

    /* Delete all rights files
     *
     * @param aMessage Not used
     * @return  KErrNone if successful
     */
    TInt DeleteRightsL( const RMessage2& aMessage );	
    
private:
    /* Desctructor, closes the file
     */
	~CWmDrmFileServerSession();
    
    /* Returns a reference to the owning server
     * @return  File server reference
     */
	inline CWmDrmFileServer& Server();

    /* Main service function
     *
     * @param aMessage  IPC message
     */
	void ServiceL( const RMessage2& aMessage );
    
    /* Service error handling function, panics the client if necessary.
     *
     * @param aMessage  IPC message
     * @param TInt      Error code to handle
     */
	void ServiceError( const RMessage2& aMessage, TInt aError );

    // Handle to the currently opened file
    RFile iFile;
	};

#endif
