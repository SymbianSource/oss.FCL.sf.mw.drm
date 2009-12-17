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
* Description:  Observer interface for CheckRightsAmount
 *
*/


#ifndef MDRMUICHECKRIGHTSOBSERVER_H
#define MDRMUICHECKRIGHTSOBSERVER_H

#include <drmhandleerrorobserver.h>
#include <drmutilitytypes.h>

namespace DRM
    {

    /**
     *  An interface containing callbacks for CDrmUiHandling::CheckRightsAmount()
     *
     *  By implementing these methods observer can get more information about
     *  what kind of rights content has and can also override the default
     *  behaviour.
     *
     *  @code
     *      #include <DrmUiHandling.h>
     *      #include <DrmUiCheckRightsObserver.h>
     *
     *      using namespace DRM;
     *
     *      // Call NewLC() to create an instance of CDrmUiHandling.
     *      CDrmUiHandling* drmUiHandler = CDrmUiHandling::NewLC();
     *
     *      // Handling the error got when tried to read the file
     *      drmHandlerError = drmUiHandler->CheckRightsAmountL( file, this );
     *
     *      // delete the created instance of CDrmUiHandling
     *      CleanupStack::PopAndDestroy( drmUiHandler );
     *
     *      ...
     *
     *      // implement callbacks
     *      TCheckRightsAction CMyClass::RightsLeft( TInt aDays, TInt aCounts, TInt aAccumulatedMinutes )
     *          {
     *          return ECheckRightsActionDefault;
     *          }
     *
     *      TCheckRightsAction CMyClass::RightsNotValid( TCheckRightsStatus aRightsStatus )
     *          {
     *          return ECheckRightsActionDefault;
     *          }
     *
     *  @endcode
     *
     *  @since S60 V5.0
     *  @see CDrmUiHandling::CheckRightsAmount
     */
class MDrmUiCheckRightsObserver : public MDrmHandleErrorObserver
    {

public:

    /**
     * Tells how much rights are left
     *
     * @since S60 v5.0
     * @param[in] aOperationId     Identifier of the asynchronous operation
     *                             which was returned when the asynchronous
     *                             operation was initiated, Zero(0) if it
     *                             is a call back from a synchronous
     *                             operation
     * @param[in] aUnconstrained   ETrue, if the content either has no
     *                             constraints or if the content is only
     *                             for example start time based. Or the
     *                             time left is so long that it does not
     *                             fit to the aTime parameter's
     *                             TTimeIntervalSeconds otherwise EFalse.
     *
     *                             This parameter needs to always be
     *                             checked first, if the value is ETrue
     *                             the other parameters: aTime, aCounts and
     *                             aAccumulated should be ignored.
     *
     * @param[in] aTime            Time left in seconds. Zero(0) if
     *                             the content does not have count
     *                             constraint
     * @param[in] aCounts          Counts left. Zero(0) if content
     *                             does not have count constraint.
     * @param[in] aAccumulated     Accumulated time left in seconds.
     *                             Zero(0) if content does not have
     *                             accumulated time constraint.
     *
     * @return ECheckRightsActionDefault DRM Utility performs the default
     *         handling which is the handling specified in the S60 UI
     *         specification
     *         ECheckRightsActionOpenDetailsView Details view is opened,
     *         no notes shown by DRM Utility
     *         ECheckRightsActionAcquireNewRights New rights are acquired,
     *         no notes shown by DRM Utility
     *         ECheckRightsActionIgnore DRM Utility does not do anything
     */
    virtual TCheckRightsAction RightsLeft(
            TInt aOperationId,
            TBool aUnconstrained,
            TTimeIntervalSeconds aTime,
            TInt aCounts,
            TTimeIntervalSeconds aAccumulated ) = 0;

    /**
     * Called if there are no valid rights.
     *
     * @since S60 v5.0
     * @param[in] aOperationId      Identifier of the asynchronous operation
     *                              which was returned when the asynchronous
     *                              operation was initiated, Zero(0) if it
     *                              is a call back from a synchronous
     *                              operation
     * @parem[in] aRightsStatus     The reason why rights are invalid
     * @param[in] aReason           Rejection reason, a bitvector which
     *                              states for what reasons rights have
     *                              been rejected from use using
     *                              TRejectionType
     *
     * @return ECheckRightsActionDefault DRM Utility performs the default
     *         handling which is the handling specified by the S60 UI
     *         specification
     *         ECheckRightsActionOpenDetailsView Details view is opened,
     *         no notes shown by DRM Utility
     *         ECheckRightsActionAcquireNewRights New rights are acquired,
     *         no notes shown by DRM Utility
     *         ECheckRightsActionIgnore DRM Utility does not do anything
     */
    virtual TCheckRightsAction RightsNotValid(
            TInt aOperationId,
            TCheckRightsStatus aRightsStatus,
            TInt aReason ) = 0;

protected:

    };
    
    }

#endif // MDRMUICHECKRIGHTSOBSERVER_H
