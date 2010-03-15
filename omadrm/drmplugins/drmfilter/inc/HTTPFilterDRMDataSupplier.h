/*
 * Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Declaration of DRM Http Filter Data Supplier
 *
 */

#ifndef _HTTPFILTERDRMDATASUPPLIER_H_
#define _HTTPFILTERDRMDATASUPPLIER_H_
//-----------------------------------------------------------------------------

#include <s32mem.h>
#include <http/mhttpdatasupplier.h>
#include <Oma1DcfCreator.h>
//-----------------------------------------------------------------------------

class CDRMMessageParser;
class RMemWriteStream;
class CHTTPFilterDRMDataSupplier;
class ContentAccess::TAgent;
class ContentAccess::CManager;
class CHTTPFilterDRM;
//-----------------------------------------------------------------------------

// Structure allows distinguishing between standard DRM content and Hutchinson's protection
enum TProcessedContentType
    {
    EStandardDRMContent = 0,
    EHutchinsonCFMNoFirstChunk,
    EHutchinsonCFMWithRingingToneNoFirstChunk,
    EHutchinsonCFMNoContinuation,
    EUnknownContent
    };

/**
 * HeapArray8 - the helper class for memory re-allocation
 *
 * @lib DRMFilter.lib
 * @since 2.6
 */
class HeapArray8
    {
public:
    static HeapArray8* NewMaxL( TInt max_ );

    // operations
    TBool Append( const TDesC8& src_ );
    TBool Append( const TUint8* src_, TInt len_ );
    TBool Insert( TInt index, const TDesC8 &src_ );
    TBool Insert( TInt index, const TUint8* src_, TInt insertLen );

    TPtr8& GetPtr()
        {
        return ptr;
        }
    ~HeapArray8();

private:
    HeapArray8();
    void ConstructL( TInt );

    HBufC8* buf;
    TPtr8 ptr;
    };

/**
 * TDRMMemBuf - the helper class for memory re-allocation
 *
 * @lib DRMFilter.lib
 * @since 2.6
 */
class TDRMMemBuf : public TMemBuf
    {
public:
    static TDRMMemBuf* NewL( TInt aLength );
    ~TDRMMemBuf();

protected:
    virtual void DoWriteL( const TAny* aPtr, TInt aLength );
    virtual TStreamPos DoSeekL( TMark aMark, TStreamLocation aLocation,
        TInt anOffset );

private:
    void ConstructL( TInt aLength );

private:
    HeapArray8* iBuf;
    friend class CHTTPFilterDRMDataSupplier;
    };

/**
 * a data supplier used for replace the default supplier in response data
 *
 * @lib DRMFilter.lib
 * @since 2.6
 */
class CHTTPFilterDRMDataSupplier : public CBase,
    public MHTTPDataSupplier,
    public RMemWriteStream
    {
public:
    // ctor & dtor
    static CHTTPFilterDRMDataSupplier* NewL( TInt,
        MHTTPDataSupplier* iDataBody, CHTTPFilterDRM* aOwner );
    /** Support for Hutchinson's content protection scheme, CFM
     *
     */
    static CHTTPFilterDRMDataSupplier* NewL( TInt,
        MHTTPDataSupplier* iDataBody, TProcessedContentType aType,
        CHTTPFilterDRM* aOwner );
    /*
     *
     */
    virtual ~CHTTPFilterDRMDataSupplier();
    //-------------------------------------------------------------------------

    // virtual functions from MHTTPDataSupplier
    virtual TBool GetNextDataPart( TPtrC8& aDataPart );
    virtual void ReleaseData();
    virtual TInt OverallDataSize();
    virtual TInt Reset();
    TInt GetTransId() const
        {
        return iTransId;
        }
    //-------------------------------------------------------------------------


    void ProcessDataPartL();

    //-------------------------------------------------------------------------

private:
    CHTTPFilterDRMDataSupplier( TInt aTransId, MHTTPDataSupplier* iDataBody,
        CHTTPFilterDRM* aOwner );
    CHTTPFilterDRMDataSupplier( const CHTTPFilterDRMDataSupplier& ); // not implemented
    CHTTPFilterDRMDataSupplier& operator=( const CHTTPFilterDRMDataSupplier& ); // not implemented
    ContentAccess::CManager* GetCafDataL( TAgent& aAgent );
    //-------------------------------------------------------------------------

    void ConstructL( TInt aSize );
    /** Support for Hutchinson's content protection scheme, CFM
     *
     */
    void ConstructL( TInt aSize, TProcessedContentType /*aType*/);
    /**
     *
     */
    void AppendDataL( const TDesC8& aData );
    //-------------------------------------------------------------------------

    TInt iTransId;
    HBufC8* iBuf;
    TPtr8 iBufPtr;
    MHTTPDataSupplier *iPHData; // data supplier from protocol handler
    TBool iSendReady; // ready for sending?
    TBool iLastPart; // should last part info be sent

    CDRMMessageParser* iDRMMessageParser;
    TInt iPrevPos;
    TDRMMemBuf* iMemBuf;
    /** Support for Hutchinson's content protection scheme, CFM
     *
     */
private:
    HBufC8* iContentMimeType;
    TProcessedContentType iProcessedContentType;
    COma1DcfCreator *iDRMOma1DcfCreator;
    CHTTPFilterDRM* iOwner;
    TInt iDataPartSize;
public:
    void SetEstimatedArrivalTime( TInt aXOmaHeaderVal );
    void SetContentMimeTypeL( const TDesC8& aMimeType );
    void SetProcessedContentType( TProcessedContentType aType )
        {
        iProcessedContentType = aType;
        }
    //-------------------------------------------------------------------------
    };
//-----------------------------------------------------------------------------
#endif // !_HTTPFILTERDRMDATASUPPLIER_H_
