/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handling storing location of DRM rights
*
*/


#ifndef DRMRIGHTSSTORINGLOCATION_H
#define DRMRIGHTSSTORINGLOCATION_H

#include "drmutilityinternaltypes.h"

// CLASS DECLARATION
/**
*  DrmRightsStoringLocation
*
*  Collection of methods for the implementation of configurable 
*  storing location of DRM rights
*/
class DrmRightsStoringLocation
{
    public:
        /**
        * Checks if the storing location of DRM Rights is configured in the
        * Central Repository key. The function returns truth value based on
        * the availability of the configuration information. If the configured 
        * drive is not found, the function returns default system drive as the  
        * storage drive for rights.  
        * @param aFs               File server session
        * @param aDrmScheme        The DRM scheme, e.g., OMA DRM or WM DRM    
        * @param aChar             The drive letter from the Central Repository
                                   or the default system drive if key is not found
        * @return                  Truth value based on the availability of the 
        *                          storage drive configuration information                 
        */
        IMPORT_C static TBool CheckDrmRightsStorageDriveL( RFs &aFs,
            TDrmScheme& aDrmScheme, TChar& aChar );       
                                       
    private:
                                     
};

#endif      // DRMRIGHTSSTORINGLOCATION_H

// End of File