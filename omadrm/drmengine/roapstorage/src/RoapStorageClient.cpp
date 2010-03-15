/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Client Class of ROAP Storage Server
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <f32file.h>
#include <s32file.h>
#include <x509cert.h>
#include <x509keys.h>
#include <asn1dec.h>
#include <hash.h>
#include <s32mem.h>
#include <e32base.h>  // CleanupResetAndDestroyPushL dependencies
#include "OmaCrypto.h"
#include "RoapStorageClient.h"
#include "DRMRIContext.h"
#include "DRMDomainContext.h"
#include "drmroapclientserver.h"
#include "DRMRightsClient.h"
#include "cleanupresetanddestroy.h" // CleanupResetAndDestroyPushL
// LOCAL CONSTANTS AND MACROS

LOCAL_C const TInt KMaxMessageSlots = 4;
const TInt KMaxElementLength = 327680;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ArrayToBuffer
// Created buffer will contain <amount of elements:int32, data1, data2 ...>
// -----------------------------------------------------------------------------
//
LOCAL_C HBufC8* ArrayToBufferLC( const RPointerArray<HBufC8>& aArray )
    {
    HBufC8* buffer = NULL;
    TInt32 sizeOfElements = 0;

    for ( TInt i = 0; i < aArray.Count(); i++ )
        {
        // sizeof(TInt) is for element length info
        sizeOfElements += aArray[i]->Length() + sizeof(TInt);
        }

    // length of elements + amount of elements
    sizeOfElements += sizeof(TInt32);

    buffer = HBufC8::NewMaxLC( sizeOfElements );

    RMemWriteStream stream( (TAny*)( buffer->Ptr() ), sizeOfElements );
    CleanupClosePushL( stream );

    stream.WriteInt32L( aArray.Count() );

    for ( TInt i = 0; i < aArray.Count(); i++ )
        {
        stream << *aArray[i];
        }

    CleanupStack::PopAndDestroy( &stream );

    return buffer;
    }

// -----------------------------------------------------------------------------
// BufferToArrayL
// Created buffer will contain <amount of elements:int32, element data1, element data2...>
// -----------------------------------------------------------------------------
//
LOCAL_C RPointerArray<HBufC8> BufferToArrayL( TDesC8& aBuffer )
    {
    TInt32 count = 0;
    HBufC8* element = NULL;
    RPointerArray<HBufC8> array;
    TInt size = aBuffer.Size();
    RMemReadStream stream( (TAny*)( aBuffer.Ptr() ), size );
    CleanupClosePushL( stream );

    CleanupResetAndDestroyPushL( array );

    // amount of elements
    count = stream.ReadInt32L();

    // for each in RPointerArray
    for ( TInt i = 0; i < count; i++ )
        {
        // Read the element and append it to array
        element = HBufC8::NewLC( stream, KMaxElementLength );
        array.AppendL( element );
        CleanupStack::Pop( element );
        }

    CleanupStack::Pop( &array );
    CleanupStack::PopAndDestroy( &stream );

    return array;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RoapStorageClient::RoapStorageClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C Roap::RRoapStorageClient::RRoapStorageClient()
    {
    }

// -----------------------------------------------------------------------------
// RRoapStorageClient::~RRoapStorageClient()
// -----------------------------------------------------------------------------
//
EXPORT_C Roap::RRoapStorageClient::~RRoapStorageClient()
    {
    }

// -----------------------------------------------------------------------------
// RoapStorageClient::Connect
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt Roap::RRoapStorageClient::Connect()
    {
    TInt ret( KErrNone );

    const TVersion requiredVersion( Roap::KServerMajorVersion,
        Roap::KServerMinorVersion, Roap::KServerBuildVersion );

    RDRMRightsClient::StartServer();

    ret = CreateSession( KServerName, requiredVersion, KMaxMessageSlots );
    return ret;
    }

// -----------------------------------------------------------------------------
// RoapStorageClient::GetDevicePublicKeyL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt Roap::RRoapStorageClient::GetDevicePublicKeyL(
    CRSAPublicKey*& aPublicKey )
    {
    HBufC8* publicKeyDer( NULL );
    CX509SubjectPublicKeyInfo* publicKeyInfo( NULL );
    TX509KeyFactory factory;

    GetDevicePublicKeyDerL( publicKeyDer );
    CleanupStack::PushL( publicKeyDer );
    publicKeyInfo = CX509SubjectPublicKeyInfo::NewLC( *publicKeyDer );
    aPublicKey = factory.RSAPublicKeyL( publicKeyInfo->KeyData() );

    CleanupStack::PopAndDestroy( publicKeyInfo );
    CleanupStack::PopAndDestroy( publicKeyDer );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RoapStorageClient::GetDevicePublicKeyDerL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt Roap::RRoapStorageClient::GetDevicePublicKeyDerL(
    HBufC8*& aPublicKey )
    {
    TInt size = 0;
    TInt error = KErrNone;
    TPckg<TInt> package( size );

    error = SendReceive( Roap::EGetPublicKey, TIpcArgs( &package ) );

    User::LeaveIfError( error );

    HBufC8* publicKey( HBufC8::NewMaxLC( size ) );
    TPtr8 objectPkg( const_cast<TUint8*> ( publicKey->Ptr() ), size, size );

    User::LeaveIfError( SendReceive( Roap::EGetData, TIpcArgs( &objectPkg ) ) );

    CleanupStack::Pop( publicKey );
    aPublicKey = publicKey;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RoapStorageClient::GetDevicePublicKeyHashL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt Roap::RRoapStorageClient::GetDevicePublicKeyHashL( TDes8& aHash )
    {
    CSHA1* hash = NULL;
    HBufC8* key = NULL;

    GetDevicePublicKeyDerL( key );
    CleanupStack::PushL( key );
    hash = CSHA1::NewL();
    CleanupStack::PushL( hash );
    hash->Hash( *key );
    aHash.Copy( hash->Final() );
    CleanupStack::PopAndDestroy( hash );
    CleanupStack::PopAndDestroy( key );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RoapStorageClient::GetDeviceCertificateChainL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt Roap::RRoapStorageClient::GetDeviceCertificateChainL(
    RPointerArray<HBufC8>& aCertChain )
    {
    TInt size = 0;
    TInt error = KErrNone;
    TPckg<TInt> package( size );
    HBufC8* certChainBuf = NULL;

    error = SendReceive( Roap::EGetCertificates, TIpcArgs( &package ) );

    User::LeaveIfError( error );

    certChainBuf = HBufC8::NewMaxLC( size );
    TPtr8 objectPkg( const_cast<TUint8*> ( certChainBuf->Ptr() ), size, size );

    User::LeaveIfError( SendReceive( Roap::EGetData, TIpcArgs( &objectPkg ) ) );

    aCertChain = BufferToArrayL( *certChainBuf );

    CleanupStack::PopAndDestroy( certChainBuf );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RoapStorageClient::SignL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt Roap::RRoapStorageClient::SignL(
    const TDesC8& aHash,
    HBufC8*& aSignature )
    {
    TInt size = 0;
    TInt error = KErrNone;
    TPckg<TInt> package( size );

    if ( aHash.Length() )
        {
        error
            = SendReceive( Roap::ESignMessage, TIpcArgs( &package, &aHash ) );

        User::LeaveIfError( error );

        HBufC8* signature( HBufC8::NewMaxLC( size ) );
        TPtr8
            objectPkg( const_cast<TUint8*> ( signature->Ptr() ), size, size );

        User::LeaveIfError( SendReceive( Roap::EGetData,
            TIpcArgs( &objectPkg ) ) );

        CleanupStack::Pop( signature );
        aSignature = signature;
        return KErrNone;
        }

    User::Leave( KErrArgument );
    return KErrArgument;
    }

// -----------------------------------------------------------------------------
// RoapStorageClient::SignL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt Roap::RRoapStorageClient::RsaSignL(
    const TDesC8& aHash,
    HBufC8*& aSignature )
    {
    TInt size = 0;
    TInt error = KErrNone;
    TPckg<TInt> package( size );

    if ( aHash.Length() )
        {
        error = SendReceive( Roap::ERsaSign, TIpcArgs( &package, &aHash ) );

        User::LeaveIfError( error );

        HBufC8* signature( HBufC8::NewMaxLC( size ) );
        TPtr8
            objectPkg( const_cast<TUint8*> ( signature->Ptr() ), size, size );

        User::LeaveIfError( SendReceive( Roap::EGetData,
            TIpcArgs( &objectPkg ) ) );

        CleanupStack::Pop( signature );
        aSignature = signature;
        return KErrNone;
        }

    User::Leave( KErrArgument );
    return KErrArgument;
    }

// -----------------------------------------------------------------------------
// RoapStorageClient::VerifyL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool Roap::RRoapStorageClient::VerifyL(
    const TDesC8& aSignature,
    const TDesC8& aHash,
    const RPointerArray<HBufC8>& aCertificateChain )
    {
    CRSAPublicKey* publicKey( NULL );
    TBool r( ETrue );
    CX509Certificate* cert( NULL );
    TX509KeyFactory factory;

    if ( aCertificateChain.Count() <= 0 )
        {
        User::Leave( KErrArgument );
        }
    cert = CX509Certificate::NewLC( *aCertificateChain[0] );
    publicKey = factory.RSAPublicKeyL( cert->PublicKey().KeyData() );
    CleanupStack::PushL( publicKey );
    r = OmaCrypto::RsaPssVerifyHashL( publicKey, aSignature, aHash );
    CleanupStack::PopAndDestroy( publicKey );
    CleanupStack::PopAndDestroy( cert );
    return r;
    }

// -----------------------------------------------------------------------------
// RoapStorageClient::GetRIContextL
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMRIContext* Roap::RRoapStorageClient::GetRIContextL(
    const TDesC8& aRiID )
    {
    TInt error = KErrNone;
    CDRMRIContext* context( NULL );

    if ( aRiID.Length() )
        {
        TInt size = 0;
        TPckg<TInt> package( size );

        error
            = SendReceive( Roap::EGetRiContext, TIpcArgs( &package, &aRiID ) );

        if ( error == KErrNotFound )
            {
            return NULL;
            }

        User::LeaveIfError( error );

        HBufC8* contextData = HBufC8::NewMaxLC( size );

        // Package 'object' into TPtr8.
        TPtr8 objectPkg( const_cast<TUint8*> ( contextData->Ptr() ), size,
            size );

        User::LeaveIfError( SendReceive( Roap::EGetData,
            TIpcArgs( &objectPkg ) ) );

        context = CDRMRIContext::NewLC();

        context->ImportL( contextData->Des() );

        CleanupStack::Pop( context );
        CleanupStack::PopAndDestroy( contextData );
        }
    else
        {
        User::Leave( KErrArgument );
        }

    return context;
    }

// -----------------------------------------------------------------------------
// RoapStorageClient::GetDomainContextL
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMDomainContext* Roap::RRoapStorageClient::GetDomainContextL(
    const TDesC8& aDomainID )
    {
    TInt error = KErrNone;
    CDRMDomainContext* context( NULL );

    if ( aDomainID.Length() )
        {
        TInt size = 0;
        TPckg<TInt> package( size );

        // Call the server. Throw an exception in case of an error.
        error = SendReceive( Roap::EGetDomainContext, TIpcArgs( &package,
            &aDomainID ) );

        if ( error == KErrNotFound )
            {
            return NULL;
            }

        HBufC8* contextData = HBufC8::NewMaxLC( size );

        // Package 'object' into TPtr8.
        TPtr8 objectPkg( const_cast<TUint8*> ( contextData->Ptr() ), size,
            size );

        User::LeaveIfError( SendReceive( Roap::EGetData,
            TIpcArgs( &objectPkg ) ) );

        context = CDRMDomainContext::NewLC();

        context->ImportL( contextData->Des() );

        CleanupStack::Pop( context );
        CleanupStack::PopAndDestroy( contextData );

        return context;
        }

    User::Leave( KErrArgument );

    return NULL;
    }

// -----------------------------------------------------------------------------
// RRoapStorageClient::AddRIContextL
// Add a new RI Context to the ROAP storage
// -----------------------------------------------------------------------------
//
EXPORT_C void Roap::RRoapStorageClient::AddRIContextL(
    const CDRMRIContext& aRiContext )
    {
    HBufC8* contextData = aRiContext.ExportL();
    TInt size = aRiContext.Size();
    TPtr8 riContext( NULL, 0 );
    if ( size )
        {
        riContext.Set( const_cast<TUint8*> ( contextData->Ptr() ), size, size );

        User::LeaveIfError( SendReceive( Roap::EAddRiContext, TIpcArgs(
            &riContext ) ) );
        }
    delete contextData;
    contextData = NULL;
    }

// -----------------------------------------------------------------------------
// RRoapStorageClient::AddDomainContextL
// Add a new Domain Context to the ROAP storage
// -----------------------------------------------------------------------------
//
EXPORT_C void Roap::RRoapStorageClient::AddDomainContextL(
    const CDRMDomainContext& aDomainContext,
    const RPointerArray<HBufC8>& aMacs,
    const RPointerArray<HBufC8>& aDomainKeyElements,
    TKeyTransportScheme& aTransportScheme )
    {
    TPckg<TKeyTransportScheme> package( aTransportScheme );
    HBufC8* contextData = aDomainContext.ExportL();
    CleanupStack::PushL( contextData );
    TInt size = aDomainContext.Size();
    TPtr8 contextPtr( NULL, 0 );
    TPtr8 domainXmltPtr( 0, 0 );
    TPtr8 macValuesPtr( 0, 0 );
    HBufC8* domainXml = NULL;
    HBufC8* macValues = NULL;

    domainXml = ArrayToBufferLC( aDomainKeyElements );
    macValues = ArrayToBufferLC( aMacs );

    if ( size && domainXml && macValues )
        {
        contextPtr.Set( const_cast<TUint8*> ( contextData->Ptr() ), size,
            size );
        domainXmltPtr.Set( domainXml->Des() );
        macValuesPtr.Set( macValues->Des() );

        User::LeaveIfError( SendReceive( Roap::EAddDomainContext, TIpcArgs(
            &contextPtr, &package, &macValuesPtr, &domainXmltPtr ) ) );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    CleanupStack::PopAndDestroy( macValues );
    CleanupStack::PopAndDestroy( domainXml );
    CleanupStack::PopAndDestroy( contextData );
    }

// -----------------------------------------------------------------------------
// RRoapStorageClient::DeleteRiContextL
// -----------------------------------------------------------------------------
//
EXPORT_C void Roap::RRoapStorageClient::DeleteRiContextL( const TDesC8& aRiID )
    {
    if ( aRiID.Length() )
        {
        User::LeaveIfError( SendReceive( Roap::EDeleteRiContext, TIpcArgs(
            &aRiID ) ) );
        return;
        }
    User::Leave( KErrArgument );
    }

// -----------------------------------------------------------------------------
// RRoapStorageClient::DeleteDomainContextL
// -----------------------------------------------------------------------------
//
EXPORT_C void Roap::RRoapStorageClient::DeleteDomainContextL(
    const TDesC8& aDomainID )
    {
    if ( aDomainID.Length() )
        {
        User::LeaveIfError( SendReceive( Roap::EDeleteDomainContext,
            TIpcArgs( &aDomainID ) ) );
        return;
        }
    User::Leave( KErrArgument );
    }

// -----------------------------------------------------------------------------
// RRoapStorageClient::DeleteExpiredRIsL
// -----------------------------------------------------------------------------
//
EXPORT_C void Roap::RRoapStorageClient::DeleteExpiredRIsL( const TTime& aTime )
    {
    TPckg<TTime> package( aTime );

    User::LeaveIfError( SendReceive( Roap::EDeleteExpiredRIs, TIpcArgs(
        &package ) ) );
    }

// -----------------------------------------------------------------------------
// RRoapStorageClient::DeleteExpiredDomainsL
// -----------------------------------------------------------------------------
//
EXPORT_C void Roap::RRoapStorageClient::DeleteExpiredDomainsL(
    const TTime& aTime )
    {
    TPckg<TTime> package( aTime );

    User::LeaveIfError( SendReceive( Roap::EDeleteExpiredDomains, TIpcArgs(
        &package ) ) );
    }

// -----------------------------------------------------------------------------
// RRoapStorageClient::DeleteExpiredContextsL
// -----------------------------------------------------------------------------
//
EXPORT_C void Roap::RRoapStorageClient::DeleteExpiredContextsL(
    const TTime& aTime )
    {
    TPckg<TTime> package( aTime );

    User::LeaveIfError( SendReceive( Roap::EDeleteExpired,
        TIpcArgs( &package ) ) );
    }

// -----------------------------------------------------------------------------
// RRoapStorageClient::DeleteAllL
// -----------------------------------------------------------------------------
//
EXPORT_C void Roap::RRoapStorageClient::DeleteAllL()
    {
    User::LeaveIfError( SendReceive( Roap::EDeleteAll ) );
    }

// -----------------------------------------------------------------------------
// RRoapStorageClient::WhiteListURLExistsL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool Roap::RRoapStorageClient::WhiteListURLExistsL(
    const TDesC8& aURL, TBool& aFromPreconfiguredWhiteList )
    {
    TInt r = KErrArgument;

    TPckg<TBool> preConfiguredInOutParam( aFromPreconfiguredWhiteList );
    if ( aURL.Length() )
        {
        // Call the server.
        r = SendReceive( Roap::EWhiteListCheck, TIpcArgs( &aURL,
            &preConfiguredInOutParam ) );
        if ( r == KErrNone )
            {
            return ETrue;
            }
        else if ( r == KErrNotFound )
            {
            return EFalse;
            }
        }
    User::LeaveIfError( r );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// RRoapStorageClient::SelectTrustedRootL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt Roap::RRoapStorageClient::SelectTrustedRootL(
    const RPointerArray<HBufC8>& aTrustedRoots,
    TDes8& aSelectedRoot ) const
    {
    TInt ret = KErrNone;

    if ( aTrustedRoots.Count() )
        {
        HBufC8* rootsBuf = NULL;
        rootsBuf = ArrayToBufferLC( aTrustedRoots );

        ret = SendReceive( Roap::ESelectRoot, TIpcArgs( rootsBuf,
            &aSelectedRoot ) );

        CleanupStack::PopAndDestroy( rootsBuf );
        return ret;
        }

    return KErrArgument;
    }

// -----------------------------------------------------------------------------
// RRoapStorageClient::SelectTrustedRootL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt Roap::RRoapStorageClient::SelectTrustedRootL(
    const TDesC8& aTrustedRoot ) const
    {
    TInt ret = KErrNone;

    if ( aTrustedRoot.Length() > SHA1_HASH )
        {
        User::Leave( KErrArgument );
        }

    ret = SendReceive( Roap::EActivateRoot, TIpcArgs( &aTrustedRoot ) );
    return ret;
    }

// -----------------------------------------------------------------------------
// RRoapStorageClient::GetTrustedRootsL()
// -----------------------------------------------------------------------------
//
EXPORT_C void Roap::RRoapStorageClient::GetTrustedRootsL(
    RPointerArray<HBufC8>& aTrustedRoots ) const
    {
    TInt size = 0;
    TPckg<TInt> package( size );

    User::LeaveIfError( SendReceive( Roap::EGetTrustedRoots, TIpcArgs(
        &package ) ) );

    HBufC8* rootsData = HBufC8::NewMaxLC( size );

    // Package 'object' into TPtr8.
    TPtr8 objectPkg( const_cast<TUint8*> ( rootsData->Ptr() ), size, size );

    User::LeaveIfError( SendReceive( Roap::EGetData, TIpcArgs( &objectPkg ) ) );

    aTrustedRoots = BufferToArrayL( *rootsData );

    CleanupStack::PopAndDestroy( rootsData );
    }

// -----------------------------------------------------------------------------
// RRoapStorageClient::GetTrustedRootL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* Roap::RRoapStorageClient::GetRootCertificateL(
    const TDesC& aSubject ) const
    {
    TInt error = KErrNone;
    HBufC8* rootCert = NULL;

    if ( aSubject.Length() )
        {
        TInt size = 0;
        TPckg<TInt> package( size );

        error = SendReceive( Roap::EGetRootCert, TIpcArgs( &package,
            &aSubject ) );

        if ( error == KErrNotFound )
            {
            return NULL;
            }

        User::LeaveIfError( error );

        rootCert = HBufC8::NewMaxLC( size );

        // Package 'object' into TPtr8.
        TPtr8 objectPkg( const_cast<TUint8*> ( rootCert->Ptr() ), size, size );

        User::LeaveIfError( SendReceive( Roap::EGetData,
            TIpcArgs( &objectPkg ) ) );

        CleanupStack::Pop( rootCert );
        return rootCert;
        }

    User::Leave( KErrArgument );

    return NULL;
    }

// -----------------------------------------------------------------------------
// RRoapStorageClient::GetMeteringData()
// -----------------------------------------------------------------------------
//
#ifndef RD_DRM_METERING
EXPORT_C HBufC8* Roap::RRoapStorageClient::GetMeteringDataL(
    TDesC8& /*aRiId*/,
    TDes8& /*aMacKey*/,
    TDes8& /*aEncKeyHash*/,
    HBufC8*& /*aEncryptedMekAndMak*/ )
    {
    return NULL;
    }
#else
EXPORT_C HBufC8* Roap::RRoapStorageClient::GetMeteringDataL(
    TDesC8& aRiId,
    TDes8& aMacKey,
    TDes8& aEncKeyHash,
    HBufC8*& aEncryptedMekAndMak )
    {
    TInt error = KErrNone;
    HBufC8* meteringData = NULL;
    HBufC8* csBuffer = NULL;

    if ( aRiId.Length() )
        {
        TInt mekAndMakSize = 0;
        TInt meteringDataSize = 0;
        TPckg<TInt> MakMek( mekAndMakSize );
        TPckg<TInt> dataSize( meteringDataSize );

        error = SendReceive( Roap::EGetMeteringData, TIpcArgs( &aRiId,
            &MakMek, &dataSize ) );

        if ( error == KErrNotFound )
            {
            return NULL;
            }

        User::LeaveIfError( error );

        csBuffer = HBufC8::NewMaxLC( OmaCrypto::KMacSize + SHA1_HASH
            + mekAndMakSize + meteringDataSize );

        // Package 'object' into TPtr8.
        TPtr8 objectPkg( const_cast<TUint8*> ( csBuffer->Ptr() ),
            OmaCrypto::KMacSize + SHA1_HASH + mekAndMakSize
                + meteringDataSize, OmaCrypto::KMacSize + SHA1_HASH
                + mekAndMakSize + meteringDataSize );

        User::LeaveIfError( SendReceive( Roap::EGetData,
            TIpcArgs( &objectPkg ) ) );

        aMacKey = objectPkg.Mid( 0, OmaCrypto::KMacSize );
        aEncKeyHash = objectPkg.Mid( OmaCrypto::KMacSize, SHA1_HASH );
        aEncryptedMekAndMak = objectPkg.Mid( OmaCrypto::KMacSize + SHA1_HASH,
            mekAndMakSize ).AllocL();
        meteringData = objectPkg.Mid( OmaCrypto::KMacSize + SHA1_HASH
            + mekAndMakSize, meteringDataSize ).AllocL();
        CleanupStack::PopAndDestroy( csBuffer );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    return meteringData;
    }
#endif //RD_DRM_METERING
// -----------------------------------------------------------------------------
// RRoapStorageClient::DeleteMeteringDataL
// -----------------------------------------------------------------------------
//
#ifndef RD_DRM_METERING
EXPORT_C void Roap::RRoapStorageClient::DeleteMeteringDataL(
    const TDesC8& /*aRiId*/)
    {
    return;
    }
#else
EXPORT_C void Roap::RRoapStorageClient::DeleteMeteringDataL(
    const TDesC8& aRiId )
    {
    if ( aRiId.Length() )
        {
        User::LeaveIfError( SendReceive( Roap::EDeleteMeteringData, TIpcArgs(
            &aRiId ) ) );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    return;
    }
#endif //RD_DRM_METERING
// -----------------------------------------------------------------------------
// RRoapStorageClient::GetAllRIContextsL
// -----------------------------------------------------------------------------
//
#ifndef RD_DRM_METERING
EXPORT_C void Roap::RRoapStorageClient::GetAllRIContextsL(
    RPointerArray<Roap::CDRMRIContext>& /*aRiContexts*/)
    {
    }
#else
EXPORT_C void Roap::RRoapStorageClient::GetAllRIContextsL(
    RPointerArray<CDRMRIContext>& aRiContexts )
    {
    TInt error = KErrNone;
    CDRMRIContext* context = NULL;

    // Clear whatever is in the list.
    aRiContexts.ResetAndDestroy();

    TInt size = 0;
    TInt blocksize = 0;

    TPckg<TInt> package( size );

    error = SendReceive( Roap::ERetrieveAllRIContexts, TIpcArgs( &package ) );

    // None are found is not an error, but just an empty list.
    if ( error == KErrNotFound )
        {
        return;
        }

    // otherwise it is an error.
    User::LeaveIfError( error );

    // Get the data block received.


    HBufC8* contextData = HBufC8::NewMaxLC( size );

    // Package 'object' into TPtr8.
    TPtr8 objectPkg( const_cast<TUint8*> ( contextData->Ptr() ), size, size );

    User::LeaveIfError( SendReceive( Roap::EGetData, TIpcArgs( &objectPkg ) ) );

    RMemReadStream stream( (TAny*)( objectPkg.Ptr() ), size );
    CleanupClosePushL( stream );

    // We have the data, parse it and fill the aRiContexts:

    blocksize = stream.ReadInt32L();

    while ( blocksize )
        {
        // Create a new context:
        context = CDRMRIContext::NewLC();

        // import it from the data block
        context->InternalizeL( stream );

        // add it to the pointer array
        aRiContexts.AppendL( context );

        // pop it from the cleanup stack
        CleanupStack::Pop( context ); // context


        // read the new block size and increment the offset:
        blocksize = stream.ReadInt32L();
        }

    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( contextData );
    return;
    }

#endif // RD_DRM_METERING
// -----------------------------------------------------------------------------
// RRoapStorageClient::UpdateRiContextL
// -----------------------------------------------------------------------------
//
#ifndef RD_DRM_METERING
EXPORT_C void Roap::RRoapStorageClient::UpdateRIContextL(
    const Roap::CDRMRIContext& /*aContext*/)
    {
    }
#else
EXPORT_C void Roap::RRoapStorageClient::UpdateRIContextL(
    const CDRMRIContext& aContext )
    {
    HBufC8* contextData = aContext.ExportL();
    CleanupStack::PushL( contextData );
    TInt size = aContext.Size();
    TPtr8 riContext( NULL, 0 );
    if ( size )
        {
        riContext.Set( const_cast<TUint8*> ( contextData->Ptr() ), size, size );

        User::LeaveIfError( SendReceive( Roap::EUpdateRIContext, TIpcArgs(
            &riContext ) ) );
        }
    CleanupStack::PopAndDestroy( contextData );
    }
#endif // RD_DRM_METERING

// -----------------------------------------------------------------------------
// RRoapStorageClient::UpdateDrmTimeL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool Roap::RRoapStorageClient::UpdateDrmTimeL(
    const RPointerArray<HBufC8>& aCertChain,
    const RPointerArray<HBufC8>& aOcspResponses,
    const TDesC8& aNonce )
    {
    TBool clockUpdated( EFalse );
    TPckg<TBool> retParam( clockUpdated );
    HBufC8* certChain( ArrayToBufferLC( aCertChain ) );
    HBufC8* ocspResponses( ArrayToBufferLC( aOcspResponses ) );
    TPtrC8 nonceParam( aNonce );
    User::LeaveIfError( SendReceive( Roap::EUpdateDrmTime, TIpcArgs(
        certChain, ocspResponses, &nonceParam, &retParam ) ) );
    CleanupStack::PopAndDestroy( ocspResponses );
    CleanupStack::PopAndDestroy( certChain );
    return clockUpdated;
    }

// -----------------------------------------------------------------------------
// RRoapStorageClient::VerifyOcspResponsesL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool Roap::RRoapStorageClient::VerifyOcspResponsesL(
    const RPointerArray<HBufC8>& aOcspResponses,
    const TDesC8& aRiCaCert,
    const RPointerArray<HBufC8>& aCertSerialNums )
    {
    TBool ret( EFalse );
    HBufC8* ocspData( ArrayToBufferLC( aOcspResponses ) );
    HBufC8* serialNumsData( ArrayToBufferLC( aCertSerialNums ) );

    TPtrC8 riCaCertParam( aRiCaCert );
    TPckg<TBool> retParam( ret );

    User::LeaveIfError( SendReceive( Roap::EVerifyOcspResponses, TIpcArgs(
        ocspData, &riCaCertParam, serialNumsData, &retParam ) ) );

    CleanupStack::PopAndDestroy( serialNumsData );
    CleanupStack::PopAndDestroy( ocspData );
    return ret;
    }

// -----------------------------------------------------------------------------
// RRoapStorageClient::GetOcspResponderIdL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* Roap::RRoapStorageClient::GetOcspResponderIdL(
    const TDesC8& aRiID )
    {
    TPtrC8 riIdParam( aRiID );
    TInt size( 0 );
    TPckg<TInt> sizeParam( size );

    User::LeaveIfError( SendReceive( Roap::EGetOcspResponderId, TIpcArgs(
        &riIdParam, &sizeParam ) ) );

    if ( size == 0 )
        {
        return NULL;
        }
    HBufC8* responderData = HBufC8::NewMaxLC( size );

    // Package 'object' into TPtr8.
    TPtr8 objectPkg( responderData->Des() );

    User::LeaveIfError( SendReceive( Roap::EGetData, TIpcArgs( &objectPkg ) ) );

    CleanupStack::Pop( responderData );
    return responderData;
    }
//  End of File
