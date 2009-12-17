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
* Description:  DrmUtilityWmdrmWrapper loader
*
*/


#ifndef C_DRMUTILITYWMDRM_H
#define C_DRMUTILITYWMDRM_H

//*** system include files go here:
#include <e32base.h>

//*** forward declarations go here
class CCoeEnv;

namespace DRM 
    {

    //*** forward declarations go here:
    class CDrmUtilityWMDrmWrapper;
    class MDrmUtilityWMDrmWrapper;

/**
 *  This class encapsulates WMDrm functionality
 *
 *  @lib drmutilitycommon.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CDrmUtilityWMDrm ) : public CBase
    {
    
public:

    /**
     * Creates a new CDrmUtilityWMDrm object and returns a pointer to it
     *
     * @return A functional CDrmUtilityCommon -object
     * @leave System wide error code
     */	
    IMPORT_C static CDrmUtilityWMDrm* NewL( CCoeEnv* aCoeEnv = NULL );
    
    /**
     * Creates a new CDrmUtilityWMDrm object and returns a pointer to it
     * leaves the returned object to the cleanup stack
     *
     * @return A functional CDrmUtilityCommon -object
     * @leave System wide error code
     */	
    IMPORT_C static CDrmUtilityWMDrm* NewLC( CCoeEnv* aCoeEnv = NULL );


    /**
    * Destructor.
    */
    virtual ~CDrmUtilityWMDrm();

    /**
     * Returns a pointer to a class which implements the
     * MDrmUtilityWMDrmWrapper interface. The ownership
     * of the instance stays with the CDrmUtilityWMDrm -class
     *
     * @since S60 v5.0
     *
     * @return A pointer to a functional object implementing the
     *         MDrmUtilityWMDrmWrapper interface
     *
     * @leave None
     *
     * @see MDrmUtilityWMDrmWrapper
     */
    IMPORT_C MDrmUtilityWMDrmWrapper* GetWrapper();

protected:

private:

    CDrmUtilityWMDrm();

    void ConstructL( CCoeEnv* aCoeEnv );

private: // Data members

    // Handle to load WMDrmWrapper dynamically
    RLibrary iWMDrmDll;
    
    // WMDrm wrapper
    CDrmUtilityWMDrmWrapper* iWMDrmHandler;

    };

    }

#endif // C_DRMUTILITYWMDRM_H
