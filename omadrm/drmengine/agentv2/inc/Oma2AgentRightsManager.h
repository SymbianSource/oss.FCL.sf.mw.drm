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
* Description: Class definition for OMA DRM CAF Agent Rights Manager
*
*/



#ifndef OMA2AGENTRIGHTSMANAGER_H
#define OMA2AGENTRIGHTSMANAGER_H

//  INCLUDES
#include <caf/caf.h>
#include <caf/agentinterface.h>

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
//class ?FORWARD_CLASSNAME;

namespace ContentAccess
{

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class COma2AgentRightsManager : public CAgentRightsManager
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static COma2AgentRightsManager* NewL();
        static COma2AgentRightsManager* NewLC();
        
        /**
        * Destructor.
        */
        ~COma2AgentRightsManager();

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

		// From CAgentRightsManager
		void ListAllRightsL(RStreamablePtrArray<CRightsInfo>& aArray) const;
		void ListRightsL(RStreamablePtrArray<CRightsInfo>& aArray, const TDesC& aUri) const;
		void ListRightsL(RStreamablePtrArray<CRightsInfo>& aArray, TVirtualPathPtr& aVirtualPath) const;
		void ListContentL(RStreamablePtrArray<CVirtualPath>& aArray, CRightsInfo& aRightsInfo) const;
		MAgentRightsBase* GetRightsDataL(const CRightsInfo& aRightsInfo) const;
		TInt DeleteRightsObject(const CRightsInfo& aRightsInfo);
		TInt DeleteAllRightsObjects(const TVirtualPathPtr& aVirtualPath);
        TInt SetProperty(TAgentProperty aProperty, TInt aValue);
		
	private:
        
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

        /**
        * C++ default constructor.
        */
        COma2AgentRightsManager();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // COma2AgentRightsManager( const COma2AgentRightsManager& );
        // Prohibit assigment operator if not deriving from CBase.
        // COma2AgentRightsManager& operator=( const COma2AgentRightsManager& );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data

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

#endif      // OMA2AGENTRIGHTSMANAGER_H   
            
// End of File
