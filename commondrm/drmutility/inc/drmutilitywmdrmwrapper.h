/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Wraps WM DRM operations for use with DRM Utility
*
*/


#ifndef CDRMUTILITYWMDRMWRAPPER_H
#define CDRMUTILITYWMDRMWRAPPER_H

//*** system include files go here:
#include <e32base.h>
#include <f32file.h>
#include <AknServerApp.h>
#include <caf/caf.h>
#include <caf/cafplatform.h>
#include <drmutilitytypes.h>

//*** user include files go here:
#include "drmwmdrmwrapper.h"

class CWmDrmDlaWrapper;

namespace DRM
    {

    class CDrmUtilityUI;

 /**
  *  Utility class for DRM related common functions
  *
  *  @lib drmutilityemdrmwrapper.lib
  *  @since S60 v5.0
  */
NONSHARABLE_CLASS( CDrmUtilityWMDrmWrapper ) : public CBase, public MDrmUtilityWMDrmWrapper,
    public MAknServerAppExitObserver
    {

public:

    /**
     * Creates a new CDrmUtilityWMDrmWrapper object and returns a pointer to it
     *
     * @return A functional CDrmUtilityWMDrmWrapper -object
     * @leave System wide error code
     */
    IMPORT_C static CDrmUtilityWMDrmWrapper* NewL();

    /**
     * Creates a new CDrmUtilityWMDrmWrapper object and returns a pointer to it
     * Leaves the pointer to the cleanup stack
     *
     * @return A functional CDrmUtilityWMDrmWrapper -object
     * @leave System wide error code
     */
    IMPORT_C static CDrmUtilityWMDrmWrapper* NewLC();

    /**
     * Destructor
     */
    virtual ~CDrmUtilityWMDrmWrapper();


public: // From MDrmUtilityWMDrmWrapper

    /**
     * Performs the check rights amount operation for windows media files
     *
     * @param[in]   aContent    The content itself
     * @param[in]   aIntent     The intent for the rights to check
     * @param[in]   aObserver   Observer for callbacks, NULL if not
     *                          available
     * @param[in]   aOperatioId The operation id of the async operation
     * @return none
     * @leave System wide error code
     */
    IMPORT_C void CheckWmRightsAmountL(
        ContentAccess::CData& aContent,
        ContentAccess::TIntent aIntent,
        MDrmUiCheckRightsObserver* aObserver,
        TInt aOperationId );

    /**
     * Performs the handle error operation for windows media files
     *
     * @param[in]   aContent    The content itself
     * @param[in]   aIntent     The intent for the rights to check
     * @param[in]   aError      The error code to be handled
     * @param[in]   aObserver   Observer for callbacks, NULL if not
     *                          available
     * @param[in]   aOperatioId The operation id of the async operation
     * @return none
     * @leave System wide error code
     */
    IMPORT_C void HandleWmErrorL(
        ContentAccess::CData& aContent,
        ContentAccess::TIntent aIntent,
        TInt aError,
        MDrmHandleErrorObserver* aObserver,
        TInt aOperationId );

    /**
     * Method for setting the control environment
     *
     * @param[in]   aCoeEnv     The control environment
     * @return none
     */
    IMPORT_C void SetCoeEnv(
        CCoeEnv* aCoeEnv );

protected:

private:

    CDrmUtilityWMDrmWrapper();

    void ConstructL();

//*** Callback methods:

    // Callback to observer with info rights not valid and why
    void CallRightsNotValidL(
        MDrmUiCheckRightsObserver* aObserver,
        TInt aOperationId,
        TCheckRightsStatus aRightsStatus,
        TUint32 aReason,
        ContentAccess::CData& aContent,
        const TDesC& aContentUri );

    // Callback to the observer with the rights left information
    // if no observer do default handling
    void CallRightsLeftL(
        MDrmUiCheckRightsObserver* aObserver,
        TInt aOperationId,
        TBool aUnconstrained,
        TTimeIntervalSeconds aTime,
        TInt aCounts,
        TTimeIntervalSeconds aAccumulated,
        ContentAccess::CData& aContent,
        const TDesC& aContentUri );

    void CallRightsAvailable(
        DRM::MDrmHandleErrorObserver* aObserver,
        TInt aOperationId,
        TInt aError );


//*** Helper methods:

    // Get rights data for WM DRM content
    void GetRightsDataL(
        const TDesC& aUniqueId,
        TBool& aUnconstrained,
        TTimeIntervalSeconds& aTime,
        TInt& aCounts,
        TTimeIntervalSeconds& aAccumulated );

    // Get a rights manager instance
    ContentAccess::CRightsManager* GetRightsManagerL();

    // Rejection reason mapping from agent reasons to generic
    TInt RejectReason( TUint32 aReason );

    // Launch rights manager UI
    void LaunchRightsManagerUiL( const TDesC& aParam16 );

    // Form string needed to launch rights manager UI embedded.
    void CreateLaunchParamL( const TDesC* aUrl,
                             HBufC*& aLaunchParam );

    // Handles exit from service from MAknServerAppExitObserver
    void HandleServerAppExit( TInt aReason );

//*** Notes:

    // Show the appropriate notes
    void ShowNoRightsNoteL(
        ContentAccess::CData& aContent,
        TUint32 aReason,
        TInt aOperationId,
        DRM::MDrmHandleErrorObserver* aObserver );

//*** DLA:

    void LoadDlaWrapperL();
    void GetRFileFromCDataL( ContentAccess::CData& aContent, RFile& aFile );
    TBool IsDlaLicenseAcquisitionSilentL( RFile& aFile  );
    void DlaLicenseAcquisitionL( RFile& aFile );
    void SilentDlaLicenseAcquisitionL( ContentAccess::CData& aContent );
    TInt DefaultAccessPointL();

private: // Data members

    // Control environment, not owned
    CCoeEnv* iCoeEnv;

    // DrmUtilityCommon
    CDrmUtilityUI* iDrmUtilityUi;

    // Wait for the notes and queries
    CActiveSchedulerWait iWait;

    RFs iFs;

    RLibrary iDlaWrapperDll;
    CWmDrmDlaWrapper* iDlaWrapper;
    };
}


#endif // CDRMUTILITYWMDRMWRAPPER_H
