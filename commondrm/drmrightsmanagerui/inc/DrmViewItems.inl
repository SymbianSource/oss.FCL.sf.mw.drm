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
* Description:  TItemData inline functions
*
*/


#ifndef DRMVIEWITEMS_INL
#define DRMVIEWITEMS_INL


// ---------------------------------------------------------
// TItemData::IndexInArray
// ---------------------------------------------------------
//
inline TInt TItemData::IndexInArray() 
    { 
    return iIndexInArray; 
    }


// ---------------------------------------------------------
// TItemData::ItemName
// ---------------------------------------------------------
//
inline void TItemData::ItemName( TFileName& aItemName ) 
    { 
    aItemName = iItemName; 
    }


// ---------------------------------------------------------
// TItemData::ItemFullName
// ---------------------------------------------------------
//
inline void TItemData::ItemFullName( TFileName& aItemFullName ) 
    { 
    aItemFullName = iItemFullName; 
    }


// ---------------------------------------------------------
// TItemData::IndexIcon
// ---------------------------------------------------------
//
inline TInt TItemData::IndexIcon()
    {
    return iIndexIcon;
    }


// ---------------------------------------------------------
// TItemData::TypeOfObject
// ---------------------------------------------------------
//
inline TInt TItemData::TypeOfObject()
    {
    return iTypeOfObject;
    }


// ---------------------------------------------------------
// TItemData::ContentCanBeSent
// ---------------------------------------------------------
//
inline TBool TItemData::ContentCanBeSent()
    {
    return iContentCanBeSent;
    }


// ---------------------------------------------------------
// TItemData::UnlimitedRights
// ---------------------------------------------------------
//
inline TBool TItemData::UnlimitedRights()
    {
    return iUnlimitedRights;
    }

// ---------------------------------------------------------
// TItemData::IndividualConstraint
// ---------------------------------------------------------
//
inline TBool TItemData::IndividualConstraint()
    {
    return iIndividualConstraint;
    }


// ---------------------------------------------------------
// TItemData::UsageAllowed
// ---------------------------------------------------------
//
inline TBool TItemData::UsageAllowed()
    {
    return iUsageAllowed;
    }

// ---------------------------------------------------------
// TItemData::IsFuture
// ---------------------------------------------------------
//
inline TBool TItemData::IsFuture()
    {
    return iTypeOfObject & EFutureObject;
    }


// ---------------------------------------------------------
// TItemData::IsGroup
// ---------------------------------------------------------
//
inline TBool TItemData::IsGroup()
    {
    return iTypeOfObject & EGroupObject;
    }


// ---------------------------------------------------------
// TItemData::FutureActivationTime
// ---------------------------------------------------------
//
inline TTime TItemData::FutureActivationTime()
    {
    return iFutureActivationTime;
    }


// ---------------------------------------------------------
// TItemData::LocalID
// ---------------------------------------------------------
//
inline TUint32 TItemData::LocalID()
    {
    return iLocalID;
    }


// ---------------------------------------------------------
// TItemData::ContentURI
// ---------------------------------------------------------
//
inline void TItemData::ContentURI( TBuf8<KMaxFileName>& aContentURI )
    {
    aContentURI = iContentURI;
    }


// ---------------------------------------------------------
// TItemData::SetIndexInArray
// ---------------------------------------------------------
//
inline void TItemData::SetIndexInArray( TInt aIndex )
    {
    iIndexInArray = aIndex;
    }


// ---------------------------------------------------------
// TItemData::SetItemName
// ---------------------------------------------------------
//
inline void TItemData::SetItemName( const TDesC& aItemName )
    {
    iItemName = aItemName;
    }


// ---------------------------------------------------------
// TItemData::SetItemFullName
// ---------------------------------------------------------
//
inline void TItemData::SetItemFullName( const TDesC& aItemFullName )
    {
    iItemFullName = aItemFullName;
    }


// ---------------------------------------------------------
// TItemData::SetContentCanBeSent
// ---------------------------------------------------------
//
inline void TItemData::SetContentCanBeSent( const TBool aContentCanBeSent )
    {
    iContentCanBeSent = aContentCanBeSent;
    }


// ---------------------------------------------------------
// TItemData::SetUnlimitedRights
// ---------------------------------------------------------
//
inline void TItemData::SetUnlimitedRights( const TBool aUnlimitedRights )
    {
    iUnlimitedRights = aUnlimitedRights;
    }

// ---------------------------------------------------------
// TItemData::IndividualConstraint
// ---------------------------------------------------------
//
inline void TItemData::SetIndividualConstraint( const TBool aIndividualConstraint )
    {
    iIndividualConstraint = aIndividualConstraint;
    }


// ---------------------------------------------------------
// TItemData::UsageAllowed
// ---------------------------------------------------------
//
inline void TItemData::SetUsageAllowed( const TBool aUsageAllowed )
    {
    iUsageAllowed = aUsageAllowed;
    }


#endif

// End of File
