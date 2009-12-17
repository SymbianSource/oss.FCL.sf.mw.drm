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
* Description:  DrmHelperTest test module.
*
*/


#ifndef DRMHELPERTEST_H
#define DRMHELPERTEST_H

// INCLUDES
#include <f32file.h>
#include <stiflogger.h>
#include "stiftestmodule.h"

// CONSTANTS

// MACROS

// Logging path
_LIT( KDrmHelperTestLogPath, "\\logs\\testframework\\drmhelpertest\\" );
// Log file
_LIT( KDrmHelperTestLogFile, "drmhelpertest.txt" );


#define GETPTR &
#define ENTRY(str,func) {_S(str), GETPTR func,0,0,0}
#define FUNCENTRY(func) {_S(#func), GETPTR func,0,0,0}
#define OOM_ENTRY(str,func,a,b,c) {_S(str), GETPTR func,a,b,c}
#define OOM_FUNCENTRY(func,a,b,c) {_S(#func), GETPTR func,a,b,c}

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CDrmHelperTest;

// DATA TYPES
typedef TInt (CDrmHelperTest::* TestFunction)(TTestResult&);

// CLASS DECLARATION

/**
*  An internal structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib drmhelpertest.lib
*  @since S60 v3.2
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
*  @lib drmhelpertest.lib
*  @since S60 v3.2
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
*  This a DrmHelperTest class.
*
*  @lib drmhelpertest.lib
*  @since S60 v3.2
*/
NONSHARABLE_CLASS( CDrmHelperTest ) : public CTestModuleBase
    {
    public:  // Constructors and destructor


        /**
        * Two-phased constructor.
        */
        static CDrmHelperTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CDrmHelperTest();

    public: // Functions from base classes

        /**
        * From CTestModuleBase InitL is used to initialize the
        *       DrmHelperTest. It is called once for every instance of
        *       TestModuleDrmHelperTest after its creation.
        * @since S60 v3.2
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first
        *               created instance of DrmHelperTest.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * From CTestModuleBase GetTestCasesL is used to inquiry test cases
        *   from DrmHelperTest.
        * @since S60 v3.2
        * @param aTestCaseFile Test case file (optional)
        * @param aTestCases  Array of TestCases returned to test framework
        * @return Symbian OS error code
        */
        TInt GetTestCasesL( const TFileName& aTestCaseFile,
                            RPointerArray<TTestCaseInfo>& aTestCases );

        /**
        * From CTestModuleBase RunTestCaseL is used to run an individual
        *   test case.
        * @since S60 v3.2
        * @param aCaseNumber Test case number
        * @param aTestCaseFile Test case file (optional)
        * @param aResult Test case result returned to test framework (PASS/FAIL)
        * @return Symbian OS error code (test case execution error, which is
        *           not reported in aResult parameter as test case failure).
        */
        TInt RunTestCaseL( const TInt aCaseNumber,
                           const TFileName& aTestCaseFile,
                           TTestResult& aResult );

    private:

        /**
        * C++ default constructor.
        */
        CDrmHelperTest();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // CDrmHelperTest( const CDrmHelperTest& );
        // Prohibit assigment operator if not deriving from CBase.
        // CDrmHelperTest& operator=( const CDrmHelperTest& );

        /**
        * Function returning test case name and pointer to test case function.
        * @since S60 v3.2
        * @param aCaseNumber test case number
        * @return TCaseInfo
        */
        const TCaseInfo Case ( const TInt aCaseNumber ) const;

        /**
        * Function for setting test case result.
        * @since S60 v3.2
        * @param aResult result structure of case under test
        * @param aError (first seen) error code
        */
        void SetResult( TTestResult& aResult, TInt aError );

        /**
        * Actual Hardcoded test case functions are listed below.
        */

        /**
        * CDRMHelper construct test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt ConstructTestL( TTestResult& aResult );

        /**
        * IndicateIdle test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt IndicateIdleTestL( TTestResult& aResult );


        /**
        * ActivateContentL test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt ActivateContentLTestL( TTestResult& aResult );


        /**
        * CanSetAutomated test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt CanSetAutomatedTestL( TTestResult& aResult );


        /**
        * CheckRightsAmountL test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt CheckRightsAmountLTestL( TTestResult& aResult );


        /**
        * CheckRightsPercentL test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt CheckRightsPercentLTestL( TTestResult& aResult );


        /**
        * Consume2 test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt Consume2TestL( TTestResult& aResult );


        /**
        * ConsumeFile2 test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt ConsumeFile2TestL( TTestResult& aResult );


        /**
        * EmbeddedPreviewCompletedL test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt EmbeddedPreviewCompletedLTestL( TTestResult& aResult );


        /**
        * GetContentURIList test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt GetContentURIListTestL( TTestResult& aResult );


        /**
        * GetPreviewRightsL test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt GetPreviewRightsLTestL( TTestResult& aResult );


        /**
        * GetRightsDetailsL test case for CDRMRightsConstraints.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt GetRightsDetailsLTestL( TTestResult& aResult );


        /**
        * GetRightsDetailsL test case for CDRMHelperRightsConstraints.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt GetHlpRightsDetailsLTestL( TTestResult& aResult );


        /**
        * HandleErrorL test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt HandleErrorLTestL( TTestResult& aResult );


        /**
        * HandleErrorOrPreviewL test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt HandleErrorOrPreviewLTestL( TTestResult& aResult );


        /**
        * HasInfoUrlL test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt HasInfoUrlLTestL( TTestResult& aResult );


        /**
        * HasPreviewL test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt HasPreviewLTestL( TTestResult& aResult );


        /**
        * LaunchDetailsViewEmbeddedL test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt LaunchDetailsViewEmbeddedLTestL( TTestResult& aResult );


        /**
        * OpenInfoUrlL test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt OpenInfoUrlLTestL( TTestResult& aResult );


        /**
        * RemoveAutomated test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt RemoveAutomatedTestL( TTestResult& aResult );


        /**
        * RemoveAutomatedPassive test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt RemoveAutomatedPassiveTestL( TTestResult& aResult );


        /**
        * Test case for class CDRMHelperRightsConstraints.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt RightsConstraintsTestL( TTestResult& aResult );


        /**
        * SetAutomated test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt SetAutomatedTestL( TTestResult& aResult );


        /**
        * SetAutomatedPassive test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt SetAutomatedPassiveTestL( TTestResult& aResult );


        /**
        * SetAutomatedSilent test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt SetAutomatedSilentTestL( TTestResult& aResult );


        /**
        * SetAutomatedType test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt SetAutomatedTypeTestL( TTestResult& aResult );


        /**
        * Test case for testing empty set limits methods.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt SetLimitsTestL( TTestResult& aResult );


        /**
        * SetPreviewMediaType test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt SetPreviewMediaTypeTestL( TTestResult& aResult );


        /**
        * ShowAutomatedNote test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt ShowAutomatedNoteTestL( TTestResult& aResult );


        /**
        * ShowDRMUINotification2L test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt ShowDRMUINotification2LTestL( TTestResult& aResult );


        /**
        * SupportedDRMMethods2 test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt SupportedDRMMethods2TestL( TTestResult& aResult );


        /**
        * SupportedDataType test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt SupportedDataTypeTestL( TTestResult& aResult );


        /**
        * UnRegisterDataType test case.
        * @since S60 v3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt UpdateDataTypesTestL( TTestResult& aResult );

    private:    // Data
        // Pointer to test (function) to be executed
        TestFunction iMethod;

        // Pointer to logger
        CStifLogger* iLog;

        //Handle to RFs
        RFs iFs;
    };

#endif      // DRMHELPERTEST_H

// End of File
