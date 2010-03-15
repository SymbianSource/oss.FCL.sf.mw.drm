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
* Description:
*
*/



#ifndef OMA2AGENTMANAGER_H
#define OMA2AGENTMANAGER_H

//  INCLUDES
#include <caf/caf.h>
#include <caf/cafplatform.h>
#include <caf/agentinterface.h>
#include <DRMNotifier.h>

class CFileMan;

namespace ContentAccess
{

// CLASS DECLARATION

class COma2AgentManager : public CAgentManager, public MDRMEventObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static COma2AgentManager* NewL();
        static COma2AgentManager* NewLC();

        /**
        * Destructor.
        */
        ~COma2AgentManager();

    public: // New functions

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From ?base_class ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */

        // From CAgentManagerBase
        TInt DeleteFile(const TDesC &aFileName);
        TInt CopyFile(const TDesC& aSource, const TDesC& aDestination);
        TInt CopyFile(RFile& aSource, const TDesC& aDestination);
        TInt RenameFile(const TDesC& aSource, const TDesC& aDestination);
        TInt MkDir(const TDesC& aPath);
        TInt MkDirAll(const TDesC& aPath);
        TInt RmDir(const TDesC& aPath);
        TInt GetDir(const TDesC& aName,TUint anEntryAttMask,TUint anEntrySortKey, CDir*& anEntryList) const;
        TInt GetDir(const TDesC& aName,TUint anEntryAttMask,TUint anEntrySortKey, CDir*& anEntryList,CDir*& aDirList) const;
        TInt GetDir(const TDesC& aName,const TUidType& anEntryUid,TUint anEntrySortKey, CDir*& aFileList) const;
        TInt GetAttribute(TInt aAttribute, TInt& aValue, const TVirtualPathPtr& aVirtualPath);
        TInt GetAttributeSet(RAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath);
        TInt GetStringAttributeSet(RStringAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath);
        TInt GetStringAttribute(TInt aAttribute, TDes& aValue, const TVirtualPathPtr& aVirtualPath);
        void NotifyStatusChange(const TDesC& aURI, TEventMask aMask, TRequestStatus& aStatus);
        TInt CancelNotifyStatusChange(const TDesC& aURI, TRequestStatus& aStatus);
        TInt SetProperty(TAgentProperty aProperty, TInt aValue);
        void DisplayInfoL(TDisplayInfo aInfo, const TVirtualPathPtr& aVirtualPath);

        // From CAgentManager
        TBool IsRecognizedL(const TDesC& aURI, TContentShareMode aShareMode) const;
        TBool IsRecognizedL(RFile& aFile) const;
        TBool RecognizeFileL(const TDesC& aFileName, const TDesC8& aBuffer, TDes8& aFileMimeType, TDes8& aContentMimeType) const;
        TInt AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer);
        void AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer, TRequestStatus& aStatus);
        void PrepareHTTPRequestHeaders(RStringPool& aStringPool, RHTTPHeaders& aRequestHeaders) const;
        void DisplayManagementInfoL();

public: // From MDRMEventObserver
        virtual void HandleEventL(
            MDRMEvent* aEvent);

    private:
        RFs iFs;

        // File manager
        CFileMan* iFileManager;

        // Oma based extensions
        HBufC8* iOmaBasedMimeType;

    private:

        /**
        * C++ default constructor.
        */
        COma2AgentManager();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        void FetchOmaBasedInfoL();
        void PrepareHTTPRequestHeadersL(RStringPool& aStringPool, RHTTPHeaders& aRequestHeaders) const;


    protected:  // Data
        // Request status for change notifications
        TRequestStatus* iStatus;

        // Notifier for change notifications
        CDRMNotifier* iNotifier;

        // Watched events
        TEventMask iWatchedEvents;

        // Watched content ID
        HBufC8* iWatchedId;
    };

}

#endif      // OMA2AGENTMANAGER_H

// End of File
