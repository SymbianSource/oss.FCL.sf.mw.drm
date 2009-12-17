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
* Description:  the server implementation of DRM Helper
*
*/


#ifndef DRMHELPERSERVER_H
#define DRMHELPERSERVER_H

// INCLUDES
#include <e32base.h>
#include "DRMHelperCommon.h"
#include <DRMEventObserver.h>
#include <caf/caf.h>
#include <DRMRightsClient.h> // RDRMRightsClient
// DATA TYPES
// FORWARD DECLARATIONS
class CEndTimeBased;
class CEndTimeFactory;
class RWriteStream;
class RReadStream;
class CDRMNotifier;
class CIdleObserver;
class CDRMRights;
class CConfigObserver;
class CDRMPermission;
class CEnvironmentChangeNotifier;

// CLASS DECLARATION
class TTimeBased
    {
    public:
        void SetStart(const TTime& aStart) {iStartTime = aStart;};
        void SetEnd(const TTime& aEnd) {iEndTime = aEnd;};
        const TTime& StartTime(){return iStartTime;};
        const TTime& EndTime(){return iEndTime;};
    private:
        TTime iStartTime;
        TTime iEndTime;
    };


/**
*  This class implements the DRM5 Helper Server functionality.
*
*  @lib DRMHelperServer.exe
*  @since S60Rel3.0
*/


class CDRMHelperServer : public CServer2 , MDRMEventObserver
    {
    public:  // Constructors and destructor
        
         /**
         * Two-phased constructor.
         * @since S60Rel3.0
         * @return Pointer to newly created server instance. And
         *  push the oject into cleanup stack
         */
        static CDRMHelperServer* NewLC();

        /**
         * Destructor.
         * @since S60Rel3.0
         */
        ~CDRMHelperServer();


        
    public: // New functions

        /**
         * Startup starts the actual DRM Helper server after initializing
         * the cleanup stack and active scheduler.
         * @since S60Rel3.0
         * @return system error code if error happens
         */
        static TInt Startup();

        /**
         * HandleExpiredL handle expiration case for one item.
         * @since S60Rel3.0
         * @param aUri the specific content uri for this item.
         * @param aTimes the times that this case has been informed .
         */
        void HandleExpiredL( CEndTimeBased*& aItem );

        /**
         * HandleIdleL inform all the cases that right is about to expire
         * @since S60Rel3.0
         */
        void HandleIdleL();

        /**
         * UpdateL update items
         * @since S60Rel3.0
         * @param aUri the specific content uri for this item.
         * @param aPermType permission type
         * @param aRegType regist type
         * @param aAutoType automatic type
         */
        void UpdateL();
        void UpdateL( const TDesC8& aUri );
        void UpdateL( const TDesC8& aUri , const TUint8& aPermType );
        void UpdateL( const TDesC8& aUri , const TUint8& aPermType , const TUint8& aRegType );
        void UpdateL( 
                const TDesC8& aUri , 
                const TUint8& aPermType , 
                const TUint8& aRegType , 
                const TUint8& aAutoType );

        /**
         * RegisterL register one item with specific content uri.
         * @since S60Rel3.0
         * @param aUri the specific content uri for this item.
         * @param aPermType permission type
         * @param aRegType regist type
         * @param aAutoType automatic type
         */
        void RegisterL( 
                        const TDesC8& aUri , 
                        const TUint8& aPermType , 
                        const TUint8& aRegType , 
                        const TUint8& aAutoType );


        /**
         * IsRegistered register one item with specific content uri.
         * @since S60Rel3.0
         * @param aUri the specific content uri for this item.
         * @param aPermType permission type
         * @param aRegType regist type
         * @param aAutoType automatic type
         * @return ETrue if registered, otherwise EFalse
         */
        TBool IsRegistered(
                        const TDesC8& aUri , 
                        const TUint8& aPermType , 
                        const TUint8& aRegType , 
                        const TUint8& aAutoType );
        /**
         * IsRegistered register one item with specific content uri.
         * @since S60Rel3.0
         * @param aUri the specific content uri for this item.
         * @param aPermType permission type
         * @param aRegType regist type
         * @return ETrue if registered, otherwise EFalse
         */
        TBool IsRegistered(
                        const TDesC8& aUri , 
                        const TUint8& aPermType , 
                        const TUint8& aRegType  );                


        /**
         * IsRegistered register one item with specific content uri.
         * @since S60Rel3.0
         * @param aUri the specific content uri for this item.
         * @param aRegType regist type
         * @return ETrue if registered, otherwise EFalse
         */
        TBool IsRegistered2(
                        const TDesC8& aUri , 
                        const TUint8& aRegType,
                        const TUint8& aAutoType  );

        /**
         * RemoveL remove one item with specific content uri. 
         * @since S60Rel3.0
         * @param aPermType permission type
         * @param aRegType regist type
         * @param aAutoType automatic type
         * @param aUri the specific content uri for this item.
         */
        void RemoveL(
                        const TDesC8& aUri , 
                        const TUint8& aPermType , 
                        const TUint8& aRegType , 
                        const TUint8& aAutoType );

        /**
         * StoreL store the list into data file
         * @since S60Rel3.0
         */
        void StoreL();

        /**
         * RestoreL restore the list from data file 
         * @since S60Rel3.0
         */
        void RestoreL();

        /**
         * ExternalizeL externalize the list into data file
         * @param aStream the data file stream.
         * @since S60Rel3.0
         */
        void ExternalizeL(RWriteStream& aStream) const;

        /**
         * InternalizeL internalize the list from data file
         * @param aStream the data file stream.
         * @since S60Rel3.0
         */
        void InternalizeL(RReadStream& aStream);

        /**
         * RemoveAllL remove all the items in the list
         * @since S60Rel3.0
         */
        void RemoveAllL();

        /**
         * GetNotificationL register one item to notifier in order to get notification if rights changed
         * @since S60Rel3.0
         * @param aUri the content uri for the item.
         */
        void GetNotificationL(const TDesC8& aUri);

        /**
         * RemoveNotificationL unregister item from notifier
         * @since S60Rel3.0
         * @param aUri the content uri for the item.
         */
        void RemoveNotificationL(const TDesC8& aUri);

        /**
         * UiTimeChanged 
         * 
         * callback function for CEnvironmentChangeNotifier when UI time is changed 
         * @since S60Rel2.6
         * @param aPointer a Helper Server instance
         */        
        static TInt UiTimeChanged(void* aPointer);

    public: // Functions from base classes
         /**
         * HandleEventL from MDRMEventObserver
         * @since S60Rel3.0
         * @param aEvent an event from DRM notifier
         */
        void HandleEventL( MDRMEvent* aEvent ); 
    protected:  // New functions
        
    protected:  // Functions from base classes
        //class CServer
        /**
         * RunError.
         * @since S60Rel3.0
         * @param aError Error code from RunL function.
         * @return An error is returned if RunError() cannot process the
         * error.
         */
        TInt RunError( TInt aError );
        
    private:
        /**
         * C++ default constructor.
         * @since S60Rel3.0
         */
        CDRMHelperServer();
        
        /**
         * By default Symbian 2nd phase constructor is private.
         * @since S60Rel3.0
         */
        void ConstructL();

        /**
         * @function PanicClient
         * @semantics panic the client
         * @param aMessage the message channel to the client
         * @param aReason the reason code for the panic.
         */
        static void PanicClient(const RMessage2& aMessage, TDRMHelperServPanic aReason);

        /*!
         * @function PanicServer
         * @semantics panic the server
         * @param aReason the reason code for the panic.
         */
        static void PanicServer(TDRMHelperServPanic aReason);

        /**
         * StartupL called by StartUp
         * @since S60Rel3.0 
         */
        static void StartupL();

        /**
         * CheckExpiration check the expiration info for one content uri
         * @since S60Rel3.0
         * @param aUri content uri.
         * @param aEndTime end time for rights.
         * @param aPermType permission type
         * @param aRegType regist type
         * @return CDRMRights::EFullRights or DRMCommon::ENoRights or EOk
         */
        void CheckExpirationL( 
                const TDesC8& aUri , 
                const TUint8& aPermType , 
                const TUint8& aRegType , 
                TTime& aEndTime );

        /**
         * SetKeyValueL set key value for CenRep structure
         * @since S60Rel3.0
         * @param aKeyValue key value.
         */
        void SetKeyValueL( const TDesC8& aKeyValue , const TUint8& aRegType );
        
        /**
         * FormatKeyValueL format key value for CenRep structure
         * 
         * @since S60Rel3.0
         * @param aBuf key value. it is pushed into cleanup stack
         * @param aUri the specific content uri for this item.
         * @param aTimes the times that this case has been informed.
         * @param aMark 'E' if expired 'V' if about to expire
         */
        void FormatKeyValueLC( HBufC8*& aBuf , CEndTimeBased*& aItem , const TDesC8& aMark );

        /**
         * CheckPermission get time based info from permission
         * 
         * @since S60Rel3.0
         * @param aPermission permission from DRM Rights database
         * @param aPermType permission type
         * @param aTime to store start and end time
         * @param aInterval to store time interval
         * @param aIndividual to check invididual constraints
         * @return 
         */        
        void CheckPermission(
                CDRMPermission* aPermission ,
                const TUint8& aPermType , 
                TTimeBased& aTime , 
                TTimeIntervalSeconds& aInterval,
                const RPointerArray<HBufC8>& aIndividual );
        
        ContentAccess::TIntent MapPermissionType(TUint8 aPermType);
    private: // Functions from base classes
        // Class CServer
        /** 
         * NewSessionL
         * @since S60Rel3.0
         */
        CSession2* NewSessionL( const TVersion& aVersion,
                                       const RMessage2& aMessage ) const;
        
    public:     // Data
        //opcodes used in message passing between client and server
        enum TDRMHSServiceType
            {
            EActive = 101,
            EPassive = 102
            };
    protected:  // Data
        
    private:    // Data
        CEndTimeFactory* iEndTimeFactory;  
        CDRMNotifier* iNotifier;
        CIdleObserver* iIdleObserver;
        CEnvironmentChangeNotifier* iUiTimeNotifier;
        RFs iFs;
        RDRMRightsClient iDrm;
    public:     // Friend classes
        
    protected:  // Friend classes
        
    private:    // Friend classes

};



#endif      // DRMHELPERSERVER_H   
            
// End of File
