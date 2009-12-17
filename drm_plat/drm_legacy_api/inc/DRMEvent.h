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
* Description:  Notification Event object base class
*
*/



#ifndef MDRMEVENT_H
#define MDRMEVENT_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h> 

// CONSTANTS

// MACROS

// DATA TYPES
typedef TUint32 TDRMEventType;

const TDRMEventType KEventNone      = 0;
const TDRMEventType KEventAddRemove = 1;
const TDRMEventType KEventModify    = 2;
const TDRMEventType KEventTimeChange = 3;

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  This class offers the base class for all DRM Notification events
*  It can be used as it is, but it is meant that Other specific to an event
*  classes are derived from it
*
*  @lib DRMCommon.dll    
*  @since S60Rel2.6
*/

class MDRMEvent
{
public: // Constructors and destructor
    /**
    * Constructor
    *
    * @param aEvent The event type of the object to be created
    */
    IMPORT_C MDRMEvent(const TDRMEventType& aEvent); 

    /**
    * Destructor
    */
    IMPORT_C virtual ~MDRMEvent();

public: // new functions
    /**
    * ExternalizeL
    *
    * Writes the objects data into a stream
    *
    * @param aOutput : an output stream where to write
    * @since S60Rel2.6
    */
    IMPORT_C virtual void ExternalizeL(RWriteStream& aOutput);
    
    /**
    * InternalizeL
    *
    * Reads the objects data from a stream
    *
    * @param aInput : an input stream where to read from
    * @since S60Rel2.6
    */
    IMPORT_C virtual void InternalizeL(RReadStream& aInput);

    /**
    * GetEventType
    *
    * Returns the event type of the Event in aEventType
    *
    * @param aEventType : Out parameter for the event type
    * @since S60Rel2.6
    */
    IMPORT_C void GetEventType(TDRMEventType& aEventType) const;

protected:  // Data

    TDRMEventType iType;

private:   
    /**
    * Default constructor - prevented
    */
    MDRMEvent( void );
    
    // Prohibit copy constructor
    MDRMEvent( const MDRMEvent& );

    // Prohibit assigment operator
    MDRMEvent& operator=( const MDRMEvent& );    
};

#endif      // MDRMEVENT_H

// End of File
