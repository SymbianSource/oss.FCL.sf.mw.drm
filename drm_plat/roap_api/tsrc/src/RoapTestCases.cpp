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
* Description:  ?Description
*
*/



// [INCLUDE FILES] - do not remove
#include <e32math.h>
#include "RoapTest.h"
#include "RoapTrigger.h"
#include "RoapEng.h"
#include "RoapEngBase.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def
_LIT( KRunning, "Running" );
_LIT( KFinished, "Finished" );
_LIT( KPassed, "Passed" );

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRoapTest::Case
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
const TCaseInfo CRoapTest::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    RoapTest.cpp file and to RoapTest.h 
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
        // [test cases entries] - do not remove
        
        // NOTE: When compiled to GCCE, there must be Classname::
        // declaration in front of the method name, e.g. 
        // CRoapTest::PrintTest. Otherwise the compiler
        // gives errors.
        
        ENTRY( "RoapTrigger::ConstructionDestructionTrigger", CRoapTest::ConstructionDestructionTriggerL ),
        ENTRY( "RoapTrigger::MessageAsXml", CRoapTest::MessageAsXmlL ),
        ENTRY( "RoapTrigger::ValidTrigger", CRoapTest::ValidTriggerL ),
        ENTRY( "RoapEng/RoapEngBase::ConstructionDestructionRoapEng", CRoapTest::ConstructionDestructionRoapEngL ),
        ENTRY( "RoapEngBase::SetTrigger", CRoapTest::SetTriggerL ),
        ENTRY( "RoapEngBase::Accept", CRoapTest::AcceptL ),
        ENTRY( "RoapEngBase::Reject", CRoapTest::RejectL ),
        ENTRY( "RoapEngBase::DoCleanup", CRoapTest::DoCleanupL ),
        ENTRY( "RoapEngBase::HandleRoReponse", CRoapTest::HandleRoReponseL ),
        ENTRY( "RoapEngBase::Trigger", CRoapTest::TriggerL )
                
        // Example how to use OOM functionality
        //OOM_ENTRY( "Loop test with OOM", CRoapTest::LoopTest, ETrue, 2, 3),
        //OOM_FUNCENTRY( CRoapTest::PrintTest, ETrue, 1, 3 ),
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


// from CRoapTrigger

// -----------------------------------------------------------------------------
// CRoapTest::
// -----------------------------------------------------------------------------
// 
TInt CRoapTest::ConstructionDestructionTriggerL( TTestResult& aResult )
    {
    _LIT( KData, "CRoapTest::ConstructionDestructionTriggerL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase1, "TestCase1: Testing NewL:s of RoapTrigger: Expecting creation to succeed" );
    iLog->Log( KTestCase1 );
    
    Roap::CRoapTrigger* trigger = Roap::CRoapTrigger::NewL();

    TL( trigger );
    delete trigger;
    
    
    Roap::TTriggerType type = Roap::ERegistrationTrigger;
    _LIT8(KRiIdDummy,"abcdefghijabcdefghij");
    _LIT8(KDomainIdDummy,"DomainId001");
    _LIT8(KRoapUrl,"http://www.localhost.localdomain:80");
    RPointerArray<HBufC8> listRo;
    RPointerArray<HBufC8> listCid;
    
    trigger = Roap::CRoapTrigger::NewL( type,
                                        KRiIdDummy,
                                        KDomainIdDummy,
                                        KRoapUrl,
                                        listRo,
                                        listCid);

    TL( trigger );
    delete trigger;
    
    //Passed
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CRoapTest::ConstructionDestructionTriggerL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    };
                                  
// -----------------------------------------------------------------------------
// CRoapTest::
// -----------------------------------------------------------------------------
//                                 
TInt CRoapTest::MessageAsXmlL( TTestResult& aResult )
    {
    _LIT( KData, "CRoapTest::MessageAsXmlL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase2, "TestCase2: Testing MessageAsXmlL of RoapTrigger: Expecting an output buffer" );
    iLog->Log( KTestCase2 );
    
    Roap::CRoapTrigger* trigger = Roap::CRoapTrigger::NewL();
    HBufC8* xmlData = NULL;
    
    Roap::TTriggerType type = Roap::ERegistrationTrigger;
    _LIT8(KRiIdDummy,"abcdefghijabcdefghij");
    _LIT8(KDomainIdDummy,"DomainId001");
    _LIT8(KRoapUrl,"http://www.localhost.localdomain:80");
    RPointerArray<HBufC8> listRo;
    RPointerArray<HBufC8> listCid;
    
    trigger = Roap::CRoapTrigger::NewL( type,
                                        KRiIdDummy,
                                        KDomainIdDummy,
                                        KRoapUrl,
                                        listRo,
                                        listCid);
                                                                            
    TL( trigger );
    CleanupStack::PushL(trigger);
    
    xmlData = trigger->MessageAsXmlL();
    
    delete xmlData;
    CleanupStack::PopAndDestroy();
    
    //Passed
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CRoapTest::MessageAsXmlL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;    
    };
        
        
// -----------------------------------------------------------------------------
// CRoapTest::
// -----------------------------------------------------------------------------
//        
TInt CRoapTest::ValidTriggerL( TTestResult& aResult )
    {
    _LIT( KData, "CRoapTest::MessageAsXmlL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase3, "TestCase3: Testing ValidTrigger of RoapTrigger: Expecting an output buffer" );
    iLog->Log( KTestCase3 );
    
    Roap::CRoapTrigger* trigger = Roap::CRoapTrigger::NewL();
    
    Roap::TTriggerType type = Roap::ERegistrationTrigger;
    _LIT8(KRiIdDummy,"abcdefghijabcdefghij");
    _LIT8(KDomainIdDummy,"DomainId001");
    _LIT8(KRoapUrl,"http://www.localhost.localdomain:80");
    RPointerArray<HBufC8> listRo;
    RPointerArray<HBufC8> listCid;
    
    trigger = Roap::CRoapTrigger::NewL( type,
                                        KRiIdDummy,
                                        KDomainIdDummy,
                                        KRoapUrl,
                                        listRo,
                                        listCid);
                                                                            
    TL( trigger );
    
    TL( trigger->ValidTrigger() );
    
    delete trigger;
    
    //Passed
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CRoapTest::ValidTriggerL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;    
    };

// from CRoapEng

// -----------------------------------------------------------------------------
// CRoapTest::
// -----------------------------------------------------------------------------
// 
TInt CRoapTest::ConstructionDestructionRoapEngL( TTestResult& aResult )
    {
    _LIT( KData, "CRoapTest::ConstructionDestructionRoapEngL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase4, "TestCase4: Testing NewL:s of CRoapEng and CRoapEngBase: Expecting a working RoapEng" );
    iLog->Log( KTestCase4 );
    
    Roap::CRoapEng* roapEng = Roap::CRoapEng::NewL();
    
    TL( roapEng );
    
    delete roapEng;
    
    //Passed
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CRoapTest::ConstructionDestructionRoapEngL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;
    };
        

// from CRoapEngBase
    
// -----------------------------------------------------------------------------
// CRoapTest::
// -----------------------------------------------------------------------------
// 
TInt CRoapTest::SetTriggerL( TTestResult& aResult )
    {
    _LIT( KData, "CRoapTest::SetTriggerL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase5, "TestCase5: Testing SetTriggerL of RoapEngBase" );
    iLog->Log( KTestCase5 );
    
    Roap::CRoapTrigger* trigger = Roap::CRoapTrigger::NewL();
    HBufC8* xmlData = NULL;
    
    Roap::TTriggerType type = Roap::ERegistrationTrigger;
    _LIT8(KRiIdDummy,"abcdefghijabcdefghij");
    _LIT8(KDomainIdDummy,"DomainId001");
    _LIT8(KRoapUrl,"http://www.localhost.localdomain:80");
    RPointerArray<HBufC8> listRo;
    RPointerArray<HBufC8> listCid;
    Roap::TRiContextStatus riStatus;
    Roap::TDomainOperation domainOperation;    
    
    trigger = Roap::CRoapTrigger::NewL( type,
                                        KRiIdDummy,
                                        KDomainIdDummy,
                                        KRoapUrl,
                                        listRo,
                                        listCid);
                                                                            
    TL( trigger );
    CleanupStack::PushL(trigger);
    
    xmlData = trigger->MessageAsXmlL();
    CleanupStack::PushL( xmlData );
    
    Roap::CRoapEng* roapEng = Roap::CRoapEng::NewL();
    CleanupStack::PushL( roapEng );
    
    // Actual Test:
    roapEng->SetTriggerL( *xmlData,
                          NULL,
                          type,
                          riStatus,
                          domainOperation,
                          listCid );
    
    
    CleanupStack::PopAndDestroy(roapEng);
    CleanupStack::PopAndDestroy(xmlData);
    CleanupStack::PopAndDestroy(trigger);
            
    //Passed
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CRoapTest::SetTriggerL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;     
    };

// -----------------------------------------------------------------------------
// CRoapTest::
// -----------------------------------------------------------------------------
//  
TInt CRoapTest::AcceptL( TTestResult& aResult )
    {
    _LIT( KData, "CRoapTest::AcceptL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase6, "TestCase6: Testing AcceptL of CRoapEngBase" );
    iLog->Log( KTestCase6 );
    
    Roap::CRoapEng* roapEng = Roap::CRoapEng::NewL();
    TRequestStatus statusData;
    TRequestStatus* status = &statusData;
    
    TL( roapEng );
    
    // Ignore error for now, method got called ok:
    TRAPD( err, roapEng->AcceptL( NULL, status ));
    
    delete roapEng;
    
    //Passed
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CRoapTest::AcceptL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;    
    };
        
// -----------------------------------------------------------------------------
// CRoapTest::
// -----------------------------------------------------------------------------
// 
TInt CRoapTest::RejectL( TTestResult& aResult )
    {
    _LIT( KData, "CRoapTest::RejectL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase7, "TestCase7: Testing RejectL of CRoapEngBase" );
    iLog->Log( KTestCase7 );
    
    
    Roap::CRoapTrigger* trigger = Roap::CRoapTrigger::NewL();
    HBufC8* xmlData = NULL;
    
    Roap::TTriggerType type = Roap::ERegistrationTrigger;
    _LIT8(KRiIdDummy,"abcdefghijabcdefghij");
    _LIT8(KDomainIdDummy,"DomainId001");
    _LIT8(KRoapUrl,"http://www.localhost.localdomain:80");
    RPointerArray<HBufC8> listRo;
    RPointerArray<HBufC8> listCid;
    Roap::TRiContextStatus riStatus;
    Roap::TDomainOperation domainOperation;    
    
    trigger = Roap::CRoapTrigger::NewL( type,
                                        KRiIdDummy,
                                        KDomainIdDummy,
                                        KRoapUrl,
                                        listRo,
                                        listCid);
                                                                            
    TL( trigger );
    CleanupStack::PushL(trigger);
    
    xmlData = trigger->MessageAsXmlL();
    CleanupStack::PushL( xmlData );
    
    Roap::CRoapEng* roapEng = Roap::CRoapEng::NewL();
    CleanupStack::PushL( roapEng );
    

    roapEng->SetTriggerL( *xmlData,
                          NULL,
                          type,
                          riStatus,
                          domainOperation,
                          listCid );
    
    // Actual Test:    
    roapEng->Reject();
    
    CleanupStack::PopAndDestroy(roapEng);
    CleanupStack::PopAndDestroy(xmlData);
    CleanupStack::PopAndDestroy(trigger);
    
    //Passed
    iLog->Log( KPassed );
    
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CRoapTest::RejectL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;    
    };
        
// -----------------------------------------------------------------------------
// CRoapTest::
// -----------------------------------------------------------------------------
// 
TInt CRoapTest::DoCleanupL( TTestResult& aResult )
    {
    _LIT( KData, "CRoapTest::DoCleanupL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase8, "TestCase8: Testing DoCleanupL of CRoapEngBase" );
    iLog->Log( KTestCase8 );
    
    Roap::CRoapTrigger* trigger = Roap::CRoapTrigger::NewL();
    HBufC8* xmlData = NULL;
    
    Roap::TTriggerType type = Roap::ERegistrationTrigger;
    _LIT8(KRiIdDummy,"abcdefghijabcdefghij");
    _LIT8(KDomainIdDummy,"DomainId001");
    _LIT8(KRoapUrl,"http://www.localhost.localdomain:80");
    RPointerArray<HBufC8> listRo;
    RPointerArray<HBufC8> listCid;
    Roap::TRiContextStatus riStatus;
    Roap::TDomainOperation domainOperation;    
    
    trigger = Roap::CRoapTrigger::NewL( type,
                                        KRiIdDummy,
                                        KDomainIdDummy,
                                        KRoapUrl,
                                        listRo,
                                        listCid);
                                                                            
    TL( trigger );
    CleanupStack::PushL(trigger);
    
    xmlData = trigger->MessageAsXmlL();
    CleanupStack::PushL( xmlData );
    
    Roap::CRoapEng* roapEng = Roap::CRoapEng::NewL();
    CleanupStack::PushL( roapEng );
    

    roapEng->SetTriggerL( *xmlData,
                          NULL,
                          type,
                          riStatus,
                          domainOperation,
                          listCid );
    
    // Actual Test:    
    roapEng->DoCleanup();
    
    CleanupStack::PopAndDestroy(roapEng);
    CleanupStack::PopAndDestroy(xmlData);
    CleanupStack::PopAndDestroy(trigger);
    
    //Passed
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CRoapTest::DoCleanupL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;     
    };   
        
// -----------------------------------------------------------------------------
// CRoapTest::
// -----------------------------------------------------------------------------
// 
TInt CRoapTest::HandleRoReponseL( TTestResult& aResult )
    {
    _LIT( KData, "CRoapTest::HandleRoReponseL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase9, "TestCase9: Testing HandleRoReponseL of CRoapEngBase" );
    iLog->Log( KTestCase9 );
    
    Roap::CRoapEng* roapEng = Roap::CRoapEng::NewL();
    CleanupStack::PushL( roapEng );
    
    RPointerArray<CDRMRights> rights;
    
    TRAPD(err, roapEng->HandleRoReponseL( KNullDesC8, rights ));
    
    CleanupStack::PopAndDestroy(roapEng);
    
    //Passed
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CRoapTest::HandleRoReponseL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;    
    }; 
        

// -----------------------------------------------------------------------------
// CRoapTest::
// -----------------------------------------------------------------------------
// 
TInt CRoapTest::TriggerL( TTestResult& aResult )
    {
    _LIT( KData, "CRoapTest::TriggerL" );
    TestModuleIf().Printf( 0, KRunning, KData );
    iLog->Log( KData );
   
    // Test Cases
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------

    _LIT( KTestCase10, "TestCase10: Testing Trigger of CRoapEngBase" );
    iLog->Log( KTestCase10 );
    
    Roap::CRoapTrigger* trigger = Roap::CRoapTrigger::NewL();
    HBufC8* xmlData = NULL;
    
    Roap::TTriggerType type = Roap::ERegistrationTrigger;
    _LIT8(KRiIdDummy,"abcdefghijabcdefghij");
    _LIT8(KDomainIdDummy,"DomainId001");
    _LIT8(KRoapUrl,"http://www.localhost.localdomain:80");
    RPointerArray<HBufC8> listRo;
    RPointerArray<HBufC8> listCid;
    Roap::TRiContextStatus riStatus;
    Roap::TDomainOperation domainOperation;    
    
    trigger = Roap::CRoapTrigger::NewL( type,
                                        KRiIdDummy,
                                        KDomainIdDummy,
                                        KRoapUrl,
                                        listRo,
                                        listCid);
                                                                            
    TL( trigger );
    CleanupStack::PushL(trigger);
    
    xmlData = trigger->MessageAsXmlL();
    CleanupStack::PushL( xmlData );
    
    Roap::CRoapEng* roapEng = Roap::CRoapEng::NewL();
    CleanupStack::PushL( roapEng );
    

    roapEng->SetTriggerL( *xmlData,
                          NULL,
                          type,
                          riStatus,
                          domainOperation,
                          listCid );
    
    // Actual Test:    
    roapEng->Trigger();
    
    CleanupStack::PopAndDestroy(roapEng);
    CleanupStack::PopAndDestroy(xmlData);
    CleanupStack::PopAndDestroy(trigger);
    
    //Passed
    iLog->Log( KPassed );

    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Test Cases End 
    
    TestModuleIf().Printf( 0, KFinished, KData);

    //Test case passed
    _LIT( KDescription, "CRoapTest::TriggerL passed" );
    iLog->Log( KDescription );
    aResult.SetResult( KErrNone, KDescription );

    //Case was executed
    return KErrNone;         
    };




// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
?type ?classname::?member_function(
   ?arg_type arg,
   ?arg_type arg )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ?function_name implements...
// ?implementation_description.
// Returns: ?value_1: ?description
//          ?value_n: ?description
//                    ?description
// -----------------------------------------------------------------------------
//
/*
?type  ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg )  // ?description
    {

    ?code

    }
*/
//  [End of File] - do not remove
