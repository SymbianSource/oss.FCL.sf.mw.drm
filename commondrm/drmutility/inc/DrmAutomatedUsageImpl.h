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
* Description:  Implementation class of DRM Automated Usage API
*
*/


#ifndef CDRMAUTOMATEDUSAGEIMPL_H
#define CDRMAUTOMATEDUSAGEIMPL_H

#include <e32base.h>
#include <f32file.h>
#include <caf/caftypes.h>
#include <caf/data.h>
#include <DRMRightsClient.h>
#include <drmautomatedusage.h>
#include <drmautomatedusageobserver.h>

#include "rdrmhelperclient.h"
#include "DrmQueue.h"

//forward declarations
class CDRMConstraint;

namespace DRM
{
class CDrmAutomatedUsageData;
class CDrmUtility;
class CDrmUtilityUI;

/**
*  Utility class for DRM related automated usage handling
*
*  This is implementation class for interface DRM::DrmAutomatedUsage.
*  This is only used via class DRM::DrmAutomatedUsage
*
*  @lib drmautomatedusageimpl.lib
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CDrmAutomatedUsageImpl ) : public CActive
    {

public:

    /** Automated usage states */
    enum TState
        {
        EInit,
        EHandeError,
        ESetAutomated,
        EReady,
        EComplete
        };

public:

    IMPORT_C static CDrmAutomatedUsageImpl* NewL( CCoeEnv* aCoeEnv,
                                                  CDrmUtility* aDrmUtility );

    IMPORT_C static CDrmAutomatedUsageImpl* NewLC( CCoeEnv* aCoeEnv,
                                                   CDrmUtility* aDrmUtility );

    virtual ~CDrmAutomatedUsageImpl();

    /**
    * Checks if given content can be set as an automated content.
    *
    * @since  S60 5.0
    *
    * @param[in]   aFile           file handle to the file
    * @param[in]   aIntent         See ContentAccess::Intent
    * @param[in]   AutomatedType   Usage intention of the automated content
    *
    * @return ETrue if the content can be set as automated content
    *         EFalse if the content can't be set as automated content
    *
    * @leave System wide or DRM specific error code.
    *
    * @see RFile
    * @see ContentAccess::TIntent
    * @see DRM::TDrmAutomatedType
    *
    * @see DRM::CDrmaAutomatedUsage
    */
    IMPORT_C TBool CanSetAutomatedL(
        RFile& aFile,
        const ContentAccess::TIntent aIntent,
        const TDrmAutomatedType aAutomatedType );


    /**
    * Checks if given content can be set as an automated content.
    *
    * @since  S60 5.0
    *
    * @param[in]   aData           CData object to the protected content
    * @param[in]   aIntent         See ContentAccess::Intent
    * @param[in]   AutomatedType   Usage intention of the automated content
    *
    * @return ETrue if the content can be set as automated content
    *         EFalse if the content can't be set as automated content
    *
    * @leave System wide or DRM specific error code.
    *
    * @see ContentAccess::CData
    * @see ContentAccess::TIntent
    * @see DRM::TDrmAutomatedType
    *
    * @see DRM::CDrmaAutomatedUsage
    */
    IMPORT_C TBool CanSetAutomatedL(
        const ContentAccess::CData& aData,
        const ContentAccess::TIntent aIntent,
        const TDrmAutomatedType aAutomatedType );


    /**
    * Registers given content to be used as an automated content.
    *
    * @since S60 5.0
    *
    * @param[in]   aFile           file handle to the file
    * @param[in]   aIntent         See ContentAccess::Intent
    * @param[in]   aAutomatedType  Usage intention of the automated content
    * @param[in]   aServiceType    default: EAUActive
    *
    * @leave System wide or DRM specific error code.
    *
    * @see RFile
    * @see ContentAccess::TIntent
    * @see DRM::TDrmAutomatedType
    * @see DRM::TDrmAutomatedServiceType
    *
    * @see DRM::CDrmaAutomatedUsage
    */
    IMPORT_C void SetAutomatedL(
        RFile& aFile,
        const ContentAccess::TIntent aIntent,
        const TDrmAutomatedType aAutomatedType,
        const TDrmAutomatedServiceType aServiceType = EAUActive );


    /**
    * Registers given content to be used as an automated content.
    *
    * @since S60 5.0
    *
    * @param[in]   aFile           file handle to the file
    * @param[in]   aIntent         See ContentAccess::Intent
    * @param[in]   aAutomatedType  Usage intention of the automated content
    * @param[in]   aObserver       Async callback observer
    * @param[in]   aServiceType    default: EAUActive
    *
    * @return Operation Identifier for the async request
    *         required for cancelling an operation and
    *         identifying which request has been completed
    *
    * @leave System wide or DRM specific error code.
    *
    * @see RFile
    * @see ContentAccess::TIntent
    * @see DRM::TDrmAutomatedType
    * @see DRM::MDrmAsyncObserver
    * @see DRM::TDrmAutomatedServiceType
    *
    * @see DRM::CDrmaAutomatedUsage
    */
    IMPORT_C TInt SetAutomatedAsyncL(
        RFile& aFile,
        const ContentAccess::TIntent aIntent,
        const TDrmAutomatedType aAutomatedType,
        MDrmAsyncObserver& aObserver,
        const TDrmAutomatedServiceType aServiceType = EAUActive );


    /**
    * Registers given content to be used as an automated content.
    *
    * @since S60 5.0
    *
    * @param[in]   aData           CData object to the protected content
    * @param[in]   aIntent         See ContentAccess::Intent
    * @param[in]   aAutomatedType  Usage intention of the automated content
    * @param[in]   aServiceType    default: EAUActive
    *
    * @leave System wide or DRM specific error code.
    *
    * @see ContentAccess::CData
    * @see ContentAccess::TIntent
    * @see DRM::TDrmAutomatedType
    * @see DRM::TDrmAutomatedServiceType
    *
    * @see DRM::CDrmaAutomatedUsage
    */
    IMPORT_C void SetAutomatedL(
        const ContentAccess::CData& aData,
        const ContentAccess::TIntent aIntent,
        const TDrmAutomatedType aAutomatedType,
        const TDrmAutomatedServiceType aServiceType = EAUActive );


    /**
    * Registers given content to be used as an automated content.
    *
    * @since S60 5.0
    *
    * @param[in]   aData           CData object to the protected content
    * @param[in]   aIntent         See ContentAccess::Intent
    * @param[in]   aAutomatedType  Usage intention of the automated content
    * @param[in]   aObserver       Async callback observer
    * @param[in]   aServiceType    default: EAUActive
    *
    * @return Operation Identifier for the async request
    *         required for cancelling an operation and
    *         identifying which request has been completed
    *
    * @leave System wide or DRM specific error code.
    *
    * @see ContentAccess::CData
    * @see ContentAccess::TIntent
    * @see DRM::TDrmAutomatedType
    * @see DRM::MDrmAsyncObserver
    * @see DRM::TDrmAutomatedServiceType
    *
    * @see DRM::CDrmaAutomatedUsage
    */
    IMPORT_C TInt SetAutomatedAsyncL(
        const ContentAccess::CData& aData,
        const ContentAccess::TIntent aIntent,
        const TDrmAutomatedType aAutomatedType,
        MDrmAsyncObserver& aObserver,
        const TDrmAutomatedServiceType aServiceType = EAUActive );


    /**
    * Unregisters given content from being used as an automated content.
    *
    * @since S60 5.0
    *
    * @param[in]   aFile           file handle to the file
    * @param[in]   aIntent         See ContentAccess::Intent
    * @param[in]   aAutomatedType  Usage intention of the automated content
    * @param[in]   aServiceType    default: EAUActive
    *
    * @leave System wide or DRM specific error code.
    *
    * @see RFile
    * @see ContentAccess::TIntent
    * @see DRM::TDrmAutomatedType
    * @see DRM::TDrmAutomatedServiceType
    *
    * @see DRM::CDrmaAutomatedUsage
    */
    IMPORT_C void RemoveAutomatedL(
        RFile& aFile,
        const ContentAccess::TIntent aIntent,
        const TDrmAutomatedType aAutomatedType,
        const TDrmAutomatedServiceType aServiceType = EAUActive );


    /**
    * Unregisters given content from being used as an automated content.
    *
    * @since S60 5.0
    *
    * @param[in]   aFile           file handle to the file
    * @param[in]   aIntent         See ContentAccess::Intent
    * @param[in]   aAutomatedType  Usage intention of the automated content
    * @param[in]  aObserver       Async callback observer
    * @param[in]   aServiceType    default: EAUActive
    *
    * @return Operation Identifier for the async request
    *         required for cancelling an operation and
    *         identifying which request has been completed
    *
    * @leave System wide or DRM specific error code.
    *
    * @see RFile
    * @see ContentAccess::TIntent
    * @see DRM::TDrmAutomatedType
    * @see DRM::MDrmAsyncObserver
    * @see DRM::TDrmAutomatedServiceType
    *
    * @see DRM::CDrmaAutomatedUsage
    */
    IMPORT_C TInt RemoveAutomatedAsyncL(
        RFile& aFile,
        const ContentAccess::TIntent aIntent,
        const TDrmAutomatedType aAutomatedType,
        MDrmAsyncObserver& aObserver,
        const TDrmAutomatedServiceType aServiceType = EAUActive );


    /**
    * Unregisters given content from being used as an automated content.
    *
    * @since S60 5.0
    *
    * @param[in]   aData           CData object to the protected content
    * @param[in]   aIntent         See ContentAccess::Intent
    * @param[in]   aAutomatedType  Usage intention of the automated content
    * @param[in]   aServiceType    default: EAUActive
    *
    * @leave System wide or DRM specific error code.
    *
    * @see ContentAccess::CData
    * @see ContentAccess::TIntent
    * @see DRM::TDrmAutomatedType
    * @see DRM::TDrmAutomatedServiceType
    *
    * @see DRM::CDrmaAutomatedUsage
    */
    IMPORT_C void RemoveAutomatedL(
        const ContentAccess::CData& aData,
        const ContentAccess::TIntent aIntent,
        const TDrmAutomatedType aAutomatedType,
        const TDrmAutomatedServiceType aServiceType = EAUActive );


    /**
    * Unregisters given content from being used as an automated content.
    *
    * @since S60 5.0
    *
    * @param[in]   aData           CData object to the protected content
    * @param[in]   aIntent         See ContentAccess::Intent
    * @param[in]   aAutomatedType  Usage intention of the automated content
    * @param[in]   aObserver       Async callback observer
    * @param[in]   aServiceType    default: EAUActive
    *
    * @return Operation Identifier for the async request
    *         required for cancelling an operation and
    *         identifying which request has been completed
    *
    * @leave System wide or DRM specific error code.
    *
    * @see ContentAccess::CData
    * @see ContentAccess::TIntent
    * @see DRM::TDrmAutomatedType
    * @see DRM::MDrmAsyncObserver
    * @see DRM::TDrmAutomatedServiceType
    *
    * @see DRM::CDrmaAutomatedUsage
    */
    IMPORT_C TInt RemoveAutomatedAsyncL(
        const ContentAccess::CData& aData,
        const ContentAccess::TIntent aIntent,
        const TDrmAutomatedType aAutomatedType,
        MDrmAsyncObserver& aObserver,
        const TDrmAutomatedServiceType aServiceType = EAUActive );


    /**
    * Unregisters given content from being used as an automated content.
    *
    * @since S60 5.0
    *
    * @param[in]   aUniqueId       unique content id
    * @param[in]   aIntent         See ContentAccess::Intent
    * @param[in]   aAutomatedType  Usage intention of the automated content
    * @param[in]   aServiceType    default: EAUActive
    *
    * @leave System wide or DRM specific error code.
    *
    * @see ContentAccess::TIntent
    *
    * @see ContentAccess::CData::GetStringAttribute()
    * @see ContentAccess::TStringAttribute
    * @see ContentAccess::TIntent
    * @see DRM::TDrmAutomatedType
    * @see DRM::TDrmAutomatedServiceType
    *
    * @see DRM::CDrmaAutomatedUsage
    */
    IMPORT_C void RemoveAutomatedL(
        const TDesC& aUniqueId,
        const ContentAccess::TIntent aIntent,
        const TDrmAutomatedType aAutomatedType,
        const TDrmAutomatedServiceType aServiceType = EAUActive );


    /**
    * Unregisters given content from being used as an automated content.
    *
    * @since S60 5.0
    *
    * @param[in]   aUniqueId       unique content id
    * @param[in]   aIntent         See ContentAccess::Intent
    * @param[in]   aAutomatedType  Usage intention of the automated content
    * @param[in]   aObserver       Async callback observer
    * @param[in]   aServiceType    default: EAUActive
    *
    * @return Operation Identifier for the async request
    *         required for cancelling an operation and
    *         identifying which request has been completed
    *
    * @leave System wide or DRM specific error code.
    *
    * @see ContentAccess::CData::GetStringAttribute()
    * @see ContentAccess::TStringAttribute
    * @see ContentAccess::TIntent
    * @see DRM::TDrmAutomatedType
    * @see DRM::MDrmAsyncObserver
    * @see DRM::TDrmAutomatedServiceType
    *
    * @see DRM::CDrmaAutomatedUsage
    */
    IMPORT_C TInt RemoveAutomatedAsyncL(
        const TDesC& aUniqueId,
        const ContentAccess::TIntent aIntent,
        const TDrmAutomatedType aAutomatedType,
        MDrmAsyncObserver& aObserver,
        const TDrmAutomatedServiceType aServiceType = EAUActive );


    /**
    * Registers client to listen events related to automated usage. If
    * the client doesn't do this, all the events are handled automatically
    * according to the S60 style.
    *
    * The observer registered with this function needs to be unregistered
    * before it is deleted.
    *
    * @since S60 5.0
    *
    * @param[in]    aObserver
    *
    * @leave System wide or DRM specific error code.
    *
    * @see DRM::MDrmAutomatedUsageObserver
    *
    * @see DRM::CDrmaAutomatedUsage
    */
    IMPORT_C void RegisterEventObserverL(
        MDrmAutomatedUsageObserver& aObserver );


    /**
    * Unregisters client from listening to events related to automated usage.
    *
    * If register event observer has been called, it must be unregistered
    * before the observer object is deleted.
    *
    * @since S60 5.0
    *
    * @param[in]    aObserver
    *
    * @leave System wide or DRM specific error code.
    *
    * @see DRM::MDrmAutomatedUsageObserver
    *
    * @see DRM::CDrmaAutomatedUsage
    */
    IMPORT_C void UnregisterEventObserverL(
        MDrmAutomatedUsageObserver& aObserver );


    /**
    * Cancel an asyncronous operation
    *
    * @since S60 5.0
    *
    * @param[in]   aOperationId    identifier of the async operation
    *                              to be cancelled
    * @return KErrNotFound if the operation has already been executed
    *         or it does not exist
    *
    * @see DRM::CDrmaAutomatedUsage
    */
    IMPORT_C TInt CancelOperation( TInt aOperationId );


    /**
    * Implementation for MDrmErrorHandling::HandleErrorAsyncL
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
    * @see MDrmErrorHandling
    */
    IMPORT_C TInt HandleErrorAsyncL(
        RFile& aFile,
        ContentAccess::TIntent aIntent,
        TInt aError,
        MDrmHandleErrorObserver& aObserver );

    /**
    * Implementation for MDrmErrorHandling::HandleErrorL
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
    * @see MDrmErrorHandling
    */
    IMPORT_C void HandleErrorL(
        RFile& aFile,
        ContentAccess::TIntent aIntent,
        TInt aError,
        MDrmHandleErrorObserver* aObserver );

    /**
    * Implementation for MDrmErrorHandling::HandleErrorAsyncL
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
    * @see MDrmErrorHandling
    */
    IMPORT_C TInt HandleErrorAsyncL(
        ContentAccess::CData& aFile,
        ContentAccess::TIntent aIntent,
        TInt aError,
        MDrmHandleErrorObserver& aObserver );

    /**
    * Implementation for MDrmErrorHandling::HandleErrorL
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
    * @see MDrmErrorHandling
    */
    IMPORT_C void HandleErrorL(
        ContentAccess::CData& aFile,
        ContentAccess::TIntent aIntent,
        TInt aError,
        MDrmHandleErrorObserver* aObserver );


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
    * From CActive Called if RunL leaves
    */
    TInt RunError( TInt aError );


private: // Member functions

    CDrmAutomatedUsageImpl( CCoeEnv* aCoeEnv,
                            CDrmUtility* aDrmUtility );

    void ConstructL();

    void Activate( TRequestStatus*& aStatus );

    // Checks whether there exists suitable oma RO for automated use
    TBool DoCanSetAutomatedL(
        const TDesC8& aUniqueId8,
        const ContentAccess::TIntent aIntent,
        const TDrmAutomatedType aAutomatedType,
        const TBool aUseQueries = EFalse );

    // Additional RO checks tor theme.
    TBool DoCanSetAutomatedThemeL( const TDesC8& aUniqueId8 );


    TInt DoSetAutomatedL( CDrmAutomatedUsageData* aAutomatedUsageData );


    TInt DoRemoveAutomated( CDrmAutomatedUsageData* aAutomatedUsageData );


private: // Data members

    // Oma Drm client
    RDRMRightsClient iOmaClient;

    // WM Drm Client
    // RWMDrmClient iWmClient;

    // own
    RDrmHelperClient iDrmHelperClient;

    // not own
    MDrmAutomatedUsageObserver* iAuObserver;

    // not own
    CCoeEnv* iCoeEnv;

    // not own
    CDrmUtility* iDrmUtility;

    // own
    CDrmQueue< CDrmAutomatedUsageData >* iDrmQueue;

    // own
    CDrmUtilityUI* iDrmUtilityUi;

     // Async observer and current operation id for async operations
     // not own
    MDrmAsyncObserver* iObserver;
     // not own
    TInt iOperationId;
    };
}

#endif // CDrmAutomatedUsageImpl_H
