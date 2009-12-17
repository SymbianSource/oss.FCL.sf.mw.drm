/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for UI plugins
*
*/


#ifndef C_WMDRMDLAUIPLUGINIF_H
#define C_WMDRMDLAUIPLUGINIF_H

#include <e32base.h>

class MWmDrmDlaCancelObserver;

/**
 *  Interface which all WMDRM DLA UI Plugins have to implement
 *
 *  @since S60 v9.1
 */
class CWmDrmDlaUiPluginIf : public CBase
    {

public:

    // Instantiates an object of this type 
    inline static CWmDrmDlaUiPluginIf* NewL( TUid aUid );

    // Destructor.
    inline virtual ~CWmDrmDlaUiPluginIf();

    /**
     * Called by the framework to set cancel observer. UI plugin can use
     * observer to inform framework about user cancelling DLA process from
     * UI component.
     * 
     * @param aObserver Cancellation observer
     */
    virtual void SetCancelObserver( MWmDrmDlaCancelObserver* aObserver ) = 0;
    
    /**
     * Called to inform UI plugin if the license acquisition is silent
     *
     * @param aSilent ETrue if license acquisition should be silent
     */
    virtual void SetSilent( TBool aSilent ) = 0;

    /**
     * Called to inform UI plugin about the progress
     *
     * @param aState License acquisition progress state
     * @see wmdrmdlatypes.h
     */
    virtual void SetProgress( TInt aState ) = 0;

    /**
     * Informs that error happened during the license acquisition.
     * If HandleErrorL leaves with KErrNotSupported the framework handles the
     * error code.
     * If HandleErrorL leaves with any other standard Symbian error code than
     * KErrNotSupported the framework handles the leave code.
     *
     * @param aError Error code
     * @leave KErrNotSupported if plugin does not handle the error code
     */
    virtual void HandleErrorL( TInt aError ) = 0;

private: // data

    TUid iDlaUiPluginUid;

    };

#include <wmdrmdlauiplugin.inl>

#endif // C_WMDRMDLAUIPLUGINIF_H
