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
* Description:  Declaration of the WMDRM CAF Content Agent
*
*/


#ifndef WMDRMAGENTCONTENT_H
#define WMDRMAGENTCONTENT_H

//  INCLUDES
#include <f32file.h>
#include <caf.h>
#include <agentinterface.h>
#include "asf.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
namespace ContentAccess
    {

/**
*  WMDRM CAF Content Agent implementation
*
*  @lib wmdrmagent.lib
*  @since Series 60 3.1
*/
    class CWmDrmAgentContent : public CAgentContent
        {
    public:
        // Two phase constructor used when the file is opened
        // using a file name
        static CWmDrmAgentContent* NewL(const TDesC& aURI, TContentShareMode aShareMode);
        static CWmDrmAgentContent* NewLC(const TDesC& aUri,TContentShareMode aShareMode);

        // Two phase constructor used when the file is opened with
        // a file handle
        static CWmDrmAgentContent* NewL(RFile& aFile);
        static CWmDrmAgentContent* NewLC(RFile& aFile);


        virtual ~CWmDrmAgentContent();

    public:
        // From CAgentContent
        virtual TInt OpenContainer(const TDesC& aUniqueId);
        virtual TInt CloseContainer();
        virtual void GetEmbeddedObjectsL(RStreamablePtrArray<CEmbeddedObject>& aArray);
        virtual void GetEmbeddedObjectsL(RStreamablePtrArray<CEmbeddedObject>& aArray, TEmbeddedType aType);
        virtual TInt Search(RStreamablePtrArray<CEmbeddedObject>& aArray, const TDesC8& aMimeType, TBool aRecursive);
        virtual TInt GetAttribute(TInt aAttribute, TInt& aValue, const TDesC& aUniqueId);
        virtual TInt GetAttributeSet(RAttributeSet& aAttributeSet, const TDesC& aUniqueId);
        virtual TInt GetStringAttribute(TInt aAttribute, TDes& aValue, const TDesC& aUniqueId);
        virtual TInt GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, const TDesC& aUniqueId);
        virtual TInt AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer);
        virtual void AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer, TRequestStatus& aStatus);
        virtual void NotifyStatusChange(TEventMask aMask, TRequestStatus& aStatus, const TDesC& aUniqueId);
        virtual TInt CancelNotifyStatusChange(TRequestStatus& aStatus, const TDesC& aUniqueId);
        virtual void RequestRights(TRequestStatus& aStatus, const TDesC& aUniqueId);
        virtual TInt CancelRequestRights(TRequestStatus& aStatus, const TDesC& aUniqueId);
        virtual void DisplayInfoL(TDisplayInfo aInfo, const TDesC& aUniqueId);
        virtual TInt SetProperty(TAgentProperty aProperty, TInt aValue);

    private:
        CWmDrmAgentContent();
        void ConstructL(const TDesC& aURI, TContentShareMode aShareMode);
        void ConstructL(RFile& aFile);

    protected:  // Data
        //TContentShareMode iShareMode;
        RFs iFs;
        RFile iFile;
        HBufC* iURI;
        CAsf* iAsf;
        };
} // namespace ContentAccess
#endif      // WMDRMAGENTCONTENT_H

// End of File
