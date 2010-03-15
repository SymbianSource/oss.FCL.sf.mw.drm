/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares class TViewItems
*
*/


#ifndef DRMVIEWITEMS_H
#define DRMVIEWITEMS_H


// INCLUDES
#include "DRMRightsManager.hrh"


/**
* Class containing data for the items to be showed in the view
*/
class TItemData
    {
    public:  // Constructor
        /**
        * C++ default constructor.
        */
        TItemData();

        /**
        * C++ default constructor.
        * @param aIndexInArray          Index in array
        * @param aItemName              Name of the item
        * @param aItemFullName          Path name of the item
        * @param aIndexIcon             Index of the icon
        * @param aTypeOfObject          Attributes of the object
        * @param aContentCanBeSent      Object can be sent or not
        * @param aContentURI            Content URI of the object
        * @param aFutureActivationTime  Time in the future when the RO will be
        *                               activated.
        * @param aUnlimitedRights       Rights unlimited or not
        * @param aIndividualConstraint  Individual constraint exists or not
        * @param aUsageAllowed          Content usage allowed or not with current SIM
        */
        TItemData( const TInt aIndexInArray,
                   const TDesC& aItemName,
                   const TDesC& aItemFullName,
                   const TInt aIndexIcon,
                   const TInt aTypeOfObject,
                   const TBool aContentCanBeSent,
                   const TDesC8& aContentURI,
                   const TTime& aFutureActivationTime,
                   const TBool aUnlimitedRights,
                   const TBool aIndividualConstraint,
                   const TBool aUsageAllowed );

        /**
        * C++ default constructor.
        * @param aIndexInArray          Index in array
        * @param aItemName              Name of the item
        * @param aItemFullName          Path name of the item
        * @param aIndexIcon             Index of the icon
        * @param aTypeOfObject          Attributes of the object
        * @param aLocalID               Local ID of the object
        * @param aContentURI            Content URI of the object
        * @param aFutureActivationTime  Time in the future when the RO will be
        *                               activated.
        * @param aUnlimitedRights       Rights unlimited or not
        */
        TItemData( const TInt aIndexInArray,
                   const TDesC& aItemName,
                   const TDesC& aItemFullName,
                   const TInt aIndexIcon,
                   const TInt aTypeOfObject,
                   const TUint32 aLocalID,
                   const TDesC8& aContentURI,
                   const TTime& aFutureActivationTime,
                   const TBool aUnlimitedRights,
                   const TBool aIndividualConstraint,
                   const TBool aUsageAllowed );



    public:     // New functions
        /**
        * Returns the index of the element in the array.
        * @return Index in array
        */
        inline TInt IndexInArray();

        /**
        * Returns the name of the item.
        * @param aItemName The name of the item.
        */
        inline void ItemName( TFileName& aItemName );

        /**
        * Returns the path Name of the item.
        * @param aItemFullName  The path name of the item.
        */
        inline void ItemFullName( TFileName& aItemFullName );

        /**
        * Returns the index of the icon.
        * @return Index of the icon to be used for the object.
        */
        inline TInt IndexIcon();

        /**
        * Returns the type of object
        * @return The type of the object.
        */
        inline TInt TypeOfObject();

        /**
        * Returns the future rights attribute of the object.
        * @return ETrue if the object has future rights.
        */
        inline TBool IsFuture();

        /**
        * Returns the group attribute of the object.
        * @return ETrue if the object is a Group.
        */
        inline TBool IsGroup();

        /**
        * Returns if the content can be sent or not.
        * @return ETrue if the rights object is connected to an object that can
        * be sent.
        */
        inline TBool ContentCanBeSent();

        /**
        * Returns if objects rights are unlimited or not.
        * @param aUnlimitedRights ETrue if unlimited rights.
        */
        inline TBool UnlimitedRights();

        /**
        * Returns if object contains individual constraint or not.
        * @param aIndividualConstraint ETrue if individual constraint
        */
        inline TBool IndividualConstraint();

        /**
        * Returns if object use is allowed with current SIM or not.
        * @param aUsageAllowed ETrue if allowed
        */
        inline TBool UsageAllowed();

        /**
        * Returns the time in the future when the RO will be activated.
        * @return The activation time.
        */
        inline TTime FutureActivationTime();

        /**
        * Returns the local ID of the active RO.
        * @return The local ID.
        */
        inline TUint32 LocalID();

        /**
        * Returns the content URI of the object.
        * @param aContentURI The content URI.
        */
        inline void ContentURI( TBuf8<KMaxFileName>& aContentURI );

        /**
        * Sets the value for the index in array.
        * @param aIndex The index to be set.
        */
        inline void SetIndexInArray( TInt aIndex );

        /**
        * Sets the name of the item.
        * @param aItemName The name of the item.
        */
        inline void SetItemName( const TDesC& aItemName );

        /**
        * Sets the path name of the item.
        * @param aItemFullName The pathname of the item.
        */
        inline void SetItemFullName( const TDesC& aItemFullName );

        /**
        * Sets the "content can be sent" attribute of the object.
        * @param aContentCanBeSent ETrue if the content can be sent.
        */
        inline void SetContentCanBeSent( const TBool aContentCanBeSent );

        /**
        * Sets the unlimited rights attribute of the object.
        * @param aUnlimitedRights ETrue if unlimited rights.
        */
        inline void SetUnlimitedRights( const TBool aUnlimitedRights );

        /**
        * Sets the individual constraint attribute.
        * @param aIndividualConstraint ETrue if individual constraint
        */
        inline void SetIndividualConstraint( const TBool aIndividualConstraint );

        /**
        * Sets the usage allowed with current SIM attribute.
        * @param aUsageAllowed ETrue if allowed
        */
        inline void SetUsageAllowed( const TBool aUsageAllowed );

    private:
        // The index in the array of the elements
        TInt iIndexInArray;

        // Name of the item
        TFileName iItemName;

        // Full name of the item
        TFileName iItemFullName;

        // Index of the icon to be used
        TInt iIndexIcon;

        // Attributes of the object
        TInt iTypeOfObject;

        // ETrue if object can be sent
        TBool iContentCanBeSent;

        // Local ID of the active RO
        TUint32 iLocalID;

        // Content URI of the object
        TBuf8<KMaxFileName> iContentURI;

        // Time in the future when the RO will be activated
        TTime iFutureActivationTime;

        // ETrue if unlimited rights
        TBool iUnlimitedRights;

        // ETrue if individual constraint exists
        TBool iIndividualConstraint;

        // ETrue if content usage is allowed with current SIM
        TBool iUsageAllowed;

    };


// Include inline functions
#include "DrmViewItems.inl"

#endif


// End of File
