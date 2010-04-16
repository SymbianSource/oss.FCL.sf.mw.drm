/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of the DCF cache class
*
*/



#ifndef DCFCACHE_H
#define DCFCACHE_H

//  INCLUDES
#include <DcfCommon.h>
#include <mode.h>
#include <symmetric.h>
#include <rijndael.h>

// enables async reading
#define ASYNC_READ

// FORWARD DECLARATIONS

class RFile;
class RDRMRightsClient;
class CDcfCommon;

// CLASS DECLARATION

/**
 *  Cache for reading decrypted or plain data from a DCF file
 *
 *  @lib OmaDrmAgent.dll
 *  @since Series 60 3.0
 */

class CDcfCache : public
#ifdef ASYNC_READ
    CActive
#else
    CBase
#endif
    {

public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CDcfCache* NewL(
        RDRMRightsClient& aRightsClient,
        RFile& aFile,
        CDcfCommon& aDcf,
        TInt aPageSize = 2048,
        TInt aPageCount = 32 );

    /**
     * Destructor.
     */
    virtual ~CDcfCache();

public:
    // New functions

    /**
     * Read data from the file via the cache. Depending on the decryption
     * mode, the data is decrypted in the RightsServer or in the cache
     * @since Series 60 3.0
     * @param aPos Position from the start of the file where to read from
     * @param aDes Descriptor holding the read data
     * @param aLength Amount of data to read
     * @return KErrNone if the data is read successfully
     */
    TInt Read(
        TInt& aPos,
        TDes8& aDes,
        TInt aLength );

    /**
     * Set the decryption key to be used if decryption shall happen in the
     * cache itself.
     * @since Series 60 3.0
     * @param aKey AES128 decryption key
     * @return ?description
     */
    void SetKey(
        const TDesC8& aKey );

#ifdef ASYNC_READ

    TInt Read(
        TInt aPos,
        TDes8& aDes,
        TInt aLength,
        TRequestStatus& aStatus );

    void ReadCancel( TRequestStatus& aStatus );
#endif

protected:
    // New functions

    void CachedReadL(
        TInt& aPos,
        TDes8& aDes,
        TInt aLength );

    void UncachedReadL(
        TInt& aPos,
        TDes8& aDes,
        TInt aLength );

    /**
     * Check if a file position is in the give cache page
     * @since Series 60 3.0
     * @param aPage Page to check
     * @param aPosition Position to check
     * @return ETrue if the position is in the cache page
     */
    TBool InPage( TInt aPage, TInt aPosition );

    /**
     * Return the index of a free cache page
     * @since Series 60 3.0
     * @return Index of a free cache page
     */
    TInt GetFreePage();

    /**
     * Read a page of plain data for a given file position into the cache
     * @since Series 60 3.0
     * @param aPage Page to read
     * @param aPosition Position to read
     */
    void ReadPageL( TInt aPage, TInt aPosition );

    /**
     * Read a page of data for a given file position into the cache and
     * decrypt it
     * @since Series 60 3.0
     * @param aPage Page to read
     * @param aPosition Position to read
     */
    void ReadAndDecryptPageL( TInt aPage, TInt aPosition );

    /**
     * Copy data from a cache page into a descriptor
     * @since Series 60 3.0
     * @param aPage Page from which to read
     * @param aDes Decscriptor where the data is stored
     * @param aPosition Position in the file, updated with the new
     *                  position after the data has been read
     * @param aLength Amount of data to be read, updated with the actual
     *                amount that was read
     */
    void CopyOut( TInt aPage, TDes8& aDes, TInt& aPosition, TInt& aLength );

    /**
     * Decrypt a memory buffer using either the stored key or the
     * RightsServer.
     * @since Series 60 3.0
     * @param aIv AES CBC initialization vector
     * @param aPtr Data to be decrypted
     */
    void DecryptL( const TDesC8& aIv, TPtr8& aPtr );

#ifdef ASYNC_READ
    void ReadAsyncL();
#endif

protected:
    // Functions from base classes

#ifdef ASYNC_READ

    /**
     * From CActive
     * @since Series60 3.0
     */
    void RunL();

    /**
     * From CActive
     * @since Series60 3.0
     */
    void DoCancel();

    /**
     * From CActive
     */
    TInt RunError( TInt aError );
#endif

private:

    /**
     * C++ default constructor.
     */
    CDcfCache(
        RDRMRightsClient& aRightsClient,
        RFile& aFile,
        CDcfCommon& aDcf,
        TInt aPageSize,
        TInt aPageCount );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

protected:
    // Types and enumerations

    enum TDecryptionMode
        {
        EServerSide,
        EClientSide,
        };

#ifdef ASYNC_READ

    enum TReadMode
        {
        ENonPosRead,
        EPosRead,
        };

#endif
protected:
    // Data
    RFile& iFile;
    RDRMRightsClient& iRightsClient;
    CDcfCommon& iDcf;
    TInt iPageSize;
    TInt iPageCount;
    RPointerArray<TUint8> iPage;
    RArray<TInt> iPageUsageCount;
    RArray<TInt> iPagePosition;
    TEncryptionMethod iEncryptionMethod;
    TBuf8<KDCFKeySize> iKey;
    TDecryptionMode iDecryptionMode;

#ifdef ASYNC_READ
    TRequestStatus* iAsyncStatus;
    TInt iPos;
    TDes8* iDes;
    TInt iLength;
    TReadMode iOperation;
    TBool iAsyncReadingOngoing;
    TInt iError;
#endif
    };

#endif      // DCFCACHE_H
// End of File
