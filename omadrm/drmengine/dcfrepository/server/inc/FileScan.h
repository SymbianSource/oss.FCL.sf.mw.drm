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


#ifndef CFILESCAN_H
#define CFILESCAN_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// CLASS DECLARATION
class RFs;
class CDcfRepSrv;
class CDir;
class CSearchLeaf;
/**
*  CFileScan
*
*  @lib DcfRepSrv.exe
*  @since 3.0
*/
class CFileScan : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CFileScan* NewL( RFs& aFs );
        
        /**
        * Destructor.
        */
        virtual ~CFileScan();

    public: // New functions
        

		
		
		/**
		*
        * SearchContent
		*
		* Function starts the active objects to search protected file through whole file system 
		*
        * @since 2.5
        * @return KErrNone if correctly done
        */
		TInt SearchContent(CDcfRepSrv* aServer);


    public: // Functions from base classes

		/**
		*
		* From CActive
		*
        * DoCancel
		*
		* Function is called by Cancel() and set all status to initial status.
		*
        * @since 2.5
		*/
		void DoCancel();
		
		/**
		*
		* From CActive
		*
        * DoCancel
		*
		* Function is called when the request is completed
		*
        * @since 2.5
		*/
		void RunL();
        
    protected:  // New functions
    protected:  // Functions from base classes
    private:

        /**
        * C++ default constructor.
        */
        CFileScan( RFs& aFs );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

		TInt SearchDrive();

		TInt SearchFolder( CDir*& aFolderList );

		TInt SearchFile( CDir*& aFileList );

		TInt SearchNext();

		TInt CheckDrive();

		TInt CheckFolder();
		
		TInt IsProtected( const TDesC& aFileName , TBool& aIsDCF );

		TInt ToNextLeaf();

		TInt UpFolder();

		void CleanInternal();
		
		CSearchLeaf* GetRootLeaf();

    public:     // Data
    
    protected:  // Data

    private:    // Data
        CDcfRepSrv* iServer; //server
		RFs* iFs;
		TFileName iLastPosition;
		CSearchLeaf* iCurrentLeaf;
		TBool iSearching;
		TBool iDeeper;
		

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };



#endif      // CFILESCAN_H 
            
// End of File
