/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition for Dynamically loaded wrapper for Drm Service Api
*
*/


#ifndef CDRMSERVICEAPIWRAPPER_H
#define CDRMSERVICEAPIWRAPPER_H

#include <e32base.h>
#include "DRMTypes.h"

/**
 * Environment gate function
 *
 * @since S60 3.2
 * @return pointer to Drm Service API wrapper
 */
IMPORT_C TAny* GateFunctionDrmServiceApiWrapper();

class MDrmServiceApiWrapper
    {
public:
    /**
     * Get the secure time
     *
     * @since   S60 v3.2
     * @param   aTime           Output parameter containing the
     *                          secure time in UTC.
     * @param   aTimeZone       Time zone information including summer/winter time
     *                          in +/- 15 minutes
     * @param   aSecurityLevel  Output parameter Securiry level.
     * @return  Symbian OS error code if any.
     */
    virtual TInt GetSecureTime( TTime& aTime,
                                TInt& aTimeZone,
                                DRMClock::ESecurityLevel& aSecurityLevel ) const = 0;

    /**
     * Update the secure time
     *
     * Requires DRM Capability
     *
     * @since   S60 v3.2
     * @param   aTime           Input parameter containing the
     *                          secure time in UTC.
     * @param   aTimeZone       Time zone information including
     *                          summer/winter time
     *                          in +/- 15 minutes
     * @return  Symbian OS error code if any.
     */
    virtual TInt UpdateSecureTime( const TTime& aTime, const TInt& aTimeZone ) = 0;


    /**
     * Get the device public key in Der format
     *
     * @since   S60 v3.2
     * @param   aPublicKey      Return parameter for the public key owned by
     *                          the caller
     * @return  none
     * @leave Symbian OS error code
     */
    virtual void GetDevicePublicKeyDerL( HBufC8*& aPublicKey ) = 0;

    /**
     * Sign the value given
     *
     * @since   S60 v3.2
     * @param   aHash           Value to be signed
     * @param   aSignature      Return parameter for the signature, owned by
     *                          the caller
     * @return  none
     * @leave Symbian OS error code
     */
    virtual void SignL( const TDesC8& aHash, HBufC8*& aSignature ) = 0;
    };


namespace DRM
    {

// forward declaration
class CDrmServiceApi;


/**
 *  Class for connecting to the WmDrmServer
 *
 *  @lib WmDrmClientWrapper
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CDrmServiceApiWrapper ): public CBase,
                                            public MDrmServiceApiWrapper
    {

public:

    static CDrmServiceApiWrapper* NewL();

    static CDrmServiceApiWrapper* NewLC();

    virtual ~CDrmServiceApiWrapper();

    /**
     * Get the secure time
     *
     * @since   S60 v3.2
     * @param   aTime           Output parameter containing the
     *                          secure time in UTC.
     * @param   aTimeZone       Time zone information including summer/winter time
     *                          in +/- 15 minutes
     * @param   aSecurityLevel  Output parameter Securiry level.
     * @return  Symbian OS error code if any.
     */
    virtual TInt GetSecureTime( TTime& aTime,
                                TInt& aTimeZone,
                                DRMClock::ESecurityLevel& aSecurityLevel ) const;

    /**
     * Update the secure time
     *
     * Requires DRM Capability
     *
     * @since   S60 v3.2
     * @param   aTime           Input parameter containing the
     *                          secure time in UTC.
     * @param   aTimeZone       Time zone information including
     *                          summer/winter time
     *                          in +/- 15 minutes
     * @return  Symbian OS error code if any.
     */
    virtual TInt UpdateSecureTime( const TTime& aTime, const TInt& aTimeZone );


    /**
     * Get the device public key in Der format
     *
     * @since   S60 v3.2
     * @param   aPublicKey      Return parameter for the public key owned by
     *                          the caller
     * @return  none
     * @leave Symbian OS error code
     */
    virtual void GetDevicePublicKeyDerL( HBufC8*& aPublicKey );

    /**
     * Sign the value given
     *
     * @since   S60 v3.2
     * @param   aHash           Value to be signed
     * @param   aSignature      Return parameter for the signature, owned by
     *                          the caller
     * @return  none
     * @leave Symbian OS error code
     */
    virtual void SignL( const TDesC8& aHash, HBufC8*& aSignature );

protected:

private:

    /**
    * C++ default constructor.
    */
    CDrmServiceApiWrapper();

    void ConstructL();


private: // data

    /**
     * Download manager session
     */
    CDrmServiceApi* iServiceApi;

    };

    } // namespace DRM

#endif // CDRMSERVICEAPIWRAPPER_H
