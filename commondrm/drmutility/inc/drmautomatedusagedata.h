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
* Description:  Data class for async operations
*                in DrmAutomatedUsageImpl class
*
*/


#ifndef CDRMAUTOMATEDUSAGEDATA_H
#define CDRMAUTOMATEDUSAGEDATA_H

#include <e32base.h>
#include <caf/caftypes.h>
#include <caf/data.h>
#include <drmutilitytypes.h>

namespace DRM
{

class MDrmAsyncObserver;

/**
*  DRM Automated Usage Data storage class
*
*  parameter container class used by DRM::CDrmAutomatedUsageImpl
*
*  @lib drmautomatedusageimpl.lib
*
*  @since S60 5.0
*
*  @see DRM::CDrmAutomatedUsageImpl
*/
NONSHARABLE_CLASS( CDrmAutomatedUsageData ) : public CBase
    {

public: // Members
    /**
    * Possible operations for atuomated content relresented by
    * object of this class
    */
    enum TOperation
        {
        ESetAutomated,
        ERemoveAutomated,
        EHandeError
        };

    /**
    * Creates a new CDrmAutomatedUsageData -object
    *
    * @since S60 5.0
    * @return A functional CDrmAutomatedUsageData -object
    * @leave System wide or DRM specific error code
    */
    static CDrmAutomatedUsageData* NewL(
        const ContentAccess::CData& aData,
        ContentAccess::TIntent aIntent,
        TDrmAutomatedType aAutomatedType,
        MDrmAsyncObserver* aObserver,
        TDrmAutomatedServiceType aServiceType,
        TOperation aOperation );


    /**
    * Creates a new CDrmAutomatedUsageData -object and leaves
    * it into the cleanup stack
    *
    * @since S60 5.0
    * @return A functional CDrmAutomatedUsageData -object
    * @leave System wide or DRM specific error code
    */
    static CDrmAutomatedUsageData* NewLC(
        const ContentAccess::CData& aData,
        ContentAccess::TIntent aIntent,
        TDrmAutomatedType aAutomatedType,
        MDrmAsyncObserver* aObserver,
        TDrmAutomatedServiceType aServiceType,
        TOperation aOperation );

    /**
    * Creates a new CDrmAutomatedUsageData -object
    *
    * @since S60 5.0
    * @return A functional CDrmAutomatedUsageData -object
    * @leave System wide or DRM specific error code
    */
    static CDrmAutomatedUsageData* NewL(
        const TDesC& aUniqueId,
        ContentAccess::TIntent aIntent,
        TDrmAutomatedType aAutomatedType,
        MDrmAsyncObserver* aObserver,
        TDrmAutomatedServiceType aServiceType,
        TOperation aOperation );


    /**
    * Creates a new CDrmAutomatedUsageData -object and leaves
    * it into the cleanup stack
    *
    * @since S60 5.0
    * @return A functional CDrmAutomatedUsageData -object
    * @leave System wide or DRM specific error code
    */
    static CDrmAutomatedUsageData* NewLC(
        const TDesC& aUniqueId,
        ContentAccess::TIntent aIntent,
        TDrmAutomatedType aAutomatedType,
        MDrmAsyncObserver* aObserver,
        TDrmAutomatedServiceType aServiceType,
        TOperation aOperation );

    /**
    * Destructor.
    */
    ~CDrmAutomatedUsageData();

public: // Variables:
    // Operation code
    TOperation iOperation;

    // Operation id
    // Owned
    TInt iOperationId;

    // The content id requested as 16 bit descriptor
    // Owned
    HBufC16* iUniqueId;

    // The content id requested as 8 bit descriptor, this exists if it's
    // For internal DRM API
    // Owned
    HBufC8* iUniqueId8;

    // Intent
    // Owned
    ContentAccess::TIntent iIntent;

    // Desired automated content type for automated content
    // Owned
    TDrmAutomatedType iAutomatedType;

    // Observer pointer for notifying asynchronous operations
    // Not owned
    MDrmAsyncObserver* iObserver;

    // Desired automated content type for automated content
    TDrmAutomatedServiceType iServiceType;


private:

    /**
    * C++ default constructor.
    */
    CDrmAutomatedUsageData(
        ContentAccess::TIntent aIntent,
        TDrmAutomatedType aAutomatedType,
        MDrmAsyncObserver* aObserver,
        TDrmAutomatedServiceType aServiceType,
        TOperation aOperation );

    /**
    * C++ copy constructor.
    */
    CDrmAutomatedUsageData( const CDrmAutomatedUsageData& );

    /**
    * 2nd phase constructor
    */
    void ConstructL( const ContentAccess::CData& aData );

    void ConstructL( const TDesC& aUniqueId );

    /**
    * Initialises operationId to unique value.
    */
    void SetOperationId();
    };
}


#endif // CDRMAUTOMATEDUSAGEDATA_H
