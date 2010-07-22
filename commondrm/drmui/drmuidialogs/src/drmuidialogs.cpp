/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CDrmUIDialogs class.
*
*/

#include "drmuidialogs.h"                       // CDrmUIDialogs
#include <hb/hbcore/hbsymbianvariant.h>         // CHbSymbianVariantMap

// Identifier of DRM UI device notification dialog plugin
_LIT( KDrmUIDeviceDialogPlugin, "com.nokia.hb.drmuidialog/1.0" );

// Keys for the parameters passed to notification dialog plugin
_LIT( KDrmUIDialogId, "dialogId" );
_LIT( KDrmUIInsertText, "insertText" );
_LIT( KDrmUIInsertInt, "insertInt" );

// Keys name for result sent from notification dialog plugin
_LIT( KDrmUIDialogResult, "result");


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDrmUIDialogs::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CDrmUIDialogs* CDrmUIDialogs::NewLC()
    {
    CDrmUIDialogs* self = new( ELeave ) CDrmUIDialogs();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CDrmUIDialogs::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CDrmUIDialogs* CDrmUIDialogs::NewL()
    {
    CDrmUIDialogs* self = CDrmUIDialogs::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CDrmUIDialogs::~CDrmUIDialogs()
// ---------------------------------------------------------------------------
//
CDrmUIDialogs::~CDrmUIDialogs()
    {
    Cancel();
    delete iWait;
    delete iDeviceDialog;
    delete iVariantMap;
    }

// ---------------------------------------------------------------------------
// CDrmUIDialogs::ShowNoteL()
// ---------------------------------------------------------------------------
EXPORT_C TInt CDrmUIDialogs::ShowNoteL( TInt aDialogId, const TDesC& aString, TInt aValue )
    {
    // Add the dialog id to the variant map
    AddParamL( KDrmUIDialogId, aDialogId );
    
    // Add the string to the variant map if it exists
    if ( aString.Compare( KNullDesC ) )
        {
        AddParamL( KDrmUIInsertText, aString );
        }

    // Add the int to the variant map
    if ( aValue >= 0 )
        {
        AddParamL( KDrmUIInsertInt, aValue );
        }

    DisplayDeviceDialogL();

    TInt error = WaitUntilDeviceDialogClosed();
    User::LeaveIfError( error );

    return iReturnValue;
    }

// ---------------------------------------------------------------------------
// CDrmUIDialogs::DoCancel()
// ---------------------------------------------------------------------------
//
void CDrmUIDialogs::DoCancel()
    {
    if( iWait && iWait->IsStarted() && iWait->CanStopNow() )
        {
        iCompletionCode = KErrCancel;
        iWait->AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// CDrmUIDialogs::RunL()
// ---------------------------------------------------------------------------
//
void CDrmUIDialogs::RunL()
    {
    if( iWait )
        {
        iWait->AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// CDrmUIDialogs::DataReceived()
// ---------------------------------------------------------------------------
//
void CDrmUIDialogs::DataReceived( CHbSymbianVariantMap& aData )
    {
    const CHbSymbianVariant* resultVariant = aData.Get( KDrmUIDialogResult );
 
    if( resultVariant )
        {
        TInt* result = resultVariant->Value<TInt>();

        if( result )
            {
            iReturnValue = *result;
            }
        }
    }

// ---------------------------------------------------------------------------
// CDrmUIDialogs::DeviceDialogClosed()
// ---------------------------------------------------------------------------
//
void CDrmUIDialogs::DeviceDialogClosed( TInt aCompletionCode )
    {
    iCompletionCode = aCompletionCode;
    iIsDisplayingDialog = EFalse;

    TRequestStatus* status( &iStatus );
    User::RequestComplete( status, KErrNone );
    }

// ---------------------------------------------------------------------------
// CDrmUIDialogs::CDrmUIDialogs()
// ---------------------------------------------------------------------------
//
CDrmUIDialogs::CDrmUIDialogs() : CActive( CActive::EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CDrmUIDialogs::ConstructL()
// ---------------------------------------------------------------------------
//
void CDrmUIDialogs::ConstructL()
    {
    iReturnValue = KErrNone;
    iWait = new( ELeave ) CActiveSchedulerWait;
    iDeviceDialog = CHbDeviceDialogSymbian::NewL();
    iVariantMap = CHbSymbianVariantMap::NewL();
    }

// ---------------------------------------------------------------------------
// CDrmUIDialogs::ClearParamsL()
// ---------------------------------------------------------------------------
//
void CDrmUIDialogs::ClearParamsL()
    {
    if( iVariantMap )
        {
        delete iVariantMap;
        iVariantMap = NULL;
        }
    iVariantMap = CHbSymbianVariantMap::NewL();
    }

// ---------------------------------------------------------------------------
// CDrmUIDialogs::ClearParamsAndSetDialogIdL()
// ---------------------------------------------------------------------------
//
void CDrmUIDialogs::ClearParamsAndSetDialogIdL( TInt aDialogId )
    {
    ClearParamsL();
    AddParamL( KDrmUIDialogId, aDialogId );
    }

// ---------------------------------------------------------------------------
// CDrmUIDialogs::AddParamL()
// ---------------------------------------------------------------------------
//
void CDrmUIDialogs::AddParamL( const TDesC& aKey, TInt aValue )
    {
    CHbSymbianVariant* variant = NULL;
    variant = CHbSymbianVariant::NewL( &aValue, CHbSymbianVariant::EInt );
    iVariantMap->Add( aKey, variant );  // Takes ownership of variant
    }

// ---------------------------------------------------------------------------
// CDrmUIDialogs::AddParamL()
// ---------------------------------------------------------------------------
//
void CDrmUIDialogs::AddParamL( const TDesC& aKey, const TDesC& aValue )
    {
    CHbSymbianVariant* variant = NULL;
    variant = CHbSymbianVariant::NewL( &aValue, CHbSymbianVariant::EDes );
    iVariantMap->Add( aKey, variant );  // Takes ownership of variant
    }

// ---------------------------------------------------------------------------
// CDrmUIDialogs::DisplayDeviceDialogL()
// ---------------------------------------------------------------------------
//
void CDrmUIDialogs::DisplayDeviceDialogL()
    {
    if( iIsDisplayingDialog )
        {
        iDeviceDialog->Update( *iVariantMap );
        }
    else
        {
        iDeviceDialog->Show( KDrmUIDeviceDialogPlugin, *iVariantMap, this );
        iIsDisplayingDialog = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CDrmUIDialogs::WaitUntilDeviceDialogClosed()
// ---------------------------------------------------------------------------
//
TInt CDrmUIDialogs::WaitUntilDeviceDialogClosed()
    {
    iCompletionCode = KErrInUse;

    if( !IsActive() && iWait && !iWait->IsStarted() )
        {
        iStatus = KRequestPending;
        SetActive();
        iWait->Start();
        }
    return iCompletionCode;
    }
