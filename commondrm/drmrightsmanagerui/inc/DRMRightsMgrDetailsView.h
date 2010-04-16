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
* Description:  Declares view for Details class
*
*/



#ifndef DRMRIGHTSMGRDETAILSVIEW_H
#define DRMRIGHTSMGRDETAILSVIEW_H

// INCLUDES
#include <aknview.h>
#include "DRMRightsMgrDetailsContainer.h"

// FORWARD DECLARATIONS
class CDRMRightsMgrDetailsContainer;
class CDRMRights;
class DRMCommon;


// CLASS DECLARATION

/**
*  CDRMRightsMgrDetailsView
*  view class.
*/
class CDRMRightsMgrDetailsView : public CAknView
    {
    public:
        // Panic code used in this class
        enum TDRMUIViewPanicCodes
            {
            EDRMUIViewPanicNullPtr = 1      // Failed to create the container
            };

    public: // Constructors and destructor
        /**
        * Constructor.
        * @param aStartEmbedded ETrue if it has to display details when DRMUI
        *                       was started embedded.
        * @param aViewId  ID of the view
        * @param aContext The context for the help
        */
        CDRMRightsMgrDetailsView( TBool aStartEmbedded );

        /**
        * Symbian OS two-phased constructor
        * @param aStartEmbedded ETrue if it has to display details when DRMUI
        *                       was started embedded.
        * @return DRMUI DetailsView.
        */
        static CDRMRightsMgrDetailsView* NewL( TBool aStartEmbedded );

        /**
        * Symbian OS two-phased constructor
        * @param aStartEmbedded ETrue if it has to display details when DRMUI
        *                       was started embedded.
        * @return DRMUI DetailsView.
        */
        static CDRMRightsMgrDetailsView* NewLC( TBool aStartEmbedded );

        /**
        * Symbian default constructor.
        * @param aResourceId Title resource
        */
        void ConstructL( const TInt aResourceId );

        /**
        * Destructor.
        */
        ~CDRMRightsMgrDetailsView();


    public: // from CAknView

        /**
        * Returns view id.
        * @return TUid
        */
        TUid Id() const;

        /**
        * Handles the changes of the client rect
        */
        void HandleClientRectChange();


     public:

        /**
        * From MEikCommandObserver. Handles commands.
        * @param aCommand Command to be handled.
        */
        void HandleCommandL( TInt aCommand );


    protected:
        /**
        * From AknView. Called when the view is going to be activated
        * @param aPrevViewId        Previous View ID. Not used
        * @param aCustomMessageId   Custom message ID. Not used
        * @param aCustomMessage     Custom Message. Not used
        */
        void DoActivateL(const TVwsViewId& aPrevViewId,
                        TUid aCustomMessageId,
                        const TDesC8& aCustomMessage);

        /**
        * From AknView. Called when the view is going to be deactivated
        */
        void DoDeactivate();

        /**
        * From MEikMenuObserver. Function called by the EIKON framework just
        * before it displays a menu pane.
        * @param aResourceId Menu pane resource ID
        * @param aMenuPane Menu pane pointer
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );


    public: // new
        /**
        * Refreshes the content of the listbox. Used when it is requested to
        * view details from a RO, e.g. from Keys not in use view or when DRMUI
        * is started to show only Details View.
        * @param aRights               The Rights Object
        * @param aTitle                Title to be showed in the title pane
        * @param aFullName             Full name of the object
        * @param aContentCanBeSent     ETrue if the object can be sent
        * @param aIndividualConstraint ETrue if object has individual constraint
        * @param aUsageAllowed         ETrue if object use allowed with current SIM
        */
        void RefreshListBoxL( CDRMRights* aRights, const TDesC& aTitle,
                              const TDesC& aFullName,
                              const TBool aContentCanBeSent = EFalse,
                              const TBool aIndividualConstraint = EFalse,
                              const TBool aUsageAllowed = ETrue );

        /**
        * Refreshes the content of the listbox. Used when it is requested to
        * view details from a Windows Media DRM license.
        * @param aArray                The WM DRM Rights Object array
        * @param aTitle                Title to be showed in the title pane
        * @param aFullName             Full name of the object
        */
        void RefreshListBoxL(
            ContentAccess::RStreamablePtrArray<ContentAccess::CRightsInfo>& aArray,
            const TDesC& aTitle,
            const TDesC& aFullName );

    private:    // new

        /**
        * Creates the details view container
        */
        void CreateContainerL();

        /**
        * Instantiates the new details view container. Called by DoActivateL.
        */
        void NewContainerL();

        /**
        * Removes the container
        */
        void RemoveContainer();

        /**
        * Sets the title of the view
        * @param aTitle Text to be showed as the title
        */
        void SetTitlePaneL( const TDesC& aTitle );

        /**
        * Sets Help context
        * @return The array of help contexts.
        */
        CArrayFix<TCoeHelpContext>* HelpContextL() const;

        /**
        * Sets MSK id + label
        */
        void SetMiddleSoftKeyLabelL( const TInt aResourceId,
                                     const TInt aCommandId );

        /**
        * Makes the left softkey visible or invisible
        * @param aVisible   ETrue when the left softkey should be visible.
        */
        void MakeLeftSoftkeyVisible( TBool aVisible );

        void DeleteConstraints();

        /**
        * Initializes and prepares internal data for refreshing of the
        * list box.
        * @param aFullName             Full name of the object
        * @param aDrmScheme            Tells the DRM protection scheme of the file
        * @param aRights               Data of a Rights Object
        * @param aContentCanBeSent     ETrue if the object can be sent
        * @param aIndividualConstraint ETrue if object has individual constraint
        * @param aUsageAllowed         ETrue if object use allowed with current SIM
        */
        void PrepareRefreshL( const TDesC& aFullName,
                              const TInt& aDrmScheme,
                              TRights& aRights,
                              const TBool& aContentCanBeSent,
                              const TBool& aIndividualConstraint,
                              const TBool& aUsageAllowed );

        /**
        * Finalizes refreshing of the list box
        * @param aTitle                Title to be showed in the title pane
        * @param aRights               Data of a Rights Object
        */
        void FinalizeRefreshL( const TDesC& aTitle,
                               const TRights& aRights );

    private: // Data

        // Pointer to the container
        CDRMRightsMgrDetailsContainer* iContainer;

        // ID of the view
        TUid iViewId;

        // Data about RO to be shown
        TDetails iDetails;

        // Tells if it has to display details when DRMUI was started embedded.
        TBool iStartEmbedded;
    };


#endif

// End of File
