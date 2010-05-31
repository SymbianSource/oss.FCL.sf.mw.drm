/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation class for WMDRM DLA Browser View
*
*/


// INCLUDE FILES
#include <f32file.h>
#include <bautils.h>
#include <aknViewAppUi.h>
#include <coeaui.h>
#include <coecntrl.h>
#include <brctlinterface.h>
#include <InternetConnectionManager.h>
#include <wmdrmdlaapp.rsg>
#include "wmdrmdlabrowserview.h"
#include "wmdrmdlabrowsercontainer.h"
#include "wmdrmdlaappconstants.h"

// CONTANTS
_LIT( KDataTypeLicenseResponse, "application/vnd.ms-wmdrm.lic-resp" );

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserView::ConstructL()
    {
    BaseConstructL( R_WMDRMDLA_APP_BROWSER_VIEW );
    iConMgr = CInternetConnectionManager::NewL( ETrue );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::CWmDrmDlaBrowserView
// ---------------------------------------------------------------------------
//
CWmDrmDlaBrowserView::CWmDrmDlaBrowserView()
    {
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::NewL
// ---------------------------------------------------------------------------
//
CWmDrmDlaBrowserView* CWmDrmDlaBrowserView::NewL()
    {
    CWmDrmDlaBrowserView* self = CWmDrmDlaBrowserView::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::NewLC
// ---------------------------------------------------------------------------
//
CWmDrmDlaBrowserView* CWmDrmDlaBrowserView::NewLC()
    {
    CWmDrmDlaBrowserView* self = new( ELeave ) CWmDrmDlaBrowserView();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::~CWmDrmDlaBrowserView
// ---------------------------------------------------------------------------
//
CWmDrmDlaBrowserView::~CWmDrmDlaBrowserView()
    {
    RemoveContainer();
    delete iLicenseResponse;
    delete iConMgr;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::SetIAP
// ---------------------------------------------------------------------------
//

void CWmDrmDlaBrowserView::SetIAP( TInt aIap )
    {
    iIap = aIap;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::PostL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserView::PostL(
    MBrowserViewLicenseReceivedCallback* aCallback,
    const TDesC& aPostUrl,
    const TDesC8& aPostContentType,
    const TDesC8& aPostData,
    const TDesC8& aPostContentBoundary )
    {
    iCallback = aCallback;
    iContainer->BrCtlInterface()->PostUrlL( aPostUrl,
                                            aPostContentType,
                                            aPostData,
                                            &aPostContentBoundary,
                                            (TAny*)iContainer );
    iContainer->BrCtlInterface()->SetFocus( ETrue, EDrawNow );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::LicenseResponse
// ---------------------------------------------------------------------------
//

HBufC8* CWmDrmDlaBrowserView::LicenseResponse()
    {
    return iLicenseResponse;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::Id
// ---------------------------------------------------------------------------
//
TUid CWmDrmDlaBrowserView::Id() const
    {
    return KWmDrmDlaAppBrowserViewId;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::HandleCommandL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserView::HandleCommandL(
    TInt aCommand )
    {
    if ( aCommand >= TBrCtlDefs::ECommandIdBase +
                     TBrCtlDefs::ECommandIdWMLBase &&
         aCommand <= TBrCtlDefs::ECommandIdBase +
                     TBrCtlDefs::ECommandIdRange )
        {
        BrCtlHandleCommandL( aCommand );
        }
    else
        {
        iContainer->BrCtlInterface()->HandleCommandL( (TInt)TBrCtlDefs::ECommandCancelFetch + 
                                                      (TInt)TBrCtlDefs::ECommandIdBase );            
        AppUi()->HandleCommandL( aCommand );
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::DoActivateL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserView::DoActivateL(
    const TVwsViewId& /*aPrevViewId*/,
    TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    CreateContainerL();
    AppUi()->AddToStackL( *this, iContainer );
    HandleClientRectChange();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::DoDeactivate
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserView::DoDeactivate()
    {
    RemoveContainer();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::HandleClientRectChange
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::CreateContainerL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserView::CreateContainerL()
    {
    RemoveContainer();
    iContainer = CWmDrmDlaBrowserContainer::NewL( this, this );
    iContainer->SetMopParent( this );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::RemoveContainer
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserView::RemoveContainer()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::BrCtlHandleCommandL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserView::BrCtlHandleCommandL( TInt aCommand )
    {
    iContainer->BrCtlInterface()->HandleCommandL( aCommand );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::NetworkConnectionNeededL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaBrowserView::NetworkConnectionNeededL(
    TInt* aConnectionPtr,
    TInt* aSockSvrHandle,
    TBool* aNewConn,
    TApBearerType* aBearerType )
    {
    TInt ret( 0 );
    // If not connected, try to start a new connection
    if ( !iConMgr->Connected() )
        {
        iConMgr->SetRequestedAP( iIap );
        ret = iConMgr->StartConnectionL( ETrue );
        }

    // If connected, return needed info to framework
    if ( !ret )
        {
        *aConnectionPtr =  reinterpret_cast<TInt>(&iConMgr->Connection() );
        *aSockSvrHandle =  iConMgr->SocketServer().Handle();
        *aNewConn = EFalse;
        *aBearerType = EApBearerTypeAllBearers;
        }
    else
        {
        User::Leave( KErrCancel );
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::HandleRequestL
// ---------------------------------------------------------------------------
//
TBool CWmDrmDlaBrowserView::HandleRequestL(
    RArray<TUint>* /*aTypeArray*/,
    CDesCArrayFlat* /*aDesArray*/ )
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaBrowserView::HandleDownloadL
// ---------------------------------------------------------------------------
//
TBool CWmDrmDlaBrowserView::HandleDownloadL(
    RArray<TUint>* aTypeArray,
    CDesCArrayFlat* aDesArray )
    {
    //Check that content type and local file name exists
    //There must be both, because download manager downloads POST-content
    //in advance calling this function
    TInt contentTypeIndex( aTypeArray->Find( EParamReceivedContentType ) );
    TInt fileNameIndex( aTypeArray->Find( EParamLocalFileName ) );
    if ( contentTypeIndex != KErrNotFound &&
         fileNameIndex != KErrNotFound &&
         contentTypeIndex < aDesArray->Count() &&
         fileNameIndex < aDesArray->Count() )
        {
        //Check that the downloaded content is license response
        TPtrC16 dataTypePtr( (*aDesArray)[contentTypeIndex] );
        if ( dataTypePtr.CompareF( KDataTypeLicenseResponse ) == 0 )
            {
            //Get the path to the license response,
            //read the response, delete the file and
            //make a license received - callback
            TPtrC16 filePathPtr( (*aDesArray)[fileNameIndex] );
            RFs fs;
            User::LeaveIfError( fs.Connect() );
            CleanupClosePushL( fs );
            RFile file;
            TInt size( 0 );
            User::LeaveIfError( file.Open( fs, filePathPtr, EFileRead ) );
            CleanupClosePushL( file );
            User::LeaveIfError( file.Size( size ) );
            delete iLicenseResponse;
            iLicenseResponse = NULL;
            iLicenseResponse = HBufC8::NewL( size );
            TPtr8 licensePtr( iLicenseResponse->Des() );
            User::LeaveIfError( file.Read( 0, licensePtr, size ) );
            CleanupStack::PopAndDestroy( &file );
            User::LeaveIfError( fs.Delete( filePathPtr ) );
            CleanupStack::PopAndDestroy( &fs );
            iCallback->LicenseReceived();
            return ETrue;
            }
        }
    return EFalse;
    }

