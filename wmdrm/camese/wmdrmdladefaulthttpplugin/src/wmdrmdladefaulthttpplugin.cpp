/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  WMDRM DLA Default HTTP Plugin
*
*/


#include <gulicon.h>
#include <wmdrmdlauinotifier.h>
#include "wmdrmdladefaulthttpplugin.h"
#include "wmdrmdladefaultlicacq.h"
#include "wmdrmdladefaultmetering.h"
#include "wmdrmdladefaultplayready.h"

#define _LOGGING_FILE L"wmdrmdladefaulthttpplugin.txt"
#include "logfn.h"

// CONSTANTS
const TUid KWmDrmDlaDefaultUiPluginUid = { 0x20019575 };

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultHttpPlugin::ConstructL()
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::ConstructL" );
    iLicenseAcquisition = CWmDrmDlaDefaultLicAcq::NewL( this );
    iMetering = CWmDrmDlaDefaultMetering::NewL( this );
    iPlayReady = CWmDrmDlaDefaultPlayReady::NewL( this );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::CWmDrmDlaDefaultHttpPlugin
// ---------------------------------------------------------------------------
//
CWmDrmDlaDefaultHttpPlugin::CWmDrmDlaDefaultHttpPlugin()
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::CWmDrmDlaDefaultHttpPlugin" );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::NewL
// ---------------------------------------------------------------------------
//
CWmDrmDlaDefaultHttpPlugin* CWmDrmDlaDefaultHttpPlugin::NewL()
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::NewL" );
    CWmDrmDlaDefaultHttpPlugin* self = 
        new( ELeave ) CWmDrmDlaDefaultHttpPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::~CWmDrmDlaDefaultHttpPlugin
// ---------------------------------------------------------------------------
//
CWmDrmDlaDefaultHttpPlugin::~CWmDrmDlaDefaultHttpPlugin()
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::~CWmDrmDlaDefaultHttpPlugin" );
    delete iLicenseAcquisition;
    delete iMetering;
    delete iPlayReady;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::ServiceIcon
// ---------------------------------------------------------------------------
//
CGulIcon* CWmDrmDlaDefaultHttpPlugin::ServiceIcon()
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::ServiceIcon" );
    return NULL;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::SupportedFileL
// ---------------------------------------------------------------------------
//
TBool CWmDrmDlaDefaultHttpPlugin::SupportedFileL( 
    const RFile& aFile,
    CWmDrmDlaUiNotifier*& aUiNotifier)
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::SupportedFileL" );
    return iLicenseAcquisition->SupportedFileL( aFile, aUiNotifier );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::SupportedDrmHeaderL
// ---------------------------------------------------------------------------
//
TBool CWmDrmDlaDefaultHttpPlugin::SupportedDrmHeaderL( 
    const TDesC8& aHeader,
    CWmDrmDlaUiNotifier*& aUiNotifier )
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::SupportedDrmHeaderL" );
    return iLicenseAcquisition->SupportedDrmHeaderL( aHeader, aUiNotifier );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::SupportedMeteringCertificateL
// ---------------------------------------------------------------------------
//
TBool CWmDrmDlaDefaultHttpPlugin::SupportedMeteringCertificateL( 
    const TDesC8& aCertificate,
    CWmDrmDlaUiNotifier*& aUiNotifier )
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::SupportedMeteringCertificateL" );
    return iMetering->SupportedMeteringCertificateL( aCertificate, 
                                                     aUiNotifier );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::SupportedInitiatorL
// ---------------------------------------------------------------------------
//
TBool CWmDrmDlaDefaultHttpPlugin::SupportedInitiatorL( 
    const TDesC8& aInitiator, 
    CWmDrmDlaUiNotifier*& aUiNotifier )
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::SupportedInitiatorL" );
    return iPlayReady->SupportedInitiatorL( aInitiator, aUiNotifier );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::SilentL
// ---------------------------------------------------------------------------
//
TBool CWmDrmDlaDefaultHttpPlugin::SilentL( 
    const RFile& aFile )
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::SilentL" );
    return iLicenseAcquisition->SilentL( aFile );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::ProcessDrmHeaderL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultHttpPlugin::ProcessDrmHeaderL( 
    const TDesC8& aDrmHeader )
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::ProcessDrmHeaderL" );
    iLicenseAcquisition->ProcessDrmHeaderL( aDrmHeader );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::ProcessInitiatorL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultHttpPlugin::ProcessInitiatorL( 
    const TDesC8& aInitiator )
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::ProcessInitiatorL" );
    iPlayReady->ProcessInitiatorL( aInitiator );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::AcquireLicense
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultHttpPlugin::AcquireLicense( 
    const TDesC8& aLicenseChallenge,
    HBufC* aLicenseServerUrl,
    TRequestStatus& aStatus )
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::AcquireLicense" );
    iLicenseAcquisition->AcquireLicense( aLicenseChallenge, 
                                         aLicenseServerUrl, 
                                         aStatus );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::GetLicenseResponseL
// ---------------------------------------------------------------------------
//
HBufC8* CWmDrmDlaDefaultHttpPlugin::GetLicenseResponseL()
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::GetLicenseResponseL" );
    return iLicenseAcquisition->GetLicenseResponseL();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::SendLicenseStorageStatus
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultHttpPlugin::SendLicenseStorageStatus( 
    HBufC8* aTid,
    const TDesC& aUrl,
    TInt aError, 
    TRequestStatus& aStatus )
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::SendLicenseStorageStatus" );
    iLicenseAcquisition->SendLicenseStorageStatus( aTid, 
                                                   aUrl, 
                                                   aError, 
                                                   aStatus );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::SendDrmHeaderError
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultHttpPlugin::SendDrmHeaderError( 
    const TDesC& aErrorUrl, 
    TInt aError, 
    TRequestStatus& aStatus )
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::SendDrmHeaderError" );
    iLicenseAcquisition->SendDrmHeaderError( aErrorUrl, aError, aStatus );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::GetContentUrlL
// ---------------------------------------------------------------------------
//
HBufC* CWmDrmDlaDefaultHttpPlugin::GetContentUrlL()
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::GetContentUrlL" );
    return iLicenseAcquisition->GetContentUrlL();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::GetHtmlDataL
// ---------------------------------------------------------------------------
//
HBufC* CWmDrmDlaDefaultHttpPlugin::GetHtmlDataL()
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::GetHtmlDataL" );
    return iLicenseAcquisition->GetHtmlDataL();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::CancelLicenseAcquisition
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultHttpPlugin::CancelLicenseAcquisition()
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::CancelLicenseAcquisition" );
    iLicenseAcquisition->CancelLicenseAcquisition();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::ProcessMeteringChallenge
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultHttpPlugin::ProcessMeteringChallenge( 
    const TDesC8& aMeteringChallenge, 
    const TDesC& aUrl, 
    TRequestStatus& aStatus )
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::ProcessMeteringChallenge" );
    iMetering->ProcessMeteringChallenge( aMeteringChallenge, aUrl, aStatus );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::GetMeteringResponse
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultHttpPlugin::GetMeteringResponse( 
    HBufC8*& aResponse, 
    TRequestStatus& aStatus )
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::GetMeteringResponse" );
    iMetering->GetMeteringResponse( aResponse, aStatus );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::MeteringFinished
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultHttpPlugin::MeteringFinished()
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::MeteringFinished" );
    iMetering->MeteringFinished();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::CancelMetering
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultHttpPlugin::CancelMetering()
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::CancelMetering" );
    iMetering->CancelMetering();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::ProcessJoinDomainChallenge
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultHttpPlugin::ProcessJoinDomainChallenge( 
    const TDesC8& aJoinDomainChallenge, 
    const TDesC& aUrl, 
    TRequestStatus& aStatus )
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::ProcessJoinDomainChallenge" );
    iPlayReady->ProcessJoinDomainChallenge( aJoinDomainChallenge, 
                                            aUrl, 
                                            aStatus );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::ProcessLeaveDomainChallenge
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultHttpPlugin::ProcessLeaveDomainChallenge( 
    const TDesC8& aLeaveDomainChallenge, 
    const TDesC& aUrl, 
    TRequestStatus& aStatus )
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::ProcessLeaveDomainChallenge" );
    iPlayReady->ProcessLeaveDomainChallenge( aLeaveDomainChallenge, 
                                             aUrl, 
                                             aStatus );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::SetIapId
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultHttpPlugin::SetIapId( TInt aIapId )
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::SetIapId" );
    iLicenseAcquisition->SetIapId( aIapId );
    iMetering->SetIapId( aIapId );
    iPlayReady->SetIapId( aIapId );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::InitializeUiNotifierL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaDefaultHttpPlugin::InitializeUiNotifierL()
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::InitializeUiNotifierL" );
    //Tries to initialize UiNotifier. If UiNotifier can't be created or
    //initialization fails, then leaves with error code, won't take ownership
    //of the notifier
    iUiNotifier = NULL;
    iUiNotifier = CWmDrmDlaUiNotifier::NewL();
    TRAPD( err, iUiNotifier->InitializeL( KWmDrmDlaDefaultUiPluginUid ) );
    if ( err )
        {
        delete iUiNotifier;
        iUiNotifier = NULL;
        User::Leave( err );
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::UiNotifier
// ---------------------------------------------------------------------------
//
CWmDrmDlaUiNotifier* CWmDrmDlaDefaultHttpPlugin::UiNotifier()
    {
    return iUiNotifier;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaDefaultHttpPlugin::Supported
// ---------------------------------------------------------------------------
//
TBool CWmDrmDlaDefaultHttpPlugin::Supported( 
    const TDesC8& aString,
    const TDesC8& aIdentifier)
    {
    LOGFN( "CWmDrmDlaDefaultHttpPlugin::Supported" );
    TBool supported( EFalse );
    if ( aString.FindF( aIdentifier ) != KErrNotFound )
        {
        supported = ETrue;
        }
    LOG2( "supported: %d", supported );
    return supported;
    }
