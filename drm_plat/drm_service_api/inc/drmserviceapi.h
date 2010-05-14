/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Service api which exposes some of the private functionality
*                as a domain API
*
*/


#ifndef C_CDRMSERVICEAPI_H
#define C_CDRMSERVICEAPI_H

#include <e32base.h>
#include "DRMTypes.h"

// Forward declarations
class RDRMClockClient;

namespace Roap
    {
    class RRoapStorageClient;   
    }


namespace DRM 
    {

/**
 *  Drm Service Api which provides access to some drm private functionality
 *
 *  @lib DrmServiceAPI.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CDrmServiceApi ) : public CBase
    {

public:

    /**
     * Two-phased constructor
     *
     * Creates a function CDrmServiceApi -object and returns a pointer to it
     * pointer is owned by the caller
     *
     * @since   S60 v3.2
     * @return  Symbian OS error code if any.
     */
    IMPORT_C static CDrmServiceApi* NewL();

    /**
     * Two-phased constructor
     *
     * Creates a function CDrmServiceApi -object and returns a pointer to it
     * pointer is owned by the caller, the pointer is left to the cleanup stack
     *
     * @since   S60 v3.2
     * @return  Symbian OS error code if any.
     */
    IMPORT_C static CDrmServiceApi* NewLC();

    /**
    * Destructor.
    */
    virtual ~CDrmServiceApi();


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
    IMPORT_C TInt GetSecureTime( TTime& aTime, 
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
    IMPORT_C TInt UpdateSecureTime( const TTime& aTime, const TInt& aTimeZone );


    /**
     * Get the device public key in Der format
     *
     * @since   S60 v3.2
     * @param   aPublicKey      Return parameter for the public key owned by
     *                          the caller
     * @return  none
     * @leave Symbian OS error code
     */
    IMPORT_C void GetDevicePublicKeyDerL( HBufC8*& aPublicKey );

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
    IMPORT_C void SignL( const TDesC8& aHash, HBufC8*& aSignature );

private:

    /**
    * Default constructor
    */
    CDrmServiceApi();

    /**
    * 2nd phase constructor
    */
    void ConstructL();


private: // data
    /**
    * Owned clock client
    */
    RDRMClockClient* iClockClient;
    
    /**
    * Owned roap storage client
    */
    Roap::RRoapStorageClient* iRoapStorageClient;
    };
    
    }

#endif // C_CDRMSERVICEAPI_H
