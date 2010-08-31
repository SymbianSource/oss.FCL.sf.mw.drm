/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CDrmUtilityGlobalNoteWrapper class
*
*/



// INCLUDE FILES
#include <StringLoader.h>
#include <AknGlobalConfirmationQuery.h>
#include <AknGlobalListQuery.h>
#include <badesca.h>
#include <avkon.hrh> // EAknSoftkeyNo
#include <drmutility.rsg>

#include <AknMediatorFacade.h>
#include <aknSDData.h>
#include <featmgr.h>

#include "drmutilitysecondarydisplay.h"
#include "DrmUtilityGlobalNoteWrapper.h"
#include "DrmUtilityInfoNoteWrapper.h"

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDrmUtilityGlobalNoteWrapper::CDrmUtilityGlobalNoteWrapper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
DRM::CDrmUtilityGlobalNoteWrapper::CDrmUtilityGlobalNoteWrapper()
    : CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CDrmUtilityGlobalNoteWrapper::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityGlobalNoteWrapper::ConstructL(
    CStringResourceReader* aResourceReader )
    {
    iResourceReader = aResourceReader;
    iButtonsId = R_AVKON_SOFTKEYS_YES_NO__YES;
    }

// -----------------------------------------------------------------------------
// CDrmUtilityGlobalNoteWrapper::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
DRM::CDrmUtilityGlobalNoteWrapper* DRM::CDrmUtilityGlobalNoteWrapper::NewLC(
    CStringResourceReader* aResourceReader )
    {
    DRM::CDrmUtilityGlobalNoteWrapper* self(
                                new (ELeave) CDrmUtilityGlobalNoteWrapper() );
    CleanupStack::PushL( self );
    self->ConstructL( aResourceReader );
    return self;
    }

DRM::CDrmUtilityGlobalNoteWrapper* DRM::CDrmUtilityGlobalNoteWrapper::NewL(
    CStringResourceReader* aResourceReader )
    {
    DRM::CDrmUtilityGlobalNoteWrapper* self( NewLC( aResourceReader ) );
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
DRM::CDrmUtilityGlobalNoteWrapper::~CDrmUtilityGlobalNoteWrapper()
    {
    Cancel(); // Cancel active object
    }

// -----------------------------------------------------------------------------
// CDrmUtilityGlobalNoteWrapper::ShowNoteL
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmUtilityGlobalNoteWrapper::ShowNoteWithButtonsL(
    TInt aResourceId,
    TInt aButtonsId )
    {
    TInt ret( 0 );

    iTextBuffer = iResourceReader->ReadResourceString( aResourceId );

    iButtonsId = aButtonsId;
    ret = DoShowNoteL( aResourceId );
    iButtonsId = R_AVKON_SOFTKEYS_YES_NO__YES;
    return ret;
    }

// -----------------------------------------------------------------------------
// CDrmUtilityGlobalNoteWrapper::ShowNoteWithButtonsL
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmUtilityGlobalNoteWrapper::ShowNoteWithButtonsL(
    TInt aResourceId,
    TInt aButtonsId,
    const TDesC& aString )
    {
    TInt ret( 0 );

    TBuf<DRM::KDRMNoteBufferMaxSize> srcBuffer(
        iResourceReader->ReadResourceString( aResourceId ) );

    StringLoader::Format( iTextBuffer, srcBuffer, -1, aString );

    iButtonsId = aButtonsId;
    ret = DoShowNoteL( aResourceId, aString );
    iButtonsId = R_AVKON_SOFTKEYS_YES_NO__YES;
    return ret;
    }

// -----------------------------------------------------------------------------
// CDrmUtilityGlobalNoteWrapper::ShowNoteL
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmUtilityGlobalNoteWrapper::ShowNoteL(
    TInt aResourceId,
    TInt aValue )
    {
    TBuf<DRM::KDRMNoteBufferMaxSize> srcBuffer(
        iResourceReader->ReadResourceString( aResourceId ) );

    StringLoader::Format( iTextBuffer, srcBuffer, -1, aValue );
    return DoShowNoteL( aResourceId, KNullDesC, aValue );
    }

// -----------------------------------------------------------------------------
// CDrmUtilityGlobalNoteWrapper::ShowNoteL
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmUtilityGlobalNoteWrapper::ShowNoteL(
    TInt aResourceId,
    const TDesC& aString )
    {
    TBuf<DRM::KDRMNoteBufferMaxSize> srcBuffer(
        iResourceReader->ReadResourceString( aResourceId ) );

    StringLoader::Format( iTextBuffer, srcBuffer, -1, aString );
    return DoShowNoteL( aResourceId, aString );
    }

// -----------------------------------------------------------------------------
// CDrmUtilityGlobalNoteWrapper::ShowNoteL
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmUtilityGlobalNoteWrapper::ShowNoteL(
    TInt aResourceId,
    const TDesC& aString,
    TInt aValue,
    TInt aStringPos,
    TInt aValuePos)
    {
    TBuf<DRM::KDRMNoteBufferMaxSize> srcBuffer(
        iResourceReader->ReadResourceString( aResourceId ) );

    StringLoader::Format( iTextBuffer, srcBuffer, aValuePos, aValue );
    srcBuffer = iTextBuffer;
    StringLoader::Format( iTextBuffer, srcBuffer, aStringPos, aString );
    return DoShowNoteL( aResourceId, aString, aValue );
    }

// -----------------------------------------------------------------------------
// CDrmUtilityGlobalNoteWrapper::ShowPreviewListQueryL
// -----------------------------------------------------------------------------
//
#ifdef RD_DRM_PREVIEW_RIGHT_FOR_AUDIO

TInt DRM::CDrmUtilityGlobalNoteWrapper::ShowPreviewListQueryL(
    TInt aResourceId )
    {
    TInt index( 0 );
    CAknGlobalListQuery* listQuery( CAknGlobalListQuery::NewLC() );
    HBufC* buffer( HBufC::NewLC( DRM::KDRMNoteBufferMaxSize ) );
    TPtr bufPtr( buffer->Des() );

    bufPtr = iResourceReader->ReadResourceString( R_DRMUTILITY_ACTIVATE_PREVIEW );
    listQuery->SetHeadingL( bufPtr );

    CDesCArray* listArray( new( ELeave ) CDesCArrayFlat( 2 ) );
    CleanupStack::PushL( listArray );

    bufPtr = iResourceReader->ReadResourceString( R_DRMUTILITY_ACTIVATE );
    listArray->AppendL( bufPtr );

    switch( aResourceId )
        {
        case R_DRMUTILITY_PREV_AUDIO_GET_LIST_QUERY:

            bufPtr = iResourceReader->ReadResourceString(
                                            R_DRMUTILITY_GET_PREVIEW );


            break;

        case R_DRMUTILITY_PREV_VIDEO_GET_LIST_QUERY:

            bufPtr = iResourceReader->ReadResourceString(
                                            R_DRMUTILITY_GET_PREVIEW_VIDEO );

            break;

        case R_DRMUTILITY_PREV_AUDIO_PLAY_LIST_QUERY:

            bufPtr = iResourceReader->ReadResourceString(
                                            R_DRMUTILITY_PLAY_PREVIEW );

            break;

        case R_DRMUTILITY_PREV_VIDEO_PLAY_LIST_QUERY:

            bufPtr = iResourceReader->ReadResourceString(
                                            R_DRMUTILITY_PLAY_PREVIEW_VIDEO );

            break;

        default:

            return 0;

        }

    listArray->AppendL( bufPtr );

    if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) &&
        DRM::CDrmUtilityInfoNoteWrapper::EvaluateCoverResourceId( aResourceId ) )
        {
        RProcess myProcess;
        TUid myProcessUid( KNullUid );
        RThread().Process( myProcess );
        myProcessUid = myProcess.Identity();

        TUtilitySDData utilityData;
        // First field is DrmUtility's Uid
        utilityData.iUtilityUid = KUidCoverUiCategoryDrmUtility;
        // ProcessId which uses DrmUtility
        utilityData.iHandlerProcessId = myProcessUid;
        TUtilitySDDataPckg pckg( utilityData );
        CAknSDData* sd( CAknSDData::NewL( KUidCoverUiCategoryDrmUtility,
                                          aResourceId,
                                          pckg ) );

        // ownership to notifier client
        listQuery->SetSecondaryDisplayData( sd );
        }

    iStatus = KRequestPending;
    listQuery->ShowListQueryL( listArray, iStatus );
    SetActive();
    iWait.Start();

    CleanupStack::PopAndDestroy( 3, listQuery ); //listArray, buffer, listQuery

    if ( iStatus.Int() != EAknSoftkeyNo )
        {
        index = iStatus.Int() + 1;
        }

    return index;
    }

#else

TInt DRM::CDrmUtilityGlobalNoteWrapper::ShowPreviewListQueryL(
    TInt /*aResourceId*/ )
    {
    return 0;
    }

#endif // RD_DRM_PREVIEW_RIGHT_FOR_AUDIO

// -----------------------------------------------------------------------------
// CDrmUtilityGlobalNoteWrapper::DoCancel
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityGlobalNoteWrapper::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CDrmUtilityGlobalNoteWrapper::RunL
// -----------------------------------------------------------------------------
//
void DRM::CDrmUtilityGlobalNoteWrapper::RunL()
    {
    iWait.AsyncStop();
    }

// -----------------------------------------------------------------------------
// CDrmUtilityGlobalNoteWrapper::DoShowNoteL
// -----------------------------------------------------------------------------
//
TInt DRM::CDrmUtilityGlobalNoteWrapper::DoShowNoteL(
    TInt aResourceId,
    const TDesC& aString,
    TInt aValue )
    {
    TPtr bufPtr( NULL, 0 );
    TInt animation( iButtonsId == R_AVKON_SOFTKEYS_YES_NO__YES ?
                                                    0 : R_QGN_NOTE_INFO_ANIM );

    CAknGlobalConfirmationQuery* globalNote(
                                        CAknGlobalConfirmationQuery::NewLC() );

    bufPtr.Set( const_cast <TUint16*>( iTextBuffer.Ptr() ),
                                       iTextBuffer.Length(),
                                       iTextBuffer.Length() );

    AknTextUtils::LanguageSpecificNumberConversion( bufPtr );


    if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) &&
        DRM::CDrmUtilityInfoNoteWrapper::EvaluateCoverResourceId( aResourceId ) )
        {
        RProcess myProcess;
        TUid myProcessUid( KNullUid );
        RThread().Process( myProcess );
        myProcessUid = myProcess.Identity();

        TUtilitySDData utilityData;
        // First field is DrmUtility's Uid
        utilityData.iUtilityUid = KUidCoverUiCategoryDrmUtility;
        // ProcessId which uses DrmUtility
        utilityData.iHandlerProcessId = myProcessUid;
        if ( aString.Compare( KNullDesC ) )
            {
            // If there is filename given, it's always in the PrimaryString
            utilityData.iStringParam.Append( aString );
            }
        if ( aValue >= 0 )
            {
            // If there is no other than numeric data, put it as SecondaryString
            utilityData.iNumParam.AppendNum( aValue );
            }

        TUtilitySDDataPckg pckg( utilityData );
        CAknSDData* sd( CAknSDData::NewL( KUidCoverUiCategoryDrmUtility,
                                          aResourceId,
                                          pckg ) );

        // ownership to notifier client
        globalNote->SetSecondaryDisplayData( sd );
        }

    iStatus = KRequestPending;
    globalNote->ShowConfirmationQueryL( iStatus,
                                        iTextBuffer,
                                        iButtonsId,
                                        animation );

    SetActive();
    iWait.Start();
    CleanupStack::PopAndDestroy( globalNote );
    if ( iStatus.Int() != EAknSoftkeyNo )
        {
        return iStatus.Int();
        }
    else
        {
        return 0;
        }
    }

//  End of File
