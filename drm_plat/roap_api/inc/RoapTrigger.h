/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#ifndef ROAP_TRIGGER_H
#define ROAP_TRIGGER_H

// INCLUDES
#include <e32base.h>

#include <hash.h>
#include "RoapMessage.h"
#include "RoapEngBase.h"

// CONSTANTS

namespace Roap
{

// CLASS DECLARATION

/**
* ROAP Trigger Data.
*/
class CRoapTrigger: public CRoapMessage
    {
    public:     // Constructors and destructor

        /**
        * Two-phased constructor. Leaves on failure.
        * @return 
        */      
        IMPORT_C static CRoapTrigger* NewL();

        /**
        * Two-phased constructor. Leaves on failure.
        * @return
        */      
        IMPORT_C static CRoapTrigger* NewL( const TTriggerType& aTriggerType,
                                            const TDesC8& aRiId,
                                            const TDesC8& aDomainId,
                                            const TDesC8& aRoapUrl,
                                            const RPointerArray<HBufC8>& aRoIdList,
                                            const RPointerArray<HBufC8>& aContentIdList );
                                  
        /**
        * 
        * @return
        */                                   
        IMPORT_C HBufC8* MessageAsXmlL(void);
        
        /**
        * Destructor.
        */      
        IMPORT_C virtual ~CRoapTrigger();
        
        /**
        * Validates Trigger.
        * Returns ETrue if Trigge is valid 
        */
        IMPORT_C TBool ValidTrigger();
        
    protected:  // Constructors and destructor

        /**
        * Constructor.
        */      
        CRoapTrigger();
        
        /**
        * Second phase constructor. Leaves on failure.
        */      
        void ConstructL();
        
        /**
        * Second phase constructor. Leaves on failure.
        */              
        void ConstructL( const TTriggerType& aTriggerType,
                         const TDesC8& aRiId,
                         const TDesC8& aDomainId,
                         const TDesC8& aRoapUrl,
                         const RPointerArray<HBufC8>& aRoIdList,
                         const RPointerArray<HBufC8>& aContentIdList );

    public:    // data
        TBuf8<SHA1_HASH> iRiId;
        HBufC8* iId;
        HBufC8* iDomainId;
        RPointerArray<HBufC8> iRoIdList;
        RPointerArray<HBufC8> iContentIdList;
        TTriggerType iTriggerType;
        HBufC8* iRoapUrl;
        HBufC8* iSignature;
        HBufC8* iEncKey;
        HBufC8* iNonce;
        HBufC8* iDigestValue;
        HBufC8* iRiAlias;
        HBufC8* iDomainAlias;
    };
}

#endif
