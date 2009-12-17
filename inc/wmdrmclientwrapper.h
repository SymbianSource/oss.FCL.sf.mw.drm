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
* Description:  
*
*/


#ifndef CWMDRMCLIENTWRAPPER_H
#define CWMDRMCLIENTWRAPPER_H

#include "wmdrmclient.h"

/**
 * Environment gate function
 *
 * @since S60 3.2
 * @return pointer to DMgr handler
 */
IMPORT_C TAny* GateFunctionWmDrmClientWrapper();

class MWmDrmClientWrapper
    {
public:
    virtual TInt Connect() = 0;
    };



/**
 *  Class for connecting to the WmDrmServer
 *
 *  @lib WmDrmClientWrapper
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CWmDrmClientWrapper ): public CBase,
                                          public MWmDrmClientWrapper
    {

public:

    static CWmDrmClientWrapper* NewL();

    static CWmDrmClientWrapper* NewLC();

    virtual ~CWmDrmClientWrapper();

    /**
     * Start up the WmDrmServer
     */
    virtual TInt Connect();
    
protected:

private:

    /**
    * C++ default constructor.
    */
    CWmDrmClientWrapper();

    void ConstructL();


private: // data

    /**
     * Download manager session
     */
    RWmDrmClient iClient;
    
    };

#endif // CWMDRMCLIENTWRAPPER_H
