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
* Description:  Declares container control for details view
*
*/


#ifndef DRMRIGHTSMGRDETAILSCONTAINER_H
#define DRMRIGHTSMGRDETAILSCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <eiklbo.h>
#include <eiktxlbx.h>

#include <aknnavide.h>
#include <eikspane.h>
#include <aknappui.h>

// caf
#include <caf/caf.h>
#include <caf/caftypes.h>
#include <caf/rightsinfo.h>

#include "DRMRightsMgrDetailsContainer.h"

// FORWARD DECLARATIONS
class CDRMRightsConstraints;

/**
* Status of the RO
*/
enum TDRMUIRightsStatus
    {
    EDRMUIStatusInvalid,    // RO is invalid
    EDRMUIStatusValid,      // RO is valid
    EDRMUIStatusFuture,     // RO has future rights
    EDRMUIStatusExpired     // RO expired
    };


// STRUCT DEFINITIONS

/**
* Data about a certain RightsObject
*/
struct TRights
    {
    // Status of the RO
    TDRMUIRightsStatus iStatus;

    // Play constraints
    CDRMRightsConstraints* iPlayConstraint;

    // Display constraints
    CDRMRightsConstraints* iDisplayConstraint;

    // Print constraints
    CDRMRightsConstraints* iPrintConstraint;

    // Execute constraints
    CDRMRightsConstraints* iExecuteConstraint;
    };

/**
* Data about a certain Content
*/
struct TDetails
    {
    // Array containing data about related rights
    CArrayFixFlat<TRights>* iRights;

    // The index in the array of the active RO
    TInt iActiveROindex;

    // ETrue if the object can be sent
    TBool iContentCanBeSent;

    // Location of the content
    TFileName iContentPath;

    // ETrue if Individual constraint exists
    TBool iIndividualConstraint;

    // ETrue if content can be used with current SIM
    TBool iUsageAllowed;

    // The DRM protection of the given file.
    TInt iDrmScheme;

    };


// CLASS DECLARATION

/**
*  CDRMRightsMgrDetailsContainer
*  container control class.
*/
class CDRMRightsMgrDetailsContainer : public CCoeControl,
                                      public MEikListBoxObserver,
                                      private MCoeControlObserver

    {
    public: // Constructors and destructor
        /**
        * C++ constructor.
        * @param aDetails       The details to be displayed
        * @param aStartEmbedded ETrue if it has to display details when DRMUI
        *                       was started embedded.
        */
        CDRMRightsMgrDetailsContainer( TDetails& aDetails,
                                       TBool aStartEmbedded );

        /**
        * Symbian default constructor.
        * const TRect& aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect );

        /**
        * Destructor.
        */
        ~CDRMRightsMgrDetailsContainer();

    protected: // Functions from base classes
        /**
        * Handle resource change events.
        * @param aType: The type of resources that have changed
        */
        void HandleResourceChange( TInt aType );

        /**
        * From CoeControl. Responds to size changes to sets the size and
        * position of the contents of this control.
        */
        void SizeChanged();

        /**
        * Handle key events.
        * @param aKeyEvent: key event
        * @param aType: type of event
        * @return The key response, if it was consumed or not.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

        /**
        * From CoeControl. Responds to focus changes.
        */
        void FocusChanged(TDrawNow aDrawNow);

    private: // Functions from base classes

        /**
        * From CoeControl. Gets the number of controls contained in a compound
        * control.
        * @return The number of component controls contained.
         */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl. Gets the specified component of a compound control.
        * @param  The index of the control to get
        * @return The component control with an index of aIndex
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl. Draw controls
        * @param aRect The region of the control to be redrawn.
        */
        void Draw( const TRect& aRect ) const;

        /**
        * Event handling section
        * @param aControl The control that sent the event
        * @param aEventType The event type
        */
        virtual void HandleControlEventL( CCoeControl* aControl,
                                          TCoeEvent aEventType );

     public: // New functions
        /**
        * Updates scrollbar in order to draw correct scroll bar.
        */
        void SetScrollBarL();

        /**
        * Updates content of the listbox.
        * @param aDetails   The details to be displayed
        */
        void UpdateListBoxContentL( TDetails& aDetails );


    private:    // New functions
        /**
        * Constructs listbox and initializes its sub-folders
        * @param aResLbxId resource ID of the listbox item array
        */
        void ConstructListBoxL();

        /**
        * Appending icons from resource file
        */
        virtual void SetIconsL();

        /** Handles listbox event.
        * @param aListBox   The originating list box
        * @param aEventType Type of event to be handled.
        */
        void HandleListBoxEventL( CEikListBox* aListBox,
                                  TListBoxEvent aEventType );

        /**
        * Add Rights details line(s) in listbox.
        * @param aNumberOfKey   The index of the key to be processed
        * @param aDetails       The details to be displayed
        * @param aItems         Array where the listbox items have to be added
        */
        void AddRightsDetailsL( TInt aNumberOfKey, TDetails& aDetails,
                                CDesCArray* aItems );
        /**
        * Add Status line in listbox.
        * @param aStatus        Status of the key that is processed
        * @param aItems         Array where the listbox items have to be added
        */
        void AddStatusLineL( TDRMUIRightsStatus aStatus, CDesCArray* aItems );

        /**
        * Add Content Sending line in listbox.
        * @param aContentCanBeSent ETrue if content can be sent
        * @param aItems Array where the listbox items have to be added
        */
        void AddContentSendingLineL( TBool aContentCanBeSent,
                                     CDesCArray* aItems );

        /**
        * Add Content use line in listbox.
        * @param aUsageAllowed ETrue if content can be used with this SIM
        * @param aItems        Array where the listbox items have to be added
        */
        void AddContentUseLineL( TBool aUsageAllowed,
                                 CDesCArray* aItems );

        /**
        * Add Rights line in listbox.
        * @param aResourceId Resource Id of the type of right
        * @param aConstraint Constraint to be parsed
        * @param aItems      Array where the listbox items have to be added
        * @param aIsWmDrm    Whether WM DRM or OMA DRM file is concerned
        * @param aIntent     The usage intent of the file, possible values
                             ContentAccess:EPlay, ContentAccess:EView,
                             ContentAccess:EExecute and ContentAccess:EPrint
        */
        void AddRightsLineL( CDRMRightsConstraints* aConstraint,
                               CDesCArray* aItems,
                               const TBool aIsWmDrm,
                               const TUint32& aIntent );

        /**
        * Split time in single elements
        * @param aInterval  Time interval to be split
        * @param aIntYrs    Contains in output the number of years
        * @param aIntMon    Contains in output the number of months
        * @param aIntDay    Contains in output the number of days
        * @param aIntHrs    Contains in output the number of hours
        * @param aIntMin    Contains in output the number of minutes
        * @param aIntSec    Contains in output the number of seconds
        */
        void SplitTime( const TTimeIntervalSeconds& aInterval,
                        TInt& aIntYrs, TInt& aIntMon, TInt& aIntDay,
                        TInt& aIntHrs, TInt& aIntMin, TInt& aIntSec );

        /**
        * Add elements of time to the string
        * @param aIntYrs    Number of years
        * @param aIntMon    Number of months
        * @param aIntDay    Number of days
        * @param aIntHrs    Number of hours
        * @param aIntMin    Number of minutes
        * @param aIntSec    Number of seconds
        * @return   Buffer where the parts have to be written
        */
        HBufC* AddPartsOfTimeL( TInt aIntYrs, TInt aIntMon, TInt aIntDay,
                                TInt aIntHrs, TInt aIntMin, TInt aIntSec );

        /**
        * Add a single element of time to the string array
        * @param aNumOfElements         Number of elements of the interval
        * @param aResourceIdSingle      Resource id to be used in case of
        *                               single unit
        * @param aResourceIdOneFinal    Resource id to be used in case of
        *                               units ending with 1, from 21
        *                               (requested by Slavic languages)
        * @param aResourceIdTwoFour     Resource id to be used in case of
        *                               units included from two to four, except
        *                               12-14 (requested by Slavic languages)
        * @param aResourceIdFiveZero    Resource id to be used in case of
        *                               units ending from 5 to 9, plus range
        *                               from 11 to 14 (requested by Slavic
        *                               languages)
        * @param aStrings               Array of strings where to add the
        *                               element
        */
        void AddSinglePartOfTimeL( TInt aNumOfElements,
                                   TInt aResourceIdSingle,
                                   TInt aResourceIdOneFinal,
                                   TInt aResourceIdTwoFour,
                                   TInt aResourceIdFiveZero,
                                   CDesCArrayFlat* aStrings );

        /**
        * Add File location line in listbox.
        * @param aContent   Array containing paths of related MOs
        * @param aItems     Array where the listbox items have to be added
        */
        void AddLocationLineL( const TDesC& aContentPath,
                               CDesCArray* aItems );

        /**
        * Add "Usage rights" line
        * @param aItems         Array where the listbox items have to be added
        * @param aTypeOfRights  String reporting the type of rights
                                ContentAccess:EPlay, ContentAccess:EView,
                                ContentAccess:EExecute and ContentAccess:EPrint
        */
        void AddUsageRightsL( CDesCArray* aItems,
                              const TUint32& aIntent );

        /**
        * Add "Usage times left" line
        * @param aCounter       Number of times
        * @param aItems         Array where the listbox items have to be added
        * @param aTypeOfRights  String reporting the type of rights
                                ContentAccess:EPlay, ContentAccess:EView,
                                ContentAccess:EExecute and ContentAccess:EPrint
        */
        void AddUsageTimesLeftL( TUint32 aCounter, CDesCArray* aItems,
                                 const TUint32& aIntent );

        /**
        * Add "Rights valid from" line
        * @param aStartDateTime Start Datetime
        * @param aItems         Array where the listbox items have to be added
        * @param aTypeOfRights  String reporting the type of rights
                                ContentAccess:EPlay, ContentAccess:EView,
                                ContentAccess:EExecute and ContentAccess:EPrint
        */
        void AddRightsValidFromL( const TTime& aStartDateTime,
                                  CDesCArray* aItems,
                                  const TUint32& aIntent );

        /**
        * Add "Rights valid until" line
        * @param aEndDateTime   Expiring Datetime
        * @param aItems         Array where the listbox items have to be added
        * @param aTypeOfRights  String reporting the type of rights
                                ContentAccess:EPlay, ContentAccess:EView,
                                ContentAccess:EExecute and ContentAccess:EPrint
        */
        void AddRightsValidUntilL( const TTime& aEndDateTime,
                                   CDesCArray* aItems,
                                   const TUint32& aIntent );

        /**
        * Add "Usage time left" line
        * @param aInterval      Interval of usage time left
        * @param aItems         Array where the listbox items have to be added
        * @param aIsAccumulatedTime ETrue if the line is for accumulated time

                                ContentAccess:EPlay, ContentAccess:EView,
                                ContentAccess:EExecute and ContentAccess:EPrint
        */
        void AddUsageTimeLeftL( const TTimeIntervalSeconds& aInterval,
                                CDesCArray* aItems,
                                const TBool aIsAccumulatedTime,
                                const TUint32& aIntent );

    private: //data
        // The details to be displayed
        TDetails iDetails;

        // Tells if it has to display details when DRMUI was started embedded.
        TBool iStartEmbedded;

        //pointer to newly created list box.
        CEikTextListBox* iListBox;

    };

#endif

// End of File
