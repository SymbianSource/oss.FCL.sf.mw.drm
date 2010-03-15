/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of the Oma2AgentAttributes class
*
*/



#ifndef __Oma2AGENTATTRIBUTES_H__
#define __Oma2AGENTATTRIBUTES_H__

#include <caf/caftypes.h>
#include <caf/virtualpathptr.h>
#include <caf/attributeset.h>
#include <caf/stringattributeset.h>
#include "Oma2Agent.h"
#include "oma2dcf.h"

using namespace ContentAccess;

class RDRMRightsClient;

/**
All attribute values are common between the CAgentData, CAgentContent and
CAgentManager. The OMA DCF attributes are mostly fixed anyway so this class
implements helper functions for all of the attribute functions
*/

class TOma2AgentAttributes
    {
public:
    static TInt GetAttribute(
        CDcfCommon& aDcfFile,
        TInt aAttribute,
        const TVirtualPathPtr& aVirtualPath,
        RDRMRightsClient* aRightsClient = NULL);

    static TInt GetAttributeSet(
        CDcfCommon& aDcfFile,
        RAttributeSet& aAttributeSet,
        const TVirtualPathPtr& aVirtualPath,
        RDRMRightsClient* aRightsClient = NULL);

    static TInt GetStringAttribute(
        CDcfCommon& aDcfFile,
        TInt aAttribute,
        TDes& aValue,
        const TVirtualPathPtr& aVirtualPath,
        RDRMRightsClient* aRightsClient = NULL);

    static TInt GetStringAttributeSet(
        CDcfCommon& aDcfFile,
        RStringAttributeSet& aStringAttributeSet,
        const TVirtualPathPtr& aVirtualPath,
        RDRMRightsClient* aRightsClient = NULL);
    };

#endif // __Oma2AGENTATTRIBUTES_H__
