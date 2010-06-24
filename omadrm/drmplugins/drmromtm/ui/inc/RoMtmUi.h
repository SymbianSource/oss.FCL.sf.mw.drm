/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of Rights Object MTM UI
*
*/


#ifndef RIGHTSOBJECTMTMUI_H
#define RIGHTSOBJECTMTMUI_H

//  INCLUDES
#include <e32base.h>
#include <mtmuibas.h>
#include <AknServerApp.h>

// CONSTANTS
// MACRightsObjectS
// DATA TYPES
enum TMessageType
    {
    ERo, //open RO manager embedded
    ERoapTrigger, //open roap with file name
    ERoapTriggerRoAcquisition // remove prefix and open roap with file name.
    };
// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAknLaunchAppService;

class CDocumentHandler;


// CLASS DECLARATION
/**
*   CRightsObjectMtmUi
*
*  @lib romtmui.lib
*  @since 2.5
*/
//
// CRightsObjectMtmUi: User Interface MTM
//
class CRightsObjectMtmUi : public CBaseMtmUi , public MAknServerAppExitObserver , public MApaServerAppExitObserver
    {
    public:
        // Construction
        /**
        * Two-phased constructor.
        */
        static CRightsObjectMtmUi* NewL( CBaseMtm& aBaseMtm , CRegisteredMtmDll& aRegisteredMtmDll );

        // Destruction

        /**
        * Destructor.
        */
        ~CRightsObjectMtmUi();

        /*************************************
        *
        *       from CBaseMtmUi
        *
        **************************************/


        /**
        * CancelL.
        * @since 2.5
        *
        * Not Supported
        *
        */
        CMsvOperation* CancelL( TRequestStatus& aStatus, const CMsvEntrySelection& aSelection );

        // Context-specific
        /**
        * OpenL.
        * @since 2.5
        *
        * @param aStatus The request status to be completed when the operation has finished
        * @return If successful, this is an asynchronously completing open operation.
        *         If failed, this is a completed operation, with status set to the relevant error code.
        */
        CMsvOperation* OpenL( TRequestStatus& aStatus );

        /**
        * OpenL.
        * @since 2.5
        *
        * @param aStatus The request status to be completed when the operation has finished
        * @param aSelection Selection of message entries
        * @return If successful, this is an asynchronously completing open operation.
        *         If failed, this is a completed operation, with status set to the relevant error code.
        */
        CMsvOperation* OpenL( TRequestStatus& aStatus , const CMsvEntrySelection& aSelection );

        /**
        * CloseL.
        * @since 2.5
        *
        * Not Supported
        *
        */
        CMsvOperation* CloseL( TRequestStatus& aStatus );

        /**
        * CloseL.
        * @since 2.5
        *
        * Not Supported
        *
        */
        CMsvOperation* CloseL( TRequestStatus& aStatus , const CMsvEntrySelection& aSelection );

        // Launches editor/settings dialog as appropriate
        /**
        * EditL.
        * @since 2.5
        *
        * Not Supported
        *
        */
        CMsvOperation* EditL( TRequestStatus& aStatus );

        /**
        * EditL.
        * @since 2.5
        *
        * Not Supported
        *
        */
        CMsvOperation* EditL( TRequestStatus& aStatus , const CMsvEntrySelection& aSelection ) ;

        // Launches viewer/settings dialog as appropriate
        /**
        * ViewL.
        * @since 2.5
        *
        * @param aStatus The request status to be completed when the operation has finished
        * @return If successful, this is an asynchronously completing open operation.
        *         If failed, this is a completed operation, with status set to the relevant error code.
        */
        CMsvOperation* ViewL( TRequestStatus& aStatus );

        /**
        * ViewL.
        * @since 2.5
        *
        * @param aStatus The request status to be completed when the operation has finished
        * @param aSelection Selection of message entries
        * @return If successful, this is an asynchronously completing open operation.
        *         If failed, this is a completed operation, with status set to the relevant error code.
        */
        CMsvOperation* ViewL( TRequestStatus& aStatus , const CMsvEntrySelection& aSelection );

        // Reply/forward
        /**
        * ReplyL.
        * @since 2.5
        *
        * Not Supported
        *
        */
        CMsvOperation* ReplyL( TMsvId aDestination , TMsvPartList aPartList , TRequestStatus& aCompletionStatus );

        /**
        * ForwardL.
        * @since 2.5
        *
        * Not Supported
        *
        */
        CMsvOperation* ForwardL( TMsvId aDestination , TMsvPartList aPartList , TRequestStatus& aCompletionStatus );


    public:
        void HandleServerAppExit(TInt aReason);


    protected:
        /*************************************
        *
        *       from CBaseMtm
        *
        **************************************/
        void GetResourceFileName( TFileName& aFileName ) const;

    protected:
        CRightsObjectMtmUi( CBaseMtm& aBaseMtm , CRegisteredMtmDll& aRegisteredMtmDll );
        void ConstructL();

    private:
        void CheckMessageTypeL();
        CMsvOperation* LaunchEditorApplicationL( TRequestStatus& aStatus , TMsvId aId , TBool aReadOnly );
        void SetProgressSuccess( TPckgBuf<TMsvLocalOperationProgress>& aProgress , TMsvId aId );
        CMsvOperation* CompletedOperationL( TRequestStatus& aObserverStatus );
        TBool ShowQueryL( TDesC16& aFile );
        TInt CheckTriggerTypeL( TDesC16& aFile );

    private:
        TMessageType iType;
        RFs iFs;
        CEikProcess* iHostProcess;
    
    };

#endif      // RIGHTSOBJECTMTMUI_H

// End of File
