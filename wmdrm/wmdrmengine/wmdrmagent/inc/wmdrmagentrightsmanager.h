/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of the WMDRM Rights Manager CAF Agent
*
*/


#ifndef WMDRMAGENTRIGHTSMANAGER_H
#define WMDRMAGENTRIGHTSMANAGER_H

//  INCLUDES
#include <caf.h>
#include <agentinterface.h>

namespace ContentAccess
{

// CLASS DECLARATION

/**
*  Rights manager implementation for WMDRM
*
*  @lib wmdrmagent.dll
*  @since S60 3.2
*/
class CWmDrmAgentRightsManager : public CAgentRightsManager
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CWmDrmAgentRightsManager* NewL();
        static CWmDrmAgentRightsManager* NewLC();

        /**
        * Destructor.
        */
        ~CWmDrmAgentRightsManager();

    public: // Functions from base classes

        // From CAgentRightsManager
        void ListAllRightsL(RStreamablePtrArray<CRightsInfo>& aArray) const;
        void ListRightsL(RStreamablePtrArray<CRightsInfo>& aArray, const TDesC& aUri) const;
        void ListRightsL(RStreamablePtrArray<CRightsInfo>& aArray, TVirtualPathPtr& aVirtualPath) const;
        void ListContentL(RStreamablePtrArray<CVirtualPath>& aArray, CRightsInfo& aRightsInfo) const;
        MAgentRightsBase* GetRightsDataL(const CRightsInfo& aRightsInfo) const;
        TInt DeleteRightsObject(const CRightsInfo& aRightsInfo);
        TInt DeleteAllRightsObjects(const TVirtualPathPtr& aVirtualPath);
        TInt SetProperty(TAgentProperty aProperty, TInt aValue);

    private:

        /**
        * C++ default constructor.
        */
        CWmDrmAgentRightsManager();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // New functions
        // Data

    };

}

#endif      // WMDRMAGENTRIGHTSMANAGER_H

// End of File
