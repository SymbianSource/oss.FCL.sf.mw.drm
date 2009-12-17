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
* Description:  Observer for automated usage UI notifications
 *
*/


#ifndef MDRMAUTOMATEDUSAGEOBSERVER_H
#define MDRMAUTOMATEDUSAGEOBSERVER_H


#include <caf/caftypes.h>

#include <drmutilitytypes.h>

namespace DRM
    {


/**
*  Observer for DRM automated usage
*
*  In order to be able to get notifications of drm automated usage events
*  client must implement this interface
*
*  Usage:
*  @code
*        // Call NewL() to create an instance of CDrmAutomatedUsage. 
*        iDrmAutomatedUsage = DRM::CDrmAutomatedUsage::NewL();
*        
*        // register to listen automated usage events
*        iDrmAutomatedUsage->RegisterEventObserverL( this );
*        
*        // Implement automated usage callback
*        DRM::TAutomatedUsageAction CMyClass::AutomatedUsageEvent( 
*            DRM::TAutomatedUsageEvent aAutomatedUsageEvent,
*            const TDesC& aUniqueId,
*            ContentAccess::TIntent aIntent )
*            {
*            if ( aAutomatedUsageEvent == DRM::EAutomatedContentExpired )
*                {
*                // some automated content expired
*                
*                // check unique id if we are interested about this content
*                if ( !aUniqueId.Compare( myUniqueId ) )
*                    {
*                    // in this example we let utility to handle situation
*                    return DRM::EAUActionDefault;
*                    }
*                }
*                return DRM::EAUActionIgnore;
*            }
*  @endcode
*
*
*  @lib drmautomatedusage.lib
*  @since S60 5.0
*/
class MDrmAutomatedUsageObserver
    {
public:

    /**
    * Callback for automated usage events
    *
    * @since S60 5.0
    * @param aAutomatedUsageEvent   Automated usage event type
    * @param aUniqueId              Unique ID of the content related to event
    * @param aType                  Type of the automated content related to
    *                               the event
    *
    * @return action how to proceed
    *
    * @see ContentAccess::TIntent
    * @see DRM::TAutomatedUsageAction
    * @see DRM::TAutomatedUsageEvent
    */
    virtual TAutomatedUsageAction AutomatedUsageEvent(
        TAutomatedUsageEvent aAutomatedUsageEvent,
        const TDesC& aUniqueId,
        const TDrmAutomatedType aType,
        const TDrmAutomatedServiceType aServiceType,
        ContentAccess::TIntent aIntent = ContentAccess::EUnknown ) = 0;

    /**
    * Callback for drm utility handled event keycodes.
    *
    * @since S60 5.0
    * @param aAutomatedUsageEvent   Automated usage event type
    * @param aUniqueId              Unique ID of the content under event
    * @apram aKeycode               button/key code returned from user
    *                               interaction
    * @return action how to proceed
    *
    * @see DRM::TAutomatedUsageEvent
    */
    virtual void AutomatedUsageKeyCode(
        TAutomatedUsageEvent aAutomatedUsageEvent,
        const TDesC& aUniqueId,
        TInt aKeyCode ) = 0;
    };
}

#endif // MDRMAUTOMATEDUSAGEOBSERVER_H
