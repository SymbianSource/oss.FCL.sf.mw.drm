/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*     Implementation of CRoHandlerStringResourceReader
*
*
*/


// INCLUDE FILES

#include "RoHandlerStringResourceReader.h"
#include <f32file.h>
#include <barsread.h>
#include <bautils.h>


// ============================= LOCAL FUNCTIONS ===============================

//#define _DRM_TESTING

#ifdef _DRM_TESTING
LOCAL_C void WriteLogL( const TDesC8& text , RFs &aFs );
LOCAL_C void WriteFileL( const TDesC8& text , RFs &aFs , const TDesC& aName );
LOCAL_C void CreateLogL();
LOCAL_C void WriteL( const TDesC8& aText );
LOCAL_C void WriteL( const TDesC& aText );
LOCAL_C void WriteL( const TDesC8& aText , TInt aErr );
LOCAL_C void WriteCurrentTimeL();
#endif

// -----------------------------------------------------------------------------
// Testing stuff
// -----------------------------------------------------------------------------
//

#ifdef _DRM_TESTING
LOCAL_C void WriteLogL( const TDesC8& text , RFs &aFs )
    {
    _LIT( KLogFile , "c:\\StringResourceReader.txt" );
    WriteFileL( text , aFs , KLogFile );
    }

LOCAL_C void WriteFileL( const TDesC8& text , RFs &aFs , const TDesC& aName )
    {
    RFile file;
    TInt size;
    User::LeaveIfError( file.Open( aFs, aName , EFileWrite ) );
    CleanupClosePushL( file );
    User::LeaveIfError( file.Size( size ) );
    User::LeaveIfError( file.Write( size, text ) );
    CleanupStack::PopAndDestroy(); //file
    }

LOCAL_C void CreateLogL()
    {
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    RFile file;
    User::LeaveIfError( file.Replace( fs , _L("c:\\StringResourceReader.txt") , EFileWrite ) );
    file.Close();
    CleanupStack::PopAndDestroy(); //fs
    }

LOCAL_C void WriteL( const TDesC& aText )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    HBufC8* text = HBufC8::NewLC(1000);
    TPtr8 textptr(text->Des() );
    textptr.Append( aText );
    textptr.Append( _L8("\r\n") );
    WriteLogL(textptr , fs);
    CleanupStack::PopAndDestroy(text);
    CleanupStack::PopAndDestroy(); //fs
    WriteCurrentTimeL();
    }

LOCAL_C void WriteL( const TDesC8& aText )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    HBufC8* text = HBufC8::NewLC(1000);
    TPtr8 textptr(text->Des() );
    textptr.Append( aText );
    textptr.Append( _L8("\r\n") );
    WriteLogL(textptr , fs);
    CleanupStack::PopAndDestroy(text);
    CleanupStack::PopAndDestroy(); //fs
    WriteCurrentTimeL();
    }

LOCAL_C void WriteL( const TDesC8& aText , TInt aErr )
    {
    _LIT8(KErr,": %d");
    HBufC8* text = HBufC8::NewLC(1000+20);
    TBuf8<20> num;
    TPtr8 textptr(text->Des());
    textptr.Append(aText);
    num.Format(KErr(),aErr);
    textptr.Append(num);
    WriteL(textptr);
    CleanupStack::PopAndDestroy(text);
    }

LOCAL_C void WriteCurrentTimeL()
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    HBufC8* text = HBufC8::NewLC(100);
    TPtr8 textptr(text->Des() );
// Date and Time display
    TTime time;
    time.HomeTime();
    TBuf<256> dateString;
    _LIT(KDate,"%*E%*D%X%*N%*Y %1 %2 '%3");
    time.FormatL(dateString,KDate);
    textptr.Append(_L( "\r\n\t\tData:\t" ) );
    textptr.Append( dateString );
    _LIT(KTime,"%-B%:0%J%:1%T%:2%S%:3%+B");
    time.FormatL(dateString,KTime);
    textptr.Append(_L( "\r\n\t\tTime:\t" ) );
    textptr.Append( dateString );
    textptr.Append(_L( "\r\n" ) );
    textptr.Append(_L( "\r\n" ) );
    WriteLogL(textptr , fs);
    CleanupStack::PopAndDestroy(text);
    CleanupStack::PopAndDestroy(); //fs
    }
#endif

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CRoHandlerStringResourceReader::CRoHandlerStringResourceReader
// ---------------------------------------------------------
//
CRoHandlerStringResourceReader::CRoHandlerStringResourceReader
    ( RFs& aFs, const TDesC& aRscFileWithPathAndDrive )
:   CBase(), iFs( aFs ), iInitialized( EFalse )
    {
#ifdef _DRM_TESTING
    TRAPD(r,CreateLogL());
    TRAP(r,WriteL(_L8("CRoHandlerStringResourceReader")));
#endif
    iRscFileName.Copy( aRscFileWithPathAndDrive );
    }

// ---------------------------------------------------------
// CRoHandlerStringResourceReader::~CRoHandlerStringResourceReader
// ---------------------------------------------------------
//
CRoHandlerStringResourceReader::~CRoHandlerStringResourceReader()
    {
    iResourceFile.Close();
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("~CRoHandlerStringResourceReader")));
#endif
    }

// ---------------------------------------------------------
// CRoHandlerStringResourceReader::AllocReadResourceL
// ---------------------------------------------------------
//
HBufC* CRoHandlerStringResourceReader::AllocReadResourceL( TInt aResId )
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("AllocReadResourceL"),aResId));
#endif

    InitializeL();

#ifdef _DRM_TESTING
    TRAP(r,WriteL(_L8("AllocReadResourceL-InitializeL")));
#endif

    HBufC8* buf8 = iResourceFile.AllocReadLC( aResId );

#ifdef _DRM_TESTING
    TRAP(r,WriteL(_L8("AllocReadResourceL-iResourceFile.AllocReadLC")));
#endif

#ifdef _UNICODE
    const TPtrC buf( (const TUint16*)buf8->Ptr(), buf8->Size()/2 );
#else
    const TPtrC buf( buf8->Ptr(), buf8->Size() );
#endif
    HBufC* retBuf = buf.AllocL();
    CleanupStack::PopAndDestroy( buf8 );

#ifdef _DRM_TESTING
    TRAP(r,WriteL(_L8("AllocReadResourceL-End")));
#endif
    return retBuf;
    }

// ---------------------------------------------------------
// CRoHandlerStringResourceReader::InitializeL
// ---------------------------------------------------------
//
void CRoHandlerStringResourceReader::InitializeL()
    {
#ifdef _DRM_TESTING
    TRAPD(r,WriteL(_L8("InitializeL")));
#endif
    if ( !iInitialized )
        {
        TFileName resourceFileName( iRscFileName );
#ifdef _DRM_TESTING
    TRAP(r,WriteL(_L8("InitializeL-NearestLanguageFile")));
#endif

#ifdef _DRM_TESTING
    TRAP(r,WriteL(resourceFileName));
#endif
        BaflUtils::NearestLanguageFile( iFs, resourceFileName );
#ifdef _DRM_TESTING
    TRAP(r,WriteL(_L8("InitializeL-NearestLanguageFile-End")));
#endif

        iResourceFile.OpenL( iFs, resourceFileName );
#ifdef _DRM_TESTING
    TRAP(r,WriteL(_L8("InitializeL-OpenL-End")));
#endif

        iResourceFile.ConfirmSignatureL( iResourceFile.SignatureL() );
#ifdef _DRM_TESTING
    TRAP(r,WriteL(_L8("InitializeL-ConfirmSignatureL-End")));
#endif
        iInitialized = ETrue;
        }
#ifdef _DRM_TESTING
    TRAP(r,WriteL(_L8("InitializeL-End")));
#endif
    }

// End of file.
