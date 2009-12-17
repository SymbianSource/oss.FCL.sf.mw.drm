/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Internal type definitions for DRM Utility
*
*/


#ifndef DRMUTILITYINTERNALTYPES_H
#define DRMUTILITYINTERNALTYPES_H

// DRM scheme of the file
enum TDrmScheme
    {
    // Unknown
    EDrmSchemeUnknownDrm = 0,
            
    // OMA DRM
    EDrmSchemeOmaDrm,
            
    // WM DRM
    EDrmSchemeWmDrm
    };    

// Launch types of DRM Rights Manager
enum TDrmLaunchParam
    {
    // Embedded launch from DRM Helper
    EDrmLaunchParamEmbeddedHelper = 1,
    
    // Standalone launch from DRM Helper
    EDrmLaunchParamStandAloneHelper,
    
    // Embedded launch from DRM Utility with full file path included
    // as a separate parameter
    EDrmLaunchParamEmbeddedUtility,
    
    // Standalone launch from DRM Utility with full file path included
    // as a separate parameter 
    EDrmLaunchParamStandAloneUtility
    
    };

    
#endif      // DRMUTILITYINTERNALTYPES_H   
            
// End of File