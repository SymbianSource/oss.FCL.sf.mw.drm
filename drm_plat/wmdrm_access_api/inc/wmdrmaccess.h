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
* Description:
*  wmdrmaccess.h
*
*/

#ifndef _WMDRM_ACCESS_API_H_
#define _WMDRM_ACCESS_API_H_

// Forward declarations
class CWmDrmAccessEcomInterface;

// Class declaration
/**
*  Provides access to WM DRM protected files
*  (e.g. decrypting services for WM DRM protected files)
*
*  @code
*  // Way to use CWmDrmAccess
*
*  CWmDrmAccess* access( CWmDrmAccess::NewL() );
*  // ...
*  // fetch file header of the WM DRM protected file to be decrypted.
*  // Assign it to initializationData
*  // ...
*  // Initialize with given initialisation data
*  access->Initialize( initializationData );
*  // ...
*  // read encrypted data block to buffer
*  // ...
*  CWmDrmAccess->Decrypt( buffer );
*  // use decrypted data in buffer
*  // ...
*  // when all data has been decrypted, close decryption context
*  access->Close();
*  delete access;
*  @endcode
*
*/
NONSHARABLE_CLASS( CWmDrmAccess ) : public CBase
    {
public:
    /** Constructors */
    IMPORT_C static CWmDrmAccess* NewL();
    IMPORT_C static CWmDrmAccess* NewLC();

    /** Destructor */
    IMPORT_C virtual ~CWmDrmAccess();

    /**
    * Initialises decryption context
    * This initialises decryption session for given WM DRM protected file.
    *
    * @param aHeader    This parameter is for inputting header
    *                   of the given WM DRM protected file to be decrypted
    *                   to WM DRM system.
    *
    *
    * @return           A Symbian OS error code, KErrNone on success.
    *
    * @see CWmDrmAccess::Close
    *
    */
    IMPORT_C TInt Initialize( const TDesC8& aHeader );


    /**
    * Decrypts encrypted data in place.
    * This is for decrypting data of the file, header of which is used
    *  as initialisation parameter at CWmDrmAccess::Initialize.
    *
    * @param aBuffer    This will contain data to be decrypted.
    *                   On successful completion, given data is
    *                   decrypted in aBuffer.
    *
    * @return           A Symbian OS error code, KErrNone on success.
    *
    * One small sample describing the use of the function.
    * @code
    * RBuf8 buffer;
    * // ...
    * // Fetch encrypted data to buffer
    * FetchEncryptedData( buffer );
    *
    * // Decrypt encrypted data.
    * TInt error = access->Decrypt( buffer );
    * if ( !error )
    *     {
    *     UseDecryptedData( buffer );
    *     }
    * @endcode
    *
    * @see CWmDrmAccess::Initialize
    * @see CWmDrmAccess::Close
    *
    */
    IMPORT_C TInt Decrypt( TDes8& aBuffer );


    /**
    * Closes decryption context.
    *
    * @return           A Symbian OS error code, KErrNone on success.
    *
    * @see CWmDrmAccess::Initialize
    *
    */
    IMPORT_C TInt Close();

private:
    /** Default constructor */
    CWmDrmAccess();
    /** Second phase constructor */
    void ConstructL();

    /** Internal interface to the WM DRM system */
    CWmDrmAccessEcomInterface* iWmDrmAccessEcomInterface;

    };
#endif // _WMDRM_ACCESS_API_H_
