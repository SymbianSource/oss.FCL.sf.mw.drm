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
* Description:  
*
*/


#ifndef DRMPLAYSERVERTEST_H
#define DRMPLAYSERVERTEST_H

#include <e32base.h>
#include <DrmAudioSamplePlayer.h>

class CDRMPlayServerTest : public CBase, public MDrmAudioPlayerCallback
    {
    
    public:
        
        static CDRMPlayServerTest* NewLC();

        ~CDRMPlayServerTest();

        TInt ExecutePlayServerTest();

        void MdapcInitComplete( TInt aError, 
                                const TTimeIntervalMicroSeconds& aDuration );

        void MdapcPlayComplete( TInt aError );

   private:
        
        CDRMPlayServerTest();
        
        void ConstructL();
   
   private:  // Data
    
        CDrmPlayerUtility* iDrmPlayerUtility;
        CActiveSchedulerWait* iWait;
        TInt iError;

    };

#endif // DRMPLAYSERVERTEST_H

// End of File
