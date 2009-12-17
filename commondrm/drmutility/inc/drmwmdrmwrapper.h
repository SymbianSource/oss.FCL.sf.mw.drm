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
* Description:  Interface for WM DRM Wrapper
*
*/


#ifndef M_DRMUTILITYWMDRMWRAPPER_H
#define M_DRMUTILITYWMDRMWRAPPER_H

#include <e32base.h>

//*** forward declarations go here:
class CCoeEnv;

namespace DRM
    {
 
class MDrmUiCheckRightsObserver;
class MDrmHandleErrorObserver;   
    

/**
 *
 *  @since S60 v5.0
 */
class MDrmUtilityWMDrmWrapper
    {

public:

    /**
     * Performs the check rights amount operation for windows media files
     *
     * @param[in]   aContent    The content itself
     * @param[in]   aIntent     The intent for the rights to check
     * @param[in]   aObserver   Observer for callbacks, NULL if not 
     *                          available
     * @param[in]   aOperatioId The operation id of the async operation
     * @return none
     * @leave System wide error code
     */	
    virtual void CheckWmRightsAmountL(
        ContentAccess::CData& aContent, 
        ContentAccess::TIntent aIntent, 
        MDrmUiCheckRightsObserver* aObserver,
        TInt aOperationId ) = 0;

    /**
     * Performs the handle error operation for windows media files
     *
     * @param[in]   aContent    The content itself
     * @param[in]   aIntent     The intent for the rights to check
     * @param[in]   aError      The error code to be handled
     * @param[in]   aObserver   Observer for callbacks, NULL if not 
     *                          available
     * @param[in]   aOperatioId The operation id of the async operation
     * @return none
     * @leave System wide error code
     */	
    virtual void HandleWmErrorL( 
        ContentAccess::CData& aContent, 
        ContentAccess::TIntent aIntent, 
        TInt aError,
        MDrmHandleErrorObserver* aObserver,
        TInt aOperationId ) = 0;

    /**
     * Method for setting the control environment
     *
     * @param[in]   aCoeEnv     The control environment
     * @return none
     */	
    virtual void SetCoeEnv(  
        CCoeEnv* aCoeEnv ) = 0; 

    };
    
    }


#endif // M_MDRMUTILITYWMDRMWRAPPER_H
