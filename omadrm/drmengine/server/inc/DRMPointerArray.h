/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef DRMPOINTERARRAY_H
#define DRMPOINTERARRAY_H

#include <e32base.h>
#include <e32cmn.h>

template < class T > NONSHARABLE_CLASS( RDRMPointerArray ) : public RPointerArray< T >
    {
    public:
        inline explicit RDRMPointerArray( TInt aGranularity );
        inline RDRMPointerArray( TInt aMinGrowBy, TInt aFactor );
        inline RDRMPointerArray();
        virtual ~RDRMPointerArray();

        // If set to ETrue, RPointerArray::ResetAndDestroy() is called
        // automatically in destructor. Otherwise only RPointerArrary::Close()
        // is called.
        inline void SetAutoCleanup( const TBool aCleanup );

    protected:
        TBool iDoResetAndDestroy;
    };

template < class T > NONSHARABLE_CLASS( CDRMPointerArray ) : public CBase, public RDRMPointerArray< T >
    {
    public:
        static CDRMPointerArray* NewLC( TInt aGranularity );
        static CDRMPointerArray* NewL( TInt aGranularity );
        static CDRMPointerArray* NewLC( TInt aMinGrowBy, TInt aFactor );
        static CDRMPointerArray* NewL( TInt aMinGrowBy, TInt aFactor );
        static CDRMPointerArray* NewLC();
        static CDRMPointerArray* NewL();

        // Note: Does not do ResetAndDestroy() by default.
        // Use SetAutoCleanup to do reallyclean.
        ~CDRMPointerArray();


    protected:
        explicit CDRMPointerArray( TInt aGranularity );
        CDRMPointerArray( TInt aMinGrowBy, TInt aFactor );
        CDRMPointerArray();

    private:
    };


#include "DRMPointerArray.inl"

#endif // DRMPOINTERARRAY_H