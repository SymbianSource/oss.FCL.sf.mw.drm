/*
* Copyright (c) 2007 - 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DrmRightsTest hard coded test cases
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <apmstd.h>
#include <f32file.h>
#include <s32std.h>
#include <s32file.h>
#include <drmcommon.h>

#include "drmpermission.h"
#include "drmpointerarray.h"
#include "drmrights.h"
#include "drmrightsclient.h"
#include "drmrightsparser.h"
#include "drmrightstest.h"

// CONSTANTS

const TInt KCountsLeft = 99;
const TInt KNewCount = 50;
const TInt KNewOriginalCount = 70;
const TInt KNewIntervalSeconds = 300;
const TInt KTimedCount = 100;
const TInt KUriSize = 20;

//#ifdef __WINSCW__
_LIT( KOma1Content, "c:\\testing\\data\\content.dcf" );
_LIT( KStoreFile, "c:\\testing\\data\\asset.dat" );
_LIT( KStoreFile2, "c:\\testing\\data\\permission.dat" );
_LIT( KStoreFile3, "c:\\testing\\data\\rights.dat" );
//#else
//_LIT( KOma1Content, "e:\\testing\\data\\drm\\content.dcf" );
//_LIT( KStoreFile, "e:\\testing\\data\\drm\\asset.dat" );
//_LIT( KStoreFile2, "e:\\testing\\data\\drm\\permission.dat" );
//_LIT( KStoreFile3, "e:\\testing\\data\\drm\\rights.dat" );
//#endif

_LIT8( KContentId1, "cid:http://test.content@test.com" );
_LIT8( KContentId2, "cid:" );
_LIT8( KContentIdParent, "ro-parent2" );
_LIT8( KUri, "test.encrypt");

_LIT(KNullDate,"00000000:000000.000000");

_LIT8( KROHead1, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\
<!DOCTYPE o-ex:rights PUBLIC \"-//OMA//DTD DRMREL 1.0//EN\" \
\"http://www.oma.org/dtd/dr\">\
<o-ex:rights \
xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" \
xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" \
xmlns:oma-dd=\"http://www.openmobilealliance.com/oma-dd\" \
xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\">\
<o-ex:context><o-dd:version>1.0</o-dd:version>\
</o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context>\
<o-dd:uid>" );

_LIT8( KROHead2, "<o-ex:rights \
xmlns:o-ex = \"http://odrl.net/1.1/ODRL-EX\" \
xmlns:o-dd = \"http://odrl.net/1.1/ODRL-DD\" \
xmlns:ds = \"http://www.w3.org/2000/09/xmldsig#/\">\
<o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context>\
<o-ex:agreement><o-ex:asset><o-ex:context>\
<o-dd:uid>" );

_LIT8( KROParentHeadPlay, "<rights \
xmlns:o-ex= \"http://odrl.net/1.1/ODRL-EX\" o-ex:id= \"REL0\"><o-ex:context>\
<o-dd:version xmlns:o-dd= \"http://odrl.net/1.1/ODRL-DD\">\
2.0</o-dd:version><o-dd:uid xmlns:o-dd= \"http://odrl.net/1.1/ODRL-DD\">\
offer-parent2-full.offer-parent2-full.6otCOdBKVXzA0WLE</o-dd:uid>\
</o-ex:context><o-ex:agreement><o-ex:asset o-ex:id= \"a1\"><o-ex:context>\
<o-dd:uid xmlns:o-dd= \"http://odrl.net/1.1/ODRL-DD\">ro-parent2)" );

_LIT8( KROParentTailPlay, "</o-dd:uid></o-ex:context></o-ex:asset>\
<o-ex:permission><o-ex:asset o-ex:idref= \"a1\"></o-ex:asset>\
<o-dd:play xmlns:o-dd= \"http://odrl.net/1.1/ODRL-DD\">\
</o-dd:play></o-ex:permission></o-ex:agreement></rights>" );

_LIT8( KROTailAccumulatedPlay, "</o-dd:uid></o-ex:context><ds:KeyInfo>\
<ds:KeyValue>MyK9p+SvfH1vJhAR/9Uhww==</ds:KeyValue></ds:KeyInfo>\
</o-ex:asset><o-ex:permission><o-dd:play><o-ex:constraint>\
<o-dd:accumulated>P5DT10H00M30S</o-dd:accumulated></o-ex:constraint>\
</o-dd:play></o-ex:permission></o-ex:agreement></o-ex:rights>" );

_LIT8( KROTailCounterExecute, "</o-dd:uid></o-ex:context><ds:KeyInfo>\
<ds:KeyValue>NyxU8kZzX280ssYV9aCvgQ==</ds:KeyValue></ds:KeyInfo>\
</o-ex:asset><o-ex:permission><o-dd:execute><o-ex:constraint>\
<o-dd:count>99</o-dd:count></o-ex:constraint></o-dd:execute>\
</o-ex:permission></o-ex:agreement></o-ex:rights>" );

_LIT8( KROTailPreviewCounterExecute, "</o-dd:uid></o-ex:context><ds:KeyInfo>\
<ds:KeyValue>NyxU8kZzX280ssYV9aCvgQ==</ds:KeyValue></ds:KeyInfo>\
</o-ex:asset><o-ex:permission><o-dd:execute><o-ex:constraint>\
<o-dd:count>1</o-dd:count></o-ex:constraint></o-dd:execute>\
</o-ex:permission></o-ex:agreement></o-ex:rights>" );

_LIT8(KROTailDisplayPrint, "</o-dd:uid></o-ex:context><ds:KeyInfo>\
<ds:KeyValue>NyxU8kZzX280ssYV9aCvgQ==</ds:KeyValue></ds:KeyInfo>\
</o-ex:asset><o-ex:permission><o-dd:display></o-dd:display><o-dd:print>\
</o-dd:print></o-ex:permission></o-ex:agreement></o-ex:rights>" );

_LIT8( KROTailFull, "</o-dd:uid></o-ex:context>\
<ds:KeyInfo><ds:KeyValue>PgIkd7w3E5TWWcc7Inp8Yw==</ds:KeyValue>\
</ds:KeyInfo></o-ex:asset><o-ex:permission>\
<o-dd:play><o-ex:constraint></o-ex:constraint></o-dd:play>\
<o-dd:display><o-ex:constraint></o-ex:constraint></o-dd:display>\
<o-dd:execute><o-ex:constraint></o-ex:constraint></o-dd:execute>\
<o-dd:print><o-ex:constraint></o-ex:constraint></o-dd:print>\
</o-ex:permission></o-ex:agreement></o-ex:rights>" );

_LIT8( KROTailIntervalExecute, "</o-dd:uid></o-ex:context><ds:KeyInfo>\
<ds:KeyValue>NyxU8kZzX280ssYV9aCvgQ==</ds:KeyValue></ds:KeyInfo>\
</o-ex:asset><o-ex:permission><o-dd:execute><o-ex:constraint>\
<o-dd:interval>P7DT20M</o-dd:interval></o-ex:constraint>\
</o-dd:execute></o-ex:permission></o-ex:agreement></o-ex:rights>" );

_LIT8( KROTailPlay, "</o-dd:uid></o-ex:context><ds:KeyInfo>\
<ds:KeyValue>NyxU8kZzX280ssYV9aCvgQ==</ds:KeyValue>\
</ds:KeyInfo></o-ex:asset><o-ex:permission>\
<o-dd:play/></o-ex:permission></o-ex:agreement></o-ex:rights>" );

_LIT8( KROTailStartEndExecute, "</o-dd:uid></o-ex:context><ds:KeyInfo>\
<ds:KeyValue>NyxU8kZzX280ssYV9aCvgQ==</ds:KeyValue></ds:KeyInfo>\
</o-ex:asset><o-ex:permission><o-dd:execute><o-ex:constraint>\
<o-dd:datetime><o-dd:start>2003-05-12T15:35:56Z</o-dd:start>\
<o-dd:end>2005-01-30T15:35:56Z</o-dd:end></o-dd:datetime>\
</o-ex:constraint></o-dd:execute></o-ex:permission>\
</o-ex:agreement></o-ex:rights>" );

_LIT8( KROTailStartEndIntervalPlay, "</o-dd:uid></o-ex:context><ds:KeyInfo>\
<ds:KeyValue>MyK9p+SvfH1vJhAR/9Uhww==</ds:KeyValue></ds:KeyInfo>\
</o-ex:asset><o-ex:permission><o-dd:play><o-ex:constraint>\
<o-dd:datetime><o-dd:start>2007-05-12T15:35:56</o-dd:start>\
<o-dd:end>2008-01-30T15:35:56</o-dd:end></o-dd:datetime>\
<o-dd:interval>P15DT10H30M20S</o-dd:interval></o-ex:constraint>\
</o-dd:play></o-ex:permission></o-ex:agreement>\
</o-ex:rights>" );

_LIT8( KROTailTimedPlay, "</o-dd:uid></o-ex:context><ds:KeyInfo>\
<ds:KeyValue>MyK9p+SvfH1vJhAR/9Uhww==</ds:KeyValue></ds:KeyInfo>\
</o-ex:asset><o-ex:permission><o-dd:play><o-ex:constraint>\
<o-dd:timed-count timer=\"60\">100</o-dd:timed-count>\
</o-ex:constraint></o-dd:play></o-ex:permission></o-ex:agreement>\
</o-ex:rights>" );

_LIT( KRunning, "Running" );
_LIT( KFinished, "Finished" );
_LIT( KPassed, "Passed" );

// LOCAL FUNCTION PROTOTYPES
LOCAL_C void DeleteRODBL( );
LOCAL_C void AddROGetRightsL( const TDesC8& aHead, const TDesC8& aCID, 
    const TDesC8& aTail, RPointerArray<CDRMRights>& aRights  );


// ============================= LOCAL FUNCTIONS ==============================
    
// ----------------------------------------------------------------------------
// AddROGetRightsL
// Helper function with which test cases can add ROs and get the added rights
// (in the CDRMPointerArray).
// ----------------------------------------------------------------------------
//
LOCAL_C void AddROGetRightsL( const TDesC8& aHead, const TDesC8& aCID, 
    const TDesC8& aTail, RPointerArray<CDRMRights>& aRights )
    {
    
    aRights.ResetAndDestroy();
    
	TInt roSize( aHead.Size() + aCID.Size() + aTail.Size() );
    HBufC8* buf( HBufC8::NewLC( roSize ) );
    TPtr8 tmp( buf->Des() );
    
    tmp.Append( aHead );
    tmp.Append( aCID );
    tmp.Append( aTail );
    
    CDrmRightsParser* rp( CDrmRightsParser::NewL() );
    CleanupStack::PushL( rp );
    
    rp->ParseAndStoreL( *buf, aRights );
    
    CleanupStack::PopAndDestroy( 2, buf ); // buf, rp
    
	}

// ----------------------------------------------------------------------------
// DeleteRODBL
// Helper function with which test cases can delete RO database.
// ----------------------------------------------------------------------------
//
LOCAL_C void DeleteRODBL( )
    {
	RDRMRightsClient client;
    User::LeaveIfError( client.Connect() );
    client.DeleteAll();
    client.Close();
    }

// ----------------------------------------------------------------------------
// TDoResetAndDestroy
// Template method used to push RPointerArrays to the cleanup stack. Takes
// care of deleting all pointers in the array.
// ----------------------------------------------------------------------------
template < class T > class TDoResetAndDestroy
    {
    public:
        TDoResetAndDestroy( T& aT ):iObj( &aT )
            {
            };
        void PushL()
            {
            TCleanupItem item( &Reset, iObj );
            CleanupStack::PushL( item );
            };
                         
        void Pop()
            {
            CleanupStack::Pop();
            };
        static void Reset( TAny* aPtr )
            {
            __ASSERT_ALWAYS( aPtr, User::Invariant() );
            reinterpret_cast< T* >( aPtr )->ResetAndDestroy();
            };
                
    private:
        TDoResetAndDestroy(); // prohibit
    
    private:
        T* iObj;
    };

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CDrmRightsTest::Case
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
// ----------------------------------------------------------------------------
//
const TCaseInfo CDrmRightsTest::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    DRMRightsTest.cpp file and to DRMRightsTest.h 
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
        // CDRMRightsTest::PrintTest. Otherwise the compiler
        // gives errors.
        ENTRY( "Rig::ConstructDcfCommonFileNameTestL", 
                CDrmRightsTest::ConstructDcfCommonFileNameTestL ),
        ENTRY( "Rig::ConstructDcfCommonFileHandleTestL", 
                CDrmRightsTest::ConstructDcfCommonFileHandleTestL ),
        ENTRY( "Rig::ConstructAssetTestL", 
                CDrmRightsTest::ConstructAssetTestL ),
        ENTRY( "Rig::AssetDuplicateTestL", 
                CDrmRightsTest::AssetDuplicateTestL ),
        ENTRY( "Rig::AssetExternalizeInternalizeTestL", 
                CDrmRightsTest::AssetExternalizeInternalizeTestL ),
        ENTRY( "Rig::ConstructPermissionTestL", 
                CDrmRightsTest::ConstructPermissionTestL ),
        ENTRY( "Rig::PermissionSizeTestL", 
                CDrmRightsTest::PermissionSizeTestL ),
        ENTRY( "Rig::PermissionExternalizeInternalizeTestL", 
                CDrmRightsTest::PermissionExternalizeInternalizeTestL ),
        ENTRY( "Rig::PermissionStatefulTestL",
                CDrmRightsTest::PermissionStatefulTestL ),
        ENTRY( "Rig::PermissionChildTestL",
                CDrmRightsTest::PermissionChildTestL ),
        ENTRY( "Rig::PermissionConstraintForIntentTestL",
                CDrmRightsTest::PermissionConstraintForIntentTestL ), 
        ENTRY( "Rig::PermissionTopLevelConstraintTestL",
                CDrmRightsTest::PermissionTopLevelConstraintTestL ),
        ENTRY( "Rig::PermissionConsumeRightsTestL",
                CDrmRightsTest::PermissionConsumeRightsTestL ),
        ENTRY( "Rig::PermissionExportImportTestL",
                CDrmRightsTest::PermissionExportImportTestL ),
        ENTRY( "Rig::PermissionDuplicateTestL",
                CDrmRightsTest::PermissionDuplicateTestL ),
        ENTRY( "Rig::PermissionExpiredTestL",
                CDrmRightsTest::PermissionExpiredTestL ),
        ENTRY( "Rig::PermissionValidTestL",
                CDrmRightsTest::PermissionValidTestL ),
        ENTRY( "Rig::PermissionMergeTestL",
                CDrmRightsTest::PermissionMergeTestL ),
        ENTRY( "Rig::PermissionSoftwareConstrainedTestL",
                CDrmRightsTest::PermissionSoftwareConstrainedTestL ),
        ENTRY( "Rig::ConstructConstraintTestL",
                CDrmRightsTest::ConstructConstraintTestL ),
        ENTRY( "Rig::ConstructRightsConstraintsTestL",
                CDrmRightsTest::ConstructRightsConstraintsTestL ),
        ENTRY( "Rig::RightsConstraintsFullRightsTestL",
                CDrmRightsTest::RightsConstraintsFullRightsTestL ),
        ENTRY( "Rig::RightsConstraintsIsPreviewTestL",
                CDrmRightsTest::RightsConstraintsIsPreviewTestL ),
        ENTRY( "Rig::RightsConstraintsGetConstraintInfoTestL",
                CDrmRightsTest::RightsConstraintsGetConstraintInfoTestL ),
        ENTRY( "Rig::RightsConstraintsGetExpirationDetailsTestL",
                CDrmRightsTest::RightsConstraintsGetExpirationDetailsTestL ),
        ENTRY( "Rig::RightsConstraintsGetCountersTestL",
                CDrmRightsTest::RightsConstraintsGetCountersTestL ),
        ENTRY( "Rig::RightsConstraintsGetTimedCountersTestL",
                CDrmRightsTest::RightsConstraintsGetTimedCountersTestL ),
        ENTRY( "Rig::RightsConstraintsGetStartTimeTestL",
                CDrmRightsTest::RightsConstraintsGetStartTimeTestL ),
        ENTRY( "Rig::RightsConstraintsGetEndTimeTestL",
                CDrmRightsTest::RightsConstraintsGetEndTimeTestL ),
        ENTRY( "Rig::RightsConstraintsGetIntervalTestL",
                CDrmRightsTest::RightsConstraintsGetIntervalTestL ),
        ENTRY( "Rig::RightsConstraintsGetIntervalStartTestL", 
                CDrmRightsTest::RightsConstraintsGetIntervalStartTestL ),
        ENTRY( "Rig::RightsConstraintsGetAccumulatedTestL",
                CDrmRightsTest::RightsConstraintsGetAccumulatedTestL ),
        ENTRY( "Rig::RightsConstraintsSetCountersTestL",
                CDrmRightsTest::RightsConstraintsSetCountersTestL ),
        ENTRY( "Rig::RightsConstraintsSetStartTimeTestL",
                CDrmRightsTest::RightsConstraintsSetStartTimeTestL ),
        ENTRY( "Rig::RightsConstraintsSetEndTimeTestL",
                CDrmRightsTest::RightsConstraintsSetEndTimeTestL ),
        ENTRY( "Rig::RightsConstraintsSetIntervalTestL",
                CDrmRightsTest::RightsConstraintsSetIntervalTestL ),
        ENTRY( "Rig::RightsConstraintsSetIntervalStartTestL",
                CDrmRightsTest::RightsConstraintsSetIntervalStartTestL ),
        ENTRY( "Rig::RightsConstraintsGetConstraintTestL",
                CDrmRightsTest::RightsConstraintsGetConstraintTestL ),
        ENTRY( "Rig::RightsConstraintsSetConstraintTestL",
                CDrmRightsTest::RightsConstraintsSetConstraintTestL ),
        ENTRY( "Rig::RightsConstraintsAssignmentTestL",
                CDrmRightsTest::RightsConstraintsAssignmentTestL ),
        ENTRY( "Rig::RightsGetLocalIdTestL",
                CDrmRightsTest::RightsGetLocalIdTestL ),
        ENTRY( "Rig::RightsGetAddTimeL",
                CDrmRightsTest::RightsGetAddTimeTestL ),
        ENTRY( "Rig::RightsGetRightsInfoTestL",
                CDrmRightsTest::RightsGetRightsInfoTestL ),
        ENTRY( "Rig::RightsGetExpirationDetailsTestL",
                CDrmRightsTest::RightsGetExpirationDetailsTestL ),
        ENTRY( "Rig::RightsGetPrintRightTestL",
                CDrmRightsTest::RightsGetPrintRightTestL ),
        ENTRY( "Rig::RightsSetContentURIandLocalIDTestL",
                CDrmRightsTest::RightsSetContentURIAndLocalIDTestL ),
        ENTRY( "Rig::RightsSetContentURITestL",
                CDrmRightsTest::RightsSetContentURITestL ),
        ENTRY( "Rig::RightsSetAddTimeTestL",
                CDrmRightsTest::RightsSetAddTimeTestL ),
        ENTRY( "Rig::RightsSetPlayRightTestL",
                CDrmRightsTest::RightsSetPlayRightTestL ),
        ENTRY( "Rig::RightsSetDisplayRightTestL",
                CDrmRightsTest::RightsSetDisplayRightTestL ),
        ENTRY( "Rig::RightsSetPrintRightTestL",
                CDrmRightsTest::RightsSetPrintRightTestL ),
        ENTRY( "Rig::RightsSetExecuteRightTestL",
                CDrmRightsTest::RightsSetExecuteRightTestL ),
        ENTRY( "Rig::RightsAssignmentTestL",
                CDrmRightsTest::RightsAssignmentTestL ),
        ENTRY( "Rig::RightsExternalizeInternalizeTestL",
                CDrmRightsTest::RightsExternalizeInternalizeTestL ),
        ENTRY( "Rig::RightsMergeTestL",
                CDrmRightsTest::RightsMergeTestL ),                                                       
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

// ----------------------------------------------------------------------------
// ConstructDcfCommonFileNameTestL
// Constructor test, constructs DcfCommon object from the file
// of which the name is given as an argument.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::ConstructDcfCommonFileNameTestL( TTestResult& aResult ) 
    {
    
    _LIT( KData, "CDrmRights::ConstructDcfCommonFileNameTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase1: Testing NewL of DcfCommon: Filename given" );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "as an argument, expecting functional DcfCommon" );
    iLog->Log( KTestCase2 ); 
    
    CDcfCommon* dcf = CDcfCommon::NewL( KOma1Content );
    TL( dcf );
    delete dcf;
    
    // Passed
    iLog->Log( KPassed );

    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData );

    // Test case passed
    _LIT( KDescription, "CDrmRightsTest::ConstructDcfCommonFileNameTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// ConstructDcfCommonFileHandleTestL
// Constructor test, constructs DcfCommon object from the file
// to which a file handle is given as an argument.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::ConstructDcfCommonFileHandleTestL( TTestResult& aResult ) 
    {
    
    _LIT( KData, "CDrmRights::ConstructDcfCommonFileHandleTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    RFile file;
    RFs GFs;
    User::LeaveIfError( GFs.Connect() );
    CleanupClosePushL( GFs );
    
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase2: Testing NewL of DcfCommon: File handle" );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "given as an argument, expecting functional DcfCommon" );
    iLog->Log( KTestCase2 );
    
    User::LeaveIfError( file.Open( GFs, KOma1Content, 
        EFileRead | EFileShareReadersOrWriters ) );
    CleanupClosePushL( file );
    
    CDcfCommon* dcf = CDcfCommon::NewL( file );
    TL( dcf );
    delete dcf;
    CleanupStack::PopAndDestroy(2, &GFs ); // file
    
    // Passed
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescription, "CDrmRightsTest::ConstructDcfCommonFileHandleTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// ConstructAssetTestL
// Constructor tests, constructs CDrmAsset object.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::ConstructAssetTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::ConstructAssetTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase3: Testing NewL of CDrmAsset, expecting" );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "functional CDrmAsset" );
    iLog->Log( KTestCase2 );
    
    CDRMAsset* asset = CDRMAsset::NewL();
    TL( asset );
    delete asset;
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase3, "TestCase4: Testing NewLC of CDrmAsset, expecting" ); 
    iLog->Log( KTestCase3 );
    
    _LIT( KTestCase4, "functional CDrmAsset" );
    iLog->Log( KTestCase4 );
    
    CDRMAsset* asset2 = CDRMAsset::NewLC();
    TL( asset2 );
    CleanupStack::PopAndDestroy( asset2 );
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescription, "CDrmRightsTest::ConstructAssetTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    
    }

// ----------------------------------------------------------------------------
// AssetDuplicateTestL
// Duplication test, duplicates CDrmAsset object.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::AssetDuplicateTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::AssetDuplicateTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMAsset* duplicate( NULL );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase5: Testing duplicating of CDrmAsset,");
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "expecting functional duplicate of CDrmAsset" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId1, KROTailIntervalExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
     
    // Check the validity of the asset
    TL( &( rights[0]->GetAsset() ) );
    
    duplicate = CDRMAsset::NewLC();
    
    // Make a duplicate from the asset part of the rights.
    duplicate->DuplicateL( rights[0]->GetAsset() );
    
    TL( duplicate );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // item, duplicate
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescription, "CDrmRightsTest::AssetDuplicateTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;
    
    }

// ----------------------------------------------------------------------------
// AssetExternalizeInternalizeTestL
// Externalization/Internalization test, CDrmAsset object
// is first externalized and then internalized.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::AssetExternalizeInternalizeTestL ( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::AssetExternalizeInternalizeTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase6: Testing externalizing of CDrmAsset" );
    iLog->Log( KTestCase1 );
    
    RPointerArray<CDRMRights> rights;
    RFs GFs;
    RStoreWriteStream outStream;
    TStreamId rootId, assetId;
    
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
 
    User::LeaveIfError( GFs.Connect() );
    CleanupClosePushL( GFs );
    
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId1, KROTailCounterExecute, rights ); 
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
     
    // Check the validity of the asset
    TL( &( rights[0]->GetAsset() ) );
    
    // Create a file store for externalization   
    CDirectFileStore* store = CDirectFileStore::ReplaceLC( GFs, 
        KStoreFile, EFileWrite );
    
    // Set the UID of the file
    store->SetTypeL( KDirectFileStoreLayoutUid );
    
    // Declare and construct the output stream 
    assetId = outStream.CreateLC( *store );
    
    ( rights[0]->GetAsset() ).ExternalizeL( outStream );
    outStream.CommitL();
    
    // Set this stream Id to be the root Id
    store->SetRootL( assetId );
    
    // Flush the client side buffer to the store
    store->CommitL();
    
    CleanupStack::PopAndDestroy( 2, store ); // store, outStream
    store = NULL;
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase2, "TestCase7: Testing internalizing of CDrmAsset" );
    iLog->Log( KTestCase2 );
    
    RStoreReadStream inStream;
 
    // Open the file store
    store = CDirectFileStore::OpenLC( GFs, KStoreFile, 
        EFileRead );
    
    // Open the root stream
    inStream.OpenLC( *store, store->Root() );
    
    // Read the data
    ( rights[0]->GetAsset() ).InternalizeL( inStream );
    
    CleanupStack::PopAndDestroy( 4, &rights ); // item, GFs, store, inStream
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::AssetExternalizeInternalizeTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed 
    return KErrNone; 
    }

// ----------------------------------------------------------------------------
// ConstructPermissionTestL
// Constructor test, constructs CDrmPermission object
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::ConstructPermissionTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::ConstructPermissionTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    // Test Cases
    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase8: Testing NewL, expecting CDrmPermission" );
    iLog->Log( KTestCase1 );
    
    CDRMPermission* permission = CDRMPermission::NewL();
    TL( permission );
    delete permission;
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase2, "TestCase9: Testing NewLC, expecting CDrmPermission" );
    iLog->Log( KTestCase2 );
    
    CDRMPermission* permission2 = CDRMPermission::NewLC();
    TL( permission2 );
    CleanupStack::PopAndDestroy( permission2 );
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescription, "CDrmRightsTest::ConstructPermissionTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;
    
    }

// ----------------------------------------------------------------------------
// PermissionSizeTestL
// Size definition test, counts the size of CDrmPermission object.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::PermissionSizeTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::PermissionSizeTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    TInt size = 0;
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase10: Testing Size of CDrmPermission," ); 
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "expecting size > 0" );
    iLog->Log( KTestCase2 );
           
    RPointerArray<CDRMRights> rights;     
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId1, KROTailIntervalExecute, rights );
    
    // Get the permission
    CDRMPermission& permission = rights[0]->GetPermission();
    
    TL( &permission );
    
    // Check size of the permission
    size = permission.Size();
    
    TL( size > 0 );
    
    CleanupStack::PopAndDestroy( &rights );
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescription, "CDrmRightsTest::PermissionSizeTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;
   
    }

// ----------------------------------------------------------------------------
// PermissionExternalizeInternalizeTestL
// Externalization/Internalization test, CDrmPermission object
// is first externalized and then internalized.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::PermissionExternalizeInternalizeTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::PermissionExternalizeInternalizeL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase11: Testing externalize of CDrmPermission" );
    iLog->Log( KTestCase1 );
    
    RStoreWriteStream outStream;
    TStreamId rootId, permissionId;
    RFs GFs;
    
    RPointerArray<CDRMRights> rights;    
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
        
    DeleteRODBL();
    
    AddROGetRightsL( KROHead2, KContentId2, KROTailStartEndExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
     
    CDRMPermission& permission = rights[0]->GetPermission();
    
    TL( &permission );
    
    User::LeaveIfError( GFs.Connect() );
    CleanupClosePushL( GFs );
     
    CDirectFileStore* store = CDirectFileStore::ReplaceLC( GFs, 
        KStoreFile2, EFileWrite );
    
    store->SetTypeL( KDirectFileStoreLayoutUid );
     
    permissionId = outStream.CreateLC( *store );
    permission.ExternalizeL( outStream );
    outStream.CommitL();
    
    store->SetRootL( permissionId );
    store->CommitL();
    
    CleanupStack::PopAndDestroy( 2 ); // store, outStream 
    store = NULL;
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase2, "TestCase12: Testing internalize of CDrmPermission" );
    iLog->Log( KTestCase2 );
    
    RStoreReadStream inStream;
 
    // Open the file store
    store = CDirectFileStore::OpenLC( GFs, KStoreFile2, 
        EFileRead );
    
    // Open the root stream
    inStream.OpenLC( *store, store->Root() );
    
    // Read the data
    permission.InternalizeL( inStream );
    
    CleanupStack::PopAndDestroy( 4, &rights ); // rights, GFs, store, inStream
     
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::PermissionExternalizeInternTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;
    } 


// ----------------------------------------------------------------------------
// PermissionStatefulTestL
// Test for checking whether Permission is stateful or not.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::PermissionStatefulTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::PermissionStatefulTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    TBool ret( EFalse );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase13: Testing if RO stateful, expecting ETrue" );
    iLog->Log( KTestCase1 );

    RPointerArray<CDRMRights> rights;    
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
        
    DeleteRODBL();
    
    AddROGetRightsL( KROHead2, KContentId2, KROTailCounterExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get the permission
    CDRMPermission& permission = rights[0]->GetPermission();
    TL( &permission );
    
    ret = permission.Stateful();
    
    TL( ret );
    
    CleanupStack::PopAndDestroy( &rights );
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescription, "CDrmRightsTest::PermissionStatefulTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// PermissionChildTestL
// Test for checking whether rights is of parent or child type. 
// ----------------------------------------------------------------------------    
TInt CDrmRightsTest::PermissionChildTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::PermissionChildTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    TBool ret( EFalse );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase14: Testing if RO is child: expecting EFalse");
    iLog->Log( KTestCase1 );

    RPointerArray<CDRMRights> rights;    
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    //AddROGetRightsL( KROHead1, KContentId1, KROTailFull, rights );
    AddROGetRightsL( KROParentHeadPlay, KContentId1, KROParentTailPlay, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get the permission
    CDRMPermission& permission = rights[0]->GetPermission();
    TL( &permission );
    
    ret = permission.Child();
    
    TL( !ret );
    
    CleanupStack::PopAndDestroy( &rights );
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescription, "CDrmRightsTest::PermissionChildTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;
    }


// ----------------------------------------------------------------------------
// PermissionConstraintForIntentTestL
// Test for fetching constraint matching the given intent. 
// ----------------------------------------------------------------------------  
TInt CDrmRightsTest::PermissionConstraintForIntentTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::ConstraintForIntentTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMConstraint* constraint( NULL );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase15: Testing constraint for Play intent," );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "expecting functional CDrmConstraint" );
    iLog->Log( KTestCase2 );
   
    RPointerArray<CDRMRights> rights;        
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailPlay, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get the permission
    CDRMPermission& permission = rights[0]->GetPermission();
    TL( &permission );
    
    constraint = permission.ConstraintForIntent( ContentAccess::EPlay );
    TL( constraint );
    
    CleanupStack::PopAndDestroy( &rights );
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::PermissionConstrForIntentTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// PermissionTopLevelConstraintTestL
// Test for checking top level constraint of the permission. 
// ----------------------------------------------------------------------------  
TInt CDrmRightsTest::PermissionTopLevelConstraintTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::TopLevelConstraintTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMConstraint* top( NULL );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase16: Testing fetching top level constraint," );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "expecting NULL pointer" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;       
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROParentHeadPlay, KContentIdParent, KROParentTailPlay, 
        rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get the permission
    CDRMPermission& permission = rights[0]->GetPermission();
    TL( &permission );
    
    // Check whether top level constraint exists or not
    top = permission.TopLevelConstraint();
    
    // Check top level constraint
    TL( !top );
    
    CleanupStack::PopAndDestroy( &rights );
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::PermissionTopLevelConstraintTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// PermissionConsumeRightsTestL
// Test for consuming rights of the permission. 
// ----------------------------------------------------------------------------    
TInt CDrmRightsTest::PermissionConsumeRightsTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::ConsumeRightsTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    TTime currentTime ( Time::NullTTime() );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase17: Testing consuming rights" );
    iLog->Log( KTestCase1 );
    
    RPointerArray<CDRMRights> rights;     
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailIntervalExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get the permission
    CDRMPermission& permission = rights[0]->GetPermission();
    TL( &permission );
    
    currentTime.UniversalTime();
    permission.ConsumeRights( ContentAccess::EExecute, currentTime );
    
    CleanupStack::PopAndDestroy( &rights );
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescription, "CDrmRightsTest::ConsumeRightsTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;    
    }
// ----------------------------------------------------------------------------
// PermissionExportImportTestL
// Test for exporting and importing permission. 
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::PermissionExportImportTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::ExportTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    HBufC8* buf( NULL );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase18: Testing exporting CDrmPermission," );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "expecting HBufC8" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;     
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead1, KContentId1, KROTailFull, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get the permission
    CDRMPermission& permission = rights[0]->GetPermission();
    TL( &permission );
    
    // Export permission
    buf = permission.ExportL();
    
    // Check exported buffer
    TL( buf );
    
    CleanupStack::PopAndDestroy( &rights );
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase3, "TestCase19: Testing importing HBufC8" );
    iLog->Log( KTestCase3 );
    
    _LIT( KTestCase4, "for CDrmPermission" );
    iLog->Log( KTestCase4 );
    
    CDRMPermission* permission2 = CDRMPermission::NewLC();
    
    CleanupStack::PushL( buf );    
    permission2->ImportL( *buf );
    CleanupStack::PopAndDestroy( 2, permission2 ); // permission2, buf
  
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescription, "CDrmRightsTest::PermissionExportImportTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;    
    }

// ----------------------------------------------------------------------------
// PermissionDuplicateTestL
// Test for duplicating permission. 
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::PermissionDuplicateTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::PermissionDuplicateTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase20: Testing duplicating CDrmPermission," );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "expecting valid duplicate of CDrmPermission" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;     
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    CDRMPermission* duplicate = CDRMPermission::NewLC();
    TL( duplicate );
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailCounterExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get the permission
    CDRMPermission& permission = rights[0]->GetPermission();
    TL( &permission );
    
    // Duplicate permission
    duplicate->DuplicateL( permission );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, duplicate
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescription, "CDrmRightsTest::PermissionDuplicateTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;    
    }

// ----------------------------------------------------------------------------
// PermissionExpiredTestL
// Test for checking if permission is expired. 
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::PermissionExpiredTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::PermissionExpiredTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    TTime currentTime( Time::NullTTime() );
    TBool ret( EFalse );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase21: Testing if CDrmPermission is expired," );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "expecting ETrue" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;     
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailStartEndExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get the permission
    CDRMPermission& permission = rights[0]->GetPermission();
    TL( &permission );
    
    currentTime.UniversalTime();
    
    ret = permission.Expired( currentTime );
    TL( ret );
    
    CleanupStack::PopAndDestroy( &rights ); // rights
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescription, "CDrmRightsTest::PermissionExpiredTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;    
    }

// ----------------------------------------------------------------------------
// PermissionValidTestL
// Test for checking if permission is valid. 
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::PermissionValidTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::PermissionValidTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    TTime currentTime;
    TUint32 reason;
    TBool ret( EFalse );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase22: Testing if CDrmPermission is valid," );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "Expecting: ETrue" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    RPointerArray<HBufC8> imsi;
    
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
        
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead1, KContentId1, KROTailFull, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get the permission
    CDRMPermission& permission = rights[0]->GetPermission();
    TL( &permission );
    
    currentTime.UniversalTime();
    
    // Check if permission is valid
    ret = permission.Valid( currentTime, imsi, reason );
    TL( ret )
    
    CleanupStack::PopAndDestroy( &rights ); // rights
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescription, "CDrmRightsTest::PermissionValidTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;    
    }

// ----------------------------------------------------------------------------
// PermissionMergeTestL
// Test for merging two permissions. 
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::PermissionMergeTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::PermissionMergeTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase23: Test to merge two CDrmPermissions," );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "expecting functional merged CDrmPermission." );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add the first RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailCounterExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get the first permission
    CDRMPermission* permission1 = CDRMPermission::NewLC();
    permission1->DuplicateL( rights[0]->GetPermission() );
    TL( &permission1 );
    
    rights.ResetAndDestroy();
    
    // Add the second RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailPlay, rights );
    
    // Get the second permission
    CDRMPermission* permission2 = CDRMPermission::NewLC();
    permission2->DuplicateL( rights[0]->GetPermission() );
    TL( &permission2 );
    
    permission1->Merge( *permission2 );
    
    CleanupStack::PopAndDestroy( 3, &rights ); // rights, perm1, permission2
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescription, "CDrmRightsTest::PermissionMergeTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;    
    }


// ----------------------------------------------------------------------------
// PermissionSoftwareConstrainedTestL
// Test for checking if permission has software constraint or not. 
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::PermissionSoftwareConstrainedTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::SoftwareConstrainedTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    TBool ret( ETrue );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase24: Testing if CDrmPermission has software" );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "constraint, expecting EFalse" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
        
    // Delete RO database
    DeleteRODBL();
    
    // Add the first RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailStartEndExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
     
    CDRMPermission& permission = rights[0]->GetPermission();
    TL( &permission );
    
    ret = permission.SoftwareConstrained();
    T1L( ret, EFalse );
    
    CleanupStack::PopAndDestroy( &rights ); // rights
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescription, "CDrmRightsTest::SoftwareConstrainedTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;    
    }

// ----------------------------------------------------------------------------
// ConstructConstraintTestL
// Constructor test, constructs CDrmConstraint.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::ConstructConstraintTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::ConstructConstraintTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
       
    CDRMConstraint* constraint( NULL );
       
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase25: Testing NewLC of CDrmConstraint,");
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "expecting functional CDrmConstraint object" );
    iLog->Log( KTestCase2 );
    
    constraint = CDRMConstraint::NewLC();
    TL( constraint )
    CleanupStack::PopAndDestroy();
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescription, "CDrmRightsTest::ConstructConstraintTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;    
    }
    
// ----------------------------------------------------------------------------
// ConstructRightsConstraintsTestL
// Constructor test, constructs CDrmRightsConstraints object. 
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::ConstructRightsConstraintsTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::ConstructRightsConstraintsTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
       
    CDRMRightsConstraints* constraint( NULL );
       
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase26: Testing NewL of CDrmRightsConstraints:" ); 
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "expecting functional CDrmRightsConstraints");
    iLog->Log( KTestCase2 );
    
    constraint = CDRMRightsConstraints::NewL();
    TL( constraint )
    delete constraint;
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsConstraintsConstructTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }

// ----------------------------------------------------------------------------
// RightsConstraintsFullRightsTestL
// Test for checking if object has full rights or not.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsConstraintsFullRightsTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsConstraintsFullRightsTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase27: Testing if object has full rights," );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "expecting ETrue" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead1, KContentId1, KROTailFull, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get display rights
    error = rights[0]->GetDisplayRight( constraint );
    
    CleanupStack::PushL( constraint );
    
    // Check constraint and display rights.
    TL( constraint );
    T1L( error, DRMCommon::EOk );
    
    // Check whether we have full rights or not.
    T1L( ETrue, constraint->FullRights() );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsConstraintsFullRightsTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }

// ----------------------------------------------------------------------------
// RightsConstraintsIsPreviewTestL
// Test for checking if object has preview rights constraint.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsConstraintsIsPreviewTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsConstraintsIsPreviewTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase28: Testing if object has preview rights," );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "expecting ETrue" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailPreviewCounterExecute, 
        rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get exeecute rights
    error = rights[0]->GetExecuteRight( constraint );
    
    CleanupStack::PushL( constraint );
    
    // Check constraint and play rights.
    TL( constraint );
    T1L( error, KErrNone );
    
    // Check whether we have preview rights or not
    T1L( ETrue, constraint->IsPreview() );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsConstraintsFullRightsTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }


// ----------------------------------------------------------------------------
// RightsConstraintsGetConstraintInfoTestL
// Test for checking the expiration and constraint details of the object.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsConstraintsGetConstraintInfoTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsConstraintsGetConstraintInfoTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    TUint32 expiration;
    TUint32 constType;
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    TInt temp( CDRMRights::EInvalidRights);
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase29: Test to get object constraint info," ); 
    iLog->Log( KTestCase1 );
    
    _LIT( KTest2, "expecting: !EInvalidRights, EValidRights, ECountBased" );
    iLog->Log( KTest2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();  
  
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailCounterExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get execute rights
    error = rights[0]->GetExecuteRight( constraint );
    
    CleanupStack::PushL( constraint );
    
    // Check constraint and execute rights.
    TL( constraint );
    T1L( error, DRMCommon::EOk );
    
    // Check the expiration and constraint details of the object
    TL( temp != constraint->GetConstraintInfo( 
          expiration, constType ) );
    T1L( CDRMRights::EValidRights, expiration)
    T1L( CDRMRights::ECountBased, constType );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsConstrGetConstrInfoTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }

// ----------------------------------------------------------------------------
// RightsConstraintsGetExpirationDetailsTestL
// Test for checking the end time and amount of counts left of of the object.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsConstraintsGetExpirationDetailsTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsConstrGetExpirationDetailsTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    TTime endTime;
    TTime tempTime;
    TInt countsLeft;
    
    TInt error( KErrNone );
    CDRMRightsConstraints* constraint( NULL );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase30: Test to get end time and amount" );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "of counts left, expecting: 0, 99, DRMCommon:EOk" );
    iLog->Log( KTestCase2 ); 
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
        
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailCounterExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get execute rights
    error = rights[0]->GetExecuteRight( constraint );
    
    CleanupStack::PushL( constraint );
    
    // Check constraint and execute rights.
    TL( constraint );
    T1L( error, DRMCommon::EOk );
    
    // Default value
    endTime.Set( KNullDate );
    
    // Get expiration details
    error = constraint->GetExpirationDetails( endTime, countsLeft );
    
    // Initialize for comparison of the end time 
    tempTime.Set( KNullDate );
    
    // Check return value, end time and counts left
    T1L( error, DRMCommon::EOk );
    TL( endTime == tempTime );
    T1L( countsLeft, KCountsLeft );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsConstrGetExpirDetailsTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }
    
// ----------------------------------------------------------------------------
// RightsConstraintsGetCountersL
// Test for checking the current and original value of the count
// restriction.
// ---------------------------------------------------------------------------- 
TInt CDrmRightsTest::RightsConstraintsGetCountersTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsConstraintsGetCountersL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    TUint32 counter;
    TUint32 originalCounter;
    TTime currentTime;
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
     
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase31: Test to get counter details of" ); 
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "object, expecting: 98, 99, DRMCommon::EOk" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
      
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailCounterExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    CDRMPermission& permission = rights[0]->GetPermission();
    TL( &permission );
    
    currentTime.UniversalTime();
    
    // Consume one rights first
    permission.ConsumeRights( ContentAccess::EExecute, currentTime );
    
    // Get execute rights
    error = rights[0]->GetExecuteRight( constraint );
    
    CleanupStack::PushL( constraint );
    
    // Check constraint and execute rights.
    TL( constraint );
    T1L( error, DRMCommon::EOk);
    
    // Get counters
    error = constraint->GetCounters( counter, originalCounter );
    
    // Check return value, count (should be original count minus one) 
    // and original count
    T1L( error, DRMCommon::EOk );
    T1L( counter, KCountsLeft - 1 );
    T1L( originalCounter, KCountsLeft );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsConstraintsGetCountersTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }

// ----------------------------------------------------------------------------
// RightsConstraintsGetTimedCountersTestL
// Test for checking the current and original value of the timed
// count restriction.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsConstraintsGetTimedCountersTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsConstraintsGetTimedCountersTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    TUint32 timedCounter;
    TUint32 originalCounter;
    TTime currentTime;
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase32: Test to get timed counter details of" ); 
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "object, expecting 100, 0, DRMCommon:EOk" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailTimedPlay, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get play rights
    error = rights[0]->GetPlayRight( constraint );
    
    CleanupStack::PushL( constraint );
    
    // Check constraint and execute rights
    TL( constraint );
    T1L( error, DRMCommon::EOk);
    
    // Get timed count and original count
    error = constraint->GetTimedCounters( timedCounter, originalCounter );
    
    // Check return value, timed count and original count 
    T1L( error, DRMCommon::EOk );
    T1L( timedCounter, KTimedCount  );
    T1L( originalCounter, 0 );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsConstrGetTimedCountersTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }

// ----------------------------------------------------------------------------
// RightsConstraintsGetStartTimeTestL
// Test for checking the start time constraint.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsConstraintsGetStartTimeTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsConstraintsGetStartTimeTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    TTime currentTime( Time::NullTTime() );
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase33: Test to get start time constraint of" ); 
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "object, expecting: start time, DRMCommon::EOk" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailStartEndExecute, rights );

    // Get execute rights
    error = rights[0]->GetExecuteRight( constraint );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    CleanupStack::PushL( constraint );
    
    // Check constraint and execute rights.
    TL( constraint );
    T1L( error, DRMCommon::EOk);
    
    // Get start time
    error = constraint->GetStartTime( currentTime );
    
    // Check start time
    T1L( error, DRMCommon::EOk );
    TL( currentTime != Time::NullTTime() );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsConstrGetStartTimeTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }

// ----------------------------------------------------------------------------
// RightsConstraintsGetEndTimeTestL
// Test for checking the end time constraint.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsConstraintsGetEndTimeTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsConstraintsGetEndTimeTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    TTime currentTime( Time::NullTTime() );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase34: Test to get end time constraint of" ); 
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "object, expecting: end time, DRMCommon::EOk" ); 
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailStartEndExecute, rights );
    
    // Get execute rights
    error = rights[0]->GetExecuteRight( constraint );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    CleanupStack::PushL( constraint );
    
    // Check constraint and execute rights.
    TL( constraint );
    T1L( error, DRMCommon::EOk);
    
    // Get end time
    error = constraint->GetEndTime( currentTime );
    
    T1L( DRMCommon::EOk, error );
    TL( currentTime != Time::NullTTime() );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsConstraintsGetEndTimeTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }

// ----------------------------------------------------------------------------
// RightsConstraintsGetIntervalTestL
// Test for checking the interval time constraint.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsConstraintsGetIntervalTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsConstraintsGetIntervalTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    TTimeIntervalSeconds interval( 0 );
    TTimeIntervalSeconds temp( 0 ); 
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase35: Test to get interval constraint of" );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "object, expecting interval, DRMCommon::EOk" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailIntervalExecute, rights );
    
    // Get execute rights
    error = rights[0]->GetExecuteRight( constraint );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    CleanupStack::PushL( constraint );
    
    // Check constraint and execute rights.
    TL( constraint );
    T1L( error, DRMCommon::EOk);
    
    // Get interval 
    error = constraint->GetInterval( interval );
    
    // Check return value and interval
    T1L( DRMCommon::EOk, error );
    TL( interval > temp );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsConstraintsGetIntervalTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }

// ----------------------------------------------------------------------------
// RightsConstraintsGetIntervalStartTestL
// Test for checking the interval start time constraint.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsConstraintsGetIntervalStartTestL
    ( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsConstraintsGetIntervalStartTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    TTime currentTime( Time::NullTTime() );
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    TTime intervalStart( Time::NullTTime() ); 
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase36: Test to get interval start time" ); 
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "constraint, expecting DRMCommon::EOk, interval" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
   
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, 
        KROTailStartEndIntervalPlay, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    CDRMPermission& permission = rights[0]->GetPermission();
    TL( &permission );
    
    currentTime.UniversalTime();
    
    // Consume one rights to start the interval
    permission.ConsumeRights( ContentAccess::EPlay, currentTime );
    
    // Get play rights
    error = rights[0]->GetPlayRight( constraint );
    
    CleanupStack::PushL( constraint );
    
    // Check constraint and execute rights.
    TL( constraint );
    T1L( error, DRMCommon::EOk);
    
    // Get interval 
    error = constraint->GetIntervalStart( intervalStart );
    
    T1L( DRMCommon::EOk, error );
    TL( intervalStart != Time::NullTTime() );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsConstrGetIntervalStartL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }

// ----------------------------------------------------------------------------
// RightsConstraintsGetAccumulatedTestL
// Test for checking the accumulated time constraint.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsConstraintsGetAccumulatedTestL(
    TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsConstraintsGetAccumulatedTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    TTimeIntervalSeconds accumulated( 0 );
    TTimeIntervalSeconds temp( 0 ); 
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase37: Test to get accumulated constraint" );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "of object, expecting DRMCommon::EOk, accumul. time" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailAccumulatedPlay, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get play rights
    error = rights[0]->GetPlayRight( constraint );
    
    CleanupStack::PushL( constraint );
    
    // Check constraint and play rights.
    TL( constraint );
    T1L( error, DRMCommon::EOk);
    
    // Get accumulated time 
    error = constraint->GetAccumulated( accumulated );
    
    // Check return value and accumulated time constraint
    T1L( DRMCommon::EOk, error );
    TL( accumulated > temp );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsConstrGetAccumulatedTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }
    
// ----------------------------------------------------------------------------
// RightsConstraintsSetCountersTestL
// Test for setting the counters of the constraint.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsConstraintsSetCountersTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsConstraintsSetCountersTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    TUint32 counter;
    TUint32 originalCounter;
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase38: Test to set counter constraints of" );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "object, expecting: DRMCommon:EOk" );
    iLog->Log( KTestCase2 );
      
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailCounterExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get execute rights
    error = rights[0]->GetExecuteRight( constraint );
    
    CleanupStack::PushL( constraint );
    
    // Check constraint and execute rights
    TL( constraint );
    T1L( error, DRMCommon::EOk);
    
    // Set count and original count to new values
    error = constraint->SetCounters( KNewCount, KNewOriginalCount );
    T1L( error, DRMCommon::EOk );
    
    // Get count and original count to check that setting 
    // count and original count was successful.
    error = constraint->GetCounters( counter, originalCounter );
    T1L( error, DRMCommon::EOk );
    
    T1L( counter, KNewCount );
    T1L( originalCounter, KNewOriginalCount );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsConstraintsSetCountersTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }        

// ----------------------------------------------------------------------------
// RightsConstraintsSetStartTimeTestL
// Test for setting the start time constraint.
// ----------------------------------------------------------------------------    
TInt CDrmRightsTest::RightsConstraintsSetStartTimeTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsConstraintsSetStartTimeTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    TTime startTime( Time::NullTTime() );
    TTime newStartTime( Time::NullTTime() ); 
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase39: Test to set start time of" ); 
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "object, expecting DRMCommon::EOk" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailStartEndExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get execute rights
    error = rights[0]->GetExecuteRight( constraint );
    
    CleanupStack::PushL( constraint );
    
    TL( constraint );
    T1L( error, DRMCommon::EOk);
    
    startTime.UniversalTime();
    
    // Set start time
    error = constraint->SetStartTime( startTime );
    T1L( error, DRMCommon::EOk );
    
    // Check that start time change has been made succesfully
    error = constraint->GetStartTime( newStartTime );
    T1L( error, DRMCommon::EOk );
    
    TL( startTime == newStartTime );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsConstrSetStartTimeTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }            

// ----------------------------------------------------------------------------
// RightsConstraintsSetEndTimeTestL
// Test for setting the end time constraint.
// ---------------------------------------------------------------------------- 
TInt CDrmRightsTest::RightsConstraintsSetEndTimeTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsConstraintsSetEndTimeTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    TTime endTime( Time::NullTTime() );
    TTime newEndTime( Time::NullTTime() ); 
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    _LIT( KTestCase1, "TestCase40: Test to set end time constraint of" ); 
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "object, expecting DRMCommon::EOk" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailStartEndExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get execute rights
    error = rights[0]->GetExecuteRight( constraint );
    
    CleanupStack::PushL( constraint );
    
    // Check constraint and execute rights
    TL( constraint );
    T1L( error, DRMCommon::EOk);
    
    endTime.UniversalTime();
    
    // Set end time
    error = constraint->SetEndTime( endTime );    
    T1L( error, DRMCommon::EOk );
    
    // Check that end time change has been made succesfully
    error = constraint->GetEndTime( newEndTime );
    T1L( error, DRMCommon::EOk );
    
    TL( endTime == newEndTime );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsConstraintsSetEndTimeTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }            

// ----------------------------------------------------------------------------
// RightsConstraintsSetIntervalTestL
// Test for setting the interval time constraint.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsConstraintsSetIntervalTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsConstraintsSetIntervalTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    TTimeIntervalSeconds interval( KNewIntervalSeconds );
    TTimeIntervalSeconds newInterval( 0 ); 
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase41: Test to set interval constraint of "); 
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "object, expecting DRMCommon::EOk" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailIntervalExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get execute rights
    error = rights[0]->GetExecuteRight( constraint );
    
    CleanupStack::PushL( constraint );
    
    // Check constraint and execute rights
    TL( constraint );
    T1L( error, DRMCommon::EOk);
    
    // Set interval
    error = constraint->SetInterval( interval );
    T1L( error, DRMCommon::EOk );
    
    // Get interval to check that interval change has been made correctly.
    error = constraint->GetInterval( newInterval );
    T1L( error, DRMCommon::EOk );
    
    TL( interval == newInterval );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsConstraintsSetIntervalTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }            

// ----------------------------------------------------------------------------
// RightsConstraintsSetIntervalStartTestL
// Test for setting the interval start time constraint.
// ----------------------------------------------------------------------------    
TInt CDrmRightsTest::RightsConstraintsSetIntervalStartTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsConstraintsSetIntervalStartTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    TTime startTime( Time::NullTTime() );
    TTime newStartTime( Time::NullTTime() ); 
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase42: Test to set interval start time" );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "constraint of object, expecting DRMCommon::EOk" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, 
        KROTailStartEndIntervalPlay, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
     
    // Get play rights
    error = rights[0]->GetPlayRight( constraint );
    
    CleanupStack::PushL( constraint );
    
    // Check constraint and play rights
    TL( constraint );
    T1L( error, DRMCommon::EOk);
    
    startTime.UniversalTime();
    
    // Set interval
    error = constraint->SetIntervalStart( startTime );
    T1L( error, DRMCommon::EOk );
    
    // Check that interval start time change has been made succesfully
    error = constraint->GetIntervalStart( newStartTime );
    T1L( error, DRMCommon::EOk );
    
    TL( startTime == newStartTime );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsConstrSetIntervalStartTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }

// ----------------------------------------------------------------------------
// RightsConstraintsGetConstraintTestL
// Test for getting constraint.
// ----------------------------------------------------------------------------    
TInt CDrmRightsTest::RightsConstraintsGetConstraintTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsConstraintsGetConstraintTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
     
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase43: Test to get constraint of object," );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "expecting valid reference to CDrmRightsConstraints" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
        
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailDisplayPrint, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get display rights
    error = rights[0]->GetDisplayRight( constraint );
    
    CleanupStack::PushL( constraint );
    
    // Check constraint and display rights
    TL( constraint );
    T1L( error, DRMCommon::EOk);
    
    // Get constraint
    CDRMConstraint& constraintRef = constraint->GetConstraint();
    
    // Check reference
    TL( &constraintRef );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsConstrGetConstraintTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }                

// ----------------------------------------------------------------------------
// RightsConstraintsSetConstraintTestL
// Test for setting constraint.
// ----------------------------------------------------------------------------    
TInt CDrmRightsTest::RightsConstraintsSetConstraintTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsConstraintsSetConstraintTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase44: Test to set constraint of rights object" );
    iLog->Log( KTestCase1 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
        
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailCounterExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get execute rights
    error = rights[0]->GetExecuteRight( constraint );
    
    CleanupStack::PushL( constraint );
    
    // Check constraint and execute rights
    TL( constraint );
    T1L( error, DRMCommon::EOk);
    
    // Get constraint
    CDRMConstraint& constraintRef = constraint->GetConstraint();
    
    TL( &constraintRef );
    
    // Create new CDrmRightsConstraints for setting the constraint.
    CDRMRightsConstraints* constraint2 = CDRMRightsConstraints::NewL();
    
    TL( constraint2 )
    CleanupStack::PushL( constraint2 );
    
    // Set the constraint according to the referenced constraint
    constraint2->SetConstraint( constraintRef );
    
    CleanupStack::PopAndDestroy( 3, &rights ); // rights, constraint, constr2
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsConstrSetConstraintTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }                


// ----------------------------------------------------------------------------
// RightsConstraintsAssignmentTestL
// Test for assigning constraint to another constraint.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsConstraintsAssignmentTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsConstraintsAssignmentTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase45: Test for assignment operator of" );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "CDrmRightsConstraints, expecting functional copy" );
    iLog->Log( KTestCase2 );
    
    _LIT( KTestCase3, "of CDrmRightsConstraints" );
    iLog->Log( KTestCase3 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
        
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailIntervalExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get execute rights
    error = rights[0]->GetExecuteRight( constraint );
    
    CleanupStack::PushL( constraint );
    
    // Check constraint and execute rights
    TL( constraint );
    T1L( error, DRMCommon::EOk);
    
    // Create new CDrmRightsConstraints for assign operation.
    CDRMRightsConstraints* constraint2 = CDRMRightsConstraints::NewL();
    
    TL( constraint2 )
    CleanupStack::PushL( constraint2 );
    
    constraint2 = constraint;
    TL( constraint2 )
    
    CleanupStack::PopAndDestroy( 3, &rights ); // rights, constraint, constr2
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsConstraintsAssignmentTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }                

// ----------------------------------------------------------------------------
// RightsGetLocalIdTestL
// Test for getting local id of the object.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsGetLocalIdTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsGetLocalIdTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    TUint32 localId( 0 );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase46: Test for getting local id of object," );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "expecting non-zero local id" );
    iLog->Log( KTestCase2 );
     
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
        
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailDisplayPrint, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get local id
    localId = rights[0]->GetLocalID();
    
    // Check local id value
    TL( localId );
    
    CleanupStack::PopAndDestroy( &rights ); // rights
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsGetLocalIdTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }                

// ----------------------------------------------------------------------------
// RightsGetAddTimeTestL
// Test for getting time when rights object was added to the database.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsGetAddTimeTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsGetAddTimeTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    TTime time( Time::NullTTime() );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase47: Test to get time when rights object" );
    iLog->Log( KTestCase1 );
     
    _LIT( KTestCase2, "was added to the database, expecting NULL time" );
    iLog->Log( KTestCase2 ); 
     
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
        
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead1, KContentId1, KROTailFull, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get the time when rights object was added to the database
    rights[0]->GetAddTime( time );
    
    // Check time
    TL( time == Time::NullTTime() );
    
    CleanupStack::PopAndDestroy( &rights ); // rights
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsGetAddTimeTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }                

// ----------------------------------------------------------------------------
// RightsGetRightsInfoTestL
// Test for getting restriction details associated with given permission.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsGetRightsInfoTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsGetAddTimeTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRights::TRestriction restriction;
    CDRMRights::TExpiration expiration;
    TUint32 constraints;
    TInt ret( CDRMRights::ENoRights );
    
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase48: Test to get restriction details of" );
    iLog->Log( KTestCase1 );
     
    _LIT( KTestCase2, "the rights object, expecting ERestrictedRights," );
    iLog->Log( KTestCase2 );
    
    _LIT( KTestCase3, "EInActiveInterval, !ENoRights" );
    iLog->Log( KTestCase3 ); 
     
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
        
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailIntervalExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get the time when rights object was added to the database
    ret = rights[0]->GetRightsInfo( EExecute, restriction, expiration,
        constraints );
    
    // Check rights status
    T1L( restriction, CDRMRights::ERestrictedRights );
    
    // Check expiration status
    T1L( expiration, CDRMRights::EValidRights );
    
    // Check constraint details
    T1L( constraints, CDRMRights::EInActiveInterval );
    
    // Check priority value of the rights object
    TL( ret != CDRMRights::ENoRights );
    
    CleanupStack::PopAndDestroy( &rights ); // rights
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsGetRightsInfoTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }           

//  End of File

// ----------------------------------------------------------------------------
// RightsGetRightsDetailsTestL
// Test for getting restriction details associated with given permission.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsGetExpirationDetailsTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsGetRightsDetailsTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    TTime endTime( Time::NullTTime() );
    TInt error( CDRMRights::ENoRights );
    TInt counts( 0 );
   
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase48: Test to get the end time and amount" );
    iLog->Log( KTestCase1 );
     
    _LIT( KTestCase2, "of counts of the rights object, expecting valid" );
    iLog->Log( KTestCase2 );
    
    _LIT( KTestCase3, "end time, -1, EOk" );
    iLog->Log( KTestCase3 ); 
     
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
        
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailStartEndExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get the end time and amount of counts of the rights object
    error = rights[0]->GetExpirationDetails( EExecute, endTime, counts );
    
    // Check end time
    TL( endTime != Time::NullTTime() );
    
    // Check amount of counts (should be no rights, i.e. -1)
    T1L( counts, -1 );
    
    // Check return value
    T1L( error, CDRMRights::EOk );
    
    CleanupStack::PopAndDestroy( &rights ); // rights
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsGetRightsInfoTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }           

// ----------------------------------------------------------------------------
// RightsGetPrintRightTestL
// Test for getting the constraints associated with the print rights.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsGetPrintRightTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsGetPrintRightTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase49: Test to get the constraint associated" );
    iLog->Log( KTestCase1 );
     
    _LIT( KTestCase2, "with the print right, expecting valid constraint" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
        
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailDisplayPrint, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get print rights
    error = rights[0]->GetPrintRight( constraint );
    
    CleanupStack::PushL( constraint );
    
    // Check constraint and display rights.
    TL( constraint );
    T1L( error, DRMCommon::EOk );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsGetPrintRightTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }           

// ----------------------------------------------------------------------------
// RightsSetContentURIandLocalIDTestL
// Test for setting the content Uri and Local Id of the object.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsSetContentURIAndLocalIDTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsSetContentURIandLocalIDTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRights* rights( NULL );
    HBufC8* uri( NULL );
    TUint id( 0 );
    TPtr8 contentUri(NULL, 0, 0);
    TInt error( KErrNone );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase50: Test to set the content Uri and the" );
    iLog->Log( KTestCase1 );
     
    _LIT( KTestCase2, "local Id of the object, expecting DRMCommon::EOk" );
    iLog->Log( KTestCase2 );
    
    // Create rights object
    rights = CDRMRights::NewL();
    CleanupStack::PushL( rights );
	
	// Compose Content URI 
	uri = HBufC8::NewL( KUriSize );
	contentUri.Set( uri->Des() );
	contentUri.Format( KUri );
	
	// Set Content URI and local ID
	error = rights->SetContentURIAndLocalID( uri, id );    
    
    T1L( error, DRMCommon::EOk );
    
    CleanupStack::PopAndDestroy( rights ); // rights
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDesc, "CDrmRightsTest::RightsSetContentURIandLocalIDTestL passed" );
    iLog->Log( KDesc );
    aResult.SetResult( KErrNone, KDesc );

    // Case was executed
    return KErrNone;    
    }           

//  End of File

// ----------------------------------------------------------------------------
// RightsSetContentURITestL
// Test for setting the content Uri.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsSetContentURITestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsSetContentURITestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRights* rights( NULL );
    HBufC8* uri( NULL );
    TPtr8 contentUri(NULL, 0, 0);
    TInt error( KErrNone );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase51: Test to set the content Uri of the" );
    iLog->Log( KTestCase1 );
     
    _LIT( KTestCase2, "object, expecting DRMCommon::EOk" );
    iLog->Log( KTestCase2 );
    
    // Create rights object
    rights = CDRMRights::NewL();
    CleanupStack::PushL( rights );
	
	// Compose Content URI 
	uri = HBufC8::NewL( KUriSize );
	contentUri.Set( uri->Des() );
	contentUri.Format( KUri );
	
	// Set Content URI and local ID
	error = rights->SetContentURI( uri );    
    
    T1L( error, DRMCommon::EOk );
    
    CleanupStack::PopAndDestroy( rights ); // rights
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsSetContentURITestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }           

// ----------------------------------------------------------------------------
// RightsSetAddTimeTestL
// Test for setting the time when rights object was added to the database
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsSetAddTimeTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsSetAddTimeTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRights* rights( NULL );
    TTime time( Time::NullTTime() );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase52: Test to set the add time, expecting" );
    iLog->Log( KTestCase1 );
     
    _LIT( KTestCase2, "DRMCommon::EOk" );
    iLog->Log( KTestCase2 );
    
    // Create rights object
    rights = CDRMRights::NewL();
    CleanupStack::PushL( rights );
	
	time.UniversalTime();
	
	// Set current time
	rights->SetAddTime( time );    
    
    CleanupStack::PopAndDestroy( rights ); // rights
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsSetAddTimeTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }           

// ----------------------------------------------------------------------------
// RightsSetPlayRightL
// Test for setting the play constraints.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsSetPlayRightTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsSetPlayRightTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase53: Test to set the play constraint" );
    iLog->Log( KTestCase1 );
     
    _LIT( KTestCase2, "of the object, expecting DRMCommon::EOk" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
        
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailPlay, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get play rights
    error = rights[0]->GetPlayRight( constraint );
    CleanupStack::PushL( constraint );
    
    // Check constraint and play rights.
    TL( constraint );
    T1L( error, DRMCommon::EOk );
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailTimedPlay, rights );
    T1L( rights.Count(), 1 );
    
    // Try to set play rights
    error = rights[0]->SetPlayRight( constraint ); 
	
	T1L( error, DRMCommon::EOk );
	
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsSetPlayRightTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }
    
// ----------------------------------------------------------------------------
// RightsSetDisplayRightL
// Test for setting the display constraints.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsSetDisplayRightTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsSetDisplayRightTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase54: Test to set the display constraint" );
    iLog->Log( KTestCase1 );
     
    _LIT( KTestCase2, "of the object, expecting DRMCommon::EOk" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
        
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailDisplayPrint, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get play rights
    error = rights[0]->GetDisplayRight( constraint );
    CleanupStack::PushL( constraint );
    
    // Check constraint and display rights.
    TL( constraint );
    T1L( error, DRMCommon::EOk );
    
    // Try to set display rights
    error = rights[0]->SetDisplayRight( constraint ); 	
	T1L( error, DRMCommon::EOk );
	
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsSetDisplayRightTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }           

// ----------------------------------------------------------------------------
// RightsSetPrintRightTestL
// Test for setting the print constraints.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsSetPrintRightTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsSetPrintRightTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase55: Test to set the print constraint" );
    iLog->Log( KTestCase1 );
     
    _LIT( KTestCase2, "of the object, expecting DRMCommon::EOk" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
        
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailDisplayPrint, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get play rights
    error = rights[0]->GetPrintRight( constraint );
    CleanupStack::PushL( constraint );
    
    // Check constraint and display rights.
    TL( constraint );
    T1L( error, DRMCommon::EOk );
    
    // Try to set print rights 
    error = rights[0]->SetPrintRight( constraint ); 	
	T1L( error, DRMCommon::EOk );
	
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsSetPrintRightTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }           

// ----------------------------------------------------------------------------
// RightsSetExecuteRightTestL
// Test for setting the execute constraints.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsSetExecuteRightTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsSetExecuteRightTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRightsConstraints* constraint( NULL );
    TInt error( KErrNone );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase56: Test to set the execute constraint" );
    iLog->Log( KTestCase1 );
     
    _LIT( KTestCase2, "of the object, expecting DRMCommon::EOk" );
    iLog->Log( KTestCase2 );
    
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
        
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailCounterExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Get execute rights
    error = rights[0]->GetExecuteRight( constraint );
    CleanupStack::PushL( constraint );
    
    // Check constraint and display rights.
    TL( constraint );
    T1L( error, DRMCommon::EOk );
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailIntervalExecute, rights );
    T1L( rights.Count(), 1 );
    
    // Set execute rights
    error = rights[0]->SetExecuteRight( constraint );

    T1L( error, DRMCommon::EOk );   
  
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, constraint
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsSetExecuteRightTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }      

// ----------------------------------------------------------------------------
// RightsAssignmentTestL
// Test for assignment operator
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsAssignmentTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsAssignmentTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    CDRMRights* testRights( NULL );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase57: Test of assignment operator for " );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "CDrmRights." );
    iLog->Log( KTestCase2 );
     
    RPointerArray<CDRMRights> rights;  
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
    
    testRights = CDRMRights::NewL();
    CleanupStack::PushL( testRights );
        
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailCounterExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    *testRights = *rights[0];
    
    // Check the assignment operation
    TL( testRights );
    
    CleanupStack::PopAndDestroy( 2, &rights ); // rights, testRights
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsAssignmentTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }      

// ----------------------------------------------------------------------------
// RightsExternalizeInternalizeTestL
// Test for externalizing and internalizing rights.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsExternalizeInternalizeTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsExternalizeInternalizeTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase58: Test to externalize CDrmRights" );
    iLog->Log( KTestCase1 );
     
    RPointerArray<CDRMRights> rights;
    RFs GFs;
    RStoreWriteStream outStream;
    TStreamId rootId, assetId;
    
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item( rights );
    item.PushL();
 
    User::LeaveIfError( GFs.Connect() );
    CleanupClosePushL( GFs );
    
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId1, KROTailCounterExecute, rights ); 
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
     
    // Check the validity of the rights
    TL( rights[0] );
    
    // Create a file store for externalization   
    CDirectFileStore* store = CDirectFileStore::ReplaceLC( GFs, 
        KStoreFile3, EFileWrite );
    
    // Set the UID of the file
    store->SetTypeL( KDirectFileStoreLayoutUid );
    
    // Declare and construct the output stream 
    assetId = outStream.CreateLC( *store );
    
    rights[0]->ExternalizeL( outStream );
    outStream.CommitL();
    
    // Set this stream Id to be the root Id
    store->SetRootL( assetId );
    
    // Flush the client side buffer to the store
    store->CommitL();
    
    CleanupStack::PopAndDestroy( 2, store ); // store, outStream
    store = NULL;
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase2, "TestCase59: Test to internalize CDrmRights" );
    iLog->Log( KTestCase2 );
    
    RStoreReadStream inStream;
 
    // Open the file store
    store = CDirectFileStore::OpenLC( GFs, KStoreFile3, 
        EFileRead );
    
    // Open the root stream
    inStream.OpenLC( *store, store->Root() );
    
    // Read the data
    rights[0]->InternalizeL( inStream );
    
    CleanupStack::PopAndDestroy( 4, &rights ); // item, GFs, store, inStream
    
    // Passed
    iLog->Log( KPassed );
    
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsExternalizeInternalizeL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }      

// ----------------------------------------------------------------------------
// RightsMergeTestL
// Test for merging two rights objects.
// ----------------------------------------------------------------------------
TInt CDrmRightsTest::RightsMergeTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmRightsTest::RightsMergeTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    // Test Cases
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase60: Test to merge two CDrmRights objects," );
    iLog->Log( KTestCase1 );
    
    _LIT( KTestCase2, "expecting functional merged CDrmRights" );
    iLog->Log( KTestCase2 );
     
    RPointerArray<CDRMRights> rights;  
    RPointerArray<CDRMRights> rights2;
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item1( rights );
    TDoResetAndDestroy< RPointerArray<CDRMRights> > item2( rights2 );
    item1.PushL();
    item2.PushL();
        
    // Delete RO database
    DeleteRODBL();
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailCounterExecute, rights );
    
    // Check that rights are stored
    T1L( rights.Count(), 1 );
    
    // Add RO and get the rights from the added RO
    AddROGetRightsL( KROHead2, KContentId2, KROTailStartEndIntervalPlay, 
        rights2 );
    
    T1L( rights2.Count(), 1 );
    
    // Merge the two rights objects
    rights[0]->Merge( *rights2[0] );
    
    TL( rights[0] );
    
    CleanupStack::PopAndDestroy( 2, &rights );
    
    // Passed
    iLog->Log( KPassed );
    
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // Test Cases End
    
    TestModuleIf().Printf( 0, KFinished, KData);

    // Test case passed
    _LIT( KDescr, "CDrmRightsTest::RightsMergeTestL passed" );
    iLog->Log( KDescr );
    aResult.SetResult( KErrNone, KDescr );

    // Case was executed
    return KErrNone;    
    }      


//  End of File
