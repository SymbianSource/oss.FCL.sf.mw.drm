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
* Description:  Interface class for the WMDRM DLA cancel observer
*
*/


#ifndef M_WMDRMDLACANCELOBSERVER_H
#define M_WMDRMDLACANCELOBSERVER_H

#include <e32base.h>

/**
* MWmDrmDlaCancelObserver
* Cancellation observer for UI plugin to inform framework
* if user has cancelled DLA progress from UI component
*/
class MWmDrmDlaCancelObserver
    {
    
    public:
        
        virtual void UserCancellation() = 0;
        
    };

#endif // M_WMDRMDLACANCELOBSERVER_H
