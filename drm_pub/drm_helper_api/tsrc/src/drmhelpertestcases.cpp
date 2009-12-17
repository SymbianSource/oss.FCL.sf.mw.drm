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
* Description:  DrmHelperTest hard coded test cases
*
*/



// INCLUDE FILES
#include <f32file.h>
#include <coemain.h>
#include <apmstd.h> //TDataType
#include <drmhelper.h>
#include <drmrightsclient.h>
#include <drmrights.h>
#include <dcfrep.h>
#include <dcfrepcli.h>
#include <caf/caferr.h>
#include <caf/data.h>
#include <caf/virtualpathptr.h>
#include "drmrightsparser.h"
#include "drmhelpertest.h"
#include "drmhelpertestdata.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
_LIT( KPassed, "Passed" );

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// DoResetAndDestroyRightsArray
// Does RPointerArray< >->ResetAndDestroy() for the given array aPtr.
// -----------------------------------------------------------------------------
//
LOCAL_C void DoResetAndDestroyRightsArray( TAny* aPtr )
    {
    ( reinterpret_cast< RPointerArray< CDRMRights >* >( aPtr ) )->
        ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// DeleteRODBL
// Helper function with which test cases can delete RO DB
// -----------------------------------------------------------------------------
//
LOCAL_C void DeleteRODBL()
    {
    RDRMRightsClient client;
    User::LeaveIfError( client.Connect() );
    client.DeleteAll();
    client.Close();
    }

// -----------------------------------------------------------------------------
// AddROL
// Helper function with which test cases can add ROs
// -----------------------------------------------------------------------------
//
LOCAL_C void AddROL(
    const TDesC8& aFormatter,
    const TDesC8& aCID,
    const TDesC8& aCEK,
    const TDesC8& aPermissions )
    {
    TInt roSize(
        aFormatter.Size() + aCID.Size() + aCEK.Size() + aPermissions.Size() );
    HBufC8* buf( HBufC8::NewLC( roSize ) );
    TPtr8 tmp( buf->Des() );

    tmp.AppendFormat( aFormatter, &aCID, &aCEK, &aPermissions );

    RPointerArray< CDRMRights > rights;
    rights.Reset();
    TCleanupItem cleanup( DoResetAndDestroyRightsArray, &rights );
    CleanupStack::PushL( cleanup );

    CDrmRightsParser* rp( CDrmRightsParser::NewL() );
    CleanupStack::PushL( rp );

    rp->ParseAndStoreL( *buf, rights );

    CleanupStack::PopAndDestroy( rp );
    CleanupStack::PopAndDestroy( &rights );
    CleanupStack::PopAndDestroy( buf );
    }

// -----------------------------------------------------------------------------
// Updates given file to DCFREpository
// -----------------------------------------------------------------------------
//
void UpdateRepositoryEntryL( const TDesC& aFile )
    {
    //Remove old entries
    CDcfRep* rep( CDcfRep::NewL() );
    CleanupStack::PushL( rep );
    rep->RemoveDcfFromDbL( aFile );
    CleanupStack::PopAndDestroy( rep );
    //And add new entry
    RDcfRepCli cli;
    User::LeaveIfError( cli.Connect() );
    CleanupClosePushL( cli );
    cli.AddFile( aFile );
    CleanupStack::PopAndDestroy( &cli );
    }

// ----------------------------------------------------------------------------
// CleanupPtrRPointerArrayHBufC8
// Does RPointerArray< >->ResetAndDestroy() for the given ppointer
// to RPointerArray deletes *aPtr and sets it to null.
// ----------------------------------------------------------------------------
//
LOCAL_C void CleanupPtrRPointerArrayHBufC8( TAny* aPtr )
    {
    RPointerArray< HBufC8 >** pPtr(
        reinterpret_cast< RPointerArray< HBufC8 >** >( aPtr ) );
    ( *pPtr )->ResetAndDestroy();
    delete *pPtr;
    *pPtr = NULL;
    }

// -----------------------------------------------------------------------------
// some caller inlines for convenience
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CallHasInfoUrlForTDesc
// -----------------------------------------------------------------------------
//
LOCAL_C inline void CallHasInfoUrlForTDesc(
    CDRMHelper*& aHelper, TInt& aTrappedError, TBool& aHasUrl,
    const TDesC& aFileName, HBufC8*& aInfoUrl )
    {
    TRAP( aTrappedError, aHasUrl = aHelper->HasInfoUrlL(
            const_cast< TDesC& >( aFileName ), aInfoUrl ) );
    }

// -----------------------------------------------------------------------------
// CallHasInfoUrlForCDataL(
// -----------------------------------------------------------------------------
//
LOCAL_C inline void CallHasInfoUrlForCDataL(
    CDRMHelper*& aHelper, TInt& aTrappedError, TBool& aHasUrl,
    const TDesC& aFileName, HBufC8*& aInfoUrl )
    {
    ContentAccess::TVirtualPathPtr virtualPath( aFileName );

    ContentAccess::CData* content(
        CData::NewLC( virtualPath, EPeek, EContentShareReadWrite ) );

    TRAP( aTrappedError, aHasUrl = aHelper->HasInfoUrlL(
            *content, aInfoUrl ) );
    CleanupStack::PopAndDestroy( content );
    }

// -----------------------------------------------------------------------------
// CallHasPreviewForTDesc
// -----------------------------------------------------------------------------
//
LOCAL_C inline void CallHasPreviewForTDesc(
    CDRMHelper*& aHelper, TInt& aTrappedError, TBool& aHasUrl,
    const TDesC& aFileName, HBufC8*& aInfoUrl )
    {
    TRAP( aTrappedError, aHasUrl = aHelper->HasPreviewL(
            const_cast< TDesC& >( aFileName ), aInfoUrl ) );
    }

// -----------------------------------------------------------------------------
// CallHasPreviewForCDataL
// -----------------------------------------------------------------------------
//
LOCAL_C inline void CallHasPreviewForCDataL(
    CDRMHelper*& aHelper, TInt& aTrappedError, TBool& aHasUrl,
    const TDesC& aFileName, HBufC8*& aInfoUrl )
    {
    ContentAccess::TVirtualPathPtr virtualPath( aFileName );

    ContentAccess::CData* content(
        CData::NewLC( virtualPath, EPeek, EContentShareReadWrite ) );

    TRAP( aTrappedError, aHasUrl = aHelper->HasPreviewL(
            *content, aInfoUrl ) );
    CleanupStack::PopAndDestroy( content );
    }

// -----------------------------------------------------------------------------
// CallGetPreviewRightsForCDataL
// -----------------------------------------------------------------------------
//
LOCAL_C inline void CallGetPreviewRightsForCDataL(
    CDRMHelper*& aHelper, TInt& aTrappedError, const TDesC& aFileName )
    {
    ContentAccess::TVirtualPathPtr virtualPath( aFileName );

    ContentAccess::CData* content(
        CData::NewLC( virtualPath, EPeek, EContentShareReadWrite ) );

    TRAP( aTrappedError, aHelper->GetPreviewRightsL( *content ) );
    CleanupStack::PopAndDestroy( content );
    }

// -----------------------------------------------------------------------------
// CallOpenInfoUrlForCDataL
// -----------------------------------------------------------------------------
//
LOCAL_C inline void CallOpenInfoUrlForCDataL(
    CDRMHelper*& aHelper, TInt& aTrappedError, const TDesC& aFileName )
    {
    ContentAccess::TVirtualPathPtr virtualPath( aFileName );

    ContentAccess::CData* content(
        CData::NewLC( virtualPath, EPeek, EContentShareReadWrite ) );

    TRAP( aTrappedError, aHelper->OpenInfoUrlL( *content ) );
    CleanupStack::PopAndDestroy( content );
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDrmHelperTest::Case
// Returns a test case by number.
//
// This function contains an array of all available test cases
// i.e pair of case name and test function. If case specified by parameter
// aCaseNumber is found from array, then that item is returned.
//
// The reason for this rather complicated function is to specify all the
// test cases only in one place. It is not necessary to understand how
// function pointers to class member functions works when adding new test
// cases. See function body for instructions how to add new test case.
// -----------------------------------------------------------------------------
//
const TCaseInfo CDrmHelperTest::Case (
    const TInt aCaseNumber ) const
     {

    /**
    * To add new test cases, implement new test case function and add new
    * line to KCases array specify the name of the case and the function
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to
    *    DrmHelperTest.cpp file and to DrmHelperTest.h
    *    header file.
    *
    * 2) Add entry to following KCases array either by using:
    *
    * 2.1: FUNCENTRY or ENTRY macro
    * ENTRY macro takes two parameters: test case name and test case
    * function name.
    *
    * FUNCENTRY macro takes only test case function name as a parameter and
    * uses that as a test case name and test case function name.
    *
    * Or
    *
    * 2.2: OOM_FUNCENTRY or OOM_ENTRY macro. Note that these macros are used
    * only with OOM (Out-Of-Memory) testing!
    *
    * OOM_ENTRY macro takes five parameters: test case name, test case
    * function name, TBool which specifies is method supposed to be run using
    * OOM conditions, TInt value for first heap memory allocation failure and
    * TInt value for last heap memory allocation failure.
    *
    * OOM_FUNCENTRY macro takes test case function name as a parameter and uses
    * that as a test case name, TBool which specifies is method supposed to be
    * run using OOM conditions, TInt value for first heap memory allocation
    * failure and TInt value for last heap memory allocation failure.
    */

    static TCaseInfoInternal const KCases[] =
        {
        // To add new test cases, add new items to this array

        // NOTE: When compiled to GCCE, there must be Classname::
        // declaration in front of the method name, e.g.
        // CDrmHelperTest::PrintTest. Otherwise the compiler
        // gives errors.

        ENTRY( "Hlp::Construct", CDrmHelperTest::ConstructTestL ),
        ENTRY( "Hlp::IndicateIdle", CDrmHelperTest::IndicateIdleTestL ),
        ENTRY( "Hlp::ActivateContentL", CDrmHelperTest::ActivateContentLTestL ),
        ENTRY( "Hlp::CanSetAutomated", CDrmHelperTest::CanSetAutomatedTestL ),
        ENTRY( "Hlp::CheckRightsAmountL", CDrmHelperTest::CheckRightsAmountLTestL ),
        ENTRY( "Hlp::CheckRightsPercentL", CDrmHelperTest::CheckRightsPercentLTestL ),
        ENTRY( "Hlp::Consume2", CDrmHelperTest::Consume2TestL ),
        ENTRY( "Hlp::ConsumeFile2", CDrmHelperTest::ConsumeFile2TestL ),
        ENTRY( "Hlp::EmbeddedPreviewCompletedL", CDrmHelperTest::EmbeddedPreviewCompletedLTestL ),
        ENTRY( "Hlp::GetContentURIList", CDrmHelperTest::GetContentURIListTestL ),
        ENTRY( "Hlp::GetPreviewRightsL", CDrmHelperTest::GetPreviewRightsLTestL ),
        ENTRY( "Hlp::GetRightsDetailsL", CDrmHelperTest::GetRightsDetailsLTestL ),
        ENTRY( "Hlp::GetRightsDetailsL(hlpconstraints)", CDrmHelperTest::GetHlpRightsDetailsLTestL ),
        ENTRY( "Hlp::HandleErrorL", CDrmHelperTest::HandleErrorLTestL ),
        ENTRY( "Hlp::HandleErrorOrPreviewL", CDrmHelperTest::HandleErrorOrPreviewLTestL ),
        ENTRY( "Hlp::HasInfoUrlL", CDrmHelperTest::HasInfoUrlLTestL ),
        ENTRY( "Hlp::HasPreviewL", CDrmHelperTest::HasPreviewLTestL ),
        ENTRY( "Hlp::LaunchDetailsViewEmbeddedL", CDrmHelperTest::LaunchDetailsViewEmbeddedLTestL ),
        ENTRY( "Hlp::OpenInfoUrlL", CDrmHelperTest::OpenInfoUrlLTestL ),
        ENTRY( "Hlp::RemoveAutomated", CDrmHelperTest::RemoveAutomatedTestL ),
        ENTRY( "Hlp::RemoveAutomatedPassive", CDrmHelperTest::RemoveAutomatedPassiveTestL ),
        ENTRY( "Hlp::RightsConstraintTestL", CDrmHelperTest::RightsConstraintsTestL),
        ENTRY( "Hlp::SetAutomated", CDrmHelperTest::SetAutomatedTestL ),
        ENTRY( "Hlp::SetAutomatedPassive", CDrmHelperTest::SetAutomatedPassiveTestL ),
        ENTRY( "Hlp::SetAutomatedSilent", CDrmHelperTest::SetAutomatedSilentTestL ),
        ENTRY( "Hlp::SetAutomatedType", CDrmHelperTest::SetAutomatedTypeTestL ),
        ENTRY( "Hlp::SetLimitsTestL", CDrmHelperTest::SetLimitsTestL ),
        ENTRY( "Hlp::SetPreviewMediaType", CDrmHelperTest::SetPreviewMediaTypeTestL ),
        ENTRY( "Hlp::ShowAutomatedNote", CDrmHelperTest::ShowAutomatedNoteTestL ),
        ENTRY( "Hlp::ShowDRMUINotification2L", CDrmHelperTest::ShowDRMUINotification2LTestL ),
        ENTRY( "Hlp::SupportedDRMMethods2", CDrmHelperTest::SupportedDRMMethods2TestL ),
        ENTRY( "Hlp::SupportedDataType", CDrmHelperTest::SupportedDataTypeTestL ),
        ENTRY( "Hlp::UpdateDataTypes", CDrmHelperTest::UpdateDataTypesTestL ),
        };

    // Verify that case number is valid
    if( (TUint) aCaseNumber >= sizeof( KCases ) /
                               sizeof( TCaseInfoInternal ) )
        {
        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"" );
        null.iMethod = NULL;
        null.iIsOOMTest = EFalse;
        null.iFirstMemoryAllocation = 0;
        null.iLastMemoryAllocation = 0;
        return null;
        }

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[ aCaseNumber ].iCaseName );
    tmp.iMethod = KCases[ aCaseNumber ].iMethod;
    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
    return tmp;

    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::SetResult
// -----------------------------------------------------------------------------
//
void CDrmHelperTest::SetResult(
    TTestResult& aResult,
    TInt aError )
    {

    if ( aError != KErrNone )
        {
        TBuf<KMaxPath> text;
        _LIT( KFormatString, "First error code: 0x%08x" );
        text.AppendFormat( KFormatString, aError );
        aResult.SetResult( KErrGeneral, text );
        }
    else
        {
        aResult.SetResult( KErrNone, KPassed );
        }
    }

// ========== The cases ========================================================

// -----------------------------------------------------------------------------
// CDrmHelperTest::ConstructTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::ConstructTestL( TTestResult& aResult )
    {
    CDRMHelper* helper( NULL );
    _LIT( KStep1,
        "Step1: DRMHelper::NewLC without arguments \
        Expecting: initialised helper");
    iLog->Log( KStep1 );
    helper = CDRMHelper::NewLC();
    CleanupStack::PopAndDestroy( helper );
    helper = NULL;
    iLog->Log( KPassed );


    _LIT( KStep2,
          "Step2: DRMHelper::NewL without arguments \
          Expecting: initialised helper");
    iLog->Log( KStep2 );
    helper = CDRMHelper::NewL();
    delete helper;
    helper = NULL;
    iLog->Log( KPassed );

    _LIT( KStep3,
          "Step3: DRMHelper::NewLC arguments RFs \
          Expecting: initiasised helper");
    iLog->Log( KStep3 );
    helper = CDRMHelper::NewLC( iFs );
    CleanupStack::PopAndDestroy( helper );
    helper = NULL;
    iLog->Log( KPassed );

    _LIT( KStep4,
          "Step4: DRMHelper::NewL arguments RFs \
          Expecting: initiasised helper");
    iLog->Log( KStep4 );
    helper = CDRMHelper::NewL( iFs );
    delete helper;
    helper = NULL;
    iLog->Log( KPassed );

    //preparations
    CCoeEnv& env( *CCoeEnv::Static() );

    _LIT( KStep5,
          "Step5: DRMHelper::NewLC arguments CCoeEnv \
          Expecting: initiasised helper");
    iLog->Log( KStep5 );
    helper = CDRMHelper::NewLC( env );
    CleanupStack::PopAndDestroy( helper );
    helper = NULL;
    iLog->Log( KPassed );

    _LIT( KStep6,
          "Step6: DRMHelper::NewL arguments CCoeEnv \
          Expecting: initiasised helper");
    iLog->Log( KStep6 );
    helper = CDRMHelper::NewL( env );
    delete helper;
    helper = NULL;
    iLog->Log( KPassed );

    _LIT( KStep7,
          "Step7: DRMHelper::NewLC arguments RFs, CCoeEnv \
          Expecting: initiasised helper");
    iLog->Log( KStep7 );
    helper = CDRMHelper::NewLC( env, iFs );
    CleanupStack::PopAndDestroy( helper );
    helper = NULL;
    iLog->Log( KPassed );

    _LIT( KStep8,
          "Step8: DRMHelper::NewL arguments RFs, CCoeEnv \
          Expecting: initiasised helper");
    iLog->Log( KStep8 );
    helper = CDRMHelper::NewL( env, iFs );
    delete helper;
    helper = NULL;
    iLog->Log( KPassed );


    SetResult( aResult, KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::IndicateIdleTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::IndicateIdleTestL( TTestResult& aResult )
    {
    TInt r( KErrNone );
    _LIT( KStep1,
          "Step1: DRMHelper::IndicateIdle \
          Expecting: clecn return from function");
    iLog->Log( KStep1 );

    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    helper->IndicateIdle();
    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, r );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CDrmHelperTest::ActivateContentLTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::ActivateContentLTestL( TTestResult& aResult )
    {
    // Note: no reasonable test doable before proper
    // UI automatisation tool present
    // Note2: this test calls both API methods of ActivateContentL
    TInt r( KErrNone );
    _LIT( KStep1,
        "Step1: DRMHelper::ActivateContentL with TDesC file name \
        of plain file \
        Expecting: leave with either KErrArgument or with KErrNotSupported");
    iLog->Log( KStep1 );

    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );

    TRAP( r, helper->ActivateContentL(
        const_cast< TDesC& >( KPlainTextFile() ) ) );
    T2L( r, KErrArgument, KErrNotSupported );
    _LIT( KStep2,
        "Step2: DRMHelper::ActivateContentL with CData object \
        and full rights \
        Expecting: leave with either KErrArgument or with KErrNotSupported");
    iLog->Log( KStep2 );
    ContentAccess::TVirtualPathPtr virtualPath( KPlainTextFile() );
    ContentAccess::CData* data( NULL );
    TRAP( r, data = CData::NewL(
        virtualPath, EPeek, EContentShareReadWrite ) );
    CleanupStack::PushL( data );
    TRAP( r, helper->ActivateContentL( *data ) );
    T2L( r, KErrArgument, KErrNotSupported );
    CleanupStack::PopAndDestroy( data );

    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::CanSetAutomatedTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::CanSetAutomatedTestL( TTestResult& aResult )
    {
    TInt r( KErrNone );
    DeleteRODBL();
    AddROL( KRO1Formatter, KCid1Jpeg, KCek1Jpeg, KDisplayCount1 );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    TBool canSetAutomated( ETrue );

    _LIT( KStep1,
        "Step1: DRMHelper::CanSetAutomated with TDesC name \
         of OMA 1 File and count RO \
         Expecting: EFalse" );
    iLog->Log( KStep1 );

    r = helper->CanSetAutomated(
        const_cast< TDesC& >( KDcf1JpegFile() ), canSetAutomated );
    TL( !canSetAutomated );
    T1L( r, KErrNone );

    _LIT( KStep2,
          "Step2: DRMHelper::CanSetAutomated with RFile handle \
            of OMA 1 File and full RO \
          Expecting: ETrue" );
    iLog->Log( KStep2 );

    AddROL( KRO1Formatter, KCid1Jpeg, KCek1Jpeg, KDisplayFull );
    RFile file;
    r = file.Open( iFs, KDcf1JpegFile, EFileRead | EFileShareAny );
    T1L( r, KErrNone );
    CleanupClosePushL( file );
    r = helper->CanSetAutomated( file, canSetAutomated );
    TL( canSetAutomated );
    T1L( r, KErrNone );
    CleanupStack::PopAndDestroy( &file );
    _LIT( KStep3,
        "Step3: DRMHelper::CanSetAutomated with TDesC8 cid \
        of OMA 1 File and no RO \
        Expecting: EFalse" );
    iLog->Log( KStep3 );
    DeleteRODBL();
    UpdateRepositoryEntryL( KDcf1JpegFile() );
    r = helper->CanSetAutomated( KCid1Jpeg(), canSetAutomated );
    TL( !canSetAutomated );
    T1L( r, KErrCANoRights );

    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::CheckRightsAmountLTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::CheckRightsAmountLTestL( TTestResult& aResult )
    {
    TInt r( KErrNone );
    TInt err( KErrNone );
    DeleteRODBL();
    AddROL( KRO1Formatter, KCid1Jpeg, KCek1Jpeg, KDisplayCount1 );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    const TInt KcountTreshold( 2 );
    const TInt KdaysTreshold( 1 );
    const TInt KNegativeCountTreshold( -3 );

    _LIT( KStep1,
        "Step1: DRMHelper::CheckRightsAmountL with TDesC name \
         of OMA 1 File and count RO \
         Expecting: EFalse" );
    iLog->Log( KStep1 );

    r = helper->CheckRightsAmountL(
        const_cast< TDesC& >( KDcf1JpegFile() ),
        KcountTreshold, KdaysTreshold );
    T1L( r, KErrNone );

    _LIT( KStep2,
          "Step2: DRMHelper::CheckRightsAmountL with RFile handle \
            of OMA 1 File and full RO \
          Expecting: ETrue" );
    iLog->Log( KStep2 );

    AddROL( KRO1Formatter, KCid1Jpeg, KCek1Jpeg, KDisplayFull );
    T1L( r, KErrNone );
    RFile file;
    r = file.Open( iFs, KDcf1JpegFile, EFileRead | EFileShareAny );
    T1L( r, KErrNone );
    CleanupClosePushL( file );
    TRAP( err, r = helper->CheckRightsAmountL( file,
        KcountTreshold, KdaysTreshold ) );

    T1L( r, KErrNone );
    T1L( err, KErrNone );
    CleanupStack::PopAndDestroy( &file );
    _LIT( KStep3,
        "Step3: DRMHelper::CheckRightsAmountL with TDesC8 cid \
        of OMA 1 File and with negative count treshold\
        Expecting: leave with KErrArgument" );
    iLog->Log( KStep3 );
    DeleteRODBL();
    UpdateRepositoryEntryL( KDcf1JpegFile() );
    TRAP( err, r = helper->CheckRightsAmountL( KCid1Jpeg(),
        KNegativeCountTreshold, KdaysTreshold ) );
    T1L( err, KErrArgument );

    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, r );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::CheckRightsPercentLTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::CheckRightsPercentLTestL( TTestResult& aResult )
    {
    TInt r( KErrNone );
    TInt err( KErrNone );
    const TInt KpercentTreshold( 20 );
    const TInt KOverFlowTreshold( 200 );
    const TInt KNegativeTreshold( -3 );

    // preparations
    DeleteRODBL();
    AddROL( KRO1Formatter, KCid1Jpeg, KCek1Jpeg, KDisplayFull );
    UpdateRepositoryEntryL( KDcf1JpegFile() );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );

    _LIT( KStep1,
        "Step1: DRMHelper::CheckRightsPercentL with TDesC name \
         of OMA 1 File and count RO \
         Expecting: EFalse" );
    iLog->Log( KStep1 );

    TRAP( err, r = helper->CheckRightsPercentL(
        const_cast< TDesC& >( KDcf1JpegFile() ),
        KNegativeTreshold ) );
    T1L( err, KErrArgument );
    T1L( r, KErrNone );

    _LIT( KStep2,
          "Step2: DRMHelper::CheckRightsPercentL with RFile handle \
            of OMA 1 File and full RO \
          Expecting: ETrue" );
    iLog->Log( KStep2 );

    T1L( r, KErrNone );
    RFile file;
    r = file.Open( iFs, KDcf1JpegFile, EFileRead | EFileShareAny );
    T1L( r, KErrNone );
    CleanupClosePushL( file );
    TRAP( err, r = helper->CheckRightsPercentL( file,
        KpercentTreshold ) );

    T1L( r, KErrNone );
    T1L( err, KErrNone );
    CleanupStack::PopAndDestroy( &file );
    _LIT( KStep3,
        "Step3: DRMHelper::CheckRightsPercentL with TDesC name \
        of OMA 1 File and with negative count treshold\
        Expecting: leave with KErrArgument" );
    iLog->Log( KStep3 );
    DeleteRODBL();
    TRAP( err, r = helper->CheckRightsPercentL(
        const_cast< TDesC& >( KDcf1JpegFile() ),
        KOverFlowTreshold ) );
    T1L( err, KErrArgument );

    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, r );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::Consume2TestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::Consume2TestL( TTestResult& aResult )
    {
    TInt r( KErrNone );
    TUint32 rightsSpec( CDRMRights::EDisplay );
    CDRMHelper::TDRMHelperConsumeAction consumeAction( CDRMHelper::EStart );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    DeleteRODBL();
    AddROL( KRO1Formatter, KCid1Jpeg, KCek1Jpeg, KDisplayCount1 );

    _LIT( KStep1,
        "Step1: DRMHelper::Consume2 with TDesC8 cid \
        of OMA 1 File and count 1 (consumestart) \
        Expecting: success" );
    iLog->Log( KStep1 );
    r = helper->Consume2( KCid1Jpeg(), rightsSpec, consumeAction );
    T1L( r, KErrNone );

    _LIT( KStep2,
        "Step2: DRMHelper::Consume2 with TDesC8 cid \
        of OMA 1 File and count 1 (consumeStop) \
        Expecting: success" );
    iLog->Log( KStep2 );
    consumeAction = CDRMHelper::EFinish;
    r = helper->Consume2( KCid1Jpeg(), rightsSpec, consumeAction );
    T1L( r, KErrNone );
    CleanupStack::PopAndDestroy( helper );

    SetResult( aResult, KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::ConsumeFile2TestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::ConsumeFile2TestL( TTestResult& aResult )
    {
    TInt r( KErrNone );
    TUint32 rightsSpec( CDRMRights::EDisplay );
    CDRMHelper::TDRMHelperConsumeAction consumeAction( CDRMHelper::EFinish );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );

    DeleteRODBL();
    AddROL( KRO1Formatter, KCid1Jpeg, KCek1Jpeg, KDisplayCount1 );

    _LIT( KStep1,
        "Step1: DRMHelper::ConsumeFile2 with TDesC filename \
        of OMA 1 File and count 1 (consumestop) \
        Expecting: KErrNotFound" );
    iLog->Log( KStep1 );
    r = helper->ConsumeFile2( KDcf1JpegFile(), rightsSpec, consumeAction );
    T1L( r, KErrNotFound );

    _LIT( KStep2,
        "Step2: DRMHelper::ConsumeFile2 with RFile handle \
        of OMA 1 File and count 1 (consumepause, no consumestart) \
        Expecting: KErrNotFound" );
    iLog->Log( KStep2 );
    consumeAction = CDRMHelper::EPause;
    r = helper->ConsumeFile2( KDcf1JpegFile(), rightsSpec, consumeAction );
    T1L( r, KErrNotFound );
    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::EmbeddedPreviewCompletedLTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::EmbeddedPreviewCompletedLTestL( TTestResult& aResult )
    {
    // Note: reasonable case not doable, since
    // all reasonable usage scenarios require user input from UI
    TInt r( KErrNone );
    _LIT( KStep1,
          "Step1: DRMHelper::EmbeddedPreviewCompletedL with OMA 1 file \
          Expecting: leave with error KErrNotSupported or \
          with KErrCANotSupported" );
    iLog->Log( KStep1 );

    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    TRAPD( err, helper->EmbeddedPreviewCompletedL(
        const_cast< TDesC& >( KDcf1JpegFile() ) ) );
    T2L( err, KErrNotSupported, KErrCANotSupported );
    //Just leaves with KErrNotSupported
    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, r );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CDrmHelperTest::GetContentURIListTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::GetContentURIListTestL( TTestResult& aResult )
    {
    TInt r( KErrNone );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    RPointerArray< HBufC8 >* uriList( NULL );

    DeleteRODBL();

    _LIT( KStep1,
          "Step1: DRMHelper::GetContentURIList, no rights present \
          Expecting: empty list" );
    iLog->Log( KStep1 );
    r = helper->GetContentURIList( uriList );
    // Check results
    TL( uriList );
    TCleanupItem item( CleanupPtrRPointerArrayHBufC8, &uriList );
    CleanupStack::PushL( item );
    T1L( r, KErrNone );
    T1L( uriList->Count(), 0 );
    // Cleanup
    CleanupStack::PopAndDestroy( &uriList );
    TL( !uriList );

    // Preaparations for step 2
    AddROL( KRO1Formatter, KCid1Jpeg, KCek1Jpeg, KDisplayFull );

    _LIT( KStep2,
          "Step2: DRMHelper::GetContentURIList with 1 rights object present \
          Expecting: correct contentUri" );
    iLog->Log( KStep2 );
    r = helper->GetContentURIList( uriList );
    // Check results
    TL( uriList );
    TCleanupItem item2( CleanupPtrRPointerArrayHBufC8, &uriList );
    CleanupStack::PushL( item2 );
    T1L( r, KErrNone );
    T1L( uriList->Count(), 1 );
    const TPtrC8 comparisonValue( ( *uriList )[ 0 ]->Des() );
    TL( comparisonValue == KCid1Jpeg() );
    // Cleanup
    CleanupStack::PopAndDestroy( &uriList );
    TL( !uriList );
    DeleteRODBL();

    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, r );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CDrmHelperTest::GetPreviewRightsLTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::GetPreviewRightsLTestL( TTestResult& aResult )
    {
    TInt r( KErrNone );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );

    DeleteRODBL();
    _LIT( KStep1,
        "Step1: CDRMHelper::GetPreviewRightsL with \
         TDesC name of plain text file \
         Expecting: leave with KErrArgument" );
    iLog->Log( KStep1 );
    TRAP( r, helper->GetPreviewRightsL(
           const_cast< TDesC& >( KPlainTextFile() ) ) );
    T1L( r, KErrArgument );

    _LIT( KStep2,
        "Step2: CDRMHelper::GetPreviewRightsL with \
         CData handle of DCF1 file \
         Expecting: leave with KErrArgument" );
    iLog->Log( KStep2 );
    CallGetPreviewRightsForCDataL( helper, r, KDcf1JpegFile() );
    T1L( r, KErrArgument );



    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::GetRightsDetailsLTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::GetRightsDetailsLTestL( TTestResult& aResult )
    {
    TInt r( KErrNone );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    CDRMRightsConstraints* cPlay( NULL );
    CDRMRightsConstraints* cView( NULL );
    CDRMRightsConstraints* cExecute( NULL );
    CDRMRightsConstraints* cPrint( NULL );
    TBool expired( EFalse );
        TBool sendingAllowed( ETrue );
    // preparations
    DeleteRODBL();
    AddROL( KRO1Formatter, KCid1Jpeg, KCek1Jpeg, KDisplayFull );
    _LIT( KStep1,
          "Step1: DRMHelper::GetRightsDetailsL with TDesC filename \
           of OMA 1 file and full Rights \
           and with pointers to CDRMRightsConstraints \
          Expecting: correct rights details" );
    iLog->Log( KStep1 );
    //Step 1
    TRAP(r, helper->GetRightsDetailsL(
        KDcf1JpegFile(), ContentAccess::EView, expired, sendingAllowed,
        cPlay, cView, cExecute, cPrint ) );
    T1L( r, KErrNone );
    TL( !sendingAllowed );
    TL( !expired );

    // Checks for play constraint
    TL( !cPlay );
    // Checks for view constraint
    TL( cView );
    CleanupStack::PushL( cView );
    //Check correctness of permission
    TL( cView->FullRights() );
    TL( !cView->IsPreview() );
    CleanupStack::PopAndDestroy( cView );
    cView = NULL;
    TL( !cExecute );
    TL( !cPrint );

    //Preperations for step 2
    DeleteRODBL();
    RFile file;
    r = file.Open( iFs, KDcf1JpegFile, EFileRead | EFileShareAny );
    T1L( r, KErrNone );
    CleanupClosePushL( file );

    _LIT( KStep2,
          "Step2: DRMHelper::GetRightsDetailsL with TDesC filename \
           of OMA 1 file and no Rights \
           and with pointers to CDRMRightsConstraints \
          Expecting: leave with error KErrCANoRights" );
    iLog->Log( KStep2 );
    TRAP(r, helper->GetRightsDetailsL(
        KDcf1JpegFile(), ContentAccess::EView, expired, sendingAllowed,
        cPlay, cView, cExecute, cPrint ) );
    T1L( r, KErrCANoRights );
    TL( !cPlay );
    TL( !cView );
    TL( !cExecute );
    TL( !cPrint );

    CleanupStack::PopAndDestroy( &file );

    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::GetHlpRightsDetailsLTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::GetHlpRightsDetailsLTestL( TTestResult& aResult )
    {
    TInt r( KErrNone );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    CDRMHelperRightsConstraints* hlpPlay( NULL );
    CDRMHelperRightsConstraints* hlpView( NULL );
    CDRMHelperRightsConstraints* hlpExecute( NULL );
    CDRMHelperRightsConstraints* hlpPrint( NULL );
    TBool expired( ETrue );
    TBool sendingAllowed( ETrue );
    // preparations
    DeleteRODBL();
    AddROL( KRO1Formatter, KCid1Jpeg, KCek1Jpeg, KDisplayCount1 );
    _LIT( KStep1,
          "Step1: DRMHelper::GetRightsDetailsL with TDesC filename \
           of OMA 1 file and count 1 display Rights \
           and with pointers to CDRMHelperRightsConstraints \
          Expecting: correct rights details" );
    iLog->Log( KStep1 );
    //Step 1
    TRAP(r, helper->GetRightsDetailsL(
        KDcf1JpegFile(), ContentAccess::EView, expired, sendingAllowed,
        hlpPlay, hlpView, hlpExecute, hlpPrint ) );
    T1L( r, KErrNone );
    TL( !sendingAllowed );
    TL( !expired );

    // Checks for play constraint
    TL( !hlpPlay );
    // Checks for view constraint
    TL( hlpView );
    CleanupStack::PushL( hlpView );
    //Check correctness of permission
    TL( !hlpView->FullRights() );
    TL( hlpView->IsPreview() );
    TUint32 counter( 0 );
    TUint32 origCounter( 0 );
    TRAP( r, hlpView->GetCountersL( counter, origCounter ) );
    T1L( r, KErrNone );
    T1L( counter, 1 );
    T1L( origCounter, 1 );
    CleanupStack::PopAndDestroy( hlpView );
    hlpView = NULL;
    TL( !hlpExecute );
    TL( !hlpPrint );

    // preparations for step2
    DeleteRODBL();
    AddROL( KRO1Formatter, KCid1Jpeg, KCek1Jpeg,
        KDisplayCount1PrintInterval1Second );
    RFile file;
    r = file.Open( iFs, KDcf1JpegFile, EFileRead | EFileShareAny );
    T1L( r, KErrNone );
    CleanupClosePushL( file );

    _LIT( KStep2,
          "Step2: DRMHelper::GetRightsDetailsL with TDesC filename \
           of OMA 1 file, count 1 display Rights, \
           interval 1 second print rights \
           and with pointers to CDRMHelperRightsConstraints \
          Expecting: correct rights details" );
    iLog->Log( KStep2 );

    TRAP(r, helper->GetRightsDetailsL(
        file, ContentAccess::EView, expired, sendingAllowed,
        hlpPlay, hlpView, hlpExecute, hlpPrint ) );
    T1L( r, KErrNone );
    CleanupStack::PushL( hlpPrint );
    CleanupStack::PushL( hlpView );
    TL( !sendingAllowed );
    TL( !expired );

    // Checks for play constraint
    TL( !hlpPlay );
    // Checks for view constraint
    TL( hlpView );
    //Check correctness of display permission
    TL( !hlpView->FullRights() );
    TL( hlpView->IsPreview() );
    counter =  0;
    origCounter = 0 ;
    TRAP( r, hlpView->GetCountersL( counter, origCounter ) );
    T1L( r, KErrNone );
    T1L( counter, 1 );
    T1L( origCounter, 1 );
    CleanupStack::PopAndDestroy( hlpView );
    hlpView = NULL;
    TL( !hlpExecute );
    TL( hlpPrint );
    //Check correctness of print permission
    TL( !hlpPrint->FullRights() );
    TL( !hlpPrint->IsPreview() );
    TTimeIntervalSeconds interval( 0 );
    const TTimeIntervalSeconds KSecond( 1 );
    TRAP( r, hlpPrint->GetIntervalL( interval ) );
    TL( interval == KSecond );

    CleanupStack::PopAndDestroy( hlpPrint );

    CleanupStack::PopAndDestroy( &file );

    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, KErrNone );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CDrmHelperTest::HandleErrorLTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::HandleErrorLTestL( TTestResult& aResult )
    {
    TInt r( KErrGeneral );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    DeleteRODBL();

    _LIT( KStep1,
          "Step1: DRMHelper::HandleErrorL with \
          error code KErrGeneral and nonexistent TDesC8 CID \
          Expecting: leave with KErrNotFound" );
    iLog->Log( KStep1 );

    TRAP( r, helper->HandleErrorL( KErrGeneral, KCidNotExist() ) );
    T1L( r, KErrNotFound );

    _LIT( KStep2,
          "Step2: DRMHelper::HandleErrorL with error code KErrGeneral \
           and TDesC name of plain text file \
          Expecting: leave with KErrArgument" );
    iLog->Log( KStep2 );
    TRAP( r, helper->HandleErrorL( KErrGeneral, KPlainTextFile() ) );
    T1L( r, KErrArgument );

    // preparations for step3
    RFile file;
    r = file.Open( iFs, KPlainTextFile, EFileRead | EFileShareAny );
    T1L( r, KErrNone );
    CleanupClosePushL( file );

    _LIT( KStep3,
          "Step3: DRMHelper::HandleErrorL with error code KErrGeneral \
           and RFile handle to plain text file \
          Expecting: leave with KErrArgument" );
    iLog->Log( KStep3 );
    TRAP( r, helper->HandleErrorL( KErrNotSupported, file ) );
    T1L( r, KErrArgument );
    CleanupStack::PopAndDestroy( &file );

    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::HandleErrorOrPreviewLTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::HandleErrorOrPreviewLTestL( TTestResult& aResult )
    {
    TInt r( KErrGeneral );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    DeleteRODBL();
    HBufC8* previewUrl( NULL );

    _LIT( KStep1,
          "Step1: DRMHelper::HandleErrorL with error code KErrGeneral \
           and TDesC name of plain text file \
          Expecting: leave with KErrArgument" );
    iLog->Log( KStep1 );
    TRAP( r, helper->HandleErrorOrPreviewL(
            KErrGeneral, KPlainTextFile(), previewUrl ) );
    T1L( r, KErrArgument );

    //Preperations for step 2
    RFile file;
    r = file.Open( iFs, KPlainTextFile, EFileRead | EFileShareAny );
    T1L( r, KErrNone );
    CleanupClosePushL( file );

    _LIT( KStep2,
          "Step2: DRMHelper::HandleErrorL with error code KErrGeneral \
           and RFile handle to plain text file \
          Expecting: leave with KErrArgument" );
    iLog->Log( KStep2 );
    TRAP( r, helper->HandleErrorOrPreviewL(
            KErrGeneral, file, previewUrl ) );
    T1L( r, KErrArgument );
    CleanupStack::PopAndDestroy( &file );

    CleanupStack::PopAndDestroy( helper );

    SetResult( aResult, KErrNone );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CDrmHelperTest::HasInfoUrlLTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::HasInfoUrlLTestL( TTestResult& aResult )
    {
    TInt r( KErrNone );
    TBool hasInfoUrl( EFalse );
    HBufC8* infoUrl( NULL );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    _LIT( KStep1,
        "Step1: DRMHelper::HasInfoUrlL with plain file \
        Expecting: error KErrNotSupported");
    iLog->Log( KStep1 );
    CallHasInfoUrlForTDesc(
        helper, r, hasInfoUrl, KPlainTextFile(), infoUrl );
    T1L( r, KErrNone );
    TL( !hasInfoUrl );
    TL( !infoUrl );

    _LIT( KStep2,
        "Step2: DRMHelper::HasInfoUrlL with plain CData \
        Expecting: error KErrNotSupported");
    iLog->Log( KStep2 );
    CallHasInfoUrlForCDataL(
        helper, r, hasInfoUrl, KPlainTextFile(), infoUrl );
    T1L( r, KErrNone );
    TL( !hasInfoUrl );
    TL( !infoUrl );

    _LIT( KStep3,
        "Step3: DRMHelper::HasInfoUrlL with DCF1 file \
        Expecting: error KErrNotSupported");
    iLog->Log( KStep3 );
    CallHasInfoUrlForTDesc(
        helper, r, hasInfoUrl, KDcf1JpegFile(), infoUrl );
    T1L( r, KErrNone );
    TL( !hasInfoUrl );
    TL( !infoUrl );

    _LIT( KStep4,
        "Step4: DRMHelper::HasInfoUrlL with DCF1 CData \
        Expecting: error KErrNotSupported");
    iLog->Log( KStep4 );
    CallHasInfoUrlForCDataL( helper, r, hasInfoUrl, KDcf1JpegFile(), infoUrl );
    T1L( r, KErrNone );
    TL( !hasInfoUrl );
    TL( !infoUrl );

    _LIT( KStep5,
        "Step5: DRMHelper::HasInfoUrlL with DCF2 file \
        Expecting: error KErrNotSupported" );
    iLog->Log( KStep5 );
    CallHasInfoUrlForTDesc(
        helper, r, hasInfoUrl, KDcf2JpegFile() , infoUrl );
    T1L( r, KErrNone );
    TL( !hasInfoUrl );
    TL( !infoUrl );

    _LIT( KStep6,
        "Step6: DRMHelper::HasInfoUrlL with DCF2 CData \
        Expecting: error KErrNotSupported" );
    iLog->Log( KStep6 );
    CallHasInfoUrlForCDataL(
        helper, r, hasInfoUrl, KDcf2JpegFile() , infoUrl );
    T1L( r, KErrNone );
    TL( !hasInfoUrl );
    TL( !infoUrl );

    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::HasPreviewLTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::HasPreviewLTestL( TTestResult& aResult )
    {
    TInt r( KErrNone );
    TBool hasPreview( EFalse );
    HBufC8* previewUrl( NULL );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    _LIT( KStep1,
        "Step1: DRMHelper::HasPreviewL with plain file \
        Expecting: error KErrNotSupported");
    iLog->Log( KStep1 );
    CallHasPreviewForTDesc(
        helper, r, hasPreview, KPlainTextFile(), previewUrl );
    T1L( r, KErrNone );
    TL( !hasPreview );
    TL( !previewUrl );

    _LIT( KStep2,
        "Step2: DRMHelper::HasPreviewL with plain CData \
        Expecting: error KErrNotSupported");
    iLog->Log( KStep2 );
    CallHasPreviewForCDataL(
        helper, r, hasPreview, KPlainTextFile(), previewUrl );
    T1L( r, KErrNone );
    TL( !hasPreview );
    TL( !previewUrl );

    _LIT( KStep3,
        "Step3: DRMHelper::HasPreviewL with DCF1 file \
        Expecting: error KErrNotSupported");
    iLog->Log( KStep3 );
    CallHasPreviewForTDesc(
        helper, r, hasPreview, KDcf1JpegFile(), previewUrl );
    T1L( r, KErrNone );
    TL( !hasPreview );
    TL( !previewUrl );

    _LIT( KStep4,
        "Step4: DRMHelper::HasPreviewL with DCF1 CData \
        Expecting: error KErrNotSupported");
    iLog->Log( KStep4 );
    CallHasPreviewForCDataL( helper, r, hasPreview, KDcf1JpegFile(), previewUrl );
    T1L( r, KErrNone );
    TL( !hasPreview );
    TL( !previewUrl );

    _LIT( KStep5,
        "Step5: DRMHelper::HasPreviewL with DCF2 file \
        Expecting: error KErrNotSupported" );
    iLog->Log( KStep5 );
    CallHasPreviewForTDesc(
        helper, r, hasPreview, KDcf2JpegFile() , previewUrl );
    T1L( r, KErrNone );
    TL( !hasPreview );
    TL( !previewUrl );

    _LIT( KStep6,
        "Step6: DRMHelper::HasPreviewL with DCF2 CData \
        Expecting: error KErrNotSupported" );
    iLog->Log( KStep6 );
    CallHasPreviewForCDataL(
        helper, r, hasPreview, KDcf2JpegFile() , previewUrl );
    T1L( r, KErrNone );
    TL( !hasPreview );
    TL( !previewUrl );

    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, r );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::LaunchDetailsViewEmbeddedLTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::LaunchDetailsViewEmbeddedLTestL( TTestResult& aResult )
    {
    TInt r( KErrNone );

    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    DeleteRODBL();
    HBufC8* uriBuf( HBufC8::NewMaxLC( KCid1Jpeg().Length() ) );

    _LIT( KStep1,
        "Step1: empty DRMHelper::LaunchDetailsViewEmbeddedL \
        with nonexistent TDesC8 CID \
        Expecting: no actions and  no errors" );
    iLog->Log( KStep1 );
    TRAP( r, helper->LaunchDetailsViewEmbeddedL( KCidNotExist() ) );
    T1L( r, KErrNone );

    _LIT( KStep2,
        "Step2: DRMHelper::LaunchDetailsViewEmbeddedL \
        with nonexistent HBufC8* CID and no rights \
        Expecting: leave with KErrCANoRights" );
    iLog->Log( KStep2 );
    TRAP( r, helper->LaunchDetailsViewEmbeddedL( uriBuf ) );
    T1L( r, KErrCANoRights );
    CleanupStack::PopAndDestroy( uriBuf );

    _LIT( KStep3,
        "Step3: DRMHelper::LaunchDetailsViewEmbeddedL \
        with TDesC name of plain text file \
        Expecting: leave with KErrArgument" );
    iLog->Log( KStep3 );
    TRAP( r, helper->LaunchDetailsViewEmbeddedL( KPlainTextFile() ) );
    T1L( r, KErrArgument );

    RFile file;
    r = file.Open( iFs, KPlainTextFile, EFileRead | EFileShareAny );
    T1L( r, KErrNone );
    CleanupClosePushL( file );

    _LIT( KStep4,
        "Step4: DRMHelper::LaunchDetailsViewEmbeddedL \
        with RFile handle to plain text file \
        Expecting: leave with KErrArgument" );
    iLog->Log( KStep4 );
    TRAP( r, helper->LaunchDetailsViewEmbeddedL( file ) );
    T1L( r, KErrArgument );
    CleanupStack::PopAndDestroy( &file );

    CleanupStack::PopAndDestroy( helper );

    SetResult( aResult, KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::OpenInfoUrlLTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::OpenInfoUrlLTestL( TTestResult& aResult )
    {
    TInt r( KErrGeneral );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );

    DeleteRODBL();
    _LIT( KStep1,
        "Step1: CDRMHelper::OpenInfoUrlL with \
         TDesC name of plain text file \
         Expecting: leave with KErrArgument" );
    iLog->Log( KStep1 );
    TRAP( r, helper->OpenInfoUrlL(
           const_cast< TDesC& >( KPlainTextFile() ) ) );
    T1L( r, KErrArgument );

    _LIT( KStep2,
        "Step2: CDRMHelper::OpenInfoUrlL with \
         CData handle of DCF1 file \
         Expecting: leave with KErrArgument" );
    iLog->Log( KStep2 );
    CallOpenInfoUrlForCDataL( helper, r, KDcf1JpegFile() );
    T1L( r, KErrArgument );


    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::RemoveAutomatedTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::RemoveAutomatedTestL( TTestResult& aResult )
    {
    TInt r( KErrGeneral );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    _LIT( KStep1,
        "Step1: DRMHelper::RemoveAutomated with TDesC name \
         of plain File \
         Expecting: KErrArgument" );
    iLog->Log( KStep1 );

    r = helper->RemoveAutomated( const_cast< TDesC& >( KPlainTextFile() ) );
    T1L( r, KErrArgument );

    RFile file;
    r = file.Open( iFs, KPlainTextFile, EFileRead | EFileShareAny );
    T1L( r, KErrNone );
    CleanupClosePushL( file );

    _LIT( KStep2,
          "Step2: DRMHelper::RemoveAutomated with RFile handle \
            of plain file \
          Expecting: KErrArgument" );
    iLog->Log( KStep2 );
    r = helper->RemoveAutomated( file );
    T1L( r, KErrArgument );
    CleanupStack::PopAndDestroy( &file );


    _LIT( KStep3,
        "Step3: DRMHelper::RemoveAutomated with nonexistent TDesC8 cid \
          Expecting: KErrNotFound" );
    iLog->Log( KStep3 );
    r = helper->RemoveAutomated( KCidNotExist() );
    T1L( r, KErrNotFound );
    CleanupStack::PopAndDestroy( helper );

    SetResult( aResult, KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::RemoveAutomatedPassiveTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::RemoveAutomatedPassiveTestL( TTestResult& aResult )
    {
    TInt r( KErrGeneral );

    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    _LIT( KStep1,
        "Step1: DRMHelper::RemoveAutomatedPassive with TDesC name \
         of plain File \
         Expecting: KErrArgument" );
    iLog->Log( KStep1 );

    r = helper->RemoveAutomatedPassive(
        const_cast< TDesC& >( KPlainTextFile() ) );
    T1L( r, KErrArgument );

    _LIT( KStep2,
          "Step2: DRMHelper::RemoveAutomatedPassive with RFile handle \
            of plain file \
          Expecting: KErrArgument" );
    iLog->Log( KStep2 );

    RFile file;
    r = file.Open( iFs, KPlainTextFile, EFileRead | EFileShareAny );
    T1L( r, KErrNone );
    CleanupClosePushL( file );
    r = helper->RemoveAutomatedPassive( file );
    T1L( r, KErrArgument );
    CleanupStack::PopAndDestroy( &file );
    _LIT( KStep3,
        "Step3: DRMHelper::RemoveAutomatedPassive with nonexistent TDesC8 cid \
          Expecting: KErrNotFound" );
    iLog->Log( KStep3 );
    r = helper->RemoveAutomatedPassive( KCidNotExist() );
    T1L( r, KErrNotFound );
    CleanupStack::PopAndDestroy( helper );


    SetResult( aResult, KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::RightsConstraintsTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::RightsConstraintsTestL( TTestResult& aResult )
    {
    TInt r( KErrNone );

    CDRMHelperRightsConstraints* play( NULL );
    CDRMHelperRightsConstraints* view( NULL );
    CDRMHelperRightsConstraints* execute( NULL );
    CDRMHelperRightsConstraints* print( NULL );
    TBool expired( ETrue );
    TBool sendingAllowed( ETrue );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    // preparations
    DeleteRODBL();
    AddROL( KRO1Formatter, KCid1Jpeg, KCek1Jpeg, KDisplayCount1 );
    TRAP(r, helper->GetRightsDetailsL(
        KDcf1JpegFile(), ContentAccess::EView, expired, sendingAllowed,
        play, view, execute, print ) );
    TL( view );
    CleanupStack::PushL( view );
    T1L( r, KErrNone );
    TL( !play );
    TL( !execute );
    TL( !print );

    _LIT( KStep1,
        "Step1: CDRMHelperRightsConstraints::FullRights \
         of for count 1 RO \
         Expecting: EFalse" );
    iLog->Log( KStep1 );
    TL( !view->FullRights() );

    _LIT( KStep2,
        "Step2: CDRMHelperRightsConstraints::IsPreview \
         of for count 1 RO \
         Expecting: ETrue" );
    iLog->Log( KStep2 );
    TL( view->IsPreview() );

    _LIT( KStep3,
        "Step3: CDRMHelperRightsConstraints::GetAccumulatedTimeL \
         of for count 1 RO \
         Expecting: Leave with error code" );
    iLog->Log( KStep3 );
    TTimeIntervalSeconds interval( -1 );
    TTime time( Time::NullTTime() );
    TRAP( r, view->GetAccumulatedTimeL( interval ) );
    T1L( r, KErrNotFound );

    _LIT( KStep4,
        "Step4: CDRMHelperRightsConstraints::GetIntervalL \
         of for count 1 RO \
         Expecting: Leave with error code" );
    iLog->Log( KStep4 );
    TRAP( r, view->GetIntervalL( interval ) );
    T1L( r, KErrNotFound );

    _LIT( KStep5,
        "Step5: CDRMHelperRightsConstraints::GetIntervalStartL \
         of for count 1 RO \
         Expecting: Leave with error code" );
    iLog->Log( KStep5 );
    TRAP( r, view->GetIntervalStartL( time ) );
    T1L( r, KErrNotFound );

    _LIT( KStep6,
        "Step6: CDRMHelperRightsConstraints::GetStartTimeL \
         of for count 1 RO \
         Expecting: Leave with error code" );
    iLog->Log( KStep6 );
    TRAP( r, view->GetStartTimeL( time ) );
    T1L( r, KErrNotFound );

    _LIT( KStep7,
        "Step7: CDRMHelperRightsConstraints::GetEndTimeL \
         of for count 1 RO \
         Expecting: Leave with error code" );
    iLog->Log( KStep7 );
    TRAP( r, view->GetEndTimeL( time ) );
    T1L( r, KErrNotFound );

    _LIT( KStep8,
        "Step8: CDRMHelperRightsConstraints::GetTimedCountL \
         of for count 1 RO \
         Expecting: Leave with error code" );
    iLog->Log( KStep8 );
    TUint32 count( 0 );
    TUint32 origCount( 0 );
    TRAP( r, view->GetTimedCountL( count, origCount, interval ) );
    T1L( r, KErrNotFound );

    _LIT( KStep9,
        "Step9: CDRMHelperRightsConstraints::GetCountL \
         of for count 1 RO \
         Expecting: Correct count" );
    iLog->Log( KStep9 );
    TRAP( r, view->GetCountersL( count, origCount ) );
    T1L( r, KErrNone );
    T1L( count, 1 );
    T1L( origCount, 1 );

    CleanupStack::PopAndDestroy( view );
    CleanupStack::PopAndDestroy( helper );


    SetResult( aResult, r );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::SetAutomatedTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::SetAutomatedTestL( TTestResult& aResult )
    {
    TInt r( KErrNone );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    // preparations
    _LIT( KStep1,
        "Step1: DRMHelper::SetAutomated with TDesC name \
         of plain File \
         Expecting: KErrNone" );
    iLog->Log( KStep1 );

    r = helper->SetAutomated( const_cast< TDesC& >( KPlainTextFile() ) );
    T1L( r, KErrNone );

    _LIT( KStep2,
          "Step2: DRMHelper::SetAutomated with RFile handle \
            of plain file \
          Expecting: KErrNone" );
    iLog->Log( KStep2 );

    RFile file;
    r = file.Open( iFs, KPlainTextFile, EFileRead | EFileShareAny );
    T1L( r, KErrNone );
    CleanupClosePushL( file );
    r = helper->SetAutomated( file );
    T1L( r, KErrNone );
    CleanupStack::PopAndDestroy( &file );
    _LIT( KStep3,
        "Step3: DRMHelper::SetAutomated with nonexistent TDesC8 cid \
          Expecting: KErrArgument" );
    iLog->Log( KStep3 );
    r = helper->SetAutomated( KCidNotExist() );
    T1L( r, KErrNotFound );
    CleanupStack::PopAndDestroy( helper );

    SetResult( aResult, KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::SetAutomatedPassiveTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::SetAutomatedPassiveTestL( TTestResult& aResult )
    {
    TInt r( KErrNone );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    _LIT( KStep1,
        "Step1: DRMHelper::SetAutomatedPassive with TDesC name \
         of plain File \
         Expecting: KErrNone" );
    iLog->Log( KStep1 );

    r = helper->SetAutomatedPassive(
        const_cast< TDesC& >( KPlainTextFile() ) );
    T1L( r, KErrNone );

    _LIT( KStep2,
          "Step2: DRMHelper::SetAutomatedPassive with RFile handle \
            of plain file \
          Expecting: KErrNone" );
    iLog->Log( KStep2 );

    RFile file;
    r = file.Open( iFs, KPlainTextFile, EFileRead | EFileShareAny );
    T1L( r, KErrNone );
    CleanupClosePushL( file );
    r = helper->SetAutomatedPassive( file );
    T1L( r, KErrNone );
    CleanupStack::PopAndDestroy( &file );
    _LIT( KStep3,
        "Step3: DRMHelper::SetAutomatedPassive with nonexistent TDesC8 cid \
          Expecting: KErrNotFound" );
    iLog->Log( KStep3 );
    r = helper->SetAutomatedPassive( KCidNotExist() );
    T1L( r, KErrNotFound );
    CleanupStack::PopAndDestroy( helper );

    SetResult( aResult, KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::SetAutomatedSilentTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::SetAutomatedSilentTestL( TTestResult& aResult )
    {
    TInt r( KErrGeneral );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );

    _LIT( KStep1,
        "Step1: DRMHelper::SetAutomatedSilent with TDesC name \
         of plain File and ETrue \
         Expecting: KErrNone" );
    iLog->Log( KStep1 );
    r = helper->SetAutomatedSilent(
        const_cast< TDesC& >( KPlainTextFile() ), ETrue );
    T1L( r, KErrNone );

    _LIT( KStep2,
          "Step2: DRMHelper::SetAutomatedSilent with RFile handle \
            of plain file and EFalse \
          Expecting: KErrNone" );
    iLog->Log( KStep2 );

    RFile file;
    r = file.Open( iFs, KPlainTextFile, EFileRead | EFileShareAny );
    T1L( r, KErrNone );
    CleanupClosePushL( file );
    r = helper->SetAutomatedSilent( file, EFalse );
    T1L( r, KErrNone );
    CleanupStack::PopAndDestroy( &file );


    _LIT( KStep3,
        "Step3: DRMHelper::SetAutomatedSilent with nonexistent TDesC8 cid \
          Expecting: KErrNotFound" );
    iLog->Log( KStep3 );
    r = helper->SetAutomatedSilent( KCidNotExist(), ETrue );
    T1L( r, KErrNotFound );

    CleanupStack::PopAndDestroy( helper );

    SetResult( aResult, KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::SetAutomatedTypeTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::SetAutomatedTypeTestL( TTestResult& aResult )
    {
    TInt r( KErrGeneral );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    _LIT( KStep1,
        "Step1: CDRMHelper::SetAutomatedType  to EAutomatedTypeOther \
        Expecting: no errors");
    iLog->Log( KStep1 );
    r = helper->SetAutomatedType( CDRMHelper::EAutomatedTypeOther );
    T1L( r, KErrNone );

    _LIT( KStep2,
        "Step2: CDRMHelper::SetAutomatedType to EAutomatedTypeRingingTone \
        Expecting: no errors");
    iLog->Log( KStep2 );
    r = helper->SetAutomatedType( CDRMHelper::EAutomatedTypeRingingTone );
    T1L( r, KErrNone );

    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, r );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::SetLimitsTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::SetLimitsTestL( TTestResult& aResult )
    {
    TInt r( KErrNone );
    TUint limit( KErrNone );
    CDRMHelper* helper( CDRMHelper::NewLC() );

    _LIT( KStep3,
        "Step3: CDRMHelper::SetTimeLimitL \
        Expecting: no errors");
    iLog->Log( KStep3 );
    TRAP( r, helper->SetTimeLimitL( limit ) );
    T1L( r, KErrNone );

    _LIT( KStep1,
        "Step1: CDRMHelper::SetCountLimitL \
        Expecting: no errors");
    iLog->Log( KStep1 );
    TRAP( r, helper->SetCountLimitL( limit ) );
    T1L( r, KErrNone );

    _LIT( KStep2,
        "Step2: CDRMHelper::SetPercentageLimitL \
        Expecting: no errors");
    iLog->Log( KStep2 );
    TRAP( r, helper->SetPercentageLimitL( limit ) );
    T1L( r, KErrNone );
    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, r );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::SetPreviewMediaTypeTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::SetPreviewMediaTypeTestL( TTestResult& aResult )
    {
    TInt r( KErrGeneral );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    _LIT( KStep1,
        "Step1: CDRMHelper::SetPreviewMediaType  to EPreviewTypeAudio \
        Expecting: no errors");
    iLog->Log( KStep1 );
    r = helper->SetPreviewMediaType( EPreviewTypeAudio );
    T1L( r, KErrNone );

    _LIT( KStep2,
        "Step2: CDRMHelper::SetPreviewMediaType to EPreviewTypeVideo \
        Expecting: no errors");
    iLog->Log( KStep2 );
    r = helper->SetPreviewMediaType( EPreviewTypeVideo );
    T1L( r, KErrNone );

    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, r );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CDrmHelperTest::ShowAutomatedNoteTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::ShowAutomatedNoteTestL( TTestResult& aResult )
    {
    TInt r( KErrNone );
    TEntry entry;
    _LIT( KNonExistentFile , "C:\\nonexistent.txt");
    r = iFs.Entry( KNonExistentFile, entry );
    T2L( r, KErrNotFound , KErrPathNotFound );

    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    _LIT( KStep1,
        "Step1: DRMHelper::ShowAutomatedNote with TDesC name \
         of nonexistent file \
         Expecting: error KErrNotFound" );
    iLog->Log( KStep1 );

    r = helper->ShowAutomatedNote( KNonExistentFile );
    T2L( r, KErrNotFound, KErrPathNotFound );

    _LIT( KStep2,
        "Step2: DRMHelper::ShowAutomatedNote with nonexistent TDesC8 CID\
         Expecting: error KErrNotFound" );
    iLog->Log( KStep2 );
    r = helper->ShowAutomatedNote( KCidNotExist );
    T1L( r, KErrNotFound );

    // preparations for step 3
    AddROL( KRO1Formatter, KCid1Jpeg, KCek1Jpeg, KDisplayFull );
    RFile file;
    r = file.Open( iFs, KDcf1JpegFile, EFileRead | EFileShareAny );
    T1L( r, KErrNone );
    CleanupClosePushL( file );
    _LIT( KStep3,
        "Step3: DRMHelper::ShowAutomatedNote with RFile handle \
         of OMA 1 file with full rights \
         Expecting: no errors and no notes shown" );
    iLog->Log( KStep3 );
    r = helper->ShowAutomatedNote( file );
    T1L( r, KErrNone );

    CleanupStack::PopAndDestroy( &file );

    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::ShowDRMUINotification2LTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::ShowDRMUINotification2LTestL( TTestResult& aResult )
    {
    TInt r( KErrNone );
    CDRMHelper::TDRMHelperNotificationID
        notificationId( CDRMHelper::ETCountBasedErrorTextId  );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );

    _LIT( KStep1,
        "Step1: legacy DRMHelper::ShowDRMUINotification2L \
         with nonexistent TDesC8& CID \
         Expecting: always error KErrNotSupported" );
    iLog->Log( KStep1 );
    r = helper->ShowDRMUINotification2L( notificationId, KCidNotExist );
    T1L( r, KErrNotSupported );

    _LIT( KStep2,
        "Step2: legacy DRMHelper::ShowDRMUINotification2L \
         with name of plain file \
         Expecting: always error KErrNotSupported" );
    iLog->Log( KStep2 );
    r = helper->ShowDRMUINotification2L( notificationId, KPlainTextFile );
    T1L( r, KErrNotSupported );

    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::SupportedDRMMethods2TestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::SupportedDRMMethods2TestL( TTestResult& aResult )
    {
    TInt r( KErrNone );
    TInt drmMethod( KErrNone );
    CDRMHelper::TDRMHelperOMALevel omaLevel( CDRMHelper::EOMA_None );
    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    r = helper->SupportedDRMMethods2( drmMethod, omaLevel );
    T1L( drmMethod & CDRMHelper::EForwardLock, CDRMHelper::EForwardLock );
    T2L(omaLevel, CDRMHelper::EOMA_1_0, CDRMHelper::EOMA_2_0 );
    T1L( r, KErrNone );
    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, r );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::SupportedDataTypeTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::SupportedDataTypeTestL( TTestResult& aResult )
    {

    TInt r( KErrNone );
    TInt dataTypesCount( KErrNotFound );
    const TInt KNegativeIndex( KErrNotFound );
    _LIT8( KUnSupportedMimeType, "x-jalla/x-jallajalla");
    TDataType dataType( KUnSupportedMimeType );
    const TDataType compared( KUnSupportedMimeType );

    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    //Step1 Querying data types count
    r = helper->DataTypesCount( dataTypesCount );
    TL( dataTypesCount > 0 );

    //Step2 Querying first supported data type
    r = helper->SupportedDataType( 0 , dataType );
    T1L( r, KErrNone );
    TL( dataType != compared );
    //Step3 Querying last supported data type
    r = helper->SupportedDataType( dataTypesCount - 1 , dataType );
    T1L( r, KErrNone );
    TL( dataType != compared );

    //Step4 Querying supported data type from negative index
    r = helper->SupportedDataType( KNegativeIndex , dataType );
    T1L( r, KErrArgument );

    //Step5 Querying supported data type from negative index
    r = helper->SupportedDataType( dataTypesCount , dataType );
    T1L( r, KErrArgument );

    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDrmHelperTest::UpdateDataTypesTestL
// -----------------------------------------------------------------------------
//
TInt CDrmHelperTest::UpdateDataTypesTestL( TTestResult& aResult )
    {
    TInt r( KErrNone );
    _LIT8( KFirstMimeType, "x-jalla/x-ekajalla");
    _LIT8( KSecondMimeType, "x-jalla/x-tokajalla");
    _LIT8( KUnSupportedMimeType, "x-jalla/x-jallajalla");
    const TDataType firstNewType( KFirstMimeType );
    const TDataType secondNewType( KSecondMimeType );
    TInt dataTypesCount( KErrNotFound );
    TInt originalDataTypesCount( KErrNotFound );
    TDataType queriedType( KUnSupportedMimeType );

    CDRMHelper* helper( CDRMHelper::NewLC( iFs ) );
    // preparations
    r = helper->DataTypesCount( originalDataTypesCount );
    T1L( r, KErrNone );

    // Step 1: register first new data type
    _LIT( KStep1,
        "Step1: CDRMHelper::RegisterDataType \
        Expecting: new registered datatype");
    iLog->Log( KStep1 );
    r = helper->RegisterDataType( firstNewType );
    T1L( r, KErrNone );
    r = helper->DataTypesCount( dataTypesCount );
    T1L( r, KErrNone );
    TL( dataTypesCount == originalDataTypesCount + 1 );
    r = helper->SupportedDataType( originalDataTypesCount, queriedType );
    T1L( r, KErrNone );
    TL( queriedType == firstNewType );

    _LIT( KStep2,
        "Step2: CDRMHelper::RegisterDataType \
        Expecting: second new registered datatype");
    iLog->Log( KStep2 );
    // Step 2: registering second data type
    r = helper->RegisterDataType( secondNewType );
    T1L( r, KErrNone );
    r = helper->DataTypesCount( dataTypesCount );
    T1L( r, KErrNone );
    TL( dataTypesCount == originalDataTypesCount + 2 );
    r = helper->SupportedDataType( originalDataTypesCount, queriedType );
    T1L( r, KErrNone );
    TL( queriedType == secondNewType );

    _LIT( KStep3,
        "Step3: CDRMHelper::UnRegisterDataType \
        Expecting: First registered type unregistered");
    iLog->Log( KStep3 );
    // Step 3: unregistering first new type
    r = helper->UnRegisterDataType( originalDataTypesCount + 1 );
    T1L( r, KErrNone );
    r = helper->DataTypesCount( dataTypesCount );
    T1L( r, KErrNone );
    TL( dataTypesCount == originalDataTypesCount + 1 );
    r = helper->SupportedDataType( originalDataTypesCount, queriedType );
    T1L( r, KErrNone );
    TL( queriedType == secondNewType );

    // Step 4: unregistering second new type
    _LIT( KStep4,
        "Step4: CDRMHelper::UnRegisterDataType \
        Expecting: second registered type unregistered");
    iLog->Log( KStep4 );
    r = helper->UnRegisterDataType( originalDataTypesCount );
    T1L( r, KErrNone );
    r = helper->DataTypesCount( dataTypesCount );
    T1L( r, KErrNone );
    TL( dataTypesCount == originalDataTypesCount );

    CleanupStack::PopAndDestroy( helper );
    SetResult( aResult, r );
    return KErrNone;
    }

//  End of File
