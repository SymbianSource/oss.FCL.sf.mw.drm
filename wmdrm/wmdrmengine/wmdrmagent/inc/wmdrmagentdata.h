/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef WMDRMAGENTDATA_H
#define WMDRMAGENTDATA_H

//  INCLUDES
#include <f32file.h>
#include <caf.h>
#include <agentinterface.h>
#include <caf/attributeset.h>
#include <caf/stringattributeset.h>

// FORWARD DECLARATIONS

class CAsf;

// CLASS DECLARATION
namespace ContentAccess
    {
    class TVirtualPathPtr;

    /**
     * WmDrm agent implementation of the CAgentData class used to read
     *
     *  @lib ECOM plugin
     *  @since S60 3.2
     */
    class CWmDrmAgentData : public CAgentData
        {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CWmDrmAgentData* NewL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode);
        static CWmDrmAgentData* NewLC(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode);
        static CWmDrmAgentData* NewL(RFile& aFile, const TDesC& aUniqueId);
        static CWmDrmAgentData* NewLC(RFile& aFile, const TDesC& aUniqueId);

        /**
        * Destructor.
        */
        ~CWmDrmAgentData();

    public: // From CAgentData
        TInt Read(TDes8& aDes);
        TInt Read(TDes8& aDes,TInt aLength);
        void Read(TDes8& aDes, TRequestStatus& aStatus);
        void Read(TDes8& aDes, TInt aLength, TRequestStatus& aStatus);
        void DataSizeL(TInt& aSize);
        TInt Seek(TSeek aMode,TInt& aPos);
        TInt SetProperty(TAgentProperty aProperty, TInt aValue);
        TInt EvaluateIntent(TIntent aIntent);
        TInt ExecuteIntent(TIntent aIntent);
        TInt GetAttribute(TInt aAttribute, TInt& aValue);
        TInt GetAttributeSet(RAttributeSet& aAttributeSet);
        TInt GetStringAttribute(TInt aAttribute, TDes& aValue);
        TInt GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet);
        TInt Read(TInt aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus);

    private:

        /**
        * C++ default constructor.
        */
        CWmDrmAgentData();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode);
        void ConstructL(RFile& aFile, const TDesC& aUniqueId);

    protected:  // Data

        /** Handle to the filesystem */
        RFs iFs;

        /** RFile object */
        RFile iFile;
        TVirtualPathPtr iVirtualPath;

        /** The ASF file */
        CAsf* iAsf;
        };

} // namespace ContentAccess

#endif // WMDRMAGENTDATA_H
