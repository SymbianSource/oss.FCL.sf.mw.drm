/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Datatype implementation for Rights Constraint
*
*/


// INCLUDE FILES
#include <s32strm.h>
#include <s32mem.h>
#include "DrmConstraint.h"

// CONSTANTS

// Synchronizing marker in the beginning of the stream in order to synchronize
// to an externalized Constraint object having the new structure.
const TInt32 KSyncMark = 0xAFCE;

// Old and new version number of the Constraint object
const TInt8 KVersion3_2_0 = 0;
const TInt8 KVersion3_2_1 = 1;

const TInt KSanityDataLengthLow = 0;
const TInt KSanityDataLengthHigh = 32768;


// ============================ LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// SanitizeL
// Performs a sanity check on length parameters
// -----------------------------------------------------------------------------
//
LOCAL_C void SanitizeL( TInt aParam )
    {
    if( aParam < KSanityDataLengthLow || aParam > KSanityDataLengthHigh )
        {
        User::Leave(KErrArgument);
        }
    }

// -----------------------------------------------------------------------------
// AppendToArrayL
// Appends the strings of array aFrom to array aTo
// -----------------------------------------------------------------------------
//
LOCAL_C void AppendToArrayL( RPointerArray<HBufC8>& aTo,
                             const RPointerArray<HBufC8>& aFrom )
    {
    HBufC8* addData = NULL;

    for( TInt i = 0; i < aFrom.Count(); i++ )
        {
        addData = aFrom[i]->AllocLC();
        aTo.AppendL( addData );
        CleanupStack::Pop( addData );
        }
    }

// -----------------------------------------------------------------------------
// CountArrayStoreSize
// Returns the size in bytes how much the array needs for storing
// -----------------------------------------------------------------------------
//
LOCAL_C TInt CountArrayStoreSize( const RPointerArray<HBufC8>& aArray )
    {
    TInt size = 0;

    for(TInt i = 0; i < aArray.Count(); i++ )
        {
        size += sizeof(TInt);
        size += aArray[i]->Size();
        }
    return size;
    }
// -----------------------------------------------------------------------------
// WriteArrayToStreamL
// Write the array to the stream
// -----------------------------------------------------------------------------
//
LOCAL_C void WriteArrayToStreamL( RWriteStream& aStream,
                                  const RPointerArray<HBufC8>& aArray )
    {
    for(TInt i = 0; i < aArray.Count(); i++ )
        {
        aStream.WriteInt32L( aArray[i]->Size() );
        aStream.WriteL( aArray[i]->Des() );
        }
    }

// -----------------------------------------------------------------------------
// ReadArrayFromStringL
// Reads the array from the string
// -----------------------------------------------------------------------------
//
LOCAL_C void ReadArrayFromStringL( const TDesC8& aString,
                                   RPointerArray<HBufC8>& aArray )
    {
    RMemReadStream inRead( static_cast<const TAny*>( aString.Ptr() ), aString.Size() );
    TInt size = 0;
    HBufC8* addData = NULL;
    TPtr8 dataBuffer(NULL,0,0);
    CleanupClosePushL( inRead );


    aArray.ResetAndDestroy();


    for( TInt i = 0; i < aString.Size();)
        {
        // If there is not enough data to read the integer
        // it means that it's an old version and the whole thing is the
        // string since in previous versions only one string is stored
        if(( aString.Size() - i) < sizeof(TInt) )
            {
            aArray.ResetAndDestroy();
            addData = aString.AllocLC();
            aArray.AppendL( addData );
            CleanupStack::Pop();
            CleanupStack::PopAndDestroy(); // inRead
            return;
            }

        size = inRead.ReadInt32L();
        i += sizeof(TInt);

        // If the size is negative or the size left is not large enough
        // it means that it's an old version and the whole thing is the
        // string since in previous versions only one string is stored.
        if( size < 0 || size > ( aString.Size() - i ) )
            {
            aArray.ResetAndDestroy();
            addData = aString.AllocLC();
            aArray.AppendL( addData );
            CleanupStack::Pop();
            CleanupStack::PopAndDestroy(); // inRead
            return;
            }
        addData = HBufC8::NewMaxLC( size );

        // Set the read buffer:
        dataBuffer.Set(const_cast<TUint8*>(addData->Ptr()), 0, size);

        // Read the data:
        inRead.ReadL( dataBuffer );

        aArray.AppendL( addData );
        CleanupStack::Pop( addData );

        i += size;
        }
    CleanupStack::PopAndDestroy();
    return;
    }

// -----------------------------------------------------------------------------
// IsIndividualConstraintValid
// -----------------------------------------------------------------------------
//
LOCAL_C TBool IsIndividualConstraintValid( const RPointerArray<HBufC8>& aConstraint,
                                           const RPointerArray<HBufC8>& aValidConstraints)
    {
    TInt retVal = 0;

    for( TInt i = 0; i < aConstraint.Count(); i++ )
        {
        for( TInt j = 0; j < aValidConstraints.Count(); j++ )
            {
            retVal = aConstraint[i]->Des().Compare( aValidConstraints[j]->Des() );
            if( !retVal )
                {
                return ETrue;
                }
            }
        }
    return EFalse;
    };

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CDRMConstraint::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMConstraint* CDRMConstraint::NewLC()
    {
    CDRMConstraint* self = new( ELeave ) CDRMConstraint();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    };

// -----------------------------------------------------------------------------
// CDRMConstraint::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMConstraint* CDRMConstraint::NewL()
    {
    CDRMConstraint* self = NewLC();
    CleanupStack::Pop();

    return self;
    };

// -----------------------------------------------------------------------------
// Default Constructor - First phase.
// Can be used by itself to generate an empty object
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMConstraint::CDRMConstraint() :
    iSyncMark( KSyncMark ),
    iVersion( KVersion3_2_1 ), // Version number for differentiation
                               // in InternalizeL.
    iStartTime( Time::NullTTime() ),
    iEndTime( Time::NullTTime() ),
    iIntervalStart( Time::NullTTime() ),
    iInterval( 0 ),
    iCounter( 0 ),
    iOriginalCounter( 0 ),
    iTimedCounter( 0 ),
    iTimedInterval( 0 ),
    iAccumulatedTime( 0 ),
    iVendorId( TUid::Null() ),
    iSecureId( TUid::Null() ),
    iActiveConstraints( 0 ),
    iDrmMeteringInfo( NULL ),
    iOriginalTimedCounter( 0 )
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMConstraint::~CDRMConstraint()
    {

    iIndividual.ResetAndDestroy();
    iIndividual.Close();

    iSystem.ResetAndDestroy();
    iSystem.Close();

#ifdef RD_DRM_METERING
    if( iDrmMeteringInfo )
        {
        delete iDrmMeteringInfo;
        iDrmMeteringInfo = NULL;
        }
#endif

    };

// -----------------------------------------------------------------------------
// CDRMConstraint::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMConstraint::ExternalizeL( RWriteStream& aStream ) const
    {

    // used for the buffers
    TInt32 dataLength = 0;

    // write the synchronizing marker
    aStream.WriteInt32L( iSyncMark );

    // Write the version number
    aStream.WriteInt32L( iVersion );

    // Write the start time
    WriteInt64L( iStartTime.Int64(), aStream );

    // Write the end time
    WriteInt64L( iEndTime.Int64(), aStream );

    // Write the interval start time
    WriteInt64L( iIntervalStart.Int64(), aStream );

    // Write the interval
    aStream.WriteInt32L( iInterval.Int() );

    // Write the counter
    aStream.WriteInt32L( iCounter );

    // Write the original counter
    aStream.WriteInt32L( iOriginalCounter );

    // Write the timed counter
    aStream.WriteInt32L( iTimedCounter );

    // Write the timed interval
    aStream.WriteInt32L( iTimedInterval.Int() );

    // Write the accumulated time
    aStream.WriteInt32L( iAccumulatedTime.Int() );

    // Write the individual
    dataLength = 0;
    if ( iIndividual.Count() )
        {
        dataLength = CountArrayStoreSize( iIndividual );
        }
    aStream.WriteInt32L( dataLength );

    if ( dataLength )
        {
        WriteArrayToStreamL( aStream, iIndividual );
        }

    // Software Vendor Id
    aStream.WriteInt32L( iVendorId.iUid );

    // Secure Id of the allowed application
    aStream.WriteInt32L( iSecureId.iUid );

    // Active constraints
    aStream.WriteUint32L( iActiveConstraints );

    // Metering info
#ifdef RD_DRM_METERING
    dataLength = 0;
    if ( iDrmMeteringInfo )
        {
        dataLength = sizeof( TTimeIntervalSeconds ) + sizeof( TUint8 );
        }

    aStream.WriteInt32L( dataLength );

    if ( dataLength )
        {
        aStream.WriteInt32L( iDrmMeteringInfo->iGraceTime.Int() );
        aStream.WriteInt8L( iDrmMeteringInfo->iAllowUseWithoutMetering );
        }

#endif

    // Write the system
    dataLength = 0;
    if ( iSystem.Count() )
        {
        dataLength = CountArrayStoreSize( iSystem );
        }

    aStream.WriteInt32L( dataLength );

    if ( dataLength )
        {
        WriteArrayToStreamL( aStream, iSystem );
        }

    // write the original timed counter
    aStream.WriteInt32L( iOriginalTimedCounter );

    // For future use
    aStream.WriteInt32L( 0 );

    };

// -----------------------------------------------------------------------------
// CDRMConstraint::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMConstraint::InternalizeL( RReadStream& aStream )
    {

    TInt64 timeData = 0;
    TInt32 temp = 0;

    // used for the buffers
    TInt dataLength = 0;
    HBufC8* dataPart = NULL;
    TPtr8 dataBuffer(NULL,0,0);

    // Read the (possible) synchronizing marker.
    iSyncMark = aStream.ReadInt32L();

    if ( iSyncMark != KSyncMark )
        {

        // The structure of the externalized Permission object is the old one.
        // The first four bytes constitute half of the eight bytes of Start time.
        // Read another four bytes from the stream (and apply bit modifications)
        // in order to reconstruct the Start time (iStartTime).
        temp = aStream.ReadInt32L();

        timeData = temp;
        timeData <<= 32;

        Mem::Copy( &timeData, &iSyncMark, sizeof(TInt32) );

        iStartTime = timeData;
        timeData = 0;

        // The version is marked as old version for differentiation in
        // InternalizeL.
        iVersion = KVersion3_2_0;

        }
    else
        {
        // The structure of the externalized Permission object is the new one.
        // Read the version and Start time.
        iVersion = aStream.ReadInt32L();

        // Read the start time
        ReadInt64L( timeData, aStream );
        iStartTime = timeData;

        }

    // Read the end time
    ReadInt64L( timeData, aStream );
    iEndTime = timeData;

    // Read the interval start time
    ReadInt64L( timeData, aStream );
    iIntervalStart = timeData;

    // Read the interval
    iInterval = aStream.ReadInt32L();

    // Read the counter
    iCounter = aStream.ReadInt32L();

    // Read the original counter
    iOriginalCounter = aStream.ReadInt32L();

    // Read the timed counter
    iTimedCounter = aStream.ReadInt32L();

    // Read the timed interval
    iTimedInterval = aStream.ReadInt32L();

    // Read the accumulated time
    iAccumulatedTime = aStream.ReadInt32L();

    // Read the individual
    dataLength = aStream.ReadInt32L();

    SanitizeL( dataLength );

    if( dataLength > 0 )
        {
        // Reserve a new buffer:
        dataPart = HBufC8::NewMaxLC( dataLength );

        // Set the read buffer:
        dataBuffer.Set(const_cast<TUint8*>(dataPart->Ptr()), 0, dataLength);

        // Read the data:
        aStream.ReadL( dataBuffer );


        // Fill the array from the string
        ReadArrayFromStringL( dataBuffer, iIndividual);

        // Pop the buffer
        CleanupStack::PopAndDestroy(); // dataPart

        }
    else
        {
        iIndividual.ResetAndDestroy();
        }


    // Read the system
    if ( iVersion == KVersion3_2_0 ) // Constraint has the old structure.
        {

        dataLength = aStream.ReadInt32L();

        SanitizeL( dataLength );

        if( dataLength > 0 )
            {
            // Reserve a new buffer:
            dataPart = HBufC8::NewMaxLC( dataLength );

            // Set the read buffer:
            dataBuffer.Set( const_cast<TUint8*>(dataPart->Ptr()), 0,
                            dataLength );

            // Read the data:
            aStream.ReadL( dataBuffer );

            // Pop the buffer
            CleanupStack::Pop(); // dataPart

            // If an old content identifier exists delete it
            if ( iSystem.Count() )
                {
                iSystem.ResetAndDestroy();
                }

            // assign the new content id
            iSystem.AppendL( dataPart );
            }
        else
            {
            // If an old system exists delete it
            if ( iSystem.Count() )
                {
                iSystem.ResetAndDestroy();
                }
            }
        }

    // Software Vendor Id
    iVendorId.iUid = aStream.ReadInt32L();

    // Secure Id of the allowed application
    iSecureId.iUid = aStream.ReadInt32L();

    // Active constraints
    iActiveConstraints = aStream.ReadUint32L();

#ifdef RD_DRM_METERING

    // Do not read metering information if the version
    // is the old one because Metering is not activated in it.
    if ( iVersion == KVersion3_2_1 )
        {

        // Metering info
        dataLength = aStream.ReadInt32L();

        SanitizeL( dataLength );

        if( dataLength > 0 )
            {

            if( !iDrmMeteringInfo )
                {
                // Reserve a new metering information instance
                iDrmMeteringInfo = new (ELeave)TDrmMeteringInfo;
                }
            else
                {
                iDrmMeteringInfo->iGraceTime = 0;
                iDrmMeteringInfo->iAllowUseWithoutMetering = EFalse;
                }

            // Read grace time
            iDrmMeteringInfo->iGraceTime = aStream.ReadInt32L();

            // Read whether content can be consumed without
            // metering being used
            iDrmMeteringInfo->iAllowUseWithoutMetering =
                aStream.ReadInt8L();

            }
        else
            {

            // If old metering information exists delete it
            if( iDrmMeteringInfo )
                {
                delete iDrmMeteringInfo;
                iDrmMeteringInfo = NULL;
                }
            }
        }

#endif //RD_DRM_METERING

    // Read the system and original timed counter
    // according to the new structure.
    if ( iVersion == KVersion3_2_1 )
        {

        dataLength = aStream.ReadInt32L();

        SanitizeL( dataLength );

        if( dataLength > 0 )
            {
            // Reserve a new buffer:
            dataPart = HBufC8::NewMaxLC( dataLength );

            // Set the read buffer:
            dataBuffer.Set(const_cast<TUint8*>(dataPart->Ptr()), 0, dataLength);

            // Read the data:
            aStream.ReadL( dataBuffer );

            // Fill the array from the string
            ReadArrayFromStringL( dataBuffer, iSystem);

            // Pop the buffer
            CleanupStack::PopAndDestroy(); // dataPart

            }
        else
            {
            iSystem.ResetAndDestroy();
            }

        // Read the original timed counter
        iOriginalTimedCounter = aStream.ReadInt32L();

        // For future use or development, reads the data at the end of the stream
        dataLength = aStream.ReadInt32L();

        SanitizeL( dataLength );

        if ( dataLength > 0 )
            {

            // Reserve a new buffer:
            dataPart = HBufC8::NewMaxLC( dataLength );

            // Set the read buffer:
            dataBuffer.Set(const_cast<TUint8*>(dataPart->Ptr()), 0, dataLength);

            // Read the data:
            aStream.ReadL( dataBuffer );

            // Pop the buffer
            CleanupStack::PopAndDestroy( dataPart );
            }
        }

    // Constraint can be considered to have the new structure from now on.
    if ( iVersion == KVersion3_2_0 )
        {
        iSyncMark = KSyncMark;
        iVersion = KVersion3_2_1;
        }

    };

// -----------------------------------------------------------------------------
// CDRMConstraint::Stateful
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMConstraint::Stateful() const
    {
    // counters, timed counters and accumulated are stateful
    if ( iActiveConstraints &  (EConstraintCounter |
                               EConstraintTimedCounter |
                               EConstraintAccumulated ) )
        {
        return ETrue;
        }

     // Non-Activated interval is stateful
    if ( ( iActiveConstraints & EConstraintInterval ) &&
        iIntervalStart == Time::NullTTime() )
        {
        return ETrue;
        }

    return EFalse;
    };

// -----------------------------------------------------------------------------
// CDRMConstraint::Size
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMConstraint::Size() const
    {
    TInt size = 0;

    // synchronizing marker
    size += sizeof(TInt32);

    // version number
    size += sizeof(TInt32);

    // usage start time
    size += sizeof(TTime);

    // usage end time
    size += sizeof(TTime);

    // interval start time
    size += sizeof(TTime);

    // interval duration
    size += sizeof(TTimeIntervalSeconds);

    // counter
    size += sizeof(TDRMCounter);

    // original counter value
    size += sizeof(TDRMCounter);

    // timed counter
    size += sizeof(TDRMCounter);

    // Interval of the timed counter constraint
    size += sizeof(TTimeIntervalSeconds);

    // accumulated time
    size += sizeof(TTimeIntervalSeconds);

    // individual allowed usage
    size += sizeof(TInt32);
    size += CountArrayStoreSize( iIndividual );

    // Software Vendor Id
    size += sizeof(TUid);

    // Secure Id of the allowed application
    size += sizeof(TUid);

    // Bitmask of active constraints
    size += sizeof(TUint32);

#ifdef RD_DRM_METERING
    // Metering information
    size += sizeof(TInt32);

    if (iDrmMeteringInfo)
        {
        size += sizeof(TTimeIntervalSeconds);
        size += sizeof(TUint8);
        }

#endif //RD_DRM_METERING

    // system allowed usage
    size += sizeof(TInt32);
    size += CountArrayStoreSize(iSystem);

    // original timed counter value
    size += sizeof(TDRMCounter);

    // For future use
    size += sizeof(TInt32);

    return size;

    };

// -----------------------------------------------------------------------------
// CDRMConstraint::Expired
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMConstraint::Expired( const TTime& aTime ) const
    {
    // Full Rights do not expire
    if ( iActiveConstraints == EConstraintNone )
        {
        return EFalse;
        }

    // First check counters, accumulated time and timed counters
    // if any of these is expired the whole thing is expired regardless of the
    // actual time based constrants or future rights

    // Counters
    if ( ( iActiveConstraints & EConstraintCounter ) && iCounter < 1 )
        {
        return ETrue;
        }

    // Accumulated time
    if ( ( iActiveConstraints & EConstraintAccumulated ) && iAccumulatedTime.Int() == 0 )
        {
        return ETrue;
        }

     // Timed Counters
    if ( ( iActiveConstraints & EConstraintTimedCounter ) && iTimedCounter < 1 )
        {
        return ETrue;
        }


    // Dont check time based rights
    if ( aTime != Time::NullTTime() )
        {

        // Check for activated intervals
        if ( ( iActiveConstraints & EConstraintInterval) && iIntervalStart != Time::NullTTime() )
            {
            TTimeIntervalSeconds current;

            aTime.SecondsFrom( iIntervalStart, current );

            if ( current >= iInterval )
                {
                return ETrue;
                }
            }

        // Check for end time
        if ( ( iActiveConstraints & EConstraintEndTime ) && aTime >= iEndTime )
            {
            return ETrue;
            }

        // Check for start time, future rights
        if ( ( iActiveConstraints & EConstraintStartTime ) && aTime < iStartTime )
            {
            return EFalse;
            }
        }

    return EFalse;
    }
// -----------------------------------------------------------------------------
// CDRMConstraint::Merge
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMConstraint::Merge( const CDRMConstraint& aConstraint )
    {
    TInt error = KErrNone;

    if ( this != &aConstraint )
        {
        if ( aConstraint.iActiveConstraints & EConstraintStartTime )
            {
            if ( iActiveConstraints & EConstraintStartTime )
                {
                iStartTime = Max( iStartTime, aConstraint.iStartTime );
                }
            else
                {
                iStartTime = aConstraint.iStartTime;
                iActiveConstraints |= EConstraintStartTime;
                }
            }

        if ( aConstraint.iActiveConstraints & EConstraintEndTime )
            {
            if ( iActiveConstraints & EConstraintEndTime )
                {
                iEndTime = Min( aConstraint.iEndTime, iEndTime );
                }
            else
                {
                iEndTime = aConstraint.iEndTime;
                iActiveConstraints |= EConstraintEndTime;
                }
            }

        if ( aConstraint.iActiveConstraints & EConstraintCounter )
            {
            if ( iActiveConstraints & EConstraintCounter )
                {
                iCounter = Min( iCounter, aConstraint.iCounter );
                iOriginalCounter = Min( iOriginalCounter, aConstraint.iOriginalCounter );
                }
            else
                {
                iCounter = aConstraint.iCounter;
                iOriginalCounter = aConstraint.iOriginalCounter;
                iActiveConstraints |= EConstraintCounter;
                }
            }

        if ( aConstraint.iActiveConstraints & EConstraintInterval )
            {
            if ( iActiveConstraints & EConstraintInterval )
                {
                iIntervalStart = Max( iIntervalStart, aConstraint.iIntervalStart );
                iInterval = Min( iInterval, aConstraint.iInterval );
                }
            else
                {
                iIntervalStart = aConstraint.iIntervalStart;
                iInterval = aConstraint.iInterval;
                iActiveConstraints |= EConstraintInterval;
                }
            }

        if ( aConstraint.iActiveConstraints & EConstraintTimedCounter )
            {
            if ( iActiveConstraints & EConstraintTimedCounter )
                {
                if ( aConstraint.iTimedCounter < iTimedCounter )
                    {
                    iTimedCounter = aConstraint.iTimedCounter;
                    iTimedInterval = aConstraint.iTimedInterval;
                    }
                }
            else
                {
                iTimedCounter = aConstraint.iTimedCounter;
                iTimedInterval = aConstraint.iTimedInterval;
                iActiveConstraints |= EConstraintTimedCounter;
                }
            }

        if ( aConstraint.iActiveConstraints & EConstraintAccumulated )
            {
            if ( iActiveConstraints & EConstraintAccumulated )
                {
                iAccumulatedTime = Min( iAccumulatedTime, aConstraint.iAccumulatedTime );
                }
            else
                {
                iAccumulatedTime = aConstraint.iAccumulatedTime;
                iActiveConstraints |= EConstraintAccumulated;
                }
            }

        if( aConstraint.iActiveConstraints & EConstraintIndividual )
            {
            // Ignore the error since we don't return an error code or leave
            TRAP( error, AppendToArrayL( iIndividual, aConstraint.iIndividual ) );
            }

        }
    }

// -----------------------------------------------------------------------------
// CDRMConstraint::Consume
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMConstraint::Consume( const TTime& aCurrentTime )
    {
    if ( ( iActiveConstraints & EConstraintInterval ) &&
        iIntervalStart == Time::NullTTime() )
        {
        iIntervalStart = aCurrentTime;
        }

    if ( iActiveConstraints & EConstraintCounter )
        {
        --iCounter;
        }
    }

// -----------------------------------------------------------------------------
// CDRMConstraint::DuplicateL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMConstraint::DuplicateL( const CDRMConstraint& aConstraint )
    {

    // synchronizing marker
    iSyncMark = aConstraint.iSyncMark;

    // version number
    iVersion = aConstraint.iVersion;

    // usage start time
    iStartTime = aConstraint.iStartTime;

    // usage end time
    iEndTime = aConstraint.iEndTime;

    // interval start time
    iIntervalStart = aConstraint.iIntervalStart;

    // interval duration
    iInterval = aConstraint.iInterval;

    // counter
    iCounter = aConstraint.iCounter;

    // original counter value
    iOriginalCounter = aConstraint.iOriginalCounter;

    // timed counter
    iTimedCounter = aConstraint.iTimedCounter;

    // Interval of the timed counter constraint
    iTimedInterval = aConstraint.iTimedInterval;

    // accumulated time
    iAccumulatedTime = aConstraint.iAccumulatedTime;

    // individual allowed usage
    iIndividual.ResetAndDestroy();

    AppendToArrayL( iIndividual, aConstraint.iIndividual );

    // Software Vendor Id
    iVendorId = aConstraint.iVendorId;

    // Secure Id of the allowed application
    iSecureId = aConstraint.iSecureId;

    // Bitmask of active constraints
    iActiveConstraints = aConstraint.iActiveConstraints;    // Bitmask

#ifdef RD_DRM_METERING
    // Metering information
    if ( aConstraint.iDrmMeteringInfo )
        {

        if( !iDrmMeteringInfo )
            {
            iDrmMeteringInfo = new (ELeave) TDrmMeteringInfo;
            }

        iDrmMeteringInfo->iGraceTime = aConstraint.iDrmMeteringInfo->iGraceTime;
        iDrmMeteringInfo->iAllowUseWithoutMetering = aConstraint.iDrmMeteringInfo->iAllowUseWithoutMetering;
        }
#endif

    // system allowed usage
    iSystem.ResetAndDestroy();
    AppendToArrayL( iSystem, aConstraint.iSystem );

    // original timed counter value
    iOriginalTimedCounter = aConstraint.iOriginalTimedCounter;

    };


// -----------------------------------------------------------------------------
// CDRMConstraint::Valid
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMConstraint::Valid( const TTime& aTime,
                                      const RPointerArray<HBufC8>& aIndividual,
                                      TUint32& aRejection ) const
    {
    TBool drmTime = EFalse;
    // Null the rejection requirement:
    aRejection = EConstraintNone;

    // Full Rights are always valid
    if ( iActiveConstraints == EConstraintNone )
        {
        return ETrue;
        }

    // First check counters, accumulated time and timed counters
    // if any of these are invalid the whole thing is invalid regardless of the
    // actual time based constraints

    // Counters
    if ( ( iActiveConstraints & EConstraintCounter ) && iCounter < 1 )
        {
        aRejection |= EConstraintCounter;
        }

    // Accumulated time
    if ( ( iActiveConstraints & EConstraintAccumulated ) && iAccumulatedTime.Int() == 0 )
        {
        aRejection |= EConstraintAccumulated;
        }

     // Timed Counters
    if ( ( iActiveConstraints & EConstraintTimedCounter ) && iTimedCounter < 1 )
        {
        aRejection |= EConstraintTimedCounter;
        }

    // Dont check time based rights
    if ( aTime != Time::NullTTime() )
        {
        drmTime = ETrue;
        // Check for activated intervals
        if ( (iActiveConstraints & EConstraintInterval) && iIntervalStart != Time::NullTTime() )
            {
            TTimeIntervalSeconds current;

            aTime.SecondsFrom( iIntervalStart, current );

            if ( ( current >= iInterval ) || ( aTime < iIntervalStart ) )
                {
                aRejection |= EConstraintInterval;
                }
            }

        // Check for end time
        if ( ( iActiveConstraints & EConstraintEndTime ) && aTime >= iEndTime )
            {
            aRejection |= EConstraintEndTime;
            }

        if ( ( iActiveConstraints & EConstraintStartTime ) && aTime < iStartTime )
            {
            aRejection |= EConstraintStartTime;
            }
        }
    else
        {
        drmTime = EFalse;

        // Check for activated intervals
        if ( (iActiveConstraints & EConstraintInterval) )
            {
            aRejection |= EConstraintInterval;
            }

        // Check for end time
        if ( ( iActiveConstraints & EConstraintEndTime ) )
            {
            aRejection |= EConstraintEndTime;
            }

        if( ( iActiveConstraints & EConstraintStartTime ) )
            {
            aRejection |= EConstraintStartTime;
            }
        }

    // IMSI Checking:
    if( iActiveConstraints & EConstraintIndividual )
        {
        if ( !aIndividual.Count() && !iIndividual.Count() )
            {
            aRejection |= EConstraintIndividual;
            }
        else if( !IsIndividualConstraintValid( iIndividual, aIndividual ) )
            {
            aRejection |= EConstraintIndividual;
            }
        }

    if( aRejection )
        {
        // drmTime is null, so some constraints may have been discarded because of that
        if( !drmTime )
            {
            aRejection |= EConstraintNullDrmTime;
            }
        return EFalse;
        }

    return ETrue;
    };


// -----------------------------------------------------------------------------
// CDRMConstraint::ConstructL
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CDRMConstraint::ConstructL()
    {
    };

// -----------------------------------------------------------------------------
// CDRMConstraint::WriteInt64L
// -----------------------------------------------------------------------------
//
void CDRMConstraint::WriteInt64L( const TInt64& aWrite, RWriteStream& aStream ) const
    {
    TPtr8 output(NULL,0,0);

    output.Set( reinterpret_cast<TUint8*>(const_cast<TInt64*>(&aWrite)),
                sizeof(TInt64), sizeof(TInt64) );

    aStream.WriteL( output, sizeof(TInt64) );
    };

// -----------------------------------------------------------------------------
// CDRMConstraint::ReadInt64L
// -----------------------------------------------------------------------------
//
void CDRMConstraint::ReadInt64L( TInt64& aRead, RReadStream& aStream )
    {
    TPtr8 input(NULL,0,0);

    input.Set( reinterpret_cast<TUint8*>(&aRead), 0, sizeof(TInt64) );

    aStream.ReadL( input, sizeof(TInt64) );
    };
// End of File
