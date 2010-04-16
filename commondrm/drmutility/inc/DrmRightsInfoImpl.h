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
* Description:  Implementation class of DRM Rights Info API
*
*/


#ifndef CDRMRIGHTSINFOIMPL_H
#define CDRMRIGHTSINFOIMPL_H

//*** system include files go here:
#include <e32base.h>
#include <f32file.h>
#include <caf/caftypes.h>
#include <DRMRightsClient.h>
#include <drmutilitytypes.h>

namespace DRM
{
    //*** Forward declarations
    class CDrmRightsInfoData;
    class MDrmAsyncObserver;
    class CDrmUtilityCommon;

 /**
  *  Utility class for DRM related rights information handling
  *
  *  @lib drmrightsinfoimpl.lib
  *  @since S60 v5.0
  */
NONSHARABLE_CLASS( CDrmRightsInfoImpl ) : public CActive
    {

public:

    IMPORT_C static CDrmRightsInfoImpl* NewL();

    IMPORT_C static CDrmRightsInfoImpl* NewLC();

    virtual ~CDrmRightsInfoImpl();


    /**
     * Checks the rights and their status for a specific unique id
     *
     * @param[in]   aUniqueId : The unique identifier or the content
     * @param[in]   aIntent : The intent for the rights to check
     * @param[out]  aDetails : Details of the rights status
     * @return none
     * @leave System wide error code
     */
    IMPORT_C void CheckRightsL(
        const TDesC& aUniqueId,
        ContentAccess::TIntent aIntent,
        TDrmRightsInfo& aDetails );

    /**
     * Checks the rights and their status for a specific unique id
     *
     * @param[in]   aUniqueId   The unique identifier or the content
     * @param[in]   aIntent     The intent for the rights to check
     * @param[out]  aDetails    Details of the rights status
     * @param[in]   aObserver   Observer which gets reported of
     *                          the async operation
     *
     * @return Operation identifier
     *
     * @leave System wide error code
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


public: // From base classes


protected: // Functions from base classes

    /**
    * From CActive Cancels async request.
    */
    void DoCancel();

    /**
    * From CActive Called when async request completes.
    */
    void RunL();

    /**
    * From CActive Called when RunL leaves
    */
    TInt RunError( TInt aError );

private:

    CDrmRightsInfoImpl();

    void ConstructL();

    void PerformCheckRightsL( CDrmRightsInfoData& aData );

    void AppendToQueue( CDrmRightsInfoData* aData );

    CDrmRightsInfoData* PopFront();


private: // Data members

    // DrmUtilityCommon
    CDrmUtilityCommon* iDrmUtilityCommon;

    // Oma Drm client
    RDRMRightsClient iOmaClient;

    // WM Drm Client
    // RWMDrmClient iWmClient;

    // current request status:
    MDrmAsyncObserver* iObserver;
    TInt iOperationId;

    // Operation Queue:
    CDrmRightsInfoData* iFirst;
    CDrmRightsInfoData* iLast;

    // To prevent queue handling from being messed up by threads
    RSemaphore iSemaphore;
    };

}

#endif // CDRMRIGHTSINFOIMPL_H
