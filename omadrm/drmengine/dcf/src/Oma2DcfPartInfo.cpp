/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include <e32base.h>
#include <f32file.h>
#include <utf.h>
#include <caf/caf.h>
#include <caf/cafplatform.h>
#include "Oma2Agent.h"
#include "oma2dcf.h"
#include "Oma2DcfPartInfo.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES
//extern ?external_function( ?arg_type,?arg_type);

// CONSTANTS

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type);

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
// COma2Dcf::COma2Dcf
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
COma2DcfPartInfo::COma2DcfPartInfo():
    iContentId(NULL),
    iMimeType(NULL),
    iType(0),
    iSize(0),
    iOffset(0)
    {
    }

// -----------------------------------------------------------------------------
// COma2Dcf::NewL
//
// -----------------------------------------------------------------------------
//
COma2DcfPartInfo* COma2DcfPartInfo::NewL()
    {
    COma2DcfPartInfo* self = new(ELeave) COma2DcfPartInfo;
    return self;
    }


// Destructor
COma2DcfPartInfo::~COma2DcfPartInfo()
    {
    delete iContentId;
    delete iMimeType;
    }


//  End of File
