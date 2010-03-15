/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  observe the idle event of system
*
*/



// INCLUDE FILES
#include    <e32base.h>
#include    <e32std.h>
#include    <f32file.h>
#include    <activeidle2domainpskeys.h>
#include    <e32property.h> //for RProperty
#include    "IdleObserver.h"
#include    "DRMHelperServer.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS


#ifdef _DRM_TESTING
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

LOCAL_C void WriteLogL( const TDesC8& text , RFs &aFs )
    {
    _LIT( KLogFile , "c:\\IOLog.txt" );
    WriteFileL( text , aFs , KLogFile );
    }

LOCAL_C void CreateLogL()
    {
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    RFile file;
    User::LeaveIfError( file.Replace( fs , _L("c:\\IOLog.txt") , EFileWrite ) );
    file.Close();
    CleanupStack::PopAndDestroy(&fs); //fs
    }

LOCAL_C void WriteL( const TDesC& aText )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    HBufC8* text = HBufC8::NewLC(1000);
    TPtr8 textptr(text->Des() );
    textptr.Append( aText );
    textptr.Append(_L( "\r\n" ));
    WriteLogL(textptr , fs);
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
    WriteLogL(textptr , fs);
    CleanupStack::PopAndDestroy(text);
    CleanupStack::PopAndDestroy(&fs); //fs
    }
#endif
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIdleObserver::CIdleObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIdleObserver::CIdleObserver( CDRMHelperServer& aServer ):
                    CActive(CActive::EPriorityStandard),
                        iServer( aServer )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CIdleObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIdleObserver::ConstructL()
    {
#ifdef _DRM_TESTING
    CreateLogL(); //test
#endif
    User::LeaveIfError( iProperty.Attach( KPSUidAiInformation, KActiveIdleState ) );
    }

// -----------------------------------------------------------------------------
// CIdleObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIdleObserver* CIdleObserver::NewL(CDRMHelperServer& aServer)
    {
    CIdleObserver* self = new( ELeave ) CIdleObserver(aServer);

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }


// Destructor
CIdleObserver::~CIdleObserver()
    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L("CIdleObserver-Destruct")) );
    TRAP( err , WriteCurrentTimeL() );
#endif
    Cancel();
    iProperty.Close();
    }


// -----------------------------------------------------------------------------
// CIdleObserver::StartL
// Start the system agent to listen to the event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIdleObserver::StartL()
    {
#ifdef _DRM_TESTING
    WriteL(_L("Start"));
    WriteCurrentTimeL();
#endif
    Cancel();
    iProperty.Subscribe( iStatus );
    SetActive();
    }


// -----------------------------------------------------------------------------
// CIdleObserver::RunL
// from CActive
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIdleObserver::RunL()
    {
#ifdef _DRM_TESTING
    //test code start
    WriteL(_L("RunL"));
    WriteCurrentTimeL();
    //test code end
#endif

    // Resubscribe before processing new value to prevent missing updates
    TInt idleStauts = 0;
    TInt err( iStatus.Int() );
    if (err == KErrNone)
        {
        StartL();
        User::LeaveIfError( iProperty.Get(
                KPSUidAiInformation,
                KActiveIdleState, idleStauts ) );
        if ( idleStauts == EPSAiForeground )
            {
            iServer.HandleIdleL();
            }
        }
    else if ( err != KErrCancel )
        {
        StartL();
        }
    }

// -----------------------------------------------------------------------------
// CIdleObserver::DoCancel
// From CActive
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIdleObserver::DoCancel()
    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L("DoCancel")) );
    TRAP( err , WriteCurrentTimeL() );
#endif
    iProperty.Cancel();
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
//  End of File
