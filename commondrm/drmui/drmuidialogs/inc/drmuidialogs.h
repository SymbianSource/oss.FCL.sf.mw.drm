/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CDrmUIDialogs API can be used to show UI dialogs via 
* notification dialog plugin.
*/


#ifndef C_DRMUIDIALOGS_H
#define C_DRMUIDIALOGS_H

#include <e32base.h>                            // CActive
#include <hb/hbcore/hbdevicedialogsymbian.h>    // MHbDeviceDialogObserver

class CHbSymbianVariantMap;
class CActiveSchedulerWait;

/**
 * CDrmUIDialogs manages and controls DRM UI notes and queries. 
 * An Orbit device dialog plugin implements the UI dialogs.
 *
 * @dll drmuidialogs.dll
 * @since 10.1
 */
NONSHARABLE_CLASS( CDrmUIDialogs ) : public CActive,
                                     public MHbDeviceDialogObserver
    {
    public:  // constructor and destructor
        /**
         * Creates new CDrmUIDialogs object and pushes it into cleanup stack.
         * @returns CDrmUIDialogs* -- new CDrmUIDialogs object
         */
        IMPORT_C static CDrmUIDialogs* NewLC();

        /**
         * Creates new CDrmUIDialogs object.
         * @returns CDrmUIDialogs* -- new CDrmUIDialogs object
         */
        IMPORT_C static CDrmUIDialogs* NewL();

        /**
         * Destructor.
         */
        CDrmUIDialogs::~CDrmUIDialogs();

    public:
        /**
        * Displays a note or query dialog synchronously. Synchronous function.
        * Returns after the user has accepted or cancelled the query.
        * @param aDialogId  dialog id of dialog to be displayed
        * @param aString    string that replaces %U in resource string
        * @param aValue     integer that replaces %N in resource string
        * @return EOk if user accepted the query, ECancelled otherwise
        */
        IMPORT_C TInt ShowNoteL( TInt aDialogId, 
                                 const TDesC& aString = KNullDesC, 
                                 TInt aValue = -1 );

    protected:  // from CActive
        void DoCancel();
        void RunL();

    private:    // from MHbDeviceDialogObserver
        void DataReceived( CHbSymbianVariantMap& aData );
        void DeviceDialogClosed( TInt aCompletionCode );

    private:
        CDrmUIDialogs();
        void ConstructL();
        void ClearParamsL();
        void ClearParamsAndSetDialogIdL( TInt aDialogId );
        void AddParamL( const TDesC& aKey, TInt aValue );
        void AddParamL( const TDesC& aKey, const TDesC& aValue );
        void DisplayDeviceDialogL();
        TInt WaitUntilDeviceDialogClosed();

    private:    // data
        CHbDeviceDialogSymbian* iDeviceDialog; // own
        CHbSymbianVariantMap* iVariantMap; // own
        CActiveSchedulerWait* iWait; // own
        TBool iIsDisplayingDialog;
        TInt iCompletionCode;
        TInt iReturnValue;
    };

#endif  // C_DRMUIDIALOGS_H
