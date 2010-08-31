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
 * Description:
 *
 */

#include <http/rhttptransaction.h>
#include <barsc.h>
#include <DRMCommon.h>
#include <f32file.h>
#include <s32buf.h>
#include <DRMMessageParser.h>
#include <caf/caf.h>
#include <caf/cafplatform.h>
#include <Oma2Agent.h>

#include "HTTPFilterDRMDataSupplier.h"
#include "HTTPFilterDRM.h"

//------------------------------------------------------------------------

const TInt KMinContentSizeToGetTheURI = 520;
const TInt KDefaultSize( 2048 );
const TInt KWholeDataPart( -1 );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHTTPFilterDRMDataSupplier::NewL
// Two-phase constructor
// -----------------------------------------------------------------------------
//
CHTTPFilterDRMDataSupplier* CHTTPFilterDRMDataSupplier::NewL( TInt aTransId,
    MHTTPDataSupplier* iDataBody, CHTTPFilterDRM* aOwner )
    {
    CHTTPFilterDRMDataSupplier* self =
        new ( ELeave ) CHTTPFilterDRMDataSupplier( aTransId, iDataBody,
            aOwner );

    CleanupStack::PushL( self );
    self->ConstructL( KDefaultSize );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHTTPFilterDRMDataSupplier::NewL
// Two-phase constructor, CFM
// -----------------------------------------------------------------------------
//
CHTTPFilterDRMDataSupplier* CHTTPFilterDRMDataSupplier::NewL( TInt aTransId,
    MHTTPDataSupplier* iDataBody, TProcessedContentType aType,
    CHTTPFilterDRM* aOwner )
    {
    CHTTPFilterDRMDataSupplier* self =
        new ( ELeave ) CHTTPFilterDRMDataSupplier( aTransId, iDataBody,
            aOwner );
    CleanupStack::PushL( self );
    self->ConstructL( KDefaultSize, aType );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHTTPFilterDRMDataSupplier::CHTTPFilterDRMDataSupplier
// constructor
// -----------------------------------------------------------------------------
//
CHTTPFilterDRMDataSupplier::CHTTPFilterDRMDataSupplier( TInt aTransId,
    MHTTPDataSupplier* iDataBody, CHTTPFilterDRM* aOwner ) :
    iTransId( aTransId ), iBufPtr( 0, 0 ), iPHData( iDataBody ), iSendReady(
        EFalse ), iLastPart( EFalse ), iOwner( aOwner )
    {
    iDRMMessageParser = 0;
    iPrevPos = 0;
    iMemBuf = 0;
    iDataPartSize = KWholeDataPart;
    }

// -----------------------------------------------------------------------------
// CHTTPFilterDRMDataSupplier::ConstructL
// Data allocation constructor, which leaves
// -----------------------------------------------------------------------------
//
void CHTTPFilterDRMDataSupplier::ConstructL( TInt aSize )
    {
    // create the output buffer
    iBuf = HBufC8::NewMaxL( aSize );

    iBufPtr.Set( iBuf->Des() );
    iBufPtr.SetLength( 0 );

    // setup the memory buffer
    iMemBuf = TDRMMemBuf::NewL( aSize );

    // create the DRM client and the writable memory stream
    iDRMMessageParser = CDRMMessageParser::NewL();

    Attach( iMemBuf );

    iDRMMessageParser->InitializeMessageParserL( *this );

    }

// -----------------------------------------------------------------------------
// CHTTPFilterDRMDataSupplier::ConstructL
// Data allocation constructor, which leaves, CFM
// -----------------------------------------------------------------------------
//
void CHTTPFilterDRMDataSupplier::ConstructL( TInt aSize,
    TProcessedContentType /*aType*/)
    {
    // create the output buffer
    iBuf = HBufC8::NewMaxL( aSize );
    iBufPtr.Set( iBuf->Des() );
    iBufPtr.SetLength( 0 );

    // setup the memory buffer
    iMemBuf = TDRMMemBuf::NewL( aSize );

    iDRMOma1DcfCreator = COma1DcfCreator::NewL();
    Attach( iMemBuf );
    }

// -----------------------------------------------------------------------------
// CHTTPFilterDRMDataSupplier::~CHTTPFilterDRMDataSupplier()
// Destructor
// -----------------------------------------------------------------------------
//
CHTTPFilterDRMDataSupplier::~CHTTPFilterDRMDataSupplier()
    {

    // First make sure that we have completed everything, before we delete
    // Anything essential
    if ( !iSendReady ) // the destructor is called by other filters
        {
        if ( iDRMMessageParser )
            {
            TRAP_IGNORE( iDRMMessageParser->FinalizeMessageParserL() );
            }
        }

    if ( iDRMMessageParser )
        {
        delete iDRMMessageParser;
        iDRMMessageParser = NULL;
        }

    if ( iMemBuf )
        {
        delete iMemBuf;
        iMemBuf = NULL;
        }

    // This used to be deleted first, but this of course caused the finalize to fail
    if ( iBuf )
        {
        delete iBuf;
        iBuf = NULL;
        }

    iPHData = 0;

    if ( iContentMimeType )
        {
        delete iContentMimeType;
        iContentMimeType = NULL;
        }

    if ( iDRMOma1DcfCreator )
        {
        delete iDRMOma1DcfCreator;
        iDRMOma1DcfCreator = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CHTTPFilterDRMDataSupplier::AppendDataL
// Append a bulk of data into the supplier's buffer
// reallocation is needed when buffer is not big enough
// -----------------------------------------------------------------------------
//
void CHTTPFilterDRMDataSupplier::AppendDataL( const TDesC8& aDataPart )
    {
    TInt curLen = iBufPtr.Length();
    TInt reqLen = curLen + aDataPart.Length();

    if ( reqLen > iBufPtr.MaxLength() )
        {
        TRAPD( error, ( iBuf = iBuf->ReAllocL( reqLen + aDataPart.Length() ) ) ); // realloc a bit more data, this should not happen often
        if ( error != KErrNone )
            User::Leave( KErrNoMemory );

        iBufPtr.Set( iBuf->Des() );
        iBufPtr.SetLength( curLen );
        }
    iBufPtr.Append( aDataPart );
    }

// -----------------------------------------------------------------------------
// CHTTPFilterDRMDataSupplier::EncryptContentL
// encrypt the content with DRM client
// -----------------------------------------------------------------------------
//
void CHTTPFilterDRMDataSupplier::ProcessDataPartL()
    {
    // get the data part
    TPtrC8 dataPart;
    TBool lastChunk = iPHData->GetNextDataPart( dataPart );

    /** Support for Hutchinson's content protection scheme, CFM
     *
     */
    // encrypt the data
    if ( iProcessedContentType == EStandardDRMContent )
        {
        TRAPD( err, iDRMMessageParser->ProcessMessageDataL( dataPart ) );

        // error happens
        if ( err != KErrNone )
            {
            iDRMMessageParser->FinalizeMessageParserL();
            User::Leave( err );
            }

        if ( lastChunk )
            {
            iSendReady = ETrue;
            iDRMMessageParser->FinalizeMessageParserL();
            }
        }
    else if ( iProcessedContentType
        == EHutchinsonCFMWithRingingToneNoFirstChunk )
        {
        //create rights
        CDRMRights* rights = CDRMRights::NewL();
        CleanupStack::PushL( rights );

        // Asset contains the content id
        // DrmAsset.h
        CDRMAsset* asset = CDRMAsset::NewLC();
        _LIT8(KContentURI, "flk:flkS60_3_0_Hutchinson_2005");
        asset->iUid = KContentURI().AllocL(); // This will be freed by the assets destructor

        // Set the asset to the rights class, it will duplicate the asset
        rights->SetAssetL( *asset );
        CleanupStack::PopAndDestroy( asset ); // Asset

        // DRMPermission.h
        CDRMPermission* permission = CDRMPermission::NewLC();

        // DRMConstraint.h
        // DRMTypes.h
        permission->iPlay = CDRMConstraint::NewL();
        permission->iDisplay = CDRMConstraint::NewL();
        permission->iExecute = CDRMConstraint::NewL();
        permission->iPrint = CDRMConstraint::NewL();

        permission->iAvailableRights = ERightsPlay | ERightsDisplay
            | ERightsExecute | ERightsPrint;

        permission->iRightsObjectVersion.iVersionMain = 1; // major version for Oma 1 Rights Objects

        // "ringtone=no" present
        permission->iInfoBits = ENoRingingTone;

        // Set the permission to the rights class, it will duplicate the permission
        rights->SetPermissionL( *permission );

        CleanupStack::PopAndDestroy( permission ); // Permission

        //initializing
        iDRMOma1DcfCreator->EncryptInitializeL( *this,
            iContentMimeType->Des(), rights );
        //process current chunk
        TRAPD(err, iDRMOma1DcfCreator->EncryptUpdateL(dataPart));
        if ( err )
            {
            iDRMOma1DcfCreator->EncryptFinalizeL();
            User::Leave( err );
            }

        // if there is only one chunk
        if ( lastChunk )
            {
            iSendReady = ETrue;
            iDRMOma1DcfCreator->EncryptFinalizeL();
            }

        CleanupStack::PopAndDestroy( rights );
        iProcessedContentType = EHutchinsonCFMNoContinuation;
        }
    else if ( iProcessedContentType == EHutchinsonCFMNoFirstChunk )
        {
        //initializing
        iDRMOma1DcfCreator->EncryptInitializeL( *this,
            iContentMimeType->Des(), NULL);
        //process current chunk
        TRAPD(err, iDRMOma1DcfCreator->EncryptUpdateL(dataPart));
        if ( err )
            {
            iDRMOma1DcfCreator->EncryptFinalizeL();
            User::Leave( err );
            }

        // if there is only one chunk
        if ( lastChunk )
            {
            iSendReady = ETrue;
            iDRMOma1DcfCreator->EncryptFinalizeL();
            }
        iProcessedContentType = EHutchinsonCFMNoContinuation;
        }
    else if ( iProcessedContentType == EHutchinsonCFMNoContinuation )
        {
        TRAPD(err, iDRMOma1DcfCreator->EncryptUpdateL(dataPart));
        if ( err )
            {
            iDRMOma1DcfCreator->EncryptFinalizeL();
            User::Leave( err );
            }
        if ( lastChunk )
            {
            iSendReady = ETrue;
            iDRMOma1DcfCreator->EncryptFinalizeL();
            }
        }
    else
        {
        User::Leave( KErrUnknown );
        }
    iPHData->ReleaseData();
    }

// -----------------------------------------------------------------------------
// CHTTPFilterDRMDataSupplier::GetNextDataPart
// virtual methods from MHTTPDataSupplier
// -----------------------------------------------------------------------------
//
TBool CHTTPFilterDRMDataSupplier::GetNextDataPart( TPtrC8& aDataPart )
    {

    // First call after releasedata
    if ( iDataPartSize == KWholeDataPart )
        {
        aDataPart.Set( iMemBuf->iBuf->GetPtr() );
        iDataPartSize = aDataPart.Length();
        if( iSendReady )
            {
            iLastPart = ETrue;
            }
        }
    else // Consecutive calls so that we always return the same part
         // and same amount of data
        {
        aDataPart.Set( iMemBuf->iBuf->GetPtr().Left( iDataPartSize ) );
        }

    // this check did not work, as in this phase the iDataPartSize is never
    // KWholeDataPart
    //return ( iDataPartSize == KWholeDataPart ) ? iSendReady : EFalse;

    // Always return info if this is the last part, this is only true
    // if iSendReady has been true when ReleaseData() has been called
    return iLastPart;
    }

// -----------------------------------------------------------------------------
// CHTTPFilterDRMDataSupplier::ReleaseData
// virtual methods from MHTTPDataSupplier
// -----------------------------------------------------------------------------
//
void CHTTPFilterDRMDataSupplier::ReleaseData()
    {
    if ( iDataPartSize == KWholeDataPart ||
         ( iLastPart && iSendReady ) )
        {
        TRAP_IGNORE( Sink()->SeekL( MStreamBuf::EWrite, TStreamPos(0) ) );
        if ( iSendReady )
            {
            iOwner->DeleteDataSupplier( iTransId );
            }
        }
    else
        {
        //Remove only consumed data part
        HBufC8* b( iMemBuf->iBuf->GetPtr().Mid( iDataPartSize ).AllocLC() );
        TRAP_IGNORE( Sink()->SeekL( MStreamBuf::EWrite, TStreamPos(0) ) );
        // Warning: assuming sink is big enough for write of whole b.
        TRAP_IGNORE( Sink()->WriteL( b->Ptr(), b->Length() ) );
        CleanupStack::PopAndDestroy( b );
        // Update data part size to available data.
        iDataPartSize = KWholeDataPart;

        // if we are ready processing, make sure the client knows this
        // is the last part
        if( iSendReady )
            {
            iLastPart = ETrue;
            }
        }
    }

// -----------------------------------------------------------------------------
// CHTTPFilterDRMDataSupplier::OverallDataSize
// virtual methods from MHTTPDataSupplier
// -----------------------------------------------------------------------------
//
TInt CHTTPFilterDRMDataSupplier::OverallDataSize()
    {
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CHTTPFilterDRMDataSupplier::Reset
// virtual methods from MHTTPDataSupplier
// -----------------------------------------------------------------------------
//
TInt CHTTPFilterDRMDataSupplier::Reset()
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CHTTPFilterDRMDataSupplier::SetContentMimeTypeL
// Sets the MIME type of comming content - used for ringingtone functionality
// Caller does not care about freeing this buffer, CFM
// -----------------------------------------------------------------------------
//
void CHTTPFilterDRMDataSupplier::SetContentMimeTypeL( const TDesC8& aMimeType )
    {
    //in case someone call this function more than once
    delete iContentMimeType;
    iContentMimeType = NULL;

    iContentMimeType = aMimeType.AllocL();
    }

// -----------------------------------------------------------------------------
// CHTTPFilterDRMDataSupplier::SetEstimatedArrivalTime
// Sets the estimated arrival time, the x-oma-drm-separate-delivery header
//-----------------------------------------------------------------------------
//

#ifdef __DRM_FULL
void CHTTPFilterDRMDataSupplier::SetEstimatedArrivalTime( TInt aXOmaHeaderVal )
    {
    // get the data part
    ContentAccess::CManager* manager = NULL;
    ContentAccess::TAgent agent;
    TRequestStatus status;
    TInt r = KErrNone;
    DRMCommon::TContentProtection protection;
    HBufC8* MIMEType = NULL;
    HBufC8* contentURI = NULL;
    HBufC8* xomaData = NULL;
    TUint dataLength;
    TPtrC8 dataPart;
    DRMCommon* drmCommon = NULL;

        TRAP(r, drmCommon = DRMCommon::NewL());
    if ( r )
        {
        return;
        }

    r = drmCommon->Connect();
    if ( r )
        {
        delete drmCommon;
        return;
        }

    iPHData->GetNextDataPart( dataPart );

    //in case the chunk is too short engine would panic
    if ( dataPart.Length() < KMinContentSizeToGetTheURI )
        return;

        // Find the caf agent and create manager
        TRAP( r, manager = GetCafDataL( agent ));

    // if an error occurs, return
    if ( r )
        {
        delete drmCommon;
        return;
        }

    // Get the content info, if it fails dont do anything
    if ( drmCommon->GetContentInfo( dataPart, protection, MIMEType,
        contentURI, dataLength ) == DRMCommon::EOk && protection
        != DRMCommon::ENoDCFFile )
        {
        // Create a buffer large enough for the time and uri
        xomaData = HBufC8::NewMax( sizeof(TInt) + contentURI->Size() );

        // if the creation fails free memory and return
        if ( !xomaData )
            {
            delete MIMEType;
            delete contentURI;
            delete manager;
            delete drmCommon;
            return;
            }

        // Copy the value of the xoma header:
        Mem::Copy( const_cast<TUint8*> ( xomaData->Ptr() ), &aXOmaHeaderVal,
            sizeof(TInt) );

        // Copy the value of the uri:
        Mem::Copy( const_cast<TUint8*> ( xomaData->Ptr() ) + sizeof(TInt),
            contentURI->Ptr(), contentURI->Size() );

        TPtr8 temp( NULL, 0, 0 );
        TPtr8 buffer( xomaData->Des() );

        // ignore any error, we couldn't do anything about it anyway
        r = manager->AgentSpecificCommand( agent,
            ContentAccess::ESetPendingRightsETA, buffer, temp );
        }
    delete MIMEType;
    delete contentURI;
    delete manager;
    delete xomaData;
    delete drmCommon;
    }
#else
void CHTTPFilterDRMDataSupplier::SetEstimatedArrivalTime(TInt /*aXOmaHeaderVal*/)
    {
    }
#endif

// -----------------------------------------------------------------------------
// CHTTPFilterDRMDataSupplier::SetContentMimeTypeL
// Sets the MIME type of comming content - used for ringingtone functionality
// Caller does not care about freeing this buffer
// -----------------------------------------------------------------------------
//
ContentAccess::CManager* CHTTPFilterDRMDataSupplier::GetCafDataL(
    TAgent& aAgent )
    {
    TPtr8 ptr( NULL, 0, 0 );
    RArray<TAgent> agents;
    TRequestStatus status;
    TInt i;

    CleanupClosePushL( agents );
    CManager* manager = CManager::NewLC();

    manager->ListAgentsL( agents );

    for ( i = 0; i < agents.Count(); i++ )
        {
        if ( agents[i].Name().Compare( KOmaDrm2AgentName ) == 0 )
            {
            aAgent = agents[i];
            break;
            }
        }
    CleanupStack::Pop( manager );
    CleanupStack::PopAndDestroy( &agents );
    return manager;
    }

//=============================================================================
// TDRMMemBuf functions
//=============================================================================
TDRMMemBuf* TDRMMemBuf::NewL( TInt aLength )
    {
    TDRMMemBuf* self = new ( ELeave ) TDRMMemBuf;
    CleanupStack::PushL( self );
    self->ConstructL( aLength );
    CleanupStack::Pop( self );
    return self;
    }

TDRMMemBuf::~TDRMMemBuf()
    {
    delete iBuf;
    iBuf = 0;
    }

void TDRMMemBuf::ConstructL( TInt aLength )
    {
    // create the buffer and set it as intermidiate
    iBuf = HeapArray8::NewMaxL( aLength );
    TUint8* base = ( TUint8* )( iBuf->GetPtr().Ptr() );
    Set( base, base + aLength );
    }

void TDRMMemBuf::DoWriteL( const TAny* aPtr, TInt aLength )
    {
    //make sure there is enough room for writing
    if ( iBuf->Append( ( TUint8* )aPtr, aLength ) )
        {
        // re-allocation happened
        TUint8* base = ( TUint8* )( iBuf->GetPtr().Ptr() );
        Set( base, base + iBuf->GetPtr().MaxLength() );
        }
    }

TStreamPos TDRMMemBuf::DoSeekL( TMark aMark, TStreamLocation aLocation,
    TInt anOffset )
    {
    if ( aMark == MStreamBuf::EWrite && aLocation == EStreamBeginning )
        iBuf->GetPtr().SetLength( anOffset );

    return TMemBuf::DoSeekL( aMark, aLocation, anOffset );
    }

//=============================================================================
// HeapArray functions
//=============================================================================
HeapArray8* HeapArray8::NewMaxL( TInt max_ )
    {
    HeapArray8* self = new ( ELeave ) HeapArray8();
    CleanupStack::PushL( self );
    self->ConstructL( max_ );
    CleanupStack::Pop( self );
    return self;
    }

void HeapArray8::ConstructL( TInt max_ )
    {
    buf = HBufC8::NewMaxL( max_ );
    ptr.Set( buf->Des() );
    ptr.SetLength( 0 );
    }

HeapArray8::HeapArray8() :
    buf( 0 ), ptr( 0, 0 )
    {
    }

HeapArray8::~HeapArray8()
    {
    delete buf;
    }

TBool HeapArray8::Append( const TDesC8 &src_ )
    {
    return Insert( ptr.Length(), src_ );
    }

TBool HeapArray8::Insert( TInt index, const TDesC8 &src_ )
    {
    TInt insertLen = src_.Length();
    TBool bRellocated = EFalse;

    if ( ptr.Length() + insertLen > ptr.MaxLength() )
        {
        TInt oLen = ptr.Length();
        buf = buf->ReAlloc( ptr.Length() + insertLen * 2 );
        ptr.Set( buf->Des() );
        ptr.SetLength( oLen );
        bRellocated = ETrue;
        }
    ptr.Insert( index, src_ );
    return bRellocated;
    }

TBool HeapArray8::Insert( TInt index, const TUint8 *src_, TInt insertLen )
    {
    TPtrC8 ptr( src_, insertLen );
    return Insert( index, ptr );
    }

TBool HeapArray8::Append( const TUint8* src_, TInt len_ )
    {
    TPtrC8 p( src_, len_ );
    return Insert( ptr.Length(), p );
    }
