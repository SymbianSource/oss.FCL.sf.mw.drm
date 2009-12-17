/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Rights object notification event
*
*/



#ifndef CDRMEVENTADDREMOVE_H
#define CDRMEVENTADDREMOVE_H

//  INCLUDES
#include <e32base.h>
#include <DRMEvent.h>   

// CONSTANTS

// MACROS

// DATA TYPES
typedef enum 
    {
    ERightsObjectRecieved = 0,
    ERightsObjectDeleted = 1,
    ERightsObjectDeletedAll = 2,
    } TAddRemoveEventStatus;

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  DRM Rights Object notication event class
*  Used as data structure in the notification service
*
*  @lib DRMCommon.dll
*  @since S60Rel2.6
*/

class CDRMEventAddRemove : public MDRMEvent, public CBase 
{
public: // Constructors and destructor


    /**
    * NewLC
    *
    * Creates and instance of the CDRMEventAddRemove class and
    * returns a pointer to it Leaves the object in the cleanup
    * stack
    *
    * @return A functional CDRMEventAddRemove -object
    * @since S60Rel3.0
    */
    IMPORT_C static CDRMEventAddRemove* NewLC( TAddRemoveEventStatus aStatus );

    /**
    * NewL
    *
    * Creates and instance of the CDRMEventAddRemove class and
    * returns a pointer to it
    *
    * @return A functional CDRMEventAddRemove -object
    * @since S60Rel3.0
    */
    IMPORT_C static CDRMEventAddRemove* NewL( TAddRemoveEventStatus aStatus );
    
    /**
    * Destructor
    */   
    IMPORT_C virtual ~CDRMEventAddRemove();

public: // new functions


    /**
    * SetContentIDL
    *
    * Sets the content id of the object and leaves if an error occurs
    *
    * @param aContentID : Descriptor containing the content id
    * @since S60Rel2.6
    */
    IMPORT_C virtual void SetContentIDL( const TDesC8& aContentID );

    /**
    * Status
    *
    * Returns the Rights Object event status
    *
    * @param aEventType : Out parameter for the event type
    * @since S60Rel2.6
    */
    IMPORT_C virtual TAddRemoveEventStatus Status() const;

    /**
    * GetContentIDL
    *
    * Returns a pointer to newly reserved buffer that containts the
    * content id. The caller must free the memory. 
    *
    * @since S60Rel2.6
    */
    IMPORT_C virtual HBufC8* GetContentIDL() const;

public: // Functions from base classes
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

private:

    /**
    * Constructor
    *
    * @param  aStatus the status of the event as TAddRemoveEventStatus 
    */
    CDRMEventAddRemove( TAddRemoveEventStatus aStatus );
    
    /**
    * C++ default constructor.
    */
    CDRMEventAddRemove( void );

    /**
    * First Phase constructor
    */
    void ConstructL();
   
    // Prohibit copy constructor
    CDRMEventAddRemove( const CDRMEventAddRemove& );

    // Prohibit assigment operator
    CDRMEventAddRemove& operator=( const CDRMEventAddRemove& );

    // The content identifier
    HBufC8* iContentID;

    // The status of the event
    TAddRemoveEventStatus iStatus;
};

#endif      // CDRMEVENTADDREMOVE_H
            
// End of File
