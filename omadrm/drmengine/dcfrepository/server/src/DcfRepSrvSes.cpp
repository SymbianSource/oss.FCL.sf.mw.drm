/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  server session implementation
*
*/



// INCLUDE FILES
#include    "DcfRepSrv.h"
#include    "DcfRepSrvSes.h"
#include    "DcfRepCommon.h"
#include    "Pair.h"
#include    <e32std.h>
#include    <e32base.h>

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
const TUint32 KTrustedShutdownClient = 0x10205CB5;

// MACROS
#define SERVER const_cast< CDcfRepSrv* >( \
    reinterpret_cast< const CDcfRepSrv* >( Server() ) )

// LOCAL CONSTANTS AND MACROS
const TInt KSanityDataLengthLow = 0;
const TInt KSanityDataLengthHigh = 32768;

// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// #define _DRM_TESTING
// ============================= LOCAL FUNCTIONS ===============================
#ifdef _DRM_TESTING
LOCAL_C void WriteLogL( const TDesC8& text , RFs &aFs );
LOCAL_C void WriteFileL( const TDesC8& text , RFs &aFs , const TDesC& aName );
LOCAL_C void CreateLogL();
LOCAL_C void WriteL( const TDesC8& aText );
LOCAL_C void WriteL( const TDesC8& aText , TInt aErr );
LOCAL_C void WriteCurrentTimeL();
#endif

// -----------------------------------------------------------------------------
// SanitizeL
// Performs a sanity check on length parameters
// -----------------------------------------------------------------------------
//
LOCAL_C void SanitizeL( TInt aParam )
    {
    if( aParam <= KSanityDataLengthLow || aParam > KSanityDataLengthHigh )
        {
        User::Leave(KErrArgument);
        }
    }
    
// -----------------------------------------------------------------------------
// Testing stuff
// -----------------------------------------------------------------------------
//
#ifdef _DRM_TESTING

LOCAL_C void WriteLogL( const TDesC8& text , RFs &aFs )
    {
    _LIT( KLogFile , "c:\\CDcfRepSrvSes.txt" );
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
    User::LeaveIfError( file.Replace( fs , _L("c:\\CDcfRepSrvSes.txt") , EFileWrite ) );
    file.Close();
    CleanupStack::PopAndDestroy(&fs); //fs
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
    CleanupStack::PopAndDestroy(&fs); //fs
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
    CleanupStack::PopAndDestroy(&fs); //fs
    }
#endif
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDcfRepSrvSes::CDcfRepSrvSes
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDcfRepSrvSes::CDcfRepSrvSes()
    {
    }

// -----------------------------------------------------------------------------
// CDcfRepSrvSes::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDcfRepSrvSes::ConstructL()
    {
#ifdef _DRM_TESTING
    CreateLogL();
    WriteL(_L8("ConstructL"));
#endif
    }

// -----------------------------------------------------------------------------
// CDcfRepSrvSes::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDcfRepSrvSes* CDcfRepSrvSes::NewL()
    {
    CDcfRepSrvSes* self = new( ELeave ) CDcfRepSrvSes;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

    
// Destructor
CDcfRepSrvSes::~CDcfRepSrvSes()
    {
#ifdef _DRM_TESTING
    TRAPD(err,WriteL(_L8("~CDcfRepSrvSes")));
#endif

    SERVER->RemoveMessageL( iMessage );


    if (iCidList)
        {
        iCidList->ResetAndDestroy();
        iCidList->Close();
        delete iCidList;    
        }
        
    if (iPairList)
        {
        iPairList->ResetAndDestroy();
        iPairList->Close();
        delete iPairList;    
        }    
    }



// -----------------------------------------------------------------------------
// CDcfRepSrvSes::ServiceL
// This method runs DispatchL() under TRAP harness, since every error case
// can be handled ==> no need to let this function leave.
// -----------------------------------------------------------------------------
//
void CDcfRepSrvSes::ServiceL( const RMessage2& aMessage )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("ServiceL"));
#endif
    // Trap possible errors...
    TRAPD( error, DispatchL( aMessage ) );
    
    if ( error )
        {
#ifdef _DRM_TESTING
    WriteL(_L8("ServiceL->error"),error);
#endif
        aMessage.Complete( error );
        return;
        }
    }

// -----------------------------------------------------------------------------
// CDcfRepSrvSes::DispatchL
// Checks which command the user requested, and forwards the request to 
// appropriate private method. This helps to keep the code more readable.
// -----------------------------------------------------------------------------
//
void CDcfRepSrvSes::DispatchL( const RMessage2& aMessage ) 
    {
#ifdef _DRM_TESTING
    WriteL(_L8("DispatchL"));
#endif

    TBool complete = ETrue;
    switch ( aMessage.Function() )
        {
        case EAddFile:
            AddFileL( aMessage );
            break;
        case EFullScan:
            FullScanL( aMessage );
            complete = EFalse;
            break;
        case EScanAdd:
            ScanAddL( aMessage );
            break;
        case EScanEnd:
            ScanEndL( aMessage );
            complete = EFalse;
            break;
        case EScanStart:
            ScanStartL( aMessage );
            break;
        case ESetTtidStart:
            SetTtidStartL( aMessage );
            break;
        case ESetTtidAdd:
            SetTtidAddL( aMessage );
            break;                    
        case ESetTtidEnd:
            SetTtidEndL( aMessage );
            complete = EFalse;
            break;
        case ENotifyIdle:
            NotifyIdleL( aMessage );
            complete = EFalse;
            break;
        case EStopWatching:
            StopWatchingL( aMessage );
            break;
        default:
            User::Leave( KErrNotSupported );
        }
    if (complete)
        {
        aMessage.Complete(KErrNone);
        }
    }
    
    
// -----------------------------------------------------------------------------
// CDcfRepSrvSes::AddFileL
// -----------------------------------------------------------------------------
//    
void CDcfRepSrvSes::AddFileL( const RMessage2& aMessage )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("AddFileL"));
#endif

    TFileName file;
    aMessage.ReadL( 0, file );
    SERVER->AddFileL(file);    
    }
    
// -----------------------------------------------------------------------------
// CDcfRepSrvSes::FullScanL
// -----------------------------------------------------------------------------
//    
void CDcfRepSrvSes::FullScanL( const RMessage2& aMessage )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("FullScanL"));
#endif

    TInt err = SERVER->State();
    if (err!=EStateIdle)
        {
        User::Leave(KErrServerBusy);    
        }
    else
        {
        iMessage = SERVER->SetMessageL(aMessage);
        SERVER->SetState(EStateFullScan);   
        User::LeaveIfError(SERVER->Scan());
        }
    }

// -----------------------------------------------------------------------------
// CDcfRepSrvSes::ScanSingleL
// -----------------------------------------------------------------------------
//
void CDcfRepSrvSes::ScanAddL( const RMessage2& aMessage )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("ScanAddL"));
#endif   
 
    TInt size = 0;
    HBufC8* cid = NULL;
    TPtr8 data(NULL,0,0);
    if (iPreviousAction == EScanStart || iPreviousAction == EScanAdd)
        {
        size = aMessage.GetDesLength( 0 );
        SanitizeL(size);
        cid = HBufC8::NewMaxLC( size );
        data.Set( cid->Des() );
        data.SetLength(size);
        aMessage.ReadL( 0, data );
        User::LeaveIfError(iCidList->Append(cid));
        CleanupStack::Pop(cid);    
        iPreviousAction = EScanAdd;
        }
    }
    
// -----------------------------------------------------------------------------
// CDcfRepSrvSes::ScanEndL
// -----------------------------------------------------------------------------
//    
void CDcfRepSrvSes::ScanEndL( const RMessage2& aMessage )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("ScanEndL"));
#endif  

    TInt err = 0;
    if (iPreviousAction == EScanAdd)
        {
        err = SERVER->State();
        if (err!=EStateIdle)
            {
            User::Leave(KErrServerBusy);    
            }
        else
            {
            iMessage = SERVER->SetMessageL(aMessage);
            SERVER->SetCidList(iCidList);
            iCidList = NULL;
            SERVER->SetState(EStateScan);
            User::LeaveIfError(SERVER->Scan());
            } 
        }
    }

// -----------------------------------------------------------------------------
// CDcfRepSrvSes::ScanStartL
// -----------------------------------------------------------------------------
//
void CDcfRepSrvSes::ScanStartL( const RMessage2& /*aMessage*/ )
    {
    TInt err = SERVER->State();
    
    if (err!=EStateIdle)
        {
        User::Leave(KErrServerBusy);    
        }
    else
        {
        if (iCidList)
            {
            // clean list
            iCidList->ResetAndDestroy();    
            }
        else
            {
            // initilize list
            iCidList = new (ELeave) RPointerArray<HBufC8>();        
            }
        
        } 
    iPreviousAction = EScanStart;  
    }

// -----------------------------------------------------------------------------
// CDcfRepSrvSes::SetTtidStartL
// -----------------------------------------------------------------------------
//    
void CDcfRepSrvSes::SetTtidStartL( const RMessage2& /*aMessage*/ )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("SetTtidStartL"));
#endif    
    
    TInt err = SERVER->State();
    if (err!=EStateIdle)
        {
        User::Leave(KErrServerBusy);    
        }    
    else
        {
        if (iPairList)
            {
            // clean list
            iPairList->ResetAndDestroy();    
            }
        else
            {
            // initilize list
            iPairList = new (ELeave) RPointerArray<CPair>();        
            }        
        }
    iPreviousAction = ESetTtidStart;  
    }

// -----------------------------------------------------------------------------
// CDcfRepSrvSes::SetTtidAddL
// -----------------------------------------------------------------------------
//
void CDcfRepSrvSes::SetTtidAddL( const RMessage2& aMessage )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("SetTtidAddL"));
#endif 

    TInt size = 0;
    TPtr8 data(NULL,0,0);
    if (iPreviousAction == ESetTtidStart || iPreviousAction == ESetTtidAdd)
        {
        CPair* pair = CPair::NewL();
        CleanupStack::PushL(pair);
        
        size = aMessage.GetDesLength( 0 );
        SanitizeL( size );
        pair->iCid = HBufC8::NewMaxL( size );
        data.Set( pair->iCid->Des() );
        data.SetLength(size);
        aMessage.ReadL( 0, data );
        
        
        size = aMessage.GetDesLength( 1 );
        SanitizeL( size );
        pair->iTtid = HBufC8::NewMaxL( size );
        data.Set( pair->iTtid->Des() );
        data.SetLength(size);
        aMessage.ReadL( 1, data );
        
        User::LeaveIfError(iPairList->Append(pair));
        CleanupStack::Pop(pair);
            
        iPreviousAction = ESetTtidAdd;
        }    
    }


    
// -----------------------------------------------------------------------------
// CDcfRepSrvSes::SetTtidEndL
// -----------------------------------------------------------------------------
//    
void CDcfRepSrvSes::SetTtidEndL( const RMessage2& aMessage )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("SetTtidEndL"));
#endif 

    TInt err = 0;
    if (iPreviousAction == ESetTtidAdd)
        {
        err = SERVER->State();
        if (err!=EStateIdle)
            {
            User::Leave(KErrServerBusy);    
            }
        else
            {
            iMessage = SERVER->SetMessageL(aMessage);
            SERVER->SetPairList(iPairList);
            iPairList = NULL;
            SERVER->SetState(EStateSetTtid);
            User::LeaveIfError(SERVER->Scan());
            } 
        }   
    }

// -----------------------------------------------------------------------------
// CDcfRepSrvSes::NotifyIdleL
// -----------------------------------------------------------------------------
//    
void CDcfRepSrvSes::NotifyIdleL( const RMessage2& aMessage )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("NotifyIdleL"));
#endif

    TInt err = SERVER->State();
    if (err==EStateIdle)
        {
#ifdef _DRM_TESTING
    WriteL(_L8("NotifyIdleL->Idle"));
#endif
        aMessage.Complete(KErrNone);
        }
    else
        {
        iMessage = SERVER->SetMessageL(aMessage);
        }
    }

// -----------------------------------------------------------------------------
// CDcfRepSrvSes::StopWatchingL
// -----------------------------------------------------------------------------
//    
void CDcfRepSrvSes::StopWatchingL( const RMessage2& aMessage )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("StopWatchingL"));
#endif
    _LIT_SECURITY_POLICY_S0( sidCheck, KTrustedShutdownClient );
    if ( sidCheck.CheckPolicy( aMessage ) )
        {
        SERVER->StopWatchingL();
        }
    }
//  End of File  
