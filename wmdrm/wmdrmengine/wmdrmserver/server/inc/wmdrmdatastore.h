/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  WMDRM data store 
*
*/


#ifndef CWMDRMDATASTORE_H
#define CWMDRMDATASTORE_H

#include <e32base.h>
#include "wmdrmserver.h"

class CWmDrmDataStore : public CBase
    {
    public:
    
        static CWmDrmDataStore* NewL( CWmDrmServer* aServer );
        ~CWmDrmDataStore();
        
        TWmDrmStoreState DataStoreStateL();
        
        void UpdateDummyDbFileL( TInt aSize, TBool aConfiguredDrive );
        TInt DummyDBSizeL( TBool aConfiguredDrive );
        
    private:

        CWmDrmDataStore( CWmDrmServer* aServer );
        void ConstructL();
        
        void ReadInitialFreeSpaceL( const TDesC& aFileName, TBool& aConfiguredDrive );
        void WriteInitialFreeSpaceL( const TDesC& aFileName, TBool& aConfiguredDrive );
        void InitializeDummyDbFileL( const TDesC& aFileName, RFile& aDummyDb, 
        		TBool& aConfiguredDrive );
        TInt DataStoreSizeL( TBool aConfiguredDrive );
        void PrepareInfoFilesL( TBool aConfiguredDrive, TChar aDriveLetter, 
			RFile& aDummyDb );
        
    private:    
    
        // Not owned
        CWmDrmServer* iServer;
        
        // The file and variables ending with '2' are used only
        // in the case when the WM DRM rights are stored to a
        // configured drive other than the default system drive.  
        RFile iDummyDb;
        RFile iDummyDb2;
        TInt64 iInitialFreeSpace; 
        TInt64 iInitialFreeSpace2;
        TInt64 iMinFreeSpace;
        TInt64 iMinFreeSpace2;
        TInt iMaxSpaceRatio;
        TInt iMaxSpaceRatio2;
        TInt iDummyDbInitialSize;
        TInt iDummyDbInitialSize2;
        
        // Whether WM DRM rights are configured to be stored to an internal
        // mass drive or not
        TBool iWmDrmRightsConfigFound;    
    };

#endif // CWMDRMDATASTORE_H