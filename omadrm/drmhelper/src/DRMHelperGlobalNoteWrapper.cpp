/*
* Copyright (c) 2004 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CDRMHelperGlobalNoteWrapper class
*
*/



// INCLUDE FILES
#include "DRMHelperGlobalNoteWrapper.h"
#include "DRMHelperInfoNoteWrapper.h"

#include <StringLoader.h>
#include <AknGlobalConfirmationQuery.h>
#include <avkon.hrh> // EAknSoftkeyNo

#include <SecondaryDisplay/DRMHelperSecondaryDisplay.h>
#include <AknMediatorFacade.h>
#include <aknSDData.h>
#include <featmgr.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMHelperGlobalNoteWrapper::CDRMHelperGlobalNoteWrapper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMHelperGlobalNoteWrapper::CDRMHelperGlobalNoteWrapper() : CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CDRMHelperGlobalNoteWrapper::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDRMHelperGlobalNoteWrapper::ConstructL(
    CStringResourceReader* aResourceReader )
    {
    iResourceReader = aResourceReader;
    iButtonsId = R_AVKON_SOFTKEYS_YES_NO__YES;
    }

// -----------------------------------------------------------------------------
// CDRMHelperGlobalNoteWrapper::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMHelperGlobalNoteWrapper* CDRMHelperGlobalNoteWrapper::NewLC(
    CStringResourceReader* aResourceReader )
    {
    CDRMHelperGlobalNoteWrapper* self = new (ELeave) CDRMHelperGlobalNoteWrapper();
    CleanupStack::PushL( self );
    self->ConstructL( aResourceReader );
    return self;
    }

CDRMHelperGlobalNoteWrapper* CDRMHelperGlobalNoteWrapper::NewL(
    CStringResourceReader* aResourceReader )
    {
    CDRMHelperGlobalNoteWrapper* self = NewLC( aResourceReader );
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
CDRMHelperGlobalNoteWrapper::~CDRMHelperGlobalNoteWrapper()
    {
    Cancel(); // Cancel active object
    }

// -----------------------------------------------------------------------------
// CDRMHelperGlobalNoteWrapper::ShowNoteL
// -----------------------------------------------------------------------------
//
TInt CDRMHelperGlobalNoteWrapper::ShowNoteWithButtonsL(
    TInt aResourceId,
    TInt aButtonsId )
    {
    TInt ret;

    iTextBuffer = iResourceReader->ReadResourceString( aResourceId );

    iButtonsId = aButtonsId;
    ret = DoShowNoteL( aResourceId );
    iButtonsId = R_AVKON_SOFTKEYS_YES_NO__YES;
    return ret;
    }

// -----------------------------------------------------------------------------
// CDRMHelperGlobalNoteWrapper::ShowNoteWithButtonsL
// -----------------------------------------------------------------------------
//
TInt CDRMHelperGlobalNoteWrapper::ShowNoteWithButtonsL(
    TInt aResourceId,
    TInt aButtonsId,
    const TDesC& aString )
    {
    TInt ret;

    TBuf<KDRMNoteBufferMaxSize> srcBuffer =
        iResourceReader->ReadResourceString( aResourceId );

    StringLoader::Format( iTextBuffer, srcBuffer, -1, aString );

    iButtonsId = aButtonsId;
    ret = DoShowNoteL( aResourceId, aString );
    iButtonsId = R_AVKON_SOFTKEYS_YES_NO__YES;
    return ret;
    }

// -----------------------------------------------------------------------------
// CDRMHelperGlobalNoteWrapper::ShowNoteL
// -----------------------------------------------------------------------------
//
TInt CDRMHelperGlobalNoteWrapper::ShowNoteL( TInt aResourceId, TInt aValue )
    {
    TBuf<KDRMNoteBufferMaxSize> srcBuffer =
        iResourceReader->ReadResourceString( aResourceId );

    StringLoader::Format( iTextBuffer, srcBuffer, -1, aValue );
    return DoShowNoteL( aResourceId, KNullDesC, aValue );
    }

// -----------------------------------------------------------------------------
// CDRMHelperGlobalNoteWrapper::ShowNoteL
// -----------------------------------------------------------------------------
//
TInt CDRMHelperGlobalNoteWrapper::ShowNoteL(
    TInt aResourceId,
    const TDesC& aString )
    {
    TBuf<KDRMNoteBufferMaxSize> srcBuffer =
        iResourceReader->ReadResourceString( aResourceId );

    StringLoader::Format( iTextBuffer, srcBuffer, -1, aString );
    return DoShowNoteL( aResourceId, aString );
    }

// -----------------------------------------------------------------------------
// CDRMHelperGlobalNoteWrapper::ShowNoteL
// -----------------------------------------------------------------------------
//
TInt CDRMHelperGlobalNoteWrapper::ShowNoteL(
    TInt aResourceId,
    const TDesC& aString,
    TInt aValue,
    TInt aStringPos,
    TInt aValuePos)
    {
    TBuf<KDRMNoteBufferMaxSize> srcBuffer =
        iResourceReader->ReadResourceString( aResourceId );

    StringLoader::Format( iTextBuffer, srcBuffer, aValuePos, aValue );
    srcBuffer = iTextBuffer;
    StringLoader::Format( iTextBuffer, srcBuffer, aStringPos, aString );
    return DoShowNoteL( aResourceId, aString, aValue );
    }

// -----------------------------------------------------------------------------
// CDRMHelperGlobalNoteWrapper::DoCancel
// -----------------------------------------------------------------------------
//
void CDRMHelperGlobalNoteWrapper::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelperGlobalNoteWrapper::RunL
// -----------------------------------------------------------------------------
//
void CDRMHelperGlobalNoteWrapper::RunL()
    {
    iWait.AsyncStop();
    }

// -----------------------------------------------------------------------------
// CDRMHelperGlobalNoteWrapper::DoShowNoteL
// -----------------------------------------------------------------------------
//
TInt CDRMHelperGlobalNoteWrapper::DoShowNoteL( TInt aResourceId,
    const TDesC& aString, TInt aValue)
    {
    TPtr bufPtr( 0,0 );

    RProcess myProcess;
    TUid myProcessUid( KNullUid );

    TInt animation =
        iButtonsId == R_AVKON_SOFTKEYS_YES_NO__YES ? 0 : R_QGN_NOTE_INFO_ANIM;
    CAknGlobalConfirmationQuery* globalNote =
        CAknGlobalConfirmationQuery::NewLC();

    bufPtr.Set( const_cast <TUint16*>( iTextBuffer.Ptr() ), iTextBuffer.Length(),
        iTextBuffer.Length() );
    AknTextUtils::LanguageSpecificNumberConversion( bufPtr );

    if( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) &&
        CDRMHelperInfoNoteWrapper::EvaluateCoverResourceId( aResourceId ) )
        {
        RThread().Process( myProcess );
        myProcessUid = myProcess.Identity();

        THelperSDData helperData;
        helperData.iHelperUid = KUidCoverUiCategoryDRMHelper; // First field is DRMHelper's Uid
        helperData.iHandlerProcessId = myProcessUid; // ProcessId which uses DRMHelper
        if( aString.Compare( KNullDesC ) )
            {
            // If there is filename given, it's always in the PrimaryString
            helperData.iStringParam.Append( aString );
            }
        if( aValue >= 0 )
            {
            // If there is no other than numeric data, put it as SecondaryString
            helperData.iNumParam.AppendNum( aValue );
            }

        THelperSDDataPckg pckg( helperData );
        CAknSDData* sd = CAknSDData::NewL( KUidCoverUiCategoryDRMHelper, aResourceId, pckg );
        globalNote->SetSecondaryDisplayData( sd ); // ownership to notifier client
        }

    iStatus = KRequestPending;
    globalNote->ShowConfirmationQueryL( iStatus, iTextBuffer, iButtonsId,
        animation );
    SetActive();
    iWait.Start();
    CleanupStack::PopAndDestroy( globalNote );
    return MappedButtonId( iStatus.Int() );
    }

// -----------------------------------------------------------------------------
// CDRMHelperGlobalNoteWrapper::MappedButtonId
// -----------------------------------------------------------------------------
//
TInt CDRMHelperGlobalNoteWrapper::MappedButtonId( TInt aButtonId ) const
    {
    switch ( aButtonId )
        {
        // Do not test for case EEikBidCancel;
        // it is defined to be the same as EAknSoftkeyCancel

        case EAknSoftkeyCancel:
        case EAknSoftkeyBack:
        case EAknSoftkeyNo:
            return 0;

        default:
            break;
        }

    // Also process user range:

    if ( aButtonId >= EAknSoftkeyLowestUserRejectId
        && aButtonId < EAknSoftkeyLowestUserAcceptId )
        return 0;

    return aButtonId;
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
