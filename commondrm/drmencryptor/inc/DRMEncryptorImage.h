/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Image to be shown in DRMEncryptor screen.
*
*/


#ifndef DRMEncryptorIMAGE_H
#define DRMEncryptorIMAGE_H

// INCLUDES

// FORWARD DECLARATIONS
class CFbsBitmap;

// CONSTANTS

const TInt KImageTopMargin = 4 ;
const TInt KImageBottomMargin = 4;

// CLASS DECLARATION

/**
* CDRMEncryptorImage
*/
class CDRMEncryptorImage : public CBase
    {
    public: // Constructors and destructor

        static CDRMEncryptorImage* NewLC( const TDesC& aFileName, TInt aBitmapId,
                                   TInt aStartLine, TInt aBaseLineDelta );

        /**
        * Destructor
        */
        ~CDRMEncryptorImage();

    public: // New functions

        TInt HeightInPixels() const;
        TInt WidthInPixels() const;
        TInt StartLine() const;
        TInt EndLine() const;
        TInt Lines() const;
        const CFbsBitmap* Bitmap() const;

    private: // private constructor

        /**
        * C++ default constructor.
        */
        CDRMEncryptorImage();

    private:  // Data

        CFbsBitmap* iBitmap; // owned
        TInt iStartLine;
        TInt iEndLine;
    };

#endif // DRMEncryptorIMAGE_H

// End of File
