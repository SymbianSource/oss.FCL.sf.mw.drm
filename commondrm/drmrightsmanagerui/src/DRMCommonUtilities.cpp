/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Common utilities functions
*
*/


// INCLUDE FILES
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <Oma2Agent.h>

#include <DRMRightsManager.rsg>
#include <data_caging_path_literals.hrh>

#include <utf.h>

#include "DRMCommonUtilities.h"
#include "DRMRightsMgrApp.h"
#include "DRMUILogger.h"
#include "DRMRightsMgrAppUi.h"
#include "drmutilityinternaltypes.h"

// CONSTANTS

// ROM folder
_LIT( KDriveZ, "z:" );

// Private folder
_LIT( KPrivateFolder, "\\private" );
_LIT( KSystemFolder, "\\system" );
_LIT( KTempFolder, "\\temp" );

// Name of the MBM file containing icons
_LIT( KDRMUIMBMFile, "drmrightsmanager.mbm" );

// ================= MEMBER FUNCTIONS =======================
//
// -----------------------------------------------------------------------------
// DRMCommonUtilities::ShowConfirmationQueryL
// -----------------------------------------------------------------------------
//
TBool DRMCommonUtilities::ShowConfirmationQueryL( const TInt aResourceId,
                                                  const TDesC& aString,
                                                  CCoeEnv* aLoaderEnv )
    {
    HBufC* stringNote = StringLoader::LoadLC( aResourceId, aString,
                                              aLoaderEnv );
    TBool retVal = ShowConfirmationQueryL( *stringNote );
    CleanupStack::PopAndDestroy( stringNote ); // stringNote

    return retVal;
    }


// -----------------------------------------------------------------------------
// DRMCommonUtilities::ShowConfirmationQueryL
// -----------------------------------------------------------------------------
//
TBool DRMCommonUtilities::ShowConfirmationQueryL( const TInt aResourceId,
                                                  CCoeEnv* aLoaderEnv )
    {
    HBufC* stringNote = StringLoader::LoadLC( aResourceId, aLoaderEnv );
    TBool retVal = ShowConfirmationQueryL( *stringNote );
    CleanupStack::PopAndDestroy( stringNote ); // stringNote

    return retVal;
    }


// -----------------------------------------------------------------------------
// DRMCommonUtilities::ShowConfirmationQueryL
// -----------------------------------------------------------------------------
//
TBool DRMCommonUtilities::ShowConfirmationQueryL( const TDesC& aPrompt )
    {
    CAknQueryDialog *queryDialog = CAknQueryDialog::NewL( CAknQueryDialog::ENoTone );

    return queryDialog->ExecuteLD( R_DRM_CONFIRMATION_QUERY, aPrompt ) != 0;
    }


// -----------------------------------------------------------------------------
// DRMCommonUtilities::ShowConfirmationNoteL
// -----------------------------------------------------------------------------
//
void DRMCommonUtilities::ShowConfirmationNoteL( const TInt aResourceId,
                                                const TDesC& aString,
                                                CCoeEnv* aLoaderEnv )
    {
    HBufC* stringNote = StringLoader::LoadLC( aResourceId, aString,
                                              aLoaderEnv );
    ShowConfirmationNoteL( *stringNote );
    CleanupStack::PopAndDestroy( stringNote ); // stringNote
    }


// -----------------------------------------------------------------------------
// DRMCommonUtilities::ShowConfirmationNoteL
// -----------------------------------------------------------------------------
//
void DRMCommonUtilities::ShowConfirmationNoteL( const TInt aResourceId,
                                                CCoeEnv* aLoaderEnv )
    {
    HBufC* stringNote = StringLoader::LoadLC( aResourceId, aLoaderEnv );
    ShowConfirmationNoteL( *stringNote );
    CleanupStack::PopAndDestroy( stringNote ); // stringNote
    }


// -----------------------------------------------------------------------------
// DRMCommonUtilities::ShowConfirmationNoteL
// -----------------------------------------------------------------------------
//
void DRMCommonUtilities::ShowConfirmationNoteL( const TDesC& aPrompt )
    {
    CAknConfirmationNote* confNote = new( ELeave )CAknConfirmationNote(
                                                                    ETrue );
    confNote->ExecuteLD( aPrompt );
    }


// -----------------------------------------------------------------------------
// DRMCommonUtilities::ShowConfirmationNoteL
// -----------------------------------------------------------------------------
//
void DRMCommonUtilities::ShowInformationNoteL( const TInt aResourceId,
                                               CCoeEnv* aLoaderEnv )
    {
    HBufC* stringLabel = StringLoader::LoadLC( aResourceId, aLoaderEnv );
    CAknInformationNote* dialog = new( ELeave )CAknInformationNote( ETrue );
    dialog->ExecuteLD( *stringLabel );
    CleanupStack::PopAndDestroy( stringLabel );   // stringLabel
    }


// -----------------------------------------------------------------------------
// DRMCommonUtilities::GetFutureActivationTime
// -----------------------------------------------------------------------------
//
TTime DRMCommonUtilities::GetFutureActivationTime( CDRMRights& aRights )
    {
    CDRMRightsConstraints* constraints = NULL;
    TTime playStartTime( Time::MaxTTime() );
    TTime displayStartTime( Time::MaxTTime() );
    TTime printStartTime( Time::MaxTTime() );
    TTime executeStartTime( Time::MaxTTime() );

    aRights.GetPlayRight( constraints );
    if ( constraints )
        {
        constraints->GetStartTime( playStartTime );
        delete constraints;
        constraints = NULL;
        }

    aRights.GetDisplayRight( constraints );
    if ( constraints )
        {
        constraints->GetStartTime( displayStartTime );
        delete constraints;
        constraints = NULL;
        }

    aRights.GetPrintRight( constraints );
    if ( constraints )
        {
        constraints->GetStartTime( printStartTime );
        delete constraints;
        constraints = NULL;
        }

    aRights.GetExecuteRight( constraints );
    if ( constraints )
        {
        constraints->GetStartTime( executeStartTime );
        delete constraints;
        constraints = NULL;
        }

    return ( Min( Min( playStartTime, displayStartTime ),
                  Min( printStartTime, executeStartTime ) ) );
    }


// -----------------------------------------------------------------------------
// DRMCommonUtilities::GetIconsFileName
// -----------------------------------------------------------------------------
//
void DRMCommonUtilities::GetIconsFileName( TDes& aIconsFileName )
    {
    aIconsFileName.Append( KDriveZ );
    aIconsFileName.Append( KDC_APP_BITMAP_DIR );
    aIconsFileName.Append( KDRMUIMBMFile );

    return;
    }


// -----------------------------------------------------------------------------
// DRMCommonUtilities::IsInPrivateFolderL
// -----------------------------------------------------------------------------
//
TBool DRMCommonUtilities::IsInPrivateFolderL( const TDesC& aContentPath )
    {
    TInt retval = EFalse;
    TParse path;
    User::LeaveIfError( path.Set( KNullDesC, &aContentPath, NULL ) );

    HBufC* driveBuf = HBufC::NewLC( KMaxFileName );

    TPtr driveAndPath( const_cast<TUint16*>( driveBuf->Ptr()), 0, KMaxFileName );
    driveAndPath = path.DriveAndPath();
    driveAndPath.LowerCase();

    retval = ( driveAndPath.Find( KPrivateFolder ) == path.Drive().Length() ||
               driveAndPath.Find( KSystemFolder ) == path.Drive().Length() ||
               driveAndPath.Find( KTempFolder ) == path.Drive().Length() );
    CleanupStack::PopAndDestroy( driveBuf );
    return retval;
    }

// -----------------------------------------------------------------------------
// DRMCommonUtilities::RightsIssuerUrlL
// -----------------------------------------------------------------------------
//
TInt DRMCommonUtilities::RightsIssuerUrlL( const TFileName& aFileName,
                                           HBufC*& aRightsIssuerURL )
    {
    TVirtualPathPtr virtualPath( aFileName, KDefaultContentObject );
    TBuf<KUrlMaxLen> rightsIssuer;
    TInt retval( KErrNone );

    CData* content = NULL;
    TRAPD( r, content = CData::NewL( virtualPath, EPeek, EContentShareReadWrite ) );
    if ( r == KErrInUse )
        {
        content = CData::NewL( virtualPath, EPeek, EContentShareReadOnly );
        }
    else if ( r != KErrNone )
        {
        retval = r;
        }

    CleanupStack::PushL( content );

    if ( retval == KErrNone )
        {
        // Get value of rights-issuer header field
        retval = content->GetStringAttribute( ERightsIssuerUrl, rightsIssuer );
        if ( ( retval == KErrNone ) && ( rightsIssuer.Length() != 0 ) )
            {
            aRightsIssuerURL = rightsIssuer.AllocL();
            }
        else
            {
            delete aRightsIssuerURL;
            aRightsIssuerURL = NULL;
            retval = KErrNotFound;
            }
        }
    CleanupStack::PopAndDestroy( content );
    return retval;
    }


// -----------------------------------------------------------------------------
// DRMCommonUtilities::GetRightsStatus
// -----------------------------------------------------------------------------
//
TInt DRMCommonUtilities::GetRightsStatus( CDRMRights& aRights,
                                          CDRMRights::TExpiration& aExpir)
    {
    TUint32 constraintSpec = 0;
    CDRMRights::TRestriction restriction;
    TUint32 constType;
    TInt err = KErrNone;

    err = aRights.GetRightsInfo( constraintSpec, restriction, aExpir, constType );

    return err;
    }


// -----------------------------------------------------------------------------
// DRMCommonUtilities::ParseParametersAndStartL
// -----------------------------------------------------------------------------
//
void DRMCommonUtilities::ParseParametersAndStartL( TLex aLex,
                                                   TBool aStartEmbedded,
                                                   CDRMRightsMgrAppUi& aAppUi )
    {
    CArrayFixFlat<TPtrC>* params;       // array of parameters
    HBufC* url16( NULL );
    HBufC* fullName( NULL );
    TUint32 drmScheme( EDrmSchemeUnknownDrm );
    params = new( ELeave )CArrayFixFlat<TPtrC>( sizeof( TPtrC ) );
    CleanupStack::PushL( params );
    TChar ch;

    aLex.Mark();
    while ( !aLex.Eos() )                // Extract the parameters
        {
        ch = aLex.Get();
        if ( ch == '\x00' ) {
            aLex.UnGet();
            params->AppendL( aLex.MarkedToken() );
            aLex.Get();
            aLex.Mark();
            }
        }

    TInt startParam = KMaxTInt;

    // Parse the DRM Rights Manager launch parameter string. First parameter
    // indicates the launching type (embedded, stand alone, embedded with full
    // path name parameter or stand alone with full path name parameter), second
    // parameter is Local ID for OMA DRM file, third one is the content URI in
    // case of OMA DRM and full file path name in case of WM DRM, whereas the fourth
    // parameter indicates the DRM protection scheme of the file for which the
    // details view has been launched. The fifth parameter is the full file path in
    // case of an OMA DRM file.
    TLex lexStart ( params->At( 0 ) );

    // First parameter: type of operation to be performed
    User::LeaveIfError( lexStart.Val( startParam ) );

    if ( ( aStartEmbedded && ( startParam == EDrmLaunchParamEmbeddedHelper ||
        startParam == EDrmLaunchParamEmbeddedUtility ) ) ||
         ( !aStartEmbedded && ( startParam == EDrmLaunchParamStandAloneHelper ||
        startParam == EDrmLaunchParamStandAloneUtility ) ) )
        {
        TUint32 localID;

        TLex lexLocalID ( params->At( 1 ) );

        // Second parameter: LocalID
        User::LeaveIfError( lexLocalID.Val( localID, EDecimal ) );

        // Third parameter: Content URI
        HBufC8* contentURI = HBufC8::NewLC( params->At( 2 ).Length() );
        contentURI->Des().Copy( params->At( 2 ) );

        if ( params->Count() > 3 && ( startParam == EDrmLaunchParamEmbeddedUtility
            || startParam == EDrmLaunchParamStandAloneUtility ) )
            {

            // Fourth parameter: DRM protection scheme
            TLex lexDrmType( params->At( 3 ) );
            lexDrmType.Val( drmScheme, EDecimal );

            TParse parser;

            if ( drmScheme == EDrmSchemeWmDrm )
                {
                // We have a WM DRM file
                // Find out the file name and the item name and store them to
                // the file details.
                url16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
                    contentURI->Des() );
                CleanupStack::PushL( url16 );

                if ( url16 )
                    {
                    // The received fourth parameter already has the
                    // full path name
                    aAppUi.SetSelectedItemFullName( url16->Des() );
                    User::LeaveIfError( parser.Set( url16->Des(), NULL, NULL ) );
                    // Get the file name and extension
                    aAppUi.SetSelectedItemName( parser.NameAndExt() );
                    }
                CleanupStack::PopAndDestroy( url16 );
                }

            // Fifth parameter: Full file path for OMA DRM
            if ( drmScheme == EDrmSchemeOmaDrm &&
                ( startParam == EDrmLaunchParamEmbeddedUtility ||
                startParam == EDrmLaunchParamStandAloneUtility ) )
                {
                // Find out the file name and the item name and store them to
                // the file details.
                fullName = HBufC::NewLC( params->At( 4 ).Length() );
                fullName->Des().Copy( params->At( 4 ) );

                aAppUi.SetSelectedItemFullName( fullName->Des() );
                User::LeaveIfError( parser.Set( fullName->Des(), NULL, NULL ) );
                // Get the file name and extension
                aAppUi.SetSelectedItemName( parser.NameAndExt() );
                CleanupStack::PopAndDestroy( fullName );
                }
            }
        else
            {
            // DRM Helper has launched DRM Rights Manager. Currently DRM Helper
            // blocks attempts to launch DRM Rights Manager for both the plain
            // and WM DRM content.
            drmScheme = EDrmSchemeOmaDrm;
            }

        aAppUi.StartOnlyForDetailsL( *contentURI, localID, aStartEmbedded,
            drmScheme );

        CleanupStack::PopAndDestroy( contentURI );
        }

    CleanupStack::PopAndDestroy( params );
    }

// -----------------------------------------------------------------------------
// DRMCommonUtilities::SplitPath
// -----------------------------------------------------------------------------
//
TPtrC DRMCommonUtilities::SplitPath( const TDesC& aFileName, TPtrC& aRestOfName )
    {
    TPtrC ptr( KNullDesC );
    TInt pos = aFileName.Locate('\\'); // first backslash
    TInt length = aFileName.Length();

    if ( pos != KErrNotFound && pos < length )
        {
        TInt totalPos = pos;
        TPtrC name1 = aFileName.Mid(totalPos + 1);
        pos = name1.Locate('\\'); // second backslash
        length = name1.Length();

        if ( pos != KErrNotFound && pos < length )
            {
            totalPos += pos + 1;
            TPtrC name2 = aFileName.Mid(totalPos + 1);
            pos = name2.Locate('\\'); // third backslash
            length = name2.Length();

            if ( pos != KErrNotFound && pos < length )
                {
                totalPos += pos + 1;
                aRestOfName.Set( aFileName.Mid( totalPos + 1, aFileName.Length() - 1 - totalPos ));
                // return the leftmost part of the filename up to third backslash
                return aFileName.Mid(0, totalPos + 1);
                }
            }
        }
    return ptr;
    }



// End of File
