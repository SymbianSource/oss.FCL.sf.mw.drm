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
* Description:  Interface class for WMDRM DLA plugin container
*
*/


#ifndef C_WMDRMDLAPLUGINCONTAINER_H
#define C_WMDRMDLAPLUGINCONTAINER_H

#include <e32base.h>

class CWmDrmDlaHttpPluginIf;
class CWmDrmDlaUiNotifier;

/**
 *  WMDRM DLA Plugin Container interface
 *
 *  @lib wmdrmdla.lib
 *  @since S60 v9.1
 */
NONSHARABLE_CLASS( CWmDrmDlaPluginContainer ) : public CBase
    {

public:

    static CWmDrmDlaPluginContainer* NewL();
    static CWmDrmDlaPluginContainer* NewLC();

    /**
    * Destructor.
    */
    virtual ~CWmDrmDlaPluginContainer();
    
    void SetHttpPlugin( CWmDrmDlaHttpPluginIf* aHttpPlugin );
    void GetHttpPlugin( CWmDrmDlaHttpPluginIf*& aHttpPlugin );
    CWmDrmDlaHttpPluginIf* HttpPlugin();
    
    void SetUiNotifier( CWmDrmDlaUiNotifier* aUiNotifier );
    void GetUiNotifier( CWmDrmDlaUiNotifier*& aUiNotifier );
    
    void SetIndex( TInt aIndex );
    TInt Index();

protected:

private:

    CWmDrmDlaPluginContainer();

    void ConstructL();

private: // data

    CWmDrmDlaHttpPluginIf* iHttpPlugin;
    CWmDrmDlaUiNotifier* iUiNotifier;
    TInt iIndex;
    
    };

#endif // C_WMDRMDLAPLUGINCONTAINER_H
