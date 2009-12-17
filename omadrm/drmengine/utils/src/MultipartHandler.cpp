/*
* Copyright (c) 2005 - 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  utility class for handling multipart/related HTTP responses
*                in ROAP
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <hash.h>
#include "Base64.h"
#include "MultipartHandler.h"

#ifdef _DEBUG

#include <e32debug.h>
#define MULTIPART_DEBUGLIT( a ) RDebug::Print( _L ( a ) );

#else // DEBUG

#define MULTIPART_DEBUGLIT( a )

#endif // DEBUG

// CONSTANTS

_LIT8( KBoundaryPrefix, "--" );
_LIT8( KCrLf, "\r\n" );
_LIT8( KCrLfCrLf, "\r\n\r\n" );


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// TMultipartHandler::TMultipartHandler
// ---------------------------------------------------------------------------
//
TMultipartHandler::TMultipartHandler():
    iState( EPreamble )
    {
    MULTIPART_DEBUGLIT( "TMultipartHandler::TMultipartHandler -->" )
    iBuffer.SetLength( 0 );
    MULTIPART_DEBUGLIT( "--> TMultipartHandler::TMultipartHandler" )
    }

// ---------------------------------------------------------------------------
// TMultipartHandler::WriteL
// ---------------------------------------------------------------------------
//
void TMultipartHandler::WriteL(
    const TDesC8& aData )
    {
    MULTIPART_DEBUGLIT( "TMultipartHandler::WriteL -->" )
    switch ( iState )
        {
        case EPreamble:
            MULTIPART_DEBUGLIT( "EPreamble" )
            HandlePreambleL( aData );
            break;
        case EBodyPartHeaders:
            MULTIPART_DEBUGLIT( "EBodyPartHeaders" )
            HandleBodyPartHeadersL( aData );
            break;
        case EBodyPart:
            MULTIPART_DEBUGLIT( "EBodyPart" )
            HandleBodyPartL( aData );
            break;
        case EEpilog:
            MULTIPART_DEBUGLIT( "EEpilog" )
            break;
        }
    MULTIPART_DEBUGLIT( "--> TMultipartHandler::WriteL" )
    }

// ---------------------------------------------------------------------------
// TMultipartHandler::FindDashBoundary
// ---------------------------------------------------------------------------
//
TBool TMultipartHandler::FindDashBoundary(
    const TDesC8& aBuffer,
    TInt& aStart,
    TInt& aEnd )
    {
    MULTIPART_DEBUGLIT( "TMultipartHandler::FindDashBoundary -->" )
    TBool r( EFalse );
    const TInt KBoundaryPrefixLength( KBoundaryPrefix().Length() );
    const TInt KCrLfLength( KCrLf().Length() );
    aStart = aBuffer.Find( KBoundaryPrefix );
    if ( aStart >= 0 )
        {
        MULTIPART_DEBUGLIT( "TMultipartHandler::FindDashBoundary aStart nonnegative" )
        aEnd = aBuffer.Mid( aStart ).Find( KCrLf );
        aEnd += aStart;
        if ( aEnd > aStart + KBoundaryPrefixLength )
            {
            MULTIPART_DEBUGLIT( "TMultipartHandler::FindDashBoundary  aEnd > aStart + (boundary prefix length)" )
            const TInt delimiterLength( aEnd - aStart + KCrLfLength );
            __ASSERT_ALWAYS( KMaxBoundaryLength  >= delimiterLength, User::Invariant() );
            iDelimiter.Copy( KCrLf );
            iDelimiter.Append( KBoundaryPrefix );
            iDelimiter.Append( aBuffer.Mid(
                aStart + KBoundaryPrefixLength,
                aEnd - aStart - KBoundaryPrefixLength ) );
            r = ETrue;
            MULTIPART_DEBUGLIT( "TMultipartHandler::FindDashBoundary  found dash boundary" )
            }
        }
    MULTIPART_DEBUGLIT( "--> TMultipartHandler::FindDashBoundary" )
    return r;
    }

// ---------------------------------------------------------------------------
// TMultipartHandler::FindHeaderEnd
// ---------------------------------------------------------------------------
//
TBool TMultipartHandler::FindHeaderEnd(
    const TDesC8& aBuffer,
    TInt& aStart,
    TInt& aEnd )
    {
    MULTIPART_DEBUGLIT( "TMultipartHandler::FindHeaderEnd -->" )
    TBool r( EFalse );

    aStart = aBuffer.Find( KCrLfCrLf );
    if ( aStart >= 0 )
        {
        aEnd = aStart + KCrLfCrLf().Length();
        r = ETrue;
        }
    MULTIPART_DEBUGLIT( "--> TMultipartHandler::FindHeaderEnd" )
    return r;
    }

// ---------------------------------------------------------------------------
// TMultipartHandler::FindDelimiter
// ---------------------------------------------------------------------------
//
TBool TMultipartHandler::FindDelimiter(
    const TDesC8& aBuffer,
    TInt& aStart,
    TInt& aEnd )
    {
    MULTIPART_DEBUGLIT( "TMultipartHandler::FindDelimiter -->" )
    TBool r( EFalse );

    aStart = aBuffer.Find( iDelimiter );
    if ( aStart >= 0 )
        {
        aEnd = aStart + iDelimiter.Length();
        r = ETrue;
        }
    MULTIPART_DEBUGLIT( "--> TMultipartHandler::FindDelimiter" )
    return r;
    }

// ---------------------------------------------------------------------------
// TMultipartHandler::HandlePreambleL
// ---------------------------------------------------------------------------
//
void TMultipartHandler::HandlePreambleL(
    const TDesC8& aData )
    {
    MULTIPART_DEBUGLIT( "TMultipartHandler::HandlePreambleL -->" )
    TInt bStart( 0 );
    TInt bEnd( 0 );
    HBufC8* buffer( NULL );

    buffer = HBufC8::NewLC( iBuffer.Length() + aData.Length() );
    buffer->Des().Copy( iBuffer );
    buffer->Des().Append( aData );
    if ( !FindDashBoundary( *buffer, bStart, bEnd ) )
        {
        iBuffer.Copy( buffer->Right( Min(
                    KMaxBoundaryLength, buffer->Length() ) ) );
        }
    else
        {
        iBuffer.SetLength( 0 );
        iState = EBodyPartHeaders;
        StartBodyPartHeadersL();
        HandleBodyPartHeadersL( buffer->Mid( bEnd + KCrLf().Length() ) );
        }
    CleanupStack::PopAndDestroy( buffer );
    MULTIPART_DEBUGLIT( "--> TMultipartHandler::HandlePreambleL" )
    }

// ---------------------------------------------------------------------------
// TMultipartHandler::HandleBodyPartHeadersL
// ---------------------------------------------------------------------------
//
void TMultipartHandler::HandleBodyPartHeadersL(
    const TDesC8& aData )
    {
    MULTIPART_DEBUGLIT( "TMultipartHandler::HandleBodyPartHeadersL -->" )
    TInt bStart( 0 );
    TInt bEnd( 0 );
    HBufC8* buffer( HBufC8::NewLC( iBuffer.Length() + aData.Length() ) );
    buffer->Des().Copy( iBuffer );
    buffer->Des().Append( aData );
    iBuffer.Copy( buffer->Right( Min(
                KMaxBoundaryLength, buffer->Length() ) ) );
    if ( !FindHeaderEnd( *buffer, bStart, bEnd ) )
        {
        const TInt lengthNoBoundary( buffer->Length() - KMaxBoundaryLength );
        if (  lengthNoBoundary > 0 )
            {
            HandleBodyHeaderDataL( buffer->Left( lengthNoBoundary ) );
            }
        }
    else
        {
        HandleBodyHeaderDataL( buffer->Left( bStart ) );
        iBuffer.SetLength( 0 );
        iState = EBodyPart;
        StartBodyPartL();
        HandleBodyPartL( buffer->Mid( bEnd ) );
        }
    CleanupStack::PopAndDestroy( buffer );
    MULTIPART_DEBUGLIT( "--> TMultipartHandler::HandleBodyPartHeadersL" )
    }

// ---------------------------------------------------------------------------
// TMultipartHandler::HandleBodyPartL
// ---------------------------------------------------------------------------
//
void TMultipartHandler::HandleBodyPartL(
    const TDesC8& aData )
    {
    MULTIPART_DEBUGLIT( "TMultipartHandler::HandleBodyPartL -->" )
    TInt bStart( 0 );
    TInt bEnd( 0 );

    HBufC8* buffer( HBufC8::NewLC( iBuffer.Length() + aData.Length() ) );
    buffer->Des().Copy( iBuffer );
    buffer->Des().Append( aData );

    iBuffer.Copy( buffer->Right( Min(
                KMaxBoundaryLength, buffer->Length() ) ) );

    if ( !FindDelimiter( *buffer, bStart, bEnd ) )
        {
        const TInt lengthNoBoundary( buffer->Length() - KMaxBoundaryLength );
        if ( lengthNoBoundary > 0 )
            {
            HandleBodyDataL( buffer->Left( lengthNoBoundary ) );
            }
        }
    else
        {
        HandleBodyDataL( buffer->Left( bStart ) );
        iBuffer.SetLength( 0 );
        if ( ( *buffer )[ bEnd ] == '-' && ( *buffer )[ bEnd + 1 ] == '-' )
            {
            EndBodyPartL();
            iState = EEpilog;
            }
        else
            {
            iState = EBodyPartHeaders;
            StartBodyPartHeadersL();
            HandleBodyPartHeadersL( buffer->Mid( bEnd + KCrLf().Length() ) );
            }
        }
    CleanupStack::PopAndDestroy( buffer );
    MULTIPART_DEBUGLIT( "--> TMultipartHandler::HandleBodyPartL" )
    }

// ---------------------------------------------------------------------------
// TMultipartHandler::StartBodyPartHeadersL
// ---------------------------------------------------------------------------
//
void TMultipartHandler::StartBodyPartHeadersL()
    {
    MULTIPART_DEBUGLIT( "--> TMultipartHandler::StartBodyPartHeadersL -->" )
    }

// ---------------------------------------------------------------------------
// TMultipartHandler::EndBodyPartHeadersL
// ---------------------------------------------------------------------------
//
void TMultipartHandler::EndBodyPartHeadersL()
    {
    MULTIPART_DEBUGLIT( "--> TMultipartHandler::EndBodyPartHeadersL -->" )
    }

// ---------------------------------------------------------------------------
// TMultipartHandler::HandleBodyHeaderDataL
// ---------------------------------------------------------------------------
//
void TMultipartHandler::HandleBodyHeaderDataL(
    const TDesC8& /*aData*/ )
    {
    MULTIPART_DEBUGLIT( "--> TMultipartHandler::HandleBodyHeaderDataL -->" )
    }

// ---------------------------------------------------------------------------
// TMultipartHandler::StartBodyPartL
// ---------------------------------------------------------------------------
//
void TMultipartHandler::StartBodyPartL()
    {
    MULTIPART_DEBUGLIT( "--> TMultipartHandler::StartBodyPartL -->" )
    }

// ---------------------------------------------------------------------------
// TMultipartHandler::EndBodyPartL
// ---------------------------------------------------------------------------
//
void TMultipartHandler::EndBodyPartL()
    {
    MULTIPART_DEBUGLIT( "--> TMultipartHandler::EndBodyPartL -->" )
    }

// ---------------------------------------------------------------------------
// TMultipartHandler::HandleBodyDataL
// ---------------------------------------------------------------------------
//
void TMultipartHandler::HandleBodyDataL(
    const TDesC8& /*aData*/ )
    {
    MULTIPART_DEBUGLIT( "--> TMultipartHandler::HandleBodyDataL -->" )
    }
