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
* Description:  Server common implementation
*
*/



#ifndef CPAIR_H
#define CPAIR_H

//  INCLUDES
#include <e32base.h>


// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION


/**
*  Server Session implementation.
*  DCF Repository
*
*  @lib 
*  @since Series 60 3.0 
*/
class CPair : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CPair* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CPair();

    private:

        /**
        * C++ default constructor.
        */
        CPair();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        CPair( const CPair& );
        // Prohibit assigment operator if not deriving from CBase.
        CPair& operator=( const CPair& );

    public:     // Data
        HBufC8* iCid;
        HBufC8* iTtid;

    };

#endif      // CPAIR_H   
            
// End of File
