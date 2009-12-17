/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Data class for async operations in DrmRightInfoImpl -class 
*
*/


#ifndef CDRMRIGHTSINFODATA_H
#define CDRMRIGHTSINFODATA_H

//*** system include files go here:
#include <e32base.h>
#include <caf/caftypes.h>
#include <drmutilitytypes.h>

namespace DRM 
{

    //*** Forward declaration
    class MDrmAsyncObserver;

 /**
  *  DRM Rights Info Data storage class
  *
  *  @lib drmrightsinfoimpl.lib
  *  @since S60 v5.0
  */
NONSHARABLE_CLASS( CDrmRightsInfoData ) : public CBase
    {

public: // Members

    /**
    * Creates a new CDrmRightsInfoData -object
    *
    * @since S60 5.0
    * @return A functional CDrmRightsInfoData -object
    * @leave System wide or DRM specific error code
    */
    static CDrmRightsInfoData* NewL();


    /**
    * Creates a new CDrmRightsInfoData -object and leaves
    * it into the cleanup stack
    *
    * @since S60 5.0
    * @return A functional CDrmRightsInfoData -object
    * @leave System wide or DRM specific error code
    */    
    static CDrmRightsInfoData* NewLC();
   
    /**
    * Destructor.
    */	
    ~CDrmRightsInfoData();

public: // Variables:
    // Operation request status
    MDrmAsyncObserver* iObserver;
    
    // Operation id
    TInt iOperationId;

    // Operation code
    TInt iOperation;

    // The content id requested as 16 bit descriptor
    HBufC16* iUniqueId;

    // The content id requested as 8 bit descriptor, this exists if it's
    // required by the operation
    HBufC8* iUniqueId8;

    // Intent
    ContentAccess::TIntent iIntent;
    
    // Details
    TDrmRightsInfo* iDetails;
    
    // Queue Pointer:
    CDrmRightsInfoData* iNext;

private:

	/**
    * C++ default constructor.
    */
    CDrmRightsInfoData();

    /**
    * 2nd phase constructor
    */
    void ConstructL();
 
    };
}


#endif // CDRMRIGHTSINFODATA_H
