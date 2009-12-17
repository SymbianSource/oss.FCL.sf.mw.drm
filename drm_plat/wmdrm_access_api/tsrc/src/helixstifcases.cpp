/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file contains hardcoded module implementation.
*
*/


// [INCLUDE FILES] - do not remove
#include <e32math.h>
#include <e32base.h>
#include <wmdrmaccess.h>
#include "helixstif.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Chelixstif::Case
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
const TCaseInfo Chelixstif::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    helixstif.cpp file and to helixstif.h 
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
        // Chelixstif::PrintTest. Otherwise the compiler
        // gives errors.
        
        ENTRY( "ConstructTtest", Chelixstif::ConstructTestL ),
        ENTRY( "InitializeTest", Chelixstif::InitializeTestL ),
        ENTRY( "DecryptTest", Chelixstif::DecryptTestL ),
        ENTRY( "CloseTest", Chelixstif::CloseTestL )
        
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
// Chelixstif::ConstructTest
// Constructor test, constructs CWmDrmAccess object
// -----------------------------------------------------------------------------
//
TInt Chelixstif::ConstructTestL( TTestResult& aResult )
    {

    CWmDrmAccess* helixnewltest = CWmDrmAccess::NewL();
    
    _LIT( KState, "TestNewL" );
    
    iLog->Log( KState );
    
    // Sets test case result and description
    
    _LIT( KDescription, "ConstructTest passed" );
    
    // Check the condition
    
    TL( helixnewltest != NULL );
    
    aResult.SetResult( KErrNone, KDescription );

    delete helixnewltest;
    
    // Case was executed
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// Chelixstif::InitializeTest
// Initialize test, check for the Initialize function
// -----------------------------------------------------------------------------
//
TInt Chelixstif::InitializeTestL( TTestResult& aResult )
    {
    _LIT8(KEmpty,"abc");
    
    CWmDrmAccess* helixinitializetest = CWmDrmAccess::NewL();
    
    // Check the condition
    
    TL(KErrNone != helixinitializetest->Initialize( KEmpty() )) ;
    
    TL( helixinitializetest != NULL );
    
    _LIT( KState, "TestInitialize" );
    
    iLog->Log( KState );
    
    // Sets test case result and description
    
    _LIT( KDescription, "InitializeTest passed" );
    
    aResult.SetResult( KErrNone, KDescription );
        
    delete helixinitializetest;
    
    // Case was executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// Chelixstif::DecryptTest
// Decrypt test, check for the Decrypt function
// -----------------------------------------------------------------------------
//    
    
TInt Chelixstif::DecryptTestL( TTestResult& aResult )
    {
    
    CWmDrmAccess* helixdecrypttest = CWmDrmAccess::NewLC();
        
    TPtr8 buffer(NULL,0);
        
    HBufC8* buf = HBufC8::NewMaxLC(5);
        
    buffer.Set( const_cast<TUint8*>(buf->Ptr()),0,5);
    buffer.Append(_L8("abcde"));
    
    TInt retval;
    
    retval = helixdecrypttest->Decrypt( buffer );
    
    TL( KErrNone != retval ) ;
        
    _LIT( KState, "TestDecrypt" );
    
    iLog->Log( KState );
    
    // Sets test case result and description
    
    _LIT( KDescription, "DecryptTest passed" );
    
    // Check for the condition
    
    TL( helixdecrypttest != NULL );
    TL( retval == KErrNotReady );
    
    aResult.SetResult( KErrNone, KDescription );

    CleanupStack::PopAndDestroy(buf);
    CleanupStack::PopAndDestroy(helixdecrypttest);
       
    // Case was executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Chelixstif::CloseTest
// Close test, check for the Close function
// -----------------------------------------------------------------------------
//    
TInt Chelixstif::CloseTestL( TTestResult& aResult )
    {
    CWmDrmAccess* helixclosetest = CWmDrmAccess::NewLC();
        
    _LIT( KState, "TestClose" );
    
    iLog->Log( KState );
    
    int retval;

    TL( helixclosetest != NULL );
    
    retval = helixclosetest->Close();
    
    // Sets test case result and description
    
    _LIT( KDescription, "CloseTest passed" );
    
    // Check for the condition
    
    TL( retval == KErrNone );
    
    aResult.SetResult( KErrNone, KDescription );

    CleanupStack::PopAndDestroy(helixclosetest);
    
    // Case was executed
    return KErrNone;

    }

//  [End of File] - do not remove
