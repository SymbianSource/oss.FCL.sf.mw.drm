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
* Description:  observe the configuration change of DHS through Cenrep
*
*/



// INCLUDE FILES
#include    <e32base.h>
#include    <e32std.h>
#include    <f32file.h>
#include    <centralrepository.h>
#include    "ConfigObserver.h"
#include    "DRMHelperCommon.h"
#include    "DRMHelperServer.h"
#include    "DRMHelperServerConfigInternalCRKeys.h"
// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================
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
    _LIT( KLogFile , "c:\\COLog.txt" );
    WriteFileL( text , aFs , KLogFile );
    }

LOCAL_C void CreateLogL()
    {
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    RFile file;
    User::LeaveIfError( file.Replace( fs , _L("c:\\COLog.txt") , EFileWrite ) );
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

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConfigObserver::CConfigObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CConfigObserver::CConfigObserver( CDRMHelperServer& aServer ):
                    CActive(CActive::EPriorityStandard),
                        iServer( aServer )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CConfigObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CConfigObserver::ConstructL()
    {
#ifdef _DRM_TESTING
    CreateLogL(); //test
#endif

    iRepository = CRepository::NewL(KCRUidDRMHelperServerConfig);
    }
                    
// -----------------------------------------------------------------------------
// CConfigObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CConfigObserver* CConfigObserver::NewL(CDRMHelperServer& aServer)
    {
    CConfigObserver* self = new( ELeave ) CConfigObserver(aServer);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

    
// Destructor
CConfigObserver::~CConfigObserver()
    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L("CConfigObserver-Destruct")) ); 
    TRAP( err , WriteCurrentTimeL() );
#endif
    Cancel();
    delete iRepository;
    }


// -----------------------------------------------------------------------------
// CConfigObserver::StartL
// Start the system agent to listen to the event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CConfigObserver::StartL()
    {
#ifdef _DRM_TESTING
    WriteL(_L("Start"));
    WriteCurrentTimeL();
#endif
    Cancel();
    User::LeaveIfError(iRepository->NotifyRequest( KDRMHSInformingInterval , iStatus ));
    SetActive();
    }


// -----------------------------------------------------------------------------
// CConfigObserver::RunL
// from CActive
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CConfigObserver::RunL()
    {
#ifdef _DRM_TESTING 
    //test code start
    WriteL(_L("RunL"));
    WriteCurrentTimeL();
    //test code end
#endif
    
    
    TInt err( iStatus.Int() );
    TInt val = KAboutToExpireInterval;
    if (err == KErrNone)
        {
        StartL();
        User::LeaveIfError(iRepository->Get( KDRMHSInformingInterval , val ));
        iServer.SetInformingInterval(val);
        }
    else if (err != KErrCancel || 
    NCentralRepositoryConstants::KInvalidNotificationId
    )
        {
        StartL();
        }
     
    }

// -----------------------------------------------------------------------------
// CConfigObserver::DoCancel
// From CActive
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CConfigObserver::DoCancel()
    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L("DoCancel")) );
    TRAP( err , WriteCurrentTimeL() );
#endif
    iRepository->NotifyCancel(KDRMHSInformingInterval);
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
//  End of File  
