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
* Description:  ?Description
*
*/



#ifndef RESPPARSER_H
#define RESPPARSER_H

//  INCLUDES
#include <xml/parser.h>
#include <xml/contenthandler.h>
#include <xml/xmlparsererrors.h>

using namespace Xml;

namespace Roap
{
// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS

class CRoapParser;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class MRespParser
    {
    public: // New functions

        virtual void OnStartElementL(
            CRoapParser& aParser,
            TInt aState,
            const RTagInfo& aElement,
            const RAttributeArray& aAttributes) = 0;

        virtual void OnEndElementL(
            CRoapParser& aParser,
            TInt aState,
            const RTagInfo& aElement) = 0;
    };
}

#endif      // ROAPPARSER_H

// End of File
