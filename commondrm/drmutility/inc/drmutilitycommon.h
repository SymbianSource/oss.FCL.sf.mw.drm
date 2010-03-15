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
* Description:  Provides operations for common usage of DRM Utility API
*
*/


#ifndef CDRMUTILITYCOMMON_H
#define CDRMUTILITYCOMMON_H

//*** system include files go here:
#include <e32base.h>
#include <caf/caftypes.h>
#include <DRMRightsClient.h>
#include <drmutilitytypes.h>

//*** forward declarations go here:
class CDRMConstraint;

namespace DRM
{

 /**
  *  Utility class for DRM related common functions
  *
  *  @lib drmutilitycommon.lib
  *  @since S60 v5.0
  */
NONSHARABLE_CLASS( CDrmUtilityCommon ) : public CBase
    {

public:

    /**
     * Creates a new CDrmUtilityCommon object and returns a pointer to it
     *
     * @return A functional CDrmUtilityCommon -object
     * @leave System wide error code
     */
    IMPORT_C static CDrmUtilityCommon* NewL();

    /**
     * Creates a new CDrmUtilityCommon object and returns a pointer to it
     * leaves the returned object to the cleanup stack
     *
     * @return A functional CDrmUtilityCommon -object
     * @leave System wide error code
     */
    IMPORT_C static CDrmUtilityCommon* NewLC();

    /**
     * Destructor
     */
    virtual ~CDrmUtilityCommon();

    /**
     * Checks the rights and their status for a specific unique id
     * implementation
     *
     * @param[in]   aContentUri The unique identifier or the content
     * @param[in]   aIntent     The intent for the rights to check
     * @param[out]  aConstraint A Constraint of the future rights,
     *                          must be initialized
     *
     * @return Rights status
     * @leave System wide error code
     * @see ContentAccess:TIntent
     */
    IMPORT_C TDrmRightsInfo GetOmaRightsStatusL(
                                            HBufC8*& aContentUri,
                                            ContentAccess::TIntent aIntent,
                                            CDRMConstraint* aConstraint );


protected:

private:

    CDrmUtilityCommon();

    void ConstructL();

private: // Data members

    // Oma Drm client
    RDRMRightsClient iOmaClient;

    };
}

#endif // CDRMUTILITYCOMMON_H
