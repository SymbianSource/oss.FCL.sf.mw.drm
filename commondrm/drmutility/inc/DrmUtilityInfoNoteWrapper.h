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



#ifndef __DRMUTILITYINFONOTEWRAPPER_H__
#define __DRMUTILITYINFONOTEWRAPPER_H__

//  INCLUDES
#include <e32base.h>
#include <AknGlobalNote.h>

namespace DRM
{

/**
*  Wrapper class for displaying global notes.
*/
NONSHARABLE_CLASS ( CDrmUtilityInfoNoteWrapper ) : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CDrmUtilityInfoNoteWrapper* NewL();

        static CDrmUtilityInfoNoteWrapper* NewLC();

        /**
        * Destructor.
        */
        virtual ~CDrmUtilityInfoNoteWrapper();

    public: // New functions


        /**
        * Synchronous wrapper for showing global note.
        * @param aType type of Info note to be displayed
        * @param aNoteText string containing note text
        * @param aResource resource identifier
        * @param aString is transferred for CoverUi string data
        * @param aValue is transferred for CoverUi numerical data
        * @return button code
        */
        void ShowNoteL(  TAknGlobalNoteType aType,
                         const TDesC& aNoteText,
                         TInt aResourceId = -1,
                         const TDesC& aString = KNullDesC,
                         TInt aValue = -1 );

        /**
        * Synchronous wrapper for showing global note.
        * @param[inout] aResourceId takes primary display
        *               resource ID and returns CoverUiId
        * @return       boolean if ID was valid CoverUiId
        */
        static TBool EvaluateCoverResourceId( TInt& aResourceId );

    private: // From CActive

        void DoCancel();
        void RunL();

    private:

        /**
        * C++ default constructor.
        */
        CDrmUtilityInfoNoteWrapper();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // Data

        CAknGlobalNote* iNote;
    };

}

#endif      // __DRMUTILITYINFONOTEWRAPPER_H__

// End of File

