/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include <ezlib.h>
#include <e32math.h>
#include <symmetric.h>
#include <bacntf.h>

#include "wmdrmkeystorage.h"
#include "slotdatacache.h"
#include "clock.h"
#include "wmdrmserver.h"
#include "drmserviceapiwrapper.h"
#include "drmrightsstoringlocation.h"
#include "drmutilityinternaltypes.h"

#define _LOGGING_FILE L"wmdrmserver.txt"

#include "flogger.h"
#include "logfn.h"


// Constants

#if defined(FF_PLATFORM_SIMULATOR) || defined(__WINSCW__)
_LIT8( KDummyKey, "0123456789012345" );
#endif

_LIT(KDrmServiceApiWrapperName, "drmserviceapiwrapper.dll");
static const TInt KDrmServiceApiWrapperGateOrdinal = 1;

//---------------------------------------------------------------------------
// CClock::NewL
// Second phase constructor
//---------------------------------------------------------------------------
//
CClock* CClock::NewL( CWmDrmServer* aServer )
    {
	LOGFN( "CClock::NewL" );
    CClock* self = new (ELeave) CClock( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

//---------------------------------------------------------------------------
// CClock::CClock
// Constructor
//---------------------------------------------------------------------------
//
CClock::CClock( CWmDrmServer* aServer ):
    CTimer( -1 ),
    iTimeIsGood( EFalse ),
    iServer( aServer ),
    iServiceApiLoaded( EFalse ),
    iServiceApi( NULL )
	{
	CActiveScheduler::Add( this );
	}

//---------------------------------------------------------------------------
// CClock::~CClock
// Destructor
//---------------------------------------------------------------------------
//
CClock::~CClock()
    {
    delete iChangeNotifier;
    delete iServiceApi;    
    iServiceApiLib.Close();
    }	

//---------------------------------------------------------------------------
// CClock::TimeIsGood
//---------------------------------------------------------------------------
//
TBool CClock::TimeIsGood()
    {
    return iTimeIsGood;
    }	

//---------------------------------------------------------------------------
// CClock::SetTimeAsGoodL
//---------------------------------------------------------------------------
//
void CClock::SetTimeAsGoodL( TBool aGood )
    {
    iTimeIsGood = aGood;
    if ( iTimeIsGood )
        {
        iTime.UniversalTime();
        WriteTimeL();
        }
    }	

//---------------------------------------------------------------------------
// CClock::ConstructL
//---------------------------------------------------------------------------
//
void CClock::ConstructL()
	{
    TInt r = KErrNone;
    TInt trap = KErrNone;
    
	LOGFN( "CClock::ConstructL" );
	CTimer::ConstructL();
	iChangeNotifier = CEnvironmentChangeNotifier::NewL( EPriorityNormal,
	    TCallBack( CClock::ChangeCallback, this ) );
    TRAP( trap, r = ReadTimeL() );
	if ( trap != KErrNone || r != KErrNone )
	    {
	    // Init Change: init to a date prior to the manufacturing date:
	    // 00:00:00:000000 October 2nd, 2007.
	    // iTime.UniversalTime();
	    iTime = TDateTime(2007,EOctober,1,0,0,0,0);
	    WriteTimeL();
	    }
	}

//---------------------------------------------------------------------------
// CClock::Start
//---------------------------------------------------------------------------
//
void CClock::Start()
	{
	LOGFN( "CClock::Start" );
	After( KClockInterval );
	iChangeNotifier->Start();
	}

//---------------------------------------------------------------------------
// CClock::ChangeCallback
//---------------------------------------------------------------------------
//	
TInt CClock::ChangeCallback( TAny* aClock )
    {
	LOGFN( "CClock::ChangeCallback" );
	reinterpret_cast< CClock*>( aClock )->HandleChange();
	return KErrNone;
    }

//---------------------------------------------------------------------------
// CClock::EvaluateCurrentTime
//---------------------------------------------------------------------------
//    
void CClock::EvaluateCurrentTime()
    {
    TTime time;
    TTimeIntervalSeconds delta;
    TDateTime dateTime;

    LOGFN( "CClock::EvaluateCurrentTime" );
    time.UniversalTime();
    dateTime = iTime.DateTime();

    if( time.Int64() < iTime.Int64() ) 
        {
        iTimeIsGood = EFalse;
        LOG1( "Time invalid" );
        }
    else
        {
        iTimeIsGood = ETrue;
        LOG1( "Time valid" );
        }
    }

//---------------------------------------------------------------------------
// CClock::ReadTimeL
//---------------------------------------------------------------------------
//
TInt CClock::ReadTimeL()
    {
    RFile file;
    TBuf8<sizeof( TTime ) + 2 * KAESKeyLength> encryptedData;
    TBuf8<sizeof( TTime ) + 2 * KAESKeyLength> decryptedData;
    TBuf8<KAESKeyLength> key;
    TBuf8<KAESKeyLength> iv;
    CBufferedDecryptor* decryptor = NULL;
    CModeCBCDecryptor* cbcDecryptor = NULL;
    CAESDecryptor* aesDecryptor = NULL;
    CPaddingPKCS7* padding = NULL;
    TInt r = KErrNone;
    TInt size = 0;
    TDrmScheme drmScheme( EDrmSchemeWmDrm );
    TChar driveLetter;
    TBool wmDrmRightsConfigFound( EFalse );
    TFileName timeSaverFile;
        
    LOGFNR( "CClock::ReadTimeL", r );    
    
    // Check which drive is configured in the Central Repository Key
    // for the desired storing location of WM DRM rights. Time saver
    // file should be read from that location, too.
	wmDrmRightsConfigFound = DrmRightsStoringLocation::CheckDrmRightsStorageDriveL( 
        iServer->Fs(), drmScheme, driveLetter );
    
    if( wmDrmRightsConfigFound )
        {
        LOG1( "ReadTimeL: Rights Config Found" );    
        }
   
	timeSaverFile.Format( KTimeSaverFile, (TUint)driveLetter );
    
    r = file.Open( iServer->Fs(), timeSaverFile, EFileRead );
    if ( r == KErrNone )
    	{
	    CleanupClosePushL( file );
	    
	    User::LeaveIfError( file.Size( size ) );
	    
	    if( size != ( 2 * KAESKeyLength ) )
	        {
	        User::Leave(KErrCorrupt);
	        }
	    
	    User::LeaveIfError( file.Read( iv ) );
	    User::LeaveIfError( file.Read( encryptedData ) );
#if defined(FF_PLATFORM_SIMULATOR) || defined(__WINSCW__)
	    key.Copy( KDummyKey );
#else
	    iServer->Cache()->iKeyStorage->GetDeviceSpecificKeyL( key );
#endif
	    
        aesDecryptor = CAESDecryptor::NewL( key );
        CleanupStack::PushL( aesDecryptor );
        
        cbcDecryptor = CModeCBCDecryptor::NewL( aesDecryptor, iv );
        CleanupStack::Pop( aesDecryptor );
        CleanupStack::PushL( cbcDecryptor );
        
        padding = CPaddingPKCS7::NewL( KAESKeyLength );
        CleanupStack::PushL( padding );
        
        decryptor = CBufferedDecryptor::NewL( cbcDecryptor, padding );
        CleanupStack::Pop( 2, cbcDecryptor ); //padding, cbcDecryptor
        CleanupStack::PushL( decryptor );
	
	    decryptor->ProcessFinalL( encryptedData, decryptedData );
	    TPtr8 ptr( reinterpret_cast<TUint8*>( &iTime ), sizeof( iTime ) );
	    ptr.Copy( decryptedData.Left( sizeof( TTime ) ) );
	    EvaluateCurrentTime();
	    CleanupStack::PopAndDestroy( 2, &file ); //decryptor, file
    	}
    return r;
    }

//---------------------------------------------------------------------------
// CClock::WriteTimeL
//---------------------------------------------------------------------------
//
TInt CClock::WriteTimeL()
    {
    RFile file;
    TBuf8<sizeof( TTime ) + 2 * KAESKeyLength> encryptedData;
    TBuf8<sizeof( TTime ) + 2 * KAESKeyLength> decryptedData;
    TBuf8<KAESKeyLength> key;
    TBuf8<KAESKeyLength> iv;
    CBufferedEncryptor* encryptor = NULL;
    CModeCBCEncryptor* cbcEncryptor = NULL;
    CAESEncryptor* aesEncryptor = NULL;
    CPaddingPKCS7* padding = NULL;
    TInt r = KErrNone;
    TInt pos = 0;
    TDrmScheme drmScheme( EDrmSchemeWmDrm );
    TChar driveLetter;
    TBool wmDrmRightsConfigFound( EFalse );
    TFileName tempFile;
        
    LOGFNR( "CClock::WriteTime", r );    
    
    // Check which drive is configured in the Central Repository Key
    // for the desired storing location of WM DRM rights. Time saver
    // file should be stored to that location, too.
	wmDrmRightsConfigFound = DrmRightsStoringLocation::CheckDrmRightsStorageDriveL( 
        iServer->Fs(), drmScheme, driveLetter );
 
    if( wmDrmRightsConfigFound )
        {
        LOG1( "WriteTimeL: Rights Config Found" );    
        }   
     
	tempFile.Format( KPrivateDir, (TUint)driveLetter );
    iServer->Fs().MkDirAll( tempFile );
    
    tempFile.Format( KTimeSaverFile, (TUint)driveLetter );
    User::LeaveIfError( file.Replace( iServer->Fs(), tempFile, EFileRead | EFileWrite ) );
    CleanupClosePushL( file );

    iv.SetLength( KAESKeyLength );
    TRandom::RandomL( iv );
#if defined(FF_PLATFORM_SIMULATOR) || defined(__WINSCW__)
    key.Copy( KDummyKey );
#else
    iServer->Cache()->iKeyStorage->GetDeviceSpecificKeyL( key );
#endif
    aesEncryptor = CAESEncryptor::NewL( key );
    CleanupStack::PushL( aesEncryptor );
    
    cbcEncryptor = CModeCBCEncryptor::NewL( aesEncryptor, iv );
    CleanupStack::Pop( aesEncryptor );
    CleanupStack::PushL( cbcEncryptor );
    
    padding = CPaddingPKCS7::NewL( KAESKeyLength );
    CleanupStack::PushL( padding );
    
    encryptor = CBufferedEncryptor::NewL( cbcEncryptor, padding );
    CleanupStack::Pop( 2, cbcEncryptor ); //padding, cbcEncryptor
    CleanupStack::PushL( encryptor );
        
    TPtr8 ptr( reinterpret_cast<TUint8*>( &iTime ), sizeof( iTime ), sizeof( iTime ) );
    decryptedData.Copy( ptr.Left( sizeof( TTime ) ) );
    encryptor->ProcessFinalL( decryptedData, encryptedData );
    User::LeaveIfError( file.Seek( ESeekStart, pos ) );
    User::LeaveIfError( file.SetSize( 0 ) );
    User::LeaveIfError( file.Write( iv ) );
    User::LeaveIfError( file.Write( encryptedData ) );
    iTimeIsGood = ETrue;
    CleanupStack::PopAndDestroy( 2, &file ); //encryptor, file
    return r;
    }

//---------------------------------------------------------------------------
// CClock::HandleChange
//---------------------------------------------------------------------------
//
void CClock::HandleChange()
    {
    TInt change;
    
	LOGFN( "CClock::HandleChange" );
	change = iChangeNotifier->Change();
	LOG2( "Change: %d", change );
	if ( ( change & EChangesSystemTime ) )
	    {
	    EvaluateCurrentTime();
        if ( iTimeIsGood )
            {
            iTime.UniversalTime();
            TRAP_IGNORE( WriteTimeL() );
            }
	    }
    }


//---------------------------------------------------------------------------
// CClock::LoadServiceApi
//---------------------------------------------------------------------------
//
TInt CClock::LoadServiceApi() 
    {
#ifdef __DRM_CLOCK    
    TInt err = KErrNone;
    
    if( !iServiceApiLoaded )
        {
        err = iServiceApiLib.Load( KDrmServiceApiWrapperName );
        if( err )
            {
            iServiceApiLoaded = EFalse;
            return err;
            }
        iServiceApiLoaded = ETrue;
        }
        
    if( !iServiceApi )
        {
        TLibraryFunction function = iServiceApiLib.Lookup( KDrmServiceApiWrapperGateOrdinal );
        if( function )
            {
            iServiceApi = reinterpret_cast<DRM::CDrmServiceApiWrapper*>( function() );
            if( !iServiceApi )
                {  
                iServiceApi = NULL;
                return KErrGeneral;
                }
            }
        else 
            {
            return KErrNotFound;
            }                
        }    
#endif 
    return KErrNone;               
    }

//----------------------------------------------------------------------------
// CClock::GetTimeL
// This function gets the time from DRM Clock checks the validity and
// then returns this information
// flagged for use with DRM Clock, if clock is not there, use the old implementation
//----------------------------------------------------------------------------
//
void CClock::GetTimeL( TTime& aTime, TBool& aValid )
    {
    TInt timeZone = 0;
    
#ifdef __DRM_CLOCK

    DRMClock::ESecurityLevel secLevel;

    // If it's already loaded then this just returns KErrNone
	User::LeaveIfError( LoadServiceApi() ); 
    
    User::LeaveIfError( iServiceApi->GetSecureTime( aTime, timeZone, secLevel ) );

    if( secLevel == DRMClock::KSecure )
        {
        aValid = ETrue;
        }
    else
        {
        aValid = EFalse;
        }
#else
    aTime.UniversalTime();
    aValid = EFalse;
#endif
    }

//----------------------------------------------------------------------------
// CClock::RunL
//----------------------------------------------------------------------------
//
void CClock::RunL()
	{
	LOGFN( "CClock::RunL" );
    LOG2( "Status: %d", iStatus.Int() );
    EvaluateCurrentTime();
    if ( iTimeIsGood )
        {
        iTime.UniversalTime();
        TRAP_IGNORE( WriteTimeL() );
        }
    After( KClockInterval );
	}
