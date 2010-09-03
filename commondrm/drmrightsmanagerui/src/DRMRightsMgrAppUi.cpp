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
#include <eikserverapp.h>


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

// ============================= LOCAL FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// DoResetAndDestroy
// Does RPointerArray< >->ResetAndDestroy() for the given array aPtr.
// ----------------------------------------------------------------------------
//
LOCAL_C void DoResetAndDestroyPermission( TAny* aPtr )
    {
    ( reinterpret_cast< RPointerArray< CDRMPermission >* >( aPtr ) )->
        ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// PrepareRightsObjectL
// Creates rights object based on the given permission.
// ----------------------------------------------------------------------------
//
LOCAL_C void PrepareRightsObjectL( CDRMRights*& aRights,
                                   CDRMPermission* aPermission )
    {
    CDRMAsset* asset( NULL );
    
    if ( !aPermission )
        {
        User::Leave( KErrArgument );
        }
    
    aRights = CDRMRights::NewL();
    CleanupStack::PushL( aRights );
    aRights->SetPermissionL( *aPermission );
                         
    asset = CDRMAsset::NewLC();
    aRights->SetAssetL( *asset );
    CleanupStack::PopAndDestroy( asset );
    CleanupStack::Pop( aRights );
                         
    // Content URI and Local Id are not set
    }

// ----------------------------------------------------------------------------
// EndTime
// Calculate the true end time: pick the smaller one of aTime1 & aTime2,
// but ignore Time::NullTTime anyhow.
// ----------------------------------------------------------------------------
//
LOCAL_C TTime EndTime( const TTime& aTime1, const TTime& aTime2 )
    {
    TTime nullTime = Time::NullTTime();

    if ( aTime1 == nullTime )
        {
        return aTime2;
        }

    if ( aTime2 == nullTime )
        {
        return aTime1;
        }

    return Min( aTime1, aTime2 );
    }
       
// ================= MEMBER FUNCTIONS ==========================================
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
    User::LeaveIfError( iClockClient.Connect() );
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
    
    iClockClient.Close();

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

    if ( aKeyEvent.iScanCode ==  EStdKeyEnter )
        // Default is to show details
        {
        TInt command = EAknSoftkeyClose;
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
    CEikAppServer* server = iEikonEnv->AppServer();
 
    switch ( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            {
            Exit();
            break;
            }
        case EAknSoftkeyClose:
            {

            if (server)
                {
                server->NotifyServerExit(EAknSoftkeyClose);
                }
                
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
        TRAP_IGNORE( StartOnlyForDetailsL( iContentURI->Des(), iLocalID, 
            iStartEmbedded, iDrmScheme ) );
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
    TInt err( KErrNone );
    

    aStatus = KErrNone;
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
        CheckIndividualConstraint( aContentURI, individualConstraint, usageAllowed );
        SetSelectedIndividualConstraint( individualConstraint );
        SetSelectedUsageAllowed( usageAllowed );
        }
    else
        {
        SetSelectedIndividualConstraint( EFalse );
        SetSelectedUsageAllowed( ETrue );
        
        // Get active rights if available
        err = iDRMCommon->GetActiveRights( aContentURI, 0, aRights );
        
        if ( err < 0 )
            {
            // Find out the best composition of rights that should be shown in the
            // details view. Checks also individual constraint.
            TRAP( aStatus, FindBestCompositionRightsL( aContentURI, aRights ) );
            }
        }
    
    if ( aStatus )
        {
        aRights = NULL;
        }

    CleanupStack::PopAndDestroy( dcfRep );
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
    
    TTime time;
    RPointerArray<HBufC8> individuals;
    
    RPointerArray<CDRMRights>* uriList = NULL;
    TInt r = KErrNone;
    TUint32 retval(0);
    TInt timeZone(0);
    DRMClock::ESecurityLevel secLevel = DRMClock::KInsecure;
    CDRMRightsConstraints* constraint = NULL;
    
    iClockClient.GetSecureTime(time, timeZone, secLevel);

    r = iDRMCommon->GetDetailedContentRights(aContentURI, uriList);
    if ( r )
        {
        uriList=NULL;
        }

    r = KErrNone;

    if ( !uriList || !uriList->Count() )
        {
        // no rights found
        delete uriList;
        uriList = NULL;
        }
    else
        {
        // supported IMSI information is provided by rights client
        TRAP( r, r = iRightsClient.GetSupportedIndividualsL( individuals ) );

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

        uriList->ResetAndDestroy();

        delete uriList;
        uriList = NULL;
        individuals.ResetAndDestroy();
        individuals.Close();
        }
    }

// ---------------------------------------------------------
// CDRMRightsMgrAppUi::FindBestCompositionRightsL
// ---------------------------------------------------------
//
void CDRMRightsMgrAppUi::FindBestCompositionRightsL( const TDesC8& aContentURI,
                                                     CDRMRights*& aRights )
    {
    TInt i, j, k;
    RPointerArray<CDRMPermission> permissionList;
    RPointerArray<CDRMPermission> permissionTempList;
    RPointerArray<HBufC8> parentUidList;
    RPointerArray<HBufC8> individuals;
    
    TInt count( 0 );
    TInt status( KErrNone );
    TTime time( Time::NullTTime() );
    TInt err( KErrNone );
    TUint32 reason( EConstraintNone );
    TBool individualConstraintFound( EFalse );
    
    // Composition object of best rights to be shown in the 
    // details view
    CDRMPermission* perm( NULL ); 
    
    TCleanupItem cleanupPerm( DoResetAndDestroyPermission, &permissionList );
    CleanupStack::PushL( cleanupPerm );
    
    TCleanupItem cleanupPerm2( DoResetAndDestroyPermission, 
            &permissionTempList );
    CleanupStack::PushL( cleanupPerm2 );
    
    CleanupClosePushL( parentUidList );
    
    // First get list of child ROs with the given Content URI
    iRightsClient.GetDBEntriesL( aContentURI, permissionList );            
    
    if ( permissionList.Count() > 0 )
        {        
        // Check the UIDs of the possible parent rights objects
        // and store the found ones to another pointer array for evaluation
        for ( i = 0; i < permissionList.Count(); i++ )
            {
            if ( permissionList[i]->iParentUID )
                {
                if ( i == 0 )
                    {
                    // No need to check if the parent UID is a duplicate
                    parentUidList.AppendL ( permissionList[i]->iParentUID );
                    }
                else 
                    {
                    // Check parent UID list whether we have already stored the
                    // UID or not
                    for( j = 0; j < parentUidList.Count(); j++ )
                        {
                        if ( parentUidList[j]->Des().Compare(
                                permissionList[i]->iParentUID->Des() ) != 0 )
                            {
                            parentUidList.Append( 
                                    permissionList[i]->iParentUID );
                            break;
                            }
                        }
                    }
                }
            }
        
        // Get the list of parent rights objects based on the parent UID 
        // and store the pointers from temporary array to the main list 
        for ( j = 0; j < parentUidList.Count(); j++ )
            {
            // We do not want to leave if nothing is found from the database
            TRAP( status, iRightsClient.GetDBEntriesL( parentUidList[j]->Des(),
                    permissionTempList ) );
            
            if ( status )
                {
                continue;
                }
                    
            for ( k = 0; k < permissionTempList.Count(); k++ ) 
                {
                // Store the pointer to the main list of permissions
                permissionList.AppendL ( permissionTempList[k] );
                }    
                    
            // Close the temporary pointer array so that the referenced 
            // objects of it will not be deleted in the next round of the 
            // loop because the method for getting database entries deletes 
            // also the objects (by calling reset and destroy to the given 
            // pointer array) by default.
            permissionTempList.Close();
            }
        
        parentUidList.Close();
        time.HomeTime();
        
        // supported IMSI information is provided by rights client
        TRAP( err, err = iRightsClient.GetSupportedIndividualsL( individuals ) );
        
        // Filter expired parent and child rights from the list
        // Index defines the index of the list to be operated (next)
        for ( count = permissionList.Count() ; count > 0; count-- )
            {
            if ( permissionList[count - 1]->Expired( time ) ) 
                {
                permissionList.Remove( count - 1 );
                }
            else if ( !( permissionList[count - 1]->Valid( 
                    time, individuals, reason ) ) )
                {
                if ( reason & EConstraintIndividual )
                    {
                    individualConstraintFound = ETrue;
                    permissionList.Remove( count - 1 );
                    }
                }
            }        
        
        individuals.ResetAndDestroy();
        individuals.Close();
        
        if ( permissionList.Count() == 0 )
            {
            if ( individualConstraintFound )
                {
                SetSelectedIndividualConstraint( ETrue );
                SetSelectedUsageAllowed( EFalse );
                }
            User::Leave( KErrNotFound );
            }
        else if ( permissionList.Count() == 1  )
            {
            // Keep this permission because it is the only valid (left)
            PrepareRightsObjectL( aRights, permissionList[0] );
            }
        else if ( permissionList.Count() > 1 )
            {    
            // Compare constraints from successive items of the 
            // permission list and update the composition permission 
            // accordingly. 
            CheckBetterPermissionsAndStoreCompositionLC( perm, permissionList );
                
            // Prepare the rights object with the composition permission
            PrepareRightsObjectL( aRights, perm );
            CleanupStack::PopAndDestroy( perm );   
            }     
        }
        
        CleanupStack::PopAndDestroy( 3, &permissionList ); // cleanupPerm, 
                                                           // cleanupPerm2, 
                                                           // parentUidList
    }

// -------------------------------------------------------------------
// CDRMRightsMgrAppUi::CheckBetterPermissionsAndStoreCompositionLC
// -------------------------------------------------------------------
//
void CDRMRightsMgrAppUi::CheckBetterPermissionsAndStoreCompositionLC( 
            CDRMPermission*& aCompositionPermission,
            RPointerArray<CDRMPermission>& aList )
    {       
    
    TBool firstBetter( EFalse );
    TUint i, j;
    
    if ( aList.Count() <= 0 )
        {
        User::Leave( KErrArgument );
        }
    
    // create permission if needed and set the first item
    // in the permission list as the reference permission.
    if ( !aCompositionPermission )
        {
        aCompositionPermission = CDRMPermission::NewL();
        }
         
    CleanupStack::PushL( aCompositionPermission );
    aCompositionPermission->DuplicateL( *aList[0] );
    
    // The double loop goes through the permissions and associated 
    // constraints beginning from the second entry in the permission list. 
    for ( i = 1; i < aList.Count(); i++ )
        {
        // Enumeration for TIntent from Content Access Framework used
        for ( j = EPlay; j < EPause; j++ )
            { 
            switch ( j )
                {
                case EPlay:
                    {
                    // The constraint from the permission in the list
                    // is set to composition permission if it is better
                    // than the "old" best or reference permission.
                    firstBetter = BetterPermission( *aList[i]->iPlay,
                            *aCompositionPermission->iPlay );
                    if ( firstBetter )
                        {
                        aCompositionPermission->iPlay->DuplicateL( 
                                *aList[i]->iPlay );
                        }
                    break;
                    }
                case EView:
                    {
                    firstBetter = BetterPermission( *aList[i]->iDisplay, 
                            *aCompositionPermission->iDisplay );
                    if ( firstBetter )
                        {
                        aCompositionPermission->iDisplay->DuplicateL( 
                                *aList[i]->iDisplay );
                        }
                    break;
                    }
                case EExecute:
                    {
                    firstBetter = BetterPermission( *aList[i]->iExecute,
                            *aCompositionPermission->iExecute );
                    if ( firstBetter )
                        {
                        aCompositionPermission->iExecute->DuplicateL(
                                *aList[i]->iExecute );
                        }
                    break;
                    }
                case EPrint:
                    {
                    firstBetter = BetterPermission( *aList[i]->iPrint,
                            *aCompositionPermission->iPrint );
                    if ( firstBetter )
                        {
                        aCompositionPermission->iPrint->DuplicateL( 
                                *aList[i]->iPrint );
                        }
                    break;
                    }
                default:
                    {
                    User::Leave( KErrArgument );
                    }
                }
            }
        }        
    }                     

// -----------------------------------------------------------------------------
// CDrmRightsMgrAppUi::BetterPermission
// -----------------------------------------------------------------------------
TBool CDRMRightsMgrAppUi::BetterPermission( const CDRMConstraint& aNewOne,
                                            const CDRMConstraint& aOldOne )
        {
        // Check Order:
        // 1. Full
        // 2. Start End, closest end time first
        // 3. Interval, shortest first
        // 4. Accumulated, shortest first
        // 5. Timed Counter, least counters first, longest time first
        // 6. Counter, least counters first or the first one found

        const TTime nullTime = Time::NullTTime();
        TTime oldTime = nullTime;
        TTime newTime = nullTime;
        TTime oldTimePos = nullTime;
        TTime newTimePos = nullTime;
        TTime trustedTime = nullTime;
        
        // 1. Full
        // If the old or new one is the ultimate one, don't bother to
        // check anything else.
        if ( aOldOne.iActiveConstraints == EConstraintNone )
            {
            return EFalse;
            }

        if ( aNewOne.iActiveConstraints == EConstraintNone )
            {
            return ETrue;
            }

        // 2. Start & End Time
        // Choose the one with the closest end time first
        // All RO's to this check are already checked to be valid
        // ActiveIntervals Also hit this spot
        
        trustedTime.HomeTime();
            
        // First get the start and end times from the intervals if they are active or inactive:
        if ( aOldOne.iActiveConstraints & EConstraintInterval )
            {
            if( aOldOne.iIntervalStart == nullTime )
                {
                oldTimePos = trustedTime;
                oldTimePos += TTimeIntervalSeconds( aOldOne.iInterval );
                }
            else
                {
                oldTime = aOldOne.iIntervalStart;
                oldTime += TTimeIntervalSeconds( aOldOne.iInterval );
                }
            }

        if( aNewOne.iActiveConstraints & EConstraintInterval )
            {
            if( aNewOne.iIntervalStart == nullTime )
                {
                newTimePos = trustedTime;
                newTimePos += TTimeIntervalSeconds( aNewOne.iInterval );
                }
            else
                {
                newTime = aNewOne.iIntervalStart;
                newTime += TTimeIntervalSeconds( aNewOne.iInterval );
                }
            }

        if ( aOldOne.iActiveConstraints & EConstraintEndTime || oldTime != nullTime )
            {
            oldTime = EndTime( oldTime, aOldOne.iEndTime );

            if ( aNewOne.iActiveConstraints & EConstraintEndTime || newTime != nullTime )
                {
                newTime = EndTime( newTime, aNewOne.iEndTime );

                if( newTime != oldTime )
                    {
                    return ( newTime < oldTime );
                    }
                }
            else
                {
                return EFalse;
                }
            }
        else if ( aNewOne.iActiveConstraints & EConstraintEndTime || newTime != nullTime )
            {
            return ETrue;
            }

        // 3. Inactive Intervals:
        // Choose the one with the interval ending first:
        // Continue here if the no SE's exist or SE's are the same
        if( aOldOne.iActiveConstraints & EConstraintInterval )
            {
            if( aNewOne.iActiveConstraints & EConstraintInterval )
                {
                oldTimePos = EndTime( oldTime, oldTimePos );
                newTimePos = EndTime( newTime, newTimePos );

                if( oldTimePos != newTimePos )
                    {
                    return ( newTimePos < oldTimePos );
                    }
                }
            else
                {
                if( aNewOne.iActiveConstraints & EConstraintAccumulated ||
                    aNewOne.iActiveConstraints & EConstraintTimedCounter ||
                    aNewOne.iActiveConstraints & EConstraintCounter )
                    {
                    return EFalse;
                    }
                else
                    {
                    return ETrue;
                    }
                }
            }
        else if( aNewOne.iActiveConstraints & EConstraintInterval )
            {
            if( aOldOne.iActiveConstraints & EConstraintAccumulated ||
                aOldOne.iActiveConstraints & EConstraintTimedCounter ||
                aOldOne.iActiveConstraints & EConstraintCounter )
                {
                return ETrue;
                }
            else
                {
                return EFalse;
                }
            }

        // 4. Accumulated:
        // Choose the shortest accumulated first
        // Continue here if SE's or intervals do not exist or they are the same
        if( aOldOne.iActiveConstraints & EConstraintAccumulated )
            {
            if( aNewOne.iActiveConstraints & EConstraintAccumulated )
                {
                if( aNewOne.iAccumulatedTime != aOldOne.iAccumulatedTime )
                    {
                    return ( aNewOne.iAccumulatedTime < aOldOne.iAccumulatedTime );
                    }
                }
            else
                {
                if( aNewOne.iActiveConstraints & EConstraintTimedCounter ||
                    aNewOne.iActiveConstraints & EConstraintCounter )
                    {
                    return EFalse;
                    }
                else
                    {
                    return ETrue;
                    }
                }
            }
        else if( aNewOne.iActiveConstraints & EConstraintAccumulated )
            {
            if( aOldOne.iActiveConstraints & EConstraintTimedCounter ||
                aOldOne.iActiveConstraints & EConstraintCounter )
                {
                return ETrue;
                }
            else
                {
                return EFalse;
                }
            }


        // 5. Timed Counter
        // Choose the one with least counters first. If there is an equal number of counters
        // left, use the one with the longest time
        // Continue here if SE's or intervals or accumulateds do not exist or they are the same
        if( aOldOne.iActiveConstraints & EConstraintTimedCounter )
            {
            if( aNewOne.iActiveConstraints & EConstraintTimedCounter )
                {
                if( aNewOne.iTimedCounter == aOldOne.iTimedCounter )
                    {
                    if( aNewOne.iTimedInterval != aOldOne.iTimedInterval )
                        {
                        return ( aNewOne.iTimedInterval < aOldOne.iTimedInterval );
                        }
                    else
                        {
                        if( aNewOne.iActiveConstraints & EConstraintCounter )
                            {

                            }
                        }
                    }
                else
                    {
                    return ( aNewOne.iTimedCounter < aOldOne.iTimedCounter );
                    }

                }
            else
                {
                if( aNewOne.iActiveConstraints & EConstraintCounter )
                    {
                    return EFalse;
                    }
                else
                    {
                    return ETrue;
                    }
                }
            }
        else if( aNewOne.iActiveConstraints & EConstraintTimedCounter )
            {
            if( aOldOne.iActiveConstraints & EConstraintCounter )
                {
                return ETrue;
                }
            else
                {
                return EFalse;
                }
            }

        // 6. Counter
        // Choose the one with least counters:
        // if they are the same choose the first one.
        // Continue here if SE's or intervals or accumulateds or timed counters
        // do not exist or they are the same
        if( aOldOne.iActiveConstraints & EConstraintCounter )
            {
            if( aNewOne.iActiveConstraints & EConstraintCounter )
                {
                return ( aNewOne.iCounter < aOldOne.iCounter );
                }
            else
                {
                return ETrue;
                }
            }

        // If all else fails use the old one:
        return EFalse;
        }

// End of File
