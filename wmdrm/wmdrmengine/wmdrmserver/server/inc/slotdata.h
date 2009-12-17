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
* Description:  Server definitions
*
*/


#ifndef __SLOTDATA_H
#define __SLOTDATA_H

#include <e32base.h>
#include "wmdrmserver.h"

class CSlotData: public CBase
    {
public:

    static CSlotData* NewL( 
        CWmDrmServer* aServer, 
        const TDesC8& aStore,
        const TDesC8& aNamespace,
        const TDesC8& aHashKey,
        const TDesC8& aUniqueKey );
    
    CSlotData( 
        CWmDrmServer* aServer, 
        const TDesC8& aStore,
        const TDesC8& aNamespace,
        const TDesC8& aHashKey,
        const TDesC8& aUniqueKey );
    
    ~CSlotData();


    void CreateL( TInt& aInitialSize );

    TInt OpenL( TInt& aCurrentSize );

    TInt Read( TInt aPosition, TDes8& aBuffer );

    TInt WriteL( TInt aPosition, const TDesC8& aBuffer );

    TInt ResizeL( TInt aNewSize );

    TInt DeleteL();

    TInt Size();

    void Close();

    void CloseFile();

    TBool IsOpen();

    void FlushL();
	
	CWmDrmServer* iServer;
	TBuf8<KMaxWmDrmStoreNameSize> iStore;
    TBuf8<KMaxWmDrmNamespaceNameSize> iNamespace;
    TBuf8<KWmDrmIdSize> iHashKey;
    TBuf8<KWmDrmIdSize> iUniqueKey;
	TBool iDirty;
	TBool iExists;

	// Is the file content read into the iData field and valid?
    TBool iOpen;
	
	TInt iReferences;
	RBuf8 iData;
    };    

#endif
