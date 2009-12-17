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
* Description:  the client side class for DRMHelperServer
*
*/


#ifndef RDRMHELPERCLIENT_H
#define RDRMHELPERCLIENT_H

// INCLUDES
#include <e32base.h>
#include <caf/caftypes.h>

#include "drmutilitytypes.h"
// DATA TYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


using namespace DRM;

/**
*  This class provides the client-side interface to the server session
*
*  @since S60 4.0
*/

class RDrmHelperClient : public RSessionBase
    {
public:  // Constructors and destructor

    /**
    * C++ default constructor.
    */
    RDrmHelperClient();

public:  // New functions

    /**
    * Connects to server.
    * @return System-wide error code
    */
    TInt Connect();

    /**
    * Gets the version number
    * @return version number
    */
    TVersion Version() const;

    /**
    * Register the given content uri as an automated content.
    * @param aUri: content URI
    * @param aAutomatedType: type of automated content
    * @param aIntent: intent how content is used
    * @param aServiceType: active or passive registration
    * @return System-wide error code
    * @see ContentAccess::TIntent
    */
    TInt SetAutomated(
            const TDesC8& aUri,
            const TDrmAutomatedType aAutomatedType,
            const ContentAccess::TIntent aIntent,
            const TDrmAutomatedServiceType aServiceType ) const;

    /**
    * Unregister the given content.
    * @param aUri: content URI
    * @param aServiceType: active or passive registration
    * @param aAutomatedType: type of automated content
    * @param aIntent: intent how content is used
    * @return System-wide error code
    * @see ContentAccess::TIntent
    */
    TInt RemoveAutomated(
            const TDesC8& aUri,
            const TDrmAutomatedType aAutomatedType,
            const ContentAccess::TIntent aIntent,
            const TDrmAutomatedServiceType aServiceType ) const;

    /**
    * Unregister all instances of the given content.
    * @param aUri: content URI
    * @param aServiceType: active or passive registration
    * @param aAutomatedType: type of automated content
    * @param aIntent: intent how content is used
    * @return System-wide error code
    * @see ContentAccess::TIntent
    */
    TInt RemoveAutomatedAll(
            const TDesC8& aUri,
            const TDrmAutomatedType aAutomatedType,
            const ContentAccess::TIntent aIntent,
            const TDrmAutomatedServiceType aServiceType ) const;
            /*(const TDesC8& aUri,
            TBool aActive,
            TInt aAutomatedType,
            TInt aIntent ) const;*/

    /**
    * Indicate that idle-screen has been activated.
    * @return System-wide error code
    */
    TInt IndicateIdle() const;

private:

    /**
    * Has given content been registered as automated content.
    * @param aUri content URI of the content
    * @param aAutomated (out) ETrue if registered as automated.
    * @param aType (out) Tells whether content has been
    *                    registered as active or passive.
    * @return System-wide error code
    * @see ContentAccess::TIntent
    */
    TInt IsAutomated(
            const TDesC8& aUri,
            const TDrmAutomatedType aAutomatedType,
            const ContentAccess::TIntent aIntent,
            const TDrmAutomatedServiceType aServiceType,
            TBool& aAutomated ) const;

    TInt StartServer();
    TInt CreateServerProcess();
    };



#endif      // RDRMHELPERCLIENT_H

// End of File
