/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DRMLicenseCheckerTest hard coded test cases
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include <DRMLicenseChecker.h>
#include "drmlicensecheckertest.h"

// CONSTANTS
//#ifdef __WINSCW__
_LIT( KTestFile, "c:\\testing\\data\\checker.txt" );
//#else
//_LIT( KTestFile, "e:\\testing\\data\\checker.txt" );
//#endif

_LIT( KRunning, "Running" );
_LIT( KFinished, "Finished" );
_LIT( KPassed, "Passed" );

// LOCAL FUNCTION PROTOTYPES


// ============================= LOCAL FUNCTIONS ===============================



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CDrmLicenseChecker::Case
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
const TCaseInfo CDrmLicenseCheckerTest::Case ( 
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
        // CDrmLicenceCheckerTest::PrintTest. Otherwise the compiler
        // gives errors.
        ENTRY( "Com::ConstructTestL", CDrmLicenseCheckerTest::ConstructTestL ),
        ENTRY( "Com::CheckLicenseTestL", CDrmLicenseCheckerTest::CheckLicenseTestL )
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
// CDrmLicenseCheckerTest::ConstructTestL
// -----------------------------------------------------------------------------
//
TInt CDrmLicenseCheckerTest::ConstructTestL( TTestResult& aResult )
    {
    _LIT( KData, "CDrmLicenseCheckerTest::ConstructTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Testing NewL of DrmLicenseChecker:");
    iLog->Log( KTestCase1 );
        
    CDRMLicenseChecker* checker = CDRMLicenseChecker::NewL();
    TL( checker );
    delete checker;
    
    //Passed
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDrmLicenseCheckerTest::ConstructTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDrmLicenseCheckerTest::CheckLicenseTestL
// -----------------------------------------------------------------------------
//    
TInt CDrmLicenseCheckerTest::CheckLicenseTestL(TTestResult& aResult)
    {
    _LIT( KData, "CDrmLicenseCheckerTest::CheckLicenseTestL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
    
    HBufC8* buffer( NULL );
    TInt ret( KErrNone );
    
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase2, "TestCase2: Testing CheckLicense of DrmLicenseChecker:" );
    iLog->Log( KTestCase2 );
    
    CDRMLicenseChecker* checker = CDRMLicenseChecker::NewL();
    CleanupStack::PushL( checker );
    
    ret = checker->CheckLicense( KTestFile, buffer );
    
    T1L( ret, KErrNone );
    
    // Passed
    CleanupStack::PopAndDestroy( checker );
    iLog->Log( KPassed );
    
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CDrmLicenseCheckerTest::CheckLicenseTestL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    }

// End of File

    