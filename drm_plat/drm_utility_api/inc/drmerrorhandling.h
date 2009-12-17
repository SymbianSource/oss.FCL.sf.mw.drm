/*
* Copyright (c) 2006, 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility API definition for Error Handling
 *
*/


#ifndef MDRMERRORHANDLING_H
#define MDRMERRORHANDLING_H

#include <e32base.h>
#include <f32file.h>
#include <caf/caferr.h>
#include <caf/caf.h>

#include "drmhandleerrorobserver.h"

namespace DRM
    {
    /**
     *  Utility class for DRM related UI
     *
     *  By using this class an application can resolve situation where there is
     *  no valid rights for the content, check status of rights and display
     *  an embedded rights details view.
     *
     *  @since S60 v5.0
     */
class MDrmErrorHandling
    {

public:

    /**
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
    IMPORT_C virtual TInt HandleErrorAsyncL(
            RFile& aFile,
            ContentAccess::TIntent aIntent,
            TInt aError,
            MDrmHandleErrorObserver& aObserver ) = 0;

    /**
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
    IMPORT_C virtual void HandleErrorL(
            RFile& aFile,
            ContentAccess::TIntent aIntent,
            TInt aError,
            MDrmHandleErrorObserver* aObserver ) = 0;

    /**
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
    IMPORT_C virtual TInt HandleErrorAsyncL(
            ContentAccess::CData& aFile,
            ContentAccess::TIntent aIntent,
            TInt aError,
            MDrmHandleErrorObserver& aObserver ) = 0;

    /**
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
    IMPORT_C virtual void HandleErrorL(
            ContentAccess::CData& aFile,
            ContentAccess::TIntent aIntent,
            TInt aError,
            MDrmHandleErrorObserver* aObserver ) = 0;
    };
    }



#endif // MDRMERRORHANDLING_H
