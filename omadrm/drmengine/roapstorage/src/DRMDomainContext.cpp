/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Datatype for the Common Rights Database Data
*
*/



// INCLUDE FILES
#include <s32strm.h>
#include <s32mem.h>
#include <caf/caf.h>
#include <asymmetric.h>

#include "DRMDomainContext.h"
#include "OmaCrypto.h"
#include "drmlog.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
const TInt KMaxUrlLength = 32768;
const TUint8 KMinDomainIdSize = 4;
const TUint8 KMaxDomainIdSize = 20;

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMDomainContext::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMDomainContext* CDRMDomainContext::NewLC( const TDesC8& aDomainID,
                                             const TTime& aExpiryTime,
                                             const TBool aHashChained,
                                             const RPointerArray< HBufC8 >& aDomainKeys,
                                             const TDesC8& aRiId,
                                             const TDesC8& aRightsIssuerURL )
    {
    CDRMDomainContext* self = new( ELeave ) CDRMDomainContext( aExpiryTime,
                                                               aHashChained );
    CleanupStack::PushL( self );
    self->ConstructL( aDomainID, aDomainKeys,
                      aRiId, aRightsIssuerURL );

    return self;
    };


// -----------------------------------------------------------------------------
// CDRMDomainContext::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMDomainContext* CDRMDomainContext::NewL(const TDesC8& aDomainID,
                                           const TTime& aExpiryTime,
                                           const TBool aHashChained,
                                           const RPointerArray< HBufC8 >& aDomainKeys,
                                           const TDesC8& aRiId,
                                           const TDesC8& aRightsIssuerURL  )
    {
    CDRMDomainContext* self = NewLC( aDomainID, aExpiryTime,
                                     aHashChained, aDomainKeys,
                                     aRiId, aRightsIssuerURL );
    CleanupStack::Pop();

    return self;
    };

// -----------------------------------------------------------------------------
// CDRMDomainContext::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMDomainContext* CDRMDomainContext::NewLC()
    {
    CDRMDomainContext* self = new( ELeave ) CDRMDomainContext();
    CleanupStack::PushL( self );

    return self;
    };


// -----------------------------------------------------------------------------
// CDRMDomainContext::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMDomainContext* CDRMDomainContext::NewL()
    {
    CDRMDomainContext* self = NewLC();
    CleanupStack::Pop();

    return self;
    };


// -----------------------------------------------------------------------------
// Default Constructor - First phase.
// Can be used by itself to generate an empty object
// -----------------------------------------------------------------------------
//
CDRMDomainContext::CDRMDomainContext():
    iHashChained( EFalse ),
    iRightsIssuerURL( NULL )
    {
    iDomainID.SetLength( 0 );
    iRightsIssuerID.SetLength( 0 );
    iDomainKeys.Reset();
    };


// -----------------------------------------------------------------------------
// Default Constructor - First phase.
// Can be used by itself to generate an empty object
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMDomainContext::CDRMDomainContext( const TTime& aExpiryTime,
                                      const TBool aHashChained ) :
    iExpiryTime( aExpiryTime ),
    iHashChained( aHashChained )
    {
    iDomainID.SetLength( 0 );
    iRightsIssuerID.SetLength( 0 );
    iDomainKeys.Reset();
    };

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMDomainContext::~CDRMDomainContext()
    {
    // Empty and destroy the pointer array
    iDomainKeys.ResetAndDestroy();

    if( iRightsIssuerURL )
        {
        delete iRightsIssuerURL;
        iRightsIssuerURL = NULL;
        }
    };


// -----------------------------------------------------------------------------
// CDRMDomainContext::DomainID
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CDRMDomainContext::DomainID() const
    {
    // last three digits presents the domain generation
    if ( iDomainID.Length() >= KMinDomainIdSize &&
        iDomainID.Length() <= KMaxDomainIdSize  )
        {
        return iDomainID.Left( iDomainID.Length() - 3 );
        }
    else
        {
        return KNullDesC8();
        }
    };

// -----------------------------------------------------------------------------
// CDRMDomainContext::ExpiryTime
// -----------------------------------------------------------------------------
//
EXPORT_C const TTime CDRMDomainContext::ExpiryTime() const
    {
    return iExpiryTime;
    }

// -----------------------------------------------------------------------------
// CDRMDomainContext::HashChained
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMDomainContext::HashChained() const
    {
    return iHashChained;
    }

// -----------------------------------------------------------------------------
// CDRMDomainContext::DomainKeys
// -----------------------------------------------------------------------------
//
EXPORT_C const RPointerArray< HBufC8 >& CDRMDomainContext::DomainKeys() const
    {
    return iDomainKeys;
    }

// -----------------------------------------------------------------------------
// CDRMDomainContext::DomainKey
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CDRMDomainContext::DomainKeyL( const TInt aGeneration ) const
    {
    TInt newestGeneration( DomainGeneration() );
    TInt loops( newestGeneration - aGeneration );
    HBufC8* domainKey( NULL );
    HBufC8* temp( NULL );

    if ( loops < 0 || aGeneration > newestGeneration )
        {
        User::Leave( KErrArgument );
        }

    if ( iHashChained )
        {
        domainKey = iDomainKeys[0]->AllocL();
        for ( TInt i = 0; i < loops; i++ )
            {
            CleanupStack::PushL( domainKey );
            temp = OmaCrypto::KdfL( *domainKey, KNullDesC8, OmaCrypto::KKeySize );
            CleanupStack::PopAndDestroy( domainKey );
            domainKey = temp;
            }
        }
    else
        {
        domainKey = iDomainKeys[aGeneration]->AllocL();
        }

    return domainKey;
    }

/// -----------------------------------------------------------------------------
// CDRMDomainContext::DomainGeneration
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMDomainContext::DomainGeneration() const
    {
    // last three digits presents the domain generation
    TInt generation = 0;
    TLex8 lex( iDomainID.Right( 3 ) );
    lex.Val( generation );
    return generation;
    };


// -----------------------------------------------------------------------------
// CDRMDomainContext::PublicKey
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CDRMDomainContext::RightsIssuerID() const
    {
    return iRightsIssuerID;
    };

// -----------------------------------------------------------------------------
// CDRMDomainContext::RightsIssuerURL
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CDRMDomainContext::RightsIssuerURL() const
    {
    return *iRightsIssuerURL;
    };



// -----------------------------------------------------------------------------
// CDRMDomainContext::SetDomainIDL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMDomainContext::SetDomainIDL( const TDesC8& aDomainID )
    {
    if ( aDomainID.Length() >= KMinDomainIdSize &&
        aDomainID.Length() <= KMaxDomainIdSize  )
        {
    iDomainID.Copy(aDomainID);
        }
    else
        {
        User::Leave( KErrArgument );
        }
    };

// -----------------------------------------------------------------------------
// CDRMDomainContext::SetExpiryTimeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMDomainContext::SetExpiryTimeL( const TTime& aExpiryTime )
    {
    iExpiryTime = aExpiryTime;
    };

// -----------------------------------------------------------------------------
// CDRMDomainContext::SetHashChained
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMDomainContext::SetHashChainedL( const TBool& aHashChained )
    {
    iHashChained = aHashChained;
    };


// -----------------------------------------------------------------------------
// CDRMDomainContext::SetDomainKeysL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMDomainContext::SetDomainKeysL(
    const RPointerArray< HBufC8 >& aDomainKeys )
    {
    HBufC8* tempData = NULL;

    // Clear the old algorithms
    iDomainKeys.ResetAndDestroy();

    // Append the new stuff
    for( TInt i = 0; i < aDomainKeys.Count(); i++ )
        {
        tempData = aDomainKeys[i]->AllocLC();
        iDomainKeys.AppendL( tempData );
        CleanupStack::Pop();
        }
    };

// -----------------------------------------------------------------------------
// CDRMDomainContext::SetPublicKeyL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMDomainContext::SetRightsIssuerIDL(
    const TDesC8& aRightsIssuerID )
    {
    iRightsIssuerID.Copy( aRightsIssuerID );
    };

// -----------------------------------------------------------------------------
// CDRMDomainContext::SetRightsIssuerURLL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMDomainContext::SetRightsIssuerURLL(
    const TDesC8& aRightsIssuerURL )
    {
    HBufC8* newRightsIssuerURL = aRightsIssuerURL.AllocL();

    if( iRightsIssuerURL )
        {
        delete iRightsIssuerURL;
        iRightsIssuerURL = NULL;
        }
    iRightsIssuerURL = newRightsIssuerURL;
    };

// -----------------------------------------------------------------------------
// CDRMDomainContext::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMDomainContext::ExternalizeL( RWriteStream& aStream ) const
    {
    TUint8 flag;

    // Write the ContentID
    aStream << iDomainID;

    // expiry time
    WriteInt64L( iExpiryTime.Int64(), aStream );

    // hash chained
    aStream.WriteInt8L( iHashChained );

    // write the domain keys

    // How many:
    aStream.WriteInt32L( iDomainKeys.Count() );

    // for each in rpointerarray
    for( TInt i = 0; i < iDomainKeys.Count(); i++ )
        {
        aStream << *(iDomainKeys[i]);
        }

    // Write the rights issuer ID
    aStream << iRightsIssuerID;

    // Write the rights issuer url
    if (iRightsIssuerURL)
        {
        flag = ETrue;
        aStream << flag;
        aStream << *iRightsIssuerURL;
        }
    else
        {
        flag = EFalse;
        aStream << flag;
        }
    };

// -----------------------------------------------------------------------------
// CDRMDomainContext::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMDomainContext::InternalizeL( RReadStream& aStream )
    {
    HBufC8* dataPart = 0;
    TPtr8 dataBuffer(NULL,0,0);
    TInt count = 0;
    TInt64 timeData;
    TUint8 flag;

    // Read the DomainID
    aStream >> iDomainID;

    // read the expiry time
    ReadInt64L( timeData, aStream );
    iExpiryTime = timeData;

    // read the hash chained
    iHashChained = aStream.ReadInt8L();

    // read the algorithms

    // How many:
    count = aStream.ReadInt32L();

    // for each in rpointerarray
    for( TInt i = 0; i < count; i++ )
        {
        // Read the ContentID
        dataPart = HBufC8::NewL( aStream, KMaxUrlLength );

        // assign the new content id
        iDomainKeys.AppendL( dataPart );
        }

    // Read the rights issuer ID
    aStream >> iRightsIssuerID;

    // Read the rights issuer url
    if( iRightsIssuerURL )
        {
        delete iRightsIssuerURL;
        iRightsIssuerURL = NULL;
        }

    aStream >> flag;
    if (flag)
        {
        iRightsIssuerURL = HBufC8::NewL( aStream, KMaxUrlLength );
        }
    };


// -----------------------------------------------------------------------------
// CDRMDomainContext::ImportL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMDomainContext::ImportL( const TDesC8& aBuffer )
    {
    TInt size = aBuffer.Size();
    RMemReadStream stream( (TAny*)( aBuffer.Ptr() ), size );
    CleanupClosePushL( stream );

    InternalizeL( stream );

    CleanupStack::PopAndDestroy(); // Stream
    };


// -----------------------------------------------------------------------------
// CDRMDomainContext::ExportL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CDRMDomainContext::ExportL() const
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
// CDRMDomainContext::Size
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMDomainContext::Size() const
    {
    TInt size = 0;

    // domain identifier
    size += sizeof(TInt32);
    size += iDomainID.Length();

    // size of the expiry time
    size += sizeof(TInt64);

    // size of the boolean
    size += sizeof(TBool);

    // How many:
    size += sizeof(TInt32);

    // for each in rpointerarray
    for( TInt i = 0; i < iDomainKeys.Count(); i++ )
        {
        size += sizeof(TInt32);
        size += iDomainKeys[i]->Size();
        }

    // Domain generation
    size += sizeof(TInt32);

    // rights issuer ID
    size += sizeof(TInt32);
    size += iRightsIssuerID.Length();

    // rights issuer url
    size += sizeof(TUint8);
    if( iRightsIssuerURL )
        {
        size += sizeof(TInt32);
        size += iRightsIssuerURL->Size();
        }

    return size;
    };

// -----------------------------------------------------------------------------
// CDRMDomainContext::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
void CDRMDomainContext::ConstructL( const TDesC8& aDomainID,
                                    const RPointerArray< HBufC8 > aDomainKeys,
                                    const TDesC8& aRightsIssuerID,
                                    const TDesC8& aRightsIssuerURL )
    {
    HBufC8* domainKey = NULL;

    // Copy the domain identifier
    if ( aDomainID.Length() >= KMinDomainIdSize &&
        aDomainID.Length() <= KMaxDomainIdSize  )
        {
    iDomainID.Copy( aDomainID );
        }
    else
        {
        User::Leave( KErrArgument );
        }

    // Copy the domain keys
    for( TInt i = 0; i < aDomainKeys.Count();i++ )
        {
        domainKey = aDomainKeys[i]->AllocLC();
        iDomainKeys.AppendL( domainKey );
        CleanupStack::Pop();
        }

    // copy the rights issuer ID
    iRightsIssuerID.Copy( aRightsIssuerID );

    // copy the rights issuer URL
    iRightsIssuerURL = aRightsIssuerURL.AllocL();
    };

// -----------------------------------------------------------------------------
// CDRMRIContext::WriteInt64L
// -----------------------------------------------------------------------------
//
void CDRMDomainContext::WriteInt64L( const TInt64& aWrite, RWriteStream& aStream ) const
    {
    TPtr8 output( reinterpret_cast<TUint8*>(const_cast<TInt64*>(&aWrite)),
                  sizeof(TInt64), sizeof(TInt64) );

    aStream.WriteL( output, sizeof(TInt64) );
    }

// -----------------------------------------------------------------------------
// CDRMRIContext::ReadInt64L
// -----------------------------------------------------------------------------
//
void CDRMDomainContext::ReadInt64L( TInt64& aRead, RReadStream& aStream )
    {
    TPtr8 input( reinterpret_cast<TUint8*>(&aRead), 0, sizeof(TInt64) );

    aStream.ReadL( input, sizeof(TInt64) );
    };

// End of File
