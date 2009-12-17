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
* Description:  Definition of common utilities functions
*
*/


#ifndef DRMCOMMONUTILITIES_H
#define DRMCOMMONUTILITIES_H


#include <e32std.h>
#include <coemain.h>
#include <DRMRights.h>
#include <DRMCommon.h>

class CDRMRightsMgrAppUi;

// CONSTANTS

const TInt KUrlMaxLen( 1024 );

const TInt KMSKControlID( 3 );

// Started operation
enum TOperation
    {
    EDRMRefreshingOnlyForDetails    // Refreshing when starting DRMUI embedded
    };

// CLASS DECLARATION

/**
*  DRMCommonUtilities
*
*  Collection of common utilities
*/
class DRMCommonUtilities
{
    private:
        /**
        * Shows a confirmation note
        * @param aPrompt  Prompt to be shown in the note
        */
        static void ShowConfirmationNoteL( const TDesC& aPrompt );

        /**
        * Shows a confirmation query
        * @param aPrompt  Prompt to be shown in the query
        * @return   ETrue if the query was accepted
        */
        static TBool ShowConfirmationQueryL( const TDesC& aPrompt );

    public:
        /**
        * Shows a confirmation query
        * @param aResourceId The numeric ID of the resource string to be read.
        * @param aString     Reference to the replacing string.
        * @param aLoaderEnv  Pointer to the control environment. 
        * @return   ETrue if the query was accepted
        */
        static TBool ShowConfirmationQueryL( const TInt aResourceId, 
                                             const TDesC& aString,
                                             CCoeEnv* aLoaderEnv );

        /**
        * Shows a confirmation query
        * @param aResourceId The numeric ID of the resource string to be read.
        * @param aLoaderEnv  Pointer to the control environment. 
        * @return   ETrue if the query was accepted
        */
        static TBool ShowConfirmationQueryL( const TInt aResourceId, 
                                             CCoeEnv* aLoaderEnv );

        /**
        * Shows a confirmation note
        * @param aResourceId The numeric ID of the resource string to be read.
        * @param aString     Reference to the replacing string.
        * @param aLoaderEnv  Pointer to the control environment. 
        */
        static void ShowConfirmationNoteL( const TInt aResourceId, 
                                           const TDesC& aString,
                                           CCoeEnv* aLoaderEnv );

        /**
        * Shows a confirmation note
        * @param aResourceId The numeric ID of the resource string to be read.
        * @param aLoaderEnv  Pointer to the control environment. 
        */
        static void ShowConfirmationNoteL( const TInt aResourceId, 
                                           CCoeEnv* aLoaderEnv );

        /**
        * Shows information note
        * @param aResourceId The numeric ID of the resource string to be read.
        * @param aLoaderEnv  Pointer to the control environment. 
        */
        static void ShowInformationNoteL( const TInt aResourceId, 
                                          CCoeEnv* aLoaderEnv );

        /**
        * Gets the activation time for an object that will be activated in the
        * future
        * @param aRights    The RO to be parsed
        * @return   The future activation time
        */
        static TTime GetFutureActivationTime( CDRMRights& aRights );

        /**
        * Gets the path name where the icons reside
        * @param aIconsFileName The pathname.
        */
        static void GetIconsFileName( TDes& aIconsFileName );

        /**
        * Tells if the MO is located in a private folder
        * @param aContentPath   The full name of the MO
        * @return   ETrue if the MO is located in a private folder
        */
        static TBool IsInPrivateFolderL( const TDesC& aContentPath );

        /**
        * Tells if the active view is a list view
        * @param aViewId    The Id of the active view
        * @return   ETrue if the active view is a list view
        */
        static TBool IsListView( const TUid& aViewId );

        /**
        * Gets the URL of the issuer of the rights
        * @param aDRMCommon         Pointer to the current instance of 
        *                           DRMCommon
        * @param aFileName          The full name of the MO
        * @param aRightsIssuerURL   In output it contains the URL of the issuer
        *                           of the rights
        * @return   Error code.
        */
        static TInt RightsIssuerUrlL( const TFileName& aFileName, 
                                      HBufC*& aRightsIssuerURL );

        /**
        * Gets the status of the rights: valid, expired, or future
        * @param aRights    The rights to be parsed
        * @param aExpir     Status of rights
        * @return           Error code 
        */
        static TInt GetRightsStatus( CDRMRights& aRights,
        							 CDRMRights::TExpiration& aExpir);

        /**
        * Parse the parameters used to load the DRMUI and starts the UI 
        * directly on Details View.
        * @param aLex           The parameters used to load the DRMUI (embedded
        *                       or not)
        * @param aStartEmbedded ETrue if DRMUI was launched embedded
        * @param aAppUi         Pointer to the application Ui
        */
        static void ParseParametersAndStartL( TLex aLex, TBool aStartEmbedded,
                                      		  CDRMRightsMgrAppUi& aAppUi );
                                      		  
        /**
        * Separates beginning of file path and rest of path + filename
        * e.g.for localization purposes
        * @param aFileName    File name with full path
        * @param aRestOfName  End part of file name string
        * @return Beginning part of file path if found, otherwise KNullDesc
        */
		static TPtrC SplitPath( const TDesC& aFileName,
								TPtrC& aRestOfName );
                                      		  
};


#endif

// End of File
