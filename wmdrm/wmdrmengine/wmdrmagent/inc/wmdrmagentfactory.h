/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef WMDRMAGENTFACTORY_H
#define WMDRMAGENTFACTORY_H

//  INCLUDES
#include <AgentFactory.h>
#include "WmDrmAgentContent.h"
#include "WmDrmAgentData.h"
#include "WmDrmAgentManager.h"
#include "WmDrmAgentRightsManager.h"
#include "WmDrmAgentImportFile.h"

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
	class CWmDrmAgentFactory : public CAgentFactory
		{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CWmDrmAgentFactory* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CWmDrmAgentFactory();

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
		CAgentContent* CreateContentBrowserL(const TDesC& aURI, TContentShareMode aShareMode);	
		CAgentContent* CreateContentBrowserL(RFile& aFile);
		CAgentData* CreateDataConsumerL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode);
		CAgentData* CreateDataConsumerL(RFile& aFile, const TDesC& aUniqueId);
		CAgentManager*  CreateManagerL();
		CAgentImportFile* CreateImporterL(const TDesC8& aMimeType, const CMetaDataArray& aMetaDataArray, const TDesC& aOutputDirectory, const TDesC& aSuggestedFileName);
		CAgentImportFile* CreateImporterL(const TDesC8& aMimeType, const CMetaDataArray& aMetaDataArray);
		CAgentRightsManager* CreateRightsManagerL();	

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
        CWmDrmAgentFactory();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // CWmDrmAgentFactory( const CWmDrmAgentFactory& );
        // Prohibit assigment operator if not deriving from CBase.
        // CWmDrmAgentFactory& operator=( const CWmDrmAgentFactory& );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

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

} // namespace ContentAccess
#endif // WMDRMAGENTFACTORY_H
            
// End of File
