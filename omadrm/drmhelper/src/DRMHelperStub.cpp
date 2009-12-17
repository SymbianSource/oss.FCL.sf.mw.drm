/*
* Copyright (c) 2003, 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  stub version of DRMHelper.cpp
*
*/



// INCLUDE FILES
#include    <DRMHelper.h>
#include    <eikenv.h>

#include <schemehandler.h> // for handling URLs
#include <aknnotewrappers.h> // information note


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
// CDRMHelperRightsConstraints::CDRMHelperRightsConstraints
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMHelperRightsConstraints::CDRMHelperRightsConstraints( 
    CDRMRightsConstraints* /*aConstraints*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDRMHelperRightsConstraints::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMHelperRightsConstraints* CDRMHelperRightsConstraints::NewLC( 
    CDRMRightsConstraints* aConstraints )
    {
    CDRMHelperRightsConstraints* self = 
        new (ELeave) CDRMHelperRightsConstraints( aConstraints );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CDRMHelperRightsConstraints* CDRMHelperRightsConstraints::NewL( 
    CDRMRightsConstraints* aConstraints )
    {
    CDRMHelperRightsConstraints* self = NewLC( aConstraints );
    CleanupStack::Pop( self );
    return self;
    }

    
// Destructor
EXPORT_C CDRMHelperRightsConstraints::~CDRMHelperRightsConstraints()
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::FullRights
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMHelperRightsConstraints::FullRights()
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::IsPreview
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMHelperRightsConstraints::IsPreview()
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::GetCountersL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelperRightsConstraints::GetCountersL(
    TUint32& /*aCounter*/, 
    TUint32& /*aOriginalCounter*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::GetStartTimeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelperRightsConstraints::GetStartTimeL( TTime& /*aStartTime*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::GetEndTimeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelperRightsConstraints::GetEndTimeL( TTime& /*aEndTime*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::GetIntervalL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelperRightsConstraints::GetIntervalL( 
    TTimeIntervalSeconds& /*aInterval*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::GetIntervalStartL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelperRightsConstraints::GetIntervalStartL( 
    TTime& /*aStartTime*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::GetTimedCountL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelperRightsConstraints::GetTimedCountL( TUint32& /*aCounter*/,
    TUint32& /*aOriginalCounter*/, TTimeIntervalSeconds& /*aTimer*/ )
    {
  	}


// -----------------------------------------------------------------------------
// CDRMHelperRightsConstraints::GetTimedCountL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelperRightsConstraints::GetAccumulatedTimeL( 
    TTimeIntervalSeconds& /*aAccumulatedTime*/ )
    {
    }



// -----------------------------------------------------------------------------
// CDRMHelper::CDRMHelper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMHelper::CDRMHelper( CCoeEnv& aCoeEnv ) :
    iCoeEnv( &aCoeEnv ),
    iUseCoeEnv( ETrue ), 
    iAutomatedType( EAutomatedTypeOther ),
    iPreviewMediaType(-1)
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelper::CDRMHelper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMHelper::CDRMHelper() :
    iUseCoeEnv( EFalse ),
    iAutomatedType( EAutomatedTypeOther )
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelper::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDRMHelper::ConstructL( RFs* /*aFs*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelper::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMHelper* CDRMHelper::NewLC( CCoeEnv& /*aCoeEnv*/ )
    {
    CDRMHelper* self = new (ELeave) CDRMHelper();
    CleanupStack::PushL( self );
    self->ConstructL( NULL );
    return self;
    }

EXPORT_C CDRMHelper* CDRMHelper::NewL( CCoeEnv& aCoeEnv )
    {
    CDRMHelper* self = NewLC( aCoeEnv );
    CleanupStack::Pop( self );
    return self;
    }
    
EXPORT_C CDRMHelper* CDRMHelper::NewL()
    {
    CDRMHelper* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }
    
EXPORT_C CDRMHelper* CDRMHelper::NewLC()
    {
    CDRMHelper* self = new (ELeave) CDRMHelper();
    CleanupStack::PushL( self );
    self->ConstructL( NULL );
    return self;
    }

EXPORT_C CDRMHelper* CDRMHelper::NewLC( CCoeEnv& aCoeEnv, RFs& aFs )
    {
    CDRMHelper* self = new (ELeave) CDRMHelper( aCoeEnv );
    CleanupStack::PushL( self );
    self->ConstructL( &aFs );
    return self;
    }

EXPORT_C CDRMHelper* CDRMHelper::NewL( CCoeEnv& aCoeEnv, RFs& aFs )
    {
    CDRMHelper* self = NewLC( aCoeEnv, aFs );
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C CDRMHelper* CDRMHelper::NewL( RFs& aFs )
    {
    CDRMHelper* self = NewLC( aFs );
    CleanupStack::Pop( self );
    return self;
    }
    
EXPORT_C CDRMHelper* CDRMHelper::NewLC( RFs& aFs )
    {
    CDRMHelper* self = new (ELeave) CDRMHelper();
    CleanupStack::PushL( self );
    self->ConstructL( &aFs );
    return self;
    }

// Destructor
EXPORT_C CDRMHelper::~CDRMHelper()
    {
    
    }


// -----------------------------------------------------------------------------
// CDRMHelper::HandleErrorL
// -----------------------------------------------------------------------------
//

EXPORT_C TInt CDRMHelper::HandleErrorL( TInt aError, 
    const TDesC8& /*aURI*/ )
    {
    User::LeaveIfError( aError );
    return 0;
    }
    
// -----------------------------------------------------------------------------
// CDRMHelper::HandleErrorL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::HandleErrorL( TInt aError, 
    RFile& /*aFileHandle*/ )
    {
    User::LeaveIfError( aError );
    return 0;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::HandleErrorL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::HandleErrorL( 
    TInt aError, 
    const TDesC& /*aFileName*/ )
    {
    User::LeaveIfError( aError );
    return 0;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::HandleErrorOrPreviewL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::HandleErrorOrPreviewL( 
        TInt aError, 
        RFile& /*aFile*/, 
        HBufC8*& /*aEmbeddedPreviewUri*/ )
    {
    User::LeaveIfError( aError );
    return 0;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::HandleErrorOrPreviewL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::HandleErrorOrPreviewL( 
    TInt aError, 
    const TDesC& /*aFileName*/, 
    HBufC8*& /*aEmbeddedPreviewUri*/ )
    {
    User::LeaveIfError( aError );
    return 0;    
    }
    
// -----------------------------------------------------------------------------
// CDRMHelper::SetPreviewMediaType
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetPreviewMediaType( TDRMHelperPreviewMediaType /*aMediaType*/ )
    {
    return KErrNotSupported;
    }
            
// -----------------------------------------------------------------------------
// CDRMHelper::HasPreviewL
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMHelper::TDRMHelperPreviewType CDRMHelper::HasPreviewL( 
    CData& /*aContent*/, 
    HBufC8*& /*aPreviewUri*/ )
    {
    User::Leave(KErrNotSupported);
    return CDRMHelper::TDRMHelperPreviewType( ENoPreview );
    }


// -----------------------------------------------------------------------------
// CDRMHelper::HasPreviewL
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMHelper::TDRMHelperPreviewType CDRMHelper::HasPreviewL(
    TDesC& /*aFileName*/, 
    HBufC8*& /*aPreviewUri*/ )
    {
    User::Leave(KErrNotSupported);
    return CDRMHelper::TDRMHelperPreviewType( ENoPreview );    
    }


// -----------------------------------------------------------------------------
// CDRMHelper::GetPreviewRightsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::GetPreviewRightsL( CData& /*aContent*/ )
    {
    User::Leave(KErrNotSupported);
    }


// -----------------------------------------------------------------------------
// CDRMHelper::GetPreviewRightsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::GetPreviewRightsL(TDesC& /*aFileName*/ )
    {
    User::Leave(KErrNotSupported);    
    }


// -----------------------------------------------------------------------------
// CDRMHelper::EmbeddedPreviewCompletedL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMHelper::EmbeddedPreviewCompletedL( CData& /*aContent*/ )
    {
    User::Leave(KErrNotSupported);
    return EFalse;    
    }


// -----------------------------------------------------------------------------
// CDRMHelper::EmbeddedPreviewCompletedL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMHelper::EmbeddedPreviewCompletedL( TDesC& /*aFileName*/ )
    {
    User::Leave(KErrNotSupported);
    return EFalse;    
    }

// -----------------------------------------------------------------------------
// CDRMHelper::CheckRightsPercentL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::CheckRightsPercentL( 
    const TDesC& /*aFileName*/, 
    TInt /*aThreshold*/ )
    {
    User::Leave(KErrNotSupported);
    return 0;
    }
    
// -----------------------------------------------------------------------------
// CDRMHelper::CheckRightsPercentL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::CheckRightsPercentL( 
    RFile& /*aFileHandle*/, 
    TInt /*aThreshold*/ )
    {
    User::Leave(KErrNotSupported);
    return 0;      
    }


// -----------------------------------------------------------------------------
// CDRMHelper::CheckRightsAmountL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::CheckRightsAmountL( 
    const TDesC& /*aFileName*/, 
    TInt /*aCount*/,
    TInt /*aDays*/ )
    {
    return 0;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::CheckRightsAmountL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::CheckRightsAmountL( 
    RFile& /*aFileHandle*/, 
    TInt /*aCount*/,
    TInt /*aDays*/ )
    {
    return 0;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::CheckRightsAmountL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::CheckRightsAmountL( 
    const TDesC8& /*aUri*/, 
    TInt /*aCount*/,
    TInt /*aDays*/ )
    {
    return KErrNone;
    }

EXPORT_C void CDRMHelper::SetCountLimitL( TUint /*aCounts*/ )
    {
    }

EXPORT_C void CDRMHelper::SetTimeLimitL( TUint /*aDays*/ )
    {
    }

EXPORT_C void CDRMHelper::SetPercentageLimitL( TUint /*aPercentage*/ )
    {
    }



// -----------------------------------------------------------------------------
// CDRMHelper::LaunchDetailsViewEmbeddedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::LaunchDetailsViewEmbeddedL( 
    const TDesC& /*aFileName*/ )
    {
    User::Leave(KErrNotSupported);    
    }

// -----------------------------------------------------------------------------
// CDRMHelper::LaunchDetailsViewEmbeddedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::LaunchDetailsViewEmbeddedL( 
    const HBufC8* /*aContentURI*/ )
    {
    User::Leave(KErrNotSupported);    
    }
    
// -----------------------------------------------------------------------------
// CDRMHelper::LaunchDetailsViewEmbeddedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::LaunchDetailsViewEmbeddedL( RFile& /*aFileHandle*/ )
    {
    User::Leave(KErrNotSupported);
    }
    
// -----------------------------------------------------------------------------
// CDRMHelper::LaunchDetailsViewEmbeddedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::LaunchDetailsViewEmbeddedL( 
    const TDesC8& /*aURI*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelper::GetRightsDetailsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::GetRightsDetailsL( 
    const TDesC& /*aFileName*/, 
    TUint32 /*aIntent*/,
    TBool& /*aExpired*/, 
    TBool& /*aSendingAllowed*/, 
    CDRMHelperRightsConstraints*& /*aPlay*/,
    CDRMHelperRightsConstraints*& /*aDisplay*/, 
    CDRMHelperRightsConstraints*& /*aExecute*/,
    CDRMHelperRightsConstraints*& /*aPrint*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelper::GetRightsDetailsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::GetRightsDetailsL( 
    RFile& /*aFileHandle*/, 
    TUint32 /*aIntent*/,
    TBool& /*aExpired*/, 
    TBool& /*aSendingAllowed*/, 
    CDRMHelperRightsConstraints*& /*aPlay*/,
    CDRMHelperRightsConstraints*& /*aDisplay*/, 
    CDRMHelperRightsConstraints*& /*aExecute*/,
    CDRMHelperRightsConstraints*& /*aPrint*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelper::GetRightsDetailsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::GetRightsDetailsL( 
    const TDesC& /*aFileName*/, 
    TUint32 /*aIntent*/,
    TBool& /*aExpired*/, 
    TBool& /*aSendingAllowed*/, 
    CDRMRightsConstraints*& /*aPlay*/,
    CDRMRightsConstraints*& /*aDisplay*/, 
    CDRMRightsConstraints*& /*aExecute*/,
    CDRMRightsConstraints*& /*aPrint*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDRMHelper::GetRightsDetailsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::GetRightsDetailsL( 
    RFile& /*aFileHandle*/, 
    TUint32 /*aIntent*/,
    TBool& /*aExpired*/, 
    TBool& /*aSendingAllowed*/, 
    CDRMRightsConstraints*& /*aPlay*/,
    CDRMRightsConstraints*& /*aDisplay*/, 
    CDRMRightsConstraints*& /*aExecute*/,
    CDRMRightsConstraints*& /*aPrint*/ )
    {
    }


EXPORT_C TInt CDRMHelper::ShowDRMUINotification2L( TDRMHelperNotificationID /*aTextId*/,
    const TDesC8& /*aURI*/ )
    {
    return KErrNotSupported;
    }

EXPORT_C TInt CDRMHelper::ShowDRMUINotification2L( TDRMHelperNotificationID /*aTextId*/,
    const TDesC& /*aFileName*/ )
    {
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::CanSetAutomated
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::CanSetAutomated( 
    const TDesC& /*aFilename*/, 
    TBool& aValue )
    {
    aValue = EFalse;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::CanSetAutomated
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::CanSetAutomated( 
    RFile& /*aFileHandle*/, 
    TBool& aValue )
    {
    aValue = EFalse;
    return KErrNone;    
    }


// -----------------------------------------------------------------------------
// CDRMHelper::CanSetAutomated
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::CanSetAutomated( const TDesC8& /*aURI*/, TBool& aValue )
    {
    aValue = EFalse;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::SetAutomated
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetAutomated( const TDesC& /*aFilename*/ )
    {
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::SetAutomated
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetAutomated( RFile& /*aFileHandle*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::SetAutomated
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetAutomated( const TDesC8& /*aURI*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::ShowAutomatedNote
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::ShowAutomatedNote( const TDesC& /*aFilename*/ )
    {
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::ShowAutomatedNote
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::ShowAutomatedNote( RFile& /*aFileHandle*/ )
    {
    return KErrNotSupported;
    }
    
// -----------------------------------------------------------------------------
// CDRMHelper::ShowAutomatedNote
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::ShowAutomatedNote( const TDesC8& /*aURI*/ )
    {
    return KErrNotSupported;
    }        
    
// -----------------------------------------------------------------------------
// CDRMHelper::SetAutomatedPassive
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetAutomatedPassive( const TDesC& /*aFilename*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::SetAutomatedPassive
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetAutomatedPassive( RFile& /*aFileHandle*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::SetAutomatedPassive
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetAutomatedPassive( const TDesC8& /*aURI*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::RemoveAutomated
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::RemoveAutomated( const TDesC& /*aFilename*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::RemoveAutomated
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::RemoveAutomated( RFile& /*aFileHandle*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::RemoveAutomated
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::RemoveAutomated( const TDesC8& /*aURI*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::RemoveAutomatedPassive
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::RemoveAutomatedPassive( const TDesC& /*aFilename*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::RemoveAutomatedPassive
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::RemoveAutomatedPassive( RFile& /*aFileHandle*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::RemoveAutomatedPassive
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::RemoveAutomatedPassive( const TDesC8& /*aURI*/ )
    {
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::SetAutomatedType
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetAutomatedType( 
    CDRMHelper::TDRMHelperAutomatedType /*aAutomatedType*/ )
    {
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CDRMHelper::IndicateIdle
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::IndicateIdle()
    {
    }


// -----------------------------------------------------------------------------
// CDRMHelper::SetAutomatedSilent
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetAutomatedSilent( const TDesC& /*aFilename*/, 
    TBool /*aActive*/ )
    {
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMHelper::SetAutomatedSilent
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetAutomatedSilent( RFile& /*aFileHandle*/, 
    TBool /*aActive*/ )
    {
    return KErrNone;
    }
    

// -----------------------------------------------------------------------------
// CDRMHelper::SetAutomatedSilent
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::SetAutomatedSilent( const TDesC8& /*aURI*/, 
    TBool /*aActive*/ )
    {
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CDRMHelper::Consume2
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::Consume2(
	const TDesC8& /*aContentURI*/,
	TUint32 /*aRightsSpec*/,
	TDRMHelperConsumeAction /*anAction*/)
    {
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::ConsumeFile2
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::ConsumeFile2(
	const TDesC& /*aFileName*/,
	TUint32 /*aRightsSpec*/,
	TDRMHelperConsumeAction /*anAction*/)
    {
    return KErrNotSupported;    
    }

// -----------------------------------------------------------------------------
// CDRMHelper::ConsumeFile2
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMHelper::ConsumeFile2(
	RFile& /*aFileHandle*/,
	TUint32 /*aRightsSpec*/,
	TDRMHelperConsumeAction /*anAction*/)
    {
    return KErrNotSupported;    
    }
// -----------------------------------------------------------------------------
// CDRMHelper::ActivateContentL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::ActivateContentL( CData& /*aContent*/ )
    {
    User::Leave(KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// CDRMHelper::ActivateContentL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::ActivateContentL( TDesC& /*aFileName*/ )
    {
    User::Leave(KErrNotSupported);
    }


// -----------------------------------------------------------------------------
// CDRMHelper::HasInfoUrlL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMHelper::HasInfoUrlL( CData& /*aContent*/, HBufC8*& /*aInfoUrl*/ )
    {
    User::Leave(KErrNotSupported);
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CDRMHelper::HasInfoUrlL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMHelper::HasInfoUrlL(TDesC& /*aFileName*/, HBufC8*& /*aInfoUrl*/ )
    {
    User::Leave(KErrNotSupported);
    return EFalse;    
    }
    
// -----------------------------------------------------------------------------
// CDRMHelper::OpenInfoUrlL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::OpenInfoUrlL( CData& /*aContent*/ )
    {
    User::Leave(KErrNotSupported);
    }
    
// -----------------------------------------------------------------------------
// CDRMHelper::OpenInfoUrlL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMHelper::OpenInfoUrlL(TDesC& /*aFileName*/ )
    {    
    User::Leave(KErrNotSupported);
    }

EXPORT_C TInt CDRMHelper::GetContentURIList(
    RPointerArray<HBufC8>*& /*aURIList*/)
    {
    return KErrNotSupported;
    }
    
EXPORT_C TInt CDRMHelper::DataTypesCount(
    TInt& /*aCount*/)
    {
    return KErrNotSupported;
    }
    
EXPORT_C TInt CDRMHelper::SupportedDataType(
    const TInt /*aIndex*/, TDataType& /*aDataType*/)
    {
    return KErrNotSupported;
    }

EXPORT_C TInt CDRMHelper::RegisterDataType(
    const TDataType& /*aDataType*/)
    {
    return KErrNotSupported;   
    }
    
EXPORT_C TInt CDRMHelper::UnRegisterDataType(
    const TInt /*aIndex*/)
    {
    return KErrNotSupported;   
    }

EXPORT_C TInt CDRMHelper::SupportedDRMMethods2(
    TInt& /*aDRMMethod*/, TDRMHelperOMALevel& /*aOMALevel*/)
    {
    return KErrNone;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CDRMHelper::HandleServerAppExit( TInt aReason )
    {
    if ( aReason == EAknCmdExit && !iSchemeHandler )
        {
        CAknEnv::RunAppShutter();
        }
    
    if ( iSchemeHandler )
        {
        delete iSchemeHandler;
        iSchemeHandler = NULL;
        }

    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }    
    
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
