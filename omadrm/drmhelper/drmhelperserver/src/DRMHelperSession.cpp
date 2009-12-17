/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the Helper Server session functionality 
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <f32file.h>
#include "DRMHelperCommon.h"
#include "DRMHelperServer.h"
#include "DRMHelperSession.h"



// LOCAL CONSTANTS AND MACROS
#define SERVER const_cast< CDRMHelperServer* >( \
    reinterpret_cast< const CDRMHelperServer* >( Server() ) )


// ============================ LOCAL FUNCTIONS ===============================
#ifdef _DRM_TESTING
LOCAL_C void WriteDownLogL( const TDesC8& text , RFs &aFs );
LOCAL_C void WriteFileL( const TDesC8& text , RFs &aFs , const TDesC& aName );
LOCAL_C void CreateLogL();
LOCAL_C void WriteDownL( const TDesC& aText );
LOCAL_C void WriteDownL( const TDesC8& aText );
LOCAL_C void WriteCurrentTimeL();
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMHelperSession::CDRMHelperSession
// Default constructor.
// -----------------------------------------------------------------------------
//
CDRMHelperSession::CDRMHelperSession()
    // Base class' constructor is called first.
    : CSession2() 
    {
    // Nothing.
    }

// -----------------------------------------------------------------------------
// CDRMHelperSession::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMHelperSession* CDRMHelperSession::NewL()
    {
    CDRMHelperSession* self = new( ELeave ) CDRMHelperSession();
    CleanupStack::PushL( self );
#ifdef _DRM_TESTING
    CreateLogL();
#endif
    CleanupStack::Pop(self); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CDRMHelperSession::~CDRMHelperSession
// Destructor.
// -----------------------------------------------------------------------------
//
CDRMHelperSession::~CDRMHelperSession()
    {
    TInt err = KErrNone;	
    TRAP(err , SERVER->StoreL());
    // remove warning, and enable logging in the future
		if( err ) {
			err = KErrNone;
	  }
#ifdef _DRM_TESTING
    TRAP( err , WriteDownL(_L("destructor")) );
    TRAP( err , WriteCurrentTimeL() );
#endif
    }



// -----------------------------------------------------------------------------
// CDRMHelperSession::ServiceL
// This method runs DispatchL() under TRAP harness, since every error case
// can be handled ==> no need to let this function leave.
// -----------------------------------------------------------------------------
//
void CDRMHelperSession::ServiceL( const RMessage2& aMessage )
    {
    
#ifdef _DRM_TESTING
    WriteDownL(_L("ServiceL"));
    WriteCurrentTimeL();
#endif
    
    // Trap possible errors...
    TRAPD( error, DispatchL( aMessage ) );
    
    if ( error )
        {
        aMessage.Complete( error );
        return;
        }
    }



// -----------------------------------------------------------------------------
// CDRMHelperSession::DispatchL
// Checks which command the user requested, and forwards the request to 
// appropriate private method. This helps to keep the code more readable.
// -----------------------------------------------------------------------------
//
void CDRMHelperSession::DispatchL( const RMessage2& aMessage ) 
    {
#ifdef _DRM_TESTING
    WriteDownL(_L("DispatchL"));
    WriteCurrentTimeL();
#endif
    switch ( aMessage.Function() )
        {
        case ERegister:
            RegisterL( aMessage );
            break;
        case ERemove:
            RemoveL( aMessage );
            break;
        case EIndicateIdle:
            IndicateIdleL( aMessage );
            break;
        case EIsRegistered:
            IsRegisteredL( aMessage );
            break;
        default:
            User::Leave( KErrNotSupported );
        }
    }

// -----------------------------------------------------------------------------
// CDRMHelperSession::RegisterL
// Register the content URI into helper server
// -----------------------------------------------------------------------------
//
void CDRMHelperSession::RegisterL( const RMessage2& aMessage ) 
    {
#ifdef _DRM_TESTING
    WriteDownL(_L("RegisterL"));
    WriteCurrentTimeL();
#endif

    TInt size = 0;
    TInt permType = aMessage.Int0();
    TInt regType = aMessage.Int1();
    TInt autoType = aMessage.Int2();
    
    HBufC8* cid = NULL;
    TPtr8 data( NULL , 0 , 0 );

    size = User::LeaveIfError( aMessage.GetDesLength( 3 ) );
    if (size==0)
        {
        aMessage.Complete( KErrArgument );
        return;
        }

#ifdef _DRM_TESTING
    _LIT( KSize , "size = %d");
    TBuf<20> sizeBuf;
    sizeBuf.Format(KSize ,  size);
    WriteDownL(sizeBuf);
    WriteCurrentTimeL();
#endif

    cid = HBufC8::NewLC( size );
    data.Set( cid->Des() );
    aMessage.ReadL( 3, data );

#ifdef _DRM_TESTING
    WriteDownL(data);
    WriteCurrentTimeL();
#endif

    SERVER->RegisterL( data , permType , regType , autoType );
    SERVER->StoreL();

    CleanupStack::PopAndDestroy( cid );
    aMessage.Complete( KErrNone );
    }


// -----------------------------------------------------------------------------
// CDRMHelperSession::RemoveL
// Unregister the content URI into helper server
// -----------------------------------------------------------------------------
//
void CDRMHelperSession::RemoveL( const RMessage2& aMessage ) 
    {
#ifdef _DRM_TESTING
    WriteDownL(_L("RemoveL"));
    WriteCurrentTimeL();
#endif
    TInt size = 0;
    TInt permType = aMessage.Int0();
    TInt regType = aMessage.Int1();
    TInt autoType = aMessage.Int2();
    HBufC8* cid = NULL;
    TPtr8 data( NULL , 0 , 0 );
    size = User::LeaveIfError( aMessage.GetDesLength( 3 ) );
    if (size==0)
        {
        aMessage.Complete( KErrArgument );
        return;
        }
    cid = HBufC8::NewLC( size );
    data.Set( cid->Des() );
    aMessage.ReadL( 3, data );
    
    // If this is execute, means we are using a pip file, unregister all other types as well
    if( permType == 3 )
        {
        // EPlay
        SERVER->RemoveL( data , 1 , regType , autoType );
        // EDisplay
        SERVER->RemoveL( data , 2 , regType , autoType );               
        }
    
    
    SERVER->RemoveL( data , permType , regType , autoType );
    SERVER->StoreL();
    CleanupStack::PopAndDestroy( cid );
    aMessage.Complete( KErrNone );
    }


// -----------------------------------------------------------------------------
// CDRMHelperSession::IndicateIdleL
// Unregister the content URI into helper server
// -----------------------------------------------------------------------------
//
void CDRMHelperSession::IndicateIdleL( const RMessage2& aMessage ) 
    {
#ifdef _DRM_TESTING
    WriteDownL(_L("IndicateIdleL"));
    WriteCurrentTimeL();
#endif
    SERVER->HandleIdleL();
    aMessage.Complete( KErrNone );
    }


// -----------------------------------------------------------------------------
// CDRMHelperSession::IsRegisteredL
// check if content is registered or not
// -----------------------------------------------------------------------------
//
void CDRMHelperSession::IsRegisteredL( const RMessage2& aMessage ) 
    {
#ifdef _DRM_TESTING
    WriteDownL(_L("IsRegisteredL"));
    WriteCurrentTimeL();
#endif

    TInt size = 0;
    TInt permType = aMessage.Int0();
    TInt regType = aMessage.Int1();
    TInt autoType = aMessage.Int2();
    HBufC8* cid = NULL;
    TPtr8 data( NULL , 0 , 0 );
    TBool mark = EFalse;
    TPtr8 ptr(NULL,0,0);
    size = User::LeaveIfError( aMessage.GetDesLength( 3 ) );
    if (size==0)
        {
        aMessage.Complete( KErrArgument );
        return;
        }


#ifdef _DRM_TESTING
    _LIT( KSize , "size = %d");
    TBuf<20> sizeBuf;
    sizeBuf.Format(KSize ,  size);
    WriteDownL(sizeBuf);
    WriteCurrentTimeL();
#endif

    cid = HBufC8::NewLC( size );
    data.Set( cid->Des() );
    aMessage.ReadL( 3, data );

#ifdef _DRM_TESTING
    WriteDownL(data);
    WriteCurrentTimeL();
#endif
    mark = SERVER->IsRegistered2( data , regType , autoType  );

    CleanupStack::PopAndDestroy( cid );

    aMessage.Complete( mark );
    }



#ifdef _DRM_TESTING

LOCAL_C void WriteDownLogL( const TDesC8& text , RFs &aFs )
    {
    _LIT( KLogFile , "c:\\HSSessionLog.txt" );
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
    CleanupStack::PopAndDestroy(&file); //file
    }

LOCAL_C void CreateLogL()
    {
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    RFile file;
    User::LeaveIfError( file.Replace( fs , _L("c:\\HSSessionLog.txt") , EFileWrite ) );
    file.Close();
    CleanupStack::PopAndDestroy(&fs); //fs
    }

LOCAL_C void WriteDownL( const TDesC& aText )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    HBufC8* text = HBufC8::NewLC(1000);
    TPtr8 textptr(text->Des() );
    textptr.Append( aText );
    textptr.Append( _L("\r\n") );
    WriteDownLogL(textptr , fs);
    CleanupStack::PopAndDestroy(text);
    CleanupStack::PopAndDestroy(&fs); //fs
    }

LOCAL_C void WriteDownL( const TDesC8& aText )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    HBufC8* text = HBufC8::NewLC(1000);
    TPtr8 textptr(text->Des() );
    textptr.Append( aText );
    textptr.Append( _L8("\r\n") );
    WriteDownLogL(textptr , fs);
    CleanupStack::PopAndDestroy(text);
    CleanupStack::PopAndDestroy(&fs); //fs
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
    WriteDownLogL(textptr , fs);
    CleanupStack::PopAndDestroy(text);
    CleanupStack::PopAndDestroy(&fs); //fs
    }
#endif

//  End of File
