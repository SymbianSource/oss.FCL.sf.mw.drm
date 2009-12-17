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



#ifndef CDRMEVENTMODIFY_H
#define CDRMEVENTMODIFY_H

//  INCLUDES
#include <e32base.h>
#include <DRMEvent.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  DRM Rights Object notication event class
*  Used as data structure in the notification service
*
*  @lib DRM Common
*  @since S60Rel2.6
*/

class CDRMEventModify : public MDRMEvent, public CBase
{
public: // Constructors and destructor


    /**
    * NewLC
    *
    * Creates and instance of the CDRMEventModify class and
    * returns a pointer to it Leaves the object in the cleanup
    * stack
    *
    * @return A functional CDRMEventModify -object
    * @since S60Rel3.0
    */
    IMPORT_C static CDRMEventModify* NewLC();

    /**
    * NewL
    *
    * Creates and instance of the CDRMEventModify class and
    * returns a pointer to it
    *
    * @return A functional CDRMEventModify -object
    * @since S60Rel3.0
    */
    IMPORT_C static CDRMEventModify* NewL();


    /**
    * Destructor
    */   
    IMPORT_C virtual ~CDRMEventModify();

public: // new functions


    /**
    * SetContentIDL
    *
    * Sets the content id of the object and leaves if an error occurs
    *
    * @param aContentID : Descriptor containing the content id
    * @since S60Rel2.6
    */
    IMPORT_C void SetContentIDL( const TDesC8& aContentID );

    /**
    * SetUniqueID
    *
    * Sets the content id of the related rights object
    *
    * @param aUniqueID : unique id of the rights object in question
    * @since S60Rel2.6
    */
    IMPORT_C void SetUniqueID( const TUint32 aUniqueID );

    /**
    * UniqueID
    *
    * Returns the unique id of the rights object modified
    *
    * @since S60Rel2.6
    */
    IMPORT_C TUint32 UniqueID() const;

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
    */
    CDRMEventModify(TUint32 aUniqueID);
    
    /**
    * C++ default constructor.
    */
    CDRMEventModify( void );

    /**
    * First Phase constructor
    */
    void ConstructL();
   
    // Prohibit copy constructor 
    CDRMEventModify( const CDRMEventModify& );

    // Prohibit assigment operator 
    CDRMEventModify& operator=( const CDRMEventModify& );

    // The content identifier
    HBufC8* iContentID;

    // the unique id of the rights object in question
    TUint32 iUniqueID;
};

#endif      // CDRMEVENTADDREMOVE_H
            
// End of File
