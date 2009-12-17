/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <caf.h>
#include "wmdrmagentfactory.h"
#include "wmdrmagentimportfile.h"
#include "wmdrmagentrightsmanager.h"
#include "logfn.h"

using namespace ContentAccess;

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWmDrmAgentFactory::CWmDrmAgentFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWmDrmAgentFactory::CWmDrmAgentFactory()
    {
    LOGFN( "CWmDrmAgentFactory::CWmDrmAgentFactory" );
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWmDrmAgentFactory::ConstructL()
    {
    LOGFN( "CWmDrmAgentFactory::ConstructL" );
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWmDrmAgentFactory* CWmDrmAgentFactory::NewL()
    {
    LOGFN( "CWmDrmAgentFactory::NewL" );
    CWmDrmAgentFactory* self = new( ELeave ) CWmDrmAgentFactory;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentFactory::~CWmDrmAgentFactory
// Destructor
// -----------------------------------------------------------------------------
//
CWmDrmAgentFactory::~CWmDrmAgentFactory()
    {
    LOGFN( "CWmDrmAgentFactory::~CWmDrmAgentFactory" );
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentFactory::CreateContentBrowserL
// -----------------------------------------------------------------------------
//
CAgentContent* CWmDrmAgentFactory::CreateContentBrowserL(const TDesC& aURI, TContentShareMode aShareMode)
    {
    LOGFN( "CWmDrmAgentFactory::CreateContentBrowserL" );
    return CWmDrmAgentContent::NewL(aURI, aShareMode);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentFactory::CreateContentBrowserL
// -----------------------------------------------------------------------------
//
CAgentContent* CWmDrmAgentFactory::CreateContentBrowserL(RFile& aFile)
    {
    LOGFN( "CWmDrmAgentFactory::CreateContentBrowserL (2)" );
    return CWmDrmAgentContent::NewL(aFile);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentFactory::CreateDataConsumerL
// -----------------------------------------------------------------------------
//
CAgentData* CWmDrmAgentFactory::CreateDataConsumerL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)
    {
    LOGFN( "CWmDrmAgentFactory::CreateDataConsumerL" );
    return CWmDrmAgentData::NewL(aVirtualPath, aShareMode);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentFactory::CreateDataConsumerL
// -----------------------------------------------------------------------------
//
CAgentData* CWmDrmAgentFactory::CreateDataConsumerL(RFile& aFile, const TDesC& aUniqueId)
    {
    LOGFN( "CWmDrmAgentFactory::CreateDataConsumerL (2)" );
    return CWmDrmAgentData::NewL(aFile, aUniqueId);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentFactory::CreateManagerL
// -----------------------------------------------------------------------------
//
CAgentManager*  CWmDrmAgentFactory::CreateManagerL()
    {
    LOGFN( "CWmDrmAgentFactory::CreateManagerL" );
    return CWmDrmAgentManager::NewL();
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentFactory::CreateImporterL
// -----------------------------------------------------------------------------
//
CAgentImportFile* CWmDrmAgentFactory::CreateImporterL(const TDesC8& aMimeType, const CMetaDataArray& aMetaDataArray, const TDesC& aOutputDirectory, const TDesC& aSuggestedFileName)
    {
    LOGFN( "CWmDrmAgentFactory::CreateImporterL" );
    return CWmDrmAgentImportFile::NewL(aMimeType, aMetaDataArray, aOutputDirectory, aSuggestedFileName);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentFactory::CreateImporterL
// -----------------------------------------------------------------------------
//
CAgentImportFile* CWmDrmAgentFactory::CreateImporterL(const TDesC8& aMimeType, const CMetaDataArray& aMetaDataArray)
    {
    LOGFN( "CWmDrmAgentFactory::CreateImporterL (2)" );
    return CWmDrmAgentImportFile::NewL(aMimeType, aMetaDataArray);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentFactory::CreateImporterL
// -----------------------------------------------------------------------------
//
CAgentRightsManager* CWmDrmAgentFactory::CreateRightsManagerL()
    {
    LOGFN( "CWmDrmAgentFactory::CreateRightsManagerL" );
	return CWmDrmAgentRightsManager::NewL();
    }


