/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DRMCommonTest hard coded test cases
*
*/



// INCLUDE FILES
#include <f32file.h>
#include <apmstd.h>
#include <drmcommon.h>

#include "drmcommontest.h"
#include "drmrightsclient.h"
#include "drmpermission.h"
#include "drmrights.h"
#include "drmrightsparser.h"
#include "drmpointerarray.h"

// CONSTANTS
#ifdef __WINSCW__
_LIT( KTempFile, "c:\\temp.tmp" );
_LIT( KOma1Content, "c:\\content.dcf" );
#else
_LIT( KTempFile, "e:\\testing\\data\\drm\\temp.tmp" );
_LIT( KOma1Content, "e:\\testing\\data\\drm\\content.dcf" );
#endif

_LIT8( KOma1ContentID, "cid:testcontent20090526095637-2657230746@testcontent.com" );
_LIT8( KOma1ContentMime, "image/jpeg" );

_LIT8( KROHeadOMA1, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\
<!DOCTYPE o-ex:rights PUBLIC \"-//OMA//DTD DRMREL 1.0//EN\" \
\"http://www.oma.org/dtd/dr\">\
<o-ex:rights\
   xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" \
   xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" \
   xmlns:oma-dd=\"http://www.openmobilealliance.com/oma-dd\" \
   xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\">\
  <o-ex:context>\
    <o-dd:version>1.0</o-dd:version>\
  </o-ex:context>\
  <o-ex:agreement>\
    <o-ex:asset>\
      <o-ex:context>\
        <o-dd:uid>" );

_LIT8( KROTailFullOMA1, "</o-dd:uid>\
      </o-ex:context>\
      <ds:KeyInfo><ds:KeyValue>PgIkd7w3E5TWWcc7Inp8Yw==</ds:KeyValue></ds:KeyInfo>\
    </o-ex:asset>\
  <o-ex:permission>\
\
<o-dd:play>\
      <o-ex:constraint>\
      </o-ex:constraint>\
    </o-dd:play>\
\
<o-dd:display>\
      <o-ex:constraint>\
      </o-ex:constraint>\
    </o-dd:display>\
\
<o-dd:execute>\
      <o-ex:constraint>\
      </o-ex:constraint>\
    </o-dd:execute>\
\
<o-dd:print>\
      <o-ex:constraint>\
      </o-ex:constraint>\
    </o-dd:print>\
  </o-ex:permission>\
  </o-ex:agreement>\
</o-ex:rights>" );

_LIT8( KContentName, "Content-Name" );
_LIT8( KNewName, "New Name" );

_LIT( KRunning, "Running" );
_LIT( KFinished, "Finished" );
_LIT( KPassed, "Passed" );

// LOCAL FUNCTION PROTOTYPES
LOCAL_C void DoResetDestroyAndClose( TAny* aPtr );
LOCAL_C void DoResetDestroyAndClose2( TAny* aPtr );
LOCAL_C void ReadFileL( RFs& aFs, const TDesC& aName, HBufC8*& aContent );
LOCAL_C void WriteFileL( RFs& aFs, const TDesC& aName, HBufC8*& aContent );
LOCAL_C void DeleteRODBL( );
LOCAL_C TUint32 AddROL( const TDesC8& aHead, const TDesC8& aCID, const TDesC8& aTail );
//LOCAL_C void ConsumeRoL( const TInt aIntent, const TDesC8& aContentID, const TInt seconds );

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// DoResetDestroyAndClose
// Does ResetAndDestroy() to given RPointerArray< CDRMRights >
// -----------------------------------------------------------------------------
LOCAL_C void DoResetDestroyAndClose( TAny* aPtr )
    {
    ( reinterpret_cast<RPointerArray<CDRMRights>*>( aPtr ) )->ResetAndDestroy();
    ( reinterpret_cast<RPointerArray<CDRMRights>*>( aPtr ) )->Close();
    }

// -----------------------------------------------------------------------------
// DoResetDestroy2
// Does ResetAndDestroy() to given RPointerArray< CDRMRights >
// -----------------------------------------------------------------------------
LOCAL_C void DoResetDestroyAndClose2( TAny* aPtr )
    {
    reinterpret_cast<RPointerArray<HBufC8>*>( aPtr )->ResetAndDestroy();
    reinterpret_cast<RPointerArray<HBufC8>*>( aPtr )->Close();
    }

// -----------------------------------------------------------------------------
// ReadFileL
// Helper function with which test cases can read file to the buffer
// -----------------------------------------------------------------------------
//
LOCAL_C void ReadFileL( RFs& aFs, const TDesC& aName, HBufC8*& aContent )
	{	
	RFile file;
	TInt size( 0 );
	User::LeaveIfError( file.Open ( aFs, aName, EFileRead ) );
	CleanupClosePushL( file );
	User::LeaveIfError( file.Size( size ) );
	aContent = HBufC8::NewLC( size );
	TPtr8 ptr( aContent->Des() );
	User::LeaveIfError( file.Read( 0, ptr, size ) );
	CleanupStack::Pop( aContent ); //aContent
	CleanupStack::PopAndDestroy( &file ); //file
	}

// -----------------------------------------------------------------------------
// WriteFileL
// Helper function with which test cases can write buffer to the file
// -----------------------------------------------------------------------------
//
LOCAL_C void WriteFileL( RFs& aFs, const TDesC& aName, HBufC8*& aContent )
	{	
	RFile file;
    User::LeaveIfError( file.Replace( aFs, aName, EFileWrite | EFileShareReadersOrWriters  ) );
    CleanupClosePushL( file );
    User::LeaveIfError( file.Write( *aContent ) );
    CleanupStack::PopAndDestroy( &file );
	}

// -----------------------------------------------------------------------------
// DeleteRODBL
// Helper function with which test cases can delete RO DB
// -----------------------------------------------------------------------------
//
LOCAL_C void DeleteRODBL( )
    {
	RDRMRightsClient client;
    User::LeaveIfError( client.Connect() );
    client.DeleteAll();
    client.Close();
    }

// -----------------------------------------------------------------------------
// AddROL
// Helper function with which test cases can add ROs
// Returns local id of added RO
// -----------------------------------------------------------------------------
//
LOCAL_C TUint32 AddROL(
    const TDesC8& aHead,
    const TDesC8& aCID, 
    const TDesC8& aTail
    )
    {
	TInt roSize( aHead.Size() + aCID.Size() + aTail.Size() );
    HBufC8* buf( HBufC8::NewLC( roSize ) );
    TPtr8 tmp( buf->Des() );
    
    tmp.Append( aHead );
    tmp.Append( aCID );
    tmp.Append( aTail );
    
    CDRMPointerArray<CDRMRights>* rights( CDRMPointerArray<CDRMRights>::NewLC() );
    rights->SetAutoCleanup( ETrue );
    
    CDrmRightsParser* rp( CDrmRightsParser::NewL() );
    CleanupStack::PushL( rp );
    
    rp->ParseAndStoreL( *buf, *rights );
    
    TUint32 localId( rights[0][0]->GetLocalID() );
    
    CleanupStack::PopAndDestroy( 3, buf ); //rp, rights, buf
    
    return localId;
	}
   
// -----------------------------------------------------------------------------
// ConsumeRoL
// Helper function which consumes content´s rights
// -----------------------------------------------------------------------------
//
/*LOCAL_C void ConsumeRoL( 
    const TInt aIntent, 
    const TDesC8& aContentID, 
    const TInt seconds )
    {
	RDRMRightsClient client;
    User::LeaveIfError( client.Connect() );
    
    client.Consume( aIntent, aContentID );
    User::After( seconds*1000*1000 ); // seconds
    
    client.Close();
    }*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMCommonTest::Case
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
const TCaseInfo CDRMCommonTest::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    DRMCommonTest.cpp file and to DRMCommonTest.h 
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
        // CDRMCommonTest::PrintTest. Otherwise the compiler
        // gives errors.
        ENTRY( "Com::ConstructTestL", CDRMCommonTest::ConstructTestL ),
        ENTRY( "Com::VersionTestL", CDRMCommonTest::VersionTestL ),
        ENTRY( "Com::ConnectionTestL", CDRMCommonTest::ConnectionTestL ),
        ENTRY( "Com::CheckContentRightsTestL", CDRMCommonTest::CheckContentRightsTestL ),
        ENTRY( "Com::CheckFileRightsTestForFileNameL", CDRMCommonTest::CheckFileRightsTestForFileNameL ),
        ENTRY( "Com::CheckFileRightsTestForFileHandleL", CDRMCommonTest::CheckFileRightsTestForFileHandleL ),
        ENTRY( "Com::IsProtectedContentTestL", CDRMCommonTest::IsProtectedContentTestL ),
        ENTRY( "Com::IsProtectedFileTestForFileNameL", CDRMCommonTest::IsProtectedFileTestForFileNameL ),
        ENTRY( "Com::IsProtectedFileTestForFileHandleL", CDRMCommonTest::IsProtectedFileTestForFileHandleL ),
        ENTRY( "Com::GetContentInfoTestL", CDRMCommonTest::GetContentInfoTestL ),
        ENTRY( "Com::GetFileInfoTestForFileNameL", CDRMCommonTest::GetFileInfoTestForFileNameL ),
        ENTRY( "Com::GetFileInfoTestForFileHandleL", CDRMCommonTest::GetFileInfoTestForFileHandleL ),
        ENTRY( "Com::GetContentHeaderTestL", CDRMCommonTest::GetContentHeaderTestL ),
        ENTRY( "Com::GetFileHeaderTestForFileNameL", CDRMCommonTest::GetFileHeaderTestForFileNameL ),
        ENTRY( "Com::GetFileHeaderTestForFileHandleL", CDRMCommonTest::GetFileHeaderTestForFileHandleL ),
        ENTRY( "Com::SetContentHeaderTestL", CDRMCommonTest::SetContentHeaderTestL ),
        ENTRY( "Com::SetFileHeaderTestForFileNameL", CDRMCommonTest::SetFileHeaderTestForFileNameL ),
        ENTRY( "Com::SetFileHeaderTestForFileHandleL", CDRMCommonTest::SetFileHeaderTestForFileHandleL ),
        ENTRY( "Com::GetSingleRightsObjectTestL", CDRMCommonTest::GetSingleRightsObjectTestL ),
        ENTRY( "Com::GetDetailedContentRightsTestL", CDRMCommonTest::GetDetailedContentRightsTestL ),
        ENTRY( "Com::GetDetailedFileRightsTestForFileNameL", CDRMCommonTest::GetDetailedFileRightsTestForFileNameL ),
        ENTRY( "Com::GetDetailedFileRightsTestForFileHandleL", CDRMCommonTest::GetDetailedFileRightsTestForFileHandleL ),
        ENTRY( "Com::GetActiveRightsTestL", CDRMCommonTest::GetActiveRightsTestL ),
        ENTRY( "Com::GetContentURIListTestL", CDRMCommonTest::GetContentURIListTestL ),
        ENTRY( "Com::DataTypesTestL", CDRMCommonTest::DataTypesTestL ),
        ENTRY( "Com::MergeParentAndChildTestL", CDRMCommonTest::MergeParentAndChildTestL ),
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
// CDRMCommonTest::ConstructTestL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::ConstructTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::ConstructTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Testing NewL of DrmCommon: Expecting functional DRMCommon" );
    iLog->Log( KTestCase1 );
    
    DRMCommon* common = DRMCommon::NewL();
    TL( common );
    delete common;
    
    //Passed
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::ConstructTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::VersionTestL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::VersionTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::VersionTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    TVersion version;
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to get Version of DrmCommon: Expecting version name" );
    iLog->Log( KTestCase1 );

    version = DRMCommon::Version();
    TL( version.Name() != KNullDesC );
    
    //Passed
    iLog->Log( KPassed );
    
    // -----------------------------------------------------------------------------
    
    _LIT( KTestCase2, "TestCase2: Trying to get ServerVersion of DrmCommon: Expecting version name" );
    iLog->Log( KTestCase2 );
    
    version = DRMCommon::ServerVersion();
    TL( version.Name() != KNullDesC );
    
    //Passed
    iLog->Log( KPassed );
    
    // -----------------------------------------------------------------------------
    
    _LIT( KTestCase3, "TestCase2: Trying to check supported drm methods: Expecting 15, EOMA_2_0, DRMCommon::EOk" );
    iLog->Log( KTestCase3 );
    
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
    
    TInt drmMethod( 0 );
    DRMCommon::TOMALevel omaLevel( DRMCommon::EOMA_None );
    
    TInt ret( KErrNone );
    
    ret = common->SupportedDRMMethods( drmMethod, omaLevel);
    T1L( ret, DRMCommon::EOk );
    T1L( drmMethod, 15 );
    T1L( omaLevel, DRMCommon::EOMA_2_0 );
    
    //Passed
    CleanupStack::PopAndDestroy( common );
    iLog->Log( KPassed );
    
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::VersionTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDRMCommonTest::ConnectionTestL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::ConnectionTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::ConnectionTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );

    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
    
    TInt ret( DRMCommon::EOk );
       
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase1: Testing Connect of DrmCommon: Expecting DRMCommon::EOk" );
    iLog->Log( KTestCase1 );

    ret = common->Connect();
    T1L( ret, DRMCommon::EOk );
    
    //Passed
    iLog->Log( KPassed );
    
    // -----------------------------------------------------------------------------
    
    _LIT( KTestCase2, "TestCase2: Testing Disconnect of DrmCommon: Expecting DRMCommon::EOk" );
    iLog->Log( KTestCase2 );
    
    ret = common->Disconnect();
    T1L( ret, DRMCommon::EOk );
    
    //Passed
    iLog->Log( KPassed );
    
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( common );
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::ConnectionTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::CheckContentRightsTestL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::CheckContentRightsTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::CheckContentRightsTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );

    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
    
    TInt ret( DRMCommon::EOk );
       
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase1: Trying to check Oma1 content rights: Expecting ENoRights" );
    iLog->Log( KTestCase1 );

    //Delete RO database
    DeleteRODBL();

    ret = common->CheckContentRights( KOma1ContentID, EPlay );
    T1L( ret, DRMCommon::ENoRights );
    
    //Passed
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( common );
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::CheckContentRightsTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::CheckFileRightsTestForFileNameL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::CheckFileRightsTestForFileNameL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::CheckFileRightsTestForFileNameL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
    
    TInt ret( DRMCommon::EOk );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    //Delete RO database
    DeleteRODBL();

    _LIT( KTestCase1, "TestCase1: Trying to check Oma1 file rights: Expecting ENoRights" );
    iLog->Log( KTestCase1 );

    ret = common->CheckFileRights( KOma1Content, EPlay );
    T1L( ret, DRMCommon::ENoRights );
    
    //Passed
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( common );
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::CheckFileRightsTestForFileNameL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::CheckFileRightsTestForFileHandleL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::CheckFileRightsTestForFileHandleL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::CheckFileRightsTestForFileHandleL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
    
    TInt ret( DRMCommon::EOk );
   
    //File
    RFile file;
	RFs GFs;
    User::LeaveIfError( GFs.Connect() );
    CleanupClosePushL( GFs );
    
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to check Oma1 file rights: Expecting ENoRights" );
    iLog->Log( KTestCase1 );

    //Delete RO database
    DeleteRODBL();

    User::LeaveIfError( file.Open( GFs, KOma1Content, EFileRead | EFileShareReadersOrWriters ) );
    CleanupClosePushL( file );

    ret = common->CheckFileRights( file, EPlay );
    T1L( ret, DRMCommon::ENoRights );
    
    //Passed
    CleanupStack::PopAndDestroy( &file );
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( 2, common ); //GFs, common
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::CheckFileRightsTestForFileHandleL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::IsProtectedContentTestL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::IsProtectedContentTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::IsProtectedContentTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
    
    //file
    RFs GFs;
    User::LeaveIfError( GFs.Connect() );
    CleanupClosePushL( GFs );
    
    HBufC8* content( NULL );
    TBool protection( EFalse );
    TInt ret( DRMCommon::EOk );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to find out is OMA1 content protected: Expecting ETrue, DRMCommon::EOk" );
    iLog->Log( KTestCase1 );

    ReadFileL( GFs, KOma1Content, content );
    CleanupStack::PushL( content );

    ret = common->IsProtectedContent( *content, protection );
    T1L( ret, DRMCommon::EOk );
    TL( protection );
    
    //Passed
    CleanupStack::PopAndDestroy( content );
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( 2, common ); //GFs, common
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::IsProtectedContentTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::IsProtectedFileTestForFileNameL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::IsProtectedFileTestForFileNameL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::IsProtectedFileTestForFileNameL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
    
    TBool protection( EFalse );
    TInt ret( DRMCommon::EOk );
    
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to find out is OMA1 file protected: Expecting ETrue, DRMCommon::EOk" );
    iLog->Log( KTestCase1 );

    ret = common->IsProtectedFile( KOma1Content, protection );
    T1L( ret, DRMCommon::EOk );
    TL( protection );
    
    //Passed
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( common );
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::IsProtectedFileTestForFileNameL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::IsProtectedFileTestForFileHandleL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::IsProtectedFileTestForFileHandleL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::IsProtectedFileTestForFileHandleL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
    
    //file
    RFile file;
    RFs GFs;
    User::LeaveIfError( GFs.Connect() );
    CleanupClosePushL( GFs );
    
    TBool protection( EFalse );
    TInt ret( DRMCommon::EOk );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to find out is OMA1 content protected: Expecting ETrue, DRMCommon::EOk" );
    iLog->Log( KTestCase1 );

    User::LeaveIfError( file.Open( GFs, KOma1Content, EFileRead | EFileShareReadersOrWriters ) );
    CleanupClosePushL( file );

    ret = common->IsProtectedFile( file, protection );
    T1L( ret, DRMCommon::EOk );
    TL( protection );
    
    //Passed
    CleanupStack::PopAndDestroy( &file );
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( 2, common ); //GFs, common
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::IsProtectedFileTestForFileHandleL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::GetContentInfoTestL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::GetContentInfoTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::GetContentInfoTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
    
    //file
    RFs GFs;
    User::LeaveIfError( GFs.Connect() );
    CleanupClosePushL( GFs );
    
    HBufC8* content( NULL );
    DRMCommon::TContentProtection protection( DRMCommon::ENoDCFFile );
    HBufC8* mime( NULL );
    HBufC8* contentUri( NULL );
    TUint dataLength( 0 );
    TInt ret( DRMCommon::EOk );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to get OMA1 content info: Expecting DRMCommon::EOk, ESuperDistributable, KOma1ContentMime, KOma1ContentID, >0" );
    iLog->Log( KTestCase1 );

    ReadFileL( GFs, KOma1Content, content );
    CleanupStack::PushL( content );

    ret = common->GetContentInfo( *content, protection, mime, contentUri, dataLength );
    T1L( ret, DRMCommon::EOk );
    TL( protection == DRMCommon::ESuperDistributable );
    T1L( mime->Compare( KOma1ContentMime ), 0 );
    T1L( contentUri->Compare( KOma1ContentID ), 0 );
    TL( dataLength > 0 );
    
    //Passed
    CleanupStack::PopAndDestroy( content );
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( 2, common ); //GFs, common 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::GetContentInfoTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::GetFileInfoTestForFileNameL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::GetFileInfoTestForFileNameL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::GetFileInfoTestForFileNameL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
    
    DRMCommon::TContentProtection protection( DRMCommon::ENoDCFFile );
    HBufC8* mime( NULL );
    HBufC8* contentUri( NULL );
    TUint dataLength( 0 );
    TInt ret( DRMCommon::EOk );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to get OMA1 file info: Expecting DRMCommon::EOk, ESuperDistributable, KOma1ContentMime, KOma1ContentID, >0" );
    iLog->Log( KTestCase1 );

    ret = common->GetFileInfo( KOma1Content, protection, mime, contentUri, dataLength );
    T1L( ret, DRMCommon::EOk );
    TL( protection == DRMCommon::ESuperDistributable );
    T1L( mime->Compare( KOma1ContentMime ), 0 );
    T1L( contentUri->Compare( KOma1ContentID ), 0 );
    TL( dataLength > 0 );
    
    //Passed
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( common ); 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::GetFileInfoTestForFileNameL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::GetFileInfoTestForFileHandleL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::GetFileInfoTestForFileHandleL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::GetFileInfoTestForFileHandleL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
    
    //file
    RFile file;
    RFs GFs;
    User::LeaveIfError( GFs.Connect() );
    CleanupClosePushL( GFs );
    
    DRMCommon::TContentProtection protection( DRMCommon::ENoDCFFile );
    HBufC8* mime( NULL );
    HBufC8* contentUri( NULL );
    TUint dataLength( 0 );
    TInt ret( DRMCommon::EOk );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to get OMA1 file info: Expecting DRMCommon::EOk, ESuperDistributable, KOma1ContentMime, KOma1ContentID, >0" );
    iLog->Log( KTestCase1 );

    User::LeaveIfError( file.Open( GFs, KOma1Content, EFileRead | EFileShareReadersOrWriters ) );
    CleanupClosePushL( file );

    ret = common->GetFileInfo( file, protection, mime, contentUri, dataLength );
    T1L( ret, DRMCommon::EOk );
    TL( protection == DRMCommon::ESuperDistributable );
    T1L( mime->Compare( KOma1ContentMime ), 0 );
    T1L( contentUri->Compare( KOma1ContentID ), 0 );
    TL( dataLength > 0 );
    
    //Passed
    CleanupStack::PopAndDestroy( &file );
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( 2, common ); //GFs, common
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::GetFileInfoTestForFileHandleL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::GetContentHeaderTestL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::GetContentHeaderTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::GetContentHeaderTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
    
    //file
    RFs GFs;
    User::LeaveIfError( GFs.Connect() );
    CleanupClosePushL( GFs );
    
    HBufC8* content( NULL );
    HBufC8* header( NULL );
    TInt ret( DRMCommon::EOk );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to get content name -header from Oma1 content: Expecting ContentName, DRMCommon::EOk" );
    iLog->Log( KTestCase1 );

    ReadFileL( GFs, KOma1Content, content );
    CleanupStack::PushL( content );

    ret = common->GetContentHeader( *content, KContentName, header );
    CleanupStack::PushL( header );
    T1L( ret, DRMCommon::EOk );
    TL( header != NULL );
    
    //Passed
    CleanupStack::PopAndDestroy( 2, content ); //header, content
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( 2, common ); //GFs, common  
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::GetContentHeaderTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::GetFileHeaderTestForFileNameL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::GetFileHeaderTestForFileNameL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::GetFileHeaderTestForFileNameL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
    
    TFileName filename( KOma1Content );
    HBufC8* header( NULL );
    TInt ret( DRMCommon::EOk );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to get content name -header from Oma1 file: Expecting ContentName, DRMCommon::EOk" );
    iLog->Log( KTestCase1 );

    ret = common->GetFileHeader( filename, KContentName, header );
    CleanupStack::PushL( header );
    T1L( ret, DRMCommon::EOk );
    TL( header != NULL );
    
    //Passed
    CleanupStack::PopAndDestroy( header );
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( common ); 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::GetFileHeaderTestForFileNameL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::GetFileHeaderTestForFileHandleL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::GetFileHeaderTestForFileHandleL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::GetFileHeaderTestForFileHandleL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
    
    //file
    RFile file;
    RFs GFs;
    User::LeaveIfError( GFs.Connect() );
    CleanupClosePushL( GFs );
    
    HBufC8* header( NULL );
    TInt ret( DRMCommon::EOk );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to get content name -header from Oma1 file: Expecting ContentName, DRMCommon::EOk" );
    iLog->Log( KTestCase1 );

    User::LeaveIfError( file.Open( GFs, KOma1Content, EFileRead | EFileShareReadersOrWriters ) );
    CleanupClosePushL( file );

    ret = common->GetFileHeader( file, KContentName, header );
    CleanupStack::PushL( header );
    T1L( ret, DRMCommon::EOk );
    TL( header != NULL );
    
    //Passed
    CleanupStack::PopAndDestroy( 2, &file ); //header, file
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( 2, common ); //GFs, common   
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::GetFileHeaderTestForFileHandleL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::SetContentHeaderTestL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::SetContentHeaderTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::SetContentHeaderTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
    
    //file
    RFs GFs;
    User::LeaveIfError( GFs.Connect() );
    CleanupClosePushL( GFs );
    
    HBufC8* content( NULL );
    HBufC8* header( NULL );
    TInt ret( DRMCommon::EOk );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to set content name -header for Oma1 content: Expecting new name to be set, DRMCommon::EOk" );
    iLog->Log( KTestCase1 );

    ReadFileL( GFs, KOma1Content, content );
    
    ret = common->SetContentHeader( content, KContentName, KNewName );
    CleanupStack::PushL( content );
    T1L( ret, DRMCommon::EOk );
    
    ret = common->GetContentHeader( *content, KContentName, header );
    CleanupStack::PushL( header );
    T1L( ret, DRMCommon::EOk );
    T1L( header->Compare( KNewName ), 0 );
    
    //Passed
    CleanupStack::PopAndDestroy( 2, content ); //header, content
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( 2, common ); //GFs, common
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::SetContentHeaderTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::SetFileHeaderTestForFileNameL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::SetFileHeaderTestForFileNameL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::SetFileHeaderTestForFileNameL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
    
    //file
    RFs GFs;
    User::LeaveIfError( GFs.Connect() );
    CleanupClosePushL( GFs );
    
    HBufC8* content( NULL );
    ReadFileL( GFs, KOma1Content, content );
    CleanupStack::PushL( content );
    WriteFileL( GFs, KTempFile, content );
    CleanupStack::PopAndDestroy( 2, &GFs ); //file, GFs    

    TFileName filename( KTempFile );
    HBufC8* header( NULL );
    TInt ret( DRMCommon::EOk );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to set content name -header for Oma1 file: Expecting new name to be set, DRMCommon::EOk" );
    iLog->Log( KTestCase1 );

    ret = common->SetFileHeader( KTempFile, KContentName, KNewName );
    T1L( ret, DRMCommon::EOk );
    
    ret = common->GetFileHeader( filename, KContentName, header );
    CleanupStack::PushL( header );
    T1L( ret, DRMCommon::EOk );
    T1L( header->Compare( KNewName ), 0 );
    
    //Passed
    CleanupStack::PopAndDestroy( header );
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( common ); 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::SetFileHeaderTestForFileNameL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::SetFileHeaderTestForFileHandleL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::SetFileHeaderTestForFileHandleL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::SetFileHeaderTestForFileHandleL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
    
    //file
    RFile file;
    RFs GFs;
    User::LeaveIfError( GFs.Connect() );
    CleanupClosePushL( GFs );
    
    HBufC8* content( NULL );
    ReadFileL( GFs, KOma1Content, content );
    CleanupStack::PushL( content );
    WriteFileL( GFs, KTempFile, content );
    CleanupStack::PopAndDestroy( content );
    
    HBufC8* header( NULL );
    TInt ret( DRMCommon::EOk );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to set content name -header for Oma1 file: Expecting new name to be set, DRMCommon::EOk" );
    iLog->Log( KTestCase1 );

    User::LeaveIfError( file.Open( GFs, KTempFile, EFileWrite | EFileShareReadersOrWriters ) );
    CleanupClosePushL( file );

    ret = common->SetFileHeader( file, KContentName, KNewName );
    T1L( ret, DRMCommon::EOk );
    CleanupStack::PopAndDestroy( &file );
    
    User::LeaveIfError( file.Open( GFs, KTempFile, EFileRead | EFileShareReadersOrWriters ) );
    CleanupClosePushL( file );
    
    ret = common->GetFileHeader( file, KContentName, header );
    CleanupStack::PushL( header );
    T1L( ret, DRMCommon::EOk );
    T1L( header->Compare( KNewName ), 0 );
    
    //Passed
    CleanupStack::PopAndDestroy( 2, &file ); //header, file
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( 2, common ); //GFs, common 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::SetFileHeaderTestForFileHandleL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::GetSingleRightsObjectTestL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::GetSingleRightsObjectTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::GetSingleRightsObjectTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
   
    TUint32 localId( 0 );
    CDRMRights* right( NULL );
    TInt ret( DRMCommon::EOk );
    
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to get single RO: Expecting RO, DRMCommon::EOk" );
    iLog->Log( KTestCase1 );

    //Delete RO database
    DeleteRODBL();

    localId = AddROL( KROHeadOMA1, KOma1ContentID, KROTailFullOMA1 );
    
    ret = common->GetSingleRightsObject( KOma1ContentID, localId, right );
    CleanupStack::PushL( right );
    T1L( ret, DRMCommon::EOk );
    TL( right );

    //Passed
    CleanupStack::PopAndDestroy( right );
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( common );
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::GetSingleRightsObjectTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::GetDetailedContentRightsTestL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::GetDetailedContentRightsTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::GetDetailedContentRightsTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
   
    RPointerArray<CDRMRights>* rights( NULL );
    TInt ret( DRMCommon::EOk );
    
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to get detailed rights of Oma1 full rights content: Expecting 1 RO, DRMCommon::EOk" );
    iLog->Log( KTestCase1 );

    //Delete RO database
    DeleteRODBL();

    AddROL( KROHeadOMA1, KOma1ContentID, KROTailFullOMA1 );
    
    ret = common->GetDetailedContentRights( KOma1ContentID, rights );
    TCleanupItem cleanup( DoResetDestroyAndClose, rights );
    CleanupStack::PushL( cleanup );
    
    T1L( ret, DRMCommon::EOk );
    T1L( rights->Count(), 1 );

    //Passed
    CleanupStack::PopAndDestroy(); //cleanup
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( common );
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::GetDetailedContentRightsTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::GetDetailedFileRightsTestForFileNameL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::GetDetailedFileRightsTestForFileNameL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::GetDetailedFileRightsTestForFileNameL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
   
    RPointerArray<CDRMRights>* rights( NULL );
    TInt ret( DRMCommon::EOk );
    
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to get detailed rights of Oma1 full rights file: Expecting 1 RO, DRMCommon::EOk" );
    iLog->Log( KTestCase1 );

    //Delete RO database
    DeleteRODBL();

    AddROL( KROHeadOMA1, KOma1ContentID, KROTailFullOMA1 );
    
    ret = common->GetDetailedFileRights( KOma1Content, rights );
    TCleanupItem cleanup( DoResetDestroyAndClose, rights );
    CleanupStack::PushL( cleanup );
    
    T1L( ret, DRMCommon::EOk );
    T1L( rights->Count(), 1 );

    //Passed
    CleanupStack::PopAndDestroy(); //cleanup
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( common ); 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::GetDetailedFileRightsTestForFileNameL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::GetDetailedFileRightsTestForFileHandleL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::GetDetailedFileRightsTestForFileHandleL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::GetDetailedFileRightsTestForFileHandleL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
   
    //File
    RFile file;
	RFs GFs;
    User::LeaveIfError( GFs.Connect() );
    CleanupClosePushL( GFs );
   
    RPointerArray<CDRMRights>* rights( NULL );
    TInt ret( DRMCommon::EOk );
    
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to get detailed rights of Oma1 full rights file: Expecting 1 RO, DRMCommon::EOk" );
    iLog->Log( KTestCase1 );

    //Delete RO database
    DeleteRODBL();

    AddROL( KROHeadOMA1, KOma1ContentID, KROTailFullOMA1 );
    
    User::LeaveIfError( file.Open( GFs, KOma1Content, EFileRead | EFileShareReadersOrWriters ) );
    CleanupClosePushL( file );
    
    ret = common->GetDetailedFileRights( file, rights );
    TCleanupItem cleanup( DoResetDestroyAndClose, rights );
    CleanupStack::PushL( cleanup );
    
    T1L( ret, DRMCommon::EOk );
    T1L( rights->Count(), 1 );

    //Passed
    CleanupStack::PopAndDestroy( 2, &file ); //cleanup, file
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( 2, common ); //GFs, common
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::GetDetailedFileRightsTestForFileHandleL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::GetActiveRightsTestL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::GetActiveRightsTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::GetActiveRightsTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
   
    CDRMRights* right( NULL );
    TInt ret( DRMCommon::ENoRights );
    
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to get active rights of Oma1 full rights content: Expecting RO, CDRMRights::EFullRights" );
    iLog->Log( KTestCase1 );

    //Delete RO database
    DeleteRODBL();

    AddROL( KROHeadOMA1, KOma1ContentID, KROTailFullOMA1 );
    
    ret = common->GetActiveRights( KOma1ContentID, EPlay,  right );
    CleanupStack::PushL( right );
    T1L( ret, CDRMRights::EFullRights );
    TL( right );

    //Passed
    CleanupStack::PopAndDestroy( right ),
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( common );
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::GetActiveRightsTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::GetContentURIListTestL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::GetContentURIListTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::GetContentURIListTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
   
    RPointerArray<HBufC8>* uris( NULL );
    TInt ret( DRMCommon::EOk );
    
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to get content uri list with Oma1 content: Expecting only Oma1 content, DRMCommon::EOk" );
    iLog->Log( KTestCase1 );

    //Delete RO database
    DeleteRODBL();

    AddROL( KROHeadOMA1, KOma1ContentID, KROTailFullOMA1 );
    
    ret = common->GetContentURIList( uris );
    TCleanupItem cleanup( DoResetDestroyAndClose2, uris );
    CleanupStack::PushL( cleanup );
    
    T1L( ret, DRMCommon::EOk );
    T1L( uris->Count(), 1 );
    T1L( uris[0][0]->Compare( KOma1ContentID ), 0 );

    //Passed
    CleanupStack::PopAndDestroy(); //cleanup
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( common );
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::GetContentURIListTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDRMCommonTest::DataTypesTestL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::DataTypesTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::DataTypesTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
   
    TInt ret( DRMCommon::EOk );
    
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Trying to get number of static data types: Expecting count>0, DRMCommon::EOk" );
    iLog->Log( KTestCase1 );

    TInt count( 0 );

    ret = common->StaticDataTypesCount( count );
    T1L( ret, DRMCommon::EOk );
    TL( count > 0 );
    
    //Passed
    iLog->Log( KPassed );
    
    // -----------------------------------------------------------------------------
    
    _LIT( KTestCase2, "TestCase2: Trying to get number of data types: Expecting count>0, DRMCommon::EOk" );
    iLog->Log( KTestCase2 );

    ret = common->DataTypesCount( count );
    T1L( ret, DRMCommon::EOk );
    TL( count > 0 );
    
    //Passed
    iLog->Log( KPassed );
    
    // -----------------------------------------------------------------------------
    
    _LIT( KTestCase3, "TestCase3: Trying to register a data type: Expecting DRMCommon::EOk" );
    iLog->Log( KTestCase3 );

    _LIT8( KDataType, "foo/bar" );
    TDataType datatype( KDataType );

    ret = common->RegisterDataType( datatype );
    T1L( ret, DRMCommon::EOk );
    
    //Passed
    iLog->Log( KPassed );
    
    // -----------------------------------------------------------------------------
    
    _LIT( KTestCase4, "TestCase4: Trying to get get previously added data type: KDataType, DRMCommon::EOk" );
    iLog->Log( KTestCase4 );

    TDataType datatype2;

    ret = common->SupportedDataType( count, datatype2 );
    T1L( ret, DRMCommon::EOk );
    T1L( datatype2.Des8().Compare( KDataType ), 0 );
    
    //Passed
    iLog->Log( KPassed );
    
    // -----------------------------------------------------------------------------
    
    _LIT( KTestCase5, "TestCase5: Trying to unregister a data type: Expecting DRMCommon::EOk" );
    iLog->Log( KTestCase5 );

    ret = common->UnRegisterDataType( count );
    T1L( ret, DRMCommon::EOk );
    
    //Passed
    iLog->Log( KPassed );
    
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( common ); 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::DataTypesTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
        
// -----------------------------------------------------------------------------
// CDRMCommonTest::MergeParentAndChildTestL
// -----------------------------------------------------------------------------
//
TInt CDRMCommonTest::MergeParentAndChildTestL( 
    TTestResult& aResult )
    {
    _LIT( KData, "CDRMCommonTest::MergeParentAndChildTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    DRMCommon* common( DRMCommon::NewL() );
    CleanupStack::PushL( common );
    
    CDRMRights* right( NULL );
    
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    
    _LIT( KTestCase1, "TestCase1: Trying to merge NULL parent and child rights: Expecting function call to finish" );
    iLog->Log( KTestCase1 );

    common->MergeParentAndChild( right );
    
    //Passed
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    CleanupStack::PopAndDestroy( common );
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDRMCommonTest::MergeParentAndChildTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }

//  End of File
