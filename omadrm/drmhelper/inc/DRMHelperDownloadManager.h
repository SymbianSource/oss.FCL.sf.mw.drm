/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DRMHelperDownloadManager declaration
*
*/


#ifndef CDRMHELPERDOWNLOADMANAGER_H
#define CDRMHELPERDOWNLOADMANAGER_H

#include <e32base.h>
#include <ConeResLoader.h>

class CDrmUtilityDmgrWrapper;

/**
 *  This class encapsulates Download manager functionality.
 *
 *  @lib DrmHelper.lib
 *  @since S60 3.0
 */
NONSHARABLE_CLASS(CDrmHelperDownloadManager) : public CBase
    {
public:

    static CDrmHelperDownloadManager* NewL();
    virtual ~CDrmHelperDownloadManager();

    /**
     * Download a ROAP trigger from URL and handle it
     *
     * @since S60 3.0
     * @param aUrl URL to be downloaded
     */
    void DownloadAndHandleRoapTriggerL( const HBufC8* aUrl, CCoeEnv& aCoeEnv );

    void DownloadAndHandleRoapTriggerL( const HBufC8* aUrl );

    HBufC8* GetErrorUrlL();


protected:

private:

    CDrmHelperDownloadManager();
    void ConstructL();

private:

    /**
     * Download manager handler
     */
    RLibrary iDMgrDll;
    CDrmUtilityDmgrWrapper* iDMgrHandler;
    };

#endif      // CDRMHELPERDOWNLOADMANAGER_H
