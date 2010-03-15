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
* Description:  Declaration of the OMA DRM CAF Data Agent
*
*/



#ifndef OMA2AGENTDATA_H
#define OMA2AGENTDATA_H

//  INCLUDES
#include <f32file.h>
#include <caf/caf.h>
#include <caf/cafplatform.h>
#include <caf/agentinterface.h>
#include <DcfCommon.h>
#include <DRMRightsClient.h>

//this header can be removed when ASYNC_READ flag is removed
#include "DcfCache.h"

// FORWARD DECLARATIONS

class CDcfCache;

// CLASS DECLARATION

namespace ContentAccess
    {

    /**
     *  OMA DRM CAF Data Agent
     *  @lib omadrmagent.lib
     */
    class COma2AgentData : public CAgentData
        {
    public:
        // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static COma2AgentData* NewL( const TVirtualPathPtr& aVirtualPath,
            TContentShareMode aShareMode );
        static COma2AgentData* NewLC( const TVirtualPathPtr& aVirtualPath,
            TContentShareMode aShareMode );

        static COma2AgentData* NewL( RFile& aFile, const TDesC& aUniqueId );
        static COma2AgentData* NewLC( RFile& aFile, const TDesC& aUniqueId );

        /**
         * Destructor.
         */
        ~COma2AgentData();

    public:
        // From CAgentData
        TInt Read( TDes8& aDes );
        TInt Read( TDes8& aDes, TInt aLength );
        void Read( TDes8& aDes, TRequestStatus& aStatus );
        void Read( TDes8& aDes, TInt aLength, TRequestStatus& aStatus );
        void DataSizeL( TInt& aSize );
        TInt Seek( TSeek aMode, TInt& aPos );
        TInt SetProperty( TAgentProperty aProperty, TInt aValue );
        TInt EvaluateIntent( TIntent aIntent );
        TInt ExecuteIntent( TIntent aIntent );
        TInt GetAttribute( TInt aAttribute, TInt& aValue );
        TInt GetAttributeSet( RAttributeSet& aAttributeSet );
        TInt GetStringAttribute( TInt aAttribute, TDes& aValue );
        TInt GetStringAttributeSet( RStringAttributeSet& aStringAttributeSet );
#ifdef ASYNC_READ
        void ReadCancel( TRequestStatus& aStatus );
        TInt Read( TInt aPos, TDes8& aDes, TInt aLength,
            TRequestStatus& aStatus );
#endif

    private:

        /**
         * C++ default constructor.
         */
        COma2AgentData();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL( const TVirtualPathPtr& aVirtualPath,
            TContentShareMode aShareMode );
        void ConstructL( RFile& aFile, const TDesC& aUniqueId );
        void InitializeL();

        // Prohibit copy constructor if not deriving from CBase.
        // COma2AgentData( const COma2AgentData& );
        // Prohibit assigment operator if not deriving from CBase.
        // COma2AgentData& operator=( const COma2AgentData& );

    protected:
        // Data
        CDcfCommon* iDcf;
        RFs iFs;
        RFile iFile;
        TInt iDataPosition;
        TInt iRightsStatus;
        RDRMRightsClient iRdb;
        TVirtualPathPtr iVirtualPath;
        CDcfCache* iCache;
        TBool iGroupKeyUsed;
        TInt iLastFileSize;
        HBufC* iUniqueId;

        };

    }

#endif      // OMA2AGENTDATA_H
// End of File
