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
* Description:  Interface class for WMDRM DLA UI
*
*/


#ifndef C_WMDRMDLAUI_H
#define C_WMDRMDLAUI_H

#include <e32base.h>

class CCoeEnv;
class CAknGlobalNote;
class MDesCArray;
class CGulIcon;

/**
 * Interface for wait note callback.
 */
class MWmDrmDlaWaitNoteCallback
    {
    public:
        
        /**
         * Called if user cancels the wait note
         */
        virtual void UserWaitNoteCancellation() = 0;

    };

/**
 *  WMDRM DLA UI Interface
 *
 *  @lib wmdrmdlautils.lib
 *  @since S60 v9.1
 */
NONSHARABLE_CLASS( CWmDrmDlaUi ) : public CActive
    {

    public:

        /**
         * This class must have an access to CCoeEnv,
         * otherwise it will leave with KErrNotSupported
         */
        IMPORT_C static CWmDrmDlaUi* NewL();
        IMPORT_C static CWmDrmDlaUi* NewLC();

        /**
         * Destructor.
         */
        virtual ~CWmDrmDlaUi();

        /**
         * Shows error note
         *
         * @param aTextResourceId Resource id of the text to be shown
         * @leave with standard Symbian error code in case of error
         */
        IMPORT_C void ShowErrorNoteL( TInt aTextResourceId );
        
        /**
         * Shows wait note
         *
         * @param aTextResourceId Resource id of the text to be shown
         * @param aCallback Callback that will be called, 
         *                  if user cancels the wait note. Can be NULL.
         * @leave with KErrInUse, if wait note is already shown, otherwise
         *        standard Symbian error code in case of error.
         */
        IMPORT_C void ShowWaitNoteL( TInt aTextResourceId, 
                                     MWmDrmDlaWaitNoteCallback* aCallback );
        
        /**
         * Removes wait note
         */
        IMPORT_C void RemoveWaitNote();
        
        /**
         * Shows list query
         *
         * @param aItems Item array to be shown, 
         *               ownership is transferred to the list query.
         * @param aIcons Icon array to be shown, 
         *               ownership is transferred to the list query.
         * @return Index of the user selection or -1, if user cancelled query.
         * @leave with KErrArgument, if any parameter is NULL, otherwise
         *        standard Symbian error code in case of error.
         */
        IMPORT_C TInt ShowListQueryL( MDesCArray* aItems, 
                                      CArrayPtr<CGulIcon>* aIcons );
        
    protected: // from base class CActive

        /**
        * From CActive Cancels async request.
        */
        void DoCancel();

        /**
        * From CActive Called when async request completes.
        */
        void RunL();
        
        /**
        * From CActive Called if RunL leaves.
        */
        TInt RunError( TInt aError );    
        
    private:

        CWmDrmDlaUi();
        void ConstructL();
        void CancelWaitNote();

    private: // data

        //Control environment, not owned
        CCoeEnv* iCoeEnv;
    
        // Path of the resource file
        HBufC* iWmdrmDlaUtilsResourceFile;

        // Resource offsets in CoeEnv
        TInt iWmdrmDlaUtilsResourceFileOffset;
        
        CAknGlobalNote* iGlobalNote;
        TInt iGlobalNoteId;
        HBufC* iGlobalNoteText;
        
        //Not owned
        MWmDrmDlaWaitNoteCallback* iCallback;
   
    };

#endif // C_WMDRMDLAUI_H
