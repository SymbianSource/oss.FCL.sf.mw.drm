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
#include <caf/caf.h>
#include <caf/cafplatform.h>
#include "Oma2AgentFactory.h"

using namespace ContentAccess;

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
// COma2AgentFactory::COma2AgentFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
COma2AgentFactory::COma2AgentFactory()
    {
    }

// -----------------------------------------------------------------------------
// COma2AgentFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void COma2AgentFactory::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// COma2AgentFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
COma2AgentFactory* COma2AgentFactory::NewL()
    {
    COma2AgentFactory* self = new( ELeave ) COma2AgentFactory;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
COma2AgentFactory::~COma2AgentFactory()
    {
    }


// -----------------------------------------------------------------------------
// COma2AgentFactory::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CAgentContent* COma2AgentFactory::CreateContentBrowserL(const TDesC& aURI, TContentShareMode aShareMode)
    {
    return COma2AgentContent::NewL(aURI, aShareMode);
    }

CAgentContent* COma2AgentFactory::CreateContentBrowserL(RFile& aFile)
    {
    return COma2AgentContent::NewL(aFile);
    }

CAgentData* COma2AgentFactory::CreateDataConsumerL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)
    {
    return COma2AgentData::NewL(aVirtualPath, aShareMode);
    }

CAgentData* COma2AgentFactory::CreateDataConsumerL(RFile& aFile, const TDesC& aUniqueId)
    {
    return COma2AgentData::NewL(aFile, aUniqueId);
    }

CAgentManager*  COma2AgentFactory::CreateManagerL()
    {
    return COma2AgentManager::NewL();
    }

CAgentImportFile* COma2AgentFactory::CreateImporterL(const TDesC8& aMimeType, const CMetaDataArray& aMetaDataArray, const TDesC& aOutputDirectory, const TDesC& aSuggestedFileName)
    {
    return COma2AgentImportFile::NewL(aMimeType, aMetaDataArray, aOutputDirectory, aSuggestedFileName);
    }

CAgentImportFile* COma2AgentFactory::CreateImporterL(const TDesC8& aMimeType, const CMetaDataArray& aMetaDataArray)
    {
    return COma2AgentImportFile::NewL(aMimeType, aMetaDataArray);
    }

CAgentRightsManager* COma2AgentFactory::CreateRightsManagerL()
    {
    return COma2AgentRightsManager::NewL();
    }

