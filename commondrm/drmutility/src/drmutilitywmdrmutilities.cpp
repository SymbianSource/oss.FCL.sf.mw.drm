/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  WM DRM specific methods for DRM Rights Manager
*
*/


#include <e32std.h>
#include <DRMRights.h>

#include "DRMClockClient.h"
#include "drmutilitywmdrmutilities.h"
#include "DRMRights.h"

// CONSTANTS
_LIT( KLicenseType, "licensetype=" );
_LIT( KTime, "time" );
_LIT( KCount, "count" );
_LIT( KDuration, "duration" );
_LIT( KTimeCount, "time-count" );
_LIT( KUnlimited, "unlimited" );

_LIT( KDurationLeft, "duration=" );
_LIT( KCountLeft, "countleft=" );
_LIT( KStartTime, "starttime=" );
_LIT( KEndTime, "endtime=" );

const TUint8 KFoundStartTime = 0x01;
const TUint8 KFoundEndTime = 0x02;

// masks for constraint existence
const TInt KWmDrmConstraintCount( 1 );
const TInt KWmDrmConstraintStartTime ( 2 );
const TInt KWmDrmConstraintEndTime( 4 );
const TInt KWmDrmConstraintDuration( 8 );

#ifdef _DEBUG
// debug panic
_LIT( KWmDrmUtilitiesDebugPanicMessage, "WmDrmUtilitiesDebugPanic" );
const TInt KWmDrmUtilitiesDebugPanicCode( 2 );
#endif

// LOCAL FUNCTION PROTOTYPES
// Parses the given lexicographic representation. Return value indicates
// whether the given end character of the searched substring was found or not.
// Special handling is applied for certain separators (e.g. ';', '-')
LOCAL_C TBool ParseString( TChar aChar, TLex& aLex, TPtrC16& aPtr );

// ================= MEMBER FUNCTIONS =======================

LOCAL_C TBool ParseString( TChar aChar, TLex& aLex, TPtrC16& aPtr )
    {
    TBool found( EFalse );
    aLex.Mark();

    while ( !aLex.Eos() && found == EFalse )
        {
        TChar ch;
        ch = aLex.Get();
        if ( ch == aChar )
            {
            if ( ch == ';' || ch == '-' || ch == ':' || ch == 'T' || ch == 'Z' )
                {
                // Remove char from the token string and read it from the lex.
                aLex.UnGet();
                aPtr.Set( aLex.MarkedToken() );
                aLex.Get();
                }
            else
                {
                aPtr.Set( aLex.MarkedToken() );
                }
            found = ETrue;
            }
        }
    return found;
    }

// ----------------------------------------------------------------------------
// DrmUtilityWmDrmUtilities::ParseWmDrmTimeRightsL
// ----------------------------------------------------------------------------
EXPORT_C void DrmUtilityWmDrmUtilities::ParseWmDrmTimeRightsL( TLex& aLex,
    TTime& aStartTime, TTime& aEndTime )
    {
    TPtrC16 token( NULL, 0 );
    TUint8 times( 0 );
    TTime localTime( Time::NullTTime() );
    TTime universalTime( Time::NullTTime() );
    TTimeIntervalSeconds timeDifference( 0 ); // local time - secure time
    TInt years, months, days, hours, minutes, secs;
    TChar ch;
    TLex lex;
    TTime drmTime;
    TInt timeZone;

    DRMClock::ESecurityLevel secLevel = DRMClock::KInsecure;

    // The format of the start and end time strings is the following:
    // "starttime=yy-mm-ddThh:mins:ssZ
    // "endtime=yy-mm-ddThh:mins:ssZ
    // where yy = years, mm = months, dd = days, hh = hours,
    // mins = minutes and ss = seconds, "-", "T", ":" and "Z"
    // are separators.
    while ( !aLex.Eos() && ( aLex.Peek() != 0 ) )
        {
        // The next string should be start time, end time or then
        // it does not exist.
        ParseString( '=', aLex, token );

        if ( token.CompareF( KStartTime ) == 0 )
            {
            times |= KFoundStartTime;
            }
        else if ( token.CompareF( KEndTime ) == 0 )
            {
            times |= KFoundEndTime;
            }
        else
            {
            // No start time ("starttime=") or end time ("endtime=")
            // string was found -> return
            return;
            }

        if ( ( times & KFoundStartTime ) || ( times & KFoundEndTime ) )
            {
            // Search for years, months, days, hours, minutes, secs
            if ( ParseString( '-', aLex, token ) )
                {
                TLex lexYear( token );
                User::LeaveIfError( lexYear.Val( years ) );
                }
            else
                {
                User::Leave( KErrNotFound );
                }

            if ( ParseString( '-', aLex, token ) )
                {
                TLex lexMonths( token );
                User::LeaveIfError( lexMonths.Val( months ) );
                }
            else
                {
                User::Leave( KErrNotFound );
                }

            if ( ParseString( 'T', aLex, token ) )
                {
                TLex lexDays( token );
                User::LeaveIfError( lexDays.Val( days ) );
                }
            else
                {
                User::Leave( KErrNotFound );
                }

            if( ParseString( ':', aLex, token ) )
                {
                TLex lexHours( token );
                User::LeaveIfError( lexHours.Val( hours ) );
                }
            else
                {
                User::Leave( KErrNotFound );
                }

            if ( ParseString( ':', aLex, token ) )
                {
                TLex lexMinutes( token );
                User::LeaveIfError( lexMinutes.Val( minutes ) );
                }
            else
                {
                User::Leave( KErrNotFound );
                }

            if( ParseString( 'Z', aLex, token ) )
                {
                TLex lexSeconds( token );
                User::LeaveIfError( lexSeconds.Val( secs ) );
                }
            else
                {
                User::Leave( KErrNotFound );
                }

            ParseString( ';', aLex, token );
            token.Set( NULL, 0 );

            TDateTime date( years, (TMonth)( months - 1 ), days - 1, hours,
                minutes, secs, 0 );
            TTime dateTime( date );

            // Get secure time from DRM clock
            RDRMClockClient client;

            User::LeaveIfError( client.Connect() );
            client.GetSecureTime( drmTime, timeZone, secLevel );
            client.Close();

            localTime.HomeTime();

            if ( secLevel == DRMClock::KSecure )
                {
                // Calculate the difference between local time and secure time
                localTime.SecondsFrom( drmTime, timeDifference );
                }
            else
                {
                // Calculate the difference between local and universal time
                universalTime.UniversalTime();
                localTime.SecondsFrom( universalTime, timeDifference );
                }

            dateTime += timeDifference;

            if ( times & KFoundStartTime )
                {
                aStartTime = dateTime;
                times &= ~KFoundStartTime;
                }
            else if ( times & KFoundEndTime )
                {
                aEndTime = dateTime;
                times &= ~KFoundEndTime;
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// DrmUtilityWmDrmUtilities::ParseWmDrmCountRightsL
// ----------------------------------------------------------------------------
//
EXPORT_C TBool DrmUtilityWmDrmUtilities::ParseWmDrmCountRightsL( TLex& aLex,
    TUint32& aCounts )
    {
    TPtrC16 token( NULL, 0 );
    TInt counts( 0 );
    TChar ch;
    TLex lex;

    ParseString( '=', aLex, token );

    if ( token.CompareF( KCountLeft ) == 0 )
        {
        ParseString( ';', aLex, token );
        TLex lexCount( token );
        User::LeaveIfError( lexCount.Val( counts ) );
        aCounts = counts;
        return ETrue;
        }
    else
        {
        // No counts left ("countleft=") string was found.
        // -> No count information was found, return EFalse
        return EFalse;
        }
    }

// ----------------------------------------------------------------------------
// DrmUtilityWmDrmUtilities::ParseWmDrmDurationRightsL
// ----------------------------------------------------------------------------
//
EXPORT_C TBool DrmUtilityWmDrmUtilities::ParseWmDrmDurationRightsL( TLex& aLex,
    TTimeIntervalSeconds& aDuration )
    {
    TPtrC16 token( NULL, 0 );
    TChar ch;
    TInt duration( 0 );

    ParseString( '=', aLex, token );

    if ( token.CompareF( KDurationLeft ) == 0 )
        {
        ParseString( ';', aLex, token );
        TLex lexDuration( token );
        User::LeaveIfError( lexDuration.Val( duration ) );
        aDuration = duration;
        return ETrue;
        }
    else
        {
        // No duration left ("durationleft=") string was found.
        // -> No duration information was found, return EFalse
        return EFalse;
        }
    }

// ----------------------------------------------------------------------------
// DrmUtilityWmDrmUtilities::ParseWmDrmStringL
// ----------------------------------------------------------------------------
//
EXPORT_C void DrmUtilityWmDrmUtilities::ParseWmDrmStringL(
    ContentAccess::CRightsInfo& aRights,
    CDRMRightsConstraints*& aRightsConstraint )
    {

    __ASSERT_DEBUG( !aRightsConstraint,
                    User::Panic( KWmDrmUtilitiesDebugPanicMessage,
                                 KWmDrmUtilitiesDebugPanicCode ) );

    TChar ch;
    TTime startTime( Time::NullTTime() );
    TTime endTime( Time::NullTTime() );
    TTimeIntervalSeconds duration( 0 );
    TUint32 counter( 0 );

    TPtrC16 token( NULL, 0 );

    // Parse the WM DRM rights string
    TLex lex( aRights.Description() );

    // First, find the license type format string ("licensetype=")
    ParseString( '=', lex, token );

    if ( token.CompareF( KLicenseType ) != 0 )
        {
        // License type format string was not found
        User::Leave( KErrArgument );
        }

    // Peek for the end of string (eos) in case of (licensetype="")
    ch = lex.Peek();
    if ( ch == 0 )
        {
        return;
        }

    // Check the license type
    ParseString( ';', lex, token );

    aRightsConstraint = CDRMRightsConstraints::NewL();
    CleanupStack::PushL( aRightsConstraint );

    // Check what kind of rights are in question by parsing the string
    // onward. The possible rights are date time (start time and/or end time),
    // count, duration, time count (count and/or date time) and unlimited
    // rights. The substrings for certain rights are further parsed in
    // specific methods.
    if ( token.CompareF( KTime ) == 0 )
        {
        ParseWmDrmTimeRightsL( lex, startTime, endTime );
        if ( Time::NullTTime() != startTime )
            {
            aRightsConstraint->SetStartTime( startTime );
            }
        if ( Time::NullTTime() != endTime )
            {
            aRightsConstraint->SetEndTime( endTime );
            }
        }
    else if ( token.CompareF( KCount ) == 0 )
        {
        if ( ParseWmDrmCountRightsL( lex, counter ) )
            {
            aRightsConstraint->SetCounters( counter, counter );
            }
        }
    else if ( token.CompareF( KDuration ) == 0 )
        {
        if ( ParseWmDrmDurationRightsL( lex, duration ) )
            {
            aRightsConstraint->SetInterval( duration );
            }
        }
    else if ( token.CompareF( KTimeCount ) == 0 )
        {
        counter = 0;
        if ( ParseWmDrmCountRightsL( lex, counter ) )
            {
            aRightsConstraint->SetCounters( counter, counter );
            }
        ParseWmDrmTimeRightsL( lex, startTime, endTime );
        if ( Time::NullTTime() != startTime )
            {
            aRightsConstraint->SetStartTime( startTime );
            }
        if ( Time::NullTTime() != endTime )
            {
            aRightsConstraint->SetEndTime( endTime );
            }
        }
    else if ( token.CompareF( KUnlimited ) != 0 )
        {
        // Unknown license type
        User::Leave( KErrArgument );
        }

    CleanupStack::Pop( aRightsConstraint );

    }

// -----------------------------------------------------------------------------
// DrmUtilityWmDrmUtilities::CheckWmDrmRightsL
// -----------------------------------------------------------------------------
//
EXPORT_C void DrmUtilityWmDrmUtilities::CheckWmDrmRightsL( TBool& aUnconstrained,
    TTimeIntervalSeconds& aTime,
    TInt& aCounts,
    ContentAccess::CRightsInfo& aRights )
    {

    TChar ch;
    TPtrC16 token( NULL, 0 );
    TInt constraints( 0 );
    TInt error( KErrNone );
    TTime startTime( Time::NullTTime() );
    TTime endTime( Time::NullTTime() );
    TUint32 counter( 0 );
    TTime now( Time::NullTTime() ); // current time
    // End time of rights when duration is taken into account.
    TTime durationEndTime( Time::NullTTime() );
    TTimeIntervalSeconds secondsLeft( 0 ); // seconds to end of time based rights

    // Parse WM DRM rights string
    TLex lex( aRights.Description() );

    // First, find the license type format string ("licensetype=")
    ParseString( '=', lex, token );

    if ( token.CompareF( KLicenseType ) != 0 )
        {
        // License type format string was not found
        User::Leave( KErrArgument );
        }

    // Peek for the end of string (Eos) in case of (licensetype="")
    ch = lex.Peek();
    if ( ch == 0 )
        {
        return;
        }

    // Check the license type
    ParseString( ';', lex, token );

    // Check what kind of rights are in question by parsing the string
    // onward. The possible rights are date time (start time and/or end time),
    // count, duration, time count (count and/or date time) and unlimited
    // rights. The substrings for certain rights are further parsed in
    // specific methods.
    if ( token.CompareF( KTime ) == 0 )
        {
        ParseWmDrmTimeRightsL( lex, startTime, endTime );
        if ( Time::NullTTime() != startTime )
            {
            constraints |= KWmDrmConstraintStartTime;
            }
        if ( Time::NullTTime() != endTime )
            {
            constraints |= KWmDrmConstraintEndTime;
            }
        }
    else if ( token.CompareF( KCount ) == 0 )
        {
        if ( ParseWmDrmCountRightsL( lex, counter ) )
            {
            aCounts = counter;
            constraints |= KWmDrmConstraintCount;
            }
        }
    else if ( token.CompareF( KDuration ) == 0 )
        {
        if ( ParseWmDrmDurationRightsL( lex, aTime ) )
            {
            if ( aTime < (TTimeIntervalSeconds)0 )
                {
                aTime = 0;
                }
            constraints |= KWmDrmConstraintDuration;
            }
        }
    else if ( token.CompareF( KTimeCount ) == 0 )
        {
        if ( ParseWmDrmCountRightsL( lex, counter ) )
            {
            aCounts = counter;
            constraints |= KWmDrmConstraintCount;
            }
        ParseWmDrmTimeRightsL( lex, startTime, endTime );
        if ( Time::NullTTime() != startTime )
            {
            constraints |= KWmDrmConstraintStartTime;
            }
        if ( Time::NullTTime() != endTime )
            {
            constraints |= KWmDrmConstraintEndTime;
            }
        }
    else if ( token.CompareF( KUnlimited ) == 0 )
        {
        aUnconstrained = ETrue;
        return;
        }
    else
        {
        // Unknown license type
        User::Leave( KErrArgument );
        }

    // Get current time
    now.HomeTime();

    // The rights are not constrained
    if ( !constraints )
        {
        aUnconstrained = ETrue;
        }

    if ( constraints & KWmDrmConstraintStartTime )
        {
        // The start time is in the past or the current time
        if ( ( now >= startTime ) &&
            !( constraints & KWmDrmConstraintEndTime ) &&
            !( constraints & KWmDrmConstraintCount ) )
            {
            aUnconstrained = ETrue;
            }

        // This use case is unclear and should be specified later.
        // There are counts for the content, but the start time has
        // not been reached.
        if ( ( constraints & KWmDrmConstraintCount ) &&
            ( now < startTime ) )
            {
            aCounts = 0;
            }
        }

    // Find out the amount of time that the rights have left in
    // case the rights have end time constraint.
    if ( constraints & KWmDrmConstraintEndTime )
        {
        error = endTime.SecondsFrom( now, secondsLeft );

        // The difference between current time and the end time does not
        // fit to 32-bit number, the start time has been reached and there
        // are no count constraints defined.
        if ( ( error == KErrOverflow ) &&
            ( !( constraints & KWmDrmConstraintStartTime ) ||
            ( now >= startTime ) ) && !( constraints & KWmDrmConstraintCount ) )
            {
            aUnconstrained = ETrue;
            }

        // End time has not been reached and start time has been reached
        if ( ( secondsLeft > (TTimeIntervalSeconds)0 ) && ( now >= startTime ) )
            {
            aTime = secondsLeft;
            }

        // This use case is unclear and should be specified later.
        // There are counts for the content, but the end time has
        // been reached.
        if ( ( constraints & KWmDrmConstraintCount ) &&
            ( now > endTime ) )
            {
            aCounts = 0;
            }
        }

    return;

    }

// End of File
