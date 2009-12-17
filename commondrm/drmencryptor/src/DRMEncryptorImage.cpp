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
* Description:  
*
*/


// INCLUDE FILES
#include <fbs.h>
#include "DRMEncryptorImage.h"

// ================= MEMBER FUNCTIONS ==========================================

CDRMEncryptorImage::CDRMEncryptorImage()
    {
    }

CDRMEncryptorImage::~CDRMEncryptorImage()
    {
    delete iBitmap;
    }

CDRMEncryptorImage* CDRMEncryptorImage::NewLC( const TDesC& aFileName,
                                 TInt aBitmapId,
                                 TInt aStartLine,
                                 TInt aBaseLineDelta )
    {
    CDRMEncryptorImage* self = new( ELeave ) CDRMEncryptorImage();
    CleanupStack::PushL( self );

    self->iBitmap = new( ELeave ) CFbsBitmap;
    self->iBitmap->Load( aFileName, aBitmapId );

    self->iStartLine = aStartLine;

    // enough lines so that image and margins fit in them.

    TInt lines( ( self->HeightInPixels() + 
                  KImageTopMargin + 
                  KImageBottomMargin +
                  aBaseLineDelta - 1 ) / aBaseLineDelta );

    self->iEndLine = aStartLine + lines - 1;
    return self;
    }

// -----------------------------------------------------------------------------
// CDRMEncryptorImage::HeightInPixels()
// -----------------------------------------------------------------------------

TInt CDRMEncryptorImage::HeightInPixels() const
    {
    return iBitmap->SizeInPixels().iHeight;
    }

// -----------------------------------------------------------------------------
// CDRMEncryptorImage::WidthInPixels()
// -----------------------------------------------------------------------------

TInt CDRMEncryptorImage::WidthInPixels() const
    {
    return iBitmap->SizeInPixels().iWidth;
    }

// -----------------------------------------------------------------------------
// CDRMEncryptorImage::StartLine()
// -----------------------------------------------------------------------------

TInt CDRMEncryptorImage::StartLine() const
    {
    return iStartLine;
    }

// -----------------------------------------------------------------------------
// CDRMEncryptorImage::EndLine()
// -----------------------------------------------------------------------------

TInt CDRMEncryptorImage::EndLine() const
    {
    return iEndLine;
    }

// -----------------------------------------------------------------------------
// CDRMEncryptorImage::Lines()
// -----------------------------------------------------------------------------

TInt CDRMEncryptorImage::Lines() const
    {
    return iEndLine - iStartLine + 1;
    }

// -----------------------------------------------------------------------------
// CDRMEncryptorImage::Bitmap()
// -----------------------------------------------------------------------------

const CFbsBitmap* CDRMEncryptorImage::Bitmap() const
    {
    return iBitmap;
    }

// End of File  
