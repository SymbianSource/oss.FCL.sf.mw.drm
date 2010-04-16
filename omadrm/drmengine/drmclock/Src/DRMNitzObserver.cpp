/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the DRM Nitz Observer
*
*/



// INCLUDE FILES
#include "DRMNitzObserver.h"
#include "DRMClock.h"
#include "drmlog.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KTimeZoneValidBit(0x3F);     // Time zone difference
const TInt KTimeZoneSignBit(0x80);      // Sign of time zone difference
const TInt KNitzYearOffset(2000);       // Adjusting the year value received
const TInt KMaximumTwoDigitValue(99);
// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CDRMNitzObserver::CDRMNitzObserver
// Default Constructor
// ---------------------------------------------------------
//
CDRMNitzObserver::CDRMNitzObserver( RMobilePhone& aMobilePhone, CDRMClock* aDRMClock) :
    CActive(EPriorityNormal) ,iDRMClock( aDRMClock ), iMobilePhone( &aMobilePhone ),
    iError( KErrNone )
    {
    }

// ---------------------------------------------------------
// CDRMNitzObserver::~CDRMNitzObserver
// Destructor
// ---------------------------------------------------------
//
CDRMNitzObserver::~CDRMNitzObserver()
    {
    Cancel();
    };


// ---------------------------------------------------------
// CDRMNitzObserver::ConstructL
// Two-phase Constructor
// ---------------------------------------------------------
//
void CDRMNitzObserver::ConstructL()
    {
    DRMLOG( _L("ConstructL "));

    // Get the NITZ info on the creation, might not be available yet
    iError = iMobilePhone->GetNITZInfo(iNitzInfo);


    DRMLOG2( _L("ConstructL: Error = %d"), iError );

    }


// ---------------------------------------------------------
// CDRMNitzObserver::NewL
// Two-phase constructor
// ---------------------------------------------------------
//
CDRMNitzObserver* CDRMNitzObserver::NewL( RMobilePhone& aMobilePhone,
                                         CDRMClock* aDRMClock)
    {
    CDRMNitzObserver* self = new(ELeave) CDRMNitzObserver(aMobilePhone,aDRMClock);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CDRMNitzObserver::Start
// Adds the active object into the ActiveScheduler
// and sets the object active, makes sure RunL is called
// atleast once
// ---------------------------------------------------------
//
void CDRMNitzObserver::Start()
    {
    TRequestStatus* status = 0;

    DRMLOG2( _L("iError == %d "), iError );

    if( iError == KErrNotSupported )
        {
        // NITZ never supported, kill the observer
        if( IsAdded() )
            {
            Deque();
            return;
            }
        return;
        }

    if( !IsAdded() )
        {
        CActiveScheduler::Add(this);
        }
    if ( !IsActive() )
        {
        SetActive();
        }
    status = &iStatus;
    User::RequestComplete(status,KErrNone);
    }


// Ignore the error:
TInt CDRMNitzObserver::RunError( TInt /*aError*/ )
    {
    SetActive();
    return KErrNone;
    }


// ---------------------------------------------------------
// CDRMNitzObserver::RunL
// Active object RunL
// ---------------------------------------------------------
//
void CDRMNitzObserver::RunL()
    {
    TTime timeData;
    TInt timeZone( 0 );

    // Check if the nitz has changed and is available, if so call update
    // otherwise just wait

    DRMLOG( _L("ObserverRunL Called\n\r ") );

    iStatus = KRequestPending;

    iMobilePhone->NotifyNITZInfoChange(iStatus, iNitzInfo);

    DRMLOG( _L("NotifyNITZInfoChange:"));
    if( GetNitzTime( timeData, timeZone ) )
        {
        DRMLOG( _L("Resetting secure time from NITZ observer:"));
        TRAPD(error, iDRMClock->ResetSecureTimeL( timeData, timeZone ) );
        if( error )
            {
            DRMLOG( _L("Resetting secure time from NITZ observer returned an error."));
            }
        DRMLOG( _L("Resetting secure time from NITZ observer successful."));

        if( error != KErrArgument )
            {
            if( error )
                {
                DRMLOG( _L("ObserverRunL Called error \n\r ") );
                }
            User::LeaveIfError( error );
            }
        }

    SetActive();
    };


// ---------------------------------------------------------
// CDRMNitzObserver::DoCancel
// Cancels the active object
// ---------------------------------------------------------
//
void CDRMNitzObserver::DoCancel()
    {
    // cancel the notify change
    iMobilePhone->CancelAsyncRequest(EMobilePhoneNotifyNITZInfoChange);
    };


// ---------------------------------------------------------
// CDRMNitzObserver::CheckDateTimeVal(const TDateTime& aDateTime)
// Check the date time value is valid.
// ---------------------------------------------------------
//
TInt CDRMNitzObserver::CheckDateTimeVal(const TDateTime& aDateTime)
    {
    TDateTime tmp;
    return tmp.Set(aDateTime.Year(),
                   aDateTime.Month(),
                   aDateTime.Day(),
                   aDateTime.Hour(),
                   aDateTime.Minute(),
                   aDateTime.Second(),
                   aDateTime.MicroSecond());
    }

// ---------------------------------------------------------
// CDRMNitzObserver::GetNitzTime(const TDateTime& aDateTime)
// Gets the nitz time from iNitzInfo
// ---------------------------------------------------------
//
TBool CDRMNitzObserver::GetNitzTime(TTime& aNitzTime, TInt& aTimeZone)
    {
    TInt32 nitzCaps(iNitzInfo.iNitzFieldsUsed);

    if (nitzCaps & RMobilePhone::KCapsTimezoneAvailable)
        {
        TInt timezone(iNitzInfo.iTimeZone & KTimeZoneValidBit);

        if (iNitzInfo.iTimeZone & KTimeZoneSignBit)
            {
            // Changes sign
            timezone = - timezone;
            }

        // TimeZone info
        aTimeZone = timezone;
        }

    if (nitzCaps & RMobilePhone::KCapsTimeAvailable)
        {
        TDateTime dateTime;
        TInt fourDigitYear(iNitzInfo.Year());

        //Check if our TSY returns 2 digits in stead of 4 digits for the year value
        if (fourDigitYear <= KMaximumTwoDigitValue)        // The maximum year value in UI applications is 2060
            {
           //The year received from TSY is 2 digits we make it 4 digits
            fourDigitYear = fourDigitYear + KNitzYearOffset;  // 2000
            }

        dateTime.Set(fourDigitYear,
                    TMonth(iNitzInfo.Month()),
                    iNitzInfo.Day(),
                    iNitzInfo.Hour(),
                    iNitzInfo.Minute(),
                    iNitzInfo.Second(),
                    iNitzInfo.MicroSecond());

        DRMLOG2( _L( "CDRMNitzObserver::GetNitzTime: DateTime: %d" ), fourDigitYear );
        DRMLOG2( _L( ":%d" ), iNitzInfo.Month() );
        DRMLOG2( _L( ":%d" ), iNitzInfo.Day() );
        DRMLOG2( _L( ":%d" ), iNitzInfo.Hour() );
        DRMLOG2( _L( ":%d" ), iNitzInfo.Minute() );
        DRMLOG2( _L( ":%d" ), iNitzInfo.Second() );

        if (KErrNone != CheckDateTimeVal(dateTime))
            {
            return EFalse;
            }
        // Transfer the time into a TTime object, UTC
        aNitzTime = dateTime;
        return ETrue;
        }

    return EFalse;

    };
