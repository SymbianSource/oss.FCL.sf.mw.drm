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
* Description:  Interface to send notifications from http plugin ui plugin
*
*/


#ifndef C_WMDRMDLAUINOTIFIERIMPL_H
#define C_WMDRMDLAUINOTIFIERIMPL_H

#include <e32base.h>
#include <wmdrmdlatypes.h>
#include <wmdrmdlacancelobserver.h>

class CWmDrmDlaUiPluginIf;

/**
 *  WMDRM DLA UiNotifier Impl
 * 
 *  @lib wmdrmdla.lib
 *  @since S60 v9.1
 */
NONSHARABLE_CLASS( CWmDrmDlaUiNotifierImpl ) : public CBase
    {

public:

    static CWmDrmDlaUiNotifierImpl* NewL();
    static CWmDrmDlaUiNotifierImpl* NewLC();    

    /**
    * Destructor.
    */
    virtual ~CWmDrmDlaUiNotifierImpl();

    /**
     * Selects UI plugin based on UID
     *
     * @param aUid Uid of the plugin implementation
     * @leave KErrNotFound if corresponding plugin is not found
     */
    void InitializeL( TUid aUid );

    /**
     * Selects UI plugin based on data
     * If corresponding plugin is not be found, tries to initialize default
     * plugin
     *
     * @param aOpaqueData Opaque data of the plugin implementation
     * @leave KErrNotFound if corresponding and default plugin is not found
     */
    void InitializeL( TDesC8& aOpaqueData );

    /**
     * Called by the framework to set cancel observer. UI plugin can use this
     * to inform framework about user cancelling DLA process from UI component.
     * 
     * @param aObserver Cancellation observer
     */
    void SetCancelObserver( MWmDrmDlaCancelObserver* aObserver );
    
    /**
     * Informs that license acquisition is silent
     *
     * @param aSilent Is license acquisition silent
     */
    void SetSilent( TBool aSilent );

    /**
     * Passes progress information of license acquisition to UI
     *
     * @param aState License acquisition progress state
     * @see wmdrmdlatypes.h
     */
    void SetProgress( TInt aState );

    /**
     * Informs that error happened during the license acquisition
     *
     * @param aError Error code
     */
    void HandleErrorL( TInt aError );

protected:

private:

    CWmDrmDlaUiNotifierImpl();

    void ConstructL();

private: // data

    CWmDrmDlaUiPluginIf* iUiPlugin;

    };

#endif // C_WMDRMDLAUINOTIFIERIMPL_H
