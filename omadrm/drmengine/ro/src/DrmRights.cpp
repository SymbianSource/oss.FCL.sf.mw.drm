/*
* Copyright (c) 2002 - 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of common DRM operations
*
*/


// INCLUDE FILES

#include "DrmAsset.h"
#include "DRMRights.h"
#include "DrmPermission.h"
#include "DrmConstraint.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

_LIT(KNullDate,"00000000:000000.000000");

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::CDRMRightsConstraints
// Default constructor
// -----------------------------------------------------------------------------
EXPORT_C CDRMRightsConstraints::CDRMRightsConstraints(void)
    {
    iConstraint = NULL;
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
EXPORT_C void CDRMRightsConstraints::ConstructL(void)
    {
    iConstraint = CDRMConstraint::NewL();
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CDRMRightsConstraints* CDRMRightsConstraints::NewL(void)
    {
    CDRMRightsConstraints* self = new (ELeave) CDRMRightsConstraints;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
EXPORT_C CDRMRightsConstraints::~CDRMRightsConstraints(void)
    {
    if (iConstraint) delete iConstraint;
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::FullRights
// The FullRights function checks if the object contains any restrictions
// or if usage is unconstrained.
// -----------------------------------------------------------------------------
EXPORT_C TBool CDRMRightsConstraints::FullRights(void)
    {
    return iConstraint->iActiveConstraints == EConstraintNone ? ETrue : EFalse;
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::IsPreview
// The function checks if the object represents a preview constraint.
// -----------------------------------------------------------------------------
EXPORT_C TBool CDRMRightsConstraints::IsPreview(void)
    {
    if ((iConstraint->iActiveConstraints & EConstraintCounter) &&
        iConstraint->iOriginalCounter == 1)
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::GetConstraintInfo
// This function checks the expiration and the constraints of the object and
// returns the prioryty value.
// -----------------------------------------------------------------------------
EXPORT_C void CDRMRightsConstraints::SetConstraint(CDRMConstraint& aConstraint)
    {
    TInt error = KErrNone;
    if( !iConstraint )
        {
        TRAP( error, iConstraint = CDRMConstraint::NewL());
        if( error )
            {
            return;
            }
        }
    TRAP( error, iConstraint->DuplicateL( aConstraint ) );
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::GetConstraintInfo
// This function checks the expiration and the constraints of the object and
// returns the prioryty value.
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRightsConstraints::GetConstraintInfo(
    TUint32& aExpiration,
    TUint32& aConstType)
    {
    TTime currentTime;
    TTime time;
    TTimeIntervalSeconds interval;
    TTimeIntervalSeconds diff;
    TInt result = KErrNone;
    currentTime.HomeTime();
    TUint32 counter = 0;
    TUint32 originalCounter = 0;
    TInt priorValue = 0;
    aConstType = NULL;

    aExpiration = CDRMRights::EValidRights;

    if ( FullRights() )
        {
        aConstType = CDRMRights::ENoConstraints;
        return ENoConstraints * aExpiration;
        }

    result = GetEndTime(time);
    if (result == KErrNone)
        {
        aConstType |= CDRMRights::ETimeBased;
        if ( time < currentTime )
            {
            aExpiration = CDRMRights::EExpiredRights;
            }
        }

    result = GetStartTime(time);
    if (result == KErrNone && aExpiration != CDRMRights::EExpiredRights)
        {
        aConstType |= CDRMRights::ETimeBased;
        if ( time > currentTime )
            {
            aExpiration = CDRMRights::EFutureRights;
            }
        }

    result = GetInterval(interval);
    if(result == KErrNone && aExpiration != CDRMRights::EExpiredRights)
        {
        result = GetIntervalStart(time);
        if(result == KErrNone)
            {
            aConstType |= CDRMRights::ETimeBased;
            currentTime.SecondsFrom(time, diff);
            if (diff.Int() < 0)
                {
                aExpiration = CDRMRights::EFutureRights;
                }
            else if ( interval < diff )
                {
                aExpiration = CDRMRights::EExpiredRights;
                }
            }
        else
            {
            aConstType |= CDRMRights::EInActiveInterval;
            }
        }
    result = GetCounters( counter , originalCounter );
    if (result == KErrNone)
        {
        aConstType |= CDRMRights::ECountBased;
        if ( counter == 0 )
            {
            aExpiration = CDRMRights::EExpiredRights;
            }
        }
    result = GetTimedCounters( counter , originalCounter );
    if (result == KErrNone)
        {
        aConstType |= CDRMRights::ECountBased;
        if ( counter == 0 )
            {
            aExpiration = CDRMRights::EExpiredRights;
            }
        }
    result = GetAccumulated(interval);
    if(result == KErrNone && aExpiration != CDRMRights::EExpiredRights)
        {
        aConstType |= CDRMRights::ETimeBased | CDRMRights::ETimeIsAccumulatedTime;
        if (interval.Int() == 0)
            {
            aExpiration = CDRMRights::EExpiredRights;
            }
        }
    if (!aConstType)
        {
        return CDRMRights::EInvalidRights;
        }

    if ( (aConstType & CDRMRights::ECountBased) &&
        !(aConstType & CDRMRights::EInActiveInterval) &&
        !(aConstType & CDRMRights::ETimeBased) )
        {
        priorValue = ECount * aExpiration;
        }
    else if ( (aConstType & CDRMRights::ECountBased) &&
        (aConstType & CDRMRights::EInActiveInterval) &&
        !(aConstType & CDRMRights::ETimeBased) )
        {
        priorValue = EIntervalCount * aExpiration;
        }
    else if ( (aConstType & CDRMRights::ECountBased) &&
        (aConstType & CDRMRights::EInActiveInterval) &&
        (aConstType & CDRMRights::ETimeBased) )
        {
        priorValue = ETimeIntervalCount * aExpiration;
        }
    else if ( (aConstType & CDRMRights::ECountBased) &&
        !(aConstType & CDRMRights::EInActiveInterval) &&
        (aConstType & CDRMRights::ETimeBased) )
        {
        priorValue = ETimeCount * aExpiration;
        }
    else if ( !(aConstType & CDRMRights::ECountBased) &&
        (aConstType & CDRMRights::EInActiveInterval) &&
        !(aConstType & CDRMRights::ETimeBased) )
        {
        priorValue = EInterval * aExpiration;
        }
    else if ( !(aConstType & CDRMRights::ECountBased) &&
        (aConstType & CDRMRights::EInActiveInterval) &&
        (aConstType & CDRMRights::ETimeBased) )
        {
        priorValue = ETimeInterval * aExpiration;
        }
    else if ( !(aConstType & CDRMRights::ECountBased) &&
        !(aConstType & CDRMRights::EInActiveInterval) &&
        (aConstType & CDRMRights::ETimeBased) )
        {
        priorValue = ETime * aExpiration;
        }

    return priorValue;
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::GetExpirationDetails
// The function returns the end time and amount of counts left of the object.
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRightsConstraints::GetExpirationDetails(
    TTime& aEndTime,
    TInt& aCountsLeft)
    {
    TTime endTime;
    endTime.Set(KNullDate);
    TTimeIntervalSeconds interval;
    TTime intervalStart;
    intervalStart.Set(KNullDate);
    TTime intervalEnd;
    intervalEnd.Set(KNullDate);

    TUint32 counter = 0;
    TUint32 originalCounter = 0;

    TBool isEndTime = EFalse;
    TBool isInterval = EFalse;
    TBool isCount = EFalse;

    if (GetCounters(counter, originalCounter) == KErrNone)
        {
        isCount = ETrue;
        aCountsLeft = counter;
        }
    else
        {
        aCountsLeft = -1;
        }
    if (GetEndTime(endTime) == KErrNone)
        {
        isEndTime = ETrue;
        }
    if (GetIntervalStart(intervalStart) == KErrNone)
        {
        isInterval = ETrue;
        GetInterval(interval);
        intervalEnd = intervalStart + interval;
        }
    if (isInterval && isEndTime)
        {
        if (intervalEnd > endTime)
            {
            aEndTime = endTime;
            }
        else
            {
            aEndTime = intervalEnd;
            }
        }
    else if (isInterval)
        {
        aEndTime = intervalEnd;
        }
    else if (isEndTime)
        {
        aEndTime = endTime;
        }
    else if (!isCount)
        {
        // The object contains either a full or an inactive interval rights
        aEndTime.Set(KNullDate);
        aCountsLeft = -1;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::GetCounters
// This function returns the current value of the count restriction and
// original value of the count restriction.
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRightsConstraints::GetCounters(
    TUint32& aCounter,
    TUint32& aOriginalCounter)
    {
    if (iConstraint->iActiveConstraints & EConstraintCounter)
        {
        aCounter = iConstraint->iCounter;
        aOriginalCounter = iConstraint->iOriginalCounter;
        return KErrNone;
        }
    else
        {
        return CDRMRights::ENoRights;
        }
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::GetCounters
// This function returns the current value of the count restriction and
// original value of the count restriction.
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRightsConstraints::GetTimedCounters(
    TUint32& aCounter,
    TUint32& aOriginalCounter)
    {
    if (iConstraint->iActiveConstraints & EConstraintTimedCounter)
        {
        aCounter = iConstraint->iTimedCounter;
        aOriginalCounter = iConstraint->iOriginalCounter;
        return KErrNone;
        }
    else
        {
        return CDRMRights::ENoRights;
        }
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::GetStartTime
//
// This function returns the time when the constraint becomes valid or has
// become valid.
//
// Cases:
// 1. If an active interval is present, that time is always the correct start
//    time
// 2. If start time is defined that is the start time
// 3. No active interval or start time defined CDRMRights::ENoRights
//
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRightsConstraints::GetStartTime(
    TTime& aStartTime)
    {
    // Activated interval is always the same or later than
    // the start time of the start-end

    if (iConstraint->iActiveConstraints & EConstraintInterval &&
        (iConstraint->iIntervalStart != Time::NullTTime() ) )
        {
        aStartTime = iConstraint->iIntervalStart;
        }
    else if (iConstraint->iActiveConstraints & EConstraintStartTime)
        {
        aStartTime = iConstraint->iStartTime;
        }
    else
        {
        return CDRMRights::ENoRights;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::GetEndTime
//
// This function returns the time when the constraint will become invalid
//
// Cases:
// 1. If both end time and activated interval is present the one which is closer
//    to the present time will be chosen
// 2. If only end time is available, that is returned
// 3. If only activated interval is available, that is returned
// 4. Otherwise CDRMRights::ENoRights
//
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRightsConstraints::GetEndTime(
    TTime& aEndTime)
    {
    // If both end time and interval have been activated
    // take the one which is closer

    if ((iConstraint->iActiveConstraints & EConstraintEndTime) &&
        (iConstraint->iActiveConstraints & EConstraintInterval) &&
        (iConstraint->iIntervalStart != Time::NullTTime() ) )
        {
        if( iConstraint->iEndTime <= (iConstraint->iIntervalStart +
                                      iConstraint->iInterval) )
            {
            aEndTime = iConstraint->iEndTime;
            }
        else
            {
            aEndTime = (iConstraint->iIntervalStart +
                        iConstraint->iInterval);
            }
        }
    else if (iConstraint->iActiveConstraints & EConstraintEndTime)
        {
        aEndTime = iConstraint->iEndTime;
        }
    else if (iConstraint->iActiveConstraints & EConstraintInterval &&
             (iConstraint->iIntervalStart != Time::NullTTime() ) )
        {
        aEndTime = (iConstraint->iIntervalStart +
                    iConstraint->iInterval);
        }
    else
        {
        return CDRMRights::ENoRights;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::GetInterval
// This function returns the interval time constraint
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRightsConstraints::GetInterval(
    TTimeIntervalSeconds& aInterval)
    {
    if (iConstraint->iActiveConstraints & EConstraintInterval)
        {
        aInterval = iConstraint->iInterval;
        return KErrNone;
        }
    else
        {
        return CDRMRights::ENoRights;
        }
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::GetIntervalStart
// This function returns the start of the interval time constraint
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRightsConstraints::GetIntervalStart(
    TTime& aIntervalStart)
    {
    if (iConstraint->iActiveConstraints & EConstraintInterval &&
        iConstraint->iIntervalStart != Time::NullTTime())
        {
        aIntervalStart = iConstraint->iIntervalStart;
        return KErrNone;
        }
    else
        {
        return CDRMRights::ENoRights;
        }
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::GetAccumulated
// This function returns the accumulated time constraint
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRightsConstraints::GetAccumulated(
    TTimeIntervalSeconds& aAccumulated)
    {
    if (iConstraint->iActiveConstraints & EConstraintAccumulated)
        {
        aAccumulated = iConstraint->iAccumulatedTime;
        return KErrNone;
        }
    else
        {
        return CDRMRights::ENoRights;
        }
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::GetCounters
// This function returns the current value of the count restriction and
// original value of the count restriction.
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRightsConstraints::SetCounters(
    TUint32 aCounter,
    TUint32 aOriginalCounter)
    {
    iConstraint->iCounter = aCounter;
    iConstraint->iOriginalCounter = aOriginalCounter;
    iConstraint->iActiveConstraints |= EConstraintCounter;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::SetStartTime
// This function sets the start time constraint
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRightsConstraints::SetStartTime(
    TTime aStartTime)
    {
    iConstraint->iStartTime = aStartTime;
    iConstraint->iActiveConstraints |= EConstraintStartTime;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::SetEndTime
// This function sets the end time constraint
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRightsConstraints::SetEndTime(
    TTime aEndTime)
    {
    iConstraint->iEndTime = aEndTime;
    iConstraint->iActiveConstraints |= EConstraintEndTime;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::SetInterval
// This function sets the interval time constraint
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRightsConstraints::SetInterval(
    TTimeIntervalSeconds aInterval)
    {
    iConstraint->iInterval = aInterval;
    iConstraint->iActiveConstraints |= EConstraintInterval;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::SetIntervalStart
// This function sets the start of the interval time constraint
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRightsConstraints::SetIntervalStart(
    TTime aStartTime)
    {
    iConstraint->iIntervalStart = aStartTime;
    iConstraint->iActiveConstraints |= EConstraintInterval;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::GetConstraint
// -----------------------------------------------------------------------------
EXPORT_C CDRMConstraint& CDRMRightsConstraints::GetConstraint(void)
    {
    return *iConstraint;
    }

// -----------------------------------------------------------------------------
// CDRMRightsConstraints::operator=
// Assignment operator.
// -----------------------------------------------------------------------------
EXPORT_C CDRMRightsConstraints& CDRMRightsConstraints::operator=(
    CDRMRightsConstraints& aConstraint)
    {
    if (iConstraint)
        {
        delete iConstraint;
        iConstraint = NULL;
        }
    TRAP_IGNORE( iConstraint = CDRMConstraint::NewL() );
    if (iConstraint)
        {
        TRAP_IGNORE( iConstraint->DuplicateL( aConstraint.GetConstraint() ) );
        }

    return *this;
    }

// -----------------------------------------------------------------------------
// CDRMRights::CDRMRights
// Default constructor.
// -----------------------------------------------------------------------------
EXPORT_C CDRMRights::CDRMRights(void):
    iAsset(NULL),
    iPermission(NULL)
    {
    }

// -----------------------------------------------------------------------------
// CDRMRights::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
EXPORT_C void CDRMRights::ConstructL(void)
    {
    iPermission = CDRMPermission::NewL();
    iAsset = CDRMAsset::NewL();
    }

// -----------------------------------------------------------------------------
// CDRMRights::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CDRMRights* CDRMRights::NewL(void)
    {
    CDRMRights* self = new (ELeave) CDRMRights;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
EXPORT_C CDRMRights::~CDRMRights(void)
    {
    delete iAsset;
    delete iPermission;
    }

// -----------------------------------------------------------------------------
// CDRMRights::GetContentURI
// This function returns the content URI of the object
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRights::GetContentURI(HBufC8*& aContentURI)
    {
    TInt r;

    r = KErrNone;
    if (iAsset && iAsset->iUid)
        {
        aContentURI = HBufC8::NewMax(iAsset->iUid->Size());
        if (aContentURI)
            {
            *aContentURI = *iAsset->iUid;
            }
        else r = KErrNoMemory;
        }
    else
        {
        aContentURI = NULL;
        }

    return r;
    }

// -----------------------------------------------------------------------------
// CDRMRights::GetLocalID
// This function returns the local ID of the object
// -----------------------------------------------------------------------------
EXPORT_C TUint32 CDRMRights::GetLocalID(void)
    {
    return iPermission->iUniqueID;
    }

// -----------------------------------------------------------------------------
// CDRMRights::GetAddTime
// This function returns the time when the rights object was added to the
// rights database.
// -----------------------------------------------------------------------------
EXPORT_C void CDRMRights::GetAddTime(TTime& aTime)
    {
    aTime = iPermission->iOriginalInsertTime;
    }

// -----------------------------------------------------------------------------
// CDRMRights::GetRightsInfo
// This function checks the expiration and the constraints of the object and
// retunrs the best constraint priority value.
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRights::GetRightsInfo(
    TUint32 aConstraintSpec,
    TRestriction& aRestriction,
    TExpiration& aExpiration,
    TUint32& aConstType)
    {
    TInt r = 0;
    TBool noRights = ETrue;

    TUint32 playConst = NULL;
    TUint32 displayConst = NULL;
    TUint32 exeConst = NULL;
    TUint32 printConst = NULL;

    TUint32 playExpir = EExpiredRights;
    TUint32 displayExpir = EExpiredRights;
    TUint32 printExpir = EExpiredRights;
    TUint32 exeExpir = EExpiredRights;

    TInt playValue = NULL;
    TInt displayValue = NULL;
    TInt exeValue = NULL;
    TInt printValue = NULL;
    TInt compareValue = NULL;

    CDRMRightsConstraints* constraint = NULL;

    r = GetPlayRight(constraint);
    if (r && r != ENoRights)
        {
        return KErrNoMemory;
        }
    if (r == KErrNone &&
       (aConstraintSpec == CDRMRights::EPlay || aConstraintSpec == 0 ) )
        {
        noRights = EFalse;
        if (constraint->FullRights())
            {
            delete constraint;
            constraint = NULL;
            aExpiration = EValidRights;
            aConstType = ENoConstraints;
            aRestriction = EFullRights;
            return EFullRights;
            }
        else
            {
            playValue = constraint->GetConstraintInfo(playExpir, playConst);
            if (playValue > compareValue)
                {
                compareValue = playValue;
                if(constraint->IsPreview())
                    {
                    aRestriction = EPreviewRights;
                    }
                else
                    {
                    aRestriction = ERestrictedRights;
                    }
                }
            }
        }

    delete constraint;
    constraint = NULL;

    r = GetDisplayRight(constraint);
    if (r && r != ENoRights)
        {
        return KErrNoMemory;
        }
    if (r == KErrNone &&
       (aConstraintSpec == CDRMRights::EDisplay || aConstraintSpec == 0 ) )
        {
        noRights = EFalse;
        if (constraint->FullRights())
            {
            delete constraint;
            constraint = NULL;
            aExpiration = EValidRights;
            aConstType = ENoConstraints;
            aRestriction = EFullRights;
            return EFullRights;
            }
        else
            {
            displayValue = constraint->GetConstraintInfo(displayExpir, displayConst);
            if (displayValue > compareValue)
                {
                compareValue = displayValue;
                if(constraint->IsPreview())
                    {
                    aRestriction = EPreviewRights;
                    }
                else
                    {
                    aRestriction = ERestrictedRights;
                    }
                }
            }
        }

    delete constraint;
    constraint = NULL;

    r = GetExecuteRight(constraint);
    if (r && r != ENoRights)
        {
        return KErrNoMemory;
        }
    if (r == KErrNone &&
       (aConstraintSpec == CDRMRights::EExecute || aConstraintSpec == 0 ) )
        {
        noRights = EFalse;
        if (constraint->FullRights())
            {
            delete constraint;
            constraint = NULL;
            aExpiration = EValidRights;
            aConstType = ENoConstraints;
            aRestriction = EFullRights;
            return EFullRights;
            }
        else
            {
            exeValue = constraint->GetConstraintInfo(exeExpir, exeConst);
            if (exeValue > compareValue)
                {
                compareValue = exeValue;
                if(constraint->IsPreview())
                    {
                    aRestriction = EPreviewRights;
                    }
                else
                    {
                    aRestriction = ERestrictedRights;
                    }
                }
            }
        }

    delete constraint;
    constraint = NULL;

    r = GetPrintRight(constraint);
    if (r && r != ENoRights)
        {
        return KErrNoMemory;
        }
    if (r == KErrNone &&
       (aConstraintSpec == CDRMRights::EPrint || aConstraintSpec == 0 ) )
        {
        noRights = EFalse;
        if (constraint->FullRights())
            {
            delete constraint;
            constraint = NULL;
            aExpiration = EValidRights;
            aConstType = ENoConstraints;
            aRestriction = EFullRights;
            return EFullRights;
            }
        else
            {
            printValue = constraint->GetConstraintInfo(printExpir, printConst);
            if (printValue > compareValue)
                {
                compareValue = exeValue;
                if(constraint->IsPreview())
                    {
                    aRestriction = EPreviewRights;
                    }
                else
                    {
                    aRestriction = ERestrictedRights;
                    }
                }
            }
        }

    delete constraint;
    constraint = NULL;

    if (noRights)
        {
        delete constraint;
        constraint = NULL;
        aExpiration = EExpiredRights;
        return ENoRights;
        }

    if (aRestriction != ERestrictedRights)
        {
        aRestriction = EPreviewRights;
        }

    if (playExpir == EExpiredRights &&
        displayExpir == EExpiredRights &&
        printExpir == EExpiredRights &&
        exeExpir == EExpiredRights)
        {
        aExpiration = EExpiredRights;
        }

    if (playExpir == EFutureRights ||
        displayExpir == EFutureRights ||
        printExpir == EFutureRights ||
        exeExpir == EFutureRights)
        {
        aExpiration = EFutureRights;
        }

    if (playExpir == EValidRights ||
        displayExpir == EValidRights ||
        printExpir == EValidRights ||
        exeExpir == EValidRights)
        {
        aExpiration = EValidRights;
        }

    if (compareValue == playValue) aConstType = playConst;
    if (compareValue == displayValue) aConstType = displayConst;
    if (compareValue == exeValue) aConstType = exeConst;
    if (compareValue == printValue) aConstType = printConst;

    return compareValue;
    }

// -----------------------------------------------------------------------------
// CDRMRights::GetExpirationDetails
// The function returns the end time and amount of counts left of the object.
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRights::GetExpirationDetails(
    TUint32 aConstraintSpec,
    TTime& aEndTime,
    TInt& aCountsLeft)
    {
    TInt r = 0;
    CDRMRightsConstraints* constraint = NULL;

    TTime playEndTime;
    playEndTime.Set(KNullDate);
    TTime displayEndTime;
    displayEndTime.Set(KNullDate);
    TTime executeEndTime;
    executeEndTime.Set(KNullDate);
    TTime printEndTime;
    printEndTime.Set(KNullDate);
    TTime compareTime;
    compareTime.Set(KNullDate);

    TInt playCounter = 0;
    TInt displayCounter = 0;
    TInt executeCounter = 0;
    TInt printCounter = 0;

    TBool noRights = ETrue;

    aEndTime.Set(KNullDate);
    aCountsLeft  = 0;

    r = GetPlayRight(constraint);
    if (r == KErrNone &&
       (aConstraintSpec == CDRMRights::EPlay || aConstraintSpec == 0 ) )
        {
        noRights = EFalse;
        if (constraint->FullRights())
            {
            delete constraint;
            constraint = NULL;
            aEndTime.Set(KNullDate);
            aCountsLeft = -1;
            return KErrNone;
            }
        else
            {
            r = constraint->GetExpirationDetails(playEndTime, playCounter);
            if(playEndTime == compareTime && playCounter == -1)
                {
                delete constraint;
                constraint = NULL;
                return KErrNone;
                }
            }
        }

    delete constraint;
    constraint = NULL;

    r = GetDisplayRight(constraint);
    if (r == KErrNone &&
       (aConstraintSpec == CDRMRights::EDisplay || aConstraintSpec == 0 ) )
        {
        noRights = EFalse;
        if (constraint->FullRights())
            {
            delete constraint;
            constraint = NULL;
            aEndTime.Set(KNullDate);
            aCountsLeft = -1;
            return KErrNone;
            }
        else
            {
            r = constraint->GetExpirationDetails(displayEndTime, displayCounter);
            if(displayEndTime == compareTime && displayCounter == -1)
                {
                delete constraint;
                constraint = NULL;
                return KErrNone;
                }
            }
        }

    delete constraint;
    constraint = NULL;

    r = GetExecuteRight(constraint);
    if (r == KErrNone &&
       (aConstraintSpec == CDRMRights::EExecute || aConstraintSpec == 0 ) )
        {
        noRights = EFalse;
        if (constraint->FullRights())
            {
            delete constraint;
            constraint = NULL;
            aEndTime.Set(KNullDate);
            aCountsLeft = -1;
            return KErrNone;
            }
        else
            {
            r = constraint->GetExpirationDetails(executeEndTime, executeCounter);
            if(executeEndTime == compareTime && executeCounter == -1)
                {
                delete constraint;
                constraint = NULL;
                return KErrNone;
                }
            }
        }

    delete constraint;
    constraint = NULL;

    r = GetPrintRight(constraint);
    if (r == KErrNone &&
       (aConstraintSpec == CDRMRights::EPrint || aConstraintSpec == 0 ) )
        {
        noRights = EFalse;
        if (constraint->FullRights())
            {
            delete constraint;
            constraint = NULL;
            aEndTime.Set(KNullDate);
            aCountsLeft = -1;
            return KErrNone;
            }
        else
            {
            r = constraint->GetExpirationDetails(printEndTime, printCounter);
            if(printEndTime == compareTime && printCounter == -1)
                {
                delete constraint;
                constraint = NULL;
                return KErrNone;
                }
            }
        }

    delete constraint;
    constraint = NULL;

    if (noRights)
        {
        aEndTime.Set(KNullDate);
        aCountsLeft = -1;
        return ENoRights;
        }

    compareTime.HomeTime();

    if (playCounter > aCountsLeft)
        {
        aEndTime = playEndTime;
        aCountsLeft = playCounter;
        }
    if (displayCounter > aCountsLeft)
        {
        aEndTime = displayEndTime;
        aCountsLeft = displayCounter;
        }
    if (executeCounter > aCountsLeft)
        {
        aEndTime = executeEndTime;
        aCountsLeft = executeCounter;
        }
    if (printCounter > aCountsLeft)
        {
        aEndTime = printEndTime;
        aCountsLeft = printCounter;
        }

    if (aCountsLeft > 0 &&
        playEndTime < compareTime && displayEndTime < compareTime &&
        executeEndTime < compareTime && printEndTime < compareTime)
        {
        return KErrNone;
        }

    if (playEndTime > aEndTime)
        {
        aEndTime = playEndTime;
        aCountsLeft  = playCounter;
        }
    if (displayEndTime > aEndTime)
        {
        aEndTime = displayEndTime;
        aCountsLeft  = displayCounter;
        }
    if (executeEndTime > aEndTime)
        {
        aEndTime = executeEndTime;
        aCountsLeft  = executeCounter;
        }
    if (printEndTime > aEndTime)
        {
        aEndTime = printEndTime;
        aCountsLeft  = printCounter;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMRights::GetPlayRight
// This function returns the constraints associated with the play right.
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRights::GetPlayRight(
    CDRMRightsConstraints*& aConstraint)
    {
    TInt r = ENoRights;

    aConstraint = NULL;
    if (iPermission->iAvailableRights & ERightsPlay)
        {
        TRAP(r, aConstraint = CDRMRightsConstraints::NewL());
        if (r == KErrNone)
            {
            aConstraint->SetConstraint(*iPermission->iPlay);
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// CDRMRights::GetDisplayRight
// This function returns the constraints associated with the display right.
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRights::GetDisplayRight(
    CDRMRightsConstraints*& aConstraint)
    {
    TInt r = ENoRights;

    aConstraint = NULL;
    if (iPermission->iAvailableRights & ERightsDisplay)
        {
        TRAP(r, aConstraint = CDRMRightsConstraints::NewL());
        if (r == KErrNone)
            {
            aConstraint->SetConstraint(*iPermission->iDisplay);
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// CDRMRights::GetPrintRight
// This function returns the constraints associated with the print right.
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRights::GetPrintRight(
    CDRMRightsConstraints*& aConstraint)
    {
    TInt r = ENoRights;

    aConstraint = NULL;
    if (iPermission->iAvailableRights & ERightsPrint)
        {
        TRAP(r, aConstraint = CDRMRightsConstraints::NewL());
        if (r == KErrNone)
            {
            aConstraint->SetConstraint(*iPermission->iPrint);
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// CDRMRights::GetExecuteRight
// This function returns the constraints associated with the execute right.
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRights::GetExecuteRight(
    CDRMRightsConstraints*& aConstraint)
    {
    TInt r = ENoRights;

    aConstraint = NULL;
    if (iPermission->iAvailableRights & ERightsExecute)
        {
        TRAP(r, aConstraint = CDRMRightsConstraints::NewL());
        if (r == KErrNone)
            {
            aConstraint->SetConstraint(*iPermission->iExecute);
            }
        }
    return r;
    }

// -----------------------------------------------------------------------------
// CDRMRights::SetContentURIAndLocalID
// This function sets the content URI and local ID of the object
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRights::SetContentURIAndLocalID(
    HBufC8* aContentURI,
    TUint32 aLocalID)
    {
    TInt r = KErrNone;

    if (!iAsset)
        {
        TRAP(r, iAsset = CDRMAsset::NewL());
        }

    if (iAsset)
        {
        if( iAsset->iUid )
            {
            delete iAsset->iUid;
            }
        iAsset->iUid = aContentURI;

        if ( !iPermission )
            {
            TRAP(r, iPermission = CDRMPermission::NewL());
            }

        if ( iPermission )
            {
            iPermission->iUniqueID = aLocalID;
            }
        }

    return r;
    }

// -----------------------------------------------------------------------------
// CDRMRights::SetContentURI
// This function sets the content URI of the object
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRights::SetContentURI(
    HBufC8* aContentURI)
    {
    if (iAsset)
        {
        delete iAsset->iUid;
        iAsset->iUid = aContentURI;
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMRights::SetLocalID
// This function sets the content URI and local ID of the object
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRights::SetLocalID(
    TUint32 aLocalID)
    {
    iPermission->iUniqueID = aLocalID;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMRights::GetAddTime
// This function sets the time when the rights object was added to the
// rights database.
// -----------------------------------------------------------------------------
EXPORT_C void CDRMRights::SetAddTime(
    TTime aTime)
    {
    iPermission->iOriginalInsertTime = aTime;
    }

// -----------------------------------------------------------------------------
// CDRMRights::SetPlayRight
// This function sets the constraints associated with the play right.
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRights::SetPlayRight(
    CDRMRightsConstraints* aConstraint)
    {
    TInt error = KErrNone;

    if( !iPermission )
        {
        TRAP( error, iPermission = CDRMPermission::NewL());
        if( error )
            {
            return error;
            }
        }
    else if( !iPermission->iPlay )
        {
        TRAP( error, iPermission->iPlay = CDRMConstraint::NewL() );
        if( error )
            {
            return error;
            }
        }

    TRAP( error, iPermission->iPlay->DuplicateL( aConstraint->GetConstraint() ) );
    return error;
    }

// -----------------------------------------------------------------------------
// CDRMRights::SetDisplayRight
// This function sets the constraints associated with the display right.
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRights::SetDisplayRight(
    CDRMRightsConstraints* aConstraint)
    {
    TInt error = KErrNone;

    if( !iPermission )
        {
        TRAP( error, iPermission = CDRMPermission::NewL());
        if( error )
            {
            return error;
            }
        }
    else if( !iPermission->iDisplay )
        {
        TRAP( error, iPermission->iDisplay = CDRMConstraint::NewL() );
        if( error )
            {
            return error;
            }
        }

    TRAP( error, iPermission->iDisplay->DuplicateL( aConstraint->GetConstraint() ) );
    return error;
    }

// -----------------------------------------------------------------------------
// CDRMRights::SetPrintRight
// This function sets the constraints associated with the print right.
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRights::SetPrintRight(
    CDRMRightsConstraints* aConstraint)
    {
    TInt error = KErrNone;

    if( !iPermission )
        {
        TRAP( error, iPermission = CDRMPermission::NewL());
        if( error )
            {
            return error;
            }
        }
    else if( !iPermission->iPrint )
        {
        TRAP( error, iPermission->iPrint = CDRMConstraint::NewL() );
        if( error )
            {
            return error;
            }
        }

    TRAP( error, iPermission->iPrint->DuplicateL( aConstraint->GetConstraint() ) );
    return error;
    }

// -----------------------------------------------------------------------------
// CDRMRights::SetExecuteRight
// This function sets the constraints associated with the execute right.
// -----------------------------------------------------------------------------
EXPORT_C TInt CDRMRights::SetExecuteRight(
    CDRMRightsConstraints* aConstraint)
    {
    TInt error = KErrNone;

    if( !iPermission )
        {
        TRAP( error, iPermission = CDRMPermission::NewL());
        if( error )
            {
            return error;
            }
        }
    else if( !iPermission->iExecute )
        {
        TRAP( error, iPermission->iExecute = CDRMConstraint::NewL() );
        if( error )
            {
            return error;
            }
        }

    TRAP( error, iPermission->iExecute->DuplicateL( aConstraint->GetConstraint() ) );
    return error;
    }

// -----------------------------------------------------------------------------
// CDRMRights::GetPermission
// -----------------------------------------------------------------------------
EXPORT_C CDRMPermission& CDRMRights::GetPermission(void)
    {
    return *iPermission;
    }

// -----------------------------------------------------------------------------
// CDRMRights::SetPermissionL
// -----------------------------------------------------------------------------
EXPORT_C void CDRMRights::SetPermissionL(CDRMPermission& aRO)
    {
    delete iPermission;
    iPermission = NULL;
    iPermission = CDRMPermission::NewL();
    iPermission->DuplicateL(aRO);
    }

// -----------------------------------------------------------------------------
// CDRMRights::GetAsset
// -----------------------------------------------------------------------------
EXPORT_C CDRMAsset& CDRMRights::GetAsset(void)
    {
    return *iAsset;
    }

// -----------------------------------------------------------------------------
// CDRMRights::SetAssetL
// -----------------------------------------------------------------------------
EXPORT_C void CDRMRights::SetAssetL(CDRMAsset& aAsset)
    {
    delete iAsset;
    iAsset = NULL;
    iAsset = CDRMAsset::NewL();
    iAsset->DuplicateL(aAsset);
    }

// -----------------------------------------------------------------------------
// CDRMRights::operator=
// Assignment operator.
// -----------------------------------------------------------------------------
EXPORT_C CDRMRights& CDRMRights::operator=(
    CDRMRights& aRight)
    {
    TInt r = KErrNone;

    if (this == &aRight)
        {
        return *this;
        }

    TRAP(r, SetAssetL(aRight.GetAsset()));
    TRAP(r, SetPermissionL(aRight.GetPermission()));

    return* this;
    }


// -----------------------------------------------------------------------------
// CDRMRights::ExternalizeL
// -----------------------------------------------------------------------------
EXPORT_C void CDRMRights::ExternalizeL(RWriteStream& aStream) const
    {
    iAsset->ExternalizeL(aStream);
    iPermission->ExternalizeL(aStream);
    }

// -----------------------------------------------------------------------------
// CDRMRights::InternalizeL
// -----------------------------------------------------------------------------
EXPORT_C void CDRMRights::InternalizeL(RReadStream& aStream)
    {
    delete iAsset;
    iAsset = NULL;
    delete iPermission;
    iPermission=NULL;

    iAsset = CDRMAsset::NewL();
    iPermission = CDRMPermission::NewL();

    iAsset->InternalizeL(aStream);
    iPermission->InternalizeL(aStream);
    }

// -----------------------------------------------------------------------------
// CDRMRights::Merge
// -----------------------------------------------------------------------------
EXPORT_C void CDRMRights::Merge(const CDRMRights& aRights)
    {
    iPermission->Merge(*aRights.iPermission);
    }

// End of file
