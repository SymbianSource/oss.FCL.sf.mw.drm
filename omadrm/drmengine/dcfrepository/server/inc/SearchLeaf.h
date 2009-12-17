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
* Description:  Server common implementation
*
*/



#ifndef CSEARCHLEAF_H
#define CSEARCHLEAF_H

//  INCLUDES
#include <e32base.h>


// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CDir;

// CLASS DECLARATION


/**
*  Server Session implementation.
*  DCF Repository
*
*  @lib 
*  @since Series 60 3.0 
*/
class CSearchLeaf : public CBase
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSearchLeaf* NewL( CSearchLeaf*& aRoot , const TDesC& aFolderName );
        static CSearchLeaf* NewLC( CSearchLeaf*& aRoot , const TDesC& aFolderName );      

        /**
        * Destructor.
        */
        virtual ~CSearchLeaf();	

		const TDesC& FolderName();
	
		CSearchLeaf*& Root();
		
		RPointerArray<CSearchLeaf>& LeafList();

		void SetLeafL( CDir*& aLeaf );

		void RemoveLeaf( CSearchLeaf*& aLeaf );
	
	protected:

	private:
       /**
        * C++ default constructor.
        */
        CSearchLeaf( CSearchLeaf*& aRoot );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC& aFolderName );


	public:     // Data
    protected:  // Data
    private:    // Data
		CSearchLeaf* iRoot;
		RPointerArray<CSearchLeaf> iLeafList;
		HBufC* iFolderName;
	};


#endif      // CSEARCHLEAF_H   
            
// End of File
