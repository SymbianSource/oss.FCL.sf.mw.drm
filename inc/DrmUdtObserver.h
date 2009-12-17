/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#ifndef DRM_UDT_OBSERVER_H
#define DRM_UDT_OBSERVER_H

// INCLUDES
// CLASS DECLARATION

class TUdtStateInfo
    {
    public:
    
    enum TUdtState
        {
        EUdtNotStarted,
        EUdtReguest,
        EUdtStatusNotification,
        EUdtRegistration,
        EUdtDeleteConfirmation,
        EUdtKeyRestore,
        EUdtComplete
        };
        
    TUdtState iState;
    
    TInt iProgress; // 0 - 100
    
    TInt iError;
    };

class MDrmUdtObserver
    {
    public:
    
    /**
    * ConnectionStartedL
    * 
    * The function is called by DrmUdtModule when a network connection is to
    * be established.
    * 
    * @since  3.0
    *
    * @leave  System wide error code */
    virtual void ConnectionStartedL() = 0;

    /**
    * UdtProgressInfoL
    * 
    * The function is called by DrmUdtModule to provide UDT state information to the caller
    * 
    * @since  3.0
    *
    * @leave  System wide error code */
    virtual void UdtProgressInfoL( TUdtStateInfo& aProgress ) = 0;
    
    };
    
#endif /* DRM_UDT_OBSERVER_H */
