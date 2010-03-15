/*
* Copyright (c) 2003 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CDRMRightsMgrAppUi class
*
*/


// INCLUDE FILES
#include <DcfRep.h>
#include <DcfEntry.h>
#include <DRMCommon.h>
#include <DcfCommon.h>
#include <wmdrmagent.h> // for WMDRM file details view

#include <DRMRightsManager.rsg>

#include <AknWaitDialog.h>
#include <aknlistquerydialog.h>
#include <StringLoader.h>
#include <aknnavide.h>
#include <AknDef.h>
#include <barsread.h>  // for resource reader
#include <centralrepository.h>
#include <coeutils.h>

#include <starterclient.h>

// character conversions
#include <utf.h>

// caf
#include <caf/data.h>
#include <caf/caftypes.h>

#include "DRMRightsMgrAppUi.h"
#include "DRMRightsMgrDetailsView.h"
#include "DRMRightsMgrDocument.h"
#include "DRMCommonUtilities.h"
#include "DRMUILogger.h"
#include "DRMRightsManagerPrivateCRKeys.h"
#include "DRMClockClient.h"
#include "drmutilityinternaltypes.h"

#include "Oma1Dcf.h"
#include "oma2dcf.h"
// CONSTANTS
_LIT8( Kflk, "flk:" );
_LIT8( Kldf, "ldf:" );

// ================= MEMBER FUNCTIONS =======================
//
// -----------------------------------------------------------------------------
// CDRMRightsMgrAppUi::CDRMRightsMgrAppUi
// -----------------------------------------------------------------------------
//
CDRMRightsMgrAppUi::CDRMRightsMgrAppUi()
: iStartEmbedded( EFalse ),
  iStatusScan( EDRMStatusFinished ),
  iDrmScheme( EDrmSchemeUnknownDrm ),
  iLocalID( 0 ),
  iContentURI( NULL ),
  iForegroundHasBeenActive( EFalse )
    {
    CLOG_WRITE( "CDRMRightsMgrAppUi::CDRMRightsMgrAppUi" );
    }


// ---------------------------------------------------------
// void CDRMRightsMgrAppUi::ConstructL
// ---------------------------------------------------------
//
void CDRMRightsMgrAppUi::ConstructL()
    {
    CLOG_WRITE( "-->ConstructL" );

    iCoeEnv->AddForegroundObserverL( *this );

    CDRMRightsMgrDetailsView* detailsView( NULL );

    BaseConstructL( EAknEnableSkin | EAppOrientationAutomatic |
        EAknEnableMSK | EAknSingleClickCompatible  );

    User::LeaveIfError( iRightsClient.Connect() );
    iDRMCommon = DRMCommon::NewL();
    if ( !iDRMCommon )
        {
        ProcessEngineErrorL( ETrue );
        }

    iStartEmbedded = iEikonEnv->StartedAsServerApp();

    // Create details view
    if ( iStartEmbedded )
        {
        detailsView = CDRMRightsMgrDetailsView::NewLC( ETrue );
        }
    else
        {
        detailsView = CDRMRightsMgrDetailsView::NewLC( EFalse );
        }

    AddViewL( detailsView );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( detailsView );
    }


// ----------------------------------------------------
// CDRMRightsMgrAppUi::~CDRMRightsMgrAppUi
// ----------------------------------------------------
//
CDRMRightsMgrAppUi::~CDRMRightsMgrAppUi()
    {

    iCoeEnv->RemoveForegroundObserver( *this );

    delete iWaitDialog;

    delete iDRMCommon;

    iRightsClient.Close();

    if ( iStartEmbedded && iDoorObserver )
        {
        iDoorObserver->NotifyExit( MApaEmbeddedDocObserver::ENoChanges );
        }

    if ( iContentURI )
        {
        delete iContentURI;
        }

    }


// ---------------------------------------------------------
// CDRMRightsMgrAppUi::HandleKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CDRMRightsMgrAppUi::HandleKeyEventL( const TKeyEvent& aKeyEvent,
                                                  TEventCode /*aType*/ )
    {
    TChar charCode( aKeyEvent.iCode );

    if ( charCode == EKeyEnter )
        // Default is to show details
        {
        TInt command = EDRMRightsManagerCmdAppViewDet;
        HandleCommandL( command );
        return EKeyWasConsumed;
        }
    else if ( charCode == EKeyLeftArrow || charCode == EKeyRightArrow
        || charCode == EKeyBackspace )
        {
        return EKeyWasConsumed;
        }

    return EKeyWasNotConsumed;
    }


// ----------------------------------------------------
// CDRMRightsMgrAppUi::HandleCommandL
// ---------------------------------------------------------
//
void CDRMRightsMgrAppUi::HandleCommandL( TInt aCommand )
    {

    switch ( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            {
            Exit();
            break;
            }
        default:
            break;
        }
    }


// ----------------------------------------------------
// CDRMRightsMgrAppUi::StartOnlyForDetailsL
// ---------------------------------------------------------
//
void CDRMRightsMgrAppUi::StartOnlyForDetailsL( const TDesC8& aContentURI,
                                               const TUint32 aLocalID,
                                               const TBool aEmbedded,
                                               const TInt aDrmScheme )
    {

    // Store the content related information when the details view
    // is to be shown for the first time so that the information will be
    // available for details view refreshing in the future.
    if ( !iContentURI )
        {
        iDrmScheme = aDrmScheme;
        iLocalID = aLocalID;
        iContentURI = aContentURI.AllocL();
        }

    // For storing WM DRM rights information
    ContentAccess::RStreamablePtrArray<ContentAccess::CRightsInfo> array;
    CleanupClosePushL( array );

    TInt status( KErrCANoRights );
    CDRMRights* rights = NULL;

    if ( aEmbedded )
        {
        iStatusScan = EDRMStatusOpeningEmbedded;
        }
    else
        {
        iStatusScan = EDRMStatusOpening;
        }

    // Check the status of rights. Currently supports OMA and WM DRM.
    switch ( iDrmScheme )
        {
        case EDrmSchemeOmaDrm:
            CheckOmaDrmRightsL( aContentURI, aLocalID, rights, status );
            if ( status < 0 )
                {
                rights = NULL;
                }
            break;

        case EDrmSchemeWmDrm:
            CheckWmDrmRightsL( aContentURI, status, array );
            break;

        default:
            break;
        }

    TFileName itemName;
    TFileName fullName;

    // Find the name and full name for the current item
    SelectedItemName( itemName );
    SelectedItemFullName( fullName );

    TUid uidView = TUid::Uid( EDRMDetailsView );
    CDRMRightsMgrDetailsView* detailsView =
                                ( CDRMRightsMgrDetailsView* )View( uidView );
    if ( detailsView )
        {
        if ( !aEmbedded )
            {
            // If DRM Rights manager has not been launched as embedded,
            // activate details wiew as a local view.
            ActivateLocalViewL( uidView );
            }

            // Refresh the details view window with rights information of the
            // current item
            switch ( iDrmScheme )
                {
                case EDrmSchemeOmaDrm:
                    // Refresh the listbox
                    TRAP( status, detailsView->RefreshListBoxL( rights, itemName,
                        fullName, SelectedContentCanBeSent(), IndividualConstraint(),
                        UsageAllowed() ) );
                    break;

                case EDrmSchemeWmDrm:
                    // WM DRM, Refresh the listbox
                    TRAP( status, detailsView->RefreshListBoxL( array,
                        itemName, fullName ) );
                    break;

                default:
                    break;
                }

            if ( status != KErrNone )
                {
                // If there was an error, show to the user
                ProcessEngineErrorL( ETrue );
            }
        }
    if ( rights )
        {
        delete rights;
        }

    CleanupStack::PopAndDestroy( &array );
    }

// ----------------------------------------------------------------------------
// CDRMRightsMgrAppUi::CreateWaitDialogLC
// ----------------------------------------------------------------------------
//
void CDRMRightsMgrAppUi::CreateWaitDialogLC()
    {
    if ( iWaitDialog )
        {
        delete iWaitDialog;
        iWaitDialog = NULL;
        }

    // Create WaitDialog with message and Cancel button
    iWaitDialog = new( ELeave )CAknWaitDialog( ( REINTERPRET_CAST(
                                CEikDialog**, &iWaitDialog ) ), EFalse );

    iWaitDialog->SetCallback( this );
    iWaitDialog->PrepareLC( R_WAITNOTE );

    iWaitDialog->RunLD();
    }

// ----------------------------------------------------------------------------
// CDRMRightsMgrAppUi::GetItemNameL
// ----------------------------------------------------------------------------
//
void CDRMRightsMgrAppUi::GetItemNameL( const TDesC& aFullName,
                                       TDes& aItemName,
                                       const TDesC8& aID,
                                       const TBool aIsGroup )
    {
    CDcfCommon* dcf = NULL;
    HBufC8* buffer( NULL );
    TBool getName = ETrue;
    TParse parse;

    // don't get the name if dealing with a file from a group
    if ( !aIsGroup )
        {
        TRAPD( r, dcf = CDcfCommon::NewL( aFullName ) );
        if ( r == KErrNone &&
             dcf &&
             dcf->iVersion == EOma2Dcf &&
             static_cast<COma2Dcf*>( dcf )->iGroupId )
            {
            getName = EFalse;
            parse.Set( aFullName, NULL, NULL );
            aItemName = parse.NameAndExt();
            }
        delete dcf;
        }
    if ( getName )
        {
        HBufC* itemNameBuf = NULL;
        TInt err = iRightsClient.GetName( aID, itemNameBuf );

        if ( err == KErrNotFound || itemNameBuf->Length() == 0 )
            {
            // Do not show name if group rights or forward lock
            if ( !aIsGroup )
                {
                if ( ( iRightsClient.ForwardLockURI( buffer ) ==
                        KErrNone ) && buffer )
                    {

                    // Not forward lock
                    if ( aID.Compare( *buffer ) != 0 )
                        {
                        parse.Set( aFullName, NULL, NULL );
                        aItemName = parse.NameAndExt();
                        }
                    delete buffer;
                    buffer = NULL;

                    }
                else
                    {
                    parse.Set( aFullName, NULL, NULL );
                    aItemName = parse.NameAndExt();
                    }
                }
            }
        else if ( err == KErrNone )
            {
            // Forward lock or combined delivery
            // If forward lock, do not show name
            if ( iRightsClient.ForwardLockURI( buffer ) == KErrNone &&
                    buffer )
                {
                if ( aID.Compare( *buffer ) != 0 )
                    {
                    // Combined delivery
                    aItemName = *itemNameBuf;
                    }
                delete buffer;
                }
            else
                {
                // Do not show name if having group rights
                if ( !aIsGroup )
                    {
                    aItemName = *itemNameBuf;
                    }
                }
            }
        else
            {
            User::Leave( err );
            }

        if ( itemNameBuf )
            {
            delete itemNameBuf;
            }
        }
    }



// ----------------------------------------------------------------------------
// CDRMRightsMgrAppUi::GetItemDataL
// ----------------------------------------------------------------------------
//
TInt CDRMRightsMgrAppUi::GetItemDataL( const TDesC& aFileName,
                                       const TDesC8& aContentURI,
                                       TBool& aListable,
                                       TBool& aSendable )
    {
    TInt retval( KErrNone );
    aSendable = aListable = EFalse;
    HBufC8* buffer;

    if ( DRMCommonUtilities::IsInPrivateFolderL( aFileName ) )
        {
        if ( aContentURI.Left( 4 ).Compare( Kflk ) == 0 )
            {
            // FL or CD content
            if ( iRightsClient.ForwardLockURI( buffer ) == KErrNone &&
                 buffer )
                {
                if ( aContentURI.Compare( *buffer ) != 0 ) //forward lock content?
                    {
                    // CD content
                    aListable = ETrue;
                    }
                delete buffer;
                }
            else
                {
                retval = KErrGeneral;
                }
            }
        else if (aContentURI.Left( 4 ).Compare( Kldf ) != 0) //local data file?
            {
            // SD or OMA DRM2 content
            // also rights issuer url existence SHOULD be checked but not possible here
            aListable = aSendable = ETrue;
            }
        }
    else
        {
        if ( ConeUtils::FileExists( aFileName ) )
            {
            TInt protection;
            CData* content = NULL;
            TVirtualPathPtr virtualPath( aFileName, KDefaultContentObject );

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
                retval = content->GetAttribute( EDeliveryMethod, protection );
                if ( retval == KErrNone )
                    {
                    aListable = ( protection == EOmaDrm1ForwardLockDcf ||
                                  protection == EOmaDrm1CombinedDelivery ||
                                  protection == EOmaDrm1CombinedDeliveryDcf ||
                                  protection == EOmaDrm1SeparateDelivery ||
                                  protection == EOmaDrm2 );

                    if ( protection == EOmaDrm1SeparateDelivery ||
                         protection == EOmaDrm2 )
                        {
                        TBuf<KUrlMaxLen> rightsIssuer;

                        // Get rights issuer URL
                        retval = content->GetStringAttribute( ERightsIssuerUrl, rightsIssuer );
                        if ( ( retval == KErrNone ) && ( rightsIssuer.Length() > 0 ) )
                            {
                            aSendable = ETrue;
                            }
                        else
                            {
                            // Can't be sent, any error returned can be ignored
                            aSendable = EFalse;
                            retval = KErrNone;
                            }
                        }
                    }
                }
            CleanupStack::PopAndDestroy( content );
            }
        }

    return retval;
    }


// ----------------------------------------------------------------------------
// CDRMRightsMgrAppUi::DialogDismissedL
// ----------------------------------------------------------------------------
//
void CDRMRightsMgrAppUi::DialogDismissedL( TInt aButtonId )
    {
    if ( aButtonId == -1 )
        {
        // When pressing cancel button.
        // Only cancel if we know that there is an outstanding request
        if ( iStatusScan == EDRMStatusOpening ||
             iStatusScan == EDRMStatusOpeningEmbedded )
            {
            // Issue cancel to service provider
            iRightsClient.Cancel();

            if ( iStatusScan == EDRMStatusOpening )
                {
                // Cancel pressed when opening application --> Exit
                HandleCommandL( EEikCmdExit );
                }
            else if ( iStatusScan == EDRMStatusOpeningEmbedded )
                {
                // Cancel pressed when opening application embedded -->
                // Shut the app
                RunAppShutter();
                }
            }
        }
    }

// ----------------------------------------------------
// CDRMRightsMgrAppUi::HandleGainingForeground()
// ----------------------------------------------------
//
void CDRMRightsMgrAppUi::HandleGainingForeground()
    {
    // Refresh details view when the details view is returned
    // from the background to the foreground.
    if ( iForegroundHasBeenActive && iContentURI )
        {
        StartOnlyForDetailsL( iContentURI->Des(), iLocalID,
            iStartEmbedded, iDrmScheme );
        }
    }


// ----------------------------------------------------
// CDRMRightsMgrAppUi::HandleLosingForeground()
// ----------------------------------------------------
//
void CDRMRightsMgrAppUi::HandleLosingForeground()
    {
    iForegroundHasBeenActive = ETrue;
    }

// ----------------------------------------------------
// CDRMRightsMgrAppUi::NotifyExit
// ----------------------------------------------------
//
void CDRMRightsMgrAppUi::NotifyExit( TExitMode /*aMode*/ )
    {
    }


// ----------------------------------------------------
// CDRMRightsMgrAppUi::HandleResourceChangeL
// ----------------------------------------------------
//
void CDRMRightsMgrAppUi::HandleResourceChangeL( TInt aType )
    {

    CAknViewAppUi::HandleResourceChangeL( aType );

    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TVwsViewId viewId;
        CDRMRightsMgrDetailsView* detailsView;
        if ( GetActiveViewId( viewId ) == KErrNone )
            {
            detailsView = ( CDRMRightsMgrDetailsView* )View( viewId.iViewUid );
            if ( detailsView )
                {
                detailsView->HandleClientRectChange();
                }
            }
        else
            {
             detailsView = ( CDRMRightsMgrDetailsView* )View( TUid::Uid( EDRMDetailsView ) );
             if ( detailsView )
                {
                detailsView->HandleClientRectChange();
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CDRMRightsMgrAppUi::ProcessCommandParametersL
// -----------------------------------------------------------------------------
//
TBool CDRMRightsMgrAppUi::ProcessCommandParametersL( TApaCommand aCommand,
                                               TFileName& aDocumentName,
                                               const TDesC8& /*aTail*/ )
    {
    if ( aCommand == EApaCommandOpen )
        {
        OpenDetailsViewNotEmbeddedL( aDocumentName );
        }
    else if ( !iStartEmbedded )
        {
        if ( IsForeground() )
            {
            ActivateLocalViewL( TUid::Uid( EDRMDetailsView ) );
            }
        }

    return EFalse;
    }


// -----------------------------------------------------------------------------
// CDRMRightsMgrAppUi::ProcessMessageL
// -----------------------------------------------------------------------------
//
void CDRMRightsMgrAppUi::ProcessMessageL( TUid /*aUid*/, const TDesC8& aParams )
    {
    HBufC16* buf16 = HBufC16::NewLC( aParams.Length() );
    buf16->Des().Copy( aParams );
    OpenDetailsViewNotEmbeddedL( *buf16 );
    CleanupStack::PopAndDestroy( buf16 );
    }


// -----------------------------------------------------------------------------
// CDRMRightsMgrAppUi::OpenDetailsViewNotEmbeddedL
// -----------------------------------------------------------------------------
//
void CDRMRightsMgrAppUi::OpenDetailsViewNotEmbeddedL( const TDesC& aParams )
    {
    TLex lex( aParams );
    DRMCommonUtilities::ParseParametersAndStartL( lex, EFalse, *this );
    }

// ----------------------------------------------------------------------------
// CDRMRightsMgrAppUi::ProcessEngineErrorL
// ----------------------------------------------------------------------------
//
void CDRMRightsMgrAppUi::ProcessEngineErrorL( TBool aCloseImmediately )
    {
    if ( DRMCommonUtilities::ShowConfirmationQueryL(
                                                R_QTN_DRM_MGR_QUERY_RESTART,
                                                iEikonEnv ) )
        {
        RStarterSession starterSession;
        if ( starterSession.Connect() == KErrNone )
            {
            starterSession.Reset( RStarterSession::EUnknownReset );
            starterSession.Close();
            }
        }

    if ( aCloseImmediately )
        {
        HandleCommandL( EEikCmdExit ); // at least exit, if it doesn't restart
        }
    else
        {
        RunAppShutter();
        }
    }

// -----------------------------------------------------------------------------
// CDrmRightsMgrAppUi::GetRightsManagerL
// -----------------------------------------------------------------------------
//
ContentAccess::CRightsManager* CDRMRightsMgrAppUi::GetRightsManagerL()
    {
    ContentAccess::CManager* manager( NULL );
    ContentAccess::CRightsManager* rightsManager( NULL );
    RArray<ContentAccess::TAgent> agents;
    TInt agent( 0 );

    CleanupClosePushL( agents );
    manager = ContentAccess::CManager::NewLC();

    manager->ListAgentsL( agents );

    for ( agent = 0; agent < agents.Count(); agent++ )
        {
        if (agents[agent].Name().Compare( KWmDrmAgentName ) == 0)
            {
            break;
            }
        }

    // If no WM DRM agent is found, leave
    if ( agent >= agents.Count() )
        {
        User::Leave( KErrNotSupported );
        }


    // Create a CRightsManager for the agent found
    rightsManager = manager->CreateRightsManagerL( agents[agent] );

    CleanupStack::PopAndDestroy( manager );
    CleanupStack::PopAndDestroy( &agents );
    return rightsManager;
    }

// ---------------------------------------------------------
// CDRMRightsMgrAppUi::CheckOmaDrmRightsL
// ---------------------------------------------------------
//
void CDRMRightsMgrAppUi::CheckOmaDrmRightsL( const TDesC8& aContentURI,
                                             const TUint32 aLocalID,
                                             CDRMRights*& aRights,
                                             TInt& aStatus )
    {
    TBool listable( EFalse ), sendable( EFalse );
    TBool individualConstraint( EFalse ), usageAllowed( EFalse );

    CDcfRep* dcfRep = CDcfRep::NewL();
    CleanupStack::PushL( dcfRep );

    if ( dcfRep )
        {
        TRAPD( err, dcfRep->OrderListL( aContentURI ) );
        if ( !err )
            {
            TFileName fullName;

            CDcfEntry* entry = dcfRep->NextL();
            if ( entry )
                {
                fullName = entry->FileName();
                SetSelectedItemFullName( fullName );

                TFileName itemName;

                if ( entry->GroupId().Length() > 0 )
                    {
                    GetItemNameL( fullName, itemName, aContentURI, ETrue );
                    }
                else
                    {
                    GetItemNameL( fullName, itemName, aContentURI, EFalse );
                    }

                delete entry;

                SetSelectedItemName( itemName );

                if ( GetItemDataL( fullName, aContentURI, listable,
                    sendable ) == KErrNone )
                    {
                    SetSelectedContentCanBeSent( sendable );
                    }
                }
            }
        else // No related media was found
            {
            SetSelectedItemFullName( KNullDesC );
            TFileName itemName;
            // Treat in GetItemNameL as if having group rights
            GetItemNameL( KNullDesC, itemName, aContentURI, ETrue );
            SetSelectedItemName( itemName );

            SetSelectedContentCanBeSent( EFalse );
            }
        }
    else
        {
        User::Leave( KErrGeneral );
        }

    if ( aLocalID > 0 )
        {
        aStatus = iDRMCommon->GetSingleRightsObject( aContentURI,
            aLocalID, aRights );
        if ( aStatus )
            {
            aRights = NULL;
            }
        }
    else
        {
        aStatus = iDRMCommon->GetActiveRights( aContentURI, 0, aRights );
        }

    CheckIndividualConstraint( aContentURI, individualConstraint, usageAllowed );
    SetSelectedIndividualConstraint( individualConstraint );
    SetSelectedUsageAllowed( usageAllowed );

    CleanupStack::PopAndDestroy( dcfRep );

    // Do not show the note, show license information in the details view.
    }

// ---------------------------------------------------------
// CDRMRightsMgrAppUi::CheckWmDrmRightsL
// ---------------------------------------------------------
//
void CDRMRightsMgrAppUi::CheckWmDrmRightsL( const TDesC8& aContentURI,
    TInt& aStatus, ContentAccess::RStreamablePtrArray<ContentAccess::CRightsInfo>& aArray )
    {

    ContentAccess::CRightsManager* manager( NULL );
    HBufC* url16( NULL );

    manager = GetRightsManagerL();
    CleanupStack::PushL( manager );

    url16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L( aContentURI );
    CleanupStack::PushL( url16 );

    // Wait note is not shown anymore when checking the rights.
    // Get the list of rights, if leave occurs then there are no rights
    // or the rights are expired.
    TRAP( aStatus, manager->ListRightsL( aArray, url16->Des() ) );

    if( aArray.Count() )
        {
        switch( aArray[0]->RightsStatus() )
            {
            // Rights don't exist
            case ContentAccess::ERightsStatusNone:
            case ContentAccess::ERightsStatusPending:

                // Map the rights status
                aStatus = KErrCANoRights;
                break;

            // Rights exist:
            case ContentAccess::ERightsStatusValid:
            case ContentAccess::ERightsStatusExpired:

                aStatus = KErrNone;
                break;

            default:
                aStatus = KErrCANoRights;
                break;
            }
        }
    else
        {
        aStatus = KErrCANoRights;
        }

    CleanupStack::PopAndDestroy( url16 );
    CleanupStack::PopAndDestroy( manager );
    }

// ---------------------------------------------------------
// CDRMRightsMgrAppUi::CheckIndividualConstraintL
// ---------------------------------------------------------
//
void CDRMRightsMgrAppUi::CheckIndividualConstraint( const TDesC8& aContentURI,
                                                    TBool& aIndividualConstraint,
                                                    TBool& aUsageAllowed )
    {
    RPointerArray<CDRMRights>* uriList = NULL;
    TInt r = KErrNone;
    TUint32 retval(0);
    DRMClock::ESecurityLevel secLevel = DRMClock::KInsecure;
    CDRMRightsConstraints* constraint = NULL;
    RDRMClockClient client;
    RDRMRightsClient rclient;
    TTime time;
    RPointerArray<HBufC8> individuals;

    r = client.Connect();
    if ( r == KErrNone )
        {
        TTime time;
        TInt timeZone(0);
        client.GetSecureTime(time, timeZone, secLevel);
        }

    r = iDRMCommon->GetDetailedContentRights(aContentURI, uriList);
    if ( r )
        {
        uriList=NULL;
        }

    r = KErrNone;

    if ( !uriList || !uriList->Count() )
        {
        // no rights found
        client.Close();
        delete uriList;
        uriList = NULL;
        }
    else
        {
        // supported IMSI information is provided by rights client
        r = rclient.Connect();
        if( r == KErrNone)
            {
            TRAP( r, r = rclient.GetSupportedIndividualsL( individuals ) );
            rclient.Close();
            }

        // Check only the first entry in the list. This is to be expanded to check
        // all the entries in the list.
        for(TInt i = 0; i < 1; ++i)
            {
            r = (*uriList)[i]->GetPlayRight(constraint);
            if ( r != KErrNone )
                {
                r = (*uriList)[i]->GetDisplayRight(constraint);
                }
            if ( r != KErrNone )
                {
                r = (*uriList)[i]->GetExecuteRight(constraint);
                }
            if ( r != KErrNone )
                {
                r = (*uriList)[i]->GetPrintRight(constraint);
                }
            if ( r != KErrNone )
                {
                delete constraint;
                constraint = NULL;
                continue;
                }

            if( constraint->GetConstraint().iActiveConstraints & EConstraintIndividual )
                {
                aIndividualConstraint = ETrue;
                if ( constraint->GetConstraint().Valid( time, individuals, retval ) )
                    {
                    aUsageAllowed = ETrue;
                    }
                }
            delete constraint;
            constraint = NULL;
            }

        client.Close();
        uriList->ResetAndDestroy();

        delete uriList;
        uriList = NULL;
        individuals.ResetAndDestroy();
        individuals.Close();
        }
    }

// End of File
