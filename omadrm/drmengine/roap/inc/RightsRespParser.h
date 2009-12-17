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



#ifndef RIGHTSRESPPARSER_H
#define RIGHTSRESPPARSER_H

//  INCLUDES

#include <hash.h>
#include "RespParser.h"

namespace Roap
{
// FORWARD DECLARATIONS
class CRightsResp;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class TRightsRespParser : public MRespParser
    {
    public:  // Constructors
        
        TRightsRespParser(
            CRightsResp* aResponse);

    public: // New functions
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From ?base_class ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
        
        virtual void OnStartElementL(
            CRoapParser& aParser,
            TInt aState,
            const RTagInfo& aElement,
            const RAttributeArray& aAttributes);

        virtual void OnEndElementL(
            CRoapParser& aParser,
            TInt aState,
            const RTagInfo& aElement);

    protected:  // New functions
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

    public:     // Data
        // ?one_line_short_description_of_data
    
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
        CRightsResp* iResponse;

    private:    // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
         
        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };
}

#endif      // RIGHTSRESPPARSER_H   
            
// End of File
