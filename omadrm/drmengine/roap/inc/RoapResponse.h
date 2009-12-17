/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#ifndef ROAP_RESPONSE_H
#define ROAP_RESPONSE_H

#include <e32base.h>
#include <f32file.h>
#include <apmstd.h>
#include "MultipartHandler.h"

namespace Roap
{

// CLASS DECLARATION

class CRoapResponse: public CBase, public TMultipartHandler
    {
    public:
    
        enum TMultipartState
            {
            ERights,
            EDcf,
            EOther
            };
    
    public:     // Construct / destruct
     
        static CRoapResponse* NewL();
        
        virtual ~CRoapResponse();

    public:

        const TDesC8& ProtocolUnit() const;
        
        const TFileName& DcfFileName() const;
        
        const TDataType& DataType() const;
        
        void SetDcfPathL( const TPath& aPath );
        
        void AppendMultipartDataL( const TDesC8& aData );

        virtual void HandleBodyDataL( const TDesC8& aData );

        virtual void StartBodyPartL();
        
        virtual void EndBodyPartL();
        
        void SetContentNameL( const TDes& aName );
        
        void GetContentNameLC( HBufC*& aName ) const;
        
        void SetMaxSize( const TInt& aSize );
        
        TInt MaxSize() const;

    protected: 
     
        CRoapResponse();
            
        void ConstructL();
        
    private:    // friends

        friend class CRoapHttpHandler;

    private:    // data

        HBufC8* iPdu;
        TFileName iDcfFileName; // contains the full path and file name of the temp DCF file
        HBufC* iContentName; // content name to be used as a filename when saving temp DCF
        TDataType iDataType;
        TMultipartState iMultipartState;
        TInt iMaxSize;
    };        
}

#endif 
