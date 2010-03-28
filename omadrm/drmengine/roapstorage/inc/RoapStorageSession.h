/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class declares the interface of class CRoapStorageSession.
*                handles the queues for notification events
*
*/


#ifndef ROAP_STORAGE_SESSION_H
#define ROAP_STORAGE_SESSION_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
// MACROS
// FUNCTION PROTOTYPES
// DATA TYPES

// FORWARD DECLARATIONS
class RMessage2;
class MDrmKeyStorage;
class LazyClockClient;

// CLASS DECLARATION

/**
*  This class is the server side instance of C/S communications of a certain
*  session.

*  @since 3.0
*/
NONSHARABLE_CLASS( CRoapStorageSession ): public CSession2
{
    enum TPanic
        {
        EPanicBadDescriptor, EPanicBadFunction
        };
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @param aClient Associated client side thread.
     * @return New session instance.
     */
    static CRoapStorageSession* NewL();

    /**
     * Destructor.
     * @since 3.0
     */
    virtual ~CRoapStorageSession();

public:
    // Functions from base classes

   /**
     * From CSession2: Handles the service request event.
     * @since 3.0
     * @param aMessage The message related to the event.
     */
    void ServiceL( const RMessage2& aMessage );

    /**
     * Utility function for ServiceL
     * @since 3.0
     * @param aMessage The message related to the event.
     * @exception Method leaves with appropriate exception value
     *            if any errors occured.
     */
    void DispatchL( const RMessage2& aMessage );

    /**
     * From CSession2: Handles ServiceL errors
     * @since 3.0
     */
    void ServiceError( const RMessage2& aMessage, TInt aError );

private:

    /**
     * C++ constructor.
     * @since 3.0
     * @param aClient Client thread.
     */
    CRoapStorageSession();

    /**
     * Second phase constructor.
     * @since 3.0
     */
    void ConstructL();

    static void PanicClient( const RMessage2& aMessage, TPanic aReason );

    void AddRiContextL( const RMessage2& aMessage );

    void AddDomainContextL( const RMessage2& aMessage );

    void GetRiContextL( const RMessage2& aMessage );

    void GetDomainContextL( const RMessage2& aMessage );

    void GetDataL( const RMessage2& aMessage );

    void DeleteRiContextL( const RMessage2& aMessage );

    void DeleteDomainContextL( const RMessage2& aMessage );

    void DeleteExpiredRIsL( const RMessage2& aMessage );

    void DeleteExpiredDomainsL( const RMessage2& aMessage );

    void WhiteListUrlExistsL( const RMessage2& aMessage );

    void GetDevicePublicKeyDerL( const RMessage2& aMessage );

    void GetDeviceCertificateChainL( const RMessage2& aMessage );

    void SignL( const RMessage2& aMessage );

    void SelectTrustedRootL( const RMessage2& aMessage );

    void ActivateTrustedRootL( const RMessage2& aMessage );

    void GetTrustedRootsL( const RMessage2& aMessage );

    void GetRootCertificateL( const RMessage2& aMessage );

    void DeleteExpiredContextsL( const RMessage2& aMessage );

    void DeleteAllL( const RMessage2& aMessage );

    void RsaSignL( const RMessage2& aMessage );

    void GetMeteringDataL( const RMessage2& aMessage );

    void DeleteMeteringDataL( const RMessage2& aMessage );

    void GetAllRIContextsL( const RMessage2& aMessage );

    void UpdateRIContextL( const RMessage2& aMessage );

    void UpdateDrmTimeL( const RMessage2& aMessage );

    void VerifyOcspResponsesL( const RMessage2& aMessage );

    void GetOcspResponderIdL( const RMessage2& aMessage );

    // Prohibit copy constructor.
    CRoapStorageSession( const CRoapStorageSession& );
    // Prohibit assigment operator.
    CRoapStorageSession& operator=( const CRoapStorageSession& );

private:
    // Data

    HBufC8* iPreparedData;

    MDrmKeyStorage* iKeyStorage;

    TBool iDeleteExpired;

    LazyClockClient* iLazyClock;

};
#endif      // ROAP_STORAGE_SESSION_H

// End of File
