/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Store for parent index of Decision Making Machine
*
*/


#ifndef DRMPARENTSTORAGE_H
#define DRMPARENTSTORAGE_H

// INCLUDES
#include <badesca.h> // CDesC8ArraySeg
#include "drmenginetypedefs.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FORWARD DECLARATIONS

// TYPE DEFINITIONS

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
*  CDRMParentStorage is a container for parent permissions for Decision 
*  Making Machine.
*
*  @lib RightsServer.exe
*  @since 3.0
*/
NONSHARABLE_CLASS( CDRMParentStorage ) : public CBase
    {
    public: // Constructors and destructor
        /**
        * NewLC
        *
        * Two-phase constructor.
        *
        * @since    3.0
        * @return   A pointer to newly created object.
        */
        static CDRMParentStorage* NewLC();
        
        /**
        * NewLC
        *
        * Two-phase constructor.
        *
        * @since    3.0
        * @return   A pointer to newly created object.
        */
        static CDRMParentStorage* NewL();
        
        /**
        * Destructor.
        * @since Sereis 60 Release 3.0
        */        
        ~CDRMParentStorage();
        
        /**
        * Creates a new list into the store.
        * @since Sereis 60 Release 3.0
        * @return A reference to a newly created list.
        */
        CDRMPermissionList& NewListL();
        
        /**
        * Adds the id to the list. Assumes the list has been retrieved
        * earlier with NewListL(), and the list has been filled properly.
        * @since Sereis 60 Release 3.0
        * @param aCID Content-ID
        */
        void AddL( const TDesC8& aCID );
        
        /**
        * Checks whether the id is marked as bad parent.
        * @since Sereis 60 Release 3.0
        * @param aCID Content-ID
        * @return boolean.
        */
        TBool Bad( const TDesC8& aCID );
        
        /**
        * Check if there are permissions for a given id
        * @since Sereis 60 Release 3.0
        * @param aCID The id to be found
        * @return List of permissions associated to the id.
        */
        TBool HasPermissions( const TDesC8& aCID );

        /**
        * Find operator. 
        * Assumes the id is in store, and will panic if there are no permissions.
        * @since Sereis 60 Release 3.0
        * @param aCID The id to be found
        * @return List of permissions associated to the id.
        */
        CDRMPermissionList& operator[]( const TDesC8& aCID );
        
    protected: 
        /**
        * Default constructor.
        * @since Sereis 60 Release 3.0
        */
        CDRMParentStorage();
        
        /*
        * 2nd phase constructor.
        * @since Sereis 60 Release 3.0
        */
        void ConstructL();
    private:
        // An array of parent IDs. Ordered list.
        CDesC8ArraySeg* iParentIDs;
        
        // An array of permission lists. In sync with iParentIDs.
        RPointerArray< CDRMPermissionList > iParents;
        
    };

#endif // DRMPARENTSTORAGE_H


// End of file
