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
* Description:  Generic interface class for Asynchronous callbacks from
 *                asynchronous functions in the DRM Utility API
 *
 *
*/


#ifndef MDRMASYNCOBSERVER_H
#define MDRMASYNCOBSERVER_H

#include <e32def.h>

namespace DRM
    {
    /**
     *  Observer class for Asynchronous operations in DRM Utility
     *
     *  Provides callback(s) to the the observer when the asynchronous operations
     *  complete
     *  @code
     *   ?good_class_usage_example(s)
     *  @endcode
     *
     *  @since S60 v5.0
     */
class MDrmAsyncObserver
    {
public:

    /**
     * A callback function which tells when the asynchronous operation has been
     * completed
     *
     * @since S60 v5.0
     * @param[in] aOperationId          Identifier of the asynchronous operation
     *                                  which was returned when the asynchronous
     *                                  operation was initiated
     * @param[in] aOperationStatus      The final status of the operation
     * @return None
     * @leave None
     */
    virtual void OperationCompleted( TInt aOperationId, TInt aOperationStatus ) = 0;
    };

    }
#endif // MDRMASYNCOBSERVER_H
