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


#ifndef CDRMUIHANDLINGDATA_H
#define CDRMUIHANDLINGDATA_H

//*** system include files go here:
#include <e32base.h>
#include <f32file.h>
#include <caf/caftypes.h>
#include <drmutilitytypes.h>

//*** forward declarations go here:
namespace ContentAccess
    {
    class CData;
    }

namespace DRM 
{
    
    //*** forward declarations go here:
    class MDrmAsyncObserver;
    class MDrmHandleErrorObserver;
    class MDrmUiCheckRightsObserver;
    
 /**
  *  DRM UI Handling Data storage class
  *
  *  @lib drmuihandlingimpl.lib
  *  @since S60 v5.0
  */
NONSHARABLE_CLASS( CDrmUiHandlingData ) : public CBase
    {

public: // Members

    /**
    * Creates a new CDrmUiHandlingData -object
    *
    * @since S60 v5.0
    * @return A functional CDrmUiHandlingData -object
    * @leave System wide or DRM specific error code
    */
    static CDrmUiHandlingData* NewL();


    /**
    * Creates a new CDrmUiHandlingData -object and leaves
    * it into the cleanup stack
    *
    * @since S60 v5.0
    * @return A functional CDrmUiHandlingData -object
    * @leave System wide or DRM specific error code
    */    
    static CDrmUiHandlingData* NewLC();
   
    /**
    * Destructor.
    */	
    ~CDrmUiHandlingData();

public: // Variables:
    // Operation request status
    MDrmAsyncObserver* iObserver;
    
    // Operation identifier
    TInt iOperationId;

    // Operation code
    TInt iOperation;
    
    // File / content
    RFile* iFile;
    ContentAccess::CData* iFileData;

    // Intent
    ContentAccess::TIntent iIntent;
    
    // Error code
    TInt iError;
    
    // Error observer
    MDrmHandleErrorObserver* iErrorObserver;
    
    // Check rights observer
    MDrmUiCheckRightsObserver* iCheckRightsObserver;
    
    // Url type
    TDrmUiUrlType* iType;
    TDrmUiUrlType iTypeParam;
    
    // Queue Pointer:
    CDrmUiHandlingData* iNext;

private:

	/**
    * C++ default constructor.
    */
    CDrmUiHandlingData();

    /**
    * 2nd phase constructor
    */
    void ConstructL();
 
    };
}

#endif // CDRMUIHANDLINGDATA_H
