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
* Description:  Client definitions
*
*/


#ifndef __WMDRMFILESERVERCLIENT_H
#define __WMDRMFILESERVERCLIENT_H

#include <e32std.h>

// Name of the WMDRM file server executable
_LIT( KWmDrmFileServerName, "!wmdrmfileserver.exe" );

// Name of the WMDRM file server process
_LIT( KWmDrmFileServerImg, "wmdrmfileserver" );

// Location of the WMDRM file server root folder
_LIT( KStoreRoot, "%c:\\sys\\private\\2000B180" );

/* Interface to the WMDRM file server, which manages files in protected location.
 * Clients need DRM capability to access all functions.
 */

class RWmDrmFileServerClient: public RSessionBase
	{
public:

    /* Connect to the server.
     *
     * @capability DRM
     * @return KErrNone if successful
     */
	IMPORT_C TInt Connect();
    

    /* Open the given file under the file server root.
     *
     * @capability DRM
     * @param aFilename File to open
     * @return  KErrNone if successful, KErrNotFound if the file does not exist,
     *          KErrArgument if the file name is not valid
     */
    IMPORT_C TInt Open( const TDesC& aFilename );
    

    /* Closes an open file.
     */
    IMPORT_C void CloseFile();


    /* Creates a file under the file server root and opens it.
     *
     * @capability DRM
     * @param aFilename File to create
     * @return KErrNone if successful, KErrArgument if the file name is not valid,
     *         KErrDiskFull if system drive doesn't have enough free space
     */
    IMPORT_C TInt Create( const TDesC& aFilename );


    /* Read the whole content of the file into a buffer
     *
     * @capability DRM
     * @param aBuffer   Buffer for file content
     * @return  KErrNone if successful, KErrNotReady if the file is not open,
     *          KErrBadDescriptor if not enough space is available in the buffer.
     */
    IMPORT_C TInt Read( TDes8& aBuffer );

    /* Replaces the file content with a buffer, truncating the file if needed.
     *
     * @capability DRM
     * @param aBuffer   New file content
     * @return  KErrNone if successful, KErrNotReady if the file is not open,
     *          KErrDiskFull if system drive doesn't have enough free space
     */
    IMPORT_C TInt Write( const TDesC8& aBuffer );

    /* Returns the size of the currently open file
     *
     * @capability DRM
     * @param aSize Output parameter for the size
     * @return KErrNone if successful, KErrNotReady if the file is not open
     */
    IMPORT_C TInt Size( TInt& aSize );

    /* Deletes a given file under the store root.
     *
     * @capability DRM
     * @param aFilename File to delete
     * @return  KErrNone if successful, KErrArgument if the file name is not valid
     */
    IMPORT_C TInt Delete( const TDesC& aFilename );

    /* Checks whether a file is currently opened for this session.
     *
     * @capability DRM
     * @return  ETrue if a file is open, EFalse otherwise
     */
    IMPORT_C TBool IsOpen();


    /* Creates a complete path under the file server root, including all path
     * components.
     *
     * @capability DRM
     * @param aPath             Path to create
     * @param aCreateIfMissing  Flag indicating whether the path should be created if
     *                          it doesn't exist
     * @return KErrNone if successful, KErrArgument if the file name is not valid
     *         KErrDiskFull if system drive doesn't have enough free space
     */
    IMPORT_C TInt MkDirAll( const TDesC& aPath, TBool aCreateIfMissing = ETrue );

    /* Remove a complete path including all contents.
     *
     * @capability DRM
     * @param aPath Path to remove
     * @return KErrNone if successful, KErrArgument if the file name is not valid
     */
    IMPORT_C TInt RmDirAll( const TDesC& aPath );

    /* Remove the last directory in a path, if the last directory is empty.
     *
     * @capability DRM
     * @param aPath 
     * @return KErrNone if successful, KErrArgument if the file name is not valid
     */
    IMPORT_C TInt RmDir( const TDesC& aPath );
    
    
    /* Updates the secure time, only callable by DRM Rights Server
     *
     * @capability DRM
     * @return  KErrNone if the operation succeeds
     *          Symbian OS error code if an error occurs
     */    
    IMPORT_C TInt UpdateSecureTime( TTime& aHomeTime, TTime& aUTCTime );
    
    /* Delete all the rights
     *
     * @capability DRM
     * @return  KErrNone if successful
     */
    IMPORT_C TInt DeleteRights();
    
	};

#endif
