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
* Description:  Implementation of details view class
*
*/


// INCLUDE FILES
#include <coeutils.h>
#include <avkon.rsg>
#include <akntitle.h>
#include <aknview.h>
#include <aknViewAppUi.h>
#include <StringLoader.h>
#include <eikbtgpc.h>

#include <DRMRightsManager.rsg>

#include "DRMRightsMgrDetailsView.h"
#include "DRMCommonUtilities.h"
#include "DRMRightsMgrAppUi.h"
#include "DRMRightsManager.hrh"
#include "drmutilitywmdrmutilities.h"
#include "drmutilityinternaltypes.h"

#ifdef _DEBUG
_LIT(KDRMUIDoActivateError, "DoActivateL");
#endif

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================
//
// -----------------------------------------------------------------------------
// CDRMRightsMgrDetailsView::CDRMRightsMgrDetailsView
// -----------------------------------------------------------------------------
//
CDRMRightsMgrDetailsView::CDRMRightsMgrDetailsView( TBool aStartEmbedded )
: iContainer( NULL ), iViewId( TUid::Uid( EDRMDetailsView ) ),
  iStartEmbedded( aStartEmbedded )
    {
    }


// ---------------------------------------------------------------------------
// CDRMRightsMgrDetailsView::NewL
// ---------------------------------------------------------------------------
//
CDRMRightsMgrDetailsView* CDRMRightsMgrDetailsView::NewL(
                                                        TBool aStartEmbedded )
    {
    CDRMRightsMgrDetailsView* self = NewLC( aStartEmbedded );

    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CDRMRightsMgrDetailsView::NewLC
// ---------------------------------------------------------------------------
//
CDRMRightsMgrDetailsView* CDRMRightsMgrDetailsView::NewLC(
                                                        TBool aStartEmbedded )
    {
    CDRMRightsMgrDetailsView* self =
        new( ELeave ) CDRMRightsMgrDetailsView( aStartEmbedded );
    CleanupStack::PushL( self );
    self->ConstructL( 0 );

    return self;
    }


// ---------------------------------------------------------
// CDRMRightsMgrDetailsView::ConstructL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsView::ConstructL( const TInt /*aResourceId*/ )
    {
    BaseConstructL( R_DRMRIGHTSMANAGER_DETAILSVIEW );
    iDetails.iRights = new( ELeave )CArrayFixFlat<TRights>( 1 );
    }


// ---------------------------------------------------------
// CDRMRightsMgrDetailsView::~CDRMRightsMgrDetailsView
// ---------------------------------------------------------
//
CDRMRightsMgrDetailsView::~CDRMRightsMgrDetailsView()
    {
    delete iDetails.iRights;
    }

// ---------------------------------------------------------------------------
// CDRMRightsMgrDetailsView::CreateContainerL()
// ---------------------------------------------------------------------------
//
void CDRMRightsMgrDetailsView::CreateContainerL()
    {
    NewContainerL();
    __ASSERT_DEBUG( iContainer, User::Panic( KDRMUIDoActivateError,
                                             EDRMUIViewPanicNullPtr ) );
    iContainer->SetMopParent( this );

    TRAPD( error, iContainer->ConstructL( ClientRect() ) );

    if ( error )
        {
        delete iContainer;
        iContainer = NULL;
        User::Leave( error );
        }
    }

// ---------------------------------------------------------------------------
// CDRMRightsMgrDetailsView::NewContainerL
// ---------------------------------------------------------------------------
//
void CDRMRightsMgrDetailsView::NewContainerL()
    {
    iContainer = new( ELeave ) CDRMRightsMgrDetailsContainer( iDetails,
                                                              iStartEmbedded );
    }

// ---------------------------------------------------------------------------
// CDRMRightsMgrDetailsView::RemoveContainer()
// ---------------------------------------------------------------------------
void CDRMRightsMgrDetailsView::RemoveContainer()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CDRMRightsMgrDetailsView::SetTitlePaneL
// ----------------------------------------------------------------------------
//
void CDRMRightsMgrDetailsView::SetTitlePaneL( const TDesC& aTitle )
    {
    // Fetches pointer to the default title pane control.
    CAknTitlePane* titlePane = static_cast<CAknTitlePane*>( StatusPane()->
                            ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    if ( titlePane )
        {
        // Set title text
        titlePane->SetTextL( aTitle );
        }
    }

// ---------------------------------------------------------
// CDRMRightsMgrDetailsView::Id()
// ---------------------------------------------------------
//
TUid CDRMRightsMgrDetailsView::Id() const
    {
    return iViewId;
    }

// ---------------------------------------------------------
// void CDRMRightsMgrDetailsView::HandleCommandL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsView::HandleCommandL( TInt aCommand )
    {

        CDRMRightsMgrAppUi& appUi =
                                *static_cast<CDRMRightsMgrAppUi*>( AppUi() );

    if ( aCommand == EAknSoftkeyOk )
        {
            appUi.HandleCommandL( EAknSoftkeyClose ); // Ok softkey was pressed
        }
    else
        {
        appUi.HandleCommandL( aCommand );        // Any other case
        }
    }

// ---------------------------------------------------------
// CDRMRightsMgrDetailsView::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsView::DynInitMenuPaneL( TInt /*aResourceId*/,
                                     CEikMenuPane* /*aMenuPane*/ )
    {
    }

// ---------------------------------------------------------
// CDRMRightsMgrDetailsView::HandleClientRectChange
// ---------------------------------------------------------
//
void CDRMRightsMgrDetailsView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------------------------
// CDRMRightsMgrDetailsView::DoActivateL
// ---------------------------------------------------------------------------
//
void CDRMRightsMgrDetailsView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                                            TUid /*aCustomMessageId*/,
                                            const TDesC8& /*aCustomMessage*/ )
    {
    RemoveContainer();
    CreateContainerL();
    AppUi()->AddToStackL( *this, iContainer );

    if ( iStartEmbedded )
        {
        MakeLeftSoftkeyVisible( EFalse );
        }

    HandleClientRectChange();

    }


// ---------------------------------------------------------------------------
// CDRMRightsMgrDetailsView::DoDeactivate
// ---------------------------------------------------------------------------
//
void CDRMRightsMgrDetailsView::DoDeactivate()
    {

    DeleteConstraints();
    RemoveContainer();
    }

// ----------------------------------------------------------------------------
// CDRMRightsMgrDetailsView::RefreshListBoxL
// ----------------------------------------------------------------------------
//
void CDRMRightsMgrDetailsView::RefreshListBoxL( CDRMRights* aRights,
                                                const TDesC& aTitle,
                                                const TDesC& aFullName,
                                                const TBool aContentCanBeSent,
                                                const TBool aIndividualConstraint,
                                                const TBool aUsageAllowed )
    {
    CDRMRights::TExpiration expir;
    TInt err = KErrNone;

    TRights rights;

    PrepareRefreshL( aFullName, EDrmSchemeOmaDrm, rights, aContentCanBeSent,
        aIndividualConstraint, aUsageAllowed );

    if ( aRights )
        {
        err = DRMCommonUtilities::GetRightsStatus( *aRights, expir );

        if ( err != KErrCANoRights &&
             err != KErrCANoPermission )
            {
            if ( expir == CDRMRights::EExpiredRights )
                {
                rights.iStatus = EDRMUIStatusExpired;
                }
            else if ( expir == CDRMRights::EFutureRights )
                {
                rights.iStatus = EDRMUIStatusFuture;
                }
            else
                {
                rights.iStatus = EDRMUIStatusValid;
                }

            aRights->GetPlayRight( rights.iPlayConstraint );

            aRights->GetDisplayRight( rights.iDisplayConstraint );

            aRights->GetPrintRight( rights.iPrintConstraint );

            aRights->GetExecuteRight( rights.iExecuteConstraint );
            }

        }

    FinalizeRefreshL( aTitle, rights );

    }

// ----------------------------------------------------------------------------
// CDRMRightsMgrDetailsView::RefreshListBoxL
// ----------------------------------------------------------------------------
//
void CDRMRightsMgrDetailsView::RefreshListBoxL(
    ContentAccess::RStreamablePtrArray<ContentAccess::CRightsInfo>& aRights,
    const TDesC& aTitle,
    const TDesC& aFullName )
    {

    TRights rights;

    PrepareRefreshL( aFullName, EDrmSchemeWmDrm, rights, ETrue, EFalse, ETrue );

    if ( aRights.Count() > 0 )
        {
        // Map pending or no rights to expired for the UI as default.
        switch ( aRights[0]->RightsStatus() )
            {
            case ContentAccess::ERightsStatusNone:
                rights.iStatus = EDRMUIStatusExpired;
                break;
            case ContentAccess::ERightsStatusPending:
                rights.iStatus = EDRMUIStatusExpired;
                break;
            case ContentAccess::ERightsStatusValid:
                rights.iStatus = EDRMUIStatusValid;
                break;
            case ContentAccess::ERightsStatusExpired:
                rights.iStatus = EDRMUIStatusExpired;
                break;
            default:
                rights.iStatus = EDRMUIStatusExpired;
                break;
            }

        DrmUtilityWmDrmUtilities::ParseWmDrmStringL( *aRights[0],
            rights.iPlayConstraint );
        }
    else
        {
        // WMDRM agent returned an empty array of rights.
        // The rights can be missing or expired -> show
        // expired in the UI as default.
        rights.iStatus = EDRMUIStatusExpired;
        }

    FinalizeRefreshL( aTitle, rights );

    }

// ----------------------------------------------------------------------------
// CDRMRightsMgrDetailsView::MakeLeftSoftkeyVisible
// ----------------------------------------------------------------------------
//
void CDRMRightsMgrDetailsView::MakeLeftSoftkeyVisible( TBool aVisible )
    {
    CEikButtonGroupContainer* btnGroupContainer = Cba();
    if ( btnGroupContainer )
        {
        TInt commandID = btnGroupContainer->ButtonGroup()->CommandId( 0 );
        TBool isVisible = btnGroupContainer->IsCommandVisible( commandID );

        if ( isVisible != aVisible )
            {
            btnGroupContainer->MakeCommandVisible( commandID, aVisible );
            }
        }
    }

// ----------------------------------------------------------------------------
// CDRMRightsMgrDetailsView::DeleteConstraints
// ----------------------------------------------------------------------------
//
void CDRMRightsMgrDetailsView::DeleteConstraints()
    {
    for ( TInt i = 0; i < iDetails.iRights->Count(); i++ )
        {
        if ( iDetails.iRights->At( i ).iPlayConstraint )
            {
            delete iDetails.iRights->At( i ).iPlayConstraint;
            iDetails.iRights->At( i ).iPlayConstraint = NULL;
            }

        if ( iDetails.iRights->At( i ).iDisplayConstraint )
            {
            delete iDetails.iRights->At( i ).iDisplayConstraint;
            iDetails.iRights->At( i ).iDisplayConstraint = NULL;
            }

        if ( iDetails.iRights->At( i ).iPrintConstraint )
            {
            delete iDetails.iRights->At( i ).iPrintConstraint;
            iDetails.iRights->At( i ).iPrintConstraint = NULL;
            }

        if ( iDetails.iRights->At( i ).iExecuteConstraint )
            {
            delete iDetails.iRights->At( i ).iExecuteConstraint;
            iDetails.iRights->At( i ).iExecuteConstraint = NULL;
            }
        }
    }

// ----------------------------------------------------------------------------
// CDRMRightsMgrDetailsView::PrepareRefreshL
// ----------------------------------------------------------------------------
//
void CDRMRightsMgrDetailsView::PrepareRefreshL( const TDesC& aFullName,
                                                const TInt&  aDrmScheme,
                                                TRights& aRights,
                                                const TBool& aContentCanBeSent,
                                                const TBool& aIndividualConstraint,
                                                const TBool& aUsageAllowed )
    {

    MakeLeftSoftkeyVisible( ETrue );

    iDetails.iContentPath.Zero();

    if ( DRMCommonUtilities::IsInPrivateFolderL( aFullName ) )
        {
        // not able to check further
        iDetails.iContentPath = aFullName;
        }
    else
        {
        if ( aFullName.Length() && ConeUtils::FileExists( aFullName ) )
            {
            iDetails.iContentPath = aFullName;
            }
        }

    iDetails.iContentCanBeSent = aContentCanBeSent;
    iDetails.iIndividualConstraint = aIndividualConstraint;
    iDetails.iUsageAllowed = aUsageAllowed;
    iDetails.iActiveROindex = 0;
    iDetails.iRights->Reset();
    iDetails.iDrmScheme = aDrmScheme;

    aRights.iPlayConstraint = NULL;
    aRights.iDisplayConstraint = NULL;
    aRights.iPrintConstraint = NULL;
    aRights.iExecuteConstraint = NULL;
    aRights.iStatus = EDRMUIStatusInvalid;

    }

// ----------------------------------------------------------------------------
// CDRMRightsMgrDetailsView::FinalizeRefreshL
// ----------------------------------------------------------------------------
//
void CDRMRightsMgrDetailsView::FinalizeRefreshL( const TDesC& aTitle,
    const TRights& aRights )
    {

    iDetails.iRights->AppendL( aRights );

    SetTitlePaneL( aTitle );

    if ( iContainer )
        {
        CDRMRightsMgrDetailsContainer& container =
                    *static_cast<CDRMRightsMgrDetailsContainer*>( iContainer );
        container.UpdateListBoxContentL( iDetails );
        }
    }

// End of File
