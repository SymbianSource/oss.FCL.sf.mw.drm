/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for the DRM Rights database
*
*/


#ifndef DRMOBSOLETEFINDER_H
#define DRMOBSOLETEFINDER_H

// INCLUDES

#include <e32base.h>
#include "DRMPointerArray.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FORWARD DECLARATIONS
class RFs;
class CDir;
class CDRMRightsDB;
class CDcfRep;


// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
*  CDRMObsoleteFinder implements expired rights cleanup
*  for drm rights database
*
*  @lib RightsServer.exe
*  @since 3.0
*/

NONSHARABLE_CLASS( CDRMObsoleteFinder ) : public CActive
    {
    public: // Constructors and destructor

        /**
        * NewL
        *
        * Creates an instance of the CDRMObsoleteFinder class and returns a pointer
        * to it
        *
        * @since    3.0
        * @param    aFs : Open file server session
        * @param    aDatabase : CDRMRightsDB object
        * @param    aStatus : The request status to complete when the operation is
        *                     fully done
        * @param    aStream : stream to write the result into
        * @param    aPerformScan : if a filesystem scan need to be performed
        *
        * @return   Functional CDRMObsoleteFinder object, Function leaves if an error
        *           occurs.
        */
        static CDRMObsoleteFinder* NewL( RFs& aFs,
                                        CDRMRightsDB* aDatabase,
                                        TRequestStatus& aStatus,
                                        RWriteStream& aStream,
                                        TBool aPerformScan );

        /**
        * Destructor
        */
        virtual ~CDRMObsoleteFinder();

    public: // New functions

        /**
        * ExecuteCleanupLD
        *
        * Delete expired permissions. The object is deleted when
        * the function completes
        *
        * @since    3.0
        * @return   None
        *
        */
        void ExecuteFinderLD();


        void DoCleanup();

    protected:
        /**
        * From CActive: RunL.
        */
        void RunL();


        /**
        * From CActive: DoCancel performs cancel
        */
        void DoCancel();

        /**
        * From CActive: RunError checks the errors from RunL.
        */
        TInt RunError( TInt aError );

    private:
        /**
        * Default Constructor - First phase. Prevented.
        */
        CDRMObsoleteFinder();

        /**
        * Default Constructor - First phase.
        */
        CDRMObsoleteFinder( RFs& aFs,
                            CDRMRightsDB* aDatabase,
                            TRequestStatus& aStatus,
                            RWriteStream& aStream );

        /**
        * ConstructL
        *
        * Second phase constructor
        *
        * @since  3.0
        * @return Leaves if an error occurs
        */
        void ConstructL( const TBool aPerformScan );

        /**
        * Assignment operator - Prevented
        */
        CDRMObsoleteFinder& operator =( const CDRMObsoleteFinder& );

        /**
        * Copy constructor - Prevented
        */
        CDRMObsoleteFinder( const CDRMObsoleteFinder& );


        /**
        * Get possible parents the content if there are any
        *
        * @since    3.0
        * @param    aContentId : identifier of the content whom to check for
        *                        parents
        * @param    aParents : pointer array of parents the new ones will be
        *                      inserted
        * @return none
        */
        void GetParentsL( const TDesC8& aContentId,
                          RPointerArray<HBufC8>& aParents );

        /**
        * Write the Obsolete contents to the stream
        *
        * @since    3.0
        * @return   none
        * @leave    Symbian OS error code
        */
        void ObsoleteToStreamL();

    private:
        // Dcf repository client
        CDcfRep* iDcfClient;

        // List of content id's
        CDRMPointerArray<HBufC8>* iContents;

        // List of content id's
        CDRMPointerArray<HBufC8>* iParents;

        // List of content id's
        CDRMPointerArray<HBufC8>* iNoContents;

        // Index we are going at:
        TInt iIndex;

        // fileserver session
        RFs& iFileServer;

        // output stream
        RWriteStream& iStream;

        // rights database file, fileserver subsession
        CDRMRightsDB* iRightsDb;

        // request status to be set complete
        TRequestStatus& iOperationStatus;

        // State of the active object
        TInt iState;

        // Cancelling status
        TInt iCancel;
    };

#endif      // DRMOBSOLETEFINDER_H

// End of File
