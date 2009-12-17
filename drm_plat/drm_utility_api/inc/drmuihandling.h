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
* Description:  Utility API for DRM related UI
 *
*/


#ifndef C_CDRMUIHANDLING_H
#define C_CDRMUIHANDLING_H

#include <e32base.h>
#include <f32file.h>
#include <caf/caftypes.h>
#include <drmutilitytypes.h>

//*** forward declarations go here:
namespace ContentAccess
    {
    class CData;
    }

class CCoeEnv;

namespace DRM
    {

    //*** forward declarations go here:
    class CDrmUtility;
    class MDrmErrorHandling;
    class MDrmAsyncObserver;
    class MDrmHandleErrorObserver;
    class MDrmUiCheckRightsObserver;
    class CDrmUiHandlingImpl;

    /**
     *  Utility class for DRM related UI
     *
     *  By using this class an application can resolve situation where there is
     *  no valid rights for the content, check status of rights and display
     *  an embedded rights details view.
     *
     *  Usage:
     *  @code
     *      #include <DrmUiHandling.h>
     *
     *      using namespace DRM;
     *
     *      // Call NewLC() to create an instance of CDrmUiHandling.
     *      CDrmUiHandling* drmUiHandler = CDrmUiHandling::NewLC();
     *
     *      // Handling the error got when tried to read the file
     *      drmHandlerError = drmUiHandler->HandleErrorL( file, error, NULL );
     *
     *      // checking the status of the rights
     *      drmHandlerError = drmUiHandler->CheckRightsAmountL( file, NULL );
     *
     *      // opening embedded details view
     *      drmHandlerError = drmUiHandler->ShowDetailsViewL( file );
     *
     *      // delete the created instance of CDrmUiHandling
     *      CleanupStack::PopAndDestroy( drmUiHandler );
     *  @endcode
     *
     *  @lib drmutilityuihandling.lib
     *  @since S60 v5.0
     */
NONSHARABLE_CLASS( CDrmUiHandling ) : public CBase
    {

public:

    /**
     * Creates a new CDrmUiHandling object and returns a pointer to it
     *
     * @param aCoeEnv   A pointer to an instance of CCoeEnv. If the
     *                  parameter is not provided the global instance
     *                  will be used. In the case of a server process
     *                  where the global instance is not available,
     *                  global notes will be used.
     *
     *                  If no CCoeEnv instance is available, applications
     *                  launched will be launched as standalone
     *                  applications.
     *
     * @return A functional CDrmUiHandling -object
     *
     * @leave System wide error code
     */
    IMPORT_C static CDrmUiHandling* NewL( CCoeEnv* aCoeEnv = NULL );

    /**
     * Creates a new CDrmUiHandling object and returns a pointer to it
     * Leaves the pointer to the cleanup stack
     *
     * @param aCoeEnv   A pointer to an instance of CCoeEnv. If the
     *                  parameter is not provided the global instance
     *                  will be used. In the case of a server process
     *                  where the global instance is not available,
     *                  global notes will be used.
     *
     *                  If no CCoeEnv instance is available, applications
     *                  launched will be launched as standalone
     *                  applications.
     *
     * @return A functional CDrmUiHandling -object
     *
     * @leave System wide error code
     */

    IMPORT_C static CDrmUiHandling* NewLC( CCoeEnv* aCoeEnv = NULL );

    /**
     * Destructor
     */
    virtual ~CDrmUiHandling();


    /**
     * Returns a reference to a CDrmUtility instance. The ownership
     * of the instance stays with the CDrmUiHandling -class
     *
     * @since S60 v5.0
     *
     * @return A reference to a functional CDrmUtility instance
     *
     * @leave None
     *
     * @see CDrmUtility
     */
    IMPORT_C CDrmUtility& GetUtility() const;


    /**
     * Returns a reference to a class which implements the
     * MDrmErrorHandling interface. The ownership
     * of the instance stays with the CDrmUiHandling -class
     *
     * @since S60 v5.0
     *
     * @return A reference to a functional object implementing the
     *         MDrmErrorHandling interface
     *
     * @leave None
     *
     * @see MDrmErrorHandling
     */
    IMPORT_C MDrmErrorHandling& GetErrorHandler() const;


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
     * @see ContentAccess::TIntent
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
     * @see ContentAccess::TIntent
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
     * @see ContentAccess::TIntent
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
     * @see ContentAccess::TIntent
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
    IMPORT_C TInt ShowDetailsViewAsyncL( RFile& aFile,
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
    IMPORT_C void ShowDetailsViewL( RFile& aFile );

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
    IMPORT_C TInt ShowDetailsViewAsyncL( ContentAccess::CData& aFile,
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
    IMPORT_C void ShowDetailsViewL( ContentAccess::CData& aFile );


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
     * @param[in]   aType           type of the requested url,
     *                              only a single url may be requested
     *                              at a time
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
     * @param[in]   aType           type of the requested url,
     *                              only a single url may be requested
     *                              at a time
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
     * Cancel an asyncronous operation
     *
     * @since S60 v5.0
     * @param[in]   aOperationId    identifier of the async operation
     *                              to be cancelled
     * @return KErrNotFound if the operation has already been executed
     *         or it does not exist
     */
    IMPORT_C TInt CancelOperation( TInt aOperationId );


private:
    /**
     * Default Constructor
     */
    CDrmUiHandling();

    /**
     * 2nd phase constructor
     */
    void ConstructL( CCoeEnv* aCoeEnv );

private: // data

    /**
     * Pointer to the implementation
     * Owned
     */
    CDrmUiHandlingImpl* iImplementation;

    /**
     * Pointer to a CDrmUtility -Instance
     * Owned
     */
    CDrmUtility* iUtility;

    /**
     * Pointer to an MDrmErrorHandling interface class instance
     * Owned
     */
    MDrmErrorHandling* iErrorHandler;

    };
    }



#endif // C_CDRMUIHANDLING_H
