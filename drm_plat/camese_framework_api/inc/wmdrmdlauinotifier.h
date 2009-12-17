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


#ifndef C_WMDRMDLAUINOTIFIER_H
#define C_WMDRMDLAUINOTIFIER_H

#include <e32base.h>

class MWmDrmDlaCancelObserver;
class CWmDrmDlaUiNotifierImpl;

/**
 *  WMDRM DLA UiNotifier
 *
 *  @lib wmdrmdla.lib
 *  @since S60 v9.1
 */
NONSHARABLE_CLASS( CWmDrmDlaUiNotifier ) : public CBase
    {

public:

    IMPORT_C static CWmDrmDlaUiNotifier* NewL();
    IMPORT_C static CWmDrmDlaUiNotifier* NewLC();    

    /**
    * Destructor.
    */
    virtual ~CWmDrmDlaUiNotifier();

    /**
     * Selects UI plugin based on UID
     *
     * @param aUid Uid of the plugin implementation
     * @leave KErrNotFound if corresponding plugin is not found
     */
    IMPORT_C void InitializeL( TUid aUid );

    /**
     * Selects UI plugin based on data
     * If corresponding plugin is not be found, tries to initialize default
     * plugin
     *
     * @param aOpaqueData Opaque data of the plugin implementation
     * @leave KErrNotFound if corresponding and default plugin is not found
     */
    IMPORT_C void InitializeL( TDesC8& aOpaqueData );

    /**
     * Called by the framework to set cancel observer. UI plugin can use this
     * to inform framework about user cancelling DLA process from UI component.
     * 
     * @param aObserver Cancellation observer
     */
    IMPORT_C void SetCancelObserver( MWmDrmDlaCancelObserver* aObserver );
    
    /**
     * Informs that license acquisition is silent
     *
     * @param aSilent Is license acquisition silent
     */
    IMPORT_C void SetSilent( TBool aSilent );

    /**
     * Passes progress information to UI plugin
     *
     * @param aState License acquisition progress state
     * @see wmdrmdlatypes.h
     */
    IMPORT_C void SetProgress( TInt aState );

    /**
     * Informs that error happened during the license acquisition
     *
     * @param aError Error code
     */
    IMPORT_C void HandleErrorL( TInt aError );

protected:

private:

    CWmDrmDlaUiNotifier();

    void ConstructL();

private: // data

    /**
     * Implementation class
     * Own.
     */
    CWmDrmDlaUiNotifierImpl* iImpl;

    };

#endif // C_WMDRMDLAUINOTIFIER_H
