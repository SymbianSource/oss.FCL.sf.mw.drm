/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
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

#ifdef RD_MULTIPLE_DRIVE
#include <DriveInfo.h>
#endif

#include "OmaCrypto.h"
#include "RoapStorageClient.h"

using namespace Roap;

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

#ifdef RD_MULTIPLE_DRIVE
_LIT(KCertChainDir, "%c:\\drm\\PKI\\");
_LIT(KSigningCertPrefix, "SigningCert");
_LIT(KDeviceCertFileName, "%c:\\drm\\PKI\\DeviceCert.der");
_LIT(KDevicePrivateKeyFileName, "%c:\\drm\\PKI\\DevicePrivateKey.der");
#else
_LIT(KCertChainDir, "c:\\drm\\PKI\\");
_LIT(KSigningCertPrefix, "SigningCert");
_LIT(KDeviceCertFileName, "c:\\drm\\PKI\\DeviceCert.der");
_LIT(KDevicePrivateKeyFileName, "c:\\drm\\PKI\\DevicePrivateKey.der");
#endif


// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
LOCAL_C void ReadFileL(
    HBufC8*& aContent,
    const TDesC& aName);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
LOCAL_C void ReadFileL(
    HBufC8*& aContent,
    const TDesC& aName)
	{	
	TInt size = 0;
	RFile file;
	RFs fs;

    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
	User::LeaveIfError(file.Open(fs, aName, EFileRead));
    CleanupClosePushL(file);
    User::LeaveIfError(file.Size(size));
    aContent = HBufC8::NewLC(size);
    TPtr8 ptr(aContent->Des());
    User::LeaveIfError(file.Read(ptr, size));
	CleanupStack::Pop(); //aContent
	CleanupStack::PopAndDestroy(2); // file, fs
	}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RoapStorageClient::RoapStorageClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
RRoapStorageClient::RRoapStorageClient()
    {
    }

// Destructor
RRoapStorageClient::~RRoapStorageClient()
    {
    
    }

// -----------------------------------------------------------------------------
// RoapStorageClient::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt RRoapStorageClient::Connect(void)
    {
    return KErrNone;
    }

TInt RRoapStorageClient::GetDeviceCertificateL(
    HBufC8*& aCert)
    {
    
#ifndef RD_MULTIPLE_DRIVE

    ReadFileL(aCert, KDeviceCertFileName);
    
#else //RD_MULTIPLE_DRIVE
    
    RFs fs;
    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
	
	User::LeaveIfError( fs.Connect() );
	fs.DriveToChar( driveNumber, driveLetter );
    fs.Close();
    
	TFileName deviceCertFileName;
	deviceCertFileName.Format( KDeviceCertFileName, (TUint)driveLetter );
    
    ReadFileL(aCert, deviceCertFileName);
    
#endif
    
    return KErrNone;
    }

TInt RRoapStorageClient::GetDevicePublicKeyL(
    CRSAPublicKey*& aPublicKey)
    {
    HBufC8* certData;
    CX509Certificate* cert;
    TX509KeyFactory factory;
    
#ifndef RD_MULTIPLE_DRIVE

    ReadFileL(certData, KDeviceCertFileName);
    
#else //RD_MULTIPLE_DRIVE
    
    RFs fs;
    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
	
	User::LeaveIfError( fs.Connect() );
	fs.DriveToChar( driveNumber, driveLetter );
    fs.Close();
    
	TFileName deviceCertFileName;
	deviceCertFileName.Format( KDeviceCertFileName, (TUint)driveLetter );
    
    ReadFileL(certData, deviceCertFileName);
    
#endif
    
    CleanupStack::PushL(certData);
    cert = CX509Certificate::NewLC(*certData);
    aPublicKey = factory.RSAPublicKeyL(cert->PublicKey().KeyData());
    CleanupStack::PopAndDestroy(2); // cert, certData
    return KErrNone;
    }
    
TInt RRoapStorageClient::GetDevicePublicKeyDerL(
    HBufC8*& aPublicKey)
    {
    HBufC8* certData;
    CX509Certificate* cert;
    
#ifndef RD_MULTIPLE_DRIVE

    ReadFileL(certData, KDeviceCertFileName);
    
#else //RD_MULTIPLE_DRIVE
    
    RFs fs;
    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
	
	User::LeaveIfError( fs.Connect() );
	fs.DriveToChar( driveNumber, driveLetter );
    fs.Close();
    
	TFileName deviceCertFileName;
	deviceCertFileName.Format( KDeviceCertFileName, (TUint)driveLetter );
    
    ReadFileL(certData, deviceCertFileName);
    
#endif
     
    CleanupStack::PushL(certData);
    cert = CX509Certificate::NewLC(*certData);
    aPublicKey = cert->PublicKey().KeyData().AllocL();
    CleanupStack::PopAndDestroy(2); // cert, certData
    return KErrNone;
    }

TInt RRoapStorageClient::GetDevicePublicKeyHashL(
    TDes8& aHash)
    {
    CSHA1* hash = NULL;
    HBufC8* key = NULL;
    
    GetDevicePublicKeyDerL(key);
    CleanupStack::PushL(key);
    hash = CSHA1::NewL();
    CleanupStack::PushL(hash);
    hash->Hash(*key);
    aHash.Copy(hash->Final());
    CleanupStack::PopAndDestroy(2); // hash, key
    return KErrNone;
    }

TInt RRoapStorageClient::GetDevicePrivateKeyL(
    CRSAPrivateKey*& aKey)
    {
    HBufC8* raw = NULL;
	TASN1DecInteger encInt;
    TInt pos = 0;
    
#ifndef RD_MULTIPLE_DRIVE

    ReadFileL(raw, KDevicePrivateKeyFileName);
    
#else //RD_MULTIPLE_DRIVE
    
    RFs fs;
    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
	
	User::LeaveIfError( fs.Connect() );
	fs.DriveToChar( driveNumber, driveLetter );
    fs.Close();
    
	TFileName devicePrivateKeyFileName;
	devicePrivateKeyFileName.Format( 
	                    KDevicePrivateKeyFileName, (TUint)driveLetter );
    
    ReadFileL(raw, devicePrivateKeyFileName);
    
#endif
    
    CleanupStack::PushL(raw);

	TASN1DecGeneric gen(*raw);
	gen.InitL();
	pos += gen.LengthDERHeader();
	if (gen.Tag() != EASN1Sequence)
        {
		User::Leave(KErrArgument);
        }

    encInt.DecodeDERShortL(*raw, pos); // version
	RInteger modulus = encInt.DecodeDERLongL(*raw, pos);
    CleanupStack::PushL(modulus);
	RInteger publicExponent = encInt.DecodeDERLongL(*raw, pos);
    CleanupStack::PushL(publicExponent);
	RInteger privateExponent = encInt.DecodeDERLongL(*raw, pos);
    CleanupStack::PushL(privateExponent);

	aKey = CRSAPrivateKeyStandard::NewL(modulus, privateExponent);

    CleanupStack::Pop(); // privateExponent
    CleanupStack::PopAndDestroy();// publicExponent
    CleanupStack::Pop(); // modulus
    CleanupStack::PopAndDestroy(); // raw

    return KErrNone;
    }

TInt RRoapStorageClient::GetDeviceCertificateChainL(
    RPointerArray<HBufC8>& aCertChain)
    {
    RFs fs;
    RFile file;
    TFileName fileName;
    CDir* dir;
    HBufC8* cert;
    TInt size;
    TPtr8 ptr(0, 0);
    TInt itemsToPop = 0;
    TInt i;
    TEntry entry;

    CleanupClosePushL(aCertChain);
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    
#ifndef RD_MULTIPLE_DRIVE

    User::LeaveIfError(fs.GetDir(KCertChainDir, KEntryAttMatchMask,
        ESortByName, dir));
    User::LeaveIfError(fs.SetSessionPath(KCertChainDir));
    
#else //RD_MULTIPLE_DRIVE
    
    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
	fs.DriveToChar( driveNumber, driveLetter );
    
	TFileName certChainDir;
	certChainDir.Format( KCertChainDir, (TUint)driveLetter );
    
    User::LeaveIfError(fs.GetDir(certChainDir, KEntryAttMatchMask,
        ESortByName, dir));
    User::LeaveIfError(fs.SetSessionPath(certChainDir));
    
#endif
    
    CleanupStack::PushL(dir);
    GetDeviceCertificateL(cert);
    CleanupStack::PushL(cert);
    User::LeaveIfError(aCertChain.Append(cert));
    itemsToPop++;
    for (i = 0; i < dir->Count(); i++)
        {
        entry = (*dir)[i];
        if (entry.iName.Left(KSigningCertPrefix().Length()).CompareF(
            KSigningCertPrefix) == 0)
            {
            User::LeaveIfError(file.Open(fs, entry.iName, EFileRead));
            CleanupClosePushL(file);
            User::LeaveIfError(file.Size(size));
            cert = HBufC8::NewMax(size);
            ptr.Set(cert->Des());
            CleanupStack::PushL(cert);
            User::LeaveIfError(aCertChain.Append(cert));
            itemsToPop++;
            User::LeaveIfError(file.Read(ptr));
            CleanupStack::Pop(); // cert
            CleanupStack::PopAndDestroy(); // file
            CleanupStack::PushL(cert);
            }
        }
    CleanupStack::Pop(itemsToPop); // cert
    CleanupStack::PopAndDestroy(2); // fs, dir
    CleanupStack::Pop(); // aCertChain
    return KErrNone;
    }

TInt RRoapStorageClient::SignL(
    const TDesC8& aHash,
    HBufC8*& aSignature)
    {
    CRSAPrivateKey* privateKey = NULL;
    
    GetDevicePrivateKeyL(privateKey);
    CleanupStack::PushL(privateKey);
    aSignature = OmaCrypto::RsaPssSignHashL(privateKey, aHash);
    CleanupStack::PopAndDestroy(); // privateKey
    return KErrNone;
    }

TBool RRoapStorageClient::VerifyL(
    const TDesC8& aSignature,
    const TDesC8& aHash)
    {
    CRSAPublicKey* publicKey = NULL;
    TBool r = ETrue;
    
    GetDevicePublicKeyL(publicKey);
    CleanupStack::PushL(publicKey);
    r = OmaCrypto::RsaPssVerifyHashL(publicKey, aSignature, aHash);
    CleanupStack::PopAndDestroy(); // privateKey
    return r;
    }

// -----------------------------------------------------------------------------
// RoapStorageClient::GetRIContextL
// -----------------------------------------------------------------------------
//    
CDRMRIContext* RRoapStorageClient::GetRIContextL( const TDesC8& aRiID )
    {    
    return NULL;
    }
 
    
// -----------------------------------------------------------------------------
// RoapStorageClient::GetDomainContextL
// -----------------------------------------------------------------------------
//    
CDRMDomainContext* RRoapStorageClient::GetDomainContextL( const TDesC8& aDomainID )
    {    
    return NULL;
    }
    
// -----------------------------------------------------------------------------
// RRoapStorageClient::AddRIContextL
// Add a new RI Context to the ROAP storage
// -----------------------------------------------------------------------------
//
void RRoapStorageClient::AddRIContextL( const CDRMRIContext& aRiContext )
    {
    }

// -----------------------------------------------------------------------------
// RRoapStorageClient::AddDomainContextL
// Add a new RI Context to the ROAP storage
// -----------------------------------------------------------------------------
//
void RRoapStorageClient::AddDomainContextL( const CDRMDomainContext& aDomainContext,
                                            TKeyTransportScheme& aTransportScheme )
    {
    }
    
// -----------------------------------------------------------------------------
// RRoapStorageClient::DeleteRiContextL
// -----------------------------------------------------------------------------
//    
void RRoapStorageClient::DeleteRiContextL( const TDesC8& aRiID )
    {
    }
    
// -----------------------------------------------------------------------------
// RRoapStorageClient::DeleteDomainContextL
// -----------------------------------------------------------------------------
//    
void RRoapStorageClient::DeleteDomainContextL( const TDesC8& aDomainID )
    {
    }
    
// -----------------------------------------------------------------------------
// RRoapStorageClient::DeleteExpiredRIsL
// -----------------------------------------------------------------------------
//    
void RRoapStorageClient::DeleteExpiredRIsL( const TTime& aTime )
    {
    }
    
    
// -----------------------------------------------------------------------------
// RRoapStorageClient::DeleteExpiredDomainsL
// -----------------------------------------------------------------------------
//    
void RRoapStorageClient::DeleteExpiredDomainsL( const TTime& aTime )
    {
    }
    
// -----------------------------------------------------------------------------
// RRoapStorageClient::WhiteListURLExistsL
// -----------------------------------------------------------------------------
//    
TBool RRoapStorageClient::WhiteListURLExistsL( const TDesC8& aURL )
    {
    return EFalse;
    }         

//  End of File  
