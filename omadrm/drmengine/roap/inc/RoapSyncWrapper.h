/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  synchronous wrapper for ROAP
*
*/



#ifndef CROAPSYNCWRAPPER_H
#define CROAPSYNCWRAPPER_H

#include <e32base.h>
#include <RoapObserver.h>

/**
*  Synchronous wrapper for ROAP protocol suite.
*
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS(CRoapSyncWrapper) : public CActive, public Roap::MRoapObserver
    {
public:

    IMPORT_C static CRoapSyncWrapper* NewL();
    IMPORT_C virtual ~CRoapSyncWrapper();

    /**
    * Handle a ROAP trigger synchronously
    *
    * @since S60 3.1
    * @param aTrigger ROAP Trigger
    */
    IMPORT_C void HandleTriggerL( const TDesC8& aTrigger );



    /**
    * Handle a ROAP trigger synchronously without user interaction
    *
    * @since S60 3.1
    * @param aTrigger ROAP Trigger
    * @return EFalse if the trigger cannot be handled silently, e.g. no RI
    *         context exists
    */
    IMPORT_C TBool HandleTriggerSilentlyL( const TDesC8& aTrigger );

    /**
    * Handle ROAP error
    *
    * @since S60 3.1
    * @param aError Error value returned from HandleTriggerL
    * @param aErrorUrl Buffer for error url
    */
    IMPORT_C HBufC8* GetErrorUrlL( const TInt aError, TInt& aErrorType );

    /**
    * Getter for ROAP metering postResponseUrl
    *
    * @since S60 3.2
    * @return post reponse URL
    */
    IMPORT_C HBufC8* GetPostResponseUrlL();


    // From CActive.

    void DoCancel();
    void RunL();

    // From MRoapObserver

    TBool ConnectionConfL();
    TBool ContactRiConfL();
    TBool TransIdConfL();
    void RightsObjectDetailsL( const RPointerArray<CDRMRights>& aRightsList );
    void ContentDownloadInfoL( TPath& aTempFolder, TFileName& aContentName, TInt& aMaxSize );
    void ContentDetailsL( const TDesC& aPath, const TDesC8& aType,const TUid& aAppUid );
    void RoapProgressInfoL( const TInt aProgressInfo );
    void ErrorUrlL( const TDesC8& aErrorUrl );
    void PostResponseUrlL( const TDesC8& aPostResponseUrl );

private:

    CRoapSyncWrapper();
    void ConstructL();

private:

    // Used to make asynchronous call synchronous
    CActiveSchedulerWait    iWait;
    HBufC8* iErrorUrl;
    HBufC8* iPostResponseUrl;
    };

#endif      // CROAPSYNCWRAPPER_H

// End of File
