/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation class of DRM UI Handling
*
*/


#ifndef C_CDRMUIHANDLINGIMPL_H
#define C_CDRMUIHANDLINGIMPL_H

//*** system include files go here:
#include <e32base.h>
#include <f32file.h>
#include <AknServerApp.h>
#include <caf/caftypes.h>
#include <caf/caferr.h>
#include <drmagents.h>
#include <DRMRightsClient.h>
#include <drmutilitytypes.h>

//*** forward declarations go here:
class CCoeEnv;
class CDRMConstraint;

namespace ContentAccess
    {
    class CData;
    }

namespace DRM
{

    //*** forward declarations go here:
    class CDrmUtility;
    class CDrmUiHandlingData;
    class CDrmUtilityCommon;
    class CDrmUtilityUI;
    class MDrmAsyncObserver;
    class MDrmUiCheckRightsObserver;
    class MDrmHandleErrorObserver;
    class MDrmUtilityWMDrmWrapper;
    class CDrmUtilityWMDrm;

/**
 *  Implementation class for CDrmUiHandling class
 *
 *  @lib drmuihandlingimpl.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CDrmUiHandlingImpl ) : public CActive, public MAknServerAppExitObserver
    {

public:

    /** Error codes that are handled by HandleError */
    enum TDrmUiError
        {
        ENoPermission = KErrCANoPermission,
        ENoRights = KErrCANoRights,
        EPendingRights = KErrCAPendingRights
        };

public:

    /**
    * Two-phased constructor.
    */
    IMPORT_C static CDrmUiHandlingImpl* NewL( CDrmUtility* aDrmUtility, CCoeEnv* aCoeEnv = NULL );
    IMPORT_C static CDrmUiHandlingImpl* NewLC( CDrmUtility* aDrmUtility, CCoeEnv* aCoeEnv = NULL );

    /**
     * Destructor.
     */
    virtual ~CDrmUiHandlingImpl();

    /**
     * Check how much rights there are left for the content.
     * This method also displays appropriate notes, which observer can
     * override, if rights are invalid. Asynchronous version, all errors
     * are signalled using request status.
     *
     * @since S60 v5.0
     * @param[in]   aFile       file of which rights are checked.
     * @param[in]   aIntent     the CAF intent to be used for checking
     * @param[in]   aObserver   reference to observer
     *
     * @return Operation Identifier for the async request
     *         required for cancelling an operation and
     *         identifying which request has been completed
     *
     * @leave System wide error code
     *        KErrArgument  if file is not DRM protected.
     *
     * @see MDrmUiCheckRightsObserver
     */
    IMPORT_C TInt CheckRightsAmountAsyncL(
        RFile& aFile,
        ContentAccess::TIntent aIntent,
        MDrmUiCheckRightsObserver& aObserver );

     /**
     * Check how much rights there are left for the content.
     * This method also displays appropriate notes, which observer can
     * override, if rights are invalid. Synchronous version, leaves in case
     * of error.
     *
     * @since S60 v5.0
     * @param[in]   aFile       file of which rights are checked.
     * @param[in]   aIntent     the CAF intent to be used for checking
     * @param[in]   aObserver   pointer to observer, NULL if no observer
     *
     * @return none
     * @leave System wide error code
     *        KErrArgument  if file is not DRM protected.
     *
     * @see MDrmUiCheckRightsObserver
     */
    IMPORT_C void CheckRightsAmountL(
        RFile& aFile,
        ContentAccess::TIntent aIntent,
        MDrmUiCheckRightsObserver* aObserver );

    /**
     * Check how much rights there are left for the content.
     * This method also displays appropriate notes, which observer can
     * override, if rights are invalid. Asynchronous version, all errors
     * are signalled using request status.
     *
     * @since S60 v5.0
     * @param[in]   aFile       content of which rights are checked.
     * @param[in]   aIntent     the CAF intent to be used for checking
     * @param[in]   aObserver   reference to observer
     *
     * @return Operation Identifier for the async request
     *         required for cancelling an operation and
     *         identifying which request has been completed
     *
     * @leave System wide error code
     *        KErrArgument  if file is not DRM protected.
     *
     * @see MDrmUiCheckRightsObserver
     */
    IMPORT_C TInt CheckRightsAmountAsyncL(
        ContentAccess::CData& aFile,
        ContentAccess::TIntent aIntent,
        MDrmUiCheckRightsObserver& aObserver );

     /**
     * Check how much rights there are left for the content.
     * This method also displays appropriate notes, which observer can
     * override, if rights are invalid. Synchronous version, leaves in case
     * of error.
     *
     * @since S60 v5.0
     * @param[in]   aFile       content of which rights are checked.
     * @param[in]   aIntent     the CAF intent to be used for checking
     * @param[in]   aObserver   pointer to observer, NULL if no observer
     * @return none
     * @leave System wide error code
     *        KErrArgument  if file is not DRM protected.
     *
     * @see MDrmUiCheckRightsObserver
     */
    IMPORT_C void CheckRightsAmountL(
        ContentAccess::CData& aFile,
        ContentAccess::TIntent aIntent,
        MDrmUiCheckRightsObserver* aObserver );

    /**
     * Displays information about rights for given content.
     * Asynchronous version.
     *
     * @since S60 v5.0
     * @param[in]   aFile           file for which rights details are
     *                              displayed.
     * @param[in]   aObserver       reference to observer
     *
     * @return Operation Identifier for the async request
     *         required for cancelling an operation and
     *         identifying which request has been completed
     *
     * @leave   System wide error code
     */
    IMPORT_C TInt ShowDetailsViewAsyncL(
        RFile& aFile,
        MDrmAsyncObserver& aObserver );

    /**
     * Displays information about rights for given content.
     * Synchronous version, leaves in case of error.
     *
     * @since S60 v5.0
     * @param[in]   aFile       file for which rights details are
     *                          displayed.
     *
     * @leave   KErrArgument    File is not DRM protected.
     * @leave   KErrCANoRights  Rights object does not exist.
     *
     * @see caferr.h
     */
    IMPORT_C void ShowDetailsViewL(
        RFile& aFile );

    /**
     * Displays information about rights for given content.
     * Asynchronous version.
     *
     * @since S60 v5.0
     * @param[in]   aFile       content for which rights details are
     *                          displayed.
     * @param[in]   aObserver   reference to observer
     *
     * @return Operation Identifier for the async request
     *         required for cancelling an operation and
     *         identifying which request has been completed
     *
     * @leave System wide error code
     */
    IMPORT_C TInt ShowDetailsViewAsyncL(
        ContentAccess::CData& aFile,
        MDrmAsyncObserver& aObserver );

     /**
     * Displays information about rights for given content.
     * Synchronous version, leaves in case of error.
     *
     * @since S60 v5.0
     * @param[in]   aFile   content for which rights details are displayed.
     *
     * @leave   KErrArgument    Content is not DRM protected.
     * @leave   KErrCANoRights  Rights object does not exist.
     * @leave   System wide error code
     *
     * @see caferr.h
     */
    IMPORT_C void ShowDetailsViewL(
        ContentAccess::CData& aFile );

     /**
     * Handle the specific url defined by the file, such as InfoUrl
     * Asynchronous method
     *
     * @since S60 v5.0
     * @param[in]   aFile           file whose url is being handled
     * @param[in]   aType           type of the requested url,
     *                              only a single url may be requested
     *                              at a time
     * @param[in]   aObserver       reference to observer
     *
     * @return Operation Identifier for the async request
     *         required for cancelling an operation and
     *         identifying which request has been completed
     *
     * @leave   KErrArgument        File is not DRM protected.
     * @leave   KErrNotSupported    Url type is not supported for the file
     */
    IMPORT_C TInt HandleUrlAsyncL(
        RFile& aFile,
        TDrmUiUrlType aType,
        MDrmAsyncObserver& aObserver );


     /**
     * Handle the specific url defined by the file, such as InfoUrl
     *
     * @since S60 v5.0
     * @param[in]   aFile        file whose url is being handled
     * @param[in]   aType        type of the requested url,
     *                           only a single url may be requested
     *                           at a time
     *
     * @leave   KErrArgument     File is not DRM protected.
     * @leave   KErrNotSupported Url type is not supported for the file
     */
    IMPORT_C void HandleUrlL(
        RFile& aFile,
        TDrmUiUrlType aType );

     /**
     * Handle the specific url defined by the file, such as InfoUrl
     * Asynchronous method
     *
     * @since S60 v5.0
     * @param[in]   aFile           content whose url is being handled
     * @param[in]   aType           type of the requested url,
     *                              only a single url may be requested
     *                              at a time
     * @param[in]   aObserver       reference to observer
     *
     * @return Operation Identifier for the async request
     *         required for cancelling an operation and
     *         identifying which request has been completed
     *
     * @leave   KErrArgument        File is not DRM protected,
     *                              or multiple url:s were requested
     * @leave   KErrNotSupported    Url type is not supported for the file
     */
    IMPORT_C TInt HandleUrlAsyncL(
        ContentAccess::CData& aFile,
        TDrmUiUrlType aType,
        MDrmAsyncObserver& aObserver );

     /**
     * Handle the specific url defined by the file, such as InfoUrl
     *
     * @since S60 v5.0
     * @param[in]   aFile        content whose url is being handled
     * @param[in]   aType        type of the requested url,
     *                           only a single url may be requested
     *                           at a time
     *
     * @leave   KErrArgument     File is not DRM protected.
     * @leave   KErrNotSupported Url type is not supported for the file
     */
    IMPORT_C void HandleUrlL(
        ContentAccess::CData& aFile,
        TDrmUiUrlType aType );


     /**
     * Get information of the available urls
     * Asynchronous method
     *
     * @since S60 v5.0
     * @param[in]   aFile           file whose url is being handled
     * @param[out]  aType           A bitmask of the supported url types
     * @param[in]   aObserver       reference to observer
     *
     * @return Operation Identifier for the async request
     *         required for cancelling an operation and
     *         identifying which request has been completed
     *
     * @leave   KErrArgument        File is not DRM protected.
     */
    IMPORT_C TInt AvailableUrlsAsyncL(
        RFile& aFile,
        TDrmUiUrlType& aType,
        MDrmAsyncObserver& aObserver );

     /**
     * Get information of the available urls
     *
     * @since S60 v5.0
     * @param[in]   aFile           file whose url is being handled
     * @param[out]  aType           A bitmask of the supported url types
     *
     * @leave   KErrArgument        File is not DRM protected.
     */
    IMPORT_C void AvailableUrlsL(
        RFile& aFile,
        TDrmUiUrlType& aType );


     /**
     * Get information of the available urls
     * Asynchronous method
     *
     * @since S60 v5.0
     * @param[in]   aFile           content whose url is being handled
     * @param[out]  aType           A bitmask of the supported url types
     * @param[in]   aObserver       reference to observer
     *
     * @return Operation Identifier for the async request
     *         required for cancelling an operation and
     *         identifying which request has been completed
     *
     * @leave   KErrArgument        File is not DRM protected.
     */
    IMPORT_C TInt AvailableUrlsAsyncL(
        ContentAccess::CData& aFile,
        TDrmUiUrlType& aType,
        MDrmAsyncObserver& aObserver );

     /**
     * Get information of the available urls
     *
     * @since S60 v5.0
     * @param[in]   aFile           content whose url is being handled
     * @param[out]  aType           A bitmask of the supported url types
     *
     * @leave   KErrArgument        File is not DRM protected.
     */
    IMPORT_C void AvailableUrlsL(
        ContentAccess::CData& aFile,
        TDrmUiUrlType& aType );


    /**
     * Handles DRM error situations related to invalid rights.
     * This method displays appropriate notes, which observer can override,
     * to the user and acquires new rights if needed. Asynchronous version,
     * all errors are signalled using request status.
     *
     * @since S60 v5.0
     * @param[in]   aFile       file for which the error happened
     * @param[in]   aIntent     the CAF intent which was used to recieve
     *                          the error
     * @param[in]   aError      error code
     * @param[in]   aObserver   reference to observer
     *
     * @return Operation Identifier for the async request
     *         required for cancelling an operation and
     *         identifying which request has been completed
     *
     * @see MDrmHandleErrorObserver
     */
    IMPORT_C TInt HandleErrorAsyncL(
        RFile& aFile,
        ContentAccess::TIntent aIntent,
        TInt aError,
        MDrmHandleErrorObserver& aObserver );

     /**
     * Handles DRM error situations related to invalid rights.
     * This method displays appropriate notes, which observer can override,
     * to the user and acquires new rights if needed. Synchronous version,
     * leaves in case of error.
     *
     * @since S60 v5.0
     * @param[in]   aFile       file for which the error happened
     * @param[in]   aIntent     the CAF intent which was used to recieve
     *                          the error
     * @param[in]   aError      error code
     * @param[in]   aObserver   pointer to observer, NULL if no observer
     *
     * @leave   KErrArgument    File is not DRM protected.
     *
     * @see MDrmHandleErrorObserver
     */
    IMPORT_C void HandleErrorL(
        RFile& aFile,
        ContentAccess::TIntent aIntent,
        TInt aError,
        MDrmHandleErrorObserver* aObserver );

    /**
     * Handles DRM error situations related to invalid rights.
     * This method displays appropriate notes, which observer can override,
     * to the user and acquires new rights if needed. Asynchronous version,
     * all errors are signalled using request status.
     *
     * @since S60 v5.0
     * @param[in]   aFile       content for which the error happened
     * @param[in]   aIntent     the CAF intent which was used to recieve
     *                          the error
     * @param[in]   aError      error code
     * @param[in]   aObserver   reference to observer
     *
     * @return Operation Identifier for the async request
     *         required for cancelling an operation and
     *         identifying which request has been completed
     *
     * @see MDrmHandleErrorObserver
     */
    IMPORT_C TInt HandleErrorAsyncL(
        ContentAccess::CData& aFile,
        ContentAccess::TIntent aIntent,
        TInt aError,
        MDrmHandleErrorObserver& aObserver );

    /**
     * Handles DRM error situations related to invalid rights.
     * This method displays appropriate notes, which observer can override,
     * to the user and acquires new rights if needed. Synchronous version,
     * leaves in case of error.
     *
     * @since S60 v5.0
     * @param[in]   aFile       content for which the error happened
     * @param[in]   aIntent     the CAF intent which was used to recieve
     *                          the error
     * @param[in]   aError      error code
     * @param[in]   aObserver   pointer to observer, NULL if no observer
     *
     * @leave   KErrArgument    File is not DRM protected.
     *
     * @see MDrmHandleErrorObserver
     */
    IMPORT_C void HandleErrorL(
        ContentAccess::CData& aFile,
        ContentAccess::TIntent aIntent,
        TInt aError,
        MDrmHandleErrorObserver* aObserver );


    /**
     * Cancel an asyncronous operation
     *
     * @since S60 v5.0
     * @param[in]   aOperationId    identifier of the async operation
     *                              to be cancelled
     * @return KErrNotFound if the operation has already been executed
     *         or it does not exist
     */
    IMPORT_C TInt CancelOperation( TInt aOperationId );

public: //From base classes


protected: // Functions from base classes

    /**
    * From CActive Cancels async request.
    */
    void DoCancel();

    /**
    * From CActive Called when async request completes.
    */
    void RunL();

    /**
    * From CActive Called when RunL leaves
    */
    TInt RunError( TInt aError );

private:

    /**
    * C++ default constructor.
    */
    CDrmUiHandlingImpl( CDrmUtility* aDrmUtility, CCoeEnv* aCoeEnv );

    void ConstructL();

    void Activate( TRequestStatus*& aStatus );

    void PerformCheckRightsAmountFileL( CDrmUiHandlingData& aData );

    void PerformCheckRightsAmountDataL( CDrmUiHandlingData& aData );

    void PerformShowDetailsViewFileL( CDrmUiHandlingData& aData );

    void PerformShowDetailsViewDataL( CDrmUiHandlingData& aData );

    void PerformHandleUrlFileL( CDrmUiHandlingData& aData );

    void PerformHandleUrlDataL( CDrmUiHandlingData& aData );

    void PerformAvailableUrlsFileL( CDrmUiHandlingData& aData );

    void PerformAvailableUrlsDataL( CDrmUiHandlingData& aData );

    void PerformHandleErrorFileL( CDrmUiHandlingData& aData );

    void PerformHandleErrorDataL( CDrmUiHandlingData& aData );

    void IsProtectedL( RFile& aFile, TPtrC& aAgent );

    void IsProtectedL( ContentAccess::CData* aContent,
                       TDrmAgentUid& aAgentUid );

    void IsProperErrorL( TInt aError );

    void CheckOmaRightsAmountL( ContentAccess::CData& aContent,
                                ContentAccess::TIntent aIntent,
                                MDrmUiCheckRightsObserver* aObserver,
                                TInt aOperationId );

    void DoCheckOmaRightsAmountL( ContentAccess::CData& aContent,
                                  HBufC8*& aContentUri,
                                  ContentAccess::TIntent aIntent,
                                  const HBufC* aRightsUrl,
                                  TDrmUiUrlType aUrlType,
                                  const HBufC8* aSilentUrl,
                                  MDrmUiCheckRightsObserver* aObserver,
                                  TInt aOperationId );

    void HandleOmaErrorL( ContentAccess::CData& aContent,
                          ContentAccess::TIntent aIntent,
                          TInt aError,
                          MDrmHandleErrorObserver* aObserver,
                          TInt aOperationId );


    void CheckOmaBasedRightsAmountL( ContentAccess::CData& aContent,
                                     ContentAccess::TIntent aIntent,
                                     MDrmUiCheckRightsObserver* aObserver,
                                     TInt aOperationId );

    void DoCheckOmaBasedRightsAmountL( ContentAccess::CData& aContent,
                                       const HBufC8* aContentUri,
                                       ContentAccess::TIntent aIntent,
                                       const HBufC* aRightsUrl,
                                       TDrmUiUrlType aUrlType,
                                       MDrmUiCheckRightsObserver* aObserver,
                                       TInt aOperationId );

    void GetOmaBasedEndTimeL( ContentAccess::CData& aData, TTime& aEndTime );

    TInt OmaBasedHeaderParserL( TInt aOffset, TPtr8 aHeaderPtr, TTime& aEndTime );

    void ShowDetailsViewL( const HBufC8* aContentId,
                           const HBufC* aFullPath );

    void ShowDetailsViewL( const HBufC* aFullPath );

    void DoHandleOmaUrlL( ContentAccess::CData& aContent,
                          TDrmUiUrlType aType );

    void GetAvailableOmaUrlsL( ContentAccess::CData& aContent,
                               TDrmUiUrlType& aType );

    TInt GetContentIdLC( const ContentAccess::CData& aContent,
                         HBufC8*& aId );

    TInt GetOmaRightsIssuerLC( const ContentAccess::CData& aContent,
                              HBufC*& aIssuer );

    TInt GetOmaDomainRightsIssuerLC( const ContentAccess::CData& aContent,
                                    HBufC*& aIssuer );

    TInt GetOmaPreviewInfoLC( const ContentAccess::CData& aContent,
                             HBufC*& aPreviewUrl,
                             TInt& aPreviewType );

    TInt GetOmaSilentRightsUrlLC( const ContentAccess::CData& aContent,
                                  HBufC8*& aSilentUrl );

    TInt GetOmaInfoUrlLC( const ContentAccess::CData& aContent,
                         HBufC*& aInfoUrl );

    TInt GetOmaPendingRightsInfoLC( const ContentAccess::CData& aContent,
                                   HBufC*& aEta );

    TInt GetContentLocationLC( const ContentAccess::CData& aContent,
                               HBufC*& aPath );

    void SelectOmaRightsUrlL( const ContentAccess::CData& aContent,
                              const HBufC* aDomainRiUrl,
                              TDrmUiUrlType& aUrlType );

    void CallRightsNotValidL( MDrmUiCheckRightsObserver* aObserver,
                              TInt aOperationId,
                              TCheckRightsStatus aRightsStatus,
                              TUint32 aReason,
                              ContentAccess::CData& aContent,
                              const HBufC8* aContentUri,
                              CDRMConstraint* aConstraint,
                              const HBufC* aRightsUrl,
                              TDrmUiUrlType aUrlType,
                              const HBufC8* aSilentUrl,
                              ContentAccess::TIntent aIntent );

    void CallRightsLeftL( MDrmUiCheckRightsObserver* aObserver,
                          TInt aOperationId,
                          TBool aUnconstrained,
                          TTimeIntervalSeconds aTime,
                          TInt aCounts,
                          TTimeIntervalSeconds aAccumulated,
                          const HBufC8* aContentUri,
                          const HBufC* aRightsUrl,
                          const HBufC8* aSilentUrl,
                          CData& aData,
                          ContentAccess::TIntent aIntent );

    void CallEmbeddedPreviewAvailableL( MDrmHandleErrorObserver* aObserver,
                                        TInt aOperationId,
                                        ContentAccess::CData& aContent,
                                        const HBufC* aPreviewUrl,
                                        TDrmRightsInfo aRightsStatus,
                                        TUint32 aReason,
                                        const HBufC* aRightsUrl,
                                        const HBufC8* aContentUri,
                                        ContentAccess::TIntent aIntent );

    void CallPreviewRightsUrlAvailableL( MDrmHandleErrorObserver* aObserver,
                                         TInt aOperationId,
                                         ContentAccess::CData& aContent,
                                         const HBufC* aPreviewUrl,
                                         TDrmRightsInfo aRightsStatus,
                                         TUint32 aReason,
                                         const HBufC* aRightsUrl,
                                         const HBufC8* aContentUri,
                                         ContentAccess::TIntent aIntent );

    void CallSilentRightsUrlAvailableL( MDrmHandleErrorObserver* aObserver,
                                        TInt aOperationId,
                                        const HBufC8* aSilentUrl,
                                        TDrmRightsInfo aRightsStatus,
                                        TUint32 aReason,
                                        const HBufC* aRightsUrl,
                                        const HBufC8* aContentUri,
                                        ContentAccess::TIntent aIntent );

    void CallRightsUrlAvailableL( MDrmHandleErrorObserver* aObserver,
                                  TInt aOperationId,
                                  TDrmRightsInfo aRightsStatus,
                                  TUint32 aReason,
                                  CDRMConstraint* aConstraint,
                                  ContentAccess::CData& aContent,
                                  const HBufC8* aContentUri,
                                  const HBufC* aRightsUrl,
                                  TDrmUiUrlType aUrlType,
                                  ContentAccess::TIntent aIntent,
                                  TInt aError );

    void CallPreviewRightsAvailable( MDrmHandleErrorObserver* aObserver,
                                     TInt aOperationId,
                                     TInt aError );

    void CallSilentRightsAvailable( MDrmHandleErrorObserver* aObserver,
                                    TInt aOperationId,
                                    TInt aError );

    void CallRightsAvailable( MDrmHandleErrorObserver* aObserver,
                              TInt aOperationId,
                              TInt aError );

    void CallPlayEmbeddedPreviewSelected( MDrmHandleErrorObserver* aObserver,
                                          TInt aOperationId,
                                          const HBufC* aPreviewUrl );

    /**
    * @return ETrue if rights renewal wanted, EFalse  otherwise
    */
    TBool ShowNoRightsNoteL( ContentAccess::CData& aContent,
                             TDrmRightsInfo aRightsStatus,
                             TUint32 aReason,
                             CDRMConstraint* aConstraint,
                             const HBufC* aRightsUrl,
                             TDrmUiUrlType aUrlType,
                             TInt aError );

    TInt GetOmaConstraintInfo( CDRMConstraint* aConstraint,
                               TUint32& aCountsLeft,
                               TTime& aEndTime );

    TInt RejectReason( TUint32 aReason );

    TDrmRightsInfo RightsInfoStatus( TCheckRightsStatus aStatus );

    void CheckOmaDomainStatusL( const ContentAccess::CData& aContent,
                                const HBufC* aDomainRiUrl,
                                TInt& aIsRegistered,
                                TInt& aIsJoined );

    /**
    * Launch rights manager UI
    */
    void LaunchRightsManagerUiL( const TDesC& aParam16 );

    /**
    * Form string needed to launch rights manager UI embedded.
    */
    void CreateLaunchParamL( const HBufC8* aUrl,
                             HBufC*& aLaunchParam,
                             const HBufC* aFullPath );

    void CreateLaunchParamL( const HBufC* aFullPath,
                             HBufC*& aLaunchParam );

    /**
    * Launch browser. Return ETrue if embedded launch
    */
    TBool LaunchBrowserL( const HBufC8* aUrl );
    TBool LaunchBrowserL( const HBufC* aUrl );

    /**
    * Get rights silently, if notes are not intended to be shown, put aShowNotes to EFalse
    */
    TInt GetSilentRightsL( const TDesC8& aUrl, const TBool aShowNotes );

    /**
    * Check if the user has allowed silent rights
    */
    TBool SilentRightsAllowedL();

    /**
    * Handles exit from service
    * from MAknServerAppExitObserver
    */
    void HandleServerAppExit(TInt aReason);

    /**
     * Appends new async request data to async operation queue.
     *
     * @param[in]   aData: The data set in/out.
     * @return none
     */
    void AppendToQueue( CDrmUiHandlingData* aData );

    /**
     * Gets the next async request data from async operation queue
     *
     * @param[in]   none
     * @return next request data
     */
    CDrmUiHandlingData* PopFront();

    void GetOmaBasedInfoL();

    /**
    * Gets SilentRights and
    * checks status after
    * CallSilentAvailble after right
    * @return Etrue, if silent RO rerieval led to usable content
    */
    TBool GetSilentAndCallL(
        MDrmHandleErrorObserver* aObserver,
        TInt aOperationId,
        const ContentAccess::TIntent aIntent,
        const HBufC8* aContentUri,
        const HBufC8* aSilentUrl,
        const TBool aShowNotes );

    /**
    * Gets Rights and
    * checks status if launched embedded.
    * CallRightsAvailable, if launched embedded or righsUrl not present
    */
    void GetRightsAndCallL(
        MDrmHandleErrorObserver* aObserver,
        TInt aOperationId,
        const ContentAccess::TIntent aIntent,
        const HBufC8* aContentUri,
        const HBufC* aRightsUrl );
private: // data

    // Control environment
    CCoeEnv* iCoeEnv;

    // DrmUtility
    CDrmUtility* iDrmUtility;

    // DrmUtilityCommon
    CDrmUtilityCommon* iDrmUtilityCommon;

    // DrmUtilityUI
    CDrmUtilityUI* iDrmUtilityUi;

    // Oma Drm client
    RDRMRightsClient iOmaClient;

    // WM Drm Wrapper - Owned
    CDrmUtilityWMDrm* iWrapperLoader;

    // WM Drm Client - Not Owned
    MDrmUtilityWMDrmWrapper* iWmClient;

    // Async observer and current operation id for async operations
    MDrmAsyncObserver* iObserver;
    TInt iOperationId;

    // Wait for the notes and queries
    CActiveSchedulerWait iWait;

    // Operation Queue:
    CDrmUiHandlingData* iFirst;
    CDrmUiHandlingData* iLast;

    // To prevent queue handling from being messed up by threads
    RSemaphore iSemaphore;

    // Oma based extensions
    HBufC* iOmaBasedAgentName;
    TInt iOmaBasedUid;
    HBufC8* iOmaBasedMimeType;

    };

}

#endif // C_CDRMUIHANDLINGIMPL_H
