/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface class for WMDRM DLA Browser Container
*
*/


#ifndef C_WMDRMDLABROWSERCONTAINER_H
#define C_WMDRMDLABROWSERCONTAINER_H

#include <coecntrl.h>

class CAknView;

class CWmDrmDlaBrowserContainer : public CCoeControl
    {

    public:

    public: // From CCoeControl

        /**
         * @see CCoeControl
         */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
         * @see CCoeControl
         */
        void FocusChanged( TDrawNow aDrawNow );
        
        /**
         * @see CCoeControl
         */
        void HandleResourceChange( TInt aType );    
    
    private:

        CWmDrmDlaBrowserContainer( CAknView* aView );

    private: // data
   
        //Not owned
        CAknView* iView;
        
    };

#endif // C_WMDRMDLABROWSERCONTAINER_H
