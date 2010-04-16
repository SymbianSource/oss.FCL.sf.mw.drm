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
* Description:  Implementation of TItemData class
*
*/


// INCLUDE FILES
#include <e32std.h>

#include "DrmViewItems.h"


// ================= MEMBER FUNCTIONS =======================
//
// -----------------------------------------------------------------------------
// TItemData::TItemData
// -----------------------------------------------------------------------------
//
TItemData::TItemData()
: iIndexInArray( -1 ),
  iItemName( NULL ),
  iItemFullName( NULL ),
  iIndexIcon( -1 ),
  iTypeOfObject( -1 ),
  iContentCanBeSent( EFalse ),
  iLocalID( 0 ),
  iContentURI( NULL ),
  iFutureActivationTime( 0 ),
  iUnlimitedRights( EFalse ),
  iIndividualConstraint( EFalse ),
  iUsageAllowed( ETrue )
    {
    }


// -----------------------------------------------------------------------------
// TItemData::TItemData
// -----------------------------------------------------------------------------
//
TItemData::TItemData( const TInt aIndexInArray,
                      const TDesC& aItemName,
                      const TDesC& aItemFullName,
                      const TInt aIndexIcon,
                      const TInt aTypeOfObject,
                      const TBool aContentCanBeSent,
                      const TDesC8& aContentURI,
                      const TTime& aFutureActivationTime,
                      const TBool aUnlimitedRights,
                      const TBool aIndividualConstraint,
                      const TBool aUsageAllowed )
: iIndexInArray( aIndexInArray ),
  iItemName( aItemName ),
  iItemFullName( aItemFullName ),
  iIndexIcon( aIndexIcon ),
  iTypeOfObject( aTypeOfObject ),
  iContentCanBeSent( aContentCanBeSent ),
  iLocalID( 0 ),
  iContentURI( aContentURI ),
  iFutureActivationTime( aFutureActivationTime ),
  iUnlimitedRights( aUnlimitedRights ),
  iIndividualConstraint( aIndividualConstraint ),
  iUsageAllowed( aUsageAllowed )
    {
    }


// -----------------------------------------------------------------------------
// TItemData::TItemData
// -----------------------------------------------------------------------------
//
TItemData::TItemData( const TInt aIndexInArray,
                      const TDesC& aItemName,
                      const TDesC& aItemFullName,
                      const TInt aIndexIcon,
                      const TInt aTypeOfObject,
                      const TUint32 aLocalID,
                      const TDesC8& aContentURI,
                      const TTime& aFutureActivationTime,
                      const TBool aUnlimitedRights,
                      const TBool aIndividualConstraint,
                      const TBool aUsageAllowed )
: iIndexInArray( aIndexInArray ),
  iItemName( aItemName ),
  iItemFullName( aItemFullName ),
  iIndexIcon( aIndexIcon ),
  iTypeOfObject( aTypeOfObject ),
  iContentCanBeSent( EFalse ),
  iLocalID( aLocalID ),
  iContentURI( aContentURI ),
  iFutureActivationTime( aFutureActivationTime ),
  iUnlimitedRights( aUnlimitedRights ),
  iIndividualConstraint( aIndividualConstraint ),
  iUsageAllowed( aUsageAllowed )
    {
    }


// End of File
