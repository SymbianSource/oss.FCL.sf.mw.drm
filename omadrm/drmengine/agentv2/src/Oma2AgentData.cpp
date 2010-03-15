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
* Description:  Implementation of the CAF Agent Data class
*
*/

// INCLUDE FILES
#include <caf/caf.h>
#include <caf/cafplatform.h>
#include <caf/bitset.h>
#include <caf/attribute.h>
#include <e32test.h>
#include "DRMCommon.h"
#include "Oma2AgentData.h"
#include "Oma2AgentAttributes.h"
#include "Oma1Dcf.h"
#include "oma2dcf.h"
#include "symmetric.h"
#include "DRMRightsClient.h"
#include "DRMProtectedRoParser.h"
#include "DcfCache.h"
#include "cleanupresetanddestroy.h"

using namespace ContentAccess;
//TRACE macros
#ifdef _DEBUG
#include <e32debug.h> // RDebug
#define TRACE( x ) RDebug::Print( _L( x ) )
#define TRACE2( x, y ) RDebug::Print( _L( x ), y )
#define TRACE3( x, y, z ) RDebug::Print( _L( x ), y, z )
#else
#define TRACE( x )
#define TRACE2( x, y )
#define TRACE3( x, y, z )
#endif
// CONSTANTS
_LIT8(KMimeAudio, "audio");
_LIT8(KMimeImage, "image");
// LOCAL FUNCTION PROTOTYPES
LOCAL_C TInt MapContentShareMode( TContentShareMode aMode );

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// MapContentShareMode
// Maps the CAF specific file share mode to the RFs/RFile sharing mode
// -----------------------------------------------------------------------------
//
LOCAL_C TInt MapContentShareMode( TContentShareMode aMode )
    {
    TInt r = EFileRead | EFileShareAny;

    switch ( aMode )
        {
        case EContentShareReadOnly:
            r = EFileRead | EFileShareReadersOnly;
            break;
        case EContentShareReadWrite:
            r = EFileRead | EFileShareReadersOrWriters;
            break;
        case EContentShareExclusive:
            r = EFileRead | EFileShareExclusive;
            break;
        }
    return r;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// COma2AgentData::COma2AgentData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
COma2AgentData::COma2AgentData() :
    iDcf( NULL ),
    iDataPosition( 0 ),
    iRightsStatus( KErrNone ),
    iVirtualPath( KNullDesC ),
    iCache( NULL ),
    iGroupKeyUsed( EFalse ),
    iLastFileSize( 0 )
    {
    }

// -----------------------------------------------------------------------------
// COma2AgentData::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void COma2AgentData::ConstructL(
    const TVirtualPathPtr& aVirtualPath,
    TContentShareMode aShareMode )
    {
    User::LeaveIfError( iRdb.Connect() );
    User::LeaveIfError( iFs.Connect() );
    User::LeaveIfError( iFs.ShareAuto() );
    User::LeaveIfError( iFile.Open( iFs, aVirtualPath.URI(),
        MapContentShareMode( aShareMode ) ) );
    iDcf = CDcfCommon::NewL( iFile );
    iVirtualPath = aVirtualPath;
    User::LeaveIfError( iDcf->OpenPart( iVirtualPath.UniqueId() ) );
    InitializeL();
    }

// -----------------------------------------------------------------------------
//
void COma2AgentData::ConstructL(
    RFile& aFile,
    const TDesC& aUniqueId )
    {
    User::LeaveIfError( iRdb.Connect() );
    User::LeaveIfError( iFs.Connect() );
    User::LeaveIfError( iFs.ShareAuto() );
    User::LeaveIfError( iFile.Duplicate( aFile ) );
    iDcf = CDcfCommon::NewL( iFile );
    User::LeaveIfError( iDcf->OpenPart( aUniqueId ) );
    InitializeL();
    iUniqueId = aUniqueId.AllocL();
    }

// -----------------------------------------------------------------------------
// COma2AgentData::InitializeL
// -----------------------------------------------------------------------------
//
void COma2AgentData::InitializeL()
    {
    TInt r = KErrNone;
    TBuf8<KDCFKeySize * 2> blocks;
    CDrmProtectedRoParser* parser;
    TInt pos;
    COma2Dcf* dcf2 = NULL;
    TInt i;
    TBool reinit = EFalse;
    TInt error = KErrNone;

    if ( iDcf->iVersion == EOma2Dcf )
        {
        dcf2 = static_cast<COma2Dcf*> ( iDcf );
        // Encrypted images are not supported, but plain ones are:
        if ( dcf2->iMimeType->Left( KMimeImage().Length() ).CompareF(
            KMimeImage ) == 0 && iDcf->iEncryptionMethod != EMethodNULL )
            {
            User::Leave( KErrNotSupported );
            }
        }

    // Set the session key for this file
    if ( iDcf->iEncryptionMethod == EMethodAES_128_CBC )
        {
        // Check if we can use the group key first
        if ( dcf2 && dcf2->iGroupId )
            {
            r = iRdb.InitializeGroupKey( *dcf2->iGroupId, *dcf2->iGroupKey,
                dcf2->iGkEncryptionMethod );
            if ( r == KErrNone )
                {
                iGroupKeyUsed = ETrue;
                }
            }
        else
            {
            r = iRdb.InitializeKey( *iDcf->iContentID );
            }

        // If the DCF contains a domain RO, try to save and use it
        if ( dcf2 && dcf2->iRightsObjects.Count() )
            {
            error = r;
            __UHEAP_MARK;
            for ( i = 0; i < dcf2->iRightsObjects.Count(); i++ )
                {
                RPointerArray<CDRMRights> rights;
                CleanupResetAndDestroyPushL( rights );
                parser = CDrmProtectedRoParser::NewL();
                CleanupStack::PushL( parser );
                // NOTE:
                // This is trapped for a reason, the file opening must not fail
                // even if the parsing of the embedded rights object fails
                // for some reason it is possible that the rights are already
                // put into the database and the leave occurs because of this,
                // also it's possible that there are existing rights
                // that work with the given content.
                TRAP( r, parser->ParseAndStoreL( *dcf2->iRightsObjects[i],
                    rights) );
                CleanupStack::PopAndDestroy( parser );
                CleanupStack::PopAndDestroy( &rights );
                if ( r == KErrNone && !reinit )
                    {
                    reinit = ETrue;
                    }
                }
            __UHEAP_MARKEND;
            if ( reinit && !iGroupKeyUsed )
                {
                r = iRdb.InitializeKey( *iDcf->iContentID );
                }
            else
                {
                r = error;
                }
            }

        if ( r != KErrCANoRights && r != KErrCANoPermission &&
            r != KErrCAPendingRights && r != KErrNone )
            {
            User::Leave( r );
            }
        iRightsStatus = r;
        }
    else if ( iDcf->iEncryptionMethod != EMethodNULL )
        {
        User::Leave( KErrNotSupported );
        }

    iCache = CDcfCache::NewL( iRdb, iFile, *iDcf, 0, 0 );
    if ( iDcf->iPadding == -1 && iRightsStatus == KErrNone )
        {
        pos = iDcf->iOffset + iDcf->iDataLength - 2 * KDCFKeySize;
        iFile.Seek( ESeekStart, pos );
        iFile.Read( blocks );
        iDcf->iPadding = iRdb.CalculatePadding( blocks );
        if ( iDcf->iPadding >= 0 )
            {
            iDcf->iPlainTextLength -= iDcf->iPadding;
            iDcf->iPlainTextLengthValid = ETrue;
            }
        }
    User::LeaveIfError( iFile.Size( iLastFileSize ) );
    }

// -----------------------------------------------------------------------------
// COma2AgentData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
COma2AgentData* COma2AgentData::NewL(
    const TVirtualPathPtr& aVirtualPath,
    TContentShareMode aShareMode )
    {
    COma2AgentData* self = NewLC( aVirtualPath, aShareMode );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// COma2AgentData::NewLC
// -----------------------------------------------------------------------------
//
COma2AgentData* COma2AgentData::NewLC(
    const TVirtualPathPtr& aVirtualPath,
    TContentShareMode aShareMode )
    {
    COma2AgentData* self = new ( ELeave ) COma2AgentData();
    CleanupStack::PushL( self );
    self->ConstructL( aVirtualPath, aShareMode );
    return self;
    }

// -----------------------------------------------------------------------------
// COma2AgentData::NewL
// -----------------------------------------------------------------------------
//
COma2AgentData* COma2AgentData::NewL(
    RFile& aFile,
    const TDesC& aUniqueId )
    {
    COma2AgentData* self = COma2AgentData::NewLC( aFile, aUniqueId );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// COma2AgentData::NewLC
// -----------------------------------------------------------------------------
//
COma2AgentData* COma2AgentData::NewLC(
    RFile& aFile,
    const TDesC& aUniqueId )
    {
    COma2AgentData* self = new ( ELeave ) COma2AgentData();
    CleanupStack::PushL( self );
    self->ConstructL( aFile, aUniqueId );
    return self;
    }

// Destructor
COma2AgentData::~COma2AgentData()
    {
    iRdb.Close();
    iFile.Close();
    iFs.Close();
    delete iDcf;
    delete iCache;
    delete iUniqueId;
    }

// -----------------------------------------------------------------------------
// COma2AgentData::DataSizeL
// Re-create the DCF because it's size may have changed.
// for progressive download etc.
// -----------------------------------------------------------------------------
//
void COma2AgentData::DataSizeL( TInt &aSize )
    {
    TInt fileSize = 0;
    TInt seekPos = 0;

    User::LeaveIfError( iFile.Size( fileSize ) );

    if ( iDcf )
        {
        // if the size of the file has changed update info:
        if ( fileSize != iLastFileSize )
            {
            // Delete the old dcf
            delete iDcf;
            iDcf = NULL;

            // seek the file to the beginning
            iFile.Seek( ESeekStart, seekPos );

            // Create new dcf as the size may have changed
            iDcf = CDcfCommon::NewL( iFile );

            // open the content part
            if ( iUniqueId )
                {
                User::LeaveIfError( iDcf->OpenPart( iUniqueId->Des() ) );
                }
            else
                {
                User::LeaveIfError( iDcf->OpenPart( iVirtualPath.UniqueId() ) );
                }
            // re-initialize the dcf, this updates the iLastFileSize
            InitializeL();
            }

        aSize = iDcf->iPlainTextLength;
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    }

// -----------------------------------------------------------------------------
// COma2AgentData::EvaluateIntent
// -----------------------------------------------------------------------------
//
TInt COma2AgentData::EvaluateIntent( TIntent aIntent )
    {
    TInt r = KErrNone;

    // Check for a mismatch in the MIME type and the intent
    if ( ( iDcf->iMimeType->Left(
            KMimeAudio().Length() ).CompareF( KMimeAudio ) == 0 &&
            ( aIntent == EView || aIntent == EPrint || aIntent == EExecute ) )
        ||
        ( iDcf->iMimeType->Left(
            KMimeImage().Length() ).CompareF( KMimeImage ) == 0 &&
            ( aIntent == EPlay || aIntent == EExecute ) ) )
        {
        r = KErrArgument;
        }
    else if ( iDcf->iEncryptionMethod )
        {
        if ( iGroupKeyUsed )
            {
            r = iRdb.CheckConsume( aIntent,
                *( static_cast<COma2Dcf*> ( iDcf )->iGroupId ) );
            }
        else
            {
            r = iRdb.CheckConsume( aIntent, *iDcf->iContentID );
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentData::ExecuteIntent
// -----------------------------------------------------------------------------
//
TInt COma2AgentData::ExecuteIntent( TIntent aIntent )
    {
    TInt r = KErrNone;
    TBuf8<KDCFKeySize> key;

    // Check for a mismatch in the MIME type and the intent
    if ( ( iDcf->iMimeType->Left(
            KMimeAudio().Length() ).CompareF( KMimeAudio ) == 0 &&
            ( aIntent == EView || aIntent == EPrint || aIntent == EExecute ) )
        ||
        ( iDcf->iMimeType->Left(
            KMimeImage().Length() ).CompareF( KMimeImage ) == 0 &&
            ( aIntent == EPlay || aIntent == EExecute ) ) )
        {
        r = KErrArgument;
        }
    else if ( iDcf->iEncryptionMethod )
        {
        // If the MIME type allows it, do the consumption
        if ( iGroupKeyUsed )
            {
            r = iRdb.Consume( aIntent,
                *( static_cast<COma2Dcf*> ( iDcf )->iGroupId ) );
            }
        else
            {
            r = iRdb.Consume( aIntent, *iDcf->iContentID );
            }

        // If the consumption succeeded, try to get the decryption key.
        // This is possible at this time because the permission is now known on
        // the server side, and the server can return the key if the
        // security level allows it. The decision is made based on the
        // used permission and the caller identity.
        if ( r == KErrNone && iRdb.GetDecryptionKey( aIntent,
            *iDcf->iContentID, key ) == KErrNone && key.Length()
            == KDCFKeySize )
            {
            iCache->SetKey( key );
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentData::Read
// -----------------------------------------------------------------------------
//
TInt COma2AgentData::Read( TDes8& aDes )
    {
    return Read( aDes, aDes.MaxLength() );
    }

// -----------------------------------------------------------------------------
// COma2AgentData::Read
// -----------------------------------------------------------------------------
//
TInt COma2AgentData::Read(
    TDes8& aDes,
    TInt aLength )
    {
    return iCache->Read( iDataPosition, aDes, aLength );
    }

// -----------------------------------------------------------------------------
// COma2AgentData::Read
// -----------------------------------------------------------------------------
//
void COma2AgentData::Read(
    TDes8& aDes,
    TRequestStatus& aStatus )
    {
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, Read( aDes, aDes.MaxLength() ) );
    }

// -----------------------------------------------------------------------------
// COma2AgentData::Read
// asynchronous read
// -----------------------------------------------------------------------------
//
void COma2AgentData::Read(
    TDes8& aDes,
    TInt aLength,
    TRequestStatus& aStatus )
    {
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, Read( aDes, aLength ) );
    }

#ifdef ASYNC_READ

// -----------------------------------------------------------------------------
// COma2AgentData::Read
// asynchronous read (not really asynchronous)
// -----------------------------------------------------------------------------
//
TInt COma2AgentData::Read(
    TInt aPos,
    TDes8& aDes,
    TInt aLength,
    TRequestStatus& aStatus )
    {
    return iCache->Read( aPos, aDes, aLength, aStatus );
    }

// -----------------------------------------------------------------------------
// COma2AgentData::ReadCancel
// -----------------------------------------------------------------------------
//
void COma2AgentData::ReadCancel( TRequestStatus& aStatus )
    {
    TRACE("+ COma2AgentData::ReadCancel");
    iCache->ReadCancel( aStatus );
    TRACE("- COma2AgentData::ReadCancel");
    }
#endif

// -----------------------------------------------------------------------------
// COma2AgentData::Seek
// -----------------------------------------------------------------------------
//
TInt COma2AgentData::Seek( TSeek aMode, TInt& aPos )
    {
    TInt r = KErrNone;

    switch ( aMode )
        {
        case ESeekStart:
            break;
        case ESeekEnd:
            aPos = iDcf->iPlainTextLength + aPos;
            break;
        case ESeekCurrent:
            aPos = iDataPosition + aPos;
            break;
        default:
            r = KErrNotSupported;
            break;
        }
    aPos = Max( aPos, 0 );
    aPos = Min( aPos, iDcf->iPlainTextLength );
    iDataPosition = aPos;
    return r;
    }

// -----------------------------------------------------------------------------
// COma2AgentData::SetProperty
// -----------------------------------------------------------------------------
//
TInt COma2AgentData::SetProperty(
    TAgentProperty /*aProperty*/,
    TInt /*aValue*/)
    {
    return KErrCANotSupported;
    }

// -----------------------------------------------------------------------------
// COma2AgentData::GetAttribute
// -----------------------------------------------------------------------------
//
TInt COma2AgentData::GetAttribute(
    TInt aAttribute,
    TInt& aValue )
    {
    TInt ret( KErrCANotSupported );
    ret = TOma2AgentAttributes::GetAttribute( *iDcf, aAttribute,
        iVirtualPath, &iRdb );
    if ( ret >= KErrNone )
        {
        aValue = ret;
        ret = KErrNone;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// COma2AgentData::GetAttributeSet
// -----------------------------------------------------------------------------
//

TInt COma2AgentData::GetAttributeSet(
    RAttributeSet& aAttributeSet )
    {
    return TOma2AgentAttributes::GetAttributeSet( *iDcf, aAttributeSet,
        iVirtualPath, &iRdb );
    }

// -----------------------------------------------------------------------------
// COma2AgentData::GetStringAttribute
// -----------------------------------------------------------------------------
//

TInt COma2AgentData::GetStringAttribute(
    TInt aAttribute,
    TDes& aValue )
    {
    return TOma2AgentAttributes::GetStringAttribute( *iDcf, aAttribute,
        aValue, iVirtualPath, &iRdb );
    }

// -----------------------------------------------------------------------------
// COma2AgentData::GetStringAttributeSet
// -----------------------------------------------------------------------------
//

TInt COma2AgentData::GetStringAttributeSet(
    RStringAttributeSet& aStringAttributeSet )
    {
    return TOma2AgentAttributes::GetStringAttributeSet( *iDcf,
        aStringAttributeSet, iVirtualPath, &iRdb );
    }

//  End of File
