/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares container control for application.
*
*/


#ifndef DRMEncryptorCONTAINER_H
#define DRMEncryptorCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <gdi.h>

// FORWARD DECLARATIONS
class CEikScrollBarFrame;
class CDRMEncryptorImage;
class CFont;
class TRect;
class TBidiText;

// CLASS DECLARATION

/**
*  CDRMEncryptorContainer  container control class.
*
*/
class CDRMEncryptorContainer : public CCoeControl
    {
    public: // Constructors and destructor
        CDRMEncryptorContainer();
        void ConstructL( const TRect& aRect );
        ~CDRMEncryptorContainer();

    private: // from CCoeControl

        void Draw( const TRect& aRect ) const;
        void ActivateL();
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aModifiers );

    private: // new functions

        void SetTextL( const TDesC& aText );
        void SetImageL( const TDesC& aFileName, TInt aBitmapId );
        void UpdateScrollIndicatorL();

    private: // Data

        CArrayPtr<HBufC>* iText;
        CArrayPtr<CDRMEncryptorImage>* iImages;
        CArrayFixFlat<TInt>* iScreenStarts;
        TInt iCurrentScreen;
        TBool iDoNotShowLastLineAgain;
        CGraphicsContext::TTextAlign iTextAlign;
        CEikScrollBarFrame* iSBFrame;
        const CFont* iFont; // not owned
        TInt iLineWidth;
        TInt iBaseLineDelta;
        TInt iTopBaseLineX;
        TInt iTopBaseLineY;
        TInt iLinesPerScreen;
    };

#endif

// End of File
