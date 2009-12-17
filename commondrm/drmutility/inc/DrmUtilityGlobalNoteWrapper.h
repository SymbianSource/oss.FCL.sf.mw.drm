/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Synchronous wrapper for showing global notes
*
*/



#ifndef __DRMUTILITYGLOBALNOTEWRAPPER_H__
#define __DRMUTILITYGLOBALNOTEWRAPPER_H__

//  INCLUDES
#include <e32base.h>
#include <stringresourcereader.h>

namespace DRM 
{

// CONSTANTS
const TInt KDRMNoteBufferMaxSize ( 256 );

/**
*  Wrapper class for displaying global notes.
*/
NONSHARABLE_CLASS ( CDrmUtilityGlobalNoteWrapper ) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CDrmUtilityGlobalNoteWrapper* NewL( 
                                    CStringResourceReader* aResourceReader );
        
        static CDrmUtilityGlobalNoteWrapper* NewLC( 
                                    CStringResourceReader* aResourceReader );
        
        /**
        * Destructor.
        */
        virtual ~CDrmUtilityGlobalNoteWrapper();

    public: // New functions

        /**
        * Synchronous wrapper for showing global note.
        * @param aTextResourceId resource id of string to be displayed
        * @param aButtonsId resource id of buttons of note
        * @return button code
        */
        TInt ShowNoteWithButtonsL( TInt aResourceId, TInt aButtonsId );
        
        /**
        * Synchronous wrapper for showing global note.
        * @param aTextResourceId resource id of string to be displayed
        * @param aButtonsId resource id of buttons of note
        * @param aString string which replaces %U in resource string
        * @return button code
        */
        TInt ShowNoteWithButtonsL( TInt aResourceId, 
                                   TInt aButtonsId, 
                                   const TDesC& aString );
        
        /**
        * Synchronous wrapper for showing global note.
        * @param aTextResourceId resource id of string to be displayed
        * @param aValue integer that replaces %N in resource string        
        * @return button code
        */
        TInt ShowNoteL( TInt aResourceId, TInt aValue );

        /**
        * Synchronous wrapper for showing global note.
        * @param aTextResourceId resource id of string to be displayed
        * @param aString string that replaces %U in resource string        
        * @return button code
        */
        TInt ShowNoteL( TInt aResourceId, const TDesC& aString );

        /**
        * Synchronous wrapper for showing global note.
        * @param aTextResourceId resource id of string to be displayed
        * @param aString string that replaces %U in resource string        
        * @param aValue integer that replaces %N in resource string        
        * @return button code
        */
        TInt ShowNoteL( TInt aResourceId, 
        	            const TDesC& aString, 
        	            TInt aValue,
        	            TInt aStringPos = -1,
        	            TInt aValuePos = -1 );
        	            
        /**
        * Synchronous wrapper for showing global  preview list query.
        * @param aTextResourceId resource id of preview structure to be
        *        displayed
        * @return button code
        */
        TInt ShowPreviewListQueryL( TInt aResourceId ); 

    private: // From CActive
    
    	void DoCancel();
    	void RunL();
        
    private:

        /**
        * C++ default constructor.
        */
        CDrmUtilityGlobalNoteWrapper();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CStringResourceReader* aResourceReader );

        /**
        * Do actual displaying of note
        */
        TInt DoShowNoteL(TInt aResourceId, 
                         const TDesC& aString = KNullDesC, 
                         TInt aValue = -1 );
        
    private:    // Data

        // Used to make asynchronous call synchronous
        CActiveSchedulerWait iWait;

        // Text buffer used with global notes. Can contain max 
        // KTextBufferMaxSize long string + filename.
        TBuf<KDRMNoteBufferMaxSize + KMaxFileName> iTextBuffer;
        
        // used to read resources
        CStringResourceReader* iResourceReader;
        
        // Resource id of buttons in confirmation query
        TInt iButtonsId;
    };

}

#endif      // __DRMUTILITYGLOBALNOTEWRAPPER_H__   
            
// End of File

