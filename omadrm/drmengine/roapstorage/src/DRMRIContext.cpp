/*
* Copyright (c) 2004 - 2008 Nokia Corporation and/or its subsidiary(-ies).
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
#include <uri8.h>

#include "drmlog.h"
#include "DRMRIContext.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
const TInt KMaxCertificateLength = 327680;
const TInt KMaxUrlLength = 32768;
const TInt KMaxRiAliasLength = 1024;

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDRMRIContext::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMRIContext* CDRMRIContext::NewLC( const TDesC8& aRIID,
                                              const TDesC8& aRiAlias,
                                              const TDesC8& aVersion,
                                              const RPointerArray< HBufC8 >& aAlgorithms,
                                              const RPointerArray< HBufC8 >& aRIURLs,
                                              const TDesC8& aRightsIssuerURL,
                                              const TTime& aExpiryTime,
                                              const RPointerArray< HBufC8 >& aChain,
                                              const RPointerArray< HBufC8 >& aResponses,
                                              const TUint8 aDeviceCertCached,
                                              const TDesC8& aSelectedDeviceRoot,
                                              const TUint8 aIsMeteringAllowed )
    {
    CDRMRIContext* self = new( ELeave ) CDRMRIContext();
    CleanupStack::PushL( self );
    self->ConstructL( aRIID, aRiAlias, aVersion, aAlgorithms,
                      aRIURLs, aRightsIssuerURL, aExpiryTime,
                      aChain, aResponses, aDeviceCertCached,
                      aSelectedDeviceRoot, aIsMeteringAllowed );

    return self;
    };


// -----------------------------------------------------------------------------
// CDRMRIContext::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMRIContext* CDRMRIContext::NewL( const TDesC8& aRIID,
                                             const TDesC8& aRiAlias,
                                             const TDesC8& aVersion,
                                             const RPointerArray< HBufC8 >& aAlgorithms,
                                             const RPointerArray< HBufC8 >& aRIURLs,
                                             const TDesC8& aRightsIssuerURL,
                                             const TTime& aExpiryTime,
                                             const RPointerArray< HBufC8 >& aChain,
                                             const RPointerArray< HBufC8 >& aResponses,
                                             const TUint8 aDeviceCertCached,
                                             const TDesC8& aSelectedDeviceRoot,
                                             const TUint8 aIsMeteringAllowed )
    {
    CDRMRIContext* self = NewLC( aRIID, aRiAlias, aVersion, aAlgorithms,
                                 aRIURLs, aRightsIssuerURL, aExpiryTime,
                                 aChain, aResponses, aDeviceCertCached,
                                 aSelectedDeviceRoot, aIsMeteringAllowed );
    CleanupStack::Pop();

    return self;
    };

// -----------------------------------------------------------------------------
// CDRMRIContext::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMRIContext* CDRMRIContext::NewLC()
    {
    CDRMRIContext* self = new( ELeave ) CDRMRIContext();
    CleanupStack::PushL( self );

    return self;
    };


// -----------------------------------------------------------------------------
// CDRMRIContext::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMRIContext* CDRMRIContext::NewL()
    {
    CDRMRIContext* self = NewLC();
    CleanupStack::Pop();

    return self;
    };


// -----------------------------------------------------------------------------
// Default Constructor - First phase.
// Can be used by itself to generate an empty object
// -----------------------------------------------------------------------------
//
CDRMRIContext::CDRMRIContext()
    {

    };

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMRIContext::~CDRMRIContext()
    {
    // Delete the algorithm array
    iAlgorithms.ResetAndDestroy();
    iAlgorithms.Close();

    // Delete the ri uri array
    iRIURLs.ResetAndDestroy();
    iRIURLs.Close();

    // Delete the certificate chain
    iCertificateChain.ResetAndDestroy();
    iCertificateChain.Close();

    // Delete the OCSP responses
    iOcspResponse.ResetAndDestroy();
    iOcspResponse.Close();

    delete iRightsIssuerURL;
    delete iRiAlias;
    };

// -----------------------------------------------------------------------------
// CDRMRIContext::RIID
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CDRMRIContext::RIID() const
    {
    return iRIID;
    };

// -----------------------------------------------------------------------------
// CDRMRIContext::RIAlias
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CDRMRIContext::RIAlias() const
    {
    return *iRiAlias;
    };


// -----------------------------------------------------------------------------
// CDRMRIContext::Version
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CDRMRIContext::Version() const
    {
    return iVersion;
    };


// -----------------------------------------------------------------------------
// CDRMRIContext::Algorithms
// -----------------------------------------------------------------------------
//
EXPORT_C const RPointerArray< HBufC8 >& CDRMRIContext::Algorithms() const
    {
    return iAlgorithms;
    };


// -----------------------------------------------------------------------------
// CDRMRIContext::RIURLs
// -----------------------------------------------------------------------------
//
EXPORT_C const RPointerArray< HBufC8 >& CDRMRIContext::RIURLs() const
    {
    return iRIURLs;
    };

// -----------------------------------------------------------------------------
// CDRMRIContext::RightsIssuerURL
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CDRMRIContext::RightsIssuerURL() const
    {
    return *iRightsIssuerURL;
    };

// -----------------------------------------------------------------------------
// CDRMRIContext::ExpiryTime
// -----------------------------------------------------------------------------
//
EXPORT_C const TTime CDRMRIContext::ExpiryTime() const
    {
    return iExpiryTime;
    };

// -----------------------------------------------------------------------------
// CDRMRIContext::CertificateChain
// -----------------------------------------------------------------------------
//
EXPORT_C const RPointerArray< HBufC8 >& CDRMRIContext::CertificateChain() const
    {
    return iCertificateChain;
    };


// -----------------------------------------------------------------------------
// CDRMRIContext::OCSPResponse
// -----------------------------------------------------------------------------
//
EXPORT_C const RPointerArray< HBufC8 >& CDRMRIContext::OCSPResponse() const
    {
    return iOcspResponse;
    };


// -----------------------------------------------------------------------------
// CDRMRIContext::DeviceCertCached
// -----------------------------------------------------------------------------
//
EXPORT_C TUint8 CDRMRIContext::DeviceCertCached() const
    {
    return iDeviceCertCached;
    };


// -----------------------------------------------------------------------------
// CDRMRIContext::SelectedDeviceRoot
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CDRMRIContext::SelectedDeviceRoot() const
    {
    return iSelectedDeviceRoot;
    };


// -----------------------------------------------------------------------------
// CDRMRIContext::IsMeteringAllowed
// -----------------------------------------------------------------------------
//
EXPORT_C TUint8 CDRMRIContext::IsMeteringAllowed() const
    {
    return iIsMeteringAllowed;
    };

// -----------------------------------------------------------------------------
// CDRMRIContext::SetRIIDL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMRIContext::SetRIIDL( const TDesC8& aRIID )
    {
    if ( ( aRIID.Length() <= 0 ) || ( aRIID.Length() > SHA1_HASH ) )
        {
        User::Leave( KErrArgument );
        }
    iRIID.Copy( aRIID );
    };


// -----------------------------------------------------------------------------
// CDRMRIContext::SetVersionL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMRIContext::SetVersionL( const TDesC8& aVersion )
    {
    if ( ( aVersion.Length() <= 0 ) ||
        ( aVersion.Length() > Roap::KMaxRoapVersionLength ) )
        {
        User::Leave( KErrArgument );
        }
    iVersion.Copy( aVersion );
    };


// -----------------------------------------------------------------------------
// CDRMRIContext::SetAlgorithmsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMRIContext::SetAlgorithmsL( const RPointerArray< HBufC8 >& aAlgorithms )
    {
    HBufC8* tempData = NULL;

    // Clear the old algorithms
    iAlgorithms.ResetAndDestroy();

    // Append the new stuff
    for( TInt i = 0; i < aAlgorithms.Count(); i++ )
        {
        tempData = aAlgorithms[i]->AllocLC();
        iAlgorithms.AppendL( tempData );
        CleanupStack::Pop();
        }
    };

// -----------------------------------------------------------------------------
// CDRMRIContext::SetRIURLsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMRIContext::SetRIURLsL( const RPointerArray< HBufC8 >& aRIURLs )
    {
    HBufC8* tempData = NULL;

    // Clear the old algorithms
    iRIURLs.ResetAndDestroy();

    // Append the new stuff
    for( TInt i = 0; i < aRIURLs.Count(); i++ )
        {
        tempData = aRIURLs[i]->AllocLC();
        iRIURLs.AppendL( tempData );
        CleanupStack::Pop();
        }
    };

// -----------------------------------------------------------------------------
// CDRMRIContext::SetRightsIssuerURLL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMRIContext::SetRightsIssuerURLL(
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
// CDRMRIContext::SetExpiryTimeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMRIContext::SetExpiryTimeL( const TTime& aExpiryTime )
    {
    iExpiryTime = aExpiryTime;
    };


// -----------------------------------------------------------------------------
// CDRMRIContext::SetCertificateChainL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMRIContext::SetCertificateChainL( const RPointerArray< HBufC8 >& aChain )
    {
    HBufC8* tempData = NULL;

    // Clear the old algorithms
    iCertificateChain.ResetAndDestroy();

    // Append the new stuff
    for( TInt i = 0; i < aChain.Count(); i++ )
        {
        tempData = aChain[i]->AllocLC();
        iCertificateChain.AppendL( tempData );
        CleanupStack::Pop();
        }
    };


// -----------------------------------------------------------------------------
// CDRMRIContext::SetOCSPResponseL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMRIContext::SetOCSPResponseL( const RPointerArray< HBufC8 >& aResponses )
    {
    HBufC8* tempData = NULL;

    // Clear the old algorithms
    iOcspResponse.ResetAndDestroy();

    // Append the new stuff
    for( TInt i = 0; i < aResponses.Count(); i++ )
        {
        tempData = aResponses[i]->AllocLC();
        iOcspResponse.AppendL( tempData );
        CleanupStack::Pop();
        }
    };


// -----------------------------------------------------------------------------
// CDRMRIContext::SetDeviceCertCached
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMRIContext::SetDeviceCertCached( TUint8& aIsCached )
    {
    iDeviceCertCached = aIsCached;
    };


// -----------------------------------------------------------------------------
// CDRMRIContext::WhiteListURLExistsL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDRMRIContext::WhiteListURLExistsL( const TDesC8& aURL ) const
    {
    TUriParser8 uri1;
    TUriParser8 uri2;
    uri1.Parse( aURL );

    for( TInt i = 0; i < iRIURLs.Count(); i++ )
        {
        uri2.Parse( *iRIURLs[i] );
        if ( uri2.Compare( uri1, EUriHost ) == 0 )
            {
            return ETrue;
            }
        }

    if( iRightsIssuerURL )
        {
        uri2.Parse( *iRightsIssuerURL );
        if ( uri2.Compare(uri1, EUriHost ) == 0 )
            {
            return ETrue;
            }
        }
    return EFalse;
    };

// -----------------------------------------------------------------------------
// CDRMRIContext::SetSelectedDeviceRoot
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMRIContext::SetSelectedDeviceRoot( const TDesC8& aSelectedDeviceRoot )
    {
    if ( ( aSelectedDeviceRoot.Length() <= 0 ) ||
        ( aSelectedDeviceRoot.Length() > SHA1_HASH ) )
        {
        return;
        }
    iSelectedDeviceRoot.Copy( aSelectedDeviceRoot );
    };

// -----------------------------------------------------------------------------
// CDRMRIContext::SetMeteringStatus
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMRIContext::SetMeteringStatus( const TBool aIsMeteringAllowed )
    {
    iIsMeteringAllowed = aIsMeteringAllowed;
    };

// -----------------------------------------------------------------------------
// CDRMRIContext::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMRIContext::ExternalizeL( RWriteStream& aStream ) const
    {
    TInt dataLength = 0;
    TUint8 flag;

    // write the iRIID
    aStream.WriteL( iRIID );

    // write the iRiAlias
    if ( iRiAlias )
        {
        flag = ETrue;
        aStream << flag;
        aStream << *iRiAlias;
        }
    else
        {
        flag = EFalse;
        aStream << flag;
        }

    // write the version length
    aStream.WriteInt8L( static_cast<TInt8>(iVersion.Length() ) );

    // write the version
    if( iVersion.Length() )
        {
        aStream.WriteL( iVersion );
        }

    // write the algorithms

    // How many:
    aStream.WriteInt32L( iAlgorithms.Count() );

    // for each in rpointerarray
    for( TInt i = 0; i < iAlgorithms.Count(); i++ )
        {
        dataLength = 0;
        if( iAlgorithms[i] )
            {
            dataLength = iAlgorithms[i]->Length();
            }
        aStream.WriteInt32L( dataLength );

        if( dataLength )
            {
            aStream.WriteL( iAlgorithms[i]->Des() );
            }
        }

    // write the algorithms

    // How many:
    aStream.WriteInt32L( iRIURLs.Count() );

    // for each in rpointerarray
    for( TInt i = 0; i < iRIURLs.Count(); i++ )
        {
        dataLength = 0;
        if( iRIURLs[i] )
            {
            dataLength = iRIURLs[i]->Length();
            }
        aStream.WriteInt32L( dataLength );

        if( dataLength )
            {
            aStream.WriteL( iRIURLs[i]->Des() );
            }
        }

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

    // write the expiry time
    WriteInt64L( iExpiryTime.Int64(), aStream );

    // How many:
    aStream.WriteInt32L( iCertificateChain.Count() );

    // for each in rpointerarray
    for( TInt i = 0; i < iCertificateChain.Count(); i++ )
        {
        aStream << *(iCertificateChain[i]);
        }

    // How many:
    aStream.WriteInt32L( iOcspResponse.Count() );

    // for each in rpointerarray
    for( TInt i = 0; i < iOcspResponse.Count(); i++ )
        {
        aStream << *(iOcspResponse[i]);
        }

    // certificate caching indication
    aStream << iDeviceCertCached;

    // write the iSelectedDeviceRoot
    if (iSelectedDeviceRoot.Length())
        {
        flag = ETrue;
        aStream << flag;
        aStream.WriteL(iSelectedDeviceRoot);
        }
    else
        {
        flag = EFalse;
        aStream << flag;
        }

    // Add metering allowance info
    aStream << iIsMeteringAllowed;
    };

// -----------------------------------------------------------------------------
// CDRMRIContext::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMRIContext::InternalizeL( RReadStream& aStream )
    {
    TInt dataLength = 0;
    HBufC8* dataPart = 0;
    TPtr8 dataBuffer(NULL,0,0);
    TInt count = 0;
    TInt64 timeData;
    TUint8 flag;

    // read the iRIID
    aStream.ReadL( iRIID );

    // Read the rights issuer Alias
    if( iRiAlias )
        {
        delete iRiAlias;
        iRiAlias = NULL;
        }

    aStream >> flag;
    if (flag)
        {
        iRiAlias = HBufC8::NewL( aStream, KMaxRiAliasLength );
        }

    // size of the version string which is not always 20
    count = aStream.ReadInt8L();

    // read the version
    if( count )
        {
        aStream.ReadL( iVersion, count );
        }

    // read the algorithms

    // How many:
    count = aStream.ReadInt32L();

    // for each in rpointerarray
    for( TInt i = 0; i < count; i++ )
        {
        // Read the ContentID
        dataLength = aStream.ReadInt32L();

        if( dataLength )
            {
            // Reserve a new buffer:
            dataPart = HBufC8::NewMaxLC( dataLength );

            // Set the read buffer:
            dataBuffer.Set(const_cast<TUint8*>(dataPart->Ptr()), 0, dataLength);

            // Read the data:
            aStream.ReadL( dataBuffer );

            // Pop the buffer
            CleanupStack::Pop(); // dataPart

            // assign the new content id
            iAlgorithms.AppendL( dataPart );
            }
        }

    // read the urls

    // How many:
    count = aStream.ReadInt32L();

    // for each in rpointerarray
    for( TInt i = 0; i < count; i++ )
        {
        // Read the ContentID
        dataLength = aStream.ReadInt32L();

        if( dataLength )
            {
            // Reserve a new buffer:
            dataPart = HBufC8::NewMaxLC( dataLength );

            // Set the read buffer:
            dataBuffer.Set(const_cast<TUint8*>(dataPart->Ptr()), 0, dataLength);

            // Read the data:
            aStream.ReadL( dataBuffer );

            // Pop the buffer
            CleanupStack::Pop(); // dataPart

            // assign the new content id
            iRIURLs.AppendL( dataPart );
            }
        }

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

    // write the expiry time
    ReadInt64L( timeData, aStream );
    iExpiryTime = timeData;

    // How many:
    count = aStream.ReadInt32L();

    // for each in rpointerarray
    for( TInt i = 0; i < count; i++ )
        {
        // Read the certificate
        dataPart = HBufC8::NewL( aStream, KMaxCertificateLength );

        // assign the certificate
        iCertificateChain.AppendL( dataPart );
        }

    // How many:
    count = aStream.ReadInt32L();

     // for each in rpointerarray
    for( TInt i = 0; i < count; i++ )
        {
        // Read the certificate
        dataPart = HBufC8::NewL( aStream, KMaxCertificateLength );

        // assign the certificate
        iOcspResponse.AppendL( dataPart );
        }

    // certificate caching indication
    aStream >> iDeviceCertCached;

    // write the iSelectedDeviceRoot
    aStream >> flag;
    if(flag)
        {
        aStream.ReadL( iSelectedDeviceRoot );
        }

    aStream >> iIsMeteringAllowed;

    };

// -----------------------------------------------------------------------------
// CDRMRIContext::ImportL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDRMRIContext::ImportL( const TDesC8& aBuffer )
    {
    TInt size = aBuffer.Size();
    RMemReadStream stream( (TAny*)( aBuffer.Ptr() ), size );
    CleanupClosePushL( stream );

    InternalizeL( stream );

    CleanupStack::PopAndDestroy(); // Stream
    };


// -----------------------------------------------------------------------------
// CDRMRIContext::ExportL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CDRMRIContext::ExportL() const
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
// CDRMRIContext::Size
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDRMRIContext::Size() const
    {
    TInt size = 0;

    // get the size of iRIID
    size += SHA1_HASH;

    // get the size of RiAlias
    size += sizeof(TUint8);
    if( iRiAlias )
        {
        size += iRiAlias->Size();
        }

    // get the size of version
    size += sizeof(TInt8);

    size += iVersion.Length();

    // get the size of the algorithms

    // How many:
    size += sizeof(TInt32);

    // for each in rpointerarray
    for( TInt i = 0; i < iAlgorithms.Count(); i++ )
        {
        size += sizeof(TInt32);

        if( iAlgorithms[i] )
            {
            size += iAlgorithms[i]->Size();
            }
        }

    // get the size of the riurls

    // How many:
    size += sizeof(TInt32);

    // for each in rpointerarray
    for( TInt i = 0; i < iRIURLs.Count(); i++ )
        {
        size += sizeof(TInt32);

        if( iRIURLs[i] )
            {
            size += iRIURLs[i]->Size();
            }
        }

    // rights issuer url
    size += sizeof(TUint8);
    if( iRightsIssuerURL )
        {
        size += sizeof(TInt32);
        size += iRightsIssuerURL->Size();
        }

    // How many:
    size += sizeof(TInt32);

    // for each in rpointerarray
    for( TInt i = 0; i < iCertificateChain.Count(); i++ )
        {
        size += sizeof(TInt32);

        if( iCertificateChain[i] )
            {
            size += iCertificateChain[i]->Size();
            }
        }

    // How many:
    size += sizeof(TInt32);

    // for each in rpointerarray
    for( TInt i = 0; i < iOcspResponse.Count(); i++ )
        {
        size += sizeof(TInt32);

        if( iOcspResponse[i] )
            {
            size += iOcspResponse[i]->Size();
            }
        }

    // write the expiry time
    size += sizeof(TInt64);

    // certificate caching indication
    size += sizeof(TUint8);

    // get the size of iSelectedDeviceRoot
    size += sizeof(TUint8);
    if(iSelectedDeviceRoot.Length())
        {
        size += SHA1_HASH;
        }

    // get the size of iIsMeteringAllowed
    size += sizeof(TUint8);

    return size;
    };

// -----------------------------------------------------------------------------
// CDRMRIContext::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
void CDRMRIContext::ConstructL( const TDesC8& aRIID,
                                const TDesC8& aRiAlias,
                                const TDesC8& aVersion,
                                const RPointerArray< HBufC8 >& aAlgorithms,
                                const RPointerArray< HBufC8 >& aRIURLs,
                                const TDesC8& aRightsIssuerURL,
                                const TTime& aExpiryTime,
                                const RPointerArray< HBufC8 >& aChain,
                                const RPointerArray< HBufC8 >& aResponses,
                                const TUint8 aDeviceCertCached,
                                const TDesC8& aSelectedDeviceRoot,
                                const TUint8 aIsMeteringAllowed )
    {
    HBufC8* tempVar = NULL;

    if ( aRIID.Length() > SHA1_HASH )
        {
        User::Leave( KErrArgument);
        }

    iRIID.Copy( aRIID );

    if ( &aRiAlias  && aRiAlias.Length() )
        {
        iRiAlias = aRiAlias.AllocL();
        }

    // The size here should also be fixed
    iVersion.Copy( aVersion );

    // copy the algorithms
    for( TInt i = 0; i < aAlgorithms.Count(); i++ )
        {
        tempVar = aAlgorithms[i]->AllocLC();
        iAlgorithms.AppendL( tempVar );
        CleanupStack::Pop();
        }

    // copy the rights issuer urls
    for( TInt i = 0; i < aRIURLs.Count(); i++ )
        {
        tempVar = aRIURLs[i]->AllocLC();
        iRIURLs.AppendL( tempVar );
        CleanupStack::Pop();
        }

     // copy the rights issuer URL
     if ( aRightsIssuerURL.Length() )
        {
        iRightsIssuerURL = aRightsIssuerURL.AllocL();
        }

    // copy the certificate chain
    for( TInt i = 0; i < aChain.Count(); i++ )
        {
        tempVar = aChain[i]->AllocLC();
        iCertificateChain.AppendL( tempVar );
        CleanupStack::Pop();
        }

    // copy the certificate chain
    for( TInt i = 0; i < aResponses.Count(); i++ )
        {
        tempVar = aResponses[i]->AllocLC();
        iOcspResponse.AppendL( tempVar );
        CleanupStack::Pop();
        }

    // set the time
    iExpiryTime = aExpiryTime;

    // certificate caching indication
    iDeviceCertCached = aDeviceCertCached;

    if ( aSelectedDeviceRoot.Length() > SHA1_HASH )
        {
        User::Leave( KErrArgument);
        }

    iSelectedDeviceRoot.Copy( aSelectedDeviceRoot );

    iIsMeteringAllowed = aIsMeteringAllowed;
  };


// -----------------------------------------------------------------------------
// CDRMRIContext::WriteInt64L
// -----------------------------------------------------------------------------
//
void CDRMRIContext::WriteInt64L( const TInt64& aWrite, RWriteStream& aStream ) const
    {
    TPtr8 output( reinterpret_cast<TUint8*>(const_cast<TInt64*>(&aWrite)),
                  sizeof(TInt64), sizeof(TInt64) );

    aStream.WriteL( output, sizeof(TInt64) );
    }

// -----------------------------------------------------------------------------
// CDRMRIContext::ReadInt64L
// -----------------------------------------------------------------------------
//
void CDRMRIContext::ReadInt64L( TInt64& aRead, RReadStream& aStream )
    {
    TPtr8 input( reinterpret_cast<TUint8*>(&aRead), 0, sizeof(TInt64) );

    aStream.ReadL( input, sizeof(TInt64) );
    };

// End of File
