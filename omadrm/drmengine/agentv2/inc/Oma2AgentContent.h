/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of the OMA DRM CAF Content Agent
*
*/


#ifndef OMA2AGENTCONTENT_H
#define OMA2AGENTCONTENT_H

//  INCLUDES
#include <f32file.h>
#include <caf/caf.h>
#include <caf/cafplatform.h>
#include <caf/agentinterface.h>
#include <DcfCommon.h>
#include <DRMNotifier.h>
#include <DRMRightsClient.h>

// FORWARD DECLARATIONS
class CAESDecryptor;

// CLASS DECLARATION
namespace ContentAccess
{

/**
*  OMA DRM CAF Content Agent implementation
*
*  @lib OmaDrmAgent.lib
*  @since Series 60 3.0
*/
class COma2AgentContent : public CAgentContent, public MDRMEventObserver
{
public:
    static COma2AgentContent* NewL(
        const TDesC& aUri,
        TContentShareMode aShareMode);

    static COma2AgentContent* NewLC(
        const TDesC& aUri,
        TContentShareMode aShareMode);

    static COma2AgentContent* NewL(
        RFile& aFile);

    static COma2AgentContent* NewLC(
        RFile& aFile);

    void EmbedDomainRoL();

    virtual ~COma2AgentContent();

public:
    // From CAgentContent
    virtual TInt OpenContainer(
        const TDesC& aUniqueId);

    virtual TInt CloseContainer();

    virtual void GetEmbeddedObjectsL(
        RStreamablePtrArray<CEmbeddedObject>& aArray);

    virtual void GetEmbeddedObjectsL(
        RStreamablePtrArray<CEmbeddedObject>& aArray,
        TEmbeddedType aType);

    virtual TInt Search(
        RStreamablePtrArray<CEmbeddedObject>& aArray,
        const TDesC8& aMimeType,
        TBool aRecursive);

    virtual TInt GetAttribute(
        TInt aAttribute,
        TInt& aValue,
        const TDesC& aUniqueId);

    virtual TInt GetAttributeSet(
        RAttributeSet& aAttributeSet,
        const TDesC& aUniqueId);

    virtual TInt GetStringAttribute(
        TInt aAttribute,
        TDes& aValue,
        const TDesC& aUniqueId);

    virtual TInt GetStringAttributeSet(
        RStringAttributeSet& aStringAttributeSet,
        const TDesC& aUniqueId);

    virtual TInt AgentSpecificCommand(
        TInt aCommand,
        const TDesC8& aInputBuffer,
        TDes8& aOutputBuffer);

    virtual void AgentSpecificCommand(
        TInt aCommand,
        const TDesC8& aInputBuffer,
        TDes8& aOutputBuffer,
        TRequestStatus& aStatus);

    virtual void NotifyStatusChange(
        TEventMask aMask,
        TRequestStatus& aStatus,
        const TDesC& aUniqueId);

    virtual TInt CancelNotifyStatusChange(
        TRequestStatus& aStatus,
        const TDesC& aUniqueId);

    virtual void RequestRights(
        TRequestStatus& aStatus,
        const TDesC& aUniqueId);

    virtual TInt CancelRequestRights(
        TRequestStatus& aStatus,
        const TDesC& aUniqueId);

    virtual void DisplayInfoL(
        TDisplayInfo aInfo,
        const TDesC& aUniqueId);

    virtual TInt SetProperty(
        TAgentProperty aProperty,
        TInt aValue);

public: // From MDRMEventObserver
    virtual void HandleEventL(
        MDRMEvent* aEvent);

private:
    COma2AgentContent();

    void NotifyStatusChangeL(
        TEventMask aMask,
        TRequestStatus& aStatus,
        const TDesC& aUniqueId);


    void ConstructL(
        const TDesC& aUri,
        TContentShareMode aShareMode);

    void ConstructL(
        RFile& aFile);

    protected:  // Data
        CDcfCommon* iDcf;
        RFs iFs;
        RFile iFile;
        TInt iFilePosition;
        TInt iDataPosition;
        HBufC* iUri;

        // Content ID of the currently open container,
        // NULL for the outermost file
        HBufC* iCurrentContainer;

        // Request status for change notifications
        TRequestStatus* iStatus;

        // Notifier for change notifications
        CDRMNotifier* iNotifier;

        // Watched events
        TEventMask iWatchedEvents;

        // Watched content ID
        HBufC8* iWatchedId;

        // Rights client
        RDRMRightsClient iRdb;
    };

}

#endif      // OMA2AGENTCONTENT_H

// End of File
