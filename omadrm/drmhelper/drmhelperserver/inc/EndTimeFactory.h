/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handling end time based automated content
*
*/



#ifndef ENDTIMEFACTORY_H
#define ENDTIMEFACTORY_H

//  INCLUDES
// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CDRMHelperServer;
class CEndTimeBased;
// CLASS DECLARATION

/**
*  End time based rights storage class
*  
*
*  @lib DRMHelperServer.lib
*  @since Series60 2.6
*/
class CEndTimeFactory : public CTimer
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CEndTimeFactory* NewL(CDRMHelperServer& aServer);

        /**
        * Destructor.
        */
        virtual ~CEndTimeFactory();

    public: // New functions

        /**
        * Add a specific content uri into list. if the uri is already in the list, 
        * then update the status.
        * @since Series60 3.0
        * @param aUri content uri
        * @param aPermType permission type
        * @param aRegType regist type
        * @param aAutoType automatic type
        *
        */
        void AddL( 
                const TDesC8& aUri , 
                TTime aEndTime , 
                const TUint8& aPermType , 
                const TUint8& aRegType , 
                const TUint8& aAutoType );

        /**
        * update all items in the list. 
        * @since Series60 2.6
        * @param 
        * @return KErrNone if it works OK,
        */
        void UpdateL();

        /**
        * update the end time of a specific content uri in the list. 
        * @since Series60 3.0
        * @param aItem the item needs to be updated
        * @param aEndTime the new end time 
        * 
        */
        void UpdateEndTimeL( CEndTimeBased*& aItem , const TTime& aEndTime );

        /**
        * update the end time of a specific content uri in the list. 
        * @since Series60 3.0
        * @param aUri the specific content uri for this item.
        * @param aEndTime the new end time         
        * @param aPermType permission type
        * @param aRegType regist type
        */
        void UpdateEndTimeL( 
                const TDesC8& aUri , 
                const TTime& aEndTime , 
                const TUint8& aPermType , 
                const TUint8& aRegType );

        /**
        * update the end time of a specific content uri in the list. 
        * @since Series60 3.0
        * @param aUri the specific content uri for this item.
        * @param aEndTime the new end time         
        * @param aPermType permission type
        * @param aRegType regist type
        * @param aAutoType automatic type
        */
        void UpdateEndTimeL( 
                const TDesC8& aUri , 
                const TTime& aEndTime , 
                const TUint8& aPermType , 
                const TUint8& aRegType ,
                const TUint8& aAutoType );                

        /**
        * IsRegisteredL register one item with specific content uri.
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
        * IsRegisteredL register one item with specific content uri.
        * @since S60Rel3.0
        * @param aUri the specific content uri for this item.
        * @param aPermType permission type
        * @param aRegType regist type
        * @return ETrue if registered, otherwise EFalse
        */
        TBool IsRegistered( 
                const TDesC8& aUri , 
                const TUint8& aPermType , 
                const TUint8& aRegType );                        


        /**
        * IsRegisteredL register one item with specific content uri.
        * @since S60Rel3.0
        * @param aUri the specific content uri for this item.
        * @param aPermType permission type
        * @param aRegType regist type
        * @return ETrue if registered, otherwise EFalse
        */
        TBool IsRegistered2( 
                const TDesC8& aUri , 
                const TUint8& aRegType,
                const TUint8& aAutoType );  

        /**
        * IsRegisteredL register one item with specific content uri.
        * @since S60Rel3.0
        * @param aUri the specific content uri for this item.
        * @return ETrue if registered, otherwise EFalse
        */
        TBool IsRegistered( const TDesC8& aUri );      

        /**
        * Remove a specific content uri into list. 
        * @since Series60 3.0
        * @param aUri content uri
        * @param aPermType permission type
        * @param aRegType regist type
        * @param aAutoType automatic type
        */
        void Remove( 
                const TDesC8& aUri , 
                const TUint8& aPermType , 
                const TUint8& aRegType , 
                const TUint8& aAutoType );       

        /**
        * Handle items about to expire and expired when idle status comes
        * @since Series60 2.6
        * @
        */
        void HandleIdleL();

        /**
        * Externalize the list into data file. 
        * @since Series60 2.6
        * @param aStream data file stream
        */
        void ExternalizeL(RWriteStream& aStream) const;

        /**
        * Internalize the data from data file. 
        * @since Series60 2.6
        * @param aStream data file stream
        */
        void InternalizeL(RReadStream& aStream);

        /**
        * Reset the alarm
        * @since Series60 2.6
        */
        void ResetAlarm();

        /**
        * Remove all items in the list 
        * @since Series60 2.6
        */
        void RemoveAllL();

        /**
        * Handle the expiration case for one event
        * @since Series60 3.0
        */
        void HandleExpiredL();

    public: // Functions from base classes
    protected:  // New functions

        /**
        * From CActive
        * @since Series60 2.6
        */
        void RunL();

        /**
        * From CActive
        * @since Series60 2.6
        */
        void DoCancel();

    protected:  // Functions from base classes
    private:

        /**
        * C++ default constructor.
         */
        CEndTimeFactory( CDRMHelperServer& aServer );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        CEndTimeFactory( const CEndTimeFactory& );
        // Prohibit assigment operator if not deriving from CBase.
        CEndTimeFactory& operator=( const CEndTimeFactory& );



        /**
        * Find index for a specific content uri into list. 
        * @since Series60 2.6
        * @param aUri content uri
        * @param aPermType permission type
        * @param aRegType regist type
        * @param aAutoType automatic type
        * @return index for content uri in the list
        */
        TInt Find(      
                const TDesC8& aUri , 
                const TUint8& aPermType , 
                const TUint8& aRegType , 
                const TUint8& aAutoType );

        /**
        * Find index for a specific content uri into list. 
        * @since Series60 2.6
        * @param aUri content uri
        * @param aPermType permission type
        * @param aRegType regist type
        * @param aIndex the position to start searching in the list
        * @return index for content uri in the list
        */
        TInt FindNext(
                const TDesC8& aUri , 
                const TUint8& aPermType , 
                const TUint8& aRegType , 
                const TUint8& aIndex );

        /**
        * Find index for a specific content uri into list. 
        * @since Series60 2.6
        * @param aUri content uri
        * @param aRegType regist type
        * @return index for content uri in the list
        */
        TInt FindNext(
                const TDesC8& aUri ,  
                const TUint8& aindex);

        /**
        * Find index for a specific content uri into list. 
        * @since Series60 2.6
        * @param aUri content uri
        * @return index for content uri in the list
        */
        TInt Exists( 
                const TDesC8& aUri , 
                const TUint8& aRegType,
                const TUint8& aAutoType );

        /**
        * Reset the expiration status for all items in the list
        * @since Series60 2.6
        */
        void ResetExpired();


        /**
        * Reset registered times for specific content with specific type. 
        * @since Series60 2.6
        * @param aUri content uri
        * @param aPermType permission type
        * @param aRegType regist type
        * @param aAutoType automatic type
        * @param aRegTimes registered times
        * @return 
        */
        void ResetRegTimes( 
                const TDesC8& aUri , 
                const TUint8& aPermType , 
                const TUint8& aRegType , 
                const TUint8& aAutoType , 
                const TUint8& aRegTimes );

    public:     // Data
    protected:  // Data
    private:    // Data
        CDRMHelperServer& iServer;
        RPointerArray<CEndTimeBased> iList;
        TTime iAlarm;
        TTime iOverflowedAlarm;
    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes


    };

#endif      // ENDTIMEFACTORY_H   

// End of File
