/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CDRMRightsMgrDocument class
*
*/


// INCLUDE FILES
#include <AiwGenericParam.h>
#include <GenericParamConsumer.h>

#include "DRMRightsMgrDocument.h"
#include "DRMRightsMgrAppUi.h"
#include "DRMCommonUtilities.h"

#include "DRMUILogger.h"


// ================= MEMBER FUNCTIONS =======================
//
// -----------------------------------------------------------------------------
// CDRMRightsMgrDocument::CDRMRightsMgrDocument
// -----------------------------------------------------------------------------
//
CDRMRightsMgrDocument::CDRMRightsMgrDocument( CEikApplication& aApp )
: CAiwGenericParamConsumer( aApp )
    {
    }


// ----------------------------------------------------
// CDRMRightsMgrDocument::~CDRMRightsMgrDocument
// ----------------------------------------------------
//
CDRMRightsMgrDocument::~CDRMRightsMgrDocument()
    {
    }


// ----------------------------------------------------------
// CDRMRightsMgrDocument::ConstructL
// ---------------------------------------------------------
//
void CDRMRightsMgrDocument::ConstructL()
    {
    CLOG_CREATE
    }


// -----------------------------------------------------------------------------
// CDRMRightsMgrDocument* CDRMRightsMgrDocument::NewL
// -----------------------------------------------------------------------------
//
CDRMRightsMgrDocument* CDRMRightsMgrDocument::NewL( CEikApplication& aApp )
    {
    CDRMRightsMgrDocument* self = new( ELeave )CDRMRightsMgrDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// ----------------------------------------------------
// CDRMRightsMgrDocument::CreateAppUiL
// ----------------------------------------------------
//
CEikAppUi* CDRMRightsMgrDocument::CreateAppUiL()
    {
    return new( ELeave )CDRMRightsMgrAppUi();
    }


// ----------------------------------------------------
// CDRMRightsMgrDocument::OpenFileL
// ----------------------------------------------------
//
CFileStore* CDRMRightsMgrDocument::OpenFileL( TBool aDoOpen,
                                              const TDesC& /*aParams*/,
                                              RFs& /*aFs*/ )
    {
    if ( aDoOpen )
        {
        TLex lex;

        TPtrC filename;
        const CAiwGenericParamList* paramList = GetInputParameters();
        if ( paramList )
            {
            TInt index = 0;
            const TAiwGenericParam* param = NULL;
            param = paramList->FindFirst( index, EGenericParamFile,
                                          EVariantTypeDesC );
            if ( index >= 0 && param )
                {
                lex = param->Value().AsDes();
                }
            }

        CDRMRightsMgrAppUi& appUi =
                                *static_cast<CDRMRightsMgrAppUi*>( iAppUi );
        DRMCommonUtilities::ParseParametersAndStartL( lex, ETrue, appUi );
        }

    return NULL;
    }


// End of File
