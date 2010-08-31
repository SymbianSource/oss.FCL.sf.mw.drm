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
* Description:  Symbian recognizer for DRM protected files
*
*/


// INCLUDE FILES
#include <apmrec.h>
#include <apmstd.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

#include "RecDRM.h"

#define RECOGNIZE_KEY_CHAIN

// CONSTANTS
const TUid KUidDRMRecognizer={ 0x101F51F4 };

// Version must be 1 so that it conforms to OMA DRM 1.0 specification
const TInt KDCFVersionSupported( 1 );

// minimum size of supported mime-type
const TInt KMinContentTypeLen( 3 );

// maximum amount of buffer space we will ever use
const TInt KMaxBufferLength=256;

const TInt KDCFHeaderLength=3;
const TInt KLengthBoxSize = 4;
const TInt KLengthBoxType = 4;
const TInt KLengthBoxSize64 = 8;
const TInt KLengthVersion = 1;
const TInt KLengthFlags = 3;

_LIT8(KFTypPrefix, "ftyp");
_LIT8(KODFPrefix, "odcf");
_LIT8(KRoapTriggerElement, "roap-trigger:roapTrigger"); // before OMA spec CR, ROAP Trigger namespace prefix was roap-trigger
_LIT8(KRoapTriggerElement2, "roap:roapTrigger");
_LIT8(KRoapTriggerType, "application/vnd.oma.drm.roap-trigger+xml");

const TImplementationProxy ImplementationTable[] =
        {
        IMPLEMENTATION_PROXY_ENTRY(0x101F6DB8, CApaDRMRecognizer::CreateRecognizerL)
        };

#ifdef DRM_OMA2_ENABLED
TUint32 ReadUint32FromBlock(const TDesC8& aBlock, TInt aOffset)
    {
    return (aBlock[aOffset] << 24) +
        (aBlock[aOffset + 1] << 16) +
        (aBlock[aOffset + 2] << 8) +
        aBlock[aOffset + 3];
    }

TInt ReadIntFromBlock(const TDesC8& aBlock, TInt aOffset)
    {
    return (aBlock[aOffset] << 24) +
        (aBlock[aOffset + 1] << 16) +
        (aBlock[aOffset + 2] << 8) +
        aBlock[aOffset + 3];
    }

TUint16 ReadUint16FromBlock(const TDesC8& aBlock, TInt aOffset)
    {
    return ((aBlock[aOffset] << 8) + aBlock[aOffset + 1]);
    }
#endif

CApaDRMRecognizer::CApaDRMRecognizer():
    CApaDataRecognizerType( KUidDRMRecognizer,CApaDataRecognizerType::ENormal )
{

    iCountDataTypes = 0;
    return;
}

CApaDRMRecognizer::~CApaDRMRecognizer()
{
}


CApaDataRecognizerType* CApaDRMRecognizer::CreateRecognizerL()
{
    return new (ELeave) CApaDRMRecognizer ();
}


TUint CApaDRMRecognizer::PreferredBufSize()
{
    return KMaxBufferLength;
}

#ifdef _DEBUG
TDataType CApaDRMRecognizer::SupportedDataTypeL( TInt aIndex ) const
#else
TDataType CApaDRMRecognizer::SupportedDataTypeL( TInt /*aIndex*/ ) const
#endif
{
__ASSERT_DEBUG( aIndex >= 0 && aIndex < iCountDataTypes, User::Invariant() );
    return TDataType( _L8("application/vdn.omd.drm.content") ); // this should never be run
}

void CApaDRMRecognizer::DoRecognizeL( const TDesC& aName, const TDesC8& aBuffer )
{
    if ( aBuffer.Size() < 3)
        {
        return;
        }

#ifdef RECOGNIZE_KEY_CHAIN
    // Recognize device key chain
    if ( aName.Length() > 3 && aName.Right(4).CompareF(_L(".dkc")) == 0)
        {
        iConfidence = ECertain;
        iDataType = TDataType( _L8("application/x-device-key-chain") );
        return;
        }
#endif

#ifdef DRM_OMA2_ENABLED
    // Recognize ROAP Trigger
    if ( RecognizeRoapTrigger( aBuffer ) )
        {
        return;
        }

    // Recognize DCFv2
    if ( RecognizeODF( aBuffer ) )
        {
        return;
        }
#endif
    // Recognize DCFv1
    TUint8 version = aBuffer[0];
    TUint8 contentTypeLen = aBuffer[1];
    TUint8 contentURILen = aBuffer[2];

    if ( contentTypeLen < KMinContentTypeLen || contentURILen == 0 )
    {
        return;
    }
    if ( version != KDCFVersionSupported )
    {
        return;
    }

    // Too little data received
    if ( aBuffer.Size() < ( contentTypeLen + KDCFHeaderLength ) )
    {
        return;
    }

    TPtrC8 mimeType = aBuffer.Mid( KDCFHeaderLength, contentTypeLen );
    if ( mimeType.Locate( '/' ) != KErrNotFound )
    {
        iConfidence = ECertain;
        iDataType=TDataType( mimeType );
    }


    return;
}

#ifdef DRM_OMA2_ENABLED
TBool CApaDRMRecognizer::RecognizeRoapTrigger( const TDesC8& aBuffer )
{
        if ( aBuffer.FindF( KRoapTriggerElement() ) != KErrNotFound
             || aBuffer.FindF( KRoapTriggerElement2() ) != KErrNotFound )
        {
            iConfidence = ECertain;
            iDataType=TDataType( KRoapTriggerType() );
            return ETrue;
        }
    return EFalse;
}

TBool CApaDRMRecognizer::RecognizeODF( const TDesC8& aBuffer )
{
    if ( aBuffer.Size() < 24 ) return EFalse;
    TPtrC8 ftypPrefix = aBuffer.Mid( 4, KFTypPrefix().Length() );
    if ( KFTypPrefix().CompareF( ftypPrefix ) == KErrNone )
    {
        TPtrC8 odfPrefix = aBuffer.Mid( 8, KODFPrefix().Length() );
        if ( KODFPrefix().CompareF( odfPrefix ) == KErrNone )
        {
            TBuf8<4> buffer;
            TUint32 size;
            TPtr8 ptr(NULL, 0);
            TUint32 offset(20);

            // ODRM box header
            buffer.Zero();
            buffer.Copy( aBuffer.Mid( offset, 4 ));
            size = ReadUint32FromBlock( buffer, 0 );
            offset += KLengthBoxSize + KLengthBoxType + KLengthVersion + KLengthFlags;

            if (size == 1)
            {
                offset += KLengthBoxSize64;
            }
            if ( aBuffer.Size() < offset+4 ) return EFalse;

            // Discrete headers box header
            buffer.Zero();
            buffer.Copy( aBuffer.Mid( offset, 4 ));
            size = ReadUint32FromBlock( buffer, 0 );
            offset += KLengthBoxSize + KLengthBoxType + KLengthVersion + KLengthFlags;
            if ( size == 1 )
            {
                offset += KLengthBoxSize64;
            }
            if ( aBuffer.Size() < offset+1 ) return EFalse;

            // Content type
            buffer.Zero();
            buffer.Copy( aBuffer.Mid( offset, 1 ));
            if ( aBuffer.Size() < offset + 1 + buffer[0] ) return EFalse;
            TPtrC8 mimeType = aBuffer.Mid( offset+1, buffer[0] );

            iConfidence = ECertain;
            iDataType=TDataType( mimeType );
            return ETrue;
        }

    }
    return EFalse;
}
#endif

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

