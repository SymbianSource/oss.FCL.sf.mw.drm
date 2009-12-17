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
* Description:  Implementation class for WMDRM DLA HTTP FW plugin resolver
*
*/


#include <ecom/ecom.h>
#include <badesca.h>
#include <akniconutils.h>
#include <akniconarray.h>
#include <gulicon.h>
#include <wmdrmdlatypes.h>
#include <wmdrmdlahttpplugin.h>
#include <wmdrmdlauinotifier.h>
#include <wmdrmdla.mbg>
#include "wmdrmdlahttpfwpluginresolver.h"
#include "wmdrmdlaui.h"
#include "wmdrmdlaplugincontainer.h"

#define _LOGGING_FILE L"wmdrmdla.txt"
#include "logfn.h"

// CONSTANTS
_LIT( KMbmFileName, "\\resource\\apps\\wmdrmdla.mbm" );
_LIT( KTab, "\t" );

const TInt KWmDrmDlaIndexMaxLength( 5 );

// ======== LOCAL FUNCTIONS ========

// -----------------------------------------------------------------------------
// PointerArrayResetDestroyAndClose
// Template method used to push RPointerArrays to the cleanup stack. Takes
// care of deleting all pointers in the array.
// -----------------------------------------------------------------------------
//
template <class T>
LOCAL_C void PointerArrayResetDestroyAndClose( TAny* aPtr )
    {
    (reinterpret_cast<RPointerArray<T>*>(aPtr))->ResetAndDestroy();
    (reinterpret_cast<RPointerArray<T>*>(aPtr))->Close();
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// WmDrmDlaHttpFwPluginResolver::ResolveHttpPluginL
// ---------------------------------------------------------------------------
//
void WmDrmDlaHttpFwPluginResolver::ResolveHttpPluginL( 
    const RFile& aFile,
    CWmDrmDlaHttpPluginIf*& aHttpPlugin, 
    CWmDrmDlaUiNotifier*& aUiNotifier )
    {
    TInt err( KErrNone );

    LOGFNR( "WmDrmDlaHttpFwPluginResolver::ResolveHttpPluginL", err );

    //Array where all supported plugins are gathered
    RPointerArray<CWmDrmDlaPluginContainer> supportedPluginArray;
    CleanupStack::PushL( 
        TCleanupItem( 
            PointerArrayResetDestroyAndClose<CWmDrmDlaPluginContainer>, 
            &supportedPluginArray ) );

    //Array where all http plugin implementation infos are gathered
    RImplInfoPtrArray implArray;
    CleanupStack::PushL( 
        TCleanupItem( 
            PointerArrayResetDestroyAndClose<CImplementationInformation>,
            &implArray ) );
    REComSession::ListImplementationsL( KWmDrmDlaHttpPluginIfUid, implArray );

    //Lets go through all implementation infos and find out what http plugins
    //support this content
    for ( TInt i( 0 ); i < implArray.Count(); ++i )
        {
        //Only plugins with NOKIA vendor ID are currently allowed
        if ( implArray[i]->VendorId() == VID_DEFAULT )
            {
            CWmDrmDlaUiNotifier* uiNotifier( NULL );
            CWmDrmDlaHttpPluginIf* httpPlugin( NULL );
            //Try to initialize http plugin
            TRAP( err, httpPlugin = 
                        CWmDrmDlaHttpPluginIf::NewL( 
                                implArray[i]->ImplementationUid() ) );
            if ( !err )
                {
                CleanupStack::PushL( httpPlugin );
                TBool supported( EFalse );
                //Check if current http plugin supports content
                TRAP( err, supported = 
                    httpPlugin->SupportedFileL( aFile, uiNotifier ) )
                CleanupStack::PushL( uiNotifier );
                //If current http plugin supports content, then
                //add implementation index, http plugin and ui notifier to 
                //plugin container.
                //Otherwise delete current http plugin and ui notifier.
                if ( !err && supported )
                    {
                    CWmDrmDlaPluginContainer* container( 
                            CWmDrmDlaPluginContainer::NewL() );
                    container->SetIndex( i );
                    container->SetHttpPlugin( httpPlugin );
                    container->SetUiNotifier( uiNotifier );
                    CleanupStack::Pop( 2, httpPlugin ); //uiNotifier, httpPlugin
                    CleanupStack::PushL( container );                    
                    supportedPluginArray.AppendL( container );
                    CleanupStack::Pop( container );
                    }
                else
                    {
                    CleanupStack::PopAndDestroy( 2, httpPlugin ); //uiNotifier, httpPlugin
                    }
                }
            }
        }

    //Select the plugin to be used for license acquisition
    SelectPluginL( supportedPluginArray, implArray, aHttpPlugin, aUiNotifier );
    CleanupStack::PopAndDestroy( 2, &supportedPluginArray ); //implArray, supportedPluginArray
    }

// ---------------------------------------------------------------------------
// WmDrmDlaHttpFwPluginResolver::ResolveHttpPluginL
// ---------------------------------------------------------------------------
//
void WmDrmDlaHttpFwPluginResolver::ResolveHttpPluginL( 
    const TDesC8& aDrmHeader,
    CWmDrmDlaHttpPluginIf*& aHttpPlugin, 
    CWmDrmDlaUiNotifier*& aUiNotifier )
    {
    TInt err( KErrNone );

    LOGFNR( "WmDrmDlaHttpFwPluginResolver::ResolveHttpPluginL", err );

    //Array where all supported plugins are gathered
    RPointerArray<CWmDrmDlaPluginContainer> supportedPluginArray;
    CleanupStack::PushL( 
        TCleanupItem( 
            PointerArrayResetDestroyAndClose<CWmDrmDlaPluginContainer>, 
            &supportedPluginArray ) );

    //Array where all http plugin implementation infos are gathered
    RImplInfoPtrArray implArray;
    CleanupStack::PushL( 
        TCleanupItem( 
            PointerArrayResetDestroyAndClose<CImplementationInformation>, 
            &implArray ) );
    REComSession::ListImplementationsL( KWmDrmDlaHttpPluginIfUid, implArray );

    //Lets go through all implementation infos and find out what http plugins
    //support this content
    for ( TInt i( 0 ); i < implArray.Count(); ++i )
        {
        //Only plugins with NOKIA vendor ID are currently allowed
        if ( implArray[i]->VendorId() == VID_DEFAULT )
            {
            CWmDrmDlaUiNotifier* uiNotifier( NULL );
            CWmDrmDlaHttpPluginIf* httpPlugin( NULL );
            //Try to initialize http plugin
            TRAP( err, httpPlugin = 
                        CWmDrmDlaHttpPluginIf::NewL( 
                                implArray[i]->ImplementationUid() ) );
            if ( !err )
                {
                CleanupStack::PushL( httpPlugin );
                TBool supported( EFalse );
                //Check if current http plugin supports content
                TRAP( err, supported = 
                    httpPlugin->SupportedDrmHeaderL( aDrmHeader, uiNotifier ) )
                CleanupStack::PushL( uiNotifier );
                //If current http plugin supports content, then
                //add implementation index, http plugin and ui notifier to 
                //plugin container.
                //Otherwise delete current http plugin and ui notifier.
                if ( !err && supported )
                    {
                    CWmDrmDlaPluginContainer* container( 
                            CWmDrmDlaPluginContainer::NewL() );
                    container->SetIndex( i );
                    container->SetHttpPlugin( httpPlugin );
                    container->SetUiNotifier( uiNotifier );
                    CleanupStack::Pop( 2, httpPlugin ); //uiNotifier, httpPlugin
                    CleanupStack::PushL( container );                    
                    supportedPluginArray.AppendL( container );
                    CleanupStack::Pop( container );
                    }
                else
                    {
                    CleanupStack::PopAndDestroy( 2, httpPlugin ); //uiNotifier, httpPlugin
                    }
                }
            }
        }

    //Select the plugin to be used for license acquisition
    SelectPluginL( supportedPluginArray, implArray, aHttpPlugin, aUiNotifier );
    CleanupStack::PopAndDestroy( 2, &supportedPluginArray ); //implArray, supportedPluginArray
    }

// ---------------------------------------------------------------------------
// WmDrmDlaHttpFwPluginResolver::::ResolveHttpPluginWithCertificateL
// ---------------------------------------------------------------------------
//
void WmDrmDlaHttpFwPluginResolver::ResolveHttpPluginWithCertificateL( 
    const TDesC8& aCertificate,
    CWmDrmDlaHttpPluginIf*& aHttpPlugin, 
    CWmDrmDlaUiNotifier*& aUiNotifier )
    {
    TInt err( KErrNone );

    LOGFNR( "WmDrmDlaHttpFwPluginResolver::ResolveHttpPluginL", err );

    //Array where all supported plugins are gathered
    RPointerArray<CWmDrmDlaPluginContainer> supportedPluginArray;
    CleanupStack::PushL( 
        TCleanupItem( 
            PointerArrayResetDestroyAndClose<CWmDrmDlaPluginContainer>,
            &supportedPluginArray ) );

    //Array where all http plugin implementation infos are gathered
    RImplInfoPtrArray implArray;
    CleanupStack::PushL( 
        TCleanupItem( 
            PointerArrayResetDestroyAndClose<CImplementationInformation>,
            &implArray ) );
    REComSession::ListImplementationsL( KWmDrmDlaHttpPluginIfUid, implArray );

    //Lets go through all implementation infos and find out what http plugins
    //support this content
    for ( TInt i( 0 ); i < implArray.Count(); ++i )
        {
        //Only plugins with NOKIA vendor ID are currently allowed
        if ( implArray[i]->VendorId() == VID_DEFAULT )
            {
            CWmDrmDlaUiNotifier* uiNotifier( NULL );
            CWmDrmDlaHttpPluginIf* httpPlugin( NULL );
            //Try to initialize http plugin
            TRAP( err, httpPlugin = 
                        CWmDrmDlaHttpPluginIf::NewL( 
                                implArray[i]->ImplementationUid() ) );
            if ( !err )
                {
                CleanupStack::PushL( httpPlugin );
                TBool supported( EFalse );
                //Check if current http plugin supports content
                TRAP( err, supported = 
                    httpPlugin->SupportedMeteringCertificateL( aCertificate, 
                                                               uiNotifier ) )
                CleanupStack::PushL( uiNotifier );
                //If current http plugin supports content, then
                //add implementation index, http plugin and ui notifier to 
                //plugin container.
                //Otherwise delete current http plugin and ui notifier.
                if ( !err && supported )
                    {
                    CWmDrmDlaPluginContainer* container( 
                            CWmDrmDlaPluginContainer::NewL() );
                    container->SetIndex( i );
                    container->SetHttpPlugin( httpPlugin );
                    container->SetUiNotifier( uiNotifier );
                    CleanupStack::Pop( 2, httpPlugin ); //uiNotifier, httpPlugin
                    CleanupStack::PushL( container );                    
                    supportedPluginArray.AppendL( container );
                    CleanupStack::Pop( container );
                    }
                else
                    {
                    CleanupStack::PopAndDestroy( 2, httpPlugin ); //uiNotifier, httpPlugin
                    }
                }
            }
        }

    //Select the plugin to be used for metering
    SelectPluginL( supportedPluginArray, implArray, aHttpPlugin, aUiNotifier );
    CleanupStack::PopAndDestroy( 2, &supportedPluginArray ); //implArray, supportedPluginArray
    }

// ----------------------------------------------------------------------------
// WmDrmDlaHttpFwPluginResolver::SelectPluginL
// ----------------------------------------------------------------------------
//
void WmDrmDlaHttpFwPluginResolver::SelectPluginL( 
    RPointerArray<CWmDrmDlaPluginContainer>& aSupportedPluginArray,
    RImplInfoPtrArray& aImplArray,
    CWmDrmDlaHttpPluginIf*& aHttpPlugin, 
    CWmDrmDlaUiNotifier*& aUiNotifier )
    {
    LOGFN( "WmDrmDlaHttpFwPluginResolver::SelectPluginL" );
    
    //Find out how many plugins support this content
    TInt supportedCount( aSupportedPluginArray.Count() );
    LOG2( "supportedCount: %d", supportedCount );
    //If more than one plugin supports the content, then query
    //user to select one to be used
    if ( supportedCount > 1 )
        {
        CWmDrmDlaUi* ui( CWmDrmDlaUi::NewLC() );
        
        CAknIconArray* listIcons = new (ELeave) CAknIconArray( 2 );
        CleanupStack::PushL( listIcons );
                
        //Create default icon to be used with plugins that don't
        //have own icon. Default icon index is always 0.
        DefaultIconL( listIcons );
        
        CPtrC16Array* listItems = new (ELeave) CPtrC16Array( 2 );
        CleanupStack::PushL( listItems );
        
        //Go through supported plugins, try to get their display name
        //and icon. If plugin doesn't have icon, use default one.
        for ( TInt i( 0 ); i < supportedCount; ++i )
            {
            TInt index( aSupportedPluginArray[i]->Index() ); 
            TPtrC displayName( aImplArray[index]->DisplayName() ); 
            HBufC* listItem( HBufC::NewLC( displayName.Length() + 
                                           KWmDrmDlaIndexMaxLength ) );
            TPtr listItemPtr( listItem->Des() );
            CGulIcon* pluginIcon( aSupportedPluginArray[ i ]->
                                      HttpPlugin()->ServiceIcon() );
            if ( pluginIcon )
                {
                CleanupStack::PushL( pluginIcon );
                listIcons->AppendL( pluginIcon );
                CleanupStack::Pop( pluginIcon );
                listItemPtr.AppendNum( listIcons->Count() - 1 );
                }
            else
                {
                listItemPtr.AppendNum( 0 );
                }
            listItemPtr.Append( KTab );
            listItemPtr.Append( displayName );
            listItems->AppendL( listItemPtr );
            CleanupStack::Pop( listItem );
            }
        //listItems and listItems ownership is transferred to ui
        CleanupStack::Pop( 2, listIcons ); //listItems, listIcons
        //Query user to make a selection.
        //Take user selected plugin to use or leave, if user cancels query.
        TInt selection( ui->ShowListQueryL( listItems, listIcons  ) );
        LOG2( "selection: %d", selection );
        if ( selection >= 0 )
            {
            aSupportedPluginArray[selection]->GetHttpPlugin( aHttpPlugin );
            aSupportedPluginArray[selection]->GetUiNotifier( aUiNotifier );
            }
        else
            {
            User::Leave( KErrNotFound );
            }
        CleanupStack::PopAndDestroy( ui );
        }
    //Only one supporting http plugin
    else if ( supportedCount == 1 )
        {
        aSupportedPluginArray[0]->GetHttpPlugin( aHttpPlugin );
        aSupportedPluginArray[0]->GetUiNotifier( aUiNotifier );
        }
    //No supporting http plugins
    else
        {
        User::Leave( KErrNotFound );
        }
    }

// ----------------------------------------------------------------------------
// WmDrmDlaHttpFwPluginResolver::DefaultIconL
// ----------------------------------------------------------------------------
//
void WmDrmDlaHttpFwPluginResolver::DefaultIconL( 
    CAknIconArray*& aIconArray )
    {
    LOGFN( "WmDrmDlaHttpFwPluginResolver::DefaultIconL" );
    CFbsBitmap* bitmap( NULL );
    CFbsBitmap* bitmapMask( NULL );
    AknIconUtils::CreateIconLC( bitmap, bitmapMask, KMbmFileName,
                                EMbmWmdrmdlaQgn_prop_drm_valid_large,
                                EMbmWmdrmdlaQgn_prop_drm_valid_large_mask );
    CGulIcon* defaultIcon( CGulIcon::NewL( bitmap, bitmapMask ) );
    CleanupStack::Pop( 2 ); // bitmapMask, bitmap. Icon has now ownership
    CleanupStack::PushL( defaultIcon );
    aIconArray->AppendL( defaultIcon );
    CleanupStack::Pop( defaultIcon );
    }
