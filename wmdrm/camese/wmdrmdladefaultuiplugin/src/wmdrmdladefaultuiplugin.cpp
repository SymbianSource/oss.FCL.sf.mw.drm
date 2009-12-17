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
* Description:  WMDRM DLA Default UI Plugin
*
*/


#include <wmdrmdlautils.rsg>
#include <wmdrmdlatypes.h>
#include <wmdrmdlacancelobserver.h>
#include "wmdrmdlaui.h"
#include "wmdrmdladefaultuiplugin.h"

#define _LOGGING_FILE L"wmdrmdladefaultuiplugin.txt"
#include "logfn.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultUiPlugin::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultUiPlugin::ConstructL()
    {
    LOGFN( "CWmDrmDlaDefaultUiPlugin::ConstructL" );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultUiPlugin::CWmDrmDlaDefaultUiPlugin
// ---------------------------------------------------------------------------
//
CWmDrmDlaDefaultUiPlugin::CWmDrmDlaDefaultUiPlugin()
    {
    LOGFN( "CWmDrmDlaDefaultUiPlugin::CWmDrmDlaDefaultUiPlugin" );
    TRAP_IGNORE( iUi = CWmDrmDlaUi::NewL() );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultUiPlugin::NewL
// ---------------------------------------------------------------------------
//
CWmDrmDlaDefaultUiPlugin* CWmDrmDlaDefaultUiPlugin::NewL()
    {
    LOGFN( "CWmDrmDlaDefaultUiPlugin::NewL" );
    CWmDrmDlaDefaultUiPlugin* self = new( ELeave ) CWmDrmDlaDefaultUiPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultUiPlugin::~CWmDrmDlaDefaultUiPlugin
// ---------------------------------------------------------------------------
//
CWmDrmDlaDefaultUiPlugin::~CWmDrmDlaDefaultUiPlugin()
    {
    LOGFN( "CWmDrmDlaDefaultUiPlugin::~CWmDrmDlaDefaultUiPlugin" );
    delete iUi;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultUiPlugin::SetCancelObserver
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultUiPlugin::SetCancelObserver( 
    MWmDrmDlaCancelObserver* aObserver )
    {
    LOGFN( "CWmDrmDlaDefaultUiPlugin::SetCancelObserver" );
    iObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultUiPlugin::SetSilent
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultUiPlugin::SetSilent( 
    TBool /*aSilent*/ )
    {
    LOGFN( "CWmDrmDlaDefaultUiPlugin::SetSilent" );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultUiPlugin::SetProgress
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultUiPlugin::SetProgress( 
    TInt aState )
    {
    TInt err( KErrNone );
    
    LOGFNR( "CWmDrmDlaDefaultUiPlugin::SetProgress", err );

    switch( aState )
        {
        case EProcessingDRMHeader:
            if ( iUi )
                {
                TRAP( err, 
                    iUi->ShowWaitNoteL( R_WMDRMDLA_WAIT_NOTE_ACQUIRING_LICENSE,
                                        this ) );
                }
            break;

        case EProcessingMeteringChallenge:
            if ( iUi )
                {
                TRAP( err, 
                    iUi->ShowWaitNoteL( R_WMDRMDLA_WAIT_NOTE_METERING, 
                                        this ) );
                }
            break;

        case EIdle:
            if ( iUi )
                {
                iUi->RemoveWaitNote();
                }
            break;

        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultUiPlugin::HandleErrorL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultUiPlugin::HandleErrorL( 
    TInt /*aError*/ )
    {
    LOGFN( "CWmDrmDlaDefaultUiPlugin::HandleErrorL" );
    if ( iUi )
        {
        iUi->RemoveWaitNote();
        }
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultUiPlugin::UserWaitNoteCancellation
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultUiPlugin::UserWaitNoteCancellation()
    {
    LOGFN( "CWmDrmDlaDefaultUiPlugin::UserWaitNoteCancellation" );
    if ( iObserver )
        {
        iObserver->UserCancellation();
        }
    }
