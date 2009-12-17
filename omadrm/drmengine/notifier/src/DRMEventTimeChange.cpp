/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include    "DRMEventTimeChange.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================



// -----------------------------------------------------------------------------
// CDRMEventTimeChange::NewLC
// two phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMEventTimeChange* CDRMEventTimeChange::NewLC()
    {
    CDRMEventTimeChange* self = new (ELeave) CDRMEventTimeChange();
    CleanupStack::PushL( self );
    return self; 
    };

// -----------------------------------------------------------------------------
// CDRMEventTimeChange::NewL
// two phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMEventTimeChange* CDRMEventTimeChange::NewL()
    {
    CDRMEventTimeChange* self = NewLC();
    CleanupStack::Pop();
    return self;
    };


// -----------------------------------------------------------------------------
// CDRMEventTimeChange::CDRMEventTimeChange
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDRMEventTimeChange::CDRMEventTimeChange() : 
    MDRMEvent( KEventTimeChange ), 
    iOldTime( Time::NullTTime() ),
    iNewTime( Time::NullTTime() ),
    iOldTimeZone( 0 ),
    iNewTimeZone( 0 ),
    iOldSecurityLevel( DRMClock::KInsecure ),
    iNewSecurityLevel( DRMClock::KInsecure )
    {       
    };

// Destructor
EXPORT_C CDRMEventTimeChange::~CDRMEventTimeChange()
    {
    };


// -----------------------------------------------------------------------------
// CDRMEventTimeChange::SetOldTime
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMEventTimeChange::SetOldTime( const TTime& aTime )
    {
    iOldTime = aTime;
    };


// -----------------------------------------------------------------------------
// CDRMEventTimeChange::GetOldTime
// -----------------------------------------------------------------------------
//
EXPORT_C const TTime& CDRMEventTimeChange::GetOldTime() const
    {
    return iOldTime;
    };


// -----------------------------------------------------------------------------
// CDRMEventTimeChange::SetNewTime
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMEventTimeChange::SetNewTime( const TTime& aTime )
    {
    iNewTime = aTime;
    };


// -----------------------------------------------------------------------------
// CDRMEventTimeChange::GetNewTime
// -----------------------------------------------------------------------------
//
EXPORT_C const TTime& CDRMEventTimeChange::GetNewTime() const
    {
    return iNewTime;
    };

// -----------------------------------------------------------------------------
// CDRMEventTimeChange::SetOldTimeZone
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMEventTimeChange::SetOldTimeZone( const TInt aTimeZone )
    {
    iOldTimeZone = aTimeZone;
    };


// -----------------------------------------------------------------------------
// CDRMEventTimeChange::GetOldTimeZone
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMEventTimeChange::GetOldTimeZone() const
    {
    return iOldTimeZone;
    };


// -----------------------------------------------------------------------------
// CDRMEventTimeChange::SetNewTimeZone
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMEventTimeChange::SetNewTimeZone( const TInt aTimeZone )
    {
    iNewTimeZone = aTimeZone;
    };


// -----------------------------------------------------------------------------
// CDRMEventTimeChange::GetNewTimeZone
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMEventTimeChange::GetNewTimeZone() const
    {
    return iNewTimeZone;
    };

// -----------------------------------------------------------------------------
// CDRMEventTimeChange::SetOldSecurityLevel
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMEventTimeChange::SetOldSecurityLevel( 
    const DRMClock::ESecurityLevel aSecLevel )
    {
    iOldSecurityLevel = aSecLevel;
    };


// -----------------------------------------------------------------------------
// CDRMEventTimeChange::GetOldSecurityLevel
// -----------------------------------------------------------------------------
//
EXPORT_C DRMClock::ESecurityLevel CDRMEventTimeChange::GetOldSecurityLevel() const
    {
    return iOldSecurityLevel;
    };


// -----------------------------------------------------------------------------
// CDRMEventTimeChange::SetNewSecurityLevel
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMEventTimeChange::SetNewSecurityLevel( 
    const DRMClock::ESecurityLevel aSecLevel )
    {
    iNewSecurityLevel = aSecLevel;
    };


// -----------------------------------------------------------------------------
// CDRMEventTimeChange::GetNewSecurityLevel
// -----------------------------------------------------------------------------
//
EXPORT_C DRMClock::ESecurityLevel CDRMEventTimeChange::GetNewSecurityLevel() const
    {
    return iNewSecurityLevel;
    };
   
// -----------------------------------------------------------------------------
// CDRMEventTimeChange::ExternalizeL
// Writes the data of the object into the given stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMEventTimeChange::ExternalizeL(
    RWriteStream& aOutput)
    {
    TInt8 securityLevel = 0;

    MDRMEvent::ExternalizeL(aOutput);

    // output the old time
    WriteInt64L( iOldTime.Int64(), aOutput );
    
    // output the new time
    WriteInt64L( iNewTime.Int64(), aOutput );
    
    // output the old timezone 
    aOutput.WriteInt32L( iOldTimeZone );
    
    // output the new timezone
    aOutput.WriteInt32L( iNewTimeZone );
    
    // output the old security level
    securityLevel = iOldSecurityLevel;
    aOutput.WriteInt8L( securityLevel );    
            
    // output the new security level
    securityLevel = iNewSecurityLevel;
    aOutput.WriteInt8L( securityLevel ); 
    
    };


// -----------------------------------------------------------------------------
// CDRMEventTimeChange::Internalize
// Reads the data of the object from the given stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMEventTimeChange::InternalizeL(
    RReadStream& aInput) 
    {
    TInt8 securityLevel = 0;
    TInt64 timeData = 0;

    MDRMEvent::InternalizeL(aInput);

    // input the old time
    ReadInt64L( timeData, aInput );
    iOldTime = timeData;
    
    // input the new time
    ReadInt64L( timeData, aInput );    
    iNewTime = timeData;
    
    // input the old time zone
    iOldTimeZone = aInput.ReadInt32L();
    
    // input the new time zone
    iNewTimeZone = aInput.ReadInt32L();
    
    // input the old security level
    securityLevel = aInput.ReadInt8L();
    iOldSecurityLevel = static_cast<DRMClock::ESecurityLevel>(securityLevel);
    
    // input the new security level
    securityLevel = aInput.ReadInt8L();
    iNewSecurityLevel = static_cast<DRMClock::ESecurityLevel>(securityLevel);    
    
    };

// -----------------------------------------------------------------------------
// CDRMEventTimeChange::WriteInt64L
// -----------------------------------------------------------------------------
//
void CDRMEventTimeChange::WriteInt64L( const TInt64& aWrite, RWriteStream& aStream ) const
    {
    TPtr8 output(NULL,0,0);
    
    output.Set( reinterpret_cast<TUint8*>(const_cast<TInt64*>(&aWrite)), 
                sizeof(TInt64), sizeof(TInt64) );
    
    aStream.WriteL( output, sizeof(TInt64) );
    };

// -----------------------------------------------------------------------------
// CDRMEventTimeChange::ReadInt64L
// -----------------------------------------------------------------------------
//
void CDRMEventTimeChange::ReadInt64L( TInt64& aRead, RReadStream& aStream )
    {
    TPtr8 input(NULL,0,0);
    
    input.Set( reinterpret_cast<TUint8*>(&aRead), 0, sizeof(TInt64) );
    
    aStream.ReadL( input, sizeof(TInt64) );    
    };    

// End of File
