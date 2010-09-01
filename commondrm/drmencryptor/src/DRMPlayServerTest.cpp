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


#include <DrmAudioSamplePlayer.h>

#include "DRMPlayServerTest.h"

#ifdef __WINS__
_LIT( KTestFile, "c:\\data\\drm\\test.wma" );
#else
_LIT( KTestFile, "e:\\drm\\test.wma" );
#endif

// ================= MEMBER FUNCTIONS ==========================================

CDRMPlayServerTest::CDRMPlayServerTest()
    {
    }


void CDRMPlayServerTest::ConstructL()
    {
    iWait = new (ELeave) CActiveSchedulerWait();
    iDrmPlayerUtility = CDrmPlayerUtility::NewL( *this, 
                                                 0, 
                                                 EMdaPriorityPreferenceNone );
    }

CDRMPlayServerTest::~CDRMPlayServerTest()
    {
    delete iWait;
    delete iDrmPlayerUtility;
    }

CDRMPlayServerTest* CDRMPlayServerTest::NewLC()
    {
    CDRMPlayServerTest* self = new( ELeave ) CDRMPlayServerTest();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

TInt CDRMPlayServerTest::ExecutePlayServerTest()
    {
    iError = KErrNone;
    TRAP( iError, iDrmPlayerUtility->OpenFileL( KTestFile ) );
    if ( iError )
        {
        return iError;
        }
    iWait->Start();
    return iError;
    }

void CDRMPlayServerTest::MdapcInitComplete( 
    TInt aError, 
    const TTimeIntervalMicroSeconds& /*aDuration*/ )
    {
    iWait->AsyncStop();
    iError = aError;
    }

void CDRMPlayServerTest::MdapcPlayComplete( TInt aError )
    {
    iWait->AsyncStop();
    iError = aError;
    }

// End of File  

