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
* Description:  WMDRM DLA Default UI Plugin
*
*/


#ifndef C_WMDRMDLADEFAULTUIPLUGIN_H
#define C_WMDRMDLADEFAULTUIPLUGIN_H

#include <wmdrmdlauiplugin.h>
#include "wmdrmdlaui.h"
 
/**
 * WMDRM DLA Default UI Plugin
 * Shows wait note, when SetProgress is set to EProcessingDRMHeader or 
 * to EProcessingMeteringChallenge. Removes wait note, if HandleErrorL is
 * called or SetProgress is set to EIdle. Won't handle any errors, but
 * always leaves with KErrNotSupported. Supports user cancellation from
 * wait note. If CCoeEnv isn't available, then wait note can't be shown,
 * but object creation still succeeds.
 *
 *  @lib wmdrmdladefaultuiplugin.lib
 *  @since S60 v9.1
 */
NONSHARABLE_CLASS( CWmDrmDlaDefaultUiPlugin ) : public CWmDrmDlaUiPluginIf, 
                                                public MWmDrmDlaWaitNoteCallback
    {

public:

    static CWmDrmDlaDefaultUiPlugin* NewL();
    ~CWmDrmDlaDefaultUiPlugin();

public: //From CWmDrmDlaUiPluginIf
    
    /**
     * Called by the framework to set cancel observer. UI plugin can use 
     * observer to inform framework about user cancelling DLA process from
     * UI component.
     * 
     * @param aObserver Cancellation observer
     */
    void SetCancelObserver( MWmDrmDlaCancelObserver* aObserver );
    
    /**
     * Called to inform UI plugin if the license acquisition is silent
     *
     * @param aSilent ETrue if license acquisition should be silent
     */
    void SetSilent( TBool aSilent );

    /**
     * Called to inform UI plugin about the license acquisition progress
     *
     * @param aState License acquisition progress state
     */
    void SetProgress( TInt aState );

    /**
     * Informs that error happened during the license acquisition
     *
     * @param aError Error code
     */
    void HandleErrorL( TInt aError );

public: //From MWmDrmDlaWaitNoteCallback    
    
    /**
     * Called if user cancels the wait note
     */
    void UserWaitNoteCancellation();
    
private:

    CWmDrmDlaDefaultUiPlugin();
    void ConstructL();
    
private: // data

    CWmDrmDlaUi* iUi;
    
    //Not owned
    MWmDrmDlaCancelObserver* iObserver;
    
    };

#endif // C_WMDRMDLADEFAULTUIPLUGIN_H
