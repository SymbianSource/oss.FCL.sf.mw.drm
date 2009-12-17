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
* Description:  Client side implementation
*
*/



#ifndef CDCFENTRY_H
#define CDCFENTRY_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  CDcfEntry
*  Client of DCF Repository
*
*  @lib ?library
*  @since Series 60 3.0
*/
class CDcfEntry : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CDcfEntry* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CDcfEntry();

    public: // New functions
        
        /**
        * SetCidL
        * @since Series 60 3.0
        * @param aCid content ID
        * @return 
        */        
        IMPORT_C void SetCidL(const TDesC8& aCid);


        /**
        * SetGroupIdL
        * @since Series 60 3.0
        * @param aGroup Id group ID
        * @return 
        */        
        IMPORT_C void SetGroupIdL(const TDesC8& aGroupId);        
        
        /**
        * SetLocationL
        * @since Series 60 3.0
        * @param aFile filename of the DCF and 
        * @param position inside the file
        * @return 
        */        
        IMPORT_C void SetLocationL(const TDesC& aFile, const TUint16& aPos);
        
        /**
        * FileName
        * @since Series 60 3.0
        * @param 
        * @return file name
        */        
        IMPORT_C const TDesC16& FileName() const;
        
        /**
        * Position
        * @since Series 60 3.0
        * @param 
        * @return position inside a file
        */        
        IMPORT_C TUint16 Position() const;

        /**
        * Cid
        * @since Series 60 3.0
        * @param 
        * @return content ID
        */        
        IMPORT_C const TDesC8& Cid() const;
        
        /**
        * GroupId
        * @since Series 60 3.0
        * @param 
        * @return group ID
        */        
        IMPORT_C const TDesC8& GroupId() const;
        

    public: // Functions from base classes
    protected:  // New functions
    protected:  // Functions from base classes
    private:

        /**
        * C++ default constructor.
        */
        CDcfEntry();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        CDcfEntry( const CDcfEntry& );
        // Prohibit assigment operator if not deriving from CBase.
        CDcfEntry& operator=( const CDcfEntry& );

    public:     // Data
    protected:  // Data
    private:    // Data
        HBufC8* iCid;
        HBufC* iFileName;
        TUint16 iPosition;
        HBufC8* iGroupId;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // CDCFENTRY_H   
            
// End of File
