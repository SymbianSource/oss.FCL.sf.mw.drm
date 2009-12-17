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
* Description:  server common implementation
*
*/



// INCLUDE FILES
#include    "SearchLeaf.h"
#include    <e32std.h>
#include    <e32base.h>
#include    <f32file.h>

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
// CSearchLeaf::CSearchLeaf
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//    
CSearchLeaf::CSearchLeaf( CSearchLeaf*& aRoot ) : iRoot(aRoot) 
	{
	}

// -----------------------------------------------------------------------------
// CPair::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSearchLeaf::ConstructL( const TDesC& aFolderName )
	{
	iFolderName = aFolderName.AllocL();
	}
	
// Destructor
CSearchLeaf::~CSearchLeaf( void )
	{
	delete iFolderName;
	iFolderName = NULL;
	iRoot = NULL;
    iLeafList.ResetAndDestroy();
	iLeafList.Close();	    
	}

// -----------------------------------------------------------------------------
// CSearchLeaf::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSearchLeaf* CSearchLeaf::NewL(  CSearchLeaf*& aRoot , const TDesC& aFolderName )
	{
	CSearchLeaf* self = NewLC( aRoot , aFolderName );
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSearchLeaf::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSearchLeaf* CSearchLeaf::NewLC(  CSearchLeaf*& aRoot , const TDesC& aFolderName )
	{
    CSearchLeaf* self = new ( ELeave ) CSearchLeaf( aRoot );
    CleanupStack::PushL( self );
    self->ConstructL( aFolderName );
    return self; 
	}

// -----------------------------------------------------------------------------
// CSearchLeaf::SetLeafL
// -----------------------------------------------------------------------------
//
void CSearchLeaf::SetLeafL( CDir*& aLeaf )
	{
	CSearchLeaf* leaf = NULL;
	CSearchLeaf* root = this;
	TInt i = 0;
	for ( ; i < aLeaf->Count() ; i++ )
		{
		leaf = CSearchLeaf::NewL( root , ( *aLeaf )[i].iName );
		CleanupStack::PushL( leaf );
		User::LeaveIfError( iLeafList.Append( leaf ) );
		CleanupStack::Pop(leaf); //leaf
		}
	}

// -----------------------------------------------------------------------------
// CSearchLeaf::RemoveLeaf
// -----------------------------------------------------------------------------
//
void CSearchLeaf::RemoveLeaf( CSearchLeaf*& aLeaf )
	{
	TInt index = 0;
	index = iLeafList.Find( aLeaf );
	iLeafList.Remove( index );
	iLeafList.Compress();
	}
	
// -----------------------------------------------------------------------------
// CSearchLeaf::FolderName
// -----------------------------------------------------------------------------
//
const TDesC& CSearchLeaf::FolderName()
	{
	return (*iFolderName);
	}

// -----------------------------------------------------------------------------
// CSearchLeaf::Root
// -----------------------------------------------------------------------------
//
CSearchLeaf*& CSearchLeaf::Root()
	{
	return iRoot;
	}

// -----------------------------------------------------------------------------
// CSearchLeaf::LeafList
// -----------------------------------------------------------------------------
//
RPointerArray<CSearchLeaf>& CSearchLeaf::LeafList()
	{
	return iLeafList;
	}


//  End of File  
