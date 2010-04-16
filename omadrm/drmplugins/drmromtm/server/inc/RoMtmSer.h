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
* Description:  Declaration of Rights Object Server side MTM
*
*/


#ifndef RIGHTSOBJECTSERVERMTM_H
#define RIGHTSOBJECTSERVERMTM_H

//  INCLUDES
#include <e32base.h>
#include <mtclbase.h>
#include <mtsr.h>
#include <msvstore.h>
#include <msvuids.h>
#include <msvids.h>



// CONSTANTS

// MACROS
// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*   CRightsObjectServerMtm
*
*  @lib RoMtmSer.lib
*  @since 2.5
*/


//
//  CRightsObjectServerMtm: The Server-side MTM implementation
//
class CRightsObjectServerMtm : public CBaseServerMtm
    {
    public:



    // Constructor, initialisation, and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CRightsObjectServerMtm* NewL( CRegisteredMtmDll& aRegisteredMtmDll , CMsvServerEntry* aInitialEntry );

        /**
        * Destructor.
        */
        ~CRightsObjectServerMtm();

        /*************************************
        *
        *       from CBaseMtm
        *
        **************************************/
        // Copy and move

        /**
        * CopyToLocalL
        * @since 2.5
        *
        * Not Supported
        *
        */
        void CopyToLocalL( const CMsvEntrySelection& aSelection , TMsvId aDestination , TRequestStatus& aStatus );

        /**
        * CopyFromLocalL
        * @since 2.5
        *
        * Not Supported
        *
        */
        void CopyFromLocalL( const CMsvEntrySelection& aSelection , TMsvId aDestination , TRequestStatus& aStatus );

        /**
        * CopyToLocalL
        * @since 2.5
        *
        * Not Supported
        *
        */
        void CopyWithinServiceL( const CMsvEntrySelection& aSelection , TMsvId aDestination , TRequestStatus& aStatus );

        /**
        * MoveToLocalL
        * @since 2.5
        *
        * Not Supported
        *
        */
        void MoveToLocalL( const CMsvEntrySelection& aSelection , TMsvId aDestination , TRequestStatus& aStatus );

        /**
        * MoveFromLocalL
        * @since 2.5
        *
        * Not Supported
        *
        */
        void MoveFromLocalL( const CMsvEntrySelection& aSelection , TMsvId aDestination , TRequestStatus& aStatus );

        /**
        * MoveWithinServiceL
        * @since 2.5
        *
        * Not Supported
        *
        */
        void MoveWithinServiceL( const CMsvEntrySelection& aSelection , TMsvId aDestination , TRequestStatus& aStatus );

        // Create, change, delete

        /**
        * DeleteAllL
        * @since 2.5
        *
        * Not Supported
        *
        */
        void DeleteAllL( const CMsvEntrySelection& aSelection , TRequestStatus& aStatus );

        /**
        * CreateL
        * @since 2.5
        *
        * Not Supported
        *
        */
        void CreateL( TMsvEntry aNewEntry , TRequestStatus& aStatus );

        /**
        * ChangeL
        * @since 2.5
        *
        * Not Supported
        *
        */
        void ChangeL( TMsvEntry aNewEntry , TRequestStatus& aStatus );

        // Command and progress

        /**
        * StartCommandL
        * @since 2.5
        *
        * Not Supported
        *
        */
        void StartCommandL( CMsvEntrySelection& aSelection , TInt aCommand ,
            const TDesC8& aParameter , TRequestStatus& aStatus );

        /**
        * CommandExpected
        * @since 2.5
        *
        * Not Supported
        *
        */
        TBool CommandExpected();


        /**
        * Progress
        * @since 2.5
        *
        * Not Supported
        *
        */
        const TDesC8& Progress();

        // void RunL()
        // void SetInitialEntry(CMsvServerEntry* aEntry)=0;
    protected:

        /*************************************
        *
        *       from CActive
        *
        **************************************/
        void DoCancel();
        /*************************************
        *
        *       from CBaseMtm
        *
        **************************************/
        void DoRunL();
        void DoComplete(TInt aError);

    private:
        CRightsObjectServerMtm( CRegisteredMtmDll& aRegisteredMtmDll , CMsvServerEntry* aInitialEntry );
        void ConstructL();

    private:
    };

#endif      // RIGHTSOBJECTSERVERMTM_H

// End of File
