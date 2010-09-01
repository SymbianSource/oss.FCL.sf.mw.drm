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
* Description:  Interface class for WMDRM DLA Browser Container
*
*/


#ifndef C_WMDRMDLABROWSERCONTAINER_H
#define C_WMDRMDLABROWSERCONTAINER_H

#include <coecntrl.h>

class CAknView;
class MBrCtlSpecialLoadObserver;
class CBrCtlInterface;

class CWmDrmDlaBrowserContainer : public CCoeControl
    {

    public:

        static CWmDrmDlaBrowserContainer* NewL( CAknView* aView, 
                                                MBrCtlSpecialLoadObserver* aObserver );
        static CWmDrmDlaBrowserContainer* NewLC( CAknView* aView, 
                                                 MBrCtlSpecialLoadObserver* aObserver );

        /**
         * Destructor.
         */
        virtual ~CWmDrmDlaBrowserContainer();
        
        /**
         * Returns pointer to the CBrCtlInterface owned by the container.
         * @return A pointer to CBrCtlInterface
         */        
        CBrCtlInterface* BrCtlInterface();
    
        
    public: // From CCoeControl

        /**
         * @see CCoeControl
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        
        /**
         * @see CCoeControl
         */
        TInt CountComponentControls() const;

        /**
         * @see CCoeControl
         */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
         * @see CCoeControl
         */
        void SizeChanged();

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
        void ConstructL( MBrCtlSpecialLoadObserver* aObserver );

    private: // data
   
        //Not owned
        CAknView* iView;
        
        CBrCtlInterface* iBrCtlInterface;
        
    };

#endif // C_WMDRMDLABROWSERCONTAINER_H
