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


#ifndef DRMPOINTERARRAY_INL
#define DRMPOINTERARRAY_INL

template < class T > RDRMPointerArray< T >::RDRMPointerArray( TInt aGranularity ):
RPointerArray< T >( aGranularity ),
iDoResetAndDestroy( EFalse )
    {
    }
    
template < class T > RDRMPointerArray< T >::RDRMPointerArray( TInt aMinGrowBy,
                                                              TInt aFactor ):
RPointerArray< T >( aMinGrowBy, aFactor ),
iDoResetAndDestroy( EFalse )
    {
    }
    
template < class T > RDRMPointerArray< T >::RDRMPointerArray() :
RPointerArray< T >(),
iDoResetAndDestroy( EFalse )
    {
    // Nothing
    }
    
template < class T > RDRMPointerArray< T >::~RDRMPointerArray()
    {
    if ( iDoResetAndDestroy )
        {
        RPointerArray< T >::ResetAndDestroy();
        }
        
    RPointerArray< T >::Close();
    }

template < class T > void RDRMPointerArray< T >::SetAutoCleanup( const TBool aCleanup )
    {
    iDoResetAndDestroy = aCleanup;
    }


template < class T > CDRMPointerArray< T >* CDRMPointerArray< T >::NewLC( TInt aGranularity )
    {
    CDRMPointerArray< T >* self = CDRMPointerArray< T >::NewL( aGranularity );
    CleanupStack::PushL( self );
    return self;
    }

template < class T > CDRMPointerArray< T >* CDRMPointerArray< T >::NewL( TInt aGranularity )
    {
    CDRMPointerArray< T >* self = new( ELeave ) CDRMPointerArray< T >( aGranularity );
    
    return self;
    }

template < class T > 
CDRMPointerArray< T >* CDRMPointerArray< T >::NewLC( TInt aMinGrowBy, TInt aFactor )
    {
    CDRMPointerArray< T >* self = CDRMPointerArray< T >::NewL( aMinGrowBy, aFactor );
    CleanupStack::PushL( self );
    return self;
    }

template < class T > 
CDRMPointerArray< T >* CDRMPointerArray< T >::NewL( TInt aMinGrowBy, TInt aFactor )
    {
    CDRMPointerArray< T >* self = new( ELeave ) CDRMPointerArray< T >( aMinGrowBy, aFactor );
    
    return self;
    }


 
template < class T > CDRMPointerArray< T >*  CDRMPointerArray< T >::NewLC()
    {
    CDRMPointerArray< T >* self = CDRMPointerArray< T >::NewL();
    CleanupStack::PushL( self );
    
    return self;
    }
    
template < class T > CDRMPointerArray< T >*  CDRMPointerArray< T >::NewL()
    {
    CDRMPointerArray< T >* self = new ( ELeave ) CDRMPointerArray< T >;
    
    return self;
    }
    
    
template < class T > CDRMPointerArray< T >::~CDRMPointerArray()
    {
    // Nothing special
    }
    
template < class T > CDRMPointerArray< T >::CDRMPointerArray( TInt aGranularity ):
RDRMPointerArray< T >( aGranularity )
    {
    }
    
template < class T > CDRMPointerArray< T >::CDRMPointerArray( TInt aMinGrowBy, TInt aFactor ):
RDRMPointerArray< T >( aMinGrowBy, aFactor )
    {
    }

template < class T > CDRMPointerArray< T >::CDRMPointerArray():
RDRMPointerArray< T >()
    {
    }
    
#endif // DRMPOINTERARRAY_INL
