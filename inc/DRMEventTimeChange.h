/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CDRMEVENTTIMECHANGE_H
#define CDRMEVENTTIMECHANGE_H

//  INCLUDES
#include <e32base.h>
#include <DRMTypes.h>
#include <DRMEvent.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  DRM Event notification class to notify about changes to
*  secure time
*
*  @lib DRMCommon.dll
*  @since S60Rel2.8
*/

class CDRMEventTimeChange : public MDRMEvent, public CBase
{
public: // Constructors and destructor


    /**
    * NewLC
    *
    * Creates and instance of the CDRMEventTimeChange class and
    * returns a pointer to it Leaves the object in the cleanup
    * stack
    *
    * @return A functional CDRMEventTimeChange -object
    * @since S60Rel3.0
    */
    IMPORT_C static CDRMEventTimeChange* NewLC();

    /**
    * NewL
    *
    * Creates and instance of the CDRMEventTimeChange class and
    * returns a pointer to it
    *
    * @return A functional CDRMEventTimeChange -object
    * @since S60Rel3.0
    */
    IMPORT_C static CDRMEventTimeChange* NewL();

    /**
    * Destructor
    */
    IMPORT_C virtual ~CDRMEventTimeChange();

public: // new functions


// Time
    /**
    * SetOldTime
    *
    * Sets the time before the change to the given value
    *
    * @param aTime : Old time
    * @since S60Rel2.8
    */
    IMPORT_C void SetOldTime( const TTime& aTime );


    /**
    * GetOldTime
    *
    * Returns what the time was before the time changed
    *
    * @since S60Rel2.8
    */
    IMPORT_C const TTime& GetOldTime() const;


    /**
    * SetNewTime
    *
    * Sets the time after the change to the given value
    *
    * @param aTime : New time
    * @since S60Rel2.8
    */
    IMPORT_C void SetNewTime( const TTime& aTime );


    /**
    * GetNewTime
    *
    * Returns what the time is after the time changed
    *
    * @since S60Rel2.8
    */
    IMPORT_C const TTime& GetNewTime() const;

// Time zone
    /**
    * SetOldTimeZone
    *
    * Sets the timezone before the change to the given value
    * in 15 minute intervals
    *
    * @param aTime : Old timezone
    * @since S60Rel2.8
    */
    IMPORT_C void SetOldTimeZone( const TInt aTimeZone );


    /**
    * GetOldTimeZone
    *
    * Returns what the timezone was before the time changed
    * in 15 minute intervals
    *
    * @since S60Rel2.8
    */
    IMPORT_C TInt GetOldTimeZone() const;


    /**
    * SetNewTimeZone
    *
    * Sets the timezone after the change to the given value
    *
    * @param aTimeZone : New timezone
    * @since S60Rel2.8
    */
    IMPORT_C void SetNewTimeZone( const TInt aTimeZone );


    /**
    * GetNewTimeZone
    *
    * Returns what the timezone is after the time changed
    *
    * @since S60Rel2.8
    */
    IMPORT_C TInt GetNewTimeZone() const;

// Security level

    /**
    * SetOldSecurityLevel
    *
    * Sets the security level before the change to the given value
    *
    * @param aTime : Old security level
    * @since S60Rel2.8
    */
    IMPORT_C void SetOldSecurityLevel( const DRMClock::ESecurityLevel aSecLevel );


    /**
    * GetOldSecurityLevel
    *
    * Returns what the security level was before the time changed
    *
    * @since S60Rel2.8
    */
    IMPORT_C DRMClock::ESecurityLevel GetOldSecurityLevel() const;


    /**
    * SetNewSecurityLevel
    *
    * Sets the security level after the change to the given value
    *
    * @param aSecLevel : New security level
    * @since S60Rel2.8
    */
    IMPORT_C void SetNewSecurityLevel( const DRMClock::ESecurityLevel aSecLevel );


    /**
    * GetNewSecurityLevel
    *
    * Returns what the security level is after the time changed
    *
    * @since S60Rel2.8
    */
    IMPORT_C DRMClock::ESecurityLevel GetNewSecurityLevel() const;

public: // Functions from base classes

    /**
    * ExternalizeL
    *
    * Writes the objects data into a stream
    *
    * @param aOutput : an output stream where to write
    * @since S60Rel2.8
    */
    IMPORT_C virtual void ExternalizeL(RWriteStream& aOutput);

    /**
    * InternalizeL
    *
    * Reads the objects data from a stream
    *
    * @param aInput : an input stream where to read from
    * @since S60Rel2.8
    */
    IMPORT_C virtual void InternalizeL(RReadStream& aInput);


    /**
    * WriteInt64L
    *
    * Writes the 64 bit integer to the stream
    *
    * @since    3.0
    * @param    aWrite : the 64 bit integer to write
    * @param    aStream : the output stream to write to
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void WriteInt64L( const TInt64& aWrite, RWriteStream& aStream ) const;

    /**
    * ReadInt64L
    *
    * Reads the 64 bit integer from the stream
    *
    * @since    3.0
    * @param    aRead : the 64 bit integer read
    * @param    aStream : the output stream to write to
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void ReadInt64L( TInt64& aRead, RReadStream& aStream );

private:

    /**
    * C++ default constructor.
    */
    CDRMEventTimeChange( void );

    /**
    * First Phase constructor
    */
    void ConstructL();

    // Prohibit copy constructor
    CDRMEventTimeChange( const CDRMEventTimeChange& );

    // Prohibit assigment operator
    CDRMEventTimeChange& operator=( const CDRMEventTimeChange& );

    // The old time before the change
    TTime iOldTime;

    // The new time after the change
    TTime iNewTime;

    // The old timezone before the change
    TInt iOldTimeZone;

    // The new timezone after the change
    TInt iNewTimeZone;

    // The old security level before the change
    DRMClock::ESecurityLevel iOldSecurityLevel;

    // The new security level after the change
    DRMClock::ESecurityLevel iNewSecurityLevel;

};

#endif      // CDRMEVENTTIMECHANGE_H

// End of File
