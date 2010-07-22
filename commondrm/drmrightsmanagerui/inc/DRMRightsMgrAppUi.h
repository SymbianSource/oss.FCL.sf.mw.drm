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
* Description:  Declares UI class for application.
*
*/



#ifndef DRMRIGHTSMGRAPPUI_H
#define DRMRIGHTSMGRAPPUI_H

// INCLUDES
#include <aknViewAppUi.h>
#include <AknProgressDialog.h>
#include <apparc.h>
#include <DRMRightsClient.h>
#include <DrmConstraint.h>

#include "DRMClockClient.h"
#include "DrmViewItems.h"

#include "DRMRightsManager.hrh"


// FORWARD DECLARATIONS
class CAknWaitDialog;
class CDRMRights;
class DRMCommon;


// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* - view architecture
* - status pane
*/
class CDRMRightsMgrAppUi :  public  CAknViewAppUi,
                            public  MApaEmbeddedDocObserver,
                            private MProgressDialogCallback,
                            public  MCoeForegroundObserver
    {
    public:     // Data Types
        // Status of scanning of the ROs
        enum TStatusScan
            {
            // Scanning started when app launched
            EDRMStatusOpening = 0,

            // Scanning started when app launched embedded
            EDRMStatusOpeningEmbedded,

            // Scanning finished
            EDRMStatusFinished
            };

    public:    // Constructors and destructor
        /**
        * C++ default constructor
        */
        CDRMRightsMgrAppUi();

        /**
        * Symbian default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CDRMRightsMgrAppUi();


    private:    // New functions

        /**
        * Processes engine error
        * @param aCloseImmediately  ETrue if program should be close
        *                           immediately. EFalse if only app shutter
        *                           should be started.
        */
        void ProcessEngineErrorL( TBool aCloseImmediately );

        /**
        * Open Details View only, when DRMUi is launched to show only that
        * view, but not in embedded mode
        * @param aParams    The params used to launch DRMUI
        */
        void OpenDetailsViewNotEmbeddedL( const TDesC& aParams );

        /**
        * Creates the Wait Dialog to be showed during long asynchronous
        * operations
        */
        void CreateWaitDialogLC();

        /**
        * Extracts the name of the object to be showed in the list
        * @param aFullName  The path name of the object
        * @param aItemName  In output the name of the object to be showed in
        *                   the list
        * @param aID        The ID of the object
        * @param aIsGroup   Tells if the object is a group
        */
        void GetItemNameL( const TDesC& aFullName, TDes& aItemName,
                           const TDesC8& aID, const TBool aIsGroup );


        /**
        * Evaluates if the given content file can be shown in keys list, and if it
        * can be forwarded or not. Returns KErrNone if successful otherwise error code.
        * @param aFileName    File name with full path
        * @param aContentURI  Content id
        * @param aListable    Tells if MO can be shown in keys list
        * @param aSendable    Tells if MO can be forwarded
        */
        TInt GetItemDataL( const TDesC& aFileName,
                           const TDesC8& aContentURI,
                           TBool& aListable,
                           TBool& aSendable );

        /**
        * Get a rights manager instance
        * @return               Rights Manager instance
        */
        ContentAccess::CRightsManager* GetRightsManagerL();

        /**
        * Checks the status of WM DRM rights
        * @param aContentURI    ContentURI for object to be opened when
        *                       application is launched
        * @param aStatus        Status of the rights
        * @param aArray         Array for storing the rights
        */
        void CheckWmDrmRightsL( const TDesC8& aContentURI, TInt& aStatus,
            ContentAccess::RStreamablePtrArray<ContentAccess::CRightsInfo>& aArray );

        /**
        * Checks the status of OMA DRM rights
        * @param aContentURI    ContentURI for object to be opened when
        *                       application is launched
        * @param aLocalID       LocalID for object to be opened when
        *                       application is launched
        * @param aRights        Pointer to instance of CDRMRights for
        *                       storing the rights
        * @param aStatus        Status of the rights
        */
        void CheckOmaDrmRightsL( const TDesC8& aContentURI,
                                 const TUint32 aLocalID,
                                 CDRMRights*& aRights,
                                 TInt& aStatus );

        /**
        * Checks the individual constraint of OMA DRM rights
        * @param aContentURI            ContentURI for object to be opened when
        *                               application is launched
        * @param aIndividualConstraint  Whether rights have an individual
        *                               constraint or not
        * @param aUsageAllowed          Whether individually constrained rights
        *                               can be used for using the content
        */
        void CheckIndividualConstraint( const TDesC8& aContentURI,
                                        TBool& aIndividualConstraint,
                                        TBool& aUsageAllowed );
        
        /**
        * Checks the composition of best rights to be shown in the details view.
        * Leaves with KErrNotFound if no rights are found.
        * @param aContentURI            ContentURI for object to be opened when
        *                               application is launched
        * @param aRights                Pointer to instance of CDRMRights for
        *                               storing the rights                                                            
        */                               
        void FindBestCompositionRightsL( const TDesC8& aContentURI,
                                         CDRMRights*& aRights ); 

        /**
        * Compares the permissions in the the permission list one by one and 
        * constraint by constraint and stores the best constraint to the given 
        * composition permission. The function should be called only for a list of 
        * valid permissions. Puts the composition permission to cleanup stack. 
        * @param aCompositionPermission Permission which includes a composition of best 
        *                               constraints (or full rights information) from 
        *                               the given permission list.                                 
        * @param aPermissionList        List which includes permissions to be checked
        *                               constraint by constraint in order to create the
        *                               best composition permission.                                                                                          
        */                  
        void CheckBetterPermissionsAndStoreCompositionLC( 
                CDRMPermission*& aCompositionPermission,
                RPointerArray<CDRMPermission>& aList );
        /**
        * Compares two permissions, and returns ETrue if aNewOne is more suitable
        * for the usage. Assumes both are valid, i.e. not expired.
        * @param aNewOne                New constraint to be compared to the old one                             
        * @param aOldOne                Old constraint to be compared to the new one 
        */
        TBool BetterPermission( const CDRMConstraint& aNewOne,
                                const CDRMConstraint& aOldOne );
                                                    
    public: // New functions
        /**
        * @param aContentURI    ContentURI for object to be opened when
        *                       application is launched embedded
        * @param aLocalID       LocalID for object to be opened when
        *                       application is launched embedded
        * @param aEmbedded      Tells if the DRMUi was launched embedded
        * @param aDrmScheme     Tells the DRM protection scheme of the file
        */
        void StartOnlyForDetailsL( const TDesC8& aContentURI,
                                   const TUint32 aLocalID,
                                   const TBool aEmbedded,
                                   const TBool aDrmScheme );

        /**
        * Gets the index in array of items of the selected item.
        * @return The index of the selected item.
        */
        inline TInt SelectedIndexInArray();

        /**
        * Returns the selected item name.
        * @param aItemName  In output the selected item name
        */
        inline void SelectedItemName( TFileName& aItemName );

        /**
        * Returns the selected item path name.
        * @param aItemFullName  In output the selected item path name
        */
        inline void SelectedItemFullName( TFileName& aItemFullName );

        /**
        * Tells if the selected item content can be sent
        * @return   ETrue if the selected item content can be sent
        */
        inline TBool SelectedContentCanBeSent();

        /**
        * Tells if the selected item has unlimited rights
        * @return   ETrue if the selected item has unlimited rights
        */
        inline TBool UnlimitedRights();

        /**
        * Tells if the selected item has individual constraint
        * @return   ETrue if the selected item has individual constraint
        */
        inline TBool IndividualConstraint();

        /**
        * Tells if the selected item can be used with current SIM
        * @return   ETrue if the selected item can be used with current SIM
        */
        inline TBool UsageAllowed();

        /**
        * Returns the local ID of the selected RO
        * @return The local ID of the selected RO
        */
        inline TUint32 SelectedLocalID();

        /**
        * Returns the content URI of the selected RO
        * @param aContentURI    In output, the content URI of the selected RO
        */
        inline void SelectedContentURI( TBuf8<KMaxFileName>& aContentURI );

        /**
        * Tells if the selected item is a group
        * @return   ETrue if the selected item is a group
        */
        inline TBool SelectedIsGroup();

        /**
        * Sets the name of the selected item
        * @param aItemName  The name of the selected item
        */
        inline void SetSelectedItemName( const TDesC& aItemName );

        /**
        * Sets the path name of the selected item
        * @param aItemFullName  The path name of the selected item
        */
        inline void SetSelectedItemFullName( const TDesC& aItemFullName );

        /**
        * Sets the attribute that tells if the selected item content can be
        * forwarded or not
        * @param aContentCanBeSent  ETrue if the selected item content can be
        *                           forwarded
        */
        inline void SetSelectedContentCanBeSent( const TBool aContentCanBeSent );

        /**
        * Sets the individual constraint attribute.
        * @param aIndividualConstraint ETrue if individual constraint
        */
        inline void SetSelectedIndividualConstraint( const TBool aIndividualConstraint );

        /**
        * Sets the usage allowed with current SIM attribute.
        * @param aUsageAllowed ETrue if allowed
        */
        inline void SetSelectedUsageAllowed( const TBool aUsageAllowed );




    private:    // From CEikAppUi
        /**
        * From CEikAppUi, handles key events.
        * @param aKeyEvent Event to handled.
        * @param aType Type of the key event.
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed).
        */
        virtual TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent,
                                              TEventCode aType);

        /**
        * Handles resource change events.
        * @param aType: The type of resources that have changed
        */
        void HandleResourceChangeL( TInt aType );


    public:    // From CEikAppUi
        /**
        * Takes care of command handling.
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );

        /**
        * Processes shell commands.
        * @param aCommand       The shell command sent to the application.
        * @param aDocumentName  The name of the document
        * @param aTail          Command line tail.
        * @return               Always EFalse
        */
        TBool ProcessCommandParametersL( TApaCommand aCommand,
                                         TFileName& aDocumentName,
                                         const TDesC8& aTail );

        /**
        * Processes Application Architecture messages.
        * @param aUid       The messageÂs unique identifier.
        * @param aParams    Message parameters
        */
        void ProcessMessageL( TUid aUid, const TDesC8& aParams );


    public: // From MApaEmbeddedDocObserver

        /**
        * Implements the required behaviour when the editing of an embedded
        * document completes.
        * @param aMode  Indicates the state of the document
        */
        void NotifyExit( TExitMode aMode );


    public: // From MProgressDialogCallback
        /**
        * Called when/if the dialog has been dismissed.
        * @param aButtonId  Pressed button
        */
        void DialogDismissedL( TInt aButtonId );

    public: // From MCoeForegroundObserver
        /**
        * Handles the application coming to the foreground
        *
        */
        void HandleGainingForeground();

        /**
        * Handles the application going into the background
        *
        */
        void HandleLosingForeground();

    private: //Data
        // ETrue whan DRMUI is started embedded to show the Details View only.
        TBool iStartEmbedded;

        // Data of the selected item.
        TItemData iSelectedItem;

        // Status of the scanning.
        TStatusScan iStatusScan;

        // Wait dialog. Owned.
        CAknWaitDialog* iWaitDialog;

        // Pointer to DRMCommon. Owned.
        DRMCommon* iDRMCommon;

        // The DRM Rights Client.
        RDRMRightsClient iRightsClient;

        // The DRM protection of the given file.
        TInt iDrmScheme;

        // The local ID of the rights
        TUint32 iLocalID;

        // Content URI of the file for which the details view has been opened
        HBufC8* iContentURI;

        // The details view has been opened once. This information is used to
        // update the details view when returning to details view from another
        // application
        TBool iForegroundHasBeenActive;
        
        // Drm clock client
        RDRMClockClient iClockClient;
        
    };

// Include inline functions
#include "DRMRightsMgrAppUi.inl"


#endif

// End of File
