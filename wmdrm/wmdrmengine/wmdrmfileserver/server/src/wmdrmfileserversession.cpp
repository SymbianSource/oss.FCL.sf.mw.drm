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
* Description:  WMDRM Server implementation
*
*/


#include <f32file.h>
#include <systemwarninglevels.hrh>
#include "wmdrmfileserversession.h"
#include "wmdrmfileserver.h"

#define _LOGGING_FILE L"wmdrmfileserver.txt"

// Due to TCB capability logging can not be used
// #include "flogger.h"
// #include "logfn.h"


// rights server uid
const TUint32 KRightsServer = 0x101F51F2;
_LIT( KSecureStore, "securestore" );
_LIT( KLicenseStore, "licstore" );
_LIT( KDefaultNamespace, "hds");

_LIT( KStorePathTemplate, "%c:\\sys\\private\\2000B180\\%S\\%S\\");

// ======== LOCAL FUNCTIONS ========

// Implmentation from securitycheckutil.cpp with some reductions

LOCAL_C TBool CheckFileName(const TFileName& aFileName, TChar aSystemDriveChar)
    {
    
    if (aFileName.Length() > KMaxFileName)
            {
            return EFalse;
            }
    
    // Check for full drive and path
    TChar drive(aFileName[0]);
    
    _LIT(KDriveSeparator, ":\\");
    if (!drive.IsAlpha() || aFileName.Find(KDriveSeparator) != 1)
            {
            return EFalse;
            }
            
    // Check for double slashes
    _LIT(KDoubleSlash, "\\\\");
    if (aFileName.Find(KDoubleSlash) != KErrNotFound)
            {
            return EFalse;
            }
    
    // Check for .. in the path
    _LIT(KDoubleDotPath, "..\\");
    if (aFileName.Find(KDoubleDotPath) != KErrNotFound)
            {
            return EFalse;
            }
            
    // Check for references to writable SwiCertstore
    _LIT(KWritableSwiCertStore, ":\\Resource\\SwiCertstore");
    TBuf<32> writableSwiCertStorePath;
    writableSwiCertStorePath.Append(aSystemDriveChar);
    writableSwiCertStorePath.Append(KWritableSwiCertStore);
    
    if (aFileName.FindF(writableSwiCertStorePath) != KErrNotFound)
            {
            return EFalse;
            }
    
    return ETrue;
    }



LOCAL_C TBool IsSubstedDriveL(RFs& aFs, const TFileName& aFileName)
    {
    TChar drvCh = aFileName[0];
    TInt drvNum;
    User::LeaveIfError(RFs::CharToDrive(drvCh, drvNum));
    TDriveInfo drvInfo;
    User::LeaveIfError(aFs.Drive(drvInfo, drvNum));
    //if the drive is subst'd return true
    if(drvInfo.iDriveAtt & KDriveAttSubsted)
        {
        return ETrue;
        }
    return EFalse;  
    }




// ---------------------------------------------------------------------------
// Create a safe filename which can be used under the WMDRM file server
// root by filtering all illegal characters
// ---------------------------------------------------------------------------
//
LOCAL_C void CreateSafeFileNameL( 
    TFileName& aTarget, 
    const TBuf<KFileNameSize>& aSource,
    RFs &aFs )
    {
    TInt i;
    TChar c;
    TBool suffix = ETrue;
    
    TInt driveNumber( RFs::GetSystemDrive() );
	TChar driveLetter;
    User::LeaveIfError( RFs::DriveToChar( driveNumber, driveLetter ) );
    
	TFileName storeRootFile;
	storeRootFile.Format( KStoreRoot, (TUint)driveLetter );
    
    if ( aSource.Length() + storeRootFile.Length() + 1 > aTarget.MaxLength() - aTarget.Length() )
        {
        User::Leave( KErrArgument );
        }
   
    aTarget.Copy( storeRootFile );
    aTarget.Append( '\\' );
	for ( i = 0 ; i < aSource.Length(); i++ )
	    {
	    c = aSource[i];

        // Change all non-alphanumeric characters to underscores,
        // except for the colon separating the file suffix
	    if ( !c.IsAlphaDigit() && !suffix )
	        {
	        c = '_';
	        }
        
        // Once the suffix colon has been processed, replace all non-alphanumeric characters
	    if ( suffix && c == '.' )
            {
            suffix = EFalse;
            }
	    aTarget.Append( c );
	    }
	    
    if( IsSubstedDriveL( aFs, aTarget ) || !CheckFileName( aTarget, driveLetter ) )
        {
        User::Leave(KErrArgument);
        }
    }

// ---------------------------------------------------------------------------
// Extract the path components from a full filename
// ---------------------------------------------------------------------------
//
LOCAL_C void GetPath( const TDesC& aFilename, TPtrC& aPath )
    {
    TInt i = aFilename.Length() - 1;
    while ( i >= 0 && aFilename[i] != '\\' )
        {
        i--;
        }
    if ( aFilename[i] == '\\' )
        {
        i++;
        }
    return aPath.Set( aFilename.Left( i ) );
    }

// ---------------------------------------------------------------------------
// Check if system drive has <= KDRIVECCRITICALTHRESHOLD free space and in 
// that case leave with KErrDiskFull
// ---------------------------------------------------------------------------
//
LOCAL_C void CheckFreeSpaceL( RFs &aFs, TInt aBytesToWrite )
    {
    TVolumeInfo info;
    User::LeaveIfError( aFs.Volume( info, aFs.GetSystemDrive() ) );
    if ( info.iFree - aBytesToWrite <= KDRIVECCRITICALTHRESHOLD )
        {
        User::Leave( KErrDiskFull );
        }
    }

// ======== MEMBER FUNCTIONS ========

CWmDrmFileServerSession::CWmDrmFileServerSession()
	{
    //LOGFN( "CWmDrmFileServerSession::CWmDrmFileServerSession" );
	}
	
CWmDrmFileServerSession::~CWmDrmFileServerSession()
	{
	//LOGFN( "CWmDrmFileServerSession::~CWmDrmFileServerSession" );
    iFile.Close();
	}

TInt CWmDrmFileServerSession::OpenL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    TFileName path;
    TBuf<KFileNameSize> fileName;
    
    //LOGFNR( "CWmDrmFileServerSession::OpenL", r );
    __UHEAP_MARK;
    if ( aMessage.GetDesLength( 0 ) > KFileNameSize )
        {
        User::Leave( KErrArgument );
        }
    iFile.Close();
    aMessage.ReadL( 0, fileName );
    CreateSafeFileNameL( path, fileName, Server().Fs() );
    //LOG( path );
    r = iFile.Open( Server().Fs(), path, EFileRead | EFileWrite );
    __UHEAP_MARKEND;
    return r;
    }
    
TInt CWmDrmFileServerSession::CreateL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    TFileName path;
    TBuf<KFileNameSize> fileName;
    TPtrC folders( NULL, 0 );
    
    //LOGFNR( "CWmDrmFileServerSession::CreateL", r );
    __UHEAP_MARK;
    if ( aMessage.GetDesLength( 0 ) > KFileNameSize )
        {
        User::Leave( KErrArgument );
        }
    iFile.Close();
    aMessage.ReadL( 0, fileName );
    //LOG( fileName );
    CreateSafeFileNameL( path, fileName, Server().Fs() );
    GetPath( path, folders ); 
    
    CheckFreeSpaceL( Server().Fs(), 0 );
    
    Server().Fs().MkDirAll( folders );
    //LOG( path );
    r = iFile.Replace( Server().Fs(), path, EFileRead | EFileWrite );
    __UHEAP_MARKEND;
    return r;
    }
    
TInt CWmDrmFileServerSession::ReadL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    RBuf8 buf;
    TInt amountRead = 0;
    TInt pos = 0;
    
    //LOGFNR( "CWmDrmFileServerSession::ReadL", r );
    __UHEAP_MARK;
    if ( iFile.SubSessionHandle() == KNullHandle )
        {
        User::Leave( KErrNotReady );
        }
    
    buf.CreateL( aMessage.GetDesMaxLengthL( 0 ) );
    CleanupClosePushL( buf );
    iFile.Seek( ESeekStart, pos );
    r = iFile.Read( buf );
    if ( r == KErrNone )
        {
        aMessage.WriteL( 0, buf );
        amountRead = buf.Size();
        }
    aMessage.WriteL( 1, TPckg<TInt>( amountRead ) );
    //LOG2( "Read %d bytes", amountRead );
    CleanupStack::PopAndDestroy(); // buf
    __UHEAP_MARKEND;
    return r;
    }
    
TInt CWmDrmFileServerSession::WriteL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    RBuf8 buf;
    TInt pos = 0;
    
    //LOGFNR( "CWmDrmFileServerSession::WriteL", r );
    __UHEAP_MARK;
    if ( iFile.SubSessionHandle() == KNullHandle )
        {
        User::Leave( KErrNotReady );
        }
    
    CheckFreeSpaceL( Server().Fs(), aMessage.GetDesLengthL( 0 ) );
    
    buf.CreateL( aMessage.GetDesLengthL( 0 ) );
    CleanupClosePushL( buf );
    aMessage.ReadL( 0, buf );
    iFile.Seek( ESeekStart, pos );
    iFile.SetSize( 0 );
    r = iFile.Write( buf );
    CleanupStack::PopAndDestroy(); // buf
    __UHEAP_MARKEND;
    return r;
    }
    
TInt CWmDrmFileServerSession::SizeL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    TInt size;
    
    //LOGFNR( "CWmDrmFileServerSession::SizeL", r );
    __UHEAP_MARK;
    if ( iFile.SubSessionHandle() == KNullHandle )
        {
        User::Leave( KErrNotReady );
        }
    r = iFile.Size( size );
    if ( r == KErrNone )
        {
        aMessage.WriteL( 0, TPckg<TInt>( size ) );
        }
    __UHEAP_MARKEND;
    return r;
    }
    
TInt CWmDrmFileServerSession::IsOpenL( const RMessage2& /*aMessage*/ )
    {
    TInt r = KErrNone;
    
    //LOGFNR( "CWmDrmFileServerSession::IsOpenL", r );
    if ( iFile.SubSessionHandle() == KNullHandle )
        {
        r = KErrNotReady;
        }
    return r;
    }
    
TInt CWmDrmFileServerSession::CloseFileL( const RMessage2& /*aMessage*/ )
    {
    TInt r = KErrNone;
    
    //LOGFNR( "CWmDrmFileServerSession::CloseFileL", r );
    iFile.Close();
    return r;
    }
    
TInt CWmDrmFileServerSession::DeleteL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    TFileName path;
    TBuf<KFileNameSize> fileName;
    
    //LOGFNR( "CWmDrmFileServerSession::DeleteL", r );
    __UHEAP_MARK;
    if ( aMessage.GetDesLength( 0 ) > KFileNameSize )
        {
        User::Leave( KErrArgument );
        }
    aMessage.ReadL( 0, fileName );
    CreateSafeFileNameL( path, fileName, Server().Fs() );
    r = Server().Fs().Delete( path );
    //LOG( path );
    __UHEAP_MARKEND;
    return r;
    }
    
TInt CWmDrmFileServerSession::MkDirAllL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    TFileName path;
    TBuf<KFileNameSize> fileName;
    TUint att;
    
    //LOGFNR( "CWmDrmFileServerSession::MkDirAllL", r );
    __UHEAP_MARK;
    if ( aMessage.GetDesLength( 0 ) > KFileNameSize )
        {
        User::Leave( KErrArgument );
        }
    aMessage.ReadL( 0, fileName );
    CreateSafeFileNameL( path, fileName, Server().Fs() );
    //LOG( path );
    if ( !aMessage.Int1() && ( r = Server().Fs().Att( path, att ) ) != KErrNone )
        {
        User::Leave( r );
        }

    CheckFreeSpaceL( Server().Fs(), 0 );
    
    r = Server().Fs().MkDirAll( path );
    __UHEAP_MARKEND;
    return r;
    }

TInt CWmDrmFileServerSession::RmDirAllL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    TFileName path;
    TBuf<KFileNameSize> fileName;
    CFileMan* fileMan;
    
    //LOGFNR( "CWmDrmFileServerSession::RmDirAllL", r );
    __UHEAP_MARK;
    if ( aMessage.GetDesLength( 0 ) > KFileNameSize )
        {
        User::Leave( KErrArgument );
        }
    fileMan = CFileMan::NewL( Server().Fs() );
    CleanupStack::PushL( fileMan );
    aMessage.ReadL( 0, fileName );
    CreateSafeFileNameL( path, fileName, Server().Fs() );
    r = fileMan->RmDir( path );
    //LOG( path );
    CleanupStack::PopAndDestroy(); // fileMan
    __UHEAP_MARKEND;
    return r;
    }

TInt CWmDrmFileServerSession::RmDirL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    TFileName path;
    TBuf<KFileNameSize> fileName;
    
    //LOGFNR( "CWmDrmFileServerSession::RmDirL", r );
    __UHEAP_MARK;
    if ( aMessage.GetDesLength( 0 ) > KFileNameSize )
        {
        User::Leave( KErrArgument );
        }
    aMessage.ReadL( 0, fileName );
    CreateSafeFileNameL( path, fileName, Server().Fs() );
    r = Server().Fs().RmDir( path );
    //LOG( path );
    __UHEAP_MARKEND;
    return r;
    }


TInt CWmDrmFileServerSession::UpdateSecureTimeL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;
    TTime homeTime;
    TTime utcTime;
    RThread client;

    TPckg<TTime> package( homeTime );
    TPckg<TTime> package2( utcTime );
   
    // Get the client:
    aMessage.ClientL( client );
    CleanupClosePushL( client );
        
    // Check client, if client is not the rights server, access denied:
    _LIT_SECURITY_POLICY_S0(swSidCheck2, KRightsServer);

    if( !swSidCheck2().CheckPolicy(client) )
        {
        User::Leave( KErrAccessDenied );
        }
    
    CleanupStack::PopAndDestroy( &client );
    
    aMessage.ReadL( 0, package );
    aMessage.ReadL( 1, package2 );
    
    // Set the secure time:
    User::LeaveIfError( User::SetUTCTimeSecure( utcTime ) );
    //User::LeaveIfError( User::SetHomeTimeSecure( homeTime ) ); 
       
    return r;
    }

TInt CWmDrmFileServerSession::DeleteRightsL( const RMessage2& /*aMessage*/ )
    {
    TInt r = KErrNone;
    TInt finalErr = KErrNone;
	TFileName storeRootFile;
	CFileMan* fileMan = NULL;
    
    //LOGFNR( "CWmDrmFileServerSession::DeleteRightsL", r );
    __UHEAP_MARK;
    fileMan = CFileMan::NewL( Server().Fs() );
    CleanupStack::PushL( fileMan );
    // Remove the stores:
    TInt driveNumber( RFs::GetSystemDrive() );
	TChar driveLetter;
    User::LeaveIfError( RFs::DriveToChar( driveNumber, driveLetter ) );

	storeRootFile.Format( KStorePathTemplate, (TUint)driveLetter, &KDefaultNamespace, &KLicenseStore );
	//r = Server().Fs().RmDir( storeRootFile );    
    r = fileMan->RmDir( storeRootFile );
    if( r < KErrNone )
        {
        finalErr = r;
        }

	storeRootFile.Format( KStorePathTemplate, (TUint)driveLetter, &KDefaultNamespace, &KSecureStore );
    //r = Server().Fs().RmDir( storeRootFile );
    r = fileMan->RmDir( storeRootFile );     
    if( !finalErr && r < KErrNone )
        {
        finalErr = r;
        }
    	
    CleanupStack::PopAndDestroy( fileMan );    	
    __UHEAP_MARKEND;
    return finalErr;
    }

// ---------------------------------------------------------------------------
// Main service function. All services require DRM capability!
// ---------------------------------------------------------------------------
//
void CWmDrmFileServerSession::ServiceL( const RMessage2& aMessage )
	{
	TInt r = KErrNone;
	TInt trap = KErrNone;
	_LIT_SECURITY_POLICY_C1(drmCheck, ECapabilityDRM);
    RThread client;
	
	//LOGFNR( "CWmDrmFileServerSession::ServiceL", r );
#ifdef _LOGGING
    TInt c;
    TInt s;
    c = User::AllocSize(s);
    //LOG3( "Memory: %d cells, %d bytes allocated", c, s );
#endif

    aMessage.ClientL( client );
    if ( !drmCheck().CheckPolicy( client ) )
        {
        r = KErrAccessDenied;
        }
    else
        {
        switch ( aMessage.Function() )
            {
            case EFsOpen:
                TRAP( trap, r = OpenL( aMessage ) );
                break;
            case EFsCreate:
                TRAP( trap, r = CreateL( aMessage ) );
                break;
            case EFsRead:
                TRAP( trap, r = ReadL( aMessage ) );
                break;
            case EFsWrite:
                TRAP( trap, r = WriteL( aMessage ) );
                break;
            case EFsSize:
                TRAP( trap, r = SizeL( aMessage ) );
                break;
            case EFsDelete:
                TRAP( trap, r = DeleteL( aMessage ) );
                break;
            case EFsIsOpen:
                TRAP( trap, r = IsOpenL( aMessage ) );
                break;
            case EFsCloseFile:
                TRAP( trap, r = CloseFileL( aMessage ) );
                break;
            case EFsMkDirAll:
                TRAP( trap, r = MkDirAllL( aMessage ) );
                break;
            case EFsRmDirAll:
                TRAP( trap, r = RmDirAllL( aMessage ) );
                break;
            case EFsRmDir:
                TRAP( trap, r = RmDirL( aMessage ) );
                break;
            case EFsUpdateSecureTime:
                TRAP( trap, r = UpdateSecureTimeL( aMessage ) );
                break;                
            case EFsDeleteRights:
                TRAP( trap, r = DeleteRightsL( aMessage ) );
                break;
            default:
                PanicClient( aMessage, EPanicIllegalFunction );
                break;
                }
        if ( trap != KErrNone )
            {
            r = trap;
            }
        }
    client.Close();    
    if ( !aMessage.IsNull() )
        {
        aMessage.Complete( r );
	    }
    }

void CWmDrmFileServerSession::ServiceError( const RMessage2& aMessage, TInt aError )
	{
	//LOGFN( "CWmDrmFileServerSession::ServiceError" );
	//LOG2( "** Error: %d", aError );
	if ( aError == KErrBadDescriptor )
	    {
		PanicClient( aMessage, EPanicBadDescriptor );
		}
	if ( !aMessage.IsNull() )
        {
        CSession2::ServiceError( aMessage, aError );
	    }
    }

CWmDrmFileServer& CWmDrmFileServerSession::Server()
	{
	return *static_cast<CWmDrmFileServer*>( const_cast<CServer2*>( CSession2::Server() ) );
	}
