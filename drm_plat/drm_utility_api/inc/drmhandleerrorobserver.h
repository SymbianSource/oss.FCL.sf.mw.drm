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
* Description:  Observer interface for HandleError
 *
*/


#ifndef MDRMHANDLEERROROBSERVER_H
#define MDRMHANDLEERROROBSERVER_H

#include <drmasyncobserver.h>
#include <drmutilitytypes.h>

namespace DRM
    {

    /**
     *  An interface containing callbacks for CDrmUiHandling::HandleError().
     *
     *  By implementing these methods observer can get more information about
     *  what kind of rights are available and can also override the default
     *  behaviour.
     *
     *  @code
     *      #include <DrmUiHandling.h>
     *      #include <DrmHandleErrorObserver.h>
     *
     *      using namespace DRM;
     *
     *      // Call NewLC() to create an instance of CDrmUiHandling.
     *      CDrmUiHandling* drmUiHandler = CDrmUiHandling::NewLC();
     *
     *      // Handling the error got when tried to read the file
     *      drmHandlerError = drmUiHandler->HandleErrorL( file, error, this );
     *
     *      // delete the created instance of CDrmUiHandling
     *      CleanupStack::PopAndDestroy( drmUiHandler );
     *
     *      ...
     *
     *      // implement callbacks
     *      TEmbeddedPreviewAction CMyClass::EmbeddedPreviewAvailable( const TDesC& aUniqueId )
     *          {
     *          return EEmbeddedPreviewActionDefault;
     *          }
     *
     *      TPreviewRightsAction CMyClass::PreviewRightsUrlAvailable( const TDesC& aPreviewRightsUrl )
     *          {
     *          return EPreviewRightsActionDefault;
     *          }
     *
     *      TSilentRightsAction CMyClass::SilentRightsUrlAvailable( const TDesC& aSilentRightsUrl )
     *          {
     *          return ESilentRightsActionDefault;
     *          }
     *
     *      void CMyClass::PreviewRightsAvailable()
     *          {
     *          // play the content, since preview rights were retrieved
     *          }
     *
     *      void CMyClass::SilentRightsAvailable()
     *          {
     *          // play the content, since silent rights were retrieved
     *          }
     *
     *      void CMyClass::PlayEmbeddedPreviewSelected( const TDesC& aUniqueId )
     *          {
     *          // play embedded preview part
     *          ...
     *          }
     *  @endcode
     *
     *  @since S60 v5.0
     *  @see CDrmUiHandling::HandleError
     */
class MDrmHandleErrorObserver : public MDrmAsyncObserver
    {

public:

    /**
     * Called when content has embedded preview part. If
     * EEmbeddedPreviewActionDefault is returned,
     * PlayEmbeddedPreviewSelected is called if user selects to play
     * embedded preview.
     *
     * @since S60 v5.0
     * @param[in] aOperationId  Identifier of the asynchronous operation
     *                          which was returned when the asynchronous
     *                          operation was initiated, Zero(0) if it
     *                          is a call back from a synchronous
     *                          operation
     * @param[in]   aUniqueId   Unique ID of embedded preview part.
     * @param[in]   aRightsStatus Status of the rights as defined per
     *                            TDrmRightsInfo
     * @param[in]   aReason     Rejection reason, a bitvector which
     *                          states for what reasons rights have
     *                          been rejected from use using
     *                          TRejectionType
     *
     * @return  EEmbeddedPreviewActionDefault DRM Utility asks if user
     *          wants to play preview or acquire new rights
     *          EUEmbeddedPreviewActionDefaultAudio DRM Utility asks if user
     *          wants to play preview or acquire new rights for audio
     *          EUEmbeddedPreviewActionDefaultVideo DRM Utility asks if user
     *          wants to play preview or acquire new rights for video
     *          EEmbeddedPreviewActionFetchRights  DRM Utility acquires
     *          new rights, no queries shown by DRM Utility
     *          EEmbeddedPreviewActionPlayPreview  Observer takes care
     *          of playing embedded preview
     */
    virtual TEmbeddedPreviewAction EmbeddedPreviewAvailable(
            TInt aOperationId,
            const TDesC& aUniqueId,
            TInt aRightsStatus,
            TInt aReason ) = 0;

    /**
     * Called if preview rights can be acquired for content.
     *
     * @since S60 v5.0
     * @param[in] aOperationId  Identifier of the asynchronous operation
     *                          which was returned when the asynchronous
     *                          operation was initiated, Zero(0) if it
     *                          is a call back from a synchronous
     *                          operation
     * @param[in]   aPreviewRightsUrl   URL from where preview rights can
     *                                  be fetched.
     * @param[in]   aRightsStatus Status of the rights as defined per
     *                            TDrmRightsInfo
     * @param[in]   aReason     Rejection reason, a bitvector which
     *                          states for what reasons rights have
     *                          been rejected from use using
     *                          TRejectionType
     *
     * @return  EPreviewRightsActionDefault DRM Utility asks if user
     *          wants to acquire preview rights or normal rights
     *          EUPreviewRightsActionDefaultAudio DRM Utility asks if user
     *          wants to acquire preview rights or normal rights for audio and 
     *          acquires them
     *          EUPreviewRightsActionDefaultVideo DRM Utility asks if user 
     *          wants to acquire preview rights or normal rights for video and
     *          acquires them
     *          EPreviewRightsActionFetchRights DRM Utility acquires normal
     *          rights, no queries shown by DRM utility
     *          EPreviewRightsActionFetchPreviewRights Observer takes care
     *          of acquiring preview rights
     */
    virtual TPreviewRightsAction PreviewRightsUrlAvailable(
            TInt aOperationId,
            const TDesC& aPreviewRightsUrl,
            TInt aRightsStatus,
            TInt aReason ) = 0;

    /**
     * Called if silent rights can be acquired for content.
     *
     * @since S60 v5.0
     * @param[in] aOperationId  Identifier of the asynchronous operation
     *                          which was returned when the asynchronous
     *                          operation was initiated, Zero(0) if it
     *                          is a call back from a synchronous
     *                          operation
     * @param[in]   aSilentRightsUrl    URL from where silent rights can
     *                                  be fetched.
     * @param[in]   aRightsStatus Status of the rights as defined per
     *                            TDrmRightsInfo
     * @param[in]   aReason     Rejection reason, a bitvector which
     *                          states for what reasons rights have
     *                          been rejected from use using
     *                          TRejectionType
     *
     * @return  ESilentRightsActionDefault DRM Utility performs the
     *          default silent rights handling
     *          ESilentRightsActionFetchRights DRM Utility acquires normal
     *          rights
     *          ESilentRightsActionFetchSilentRights   Observer takes care
     *          of acquiring silent rights
     */
    virtual TSilentRightsAction SilentRightsUrlAvailable(
            TInt aOperationId,
            const TDesC& aSilentRightsUrl,
            TInt aRightsStatus,
            TInt aReason ) = 0;

    /**
     * Called if only rights-issuer URL or domain rights-issuer URL is
     * available.
     *
     * @since S60 v5.0
     * @param[in] aOperationId  Identifier of the asynchronous operation
     *                          which was returned when the asynchronous
     *                          operation was initiated, Zero(0) if it
     *                          is a call back from a synchronous
     *                          operation
     * @param[in]   aRightsUrl          URL from where rights can
     *                                  be fetched.
     * @param[in]   aRightsStatus Status of the rights as defined per
     *                            TDrmRightsInfo
     * @param[in]   aReason     Rejection reason, a bitvector which
     *                          states for what reasons rights have
     *                          been rejected from use using
     *                          TRejectionType
     * @param[in]   aUrlType    The type of url available in TDrmUiUrlType
     *
     * @return  ERightsActionDefault DRM Utility performs the
     *          default rights handling
     *          ERightsActionFetchRights DRM Utility acquires normal
     *          rights
     */
    virtual TRightsAction RightsUrlAvailable(
            TInt aOperationId,
            const TDesC& aRightsUrl,
            TInt aRightsStatus,
            TInt aReason,
            TInt aUrlType ) = 0;

    /**
     * Called when preview rights have been acquired for content.
     *
     * @since S60 v5.0
     * @param[in] aOperationId  Identifier of the asynchronous operation
     *                          which was returned when the asynchronous
     *                          operation was initiated, Zero(0) if it
     *                          is a call back from a synchronous
     *                          operation
     * @param[in] aError        Return status of the requested URI type
     *                          KErrNone if successful and rights are
     *                          available
     *                          KErrCANoRights if the rights are not
     *                          available
     *                          KErrNotFound if the requested URI type was
     *                          not available
     */
    virtual void PreviewRightsAvailable(
            TInt aOperationId,
            TInt aError ) = 0;

    /**
     * Called when silent rights have been acquired for content.
     *
     * @since S60 v5.0
     * @param[in] aOperationId  Identifier of the asynchronous operation
     *                          which was returned when the asynchronous
     *                          operation was initiated, Zero(0) if it
     *                          is a call back from a synchronous
     *                          operation
     * @param[in] aError        Return status of the requested URI type
     *                          KErrNone if successful and rights are
     *                          available
     *                          KErrCANoRights if the rights are not
     *                          available
     *                          KErrNotFound if the requested URI type was
     *                          not available
     */
    virtual void SilentRightsAvailable(
            TInt aOperationId,
            TInt aError ) = 0;

    /**
     * Called when rights have been acquired for content.
     *
     * @param[in] aOperationId  Identifier of the asynchronous operation
     *                          which was returned when the asynchronous
     *                          operation was initiated, Zero(0) if it
     *                          is a call back from a synchronous
     *                          operation
     * @param[in] aError        Return status of the requested URI type
     *                          KErrNone if successful and rights are
     *                          available
     *                          KErrCANoRights if the rights are not
     *                          available
     *                          KErrNotFound if the requested URI type was
     *                          not available
     * @since S60 v5.0
     */
    virtual void RightsAvailable(
            TInt aOperationId,
            TInt aError ) = 0;

    /**
     * Called if user selected to play embedded preview part.
     *
     * @since S60 v5.0
     * @param[in] aOperationId  Identifier of the asynchronous operation
     *                          which was returned when the asynchronous
     *                          operation was initiated, Zero(0) if it
     *                          is a call back from a synchronous
     *                          operation
     * @param[in]   aUniqueId   Unique ID of embedded preview part.
     */
    virtual void PlayEmbeddedPreviewSelected(
            TInt aOperationId,
            const TDesC& aUniqueId ) = 0;

protected:

    };
    }

#endif // MDRMHANDLEERROROBSERVER_H
