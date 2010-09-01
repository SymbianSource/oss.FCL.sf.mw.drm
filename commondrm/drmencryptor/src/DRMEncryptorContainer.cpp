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
* Description:
*
*/


// INCLUDE FILES

#include <eiksbfrm.h>
#include <eikscrlb.h>
#include <eikrted.h>
#include <txtrich.h>
#include <barsread.h>
#include <eikenv.h>
#include <aknenv.h>
#include <AknUtils.h>
#include <aknconsts.h>
#include <txtfrmat.h>
#include <AknBidiTextUtils.h>

#include <DRMEncryptor.rsg>
#include "DRMEncryptorContainer.h"
#include "DRMEncryptorImage.h"
#include "DRMEncryptor.hrh"

// CONSTANTS

_LIT( KDRMEncryptorPanicCategory, "DRMEncryptor" );

enum TDRMEncryptorPanic
    {
    EDRMEncryptorNotSupported = 0
    };

// constructors

CDRMEncryptorContainer::CDRMEncryptorContainer()
    {
    }

void CDRMEncryptorContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();

    // In case of APAC layout, use APAC font
    TAknLayoutId layoutId;
    iAvkonEnv->GetCurrentLayoutId( layoutId );

    if ( layoutId == EAknLayoutIdAPAC )
        {
        iFont = ApacPlain12();
        }
    else
        {
        iFont = LatinPlain12();
        }

    // Calculate various text positioning parameters
    iBaseLineDelta = iFont->HeightInPixels() * 4 / 3;

    TInt mainPaneWidth( aRect.iBr.iX - aRect.iTl.iX );
    TInt mainPaneHeight( aRect.iBr.iY - aRect.iTl.iY );
    // Line width is 87% of client rect, horizontal margins 13%
    iLineWidth = mainPaneWidth * 87 / 100;

    iTopBaseLineX = ( mainPaneWidth - iLineWidth ) / 2;

    // top margin is 6.5% of the client rect
    TInt topMargin = mainPaneHeight * 65 / 1000;
    iTopBaseLineY = topMargin + iFont->AscentInPixels();

    // minimum bottom margin is 3% of the client rect
    TInt bottomMargin = mainPaneHeight * 3 / 100;
    iLinesPerScreen =
        ( mainPaneHeight - topMargin - bottomMargin ) / iBaseLineDelta;

    iTextAlign = CGraphicsContext::ELeft;

    // Every text line on screen is one entry in this array
    iText = new( ELeave ) CArrayPtrFlat<HBufC>( 20 );
    // Every image on screen is one entry in this array
    iImages = new( ELeave ) CArrayPtrFlat<CDRMEncryptorImage>( 1 );
    // This array contains indices for lines that start the subsequent
    // screens, for custom scrolling
    iScreenStarts = new( ELeave ) CArrayFixFlat<TInt>( 5 );
    // Initialisation: first screen starts at line 0.
    iScreenStarts->AppendL( 0 );

    // Read text and image items to be shown on the screen from a resource file.

    // real resource
    TResourceReader reader;
    iEikonEnv->CreateResourceReaderLC( reader, R_DRMENCRYPTOR_MAIN_TEXT );

    TInt numItems( reader.ReadInt16() );

    for ( TInt i = 0 ; i < numItems ; i++ )
        {
        TInt type = reader.ReadInt8();

        if ( type == EDRMEncryptorTextItem )
            {
            HBufC* text = iEikonEnv->AllocReadResourceLC( reader.ReadInt32() );
    SetTextL( *text );


            CleanupStack::PopAndDestroy(); // text
            }
        else if ( type == EDRMEncryptorImageItem )
            {
            TPtrC bitmapFile = reader.ReadTPtrC();
            TInt bitmapId = reader.ReadInt16();
            SetImageL( bitmapFile, bitmapId );
            }
        else
            {
            User::Panic( KDRMEncryptorPanicCategory, EDRMEncryptorNotSupported );
            }
        }

    CleanupStack::PopAndDestroy(); // reader

    UpdateScrollIndicatorL();
    SetRect( aRect );
    ActivateL();
    }

// destructor

CDRMEncryptorContainer::~CDRMEncryptorContainer()
    {
    delete iSBFrame;
    delete iScreenStarts;

    if ( iText )
        {
        iText->ResetAndDestroy();
        delete iText;
        }

    if ( iImages )
        {
        iImages->ResetAndDestroy();
        delete iImages;
        }
    }

// -----------------------------------------------------------------------------
// CDRMEncryptorContainer::Draw()
// -----------------------------------------------------------------------------

void CDRMEncryptorContainer::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();

    //  clear the area

    gc.SetBrushColor( iEikonEnv->ControlColor( EColorWindowBackground, *this ) );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.Clear( aRect );

    // draw text
    gc.UseFont( iFont );

    // index of the first line on the screen in the text array
    TInt firstLine( (*iScreenStarts)[ iCurrentScreen ] );
    // index of the last line on the screen in the text array
    TInt lastLine( firstLine + iLinesPerScreen - 1 );

    gc.SetBrushStyle( CGraphicsContext::ENullBrush );
    TPoint position( iTopBaseLineX, iTopBaseLineY );
    TPoint topLeft;
    TSize rectSize( iLineWidth, iBaseLineDelta +iFont->DescentInPixels() );

    for ( TInt index = firstLine ;
          index < iText->Count() && index <= lastLine ;
          index++, position.iY += iBaseLineDelta )
        {
        HBufC* text = (*iText)[ index ];

        if ( text )
            {
            topLeft = TPoint( position.iX, position.iY-iBaseLineDelta );
            gc.DrawText( *text,
                         TRect( topLeft, rectSize ),
                         iBaseLineDelta,
                         iTextAlign );
            }
        }

    gc.DiscardFont();

    // draw images

    for ( TInt i = 0 ; i < iImages->Count() ; i++ )
        {
        CDRMEncryptorImage* image = (*iImages)[ i ];

        // If part of the image resides in visible lines, draw it.
        if ( image->StartLine() <= lastLine && image->EndLine() >= firstLine )
            {
            position.SetXY( iTopBaseLineX, iTopBaseLineY );
            position.iY += ( image->StartLine() - firstLine ) * iBaseLineDelta;

            position.iY -= iBaseLineDelta - iFont->DescentInPixels();
            // Now iY is the top line of rectangle where the picture is
            // centered in.
            position.iY += ( (image->Lines()+1) * iBaseLineDelta -
                             iFont->HeightInPixels() -
                             image->HeightInPixels() ) / 2;

            // If text is right-aligned, also align images to the right.

            if ( iTextAlign == CGraphicsContext::ERight )
                {
                position.iX += ( iLineWidth - image->WidthInPixels() );
                }

            gc.BitBlt( position, image->Bitmap(), aRect );
            }
        }

    }

// -----------------------------------------------------------------------------
// CDRMEncryptorContainer::ActivateL()
// -----------------------------------------------------------------------------

void CDRMEncryptorContainer::ActivateL()
    {
    CCoeControl::ActivateL();
    UpdateScrollIndicatorL();
    }

// -----------------------------------------------------------------------------
// CDRMEncryptorContainer::SetTextL()
// -----------------------------------------------------------------------------

void CDRMEncryptorContainer::SetTextL( const TDesC& aText )
    {
    CArrayFix<TPtrC>* wrappedArray =
        new( ELeave ) CArrayFixFlat<TPtrC>( 10 );

    CleanupStack::PushL( wrappedArray );

    HBufC* dataToDestroy =
        AknBidiTextUtils::ConvertToVisualAndWrapToArrayL(
            aText, iLineWidth, *iFont, *wrappedArray
        );

    TInt numLines( wrappedArray->Count() );
    for ( TInt i = 0 ; i < numLines ; i++ )
        {
        HBufC* line = (*wrappedArray)[i].AllocLC();

        if(!line->Length())
            {
            iText->AppendL( NULL );

            CleanupStack::PopAndDestroy();  // line
            }
        else
            {
            iText->AppendL( line );
            CleanupStack::Pop();  // line
            }
        }
    iText->AppendL( NULL );

    // If the last char was newline, add one extra, since
    // wrapping automatically removes it.
    if ( aText[ aText.Length() - 1 ] == '\n' )
        {
        iText->AppendL( NULL );
        }

    CleanupStack::PopAndDestroy(); // wrappedArray
    delete dataToDestroy;

    // update screen scrolling info array

    TInt lastLine( iText->Count() - 1 );
    TInt screenStart( (*iScreenStarts)[ iScreenStarts->Count() - 1 ] );

    TBool firstNewScreenHandled( EFalse );

    while ( lastLine >= screenStart + iLinesPerScreen )
        {
        if ( !firstNewScreenHandled && iDoNotShowLastLineAgain )
            {
            screenStart++;
            firstNewScreenHandled = ETrue;
            }

        screenStart += iLinesPerScreen - 1;
        iScreenStarts->AppendL( screenStart );
        }

    // if text, last line is shown again in next screen
    iDoNotShowLastLineAgain = EFalse;
    }

// -----------------------------------------------------------------------------
// CDRMEncryptorContainer::SetImageL()
// -----------------------------------------------------------------------------

void CDRMEncryptorContainer::SetImageL( const TDesC& aFileName, TInt aBitmapId )
    {
    TInt firstLineOfImage( iText->Count() );

    CDRMEncryptorImage* image =
    CDRMEncryptorImage::NewLC( aFileName, aBitmapId, firstLineOfImage, iBaseLineDelta );

    // new lines to make room for the picture

    for ( TInt i = 0 ; i < image->Lines() ; i++ )
        {
        iText->AppendL( NULL );
        }

    iImages->AppendL( image );
    CleanupStack::Pop(); // image

    // update screen scrolling info array

    TInt lastLineOfImage( iText->Count() - 1 );
    TInt screenStart( (*iScreenStarts)[ iScreenStarts->Count() - 1 ] );

    TBool firstNewScreenHandled( EFalse );

    // If the image was not fully shown in the first screen,
    // start the next screen with the image.

    if ( firstLineOfImage < screenStart + iLinesPerScreen &&
         lastLineOfImage >= screenStart + iLinesPerScreen )
        {
        screenStart = firstLineOfImage;
        iScreenStarts->AppendL( screenStart );
        firstNewScreenHandled = ETrue;
        }

    while ( lastLineOfImage >= screenStart + iLinesPerScreen )
        {
        if ( !firstNewScreenHandled && iDoNotShowLastLineAgain )
            {
            screenStart++;
            firstNewScreenHandled = ETrue;
            }

        screenStart += iLinesPerScreen - 1;
        iScreenStarts->AppendL( screenStart );
        }

    if ( lastLineOfImage == screenStart + iLinesPerScreen - 1 )
        {
        iDoNotShowLastLineAgain = ETrue;
        }
    else
        {
        iDoNotShowLastLineAgain = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CDRMEncryptorContainer::OfferKeyEventL()
// -----------------------------------------------------------------------------

TKeyResponse CDRMEncryptorContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                              TEventCode aType )
    {
    if ( aType == EEventKey && iScreenStarts->Count() > 1 )
        {
        switch ( aKeyEvent.iCode )
            {
            case EKeyUpArrow:
                if ( iCurrentScreen > 0 )
                    {
                    iCurrentScreen--;
                    DrawNow();
                    UpdateScrollIndicatorL();
                    }
                break;

            case EKeyDownArrow:
                if ( iCurrentScreen < iScreenStarts->Count() - 1 )
                    {
                    iCurrentScreen++;
                    DrawNow();
                    UpdateScrollIndicatorL();
                    }
                break;

            default:
                break;
            }
        }

    return EKeyWasConsumed;
    }

// -----------------------------------------------------------------------------
// CDRMEncryptorContainer::UpdateScrollIndicatorL()
// -----------------------------------------------------------------------------

void CDRMEncryptorContainer::UpdateScrollIndicatorL()
    {
    if ( iScreenStarts->Count() <= 1 )
        {
        return;
        }

    if ( !iSBFrame )
        {
        iSBFrame = new( ELeave ) CEikScrollBarFrame( this, NULL, ETrue );
        iSBFrame->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                           CEikScrollBarFrame::EAuto );
        iSBFrame->SetTypeOfVScrollBar( CEikScrollBarFrame::EArrowHead );
        }

    TEikScrollBarModel hSbarModel;
    TEikScrollBarModel vSbarModel;
    vSbarModel.iThumbPosition = iCurrentScreen;
    vSbarModel.iScrollSpan = iScreenStarts->Count();
    vSbarModel.iThumbSpan = 1;

    TEikScrollBarFrameLayout layout;
    TRect rect( Rect() );
    iSBFrame->TileL( &hSbarModel, &vSbarModel, rect, rect, layout );
    iSBFrame->SetVFocusPosToThumbPos( vSbarModel.iThumbPosition );
    }

// End of File
