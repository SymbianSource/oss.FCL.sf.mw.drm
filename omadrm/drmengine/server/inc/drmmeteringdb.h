/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for the Metering database
*
*/


#ifndef DRMMETERINGDB_H
#define DRMMETERINGDB_H

//  INCLUDES
#include <e32std.h>
#include <caf/caf.h>
#include <caf/cafplatform.h>
#include <d32dbms.h>
#include "DRMPointerArray.h"

// CONSTANTS
// MACROS
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CPersistentStore;
class CDrmMeteringDbData;

// DATA TYPES

// CLASS DECLARATION

/**
*  Content metering information database. This class manages storing
*  information of Content ID, Rights Issuer ID, metered count and metered
*  accumulated time.
*
*  @lib RightsServer.exe
*  @since 3.2
*/
NONSHARABLE_CLASS( RDrmMeteringDb )
    {

    public: // Constructors and destructor

        /**
        * Constructor.
        * @since S60 3.2
        */
        RDrmMeteringDb( RFs& aFs );

        /**
        * Default constructor.
        * @since S60 3.2
        */
        RDrmMeteringDb();

        public: // New functions

        /**
        * Sets the iFs according to this reference.
        *
        * @since S60 3.2
        * @param aFs Reference to open file server session.
        */
        void Set( RFs& aFs );

        /**
        * Closes the database.
        * @since S60 3.2
        */
        void Close();

        /**
        * Initializes the database.
        * @since S60 3.2
        * @param aFileName File name of the database
        */
        void InitL( const TDesC& aFileName );

        /**
        * Destructor.
        * @since S60 3.2
        */
        ~RDrmMeteringDb();

        /**
        * Add a metering data entry to the database.
        * @since S60 3.2
        * @param aMeteringData One metering data entry
        */
        void AddL( const CDrmMeteringDbData* aMeteringData );

        /**
        * Get the metering data list based on the Rights Issuer Id from the
        * metering database. Return value is ETrue if at least one entry was
        * found corresponding to the given Rights Issuer Id. Return value is
        * EFalse if no entry was found. The function will leave if an error
        * happens in the handling of the database or if the Rights Issuer Id
        * is empty or too long.
        * @since S60 3.2
        * @param aRiId Rights Issuer Id
        * @param aMeteringDataList Metering data list for a Rights Issuer
        * @return Boolean
        */
        TBool GetL( const TDesC8& aRiId,
                    CDRMPointerArray< CDrmMeteringDbData >& aMeteringDataList );

        /**
        * Delete all the metering data information associated to the given
        * Rights Issuer Id from the metering database. Return value is ETrue
        * if at least one entry was found corresponding to the given Rights
        * Issuer Id. Return value is EFalse if no entry was found. The function
        * will leave if an error happens in the handling of the database or if
        * the Rights Issuer Id is empty or too long.
        * @since S60 3.2
        * @param aRiId Rights Issuer Id
        * @return Boolean
        */
        TBool DeleteL( const TDesC8& aRiId );

    private: // Private functions

        /**
        * Opens a database.
        * @since S60 3.2
        * @param aDb Database.
        * @param aFileName File name.
        */
        void OpenDbL( RDbNamedDatabase& aDb,
                      const TDesC& aFileName );

        /**
        * Replace an existing database.
        * @since S60 3.2
        * @param aDb Database.
        * @param aFileName File name.
        */
        void ReplaceDbL( RDbNamedDatabase& aDb,
                         const TDesC& aFileName );

        /**
        * Initializes the view.
        * @since S60 3.2
        * @param aView View to initialize.
        */
        void InitViewLC( RDbView& aView );

        /**
        * Compare the Rights Issuer Id and Content Id to their counterparts in
        * the current row of the view. Return value is ETrue only if both the
        * Content Id and the Rights Issuer Id match their counterpart Ids in
        * the view. Overloaded.
        *
        * @since S60 3.2
        * @param aView View.
        * @param aCID Content Id
        * @param aRiId Rights Issuer Id
        * @return Boolean
        */
        TBool CompareIDL( RDbRowSet& aView,
                          const TDesC8& aCID,
                          const TDesC8& aRiId );

        /**
        * Compare the Rights Issuer Id to its counterpart in the current row
        * of the view. Overloaded.
        *
        * @since S60 3.2
        * @param aView View.
        * @param aRiId Rights Issuer Id
        * @return Boolean
        */
        TBool CompareIDL( RDbRowSet& aView, const TDesC8& aRiId );

        /**
        * Push a cleanup item to the cleanup stack in order to rollback
        * the database.
        * @since S60 3.2
        * @param aDb Database.
        */
        void PushL( RDbDatabase& aDb );

        /**
        * Pop the cleanup item pushed in by PushL
        * @since S60 3.2
        */
        void Pop();

    private: // Data
        // File server session handle. Not owned by this object.
        RFs* iFs;

        // Metering database
        RDbNamedDatabase iDb;

    };

#endif //DRMMETERINGDB_H