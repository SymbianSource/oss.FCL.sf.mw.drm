/*
* Copyright (c) 2002 - 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ROAP message signer class implementation
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <f32file.h>
#include <s32file.h>
#include <hash.h>
#include <signed.h>
#include <asymmetric.h>
#include <hash.h>
#include <asn1enc.h>
#include <x509cert.h>
#include "Base64.h"
#include "RoapSigner.h"
#include "RoapStorageClient.h"
#include "RoapLog.h"

using namespace Roap;

// CONSTANTS
_LIT8( KSignatureStart, "<signature>" );
_LIT8( KSignatureEnd, "</signature>" );
_LIT8( KRoEndWithPrefix, "</roap:protectedRO>" );
_LIT8( KRoEnd, "</protectedRO>" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RoapSigner::RoapSigner
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRoapSigner::CRoapSigner():
    iHash( NULL ),
    iStorage( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CRIHello::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRoapSigner::ConstructL( RRoapStorageClient& aStorageClient )
    {
    iHash = CSHA1::NewL();
    iStorage = &aStorageClient;
    }


// Destructor
CRoapSigner::~CRoapSigner()
    {
    iRequests.ResetAndDestroy();
    iResponses.ResetAndDestroy();
    delete iHash;
    }

// -----------------------------------------------------------------------------
// RoapSigner::
//
// -----------------------------------------------------------------------------
//
CRoapSigner* CRoapSigner::NewL( RRoapStorageClient& aStorageClient )
    {
    CRoapSigner* self = new ( ELeave ) CRoapSigner();

    CleanupStack::PushL( self );
    self->ConstructL( aStorageClient );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CRoapSigner::ResetRequests
// -----------------------------------------------------------------------------
//
void CRoapSigner::ResetRequests( void )
    {
    iRequests.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CRoapSigner::ResetResponses
// -----------------------------------------------------------------------------
//
void CRoapSigner::ResetResponses( void )
    {
    iResponses.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CRoapSigner::AddResponseL
// -----------------------------------------------------------------------------
//
void CRoapSigner::AddResponseL(
    const TDesC8& aResponse )
    {
    HBufC8* b = aResponse.AllocLC();
    iResponses.AppendL( b );
    CleanupStack::Pop( b );
    }


// -----------------------------------------------------------------------------
// CRoapSigner::VerifyAndAddResponseL
// -----------------------------------------------------------------------------
//
TBool CRoapSigner::VerifyAndAddResponseL(
    const TDesC8& aResponse,
    const TDesC8& aSignature,
    const RPointerArray<HBufC8>& aCertificateChain )
    {
    TBool r = ETrue;
    TInt i( 0 );
    TInt startPoint = 0;
    TInt endPoint = 0;
    TInt roEnd = 0;
    TPtr8 ptr( 0, 0 );
    TPtrC8 hash( 0, 0 );
    HBufC8* tempMessage = NULL;
    TInt lastRoEnd( 0 );

    LOGLIT( "CRoapSigner::VerifyAndAddResponseL" );

    // locate the last protectedRO endtag - there could be also <signature> elemet
    // inside the ro element
    while ( lastRoEnd >= 0 )
        {
        TInt tagIncrement( KRoEndWithPrefix().Length() );
        roEnd += lastRoEnd;
        TPtrC8 startPtr( aResponse.Mid( roEnd ) );
        lastRoEnd = startPtr.Find( KRoEndWithPrefix );
        if ( lastRoEnd == KErrNotFound )
            {
            // try again without namespace prefix
            lastRoEnd = startPtr.Find( KRoEnd );
            tagIncrement = KRoEnd().Length();
            }
        if ( lastRoEnd > 0 )
            {
            lastRoEnd += tagIncrement;
            }
        DETAILLOG2( _L( "lastRoEnd %08x:" ), lastRoEnd );
        }
    DETAILLOG2( _L( "roEnd %08x:" ), roEnd );
    if ( roEnd > 0 )
        {
        startPoint = aResponse.Right( aResponse.Length() - roEnd ).Find( KSignatureStart );
        startPoint += roEnd;
        endPoint = aResponse.Right( aResponse.Length() - roEnd ).Find( KSignatureEnd )
                                   + KSignatureEnd().Length();
        endPoint += roEnd;
        }
    else
        {
        startPoint = aResponse.Find( KSignatureStart );
        endPoint = aResponse.Find( KSignatureEnd ) + KSignatureEnd().Length();
        }

    tempMessage = HBufC8::NewMax( aResponse.Length() -
                                 2 * KSignatureStart().Length() );
    User::LeaveIfNull( tempMessage );
    ptr.Set( tempMessage->Des() );
    ptr.Copy( aResponse.Left( startPoint ) );
    ptr.Append( aResponse.Right( aResponse.Length() - endPoint ) );

    CleanupStack::PushL( tempMessage );
    iResponses.AppendL ( tempMessage );
    CleanupStack::Pop( tempMessage );
    iHash->Reset();
    for ( i = 0; i < iResponses.Count(); i++ )
        {
        DETAILLOG2( _L( "Message %d:" ), i );
        DETAILLOGHEX( iResponses[ i ]->Ptr(), iResponses[ i ]->Length() );
        iHash->Update( *iResponses[ i ] );
        }

    hash.Set( iHash->Final() );

    LOGLIT( "Hash" );
    LOGHEX( hash.Ptr(), hash.Length() );
    LOGLIT( "Signature" );
    LOGHEX( aSignature.Ptr(), aSignature.Length() );

    r= iStorage->VerifyL( aSignature, hash, aCertificateChain );

#ifdef _ROAP_TESTING
    if ( r )
        {
        LOGLIT( "Signature verification ok." );
        }
    else
        {
        LOGLIT( "Signature verification failed." );
        }
#endif

    return r;
    }


// -----------------------------------------------------------------------------
// CRoapSigner::AddRequestL
// -----------------------------------------------------------------------------
//
void CRoapSigner::AddRequestL(
    const TDesC8& aRequest )
    {
    HBufC8* b = aRequest.AllocLC();
    iRequests.AppendL( b );
    CleanupStack::Pop( b );
    }


// -----------------------------------------------------------------------------
// CRoapSigner::SignAndAddRequestL
// -----------------------------------------------------------------------------
//
HBufC8* CRoapSigner::SignAndAddRequestL(
    const TDesC8& aRequest )
    {
    TInt i;
    TInt insertPoint;
    HBufC8* s;
    HBufC8* r;
    TPtr8 ptr( 0, 0 );
    HBufC8* signature = NULL;
    HBufC8* tempMessage = NULL;
    TPtrC8 hash( 0, 0 );

    LOGLIT( "CRoapSigner::SignAndAddRequestL" );

    insertPoint = aRequest.Find( KSignatureStart ) + KSignatureStart().Length();
    tempMessage = HBufC8::NewMax( aRequest.Length() -
        2 * KSignatureStart().Length() );
    User::LeaveIfNull( tempMessage );
    ptr.Set( tempMessage->Des() );
    ptr.Copy( aRequest.Left( insertPoint - KSignatureStart().Length() ) );
    ptr.Append( aRequest.Right( aRequest.Length() - insertPoint -
        KSignatureStart().Length() - 1 ) );

    iHash->Reset();
    for ( i = 0; i < iRequests.Count(); i++ )
        {
        DETAILLOG2( _L( "Message %d:" ), i );
        DETAILLOGHEX( iRequests[ i ]->Ptr(), iRequests[ i ]->Length() );
        iHash->Update( *iRequests[ i ] );
        }
    DETAILLOG2( _L( "Message %d:" ), i );
    DETAILLOGHEX( tempMessage->Ptr(), tempMessage->Length() );
    iHash->Update( *tempMessage );
    delete tempMessage;
    hash.Set( iHash->Final() );
    LOGLIT( "Hash" );
    LOGHEX( hash.Ptr(), hash.Length() );

    iStorage->SignL( hash, signature );
    CleanupStack::PushL( signature );

    LOGLIT( "Signature" );
    LOGHEX( signature->Ptr(), signature->Length() );

    s = Base64EncodeL( *signature );
    CleanupStack::PushL( s );
    r = HBufC8::NewMax( s->Length() + aRequest.Length() );
    User::LeaveIfNull( r );
    ptr.Set( r->Des() );
    ptr.Copy( aRequest.Left( insertPoint ) );
    ptr.Append( *s );
    ptr.Append( aRequest.Right( aRequest.Length() - insertPoint ) );
    CleanupStack::PopAndDestroy( s );
    CleanupStack::PopAndDestroy( signature );

    CleanupStack::PushL( r );
    iRequests.AppendL ( r->Des().AllocL() );
    CleanupStack::Pop( r );
    return r;
    }

//  End of File
