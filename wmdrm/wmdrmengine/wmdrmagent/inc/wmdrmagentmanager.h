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



#ifndef WMDRMAGENTMANAGER_H
#define WMDRMAGENTMANAGER_H

//  INCLUDES
#include <caf.h>
#include <agentinterface.h>
#include <caf/attributeset.h>
#include <caf/stringattributeset.h>

namespace ContentAccess
    {

// CLASS DECLARATION

    class CWmDrmAgentManager : public CAgentManager
        {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CWmDrmAgentManager* NewL();
        static CWmDrmAgentManager* NewLC();

        /**
        * Destructor.
        */
        ~CWmDrmAgentManager();

    public: // New functions


    public: // Functions from base classes

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
        void DisplayManagementInfoL();
        void PrepareHTTPRequestHeaders(RStringPool& aStringPool, RHTTPHeaders& aRequestHeaders) const;
        TInt RenameDir(const TDesC& aOldName, const TDesC& aNewName);

    protected:

        static TBool IsProtectedL(const TDesC& aFileName);
        static TBool IsProtectedL(RFile& aFile);

    private:
        /**
        * C++ default constructor.
        */
        CWmDrmAgentManager();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Sets up the file manager if it's needed
        * If unsuccessful return an error code
        */
        TInt SetFileMan();

        /**
        * Combined ASF file object creator and attribute getter methods.
        * Note: these are added for minimising TRAP harnesses
        * in nonleaving attribute getter member functions.
        */
        TInt CWmDrmAgentManager::GetAttributeCreateFileL(TInt aAttribute, TInt& aValue, const TVirtualPathPtr& aVirtualPath);
        TInt CWmDrmAgentManager::GetAttributeSetCreateFileL(RAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath);
        TInt CWmDrmAgentManager::GetStringAttributeCreateFileL(TInt aAttribute, TDes& aValue, const TVirtualPathPtr& aVirtualPath);
        TInt CWmDrmAgentManager::GetStringAttributeSetCreateFileL(RStringAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath);



    protected:  // Data

    private:
        RFs iFs;
        CFileMan *iFileMan;
        };
} // namespace ContentAccess

#endif // WMDRMAGENTMANAGER_H

// End of File
