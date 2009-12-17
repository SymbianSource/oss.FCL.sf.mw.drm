/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Nitz information observer used by DRMClock
*
*/


#ifndef DRMNITZOBSERVER_H
#define DRMNITZOBSERVER_H

// INCLUDES

#include <e32base.h>	// CBase
#include <e32std.h>
#include <e32def.h>		// Type definitions
#include <bacntf.h>
#include <etelmm.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CDRMClock;

/**
*  CDRMNitzObserver implements the NITZ observer
*  required by DRM Clock
*
*  @lib unipertar.exe
*  @since 2.8
*/
NONSHARABLE_CLASS( CDRMNitzObserver ) : public CActive
    {
    public:
        /**
        * NewL
        *
        * Creates an instance of the CDRMNitzObserver class and returns a pointer to it
        *
        * @since  2.8
        * @param aMobilePhone : connected handle to RMobilePhone
        * @param aDRMClock : pointer to an instance of CDRMClock
        * @return Functional CDRMNitzObserver object, Function leaves if an error occurs.
        */    
        static CDRMNitzObserver* NewL( RMobilePhone& aMobilePhone, 
                                       CDRMClock* aDRMClock);
        
        /**
        * Start
        *
        * Adds the instance to the active scheduler unless it's already there and
        * completes a request so RunL() gets called.
        *
        * @since  2.8
        * @return none
        */  
        void Start();  
         
        /**
        * From CActive: RunL catches events from The phone server
        */
        void RunL();
                  
        void DoCancel();
        
        // Destructor
        virtual ~CDRMNitzObserver();

    protected:
          
        /**
        * From CActive: RunError checks the errors from RunL.
        */
        TInt RunError( TInt aError );   
    
    private:
        /* constructor */
        CDRMNitzObserver( RMobilePhone& aMobilePhone, CDRMClock* aDRMClock);
        
        /* secondary constructor */
        void ConstructL();
            
        /**
        * CheckDateTimeVal
        *
        * Validates the given datetime
        *
        * @since 2.8
        * @param aDateTime : the time to be validated
        * @return System wide error code
        */
        TInt CheckDateTimeVal(const TDateTime& aDateTime);

        /**
        * GetNitzTime
        *
        * Gets the NITZ time
        *
        * @since 2.8
        * @param aNitzTime : the requested NITZ time in UTC 
        * @param aTimeZone : the timezone in +/- 15 minutes 
        * @return ETrue if NITZ is available a
        *         EFalse if NITZ is not available
        */
        TBool GetNitzTime(TTime& aNitzTime, TInt& aTimeZone);
               
        CDRMClock* iDRMClock;
        RMobilePhone* iMobilePhone;
        RMobilePhone::TMobilePhoneNITZ iNitzInfo;
        TInt iError;
    };
    
#endif // DRMNITZOBSERVER_H

// End of File