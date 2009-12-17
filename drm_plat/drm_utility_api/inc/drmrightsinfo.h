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
* Description:  Provides operations for rights information handling of DRM
 *                content
 *
*/


#ifndef CDRMRIGHTSINFO_H
#define CDRMRIGHTSINFO_H

//*** system include files go here:
#include <e32base.h>
#include <caf/caftypes.h>
#include <drmutilitytypes.h>

namespace DRM
    {

    //*** forward declarations go here:
    class CDrmUtility;
    class CDrmRightsInfoImpl;
    class MDrmAsyncObserver;

    /**
     *  Utility class for DRM related rights information handling
     *
     *  @lib drmrightsinfo.lib
     *  @since S60 v5.0
     */
NONSHARABLE_CLASS( CDrmRightsInfo ) : public CBase
    {

public:

    /**
     * Creates a new CDrmRightsInfo object and returns a pointer to it
     *
     * @return A functional CDrmRightsInfo -object
     * @leave System wide error code
     */
    IMPORT_C static CDrmRightsInfo* NewL();

    /**
     * Creates a new CDrmRightsInfo object and returns a pointer to it
     * Leaves the pointer to the cleanup stack
     *
     * @return A functional CDrmRightsInfo -object
     * @leave System wide error code
     */
    IMPORT_C static CDrmRightsInfo* NewLC();

    /**
     * Destructor
     */
    virtual ~CDrmRightsInfo();


    /**
     * Returns a reference to a CDrmUtility instance. The ownership
     * of the instance stays with the CDrmRightsInfo -class
     *
     * @since S60 v5.0
     *
     * @return A reference to a functional CDrmUtility instance
     *
     * @leave None
     *
     * @see CDrmUtility
     */
    IMPORT_C CDrmUtility& GetUtility() const;



    /**
     * Checks the rights and their status for a specific unique id
     *
     * @param[in]   aUniqueId   The unique identifier of the content
     * @param[in]   aIntent     The intent for the rights to check
     * @param[out]  aDetails    Details of the rights status
     * @return none
     * @leave System wide error code
     * @see ContentAccess::TIntent
     */
    IMPORT_C void CheckRightsL(
            const TDesC& aUniqueId,
            ContentAccess::TIntent aIntent,
            TDrmRightsInfo& aDetails );

    /**
     * Checks the rights and their status for a specific unique id
     *
     * @param[in]   aUniqueId   The unique identifier of the content
     * @param[in]   aIntent     The intent for the rights to check
     * @param[out]  aDetails    Details of the rights status, the
     *                          details are available via this
     *                          parameter, once the async call completes
     *                          successfully
     * @param[in]   aObserver   reference to observer
     *
     * @return Operation Identifier for the async request
     *         required for cancelling an operation and
     *         identifying which request has been completed
     *
     * @leave System wide error code
     * @see ContentAccess::TIntent
     */
    IMPORT_C TInt CheckRightsAsyncL(
            const TDesC& aUniqueId,
            ContentAccess::TIntent aIntent,
            TDrmRightsInfo& aDetails,
            MDrmAsyncObserver& aObserver );


    /**
     * Cancel an asyncronous operation
     *
     * @since S60 v5.0
     * @param[in]   aOperationId    identifier of the async operation
     *                              to be cancelled
     * @return KErrNotFound if the operation has already been executed
     *         or it does not exist
     */
    IMPORT_C TInt CancelOperation( TInt aOperationId );

private:

    //*** private, non-derived functions go here, starting with the C++
    //*** constructor and the Symbian second-phase constructor

    CDrmRightsInfo();

    void ConstructL();

private: // Data members

    /**
     * Implementation class pointer
     * Owned
     */
    CDrmRightsInfoImpl* iImplementation;


    /**
     * CDrmUtility class pointer
     * Owned
     */
    CDrmUtility* iUtility;

    };
    }


#endif // CDRMRIGHTSINFO_H
