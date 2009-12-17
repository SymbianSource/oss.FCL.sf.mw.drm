/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class declares the interface of class CDRMClockSession and
*                passes the calls to the DRMClock
*
*/


#ifndef CDRMCLOCKSESSION_H
#define CDRMCLOCKSESSION_H

//  INCLUDES
#include <e32base.h>
#include "DRMClock.h"

// CONSTANTS
// MACROS
// FUNCTION PROTOTYPES
// DATA TYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class RMessage2;

/**
*  Server side instance of DRM Clock session.
*  This class is the server side instance of C/S communications of a certain
*  session.
*
*  @lib DRMCommon.dll
*  @since S60Rel2.6
*/
NONSHARABLE_CLASS( CDRMClockSession ) : public CSession2 
{
  public:  // Constructors and destructor
  
   /**
    * Two-phased constructor.
    *
    * @since S60Rel2.6
    * @param aClock  a pointer to an instance of CDRMClock
    * @return New session instance.
    */
   static CDRMClockSession* NewL( CDRMClock* aClock );

   /**
    * Destructor.
    * @since S60Rel2.6
    */
   virtual ~CDRMClockSession();
   
  public: // New functions
   
  
  public: // Functions from base classes
   
   /**
    * From CSession: Handles the service request event.
    * @since S60Rel2.6
    * @param aMessage The message related to the event.
    * @exception Method leaves with appropriate exception value
    *            if any errors occured.
    */
   void ServiceL( const RMessage2& aMessage );
   
  private:

   /**
    * C++ constructor.
    * @since S60Rel2.6
    * @param aClock  a pointer to an instance of CDRMClock
    */
   CDRMClockSession( CDRMClock* aClock);   

   /**
    * Second phase constructor.
    * @since S60Rel2.6
    */ 
   void ConstructL();
   
   /** 
    * ServiceL() runs DispatchL() under TRAP harness, so
    * all errors can be catched properly. DispatchL() then calls
    * appropriate private method depending on the request.
    * @since S60Rel2.6
    * @param aMessage The message associated to the event.
    */
   void DispatchL( const RMessage2& aMessage );
   
   /** 
    * Helper function for DispatchL(). 
    * @since S60Rel2.6
    * @param aMessage The message associated to the event.
    */
   void GetDRMTimeL( const RMessage2& aMessage );
   

   /** 
    * Helper function for DispatchL(). 
    * @since S60Rel2.6
    * @param aMessage The message associated to the event.
    */
   void UpdateDRMTimeL( const RMessage2& aMessage );
   

   // Prohibit copy constructor.
   CDRMClockSession( const CDRMClockSession& );
   // Prohibit assigment operator.
   CDRMClockSession& operator=( const CDRMClockSession& );

  private:    // Data
      CDRMClock* iDRMClock;
};


#endif      // CDRMCLOCKSESSION_H   

// End of File
