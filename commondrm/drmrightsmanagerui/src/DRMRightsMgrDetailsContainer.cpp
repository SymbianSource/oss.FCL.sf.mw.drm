/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CDRMRightsMgrDetailsContainer class
*
*/


// INCLUDE FILES
#include <StringLoader.h>
#include <aknlists.h>
#include <CDirectoryLocalizer.h>
#include <DRMRights.h>

#include <DRMRightsManager.rsg>

#include "DRMRightsMgrDetailsContainer.h"
#include "DRMCommonUtilities.h"
#include "drmutilityinternaltypes.h"

// CONSTANTS
LOCAL_C const TInt KMsgLineLength = 100;


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CDRMRightsMgrDetailsContainer::CDRMRightsMgrDetailsContainer
// -----------------------------------------------------------------------------
//
CDRMRightsMgrDetailsContainer::CDRMRightsMgrDetailsContainer(
                                                        TDetails& aDetails,
                                                        TBool aStartEmbedded )
: iDetails( aDetails ),
  iStartEmbedded( aStartEmbedded ),
  iListBox( NULL )
    {
    }


// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::ConstructL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::ConstructL( const TRect& aRect )
    {
    iListBox = new( ELeave )CAknDoubleStyleListBox;     // Creates new object

    CreateWindowL(); //makes the control a window-owning control

    ConstructListBoxL();

    if ( iListBox )
        {
        // set iListBox to be contained in this container
        iListBox->SetContainerWindowL( *this );
        SetScrollBarL();
        }

    SetRect( aRect );
    ActivateL();

    SetIconsL();
    }

// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::~CDRMRightsMgrDetailsContainer
// ---------------------------------------------------------
//
CDRMRightsMgrDetailsContainer::~CDRMRightsMgrDetailsContainer()
    {
    delete iListBox;
    }

// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::SizeChanged
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::SizeChanged()
   {
    if ( iListBox )
        {
        iListBox->SetRect( Rect() ); // Sets rectangle of listbox.
        }
    }

// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::CountComponentControls
// ---------------------------------------------------------
//
TInt CDRMRightsMgrDetailsContainer::CountComponentControls() const
    {
    TInt count( 0 );

    if ( iListBox )
        {
        count++;
        }

    return count; // Returns number of controls inside this container.
    }

// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::ComponentControl
// ---------------------------------------------------------
//
CCoeControl* CDRMRightsMgrDetailsContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iListBox;    // Returns the pointer to listbox object.
        default:
            return NULL;        // Exception : Returns NULL.
        }
    }

// ---------------------------------------------------------------------------
// CDRMRightsMgrDetailsContainer::HandleResourceChange
// ---------------------------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    }

// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::SetScrollBarL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::SetScrollBarL()
    {
    // Update scrollbar in order to draw correct scroll bar.
    TInt pos( iListBox->View()->CurrentItemIndex() );
    if ( iListBox->ScrollBarFrame() )
        {
        iListBox->ScrollBarFrame()->MoveVertThumbTo( pos );
        }

    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
                                                CEikScrollBarFrame::EOff,
                                                CEikScrollBarFrame::EAuto );
    }


// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CDRMRightsMgrDetailsContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                TEventCode aType )
    {
    if ( aType != EEventKey ) // Is not key event?
        {
        return EKeyWasNotConsumed;
        }
    else if ( aKeyEvent.iCode == EKeyLeftArrow ||
              aKeyEvent.iCode == EKeyRightArrow )
        {
        return EKeyWasConsumed;
        }
    else
        {
        return iListBox ? iListBox->OfferKeyEventL( aKeyEvent, aType ) :
                          EKeyWasNotConsumed;
        }
    }

// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::Draw
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::Draw( const TRect& /*aRect*/ ) const
    {
    }

// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::HandleControlEventL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::HandleControlEventL( CCoeControl* /*aControl*/,
                                             TCoeEvent /*aEventType*/ )
    {
    }

// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::SetIconsL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::SetIconsL()
    {
    }

// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::FocusChanged
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::FocusChanged(TDrawNow /*aDrawNow*/)
   {
    if ( iListBox )
        {
        iListBox->SetFocus( IsFocused() );
        }
    }

// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::ConstructListBoxL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::ConstructListBoxL()
    {
    iListBox->ConstructL( this, EAknListBoxViewerFlags /* | 
        EAknListBoxItemSpecificMenuDisabled */ );

    if ( iStartEmbedded )
        {
        iListBox->View()->SetListEmptyTextL( KNullDesC );
        }
    else
        {
        UpdateListBoxContentL( iDetails );        // Updates content
        }
    }

// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::UpdateListBoxContentL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::UpdateListBoxContentL( TDetails& aDetails )
    {
    CDesCArray* items = ( CDesCArray* )( iListBox->Model()->ItemTextArray() );
    items->Reset();

    TInt rightsCount = aDetails.iRights->Count();

    if ( rightsCount )
        {
        // Add only the details of the currently active RO, although the file
        // could be related to multiple ROs.
        AddRightsDetailsL( aDetails.iActiveROindex, aDetails, items );
        }
    else
        {
        // WMDRM agent returned an empty array of rights. Add the status line
        // of rights.
        if ( aDetails.iDrmScheme == EDrmSchemeWmDrm )
            {
            AddStatusLineL( aDetails.iRights->At( 0 ).iStatus, items );
            }
        }

    // Content sending
    if ( aDetails.iDrmScheme != EDrmSchemeUnknownDrm )
        {
        AddContentSendingLineL( aDetails.iContentCanBeSent, items );
        }

    // Individual constraint
    if ( aDetails.iIndividualConstraint )
        {
        AddContentUseLineL( aDetails.iUsageAllowed, items );
        }

    iListBox->HandleItemAdditionL();
    }


// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::HandleListBoxEventL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::HandleListBoxEventL(
                    CEikListBox* /*aListBox*/, TListBoxEvent /*aEventType*/ )
    {
    }


// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::AddRightsDetailsL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::AddRightsDetailsL( TInt aNumberOfKey,
                                                       TDetails& aDetails,
                                                       CDesCArray* aItems )
    {

    if ( aNumberOfKey < aDetails.iRights->Count() && aNumberOfKey >= 0 )
        {
        // Status of Rights
        AddStatusLineL( aDetails.iRights->At( aNumberOfKey ).iStatus,
            aItems );

        // Play Rights
        AddRightsLineL( aDetails.iRights->At( aNumberOfKey ).iPlayConstraint,
            aItems,
            aDetails.iDrmScheme,
            ContentAccess::EPlay );

        // Display Rights
        AddRightsLineL( aDetails.iRights->At( aNumberOfKey ).iDisplayConstraint,
            aItems,
            aDetails.iDrmScheme,
            ContentAccess::EView );

        // Print Rights
        AddRightsLineL( aDetails.iRights->At( aNumberOfKey ).iPrintConstraint,
            aItems,
            aDetails.iDrmScheme,
            ContentAccess::EPrint );

        // Execute Rights
        AddRightsLineL( aDetails.iRights->At( aNumberOfKey ).iExecuteConstraint,
            aItems,
            aDetails.iDrmScheme,
            ContentAccess::EExecute );

        }
    }

// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::AddStatusLineL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::AddStatusLineL( TDRMUIRightsStatus aStatus,
                                                    CDesCArray* aItems )
    {
    TBuf<KMsgLineLength> msgLine;
    HBufC* stringHolder;


    stringHolder = StringLoader::LoadL( R_QTN_DRM_DETAILS_STATUS,
                                            iEikonEnv );

    msgLine.Zero();
    msgLine.Append( '\t' );
    msgLine.Append( *stringHolder );

    delete stringHolder;
    stringHolder = NULL;

    msgLine.Append( '\t' );

    TInt resourceId;
    switch( aStatus )
        {
        case EDRMUIStatusInvalid:
            resourceId = R_QTN_DRM_DETAILS_NO_LICENCE_AVAILABLE;
            break;

        case EDRMUIStatusValid:
            resourceId = R_QTN_DRM_DETAILS_VALID;
            break;

        case EDRMUIStatusFuture:
            resourceId = R_QTN_DRM_DETAILS_NOT_VALID_YET;
            break;

        case EDRMUIStatusExpired:
            resourceId = R_QTN_DRM_DETAILS_NO_LICENCE_AVAILABLE;
            break;

        default:
            resourceId = 0;
            break;
        }

    if ( resourceId )
        {
        stringHolder = StringLoader::LoadL( resourceId, iEikonEnv );
        msgLine.Append( *stringHolder );

        delete stringHolder;
        stringHolder = NULL;
        }

    if ( aItems )
        {
        aItems->AppendL( msgLine );
        }
    }

// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::AddContentSendingLineL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::AddContentSendingLineL(
                                                    TBool aContentCanBeSent,
                                                    CDesCArray* aItems )
    {
    TBuf<KMsgLineLength> msgLine;

    msgLine.Zero();
    msgLine.Append( '\t' );
    HBufC* stringHolder = StringLoader::LoadL( R_QTN_DRM_DETAILS_SENDING,
                                               iEikonEnv );
    msgLine.Append( *stringHolder );

    delete stringHolder;
    stringHolder = NULL;

    msgLine.Append( '\t' );
    stringHolder = StringLoader::LoadL( aContentCanBeSent ?
                                        R_QTN_DRM_MGR_DET_ALLOWED :
                                        R_QTN_DRM_MGR_DET_FORBID, iEikonEnv );
    msgLine.Append( *stringHolder );

    delete stringHolder;
    stringHolder = NULL;

    if ( aItems )
        {
        aItems->AppendL( msgLine );
        }
    }

// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::AddContentUseLineL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::AddContentUseLineL( TBool aUsageAllowed,
                                                        CDesCArray* aItems )
    {
    TBuf<KMsgLineLength> msgLine;

    msgLine.Zero();
    msgLine.Append( '\t' );
    HBufC* stringHolder = StringLoader::LoadL( R_QTN_DRM_DET_CONTENT_USE,
                                               iEikonEnv );
    msgLine.Append( *stringHolder );

    delete stringHolder;
    stringHolder = NULL;

    msgLine.Append( '\t' );
    stringHolder = StringLoader::LoadL( aUsageAllowed ?
                                        R_QTN_DRM_DET_SIM_ALLOWED :
                                        R_QTN_DRM_DET_SIM_NOT_ALLOWED, iEikonEnv );
    msgLine.Append( *stringHolder );

    delete stringHolder;
    stringHolder = NULL;

    if ( aItems )
        {
        aItems->AppendL( msgLine );
        }
    }

// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::AddRightsLineL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::AddRightsLineL(
                                            CDRMRightsConstraints* aConstraint,
                                            CDesCArray* aItems,
                                            const TInt aDrmScheme,
                                            const TUint32& aIntent )
    {
    if ( !aConstraint )
        {
        return;
        }
    else if ( aConstraint->FullRights() )
        {
        // Full Rights
        AddUsageRightsL( aItems, aIntent );
        }
    else
        {
        TUint32 counter( 0 );
        TUint32 originalCounter( 0 );
        TTime startDateTime( 0 );
        TTime endDateTime( 0 );
        TTimeIntervalSeconds interval( 0 );

        if ( aConstraint->GetCounters( counter, originalCounter ) == KErrNone )
            {
            // RO has a count constraint
            AddUsageTimesLeftL( counter, aItems, aIntent );
            }

        if ( aDrmScheme == EDrmSchemeOmaDrm )
            {
            // For OMA DRM only
            CDRMConstraint& drmConstraint = aConstraint->GetConstraint();
            if ( drmConstraint.iActiveConstraints & EConstraintTimedCounter )
                {
                // RO has Timed Counter constraint
                AddUsageTimesLeftL( drmConstraint.iTimedCounter, aItems,
                                    aIntent );
                }
            }

        if ( aConstraint->GetInterval( interval ) == KErrNone )
            {

            // RO has an interval constraint
            // WM DRM duration rights are mapped to the interval constraint
            // (which is not activated).
            TTime intervalStart( 0 );

            // If the file has an interval constraint which is activated,
            // do not add information about the interval to the details view.
            if ( aConstraint->GetIntervalStart( intervalStart ) != KErrNone )
                {
                // File has an interval constraint which is not activated.
                AddUsageTimeLeftL( interval, aItems, EFalse, aIntent );
                }
            }

        if ( aConstraint->GetStartTime( startDateTime ) == KErrNone )
            {
            // RO has a start time constraint
            AddRightsValidFromL( startDateTime, aItems, aIntent );
            }

        if ( aConstraint->GetEndTime( endDateTime ) == KErrNone )
            {
            // RO has an expiring time constraint
            AddRightsValidUntilL( endDateTime, aItems, aIntent );
            }

        if ( aDrmScheme == EDrmSchemeOmaDrm )
            {
            // For OMA DRM only
            CDRMConstraint& drmConstraint = aConstraint->GetConstraint();

            if ( drmConstraint.iActiveConstraints & EConstraintAccumulated )
                {
                TTimeIntervalSeconds accInterval(
                    drmConstraint.iAccumulatedTime );

                // RO has Accumulated Time constraint
                AddUsageTimeLeftL( accInterval, aItems, ETrue, aIntent );
                }
            }

        }

    }

// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::AddUsageRightsL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::AddUsageRightsL( CDesCArray* aItems,
                                                     const TUint32& aIntent)
    {
    TBuf<KMaxFileName*2> msgLine;
    msgLine.Zero();
    msgLine.Append( '\t' );

    // Usage Rights
    // Check the intent and process the corresponding string.
    TInt resourceId( 0 );
    TInt resourceIdUnlimited( 0 );
    switch ( aIntent )
        {
        case ContentAccess::EPlay:
            resourceId = R_QTN_DRM_DETAILS_PLAYS_LEFT;
            resourceIdUnlimited = R_QTN_DRM_MGR_DET_PL_UNLIMITED;
            break;

        case ContentAccess::EView:
            resourceId = R_QTN_DRM_DETAILS_VIEWS_LEFT;
            resourceIdUnlimited = R_QTN_DRM_MGR_DET_VI_UNLIMITED;
            break;

        case ContentAccess::EPrint:
            resourceId = R_QTN_DRM_DETAILS_PRINTS_LEFT;
            resourceIdUnlimited = R_QTN_DRM_MGR_DET_PR_UNLIMITED;
            break;

        case ContentAccess::EExecute:
            resourceId = R_QTN_DRM_DETAILS_EXEC_TIMES;
            resourceIdUnlimited = R_QTN_DRM_MGR_DET_EX_UNLIMITED;
            break;

        default:
            User::Leave( KErrArgument );
            break;
        }

    HBufC* stringHolder = StringLoader::LoadL( resourceId, iEikonEnv );
    msgLine.Append( *stringHolder );

    delete stringHolder;
    stringHolder = NULL;

    msgLine.Append( '\t' );

    // Unlimited
    stringHolder = StringLoader::LoadL( resourceIdUnlimited, iEikonEnv );
    msgLine.Append( *stringHolder );

    delete stringHolder;
    stringHolder = NULL;

    if ( aItems )
        {
        aItems->AppendL( msgLine );
        }
    }


// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::AddUsageTimesLeftL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::AddUsageTimesLeftL( TUint32 aCounter,
                                                CDesCArray* aItems,
                                                const TUint32& aIntent )
    {
    TBuf<KMaxFileName*2> msgLine;
    msgLine.Zero();
    msgLine.Append( '\t' );

    // Usage times left
    // Check the intent and process the corresponding string.
    TInt resourceId( 0 );
    switch ( aIntent )
        {
        case ContentAccess::EPlay:
            resourceId = R_QTN_DRM_DETAILS_PLAYS_LEFT;
            break;

        case ContentAccess::EView:
            resourceId = R_QTN_DRM_DETAILS_VIEWS_LEFT;
            break;

        case ContentAccess::EPrint:
            resourceId = R_QTN_DRM_DETAILS_PRINTS_LEFT;
            break;

        case ContentAccess::EExecute:
            resourceId = R_QTN_DRM_DETAILS_EXEC_TIMES;
            break;

        default:
            User::Leave( KErrArgument );
            break;
        }

    HBufC* stringHolder = StringLoader::LoadL( resourceId, iEikonEnv );

    msgLine.Append( *stringHolder );

    delete stringHolder;
    stringHolder = NULL;

    msgLine.Append( '\t' );

    if ( aCounter == 1 )
        {
        stringHolder = StringLoader::LoadL( R_QTN_DRM_DETAILS_1_TIME,
                                            iEikonEnv );
        }
    else
        {
        stringHolder = StringLoader::LoadL( R_QTN_DRM_DETAILS_N_TIMES,
                                            aCounter, iEikonEnv );
        }

    msgLine.Append( *stringHolder );

    delete stringHolder;
    stringHolder = NULL;

    if ( aItems )
        {
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( msgLine );
        aItems->AppendL( msgLine );
        }
    }

// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::AddUsageTimeLeftL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::AddUsageTimeLeftL(
                                        const TTimeIntervalSeconds& aInterval,
                                        CDesCArray* aItems,
                                        const TBool aIsAccumulatedTime,
                                        const TUint32& aIntent )
    {
    TBuf<KMaxFileName*2> msgLine;
    msgLine.Zero();
    msgLine.Append( '\t' );

    HBufC* stringHolder;

    if ( aIsAccumulatedTime )
        {
        // Accumulated time constraint
        stringHolder = StringLoader::LoadL( R_QTN_DRM_DETAILS_PLAYING_TIME_LEFT,
                                            iEikonEnv );
        }
    else
        {
        // Interval constraint which has not been activated
        TInt resourceId( 0 );
        switch ( aIntent )
            {
            case ContentAccess::EPlay:
                resourceId = R_QTN_DRM_DETAILS_VALIDITY_PERIOD;
                break;

            case ContentAccess::EView:
                resourceId = R_QTN_DRM_DETAILS_VIEWING_RIGHTS;
                break;

            case ContentAccess::EPrint:
                resourceId = R_QTN_DRM_DETAILS_PRINTING_RIGHTS;
                break;

            case ContentAccess::EExecute:
                resourceId = R_QTN_DRM_DETAILS_VALIDITY_PERIOD;
                break;

            default:
                User::Leave( KErrArgument );
                break;
            }

        stringHolder = StringLoader::LoadL( resourceId, iEikonEnv );
        }

    msgLine.Append( *stringHolder );

    delete stringHolder;
    stringHolder = NULL;

    msgLine.Append( '\t' );

    TInt intYrs, intMon, intDay, intHrs, intMin, intSec;
    SplitTime( aInterval, intYrs, intMon, intDay, intHrs, intMin, intSec );

    stringHolder = AddPartsOfTimeL( intYrs, intMon, intDay,
                                    intHrs, intMin, intSec );

    msgLine.Append( *stringHolder );

    delete stringHolder;
    stringHolder = NULL;

    if ( aItems )
        {
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( msgLine );
        aItems->AppendL( msgLine );
        }
    }


// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::SplitTime
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::SplitTime(
                                const TTimeIntervalSeconds& aInterval,
                                TInt& aIntYrs, TInt& aIntMon, TInt& aIntDay,
                                TInt& aIntHrs, TInt& aIntMin, TInt& aIntSec )
    {
    TInt temp;
    TInt i;

    const TInt daysInYear = 365;
    const TInt secsInMin = 60;
    const TInt secsInHour = secsInMin*60;
    const TInt secsInDay = secsInHour*24;

    const TInt maxDaysInMonths[12] = {  30,  60,  90, 120, 150, 180,
                                       210, 240, 270, 300, 330, 360 };

    // calculate full days
    temp = aInterval.Int() / secsInDay;

    // calculate full years
    aIntYrs = temp / daysInYear;

    // calc remainder days
    temp = temp % daysInYear;

    aIntMon = 0;

    i = 0;
    if ( temp >= maxDaysInMonths[0] )
        {
        for ( i = 0; i < 11; i++ )
            {
            // found correct amount of months
            if ( temp >= maxDaysInMonths[i] && temp < maxDaysInMonths[i+1] )
                {
                // i now contains amount of full months (+1 because of table index)
                aIntMon = i+1;
                break;
                }
            }
        }

    // calc remainder days = allSecs - secsInFullYears - secsInFullMonts
    if( temp >= maxDaysInMonths[i] )
        {
        aIntDay = temp - maxDaysInMonths[i];
        }
    else
        {
        aIntDay = temp;
        }

    // calculate remainder secs
    temp = aInterval.Int() % secsInDay;

    aIntHrs = temp / secsInHour;

    // calculate remainder secs
    temp = temp % secsInHour;

    aIntMin = temp / secsInMin;

    // calculate remainder secs
    aIntSec = temp % secsInMin;
    }


// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::AddPartsOfTimeL
// ---------------------------------------------------------
//
HBufC* CDRMRightsMgrDetailsContainer::AddPartsOfTimeL(
                                    TInt aIntYrs, TInt aIntMon, TInt aIntDay,
                                    TInt aIntHrs, TInt aIntMin, TInt aIntSec )
    {
    // Only the two most meaningful data will be showed
    TInt numOfData = 0;

    CDesCArrayFlat* strings = new ( ELeave ) CDesCArrayFlat( 2 );
    CleanupStack::PushL( strings );

    if ( aIntYrs > 0 )
        {
        AddSinglePartOfTimeL( aIntYrs,
                              R_QTN_DRM_NBR_OF_YEARS_ONE,
                              R_QTN_DRM_NBR_OF_YEARS_ONE_FINAL,
                              R_QTN_DRM_NBR_OF_YEARS_TWO_FOUR,
                              R_QTN_DRM_NBR_OF_YEARS_FIVE_ZERO,
                              strings );
        numOfData++;
        }

    if ( aIntMon > 0 )
        {
        //  Second type not provided because 11 is the maximum
        AddSinglePartOfTimeL( aIntMon,
                              R_QTN_DRM_NBR_OF_MONTHS_ONE,
                              0,
                              R_QTN_DRM_NBR_OF_MONTHS_TWO_FOUR,
                              R_QTN_DRM_NBR_OF_MONTHS_FIVE_ZERO,
                              strings );
        numOfData++;
        }

    // Only if years or months were missing
    if ( aIntDay > 0 && numOfData < 2 )
        {
        AddSinglePartOfTimeL( aIntDay,
                              R_QTN_DRM_NBR_OF_DAYS_ONE,
                              R_QTN_DRM_NBR_OF_DAYS_ONE_FINAL,
                              R_QTN_DRM_NBR_OF_DAYS_TWO_FOUR,
                              R_QTN_DRM_NBR_OF_DAYS_FIVE_ZERO,
                              strings );
        numOfData++;
        }

    if ( aIntHrs > 0 && numOfData < 2 )
        {
        AddSinglePartOfTimeL( aIntHrs,
                              R_QTN_DRM_NBR_OF_HOURS_ONE,
                              R_QTN_DRM_NBR_OF_HOURS_ONE_FINAL,
                              R_QTN_DRM_NBR_OF_HOURS_TWO_FOUR,
                              R_QTN_DRM_NBR_OF_HOURS_FIVE_ZERO,
                              strings );
        numOfData++;
        }

    if ( aIntMin > 0 && numOfData < 2 )
        {
        AddSinglePartOfTimeL( aIntMin,
                              R_QTN_DRM_NBR_OF_MINS_ONE,
                              R_QTN_DRM_NBR_OF_MINS_ONE_FINAL,
                              R_QTN_DRM_NBR_OF_MINS_TWO_FOUR,
                              R_QTN_DRM_NBR_OF_MINS_FIVE_ZERO,
                              strings );
        numOfData++;
        }

    // If interval is 0, then it shows "0 seconds" anyway
    if ( ( aIntSec > 0 && numOfData < 2 ) || numOfData == 0 )
        {
        AddSinglePartOfTimeL( aIntSec,
                              R_QTN_DRM_NBR_OF_SECS_ONE,
                              R_QTN_DRM_NBR_OF_SECS_ONE_FINAL,
                              R_QTN_DRM_NBR_OF_SECS_TWO_FOUR,
                              R_QTN_DRM_NBR_OF_SECS_FIVE_ZERO,
                              strings );
        numOfData++;
        }

    HBufC* stringHolder;
    if ( numOfData == 1 )
        {
        stringHolder = StringLoader::LoadL( R_QTN_DRM_MGR_DET_INTER,
                                            strings->MdcaPoint(0), iEikonEnv );
        }
    else
        {
        stringHolder = StringLoader::LoadL( R_QTN_DRM_MGR_DET_INTER_TWO,
                                            *strings, iEikonEnv );
        }

    CleanupStack::PopAndDestroy( strings );

    return stringHolder;
    }


// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::AddSinglePartOfTimeL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::AddSinglePartOfTimeL( TInt aNumOfElements,
                                                    TInt aResourceIdSingle,
                                                    TInt aResourceIdOneFinal,
                                                    TInt aResourceIdTwoFour,
                                                    TInt aResourceIdFiveZero,
                                                    CDesCArrayFlat* aStrings )
    {
    HBufC* stringHolder;
    TInt finalOneDigit = aNumOfElements % 10;
    TInt finalTwoDigits = aNumOfElements % 100;

    if ( aNumOfElements == 1 )
        {
        stringHolder = StringLoader::LoadLC( aResourceIdSingle, iEikonEnv );
        }
    else if ( finalOneDigit == 1 && finalTwoDigits != 11 )
        {
        stringHolder = StringLoader::LoadLC( aResourceIdOneFinal,
                                             aNumOfElements,
                                             iEikonEnv );
        }
    else if ( finalOneDigit == 0 ||
              ( finalOneDigit >= 5 && finalOneDigit <= 9 ) ||
              ( finalTwoDigits >= 11 && finalTwoDigits <= 14 ) )
        {
        stringHolder = StringLoader::LoadLC( aResourceIdFiveZero,
                                             aNumOfElements,
                                             iEikonEnv );
        }
    else
        {
        stringHolder = StringLoader::LoadLC( aResourceIdTwoFour,
                                             aNumOfElements,
                                             iEikonEnv );
        }

    if ( aStrings )
        {
        aStrings->AppendL( *stringHolder );
        }

    CleanupStack::PopAndDestroy ( stringHolder );
    }


// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::AddRightsValidFromL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::AddRightsValidFromL(
                                                const TTime& aStartDateTime,
                                                CDesCArray* aItems,
                                                const TUint32& aIntent )
    {
    TBuf<KMaxFileName*2> msgLine;
    msgLine.Zero();
    msgLine.Append( '\t' );

    TInt resourceId( 0 );
    switch ( aIntent )
        {
        case ContentAccess::EPlay:
            resourceId = R_QTN_DRM_DETAILS_VALID_FROM;
            break;

        case ContentAccess::EView:
            resourceId = R_QTN_DRM_DETAILS_VIEW_RIGHTS_FROM;
            break;

        case ContentAccess::EPrint:
            resourceId = R_QTN_DRM_DETAILS_PRINT_RIGHTS_FROM;
            break;

        case ContentAccess::EExecute:
            resourceId = R_QTN_DRM_DETAILS_VALID_FROM;
            break;

        default:
            User::Leave( KErrArgument );
            break;
        }

    HBufC* stringHolder = StringLoader::LoadL( resourceId, iEikonEnv );
    msgLine.Append( *stringHolder );

    delete stringHolder;
    stringHolder = NULL;

    msgLine.Append( '\t' );

    if ( aStartDateTime != Time::NullTTime() )
        {
        TBuf<32> stringDate;
        TBuf<32> stringTime;

        HBufC* stringFormatDate = StringLoader::LoadLC(
                                                    R_QTN_DATE_USUAL_WITH_ZERO,
                                                    iEikonEnv );
        HBufC* stringFormatTime = StringLoader::LoadLC( R_QTN_TIME_USUAL,
                                                       iEikonEnv );

        aStartDateTime.FormatL( stringDate, *stringFormatDate );
        aStartDateTime.FormatL( stringTime, *stringFormatTime );
        CleanupStack::PopAndDestroy( stringFormatTime );
        CleanupStack::PopAndDestroy( stringFormatDate );

        msgLine.Append( stringDate );
        msgLine.Append( ' ' );
        msgLine.Append( stringTime );
        }

    if ( aItems )
        {
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( msgLine );
        aItems->AppendL( msgLine );
        }
    }


// ---------------------------------------------------------
// CDRMRightsMgrDetailsContainer::AddRightsValidUntilL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsContainer::AddRightsValidUntilL(
                                                const TTime& aEndDateTime,
                                                CDesCArray* aItems,
                                                const TUint32& aIntent )
    {
    TBuf<KMaxFileName*2> msgLine;
    msgLine.Zero();
    msgLine.Append( '\t' );

    TInt resourceId( 0 );
    switch ( aIntent )
        {
        case ContentAccess::EPlay:
            resourceId = R_QTN_DRM_DETAILS_VALID_UNTIL;
            break;

        case ContentAccess::EView:
            resourceId = R_QTN_DRM_DETAILS_VIEW_RIGHTS_UNTIL;
            break;

        case ContentAccess::EPrint:
            resourceId = R_QTN_DRM_DETAILS_PRINT_RIGHTS_UNTIL;
            break;

        case ContentAccess::EExecute:
            resourceId = R_QTN_DRM_DETAILS_VALID_UNTIL;
            break;

        default:
            User::Leave( KErrArgument );
            break;
        }

    HBufC* stringHolder = StringLoader::LoadL( resourceId, iEikonEnv );
    msgLine.Append( *stringHolder );

    delete stringHolder;
    stringHolder = NULL;

    msgLine.Append( '\t' );

    if ( aEndDateTime != Time::NullTTime() )
        {
        TBuf<32> stringDate;
        TBuf<32> stringTime;

        HBufC* stringFormatDate = StringLoader::LoadLC(
                                                    R_QTN_DATE_USUAL_WITH_ZERO,
                                                    iEikonEnv );
        HBufC* stringFormatTime = StringLoader::LoadLC( R_QTN_TIME_USUAL,
                                                        iEikonEnv );

        aEndDateTime.FormatL( stringDate, *stringFormatDate );
        aEndDateTime.FormatL( stringTime, *stringFormatTime );
        CleanupStack::PopAndDestroy( stringFormatTime );
        CleanupStack::PopAndDestroy( stringFormatDate );

        msgLine.Append( stringDate );
        msgLine.Append( ' ' );
        msgLine.Append( stringTime );
        }

    if ( aItems )
        {
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( msgLine );
        aItems->AppendL( msgLine );
        }
    }

// End of File
