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









#ifndef DRM_CAF_H
#define DRM_CAF_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>


#include <f32file.h>
#include <e32test.h>
#include <s32file.h>
#include <e32std.h>
#include <e32base.h>
#include <caf/data.h>

#include <Oma2Agent.h>
#include <DRMLicenseChecker.h>



// MACROS
#define TEST_CLASS_VERSION_MAJOR 51
#define TEST_CLASS_VERSION_MINOR 9
#define TEST_CLASS_VERSION_BUILD 38


// Logging path
_LIT( KDRM_CAFLogPath, "\\logs\\testframework\\DRM_CAF\\" );
// Log file
_LIT( KDRM_CAFLogFile, "DRM_CAF.txt" );
_LIT( KDRM_CAFLogFileWithTitle, "DRM_CAF_[%S].txt" );


// FORWARD DECLARATIONS
class CDRM_CAF;

// CLASS DECLARATION

/**
*  CDRM_CAF test class for STIF Test Framework TestScripter.
*/
NONSHARABLE_CLASS(CDRM_CAF) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CDRM_CAF* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CDRM_CAF();

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    private:

        /**
        * C++ default constructor.
        */
        CDRM_CAF( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        */
        void Delete();


        /**
        * Test methods are listed below.
        */

        /**
        *
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CSupplier_Oma1XmlRoL();

        virtual TInt CSupplier_Oma1WbxmlRoL();

        virtual TInt CData_NewL();

        virtual TInt CData_Read_DataSize_SeekL();

        virtual TInt CData_Read2L();
        virtual TInt CData_SetPropertyL();
        virtual TInt CData_EvaluateIntentL();
        virtual TInt CData_ExecuteIntentL();
        virtual TInt CData_GetAttributeL();
        virtual TInt CData_GetAttributeSetL();
        virtual TInt CData_GetStringAttributeL();
        virtual TInt CData_GetStringAttributeSetL();
        virtual TInt CContent_NewL();
        virtual TInt CContent_OpenCloseContainerL();
        virtual TInt CContent_GetEmbeddedObjectsL();
        virtual TInt CContent_SearchL();
        virtual TInt CContent_GetAttributeL();
        virtual TInt CContent_GetAttributeSetL();
        virtual TInt CContent_GetStringAttributeL();
        virtual TInt CContent_GetStringAttributeSetL();
        virtual TInt CContent_AgentSpecificCommandL();
        virtual TInt CContent_NotifyStatusChangeL();
        virtual TInt CContent_CancelNotifyStatusChangeL();
        virtual TInt CContent_RequestRightsL();
        virtual TInt CContent_DisplayInfoL();
        virtual TInt CContent_SetPropertyL();
        virtual TInt CContent_OpenContentL();
        virtual TInt CManager_FileOperationsL();
        virtual TInt CManager_GetAttributeL();
        virtual TInt CManager_GetAttributeSetL();
        virtual TInt CManager_GetStringAttributeL();
        virtual TInt CManager_GetStringAttributeSetL();
        virtual TInt CManager_SetPropertyL();
        virtual TInt CManager_DisplayInfoL();
        virtual TInt CManager_ListAgentsL();
        virtual TInt CManager_AgentSpecificCommandL();
        virtual TInt CManager_CreateRightsManagerL();
        virtual TInt CSupplier_IsImportSupportedL();
        virtual TInt CSupplier_PrepareHTTPRequestHeadersL();
        virtual TInt CSupplier_SetOutputDirectory_ImportFileL();
        virtual TInt CImportFile_WriteData_WriteDataComplete_OutputFileCountL();
        virtual TInt CImportFile_OutputFileL_GetImportStatusL();
        virtual TInt CImportFile_GetSuggestedOutputFileExtensionL();
        virtual TInt CImportFile_GetSuggestedOutputFileNameL();
        virtual TInt CImportFile_ContinueWithNewOutputFileL();
        virtual TInt CImportFile_ContentMimeTypeL();
        virtual TInt CRightsManager_ListRightsL();
        virtual TInt CRightsManager_ListAllRightsL();
        virtual TInt CRightsManager_ListContentL();
        virtual TInt CRightsManager_DeleteRightsObjectL();
        virtual TInt CRightsManager_DeleteAllRightsObjectsL();
        virtual TInt CRightsManager_SetPropertyL();
        virtual TInt CDRMLicenseChecker_CheckLicenseL();

        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        void CreateDirL();
        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    public:     // Data

        // MODULE DATA STRUCTURES
        TInt GLastOk;
        TInt GAllocatedCount;
        TInt GAllocatedSize;
        RFs iFs;

    private:    // Data


        // Reserved pointer for future extension
        //TAny* iReserved;
        //TInt GLastOk;
        //TInt GAllocatedCount;
        //TInt GAllocatedSize;

       // TBool flvariant;

       // RTest test;

    };

#endif      // DRM_CAF_H

// End of File
