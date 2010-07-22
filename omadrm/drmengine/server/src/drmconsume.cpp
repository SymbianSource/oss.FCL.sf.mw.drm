/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the class CDRMConsume.
*
*/


// INCLUDE FILES
#include "drmconsume.h"
#include "drmenginetypedefs.h"
#include "drmrightsdb.h"
#include "DRMRightsServer.h"
#include "DRMDbSession.h"
#include "drmlog.h"
#include "DRMEventModify.h"
#include "DRMNotifier.h"
#ifdef RD_DRM_METERING
#include "drmmeteringdb.h"
#include "drmmeteringdbdata.h"
#include "RoapStorageClient.h"
#include "DRMRIContext.h"
#endif

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS

#define NOTIFIER static_cast< CDRMRightsServer* >( const_cast< CServer2* >( \
           iSession.Server() ) )->Notifier()

#define DB static_cast< CDRMRightsServer* >( const_cast< CServer2* >( \
           iSession.Server() ) )->Database()

#define SECURETIME( a ) static_cast< CDRMRightsServer* >\
           ( const_cast< CServer2* >( \
           iSession.Server() ) )->GetSecureTime( a )

#define SERVER static_cast< CDRMRightsServer* >( const_cast< CServer2* >( \
           iSession.Server() ) )

#ifdef RD_DRM_METERING
#define METERINGDB static_cast< CDRMRightsServer* >( const_cast< CServer2* >( \
           iSession.Server() ) )->MeteringDatabase()
#endif

#define SETBIT( a, b ) ( a ) |= ( b )
#define CLRBIT( a, b ) ( a ) &= ~( 0xff & b )
#define ISSET( a, b ) ( ( a ) & ( b ) )

// LOCAL CONSTANTS AND MACROS
// Keeps track about modified permissions for UpdateDBL().
//static const TUint8 KConsumeParentModified =   0x01;
static const TUint8 KConsumeChildModified =    0x02;

static const TUint8 KConsumeHasSecureTime =    0x08;

// These keep track whether to decrease timed counters when consuming time
// based stuff (iTimedCounts member). A used timed count bit is set to zero.
// In the beginning all of these are set to one.
//static const TUint8 KParentToplevelCount =     0x01;
//static const TUint8 KParentPermCount     =     0x02;
static const TUint8 KChildToplevelCount  =     0x04;
static const TUint8 KChildPermCount      =     0x08;

static const TUint16 KConsumeDefaultTimer = 300; // 5mins

// LOCAL FUNCTION PROTOTYPES
static inline void PickSmaller( TTime& aFirst,
                                const TTime& aSecond );

// ============================= LOCAL FUNCTIONS ==========================

// ------------------------------------------------------------------------
// PickSmaller (overloaded)
//
// Set the smaller one to aFirst
// ------------------------------------------------------------------------
//
void PickSmaller( TTime& aFirst,
                  const TTime& aSecond )
    {
    if ( aSecond < aFirst )
        {
        aFirst = aSecond;
        }
    }

// ------------------------------------------------------------------------
// PickSmaller (overloaded)
//
// Set the smaller one to aFirst
// ------------------------------------------------------------------------
//
void PickSmaller( TTimeIntervalSeconds& aFirst,
                  const TTimeIntervalSeconds& aSecond )
    {
    if ( aSecond < aFirst )
        {
        aFirst = aSecond;
        }
    }

// ============================ MEMBER FUNCTIONS ==========================

// ------------------------------------------------------------------------
// CDRMConsume::NewLC
// ------------------------------------------------------------------------
//
CDRMConsume* CDRMConsume::NewLC( CDRMDbSession& aSession,
                                 const TDesC8& aURI,
                                 const TDesC8* aParentId )
    {
    DRMLOG( _L( "CDRMConsume::NewLC" ) );
    CDRMConsume* self = new( ELeave ) CDRMConsume( aSession );

    CleanupStack::PushL( self );

    self->ConstructL( aURI, aParentId );

    DRMLOG( _L( "CDRMConsume::NewLC ok" ) );
    return self;
    }

// ------------------------------------------------------------------------
// CDRMConsume::~CDRMConsume
// ------------------------------------------------------------------------
//
CDRMConsume::~CDRMConsume()
    {
    DRMLOG( _L( "CDRMConsume::~" ) );
        
    if( iURI )
        {
        SERVER->RemoveAccessingUrl( *iURI );   
        }    
        
    TInt error( KErrNone );

    if( IsActive() )
        {
        Cancel();
        }
    else
        {
        TRAP( error, DoCancelL() );
        }
    #ifdef RD_DRM_METERING
        // Update metering count and metering accumulated time to the database.
        TRAP( error, UpdateMeteringDbL() );
    #endif
    // ignore errors
    //TRAP( error, UpdateDBL() );

    delete iURI; iURI = NULL;
    delete iParentId; iParentId = NULL;
    delete iChild; iChild = NULL;
    delete iCombined; iCombined = NULL;
    DRMLOG( _L( "CDRMConsume::~ ok" ) );
    }

// ------------------------------------------------------------------------
// CDRMConsume::CDRMConsume
//
// The internal state members iTimedCounts and iCounters are by default
// set to 00001111b, and the appropriate bits are lowered when something
// changes.
// ------------------------------------------------------------------------
//
CDRMConsume::CDRMConsume( CDRMDbSession& aSession ):
    CTimer( EPriorityHigh ),
    iSession( aSession ),
    iIntent( ContentAccess::EUnknown ),
    iURI( NULL ),
    iParentId( NULL ),
    iTimedCounts( 0xf ),
    iCounters( 0xf ),
    iCountConstraintActive( EFalse ),
    iExpired( EFalse ),
    iCumulativeDelayTop( 0 ),
    iCumulativeDelayChild( 0 ),
    iTotalCumulativeTime( 0 ),
    iUsingTimedCount( 0 )
    {
    }

// ------------------------------------------------------------------------
// CDRMConsume::ConstructL
// ------------------------------------------------------------------------
//
void CDRMConsume::ConstructL( const TDesC8& aURI,
                              const TDesC8* aParentId )
    {
    DRMLOG( _L( "CDRMConsume::ConstructL" ) );
    CTimer::ConstructL();
    iURI = aURI.AllocL();
    if( aParentId )
        {
        iParentId = aParentId->AllocL();
        }
        
    SERVER->AddAccessingUrlL( aURI );
    }

// ------------------------------------------------------------------------
// CDRMConsume::HandleL
// ------------------------------------------------------------------------
//
void CDRMConsume::HandleL( ContentAccess::TIntent aIntent )
    {
    DRMLOG( _L( "CDRMConsume::HandleL" ) );

    TTime time;
    TBool secure( SECURETIME( time ) );
    InitializeL( aIntent, secure, time );

    DRMLOG( _L( "CDRMConsume::HandleL ok" ) );
    }

// ------------------------------------------------------------------------
// CDRMConsume:: Pause
// ------------------------------------------------------------------------
//
void CDRMConsume::Pause()
    {
    DRMLOG( _L( "CDRMConsume::Pause" ) );

    Cancel();

    DRMLOG( _L( "CDRMConsume::Pause ok" ) );
    }

// ------------------------------------------------------------------------
// CDRMConsume:: ContinueL
// ------------------------------------------------------------------------
//
void CDRMConsume::ContinueL()
    {
    DRMLOG( _L( "CDRMConsume::ContinueL" ) );

    Cancel();

    TInt error = KErrNone;
    TUint32 reason = 0;

    CDRMPermission* child = NULL;
    HBufC8* parent = NULL;

    // This got removed by Pause()
    if( iCountConstraintActive )
        {
        SERVER->AddActiveCountConstraintL( *iURI );
        DoContinueL();
        return;
        }

    /* Fetch the RO again in order to manage a situation when another
       instance has used the same RO (content has been consumed) while the
       other instance has not been used (player is paused) but will be
       used again (content consumption is to be continued). */
    error = iSession.FindRightsObject( iIntent, *iURI, child, parent, reason );

    /* Check if the RO found from the database matches with the one that was used
       in the original consumption of the content. If the ROs match, use the
       (possibly updated) one from the database. */
    if ( !error && child && iChild &&
         ( iChild->iUniqueID == child->iUniqueID ) &&
         ( iChild->iOriginalInsertTime == child->iOriginalInsertTime ) )
        {
        if ( iParentId )
            {
            delete iParentId;
            iParentId = parent;
            }

        delete iChild;
        iChild = child;
        }
    else
        {
        /* The original RO was not found. Delete temporary objects and also
           delete the iChild and iCombined because the RO is no longer valid and
           need to be re-fetched using InitializeL method (in DoContinueL).
           (in DoContinueL). */
        if ( iChild )
            {
            delete iChild;
            iChild = NULL;
            }

        if ( iCombined )
            {
            delete iCombined;
            iCombined = NULL;
            }

        if ( child )
            {
            delete child;
            child = NULL;
            }
        if ( parent )
            {
            delete parent;
            parent = NULL;
            }
        }

    DoContinueL();

    //HandleL( iIntent );

    DRMLOG( _L( "CDRMConsume::ContinueL ok" ) );
    }

// ------------------------------------------------------------------------
// CDRMConsume:: Stop
// ------------------------------------------------------------------------
//
void CDRMConsume::Stop()
    {
    DRMLOG( _L( "CDRMConsume::Stop" ) );

    Cancel();

    DRMLOG( _L( "CDRMConsume::Stop ok" ) );
    }

// ------------------------------------------------------------------------
// CDRMConsume:: ActivateL
//
// Calculate the smallest end time based on interval, end time,
// accumulated time & timed count.
// ------------------------------------------------------------------------
//
void CDRMConsume::ActivateL( TBool aSecureTime,
                             const TTime& aTrustedTime  )
    {
    DRMLOG( _L( "CDRMConsume::ActivateL" ) );

    __ASSERT_DEBUG( iChild && iCombined, User::Invariant() );
    TTime endTime( Time::MaxTTime() );
    TTimeIntervalSeconds timed( KMaxTInt32 );
    TBool timeUsed( EFalse );
    TBool endTimeUsed( EFalse );

    iCurrentDelay = 0;

    if ( iCombined->iActiveConstraints & EConstraintTimedCounter )
        {
        // Take this, even if timed counts have been updated.
        // This might cause unnecessary RunL's to be called, but it
        // ensures both child & parent will be consumed when needed.
        // If e.g. it would be checked that iTimedCounts == 0xf,
        // either one (child or parent) might not get updated in case of
        // "Child expired, but parent didn't -> find new child".
        PickSmaller( timed, iCombined->iTimedInterval );
        timeUsed = ETrue;
        }

    if ( iCombined->iActiveConstraints & EConstraintAccumulated )
        {
        PickSmaller( timed, iCombined->iAccumulatedTime );
        timeUsed = ETrue;
        }

    if ( iCombined->iActiveConstraints & EConstraintInterval )
        {
        if ( iCombined->iIntervalStart != Time::NullTTime() )
            {
            endTime = iCombined->iIntervalStart;
            endTime += iCombined->iInterval;
            endTimeUsed = ETrue;
            }
        else
            {
            TInt64 tmp( iCombined->iInterval.Int() );

            PickSmaller( timed, tmp );
            timeUsed = ETrue;
            }
        }

    if ( iCombined->iActiveConstraints & EConstraintEndTime )
        {
        PickSmaller( endTime, iCombined->iEndTime );
        endTimeUsed = ETrue;
        }

    // Put the "smallest time" information to "endTime".
    if ( timeUsed )
        {
        TTime current( aTrustedTime );

        current += timed;

        PickSmaller( endTime, current );
        endTimeUsed = ETrue;
        }

     // Interval gets initialised immediately, and so do count constraints.
     // Timed/accumulated won't: those are consumed after the
     // interval if secure time exists.
     Consume( ETrue, ETrue, EFalse, 0,
              aSecureTime,
              aTrustedTime );

     // In case something was modified, update the db also.
     UpdateDBL();

     if ( endTimeUsed )
        {
        // Something exists.
        TTimeIntervalSeconds secs( 0 );
        TTime current( aTrustedTime );
        TInt err( KErrNone );

        // SecondsFrom returns an error if the difference is too great.
        err = endTime.SecondsFrom( current, secs );
        if ( err )
            {
            iCurrentDelay = KConsumeDefaultTimer;
            }
        else if ( secs.Int() < 0 )
            {
            iCurrentDelay = 0; // Already expired.
            }
        else if ( secs.Int() < KConsumeDefaultTimer )
            {
            iCurrentDelay = secs.Int();
            }
        else
            {
            iCurrentDelay = KConsumeDefaultTimer;
            }

        if ( !IsAdded() )
            {
            CActiveScheduler::Add( this );
            }

        DRMLOG2( _L( "CDRMConsume::ActivateL: using interval %d" ),
                  ( TInt )iCurrentDelay );

        // secs -> microsecs. The method sets the AO active.
        After( TTimeIntervalMicroSeconds32( iCurrentDelay * 1000000 ) );

        iTime = current;

        // If we see timed things here, we also have secure time.
        //SETBIT( iMask, KConsumeHasSecureTime );
        }
    else    // For metering we always need to have this:
        {
        iCurrentDelay = KConsumeDefaultTimer;
        iTime = aTrustedTime;

        if ( !IsAdded() )
            {
            CActiveScheduler::Add( this );
            }

        DRMLOG2( _L( "CDRMConsume::ActivateL: using interval %d" ),
                  ( TInt )iCurrentDelay );

        // secs -> microsecs. The method sets the AO active.
        After( TTimeIntervalMicroSeconds32( iCurrentDelay * 1000000 ) );
        }

    // If we see timed things here, we also have secure time.
    if( aTrustedTime != Time::NullTTime())
        {
        SETBIT( iMask, KConsumeHasSecureTime );
        }
    DRMLOG( _L( "CDRMConsume::ActivateL ok" ) );
    }

// ------------------------------------------------------------------------
// CDRMConsume:: Consume
//
// Consume child & parent. Whether to consume parent's explicit
// usage permission: the information is returned earlier by
// FindRightsObjectL.
// ------------------------------------------------------------------------
//
void CDRMConsume::Consume( TBool aUpdateCounter,
                           TBool aInitInterval,
                           TBool aUpdateTimedCount,
                           const TTimeIntervalSeconds& aElapsedTime,
                           TBool aSecureTime,
                           const TTime& aTrustedTime )
    {
    DRMLOG( _L( "CDRMConsume::Consume" ) );

    __ASSERT_DEBUG( iChild && iCombined, User::Invariant() );

    // Decrease timed counters & regular counters only once.
    // In the beginning the bitmasks are both 0xF.
    if ( iChild->TopLevelConstraint() )
        {
        if ( ConsumeConstraint( *( iChild->TopLevelConstraint() ),
                                ( aUpdateCounter &&
                                  ISSET( iCounters, KChildToplevelCount ) ),
                                aInitInterval,
                                ( aUpdateTimedCount &&
                                  ISSET( iTimedCounts,
                                         KChildToplevelCount ) ),
                                aElapsedTime,
                                aSecureTime,
                                aTrustedTime,
                                iCumulativeDelayTop ) )
            {
            SETBIT( iMask, KConsumeChildModified );
            if ( aUpdateTimedCount )
                {
                CLRBIT( iTimedCounts, KChildToplevelCount );
                }
            if ( aUpdateCounter )
                {
                CLRBIT( iCounters, KChildToplevelCount );
                }
            }
        }

    if ( ConsumeConstraint( *( iChild->ConstraintForIntent( iIntent ) ),
                            ( aUpdateCounter &&
                              ISSET( iCounters, KChildPermCount ) ),
                            aInitInterval,
                            ( aUpdateTimedCount &&
                              ISSET( iTimedCounts, KChildPermCount ) ),
                            aElapsedTime,
                            aSecureTime,
                            aTrustedTime,
                            iCumulativeDelayChild ) )
        {
        SETBIT( iMask, KConsumeChildModified );
        if ( aUpdateTimedCount )
            {
            CLRBIT( iTimedCounts, KChildPermCount );
            }
        if ( aUpdateCounter )
            {
            CLRBIT( iCounters, KChildPermCount );
            }
        }

    DRMLOG( _L( "CDRMConsume::Consume ok" ) );
    }

// ------------------------------------------------------------------------
// CDRMConsume::RunL
// ------------------------------------------------------------------------
//
void CDRMConsume::RunL()
    {
    DRMLOG2( _L( "CDRMConsume::RunL with %d" ), iStatus.Int() );

    switch ( iStatus.Int() )
        {
        case KErrNone:
            // Normal completition.

        case KErrUnderflow:
            // Time already passed.

        case KErrAbort:
            // System time changed ==> consume.
            DoContinueL();
            break;

        default:
            // Some other (real) error.
            // Handled in RunError.
            User::Leave( iStatus.Int() );
        };

    DRMLOG( _L( "CDRMConsume::RunL ok" ) );
    }

// ------------------------------------------------------------------------
// CDRMConsume::RunError
// ------------------------------------------------------------------------
//
#if defined( _DEBUG ) || defined( _LOGGING )
TInt CDRMConsume::RunError( TInt aError )
#else
TInt CDRMConsume::RunError( TInt /* aError */ )
#endif
    {
    DRMLOG2( _L( "CDRMConsume::RunError: %d" ), aError );

    Deque();

    DRMLOG( _L( "CDRMConsume::RunError ok" ) );
    return 0;
    }

// ------------------------------------------------------------------------
// CDRMConsume::DoCancel
// ------------------------------------------------------------------------
//
void CDRMConsume::DoCancel()
    {
    DRMLOG( _L( "CDRMConsume::DoCancel" ) );

    TInt error( KErrNone );
    TRAP( error, DoCancelL() );

    DRMLOG2( _L( "CDRMConsume::DoCancel: %d" ), error );
    }

// ------------------------------------------------------------------------
// CDRMConsume::DoCancelL
// ------------------------------------------------------------------------
//
void CDRMConsume::DoCancelL()
    {
    DRMLOG( _L( "CDRMConsume::DoCancelL" ) );

    if ( iCurrentDelay )
        {
        TTimeIntervalSeconds secs;
        TTime trustedTime;
        TBool secureTime;

        CTimer::DoCancel();

        secureTime = SECURETIME( trustedTime );
        trustedTime.SecondsFrom( iTime, secs );

        #ifdef RD_DRM_METERING
            // Update total cumulative time for content metering purposes
            iTotalCumulativeTime = iTotalCumulativeTime.Int() + secs.Int();
        #endif

        // If the top level timed counter has not been activated yet
        // increment the counter
        if( ISSET( iTimedCounts, KChildToplevelCount ) )
            {
            iCumulativeDelayTop = iCumulativeDelayTop.Int() + secs.Int();
            }

        // If the child timed counter has not been activated yet
        // increment the counter
        if( ISSET( iTimedCounts, KChildPermCount ) )
            {
            iCumulativeDelayChild = iCumulativeDelayChild.Int() + secs.Int();
            }

        // Always >= 0.
        ConsumeTimedItemsL( secs,
                            secureTime,
                            trustedTime );
        iCurrentDelay = 0;
        }

    UpdateDBL();

    if ( SERVER->HasActiveCountConstraint( *iURI ) )
        {
        SERVER->RemoveActiveCountConstraint( *iURI );
        }

    DRMLOG( _L( "CDRMConsume::DoCancel ok" ) );
    }

// ------------------------------------------------------------------------
// CDRMConsume::CombinePermissionsL
//
// Combine iChild's & iParent's top level constraints, and merge the usage
// intention constraint (either one of them).
// ------------------------------------------------------------------------
//
void CDRMConsume::CombinePermissionsL()
    {
    DRMLOG( _L( "CDRMConsume::CombinePermissions" ) );

    __ASSERT_DEBUG( iChild, User::Invariant() );

    // Reset
    delete iCombined; iCombined = NULL;
    iCombined = CDRMConstraint::NewL();

    if ( iChild->TopLevelConstraint() )
        {
        iCombined->Merge( *( iChild->TopLevelConstraint() ) );
        }

    iCombined->Merge( *( iChild->ConstraintForIntent( iIntent ) ) );

    DRMLOG( _L( "CDRMConsume::CombinePermissions ok" ) );
    }

// ------------------------------------------------------------------------
// CDRMConsume::UpdateDBL
//
// Update the DB if something was changed.
// ------------------------------------------------------------------------
//
void CDRMConsume::UpdateDBL()
    {
    DRMLOG( _L( "CDRMConsume::UpdateDBL" ) );
    // __ASSERT_DEBUG( iChild, User::Invariant() );

    CDRMEventModify* event = NULL;
    TRequestStatus status;

    if( ISSET( iMask, KConsumeChildModified ) )
        {
        event = CDRMEventModify::NewL();
        }
    CleanupStack::PushL( event );

    if ( ISSET( iMask, KConsumeChildModified ) )
        {
        DRMLOG( _L( "CDRMConsume: commiting child to DB" ) );

        if( iParentId )
            {
            DB.UpdateDBEntryL( *iParentId, *iChild );
            // Notify
            event->SetContentIDL(*iParentId);
            event->SetUniqueID(iChild->iUniqueID);
            }
        else
            {
            DB.UpdateDBEntryL( *iURI, *iChild );
            // Notify
            event->SetContentIDL(*iURI);
            event->SetUniqueID(iChild->iUniqueID);
            }

        // Notify
        event->SetContentIDL(*iURI);
        event->SetUniqueID(iChild->iUniqueID);

        NOTIFIER.SendEventL(*event,status);
        User::WaitForRequest(status);

        CLRBIT( iMask, KConsumeChildModified );
        }

    CleanupStack::PopAndDestroy();
    DRMLOG( _L( "CDRMConsume::UpdateDBL ok" ) );
    }
// ------------------------------------------------------------------------
// CDRMConsume::ConsumeConstraint
//
// Consume child & parent. Whether to consume parent's explicit
// usage permission: the information is returned earlier by
// FindRightsObjectL.
// ------------------------------------------------------------------------
//
TBool CDRMConsume::ConsumeConstraint( CDRMConstraint& aConstraint,
                                      TBool aUpdateCounter,
                                      TBool aInitInterval,
                                      TBool aUpdateTimedCount,
                                      const TTimeIntervalSeconds& aElapsedTime,
                                      TBool aSecureTime,
                                      const TTime& aTrustedTime,
                                      TTimeIntervalSeconds& aCumulativeTime )
    {
    DRMLOG( _L( "CDRMConsume::ConsumeConstraints" ) );

    TBool res( EFalse );

    if ( aUpdateCounter &&
         ( aConstraint.iActiveConstraints & EConstraintCounter ) )
        {
        --( aConstraint.iCounter);
        res = ETrue;
        iCountConstraintActive = ETrue;
        TRAP_IGNORE( SERVER->AddActiveCountConstraintL( *iURI ) );
        }

    if ( aInitInterval &&
         ( aConstraint.iActiveConstraints & EConstraintInterval ) &&
         aSecureTime &&
         aConstraint.iIntervalStart == Time::NullTTime() )
        {
        aConstraint.iIntervalStart = aTrustedTime;
        res = ETrue;
        }

    /* change to timed counter, we don't check the latest time, we check the
    cumulated time */

    if ( aUpdateTimedCount &&
         ( aConstraint.iActiveConstraints & EConstraintTimedCounter &&
           ( aCumulativeTime >= aConstraint.iTimedInterval ||
             aElapsedTime >= aConstraint.iTimedInterval  ) ) )
        {
        --( aConstraint.iTimedCounter );
        res = ETrue;
        iCountConstraintActive = ETrue;
        TRAP_IGNORE( SERVER->AddActiveCountConstraintL( *iURI ) );
        aCumulativeTime = TTimeIntervalSeconds( 0 );
        }
    else if ( aUpdateTimedCount &&
         ( aConstraint.iActiveConstraints & EConstraintTimedCounter ) )
        {
        iUsingTimedCount = ETrue;
        }


    if ( aElapsedTime.Int() != 0 &&
         ( aConstraint.iActiveConstraints & EConstraintAccumulated ) )
        {
        __ASSERT_DEBUG( aElapsedTime.Int() > 0, User::Invariant() );

        if ( aConstraint.iAccumulatedTime < aElapsedTime )
            {
            aConstraint.iAccumulatedTime = 0;
            }
        else
            {
            aConstraint.iAccumulatedTime = aConstraint.iAccumulatedTime.Int() -
                aElapsedTime.Int();
            }

        res = ETrue;
        }

    if ( !iCountConstraintActive && aConstraint.Expired( aTrustedTime ) )
        {
        iExpired = ETrue;
        }
    else
        {
        iExpired = EFalse;
        }

    DRMLOG2(
        _L( "CDRMConsume::ConsumeConstraints ok, returning %d" ),
        ( TInt )res );

    return res;
    }

// ------------------------------------------------------------------------
// CDRMConsume::ConsumeTimedItemsL
// ------------------------------------------------------------------------
//
void CDRMConsume::ConsumeTimedItemsL( TTimeIntervalSeconds aDelay,
                                      TBool aSecureTime,
                                      const TTime& aTrustedTime )
    {
    DRMLOG( _L( "CDRMConsume::ConsumeTimedItemsL" ) );

    // Update accumulated constraints & timed count.
    Consume( EFalse,
             ETrue,
             ETrue,
             aDelay,
             aSecureTime,
             aTrustedTime );

    DRMLOG( _L( "CDRMConsume::ConsumeTimedItems ok" ) );
    }

// ------------------------------------------------------------------------
// CDRMConsume::DoContinueL
// ------------------------------------------------------------------------
//
void CDRMConsume::DoContinueL()
    {
    DRMLOG( _L( "CDRMConsume::DoContinueL" ) );

    TTime time;
    TBool secureTime( EFalse );
    secureTime = SECURETIME( time );

    if ( !iChild || !iCombined )
        {
        InitializeL( iIntent, secureTime, time );
        // User::Leave( KErrCANoRights );
        return;
        }

    ConsumeTimedItemsL( iCurrentDelay, secureTime, time );
    UpdateDBL();
    iCurrentDelay = 0;

    CombinePermissionsL();

    // If the content has expired, find new permissions, unless the expired
    // constraint was a timed count. In that case, the constraint
    // did not really expire.
    if ( SecurityLevelChanged( secureTime ) ||
         ( iCombined->Expired( time ) &&
           !SERVER->HasActiveCountConstraint( *iURI ) ) )
        {
        InitializeL( iIntent, secureTime, time );
        }
    else
        {
        ActivateL( secureTime, time );
        }

    DRMLOG( _L( "CDRMConsume::DoContinueL ok" ) );
    }

// ------------------------------------------------------------------------
// CDRMConsume::SecurityLevelChanged
// ------------------------------------------------------------------------
//
TBool CDRMConsume::SecurityLevelChanged( TBool aSecureTime ) const
    {
    DRMLOG( _L( "CDRMConsume::SecurityLevelChanged" ) );

    if ( ( ISSET( iMask, KConsumeHasSecureTime ) && aSecureTime ) ||
         !( ISSET( iMask, KConsumeHasSecureTime ) || aSecureTime ) )
        {
        return EFalse;
        }

    return ETrue;
    }

// ------------------------------------------------------------------------
// CDRMConsume::InitializeL
// ------------------------------------------------------------------------
//
void CDRMConsume::InitializeL( ContentAccess::TIntent aIntent,
                               TBool aSecureTime,
                               const TTime& aTrustedTime )
    {
    DRMLOG( _L( "CDRMConsume::InitializeL" ) );

    // aIntent is either EPlay, EView, EExecute or EPrint.
    // Store the old  consumption information in case this is
    // called in case of "ran out of permissions, find new ones"
    CDRMPermission* child( iChild );
    TUint8 timedCounts( iTimedCounts );
    TUint8 counters( iCounters );
    TUint32 reason = 0;

    // Reset.
    iTimedCounts = 0xf;
    iCounters = 0xf;
    iMask = 0x0;
    iCurrentDelay = 0;

    iExpired = ETrue;

    delete iCombined; iCombined = NULL;

    // Previous child & parent need to be stored in case the internal
    // counter states need to be restored.
    if ( child )
        {
        CleanupStack::PushL( child );
        iChild = NULL;
        }

    // If the next call won't leave, we have permissions.
    User::LeaveIfError( iSession.FindRightsObject( aIntent,
                                                   *iURI,
                                                   iChild,
                                                   iParentId,
                                                   reason ) );

    User::LeaveIfError( iSession.VerifyCredentials( iURI, iChild, aIntent ) );

    iExpired = EFalse;

    // Check whether to restore the internal state.
    if ( iIntent == aIntent )
        {
        if ( child &&
             ( child->iUniqueID == iChild->iUniqueID ) )
            {
            DRMLOG( _L( "CDRMConsume: using the previous child" ) );

            CLRBIT( iTimedCounts,
                    ( KChildToplevelCount | KChildPermCount ) & ~timedCounts );

            CLRBIT( iCounters,
                    ( KChildToplevelCount | KChildPermCount ) & ~counters );
            }
        }

    if ( child )
        {
        CleanupStack::PopAndDestroy();
        }

    iIntent = aIntent;

    CombinePermissionsL();
    ActivateL( aSecureTime, aTrustedTime );

    DRMLOG( _L( "CDRMConsume::InitializeL ok" ) );
    }

// ------------------------------------------------------------------------
// CDRMConsume::CountConstraintActive
// ------------------------------------------------------------------------
//
TBool CDRMConsume::CountConstraintActive()
    {
    return iCountConstraintActive || SERVER->HasActiveCountConstraint( *iURI );
    }

// ------------------------------------------------------------------------
// CDRMConsume::IsExpired
// ------------------------------------------------------------------------
//
TBool CDRMConsume::IsExpired()
    {
    return iExpired;
    }

// ------------------------------------------------------------------------
// CDRMConsume::GetChild
// ------------------------------------------------------------------------
//
CDRMPermission& CDRMConsume::GetChild()
    {
    return *iChild;
    }

#ifdef RD_DRM_METERING

// ------------------------------------------------------------------------
// CDRMConsume::UpdateMeteringDbL
// ------------------------------------------------------------------------
//
void CDRMConsume::UpdateMeteringDbL()
    {
    DRMLOG( _L( "CDRMConsume::UpdateMeteringDbL" ) );
    TTimeIntervalSeconds graceTime = 0;
    CDRMConstraint* constraint = NULL;

    if ( iChild )
        {
        constraint = iChild->ConstraintForIntent( iIntent );
        if ( constraint && constraint->iDrmMeteringInfo )
            {

            graceTime = constraint->iDrmMeteringInfo->iGraceTime;

            // Grace time exceeded, increase metering count and metering
            // accumulated time
            if ( iTotalCumulativeTime >= graceTime )
                {
                // Check Rights Issuer rights from Roap storage
                if ( !CheckRiContextRightsL( iChild->iRiId ) )
                    {
                    return;
                    }

                // Update database only if Rights Issuer Id is available
                if ( ( iChild->iRiId.Length() > 0 ) &&
                     ( iChild->iRiId.Length() <= KRiIdSize ) )
                    {

                    if ( iURI )
                        {
                        CDrmMeteringDbData* meteringData =
                            CDrmMeteringDbData::NewLC();
                        meteringData->iContentId = iURI->AllocL();
                        meteringData->iRiId = iChild->iRiId;
                        if( iUsingTimedCount && !iCountConstraintActive )
                            {
                            meteringData->iCount = 0;
                            }
                        else
                            {
                        meteringData->iCount = 1;
                            }

                        meteringData->iAccumulatedTime = iTotalCumulativeTime;

                        if ( iParentId )
                            {
                            meteringData->iParentUid = iParentId->AllocL();
                            }

                        METERINGDB.AddL( meteringData );
                        CleanupStack::PopAndDestroy(); // meteringData
                        }
                    }
                }
            }
        }
        DRMLOG( _L( "CDRMConsume::UpdateMeteringDbL ok" ) );
    }

// ------------------------------------------------------------------------
// CDRMConsume::CheckRiContextRightsL
// ------------------------------------------------------------------------
//

TBool CDRMConsume::CheckRiContextRightsL( const TDesC8& aRiId )
    {

    DRMLOG( _L( "CDRMConsume::CheckRiContextRightsL" ) );

    CDRMRIContext* riContext = NULL;
    TBool haveRights = EFalse;

    // Connect to the storage of the registered Rights Issuers

    if( iSession.ConnectRoapClient() == KErrNone )
        {
    riContext = iSession.RoapClient().GetRIContextL( aRiId );

    if ( riContext == NULL )
        {
        DRMLOG( _L ( "RI not registered" ) );
        User::Leave( KErrRightsServerRiNotRegistered );
        }

    // Check (via the Rights Issuer context) whether Rights Issuer
    // is allowed to use metering or not
    if ( riContext->IsMeteringAllowed() )
        {
        haveRights = ETrue;
        }

    if ( riContext )
        {
        delete riContext;
        riContext = NULL;
        }
        }

    return haveRights;

    }

#endif

// EOF
