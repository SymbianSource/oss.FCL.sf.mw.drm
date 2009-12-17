/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Datatype for Rights Constraint
*
*/


#ifndef DRMCONSTRAINT_H
#define DRMCONSTRAINT_H

// INCLUDES

#include <e32base.h>
#include "DRMTypes.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FORWARD DECLARATIONS
class RReadStream;
class RWriteStream;

// TYPE DEFINITIONS

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
*  CDRMConstraint implements a constraint for OMA DRM usage
*
*  @lib DrmRights.dll
*  @since S60 3.0
*/

class CDRMConstraint : public CBase
    {
    
public:

    struct TDrmMeteringInfo
        {
        TDrmMeteringInfo() : 
            iGraceTime( 0 ), 
            iAllowUseWithoutMetering( EFalse ){};
        
        TTimeIntervalSeconds iGraceTime;
        TBool iAllowUseWithoutMetering;
        };
    
public: // Constructors and destructor
    /**
    * NewLC
    *
    * Creates a new CDRMConstraint object and returns a pointer to it.
    * The function leaves the object to the cleanup stack.
    *
    * @since    3.0
    * @return   a functional CDRMConstraint object, The function leaves
    *           with SymbianOS error code if an error occurs.
    */
    IMPORT_C static CDRMConstraint* NewLC();

    /**
    * NewL
    *
    * Creates a new CDRMConstraint object and returns a pointer to it.
    *
    * @since    3.0
    * @return   a functional CDRMConstraint object, The function leaves
    *           with SymbianOS error code if an error occurs.
    */
    IMPORT_C static CDRMConstraint* NewL();
    
    /*
    * Destructor
    */ 
    IMPORT_C virtual ~CDRMConstraint(); 


public: // New functions
    /**
    * ExternalizeL
    * 
    * Writes the data of the object into the stream
    *
    * @since    3.0
    * @param    aStream : the output stream to write to
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
    * InternalizeL
    * 
    * Reads the data of the object from the stream
    *
    * @since    3.0
    * @param    aStream : the output stream to write to
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    IMPORT_C void InternalizeL( RReadStream& aStream );
    
    
    /**
    * Stateful
    * 
    * The function checks if the constraint is stateful
    *
    * @since    3.0
    * @return   returns ETrue if the constraint is stateful
    *                   EFalse if it is not.
    */
    IMPORT_C TBool Stateful() const;

    /**
    * Size
    *
    * returns the size of the externalized object
    *
    * @since    3.0
    * @return   returns the size of the constraint
    */   
    IMPORT_C TInt Size() const;
   
    /**
    * Expired
    *
    * returns a boolean valua about the validity of the constraint
    * Future rights are considered not be be expired and EFalse
    * is returned for them
    *
    * @since    3.0
    * @param	aTime : current secure time, if the time is
    *			        Time::NullTTime() time based constraints
    *					will not be considered to be expised and 
    *					EFalse is returned for them
    * @return   returns ETrue if the constraint is expired
    *					EFalse if the constraint is not expired
    *
    */     
    IMPORT_C TBool Expired( const TTime& aTime ) const;

    /**
    * Merge
    *
    * Merges the two constraints
    *
    * @since    3.0
    * @param	aConstraint : the constraint merged to this one
    * @return   none
    */     
    IMPORT_C void Merge( const CDRMConstraint& aConstraint );


    /**
    * Consume
    *
    * Consumes rights
    *
    * @since    3.0
    * @param	aCurrentTime : The current secure time
    * @return   none
    */     
    IMPORT_C void Consume( const TTime& aCurrentTime );
    
    
    /**
    * DuplicateL
    *
    * copies all data from the given constraint to the current one
    *
    * @since    3.0
    * @param	aConstraint : the constraint to copy from
    * @return   none, Leaves with symbian OS error code if an error occurs
    */ 
    IMPORT_C void DuplicateL( const CDRMConstraint& aConstraint );
    
    /**
    * Valid
    *
    * returns if the constraint is valid/usable at this time
    *
    * @since    3.0
    * @param	aTime : current secure time, if the time is
    *			        Time::NullTTime() time based constraints
    *					will be considered to be invalid
    * @param    aIndividual : the imsi of the phone
    * @param    aRejection : return value for why the content was rejected    
    * @return   returns ETrue if the constraint is valid
    *					EFalse if the constraint is not valid
    */ 
    IMPORT_C TBool Valid( const TTime& aTime,
                          const RPointerArray<HBufC8>& aIndividual,
                          TUint32& aRejection ) const;
    
public:

    // synchronizing marker
    TInt32 iSyncMark;
    
    // version number
    TInt32 iVersion;
 
    // usage start time
    TTime iStartTime;

    // usage end time
    TTime iEndTime;

    // interval start time
    TTime iIntervalStart;

    // interval duration
    TTimeIntervalSeconds iInterval;

    // counter
    TDRMCounter iCounter;

    // original counter value
    TDRMCounter iOriginalCounter;

    // timed counter
    TDRMCounter iTimedCounter;

    // Interval of the timed counter constraint
    TTimeIntervalSeconds iTimedInterval;

    // accumulated time
    TTimeIntervalSeconds iAccumulatedTime;

    // individual allowed usage
    RPointerArray<HBufC8> iIndividual;
	
    // Software Vendor Id
    TUid iVendorId;
    
    // Secure Id of the allowed application
    TUid iSecureId;

	// Bitmask of active constraints
    TUint32 iActiveConstraints;	// Bitmask
    
    // Metering allowance information
    TDrmMeteringInfo* iDrmMeteringInfo;
    
    // system allowed usage
	RPointerArray<HBufC8> iSystem;

    // Original timed counter
    TDRMCounter iOriginalTimedCounter;

protected:

    /*
    * Default Constructor
    */ 
    IMPORT_C CDRMConstraint();

private:

    /**
    * 2nd phase constructor
    */
    void ConstructL();

    /**
    * Copy constructor.
    */
    CDRMConstraint( const CDRMConstraint& aConstraint ); // Prevented
    
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
    };

#endif      // DRMCONSTRAINT_H   
            
// End of File
