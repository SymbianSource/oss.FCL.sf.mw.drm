/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Server declaration
*
*/



#ifndef CDCFREPSRV_H
#define CDCFREPSRV_H

//  INCLUDES
#include    <d32dbms.h>
#include <e32base.h>
#include "Pair.h"
#include "procwatcher.h"

// CONSTANTS
// MACROS
// DATA TYPES
enum TDcfRepSrvPanic
    {
    ESrvCreateServer = 1000,
    ECreateTrapCleanup
    };

enum TServerState
    {
    EStateIdle = 0,
    EStateSetTtid,
    EStateFullScan,
    EStateScan
    };
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CFileScan;
class COma2Dcf;
// CLASS DECLARATION

/**
*  Server implementation.
*  DCF Repository
*
*  @lib
*  @since Series 60 3.0
*/
class CDcfRepSrv : public CServer2, public MWatcherObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CDcfRepSrv* NewLC();

        /**
        * Destructor.
        */
        virtual ~CDcfRepSrv();

    public: // New functions

        /**
        * Startup starts the actual server after initializing
        * the cleanup stack and active scheduler.
        * @since S60Rel3.0
        * @return system error code if error happens
        */
        static TInt Startup(void);

        /**
        * AddFileL
        * @since Series 60 3.0
        * @param aFile file name
        * @KErrNotSupported if file is not protected file
        */
        void AddFileL( const TDesC& aFile );

        /**
        * ProcessFile
        * @since Series 60 3.0
        * @param aFile file name
        * @param aFlag ETrue if the file satisfy to criteria of scanning target
        * @return system error
        */
        TInt ProcessFile( const TDesC& aFile , TInt& aType );

        /**
        * State
        * @since Series 60 3.0
        * @return state of the server
        */
        TInt State() const;

        /**
        * SetState
        * @since Series 60 3.0
        * @param aMode server state
        * @return
        */
        void SetState(TServerState aMode);

        /**
        * CompleteScanning
        * @since Series 60 3.0
        * @param aRet return code
        */
        void CompleteScanning(TInt err);


        /**
        * SetMessage
        * @since Series 60 3.0
        * @param aMessage Message from client
        */
        RMessage2* SetMessageL(const RMessage2& aMessage);

        /**
        * Scan
        * @since Series 60 3.0
        * @return error code
        */
        TInt Scan();

        /**
        * SetCidList
        * @since Series 60 3.0
        * @param aList content ID list
        */
        void SetCidList(RPointerArray<HBufC8>*& aList);


        /**
        * SetPairList
        * @since Series 60 3.0
        * @param aList list of pairs for content ID and transaction ID
        */
        void SetPairList(RPointerArray<CPair>*& aList);

        /**
        * Stop watching the Rights Server
        * @since Series 60 3.1
        */
        void StopWatchingL();

        /**
        * RemoveMessageL
        * Called by destructor of the session class
        */
        void RemoveMessageL( const RMessage2* aMessage );


    public: // Functions from base classes

        //class MWatcherObserver

        /**
         * Called when the peer process died
         *
         * @since S60 3.0
         * @param aObject Changed object
         */
        void WatchedObjectChangedL( const TDesC& aObject );

    protected:  // New functions

    protected:  // Functions from base classes

        // Class CServer
        /**
        * NewSessionL
        * @since S60Rel3.0
        */
        CSession2* NewSessionL( const TVersion& aVersion,
                                       const RMessage2& aMessage ) const;


    private:

        /**
        * C++ default constructor.
        */
        CDcfRepSrv();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * StartupL called by StartUp
        * @since S60 Rel 3.0
        */
        static void StartupL();

        /**
        * PanicServer will panic with panic code
        * @since S60 Rel 3.0
        */
        static void PanicServer(TDcfRepSrvPanic aPanic);

        /**
        * CleanScanInternal will clean list for Scan
        * @since S60 Rel 3.0
        */
        void CleanScanInternal();

        /**
        * CleanDatabase will close database connection
        * @since S60 Rel 3.0
        */
        void CleanDatabase();

        /**
        * OpenDatabaseL will open database connection
        * @since S60 Rel 3.0
        */
        void OpenDatabaseL();

        /**
        * CleanTableL will clean all items in the table
        * @since S60 Rel 3.0
        */
        void CleanTableL();

        /**
        * AddDomainRoL will add domain RO into file if there is any.
        * @since S60 Rel 3.0
        */
        void AddDomainRoL( const TDesC& aFile );

        /**
        * CheckFileL will check the file type: dcf1,dcf2,plain
        * @since S60 Rel 3.0
        */
        void CheckFileL(const TDesC& aFile , TInt& aType);

        /**
        * StoreFileL will store file info
        * @since S60 Rel 3.0
        */
        void StoreFileL(const TDesC& aFile , TInt aType);

        /**
        * UpdateDatabaseL will store info into database
        * @since S60 Rel 3.0
        */
        void UpdateDatabaseL(
            const TDesC& aFile,
            TInt aPos,
            const TDesC& aCid ,
            const TDesC& aGroupId ,
            const TDesC& aTtid);

        /**
        * ResetTtidL will write new ttid into file
        * @since S60 Rel 3.0
        */
        void ResetTtidL(
            const TDesC& aFile,
            const TDesC8& aTtid
            );

        /**
        * ResetTtidL will write new ttid into file
        * @since S60 Rel 3.0
        */
        void ResetTtidL(
            COma2Dcf* aDcf,
            const TDesC8& aTtid
            );
        // Prohibit copy constructor if not deriving from CBase.
        CDcfRepSrv( const CDcfRepSrv& );
        // Prohibit assigment operator if not deriving from CBase.
        CDcfRepSrv& operator=( const CDcfRepSrv& );

    public:     // Data
    protected:  // Data
    private:    // Data
        RDbs iDbs;
        RDbNamedDatabase iDb;
        RDbView iView;
        RFs iFs;
        CFileScan* iScan;
        RPointerArray<HBufC8>* iCidList;
        RPointerArray<CPair>* iPairList;
        RPointerArray<RMessage2> iMessageList;
        TInt iState;
        CProcWatcher* iProcWatcher;
        TBool iArmed;

        // Reserved pointer for future extension
        TAny* iReserved;

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };

#endif      // CDCFREPSRV_H

// End of File
