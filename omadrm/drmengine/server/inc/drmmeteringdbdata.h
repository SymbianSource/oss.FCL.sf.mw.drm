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
* Description:  Datatype for the Metering Database Data Entry
*
*/


#ifndef DRMMETERINGDBDATA_H
#define DRMMETERINGDBDATA_H

// INCLUDES

#include <e32base.h>
#include "DRMTypes.h"

/**
*  CDrmMeteringDbData implements datatype for storing the metering data
*  of one entry in the Metering database.
*
*  @lib RightsServer.dll
*  @since 3.2
*/

NONSHARABLE_CLASS( CDrmMeteringDbData ) : public CBase 
    {
    
    public: // Constructors and destructor
        
        /**
        * NewLC
        *
        * Creates an empty instance of the CDrmMeteringDbData class and returns a pointer 
        * to it. The function leaves the object into the cleanup stack
        *
        * @since  3.2
        * @return Functional CDrmMeteringDbData object, Function leaves if an error 
        *         occurs.
        */
        static CDrmMeteringDbData* NewLC();                                     
                                     
        /**
        * NewL
        *
        * Creates an empty instance of the CDrmMeteringDbData class and returns a pointer 
        * to it.
        *       
        * @since  3.2
        * @return Functional CDrmMeteringData object, Function leaves if an error 
        *         occurs.
        */
        static CDrmMeteringDbData* NewL();                                     
          
        /**
        * Destructor
        *
        * @since  3.2
        *
        */
        virtual ~CDrmMeteringDbData();
    
    private:
        /**
        * Default Constructor - First phase.
        *
        * @since  3.2
        *
        */
        CDrmMeteringDbData(); 

        /**
        * ConstructL
        *
        * Second phase constructor - Empty
        * 
        * @since  3.2
        *
        */
        void ConstructL();
    
        /**
        * Assignment operator - Prevented
        *
        * @since  3.2
        *
        */
        CDrmMeteringDbData& operator =( const CDrmMeteringDbData& );    
    
        /**
        * Copy constructor - Prevented
        *
        * @since  3.2 
        *
        */
        CDrmMeteringDbData( const CDrmMeteringDbData& );                

    public: // Public data
    
        HBufC8* iContentId; 
        TBuf8<KRiIdSize> iRiId;
        TUint32 iCount;
        TTimeIntervalSeconds iAccumulatedTime;
        ContentAccess::TIntent iPermission;
        HBufC8* iParentUid;

    private: // Internal Data
    };
#endif      // DRMMETERINGDBDATA_H   
            
// End of File
