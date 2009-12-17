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
* Description:  Header file for WMDRM PK Client Wrapper
*
*/


#ifndef CWMDRMPKCLIENTWRAPPER_H
#define CWMDRMPKCLIENTWRAPPER_H

#include <e32base.h>

_LIT( KWmdrmPkClientWrapperName, "\\system\\libs\\wmdrmpkclientwrapper.dll" );
const TInt KWmdrmPkClientNewL = 1;

class RWmDrmPkClient;

class MWmDrmPkClientWrapper
    {

public:

    virtual TInt Connect() = 0;
    
    virtual void Close() = 0;
    
    virtual TInt DeleteRights() = 0;
    
    };

NONSHARABLE_CLASS( CWmDrmPkClientWrapper ): public CBase,
                                            public MWmDrmPkClientWrapper
    {

public:

    IMPORT_C static CWmDrmPkClientWrapper* NewL();
    
    virtual ~CWmDrmPkClientWrapper();

    TInt Connect();
    
    void Close();
     
    TInt DeleteRights();

private:

    CWmDrmPkClientWrapper();
    void ConstructL();

private: // data

    RWmDrmPkClient* iClient;

    TInt GenerateSyncChallengeL(TUint32 aMaxCount,
                                TUint32 aMaxHours,
                                TUint32 aStart,
                                TUint32 aItemsToProcess,
                                TUint32 &aNextStart,
                                TUint32 &aProcessed,
                                HBufC8*& aChallenge);
                                
    HBufC8* GetDeviceCertificateL();

    
    };

#endif // CWMDRMPKCLIENTWRAPPER_H
