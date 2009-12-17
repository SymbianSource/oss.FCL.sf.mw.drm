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
* Description:  DRMCommonTest test module.
*
*/



#ifndef DRMCOMMONTEST_H
#define DRMCOMMONTEST_H

// INCLUDES
#include "StifTestModule.h"
#include <StifLogger.h>

// Logging path
_LIT( KDRMCommonTestLogPath, "\\logs\\testframework\\DRMCommonTest\\" ); 
// Log file
_LIT( KDRMCommonTestLogFile, "DRMCommonTest.txt" ); 


#define GETPTR &
#define ENTRY(str,func) {_S(str), GETPTR func,0,0,0}
#define FUNCENTRY(func) {_S(#func), GETPTR func,0,0,0}
#define OOM_ENTRY(str,func,a,b,c) {_S(str), GETPTR func,a,b,c}
#define OOM_FUNCENTRY(func,a,b,c) {_S(#func), GETPTR func,a,b,c}

// FORWARD DECLARATIONS
class CDRMCommonTest;

typedef TInt (CDRMCommonTest::* TestFunction)(TTestResult&);

// CLASS DECLARATION

/**
*  An internal structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib DRMCommonTest.lib
*  @since S60 3.2
*/
class TCaseInfoInternal
    {
    public:
        const TText*    iCaseName;
        TestFunction    iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;
    };

// CLASS DECLARATION

/**
*  A structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib DRMCommonTest.lib
*  @since S60 3.2
*/
class TCaseInfo
    {
    public:
        TPtrC iCaseName;
        TestFunction iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;

    TCaseInfo( const TText* a ) : iCaseName( (TText*) a )
        {
        };

    };


// CLASS DECLARATION

/**
*  This a DRMCommonTest class.
*
*  @lib DRMCommonTest.lib
*  @since S60 3.2
*/
NONSHARABLE_CLASS(CDRMCommonTest) : public CTestModuleBase
    {
    public:  // Constructors and destructor


        /**
        * Two-phased constructor.
        */
        static CDRMCommonTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CDRMCommonTest();

    public: // Functions from base classes

        /**
        * From CTestModuleBase InitL is used to initialize the 
        *       DRMCommonTest. It is called once for every instance of 
        *       TestModuleDRMCommonTest after its creation.
        * @since S60 3.2
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first 
        *               created instance of DRMCommonTest.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * From CTestModuleBase GetTestCasesL is used to inquiry test cases 
        *   from DRMCommonTest. 
        * @since S60 3.2
        * @param aTestCaseFile Test case file (optional)
        * @param aTestCases  Array of TestCases returned to test framework
        * @return Symbian OS error code
        */
        TInt GetTestCasesL( const TFileName& aTestCaseFile, 
                            RPointerArray<TTestCaseInfo>& aTestCases );

        /**
        * From CTestModuleBase RunTestCaseL is used to run an individual 
        *   test case. 
        * @since S60 3.2
        * @param aCaseNumber Test case number
        * @param aTestCaseFile Test case file (optional)
        * @param aResult Test case result returned to test framework (PASS/FAIL)
        * @return Symbian OS error code (test case execution error, which is 
        *           not reported in aResult parameter as test case failure).
        */   
        TInt RunTestCaseL( const TInt aCaseNumber, 
                           const TFileName& aTestCaseFile,
                           TTestResult& aResult );

        /**
        * From CTestModuleBase; OOMTestQueryL is used to specify is particular
        * test case going to be executed using OOM conditions
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailureType OOM failure type (optional)
        * @param aFirstMemFailure The first heap memory allocation failure value (optional)
        * @param aLastMemFailure The last heap memory allocation failure value (optional)
        * @return TBool
        */
        virtual TBool OOMTestQueryL( const TFileName& /* aTestCaseFile */, 
                                     const TInt /* aCaseNumber */, 
                                     TOOMFailureType& aFailureType, 
                                     TInt& /* aFirstMemFailure */, 
                                     TInt& /* aLastMemFailure */ );

        /**
        * From CTestModuleBase; OOMTestInitializeL may be used to initialize OOM
        * test environment
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        */
        virtual void OOMTestInitializeL( const TFileName& /* aTestCaseFile */, 
                                    const TInt /* aCaseNumber */ ); 

        /**
        * From CTestModuleBase; OOMHandleWarningL
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailNextValue FailNextValue for OOM test execution (optional)
        * @return None
        *
        * User may add implementation for OOM test warning handling. Usually no
        * implementation is required.           
        */
        virtual void OOMHandleWarningL( const TFileName& /* aTestCaseFile */,
                                        const TInt /* aCaseNumber */, 
                                        TInt& /* aFailNextValue */); 

        /**
        * From CTestModuleBase; OOMTestFinalizeL may be used to finalize OOM
        * test environment
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        *
        */
        virtual void OOMTestFinalizeL( const TFileName& /* aTestCaseFile */, 
                                       const TInt /* aCaseNumber */ );

    private:

        /**
        * C++ default constructor.
        */
        CDRMCommonTest();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Function returning test case name and pointer to test case function.
        * @since S60 3.2
        * @param aCaseNumber test case number
        * @return TCaseInfo 
        */
        const TCaseInfo Case ( const TInt aCaseNumber ) const;

        /**
        * Constructor tests..
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ConstructTestL( TTestResult& aResult );

        /**
        * Version tests
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt VersionTestL( TTestResult& aResult );

        /**
        * Connection tests
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ConnectionTestL( TTestResult& aResult );
        
        /**
        * Contents rights test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt CheckContentRightsTestL( TTestResult& aResult );
        
        /**
        * Contents rights test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt CheckFileRightsTestForFileNameL( TTestResult& aResult );
        
        /**
        * Contents rights test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt CheckFileRightsTestForFileHandleL( TTestResult& aResult );
        
        /**
        * Contents protection test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt IsProtectedContentTestL( TTestResult& aResult );
        
        /**
        * Contents protection test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt IsProtectedFileTestForFileNameL( TTestResult& aResult );
        
        /**
        * Contents protection test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt IsProtectedFileTestForFileHandleL( TTestResult& aResult );
        
        /**
        * Contents info test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt GetContentInfoTestL( TTestResult& aResult );
        
        /**
        * Contents info test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt GetFileInfoTestForFileNameL( TTestResult& aResult );
        
        /**
        * Contents info test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt GetFileInfoTestForFileHandleL( TTestResult& aResult );
        
        /**
        * Contents header test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt GetContentHeaderTestL( TTestResult& aResult );
        
        /**
        * Contents header test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt GetFileHeaderTestForFileNameL( TTestResult& aResult );
        
        /**
        * Contents header test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt GetFileHeaderTestForFileHandleL( TTestResult& aResult );
        
        /**
        * Set content header test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt SetContentHeaderTestL( TTestResult& aResult );
        
        /**
        * Set content header test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt SetFileHeaderTestForFileNameL( TTestResult& aResult );
        
        /**
        * Set content header test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt SetFileHeaderTestForFileHandleL( TTestResult& aResult );
        
        /**
        * Get rights object test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt GetSingleRightsObjectTestL( TTestResult& aResult );
        
        /**
        * Get detailed content rights test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt GetDetailedContentRightsTestL( TTestResult& aResult );
        
        /**
        * Get detailed content rights test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt GetDetailedFileRightsTestForFileNameL( TTestResult& aResult );
        
        /**
        * Get detailed content rights test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt GetDetailedFileRightsTestForFileHandleL( TTestResult& aResult );
        
        /**
        * Get active rights test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt GetActiveRightsTestL( TTestResult& aResult );
        
        /**
        * Get content uri list test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt GetContentURIListTestL( TTestResult& aResult );
        
        /**
        * Data types tests
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DataTypesTestL( TTestResult& aResult );
        
        /**
        * Merge parent and child rights test
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt MergeParentAndChildTestL( TTestResult& aResult );

    private:    // Data
        // Pointer to test (function) to be executed
        TestFunction iMethod;

        // Pointer to logger
        CStifLogger * iLog; 

    };

#endif      // DRMCOMMONTEST_H

// End of File