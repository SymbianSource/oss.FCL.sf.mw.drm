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
* Description:  Implementation class header of MDrmErrorHandling API
*
*/


#ifndef C_CDRMUIERRORHANDLING_H
#define C_CDRMUIERRORHANDLING_H

#include <e32base.h>
#include <f32file.h>
#include <caf/caftypes.h>
#include <drmerrorhandling.h>

namespace DRM
{
    
    //*** forward declarations go here:
    class CDrmUiHandlingImpl;
    class MDrmHandleErrorObserver;
    
/**
 *  Implementation class for the MDrmErrorHandling interface
 *  For DRM UI Handling
 *
 *  @lib drmuihandling.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CDrmUiErrorHandling ) :
    public CBase, public MDrmErrorHandling
    {
public:

    /**
     * Two-phased constructor.
     * @param aImplementation pointer to the implementation class
     */
    IMPORT_C static CDrmUiErrorHandling* NewL(
        CDrmUiHandlingImpl* aImplementation );
    
    /**
     * Two-phased constructor. Leaves the created object into the
     * cleanup stack
     * @param aImplementation pointer to the implementation class
     */
    IMPORT_C static CDrmUiErrorHandling* NewLC(
        CDrmUiHandlingImpl* aImplementation );
    /**
    * Destructor.
    */
    virtual ~CDrmUiErrorHandling();


public: // from base class MDrmErrorHandling

    /**
     * From MDrmErrorHandling
     *
     * Handles DRM error situations related to invalid rights.
     * This method displays appropriate notes, which observer can override, 
     * to the user and acquires new rights if needed. Asynchronous version,
     * all errors are signalled using request status. 
     *
     * @since S60 v5.0
     * @param[in]   aFile       file for which the error happened
     * @param[in]   aIntent     the CAF intent which was used to recieve 
     *                          the error
     * @param[in]   aError      error code
     * @param[in]   aObserver   reference to observer
     *
     * @return Operation Identifier for the async request
     *         required for cancelling an operation and
     *         identifying which request has been completed
     *
     * @see MDrmHandleErrorObserver
     * @see ContentAccess::TIntent
     */
    virtual TInt HandleErrorAsyncL( 
        RFile& aFile,
        ContentAccess::TIntent aIntent, 
        TInt aError, 
        MDrmHandleErrorObserver& aObserver );
        
    /**
     * From MDrmErrorHandling
     *
     * Handles DRM error situations related to invalid rights.
     * This method displays appropriate notes, which observer can override, 
     * to the user and acquires new rights if needed. Synchronous version,
     * leaves in case of error. 
     *
     * @since S60 v5.0
     * @param[in]   aFile       file for which the error happened
     * @param[in]   aIntent     the CAF intent which was used to recieve 
     *                          the error
     * @param[in]   aError      error code
     * @param[in]   aObserver   pointer to observer, NULL if no observer
     *
     * @leave   KErrArgument    File is not DRM protected.
     *
     * @see MDrmHandleErrorObserver
     * @see ContentAccess::TIntent
     */
    virtual void HandleErrorL( 
        RFile& aFile,
        ContentAccess::TIntent aIntent,              
        TInt aError, 
        MDrmHandleErrorObserver* aObserver );
    
    /**
     * From MDrmErrorHandling
     *
     * Handles DRM error situations related to invalid rights.
     * This method displays appropriate notes, which observer can override, 
     * to the user and acquires new rights if needed. Asynchronous version,
     * all errors are signalled using request status. 
     *
     * @since S60 v5.0
     * @param[in]   aFile       content for which the error happened
     * @param[in]   aIntent     the CAF intent which was used to recieve 
     *                          the error         
     * @param[in]   aError      error code
     * @param[in]   aObserver   reference to observer
     *
     * @return Operation Identifier for the async request
     *         required for cancelling an operation and
     *         identifying which request has been completed
     *
     * @see MDrmHandleErrorObserver
     * @see ContentAccess::TIntent
     */
    virtual TInt HandleErrorAsyncL( 
        ContentAccess::CData& aFile,
        ContentAccess::TIntent aIntent,
        TInt aError, 
        MDrmHandleErrorObserver& aObserver );
        
    /**
     * From MDrmErrorHandling
     *
     * Handles DRM error situations related to invalid rights.
     * This method displays appropriate notes, which observer can override, 
     * to the user and acquires new rights if needed. Synchronous version,
     * leaves in case of error. 
     *
     * @since S60 v5.0
     * @param[in]   aFile       content for which the error happened
     * @param[in]   aIntent     the CAF intent which was used to recieve 
     *                          the error         
     * @param[in]   aError      error code
     * @param[in]   aObserver   pointer to observer, NULL if no observer
     *
     * @leave   KErrArgument    File is not DRM protected.
     *
     * @see MDrmHandleErrorObserver
     * @see ContentAccess::TIntent
     */
    virtual void HandleErrorL( 
        ContentAccess::CData& aFile,
        ContentAccess::TIntent aIntent,
        TInt aError, 
        MDrmHandleErrorObserver* aObserver );


private:

    /**
    * C++ constructor
    */
    CDrmUiErrorHandling();

    /**
    * Second phase constructor
    */
    void ConstructL( CDrmUiHandlingImpl* aImplementation );

private: // data

    /**
     * Pointer to the implementation class
     * Not own.
     */
    CDrmUiHandlingImpl* iImplementation;
    };

}
    
#endif // C_CDRMUIERRORHANDLING_H
