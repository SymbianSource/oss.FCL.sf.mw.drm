/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DrmUtilityDownloadManager declaration
*
*/


#ifndef CDRMUTILITYDOWNLOADMANAGER_H
#define CDRMUTILITYDOWNLOADMANAGER_H

#include <e32base.h>
#include <ConeResLoader.h>

class CDrmUtilityDmgrWrapper;

/**
 *  This class encapsulates Download manager functionality.
 */
NONSHARABLE_CLASS(CDrmUtilityDownloadManager) : public CBase
    {
public:
    
    static CDrmUtilityDownloadManager* NewL();
    virtual ~CDrmUtilityDownloadManager();

    /**
     * Download a ROAP trigger from URL and handle it
     *
     * @param aUrl URL to be downloaded
     */
    void DownloadAndHandleRoapTriggerL( const HBufC8* aUrl, CCoeEnv& aCoeEnv );

    void DownloadAndHandleRoapTriggerL( const HBufC8* aUrl );
    
    HBufC8* GetErrorUrlL();


protected:
            
private:

    CDrmUtilityDownloadManager();
    void ConstructL();

private:

    /**
	 * Download manager handler
	 */
    RLibrary iDMgrDll;
    CDrmUtilityDmgrWrapper* iDMgrHandler;
    };

#endif      // CDRMUTILITYDOWNLOADMANAGER_H   
