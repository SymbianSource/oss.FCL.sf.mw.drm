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
* Description:  Interface class for WMDRM DLA HTTP FW plugin resolver
*
*/


#ifndef WMDRMDLAHTTPFWPLUGINRESOLVER_H
#define WMDRMDLAHTTPFWPLUGINRESOLVER_H

#include <e32base.h>
#include <ecom/implementationinformation.h>

class CWmDrmDlaPluginContainer;
class CAknIconArray;

/**
 *  WMDRM DLA HTTP FW Plugin resolver interface
 *
 *  @lib wmdrmdla.lib
 *  @since S60 v9.1
 */
class WmDrmDlaHttpFwPluginResolver
    {

public:

    /**
     * Resolves HTTP plugin for the WMDRM file
     *
     * @param   aFile WMDRM file
     * @param   aHttpPlugin Contains resolved HTTP Plugin on return. 
     * @param   aUiNotifier Contains initialized UiNotifier on return. 
     *                      Can be NULL
     * @leave   KErrNotFound if there is no handler for the file
     */
    static void ResolveHttpPluginL( const RFile& aFile, 
                                    CWmDrmDlaHttpPluginIf*& aHttpPlugin,
                                    CWmDrmDlaUiNotifier*& aUiNotifier );
    
    /**
     * Resolves HTTP plugin for the DRM header
     *
     * @param   aDrmHeader DRM header
     * @param   aHttpPlugin Contains resolved HTTP Plugin on return. 
     * @param   aUiNotifier Contains initialized UiNotifier on return. 
     *                      Can be NULL
     * @leave   KErrNotFound if there is no handler for the header
     */
    static void ResolveHttpPluginL( const TDesC8& aDrmHeader, 
                                    CWmDrmDlaHttpPluginIf*& aHttpPlugin,
                                    CWmDrmDlaUiNotifier*& aUiNotifier );
    
    /**
     * Resolves HTTP plugin for the metering certificate
     *
     * @param   aCertificate Metering certificate
     * @param   aHttpPlugin Contains resolved HTTP Plugin on return. 
     * @param   aUiNotifier Contains initialized UiNotifier on return. 
     *                      Can be NULL
     * @leave   KErrNotFound if there is no handler for the certificate
     */
    static void ResolveHttpPluginWithCertificateL( 
                        const TDesC8& aCertificate,
                        CWmDrmDlaHttpPluginIf*& aHttpPlugin, 
                        CWmDrmDlaUiNotifier*& aUiNotifier );

private:

    /**
     * Selects HTTP plugin from supported plugins. Will query from the user
     * which plugin to use, if more that one plugin supports the current content.
     *
     * @param   aSupportedPluginArray Array of supported plugins
     * @param   aImplArray Array of plugins that implement WMDRM DLA HTTP
     *                     plugin interface
     * @param   aHttpPlugin Contains resolved HTTP Plugin on return. 
     * @param   aUiNotifier Contains initialized UiNotifier on return. 
     *                      Can be NULL
     * @leave   KErrNotFound if there isn't any plugins
     */
    static void SelectPluginL( 
                RPointerArray<CWmDrmDlaPluginContainer>& aSupportedPluginArray,
                RImplInfoPtrArray& aImplArray,
                CWmDrmDlaHttpPluginIf*& aHttpPlugin, 
                CWmDrmDlaUiNotifier*& aUiNotifier );
    
    /**
     * Loads the default icon and appends it to the icon array
     *
     * @param   aIconArray Array where loaded default icon is appended
     * @leave   Standard Symbian error code
     */
    static void DefaultIconL( CAknIconArray*& aIconArray );
    
    };

#endif // WMDRMDLAHTTPFWPLUGINRESOLVER_H
