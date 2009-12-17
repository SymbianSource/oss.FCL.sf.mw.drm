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
* Description:  
*
*/



// INCLUDE FILES
#include "drmpermissionitem.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================
// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CDRMCommonData::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMPermissionItem* CDRMPermissionItem::NewLC()
    {
    CDRMPermissionItem* self = new( ELeave ) CDRMPermissionItem();
    CleanupStack::PushL( self );
    self->ConstructL();    

    return self;
    };

// -----------------------------------------------------------------------------
// CDRMCommonData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDRMPermissionItem* CDRMPermissionItem::NewL()
    {
    CDRMPermissionItem* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    };

// -----------------------------------------------------------------------------
// CDRMCommonData::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
// 
CDRMPermissionItem* CDRMPermissionItem::NewLC( const CDRMConstraint& aNormalized,
                                               TInt aIndex,
                                               TInt aParentIndex )
    {
    CDRMPermissionItem* self = new( ELeave ) CDRMPermissionItem( aIndex, aParentIndex);
    CleanupStack::PushL( self );
    self->ConstructL( aNormalized );    

    return self;    
    };
                                      
// -----------------------------------------------------------------------------
// CDRMCommonData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//                                      
CDRMPermissionItem* CDRMPermissionItem::NewL( const CDRMConstraint& aNormalized,
                                     TInt aIndex,
                                     TInt aParentIndex )
    {
    CDRMPermissionItem* self = NewLC( aNormalized, aIndex, aParentIndex );
    CleanupStack::Pop( self );   

    return self;    
    };                                      

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
// 
CDRMPermissionItem::CDRMPermissionItem( TInt aIndex,
                                        TInt aParentIndex ) :
    CDRMConstraint(), 
    iIndex( aIndex ),
    iParent( aParentIndex )
    {
    
    };

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
// 
CDRMPermissionItem::CDRMPermissionItem() :
    CDRMConstraint(), 
    iIndex( -1 ),
    iParent( -1 )
    {
    
    };
    

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
// 
CDRMPermissionItem::~CDRMPermissionItem()
    {
    // Nothing
    };    
    
// -----------------------------------------------------------------------------
// CDRMPermissionItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
// 
void CDRMPermissionItem::ConstructL( const CDRMConstraint& aNormalized )
    {
    (const_cast<CDRMPermissionItem*>(this))->DuplicateL( aNormalized );
    };

// -----------------------------------------------------------------------------
// CDRMPermissionItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//  
void CDRMPermissionItem::ConstructL()
    {
    };   
