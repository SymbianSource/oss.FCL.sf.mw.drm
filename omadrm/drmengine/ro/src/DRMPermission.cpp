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
* Description:  Part of the Implementation of the DRM Rights Database
*
*/



// INCLUDE FILES
#include <s32strm.h>
#include <s32mem.h>
#include <caf/caf.h>
#include "DrmPermission.h"
#include "DrmConstraint.h"
#include "drmlog.h"
#include "Oma2Agent.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// Synchronizing marker in the beginning of the stream in order to synchronize
// to externalized Permission object having the new structure.
const TInt32 KSyncMark = 0xAFCE;

// Old and new version number of the Permission object
const TInt8 KVersion3_2_0 = 0;
const TInt8 KVersion3_2_1 = 1;


const TInt KSanityDataLengthLow = 0;
const TInt KSanityDataLengthHigh = 32768;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

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

// ============================ MEMBER FUNCTIONS ===============================




// -----------------------------------------------------------------------------
// CDRMPermission::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMPermission* CDRMPermission::NewLC()
    {
    CDRMPermission* self = new( ELeave ) CDRMPermission();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    };


// -----------------------------------------------------------------------------
// CDRMPermission::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMPermission* CDRMPermission::NewL()
    {
    CDRMPermission* self = NewLC();
    CleanupStack::Pop();

    return self;
    };

// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------
//
CDRMPermission::CDRMPermission() :
    iSyncMark( KSyncMark ),
    iVersion( KVersion3_2_1 ), // Version number for differentiation in
                               // InternalizeL.
    iUniqueID( 0 ),
    iOriginalInsertTime( Time::NullTTime() ),
    iTopLevel( NULL ),
    iPlay( NULL ),
    iDisplay( NULL ),
    iExecute(NULL ),
    iPrint( NULL ),
    iExportMode( ECopy ),
    iParentUID( NULL ),
    iRoID( NULL ),
    iDomainID( NULL ),
    iAvailableRights( 0 ),
    iInfoBits( 0 ),
    iOnExpiredUrl( NULL )
    {
    iRiId.SetLength( KRiIdSize );
    iRiId.Fill( 0 );
    };



// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMPermission::~CDRMPermission()
    {
    if( iTopLevel )
        {
        delete iTopLevel;
        iTopLevel = NULL;
        }

    if( iPlay )
        {
        delete iPlay;
        iPlay = NULL;
        }

    if( iDisplay )
        {
        delete iDisplay;
        iDisplay = NULL;
        }

    if( iExecute )
        {
        delete iExecute;
        iExecute = NULL;
        }

    if( iExport )
        {
        delete iExport;
        iExport = NULL;
        }

    if( iPrint )
        {
        delete iPrint;
        iPrint = NULL;
        }

    if( iParentUID )
        {
        delete iParentUID;
        iParentUID = NULL;
        }

    if( iRoID )
        {
        delete iRoID;
        iRoID = NULL;
        }

    if( iDomainID )
        {
        delete iDomainID;
        iDomainID = NULL;
        }

    if( iOnExpiredUrl )
        {
        delete iOnExpiredUrl;
        iOnExpiredUrl = NULL;
        }
    };

// -----------------------------------------------------------------------------
// CDRMPermission::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMPermission::ExternalizeL( RWriteStream& aStream ) const
    {

    // used for the buffers
    TInt dataLength = 0;

    // Used for mode
    TUint8 exportMode = 0;

    // Write the synchronizing marker
    aStream.WriteInt32L( iSyncMark );

    // Write the version number
    aStream.WriteInt32L( iVersion );

    // Unique ID of the permission
    aStream.WriteUint32L( iUniqueID );

    // The original insertion time
    WriteInt64L( iOriginalInsertTime.Int64(), aStream );

    // Top level constraint
    if( !iTopLevel )
        {
        User::Leave( KErrNotReady );
        }
    iTopLevel->ExternalizeL( aStream );

    // Play constraint
    if( !iPlay )
        {
        User::Leave( KErrNotReady );
        }
    iPlay->ExternalizeL( aStream );

    // Display constraint
    if( !iDisplay )
        {
        User::Leave( KErrNotReady );
        }
    iDisplay->ExternalizeL( aStream );

    // Execute constraint
    if( !iExecute )
        {
        User::Leave( KErrNotReady );
        }
    iExecute->ExternalizeL( aStream );

    // Print constraint
    if( !iPrint )
        {
        User::Leave( KErrNotReady );
        }
    iPrint->ExternalizeL( aStream );

    // Export constraint
    if( !iExport )
        {
        User::Leave( KErrNotReady );
        }
    iExport->ExternalizeL( aStream );

    // Export mode
    exportMode = iExportMode;
    aStream.WriteUint8L( exportMode );

    // Content id of the parent rights object
    dataLength = 0;
    if( iParentUID )
        {
        dataLength = iParentUID->Length();
        }
    aStream.WriteInt32L( dataLength );

    if( dataLength )
        {
        aStream.WriteL( iParentUID->Des() );
        }

    // Rights Object if of the rights delivery container
    dataLength = 0;
    if( iRoID )
        {
        dataLength = iRoID->Length();
        }
    aStream.WriteInt32L( dataLength );

    if( dataLength )
        {
        aStream.WriteL( iRoID->Des() );
        }

    // Domain identifier
    dataLength = 0;
    if( iDomainID )
        {
        dataLength = iDomainID->Length();
        }
    aStream.WriteInt32L( dataLength );

    if( dataLength )
        {
        aStream.WriteL( iDomainID->Des() );
        }

    // Available rights
    aStream.WriteUint16L( iAvailableRights );

    // Version number of the rights object
    aStream.WriteUint16L( iRightsObjectVersion.iVersionMain );
    aStream.WriteUint16L( iRightsObjectVersion.iVersionSub );

    // Info bits
    aStream.WriteInt32L( iInfoBits );

    aStream.WriteL( iRiId );

    // OnExpiredUrl
    dataLength = 0;
    if ( iOnExpiredUrl )
        {
        dataLength = iOnExpiredUrl->Length();
        }
    aStream.WriteInt32L( dataLength );

    if ( dataLength )
        {
        aStream.WriteL( iOnExpiredUrl->Des() );
        }

    // For future use
    aStream.WriteInt32L( 0 );

    };

// -----------------------------------------------------------------------------
// CDRMPermission::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMPermission::InternalizeL( RReadStream& aStream )
    {

    TInt64 timeData = 0;

    // used for the buffers
    TInt dataLength = 0;
    TPtr8 dataBuffer(NULL,0,0);
    HBufC8* dataPart = NULL;

    // (Possible) synchronizing marker
    iSyncMark = aStream.ReadInt32L();

    if ( iSyncMark == KSyncMark )
        {

        // The internalized Permission object has the new structure if the
        // synchronizing marker is found in the beginning of the stream. In that
        // case read version number and Unique ID.
        iVersion = aStream.ReadInt32L();

        // Unique ID of the permission
        iUniqueID = aStream.ReadUint32L();

        }
    else
        {

        // The internalized Permission object has the old structure. The
        // stream information that was read to iSyncMark is actually the
        // Unique ID.
        iUniqueID = (TDRMUniqueID)iSyncMark;
        iVersion = KVersion3_2_0; // Old version, needed for differentation

        }

    // The original insertion time
    ReadInt64L( timeData, aStream );
    iOriginalInsertTime = timeData;

    // Top level constraint
    if( !iTopLevel )
        {
        iTopLevel = CDRMConstraint::NewL();
        }

    iTopLevel->InternalizeL( aStream );

    // Play constraint
    if( !iPlay )
        {
        iPlay = CDRMConstraint::NewL();
        }

    iPlay->InternalizeL( aStream );

    // Display constraint
    if( !iDisplay )
        {
        iDisplay = CDRMConstraint::NewL();
        }

    iDisplay->InternalizeL( aStream );

    // Execute constraint
    if( !iExecute )
        {
        iExecute = CDRMConstraint::NewL();
        }

    iExecute->InternalizeL( aStream );

    // Print constraint
    if( !iPrint)
        {
        iPrint = CDRMConstraint::NewL();
        }

    iPrint->InternalizeL( aStream );

    // Export constraint

    iExport->InternalizeL( aStream );

    // Export mode
    iExportMode = static_cast<TExportMode>(aStream.ReadUint8L());

    // Content id of the parent rights object
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

        // Pop the buffer
        CleanupStack::Pop(); // dataPart

        // If an old content identifier exists delete it
        if( iParentUID )
            {
            delete iParentUID;
            iParentUID = NULL;
            }

        // assign the new content id
        iParentUID = dataPart;
        }
    else
        {
        // If an old individual exists delete it
        if( iParentUID )
            {
            delete iParentUID;
            iParentUID = NULL;
            }
        }


    // Rights Object if of the rights delivery container
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

        // Pop the buffer
        CleanupStack::Pop(); // dataPart

        // If an old content identifier exists delete it
        if( iRoID )
            {
            delete iRoID;
            iRoID = NULL;
            }

        // assign the new content id
        iRoID = dataPart;
        }
    else
        {
        // If an old individual exists delete it
        if( iRoID )
            {
            delete iRoID;
            iRoID = NULL;
            }
        }

    // Domain identifier
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

        // Pop the buffer
        CleanupStack::Pop(); // dataPart

        // If an old content identifier exists delete it
        if( iDomainID )
            {
            delete iDomainID;
            iDomainID = NULL;
            }

        // assign the new content id
        iDomainID = dataPart;
        }
    else
        {
        // If an old individual exists delete it
        if( iDomainID )
            {
            delete iDomainID;
            iDomainID = NULL;
            }
        }


    // Available rights
    iAvailableRights = aStream.ReadUint16L();

    // Version number of the rights object
    iRightsObjectVersion.iVersionMain = aStream.ReadUint16L();
    iRightsObjectVersion.iVersionSub = aStream.ReadUint16L();

    // Information bits
    iInfoBits = aStream.ReadInt32L();

    aStream.ReadL( iRiId );

    // New structure of Permission object
    if ( iVersion == KVersion3_2_1 )
        {

        // OnExpiredUrl
        dataLength = aStream.ReadInt32L();

        SanitizeL( dataLength );

        if( dataLength > 0 )
            {

            // Reserve a new buffer:
            dataPart = HBufC8::NewMaxLC( dataLength );

            // Set the read buffer:
            dataBuffer.Set(const_cast<TUint8*>(dataPart->Ptr()), 0,
                           dataLength);

            // Read the data:
            aStream.ReadL( dataBuffer );

            // Pop the buffer
            CleanupStack::Pop(); // dataPart

            // If an old OnExpiredUrl exists delete it
            if( iOnExpiredUrl )
                {
                delete iOnExpiredUrl;
                iOnExpiredUrl = NULL;
                }

            // assign the new OnExpiredUrl
            iOnExpiredUrl = dataPart;
            }
            else
            {
            // If an old OnExpiredUrl exists delete it
            if( iOnExpiredUrl )
                {
                delete iOnExpiredUrl;
                iOnExpiredUrl = NULL;
                }
            }

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

            // Delete the buffer
            CleanupStack::PopAndDestroy( dataPart );
            }
        }

    // Permission can be considered to have the new structure after
    // internalization.
    if ( iVersion == KVersion3_2_0 )
        {
        iSyncMark = KSyncMark;
        iVersion = KVersion3_2_1;
        }

    };

// -----------------------------------------------------------------------------
// CDRMPermission::Stateful
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMPermission::Stateful() const
    {
    if( ( (iAvailableRights & ERightsTopLevel) &&  iTopLevel->Stateful() ) )
        {
        return ETrue;
        }
    else if( (iAvailableRights & ERightsPlay) &&  iPlay->Stateful() )
        {
        return ETrue;
        }
    else if( (iAvailableRights & ERightsDisplay) &&  iDisplay->Stateful() )
        {
        return ETrue;
        }
    else if( (iAvailableRights & ERightsExecute) &&  iExecute->Stateful() )
        {
        return ETrue;
        }
    else if( (iAvailableRights & ERightsPrint) &&  iPrint->Stateful() )
        {
        return ETrue;
        }
    return EFalse;
    };

// -----------------------------------------------------------------------------
// CDRMPermission::SoftwareConstrained
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMPermission::SoftwareConstrained() const
    {
    if ( (iAvailableRights & ERightsTopLevel) &&
        iTopLevel->iActiveConstraints & ( EConstraintVendor | EConstraintSoftware ) )
        {
        return ETrue;
        }
    else if ( (iAvailableRights & ERightsPlay) &&
        iPlay->iActiveConstraints & ( EConstraintVendor | EConstraintSoftware ) )
        {
        return ETrue;
        }
    else if ( (iAvailableRights & ERightsDisplay) &&
        iDisplay->iActiveConstraints & ( EConstraintVendor | EConstraintSoftware ) )
        {
        return ETrue;
        }
    else if ( (iAvailableRights & ERightsExecute) &&
        iExecute->iActiveConstraints & ( EConstraintVendor | EConstraintSoftware ) )
        {
        return ETrue;
        }
    else if ( (iAvailableRights & ERightsPrint) &&
        iPrint->iActiveConstraints & ( EConstraintVendor | EConstraintSoftware ) )
        {
        return ETrue;
        }
    return EFalse;
    };

// -----------------------------------------------------------------------------
// CDRMPermission::Child
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMPermission::Child() const
    {
    return iParentUID ? ETrue : EFalse;
    };

// -----------------------------------------------------------------------------
// CDRMPermission::Size
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMPermission::Size() const
    {
    TInt size = 0;

    // synchronizing marker
    size += sizeof(TInt32);

    // Version number
    size += sizeof(TInt32);

    // Unique ID of the permission
    size += sizeof(TDRMUniqueID);

    // The original insertion time
    size += sizeof(TTime);

    // Top level constraint
    size += iTopLevel->Size();

    // Play constraint
    size += iPlay->Size();

    // Display constraint
    size += iDisplay->Size();

    // Execute constraint
    size += iExecute->Size();

    // Print constraint
    size += iPrint->Size();

    // Export constraint
    size += iExport->Size();

    // Export mode
    size += sizeof(TUint8);

    // Content id of the parent rights object
    size += sizeof(TInt32);

    if( iParentUID )
        {
        size += iParentUID->Size();
        }

    // Rights Object if of the rights delivery container
    size += sizeof(TInt32);

    if( iRoID )
        {
        size += iRoID->Size();
        }

    // Domain identifier
    size += sizeof(TInt32);

    if( iDomainID )
        {
        size += iDomainID->Size();
        }

    // Available rights
    size += sizeof(TUint16);

    // Version number of the rights object
    size += sizeof(TUint16);
    size += sizeof(TUint16);

    // infobits
    size += sizeof(TInt32);

    // RI ID
    size += KRiIdSize;

    // OnExpiredUrl
    size += sizeof(TInt32);

    if ( iOnExpiredUrl )
        {
        size += iOnExpiredUrl->Size();
        }

    // For future use
    size += sizeof(TInt32);

    return size;

    };

// -----------------------------------------------------------------------------
// CDRMPermission::ConstraintForIntent
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMConstraint* CDRMPermission::ConstraintForIntent(
    const ContentAccess::TIntent aIntent )
    {
    CDRMConstraint* r = NULL;

    switch ( aIntent )
        {
        case ContentAccess::EPlay:
            if ( iAvailableRights & ERightsPlay )
                {
                r = iPlay;
                }
            break;
        case ContentAccess::EView:
            if ( iAvailableRights & ERightsDisplay )
                {
                r = iDisplay;
                }
            break;
        case ContentAccess::EExecute:
            if ( iAvailableRights & ERightsExecute )
                {
                r = iExecute;
                }
            break;
        case ContentAccess::EPrint:
            if ( iAvailableRights & ERightsPrint )
                {
                r = iPrint;
                }
            break;
        }

    return r;
    }

// -----------------------------------------------------------------------------
// CDRMPermission::ConstraintForIntent
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMConstraint* CDRMPermission::TopLevelConstraint()
    {
    if ( iAvailableRights & ERightsTopLevel )
        {
        return iTopLevel;
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CDRMPermission::ConsumeRights
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMPermission::ConsumeRights( const TIntent aIntent, const TTime& aCurrentTime )
    {
    //__ASSERT_DEBUG( !( aIntent == ERightsTopLevel ), User::Leave( KErrArgument ) );

    CDRMConstraint* constraint( TopLevelConstraint() );

    if ( constraint )
        {
        constraint->Consume( aCurrentTime );
        }

    constraint = ConstraintForIntent( aIntent );
    if ( constraint )
        {
        constraint->Consume( aCurrentTime );
        }
    }

// -----------------------------------------------------------------------------
// CDRMPermission::ImportL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMPermission::ImportL( const TDesC8& aBuffer )
    {
    TInt size = aBuffer.Size();
    RMemReadStream stream( (TAny*)( aBuffer.Ptr() ), size );
    CleanupClosePushL( stream );

    InternalizeL( stream );

    CleanupStack::PopAndDestroy(); // Stream
    };


// -----------------------------------------------------------------------------
// CDRMPermission::ExportL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CDRMPermission::ExportL() const
    {
    TInt size = Size();
    HBufC8* data = HBufC8::NewMaxLC( size );

    RMemWriteStream stream( (TAny*)( data->Ptr() ), size );
    CleanupClosePushL( stream );

    ExternalizeL( stream );

    CleanupStack::PopAndDestroy(); // Stream
    CleanupStack::Pop();
    return data;
    };

// -----------------------------------------------------------------------------
// CDRMPermission::DuplicateL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMPermission::DuplicateL( const CDRMPermission& aPermission )
    {

    // Synchronizing marker
    iSyncMark = aPermission.iSyncMark;

    // Version number
    iVersion = aPermission.iVersion;

    // Unique ID of the permission
    iUniqueID = aPermission.iUniqueID;

    // The original insertion time
    iOriginalInsertTime = aPermission.iOriginalInsertTime;

    // Top level constraint
    if( !iTopLevel )
        {
        iTopLevel = CDRMConstraint::NewL();
        }
    iTopLevel->DuplicateL( *(aPermission.iTopLevel ) );

    // Play constraint
    if( !iPlay )
        {
        iPlay = CDRMConstraint::NewL();
        }
    iPlay->DuplicateL( *(aPermission.iPlay ) );

    // Display constraint
    if( !iDisplay )
        {
        iDisplay = CDRMConstraint::NewL();
        }
    iDisplay->DuplicateL( *(aPermission.iDisplay ) );

    // Execute constraint
    if( !iExecute )
        {
        iExecute = CDRMConstraint::NewL();
        }
    iExecute->DuplicateL( *(aPermission.iExecute ) );

    // Print constraint
    if( !iPrint )
        {
        iPrint = CDRMConstraint::NewL();
        }
    iPrint->DuplicateL( *(aPermission.iPrint ) );

    // Export constraint
    if( !iExport )
        {
        iExport = CDRMConstraint::NewL();
        }
    iExport->DuplicateL( *(aPermission.iExport ) );

    // Export mode
    iExportMode = aPermission.iExportMode;

    // Content id of the parent rights object
    if( iParentUID )
        {
        delete iParentUID;
        iParentUID = NULL;
        }

    if( aPermission.iParentUID )
        {
        iParentUID = aPermission.iParentUID->AllocL();
        }

    // Rights Object if of the rights delivery container
    if( iRoID )
        {
        delete iRoID;
        iRoID = NULL;
        }

    if( aPermission.iRoID )
        {
        iRoID = aPermission.iRoID->AllocL();
        }

    // Domain identifier
    if( iDomainID )
        {
        delete iDomainID;
        iDomainID = NULL;
        }

    if( aPermission.iDomainID )
        {
        iDomainID = aPermission.iDomainID->AllocL();
        }
    // Available rights
    iAvailableRights  = aPermission.iAvailableRights;   // Bitmask

    // Version number of the rights object
    iRightsObjectVersion.iVersionMain = aPermission.iRightsObjectVersion.iVersionMain;
    iRightsObjectVersion.iVersionSub = aPermission.iRightsObjectVersion.iVersionSub;

    // info bits
    iInfoBits = aPermission.iInfoBits;

    // OnExpiredUrl
    if ( iOnExpiredUrl )
        {
        delete iOnExpiredUrl;
        iOnExpiredUrl = NULL;
        }

    if ( aPermission.iOnExpiredUrl )
        {
        iOnExpiredUrl = aPermission.iOnExpiredUrl->AllocL();
        }

    };


// -----------------------------------------------------------------------------
// CDRMPermission::Expired
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMPermission::Expired( const TTime& aTime )
    {
    if( iTopLevel && iAvailableRights & ERightsTopLevel )
        {
        if( iTopLevel->Expired( aTime ) )
            {
            return ETrue;
            }
        }

    if( iPlay && iAvailableRights & ERightsPlay)
        {
        if( !iPlay->Expired( aTime ) )
            {
            return EFalse;
            }
        }

    if( iDisplay && iAvailableRights & ERightsDisplay )
        {
        if( !iDisplay->Expired( aTime ) )
            {
            return EFalse;
            }
        }

    if( iExecute && iAvailableRights & ERightsExecute )
        {
        if( !iExecute->Expired( aTime ) )
            {
            return EFalse;
            }
        }

    if( iPrint && iAvailableRights & ERightsPrint )
        {
        if( !iPrint->Expired( aTime ) )
            {
            return EFalse;
            }
        }

    if( iExport && iAvailableRights & ERightsExport )
        {
        if( !iExport->Expired( aTime ) )
            {
            return EFalse;
            }
        }

    return ETrue;
    };


// -----------------------------------------------------------------------------
// CDRMPermission::Expired
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMPermission::Valid( const TTime& aTime,
                                      const RPointerArray<HBufC8>& aIndividual,
                                      TUint32& aRejection,
                                      const TRightsType aType ) const
    {
    // Always check that if the top level rights are present
    // that they are valid
    if( iTopLevel && !iTopLevel->Valid( aTime, aIndividual, aRejection ) )
        {
        aRejection |= aRejection;
        return EFalse;
        }

    switch( aType )
        {
        case ERightsTopLevel:
            if( !iTopLevel )
                {
                return EFalse;
                }
            break;
        case ERightsPlay:
            if( !iPlay )
                {
                return EFalse;
                }
            else if( iPlay && !iPlay->Valid( aTime, aIndividual, aRejection ) )
                {
                return EFalse;
                }
            break;
        case ERightsDisplay:
            if( !iDisplay )
                {
                return EFalse;
                }
            else if( iDisplay && !iDisplay->Valid( aTime, aIndividual, aRejection ) )
                {
                return EFalse;
                }
            break;
        case ERightsExecute:
            if( !iExecute )
                {
                return EFalse;
                }
            else if( iExecute && !iExecute->Valid( aTime, aIndividual, aRejection ) )
                {
                return EFalse;
                }
            break;
        case ERightsPrint:
            if( !iPrint )
                {
                return EFalse;
                }
            else if( iPrint && !iPrint->Valid( aTime, aIndividual, aRejection ) )
                {
                return EFalse;
                }
            break;
        case ERightsExport:
            if( !iExport )
                {
                return EFalse;
                }
            else if( iExport && !iExport->Valid( aTime, aIndividual, aRejection ) )
                {
                return EFalse;
                }
            break;
        default:
            // No constraints are defined, top level doesn't work
            // on it's own, the permission is invalid
            if( !iPrint && !iDisplay && !iExecute && !iPrint )
                {
                return EFalse;
                }
            else if( iPlay && !iPlay->Valid( aTime, aIndividual, aRejection ) )
                {
                return EFalse;
                }
            else if( iDisplay && !iDisplay->Valid( aTime, aIndividual, aRejection ) )
                {
                return EFalse;
                }
            else if( iPrint && !iPrint->Valid( aTime, aIndividual, aRejection ) )
                {
                return EFalse;
                }
            else if( iExecute && !iExecute->Valid( aTime, aIndividual, aRejection ) )
                {
                return EFalse;
                }
            else if( iExport && !iExport->Valid( aTime, aIndividual, aRejection ) )
                {
                return EFalse;
                }
            break;
        }
    return ETrue;
    };



// -----------------------------------------------------------------------------
// CDRMPermission::ConstructL
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CDRMPermission::ConstructL()
    {
    iTopLevel = CDRMConstraint::NewL();
    iPlay = CDRMConstraint::NewL();
    iDisplay = CDRMConstraint::NewL();
    iExecute = CDRMConstraint::NewL();
    iPrint = CDRMConstraint::NewL();
    iExport = CDRMConstraint::NewL();

    iRightsObjectVersion.iVersionMain = EOma1Rights;
    iRightsObjectVersion.iVersionSub = 0;
    };


// -----------------------------------------------------------------------------
// CDRMPermission::WriteInt64L
// -----------------------------------------------------------------------------
//
void CDRMPermission::WriteInt64L( const TInt64& aWrite, RWriteStream& aStream ) const
    {
    TPtr8 output( reinterpret_cast<TUint8*>(const_cast<TInt64*>(&aWrite)),
                  sizeof(TInt64), sizeof(TInt64) );

    aStream.WriteL( output, sizeof(TInt64) );
    }

// -----------------------------------------------------------------------------
// CDRMPermission::ReadInt64L
// -----------------------------------------------------------------------------
//
void CDRMPermission::ReadInt64L( TInt64& aRead, RReadStream& aStream )
    {
    TPtr8 input( reinterpret_cast<TUint8*>(&aRead), 0, sizeof(TInt64) );

    aStream.ReadL( input, sizeof(TInt64) );
    };


// -----------------------------------------------------------------------------
// CDRMPermission::Merge
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMPermission::Merge( const CDRMPermission& aPermission )
    {
    iTopLevel->Merge( *( aPermission.iTopLevel ) );
    iPlay->Merge( *( aPermission.iPlay ) );
    iDisplay->Merge( *( aPermission.iDisplay ) );
    iExecute->Merge( *( aPermission.iExecute ) );
    iPrint->Merge( *( aPermission.iPrint ) );
    iExport->Merge( *( aPermission.iExport ) );
    iAvailableRights |= aPermission.iAvailableRights;
    iInfoBits |= aPermission.iInfoBits;
    }

// End of File
