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
* Description:  Implementation class for WMDRM DLA UI
*
*/


// INCLUDE FILES
#include <f32file.h>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include <eikfrlb.h>
#include <eikfrlbd.h>
#include <coemain.h>
#include <bautils.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <AknGlobalNote.h>
#include <aknlistquerydialog.h>
#include <badesca.h>
#include <gulicon.h>

#include <data_caging_path_literals.hrh>
#include <wmdrmdlautils.rsg>

#include "wmdrmdlaui.h"

#define _LOGGING_FILE L"wmdrmdlautils.txt"
#include "logfn.h"

// CONSTANTS
#ifndef RD_MULTIPLE_DRIVE
_LIT( KDriveZ, "z:" );
#else
_LIT( KRomDriveFormatter, "%c:" );
#endif

_LIT( KWmdrmDlaUtilsResFileName, "wmdrmdlautils.rsc" );

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWmDrmDlaUi::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaUi::ConstructL()
    {
    LOGFN( "CWmDrmDlaUi::ConstructL" );
    iCoeEnv = CCoeEnv::Static();
    if ( !iCoeEnv )
        {
        User::Leave( KErrNotSupported );
        }

    //Create resource file path
    TPtr wmdrmDlaUtilsResourcePtr( NULL, 0 );
    iWmdrmDlaUtilsResourceFile = HBufC::NewL( KMaxFileName );
    wmdrmDlaUtilsResourcePtr.Set( iWmdrmDlaUtilsResourceFile->Des() );

    //Append rom drive letter to the path
#ifndef RD_MULTIPLE_DRIVE

    wmdrmDlaUtilsResourcePtr.Append( KDriveZ );

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    TChar driveLetter;
    //Get rom drive number
    User::LeaveIfError(
            DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom,
                                        driveNumber ) );
    //Get drive letter from the drive number
    User::LeaveIfError(
            iCoeEnv->FsSession().DriveToChar( driveNumber,
                                              driveLetter ) );
    wmdrmDlaUtilsResourcePtr.AppendFormat( KRomDriveFormatter,
                                           (TUint)driveLetter );

#endif

    //Append directory to the path
    wmdrmDlaUtilsResourcePtr.Append( KDC_RESOURCE_FILES_DIR );
    //Append name of the resource file to the path
    wmdrmDlaUtilsResourcePtr.Append( KWmdrmDlaUtilsResFileName );

    TFileName resourceFile;
    resourceFile = *iWmdrmDlaUtilsResourceFile;
    //Find the correct language file for the resource
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(),
                                    resourceFile );
    //Add resource file to the CCoeEnv.
    //Resource file must be removed from the CCoeEnv when we are done
    iWmdrmDlaUtilsResourceFileOffset
        = iCoeEnv->AddResourceFileL( resourceFile );

    iGlobalNote = CAknGlobalNote::NewL();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUi::CWmDrmDlaUi
// ---------------------------------------------------------------------------
//
CWmDrmDlaUi::CWmDrmDlaUi() : CActive( EPriorityStandard )
    {
    LOGFN( "CWmDrmDlaUi::CWmDrmDlaUi" );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUi::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CWmDrmDlaUi* CWmDrmDlaUi::NewL()
    {
    LOGFN( "CWmDrmDlaUi::NewL" );
    CWmDrmDlaUi* self = CWmDrmDlaUi::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaUi::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CWmDrmDlaUi* CWmDrmDlaUi::NewLC()
    {
    LOGFN( "CWmDrmDlaUi::NewLC" );
    CWmDrmDlaUi* self = new( ELeave ) CWmDrmDlaUi;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaUi::~CWmDrmDlaUi
// ---------------------------------------------------------------------------
//
CWmDrmDlaUi::~CWmDrmDlaUi()
    {
    LOGFN( "CWmDrmDlaUi::~CWmDrmDlaUi" );
    Cancel();
    delete iGlobalNote;
    delete iGlobalNoteText;
    iCoeEnv->DeleteResourceFile( iWmdrmDlaUtilsResourceFileOffset );
    delete iWmdrmDlaUtilsResourceFile;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUi::ShowErrorNoteL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWmDrmDlaUi::ShowErrorNoteL(
    TInt aTextResourceId )
    {
    LOGFN( "CWmDrmDlaUi::ShowErrorNoteL" );
    HBufC* msgText( StringLoader::LoadLC( aTextResourceId, iCoeEnv ) );
    CAknResourceNoteDialog* note = new ( ELeave ) CAknErrorNote( ETrue );
    note->ExecuteLD( *msgText );
    CleanupStack::PopAndDestroy( msgText );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUi::ShowWaitNoteL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWmDrmDlaUi::ShowWaitNoteL(
    TInt aTextResourceId,
    MWmDrmDlaWaitNoteCallback* aCallback )
    {
    LOGFN( "CWmDrmDlaUi::ShowWaitNoteL" );
    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }
    iGlobalNoteText = StringLoader::LoadL( aTextResourceId, iCoeEnv );
    iGlobalNoteId = iGlobalNote->ShowNoteL( iStatus,
                                            EAknGlobalWaitNote,
                                            *iGlobalNoteText );
    iCallback = aCallback;
    SetActive();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUi::RemoveWaitNote
// ---------------------------------------------------------------------------
//
EXPORT_C void CWmDrmDlaUi::RemoveWaitNote()
    {
    LOGFN( "CWmDrmDlaUi::RemoveWaitNote" );
    Cancel();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUi::ShowListQueryL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CWmDrmDlaUi::ShowListQueryL(
    MDesCArray* aItems,
    CArrayPtr<CGulIcon>* aIcons )
    {
    LOGFN( "CWmDrmDlaUi::ShowListQueryL" );

    //This method takes the ownership of the input parameters
    //Push the input parameters to the cleanupstack to make sure we won't leak
    //memory
    CleanupStack::PushL( aIcons );
    CleanupStack::PushL( aItems );

    //Check that both inputs really exist.
    if ( !aItems || !aIcons )
        {
        User::Leave( KErrArgument );
        }

    //Create the query with index parameter
    //Query will take the ownership of the items and icons
    TInt index( 0 );
    CAknListQueryDialog* dlg = new (ELeave) CAknListQueryDialog( &index );
    dlg->PrepareLC( R_WMDRMDLA_LIST_QUERY );

    //Arrange cleanupstack in a way that query can take the ownership of the
    //items and icons
    CleanupStack::Pop( 2, aItems ); //dlg, aItems
    //Ownership of the items is transferred to query
    dlg->SetItemTextArray( aItems );
    dlg->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::PushL( dlg );
    //Ownership of the icons in transferred to query
    dlg->SetIconArrayL( aIcons );
    CleanupStack::Pop( 2, aIcons ); //dlg, aIcons
    CleanupStack::PushL( dlg );

    //Enable marquee scrolling for the text items
    CEikFormattedCellListBox* listbox(
            static_cast<CEikFormattedCellListBox*>( dlg->ListBox() ) /* ,
            EAknListBoxItemSpecificMenuDisabled */ );
    listbox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );

    //Launch the query
    TInt key( dlg->RunLD() );
    LOG2( "key: %d", key );
    //If user cancelled query, then return -1
    //Else return index of the selected service
    if ( key != EAknSoftkeyOk )
        {
        index = -1;
        }
    LOG2( "index: %d", index );
    return index;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUi::DoCancel
// ---------------------------------------------------------------------------
//
void CWmDrmDlaUi::DoCancel()
    {
    LOGFN( "CWmDrmDlaUi::DoCancel" );
    CancelWaitNote();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUi::RunL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaUi::RunL()
    {
    LOGFN( "CWmDrmDlaUi::RunL" );
    CancelWaitNote();
    if ( iCallback )
        {
        iCallback->UserWaitNoteCancellation();
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaUi::RunError
// ---------------------------------------------------------------------------
//
TInt CWmDrmDlaUi::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaUi::CancelWaitNote
// ---------------------------------------------------------------------------
//
void CWmDrmDlaUi::CancelWaitNote()
    {
    LOGFN( "CWmDrmDlaUi::CancelWaitNote" );
    if ( iGlobalNoteId )
        {
        TRAP_IGNORE( iGlobalNote->CancelNoteL( iGlobalNoteId ) );
        iGlobalNoteId = 0;
        delete iGlobalNoteText;
        iGlobalNoteText = NULL;
        }
    }
