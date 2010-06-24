/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DRM3 Engine manages all DRM related database operations.
*
*/


#ifndef CDRMRIGHTSSERVER_H
#define CDRMRIGHTSSERVER_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <connect/abclient.h>
#include "drmcommonclientserver.h"
#include "DRMReplayCache.h"
#include "drmmeteringdb.h"
#include "DRMClockClient.h"
#include "dbwatcher.h"
#include "procwatcher.h"
#include "watcherobserver.h"

// FORWARD DECLARATIONS
class CDRMRightsDB;
class CDRMNotifier;
class CDRMClock;
class CDRMXOma;
class CDRMBackupObserver;
class CDRMBackup;
class RMobilePhone;

// CLASS DECLARATIONS

// Data Class:
NONSHARABLE_CLASS( CUsageUrl ) : public CBase
    {
    public:
        CUsageUrl();
        virtual ~CUsageUrl();   
                
    public: // Data    
        HBufC8* iUrl;
        TInt    iRefCounter;        
    };     


/**
*  This class implements the DRM5 Rights Server functionality.
*
*  @lib RightsServer.exe
*  @since S60Rel2.5
*/
NONSHARABLE_CLASS( CDRMRightsServer ) : public CServer2, public MWatcherObserver
    {    
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @since S60Rel2.5
         * @return Pointer to newly created server instance.
         */
        static CDRMRightsServer* NewL();

        /**
         * Destructor.
         * @since S60Rel2.5
         */
        ~CDRMRightsServer();

    public: // New functions

        /**
        * This method returns a handle to DRM Notifier.
        * @since S60Rel2.6
        * @return Handle to DRM Notifier.
        */
        CDRMNotifier& Notifier();

        /**
        * This method starts the ROAP storage server
        *
        * @since S60Rel3.0
        */
        void StartRoapStorageL();

        /**
        * This method returns the handle to the rights database.
        *
        * @since S60Rel2.6
        * @return Handle to the rights database.
        */
        CDRMRightsDB& Database();

        RFs& FileServerSession();

        /**
        * This method returns the handle to the metering database.
        *
        * @since S60 3.2
        * @return Handle to the metering database.
        */
        RDrmMeteringDb& MeteringDatabase();

        RDRMReplayCache& ReplayCache();

        /**
        * List of the contents which may have rights objects on the way
        *
        * @since 2.6
        * @return
        */
        RPointerArray<CDRMXOma>& XOmaHeaders( void );

        /**
        * This function is used to get the secure time.
        *
        * @since S60Rel2.5
        * @param aTime Out-parameter where the time is stored.
        * @return Truth value: Is the time a secure time or not.
        */
        TBool GetSecureTime( TTime& aTime ) const;

        /**
         *
         */
        const TDesC& GetIMEIL();

        /**
         *
         */
        const CDRMPointerArray<HBufC8>& GetIMSIL();

        /**
        * This function is used to handle backup restore events for
        * the DRM system
        */
        void HandleBackupEventL( TInt aBackupEvent );

        /**
        * Checks if a content ID is in the list of currently consumed contents
        * with count constraints
        *
        * @since 3.0
        * @param aContentId content ID to search for
        * @return ETrue if the content ID is in the list
        */
        TBool HasActiveCountConstraint( const TDesC8& aContentId );

        /**
        * Removes a content ID from the list of currently consumed contents
        * with count constraints
        *
        * @since 3.0
        * @param aContentId content ID to remove
        */
        void RemoveActiveCountConstraint( const TDesC8& aContentId );

        /**
        * Adds a content ID to the list of currently consumed contents
        * with count constraints
        *
        * @since 3.0
        * @param aContentId content ID to add
        */
        void AddActiveCountConstraintL( const TDesC8& aContentId );


        /**
        * Checks if a content ID is in the list of currently consumed contents
        *
        * @since 5.2
        * @param aContentId content ID to search for
        * @return Index or the Url if the content ID is in the list
        *         KErrNotFound if the content ID is not in the list
        */
        TInt IsAccessingUrl( const TDesC8& aContentId );

        /**
        * Removes a content ID from the list of currently consumed contents
        *
        * @since 5.2
        * @param aContentId content ID to remove
        */
        void RemoveAccessingUrl( const TDesC8& aContentId );

        /**
        * Adds a content ID to the list of currently consumed contents
        *
        * @since 5.2
        * @param aContentId content ID to add
        */
        void AddAccessingUrlL( const TDesC8& aContentId );


        /**
        * Import rights objects on startup
        *
        * @since 3.0
        * @param aImportDir Directory to import from
        */
        void ImportRightsObjectsL( const TDesC& aImportDir );

        /**
         * Stop watching the DCF repository server and the rights DB
         * @since 3.1
         */
        void StopWatchingL();

    public: // Functions from base classes

        //class MBackupOperationObserver

        /**
         * HandleNotifyL
         * @since S60Rel2.5
         * @param aBackupOperationAttributes Event related attributes.
         */
        void HandleNotifyL(const TUid aUid, const TDesC& aKey,
            const TDesC& aValue);

        //class MWatcherObserver

        /**
         * Called when the RDB has been modified
         *
         * @since S60 3.0
         * @param aObject Changed directory
         */
        void WatchedObjectChangedL( const TDesC& aObject );

    protected:  // New functions

    protected:  // Functions from base classes

        //class CServer
        /**
         * RunError.
         * @since S60Rel2.5
         * @param aError Error code from RunL function.
         * @return An error is returned if RunError() cannot process the
         * error.
         */
        TInt RunError( TInt aError );

    private:
        /**
         * C++ default constructor.
         * @since S60Rel2.5
         */
        CDRMRightsServer();

        /**
         * By default Symbian 2nd phase constructor is private.
         * @since S60Rel2.5
         */
        void ConstructL();

        /**
         * This method fetches the database key and stores
         * it to the given descriptor.
         * @since S60Rel2.5
         * @param aKey Descriptor where the key is stored.
         */
        void GetDbKeyL( TDRMKey& aKey );

        /**
         * This method generates the actual key from key seed.
         * @since S60Rel2.5
         * @param aKeySeed Key seed.
         * @param aKey     The key produced.
         */
        void GenerateKeyL( HBufC*& aKeySeed,
                           TDRMKey& aKey ) const;


        void StartThreadL( const TDesC& aThreadName,
                           TThreadFunction aFunc,
                           RSemaphore& aSemaphore );


        void AppendExtendedIndividualConstraintsL(RMobilePhone* aMobilePhone = NULL);
            static void Release(TAny* aIndividualConstraintExtension);

    private: // Functions from base classes
        // Class CServer
        /**
         * NewSessionL
         * @since S60Rel2.5
         */
        CSession2* NewSessionL( const TVersion& aVersion,
                                const RMessage2& aMessage) const;

    public:     // Data

    protected:  // Data

    private:    // Data
        CDRMRightsDB* iDb;

        CDRMNotifier* iNotifier;

        RFs iFs;
        RDRMReplayCache iCache;
        RDrmMeteringDb iMeteringDb;
        RDRMClockClient iClock;
        RPointerArray<CDRMXOma>* iXOmaHeaders;
        RPointerArray<HBufC8> iActiveCountConstraints;
        RPointerArray<class CUsageUrl> iActiveUrls;

        HBufC* iIMEI;

        // Backup Observer
        CDRMBackupObserver* iBackupObserver;

        // BackupHandler
        CDRMBackup* iBackupHandler;

        // Backup client
        conn::CActiveBackupClient* iActiveBackupClient;

        // RDB Watcher
        CDbWatcher* iDbWatcher;

        // Peer process watcher
        CProcWatcher* iProcWatcher;

        // Watching causes a restart
        TBool iArmed;

        CDRMPointerArray<HBufC8>* iIMSI;
        TBool iGetImsi;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

};


#endif      // CDRMRIGHTSSERVER_H

// End of File
