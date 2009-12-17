/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The applications that want to use the notification service
*                need to inherit from this class.
*
*/



#ifndef MDRMEVENTOBSERVER_H
#define MDRMEVENTOBSERVER_H

//  INCLUDES
#include <DRMEvent.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  This class offers abstract interface to receive DRM notifications
*  from DRM notifier.
*
*  @lib DRMCommon.dll
*  @since S60Rel2.6
*/
class MDRMEventObserver
{
public: // New functions
    /**
    * HandleNotificationL
    *
    * Callback function for the event notifications
    * The MDRMEvent object ownership stays with the notifier,
    * do not delete it
    *
    * @param aEvent : an object of the type MDRMEvent
    * @since S60Rel2.6
    */
   virtual void HandleEventL( MDRMEvent* aEvent ) = 0;
};

#endif      // MDRMEVENTOBSERVER_H

// End of File
