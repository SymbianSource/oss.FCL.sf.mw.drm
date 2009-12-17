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
* Description:  CDRMRightsMgrAppUi inline functions
*
*/



#ifndef DRMRIGHTSMGRAPPUI_INL
#define DRMRIGHTSMGRAPPUI_INL


// ---------------------------------------------------------
// CDRMRightsMgrAppUi::SelectedIndexInArray
// ---------------------------------------------------------
//
inline TInt CDRMRightsMgrAppUi::SelectedIndexInArray()
    {
    return iSelectedItem.IndexInArray();
    }


// ---------------------------------------------------------
// CDRMRightsMgrAppUi::SelectedItemName
// ---------------------------------------------------------
//
inline void CDRMRightsMgrAppUi::SelectedItemName( TFileName& aItemName )
	{ 
    iSelectedItem.ItemName( aItemName );
	}


// ---------------------------------------------------------
// CDRMRightsMgrAppUi::SelectedItemFullName
// ---------------------------------------------------------
//
inline void CDRMRightsMgrAppUi::SelectedItemFullName( 
                                                     TFileName& aItemFullName )
	{ 
    iSelectedItem.ItemFullName( aItemFullName );
	}


// ---------------------------------------------------------
// CDRMRightsMgrAppUi::SelectedLocalID
// ---------------------------------------------------------
//
inline TUint32 CDRMRightsMgrAppUi::SelectedLocalID()
    {
    return iSelectedItem.LocalID();
    }


// ---------------------------------------------------------
// CDRMRightsMgrAppUi::SelectedContentURI
// ---------------------------------------------------------
//
inline void CDRMRightsMgrAppUi::SelectedContentURI( 
                                            TBuf8<KMaxFileName>& aContentURI )
    {
    iSelectedItem.ContentURI( aContentURI );
    }


// ---------------------------------------------------------
// CDRMRightsMgrAppUi::SelectedContentCanBeSent
// ---------------------------------------------------------
//
inline TBool CDRMRightsMgrAppUi::SelectedContentCanBeSent()
    {
    return iSelectedItem.ContentCanBeSent();
    }


// ---------------------------------------------------------
// CDRMRightsMgrAppUi::UnlimitedRights
// ---------------------------------------------------------
//
inline TBool CDRMRightsMgrAppUi::UnlimitedRights()
    {
    return iSelectedItem.UnlimitedRights();
    }

// ---------------------------------------------------------
// CDRMRightsMgrAppUi::IndividualConstraint
// ---------------------------------------------------------
//
inline TBool CDRMRightsMgrAppUi::IndividualConstraint()
    {
    return iSelectedItem.IndividualConstraint();
    }


// ---------------------------------------------------------
// CDRMRightsMgrAppUi::UsageAllowed
// ---------------------------------------------------------
//
inline TBool CDRMRightsMgrAppUi::UsageAllowed()
    {
    return iSelectedItem.UsageAllowed();
    }


// ---------------------------------------------------------
// CDRMRightsMgrAppUi::SelectedIsGroup
// ---------------------------------------------------------
//
inline TBool CDRMRightsMgrAppUi::SelectedIsGroup()
    {
    return iSelectedItem.IsGroup();
    }

// ---------------------------------------------------------
// CDRMRightsMgrAppUi::SetSelectedItemName
// ---------------------------------------------------------
//
inline void CDRMRightsMgrAppUi::SetSelectedItemName( const TDesC& aItemName )
    {
    iSelectedItem.SetItemName( aItemName );
    }


// ---------------------------------------------------------
// CDRMRightsMgrAppUi::SetSelectedItemFullName
// ---------------------------------------------------------
//
inline void CDRMRightsMgrAppUi::SetSelectedItemFullName( 
                                                const TDesC& aItemFullName )
    {
    iSelectedItem.SetItemFullName( aItemFullName );
    }


// ---------------------------------------------------------
// CDRMRightsMgrAppUi::SetSelectedContentCanBeSent
// ---------------------------------------------------------
//
inline void CDRMRightsMgrAppUi::SetSelectedContentCanBeSent( 
                                                const TBool aContentCanBeSent )
    {
    iSelectedItem.SetContentCanBeSent( aContentCanBeSent );
    }

// ---------------------------------------------------------
// CDRMRightsMgrAppUi::SetSelectedIndividualConstraint
// ---------------------------------------------------------
//
inline void CDRMRightsMgrAppUi::SetSelectedIndividualConstraint( 
                                                const TBool aIndividualConstraint )
    {
    iSelectedItem.SetIndividualConstraint( aIndividualConstraint );
    }


// ---------------------------------------------------------
// CDRMRightsMgrAppUi::SetSelectedUsageAllowed
// ---------------------------------------------------------
//
inline void CDRMRightsMgrAppUi::SetSelectedUsageAllowed( const TBool aUsageAllowed )
    {
    iSelectedItem.SetUsageAllowed( aUsageAllowed );
    }

#endif

// End of File
