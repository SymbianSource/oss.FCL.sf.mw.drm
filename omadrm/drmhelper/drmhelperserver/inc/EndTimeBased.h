/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handling end time based automated content
*
*/



#ifndef ENDTIMEBASED_H
#define ENDTIMEBASED_H

//  INCLUDES
#include <e32base.h>
// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CDRMHelperServer;

// CLASS DECLARATION

/**
*  End time based rights storage class
*  
*
*  @lib DRMHelperServer.lib
*  @since Series60 2.6
*/
class CEndTimeBased : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CEndTimeBased* NewL( 
            const TDesC8& aUri , 
            const TUint8& aPermType , 
            const TUint8& aRegType , 
            const TUint8& aAutoType );
        
        /**
        * Destructor.
        */
        virtual ~CEndTimeBased();

    public: // New functions
        
        /**
        * SetIsExpired
        * set whether the content rights is expired
        * @since Series60 2.6
        * @param aIsExpired if expired or not
        * 
        */
        void SetIsExpired( TBool aIsExpired );

        /**
        * check if the content rights is expired
        * @since Series60 2.6
        * @return ETrue if expired otherwise EFalse,
        */
        TBool IsExpired() const;
        
        /**
        * Set the value of count
        * @since Series60 2.6
        * @param aCount the times the content was handled
        */
        void SetCount(TInt8 aCount);

        /**
        * IncreaseCount the value of count by 1
        * @since Series60 2.6
        * 
        */
        void IncreaseCount();
        
        /**
        * get the value of count
        * @since Series60 2.6
        * @return the value of count
        */
        TInt8 Count() const;

        /**
        * IncreaseRegTimes the value of registered times by 1
        * @since Series60 2.6
        * 
        */
        void IncreaseRegTimes();

        /**
        * DecreaseRegTimes the value of registered times by 1
        * @since Series60 2.6
        * 
        */
        void DecreaseRegTimes();

        /**
        * Set the value of registered times
        * @since Series60 2.6
        * @param aCount the times the content was registered
        */
        void SetRegTimes(TInt aRegTimes);
        
        /**
        * get the value of times the content is registered
        * @since Series60 2.6
        * @return the value of count
        */
        TInt RegTimes() const;

        /**
        * get the value of register type
        * @since Series60 2.6
        * @return the value of register type
        */
        TUint8 RegType() const;

        /**
        * get the value of automated content type
        * @since Series60 2.6
        * @return the value of automated content type
        */
        TUint8 AutoType() const;

        /**
        * get the value of permission type
        * @since Series60 2.6
        * @return the value of permission type
        */
        TUint8 PermType() const;        

        /**
        * Set the value of end time
        * @since Series60 2.6
        * @param aEndTime end time for content rights
        */
        void SetEndTime( TTime aEndTime );

        /**
        * get the value of end time
        * @since Series60 2.6
        * @return the value of end time
        */
        TTime EndTime() const;

        /**
        * Set the value of content uri
        * @since Series60 2.6
        * @param aUri content uri for the automated content
        * @return KErrNone if everything works fine.
        */
        TInt SetContentUri( const TDesC8& aUri );

        /**
        * get the value of content uri
        * @since Series60 2.6
        * @return the value of content uri
        */
        HBufC8* Uri() const;

        

    public: // Functions from base classes

    protected:  // New functions
        
    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CEndTimeBased( 
            const TUint8& aPermType , 
            const TUint8& aRegType , 
            const TUint8& aAutoType );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC8& aUri );
    public:     // Data
    protected:  // Data
    private:    // Data
    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes
        HBufC8* iUri;
        TTime iEndTime;
        TBool iIsExpired;
        TInt8 iCount;
        TInt iCountReg;
        TUint8 iPermType;
        TUint8 iRegType;
        TUint8 iAutoType;
    };

#endif      // ENDTIMEBASED_H   
            
// End of File
