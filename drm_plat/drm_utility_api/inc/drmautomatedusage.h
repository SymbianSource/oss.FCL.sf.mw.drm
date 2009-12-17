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
* Description:  Provides operations for automated usage of DRM content
*
*/


#ifndef CDRMAUTOMATEDUSAGE_H
#define CDRMAUTOMATEDUSAGE_H

#include <e32base.h>
#include <f32file.h>

#include <caf/caftypes.h>
#include <caf/data.h>

#include <drmutilitytypes.h>
#include <drmautomatedusageobserver.h>
#include <drmutility.h>
#include <drmerrorhandling.h>



class CCoeEnv;

namespace DRM
{
class CDrmAutomatedUsageImpl;

/**
*  Utility class for DRM related automated usage handling
*
*  Usage:
*  @code
*      #include <drmautomatedusage.h>
*      // Instantiate class CDrmAutomatedUsage
*      DRM::CDrmAutomatedUsage* iAutomatedUsage =
*          DRM::CDrmAutomatedUsage::NewLC();
*      // ...
*      // RFile myRingToneFile;
*      // open my Ringtone for reading
*      // ...
*      ContentAccess::CData* myDataObject = ContentAccess::CData::NewL(
*          myRingToneFile,
*          ContentAccess::KDefaultContentObject,
*          ContentAccess::EPeek );
*      // check if content can be set as automated
*      if ( iAutomatedUsage->CanSetAutomatedL(
*          myDataObject,
*          ContentAccess::EPlay,
*          DRM::EAUAutomatedTypeRingingTone ) )
*          {
*          // automated usage allowed
*
*          // we register content as ringing tone in this example
*          iAutomatedUsage->SetAutomated(
*              myDataObject,
*              ContentAccess::EPlay,
*              DRM::EAUAutomatedTypeRingingTone,
*              DRM::EAUPassive );
*
*          }
*          else
*          {
*          // automated usage not allowed
*          }
*  @endcode
*
*  @lib drmautomatedusage.lib
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CDrmAutomatedUsage ) : public CBase
    {
public:

    /**
    * Creates a new CDrmAutomatedUsage object and returns a pointer to it
    *
    * @since S60 5.0
    *
    * @param aCoeEnv   A pointer to an instance of CCoeEnv. If the
    *                  parameter is not provided the global instance
    *                  will be used. In the case of a server process
    *                  where the global instance is not available,
    *                  global notes will be used.
    *
    *                  If no CCoeEnv instance is available, applications
    *                  launched will be launched as standalone
    *                  applications.
    *
    * @return A functional CDrmAutomatedUsage -object
    *
    * @leave System wide error code
    *
    * @see CCoeEnv
    *
    */
    IMPORT_C static CDrmAutomatedUsage* NewL( CCoeEnv* aCoeEnv = NULL );


    /**
    * Creates a new CDrmAutomatedUsage object and returns a pointer to it
    * Leaves the pointer to the cleanup stack
    *
    * @since S60 5.0
    *
    * @param aCoeEnv   A pointer to an instance of CCoeEnv. If the
    *                  parameter is not provided the global instance
    *                  will be used. In the case of a server process
    *                  where the global instance is not available,
    *                  global notes will be used.
    *
    *                  If no CCoeEnv instance is available, applications
    *                  launched will be launched as standalone
    *                  applications.
    *
    * @return A functional CDrmAutomatedUsage -object
    *
    * @leave System wide error code
    *
    * @see CCoeEnv
    *
    */
    IMPORT_C static CDrmAutomatedUsage* NewLC( CCoeEnv* aCoeEnv = NULL );


    /**
    * Destructor
    */
    virtual ~CDrmAutomatedUsage();


    /**
    * Returns a reference to a CDrmUtility instance. The ownership
    * of the instance stays with the CDrmAutomatedUsage -class
    *
    * @since S60 5.0
    *
    * @return A reference to a functional CDrmUtility instance
    *
    * @see DRM::CDrmUtility
    */
    IMPORT_C CDrmUtility& GetUtility() const;


    /**
    * Returns a reference to a class which implements the
    * MDrmErrorHandling interface. The ownership
    * of the instance stays with the CDrmAutomatedUsage -class
    *
    * @since S60 5.0
    *
    * @return A reference to a functional object implementing the
    *         MDrmErrorHandling interface
    *
    * @see DRM::MDrmErrorHandling
    */
    IMPORT_C MDrmErrorHandling& GetErrorHandler() const;


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
    */
    IMPORT_C TInt CancelOperation( TInt aOperationId );


private: // member functions

    /**
    *  Default constructor
    */
    CDrmAutomatedUsage();


    /**
    *  Second phase constructor
    */
    void ConstructL( CCoeEnv* aCoeEnv );


private: // data members
    /**
    * Implementation class pointer
    * Owned
    */
    CDrmAutomatedUsageImpl* iImplementation;

    /**
    * Implementation class pointer
    * Owned
    */
    MDrmErrorHandling* iErrorHandler;

    /**
    * Implementation class pointer
    * Owned
    */
    CDrmUtility* iUtility;
    };
}


#endif // CDRMAUTOMATEDUSAGE_H
