/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DRM Utility common functionality
*
*/


// INCLUDE FILES
#include <DrmPermission.h>
#include <DrmConstraint.h>

#include "drmutilitycommon.h"
#include "DRMClockClient.h"
#include "DRMPointerArray.h"

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDrmUtilityCommon::CDrmUtilityCommon
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
DRM::CDrmUtilityCommon::CDrmUtilityCommon()
    {
    }

// -----------------------------------------------------------------------------
// CDrmUtilityCommon::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityCommon::ConstructL()
    {
    User::LeaveIfError( iOmaClient.Connect() );
    }


// -----------------------------------------------------------------------------
// CDrmUtilityCommon::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmUtilityCommon* DRM::CDrmUtilityCommon::NewLC()
    {
    DRM::CDrmUtilityCommon* self( new( ELeave ) CDrmUtilityCommon() );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// -----------------------------------------------------------------------------
// CDrmUtilityCommon::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CDrmUtilityCommon* DRM::CDrmUtilityCommon::NewL()
    {
    DRM::CDrmUtilityCommon* self( NewLC() );

    CleanupStack::Pop( self );

    return self;
    }

// Destructor
DRM::CDrmUtilityCommon::~CDrmUtilityCommon()
    {
    iOmaClient.Close();
    }

// -----------------------------------------------------------------------------
// CDrmUtilityCommon::GetOmaRightsStatusL
//
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::TDrmRightsInfo DRM::CDrmUtilityCommon::GetOmaRightsStatusL(
    HBufC8*& aContentUri,
    ContentAccess::TIntent aIntent,
    CDRMConstraint* aConstraint )
    {
    CDRMPointerArray<CDRMPermission>* uriList( NULL );
    CDRMPointerArray<HBufC8>* individuals( NULL );
    RDRMClockClient clockClient;
    DRMClock::ESecurityLevel secLevel( DRMClock::KInsecure );
    TTime drmTime( Time::NullTTime() );
    TBool possiblefuture( EFalse );
    CDRMConstraint* constraint( NULL );
    CDRMConstraint* toplevel( NULL );
    TUint32 retval( 0 );
    TInt r( KErrNone );

    r = clockClient.Connect();
    CleanupClosePushL( clockClient );
    if ( !r )
        {
        TInt timeZone( 0 );
        clockClient.GetSecureTime( drmTime, timeZone, secLevel );
        }

    uriList = CDRMPointerArray<CDRMPermission>::NewLC();
    uriList->SetAutoCleanup( ETrue );
    TRAP_IGNORE( iOmaClient.GetDBEntriesL( *aContentUri, *uriList ) );

    if ( !uriList->Count() )
        {
        CleanupStack::PopAndDestroy( 2, &clockClient ); //clockClient, uriList
        return DRM::EURightsInfoMissing;
        }

    individuals = CDRMPointerArray<HBufC8>::NewLC();
    individuals->SetAutoCleanup( ETrue );
    TRAP_IGNORE( iOmaClient.GetSupportedIndividualsL( *individuals ) );

    // Now we have the time, rights and the individual constraints do the
    // checking. The rights are never valid if we get here so we don't have
    // to check for that
    for ( TInt i( 0 ); i < uriList->Count() && !possiblefuture ; i++ )
        {
        toplevel = NULL;
        constraint = NULL;

        // If the toplevel constraint is defined, get it:
        toplevel = (*uriList)[i]->TopLevelConstraint();
        // If constraint for the intent is defined, get it
        constraint = (*uriList)[i]->ConstraintForIntent( aIntent );

        // Top level constraint and constraint for intent, merge them
        if ( toplevel && constraint )
            {
            constraint->Merge( *toplevel );
            }
        // Only constraint for intent, use it
        else if ( constraint )
            {
            }
        // Only top level constraint or no constraints at all, continue
        else
            {
            continue;
            }

        // If the constraint is rejected due to non time reasons or there is no
        // time it can't be future
        constraint->Valid( drmTime, *individuals, retval );
        if ( retval & EConstraintCounter ||
             retval & EConstraintAccumulated ||
             retval & EConstraintTimedCounter ||
             retval & EConstraintIndividual ||
             retval & EConstraintNullDrmTime )
            {
            continue;
            }

        drmTime.HomeTime();

        // If the constrain has active start time and it is not valid,
        // it must be future
        if ( constraint->iActiveConstraints & EConstraintStartTime )
            {
            possiblefuture = ETrue;
            if ( aConstraint )
                {
                aConstraint->DuplicateL( *constraint );
                }
            }

        // If the constrain has active interval and it´s start time is in
        // future, it must be future
        else if ( constraint->iActiveConstraints & EConstraintInterval &&
                  constraint->iIntervalStart > drmTime )
            {
            possiblefuture = ETrue;
            if ( aConstraint )
                {
                aConstraint->DuplicateL( *constraint );
                }
            }
        } // End of for loop

    CleanupStack::PopAndDestroy( 3, &clockClient ); //individuals, urilist,
                                                    //clockClient
    return possiblefuture ? DRM::EURightsInfoFuture : DRM::EURightsInfoExpired;
    }

//  End of File
