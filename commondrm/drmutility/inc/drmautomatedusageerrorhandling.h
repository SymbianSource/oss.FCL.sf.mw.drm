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
* Description:  Implementation class header of the implementation class that
*                implements the MDrmErrorHandling API
*
*/




#ifndef C_CDRMAUTOMATEDUSAGEERRORHANDLING_H
#define C_CDRMAUTOMATEDUSAGEERRORHANDLING_H

#include <e32base.h>
#include <f32file.h>
#include <caf/caferr.h>
#include <caf/caftypes.h>

#include <drmerrorhandling.h>
#include <drmhandleerrorobserver.h>



namespace DRM
{

class CDrmAutomatedUsageImpl;

/**
*  Implementation class for the MDrmErrorHandling interface
*  For DRM automated usage errors
*  This class uses methods in CDrmAutomatedUsageimpl, and is instantiated
*  on instantiation of class DRM::CDrmAutomatedUsage.
*  The class is not intended to be instantiated from any other place.
*
*  @code
*      DRM::DrmAutomatedUsage* iMyAutomatedUsage = 
*          DRM::DrmAutomatedUsage::NewL( iMyCoeEnv );
*      // do some fancy automatedUsage related stuff
*      // ...
*      // DrmAutomatedUsage error occurs
*      DRM::MDrmErrorHandling& myErrorHandler =
*          iMyAutomatedUsage->GetErrorHandler();
*      myErrorHandler.HandleErrorL( theAutomatedUsageError );
*  @endcode
*
*  @lib drmautomatedusageimpl.lib
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CDrmAutomatedUsageErrorHandling ) :
    public CBase, public MDrmErrorHandling
    {
public:

    /**
    * Two-phased constructor.
    * @param aImplementation pointer to the implementation class
    */
    IMPORT_C static CDrmAutomatedUsageErrorHandling* NewL(
        CDrmAutomatedUsageImpl* aImplementation );

    /**
    * Two-phased constructor. Leaves the created object into the
    * cleanup stack
    * @param aImplementation pointer to the implementation class
    */
    IMPORT_C static CDrmAutomatedUsageErrorHandling* NewLC(
        CDrmAutomatedUsageImpl* aImplementation );
    /**
    * Destructor.
    */
    virtual ~CDrmAutomatedUsageErrorHandling();


public: // from base class MDrmErrorHandling

    /**
    * From MDrmErrorHandling
    *
    * Handles DRM error situations related to invalid rights.
    * This method displays appropriate notes, which observer can override,
    * to the user and acquires new rights if needed. Asynchronous version,
    * all errors are signalled using request status.
    *
    * @since S60 5.0
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
    * @see MDrmErrorHandling
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
    * @since S60 5.0
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
    * @see MDrmErrorHandling
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
    * @since S60 5.0
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
    * @see MDrmErrorHandling
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
    * @since S60 5.0
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
    * @see MDrmErrorHandling
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
    CDrmAutomatedUsageErrorHandling();

    /**
    * Second phase constructor
    */
    void ConstructL( CDrmAutomatedUsageImpl* aImplementation );

private: // data

    /**
    * Pointer to the implementation class, which
    * has actual error handling functionality
    * Not owned
    */
    CDrmAutomatedUsageImpl* iImplementation;
    };
}
#endif // C_CDRMAUTOMATEDUSAGEERRORHANDLING_H
