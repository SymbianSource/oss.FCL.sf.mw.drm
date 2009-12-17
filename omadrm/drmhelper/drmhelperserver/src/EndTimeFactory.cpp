/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handling end time based automated content
*
*/



// INCLUDE FILES
#include    <e32base.h>
#include    <e32std.h>
#include    <s32file.h>
#include    "DRMHelperServer.h"
#include    "EndTimeFactory.h"
#include    "EndTimeBased.h"


// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
LOCAL_C const TInt KInformingDelay( 1 );

// Maximum amount of times the expired item is handled before 
// unregistering without client's call.
// (In order to avoid stale registrations)
LOCAL_C const TInt KMaxHandlingCount( 50 );
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
    _LIT( KLogFile , "c:\\ETFLog.txt" );
    WriteFileL( text , aFs , KLogFile );
    }

LOCAL_C void CreateLogL()
    {
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    RFile file;
    User::LeaveIfError( file.Replace( fs , _L("c:\\ETFLog.txt") , EFileWrite ) );
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

LOCAL_C void WriteL( const TDesC8& aText )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    HBufC8* text = HBufC8::NewLC(1000);
    TPtr8 textptr(text->Des() );
    textptr.Append( aText );
    textptr.Append(_L8( "\r\n" ));
    WriteLogL(textptr , fs);
    CleanupStack::PopAndDestroy(text);
    CleanupStack::PopAndDestroy(&fs); //fs
    }

LOCAL_C void WriteTimeL( TTime aTime )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    HBufC8* text = HBufC8::NewLC(100);
    TPtr8 textptr(text->Des() );
    // Date and Time display
    TBuf<256> dateString;
    _LIT(KDate,"%*E%*D%X%*N%*Y %1 %2 '%3");
    aTime.FormatL(dateString,KDate);
    textptr.Append(_L( "\r\n\t\t\t\tData:\t" ) );
    textptr.Append( dateString );
    _LIT(KTime,"%-B%:0%J%:1%T%:2%S%:3%+B");
    aTime.FormatL(dateString,KTime);
    textptr.Append(_L( "\r\n\t\t\t\tTime:\t" ) );
    textptr.Append( dateString );
    textptr.Append(_L( "\r\n" ) );
    textptr.Append(_L( "\r\n" ) );
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

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEndTimeFactory::CEndTimeFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEndTimeFactory::CEndTimeFactory( CDRMHelperServer& aServer ):
    CTimer(CActive::EPriorityStandard),
    iServer( aServer ),
    iOverflowedAlarm( Time::MaxTTime() )
    {
    iAlarm.HomeTime();
    }

// -----------------------------------------------------------------------------
// CEndTimeFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEndTimeFactory::ConstructL()
    {
#ifdef _DRM_TESTING
    CreateLogL(); //test
#endif
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CEndTimeFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CEndTimeFactory* CEndTimeFactory::NewL(CDRMHelperServer& aServer)
    {
    CEndTimeFactory* self = new( ELeave ) CEndTimeFactory(aServer);

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }


// Destructor
CEndTimeFactory::~CEndTimeFactory()
    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L("CEndTimeFactory-Destruct")) );
    TRAP( err , WriteCurrentTimeL() );
#endif

    iList.ResetAndDestroy();
    iList.Close();
    Cancel();
    }


// -----------------------------------------------------------------------------
// CEndTimeFactory::AddL
// add a specific content uri into factory
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEndTimeFactory::AddL(
        const TDesC8& aUri ,
        TTime aEndTime ,
        const TUint8& aPermType ,
        const TUint8& aRegType ,
        const TUint8& aAutoType )
    {
#ifdef _DRM_TESTING
    WriteL(_L("Add"));
    WriteL(aUri);
    WriteCurrentTimeL();
#endif

    CEndTimeBased* newItem = NULL;
    TTime home;
    TInt index = 0;


    // check if uri is already in the list
    index = Find( aUri , aPermType , aRegType , aAutoType );
    if ( index >= 0 )
        {
        // if it is in the list then just update the end time.
        iList[index]->IncreaseRegTimes();
        UpdateEndTimeL( iList[index] , aEndTime );
        return;
        }

    // if not in the list then create one
    newItem = CEndTimeBased::NewL( aUri , aPermType , aRegType , aAutoType );
    CleanupStack::PushL(newItem);

    // set end time
    newItem->SetEndTime(aEndTime);

    // set expiration
    home.HomeTime();
    if ( home >= aEndTime )
        {
        newItem->SetIsExpired(ETrue);
        }

    // add the item into list
    User::LeaveIfError( iList.Append( newItem ) );
    CleanupStack::Pop(newItem);
    // change alarm time if necessary
    if ( home >= iAlarm && !newItem->IsExpired() )
        {
#ifdef _DRM_TESTING
        //test code start
        WriteL(_L("Add->Change iAlarm-><home"));
        WriteL(aUri);
        WriteCurrentTimeL();
        WriteL(_L("Change Time"));
        WriteTimeL(aEndTime);
        //test code end
#endif

        iAlarm = aEndTime;
        }

    if ( !newItem->IsExpired() && iAlarm >= aEndTime && iAlarm != Time::MaxTTime() )
        {
        iAlarm = aEndTime;
        Cancel();

#ifdef _DRM_TESTING
        //test code start
        WriteL(_L("Add->Change iAlarm->>Change Time"));
        WriteL(aUri);
        WriteCurrentTimeL();
        WriteL(_L("Change Time"));
        WriteTimeL(iAlarm);
        WriteL(_L("Add->Trigger iAlarm"));
        WriteL(aUri);
        WriteL(_L("Trigger Time"));
        WriteTimeL(iAlarm);
        //test code end
#endif

        CTimer::At(iAlarm+TTimeIntervalSeconds(KInformingDelay));

        }

    }

// -----------------------------------------------------------------------------
// CEndTimeFactory::UpdateL
// update a specific content uri in the list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEndTimeFactory::UpdateL()
    {
#ifdef _DRM_TESTING
    WriteL(_L("UpdateL(all)"));
    WriteCurrentTimeL();
#endif

    for( TInt i( 0 ); i < iList.Count(); i++ )
        {
        const CEndTimeBased* e( iList[i] );
        iServer.UpdateL(
                *e->Uri(), e->PermType(), e->RegType(), e->AutoType() );
        if ( e->Count() > KMaxHandlingCount )
            {
            // Client must have forgotten to unregister item properly.
            // So let's unregister now.
            Remove( *e->Uri(), e->PermType(), e->RegType(), e->AutoType() );
            }
        }
    }

// -----------------------------------------------------------------------------
// CEndTimeFactory::UpdateEndTimeL
// update the end time of a specific content uri in the list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEndTimeFactory::UpdateEndTimeL(
        const TDesC8& aUri ,
        const TTime& aEndTime ,
        const TUint8& aPermType ,
        const TUint8& aRegType
        )
    {
#ifdef _DRM_TESTING
    WriteL(_L("UpdateEndTimeL(uri,endtime,perm,reg)"));
    WriteL(aUri);
    WriteCurrentTimeL();
#endif

    TInt index = 0;
    index = FindNext(aUri,aPermType,aRegType,index);
    while ( index >= 0 )
        {
        UpdateEndTimeL( iList[index] , aEndTime );
        index++;
        index = FindNext(aUri,aPermType,aRegType,index);
        }
    }


// -----------------------------------------------------------------------------
// CEndTimeFactory::UpdateEndTimeL
// update the end time of a specific content uri in the list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEndTimeFactory::UpdateEndTimeL(
        const TDesC8& aUri ,
        const TTime& aEndTime ,
        const TUint8& aPermType ,
        const TUint8& aRegType ,
        const TUint8& aAutoType
        )
    {
#ifdef _DRM_TESTING
    WriteL(_L("UpdateEndTimeL(uri,endtime,perm,reg,autotype)"));
    WriteL(aUri);
    WriteCurrentTimeL();
#endif

    TInt index = 0;
    index = Find(aUri,aPermType,aRegType,aAutoType);
    if ( index >= 0 )
        {
        UpdateEndTimeL( iList[index] , aEndTime );
        }
    }

// -----------------------------------------------------------------------------
// CEndTimeFactory::UpdateEndTimeL
// update the end time of a specific content uri in the list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEndTimeFactory::UpdateEndTimeL( CEndTimeBased*& aItem , const TTime& aEndTime )
    {
#ifdef _DRM_TESTING
    WriteL(_L("UpdateEndTimeL(item,endtime)"));
    WriteL(aItem->Uri()->Des());
    WriteCurrentTimeL();
#endif

    aItem->SetEndTime(aEndTime);
    ResetAlarm();
    }


// -----------------------------------------------------------------------------
// CEndTimeFactory::IsRegisteredL
// check if content uri is registered into list or not
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CEndTimeFactory::IsRegistered(
        const TDesC8& aUri ,
        const TUint8& aPermType ,
        const TUint8& aRegType ,
        const TUint8& aAutoType )
    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L("IsRegisteredL(aUri,aPermType,aRegType,aAutoType)")) );
    TRAP( err , WriteL(aUri) );
    TRAP( err , WriteCurrentTimeL() );
#endif

    // check if it is in the list
    if ( Find( aUri , aPermType , aRegType , aAutoType ) >= 0 )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CEndTimeFactory::IsRegisteredL
// check if content uri is registered into list or not
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CEndTimeFactory::IsRegistered(
        const TDesC8& aUri ,
        const TUint8& aPermType ,
        const TUint8& aRegType )
    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L("IsRegisteredL(aUri,aPermType,aRegType)")) );
    TRAP( err , WriteL(aUri) );
    TRAP( err , WriteCurrentTimeL() );
#endif

    // check if it is in the list
    TInt index = 0;
    if ( FindNext( aUri , aPermType , aRegType , index ) >= 0 )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CEndTimeFactory::IsRegisteredL
// check if content uri is registered into list or not
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CEndTimeFactory::IsRegistered2(
        const TDesC8& aUri ,
        const TUint8& aRegType,
        const TUint8& aAutoType )
    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L("IsRegisteredL(aUri,aRegType)")) );
    TRAP( err , WriteL(aUri) );
    TRAP( err , WriteCurrentTimeL() );
#endif

    // check if it is in the list
    if ( Exists( aUri, aRegType, aAutoType) >= 0 )
        {
        return ETrue;
        }
    return EFalse;
    }



// -----------------------------------------------------------------------------
// CEndTimeFactory::IsRegisteredL
// check if content uri is registered into list or not
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CEndTimeFactory::IsRegistered( const TDesC8& aUri )
    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L("IsRegisteredL(aUri)")) );
    TRAP( err , WriteL(aUri) );
    TRAP( err , WriteCurrentTimeL() );
#endif

    // check if it is in the list
    TInt index = 0;
    if ( FindNext( aUri , index ) >= 0 )
        {
        return ETrue;
        }
    return EFalse;
    }



// -----------------------------------------------------------------------------
// CEndTimeFactory::Remove
// Remove a specific content uri from factory
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEndTimeFactory::Remove(
        const TDesC8& aUri ,
        const TUint8& aPermType ,
        const TUint8& aRegType ,
        const TUint8& aAutoType )
    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L("Remove")) );
    TRAP( err , WriteL(aUri) );
    TRAP( err , WriteCurrentTimeL() );
#endif

    TInt index = Find( aUri , aPermType , aRegType , aAutoType );
    if ( index >= 0 )
        {
        iList[index]->DecreaseRegTimes();

#ifdef _DRM_TESTING
        TBuf<20> buf;
        _LIT( KRegTimes , "RegTimes: %d");
        buf.Format( KRegTimes , iList[index]->RegTimes() );
        TRAP( err , WriteL(buf) );
#endif

        if ( iList[index]->RegTimes()<1 )
            {
#ifdef _DRM_TESTING
            TRAPD( err , WriteL(_L("Removing succeeded!")) );
            TRAP( err , WriteCurrentTimeL() );
#endif
            delete iList[index];
            iList[index] = NULL;
            iList.Remove(index);
            iList.Compress();
            }
        }
    }

// -----------------------------------------------------------------------------
// CEndTimeFactory::RemoveAllL
// Remove a specific content uri from factory
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEndTimeFactory::RemoveAllL()
    {
#ifdef _DRM_TESTING
    WriteL(_L("RemoveAllL"));
    WriteCurrentTimeL();
#endif
    TInt i = 0;
    for(;i<iList.Count();i++)
        {
        iServer.RemoveNotificationL( *iList[i]->Uri() );
        }
    iList.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CEndTimeFactory::RunL
// from CActive
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEndTimeFactory::RunL()
    {
#ifdef _DRM_TESTING
    //test code start
    WriteL(_L("RunL"));
    WriteL(_L("Current Time"));
    WriteCurrentTimeL();
    WriteL(_L("iAlarm"));
    WriteTimeL(iAlarm);
    //test code end
#endif

    TInt error( iStatus.Int() ); // to be used in ResetAlarm
    if ( error == KErrOverflow )
        {
        // Overflow occured, next trial after desired interval / 2
        // Let's update known overflowed alarm time 1st
        iOverflowedAlarm = iAlarm;
        TInt64 alarm( iAlarm.Int64() );
        iAlarm.HomeTime();
        TInt64 h( iAlarm.Int64() );
        TTime newAlarm( alarm / 2 + h / 2 );
        iAlarm = newAlarm;
        }

    HandleExpiredL();
    ResetAlarm();
    }

// -----------------------------------------------------------------------------
// CEndTimeFactory::DoCancel
// From CActive
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEndTimeFactory::DoCancel()
    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L("DoCancel")) );
    TRAP( err , WriteCurrentTimeL() );
#endif
    CTimer::DoCancel();
    }

// -----------------------------------------------------------------------------
// CEndTimeFactory::Find
// Find index for content uri
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CEndTimeFactory::Find(
        const TDesC8& aUri ,
        const TUint8& aPermType ,
        const TUint8& aRegType ,
        const TUint8& aAutoType )
    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L("Find(aUri,aPermType,aRegType,aAutoType)")) );
    TRAP( err , WriteL(aUri) );
    TRAP( err , WriteCurrentTimeL() );
#endif

    TInt i = 0;
    for ( ; i < iList.Count() ; i++ )
        {
        if ( iList[i]->Uri()->Compare( aUri ) == 0 )
            {
            if ( (iList[i]->PermType()==aPermType) )
                {
                if ( (iList[i]->RegType()==aRegType) )
                    {
                    if ( (iList[i]->AutoType()==aAutoType) )
                        {
                        return i;
                        }
                    }
                }
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CEndTimeFactory::FindNext
// Find index for content uri
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CEndTimeFactory::FindNext(const TDesC8& aUri,const TUint8& aIndex)
    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L("FindNext(aUri,aIndex)")) );
    TRAP( err , WriteL(aUri) );
    TRAP( err , WriteCurrentTimeL() );
#endif

    TInt i = aIndex;
    for ( ; i < iList.Count() ; i++ )
        {
        if ( iList[i]->Uri()->Compare( aUri ) == 0 )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CEndTimeFactory::FindNext
// Find index for content uri
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CEndTimeFactory::FindNext(
        const TDesC8& aUri ,
        const TUint8& aPermType ,
        const TUint8& aRegType ,
        const TUint8& aIndex )
    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L("FindNext(aUri,aPermType,aRegType,aIndex)")) );
    TRAP( err , WriteL(aUri) );
    TRAP( err , WriteCurrentTimeL() );
#endif

    TInt i = aIndex;
    for ( ; i < iList.Count() ; i++ )
        {
        if ( iList[i]->Uri()->Compare( aUri ) == 0 )
            {
            if ( (iList[i]->PermType()==aPermType) )
                {
                if ( (iList[i]->RegType()==aRegType) )
                    {
                    return i;
                    }
                }
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CEndTimeFactory::Exists
// Find index for content uri
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CEndTimeFactory::Exists(
        const TDesC8& aUri ,
        const TUint8& aRegType,
        const TUint8& aAutoType)
    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L("Exists(aUri,aRegType)")) );
    TRAP( err , WriteL(aUri) );
    TRAP( err , WriteCurrentTimeL() );
#endif

    TInt i = 0;
    for ( ; i < iList.Count() ; i++ )
        {
        if ( iList[i]->Uri()->Compare( aUri ) == 0 )
            {
            if ( (iList[i]->RegType()==aRegType) )
                {
                if ( (iList[i]->AutoType()==aAutoType) )
                    {
                    return i;
                    }
                }
            }
        }
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CEndTimeFactory::ResetAlarm
// Reset the alarm according to the end time based content list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEndTimeFactory::ResetAlarm()
    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L("ResetAlarm")) );
    TRAP( err , WriteCurrentTimeL() );
#endif
    TBool out = EFalse;
    TInt i = 0;
    TTime home;

    TTime oldAlarm( iAlarm ); // for overflow checks

    iAlarm.HomeTime();
    ResetExpired();

    for ( ; i < iList.Count() && !out ; i++ )
        {
        if ( !iList[i]->IsExpired() )
            {
            iAlarm = iList[i]->EndTime();
            out = ETrue;
            }
        }
    for ( ; i < iList.Count() ; i++ )
        {
        if ( !iList[i]->IsExpired() )
            {
            if ( iList[i]->EndTime() < iAlarm )
                {
                iAlarm = iList[i]->EndTime();
#ifdef _DRM_TESTING
                //test code start
                TRAP( err , WriteL(_L("ResetAlarm->Change iAlarm")) );
                TRAP( err , WriteL(iList[i]->Uri()->Des()) );
                TRAP( err , WriteCurrentTimeL() );
                TRAP( err , WriteL(_L("Change Time")) );
                TRAP( err , WriteTimeL(iAlarm) );
                //test code end
#endif
                }
            }

        }
    Cancel();
    home.HomeTime();

    //
    if ( iAlarm >= iOverflowedAlarm )
        {
        // let's keep old alarm, if desired alarm is later than last known overflow
        iAlarm = oldAlarm;
        }
    //

    if ( iAlarm > home && iAlarm != Time::MaxTTime() )
        {
#ifdef _DRM_TESTING
        //test code start
        TRAP( err , WriteL(_L("ResetAlarm->Trigger iAlarm")) );
        TRAP( err , WriteCurrentTimeL() );
        TRAP( err , WriteL(_L("Trigger Time")) );
        TRAP( err , WriteTimeL(iAlarm) );
        //test code end
#endif
        CTimer::At(iAlarm+TTimeIntervalSeconds(KInformingDelay));
        }
    }

// -----------------------------------------------------------------------------
// CEndTimeFactory::ResetExpired
// Reset the expiration status for all end time items
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEndTimeFactory::ResetExpired()
    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L("ResetExpired")) );
    TRAP( err , WriteCurrentTimeL() );
    TRAP( err , WriteL(_L("Time in the List")) );
#endif
    TInt i = 0;
    TTime home;
    home.HomeTime();
    for ( ; i<iList.Count() ; i++ )
        {
#ifdef _DRM_TESTING
        //test code start
        TRAP( err , WriteTimeL(iList[i]->EndTime()) );
        //test code end
#endif
        if ( !iList[i]->IsExpired() )
            {
            if ( iList[i]->EndTime() <= home )
                {
                iList[i]->SetCount(-1);
                iList[i]->SetIsExpired(ETrue);
                }
            }
        else
            {
            if ( iList[i]->EndTime() > home )
                {
                iList[i]->SetCount(-1);
                iList[i]->SetIsExpired(EFalse);
                }
            }

        }
    }

// -----------------------------------------------------------------------------
// CEndTimeFactory::HandleExpiredL
// Reset the expiration status for all end time items
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEndTimeFactory::HandleExpiredL()
    {
#ifdef _DRM_TESTING
    WriteL(_L("HandleExpiredL"));
    WriteCurrentTimeL();
#endif
    TInt i = 0;
    static TBool firstBoot = ETrue;
    ResetExpired();
    for ( ; i<iList.Count() ; i++ )
        {
        if ( iList[i]->IsExpired() && iList[i]->Count() < 0 )
            {
            iList[i]->IncreaseCount();
            iServer.HandleExpiredL( iList[i] );
            }
        if ( firstBoot && iList[i]->IsExpired() ) // Count 0 means do not give
            {                                      // note in Profiles
            iList[i]->IncreaseCount();             // If previous count is 0
            iServer.HandleExpiredL( iList[i] );    // Give a note with value 1
            }
        }
    firstBoot = EFalse;
    }

// -----------------------------------------------------------------------------
// CEndTimeFactory::HandleIdleL
// Handle about to expire- and expired- cases after idle
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEndTimeFactory::HandleIdleL()
    {
#ifdef _DRM_TESTING
    WriteL(_L("HandleIdleL"));
    WriteCurrentTimeL();
#endif
    TInt i = 0;
    iServer.UpdateL();
    ResetExpired();
    for ( ; i<iList.Count() ; i++ )
        {
        if ( iList[i]->IsExpired() )
            {
            iList[i]->IncreaseCount();
            iServer.HandleExpiredL( iList[i] );
            if ( iList[i]->Count() == 0 )           // Count 0 means do not give
                {                                   // note in Profiles
                iList[i]->IncreaseCount();          // If previous count is 0
                iServer.HandleExpiredL( iList[i] ); // Give a note with value 1
                }
            }
        }
    }



// -----------------------------------------------------------------------------
// CEndTimeFactory::ResetRegTimes
// reset the registered times for one specific content in the list
// -----------------------------------------------------------------------------
//
void CEndTimeFactory::ResetRegTimes(
        const TDesC8& aUri ,
        const TUint8& aPermType ,
        const TUint8& aRegType ,
        const TUint8& aAutoType ,
        const TUint8& aRegTimes )

    {
#ifdef _DRM_TESTING
    TRAPD( err , WriteL(_L("ResetRegTimes")) );
    TRAP( err , WriteL(aUri) );
    TRAP( err , WriteCurrentTimeL() );
#endif

    TInt index = Find( aUri , aPermType , aRegType , aAutoType );
    if ( index >= 0 )
        {
        iList[index]->SetRegTimes(aRegTimes);
        }
    }

// -----------------------------------------------------------------------------
// CEndTimeFactory::ExternalizeL
// return ETrue if item is about to expire
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEndTimeFactory::ExternalizeL( RWriteStream& aStream ) const
    {
#ifdef _DRM_TESTING
    WriteL(_L("ExternalizeL"));
    WriteCurrentTimeL();
#endif
    TInt i = 0;
    HBufC8* buf = NULL;
    aStream.WriteUint8L(iList.Count());
    for ( ; i<iList.Count() ; i++ )
        {
        buf = iList[i]->Uri();
        aStream.WriteUint8L(buf->Length());
        aStream.WriteL(buf->Des());
        aStream.WriteUint8L(iList[i]->PermType());
        aStream.WriteUint8L(iList[i]->RegType());
        aStream.WriteUint8L(iList[i]->AutoType());
        aStream.WriteInt32L(iList[i]->RegTimes());
#ifdef _DRM_TESTING
        //test code start
        WriteL(buf->Des());
        //test code end
#endif
        aStream.CommitL();
        buf = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CEndTimeFactory::InternalizeL
// return ETrue if item is about to expire
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEndTimeFactory::InternalizeL( RReadStream& aStream )
    {
#ifdef _DRM_TESTING
    WriteL(_L("InternalizeL"));
    WriteCurrentTimeL();
#endif

    TPtr8 ptr( NULL , 0 , 0 );
    TUint amount = 0;
    TUint len = 0;
    TUint8 permType = 0;
    TUint8 regType = 0;
    TUint8 autoType = 0;
    TInt regTimes = 0;
    HBufC8* buf = NULL;

    amount = aStream.ReadUint8L();


    for ( ; amount>0 ; amount-- )
        {
        len = aStream.ReadUint8L();
#ifdef _DRM_TESTING
        //test code start
        TBuf<20>amountBuf;
        _LIT(KLength , "Length:%d");
        amountBuf.Format(KLength , len);
        WriteL(amountBuf);
        //test code end
#endif
        buf = HBufC8::NewLC(len);
        ptr.Set(buf->Des());
        ptr.SetLength(len);
        aStream.ReadL(ptr , len);

#ifdef _DRM_TESTING
        //test code start
        WriteL(ptr);
        //test code end
#endif
        permType = aStream.ReadUint8L();
        regType = aStream.ReadUint8L();
        autoType = aStream.ReadUint8L();
#ifdef _DRM_TESTING
        //test code start
        _LIT(KPermType , "PermType:%d");
        amountBuf.Format(KPermType , permType);
        WriteL(amountBuf);
        _LIT(KRegType , "RegType:%d");
        amountBuf.Format(KRegType , regType);
        WriteL(amountBuf);
        _LIT(KAutoType , "AutoType:%d");
        amountBuf.Format(KAutoType , autoType);
        WriteL(amountBuf);

        //test code end
#endif
        iServer.RegisterL( ptr , permType , regType , autoType );

        regTimes = aStream.ReadInt32L();
#ifdef _DRM_TESTING
        //test code start
        _LIT(KRegTimes , "Times:%d");
        amountBuf.Format(KRegTimes , regTimes);
        WriteL(amountBuf);
        //test code end
#endif
        ResetRegTimes( ptr , permType , regType , autoType , regTimes );
        CleanupStack::PopAndDestroy(buf);
        }
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
//  End of File
