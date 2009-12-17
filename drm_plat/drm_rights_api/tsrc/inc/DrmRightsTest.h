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
* Description:  DrmRightsTest test module.
*
*/



#ifndef DRMRIGHTSTEST_H
#define DRMRIGHTSTEST_H

// INCLUDES
#include "StifTestModule.h"
#include <StifLogger.h>

// Logging path
_LIT( KDrmRightsTestLogPath, "\\logs\\testframework\\DrmRightsTest\\" ); 
// Log file
_LIT( KDrmRightsTestLogFile, "DrmRightsTest.txt" ); 

#define GETPTR &
#define ENTRY(str,func) {_S(str), GETPTR func,0,0,0}
#define FUNCENTRY(func) {_S(#func), GETPTR func,0,0,0}
#define OOM_ENTRY(str,func,a,b,c) {_S(str), GETPTR func,a,b,c}
#define OOM_FUNCENTRY(func,a,b,c) {_S(#func), GETPTR func,a,b,c}

// FORWARD DECLARATIONS
class CDrmRightsTest;

typedef TInt (CDrmRightsTest::* TestFunction)(TTestResult&);

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
*  This a DrmRightsTest class.
*  ?other_description_lines
*
*  @lib DrmRightsTest.lib
*  @since S60 3.2
*/
NONSHARABLE_CLASS(CDrmRightsTest) : public CTestModuleBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CDrmRightsTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CDrmRightsTest();

    
    public: // Functions from base classes

        /**
        * From CTestModuleBase InitL is used to initialize the DrmRightsTest. 
        *       It is called once for every instance of DrmRightsTest after
        *       its creation.
        * @since S60 3.2
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first 
        *               created instance of DrmRightsTest.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * From CTestModuleBase GetTestCasesL is used to inquiry test cases 
        *   from DrmRightsTest. 
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
                                     TOOMFailureType& /* aFailureType */,
                                     TInt& /* aFirstMemFailure */, 
                                     TInt& /* aLastMemFailure */ );

        /**
        * From CTestModuleBase; User may add implementation for OOM test 
        * environment initialization. Usually no implementation is required.
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        */
        virtual void OOMTestInitializeL( const TFileName& /* aTestCaseFile */, 
                                    const TInt /* aCaseNumber */ ); 

        /**
        * From CTestModuleBase; OOMHandleWarningL. User may add implementation 
        * for OOM test warning handling. Usually no implementation is required.
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailNextValue FailNextValue for OOM test execution (optional)
        * @return None
        */
        virtual void OOMHandleWarningL( const TFileName& /* aTestCaseFile */,
                                        const TInt /* aCaseNumber */, 
                                        TInt& /* aFailNextValue */);

        /**
        * From CTestModuleBase; OOMTestFinalizeL may be used to finalize OOM
        * test environment. Usually no implementation is required.
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        */
        virtual void OOMTestFinalizeL( const TFileName& /* aTestCaseFile */, 
                                       const TInt /* aCaseNumber */ );

    private:

        /**
        * C++ default constructor.
        */
        CDrmRightsTest();

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
        * Constructor test, constructs DcfCommon object from the file
        * of which the name is given as an argument.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ConstructDcfCommonFileNameTestL( TTestResult& aResult );
        
        /**
        * Constructor test, constructs DcfCommon object from the file
        * to which a file handle is given as an argument.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ConstructDcfCommonFileHandleTestL( TTestResult& aResult );
        
        /**
        * Constructor tests, constructs CDrmAsset object.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ConstructAssetTestL( TTestResult& aResult );
        
        /**
        * Duplication test, duplicates CDrmAsset object.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt AssetDuplicateTestL( TTestResult& aResult );
        
        /**
        * Externalization/Internalization test, CDrmAsset object
        * is first externalized and then internalized.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt AssetExternalizeInternalizeTestL( TTestResult& aResult );
        
        /**
        * Constructor test, constructs CDrmPermission object
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ConstructPermissionTestL( TTestResult& aResult );
        
        /**
        * Size definition test, counts the size of CDrmPermission object.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt PermissionSizeTestL( TTestResult& aResult );
        
        /**
        * Externalization/Internalization test, CDrmPermission object
        * is first externalized and then internalized.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt PermissionExternalizeInternalizeTestL( TTestResult& aResult );
        
        /**
        * Test for checking whether Permission is stateful or not.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt PermissionStatefulTestL( TTestResult& aResult );
        
        /**
        * Test for checking whether rights is of parent or child type.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt PermissionChildTestL( TTestResult& aResult );
        
        /**
        * Test for fetching constraint matching the given intent.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt PermissionConstraintForIntentTestL( TTestResult& aResult );
        
        /**
        * Test for checking top level constraint of the permission.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt PermissionTopLevelConstraintTestL( TTestResult& aResult );
        
        /**
        * Test for consuming rights of the permission.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt PermissionConsumeRightsTestL( TTestResult& aResult );
        
        /**
        * Test for exporting and importing permission.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt PermissionExportImportTestL( TTestResult& aResult );
        
        /**
        * Test for duplicating permission.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt PermissionDuplicateTestL( TTestResult& aResult );
        
        /**
        * Test for checking if permission is expired.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt PermissionExpiredTestL( TTestResult& aResult );
        
        /**
        * Test for checking if permission is valid.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt PermissionValidTestL( TTestResult& aResult );
        
        /**
        * Test for merging two permissions.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt PermissionMergeTestL( TTestResult& aResult );
        
        /**
        * Test for checking if permission has software constraint or not.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt PermissionSoftwareConstrainedTestL( TTestResult& aResult );
        
        /**
        * Constructor test, construct CDrmConstraint.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ConstructConstraintTestL( TTestResult& aResult );
        
        /**
        * Constructor test, construct CDrmRightsConstraints.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ConstructRightsConstraintsTestL( TTestResult& aResult );
        
        /**
        * Test for checking if object has full rights or not.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsConstraintsFullRightsTestL( TTestResult& aResult );
        
        /**
        * Test for checking if object has preview rights constraint.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsConstraintsIsPreviewTestL( TTestResult& aResult );
        
        /**
        * Test for checking the expiration and constraint details
        * of the object.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsConstraintsGetConstraintInfoTestL( TTestResult& aResult );
        
        /**
        * Test for checking the end time and amount of counts left of
        * of the object.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsConstraintsGetExpirationDetailsTestL( 
            TTestResult& aResult );
        
        /**
        * Test for checking the current and original value of the count
        * restriction.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsConstraintsGetCountersTestL( TTestResult& aResult );
        
        /**
        * Test for checking the current and original value of the timed
        * count restriction.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsConstraintsGetTimedCountersTestL( TTestResult& aResult );
        
        /**
        * Test for checking the start time constraint.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsConstraintsGetStartTimeTestL( TTestResult& aResult );
        
       /**
        * Test for checking the end time constraint.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsConstraintsGetEndTimeTestL( TTestResult& aResult );
        
        /**
        * Test for checking the interval time constraint.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsConstraintsGetIntervalTestL( TTestResult& aResult );
        
        /**
        * Test for checking the interval start time constraint.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsConstraintsGetIntervalStartTestL( TTestResult& aResult );
        
        /**
        * Test for checking the accumulated time constraint.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsConstraintsGetAccumulatedTestL( TTestResult& aResult );
        
        /**
        * Test for setting the counters of the constraint.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsConstraintsSetCountersTestL( TTestResult& aResult );
        
        /**
        * Test for setting the start time constraint.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsConstraintsSetStartTimeTestL( TTestResult& aResult );
        
        
        /**
        * Test for setting the end time constraint.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsConstraintsSetEndTimeTestL( TTestResult& aResult );
        
        /**
        * Test for setting the interval time constraint.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsConstraintsSetIntervalTestL( TTestResult& aResult );
        
        /**
        * Test for setting the interval start time constraint.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsConstraintsSetIntervalStartTestL( TTestResult& aResult );
        
        /**
        * Test for getting constraint.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsConstraintsGetConstraintTestL( TTestResult& aResult );
        
        /**
        * Test for setting constraint.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsConstraintsSetConstraintTestL( TTestResult& aResult );
        
        /**
        * Test for assigning constraint to another constraint.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsConstraintsAssignmentTestL( TTestResult& aResult );
        
        /**
        * Test for getting the local id of the object.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsGetLocalIdTestL( TTestResult& aResult );
        
        /**
        * Test for getting the time when rights object was added to
        * the database.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsGetAddTimeTestL( TTestResult& aResult );
               
        /**
        * Test for getting the restriction details associated with
        * given permission.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsGetRightsInfoTestL( TTestResult& aResult );
     
        /**
        * Test for getting the end time and amount of counts left of 
        * the object associated with given permission.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsGetExpirationDetailsTestL( TTestResult& aResult );
        
        /**
        * Test for getting the constraints associated with the print
        * right.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsGetPrintRightTestL( TTestResult& aResult );
        
        /**
        * Test for setting the content URI and Local Id of the object
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsSetContentURIAndLocalIDTestL( TTestResult& aResult );
        
        /**
        * Test for setting the content URI of the object.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsSetContentURITestL( TTestResult& aResult );
        
       /**
        * Test for setting the add time of the object.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsSetAddTimeTestL( TTestResult& aResult );
         
       /**
        * Test for setting the play right constraint of the object.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsSetPlayRightTestL( TTestResult& aResult );
        
      /**
        * Test for setting the display right constraint of the object.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsSetDisplayRightTestL( TTestResult& aResult );     
     
      /**
        * Test for setting the print right constraint of the object.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsSetPrintRightTestL( TTestResult& aResult );  
      
      /**
        * Test for setting the execute right constraint of the object.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsSetExecuteRightTestL( TTestResult& aResult );  
      
      /**
        * Test for assignment operator.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsAssignmentTestL( TTestResult& aResult );
      
      /**
        * Test for externalizing and internalizing rights object.
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsExternalizeInternalizeTestL( TTestResult& aResult );
      
      /**
        * Test for 
        * @since S60 3.2
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt RightsMergeTestL( TTestResult& aResult );
      
      
    private:    // Data
        // Pointer to test (function) to be executed
        TestFunction iMethod;

        CStifLogger * iLog;

   
    };

#endif      // DRMRIGHTSTEST_H

// End of File
